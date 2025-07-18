#include "../include/ini/ini.h"
#include "../include/settings.h"

Settings::Settings()
{
    m_iniReader = inih::INIReader{"../settings.ini"};

    std::string section = "Rendering";
    depthBuffer = m_iniReader.Get<bool>(section, "DepthBuffer");
    screenWidth = m_iniReader.Get<int>(section, "ScreenWidth");
    screenHeight = m_iniReader.Get<int>(section, "ScreenHeight");

    section = "Settings";
    selectedTrack = m_iniReader.Get<int>(section, "SelectedTrack");
    showEngine = m_iniReader.Get<bool>(section, "ShowEngine");
    showEngine2 = m_iniReader.Get<bool>(section, "ShowEngine2");
    showTrack = m_iniReader.Get<bool>(section, "ShowTrack");
    showEngineOnly = m_iniReader.Get<bool>(section, "ShowEngineOnly");

    section = "Engine";
    engineModelScale = m_iniReader.Get<float>(section, "ModelScale");
    cylinderDifferenceZ = m_iniReader.Get<float>(section, "CylinderDifferenceZ");

    section = "Camera";
    cameraZoom = m_iniReader.Get<float>(section, "Zoom");
    cameraFovY = m_iniReader.Get<float>(section, "FovY");

    section = "Track";
    trackModelFileName = m_iniReader.Get<std::string>(section, "ModelFileName");
    trackMeshes = m_iniReader.GetVector<std::string>(section, "Meshes");
    trackTextureFileNames = m_iniReader.GetVector<std::string>(section, "TextureFileNames");
    trackScale = m_iniReader.Get<float>(section, "Scale");

    section = "SelectedVehicle";
    selectedVehicleName = m_iniReader.Get<std::string>(section, "Name");

    section = selectedVehicleName;
    selectedVehicleModelFileName = m_iniReader.Get<std::string>(section, "ModelFileName");
    selectedVehicleScale = m_iniReader.Get<float>(section, "Scale");
    selectedVehicleModelRotation = m_iniReader.GetVector<float>(section, "ModelRotation");
    selectedVehicleEngineModelRotation = m_iniReader.GetVector<float>(section, "EngineModelRotation");
    selectedVehicleEngineModelPosition = m_iniReader.GetVector<float>(section, "EngineModelPosition");
    selectedVehicleTextureFileNames = m_iniReader.GetVector<std::string>(section, "TextureFileNames");
    selectedVehicleMeshNames = m_iniReader.GetVector<std::string>(section, "MeshNames");
    selectedVehicleMaterialNames = m_iniReader.GetVector<std::string>(section, "MaterialNames");
    selectedVehicleTirePositions = m_iniReader.GetVector<float>(section, "TirePositions");
    tireMeshes = m_iniReader.GetVector<std::string>(section, "TireMeshes");
    tireMaterials = m_iniReader.GetVector<std::string>(section, "TireMaterials");
    tireModelRotation = m_iniReader.GetVector<float>(section, "TireModelRotation");
}

auto Settings::Get(std::string setting)
{

}
