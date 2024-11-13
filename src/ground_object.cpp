#include "../include/ground_object.h"

#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"
#include "../include/ui_utilities.h"

GroundObject::GroundObject(EngineSimApplication* app)
{
    m_app = app;

    m_ground = nullptr;
    m_vehicle = nullptr;

    // Define the ground body.
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(-14.0f, 10.0f);

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    m_dynamic_bodies[0] = m_app->getWorld()->CreateBody(&bodyDef);

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(0.5f * 1.6f, 0.5f * 4.2f);

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 0.1f;
    fixtureDef.friction = 0.9f;

    // Add the shape to the body.
    m_dynamic_bodies[0]->CreateFixture(&fixtureDef);
    m_dynamic_bodies[0]->SetAngularDamping(0.9f);
    m_dynamic_bodies[0]->SetLinearDamping(0.9f);

    //////////7
    // Define the ground body.
    b2BodyDef groundBodyDef;
    groundBodyDef.type = b2_staticBody;
    groundBodyDef.position.Set(-20.0f, 0.0f);

    // Call the body factory which allocates memory for the ground body
    // from a pool and creates the ground box shape (also from a pool).
    // The body is also added to the world.
    m_static_bodies[0] = m_app->getWorld()->CreateBody(&groundBodyDef);

    // Define another box shape for our dynamic body.
    b2PolygonShape box;
    box.SetAsBox(2.0f, 8000.0f);

    // Define the dynamic body fixture.
    // b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 0.0f;

    // Add the shape to the body.
    m_static_bodies[0]->CreateFixture(&fixtureDef);

    groundBodyDef.position.Set(36.2f, 0.0f);
    m_static_bodies[1] = m_app->getWorld()->CreateBody(&groundBodyDef);
    m_static_bodies[1]->CreateFixture(&fixtureDef);

    // Define another box shape for our dynamic body.
    b2PolygonShape box2;
    box2.SetAsBox(0.2f, 191.5f);

    // Define the dynamic body fixture.
    // b2FixtureDef fixtureDef;
    fixtureDef.shape = &box2;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 0.0f;

    groundBodyDef.position.Set(0.0f, 109.4f);
    m_static_bodies[2] = m_app->getWorld()->CreateBody(&groundBodyDef);
    m_static_bodies[2]->CreateFixture(&fixtureDef);
}

void GroundObject::initialize(EngineSimApplication* app)
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

void GroundObject::render(const ViewParameters* view)
{
    //return;
    if(m_app->getShowEngineOnly()) return;

    b2Vec2 impulse = m_dynamic_bodies[0]->GetMass() * b2Vec2(0.0f, 0.6f);
    m_dynamic_bodies[0]->ApplyLinearImpulse(impulse, m_dynamic_bodies[0]->GetWorldCenter(), true);

    resetShader();

    if (false/*m_app->m_debug*/)
    {
        b2Fixture* fixture = m_dynamic_bodies[0]->GetFixtureList();
        b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
        b2Vec2 position = m_dynamic_bodies[0]->GetPosition();
        b2Vec2 size = 2.0f * poly->m_vertices[0];
        float angle = m_dynamic_bodies[0]->GetAngle();

        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialWhite"));

        setTransform(
            &m_atg_body,

            size.x,
            0.5f,
            size.y,

            position.x,
            -0.5f,
            position.y,

            0.0f,
            -angle,
            0.0f);

        /*
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("DebugCube"),
            0);*/

        setTransform(
            &m_atg_body,

            size.x,
            size.x,
            size.x,

            position.x,
            -1.1f,
            position.y,

            0.5f * ysMath::Constants::PI,
            -angle + 0 * 0.5f * ysMath::Constants::PI,
            0.5f * ysMath::Constants::PI);

        /*
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("body.004"),
            0);*/

        setTransform(
            &m_atg_body,

            -0.6f * size.x,
            0.6f * size.x,
            0.6f * size.x,

            position.x,
            -0.8f,
            position.y,

            0 * 0.5f * ysMath::Constants::PI,
            -angle + 0 * 0.5f * ysMath::Constants::PI,
            0 * 0.5f * ysMath::Constants::PI);

        //m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Impreza"));
        m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("RallyCar_03"));

        /*
        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("Subaru Impreza"),
            0);*/

        m_app->getEngine()->DrawModel(
            m_app->getShaders()->GetRegularFlags(),
            m_app->getAssetManager()->GetModelAsset("Ford Cosworth"),
            0);

        if (false || m_app->getDebugMode()) {
            int i = 2;
            b2Fixture* fixture2 = m_static_bodies[i]->GetFixtureList();
            b2PolygonShape* poly2 = (b2PolygonShape*)fixture2->GetShape();
            b2Vec2 position2 = m_static_bodies[i]->GetPosition();
            b2Vec2 size2 = 2.0f * poly2->m_vertices[i];
            float angle2 = m_static_bodies[0]->GetAngle();

            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("MaterialWhite"));

            setTransform(
                &m_atg_body,

                size2.x,
                0.5f,
                size2.y,

                position2.x,
                -0.5f,
                position2.y,

                0.0f,
                -angle2,
                0.0f);

            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("DebugCube"),
                0);
        }


    }



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

    //return;

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

            if (i > 180) z = 0;

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

        for (float x = -36.0f * 40; x < 4000.0f; x += size) {
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

            //m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Grass"));

            m_app->getEngine()->DrawModel(
                m_app->getShaders()->GetRegularFlags(),
                m_app->getAssetManager()->GetModelAsset("Motorway"),
                0);

            m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Lightpole"));

            int i = x;

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

            if (false && i == 0) {
                m_app->getEngine()->DrawModel(
                    m_app->getShaders()->GetRegularFlags(),
                    m_app->getAssetManager()->GetModelAsset("guardrail_02"),
                    0);
            }
            else {
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
