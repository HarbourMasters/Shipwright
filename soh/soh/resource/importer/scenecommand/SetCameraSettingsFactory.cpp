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

    //LogCameraSettingsAsXML(setCameraSettings);

    return setCameraSettings;
}

std::shared_ptr<LUS::IResource> SetCameraSettingsFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setCameraSettings = std::make_shared<SetCameraSettings>(initData);

    setCameraSettings->cmdId = SceneCommandID::SetCameraSettings;

    setCameraSettings->settings.cameraMovement = reader->IntAttribute("CameraMovement");
    setCameraSettings->settings.worldMapArea = reader->IntAttribute("WorldMapArea");

    return setCameraSettings;
}

void LogCameraSettingsAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetCameraSettings> setCameraSettings = std::static_pointer_cast<SetCameraSettings>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetCameraSettings");
    doc.InsertFirstChild(root);

    root->SetAttribute("CameraMovement", setCameraSettings->settings.cameraMovement);
    root->SetAttribute("WorldMapArea", setCameraSettings->settings.worldMapArea);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
