#ifndef ATG_ENGINE_SIM_ENGINE_SIM_APPLICATION_H
#define ATG_ENGINE_SIM_ENGINE_SIM_APPLICATION_H

#include <SDL.h>

#include "joystick.h"

#include "geometry_generator.h"
#include "simulator.h"
#include "engine.h"
#include "simulation_object.h"
#include "ui_manager.h"
#include "dynamometer.h"
#include "oscilloscope.h"
#include "audio_buffer.h"
#include "convolution_filter.h"
#include "shaders.h"
#include "engine_view.h"
#include "right_gauge_cluster.h"
#include "custom_gauge_cluster.h"
#include "cylinder_temperature_gauge.h"
#include "synthesizer.h"
#include "oscilloscope_cluster.h"
#include "performance_cluster.h"
#include "load_simulation_cluster.h"
#include "mixer_cluster.h"
#include "info_cluster.h"
#include "application_settings.h"
#include "transmission.h"

#include "delta.h"
#include "dtv.h"

#include <fstream>
#include "nlohmann/json.hpp"

#include "ini/ini.h"
#include "loader/logger.h"
#include "../dependencies/submodules/box2d/include/box2d/box2d.h"
#include "vehicle_object.h"
#include "ground_object.h"

#include <vector>

using json = nlohmann::json;

template <typename BasicJsonType>
std::string to_string(const BasicJsonType& j)
{
    return j.dump();
}

struct Camera {
    float zoom = 2.0f;
    float fovY = 1.0f;
    float aspectRatio = 16.0f / 9.0f;
    ysVector2 rotation = ysVector2(0.0f, 0.0f);
    ysVector3 position = ysVector3(0.0f, 0.0f, 0.0f);
    ysVector3 target = ysVector3(0.0f, 0.0f, 0.0f);
};

class EngineSimApplication {
    

    private:
        static std::string s_buildVersion;

    public:
        EngineSimApplication();
        virtual ~EngineSimApplication();

        json jbeam_json;

        static std::string getBuildVersion() { return s_buildVersion; }

        std::string intToString(int number);
        void initialize(void *instance, ysContextObject::DeviceAPI api);
        void run();
        void destroy();

        void loadEngine(Engine *engine, Vehicle *vehicle, Transmission *transmission);
        void drawGenerated(
                const GeometryGenerator::GeometryIndices &indices,
                int layer = 0);
        void drawGeneratedUi(
                const GeometryGenerator::GeometryIndices &indices,
                int layer = 0);
        void drawGenerated(
                const GeometryGenerator::GeometryIndices &indices,
                int layer,
                dbasic::StageEnableFlags flags);
        void configure(const ApplicationSettings &settings);
        GeometryGenerator *getGeometryGenerator() { return &m_geometryGenerator; }

        Shaders *getShaders() { return &m_shaders; }
        dbasic::TextRenderer *getTextRenderer() { return &m_textRenderer; }

        void createObjects(Engine *engine);
        void destroyObjects();
        dbasic::DeltaEngine *getEngine() { return &m_engine; }

        float pixelsToUnits(float pixels) const;
        float unitsToPixels(float units) const;

        ysVector getBackgroundColor() const { return m_background; }
        ysVector getForegroundColor() const { return m_foreground; }
        ysVector getHightlight1Color() const { return m_highlight1; }
        ysVector getPink() const { return m_pink; }
        ysVector getGreen() const { return m_green; }
        ysVector getYellow() const { return m_yellow; }
        ysVector getRed() const { return m_red; }
        ysVector getOrange() const { return m_orange; }
        ysVector getBlue() const { return m_blue; }

        const SimulationObject::ViewParameters &getViewParameters() const;
        void setViewLayer(int view) { m_viewParameters.Layer0 = view; }

        dbasic::AssetManager *getAssetManager() { return &m_assetManager; }

        int getScreenWidth() const { return m_screenWidth; }
        int getScreenHeight() const { return m_screenHeight; }

        Simulator *getSimulator() { return m_simulator; }
        InfoCluster *getInfoCluster() { return m_infoCluster; }
        ApplicationSettings* getAppSettings() { return &m_applicationSettings; }

        inih::INIReader getIniReader() { return m_iniReader; }

        float getCylinderDifferenceZ() { return m_cylinderDifferenceZ; }

        void loadMaterial(std::string filename, std::string name);
        void loadModel(std::string filename, float scale=1.0f);

        Camera getCamera() { return m_camera; }

        float getScaleModel() { return m_scaleModel; }

        bool getDebugMode() { return m_debug_mode;  }
        void setDebugMode(bool debug_mode) { m_debug_mode = debug_mode; }
        bool getShowEngine() { return m_show_engine; }
        bool getShowEngine2() { return m_show_engine2; }
        bool getShowTrack() { return m_show_track; }
        bool getShowEngineOnly() { return m_show_engine_only; }
        int getSelectedTrack() { return m_selected_track; }
        b2World* getWorld() { return m_world; }

    protected:
        void loadScript();
        void processEngineInput();
        void renderScene();

        void refreshUserInterface();

    protected:
        double m_speedSetting = 1.0;
        double m_targetSpeedSetting = 1.0;

        double m_clutchPressure = 1.0;
        double m_targetClutchPressure = 1.0;
        int m_lastMouseWheel = 0;

    protected:
        virtual void initialize();
        virtual void process(float dt);
        virtual void render();

        float m_displayAngle;
        float m_displayHeight;
        int m_gameWindowHeight;
        int m_screenWidth;
        int m_screenHeight;
        
        ApplicationSettings m_applicationSettings;
        dbasic::ShaderSet m_shaderSet;
        Shaders m_shaders;

        dbasic::DeltaEngine m_engine;
        dbasic::AssetManager m_assetManager;

        std::string m_assetPath;

        ysRenderTarget *m_mainRenderTarget;
        ysGPUBuffer *m_geometryVertexBuffer;
        ysGPUBuffer *m_geometryIndexBuffer;

        GeometryGenerator m_geometryGenerator;
        dbasic::TextRenderer m_textRenderer;

        std::vector<SimulationObject *> m_objects;
        Engine *m_iceEngine;
        Vehicle *m_vehicle;
        Transmission *m_transmission;
        Simulator *m_simulator;
        double m_dynoSpeed;
        double m_torque;

        UiManager m_uiManager;
        EngineView *m_engineView;
        RightGaugeCluster *m_rightGaugeCluster;
        OscilloscopeCluster *m_oscCluster;
        CylinderTemperatureGauge *m_temperatureGauge;
        PerformanceCluster *m_performanceCluster;
        LoadSimulationCluster *m_loadSimulationCluster;
        MixerCluster *m_mixerCluster;
        InfoCluster *m_infoCluster;
        CustomGaugeCluster* m_customGaugeCluster;
        SimulationObject::ViewParameters m_viewParameters;

        bool m_paused;

        float m_cylinderDifferenceZ;

    protected:
        void startRecording();
        void updateScreenSizeStability();
        bool readyToRecord();
        void stopRecording();
        void recordFrame();
        bool isRecording() const { return m_recording; }

        static constexpr int ScreenResolutionHistoryLength = 5;
        int m_screenResolution[ScreenResolutionHistoryLength][2];
        int m_screenResolutionIndex;
        bool m_recording;

        ysVector m_background;
        ysVector m_foreground;
        ysVector m_shadow;
        ysVector m_highlight1;
        ysVector m_highlight2;

        ysVector m_pink;
        ysVector m_orange;
        ysVector m_yellow;
        ysVector m_red;
        ysVector m_green;
        ysVector m_blue;

        ysAudioBuffer *m_outputAudioBuffer;
        AudioBuffer m_audioBuffer;
        ysAudioSource *m_audioSource;

        int m_oscillatorSampleOffset;

        enum ScreenMode { SCREEN_MODE_DRIVING, SCREEN_MODE_1, SCREEN_MODE_2, SCREEN_MODE_3, SCREEN_MODE_LAST };
        ScreenMode m_screen;

        enum Controller { GAME_CONTROLLER, MOUSE };
        Controller m_controller;

        inih::INIReader m_iniReader;

        Camera m_camera;

        ysVector2 m_dragStartMousePosition;

        float m_scaleModel;

        bool m_debug_mode = false;
        int m_selected_track;
        bool m_show_engine;
        bool m_show_engine2;
        bool m_show_track;
        bool m_show_engine_only = false;
        int m_selected_camera;
        int m_selected_car;
        b2World* m_world;
        VehicleObject* m_vehicle_object;
        ysVector m_previousPosition;
        ysVector m_previousPosition2;

        Joystick j_Controller;

#ifdef ATG_ENGINE_SIM_VIDEO_CAPTURE
        atg_dtv::Encoder m_encoder;
#endif /* ATG_ENGINE_SIM_VIDEO_CAPTURE */
};

#endif /* ATG_ENGINE_SIM_ENGINE_SIM_APPLICATION_H */
