#include "2s2h/resource/importer/scenecommand/SetCameraSettingsFactory.h"
#include "2s2h/resource/type/scenecommand/SetCameraSettings.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
SetCameraSettingsFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                       std::shared_ptr<Ship::BinaryReader> reader) {
    auto setCameraSettings = std::make_shared<SetCameraSettings>(initData);

    ReadCommandId(setCameraSettings, reader);

    return setCameraSettings;
}
} // namespace SOH
