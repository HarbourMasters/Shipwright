#include "soh/resource/importer/scenecommand/SetExitListFactory.h"
#include "soh/resource/type/scenecommand/SetExitList.h"
#include "spdlog/spdlog.h"

namespace LUS {
std::shared_ptr<IResource>
SetExitListFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetExitList>( initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SetExitListFactoryV0>();
	    break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetExitList with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::SetExitListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<IResource> resource) {
    std::shared_ptr<SetExitList> setExitList = std::static_pointer_cast<SetExitList>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setExitList);

    ReadCommandId(setExitList, reader);
	
    setExitList->numExits = reader->ReadUInt32();
    setExitList->exits.reserve(setExitList->numExits);
    for (uint32_t i = 0; i < setExitList->numExits; i++) {
        setExitList->exits.push_back(reader->ReadUInt16());
    }
}

} // namespace LUS
