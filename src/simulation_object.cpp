#include "../include/simulation_object.h"

#include "../include/piston.h"
#include "../include/cylinder_bank.h"

#include "../include/engine_sim_application.h"

SimulationObject::SimulationObject() {
    m_app = nullptr;
}

SimulationObject::~SimulationObject() {
    /* void */
}

void SimulationObject::initialize(EngineSimApplication *app) {
    m_app = app;
}

void SimulationObject::generateGeometry() {
    /* void */
}

void SimulationObject::render(const ViewParameters *settings) {
    /* void */
}

void SimulationObject::process(float dt) {
    /* void */
}

void SimulationObject::destroy() {
    /* void */
}

Piston *SimulationObject::getForemostPiston(CylinderBank *bank, int layer) {
    Engine *engine = m_app->getSimulator()->getEngine();
    Piston *frontmostPiston = nullptr;
    const int cylinderCount = engine->getCylinderCount();
    for (int i = 0; i < cylinderCount; ++i) {
        Piston *piston = engine->getPiston(i);
        if (piston->getCylinderBank() == bank) {
            if (piston->getRod()->getJournal() >= layer) {
                if (frontmostPiston == nullptr
                    || piston->getRod()->getJournal() < frontmostPiston->getRod()->getJournal()) {
                    frontmostPiston = piston;
                }
            }
        }
    }

    return frontmostPiston;
}

void SimulationObject::resetShader() {
    m_app->getShaders()->ResetBaseColor();
    m_app->getShaders()->SetObjectTransform(ysMath::LoadIdentity());
}

void SimulationObject::setTransform(
    atg_scs::RigidBody* rigidBody,
    float scaleX,
    float scaleY,
    float scaleZ,
    float lx,
    float ly,
    float lz,
    float angleX,
    float angleY,
    float angleZ,
    ysTransform* parentTransform)
{
    //scaleX = 10.0f;
    //scaleY = 10.0f;
    //scaleZ = 10.0f;
    
    double p_x, p_y;

    ysQuaternion qz = ysMath::LoadQuaternion(0, ysMath::Constants::ZAxis);

    if (rigidBody != nullptr) {
        rigidBody->localToWorld(lx, ly, &p_x, &p_y);
        qz = ysMath::LoadQuaternion((float)rigidBody->theta + angleZ, ysMath::Constants::ZAxis);
    }
    else {
        p_x = lx;
        p_y = lx;
    }

    const ysMatrix scaleTransform = ysMath::ScaleTransform(ysMath::LoadVector((float)scaleX, (float)scaleY, (float)scaleZ));

    //L = T * R * S
    ysQuaternion qx = ysMath::LoadQuaternion(angleX, ysMath::Constants::XAxis);
    ysQuaternion qy = ysMath::LoadQuaternion(angleY, ysMath::Constants::YAxis);

    ysTransform transform;
    transform.SetOrientation(ysMath::QuatMultiply(ysMath::QuatMultiply(qx, qy), qz));
    //transform.SetOrientation(ysMath::QuatMultiply(ysMath::QuatMultiply(qx,qy),qx));

    if (parentTransform != nullptr)
    {
        transform.SetParent(parentTransform);
    }

    transform.SetPosition(ysMath::LoadVector((float)p_x, (float)p_y, lz, 0.0f));

    if (m_app != nullptr)
        m_app->getShaders()->SetObjectTransform(ysMath::MatMult(transform.GetWorldTransform(), scaleTransform));
        
}
/*
void SimulationObject::setTransform(
    atg_scs::RigidBody *rigidBody,
    float scale,
    float lx,
    float ly,
    float angle,
    float z)
{
    double p_x, p_y;
    rigidBody->localToWorld(lx, ly, &p_x, &p_y);

    const ysMatrix rot = ysMath::RotationTransform(
            ysMath::Constants::ZAxis,
            (float)rigidBody->theta + angle);
    const ysMatrix trans = ysMath::TranslationTransform(
            ysMath::LoadVector((float)p_x, (float)p_y, z));
    const ysMatrix scaleTransform = ysMath::ScaleTransform(ysMath::LoadScalar(scale));

    m_app->getShaders()->SetObjectTransform(
        ysMath::MatMult(ysMath::MatMult(trans, rot), scaleTransform));
}
*/

void SimulationObject::setTransform(
    atg_scs::RigidBody* rigidBody,
    float scale,
    float lx,
    float ly,
    float angle,
    float z)
{
    double p_x, p_y;
    rigidBody->localToWorld(lx, ly, &p_x, &p_y);

    const ysMatrix rot = ysMath::RotationTransform(
        ysMath::Constants::ZAxis,
        (float)rigidBody->theta + angle);
    const ysMatrix trans = ysMath::TranslationTransform(
        ysMath::LoadVector((float)p_x, (float)p_y, z));
    const ysMatrix scaleTransform = ysMath::ScaleTransform(ysMath::LoadVector(scale));

    m_app->getShaders()->SetObjectTransform(
        ysMath::MatMult(ysMath::MatMult(trans, rot), scaleTransform));
}

void SimulationObject::setTransform(
    atg_scs::RigidBody* rigidBody,
    ysVector4 scale,
    float lx,
    float ly,
    float angle,
    float z,
    ysTransform* parentTransform)
{
    double p_x, p_y;
    rigidBody->localToWorld(lx, ly, &p_x, &p_y);

    const ysMatrix rot = ysMath::RotationTransform(
        ysMath::Constants::ZAxis,
        (float)rigidBody->theta + angle);
    const ysMatrix trans = ysMath::TranslationTransform(
        ysMath::LoadVector((float)p_x, (float)p_y, z));
    const ysMatrix scaleTransform = ysMath::ScaleTransform(ysMath::LoadVector(scale));

    ysTransform transform;
    //transform.SetOrientation(ysMath::QuatMultiply(ysMath::QuatMultiply(qx, qy), qz));
    //transform.SetOrientation(ysMath::QuatMultiply(ysMath::QuatMultiply(qx,qy),qx));

    if (parentTransform != nullptr)
        transform.SetParent(parentTransform);

    transform.SetPosition(ysMath::LoadVector((float)p_x, (float)p_y, z, 0.0f));

    ysQuaternion qz = ysMath::LoadQuaternion((float)rigidBody->theta + angle, ysMath::Constants::ZAxis);

    transform.SetOrientation(qz);

    if (m_app != nullptr)
        m_app->getShaders()->SetObjectTransform(ysMath::MatMult(transform.GetWorldTransform(), scaleTransform));

    //m_app->getShaders()->SetObjectTransform(
        //ysMath::MatMult(ysMath::MatMult(trans, rot), scaleTransform));
}

ysVector SimulationObject::tintByLayer(const ysVector &col, int layers) const {
    ysVector result = col;
    for (int i = 0; i < layers; ++i) {
        result = ysMath::Add(
            ysMath::Mul(result, ysMath::LoadScalar(0.3f)),
            ysMath::Mul(m_app->getBackgroundColor(), ysMath::LoadScalar(0.7f))
        );
    }

    return result;
}
