#include "../include/ground.h"

#include "../include/constants.h"

#include <cmath>
#include <assert.h>

Ground::Ground() {
    m_rodJournalAngles = nullptr;
    m_rodJournalCount = 0;
    m_throw = 0.0;
    m_m = 0.0;
    m_I = 0.0;
    m_flywheelMass = 0.0;
    m_p_x = m_p_y = 0.0;
    m_tdc = 0.0;
    m_frictionTorque = 0.0;
}

Ground::~Ground() {
    assert(m_rodJournalAngles == nullptr);
}

void Ground::initialize(const Parameters &params) {
    m_m = params.Mass;
    m_flywheelMass = params.FlywheelMass;
    m_I = params.MomentOfInertia;
    m_throw = params.CrankThrow;
    m_rodJournalCount = params.RodJournals;
    m_rodJournalAngles = new double[m_rodJournalCount];
    m_p_x = params.Pos_x;
    m_p_y = params.Pos_y;
    m_tdc = params.TDC;
    m_frictionTorque = params.FrictionTorque;
}

void Ground::destroy() {
    if (m_rodJournalAngles != nullptr) delete[] m_rodJournalAngles;

    m_rodJournalAngles = nullptr;
}

void Ground::getRodJournalPositionLocal(int i, double *x, double *y) {
    const double theta = m_rodJournalAngles[i];

    *x = std::cos(theta) * m_throw;
    *y = std::sin(theta) * m_throw;
}

void Ground::getRodJournalPositionGlobal(int i, double *x, double *y) {
    double lx, ly;
    getRodJournalPositionLocal(i, &lx, &ly);

    *x = lx + m_body.p_x;
    *y = ly + m_body.p_y;
}

void Ground::resetAngle() {
    m_body.theta = std::fmod(m_body.theta, 4 * constants::pi);
}

void Ground::setRodJournalAngle(int i, double angle) {
    assert(i < m_rodJournalCount && i >= 0);

    m_rodJournalAngles[i] = angle;
}

double Ground::getAngle() const {
    return m_body.theta - m_tdc;
}

double Ground::getCycleAngle(double offset) {
    const double wrapped = std::fmod(-getAngle() + offset, 4 * constants::pi);
    return (wrapped < 0)
        ? wrapped + 4 * constants::pi
        : wrapped;
}
