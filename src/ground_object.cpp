#include "../include/ground_object.h"

#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"
#include "../include/ui_utilities.h"

using std::cout;

GroundObject::GroundObject(EngineSimApplication *app)
{
    m_app = app;

    m_ground = nullptr;
    m_vehicle = nullptr;

    D("Create dynamic Box2d-body");

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){0.0f, 4.0f};
    b2BodyId bodyId = b2CreateBody(m_app->getWorld(), &bodyDef);
    b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;
    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
    m_dynamic_bodies[0] = bodyId;

    // bodyDef = b2DefaultBodyDef();
    // bodyDef.type = b2_dynamicBody;
    // bodyDef.position = {-14.0f, 10.0f};
    // m_dynamic_bodies[0] = b2CreateBody(m_app->getWorld(), &bodyDef);
    // dynamicBox = b2MakeBox(0.5f * 1.6f, 0.5f * 4.2f);

    /* shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.1f;
    shapeDef.material.friction = 0.9f;
    b2CreatePolygonShape(m_dynamic_bodies[0], &shapeDef, &dynamicBox);

    // Add the shape to the body.
    b2Body_SetAngularDamping(m_dynamic_bodies[0], 0.9f);
    b2Body_SetLinearDamping(m_dynamic_bodies[0], 0.9f); */

    if (b2Body_IsValid(m_dynamic_bodies[0]) == false)
    {
        printf("\nm_dynamic_bodies[0] not set!\n");
        exit(1);
    }

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.

    D("Create ground body");

    // Define the ground body.
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){0.0f, -10.0f};
    b2BodyId groundId = b2CreateBody(m_app->getWorld(), &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
    m_static_bodies[0] = groundId;

    /*  b2BodyDef groundBodyDef;
     groundBodyDef.type = b2_staticBody;
     groundBodyDef.position = {-20.0f, 0.0f}; */

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    //m_static_bodies[0] = b2CreateBody(m_app->getWorld(), &groundBodyDef);
    //b2Polygon box = b2MakeBox(2.0f, 8000.0f);
    //b2ShapeDef fixtureDef = b2DefaultShapeDef();
    //fixtureDef.density = 0.0f;

    // Add the shape to the body.
    //b2CreatePolygonShape(m_static_bodies[0], &fixtureDef, &box);

    D("Create ground body 2");
    // Define the ground body.
    groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){0.0f, -10.0f};
    b2BodyId groundId2 = b2CreateBody(m_app->getWorld(), &groundBodyDef);
    b2Polygon groundBox2 = b2MakeBox(50.0f, 10.0f);
    b2ShapeDef groundShapeDef2 = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId2, &groundShapeDef2, &groundBox2);
    m_static_bodies[1] = groundId2;

    //groundBodyDef.position = {36.2f, 0.0f};
    //m_static_bodies[1] = b2CreateBody(m_app->getWorld(), &groundBodyDef);

    // Define another box shape for our dynamic body.
    //b2Polygon box2 = b2MakeBox(0.2f, 191.5f);

    // Define the dynamic body fixture.
    //shapeDef = b2DefaultShapeDef();

    //shapeDef.density = 0.0f;

    // b2CreatePolygonShape(m_static_bodies[1], &shapeDef, &dynamicBox);

    //groundBodyDef.position = {0.0f, 109.4f};
    //m_static_bodies[2] = b2CreateBody(m_app->getWorld(), &groundBodyDef);

    D("Ground objects done");
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
    // return;
    if (m_app->getShowEngineOnly())
        return;

    b2Vec2 impulse = {b2Body_GetMass(m_dynamic_bodies[0]), 0.0f};
    b2Body_ApplyLinearImpulseToCenter(m_dynamic_bodies[0], impulse, true);

    resetShader();

    int i = 0;

    int l = 20;

    float scale = 0.1f;

    setTransform(
        &m_body,

        100.0f,
        100.0f,
        100.0f,

        0 * 20.0f,
        -0.7f,
        i * 15.0f + 2.0f,

        -0.5f * ysMath::Constants::PI,
        0.0f * ysMath::Constants::PI,
        -0.5f * ysMath::Constants::PI);

    // return;

    m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialLittleCity"));

    setTransform(
        &m_body,

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
    if (m_app->getSelectedTrack() == 2)
    {
        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialTunnel"));
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("TrackMonza"),
            0);
    }

    // City
    if (m_app->getSelectedTrack() == 1)
    {
        setTransform(
            &m_body,

            0.005f * scale,
            0.005f * scale,
            0.005f * scale,

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

    if (m_app->getSelectedTrack() == 3)
    {

        /*
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("track"),
            0);*/

        float size = 36.0f;
        float scale2 = 10.0f * scale;

        // ROUND
        for (int i = 0; i < 360; i += 10)
        {
            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Cone"));

            int z = 450;

            if (i > 180)
                z = 0;

            float x = (float)i / 180.0f * ysMath::Constants::PI;

            setTransform(
                &m_body,

                0.5f * scale,
                0.5f * scale,
                0.5f * scale,

                17.0f * cos(x),
                -0.7f,
                -100.0f + z + 17.0f * sin(x),

                0 * 0.5f * ysMath::Constants::PI,
                0 * 0.5f * ysMath::Constants::PI,
                1.0f * ysMath::Constants::PI);

            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("cone"),
                0);
        }

        for (float x = -36.0f * 40; x < 4000.0f; x += size)
        {
            setTransform(
                &m_body,

                scale2,
                scale2,
                scale2,

                0.0f,
                -0.7f,
                x,

                -0.5f * ysMath::Constants::PI,
                0 * 0.5f * ysMath::Constants::PI,
                0.0f * ysMath::Constants::PI);

            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Highway"));

            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("Motorway"),
                0);

            setTransform(
                &m_body,

                scale2,
                scale2,
                scale2,

                -size,
                -0.7f,
                x,

                -0.5f * ysMath::Constants::PI,
                0 * 0.5f * ysMath::Constants::PI,
                0.0f * ysMath::Constants::PI);

            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Grass"));

            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("Motorway"),
                0);

            setTransform(
                &m_body,

                scale2,
                scale2,
                scale2,

                size,
                -0.7f,
                x,

                -0.5f * ysMath::Constants::PI,
                0 * 0.5f * ysMath::Constants::PI,
                0.0f * ysMath::Constants::PI);

            // m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Grass"));

            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("Motorway"),
                0);

            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Lightpole"));

            int i = (int)x;

            if (i % (2 * 72) == 0)
            {

                setTransform(
                    &m_body,

                    0.4f * scale,
                    0.4f * scale,
                    0.4f * scale,

                    -size / 2.0f - 1.0f,
                    -1.15f,
                    x,

                    0 * 0.5f * ysMath::Constants::PI,
                    0 * 0.5f * ysMath::Constants::PI,
                    1.0f * ysMath::Constants::PI);

                m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("light_01"),
                    0);
            }

            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Guardrail"));

            setTransform(
                &m_body,

                0.2f * scale,
                0.2f * scale,
                0.2f * scale,

                -size / 2.0f + 0.1f,
                -0.5f,
                0.066f * x,

                -0.5f * ysMath::Constants::PI,
                0.0f * ysMath::Constants::PI,
                0.5f * ysMath::Constants::PI);

            if (false && i == 0)
            {
                m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("guardrail_02"),
                    0);
            }
            else
            {
                m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("guardrail_01"),
                    0);
            }

            if (i % (4 * 144) == 0)
            {
                m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Speedlimit75"));

                setTransform(
                    &m_body,

                    0.4f * scale,
                    0.4f * scale,
                    0.4f * scale,

                    -size / 2.0f - 1.0f,
                    -0.7f,
                    x + 10.0f,

                    0 * 0.5f * ysMath::Constants::PI,
                    0 * 0.5f * ysMath::Constants::PI,
                    1.0f * ysMath::Constants::PI);

                m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("speedlimit_75"),
                    0);
            }

            if (i % (36) == 0)
            {
                m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("ConcreteBlock"));

                setTransform(
                    &m_body,

                    0.2f * scale,
                    0.2f * scale,
                    0.2f * scale,

                    0.0f,
                    -0.7f,
                    20.0f + 0.07f * x,

                    0 * 0.5f * ysMath::Constants::PI,
                    0.5f * ysMath::Constants::PI,
                    1.0f * ysMath::Constants::PI);

                m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("concreteblock"),
                    0);
            }

            /*
            if (i % (36) == 0)
            {
                m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Cone"));

                setTransform(
                    &m_body,

                    0.5f * scale,
                    0.5f * scale,
                    0.5f * scale,

                    0.0f,
                    -0.7f,
                    x + 20.0f,

                    0 * 0.5f * ysMath::Constants::PI,
                    0 * 0.5f * ysMath::Constants::PI,
                    1.0f * ysMath::Constants::PI);


                m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("cone"),
                    0);
            }*/
        }
    }

    // Track Garda
    if (m_app->getSelectedTrack() == 4)
    {

        setTransform(
            &m_body,

            10.0f * scale,
            10.0f * scale,
            10.0f * scale,

            /*
            -24.0f,
            -1.15f,
            0.0f,*/

            0.0f,
            -1.15f,
            0.0f,

            0 * -0.5f * ysMath::Constants::PI,
            0 * 0.5f * ysMath::Constants::PI,
            0.0f * ysMath::Constants::PI);

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialMat"));

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("track"),
            0);

        /*
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("tile01"),
            0);*/

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialStandard"));

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("grass"),
            0);

        // m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialFence"));

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

    if (m_app->getSelectedTrack() == 4)
    {
        scale *= 10.0f;

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialCity2"));

        setTransform(
            &m_body,

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
        &m_body,

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
            &m_body,

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
