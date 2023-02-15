#include "soh/resource/importer/scenecommand/SetCutscenesFactory.h"
#include "soh/resource/type/scenecommand/SetCutscenes.h"
#include <libultraship/bridge.h>
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> SetCutscenesFactory::ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader)
{
	auto resource = std::make_shared<SetCutscenes>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	switch (version)
	{
	case 0:
		factory = std::make_shared<SetCutscenesFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load SetCutscenes with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SetCutscenesFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        		  std::shared_ptr<Resource> resource)
{
	std::shared_ptr<SetCutscenes> setCutscenes = std::static_pointer_cast<SetCutscenes>(resource);
	ResourceVersionFactory::ParseFileBinary(reader, setCutscenes);

	ReadCommandId(setCutscenes, reader);
	
	setCutscenes->fileName = reader->ReadString();
	setCutscenes->cutscene = std::static_pointer_cast<Cutscene>(LoadResource(setCutscenes->fileName.c_str(), true));
}

} // namespace Ship
