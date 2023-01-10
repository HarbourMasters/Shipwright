#include "resource/importer/SceneFactory.h"
#include "resource/type/Scene.h"
#include "resource/type/scenecommand/SceneCommand.h"
#include "spdlog/spdlog.h"
#include "resource/importer/SetLightingSettingsFactory.h"
#include "resource/importer/SetWindFactory.h"
#include "resource/importer/SetExitListFactory.h"
#include "resource/importer/SetTimeSettingsFactory.h"
#include "resource/importer/SetSkyboxSettingsFactory.h"
#include "resource/importer/SetEchoSettingsFactory.h"
#include "resource/importer/SetSoundSettingsFactory.h"

namespace Ship {
std::shared_ptr<Resource> SceneFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
    if (SceneFactory::sceneCommandFactories.empty()) {
        SceneFactory::sceneCommandFactories[cmdID] = std::make_shared<SetLightingSettingsFactory>();
        SceneFactory::sceneCommandFactories[cmdID] = std::make_shared<SetWindFactory>();
        SceneFactory::sceneCommandFactories[cmdID] = std::make_shared<SetExitListFactory>();
        SceneFactory::sceneCommandFactories[cmdID] = std::make_shared<SetTimeSettingsFactory>();
        SceneFactory::sceneCommandFactories[cmdID] = std::make_shared<SetSkyboxSettingsFactory>();
        SceneFactory::sceneCommandFactories[cmdID] = std::make_shared<SetEchoSettingsFactory>();
        SceneFactory::sceneCommandFactories[cmdID] = std::make_shared<SetSoundSettingsFactory>();
        // TODO: Do the rest...
    }

	auto resource = std::make_shared<Scene>();
	std::shared_ptr<ResourceVersionFactory> factory = nullptr;

	uint32_t version = reader->ReadUInt32();
	switch (version)
	{
	case 0:
		factory = std::make_shared<SceneFactoryV0>();
		break;
	}

	if (factory == nullptr)
	{
		SPDLOG_ERROR("Failed to load Scene with version {}", version);
		return nullptr;
	}

	factory->ParseFileBinary(reader, resource);

	return resource;
}

void Ship::SceneFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<Resource> resource)
{
	std::shared_ptr<Scene> scene = std::static_pointer_cast<Scene>(resource);
	ResourceFile::ParseFileBinary(reader, text);

	uint32_t commandCount = reader->ReadUInt32();
	scene->commands.reserve(commandCount);

	for (uint32_t i = 0; i < commandCount; i++) {
		scene->commands.push_back(ParseSceneCommand(reader));
	}
}

std::shared_ptr<SceneCommand> Ship::ParseSceneCommand(std::shared_ptr<BinaryReader> reader) {
    SceneCommandID cmdID = (SceneCommandID)reader->ReadInt32();

    reader->Seek(-sizeof(int32_t), SeekOffsetType::Current);

    std::shared_ptr<SceneCommand> result = nullptr;
    auto commandFactory = SceneFactory::sceneCommandFactories[cmdID];

    if (commandFactory != nullptr) {
        commandFactory->ReadResource(reader);
    }

    if (result == nullptr) {
        SPDLOG_ERROR("Failed to load scene command of type {}", (uint32_t)cmdID);
    }

    return result;
}

} // namespace Ship
