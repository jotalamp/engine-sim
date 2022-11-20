#include "../include/cylinder_head_object.h"
#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"
#include "../include/constants.h"

CylinderHeadObject::CylinderHeadObject() {
    m_head = nullptr;
    m_engine = nullptr;
    m_cylinderIndex = -1;
}

CylinderHeadObject::~CylinderHeadObject() {
    /* void */
}

void CylinderHeadObject::generateGeometry() {
    /* void */
}

void CylinderHeadObject::render(const ViewParameters *view) {
    if(!m_app->m_show_engine) return;
    
    resetShader();
    ysTransform *parentTransform = m_app->getSimulator()->getVehicle()->m_transform_engine;
    ysMatrix parentMatrix = parentTransform->GetWorldTransform();

    CylinderBank *bank = m_head->getCylinderBank();
    const double s = (float)bank->getBore() / 2.0f;
    const double boreSurfaceArea = constants::pi * bank->getBore() * bank->getBore() / 4.0;
    const double chamberHeight = m_head->getCombustionChamberVolume() / boreSurfaceArea;

    Piston *frontmostPiston = getForemostPiston(bank, m_cylinderIndex);

    if (frontmostPiston == nullptr) return;

    const double theta = bank->getAngle();
    double x, y;
    bank->getPositionAboveDeck(chamberHeight, &x, &y);

    const ysMatrix scale = ysMath::ScaleTransform(ysMath::LoadScalar((float)s));

    const ysMatrix translation = ysMath::TranslationTransform(
            ysMath::LoadVector((float)x, (float)y, m_cylinderIndex*m_app->getSimulator()->getEngine()->scaleZ));

    const ysMatrix T_headObject 
        = ysMath::MatMult(parentMatrix, ysMath::MatMult(translation, ysMath::RotationTransform(ysMath::Constants::ZAxis, (float)theta)));

    const ysMatrix scaleHead = ysMath::ScaleTransform( m_engine->scale * ysMath::LoadScalar((float)s));

    const ysMatrix T_head   = ysMath::MatMult(T_headObject, scale);
    const ysMatrix T_head2  = ysMath::MatMult(T_headObject, scaleHead);

    constexpr float rollerRadius = (float)units::distance(300.0, units::thou);

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialEngine"));

    if(m_cylinderIndex == 0) 
    {
        m_app->getShaders()->SetObjectTransform(T_head2);
        
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("Head_2JZ_GE_Start"),
            0x0
        );
    }

    if(m_cylinderIndex == m_app->getSimulator()->getEngine()->getCylinderCount()/m_app->getSimulator()->getEngine()->getCylinderBankCount()-1) 
    {
        m_app->getShaders()->SetObjectTransform(T_head2);
        
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("Head_2JZ_GE_End"),
            0x0
        );
    }
    else if(m_cylinderIndex > 0) 
    {
        m_app->getShaders()->SetObjectTransform(T_head2);
        
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("Head_2JZ_GE_Middle"),
            0x0
        );
    }

    m_app->getShaders()->SetObjectTransform(T_head);

    const double intakeValvePosition = (m_head->getFlipDisplay())
        ? 0.5f
        : -0.5f;

    const int layer = frontmostPiston->getCylinderIndex();

    const float intakeLift = (float)m_head->intakeValveLift(layer);
    
    const ysMatrix T_intakeValve = ysMath::MatMult(
            T_head,
            ysMath::TranslationTransform(
                ysMath::LoadVector(
                    (float)intakeValvePosition,
                    (float)(-intakeLift / s),
                    0.0f,
                    0.0f)));

    m_app->getShaders()->SetObjectTransform(T_intakeValve);
    m_app->getShaders()->SetBaseColor(m_app->getBlue());

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Valve3D"),
        0x33);

    const double exhaustLift = (float)m_head->exhaustValveLift(layer);

    const ysMatrix T_exhaustValve = ysMath::MatMult(
        T_head,
        ysMath::TranslationTransform(
            ysMath::LoadVector(
                (float)(-intakeValvePosition),
                (float)(-exhaustLift / s),
                0.0f,
                0.0f)));

    m_app->getShaders()->SetObjectTransform(T_exhaustValve);
    m_app->getShaders()->SetBaseColor(m_app->getYellow());

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Valve3D"),
        0x33);

    m_app->getShaders()->SetBaseColor(m_app->getYellow());

    Camshaft *intakeCam = m_head->getIntakeCamshaft();
    Camshaft *exhaustCam = m_head->getExhaustCamshaft();

    ysMatrix T_exhaustCam = ysMath::MatMult(
        T_headObject,
        ysMath::ScaleTransform(ysMath::LoadVector(1.0f,1.0f,9.1f*m_app->getSimulator()->getEngine()->scaleZ)));

    T_exhaustCam = ysMath::MatMult(
        T_exhaustCam,
        ysMath::TranslationTransform(ysMath::LoadVector(
            (float)(-intakeValvePosition * s),
            m_app->pixelsToUnits(5.0f) / 2 + (float)(1.99 * s + exhaustCam->getBaseRadius() + rollerRadius),
            0.0f,
            0.0f)));

    T_exhaustCam = ysMath::MatMult(
        T_exhaustCam,
        ysMath::RotationTransform(
            ysMath::Constants::ZAxis,
            (float)(
                exhaustCam->getAngle()
                + exhaustCam->getLobeCenterline(layer))));

    m_app->getShaders()->SetObjectTransform(T_exhaustCam);
    m_app->getShaders()->SetBaseColor(m_app->getYellow());

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Camshaft_2JZ_GE"),
        0x32 - layer);

    ysMatrix T_intakeCam = ysMath::MatMult(
        T_headObject,
        ysMath::ScaleTransform(ysMath::LoadVector(1.0f,1.0f,9.1f*m_app->getSimulator()->getEngine()->scaleZ)));

    T_intakeCam = ysMath::MatMult(
        T_intakeCam,
        ysMath::TranslationTransform(ysMath::LoadVector(
            (float)(intakeValvePosition * s),
            rollerRadius + m_app->pixelsToUnits(5.0f) / 2 + (float)(1.99 * s + intakeCam->getBaseRadius()),
            0.0f,
            0.0f)));

    T_intakeCam = ysMath::MatMult(
        T_intakeCam,
        ysMath::RotationTransform(
            ysMath::Constants::ZAxis,
            (float)(
                0.0f*ysMath::Constants::PI + intakeCam->getAngle()
                + intakeCam->getLobeCenterline(layer))));

    m_app->getShaders()->SetObjectTransform(T_intakeCam);
    m_app->getShaders()->SetBaseColor(m_app->getBlue());

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Camshaft_2JZ_GE"),
        0x32 - layer);
}

void CylinderHeadObject::process(float dt) {
    /* void */
}

void CylinderHeadObject::destroy() {
    /* void */
}

void CylinderHeadObject::generateCamshaft(
    Camshaft *camshaft,
    double padding,
    double rollerRadius,
    GeometryGenerator::GeometryIndices *indices)
{
    GeometryGenerator::Cam2dParameters params;
    params.baseRadius = (float)(camshaft->getBaseRadius() + padding);
    params.center_x = 0.0f;
    params.center_y = 0.0f;
    params.rollerRadius = (float)rollerRadius;
    params.lift = camshaft->getLobeProfile();
    params.maxEdgeLength = (float)units::distance(50, units::thou);

    m_app->getGeometryGenerator()->startShape();
    m_app->getGeometryGenerator()->generateCam(params);
    m_app->getGeometryGenerator()->endShape(indices);
}
