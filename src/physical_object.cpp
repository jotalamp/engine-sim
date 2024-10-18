#include "../include/physical_object.h"
#include <stdio.h>
#include "../include/engine_sim_application.h"

World::World(EngineSimApplication *app)
{
	m_app = app;
	Initialize();
}

World::~World()
{
	// Cleanup

	// Remove rigid bodies
	for (int i = dynamics_world->getNumCollisionObjects() - 1; i >= 0; i--) 
	{
		btCollisionObject* obj = dynamics_world->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		dynamics_world->removeCollisionObject(obj);
		delete obj;
	}

	// Delete collision shapes
	for (int i = 0; i < collision_shapes.size(); i++) 
	{
		btCollisionShape* shape = collision_shapes[i];
		collision_shapes[i] = 0;
		delete shape;
	}

	delete dynamics_world;
	delete solver;
	delete overlapping_pair_cache;
	delete dispatcher;
	delete collision_configuration;
}

void World::Initialize()
{
	// Initialize Bullet
	collision_configuration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_configuration);
	overlapping_pair_cache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamics_world = new btDiscreteDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -9.81, 0));
	//this->debugDraw = new DebugDraw();
	//dynamics_world->setDebugDrawer(this->debugDraw);
}

void World::AddPhysicalObject(PhysicalObject physicalObject)
{
	collision_shapes.push_back(physicalObject.collider_shape);
	dynamics_world->addRigidBody(physicalObject.body); // Add the body to the dynamics world
}

void World::Update(float delta, int maxSubSteps)
{
	dynamics_world->stepSimulation(delta, maxSubSteps); // Update physics
}



PhysicalObject::PhysicalObject(
    std::string name,
    World *world,
    btVector3 position = {0, 0, 0},
    btVector3 rotation = {0, 0, 0},
    btVector3 size = {1, 1, 1},
    Shape shape = CUBE,
    PhysicsType type = STATIC,
    float mass = 0,
    std::string model_asset,
    bool scaleModelBySize,
    btVector3 modelTranslation,
    std::string textureName)
{
	m_world = world;
	this->name = name;

	m_modelTransform = ysMatrix();

	debugModelScale = size;

	if(scaleModelBySize)
	{
		this->modelScale=size;
	}
	else
	{
		this->modelScale = btVector3(1,1,1);
	}

	
	this->modelTranslation=modelTranslation;

	if(model_asset.compare("")==0)
	{
		m_model_asset = "DebugCube";
	}
	else
	{
		m_model_asset = model_asset;
	}

	switch(shape)
	{
		case CUBE:
			collider_shape = new btBoxShape(0.5f*size);
			//if(isGeneratedModel) model = LoadModelFromMesh( GenMeshCube(size[0], size[1], size[2]) );
			break;
		case SPHERE:
			collider_shape = new btSphereShape( size[0] );
			//if(isGeneratedModel) model = LoadModelFromMesh( GenMeshSphere(size[0], 32, 64) );
			break;
		case CYLINDER:
			{
				collider_shape = new btCylinderShape(0.5f*size);

				//Mesh mesh = GenMeshCylinder(0.5f*size[0], size[1], 64);
				
				/* for (int k = 0; k < mesh.vertexCount; k++)
				{
					mesh.vertices[k*3 + 1] -= 0.5f * size[1];
				} */

				//UpdateMeshBuffer(mesh, 0, mesh.vertices, sizeof(float) * mesh.vertexCount * 3, 0);

				if(isGeneratedModel)
				{
					//model = LoadModelFromMesh( mesh );
				}
				else
				{
					//debugModel = LoadModelFromMesh(mesh);
				}
				break;
			}
		case CAPSULE:
			collider_shape = new btCapsuleShape(0.5f*size[0], 0.5f*size[1]);
			//if(isGeneratedModel) model = LoadModelFromMesh( GenMeshCylinder(0.5f*size[0], size[1], 64) );
			break;
		case CONVEX:
			{
				btConvexHullShape *tempHull = new btConvexHullShape();
				
				//int meshCount = model.meshCount;
				/* for(int m=0;m<meshCount;m++)
				{
					int vertexCount = model.meshes[m].vertexCount;

					printf("\n\n%i\n",vertexCount);

					for (int i = 0; i < vertexCount*3; i+=3)
					{
						btVector3 btv = modelScale*btVector3(model.meshes[m].vertices[i],model.meshes[m].vertices[i+1],model.meshes[m].vertices[i+2]);
						((btConvexHullShape*)tempHull)->addPoint(btv);
					}
				} */

				 //Optimize the shape
                btShapeHull shapeHull(tempHull);
                shapeHull.buildHull(0.01f);
                delete tempHull;

				collider_shape = new btConvexHullShape(reinterpret_cast<const btScalar*>(shapeHull.getVertexPointer()), shapeHull.numVertices());
				break;
			}
		case TREE_COLLISION:
			btTriangleMesh* mesh = new btTriangleMesh();
			//int meshCount = model.meshCount;
			/* for(int m=0;m<meshCount;m++)
			{
				int vertexCount = model.meshes[m].vertexCount;
				int triangleCount = model.meshes[m].triangleCount;

				for (int i = 0; i < triangleCount; i+=3)
				{
					int a = 3*model.meshes[m].indices[i*3+0];
					int b = 3*model.meshes[m].indices[i*3+1];
					int c = 3*model.meshes[m].indices[i*3+2];
					btVector3 bv1 = modelScale*btVector3(model.meshes[m].vertices[a+0],model.meshes[m].vertices[a+1],model.meshes[m].vertices[a+2]);
					btVector3 bv2 = modelScale*btVector3(model.meshes[m].vertices[b+0],model.meshes[m].vertices[b+1],model.meshes[m].vertices[b+2]);
					btVector3 bv3 = modelScale*btVector3(model.meshes[m].vertices[c+0],model.meshes[m].vertices[c+1],model.meshes[m].vertices[c+2]);
					mesh->addTriangle(bv1, bv2, bv3);
				}     
			} */
		    collider_shape = new btBvhTriangleMeshShape(mesh, true);
			break;
	}

	if(textureName.compare("")!=0)
	{
		//this->texture = LoadTexture(textureName.c_str());
		// Assign texture to default model material
		//SetTextureFilter(this->texture, TEXTURE_FILTER_ANISOTROPIC_8X); // Makes the texture smoother when upscaled
		//model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
	}

	/* model.materials[0].maps[MATERIAL_MAP_ALBEDO].color = Color({255,255,255,255});
    model.materials[0].maps[MATERIAL_MAP_METALNESS].value = 1.0f;
    model.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 0.0f;
    model.materials[0].maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
    model.materials[0].maps[MATERIAL_MAP_EMISSION].color = BLACK; */

	// Set location and rotation
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(position);
	transform.setRotation( btQuaternion(rotation[2], rotation[1], rotation[0]) );

	btScalar object_mass(mass); // Set the object's mass

	// Calculate local inertia for dynamic objects
	btVector3 local_inertia(0, 0, 0);
	if (type == DYNAMIC || mass != 0.0) // Objects with 0.0 mass are static
		collider_shape->calculateLocalInertia(mass, local_inertia);

	btDefaultMotionState* motion_state = new btDefaultMotionState(transform);

	btRigidBody::btRigidBodyConstructionInfo rb_info(object_mass, motion_state, collider_shape, local_inertia);
	rb_info.m_restitution = 0.7f;
	body = new btRigidBody(rb_info);
	body->setDamping(0.2f,0.2f);
	//body->setActivationState(DISABLE_DEACTIVATION);
	//body->setDeactivationTime(20000.0f);
	body->setSleepingThresholds(0.01f,0.01f);

	//world->AddPhysicalObject(*this);
	world->collision_shapes.push_back(collider_shape);
	world->dynamics_world->addRigidBody(body); // Add the body to the dynamics world

	printf("[ %s ] created!\n", name.c_str());
}

PhysicalObject::~PhysicalObject()
{
	printf("[ %s ] destroyed!\n", name.c_str());
}

btVector3 PhysicalObject::GetPosition()
{
	btTransform trans;

	// Get the transform of the body
	if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);
	else
		return btVector3();


	// Get position from transform
	btVector3 position = trans.getOrigin();

    return position;
}

void PhysicalObject::AddImpulse(btVector3 impulse)
{
	body->activate(true);
	body->applyCentralImpulse(impulse);
}

void PhysicalObject::ApplyForce(btVector3 force)
{
	body->activate(true);
	body->applyCentralForce(force);
}

void PhysicalObject::ApplyTorque(btVector3 torque)
{
	body->activate(true);
	body->applyTorque(torque);
}

void PhysicalObject::Render(float scale, std::string model_asset)
{
	btTransform trans;

	// Get the transform of the body
	if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);
	else
		return;

	// Get position from transform
	btVector3 position = trans.getOrigin();
	btVector3 modelPosition = position + modelTranslation;
	
	// Get rotation from transform
	btQuaternion quat = trans.getRotation();

    //L = T * R * S
	btVector3 quat_axis = quat.getAxis();
	ysQuaternion quat2 = ysMath::LoadQuaternion(quat.getAngle(), ysMath::LoadVector(quat_axis.x(), quat_axis.y(), quat_axis.z()));

	ysTransform transform;

	transform.SetOrientation(quat2);

	transform.SetPosition(ysMath::LoadVector(position.x(),position.y(),position.z(),0));

	const ysMatrix scaleTransform = ysMath::ScaleTransform(ysMath::LoadVector(modelScale.x(),modelScale.y(),modelScale.z()));

	m_world->m_app->getShaders()->SetObjectTransform(ysMath::MatMult(transform.GetWorldTransform(), scaleTransform));
	//m_world->m_app->getShaders()->SetObjectTransform(ysMath::MatMult(ysMath::MatMult(transform.GetWorldTransform(), scaleTransform),m_modelTransform));

	m_world->m_app->getShaders()->UseMaterial(m_world->m_app->getAssetManager()->FindMaterial("MaterialWhite"));

	if(model_asset.compare("")==0) model_asset = m_model_asset;

	m_world->m_app->getEngine()->DrawModel(
                m_world->m_app->getShaders()->GetRegularFlags(),
                m_world->m_app->getAssetManager()->GetModelAsset(model_asset.c_str()),
                1);
}

void PhysicalObject::DebugRender()
{
	btTransform trans;

	// Get the transform of the body
	if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);
	else
		return;

	// Get position from transform
	btVector3 position = trans.getOrigin();
	btVector3 modelPosition = position + modelTranslation;
	
	// Get rotation from transform
	btQuaternion quat = trans.getRotation();

    //L = T * R * S
	btVector3 quat_axis = quat.getAxis();
	ysQuaternion quat2 = ysMath::LoadQuaternion(quat.getAngle(), ysMath::LoadVector(quat_axis.x(), quat_axis.y(), quat_axis.z()));

	ysTransform transform;

	transform.SetOrientation(quat2);

	transform.SetPosition(ysMath::LoadVector(position.x(),position.y(),position.z(),0));

	const ysMatrix scaleTransform = ysMath::ScaleTransform(ysMath::LoadVector(debugModelScale.x(),debugModelScale.y(),debugModelScale.z()));

	m_world->m_app->getShaders()->SetObjectTransform(ysMath::MatMult(transform.GetWorldTransform(), scaleTransform));

	m_world->m_app->getShaders()->UseMaterial(m_world->m_app->getAssetManager()->FindMaterial("MaterialWhite"));

	m_world->m_app->getEngine()->DrawModel(
                m_world->m_app->getShaders()->GetRegularFlags(),
                m_world->m_app->getAssetManager()->GetModelAsset("DebugCube"),
                1);
}

void PhysicalObject::setScale(btVector3 scale)
{
	modelScale = scale;
}

void PhysicalObject::setModelTransform()
{
}

ysTransform *PhysicalObject::getTransform()
{
	btTransform trans;

	// Get the transform of the body
	if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);

	// Get position from transform
	btVector3 position = trans.getOrigin();
	btVector3 modelPosition = position + modelTranslation;
	
	// Get rotation from transform
	btQuaternion quat = trans.getRotation();

    //L = T * R * S
	btVector3 quat_axis = quat.getAxis();
	ysQuaternion quat2 = ysMath::LoadQuaternion(quat.getAngle(), ysMath::LoadVector(quat_axis.x(), quat_axis.y(), quat_axis.z()));

	m_transform.SetOrientation(quat2);

	m_transform.SetPosition(ysMath::LoadVector(position.x(),position.y(),position.z(),0));

	return &m_transform;
}

btDiscreteDynamicsWorld *World::getDynamicsWorld()
{
    return dynamics_world;
}

btRigidBody *PhysicalObject::getBody()
{
    return body;
}

btVector3 PhysicalObject::getLinearVelocity()
{
    return body->getLinearVelocity();
}

btQuaternion PhysicalObject::getOrientation()
{
	btTransform trans;

	// Get the transform of the body
	if (body && body->getMotionState())
		body->getMotionState()->getWorldTransform(trans);
	else
		return btQuaternion();

	// Get position from transform
	btVector3 position = trans.getOrigin();
	btVector3 modelPosition = position + modelTranslation;
	
	// Get rotation from transform
	btQuaternion quat = trans.getRotation();

    return quat;
}