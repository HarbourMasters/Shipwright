#include "resource/importer/SetLightListFactory.h"
#include "resource/type/SetLightList.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetLightListFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetActorList>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetActorListFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetLightList with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetLightListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        		  std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetLightList> setLightList = std::static_pointer_cast<SetLightList>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setLightList);

	ReadCommandId(setLightList, reader);
	
   	setLightList->numLights = reader->ReadUInt32();
    setLightList->lightList.reserve(setLightList->numLights);
    for (uint32_t i = 0; i < setLightList->numLights; i++) {
        LightInfo light;

        light.type = reader->ReadUByte();
		
		light.params.point.x = reader->ReadInt16();
        light.params.point.y = reader->ReadInt16();
        light.params.point.z = reader->ReadInt16();

        light.params.point.color[0] = reader->ReadUByte(); // r
        light.params.point.color[1] = reader->ReadUByte(); // g
        light.params.point.color[2] = reader->ReadUByte(); // b

        light.params.point.drawGlow = reader->ReadUByte();
		light.params.point.radius = reader->ReadInt16();

        setLightList->lightList.push_back(light);
    }
}

} // namespace Ship
