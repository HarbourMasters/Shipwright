#include "soh/resource/importer/scenecommand/SetCameraSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetCameraSettings.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource> SetCameraSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                 std::shared_ptr<Ship::BinaryReader> reader) {
    auto setCameraSettings = std::make_shared<SetCameraSettings>(initData);

    ReadCommandId(setCameraSettings, reader);
	
    setCameraSettings->settings.cameraMovement = reader->ReadInt8();
    setCameraSettings->settings.worldMapArea = reader->ReadInt32();

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogCameraSettingsAsXML(setCameraSettings);
    }

    return setCameraSettings;
}

std::shared_ptr<Ship::IResource> SetCameraSettingsFactoryXML::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setCameraSettings = std::make_shared<SetCameraSettings>(initData);

    setCameraSettings->cmdId = SceneCommandID::SetCameraSettings;

    setCameraSettings->settings.cameraMovement = reader->IntAttribute("CameraMovement");
    setCameraSettings->settings.worldMapArea = reader->IntAttribute("WorldMapArea");

    return setCameraSettings;
}
} // namespace SOH
