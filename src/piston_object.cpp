#include "../include/piston_object.h"
#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"

PistonObject::PistonObject() {
    m_piston = nullptr;
    m_wristPinHole = {};
}

PistonObject::~PistonObject() {
    /* void */
}

void PistonObject::generateGeometry() {
        /* void */
}

void PistonObject::render(const ViewParameters *view) {
    if(!m_app->m_show_engine) return;

    const int layer = m_piston->getCylinderIndex();

    resetShader();

    float scale = m_app->getSimulator()->getEngine()->scale;
    float f = scale * (float)(m_piston->getCylinderBank()->getBore() / 2);
    float scale2 = scale * (float)(m_piston->getCompressionHeight());

    setTransform(
        &m_piston->m_body,

        f,
        0.8f * scale2 + 0.2f * f, // TODO
        f,

        0.0f,
        0.1f*(float)(m_piston->getCompressionHeight()),
        m_app->getSimulator()->getEngine()->scaleZ * (float)(layer),

        0.0f,
        0.0f,
        0.0f,

        m_app->getSimulator()->getVehicle()->m_transform_engine
    );

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialEngine"));

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Piston_2JZ_GE"),
        0);
}

void PistonObject::process(float dt) {
    /* void */
}

void PistonObject::destroy() {
    /* void */
}
