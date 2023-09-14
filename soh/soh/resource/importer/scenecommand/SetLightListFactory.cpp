#include "soh/resource/importer/scenecommand/SetLightListFactory.h"
#include "soh/resource/type/scenecommand/SetLightList.h"
#include "spdlog/spdlog.h"

namespace LUS {
std::shared_ptr<IResource>
SetLightListFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<SetLightList>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
    case 0:
	factory = std::make_shared<SetLightListFactoryV0>();
	break;
    }

    if (factory == nullptr)
    {
        SPDLOG_ERROR("Failed to load SetLightList with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

void LUS::SetLightListFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        		  std::shared_ptr<IResource> resource)
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

} // namespace LUS
