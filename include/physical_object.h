#pragma once
#include <map>
#include <memory>
#include <vector>
#include <btBulletDynamicsCommon.h>
#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
//#include "CommonInterfaces/CommonRigidBodyBase.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
//#include "engine_sim_application.h"
#include "../include/engine_sim_application.h"
//#include "yds_math.h"

class ysTransform;
class EngineSimApplication;

// Collider shape
enum Shape 
{
	CUBE,
	SPHERE,
	CYLINDER,
	CAPSULE,
	CONVEX,
	TREE_COLLISION
};

enum PhysicsType 
{
	STATIC,
	DYNAMIC,
};

class PhysicalObject;

class World
{
public:
	World(EngineSimApplication *app);
	~World();
	void Initialize();
    void AddCollisionShape(btCollisionShape *shape);
    void AddRigidBody(btRigidBody* body);
	void Update(float delta, int maxSubSteps=10);
	btDiscreteDynamicsWorld *getDynamicsWorld();
    PhysicalObject* GetObject(std::string name);

private:
	void AddPhysicalObject(PhysicalObject physicalObject);
	btDefaultCollisionConfiguration* collision_configuration;
	btCollisionDispatcher* dispatcher; // Collision dispatcher, handles collision
	btBroadphaseInterface* overlapping_pair_cache; // Broadphase interface, detects overlapping objects
	btSequentialImpulseConstraintSolver* solver; // Constraint solver
	btDiscreteDynamicsWorld* dynamics_world; // The world where physics takes place
	btAlignedObjectArray<btCollisionShape*> collision_shapes; // Keeps track of collision shapes
	friend class PhysicalObject;
	EngineSimApplication *m_app;
};

class PhysicalObject
{
public:
    PhysicalObject(
		std::string name,
		World *world, 
		btVector3 position, 
		btVector3 rotation, 
		btVector3 size, 
		Shape shape, 
		PhysicsType type, 
		float mass, 
		std::string model_asset="",
		bool scaleModelBySize=true,
		btVector3 modelTranslation=btVector3(0,0,0),
		std::string textureName="");
	~PhysicalObject();
    btVector3 GetPosition();
    void AddImpulse(btVector3 impulse);
    void ApplyForce(btVector3 force);
	void ApplyTorque(btVector3 torque);
    void Render(float scale=1.0f, std::string model_asset="");
	void DebugRender();
	bool IsReady();
	bool SetTexture(int materialID, std::string textureFilePath);
	void setScale(btVector3 scale);
	void setModelTransform();
	ysTransform *getTransform();
	btRigidBody *getBody();
	btVector3 getLinearVelocity();
	btQuaternion getOrientation();

protected:
	btRigidBody* body;
	btCollisionShape* collider_shape;
	btVector3 modelScale;
	btVector3 debugModelScale;
	btVector3 modelTranslation;
	std::string name;
	friend class World;
	bool isGeneratedModel = false;
	World* m_world = nullptr;
	std::string m_model_asset = "";
	ysMatrix m_modelTransform;
	ysTransform m_transform;
};