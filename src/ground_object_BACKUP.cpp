#include "../include/ground_object.h"

#include "../include/cylinder_bank.h"
#include "../include/engine_sim_application.h"
#include "../include/ui_utilities.h"

#define OBJECTS_NUMBER 6

GroundObject::GroundObject(EngineSimApplication* app)
{
	m_app = app;

	m_ground = nullptr;
	m_vehicle = nullptr;


	/////////// DYNAMIC 0
	int i = 0;
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(4.0f, -15.0f);
	m_bodies[i] = m_app->getWorld()->CreateBody(&bodyDef);
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.5f, 0.3f);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 0.1f;
	fixtureDef.friction = 0.9f;
	m_bodies[i]->CreateFixture(&fixtureDef);
	m_bodies[i]->SetAngularDamping(0.9f);
	m_bodies[i]->SetLinearDamping(0.9f);



	/////////// Define the ground body.
	i = 1;
	b2BodyDef groundBodyDef;
	groundBodyDef.type = b2_staticBody;
	groundBodyDef.position.Set(0.0f, 0.0f);
	m_bodies[i] = m_app->getWorld()->CreateBody(&groundBodyDef);

	b2PolygonShape box;
	box.SetAsBox(200.0f, 0.2f);
	fixtureDef.shape = &box;
	fixtureDef.density = 0.0f;
	fixtureDef.friction = 0.9f;
	m_bodies[i]->CreateFixture(&fixtureDef);
	groundBodyDef.position.Set(0.0f, 2.0f);
	m_bodies[i] = m_app->getWorld()->CreateBody(&groundBodyDef);
	m_bodies[i]->CreateFixture(&fixtureDef);

	i = 2;
	b2PolygonShape box2;
	box2.SetAsBox(20.0f, 0.4f);
	fixtureDef.shape = &box2;
	fixtureDef.density = 0.0f;

	groundBodyDef.position.Set(4.0f, -2.0f);

	groundBodyDef.angle = ( -20.0f ) * M_PI / 180.0f;

	m_bodies[i] = m_app->getWorld()->CreateBody(&groundBodyDef);
	m_bodies[i]->CreateFixture(&fixtureDef);
	//m_bodies[i]->S

	i = 3;
	box2.SetAsBox(20.0f, 0.4f);
	fixtureDef.shape = &box2;
	fixtureDef.density = 0.0f;

	groundBodyDef.position.Set(42.0f, -2.0f);

	groundBodyDef.angle = (20.0f) * M_PI / 180.0f;

	m_bodies[i] = m_app->getWorld()->CreateBody(&groundBodyDef);
	m_bodies[i]->CreateFixture(&fixtureDef);
	//m_bodies[i]->S

	i = 4;
	box2.SetAsBox(20.0f, 0.4f);
	fixtureDef.shape = &box2;
	fixtureDef.density = 0.0f;

	groundBodyDef.position.Set(-99.0f, -2.0f);

	groundBodyDef.angle = (-20.0f) * M_PI / 180.0f;

	m_bodies[i] = m_app->getWorld()->CreateBody(&groundBodyDef);
	m_bodies[i]->CreateFixture(&fixtureDef);
	//m_bodies[i]->S

	i = 5;
	b2ChainShape chain;
	//box2.SetAsBox(20.0f, 0.4f);
	//box2.SetAsBox(20.0f, 0.4f);

	int c = 100;
	b2Vec2 vertices[100];
	for(int j=0;j<99;j++)
		vertices[2] = b2Vec2(0.2f*j, 1.0f*sin(0.2f*j));
	vertices[99] = b2Vec2(0.0, -1.0f);
	chain.CreateLoop(vertices, 3);
	fixtureDef.shape = &chain;
	fixtureDef.density = 0.0f;

	groundBodyDef.position.Set(-30.0f, 0.0f);

	//groundBodyDef.angle = (20.0f) * M_PI / 180.0f;

	m_bodies[i] = m_app->getWorld()->CreateBody(&groundBodyDef);
	m_bodies[i]->CreateFixture(&fixtureDef);
	
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
	resetShader();

	if (m_app->getDebugMode())
	{
		for (int i = 0; i < OBJECTS_NUMBER ; i++)
		{
			b2Fixture* fixture = m_bodies[i]->GetFixtureList();
			b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
			b2Vec2 position = m_bodies[i]->GetPosition();
			b2Vec2 size = 2.0f * poly->m_vertices[0];
			float angle = m_bodies[i]->GetAngle();

			if (i == 1) 
				position.y -= 2.0f;

			m_app->getShaders()->UseMaterial(m_app->getAssetManager()->FindMaterial("Material__00"));

			setTransform(
				&m_atg_body,

				0.5f,
				size.y,
				size.x,

				0.0f,
				-position.y,
				position.x,

				angle,
				0.0f,
				0.0f);


			if (i == 5) 
			{
				//for (int j = 0; j < 100; j++)
				{
					//b2EdgeShape* edge;
					//m_bodies[i]->GetFixtureList()->GetShape()->GetType()->(edge, j)
					//b2Vec2 position = m_bodies[i]->GetPosition();

					setTransform(
						&m_atg_body,

						0.5f,
						size.y,
						size.x,

						0.0f,
						-position.y,
						position.x,

						angle,
						0.0f,
						0.0f);

					m_app->getEngine()->DrawModel(
						m_app->getShaders()->GetRegularFlags(),
						m_app->getAssetManager()->GetModelAsset("DebugCube"),
						0);
				}
					

			}
			else
			m_app->getEngine()->DrawModel(
				m_app->getShaders()->GetRegularFlags(),
				m_app->getAssetManager()->GetModelAsset("DebugCube"),
				0);
		}
	}
}

void GroundObject::process(float dt)
{
	/* void */

}

void GroundObject::destroy()
{
	/* void */
	for (int i = 0; i < OBJECTS_NUMBER; i++)
	{
		m_bodies[i]->GetWorld()->DestroyBody(m_bodies[i]);
		m_bodies[i] = nullptr;
	}
}
