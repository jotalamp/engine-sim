#include "../include/connecting_rod_object.h"

#include "../include/engine_sim_application.h"
#include "../include/units.h"
#include "../include/ui_utilities.h"

ConnectingRodObject::ConnectingRodObject()
{
    m_connectingRod = nullptr;
}

ConnectingRodObject::~ConnectingRodObject()
{
    /* void */
}

void ConnectingRodObject::generateGeometry()
{
    GeometryGenerator *gen = m_app->getGeometryGenerator();
    const int rodJournalCount = m_connectingRod->getRodJournalCount();

    GeometryGenerator::Line2dParameters params;
    params.x0 = params.x1 = 0;
    params.y0 = (float)(m_connectingRod->getBigEndLocal() + m_connectingRod->getCrankshaft()->getThrow() * 0.6);
    params.y1 = (float)m_connectingRod->getLittleEndLocal();
    params.lineWidth = (float)(m_connectingRod->getCrankshaft()->getThrow() * 0.5);

    gen->startShape();
    gen->generateLine2d(params);

    if (rodJournalCount > 0)
    {
        GeometryGenerator::Circle2dParameters circleParams;
        circleParams.radius = static_cast<float>(m_connectingRod->getSlaveThrow()) * 1.5f;
        circleParams.center_x = 0.0f;
        circleParams.center_y = static_cast<float>(m_connectingRod->getBigEndLocal());

        gen->generateCircle2d(circleParams);
    }

    gen->endShape(&m_connectingRodBody);

    if (rodJournalCount > 0)
    {
        gen->startShape();

        GeometryGenerator::Circle2dParameters circleParams;
        circleParams.radius = static_cast<float>(m_connectingRod->getCrankshaft()->getThrow()) * 0.2f;
        for (int i = 0; i < rodJournalCount; ++i)
        {
            double x, y;
            m_connectingRod->getRodJournalPositionLocal(i, &x, &y);

            circleParams.center_x = static_cast<float>(x);
            circleParams.center_y = static_cast<float>(y);

            gen->generateCircle2d(circleParams);
        }

        gen->endShape(&m_pins);
    }
}

void ConnectingRodObject::render(const ViewParameters *view)
{
    if(!m_app->m_show_engine) return;
    //const int layer = m_connectingRod->getLayer();

    resetShader();

    float scale = m_app->getSimulator()->getEngine()->scale;

    float f = scale * (float)m_connectingRod->getCrankshaft()->getThrow();

    float f2 = 12.9f * (float)m_connectingRod->getLittleEndLocal() - (float)m_connectingRod->getBigEndLocal();

    setTransform(
        &m_connectingRod->m_body,

        f2,
        f2,
        1.0f,

        0.0f,
        (float)m_connectingRod->getBigEndLocal(),
        m_app->getSimulator()->getEngine()->scaleZ * z,

        0,
        0,
        0,
        
        m_app->getSimulator()->getVehicle()->m_transform_engine);

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialEngine"));

    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Rod_2JZ_GE"),
        0);
}

void ConnectingRodObject::process(float dt)
{
    /* void */
}

void ConnectingRodObject::destroy()
{
    /* void */
}
