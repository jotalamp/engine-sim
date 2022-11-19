#ifndef ATG_ENGINE_SIM_CRANKSHAFT_OBJECT_H
#define ATG_ENGINE_SIM_CRANKSHAFT_OBJECT_H

#include "simulation_object.h"

#include "crankshaft.h"
#include "vehicle.h"
#include "geometry_generator.h"

class CrankshaftObject : public SimulationObject {
    public:
        CrankshaftObject();
        virtual ~CrankshaftObject();

        virtual void generateGeometry();
        virtual void render(const ViewParameters *view);
        //virtual void render();
        virtual void process(float dt);
        virtual void destroy();

        Crankshaft *m_crankshaft;
        Vehicle *m_vehicle;
        float rotation;
};

#endif /* ATG_ENGINE_SIM_CRANKSHAFT_OBJECT_H */
