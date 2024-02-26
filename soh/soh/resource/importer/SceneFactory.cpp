#include "spdlog/spdlog.h"
#include "soh/resource/type/SohResourceType.h"
#include "soh/resource/importer/SceneFactory.h"
#include "soh/resource/type/Scene.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "soh/resource/importer/scenecommand/SetLightingSettingsFactory.h"
#include "soh/resource/importer/scenecommand/SetWindSettingsFactory.h"
#include "soh/resource/importer/scenecommand/SetExitListFactory.h"
#include "soh/resource/importer/scenecommand/SetTimeSettingsFactory.h"
#include "soh/resource/importer/scenecommand/SetSkyboxModifierFactory.h"
#include "soh/resource/importer/scenecommand/SetEchoSettingsFactory.h"
#include "soh/resource/importer/scenecommand/SetSoundSettingsFactory.h"
#include "soh/resource/importer/scenecommand/SetSkyboxSettingsFactory.h"
#include "soh/resource/importer/scenecommand/SetRoomBehaviorFactory.h"
#include "soh/resource/importer/scenecommand/SetCsCameraFactory.h"
#include "soh/resource/importer/scenecommand/SetCameraSettingsFactory.h"
#include "soh/resource/importer/scenecommand/SetRoomListFactory.h"
#include "soh/resource/importer/scenecommand/SetCollisionHeaderFactory.h"
#include "soh/resource/importer/scenecommand/SetEntranceListFactory.h"
#include "soh/resource/importer/scenecommand/SetSpecialObjectsFactory.h"
#include "soh/resource/importer/scenecommand/SetObjectListFactory.h"
#include "soh/resource/importer/scenecommand/SetStartPositionListFactory.h"
#include "soh/resource/importer/scenecommand/SetActorListFactory.h"
#include "soh/resource/importer/scenecommand/SetTransitionActorListFactory.h"
#include "soh/resource/importer/scenecommand/EndMarkerFactory.h"
#include "soh/resource/importer/scenecommand/SetAlternateHeadersFactory.h"
#include "soh/resource/importer/scenecommand/SetPathwaysFactory.h"
#include "soh/resource/importer/scenecommand/SetCutscenesFactory.h"
#include "soh/resource/importer/scenecommand/SetLightListFactory.h"
#include "soh/resource/importer/scenecommand/SetMeshFactory.h"

namespace SOH {
ResourceFactoryBinarySceneV0::ResourceFactoryBinarySceneV0() {
    sceneCommandFactories[SceneCommandID::SetLightingSettings] = std::make_shared<SetLightingSettingsFactory>();
    sceneCommandFactories[SceneCommandID::SetWind] = std::make_shared<SetWindSettingsFactory>();
    sceneCommandFactories[SceneCommandID::SetExitList] = std::make_shared<SetExitListFactory>();
    sceneCommandFactories[SceneCommandID::SetTimeSettings] = std::make_shared<SetTimeSettingsFactory>();
    sceneCommandFactories[SceneCommandID::SetSkyboxModifier] = std::make_shared<SetSkyboxModifierFactory>();
    sceneCommandFactories[SceneCommandID::SetEchoSettings] = std::make_shared<SetEchoSettingsFactory>();
    sceneCommandFactories[SceneCommandID::SetSoundSettings] = std::make_shared<SetSoundSettingsFactory>();
    sceneCommandFactories[SceneCommandID::SetSkyboxSettings] = std::make_shared<SetSkyboxSettingsFactory>();
    sceneCommandFactories[SceneCommandID::SetRoomBehavior] = std::make_shared<SetRoomBehaviorFactory>();
    sceneCommandFactories[SceneCommandID::SetCsCamera] = std::make_shared<SetCsCameraFactory>();
    sceneCommandFactories[SceneCommandID::SetCameraSettings] = std::make_shared<SetCameraSettingsFactory>();
    sceneCommandFactories[SceneCommandID::SetRoomList] = std::make_shared<SetRoomListFactory>();
    sceneCommandFactories[SceneCommandID::SetCollisionHeader] = std::make_shared<SetCollisionHeaderFactory>();
    sceneCommandFactories[SceneCommandID::SetEntranceList] = std::make_shared<SetEntranceListFactory>();
    sceneCommandFactories[SceneCommandID::SetSpecialObjects] = std::make_shared<SetSpecialObjectsFactory>();
    sceneCommandFactories[SceneCommandID::SetObjectList] = std::make_shared<SetObjectListFactory>();
    sceneCommandFactories[SceneCommandID::SetStartPositionList] = std::make_shared<SetStartPositionListFactory>();
    sceneCommandFactories[SceneCommandID::SetActorList] = std::make_shared<SetActorListFactory>();
    sceneCommandFactories[SceneCommandID::SetTransitionActorList] = std::make_shared<SetTransitionActorListFactory>();
    sceneCommandFactories[SceneCommandID::EndMarker] = std::make_shared<EndMarkerFactory>();
    sceneCommandFactories[SceneCommandID::SetAlternateHeaders] = std::make_shared<SetAlternateHeadersFactory>();
    sceneCommandFactories[SceneCommandID::SetPathways] = std::make_shared<SetPathwaysFactory>();
    sceneCommandFactories[SceneCommandID::SetCutscenes] = std::make_shared<SetCutscenesFactory>();
    sceneCommandFactories[SceneCommandID::SetLightList] = std::make_shared<SetLightListFactory>();
    sceneCommandFactories[SceneCommandID::SetMesh] = std::make_shared<SetMeshFactory>();
}

void ResourceFactoryBinarySceneV0::ParseSceneCommands(std::shared_ptr<Scene> scene, std::shared_ptr<LUS::BinaryReader> reader) {
    uint32_t commandCount = reader->ReadUInt32();
    scene->commands.reserve(commandCount);

    for (uint32_t i = 0; i < commandCount; i++) {
        scene->commands.push_back(ParseSceneCommand(scene, reader, i));
    }
}

std::shared_ptr<ISceneCommand> ResourceFactoryBinarySceneV0::ParseSceneCommand(std::shared_ptr<Scene> scene,
                                                                std::shared_ptr<LUS::BinaryReader> reader, uint32_t index) {
    SceneCommandID cmdID = (SceneCommandID)reader->ReadInt32();

    reader->Seek(-sizeof(int32_t), LUS::SeekOffsetType::Current);

    std::shared_ptr<ISceneCommand> result = nullptr;
    auto commandFactory = ResourceFactoryBinarySceneV0::sceneCommandFactories[cmdID];

    if (commandFactory != nullptr) {
        auto initData = std::make_shared<LUS::ResourceInitData>();
        initData->Id = scene->GetInitData()->Id;
        initData->Type = static_cast<uint32_t>(SOH::ResourceType::SOH_SceneCommand);
        initData->Path = scene->GetInitData()->Path + "/SceneCommand" + std::to_string(index);
        initData->ResourceVersion = scene->GetInitData()->ResourceVersion;
        result = std::static_pointer_cast<ISceneCommand>(commandFactory->ReadResource(initData, reader));
        // Cache the resource?
    }

    if (result == nullptr) {
        SPDLOG_ERROR("Failed to load scene command of type {} in scene {}", (uint32_t)cmdID, scene->GetInitData()->Path);
    }

    return result;
}

std::shared_ptr<LUS::IResource> ResourceFactoryBinarySceneV0::ReadResource(std::shared_ptr<LUS::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto scene = std::make_shared<Scene>(file->InitData);
    auto reader = std::get<std::shared_ptr<LUS::BinaryReader>>(file->Reader);

    ParseSceneCommands(scene, reader);

    return scene;
};
} // namespace SOH
