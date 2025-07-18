#include "../include/vehicle_object.h"

#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"
#include "../include/ui_utilities.h"

VehicleObject::VehicleObject(EngineSimApplication* app, Vehicle* vehicle)
{
    DBG;
	m_app = app;
	m_world = app->getWorld();
	m_vehicle = vehicle;
	m_material = nullptr;
	m_texture = nullptr;
	rotation = 0.0f;
	m_steeringAngle = 0.0f;
	m_wheel_rotation = 0.0f;
	m_wheel_rotation_speed = 0.0f;
	m_tire_radius = 0.34f;
	m_brakes = 0.0f;
	m_previousPosition = ysMath::LoadVector();

    DBG;
	m_selected_vehicle_name = m_app->getSettings().selectedVehicleName;
	//m_selected_vehicle_name = "Vehicle";

    DBG;
	m_mesh_names = m_app->getSettings().selectedVehicleMeshNames;
	m_material_names = m_app->getSettings().selectedVehicleMaterialNames;

    DBG;
	for (std::vector<std::string>::size_type i = 0; i != m_mesh_names.size(); i++)
		printf("\n%s", m_mesh_names[i].c_str());

    DBG;
	VehicleModel vehicleModel;
	vehicleModel.id = Bluebird;
	vehicleModel.scale = 1.0f;
	vehicleModel.height = -0.48f;

    DBG;
	std::vector<float> tirePosition = m_app->getSettings().selectedVehicleTirePositions;
	vehicleModel.tireX = tirePosition[1];
	vehicleModel.tireY = tirePosition[0];
	vehicleModel.tireFrontZ = tirePosition[2];
	vehicleModel.tireRearZ = tirePosition[3];

    DBG;
	vehicleModel.collisionBoxLength = 2.0f;
	vehicleModel.collisionBoxWidth = 0.7f;

    DBG;
	vehicleModel.transformEngine.SetOrientation(ysMath::LoadQuaternion(0.5f * ysMath::Constants::PI, ysMath::Constants::XAxis));
	vehicleModel.transformEngine.SetPosition(ysMath::LoadVector(-1.5f, 0.3f, 0.38f));

    DBG;
	m_vehicle_model = vehicleModel;

	m_vehicle_model.scale = m_app->getSettings().selectedVehicleScale;

	m_vehicle_model.transformEngine.SetParent(&m_transform);

	m_modelRotation = m_app->getSettings().selectedVehicleModelRotation;

    DBG;

	ysQuaternion qx = ysMath::LoadQuaternion(m_modelRotation[0] * ysMath::Constants::PI, ysMath::Constants::XAxis);
	ysQuaternion qy = ysMath::LoadQuaternion(m_modelRotation[1] * ysMath::Constants::PI, ysMath::Constants::YAxis);
	ysQuaternion qz = ysMath::LoadQuaternion(m_modelRotation[2] * ysMath::Constants::PI, ysMath::Constants::ZAxis);

    DBG;

	ysQuaternion orientation = ysMath::QuatMultiply(ysMath::QuatMultiply(qx, qy), qz);
	m_transform_model.SetOrientation(orientation);
	m_transform_model.SetParent(&m_transform);

    DBG;

	m_transform.SetOrientation(ysMath::Constants::QuatIdentity);
	m_transform.SetPosition(ysMath::LoadVector(0, 1, 0));
	m_transform.SetParent(nullptr);

    DBG;

	m_engineModelRotation = m_app->getSettings().selectedVehicleEngineModelRotation;
	m_engineModelPosition = m_app->getSettings().selectedVehicleEngineModelRotation;

    DBG;

	ysQuaternion eqx = ysMath::LoadQuaternion(m_engineModelRotation[0] * ysMath::Constants::PI, ysMath::Constants::XAxis);
	ysQuaternion eqy = ysMath::LoadQuaternion(m_engineModelRotation[1] * ysMath::Constants::PI, ysMath::Constants::YAxis);
	ysQuaternion eqz = ysMath::LoadQuaternion(m_engineModelRotation[2] * ysMath::Constants::PI, ysMath::Constants::ZAxis);

    DBG;

	ysQuaternion engineOrientation = ysMath::QuatMultiply(ysMath::QuatMultiply(eqx, eqy), eqz);

	m_transform_engine = m_vehicle_model.transformEngine;
	m_transform_engine.SetParent(&m_transform);
	m_transform_engine.SetOrientation(engineOrientation);
	m_transform_engine.SetPosition(ysMath::LoadVector(m_engineModelPosition[0], m_engineModelPosition[1], m_engineModelPosition[2]));

	velocity = ysMath::LoadVector();

    D("create body");
    b2BodyDef bodyDef = b2DefaultBodyDef();
    DBG;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){-14.0f, 0.0f};
    DBG;
    b2BodyId bodyId = b2CreateBody(m_world, &bodyDef);
    DBG;

    b2Polygon dynamicBox = b2MakeBox(0.5f * m_vehicle_model.collisionBoxWidth, 0.1f * m_vehicle_model.collisionBoxLength);
    DBG;
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    DBG;
    shapeDef.density = 25.0f;
    DBG;
    shapeDef.material.friction = 0.001f;
    
	DBG;

    // Add the shape to the body.
    //b2CreatePolygonShape(m_body, &shapeDef, &dynamicBox);
    DBG;

    m_body = bodyId;

    DBG;

	// Wheels
	m_tireCount = 4;

    DBG;

	// Top Down
	//m_tires[0] = new TireObject(app,world,m_vehicle,m_selected_vehicle_name,&m_transform, m_body

	m_tires[0] = new TireObject(app, m_vehicle, m_selected_vehicle_name, &m_transform, m_body, {+m_vehicle_model.tireX, m_vehicle_model.tireRearZ}, vehicleModel.tireY, true);
	m_tires[1] = new TireObject(app, m_vehicle, m_selected_vehicle_name, &m_transform, m_body, {+m_vehicle_model.tireX, m_vehicle_model.tireFrontZ}, vehicleModel.tireY, true);
	m_tires[2] = new TireObject(app, m_vehicle, m_selected_vehicle_name, &m_transform, m_body, {-m_vehicle_model.tireX, m_vehicle_model.tireRearZ}, vehicleModel.tireY, true);
	m_tires[3] = new TireObject(app, m_vehicle, m_selected_vehicle_name, &m_transform, m_body, {-m_vehicle_model.tireX, m_vehicle_model.tireFrontZ}, vehicleModel.tireY, true);

    DBG;
	//b2RevoluteJoint_SetLimits(m_tires[0]->m_joint,0,0);
    DBG;
	//m_tires[2]->m_joint->SetLimits(0.0f, 0.0f);

	bool rearWheelDrive = true;

	if (rearWheelDrive)
	{
		m_tires[0]->m_drive = true;
		m_tires[2]->m_drive = true;
	}
	else
	{
		m_tires[1]->m_drive = true;
		m_tires[3]->m_drive = true;
	}
    DBG;
}

void VehicleObject::initialize(EngineSimApplication* app)
{
}

VehicleObject::~VehicleObject()
{
}

void VehicleObject::generateGeometry()
{
	/* void */
}

void VehicleObject::render(const ViewParameters* view)
{
	// Top down
	b2Vec2 pos = b2Body_GetPosition(m_body);
	ysVector position = 
	ysMath::LoadVector(
		pos.x, m_vehicle_model.height - 0.2f, 
		pos.y - 0 * 1.3f);// = (ysVector3)m_transform.GetWorldPosition();

	rotation = -b2Body_GetRotation(m_body).c;
	//-m_body->GetAngle();

	m_transform.SetPosition(position);
	m_previousPosition = position;

	// L = T * R * S

	ysQuaternion qx = ysMath::LoadQuaternion((-0.5f) * ysMath::Constants::PI, ysMath::Constants::XAxis);
	ysQuaternion qy = ysMath::LoadQuaternion((+0.0f) * ysMath::Constants::PI, ysMath::Constants::YAxis);
	ysQuaternion qz = ysMath::LoadQuaternion((+0.0f) * ysMath::Constants::PI + rotation, ysMath::Constants::ZAxis);
	ysQuaternion orientation = ysMath::QuatMultiply(ysMath::QuatMultiply(qx, qy), qz);

	m_transform.SetOrientation(orientation);

	m_app->getShaders()->SetObjectTransform(m_transform_model.GetWorldTransform());

	if (m_app->getShowEngineOnly()) return;
	DBG;

	for (std::vector<std::string>::size_type i = 0; i != m_mesh_names.size(); i++)
	{
		if (m_app->getShowEngine2() && (m_mesh_names[i].find("engine") != std::string::npos)) break;
		DBG;
		D(m_material_names[i].c_str());
		m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial(m_material_names[i].c_str()));
		DBG;
		if (!m_app->getShowEngine() || m_mesh_names[i] != "hood")


			m_app->getEngine()->DrawModel(
				m_app->getShaders()->GetRegularFlags(),
				m_app->getAssetManager()->GetModelAsset(m_mesh_names[i].c_str()),
				1);
		DBG;

	}
	DBG;
	for (int i = 0; i < m_tireCount; i++)
		m_tires[i]->render(view);
	DBG;

	m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialWhite"));
	DBG;
}


void VehicleObject::process(float dt)
{
	if (m_app->getShowEngineOnly()) return;

	// Steering
	float angle = m_vehicle->getSteeringAngle();

	//m_tires[1]->m_joint->EnableLimit(true);
	//m_tires[3]->m_joint->EnableLimit(true);
	DBG;

	//b2RevoluteJoint_SetLimits(m_tires[1]->m_joint,angle,angle);
	//b2RevoluteJoint_SetLimits(m_tires[3]->m_joint,angle,angle);

	DBG;

	m_brakes = m_vehicle->m_brakes;

	float scale = 0.1f;

	m_wheel_rotation_speed = (1.0f - m_brakes) * scale * (float)m_vehicle->getSpeed() / (2.0f * ysMath::Constants::PI * m_tire_radius);

	float realSpeed = b2Body_GetLocalVector(m_body,b2Body_GetLinearVelocity(m_body)).y;
	//m_body->GetLocalVector(m_body->GetLinearVelocity()).y;

	DBG;

	if (m_app->getSimulator()->getTransmission()->getGear() == -1)
		m_wheel_rotation_speed = 0.99f * (1.0f - m_brakes) * scale * realSpeed / (2.0f * ysMath::Constants::PI * m_tire_radius);

	if (m_app->getSimulator()->getTransmission()->getGear() == -2)
		m_wheel_rotation -= m_wheel_rotation_speed;
	else if (m_app->getSimulator()->getTransmission()->getGear() > -2)
		m_wheel_rotation += m_wheel_rotation_speed;

	b2Vec2 currentForwardNormal = b2Body_GetWorldVector(m_body, {0, 1});

	float targetSpeedFromTireRotationSpeed = m_wheel_rotation_speed * 2.0f * ysMath::Constants::PI * m_tire_radius / scale;

	{
		for (int i = 0; i < m_tireCount; i++)
		{
			m_tires[i]->m_rotation = m_wheel_rotation;

			if (m_app->getSimulator()->getTransmission()->getGear() == -2)
				m_tires[i]->process(dt, -targetSpeedFromTireRotationSpeed);
			else
				m_tires[i]->process(dt, +targetSpeedFromTireRotationSpeed);
		}
	}

	//m_vehicle->m_rotation = -m_body->GetAngle();
	//m_vehicle->m_rotation = -m_body->GetAngle();
	m_vehicle->m_rotation = -b2Body_GetRotation(m_body).c;

	//m_body->ApplyLinearImpulse(b2Vec2(-500.0f, 0.0f), m_body->GetWorldCenter(), true);
	//m_body->ApplyForceToCenter(b2Vec2(0.0f,-10.0f*targetSpeedFromTireRotationSpeed), true);
	b2Body_ApplyForceToCenter(m_body, {0.0f,-10.0f*targetSpeedFromTireRotationSpeed}, true);
}

void VehicleObject::destroy()
{
	for (int i = 0; i < m_tireCount; i++)
	{
		m_tires[i]->destroy();
		m_tires[i] = nullptr;
	}

	b2DestroyBody(m_body);
	m_body = b2_nullBodyId;

	m_vehicle = nullptr;
}
