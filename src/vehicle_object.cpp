#include "../include/vehicle_object.h"

#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"
#include "../include/ui_utilities.h"
#include <GL/gl.h>

VehicleObject::VehicleObject(EngineSimApplication *app, World *world, Vehicle *vehicle)
{
    m_app = app;

    m_vehicle = vehicle;
    m_material = nullptr;
    m_texture = nullptr;
    rotation = 0.0f;
    steeringAngle = 0.0f;
    m_wheel_rotation = 0.0f;
    m_wheel_rotation_speed = 0.0f;
    m_tire_radius = 0.34f;
    m_brakes = 0.0f;

    m_world = world;

    m_mesh_names = m_app->getIniReader().GetVector<std::string>("Vehicle", "Meshes");
    m_material_names = m_app->getIniReader().GetVector<std::string>("Vehicle", "Materials");

    for (std::vector<std::string>::size_type i = 0; i != m_mesh_names.size(); i++)
        printf("\n%s", m_mesh_names[i].c_str());

    VehicleModel vehicleModel;
    vehicleModel.id = Bluebird;
    vehicleModel.scale = 1.0f;
    vehicleModel.height = -0.48;

    std::vector<float> tirePosition = m_app->getIniReader().GetVector<float>("Vehicle", "TirePositions");
    vehicleModel.tireX = tirePosition[1];
    vehicleModel.tireY = tirePosition[0];
    vehicleModel.tireFrontZ = tirePosition[2];
    vehicleModel.tireRearZ = tirePosition[3];

    vehicleModel.collisionBoxLength = 2.0f;
    vehicleModel.collisionBoxWidth = 0.7f;

    vehicleModel.transformEngine.SetOrientation(ysMath::LoadQuaternion(0.5f * ysMath::Constants::PI, ysMath::Constants::XAxis));
    vehicleModel.transformEngine.SetPosition(ysMath::LoadVector(-1.5f, 0.3f, 0.38));

    m_vehicle_model = vehicleModel;

    m_vehicle_model.scale = m_app->getIniReader().Get<float>("Vehicle", "Scale");

    m_vehicle_model.transformEngine.SetParent(&m_transform);

    m_modelRotation = m_app->getIniReader().GetVector<float>("Vehicle", "ModelRotation");

    ysQuaternion qx = ysMath::LoadQuaternion(m_modelRotation[0] * ysMath::Constants::PI, ysMath::Constants::XAxis);
    ysQuaternion qy = ysMath::LoadQuaternion(m_modelRotation[1] * ysMath::Constants::PI, ysMath::Constants::YAxis);
    ysQuaternion qz = ysMath::LoadQuaternion(m_modelRotation[2] * ysMath::Constants::PI, ysMath::Constants::ZAxis);

    ysQuaternion orientation = ysMath::QuatMultiply(qx, qy);
    m_transform_model.SetOrientation(orientation);
    m_transform_model.SetParent(&m_transform);

    m_transform.SetOrientation(ysMath::Constants::QuatIdentity);
    m_transform.SetPosition(ysMath::LoadVector(0,1,0));
    m_transform.SetParent(nullptr);

    m_engineModelRotation = m_app->getIniReader().GetVector<float>("Vehicle", "EngineModelRotation");
    m_engineModelPosition = m_app->getIniReader().GetVector<float>("Vehicle", "EngineModelPosition");

    ysQuaternion eqx = ysMath::LoadQuaternion(m_engineModelRotation[0] * ysMath::Constants::PI, ysMath::Constants::XAxis);
    ysQuaternion eqy = ysMath::LoadQuaternion(m_engineModelRotation[1] * ysMath::Constants::PI, ysMath::Constants::YAxis);
    ysQuaternion eqz = ysMath::LoadQuaternion(m_engineModelRotation[2] * ysMath::Constants::PI, ysMath::Constants::ZAxis);

    ysQuaternion engineOrientation = ysMath::QuatMultiply(ysMath::QuatMultiply(eqx, eqy),eqz);

    m_physicalVehicle = new PhysicalObject("Car", world, btVector3(5.0f, 1.0f, 0.0f), btVector3(0,0,0), 
        btVector3(1.2,0.4,4.0), CUBE, DYNAMIC, 1500, "", false);

    m_transform_engine = m_vehicle_model.transformEngine;
    m_transform_engine.SetParent(getTransform());
    m_transform_engine.SetOrientation(engineOrientation);
    m_transform_engine.SetPosition(ysMath::LoadVector(m_engineModelPosition[0], m_engineModelPosition[1], m_engineModelPosition[2]));

    velocity = ysMath::LoadVector();

    btRaycastVehicle::btVehicleTuning tuning;

    btVehicleRaycaster *vehicleRaycaster = new btDefaultVehicleRaycaster(world->getDynamicsWorld());

    //Creates a new instance of the raycast vehicle
    m_raycastVehicle = new btRaycastVehicle(tuning, m_physicalVehicle->getBody(), vehicleRaycaster);

    //Never deactivate the vehicle
    m_physicalVehicle->getBody()->setActivationState(DISABLE_DEACTIVATION);

    //Adds the vehicle to the world
    world->getDynamicsWorld()->addAction(m_raycastVehicle);

    //Adds the wheels to the vehicle
    btVector3 halfExtends(1.1,0.0,1.2);
    this->addWheels(&halfExtends, m_raycastVehicle, tuning);

    m_transform_camera.SetOrientation(ysMath::Constants::QuatIdentity);
    m_transform_camera.SetPosition(ysMath::LoadVector(0.0, 0.4, 0.0));
    m_transform_camera.SetParent(&m_transform_engine);
}

void VehicleObject::addWheels(
	btVector3* halfExtents,
	btRaycastVehicle* vehicle,
	btRaycastVehicle::btVehicleTuning tuning)
{
	//The direction of the raycast, the btRaycastVehicle uses raycasts instead of simiulating the wheels with rigid bodies
	btVector3 wheelDirectionCS0(0, -1, 0);

	//The axis which the wheel rotates arround
	btVector3 wheelAxleCS(-1, 0, 0);

	btScalar suspensionRestLength(0.7);

	btScalar wheelWidth(0.4);

	btScalar wheelRadius(0.35);

	//The height where the wheels are connected to the chassis
	btScalar connectionHeight(0.7);

	//All the wheel configuration assumes the vehicle is centered at the origin and a right handed coordinate system is used
	btVector3 wheelConnectionPoint(halfExtents->x() - wheelRadius, connectionHeight, halfExtents->z());

	//Adds the front wheels
	vehicle->addWheel(wheelConnectionPoint, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

	vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, 1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, true);

	//Adds the rear wheels
	vehicle->addWheel(wheelConnectionPoint* btVector3(1, 1, -1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);

	vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, -1), wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, tuning, false);

	//Configures each wheel of our vehicle, setting its friction, damping compression, etc.
	//For more details on what each parameter does, refer to the docs
	for (int i = 0; i < vehicle->getNumWheels(); i++)
	{
		btWheelInfo& wheel = vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = 25;
		wheel.m_wheelsDampingCompression = btScalar(0.2) * btSqrt(wheel.m_suspensionStiffness);//btScalar(0.8);
		wheel.m_wheelsDampingRelaxation = btScalar(0.4) * btSqrt(wheel.m_suspensionStiffness);//1;
		//Larger friction slips will result in better handling
        if(i<2)
		    wheel.m_frictionSlip = btScalar(1.7);
        else
            wheel.m_frictionSlip = btScalar(1.6);
		wheel.m_rollInfluence = 1;
	}
}

ysTransform *VehicleObject::getTransform()
{
    return m_physicalVehicle->getTransform();
}

void VehicleObject::initialize(EngineSimApplication *app)
{
}

VehicleObject::~VehicleObject()
{
}

void VehicleObject::generateGeometry()
{
    /* void */
}

void VehicleObject::render(const ViewParameters *view)
{
    btRigidBody *body = m_physicalVehicle->getBody();
    btTransform trans;

	// Get the transform of the body
	if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);
	else
		return;

	// Get position from transform
	btVector3 position1 = trans.getOrigin();
	btVector3 modelPosition = position1;
	
	// Get rotation from transform
	btQuaternion quat = trans.getRotation();

    quat = quat * btQuaternion(btVector3(1,0,0),-M_PI_2);
    quat = quat * btQuaternion(btVector3(0,0,1),+M_PI_2);

    //L = T * R * S
	btVector3 quat_axis = quat.getAxis();
	ysQuaternion quat2 = ysMath::LoadQuaternion(quat.getAngle(), ysMath::LoadVector(quat_axis.x(), quat_axis.y(), quat_axis.z()));

	ysTransform transform1;

	transform1.SetOrientation(quat2);

	transform1.SetPosition(ysMath::LoadVector(position1.x(),position1.y(),position1.z(),0));

    m_vehicle->m_transform = &transform1;

    m_vehicle_model.transformEngine.SetParent(&transform1);

    m_app->getShaders()->SetObjectTransform(transform1.GetWorldTransform());

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Material_02"));

    // L = T * R * S
    ysQuaternion qx = ysMath::LoadQuaternion((- 0.5f) * ysMath::Constants::PI, ysMath::Constants::XAxis);
    ysQuaternion qz = ysMath::LoadQuaternion((+ 0.5f) * ysMath::Constants::PI + rotation, ysMath::Constants::ZAxis);

    ysQuaternion orientation = ysMath::QuatMultiply(qx, qz);

    m_transform.SetOrientation(orientation);

    if (true)
    {
        m_transform_model.SetParent(&transform1);
        m_app->getShaders()->SetObjectTransform(m_transform_model.GetWorldTransform());

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Material_03"));

        for (std::vector<std::string>::size_type i = 0; i != m_mesh_names.size(); i++)
        {
            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial(m_material_names[i].c_str()));

            if (!m_app->m_show_engine || m_mesh_names[i] != "hood")
            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset(m_mesh_names[i].c_str()),
                1);
        }

        for(int i=0;i<4;i++)
        {
            m_raycastVehicle->getUpAxis();
            btTransform wheel1transform = m_raycastVehicle->getWheelTransformWS(i);
            btVector3 wheel1pos = wheel1transform.getOrigin();
            //btQuaternion wheel1rot = wheel1transform.getRotation();
            btQuaternion wheel1rot = m_raycastVehicle->getChassisWorldTransform().getRotation();

            btVector3 axis= wheel1rot.getAxis();

            ysTransform transform2;

            ysQuaternion q1 = ysMath::LoadQuaternion(wheel1rot.getAngle(),ysMath::LoadVector(axis.x(),axis.y(),axis.z()));
            //ysQuaternion q1 = ysMath::LoadQuaternion(wheel1rot.getAngle(),ysMath::LoadVector(axis.z(),axis.x(),axis.y()));

            //ysQuaternion q2 = ysMath::LoadQuaternion(M_PI_2,ysMath::LoadVector(1,0,0));

            //q1 = q2 * q1;
            
            transform2.SetPosition(ysMath::LoadVector(wheel1pos.x(),wheel1pos.y(),wheel1pos.z()));
            transform2.SetOrientation(q1);

            ysTransform t3;

            float angle = -m_raycastVehicle->getWheelInfo(i).m_steering;

            t3.SetOrientation(ysMath::LoadQuaternion(M_PI_2,ysMath::LoadVector(1,0,0)));

            t3.SetParent(&transform2);

             ysTransform t4;

            t4.SetOrientation(ysMath::LoadQuaternion(M_PI_2+angle,ysMath::LoadVector(0,0,1)));

            t4.SetParent(&t3);

            ysTransform t5;

            t5.SetOrientation(ysMath::LoadQuaternion(-m_wheel_rotation,ysMath::LoadVector(0,1,0)));

            t5.SetParent(&t4);

            m_app->getShaders()->SetObjectTransform(t5.GetWorldTransform());

            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Material__03"));

            if(i%2)
            {
            m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("wheel_front_right"),
                    1);
            }
            else
            {
            m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("wheel_front_left"),
                    1);
            }

            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Material__04"));

            if(i%2)
            {
                m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("wheel_front_right.001"),
                    1);
            }
            else
            {
                m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("wheel_front_left.001"),
                    1);
            }
        }
    }
}

void VehicleObject::process(float dt)
{
    m_raycastVehicle->updateVehicle(dt);

    m_vehicle->m_transform_camera = m_physicalVehicle->getTransform();
    // Steering
    float angle = m_vehicle->steeringAngle;

    btTransform tire1transform = m_raycastVehicle->getWheelTransformWS(0);

    float s = m_raycastVehicle->getCurrentSpeedKmHour();
    //printf("\nSpeed: %f",s);

    btVector3 pos = m_raycastVehicle->getChassisWorldTransform().getOrigin();

    m_brakes = m_vehicle->m_brakes;

    float scale = 0.1f;

    m_wheel_rotation_speed = (1.0f - m_brakes) * scale * (float)m_vehicle->getSpeed() / (2.0f * ysMath::Constants::PI * m_tire_radius);

    btQuaternion quat = m_physicalVehicle->getOrientation();
    btQuaternion qF = quat * btQuaternion(-1,0,0,0) * quat.inverse();
    btQuaternion qR = quat * btQuaternion(0,1,0,0) * quat.inverse();
    btVector3 forward = btVector3(qF.x(), 0, qF.z()).normalized();
    btVector3 right = btVector3(qR.x(), 0, qR.z()).normalized();

    btVector3 currentForwardNormal2 = btVector3(qF.x(),qF.y(),qF.z());
    currentForwardNormal2.setY(0);

    float forwardVelocity2 = currentForwardNormal2.dot(m_physicalVehicle->getLinearVelocity());
    float realSpeed = forwardVelocity2;

    if (m_app->getSimulator()->getTransmission()->getGear() == -1)
        m_wheel_rotation_speed = 0.99f * (1.0f - m_brakes) * scale * realSpeed / (2.0f * ysMath::Constants::PI * m_tire_radius);

    if (m_app->getSimulator()->getTransmission()->getGear() == -2)
        m_wheel_rotation -= m_wheel_rotation_speed;
    else if (m_app->getSimulator()->getTransmission()->getGear() > -2)
        m_wheel_rotation += m_wheel_rotation_speed;

    btVector3 currentRightNormal2 = btVector3(qR.x(),qR.y(),qR.z());
    currentRightNormal2.setY(0);

    float rightVelocity2 = currentRightNormal2.dot(m_physicalVehicle->getLinearVelocity());

    float targetSpeedFromTireRotationSpeed = m_wheel_rotation_speed * 2.0f * ysMath::Constants::PI * m_tire_radius / scale;
    float targetSpeedFromTireRotationSpeed2 = targetSpeedFromTireRotationSpeed;

    if (m_app->getSimulator()->getTransmission()->getGear() == -2)
        targetSpeedFromTireRotationSpeed2 *= -1.0f;

    float throttle = 300.0f * targetSpeedFromTireRotationSpeed2;

    if(throttle<0.0f) 
    {
        throttle = 0.0f;
    }

    m_raycastVehicle->setSteeringValue(-angle,0);
    m_raycastVehicle->setSteeringValue(-angle,1);

    m_raycastVehicle->applyEngineForce(throttle, 2);
	m_raycastVehicle->applyEngineForce(throttle, 3);

    for(int i=0;i<m_raycastVehicle->getNumWheels();i++) m_raycastVehicle->setBrake(m_brakes * 300.0f, i);

    btVector3 f = currentForwardNormal2;
    f.setY(0);
    float y = atan2(f.x(),f.z());
    m_vehicle->m_rotation = M_PI-y;
}

void VehicleObject::destroy()
{
    m_vehicle = nullptr;

    delete m_physicalVehicle;
}

btRaycastVehicle *VehicleObject::getRaycastVehicle()
{
    return m_raycastVehicle;
}
