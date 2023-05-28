#include "soh/resource/importer/scenecommand/SetStartPositionListFactory.h"
#include "soh/resource/type/scenecommand/SetStartPositionList.h"
#include "spdlog/spdlog.h"

namespace LUS {
std::shared_ptr<IResource> SetStartPositionListFactory::ReadResource(std::shared_ptr<ResourceInitData> initData,
                                                                    std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetStartPositionList>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion)
    {
    case 0:
	    factory = std::make_shared<SetStartPositionListFactoryV0>();
	    break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load SetStartPositionList with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::SetStartPositionListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<IResource> resource)
{
    std::shared_ptr<SetStartPositionList> setStartPositionList = std::static_pointer_cast<SetStartPositionList>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setStartPositionList);

    ReadCommandId(setStartPositionList, reader);
	
    setStartPositionList->numStartPositions = reader->ReadUInt32();
    setStartPositionList->startPositions.reserve(setStartPositionList->numStartPositions);
    for (uint32_t i = 0; i < setStartPositionList->numStartPositions; i++) {
	ActorEntry entry;

	entry.id = reader->ReadUInt16();
	entry.pos.x = reader->ReadInt16();
	entry.pos.y = reader->ReadInt16();
	entry.pos.z = reader->ReadInt16();
	entry.rot.x = reader->ReadInt16();
	entry.rot.y = reader->ReadInt16();
	entry.rot.z = reader->ReadInt16();
	entry.params = reader->ReadUInt16();

        setStartPositionList->startPositions.push_back(entry);
    }
}

} // namespace LUS
