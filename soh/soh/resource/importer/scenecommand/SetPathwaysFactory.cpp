#include "soh/resource/importer/scenecommand/SetPathwaysFactory.h"
#include "soh/resource/type/scenecommand/SetPathways.h"
#include "spdlog/spdlog.h"
#include <libultraship/libultraship.h>

namespace LUS {
std::shared_ptr<IResource>
SetPathwaysFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetPathways>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SetPathwaysFactoryV0>();
	    break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetPathways with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::SetPathwaysFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                                 std::shared_ptr<IResource> resource) {
    std::shared_ptr<SetPathways> setPathways = std::static_pointer_cast<SetPathways>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setPathways);

    ReadCommandId(setPathways, reader);
	
    setPathways->numPaths = reader->ReadUInt32();
    setPathways->paths.reserve(setPathways->numPaths);
    for (uint32_t i = 0; i < setPathways->numPaths; i++) {
        std::string pathFileName = reader->ReadString();
        auto path = std::static_pointer_cast<Path>(LUS::Context::GetInstance()->GetResourceManager()->LoadResourceProcess(pathFileName.c_str()));
        setPathways->paths.push_back(path->GetPointer());
    }
}

} // namespace LUS
