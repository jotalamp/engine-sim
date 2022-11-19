#include "../include/crankshaft_object.h"

#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"
#include "../include/ui_utilities.h"

CrankshaftObject::CrankshaftObject() {
    m_crankshaft = nullptr;
}

CrankshaftObject::~CrankshaftObject() {
    /* void */
}

void CrankshaftObject::generateGeometry() {
    /* void */
}

void CrankshaftObject::render(const ViewParameters *view) {
    if(!m_app->m_show_engine) return;

    const int journalCount = m_crankshaft->getRodJournalCount();

    float scale = m_app->getSimulator()->getEngine()->scale * (float)m_crankshaft->getThrow();

    //ysVector t = m_app->getSimulator()->getVehicle()->m_translation;

    for (int i = 0; i < journalCount; ++i) 
    {
        const int layer = i;

        float layerZ = m_app->getSimulator()->getEngine()->scaleZ;

        {
            resetShader();

            setTransform(
                &m_crankshaft->m_body,
                
                scale,
                scale,
                1.0f,

                0.0f,
                0,
                layerZ * layer,

                0.0f,
                0.0f,
                (float)m_crankshaft->getRodJournalAngle(i),

                m_app->getSimulator()->getVehicle()->m_transform_engine
            );

            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialEngine"));

            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("Crankshaft_2JZ_GE_Part_1"),
                0x32 - layer);

            float f = m_app->getSimulator()->getEngine()->scale * (float)m_crankshaft->getThrow();

            float angle = m_app->getSimulator()->getVehicle()->m_rotation;
            float z = layerZ * layer - 0.5f * layerZ;

            setTransform(
                &m_crankshaft->m_body,

                f,
                f,
                scale,

                0.0f,
                0.0f,
                z,

                0.0f,
                0*angle,
                (float)m_crankshaft->getRodJournalAngle(i),

                m_app->getSimulator()->getVehicle()->m_transform_engine
            );

            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("Crankshaft_2JZ_GE_Part_2"),
                0x32 - layer);
        }
    }

    setTransform(
        &m_crankshaft->m_body,

        scale,
        scale,
        scale,

        0.0f,
        0.0f,
        0.0f,

        0.0f*ysMath::Constants::PI,
        0.0f*ysMath::Constants::PI,
        0.0f,

        m_app->getSimulator()->getVehicle()->m_transform_engine);
    //m_app->getShaders()->SetBaseColor(grey1);
    /*
    m_app->getShaders()->SetBaseColor(ysColor::srgbiToLinear(0xFF0F0F));
    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("CrankSnout"),
        0x32);

    m_app->getShaders()->SetBaseColor(grey2);
    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("CrankSnoutThreads"),
        0x32);*/
}

void CrankshaftObject::process(float dt) {
    /* void */
}

void CrankshaftObject::destroy() {
    /* void */
}
