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

void CrankshaftObject::render(const ViewParameters *view) 
{
    if (!m_app->getShowEngine()) return;

    // TODO: change setTransform function? and check how different engine types work

    const ysVector col = mix(m_app->getBackgroundColor(), m_app->getForegroundColor(), 0.7f);

    int journalCount = m_crankshaft->getRodJournalCount();

    const float scale = 1.2f;

    for (int i = 0; i < journalCount; ++i) {
        const int layer = i;

        //int journalCount = m_app->getSimulator()->getEngine()->getCrankshaft(0)->getRodJournalCount();
        int cylinderCount = m_app->getSimulator()->getEngine()->getCylinderCount();

        float deltaZ = 0.0f;
        float offsetZ = 0.0f;
        if (m_app->getSimulator()->getEngine()->getEngineType() == Engine::V_COMMON) {
            deltaZ = 0 * i * 0.25f * m_app->getCylinderDifferenceZ();
            offsetZ = 0.125f;
        }

        //resetShader();

        setTransform(
            &m_crankshaft->m_body,

            scale * (float)m_crankshaft->getThrow(),
            scale * (float)m_crankshaft->getThrow(),
            scale * (float)m_crankshaft->getThrow(),

            0.0f,
            0.0f,
            ((float)layer + offsetZ) * m_app->getCylinderDifferenceZ() + deltaZ,

            0.0f,
            0.0f,
            (float)m_crankshaft->getRodJournalAngle(i),

            m_app->getSimulator()->getVehicle()->m_transform_engine);

       /* m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("Crankshaft_2JZ_GE_V_Part_1"),
            0x32 - 0);*/

        m_app->getShaders()->SetBaseColor(col);

        if (m_app->getSimulator()->getEngine()->getCylinderBankCount() > 2) return;

        
        if (m_app->getSimulator()->getEngine()->getEngineType() == Engine::V_COMMON) {
            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("Crankshaft_2JZ_GE_V_Part_1"),
                0x32 - layer);
        }
        else {
            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("Crankshaft_2JZ_GE_Part_1"),
                0x32 - layer);
        }

        setTransform(
            &m_crankshaft->m_body,

            scale * (float)m_crankshaft->getThrow(),

            0.0f,
            0.0f,

            (float)m_crankshaft->getRodJournalAngle(i),

            ((float)(layer) + 0.5f + offsetZ) * m_app->getCylinderDifferenceZ() + deltaZ);

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("Crankshaft_2JZ_GE_Part_2"),
            0x32 - layer);
    }
}

void CrankshaftObject::process(float dt) {
    /* void */
}

void CrankshaftObject::destroy() {
    /* void */
}
