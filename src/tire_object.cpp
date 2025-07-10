#include "../include/tire_object.h"

#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"
#include "../include/ui_utilities.h"

TireObject::TireObject(EngineSimApplication *app, b2WorldId world, Vehicle *vehicle, std::string selectedVehicleName, ysTransform *vehicleTransform, b2BodyId vehicleBody, b2Vec2 localPosition, float height, bool steering)
{
    m_app = app;
    m_world = world;

    m_height = height;

    m_mesh_names = m_app->getIniReader().GetVector<std::string>(selectedVehicleName, "TireMeshes");
    m_material_names = m_app->getIniReader().GetVector<std::string>(selectedVehicleName, "TireMaterials");
    m_modelRotation = m_app->getIniReader().GetVector<float>(selectedVehicleName, "TireModelRotation");

    ysQuaternion qx = ysMath::LoadQuaternion(m_modelRotation[0] * ysMath::Constants::PI, ysMath::Constants::XAxis);
    ysQuaternion qy = ysMath::LoadQuaternion(m_modelRotation[1] * ysMath::Constants::PI, ysMath::Constants::YAxis);
    ysQuaternion qz = ysMath::LoadQuaternion(m_modelRotation[2] * ysMath::Constants::PI, ysMath::Constants::ZAxis);

    ysQuaternion orientation = ysMath::QuatMultiply(qx, qy);
    m_transform_model.SetOrientation(orientation);

    m_vehicle_body = vehicleBody;
    m_vehicle_transform = vehicleTransform;
    m_vehicle = vehicle;

    m_drive = false;

    m_material = nullptr;
    m_texture = nullptr;
    m_rotation = 0.0f;
    position_x = -0.52f;

    m_steering = steering;

    if (localPosition.x < 0.0f)
        m_side = Left;
    else
        m_side = Right;

    // Define the dynamic body. We set its position and call the body factory.
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = b2Body_GetPosition(m_vehicle_body) + localPosition;
    //m_body = world->CreateBody(&bodyDef);
    m_body = b2CreateBody(world,&bodyDef);

    // Define another box shape for our dynamic body.
    b2Polygon dynamicBox = b2MakeBox(0.08f, 0.26f);

    /*
    b2CircleShape circle;
    //circle.m_p.Set(2.0f, 3.0f);
    circle.m_radius = 1.0f*0.26f;
    */

    // Define the dynamic body fixture.
    b2ShapeDef shapeDef = b2DefaultShapeDef();

    // Set the box density to be non-zero, so it will be dynamic.
    shapeDef.density = 2.0f;

    // Override the default friction.
    shapeDef.material.friction = 0.3f;
    shapeDef.material.restitution = 0.9f;

    // Add the shape to the body.
    b2CreatePolygonShape(m_body, &shapeDef, &dynamicBox);

    b2Vec2 pivot = {-10.0f, 20.5f};
b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
jointDef.motorSpeed = 1.0f;
jointDef.maxMotorTorque = 100.0f;
jointDef.enableMotor = true;
jointDef.lowerAngle = -0.25f * B2_PI;
jointDef.upperAngle = 0.5f * B2_PI;
jointDef.enableLimit = true;
b2JointId jointId = b2CreateRevoluteJoint(world, &jointDef);

    b2RevoluteJointDef revoluteJointDef = b2DefaultRevoluteJointDef();
    //revoluteJointDef.bodyA = m_vehicle_body;
    //revoluteJointDef.bodyB = m_body;
    //revoluteJointDef.collideConnected = false;
    //revoluteJointDef.localAnchorA.Set(localPosition.x, localPosition.y);
    //revoluteJointDef.localAnchorB.Set(0, 0); // center of the circle

    if (steering)
    {
        revoluteJointDef.enableMotor = true;
        revoluteJointDef.maxMotorTorque = 100000.0f;
        revoluteJointDef.enableLimit = true;
    }

    //m_joint = (b2RevoluteJoint *)m_world->CreateJoint(&revoluteJointDef);
    m_joint = b2CreateRevoluteJoint(m_world, &jointDef);

    if (steering)
        b2RevoluteJoint_SetLimits(m_joint,-0.8f,0.8f);
}

TireObject::~TireObject()
{
    b2DestroyJoint(m_joint);
    m_joint = b2_nullJointId;
    b2DestroyBody(m_body);
    m_body = b2_nullBodyId;
}

b2Vec2 TireObject::getLateralVelocity()
{
    b2Vec2 currentRightNormal = b2Body_GetWorldVector(m_body,{1,0});
    return b2Dot(currentRightNormal, b2Body_GetLinearVelocity(m_body)) * currentRightNormal;
}

b2Vec2 TireObject::getForwardVelocity()
{
    b2Vec2 currentForwardNormal = b2Body_GetWorldVector(m_body,{0,1});
    return b2Dot(currentForwardNormal, b2Body_GetLinearVelocity(m_body)) * currentForwardNormal;
}

void TireObject::updateFriction()
{
    // lateral linear velocity
    // float maxLateralImpulse = 2.5f;
    float maxLateralImpulse = 40.0f;
    b2Vec2 impulse = b2Body_GetMass(m_body) * -getLateralVelocity();
    if (b2Length(impulse) > maxLateralImpulse)
        impulse *= maxLateralImpulse / b2Length(impulse);
    b2Body_ApplyLinearImpulseToCenter(m_body, impulse, true);

    // angular velocity
    b2Body_ApplyAngularImpulse(m_body,0.1f * b2Body_GetRotationalInertia(m_body) * -b2Body_GetAngularVelocity(m_body), true);

    // forward linear velocity
    b2Vec2 currentForwardNormal = getForwardVelocity();
    float currentForwardSpeed = b2Length(currentForwardNormal);
    float dragForceMagnitude = -2 * currentForwardSpeed;
    b2Body_ApplyForceToCenter(m_body,dragForceMagnitude * b2Normalize(currentForwardNormal), true);
}

void TireObject::updateDrive()
{
    // find desired speed
    float desiredSpeed = (float)m_vehicle->getSpeed();
    float m_maxDriveForce = 100.0f;

    // find current speed in forward direction
    b2Vec2 currentForwardNormal = b2Body_GetWorldVector(m_body, {0,1});
    //m_body->GetWorldVector(b2Vec2(0, 1));
    float currentSpeed = b2Dot(getForwardVelocity(), currentForwardNormal);

    // apply necessary force
    float force = 0;
    if (desiredSpeed > currentSpeed)
        force = m_maxDriveForce;
    else if (desiredSpeed < currentSpeed)
        force = -m_maxDriveForce;
    else
        return;

    //m_body->ApplyForce(force * currentForwardNormal, m_body->GetWorldCenter(), true);
    b2Body_ApplyForce(m_body,force * currentForwardNormal, b2Body_GetWorldCenterOfMass(m_body), true);
}

void TireObject::render(const ViewParameters *view)
{

    resetShader();

    //b2ShapeDef *fixture = m_body->GetFixtureList();
    b2ShapeId* shapes = {};
    b2Body_GetShapes(m_body, shapes, b2Body_GetShapeCount(m_body));
    b2Polygon poly = b2Shape_GetPolygon(shapes[0]);
    //(b2Polygon *)fixture->GetShape();
    b2Vec2 position1 = b2Body_GetPosition(m_body);
    //m_body->GetPosition();
    b2Vec2 size = 2.0f * poly.vertices[0];
    float angle = b2Body_GetRotation(m_body).c;
    //m_body->GetAngle();

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialWhite"));

    // Top down

    setTransform(
        &m_vehicle->m_body,

        size.x,
        0.5f,
        size.y,

        position1.x,
        -0.5f,
        position1.y,

        0.0f,
        -angle,
        0.0f);

    // Side
    /*
    setTransform(
        &m_vehicle->m_body,

        0.5f,
        size.y,
        size.x,

        0.0f,
        position1.y,
        -position1.x,

        0.0f,
        -angle,
        0.0f);
    */

    if (m_app->getDebugMode())
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("DebugCube"),
            0);

    float scale = 0.6f;
    double speedfactor = m_app->getSimulator()->getSimulationSpeed();

    if (position_x > 0.58f)
        position_x = -0.58f;

    double p_x, p_y;
    m_atg_body.localToWorld(b2Body_GetPosition(m_body).x, b2Body_GetPosition(m_body).y, &p_x, &p_y);

    // L = T * R * S
    //
    // Top Down
    ysQuaternion qx = ysMath::LoadQuaternion(m_rotation * (float)m_app->getSimulator()->getSimulationSpeed(), ysMath::Constants::XAxis);
    ysQuaternion qy = ysMath::LoadQuaternion(-b2Body_GetRotation(m_body).c, ysMath::Constants::YAxis);

    // Side
    // ysQuaternion qx = ysMath::LoadQuaternion(m_body->GetAngle(), ysMath::Constants::XAxis);

    ysTransform transform;
    // transform.SetOrientation(qx);
    transform.SetOrientation(ysMath::QuatMultiply(qy, qx));

    // transform.SetPosition(ysMath::LoadVector(0.0f, -(float)p_y, (float)p_x, 0.0f));

    transform.SetPosition(ysMath::LoadVector((float)p_x, m_height, (float)p_y, 0.0f));

    m_app->getShaders()->SetObjectTransform(transform.GetWorldTransform());

    ///////////
    ysQuaternion qz3 = ysMath::LoadQuaternion(m_modelRotation[2] * ysMath::Constants::PI, ysMath::Constants::ZAxis);

    ysTransform transform2;
    transform2.SetOrientation(qz3);
    transform2.SetParent(&transform);
    // transform2.SetPosition(ysMath::LoadVector((float)p_x, -0.45f, (float)p_y, 0.0f));

    m_app->getShaders()->SetObjectTransform(transform2.GetWorldTransform());
    /////////

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialWheel"));

    int meshesPerTireModel = (int)m_mesh_names.size() / 2;

    // int meshesPerTireModel = 2;

    for (int i = 0; i != meshesPerTireModel; i++)
    {
        int j = i;
        if (m_side == Right)
            j += meshesPerTireModel;

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial(m_material_names[j].c_str()));

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset(m_mesh_names[j].c_str()),
            1);
    }

    if (false)
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("DebugCube"),
            0);
}

void TireObject::process(float dt, float rotationSpeed)
{
    // float frictionLimit = 0.6f;
    float frictionLimit = 0.9f;

    b2Vec2 currentForwardNormal = b2Body_GetWorldVector(m_body, {0,1});
    //m_body->GetWorldVector(b2Vec2(0, 1));

    b2Vec2 lv = getLateralVelocity();

    if (m_drive)
        lv += getForwardVelocity() - rotationSpeed * currentForwardNormal;

    //float l = lv.Length();
    float l = b2Length(lv);

    // float factor = 0.91f;
    float factor = 0.7f;

    if (l > frictionLimit)
        lv = factor * (frictionLimit / l * lv) + (1.0f - factor) * lv;

    // Update friction
    b2Vec2 impulse = b2Body_GetMass(m_body) * -lv;

    //m_body->ApplyLinearImpulse(impulse, m_body->GetWorldCenter(), true);
    b2Body_ApplyLinearImpulseToCenter(m_body, impulse, true);

    //m_body->ApplyAngularImpulse(0.2f * m_body->GetInertia() * -m_body->GetAngularVelocity(), true);
    //b2Body_ApplyAngularImpulse(m_body, 0.2f * m_body->GetInertia() * -m_body->GetAngularVelocity(), true);
    b2Body_ApplyAngularImpulse(m_body, 0.2f * b2Body_GetRotationalInertia(m_body) * -b2Body_GetAngularVelocity(m_body), true);
    // m_body->ApplyLinearImpulse(0.3f * rotationSpeed * m_body->GetWorldVector(b2Vec2(0.0f, 1.0f)), m_body->GetWorldCenter(), true);
    // m_body->ApplyAngularImpulse(0.1f*rotationSpeed, true );
    // m_body->SetAngularVelocity(10.0f * rotationSpeed);
    // m_joint->SetMotorSpeed(rotationSpeed);
    // m_app->getSimulator()->getVehicle()->
    // m_joint->SetMaxMotorTorque(9.0f);
    // m_joint->SetMotorSpeed(rotationSpeed);
}

void TireObject::destroy()
{
    /* void */
}