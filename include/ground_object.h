#ifndef ATG_ENGINE_SIM_GROUND_OBJECT_H
#define ATG_ENGINE_SIM_GROUND_OBJECT_H

#include "simulation_object.h"

#include "ground.h"
#include "vehicle.h"
#include "geometry_generator.h"

#include "../dependencies/submodules/box2d/include/box2d/box2d.h"

class GroundObject : public SimulationObject {
    public:
        GroundObject(EngineSimApplication *app);
        virtual ~GroundObject();

        virtual void initialize(EngineSimApplication *app);
        virtual void generateGeometry();
        virtual void render(const ViewParameters *view);
        virtual void process(float dt);
        virtual void destroy();

        atg_scs::RigidBody m_atg_body;
        Ground *m_ground;
        Vehicle *m_vehicle;
        b2Body *m_dynamic_bodies[1];
        b2Body *m_static_bodies[3];
        b2Body* m_bodies[2];
        atg_scs::RigidBody m_body;
};

#endif /* ATG_ENGINE_SIM_GROUND_OBJECT_H */
