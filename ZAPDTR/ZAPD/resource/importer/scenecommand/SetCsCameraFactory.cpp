#include "resource/importer/SetCsCameraFactory.h"
#include "resource/type/SetCsCamera.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetCsCameraFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetCsCamera>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SetCsCameraFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetCsCamera with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetCsCameraFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetCsCamera> SetCsCamera = std::static_pointer_cast<SetCsCamera>(resource);
	ResourceFile::ParseFileBinary(reader, text);

	ReadCommandId(SetCsCamera, reader);
	
    reader->ReadInt8();  // camSize
    reader->ReadInt32(); // segOffset

    // OTRTODO: FINISH!
}

} // namespace Ship
