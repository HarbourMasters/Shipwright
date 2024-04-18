#include "soh/resource/importer/scenecommand/SetCutscenesFactory.h"
#include "soh/resource/type/scenecommand/SetCutscenes.h"
#include <libultraship/libultraship.h>
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetCutscenesFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setCutscenes = std::make_shared<SetCutscenes>(initData);

    ReadCommandId(setCutscenes, reader);
    
    setCutscenes->fileName = reader->ReadString();
    setCutscenes->cutscene = std::static_pointer_cast<Cutscene>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(setCutscenes->fileName.c_str()));

    //LogCutscenesAsXML(setCutscenes);

    return setCutscenes;
}

std::shared_ptr<LUS::IResource> SetCutscenesFactoryXML::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData,
                                                                   tinyxml2::XMLElement* reader) {
    auto setCutscenes = std::make_shared<SetCutscenes>(initData);

    setCutscenes->cmdId = SceneCommandID::SetCutscenes;

    setCutscenes->fileName = reader->Attribute("FileName");
    setCutscenes->cutscene = std::static_pointer_cast<Cutscene>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(setCutscenes->fileName.c_str()));

    return setCutscenes;
}

void LogCutscenesAsXML(std::shared_ptr<LUS::IResource> resource) {
    std::shared_ptr<SetCutscenes> setCutscenes = std::static_pointer_cast<SetCutscenes>(resource);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("SetCutscenes");
    doc.InsertFirstChild(root);

    root->SetAttribute("FileName", setCutscenes->fileName.c_str());

    tinyxml2::XMLPrinter printer;
    doc.Accept(&printer);

    SPDLOG_INFO("{}: {}", resource->GetInitData()->Path, printer.CStr());
}
} // namespace SOH
