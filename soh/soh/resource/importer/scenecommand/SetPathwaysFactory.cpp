#include "soh/resource/importer/scenecommand/SetPathwaysFactory.h"
#include "soh/resource/type/scenecommand/SetPathways.h"
#include "spdlog/spdlog.h"
#include <libultraship/libultraship.h>

namespace SOH {
std::shared_ptr<ShipDK::IResource>
SetPathwaysFactory::ReadResource(std::shared_ptr<ShipDK::ResourceInitData> initData, std::shared_ptr<ShipDK::BinaryReader> reader) {
    auto setPathways = std::make_shared<SetPathways>(initData);

    ReadCommandId(setPathways, reader);
	
    setPathways->numPaths = reader->ReadUInt32();
    setPathways->paths.reserve(setPathways->numPaths);
    for (uint32_t i = 0; i < setPathways->numPaths; i++) {
        std::string pathFileName = reader->ReadString();
        auto path = std::static_pointer_cast<Path>(ShipDK::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(pathFileName.c_str()));
        setPathways->paths.push_back(path->GetPointer());
    }

    return setPathways;
}
} // namespace SOH
