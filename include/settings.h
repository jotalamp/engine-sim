#ifndef ATG_ENGINE_SIM_SETTINGS_H
#define ATG_ENGINE_SIM_SETTINGS_H

#include <string.h>
#include "ini/ini.h"

struct Settings
{
public:
    Settings();
    auto Get(std::string setting);
    bool depthBuffer = false;
    std::string selectedVehicleName = "NoSelectedVehicleName";
    std::string selectedVehicleModelFileName = "NoSelectedVehicleModelFileName";
    std::vector<std::string> selectedVehicleTextureFileNames;
    std::string trackModelFileName = "NoTrackModelFileName";
    std::vector<std::string> trackMeshes;
    std::vector<std::string> trackTextureFileNames;
    std::vector<std::string> selectedVehicleMeshNames;
    std::vector<std::string> selectedVehicleMaterialNames;
    bool showEngine = false;
    bool showEngine2 = false;
    bool showTrack = false;
    bool showEngineOnly = false;
    int selectedTrack = -1;
    int screenWidth = 640;
    int screenHeight = 480;
    float engineModelScale = 1.0f;
    float cameraZoom = 1.0f;
    float cameraFovY = 50.0f;
    float cylinderDifferenceZ = 1.0f;
    float trackScale = 1.0f;
    float selectedVehicleScale = 1.0f;
    std::vector<float> selectedVehicleModelRotation;
    std::vector<float> selectedVehicleEngineModelRotation;
    std::vector<float> selectedVehicleEngineModelPosition;
    std::vector<std::string> tireMeshes;
    std::vector<std::string> tireMaterials;
    std::vector<float> selectedVehicleTirePositions;
    std::vector<float> tireModelRotation;

private:
    inih::INIReader m_iniReader;
};

#endif