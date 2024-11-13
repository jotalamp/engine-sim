#include "../include/custom_gauge_cluster.h"

#include "../include/units.h"
#include "../include/gauge.h"
#include "../include/constants.h"
#include "../include/engine_sim_application.h"

#include <climits>
#include <cmath>
#include <sstream>
#include <iomanip>

CustomGaugeCluster::CustomGaugeCluster() {
    m_engine = nullptr;
    m_simulator = nullptr;

    //m_afrCluster = nullptr;
    m_tachometer = nullptr;
    m_speedometer = nullptr;
    //m_manifoldVacuumGauge = nullptr;
    //m_volumetricEffGauge = nullptr;
    //m_intakeCfmGauge = nullptr;
    //m_combusionChamberStatus = nullptr;
    //m_throttleDisplay = nullptr;
    m_fuelCluster = nullptr;
    m_isAbsolute = false;
}

CustomGaugeCluster::~CustomGaugeCluster() {
    /* void */
}

void CustomGaugeCluster::initialize(EngineSimApplication *app) {
    UiElement::initialize(app);

    m_tachometer                = addElement<CustomLabeledGauge>();
    m_speedometer               = addElement<CustomLabeledGauge>();
    //m_manifoldVacuumGauge       = addElement<LabeledGauge>();
    //m_intakeCfmGauge            = addElement<LabeledGauge>();
    //m_volumetricEffGauge        = addElement<LabeledGauge>();
    //m_combusionChamberStatus    = addElement<FiringOrderDisplay>();
    //m_throttleDisplay           = addElement<ThrottleDisplay>();
    //m_afrCluster                = addElement<AfrCluster>();
    m_fuelCluster               = addElement<FuelCluster>();

    m_speedUnits = app->getAppSettings()->speedUnits;
    m_pressureUnits = app->getAppSettings()->pressureUnits;
    //m_combusionChamberStatus->m_engine = m_engine;

    constexpr float shortenAngle = (float)units::angle(1.0, units::deg);

    m_tachometer->m_title = "ENGINE SPEED";
    m_tachometer->m_unit = "rpm";
    m_tachometer->m_precision = 0;
    m_tachometer->setLocalPosition({ 0, 0 });
    m_tachometer->m_gauge->m_min = 0;
    m_tachometer->m_gauge->m_max = 7000;
    m_tachometer->m_gauge->m_minorStep = 100;
    m_tachometer->m_gauge->m_majorStep = 1000;
    m_tachometer->m_gauge->m_maxMinorTick = INT_MAX;
    m_tachometer->m_gauge->m_thetaMin = (float)constants::pi * 1.2f;
    m_tachometer->m_gauge->m_thetaMax = -(float)constants::pi * 0.2f;
    m_tachometer->m_gauge->m_needleWidth = 4.0f;
    m_tachometer->m_gauge->m_gamma = 1.0f;
    m_tachometer->m_gauge->m_needleKs = 1000.0f;
    m_tachometer->m_gauge->m_needleKd = 20.0f;
    m_tachometer->m_gauge->setBandCount(3);
    m_tachometer->m_gauge->setBand(
        { m_app->getForegroundColor(), 400, 1000, 3.0f, 6.0f }, 0);
    m_tachometer->m_gauge->setBand(
        { m_app->getOrange(), 5000, 5500, 3.0f, 6.0f, -shortenAngle, shortenAngle }, 1);
    m_tachometer->m_gauge->setBand(
        { m_app->getRed(), 5500, 7000, 3.0f, 6.0f, shortenAngle, -shortenAngle }, 2);

    m_speedometer->m_title = "VEHICLE SPEED";
    m_speedometer->m_unit = "MPH";

    m_speedometer->m_precision = 0;
    m_speedometer->setLocalPosition({ 0, 0 });
    m_speedometer->m_gauge->m_min = 0;
    m_speedometer->m_gauge->m_max = 200;
    m_speedometer->m_gauge->m_minorStep = 5;
    m_speedometer->m_gauge->m_majorStep = 10;
    m_speedometer->m_gauge->m_maxMinorTick = 200;
    m_speedometer->m_gauge->m_thetaMin = (float)constants::pi * 1.2f;
    m_speedometer->m_gauge->m_thetaMax = -(float)constants::pi * 0.2f;
    m_speedometer->m_gauge->m_needleWidth = 4.0f;
    m_speedometer->m_gauge->m_gamma = 1.0f;
    m_speedometer->m_gauge->m_needleKs = 1000.0f;
    m_speedometer->m_gauge->m_needleKd = 20.0f;
    m_speedometer->m_gauge->setBandCount(0);
/*
    m_manifoldVacuumGauge->m_title = "MANIFOLD PRESSURE";
    m_manifoldVacuumGauge->m_unit = "inHg";
    m_manifoldVacuumGauge->m_precision = 0;
    m_manifoldVacuumGauge->setLocalPosition({ 0, 0 });
    m_manifoldVacuumGauge->m_gauge->m_min = -30;
    m_manifoldVacuumGauge->m_gauge->m_max = 5;
    m_manifoldVacuumGauge->m_gauge->m_minorStep = 1;
    m_manifoldVacuumGauge->m_gauge->m_majorStep = 5;
    m_manifoldVacuumGauge->m_gauge->m_maxMinorTick = 200;
    m_manifoldVacuumGauge->m_gauge->m_thetaMin = (float)constants::pi * 1.2f;
    m_manifoldVacuumGauge->m_gauge->m_thetaMax = -(float)constants::pi * 0.2f;
    m_manifoldVacuumGauge->m_gauge->m_needleWidth = 4.0f;
    m_manifoldVacuumGauge->m_gauge->m_gamma = 1.0f;
    m_manifoldVacuumGauge->m_gauge->m_needleKs = 1000.0f;
    m_manifoldVacuumGauge->m_gauge->m_needleKd = 50.0f;
    m_manifoldVacuumGauge->m_gauge->setBandCount(5);
    m_manifoldVacuumGauge->m_gauge->setBand(
        { m_app->getRed(), -5, -1, 3.0f, 6.0f, shortenAngle, shortenAngle }, 0);
    m_manifoldVacuumGauge->m_gauge->setBand(
        { m_app->getForegroundColor(), -1.0f, 1.0f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 1);
    m_manifoldVacuumGauge->m_gauge->setBand(
        { m_app->getOrange(), -10, -5, 3.0f, 6.0f, shortenAngle, shortenAngle }, 2);
    m_manifoldVacuumGauge->m_gauge->setBand(
        { m_app->getBlue(), -22, -10, 3.0f, 6.0f, shortenAngle, shortenAngle }, 3);
    m_manifoldVacuumGauge->m_gauge->setBand(
        { m_app->getForegroundColor(), -30, -22, 3.0f, 6.0f, shortenAngle, shortenAngle }, 4);

    m_volumetricEffGauge->m_title = "VOLUMETRIC EFF.";
    m_volumetricEffGauge->m_unit = "%";
    m_volumetricEffGauge->m_spaceBeforeUnit = false;
    m_volumetricEffGauge->m_precision = 1;
    m_volumetricEffGauge->setLocalPosition({ 0, 0 });
    m_volumetricEffGauge->m_gauge->m_min = 0;
    m_volumetricEffGauge->m_gauge->m_max = 120;
    m_volumetricEffGauge->m_gauge->m_minorStep = 5;
    m_volumetricEffGauge->m_gauge->m_majorStep = 10;
    m_volumetricEffGauge->m_gauge->m_maxMinorTick = 200;
    m_volumetricEffGauge->m_gauge->m_thetaMin = (float)constants::pi * 1.2f;
    m_volumetricEffGauge->m_gauge->m_thetaMax = -(float)constants::pi * 0.2f;
    m_volumetricEffGauge->m_gauge->m_needleWidth = 4.0f;
    m_volumetricEffGauge->m_gauge->m_gamma = 1.0f;
    m_volumetricEffGauge->m_gauge->m_needleKs = 1000.0f;
    m_volumetricEffGauge->m_gauge->m_needleKd = 50.0f;
    m_volumetricEffGauge->m_gauge->setBandCount(3);
    m_volumetricEffGauge->m_gauge->setBand(
        { m_app->getBlue(), 30, 80, 3.0f, 6.0f, 0.0f, shortenAngle }, 0);
    m_volumetricEffGauge->m_gauge->setBand(
        { m_app->getGreen(), 80, 100, 3.0f, 6.0f, shortenAngle, shortenAngle }, 1);
    m_volumetricEffGauge->m_gauge->setBand(
        { m_app->getRed(), 100, 120, 3.0f, 6.0f, shortenAngle, -shortenAngle }, 2);

    m_intakeCfmGauge->m_title = "AIR SCFM";
    m_intakeCfmGauge->m_unit = "";
    m_intakeCfmGauge->m_precision = 1;
    m_intakeCfmGauge->setLocalPosition({ 0, 0 });
    m_intakeCfmGauge->m_gauge->m_min = 0;
    m_intakeCfmGauge->m_gauge->m_max = 1200;
    m_intakeCfmGauge->m_gauge->m_minorStep = 20;
    m_intakeCfmGauge->m_gauge->m_majorStep = 100;
    m_intakeCfmGauge->m_gauge->m_maxMinorTick = 1200;
    m_intakeCfmGauge->m_gauge->m_thetaMin = (float)constants::pi * 1.2f;
    m_intakeCfmGauge->m_gauge->m_thetaMax = -(float)constants::pi * 0.2f;
    m_intakeCfmGauge->m_gauge->m_needleWidth = 4.0f;
    m_intakeCfmGauge->m_gauge->m_gamma = 1.0f;
    m_intakeCfmGauge->m_gauge->m_needleKs = 1000.0f;
    m_intakeCfmGauge->m_gauge->m_needleKd = 50.0f;
    m_intakeCfmGauge->m_gauge->setBandCount(0);*/
    //Set display units
    setUnits();
}

void CustomGaugeCluster::destroy() {
    UiElement::destroy();
}

void CustomGaugeCluster::update(float dt) {
    //m_combusionChamberStatus->m_engine = m_engine;
    //m_throttleDisplay->m_engine = m_engine;
    //m_afrCluster->m_engine = m_engine;
    m_fuelCluster->m_engine = m_engine;
    m_fuelCluster->m_simulator = m_simulator;

    UiElement::update(dt);
}

void CustomGaugeCluster::render() {
    Grid grid = { 16, 9 };

    //drawFrame(m_bounds, 1.0, m_app->getForegroundColor(), m_app->getBackgroundColor());

    //const Bounds tachSpeedCluster = grid.get(m_bounds, 0, 0, 2, 2);//m_bounds.verticalSplit(0.5f, 1.0f);
    //renderTachSpeedCluster(tachSpeedCluster);
    //const Bounds left = tachSpeedCluster.horizontalSplit(0.0f, 1.0f);
    //const Bounds right = bounds.horizontalSplit(0.5f, 1.0f);

    //const Bounds custom = grid.get(m_bounds, 0, 0, 16, 9);

    const Bounds tach = grid.get(m_bounds, 14, 7, 2, 2);
    m_tachometer->m_bounds = tach;
    m_tachometer->m_gauge->m_value = (float)std::abs(getRpm());

    constexpr float shortenAngle = (float)units::angle(1.0, units::deg);
    const float maxRpm =
        (float)std::ceil(units::toRpm(getRedline() * 1.25) / 1000.0) * 1000.0f;
    const float redline =
        (float)std::ceil(units::toRpm(getRedline()) / 500.0) * 500.0f;
    const float redlineWarning =
        (float)std::floor(units::toRpm(getRedline() * 0.9) / 500.0) * 500.0f;
    m_tachometer->m_gauge->m_max = (int)maxRpm;
    m_tachometer->m_gauge->setBandCount(3);
    m_tachometer->m_gauge->setBand(
        { m_app->getForegroundColor(), 400, 1000, 3.0f, 6.0f }, 0);
    m_tachometer->m_gauge->setBand(
        { m_app->getOrange(), redlineWarning, redline, 3.0f, 6.0f, -shortenAngle, shortenAngle }, 1);
    m_tachometer->m_gauge->setBand(
        { m_app->getRed(), redline, maxRpm, 3.0f, 6.0f, shortenAngle, -shortenAngle }, 2);


    //const Bounds speed = left.verticalSplit(0.0f, 0.5f);
    const Bounds speed = grid.get(m_bounds, 12, 7, 2, 2);
    m_speedometer->m_bounds = speed;

    m_speedometer->m_gauge->m_value = (m_speedUnits == "mph") 
        ? (float)units::convert(std::abs(getSpeed()), units::mile / units::hour) 
        : (float)units::convert(std::abs(getSpeed()), units::km / units::hour);

    const Bounds logoBounds = grid.get(m_bounds, 0, 0, 1, 1);
    drawFrame(logoBounds, 1.0f, m_app->getForegroundColor(), m_app->getForegroundColor());

    drawModel(
        //Rod_2JZ_GE_BigEnd
        m_app->getAssetManager()->GetModelAsset("Logo"),
        //m_app->getAssetManager()->GetModelAsset("Rod_2JZ_GE_BigEnd"),
        m_app->getForegroundColor(),
        //ysColor::srgbiToLinear(0xFFFFFF),
        //m_app->getBlue(),
        logoBounds.getPosition(Bounds::center),
        Point( logoBounds.height(), logoBounds.height()) * 0.75f );


    const Bounds titleBounds = grid.get(m_bounds, 1, 0, 15, 1);
    drawFrame(titleBounds, 1.0f, m_app->getForegroundColor(), m_app->getBackgroundColor());

    Grid titleSplit;
    titleSplit.h_cells = 1;
    titleSplit.v_cells = 4;
    drawAlignedText(
        "ENGINE SIMULATOR",
        titleSplit.get(titleBounds, 0, 0).inset(20.0f).move({ 0.0f, -38.0f }),
        42.0f,
        Bounds::bl,
        Bounds::bl);
    drawAlignedText(
        "YOUTUBE/ANGETHEGREAT",
        titleSplit.get(titleBounds, 0, 1).inset(20.0f).move({ 0.0f, 5.0f }),
        24.0f,
        Bounds::tl,
        Bounds::tl);
    drawAlignedText(
        "BUILD: v" + EngineSimApplication::getBuildVersion() + " // " __DATE__,
        titleSplit.get(titleBounds, 0, 2).inset(20.0f).move({ 0.0f, 10.0f }),
        16.0f,
        Bounds::tl,
        Bounds::tl);

    const Bounds fuelAirCluster = grid.get(m_bounds, 0, 7, 2, 2);// = m_bounds.verticalSplit(0.3f, 0.7f);
    //drawFrame(fuelAirCluster, 1.0, m_app->getForegroundColor(), m_app->getBackgroundColor());
    renderFuelAirCluster(fuelAirCluster);

    const Bounds gearBounds = grid.get(m_bounds, 11, 8);
    const Bounds insetBounds = gearBounds.inset(10.0f);
    const Bounds title = insetBounds.verticalSplit(0.0f, 0.1f);
    const Bounds body = insetBounds.verticalSplit(0.1f, 0.9f);

    //drawFrame(gearBounds, 1.0f, m_app->getForegroundColor(), m_app->getBackgroundColor());
    //drawCenteredText("Gear", title.inset(10.0f), 24.0f);

    const int gear = (m_simulator->getTransmission() != nullptr)
        ? m_simulator->getTransmission()->getGear()
        : -1;
    std::stringstream ss1;
    
    if (gear > -1) 
        ss1 << (gear + 1);
    else if(gear == -1)
        ss1 << "N";
    else
        ss1 << "R";

    drawCenteredText(ss1.str(), body, 64.0f, Bounds::center);

    const Bounds engineInfoBounds = grid.get(m_bounds, 6, 0, 10, 1);
    drawFrame(engineInfoBounds, 1.0f, m_app->getForegroundColor(), m_app->getBackgroundColor());

    drawAlignedText(
        (m_engine != nullptr) ? m_engine->getName() : "<NO ENGINE> :(",
        engineInfoBounds.inset(10.0f),
        24.0f,
        Bounds::lm,
        Bounds::lm);

    std::stringstream ss;
    if (m_engine != nullptr) {
        ss << std::fixed;

        if (m_engine->getDisplacement() < units::volume(1.0, units::L)) {
            ss << std::setprecision(0) << units::convert(m_engine->getDisplacement(), units::cc) << " cc -- ";
        }
        else {
            ss << std::setprecision(1) << units::convert(m_engine->getDisplacement(), units::L) << " L -- ";
        }

        ss << std::setprecision(0) << units::convert(m_engine->getDisplacement(), units::cubic_inches) << " CI";
    }
    else {
        ss << "N/A";
    }

    drawAlignedText(
        ss.str(),
        engineInfoBounds.inset(10.0f),
        24.0f,
        Bounds::rm,
        Bounds::rm);

    std::stringstream ss2;

    ss2 << std::to_string((int)m_simulator->getVehicle()->getMass());
    ss2 << " KG, ";

    ss2 << std::to_string(m_simulator->getVehicle()->getDragCoefficient());
    ss2 << " DC, ";

    ss2 << std::to_string((int)m_simulator->getVehicle()->getRollingResistance());
    ss2 << " RR";

    drawAlignedText(
        ss2.str(),
        engineInfoBounds.inset(10.0f),
        24.0f,
        Bounds::tl,
        Bounds::tl);



    UiElement::render();
}

void CustomGaugeCluster::setEngine(Engine *engine) {
    m_engine = engine;
}

void CustomGaugeCluster::renderTachSpeedCluster(const Bounds &bounds) {
    const Bounds left = bounds.horizontalSplit(0.0f, 1.0f);
    //const Bounds right = bounds.horizontalSplit(0.5f, 1.0f);

    const Bounds tach = left.verticalSplit(0.5f, 1.0f);
    m_tachometer->m_bounds = tach;
    m_tachometer->m_gauge->m_value = (float)std::abs(getRpm());

    constexpr float shortenAngle = (float)units::angle(1.0, units::deg);
    const float maxRpm =
        (float)std::ceil(units::toRpm(getRedline() * 1.25) / 1000.0) * 1000.0f;
    const float redline =
        (float)std::ceil(units::toRpm(getRedline()) / 500.0) * 500.0f;
    const float redlineWarning =
        (float)std::floor(units::toRpm(getRedline() * 0.9) / 500.0) * 500.0f;
    m_tachometer->m_gauge->m_max = (int)maxRpm;
    m_tachometer->m_gauge->setBandCount(3);
    m_tachometer->m_gauge->setBand(
        { m_app->getForegroundColor(), 400, 1000, 3.0f, 6.0f }, 0);
    m_tachometer->m_gauge->setBand(
        { m_app->getOrange(), redlineWarning, redline, 3.0f, 6.0f, -shortenAngle, shortenAngle }, 1);
    m_tachometer->m_gauge->setBand(
        { m_app->getRed(), redline, maxRpm, 3.0f, 6.0f, shortenAngle, -shortenAngle }, 2);

    const Bounds speed = left.verticalSplit(0.0f, 0.5f);
    m_speedometer->m_bounds = speed;

    m_speedometer->m_gauge->m_value = (m_speedUnits == "mph") 
        ? (float)units::convert(std::abs(getSpeed()), units::mile / units::hour) 
        : (float)units::convert(std::abs(getSpeed()), units::km / units::hour);


    //m_combusionChamberStatus->m_bounds = right;
}

void CustomGaugeCluster::renderFuelAirCluster(const Bounds &bounds) {
    Grid grid1;
    grid1.v_cells = 1;
    grid1.h_cells = 1;
    //const Bounds left = bounds.horizontalSplit(0.0f, 0.5f);
    const Bounds left = bounds;
    //const Bounds right = bounds.horizontalSplit(0.5f, 1.0f);

    const Bounds throttle = left.verticalSplit(0.5f, 1.0f);
    //m_throttleDisplay->m_bounds = throttle;

    const Bounds fuelSection = left;//.verticalSplit(0.0f, 0.5f);
    //const Bounds afr = fuelSection.horizontalSplit(0.0f, 0.5f);
    //m_afrCluster->m_bounds = afr;

    const Bounds fuelConsumption = fuelSection.horizontalSplit(0.5f, 1.0f);

    //m_engineView->setBounds(grid.get(windowBounds, 1, 0, 1, 1));
    //m_fuelCluster->m_bounds = fuelConsumption;
    m_fuelCluster->m_bounds = grid1.get(bounds, 0, 0, 1, 1);

    //constexpr double ambientPressure = units::pressure(1.0, units::atm);
    //constexpr double ambientTemperature = units::celcius(25.0);

    Grid grid = { 1, 3 };
    //const Bounds manifoldVacuum = grid.get(right, 0, 0, 1, 1);
    //m_manifoldVacuumGauge->m_bounds = manifoldVacuum;

    //const double vacuumReading = getManifoldPressureWithUnits(ambientPressure);
    if (m_isAbsolute) {
        //m_manifoldVacuumGauge->m_gauge->m_value = static_cast<float>(vacuumReading);
    }
    else {/*
        m_manifoldVacuumGauge->m_gauge->m_value = (vacuumReading > -0.5)
            ? 0.0f
            : static_cast<float>(vacuumReading);*/
    }

    const double rpm = std::fmax(getRpm(), 0.0);
    /*
    const double theoreticalAirPerRevolution = (m_engine == nullptr)
        ? 0.0
        : 0.5 * (ambientPressure * m_engine->getDisplacement())
            / (constants::R * ambientTemperature);*/
    //const double theoreticalAirPerSecond = theoreticalAirPerRevolution * rpm / 60.0;
    const double actualAirPerSecond = (m_engine == nullptr)
        ? 0.0
        : m_engine->getIntakeFlowRate();
    /*const double volumetricEfficiency = (std::abs(theoreticalAirPerSecond) < 1E-3)
        ? 0.0
        : (actualAirPerSecond / theoreticalAirPerSecond);*/
/*
    const Bounds cfmBounds = grid.get(right, 0, 1, 1, 1);
    m_intakeCfmGauge->m_bounds = cfmBounds;
    m_intakeCfmGauge->m_gauge->m_value =
        (float)units::convert(actualAirPerSecond, units::scfm);

    const Bounds volumetricEfficiencyBounds = grid.get(right, 0, 2, 1, 1);
    m_volumetricEffGauge->m_bounds = volumetricEfficiencyBounds;
    m_volumetricEffGauge->m_gauge->m_value = 100.0f * (float)volumetricEfficiency;*/
}

double CustomGaugeCluster::getManifoldPressureWithUnits(double ambientPressure) {
    if (m_pressureUnits == "inHg") {
        return units::convert(std::fmin(getManifoldPressure() - ambientPressure, 0.0), units::inHg);
    }
    else if (m_pressureUnits == "kPa") {
        return units::convert(getManifoldPressure(), units::kPa);
    }
    else if (m_pressureUnits == "mbar") {
        return units::convert(getManifoldPressure(), units::mbar);
    }
    else if (m_pressureUnits == "bar") {
        return units::convert(getManifoldPressure(), units::bar);
    }
    else if (m_pressureUnits == "psi") {
        return units::convert(std::fmin(getManifoldPressure() - ambientPressure, 0.0), units::psi);
    }
    else {
        return units::convert(std::fmin(getManifoldPressure() - ambientPressure, 0.0), units::inHg);
    }
}

double CustomGaugeCluster::getRpm() const {
    return (m_engine != nullptr)
        ? m_engine->getRpm()
        : 0;
}

double CustomGaugeCluster::getRedline() const {
    return (m_engine != nullptr)
        ? m_engine->getRedline()
        : 0;
}

double CustomGaugeCluster::getSpeed() const {
    return (m_simulator->getVehicle() != nullptr)
        ? m_simulator->getVehicle()->getSpeed()
        : 0;
}

double CustomGaugeCluster::getManifoldPressure() const {
    return (m_engine != nullptr)
        ? m_engine->getManifoldPressure()
        : units::pressure(1.0, units::atm);
}

void CustomGaugeCluster::setUnits() {
    constexpr float shortenAngle = (float)units::angle(1.0, units::deg);

    m_speedometer->m_unit = (m_speedUnits == "mph")
        ? "mph"
        : "kph";

    if (m_pressureUnits == "kPa") {
        m_isAbsolute = true;
/*
        m_manifoldVacuumGauge->m_unit = "kPa";
        m_manifoldVacuumGauge->m_gauge->m_min = 0;
        m_manifoldVacuumGauge->m_gauge->m_max = 110;
        m_manifoldVacuumGauge->m_gauge->m_minorStep = 5;
        m_manifoldVacuumGauge->m_gauge->m_majorStep = 10;
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getRed(), 110, 90, 3.0f, 6.0f, shortenAngle, shortenAngle }, 0);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), -1.0f, 1.0f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 1);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getOrange(), 30, 40, 3.0f, 6.0f, shortenAngle, shortenAngle }, 2);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getBlue(), 15, 29, 3.0f, 6.0f, shortenAngle, shortenAngle }, 3);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), 0, 14, 3.0f, 6.0f, shortenAngle, shortenAngle }, 4);*/
    }
    else if (m_pressureUnits == "mbar") {
        m_isAbsolute = true;
/*
        m_manifoldVacuumGauge->m_unit = "mbar";
        m_manifoldVacuumGauge->m_gauge->m_min = 0;
        m_manifoldVacuumGauge->m_gauge->m_max = 1100;
        m_manifoldVacuumGauge->m_gauge->m_minorStep = 50;
        m_manifoldVacuumGauge->m_gauge->m_majorStep = 100;
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getRed(), 1100, 900, 3.0f, 6.0f, shortenAngle, shortenAngle }, 0);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), -1.0f, 1.0f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 1);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getOrange(), 300, 400, 3.0f, 6.0f, shortenAngle, shortenAngle }, 2);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getBlue(), 150, 290, 3.0f, 6.0f, shortenAngle, shortenAngle }, 3);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), 0, 140, 3.0f, 6.0f, shortenAngle, shortenAngle }, 4);*/
    }
    else if (m_pressureUnits == "bar") {
        m_isAbsolute = true;
/*
        m_manifoldVacuumGauge->m_unit = "bar";
        m_manifoldVacuumGauge->m_gauge->m_min = 0;
        m_manifoldVacuumGauge->m_gauge->m_max = 1;
        m_manifoldVacuumGauge->m_gauge->m_minorStep = 1;
        m_manifoldVacuumGauge->m_gauge->m_majorStep = 1;
        m_manifoldVacuumGauge->m_precision = 2;

        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getRed(), 0.8f, 1.1f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 0);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), -1.0f, 1.0f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 1);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getOrange(), 0.3f, 0.5f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 2);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getBlue(), 0.15f, 0.29f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 3);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), 0, 0.14f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 4);*/
    }
    else if (m_pressureUnits == "psi") {/*
        m_isAbsolute = false;

        m_manifoldVacuumGauge->m_unit = "psi";
        m_manifoldVacuumGauge->m_gauge->m_min = -15;
        m_manifoldVacuumGauge->m_gauge->m_max = 3;
        m_manifoldVacuumGauge->m_gauge->m_minorStep = 1;
        m_manifoldVacuumGauge->m_gauge->m_majorStep = 5;
        m_manifoldVacuumGauge->m_precision = 1;
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getRed(), -4, 1, 3.0f, 6.0f, shortenAngle, shortenAngle }, 0);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), -1.0f, 1.0f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 1);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getOrange(), -7, -4, 3.0f, 6.0f, shortenAngle, shortenAngle }, 2);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getBlue(), -12, -7, 3.0f, 6.0f, shortenAngle, shortenAngle }, 3);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), -15, -12, 3.0f, 6.0f, shortenAngle, shortenAngle }, 4);*/
    }
    else {
        m_isAbsolute = false;
/*
        m_manifoldVacuumGauge->m_unit = "inHg";
        m_manifoldVacuumGauge->m_gauge->m_min = -30;
        m_manifoldVacuumGauge->m_gauge->m_max = 5;
        m_manifoldVacuumGauge->m_gauge->m_minorStep = 1;
        m_manifoldVacuumGauge->m_gauge->m_majorStep = 5;
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getRed(), -5, -1, 3.0f, 6.0f, shortenAngle, shortenAngle }, 0);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), -1.0f, 1.0f, 3.0f, 6.0f, shortenAngle, shortenAngle }, 1);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getOrange(), -10, -5, 3.0f, 6.0f, shortenAngle, shortenAngle }, 2);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getBlue(), -22, -10, 3.0f, 6.0f, shortenAngle, shortenAngle }, 3);
        m_manifoldVacuumGauge->m_gauge->setBand(
            { m_app->getForegroundColor(), -30, -22, 3.0f, 6.0f, shortenAngle, shortenAngle }, 4);*/
    }
}
