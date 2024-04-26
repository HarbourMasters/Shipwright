#include "soh/resource/importer/scenecommand/SetCutscenesFactory.h"
#include "soh/resource/type/scenecommand/SetCutscenes.h"
#include <libultraship/libultraship.h>
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<ShipDK::IResource>
SetCutscenesFactory::ReadResource(std::shared_ptr<ShipDK::ResourceInitData> initData, std::shared_ptr<ShipDK::BinaryReader> reader) {
    auto setCutscenes = std::make_shared<SetCutscenes>(initData);

    ReadCommandId(setCutscenes, reader);
    
    setCutscenes->fileName = reader->ReadString();
    setCutscenes->cutscene = std::static_pointer_cast<Cutscene>(ShipDK::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(setCutscenes->fileName.c_str()));

    return setCutscenes;
}
} // namespace SOH
