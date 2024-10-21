#include "../include/ground_object.h"

#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"
#include "../include/ui_utilities.h"

GroundObject::GroundObject(EngineSimApplication *app)
{
    m_app = app;

    m_ground = nullptr;
    m_vehicle = nullptr;
}

void GroundObject::initialize(EngineSimApplication *app)
{
    m_app = app;
}

GroundObject::~GroundObject()
{
    /* void */
}

void GroundObject::generateGeometry()
{
    /* void */
}

void GroundObject::render(const ViewParameters *view)
{
    resetShader();

    int i = 0;

    int l = 20;

    float scale = 1.0f;

    setTransform(
            &m_ground->m_body,

            scale,
            scale,
            scale,

            0*-24.0f,
            0*-1.15f,
            0.0f,

            0 * -0.5f * ysMath::Constants::PI,
            0*0.5f * ysMath::Constants::PI,
            0.0f * ysMath::Constants::PI);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialMat"));

        /* m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("track"),
            0); */
    //return;

    setTransform(
        &m_ground->m_body,

        100.0f,
        100.0f,
        100.0f,

        0 * 20.0f,
        -0.7f,
        i * 15.0f + 2.0f,

        -0.5f * ysMath::Constants::PI,
        0.0f * ysMath::Constants::PI,
        -0.5f * ysMath::Constants::PI);

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialLittleCity"));

    setTransform(
        &m_ground->m_body,

        400 * scale,
        400 * scale,
        400 * scale,

        0.0f,
        -0.7f,
        0.0f,

        -0.5f * ysMath::Constants::PI,
        1.0f * ysMath::Constants::PI,
        0.5f * ysMath::Constants::PI);


    // Monza
    if (m_app->m_selected_track == 2)
    {
        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialTunnel"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("TrackMonza"),
            0);
    }

    // City
    if (m_app->m_selected_track == 1)
    {
        setTransform(
            &m_ground->m_body,

            0.005 * scale,
            0.005 * scale,
            0.005 * scale,

            0.0f,
            -0.7f,
            0.0f,

            1.5f * ysMath::Constants::PI,
            0.5f * ysMath::Constants::PI,
            0.5f * ysMath::Constants::PI);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialPawn"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialCityRoads"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.001"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialParamount"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.002"),
            0);

        // m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialCityRoads"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.003"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFiller"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.004"),
            0);

        // m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFiller"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.005"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialProps"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.006"),
            0);

        // m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFiller"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.007"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Material_03"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.008"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialGreen"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.009"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialWhite"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.010"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFiller"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.011"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFiller"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.012"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialBasketball"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.013"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialClinic"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.014"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialProjects"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.015"),
            0);

        // m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFiller"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.016"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialWhite"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.017"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialLaundry"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.018"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFish"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.019"),
            0);

        // m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFiller"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.020"),
            0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialPawn"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city.021"),
            0);
    }

    // Track Garda
    if (m_app->m_selected_track == 3)
    {
        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialStandard"));

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("grass"),
            0);

        setTransform(
            &m_ground->m_body,

            scale,
            scale,
            scale,

            0*-24.0f,
            0*-1.15f,
            0.0f,

            0 * -0.5f * ysMath::Constants::PI,
            0*0.5f * ysMath::Constants::PI,
            0.0f * ysMath::Constants::PI);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialMat"));

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("track"),
            0);

        /* m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialStandard"));

        setTransform(
            &m_ground->m_body,

            scale,
            scale,
            scale,

            0*-24.0f,
            -0.1f,
            0.0f,

            0 * -0.5f * ysMath::Constants::PI,
            0*0.5f * ysMath::Constants::PI,
            0.0f * ysMath::Constants::PI);

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("grass"),
            0); */

        //m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFence"));

        if (false)
            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("fence"),
                0);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialParking"));

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("parking"),
            0);
    }

    if (m_app->m_selected_track == 4)
    {
        scale *= 10.0f;

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialCity2"));

        setTransform(
            &m_ground->m_body,

            scale,
            scale,
            scale,

            -1 * 15.0f,
            -0.5f,
            i * 15.0f,

            0 * -0.5f * ysMath::Constants::PI,
            0.0f * ysMath::Constants::PI,
            0.0f * ysMath::Constants::PI);

        // if(false)
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("city2"),
            0);
    }

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Material_04"));

    setTransform(
        &m_ground->m_body,

        scale,
        scale,
        scale,

        -1 * 15.0f,
        -0.7f,
        i * 15.0f,

        -0.5f * ysMath::Constants::PI,
        0.0f * ysMath::Constants::PI,
        0.0f * ysMath::Constants::PI);

    if (false)
    {
        setTransform(
            &m_ground->m_body,

            0.3f * scale,
            0.3f * scale,
            0.3f * scale,

            -1 * 15.0f,
            -0.7f,
            i * 15.0f,

            -0.5f * ysMath::Constants::PI,
            0.0f * ysMath::Constants::PI,
            0.0f * ysMath::Constants::PI);

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("Road_02"),
            0);
    }

    /*
    m_app->getEngine()->DrawModel(
        m_app->getShaders()->GetRegularFlags(),
        m_app->getAssetManager()->GetModelAsset("Straight_15"),
        0);
    */
    //}
}

void GroundObject::process(float dt)
{
    /* void */
}

void GroundObject::destroy()
{
    /* void */
}
