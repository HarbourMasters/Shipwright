#include "2s2h/resource/importer/scenecommand/SetPathwaysFactory.h"
#include "2s2h/resource/type/scenecommand/SetPathways.h"
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>

namespace SOH {
std::shared_ptr<Ship::IResource> SetPathwaysMMFactory::ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                                    std::shared_ptr<Ship::BinaryReader> reader) {
    auto setPathways = std::make_shared<SetPathwaysMM>(initData);

    ReadCommandId(setPathways, reader);

    setPathways->numPaths = reader->ReadUInt32();
    setPathways->paths.reserve(setPathways->numPaths);
    for (uint32_t i = 0; i < setPathways->numPaths; i++) {
        std::string pathFileName = reader->ReadString();
        auto path = std::static_pointer_cast<PathMM>(
            Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourceProcess(pathFileName.c_str()));
        setPathways->paths.push_back(path->GetPointer());
    }
    return setPathways;
}
} // namespace SOH
