#include "soh/resource/importer/scenecommand/SetCutscenesFactory.h"
#include "soh/resource/type/scenecommand/SetCutscenes.h"
#include "soh/resource/logging/SceneCommandLoggers.h"
#include <libultraship/libultraship.h>
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource>
SetCutscenesFactory::ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) {
    auto setCutscenes = std::make_shared<SetCutscenes>(initData);

    ReadCommandId(setCutscenes, reader);
    
    setCutscenes->fileName = reader->ReadString();
    setCutscenes->cutscene = std::static_pointer_cast<Cutscene>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(setCutscenes->fileName.c_str()));

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ResourceLogging"), 0)) {
        LogCutscenesAsXML(setCutscenes);
    }

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
} // namespace SOH
