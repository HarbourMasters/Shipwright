#include "soh/resource/importer/scenecommand/SetCameraSettingsFactory.h"
#include "soh/resource/type/scenecommand/SetCameraSettings.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> SetCameraSettingsFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                 std::shared_ptr<LUS::BinaryReader> reader) {
    auto setCameraSettings = std::make_shared<SetCameraSettings>(initData);

    ReadCommandId(setCameraSettings, reader);
	
    setCameraSettings->settings.cameraMovement = reader->ReadInt8();
    setCameraSettings->settings.worldMapArea = reader->ReadInt32();

    return setCameraSettings;
}
} // namespace SOH
