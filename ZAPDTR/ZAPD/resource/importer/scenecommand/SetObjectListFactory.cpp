#include "resource/importer/SetObjectListFactory.h"
#include "resource/type/SetObjectList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetObjectListFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetObjectList>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetObjectListFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetObjectList with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetObjectListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetObjectList> setObjectList = std::static_pointer_cast<SetObjectList>(resource);
	ResourceFile::ParseFileBinary(reader, text);

	ReadCommandId(SetObjectList, reader);
	
    setObjectList->numObjects = reader->ReadUInt32();
    setObjectList->objects.reserve(setObjectList->numObjects);
    for (uint32_t i = 0; i < setObjectList->numExits; i++) {
        setObjectList->objects.push_back(reader->ReadInt16());
    }
}

} // namespace Ship
