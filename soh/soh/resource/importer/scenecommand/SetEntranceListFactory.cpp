#include "soh/resource/importer/scenecommand/SetEntranceListFactory.h"
#include "soh/resource/type/scenecommand/SetEntranceList.h"
#include "spdlog/spdlog.h"

namespace LUS {
std::shared_ptr<IResource>
SetEntranceListFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetEntranceList>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	factory = std::make_shared<SetEntranceListFactoryV0>();
	break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetEntranceListList with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::SetEntranceListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<IResource> resource) {
    std::shared_ptr<SetEntranceList> setEntranceList = std::static_pointer_cast<SetEntranceList>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setEntranceList);

    ReadCommandId(setEntranceList, reader);
	
    setEntranceList->numEntrances = reader->ReadUInt32();
    setEntranceList->entrances.reserve(setEntranceList->numEntrances);
    for (uint32_t i = 0; i < setEntranceList->numEntrances; i++) {
	EntranceEntry entranceEntry;

	entranceEntry.spawn = reader->ReadInt8();
	entranceEntry.room = reader->ReadInt8();
		
        setEntranceList->entrances.push_back(entranceEntry);
    }
}

} // namespace LUS
