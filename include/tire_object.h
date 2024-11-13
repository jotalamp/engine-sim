#ifndef ATG_ENGINE_SIM_TIRE_OBJECT_H
#define ATG_ENGINE_SIM_TIRE_OBJECT_H

#include "../dependencies/submodules/box2d/include/box2d/box2d.h"

#include "simulation_object.h"

#include "tire.h"
#include "vehicle.h"
#include "geometry_generator.h"

class TireObject : public SimulationObject {
    enum Side
    {
        Left,
        Right
    };

    public:
        TireObject(EngineSimApplication *app, b2World* world, Vehicle *vehicle, std::string selectedVehicleName, ysTransform *vehicleTransform, b2Body *vehicleBody, b2Vec2 localPosition, float height, bool steering);
        virtual ~TireObject();

        virtual void render(const ViewParameters *view);
        virtual void process(float dt, float rotationSpeed);
        virtual void destroy();
        inline b2Vec2 getLateralVelocity();
        inline b2Vec2 getForwardVelocity();
        inline void updateFriction();
        inline void updateDrive();

        b2World *m_world;
        Vehicle *m_vehicle;
        b2Body *m_vehicle_body;
        b2Body *m_body;
        b2RevoluteJoint* m_joint;
        atg_scs::RigidBody m_atg_body;
        Side m_side;
        bool m_steering;
        bool m_drive;
        float m_rotation;
        float m_rotation_speed;
        float position_x;
        dbasic::Material *m_material;
        ysTexture* m_texture;
        ysTransform *m_vehicle_transform;
        float m_height;
    protected:
        std::vector<std::string> m_mesh_names;
        std::vector<std::string> m_material_names;
        ysTransform m_transform_model;
        std::vector<float> m_modelRotation;
};

#endif /* ATG_ENGINE_SIM_TIRE_OBJECT_H */
