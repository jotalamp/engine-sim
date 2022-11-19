#include "../include/vehicle.h"

#include <cmath>

Vehicle::Vehicle() {
    m_rotatingMass = nullptr;
    m_mass = 0;
    m_dragCoefficient = 0;
    m_crossSectionArea = 0;
    m_diffRatio = 0;
    m_tireRadius = 0;
    m_travelledDistance = 0;
    m_rollingResistance = 0;
    m_brakes = 0;
    steeringAngle = 0;
}

Vehicle::~Vehicle() {
    m_transform = nullptr;
    m_transform2 = nullptr;
    m_transform_engine = nullptr;
    m_transform_camera = nullptr;
}

void Vehicle::initialize(const Parameters &params) {
    m_mass = params.mass;
    m_dragCoefficient = params.dragCoefficient;
    m_crossSectionArea = params.crossSectionArea;
    m_diffRatio = params.diffRatio;
    m_tireRadius = params.tireRadius;
    m_rollingResistance = params.rollingResistance;
}

void Vehicle::update(double dt) {
    m_travelledDistance += getSpeed() * dt;

    //if(getSpeed() > 0.01f) 
    float f = clamp(1.0f - 4.0f * getSpeed(), 0.0001f, 1.0f);


    //m_rotation = clamp((float)(0.00007f * steeringAngle * getSpeed()), -0.0002f, 0.0002f);
    //m_rotation = -steeringAngle * getSpeed();
    //clamp((float)(0.00007f * steeringAngle * getSpeed()), -0.0002f, 0.0002f);
}

void Vehicle::addToSystem(atg_scs::RigidBodySystem *system, atg_scs::RigidBody *rotatingMass) {
    m_rotatingMass = rotatingMass;
}

double Vehicle::getSpeed() const {
    const double E_r = 0.5 * m_rotatingMass->I * m_rotatingMass->v_theta * m_rotatingMass->v_theta;
    const double vehicleSpeed = std::sqrt(2 * E_r / m_mass);

    return vehicleSpeed;

    // E_r = 0.5 * I * v_theta^2
    // E_k = 0.5 * m * v^2
}

double Vehicle::linearForceToVirtualTorque(double force) const {
    const double rotationToKineticRatio =
        std::sqrt(m_rotatingMass->I / m_mass);
    return rotationToKineticRatio * force;
}

float Vehicle::mapToRange(float input, float inputStart, float inputEnd, float outputStart, float outputEnd) {
    return outputStart + ((outputEnd - outputStart) / (inputEnd - inputStart)) * (input - inputStart);
}
