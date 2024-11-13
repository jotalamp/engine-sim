#ifndef ATG_ENGINE_SIM_SIMULATION_OBJECT_H
#define ATG_ENGINE_SIM_SIMULATION_OBJECT_H

#include "scs.h"
#include "delta.h"

class Piston;
class CylinderBank;
class EngineSimApplication;
class SimulationObject {
    public:
        struct ViewParameters {
            int Layer0;
            int Layer1;
            int Sublayer;
        };

    public:
        SimulationObject();
        virtual ~SimulationObject();

        virtual void initialize(EngineSimApplication *app);
        virtual void generateGeometry();
        virtual void render(const ViewParameters *settings);
        virtual void process(float dt);
        virtual void destroy();

        Piston *getForemostPiston(CylinderBank *bank, int layer);

    protected:
        void resetShader();
        void setTransform(
            atg_scs::RigidBody* rigidBody,
            float scaleX,
            float scaleY,
            float scaleZ,
            float lx,
            float ly,
            float lz,
            float angleX,
            float angleY,
            float angleZ,
            ysTransform* parentTransform = nullptr);

        void setTransform(
            atg_scs::RigidBody* rigidBody,
            ysVector4 scale = ysVector4(1.0f, 1.0f, 1.0f, 1.0f),
            float lx = 0.0f,
            float ly = 0.0f,
            float theta = 0.0f,
            float z = 0.0f,
            ysTransform* parentTransform = nullptr);

        void SimulationObject::setTransform(
            atg_scs::RigidBody* rigidBody,
            float scale = 1.0f,
            float lx = 0.0f,
            float ly = 0.0f,
            float theta = 0.0f,
            float z = 0.0f);

        ysVector tintByLayer(const ysVector &col, int layers) const;

        EngineSimApplication *m_app;
};

#endif /* ATG_ENGINE_SIM_SIMULATION_OBJECT_H */
