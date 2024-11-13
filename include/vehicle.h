#ifndef ATG_ENGINE_SIM_VEHICLE_H
#define ATG_ENGINE_SIM_VEHICLE_H

#include "scs.h"
#include "part.h"

#include "simulation_object.h"
#include "../include/utilities.h"

class Vehicle : public Part {
    public:
        struct Parameters {
            double mass;
            double dragCoefficient;
            double crossSectionArea;
            double diffRatio;
            double tireRadius;
            double rollingResistance;
        };

    public:
        Vehicle();
        ~Vehicle();

        void initialize(const Parameters &params);
        void update(double dt);
        void addToSystem(atg_scs::RigidBodySystem *system, atg_scs::RigidBody *rotatingMass);
        inline double getMass() const { return m_mass; }
        inline double getRollingResistance() const { return m_rollingResistance; }
        inline double getDragCoefficient() const { return m_dragCoefficient; }
        inline double getCrossSectionArea() const { return m_crossSectionArea; }
        inline double getDiffRatio() const { return m_diffRatio; }
        inline double getTireRadius() const { return m_tireRadius; }
        double getSpeed() const;
        inline double getTravelledDistance() const { return m_travelledDistance; }
        inline void resetTravelledDistance() { m_travelledDistance = 0; }
        double linearForceToVirtualTorque(double force) const;
        float mapToRange(float input, float inputStart, float inputEnd, float outputStart, float outputEnd);
        
        void setSteeringAngle(float steeringAngle) { m_steeringAngle = steeringAngle; }
        float getSteeringAngle() { return m_steeringAngle; }
        

    protected:
        double m_mass;
        double m_dragCoefficient;
        double m_crossSectionArea;
        double m_diffRatio;
        double m_tireRadius;
        double m_travelledDistance;
        double m_rollingResistance;
        

    public:
        atg_scs::RigidBody *m_rotatingMass;
        //double m_position_x;
        //double m_position_y;
        //ysVector m_translation;
        ysTransform *m_transform;
        ysTransform *m_transform2;
        ysTransform *m_transform_engine;
        ysTransform *m_transform_camera;
        double m_rotation;
        float m_brakes;
        float m_steeringAngle;
};

#endif /* ATG_ENGINE_SIM_VEHICLE_H */
