#include "../include/connecting_rod_object.h"

#include "../include/engine_sim_application.h"
#include "../include/ui_utilities.h"

ConnectingRodObject::ConnectingRodObject() {
    m_connectingRod = nullptr;
}

ConnectingRodObject::~ConnectingRodObject() {
    /* void */
}

void ConnectingRodObject::render(const ViewParameters *view) {

    if (!m_app->getShowEngine()) return;

    const int layer = m_connectingRod->getLayer();
    float length = (float)m_connectingRod->getLength();

    ysVector4 scaleEnds = ysVector4(1.0f, 1.0f, 1.0f, 1.0f);
    scaleEnds.Scale((float)m_connectingRod->getCrankshaft()->getThrow());

    if(m_app->getSimulator()->getEngine()->getCylinderBankCount() > 3)
        scaleEnds.Scale(0.5f);

    if (m_app->getSimulator()->getEngine()->getEngineType() == Engine::V_COMMON)
        scaleEnds.z = 0.05f;
    else
        scaleEnds.z = 0.06f;

    ysVector4 scaleMiddle = scaleEnds;
    scaleMiddle.y *= 9.4f * length;

    if (m_app->getSimulator()->getEngine()->getEngineType() == Engine::RADIAL)
        scaleMiddle.y = 0.5f * length;
    else
        scaleMiddle.y = 0.3f * length;

    resetShader();

    float angle = 0.25f * ysMath::Constants::PI;

    setTransform(
        &m_connectingRod->m_body,

        scaleEnds,

        0.0f,
        (float)m_connectingRod->getBigEndLocal(),

        0.0f,

        layer * m_app->getCylinderDifferenceZ() + (float)m_connectingRod->getPiston()->getCylinderBank()->m_dz,

        m_app->getSimulator()->getVehicle()->m_transform_engine);

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialWhite"));

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Rod_2JZ_GE_BigEnd"),
        0);

    setTransform(
        &m_connectingRod->m_body,

        scaleEnds,

        0.0f,
        1.0f * (float)m_connectingRod->getLittleEndLocal() + 0.0f * (float)m_connectingRod->getBigEndLocal(),

        0.0f,

        layer * m_app->getCylinderDifferenceZ() + (float)m_connectingRod->getPiston()->getCylinderBank()->m_dz,

        m_app->getSimulator()->getVehicle()->m_transform_engine);

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Rod_2JZ_GE_LittleEnd"),
        0);

   
    setTransform(
        &m_connectingRod->m_body,

        scaleMiddle,

        0.0f,
        0.48f * (float)m_connectingRod->getLittleEndLocal() + 0.52f * (float)m_connectingRod->getBigEndLocal(),

        0.0f,

        layer * m_app->getCylinderDifferenceZ() + (float)m_connectingRod->getPiston()->getCylinderBank()->m_dz,

        m_app->getSimulator()->getVehicle()->m_transform_engine);

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Rod_2JZ_GE_Middle"),
        0);
}

void ConnectingRodObject::process(float dt) {
    /* void */
}

void ConnectingRodObject::destroy() {
    /* void */
}
