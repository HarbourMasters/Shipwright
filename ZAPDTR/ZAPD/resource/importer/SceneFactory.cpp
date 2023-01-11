#include "resource/importer/SceneFactory.h"
#include "resource/type/Scene.h"
#include "resource/type/scenecommand/SceneCommand.h"
#include "spdlog/spdlog.h"
#include "resource/importer/SetLightingSettingsFactory.h"
#include "resource/importer/SetWindFactory.h"
#include "resource/importer/SetExitListFactory.h"
#include "resource/importer/SetTimeSettingsFactory.h"
#include "resource/importer/SetSkyboxModifierFactory.h"
#include "resource/importer/SetEchoSettingsFactory.h"
#include "resource/importer/SetSoundSettingsFactory.h"
#include "resource/importer/SetSkyboxSettingsFactory.h"
#include "resource/importer/SetRoomBehaviorFactory.h"
#include "resource/importer/SetCsCameraFactory.h"
#include "resource/importer/SetCameraSettingsFactory.h"
#include "resource/importer/SetRoomListFactory.h"
#include "resource/importer/SetCollisionHeaderFactory.h"
#include "resource/importer/SetEntranceListFactory.h"
#include "resource/importer/SetSpecialObjectsFactory.h"

namespace Ship {
std::shared_ptr<Resource> SceneFactory::ReadResource(std::shared_ptr<BinaryReader> reader)
{
    if (SceneFactory::sceneCommandFactories.empty()) {
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetLightingSettings] = std::make_shared<SetLightingSettingsFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetWind] = std::make_shared<SetWindFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetExitList] = std::make_shared<SetExitListFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetTimeSettings] = std::make_shared<SetTimeSettingsFactory>();
		SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetSkyboxModifier] = std::make_shared<SetSkyboxModifierFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetEchoSettings] = std::make_shared<SetEchoSettingsFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetSoundSettings] = std::make_shared<SetSoundSettingsFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetSkyboxSettings] = std::make_shared<SetSkyboxSettingsFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetRoomBehavior] = std::make_shared<SetRoomBehaviorFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetCsCamera] = std::make_shared<SetCsCameraFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetCameraSettings] = std::make_shared<SetCameraSettingsFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetRoomList] = std::make_shared<SetRoomListFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetCollisionHeader] = std::make_shared<SetCollisionHeaderFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetEntranceList] = std::make_shared<SetEntranceListFactory>();
        SceneFactory::sceneCommandFactories[Ship::SceneCommandID::SetSpecialObjects] = std::make_shared<SetSpecialObjectsFactory>();
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
