#include "../include/engine_sim_application.h"
#include "../include/piston_object.h"
#include "../include/connecting_rod_object.h"
#include "../include/constants.h"
#include "../include/crankshaft_object.h"
#include "../include/cylinder_bank_object.h"
#include "../include/cylinder_head_object.h"
#include "../include/ui_button.h"
#include "../include/combustion_chamber_object.h"
#include "../include/csv_io.h"
#include "../include/exhaust_system.h"
#include "../include/feedback_comb_filter.h"
#include "../include/utilities.h"
#include "../scripting/include/compiler.h"
#include <chrono>
#include <stdlib.h>
#include <sstream>

#if ATG_ENGINE_SIM_DISCORD_ENABLED
#include "../discord/Discord.h"
#endif

std::string EngineSimApplication::s_buildVersion = " 3DW.0.24.11.13";

/*
* TODO:
*
* Brakes
* Car should not move when clutch pressed
* 
*/

EngineSimApplication::EngineSimApplication() {
	m_assetPath = "";

	m_geometryVertexBuffer = nullptr;
	m_geometryIndexBuffer = nullptr;

	m_controller = GAME_CONTROLLER;

	m_paused = false;
	m_recording = false;
	m_screenResolutionIndex = 0;
	for (int i = 0; i < ScreenResolutionHistoryLength; ++i) {
		m_screenResolution[i][0] = m_screenResolution[i][1] = 0;
	}

	m_background = ysColor::srgbiToLinear(0x0E1012);
	m_foreground = ysColor::srgbiToLinear(0xFFFFFF);
	m_shadow = ysColor::srgbiToLinear(0x0E1012);
	m_highlight1 = ysColor::srgbiToLinear(0xEF4545);
	m_highlight2 = ysColor::srgbiToLinear(0xFFFFFF);
	m_pink = ysColor::srgbiToLinear(0xF394BE);
	m_red = ysColor::srgbiToLinear(0xEE4445);
	m_orange = ysColor::srgbiToLinear(0xF4802A);
	m_yellow = ysColor::srgbiToLinear(0xFDBD2E);
	m_blue = ysColor::srgbiToLinear(0x77CEE0);
	m_green = ysColor::srgbiToLinear(0xBDD869);

	m_displayHeight = (float)units::distance(2.0, units::foot);
	m_outputAudioBuffer = nullptr;
	m_audioSource = nullptr;

	m_torque = 0;
	m_dynoSpeed = 0;

	m_simulator = nullptr;
	m_engineView = nullptr;
	m_rightGaugeCluster = nullptr;
	m_temperatureGauge = nullptr;
	m_oscCluster = nullptr;
	m_performanceCluster = nullptr;
	m_loadSimulationCluster = nullptr;
	m_mixerCluster = nullptr;
	m_infoCluster = nullptr;
	m_iceEngine = nullptr;
	m_mainRenderTarget = nullptr;

	m_vehicle = nullptr;
	m_transmission = nullptr;

	m_oscillatorSampleOffset = 0;
	m_gameWindowHeight = 256;
	m_screenWidth = 256;
	m_screenHeight = 256;
	m_screen = SCREEN_MODE_DRIVING;
	m_viewParameters.Layer0 = 0;
	m_viewParameters.Layer1 = 0;

	m_displayAngle = 0.0f;

	m_dragStartMousePosition = ysVector2(0, 0);

	m_previousPosition = ysMath::LoadVector();
	m_previousPosition2 = ysMath::LoadVector();

	m_scaleModel = 1.0f;
}

EngineSimApplication::~EngineSimApplication() {
	/* void */
}

void EngineSimApplication::initialize(void* instance, ysContextObject::DeviceAPI api) {

	m_previousPosition = ysMath::LoadVector();

	m_iniReader = inih::INIReader{ "../settings.ini" };

	dbasic::Path modulePath = dbasic::GetModulePath();
	dbasic::Path confPath = modulePath.Append("delta.conf");

	std::string enginePath = "../dependencies/submodules/delta-studio/engines/basic";
	m_assetPath = "../assets";
	if (confPath.Exists()) {
		std::fstream confFile(confPath.ToString(), std::ios::in);

		std::getline(confFile, enginePath);
		std::getline(confFile, m_assetPath);
		enginePath = modulePath.Append(enginePath).ToString();
		m_assetPath = modulePath.Append(m_assetPath).ToString();

		confFile.close();
	}

	m_engine.GetConsole()->SetDefaultFontDirectory(enginePath + "/fonts/");

	const std::string shaderPath = enginePath + "/shaders/";
	const std::string winTitle = "Engine Sim | v" + s_buildVersion;
	dbasic::DeltaEngine::GameEngineSettings settings;
	settings.API = api;
	//settings.DepthBuffer = m_iniReader.Get<bool>("Rendering", "DepthBuffer");
	settings.DepthBuffer = true;
	settings.Instance = instance;
	settings.ShaderDirectory = shaderPath.c_str();
	settings.WindowTitle = winTitle.c_str();
	settings.WindowPositionX = 0;
	settings.WindowPositionY = 0;
	settings.WindowStyle = ysWindow::WindowStyle::Windowed;
	settings.WindowWidth = 1920;
	settings.WindowHeight = 1080;

	m_engine.CreateGameWindow(settings);

	m_engine.GetDevice()->CreateSubRenderTarget(
		&m_mainRenderTarget,
		m_engine.GetScreenRenderTarget(),
		0,
		0,
		0,
		0);

	m_engine.InitializeShaderSet(&m_shaderSet);
	m_shaders.Initialize(
		&m_shaderSet,
		m_mainRenderTarget,
		m_engine.GetScreenRenderTarget(),
		m_engine.GetDefaultShaderProgram(),
		m_engine.GetDefaultInputLayout());
	m_engine.InitializeConsoleShaders(&m_shaderSet);
	m_engine.SetShaderSet(&m_shaderSet);

	m_shaders.SetClearColor(ysColor::srgbiToLinear(0x34, 0x98, 0xdb));

	m_assetManager.SetEngine(&m_engine);

	m_engine.GetDevice()->CreateIndexBuffer(
		&m_geometryIndexBuffer, sizeof(unsigned short) * 200000, nullptr);
	m_engine.GetDevice()->CreateVertexBuffer(
		&m_geometryVertexBuffer, sizeof(dbasic::Vertex) * 100000, nullptr);

	m_geometryGenerator.initialize(100000, 200000);

	initialize();
}

void EngineSimApplication::loadMaterial(std::string filename, std::string name)
{
	printf("\nLoading Material: %s", name.c_str());
	
	dbasic::TextureAsset* textureAsset;
	dbasic::Material* material;

	m_assetManager.LoadTexture(("../assets/textures/" + filename).c_str(), name.c_str());
	textureAsset = m_assetManager.GetTexture(name.c_str());
	material = m_assetManager.NewMaterial();
	material->SetDiffuseMap(textureAsset->GetTexture());
	material->SetName(name.c_str());
}

void EngineSimApplication::loadModel(std::string filename, float scale)
{
	std::string path = "/models/";
	m_assetManager.CompileInterchangeFile((m_assetPath + path + filename).c_str(), scale, true);
	m_assetManager.LoadSceneFile((m_assetPath + path + filename).c_str(), true);
}

std::string EngineSimApplication::intToString(int number)
{
	std::string string = std::to_string(number);

	if (number < 10)
		return '0' + string;
	else
		return string;
}

void EngineSimApplication::initialize() {

	//std::ifstream f("../assets/vehicles/pessima/pessima_body_01.json");//\assets\vehicles\pessima
	//json data = json::parse(f);

	//jbeam_json = data;

	//j_Controller = Joystick(1);

	m_camera.target = ysVector3(0.0f, 0.0f, 0.0f);

	m_show_engine = m_iniReader.Get<bool>("Settings", "ShowEngine");
	m_show_engine2 = m_iniReader.Get<bool>("Settings", "ShowEngine2");
	m_show_track = m_iniReader.Get<bool>("Settings", "ShowTrack");

	m_show_engine_only = m_iniReader.Get<bool>("Settings", "ShowEngineOnly");
	

	float m_engineModelScale = m_iniReader.Get<float>("Engine", "ModelScale");

	m_camera.target = ysVector3(0 * 2.5f * m_cylinderDifferenceZ, 0.0f, 0.1f * m_cylinderDifferenceZ);

	m_selected_track = m_iniReader.Get<int>("Settings", "SelectedTrack");
	printf("\nSelected Track: %i", m_selected_track);

	if (m_show_engine_only)
	{
		m_camera.zoom = 1.0f;
	}
	else
	{
		m_camera.zoom = m_iniReader.Get<float>("Camera", "Zoom");
		printf("\nZoom: %f", m_camera.zoom);
	}

	m_camera.fovY = m_iniReader.Get<float>("Camera", "FovY");
	printf("\nFovY: %f", m_camera.fovY);
	m_camera.fovY *= (ysMath::Constants::PI / 180.0f);

	m_camera.rotation = ysVector2(0 * 0.5f * ysMath::Constants::PI, 0.5f * ysMath::Constants::PI);

	m_shaders.SetClearColor(ysColor::srgbiToLinear(0x34, 0x98, 0xdb));

	// Define the gravity vector.
	// Top down
	b2Vec2 gravity(0.0f, 0.0f);

	// Construct a world object, which will hold and simulate the rigid bodies.
	m_world = new b2World(gravity);

	float trackScale = getIniReader().Get<float>("Track", "Scale");

	std::string selectedVehicleModelFileName = getIniReader().Get<std::string>("SelectedVehicle", "Name");
	//std::string selectedVehicleModelFileName = "Vehicle";

	std::string vehicleModelFileName = getIniReader().Get<std::string>(selectedVehicleModelFileName, "ModelFileName");
	
	float vehicleScale = getIniReader().Get<float>(selectedVehicleModelFileName, "Scale");

	m_assetManager.CompileInterchangeFile((m_assetPath + "/assets").c_str(), 1.0f, true);
	m_assetManager.LoadSceneFile((m_assetPath + "/assets").c_str(), true);

	
	
	//m_infoCluster->setLogMessage(s);
	

	//vehicleModelFileName = s;
	//vehicleModelFileName = "pessima";

	printf("\n%s", vehicleModelFileName.c_str());
	loadModel(vehicleModelFileName, vehicleScale);

	loadModel("ConnectingRodLong", m_engineModelScale);
	loadModel("Piston_2JZ_GE", m_engineModelScale);
	loadModel("Rod_2JZ_GE", m_engineModelScale);
	loadModel("Rod_2JZ_GE_Split", m_engineModelScale);
	loadModel("Crankshaft_2JZ_GE_Part_1", m_engineModelScale);
	loadModel("Crankshaft_2JZ_GE_Part_2", m_engineModelScale);
	loadModel("Crankshaft_2JZ_GE_V_Part_1", m_engineModelScale);
	loadModel("Head_2JZ_GE_Split", m_engineModelScale);
	loadModel("Head_2JZ_GE_Split_V", m_engineModelScale);
	loadModel("Camshaft_2JZ_GE", 0.05f * m_engineModelScale);
	loadModel("Engine_2JZ_GE_Simplified", m_engineModelScale);

	loadModel("cube");
	loadModel("motorway");
	loadModel("lights");
	loadModel("trackGarda", trackScale);
	loadModel("highwaystuff");

	loadModel("infinite_grid", 100.0);

	loadMaterial("palette.png", "Palette");
	loadMaterial("motorway/highway_grass_02.png", "HighwayGrass");
	//loadModel("track_01");

	//loadModel("track_kenney");

	loadModel("rallycar_01");
	loadMaterial("rallycars/subaru_impreza_22B_alta.png", "Impreza");

	//loadModel("rallycar_03");
	//loadMaterial("rallycars/ford_escort_cosworth_alta.png", "RallyCar_03");

	//loadMaterial("rallycars/car_01.png", "Car_01");


	//loadModel("seben");

	//loadModel("seben_white");
	//loadModel("rallycar_03_");

	std::vector<std::string> textureNames = getIniReader().GetVector<std::string>(selectedVehicleModelFileName, "Textures");
	std::string textureFolder = "trueno";

	dbasic::TextureAsset* textureAsset;
	dbasic::Material* material;

	loadMaterial("highwaystuff/concreteblock.png",	"ConcreteBlock"		);
	loadMaterial("highwaystuff/cone.png",			"Cone"				);
	loadMaterial("lightpoles/lightpole.png",		"Lightpole"			);
	loadMaterial("motorway/grass.png",				"Grass"				);
	loadMaterial("motorway/highway_03.png",			"Highway"			);
	loadMaterial("highwaystuff/speedlimit_75.png",	"Speedlimit75"		);
	loadMaterial("highwaystuff/guardrail_1.png",	"Guardrail"			);

	/// GARDA
	
	loadMaterial("garda/fence.png",					"MaterialFence"		);
	loadMaterial("garda/standard_material.png",		"MaterialStandard"	);
	loadMaterial("garda/material.jpg",				"MaterialMat"		);
	loadMaterial("garda/italian.jpg",				"MaterialItalian"	);
	loadMaterial("garda/parking.jpg",				"MaterialParking"	);

    int i = 0;

    dbasic::Material* material_01;
	material_01 = m_assetManager.NewMaterial();
	material_01->SetDiffuseColor(ysColor::srgbiToLinear(0xFFFFFF));
	material_01->SetMetallic(2.0f);
	material_01->SetName("MaterialWhite");
    printf("\n%i",i++);

    material = m_assetManager.NewMaterial();
	material->SetDiffuseColor(ysColor::srgbiToLinear(0x96A7AA));
	material->SetDiffuseMix(0.27f);
	material->SetName("MaterialGlass");
    printf("\n%i", i++);

    material = m_assetManager.NewMaterial();
	material->SetDiffuseColor(ysColor::srgbiToLinear(0x000000));
	material->SetName("MaterialBlack");
    printf("\n%i", i++);

    material = m_assetManager.NewMaterial();
	material->SetDiffuseColor(ysColor::srgbiToLinear(0x888888));
	material->SetName("MaterialGrey");
    printf("\n%i", i++);

    material = m_assetManager.NewMaterial();
	material->SetDiffuseColor(ysColor::srgbiToLinear(0x0000FF));
	material->SetName("MaterialBlue");
    printf("\n%i", i++);

    material = m_assetManager.NewMaterial();
	material->SetDiffuseColor(ysColor::srgbiToLinear(0xFF0000));
	material->SetName("MaterialRed");
    printf("\nMaterialRed: %i", i++);

    int j = i;
    for (std::vector<std::string>::size_type i = 0; i != textureNames.size(); i++)
	{
		std::string textureName = textureNames[i];
		m_assetManager.LoadTexture(("../assets/textures/" + textureFolder + "/" + textureName).c_str(), ("Texture_" + textureName).c_str());
		textureAsset = m_assetManager.GetTexture(("Texture_" + textureName).c_str());
		material = m_assetManager.NewMaterial();
		material->SetDiffuseMap(textureAsset->GetTexture());
		std::string materialName = "Material__" + intToString((int)i);
		material->SetName(materialName.c_str());
        printf("\n%s", textureName.c_str());
        printf("\n%i", j++);
    }

    int k = 0;
    printf("\nTexture materials loaded\n");

    printf("\nAk%i", k++);
    m_textRenderer.SetEngine(&m_engine);
    printf("\nBk%i", k++);
    m_textRenderer.SetRenderer(m_engine.GetUiRenderer());
    printf("\nCk%i", k++);
    m_textRenderer.SetFont(m_engine.GetConsole()->GetFont());
    printf("\nDk%i", k++);
    //printf("\n%i", i++);

    //loadScript();
    printf("\nE%i", k++);

    m_audioBuffer.initialize(44100, 44100);
    printf("\nFm_audioBuffer.initialize(44100, 44100);");
    m_audioBuffer.m_writePointer = (int)(44100 * 0.1);
    printf("\nGk%i", k++);

    ysAudioParameters params;
	params.m_bitsPerSample = 16;
	params.m_channelCount = 1;
	params.m_sampleRate = 44100;
    printf("\nHk%i", k++);
    m_outputAudioBuffer =
		m_engine.GetAudioDevice()->CreateBuffer(&params, 44100);
    printf("\nIk%i", k++);

    m_audioSource = m_engine.GetAudioDevice()->CreateSource(m_outputAudioBuffer);
    printf("\nJk%i", k++);
    m_audioSource->SetMode((m_simulator->getEngine() != nullptr)
		? ysAudioSource::Mode::Loop
		: ysAudioSource::Mode::Stop);
	m_audioSource->SetPan(0.0f);
	m_audioSource->SetVolume(1.0f);

#ifdef ATG_ENGINE_SIM_DISCORD_ENABLED
	// Create a global instance of discord-rpc
	CDiscord::CreateInstance();

	// Enable it, this needs to be set via a config file of some sort. 
	GetDiscordManager()->SetUseDiscord(true);
	DiscordRichPresence passMe = { 0 };

	std::string engineName = (m_iceEngine != nullptr)
		? m_iceEngine->getName()
		: "Broken Engine";

	GetDiscordManager()->SetStatus(passMe, engineName, s_buildVersion);
#endif /* ATG_ENGINE_SIM_DISCORD_ENABLED */

	m_engine.GetGameWindow()->SetWindowStyle(ysWindow::WindowStyle::Fullscreen);
}

void EngineSimApplication::process(float frame_dt) {

	int32 velocityIterations = 2;
	int32 positionIterations = 5;

	frame_dt = static_cast<float>(clamp(frame_dt, 1 / 200.0f, 1 / 30.0f));

	float fuelLeft = (float)getSimulator()->getEngine()->getTotalVolumeFuelLeft();

	if (false)
		if (fuelLeft < 0.002f) {
			m_infoCluster->setLogMessage("FUEL TANK IS EMPTY!!");
			m_simulator->getEngine()->getIgnitionModule()->m_enabled = false;
		}

	double speed = 1.0 / 1.0;
	if (m_engine.IsKeyDown(ysKey::Code::N1)) {
		speed = 1 / 10.0;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::N2)) {
		speed = 1 / 100.0;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::N3)) {
		speed = 1 / 200.0;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::N4)) {
		speed = 1 / 500.0;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::N5)) {
		speed = 1 / 1000.0;
	}

	if (m_engine.IsKeyDown(ysKey::Code::F1)) {
		m_displayAngle += frame_dt * 1.0f;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::F2)) {
		m_displayAngle -= frame_dt * 1.0f;
	}
	else if (m_engine.ProcessKeyDown(ysKey::Code::F3)) {
		m_displayAngle = 0.0f;
	}

	m_simulator->setSimulationSpeed(speed);

	const double avgFramerate = clamp(m_engine.GetAverageFramerate(), 30.0f, 1000.0f);
	m_simulator->startFrame(1 / avgFramerate);

	m_world->Step((float)(speed * frame_dt), velocityIterations, positionIterations);

	m_vehicle_object->process((float)(speed * frame_dt));

	auto proc_t0 = std::chrono::steady_clock::now();
	const int iterationCount = m_simulator->getFrameIterationCount();
	while (m_simulator->simulateStep()) {
		m_oscCluster->sample();
	}

	auto proc_t1 = std::chrono::steady_clock::now();

	m_simulator->endFrame();

	auto duration = proc_t1 - proc_t0;
	if (iterationCount > 0) {
		m_performanceCluster->addTimePerTimestepSample(
			(duration.count() / 1E9) / iterationCount);
	}

	const SampleOffset safeWritePosition = m_audioSource->GetCurrentWritePosition();
	const SampleOffset writePosition = m_audioBuffer.m_writePointer;

	SampleOffset targetWritePosition =
		m_audioBuffer.getBufferIndex(safeWritePosition, (int)(44100 * 0.1));
	SampleOffset maxWrite = m_audioBuffer.offsetDelta(writePosition, targetWritePosition);

	SampleOffset currentLead = m_audioBuffer.offsetDelta(safeWritePosition, writePosition);
	SampleOffset newLead = m_audioBuffer.offsetDelta(safeWritePosition, targetWritePosition);

	if (currentLead > 44100 * 0.5) {
		m_audioBuffer.m_writePointer = m_audioBuffer.getBufferIndex(safeWritePosition, (int)(44100 * 0.05));
		currentLead = m_audioBuffer.offsetDelta(safeWritePosition, m_audioBuffer.m_writePointer);
		maxWrite = m_audioBuffer.offsetDelta(m_audioBuffer.m_writePointer, targetWritePosition);
	}

	if (currentLead > newLead) {
		maxWrite = 0;
	}

	int16_t* samples = new int16_t[maxWrite];
	const int readSamples = m_simulator->readAudioOutput(maxWrite, samples);

	for (SampleOffset i = 0; i < (SampleOffset)readSamples && i < maxWrite; ++i) {
		const int16_t sample = samples[i];
		if (m_oscillatorSampleOffset % 4 == 0) {
			m_oscCluster->getAudioWaveformOscilloscope()->addDataPoint(
				m_oscillatorSampleOffset,
				sample / (float)(INT16_MAX));
		}

		m_audioBuffer.writeSample(sample, m_audioBuffer.m_writePointer, (int)i);

		m_oscillatorSampleOffset = (m_oscillatorSampleOffset + 1) % (44100 / 10);
	}

	delete[] samples;

	if (readSamples > 0) {
		SampleOffset size0, size1;
		void* data0, * data1;
		m_audioSource->LockBufferSegment(
			m_audioBuffer.m_writePointer, readSamples, &data0, &size0, &data1, &size1);

		m_audioBuffer.copyBuffer(
			reinterpret_cast<int16_t*>(data0), m_audioBuffer.m_writePointer, size0);
		m_audioBuffer.copyBuffer(
			reinterpret_cast<int16_t*>(data1),
			m_audioBuffer.getBufferIndex(m_audioBuffer.m_writePointer, size0),
			size1);

		m_audioSource->UnlockBufferSegments(data0, size0, data1, size1);
		m_audioBuffer.commitBlock(readSamples);
	}

	m_performanceCluster->addInputBufferUsageSample(
		(double)m_simulator->getSynthesizerInputLatency() / m_simulator->getSynthesizerInputLatencyTarget());
	m_performanceCluster->addAudioLatencySample(
		m_audioBuffer.offsetDelta(m_audioSource->GetCurrentWritePosition(), m_audioBuffer.m_writePointer) / (44100 * 0.1));

	json j = jbeam_json["pessima_body"]["slots"][5][1];
	auto s = to_string(j);  // calling nlohmann::to_string

	s.erase(
		remove(s.begin(), s.end(), '\"'),
		s.end()
	);

	m_infoCluster->setLogMessage(s);
}

void EngineSimApplication::render() {
	for (SimulationObject* object : m_objects) {
		object->generateGeometry();
	}

	m_viewParameters.Sublayer = 0;
	for (SimulationObject* object : m_objects) {
		object->render(&getViewParameters());
	}

	m_uiManager.render();
}

float EngineSimApplication::pixelsToUnits(float pixels) const {
	const float f = m_displayHeight / m_engineView->m_bounds.height();
	return pixels * f;
}

float EngineSimApplication::unitsToPixels(float units) const {
	const float f = m_engineView->m_bounds.height() / m_displayHeight;
	return units * f;
}

void EngineSimApplication::run() {
	while (true) {
		m_engine.StartFrame();

		if (!m_engine.IsOpen()) break;
		if (m_engine.ProcessKeyDown(ysKey::Code::Escape)) {
			break;
		}

		if (m_engine.ProcessKeyDown(ysKey::Code::Return)) {
			m_audioSource->SetMode(ysAudioSource::Mode::Stop);
			loadScript();
			if (m_simulator->getEngine() != nullptr) {
				m_audioSource->SetMode(ysAudioSource::Mode::Loop);
			}
		}

		if (m_engine.ProcessKeyDown(ysKey::Code::Tab)) {
			++(int&)m_screen;
			if (m_screen == SCREEN_MODE_LAST)
				m_screen = SCREEN_MODE_DRIVING;
		}

		if (m_engine.ProcessKeyDown(ysKey::Code::F)) {
			if (m_engine.GetGameWindow()->GetWindowStyle() != ysWindow::WindowStyle::Fullscreen) {
				m_engine.GetGameWindow()->SetWindowStyle(ysWindow::WindowStyle::Fullscreen);
				m_infoCluster->setLogMessage("Entered fullscreen mode");
			}
			else {
				m_engine.GetGameWindow()->SetWindowStyle(ysWindow::WindowStyle::Windowed);
				m_infoCluster->setLogMessage("Exited fullscreen mode");
			}
		}

		m_gameWindowHeight = m_engine.GetGameWindow()->GetGameHeight();
		m_screenHeight = m_engine.GetGameWindow()->GetScreenHeight();
		m_screenWidth = m_engine.GetGameWindow()->GetScreenWidth();

		updateScreenSizeStability();

		processEngineInput();

		if (m_engine.ProcessKeyDown(ysKey::Code::Insert) &&
			m_engine.GetGameWindow()->IsActive()) {
			if (!isRecording() && readyToRecord()) {
				startRecording();
			}
			else if (isRecording()) {
				stopRecording();
			}
		}

		if (isRecording() && !readyToRecord()) {
			stopRecording();
		}

		if (!m_paused || m_engine.ProcessKeyDown(ysKey::Code::Right)) {
			process(m_engine.GetFrameLength());
		}

		m_uiManager.update(m_engine.GetFrameLength());

		renderScene();

		m_engine.EndFrame();

		if (isRecording()) {
			recordFrame();
		}
	}

	if (isRecording()) {
		stopRecording();
	}

	m_simulator->endAudioRenderingThread();
}

void EngineSimApplication::destroy() {
	m_shaderSet.Destroy();

	m_engine.GetDevice()->DestroyGPUBuffer(m_geometryVertexBuffer);
	m_engine.GetDevice()->DestroyGPUBuffer(m_geometryIndexBuffer);

	m_assetManager.Destroy();
	m_engine.Destroy();

	m_simulator->destroy();
	m_audioBuffer.destroy();

	printf("\nQuit\n");

	/*
	if (gGameController != NULL)
	{
		SDL_GameControllerClose(gGameController);
		gGameController = NULL;
	}
	else if (gJoystick != NULL)
	{
		SDL_JoystickClose(gJoystick);
		gJoystick = NULL;
	}*/

	m_world->~b2World();
	m_world = nullptr;

	SDL_Quit();
}

void EngineSimApplication::loadEngine(
	Engine* engine,
	Vehicle* vehicle,
	Transmission* transmission)
{
	destroyObjects();

	if (m_simulator != nullptr) {
		m_simulator->releaseSimulation();
		delete m_simulator;
	}

	if (m_vehicle != nullptr) {
		delete m_vehicle;
		m_vehicle = nullptr;
	}

	if (m_transmission != nullptr) {
		delete m_transmission;
		m_transmission = nullptr;
	}

	if (m_iceEngine != nullptr) {
		m_iceEngine->destroy();
		delete m_iceEngine;
	}

	m_iceEngine = engine;
	m_vehicle = vehicle;
	m_transmission = transmission;

	m_simulator = engine->createSimulator(vehicle, transmission);

	if (engine == nullptr || vehicle == nullptr || transmission == nullptr) {
		m_iceEngine = nullptr;
		m_viewParameters.Layer1 = 0;

		return;
	}

	createObjects(engine);

	m_viewParameters.Layer1 = engine->getMaxDepth();
	engine->calculateDisplacement();

	m_simulator->setSimulationFrequency((int)engine->getSimulationFrequency());
	//m_simulator->setSimulationFrequency(5000);

	Synthesizer::AudioParameters audioParams = m_simulator->synthesizer().getAudioParameters();
	audioParams.inputSampleNoise = static_cast<float>(engine->getInitialJitter());
	audioParams.airNoise = static_cast<float>(engine->getInitialNoise());
	audioParams.dF_F_mix = static_cast<float>(engine->getInitialHighFrequencyGain());
	m_simulator->synthesizer().setAudioParameters(audioParams);

	for (int i = 0; i < engine->getExhaustSystemCount(); ++i) {
		ImpulseResponse* response = engine->getExhaustSystem(i)->getImpulseResponse();

		ysSdlAudioWaveFile waveFile;
		waveFile.OpenFile(response->getFilename().c_str());
		waveFile.InitializeInternalBuffer(waveFile.GetSampleCount());
		waveFile.FillBuffer(0);
		waveFile.CloseFile();

		m_simulator->synthesizer().initializeImpulseResponse(
			reinterpret_cast<const int16_t*>(waveFile.GetBuffer()),
			waveFile.GetSampleCount(),
			(float)response->getVolume(),
			i
		);

		waveFile.DestroyInternalBuffer();
	}

	m_simulator->startAudioRenderingThread();

	m_simulator->setSimulationFrequency((int)engine->getSimulationFrequency()/2);
}

void EngineSimApplication::drawGenerated(
	const GeometryGenerator::GeometryIndices& indices,
	int layer)
{
	drawGenerated(indices, layer, m_shaders.GetRegularFlags());
}

void EngineSimApplication::drawGeneratedUi(
	const GeometryGenerator::GeometryIndices& indices,
	int layer)
{
	drawGenerated(indices, layer, m_shaders.GetUiFlags());
}

void EngineSimApplication::drawGenerated(
	const GeometryGenerator::GeometryIndices& indices,
	int layer,
	dbasic::StageEnableFlags flags)
{
	m_engine.DrawGeneric(
		flags,
		m_geometryIndexBuffer,
		m_geometryVertexBuffer,
		sizeof(dbasic::Vertex),
		indices.BaseIndex,
		indices.BaseVertex,
		indices.FaceCount,
		false,
		layer);
}

void EngineSimApplication::configure(const ApplicationSettings& settings) {
	m_applicationSettings = settings;

	if (settings.startFullscreen) {
		m_engine.GetGameWindow()->SetWindowStyle(ysWindow::WindowStyle::Fullscreen);
	}

	m_background = ysColor::srgbiToLinear(m_applicationSettings.colorBackground);
	m_foreground = ysColor::srgbiToLinear(m_applicationSettings.colorForeground);
	m_shadow = ysColor::srgbiToLinear(m_applicationSettings.colorShadow);
	m_highlight1 = ysColor::srgbiToLinear(m_applicationSettings.colorHighlight1);
	m_highlight2 = ysColor::srgbiToLinear(m_applicationSettings.colorHighlight2);
	m_pink = ysColor::srgbiToLinear(m_applicationSettings.colorPink);
	m_red = ysColor::srgbiToLinear(m_applicationSettings.colorRed);
	m_orange = ysColor::srgbiToLinear(m_applicationSettings.colorOrange);
	m_yellow = ysColor::srgbiToLinear(m_applicationSettings.colorYellow);
	m_blue = ysColor::srgbiToLinear(m_applicationSettings.colorBlue);
	m_green = ysColor::srgbiToLinear(m_applicationSettings.colorGreen);
}

void EngineSimApplication::createObjects(Engine* engine) {

	GroundObject* groundObject = new GroundObject(this);
	groundObject->m_ground = engine->getGround();
	groundObject->m_vehicle = m_vehicle;
	m_objects.push_back(groundObject);

	VehicleObject* vehicleObject = new VehicleObject(this, m_world, m_vehicle);

	m_vehicle->m_transform = &vehicleObject->m_transform;
	m_vehicle->m_transform_engine = &vehicleObject->m_transform_engine;
	m_vehicle->m_transform_camera = &vehicleObject->m_transform_camera;

	m_objects.push_back(vehicleObject);
	m_vehicle_object = vehicleObject;

	int cylindersInBank = engine->getCylinderCount() / engine->getCylinderBankCount();

	for (int i = 0; i < engine->getCylinderCount(); ++i) {
		ConnectingRodObject* rodObject = new ConnectingRodObject;
		rodObject->initialize(this);
		rodObject->m_connectingRod = engine->getConnectingRod(i);
		m_objects.push_back(rodObject);

		PistonObject* pistonObject = new PistonObject;
		pistonObject->initialize(this);
		pistonObject->m_piston = engine->getPiston(i);
		m_objects.push_back(pistonObject);

		CombustionChamberObject* ccObject = new CombustionChamberObject;
		ccObject->initialize(this);
		ccObject->m_chamber = m_iceEngine->getChamber(i);
		m_objects.push_back(ccObject);
	}

	for (int i = 0; i < engine->getCrankshaftCount(); ++i) {
		CrankshaftObject* crankshaftObject = new CrankshaftObject;
		crankshaftObject->initialize(this);
		crankshaftObject->m_crankshaft = engine->getCrankshaft(i);
		m_objects.push_back(crankshaftObject);
	}

	for (int i = 0; i < engine->getCylinderBankCount(); ++i) {
		CylinderBankObject* cbObject = new CylinderBankObject;
		cbObject->initialize(this);
		cbObject->m_bank = engine->getCylinderBank(i);
		cbObject->m_head = engine->getHead(i);
		m_objects.push_back(cbObject);

		for (int j = 0; j < cylindersInBank; ++j)
		{
			//Logger::DebugLine("1 Cylinder index: " + std::to_string(j));
			CylinderHeadObject* chObject = new CylinderHeadObject(j);
			chObject->initialize(this);
			chObject->m_head = engine->getHead(i);
			chObject->m_engine = engine;
			m_objects.push_back(chObject);
		}

		int bankIndex = cbObject->m_bank->getIndex();
		int journalCount = engine->getCrankshaft(0)->getRodJournalCount();
		int cylinderCount = engine->getCylinderCount();

		cbObject->m_bank->m_dz = 0.0f;

		if (2 * journalCount == cylinderCount)
			//if (getSimulator()->getEngine()->getEngineType() == Engine::V_COMMON)
			cbObject->m_bank->m_dz = 0.09f * bankIndex * 0.25f * getCylinderDifferenceZ();
	}

	m_cylinderDifferenceZ = m_iniReader.Get<float>("Engine", "CylinderDifferenceZ");
	m_cylinderDifferenceZ *= (float)engine->getHead(0)->getCylinderBank()->getBore();

	engine->setEngineType();
}

void EngineSimApplication::destroyObjects() {
	for (SimulationObject* object : m_objects) {
		object->destroy();
		delete object;
	}

	m_objects.clear();
}

const SimulationObject::ViewParameters&
EngineSimApplication::getViewParameters() const
{
	return m_viewParameters;
}

void EngineSimApplication::loadScript() {
	Engine* engine = nullptr;
	Vehicle* vehicle = nullptr;
	Transmission* transmission = nullptr;

//#ifdef ATG_ENGINE_SIM_PIRANHA_ENABLED
	es_script::Compiler compiler;
	compiler.initialize();
    printf("\nmain.mr->");
	const bool compiled = compiler.compile("../assets/main.mr");
	if (compiled) {
		const es_script::Compiler::Output output = compiler.execute();
		configure(output.applicationSettings);

		engine = output.engine;
		vehicle = output.vehicle;
		transmission = output.transmission;
	}
	else {
		engine = nullptr;
		vehicle = nullptr;
		transmission = nullptr;
	}

	compiler.destroy();
//#endif /* ATG_ENGINE_SIM_PIRANHA_ENABLED */

	if (vehicle == nullptr) {
		Vehicle::Parameters vehParams;
		vehParams.mass = units::mass(1597, units::kg);
		vehParams.diffRatio = 3.42;
		vehParams.tireRadius = units::distance(10, units::inch);
		vehParams.dragCoefficient = 0.25;
		vehParams.crossSectionArea = units::distance(6.0, units::foot) * units::distance(6.0, units::foot);
		vehParams.rollingResistance = 400.0;
		vehicle = new Vehicle;
		vehicle->initialize(vehParams);
	}

	if (transmission == nullptr) {
		const double gearRatios[] = { 2.97, 2.07, 1.43, 1.00, 0.84, 0.56 };
		Transmission::Parameters tParams;
		tParams.GearCount = 6;
		tParams.GearRatios = gearRatios;
		tParams.MaxClutchTorque = units::torque(1000.0, units::ft_lb);
		transmission = new Transmission;
		transmission->initialize(tParams);
	}

	loadEngine(engine, vehicle, transmission);
	refreshUserInterface();
}

void EngineSimApplication::processEngineInput() {

	if (m_iceEngine == nullptr) {
		return;
	}

	int mx, my;
	int osmx, osmy;
	float fmx;
	m_engine.GetMousePos(&mx, &my);
	m_engine.GetOsMousePos(&osmx, &osmy);
	osmx = osmx - 959;
	fmx = (float)osmx / 959.0f;
	fmx = clamp(fmx, -1.0f, 1.0f);

	//j_Controller.Update();

	float speed;
	float steeringAngle = m_vehicle->getSteeringAngle();

	if (m_engine.IsKeyDown(ysKey::Code::Left))
		m_vehicle->setSteeringAngle(-1.0f);
	else if (m_engine.IsKeyDown(ysKey::Code::Right))
		m_vehicle->setSteeringAngle(1.0f);
	else
		m_vehicle->setSteeringAngle(0.0f);


	/* if (j_Controller.Alive() && m_controller == GAME_CONTROLLER) {
		float x = j_Controller.LSx();
		speed = m_vehicle->getSpeed();
		steeringAngle = 4.0f * x * abs(x) / (1.0f + abs(0.15f * speed));
		m_vehicle->setSteeringAngle(steeringAngle);
	}
	else */
	{
		steeringAngle = fmx * abs(fmx);
		m_vehicle->setSteeringAngle(steeringAngle);
		m_infoCluster->setLogMessage("Mouse x-position: " + std::to_string(fmx));
	}

	

	

	if (m_engine.ProcessMouseButtonDown(ysMouse::Button::Middle))
		m_dragStartMousePosition = ysVector2((float)mx, (float)my);

	const float dt = m_engine.GetFrameLength();
	const bool fineControlMode = m_engine.IsKeyDown(ysKey::Code::Space);

	const int mouseWheel = m_engine.GetMouseWheel();
	const int mouseWheelDelta = mouseWheel - m_lastMouseWheel;
	m_lastMouseWheel = mouseWheel;

	m_camera.zoom = 0.98f * m_camera.zoom + 0.02f * clamp(m_camera.zoom - 2.0f * mouseWheelDelta, 0.1f, 30.0f);

	if (m_engine.ProcessKeyDown(ysKey::Code::B))
	{
		m_show_engine2 = !m_show_engine2;
		m_infoCluster->setLogMessage("Show engine 2: " + std::to_string(m_show_engine2));
	}
	if (m_engine.ProcessKeyDown(ysKey::Code::M))
	{
		m_show_engine = !m_show_engine;
		m_infoCluster->setLogMessage("Show engine: " + std::to_string(m_show_engine));
	}
	if (m_engine.ProcessKeyDown(ysKey::Code::N))
	{
		setDebugMode(!getDebugMode());
	}

	bool fineControlInUse = false;
	if (m_engine.IsKeyDown(ysKey::Code::Z)) {
		const double rate = fineControlMode
			? 0.001
			: 0.01;

		Synthesizer::AudioParameters audioParams = m_simulator->synthesizer().getAudioParameters();
		audioParams.volume = (float)clamp(audioParams.volume + mouseWheelDelta * rate * dt);

		m_simulator->synthesizer().setAudioParameters(audioParams);
		fineControlInUse = true;

		m_infoCluster->setLogMessage("[Z] - Set volume to " + std::to_string(audioParams.volume));
	}
	else if (m_engine.IsKeyDown(ysKey::Code::X)) {
		const double rate = fineControlMode
			? 0.001
			: 0.01;

		Synthesizer::AudioParameters audioParams = m_simulator->synthesizer().getAudioParameters();
		audioParams.convolution = (float)clamp(audioParams.convolution + mouseWheelDelta * rate * dt);

		m_simulator->synthesizer().setAudioParameters(audioParams);
		fineControlInUse = true;

		m_infoCluster->setLogMessage("[X] - Set convolution level to " + std::to_string(audioParams.convolution));
	}
	else if (m_engine.IsKeyDown(ysKey::Code::C)) {
		const double rate = fineControlMode
			? 0.00001
			: 0.001;

		Synthesizer::AudioParameters audioParams = m_simulator->synthesizer().getAudioParameters();
		audioParams.dF_F_mix = (float)clamp(audioParams.dF_F_mix + mouseWheelDelta * rate * dt);

		m_simulator->synthesizer().setAudioParameters(audioParams);
		fineControlInUse = true;

		m_infoCluster->setLogMessage("[C] - Set high freq. gain to " + std::to_string(audioParams.dF_F_mix));
	}
	else if (m_engine.IsKeyDown(ysKey::Code::V)) {
		const double rate = fineControlMode
			? 0.001
			: 0.01;

		Synthesizer::AudioParameters audioParams = m_simulator->synthesizer().getAudioParameters();
		audioParams.airNoise = (float)clamp(audioParams.airNoise + mouseWheelDelta * rate * dt);

		m_simulator->synthesizer().setAudioParameters(audioParams);
		fineControlInUse = true;

		m_infoCluster->setLogMessage("[V] - Set low freq. noise to " + std::to_string(audioParams.airNoise));
	}
	else if (m_engine.IsKeyDown(ysKey::Code::B)) {
		const double rate = fineControlMode
			? 0.001
			: 0.01;

		Synthesizer::AudioParameters audioParams = m_simulator->synthesizer().getAudioParameters();
		audioParams.inputSampleNoise = (float)clamp(audioParams.inputSampleNoise + mouseWheelDelta * rate * dt);

		m_simulator->synthesizer().setAudioParameters(audioParams);
		fineControlInUse = true;

		m_infoCluster->setLogMessage("[B] - Set high freq. noise to " + std::to_string(audioParams.inputSampleNoise));
	}
	else if (m_engine.IsKeyDown(ysKey::Code::N)) {
		const double rate = fineControlMode
			? 10.0
			: 100.0;

		const double newSimulationFrequency = clamp(
			m_simulator->getSimulationFrequency() + mouseWheelDelta * rate * dt,
			400.0, 400000.0);

		m_simulator->setSimulationFrequency((int)newSimulationFrequency);
		fineControlInUse = true;

		m_infoCluster->setLogMessage("[N] - Set simulation freq to " + std::to_string(m_simulator->getSimulationFrequency()));
	}
	else if (m_engine.IsKeyDown(ysKey::Code::G) && m_simulator->m_dyno.m_hold) {
		if (mouseWheelDelta > 0) {
			m_dynoSpeed += m_iceEngine->getDynoHoldStep();
		}
		else if (mouseWheelDelta < 0) {
			m_dynoSpeed -= m_iceEngine->getDynoHoldStep();
		}

		m_dynoSpeed = clamp(m_dynoSpeed, m_iceEngine->getDynoMinSpeed(), m_iceEngine->getDynoMaxSpeed());

		m_infoCluster->setLogMessage("[G] - Set dyno speed to " + std::to_string(units::toRpm(m_dynoSpeed)));
		fineControlInUse = true;
	}

	const double prevTargetThrottle = m_targetSpeedSetting;
	m_targetSpeedSetting = fineControlMode ? m_targetSpeedSetting : 0.0;
	if (m_engine.IsKeyDown(ysKey::Code::Q)) {
		m_targetSpeedSetting = 0.01;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::W)) {
		m_targetSpeedSetting = 0.1;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::E)) {
		m_targetSpeedSetting = 0.2;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::R)) {
		m_targetSpeedSetting = 1.0;
	}
	else if (fineControlMode && !fineControlInUse) {
		m_targetSpeedSetting = clamp(m_targetSpeedSetting + mouseWheelDelta * 0.0001);
	}

	/* if (j_Controller.Alive() && m_controller == GAME_CONTROLLER) {
		m_targetSpeedSetting = j_Controller.RT();
	} */

	if (prevTargetThrottle != m_targetSpeedSetting) {
		m_infoCluster->setLogMessage("Speed control set to " + std::to_string(m_targetSpeedSetting));
	}

	m_speedSetting = m_targetSpeedSetting * 0.5 + 0.5 * m_speedSetting;

	m_iceEngine->setSpeedControl(m_speedSetting);
	if (m_engine.ProcessKeyDown(ysKey::Code::M)) {
		const int currentLayer = getViewParameters().Layer0;
		if (currentLayer + 1 < m_iceEngine->getMaxDepth()) {
			setViewLayer(currentLayer + 1);
		}

		m_infoCluster->setLogMessage("[M] - Set render layer to " + std::to_string(getViewParameters().Layer0));
	}

	if (m_engine.ProcessKeyDown(ysKey::Code::OEM_Comma)) {
		if (getViewParameters().Layer0 - 1 >= 0)
			setViewLayer(getViewParameters().Layer0 - 1);

		m_infoCluster->setLogMessage("[,] - Set render layer to " + std::to_string(getViewParameters().Layer0));
	}

	if (m_engine.ProcessKeyDown(ysKey::Code::D) /*|| j_Controller.buttonPressed(j_Buttons.l_s)*/)
	//if (m_engine.ProcessKeyDown(ysKey::Code::D) || j_Controller.buttonDown(j_Buttons.l_s))
	//if(j_Controller.RSy()<-0.1f)
	{
		//"Brakes"
		m_vehicle->m_rotatingMass->v_theta *= 0.98f;
		//m_vehicle->m_rotatingMass->v_theta -= 100;
		//m_vehicle->m_brakes = j_Controller.LSy();
		m_vehicle->m_brakes = 0.99f;
		//j_Controller.Rumble(0.3,0.3);
	}
	else
		m_vehicle->m_brakes = 0.0;

	/*
	if (m_engine.ProcessKeyDown(ysKey::Code::D) || j_Controller.buttonDown(j_Buttons.l_s)) {
		m_simulator->m_dyno.m_enabled = !m_simulator->m_dyno.m_enabled;

		const std::string msg = m_simulator->m_dyno.m_enabled
			? "DYNOMOMETER ENABLED"
			: "DYNOMOMETER DISABLED";
		m_infoCluster->setLogMessage(msg);
	}*/

	if (m_engine.ProcessKeyDown(ysKey::Code::H)) {
		m_simulator->m_dyno.m_hold = !m_simulator->m_dyno.m_hold;

		const std::string msg = m_simulator->m_dyno.m_hold
			? m_simulator->m_dyno.m_enabled ? "HOLD ENABLED" : "HOLD ON STANDBY [ENABLE DYNO. FOR HOLD]"
			: "HOLD DISABLED";
		m_infoCluster->setLogMessage(msg);
	}

	if (m_simulator->m_dyno.m_enabled) {
		if (!m_simulator->m_dyno.m_hold) {
			if (m_simulator->getFilteredDynoTorque() > units::torque(1.0, units::ft_lb)) {
				m_dynoSpeed += units::rpm(500) * dt;
			}
			else {
				m_dynoSpeed *= (1 / (1 + dt));
			}

			if (m_dynoSpeed > m_iceEngine->getRedline()) {
				m_simulator->m_dyno.m_enabled = false;
				m_dynoSpeed = units::rpm(0);
			}
		}
	}
	else {
		if (!m_simulator->m_dyno.m_hold) {
			m_dynoSpeed = units::rpm(0);
		}
	}

	m_dynoSpeed = clamp(m_dynoSpeed, m_iceEngine->getDynoMinSpeed(), m_iceEngine->getDynoMaxSpeed());
	m_simulator->m_dyno.m_rotationSpeed = m_dynoSpeed;

	const bool prevStarterEnabled = m_simulator->m_starterMotor.m_enabled;
	if (m_engine.IsKeyDown(ysKey::Code::S) /*|| (j_Controller.Alive() && j_Controller.buttonPressed(j_Buttons.B))*/) {
		m_simulator->m_starterMotor.m_enabled = true;
	}
	else {
		m_simulator->m_starterMotor.m_enabled = false;
	}

	if (prevStarterEnabled != m_simulator->m_starterMotor.m_enabled) {
		const std::string msg = m_simulator->m_starterMotor.m_enabled
			? "STARTER ENABLED"
			: "STARTER DISABLED";
		m_infoCluster->setLogMessage(msg);
	}

	if (m_engine.ProcessKeyDown(ysKey::Code::A) /*|| j_Controller.buttonDown(j_Buttons.Y)*/) {
		m_simulator->getEngine()->getIgnitionModule()->m_enabled =
			!m_simulator->getEngine()->getIgnitionModule()->m_enabled;

		const std::string msg = m_simulator->getEngine()->getIgnitionModule()->m_enabled
			? "IGNITION ENABLED"
			: "IGNITION DISABLED";
		m_infoCluster->setLogMessage(msg);
	}

	if (m_engine.ProcessKeyDown(ysKey::Code::Up) /*|| j_Controller.buttonDown(j_Buttons.A) || m_engine.ProcessMouseButtonDown(ysMouse::Button::Right)*/) {
		m_simulator->getTransmission()->changeGear(m_simulator->getTransmission()->getGear() + 1);

		m_infoCluster->setLogMessage(
			"UPSHIFTED TO " + std::to_string(m_simulator->getTransmission()->getGear() + 1));
	}

	if (m_engine.ProcessKeyDown(ysKey::Code::Down) /*|| j_Controller.buttonDown(j_Buttons.X) || m_engine.ProcessMouseButtonDown(ysMouse::Button::Left)*/) {
		m_simulator->getTransmission()->changeGear(m_simulator->getTransmission()->getGear() - 1);

		if (m_simulator->getTransmission()->getGear() != -1) {
			m_infoCluster->setLogMessage(
				"DOWNSHIFTED TO " + std::to_string(m_simulator->getTransmission()->getGear() + 1));
		}
		else {
			m_infoCluster->setLogMessage("SHIFTED TO NEUTRAL");
		}
	}

	if (m_engine.IsKeyDown(ysKey::Code::T)) {
		m_targetClutchPressure -= 0.2 * dt;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::U)) {
		m_targetClutchPressure += 0.2 * dt;
	}
	else if (m_engine.IsKeyDown(ysKey::Code::Shift)) {
		m_targetClutchPressure = 0.0;
		m_infoCluster->setLogMessage("CLUTCH DEPRESSED");
	}
	/* else if (j_Controller.Alive()) {
		m_targetClutchPressure = 1 - j_Controller.LT();
	} */
	else if (!m_engine.IsKeyDown(ysKey::Code::Y)) {
		m_targetClutchPressure = 1.0;
	}

	m_targetClutchPressure = clamp(m_targetClutchPressure);

	double clutchRC = 0.001;
	if (m_engine.IsKeyDown(ysKey::Code::Space)) {
		clutchRC = 1.0;
	}

	const double clutch_s = dt / (dt + clutchRC);
	m_clutchPressure = m_clutchPressure * (1 - clutch_s) + m_targetClutchPressure * clutch_s;
	m_simulator->getTransmission()->setClutchPressure(m_clutchPressure);

	//j_Controller.RefreshState();
}

void EngineSimApplication::renderScene() {

	getShaders()->ResetBaseColor();
	getShaders()->SetObjectTransform(ysMath::LoadIdentity());

	m_textRenderer.SetColor(ysColor::linearToSrgb(m_foreground));
	m_shaders.SetClearColor(ysColor::linearToSrgb(m_shadow));

	const int screenWidth = m_engine.GetGameWindow()->GetGameWidth();
	const int screenHeight = m_engine.GetGameWindow()->GetGameHeight();
	const float aspectRatio = screenWidth / (float)screenHeight;

	const Point cameraPos = m_engineView->getCameraPosition();
	m_shaders.m_cameraPosition = ysMath::LoadVector(cameraPos.x, cameraPos.y);

	m_shaders.CalculateUiCamera((float)screenWidth, (float)screenHeight);

	bool isTargetEngine;

	//enum ScreenMode { SCREEN_MODE_DRIVING, SCREEN_MODE_1, SCREEN_MODE_2, SCREEN_MODE_3 };
	ScreenMode screenMode = (ScreenMode)m_screen;
	
	if (m_screen == SCREEN_MODE_DRIVING)
		isTargetEngine = false;
	else
		isTargetEngine = true;

	switch (screenMode)
	{
	case SCREEN_MODE_3:
	{
		Bounds windowBounds((float)screenWidth, (float)screenHeight, { 0, (float)screenHeight });
		Grid grid;
		grid.v_cells = 2;
		grid.h_cells = 3;
		Grid grid3x3;
		grid3x3.v_cells = 3;
		grid3x3.h_cells = 3;
		m_engineView->setDrawFrame(true);
		m_engineView->setBounds(grid.get(windowBounds, 1, 0, 1, 1));
		m_engineView->setLocalPosition({ 0, 0 });

		m_rightGaugeCluster->m_bounds = grid.get(windowBounds, 2, 0, 1, 2);
		m_oscCluster->m_bounds = grid.get(windowBounds, 1, 1);
		m_performanceCluster->m_bounds = grid3x3.get(windowBounds, 0, 1);
		m_loadSimulationCluster->m_bounds = grid3x3.get(windowBounds, 0, 2);

		Grid grid1x3;
		grid1x3.v_cells = 3;
		grid1x3.h_cells = 1;
		m_mixerCluster->m_bounds = grid1x3.get(grid3x3.get(windowBounds, 0, 0), 0, 2);
		m_infoCluster->m_bounds = grid1x3.get(grid3x3.get(windowBounds, 0, 0), 0, 0, 1, 2);

		m_engineView->setVisible(true);
		m_rightGaugeCluster->setVisible(true);
		m_oscCluster->setVisible(true);
		m_performanceCluster->setVisible(true);
		m_loadSimulationCluster->setVisible(true);
		m_mixerCluster->setVisible(true);
		m_infoCluster->setVisible(true);

		m_customGaugeCluster->setVisible(false);

		m_oscCluster->activate();
		break;
	}
	case SCREEN_MODE_1:
	{
		Bounds windowBounds((float)screenWidth, (float)screenHeight, { 0, (float)screenHeight });
		m_engineView->setDrawFrame(false);
		m_engineView->setBounds(windowBounds);
		m_engineView->setLocalPosition({ 0, 0 });
		m_engineView->activate();

		m_engineView->setVisible(true);
		m_rightGaugeCluster->setVisible(false);
		m_oscCluster->setVisible(false);
		m_performanceCluster->setVisible(false);
		m_loadSimulationCluster->setVisible(false);
		m_mixerCluster->setVisible(false);
		m_infoCluster->setVisible(false);

		m_customGaugeCluster->setVisible(false);

		break;
	}
	case SCREEN_MODE_2:
	{
		Bounds windowBounds((float)screenWidth, (float)screenHeight, { 0, (float)screenHeight });
		Grid grid;
		grid.v_cells = 1;
		grid.h_cells = 3;
		m_engineView->setDrawFrame(true);
		m_engineView->setBounds(grid.get(windowBounds, 0, 0, 2, 1));
		m_engineView->setLocalPosition({ 0, 0 });
		m_engineView->activate();

		m_rightGaugeCluster->m_bounds = grid.get(windowBounds, 2, 0, 1, 1);

		m_engineView->setVisible(true);
		m_rightGaugeCluster->setVisible(true);
		m_oscCluster->setVisible(false);
		m_performanceCluster->setVisible(false);
		m_loadSimulationCluster->setVisible(false);
		m_mixerCluster->setVisible(false);
		m_infoCluster->setVisible(false);

		m_customGaugeCluster->setVisible(false);

		break;
	}
	case SCREEN_MODE_DRIVING:
	{
		Bounds windowBounds((float)screenWidth, (float)screenHeight, { 0, (float)screenHeight });
		Grid grid;
		grid.v_cells = 9;
		grid.h_cells = 16;

		Bounds engineViewBounds = grid.get(windowBounds, 0, 0, 16, 9);
		m_engineView->setDrawFrame(false);

		m_engineView->setBounds(engineViewBounds);

		m_engineView->setLocalPosition({ 0, 0 });
		m_engineView->setVisible(true);

		m_infoCluster->m_bounds = grid.get(windowBounds, 0, 0, 16, 2);
		m_rightGaugeCluster->m_bounds = grid.get(windowBounds, 11, 0, 5, 9);
		m_loadSimulationCluster->m_bounds = grid.get(windowBounds, 0, 2, 5, 2);

		m_customGaugeCluster->m_bounds = grid.get(windowBounds, 0, 0, 16, 9);

		m_oscCluster->setVisible(false);
		m_performanceCluster->setVisible(false);
		m_mixerCluster->setVisible(false);

		m_infoCluster->setVisible(false);
		m_loadSimulationCluster->setVisible(false);
		m_rightGaugeCluster->setVisible(false);

		m_customGaugeCluster->setVisible(true);

		SDL_ShowCursor(SDL_DISABLE);

		break;
	}
    case SCREEN_MODE_LAST: break;
	}

	const float cameraAspectRatio =
		m_engineView->m_bounds.width() / m_engineView->m_bounds.height();
	m_engine.GetDevice()->ResizeRenderTarget(
		m_mainRenderTarget,
		(int)m_engineView->m_bounds.width(),
		(int)m_engineView->m_bounds.height(),
		(int)m_engineView->m_bounds.width(),
		(int)m_engineView->m_bounds.height()
	);
	m_engine.GetDevice()->RepositionRenderTarget(
		m_mainRenderTarget,
		(int)m_engineView->m_bounds.getPosition(Bounds::tl).x,
		screenHeight - (int)m_engineView->m_bounds.getPosition(Bounds::tl).y
	);

	int mx, my;
	m_engine.GetMousePos(&mx, &my);

	
	/* if (j_Controller.Alive() && m_controller == GAME_CONTROLLER)
	{
		if (isTargetEngine)
		{
			m_camera.rotation.x += 0.03f * -j_Controller.RSx();
			m_camera.rotation.y += 0.03f * -j_Controller.RSy();
			
		}
		else
		{
			float x = j_Controller.RSx();
			float y = j_Controller.RSy();
			if (abs(x) < 0.1f) x = 0.0f; // Dead zone x
			if (abs(y) < 0.1f) y = 0.0f; // Dead zone y
			m_camera.rotation.x += 0.03f * -x;
			m_camera.rotation.y += 0.03f * -y;
		}
	} */

	// Rotating orbit camera with mouse
	if (m_engine.IsMouseButtonDown(ysMouse::Button::Middle))
	{
		float orbitSpeed = 0.0004f;

		float deltaX = +orbitSpeed * (mx - m_dragStartMousePosition.x);
		float deltaY = -orbitSpeed * (my - m_dragStartMousePosition.y);

		if (abs(deltaX) > 0.001f)
			m_camera.rotation.x += deltaX;

		if (abs(deltaY) > 0.001f)
			m_camera.rotation.y += deltaY;

		m_camera.rotation.y = clamp(m_camera.rotation.y, 0.01f * ysMath::Constants::PI, 0.5f * ysMath::Constants::PI);
	}
	// Chase camera
	else if (!isTargetEngine)
	{
		float rotationSpeed = 0.5f * 0.06f;
		m_camera.rotation.x = (1.0f - rotationSpeed) * m_camera.rotation.x - rotationSpeed * (m_simulator->getVehicle()->m_rotation + 0.5f * ysMath::Constants::PI);

		int ry = m_engine.GetJoystickAxisRY();

		if (abs(ry) > 2000)
			m_camera.rotation.y += 0.000001f * ry;
	}

	// Limit y rotation to avoid glitches
	m_camera.rotation.y = clamp(m_camera.rotation.y, 0.01f * ysMath::Constants::PI, 0.5f * ysMath::Constants::PI);

	// Bring x rotation back between [-2PI,2PI] range
	if (m_camera.rotation.x >  2.0f * ysMath::Constants::TWO_PI) m_camera.rotation.x -= ysMath::Constants::TWO_PI;
	if (m_camera.rotation.x < -2.0f * ysMath::Constants::TWO_PI) m_camera.rotation.x += ysMath::Constants::TWO_PI;

	ysVector3 enginePosition = ysMath::GetVector3(getSimulator()->getVehicle()->m_transform_engine->GetWorldPosition());

	m_camera.position.x = enginePosition.x;
	m_camera.position.y = enginePosition.y;
	m_camera.position.z = enginePosition.z + getSimulator()->getEngine()->getCylinderCount() / (float)getSimulator()->getEngine()->getCylinderBankCount() * 0.04f;

	
	m_shaders.CalculateCamera(
		cameraAspectRatio* m_displayHeight / m_engineView->m_zoom,
		m_displayHeight / m_engineView->m_zoom,

		m_engineView->m_bounds,

		m_screenWidth,
		m_screenHeight,

		1.0f,

		m_camera.rotation.x,
		m_camera.rotation.y,

		m_camera.zoom,

		m_camera.position.x,
		m_camera.position.y,
		m_camera.position.z);
	
	/*
	m_shaders.CalculateCamera(
		cameraAspectRatio * m_displayHeight / m_engineView->m_zoom,
		m_displayHeight / m_engineView->m_zoom,

		m_engineView->m_bounds,

		m_screenWidth,
		m_screenHeight,

		1.0f,

		m_camera.rotation.x,
		m_camera.rotation.y,

		m_camera.zoom,

		enginePosition.x,
		enginePosition.y,
		enginePosition.z + getSimulator()->getEngine()->getCylinderCount() / getSimulator()->getEngine()->getCylinderBankCount() * 0.04f);
	*/

	m_geometryGenerator.reset();

	render();

	m_engine.GetDevice()->EditBufferDataRange(
		m_geometryVertexBuffer,
		(char*)m_geometryGenerator.getVertexData(),
		sizeof(dbasic::Vertex) * m_geometryGenerator.getCurrentVertexCount(),
		0);

	m_engine.GetDevice()->EditBufferDataRange(
		m_geometryIndexBuffer,
		(char*)m_geometryGenerator.getIndexData(),
		sizeof(unsigned short) * m_geometryGenerator.getCurrentIndexCount(),
		0);
}

void EngineSimApplication::refreshUserInterface() {
	m_uiManager.destroy();
	m_uiManager.initialize(this);

	m_engineView = m_uiManager.getRoot()->addElement<EngineView>();
	m_rightGaugeCluster = m_uiManager.getRoot()->addElement<RightGaugeCluster>();
	m_oscCluster = m_uiManager.getRoot()->addElement<OscilloscopeCluster>();
	m_performanceCluster = m_uiManager.getRoot()->addElement<PerformanceCluster>();
	m_loadSimulationCluster = m_uiManager.getRoot()->addElement<LoadSimulationCluster>();
	m_mixerCluster = m_uiManager.getRoot()->addElement<MixerCluster>();
	m_infoCluster = m_uiManager.getRoot()->addElement<InfoCluster>();
	m_customGaugeCluster = m_uiManager.getRoot()->addElement<CustomGaugeCluster>();

	m_infoCluster->setEngine(m_iceEngine);

	m_rightGaugeCluster->m_simulator = m_simulator;
	m_rightGaugeCluster->setEngine(m_iceEngine);

	m_customGaugeCluster->m_simulator = m_simulator;
	m_customGaugeCluster->setEngine(m_iceEngine);

	m_oscCluster->setSimulator(m_simulator);
	if (m_iceEngine != nullptr) {
		m_oscCluster->setDynoMaxRange(units::toRpm(m_iceEngine->getRedline()));
	}
	m_performanceCluster->setSimulator(m_simulator);
	m_loadSimulationCluster->setSimulator(m_simulator);
	m_mixerCluster->setSimulator(m_simulator);
}

void EngineSimApplication::startRecording() {
	m_recording = true;

#ifdef ATG_ENGINE_SIM_VIDEO_CAPTURE
	atg_dtv::Encoder::VideoSettings settings{};

	// Output filename
	settings.fname = "../workspace/video_capture/engine_sim_video_capture.mp4";
	settings.inputWidth = m_engine.GetScreenWidth();
	settings.inputHeight = m_engine.GetScreenHeight();
	settings.width = settings.inputWidth;
	settings.height = settings.inputHeight;
	settings.hardwareEncoding = true;
	settings.inputAlpha = true;
	settings.bitRate = 40000000;

	m_encoder.run(settings, 2);
#endif /* ATG_ENGINE_SIM_VIDEO_CAPTURE */
}

void EngineSimApplication::updateScreenSizeStability() {
	m_screenResolution[m_screenResolutionIndex][0] = m_engine.GetScreenWidth();
	m_screenResolution[m_screenResolutionIndex][1] = m_engine.GetScreenHeight();

	m_screenResolutionIndex = (m_screenResolutionIndex + 1) % ScreenResolutionHistoryLength;
}

bool EngineSimApplication::readyToRecord() {
	const int w = m_screenResolution[0][0];
	const int h = m_screenResolution[0][1];

	if (w <= 0 && h <= 0) return false;
	if ((w % 2) != 0 || (h % 2) != 0) return false;

	for (int i = 1; i < ScreenResolutionHistoryLength; ++i) {
		if (m_screenResolution[i][0] != w) return false;
		if (m_screenResolution[i][1] != h) return false;
	}

	return true;
}

void EngineSimApplication::stopRecording() {
	m_recording = false;

#ifdef ATG_ENGINE_SIM_VIDEO_CAPTURE
	m_encoder.commit();
	m_encoder.stop();
#endif /* ATG_ENGINE_SIM_VIDEO_CAPTURE */
}

void EngineSimApplication::recordFrame() {
#ifdef ATG_ENGINE_SIM_VIDEO_CAPTURE
	atg_dtv::Frame* frame = m_encoder.newFrame(false);
	if (frame != nullptr && m_encoder.getError() == atg_dtv::Encoder::Error::None) {
		m_engine.GetDevice()->ReadRenderTarget(m_engine.GetScreenRenderTarget(), frame->m_rgb);
	}

	m_encoder.submitFrame();
#endif /* ATG_ENGINE_SIM_VIDEO_CAPTURE */
}
