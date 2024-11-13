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
    GeometryGenerator *gen = m_app->getGeometryGenerator();

    GeometryGenerator::Circle2dParameters circleParams;
    circleParams.center_x = 0.0f;
    circleParams.center_y = (float)m_piston->getWristPinLocation();
    circleParams.maxEdgeLength = m_app->pixelsToUnits(5.0f);
    circleParams.radius = (float)(m_piston->getCylinderBank()->getBore() / 10) * 0.75f;
    gen->startShape();
    gen->generateCircle2d(circleParams);
    gen->endShape(&m_wristPinHole);
}

void PistonObject::render(const ViewParameters *view) {

    if (!m_app->getShowEngine()) return;

    const int layer = m_piston->getRod()->getLayer();

    const ysVector col = tintByLayer(m_app->getForegroundColor(), layer - view->Layer0);
    const ysVector holeCol = tintByLayer(m_app->getBackgroundColor(), layer - view->Layer0);

    resetShader();

    //m_app->getSimulator()->getVehicle()->m_transform_engine

    float scale = m_app->getScaleModel() * (float)(m_piston->getCylinderBank()->getBore() / 2);

    setTransform(
        &m_piston->m_body,

        scale,
        scale,
        scale,

        0.0f,
        0.1f * (float)(m_piston->getCompressionHeight()),
        layer * m_app->getCylinderDifferenceZ() + (float)m_piston->getCylinderBank()->m_dz,

        0.0f,
        0.0f,
        0.0f,

        m_app->getSimulator()->getVehicle()->m_transform_engine
    );

    /*setTransform(
        &m_piston->m_body,

        m_app->getScaleModel() * (float)(m_piston->getCylinderBank()->getBore() / 2),

        0.0f,
        (float)(-m_piston->getCompressionHeight() - 2.0f * m_piston->getWristPinLocation() + 0.025f),

        0.0f,
        layer * m_app->getCylinderDifferenceZ() + m_piston->getCylinderBank()->m_dz);*/

    /*
    m_app->getShaders()->SetBaseColor(col);
    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Piston"),
        0x32 - layer);*/

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialWhite"));

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Piston_2JZ_GE"),
        0);

    /*
    setTransform(&m_piston->m_body);
    m_app->getShaders()->SetBaseColor(holeCol);
    m_app->drawGenerated(m_wristPinHole, 0x32 - layer);*/
}

void PistonObject::process(float dt) {
    /* void */
}

void PistonObject::destroy() {
    /* void */
}
