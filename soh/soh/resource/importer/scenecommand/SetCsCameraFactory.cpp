#include "soh/resource/importer/scenecommand/SetCsCameraFactory.h"
#include "soh/resource/type/scenecommand/SetCsCamera.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetCsCameraFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setCsCamera = std::make_shared<SetCsCamera>(initData);

    ReadCommandId(setCsCamera, reader);
	
    reader->ReadInt8();  // camSize
    reader->ReadInt32(); // segOffset

    // OTRTODO: FINISH!

    //LogCsCameraAsXML(setCsCamera);

    return setCsCamera;
}

std::shared_ptr<LUS::IResource> SetCsCameraFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setCsCamera = std::make_shared<SetCsCamera>(initData);

    setCsCamera->cmdId = SceneCommandID::SetCsCamera;

    return setCsCamera;
}

void LogCsCameraAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetCsCamera> setCsCamera = std::static_pointer_cast<SetCsCamera>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetCsCamera");
    doc.InsertFirstChild(root);

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
