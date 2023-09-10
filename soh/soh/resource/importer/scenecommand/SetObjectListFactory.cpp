#include "soh/resource/importer/scenecommand/SetObjectListFactory.h"
#include "soh/resource/type/scenecommand/SetObjectList.h"
#include "spdlog/spdlog.h"

namespace LUS {
std::shared_ptr<IResource>
SetObjectListFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetObjectList>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	    factory = std::make_shared<SetObjectListFactoryV0>();
	    break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load SetObjectList with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::SetObjectListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<IResource> resource)
{
    std::shared_ptr<SetObjectList> setObjectList = std::static_pointer_cast<SetObjectList>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, setObjectList);

    ReadCommandId(setObjectList, reader);
	
    setObjectList->numObjects = reader->ReadUInt32();
    setObjectList->objects.reserve(setObjectList->numObjects);
    for (uint32_t i = 0; i < setObjectList->numObjects; i++) {
        setObjectList->objects.push_back(reader->ReadUInt16());
    }
}

} // namespace LUS
