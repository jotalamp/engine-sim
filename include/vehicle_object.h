#ifndef ATG_ENGINE_SIM_VEHICLE_OBJECT_H
#define ATG_ENGINE_SIM_VEHICLE_OBJECT_H

#include "simulation_object.h"

#include "vehicle.h"
#include "tire_object.h"
#include "geometry_generator.h"

enum VehicleModelID {
    Niva,
    MR2,
    Seben,
    Bluebird
};

struct VehicleModel {
    VehicleModelID id;
    float scale;
    float height;
    float tireX;
    float tireY;
    float tireFrontZ;
    float tireRearZ;
    float collisionBoxLength;
    float collisionBoxWidth;
    ysTransform transformEngine;
};

class VehicleObject : public SimulationObject {
public:
    VehicleObject(EngineSimApplication *app, b2World *world, Vehicle *vehicle);
    virtual ~VehicleObject();

    virtual void initialize(EngineSimApplication *app);
    virtual void generateGeometry();
    virtual void render(const ViewParameters *view);
    virtual void process(float dt);
    virtual void destroy();
    

    inline int getTireCount() const { return m_tireCount; }

    std::string getSelectedVehicleName() { return m_selected_vehicle_name; }

    b2World *m_world;
    Vehicle *m_vehicle;
    b2Body *m_body;
    TireObject *m_tires[4];
    float rotation;
    VehicleModel m_vehicle_model;
    // float position_x;
    // float position_y;
    // ysVector m_translation;
    // ysTransform m_transform;
    ysTransform m_transform;
    ysTransform m_transform_engine;
    ysTransform m_transform_camera;
    ysTransform m_transform_model;
    dbasic::Material *m_material;
    ysTexture *m_texture;
    
    ysVector velocity;
    float m_wheel_rotation;
    float m_wheel_rotation_speed;
    float m_tire_radius;
    float m_brakes;
protected:
    int m_tireCount;
    EngineSimApplication *m_app;
    std::vector<std::string> m_mesh_names;
    std::vector<std::string> m_material_names;
    std::string m_selected_vehicle_name;
    std::vector<float> m_modelRotation;
    std::vector<float> m_engineModelRotation;
    std::vector<float> m_engineModelPosition;
    ysVector m_previousPosition;
    float m_steeringAngle;

};

#endif /* ATG_ENGINE_SIM_VEHICLE_OBJECT_H */
