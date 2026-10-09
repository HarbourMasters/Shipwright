
#include "2s2h/resource/type/2shResourceType.h"
#include "2s2h/resource/importer/SceneFactory.h"
#include "2s2h/resource/type/Scene.h"
#include "2s2h/resource/type/scenecommand/SceneCommand.h"
#include "2s2h/resource/importer/scenecommand/SetLightingSettingsFactory.h"
#include "2s2h/resource/importer/scenecommand/SetWindSettingsFactory.h"
#include "2s2h/resource/importer/scenecommand/SetExitListFactory.h"
#include "2s2h/resource/importer/scenecommand/SetTimeSettingsFactory.h"
#include "2s2h/resource/importer/scenecommand/SetSkyboxModifierFactory.h"
#include "2s2h/resource/importer/scenecommand/SetEchoSettingsFactory.h"
#include "2s2h/resource/importer/scenecommand/SetSoundSettingsFactory.h"
#include "2s2h/resource/importer/scenecommand/SetSkyboxSettingsFactory.h"
#include "2s2h/resource/importer/scenecommand/SetRoomBehaviorFactory.h"
#include "2s2h/resource/importer/scenecommand/SetCsCameraFactory.h"
#include "2s2h/resource/importer/scenecommand/SetCameraSettingsFactory.h"
#include "2s2h/resource/importer/scenecommand/SetRoomListFactory.h"
#include "2s2h/resource/importer/scenecommand/SetCollisionHeaderFactory.h"
#include "2s2h/resource/importer/scenecommand/SetEntranceListFactory.h"
#include "2s2h/resource/importer/scenecommand/SetSpecialObjectsFactory.h"
#include "2s2h/resource/importer/scenecommand/SetObjectListFactory.h"
#include "2s2h/resource/importer/scenecommand/SetStartPositionListFactory.h"
#include "2s2h/resource/importer/scenecommand/SetActorListFactory.h"
#include "2s2h/resource/importer/scenecommand/SetTransitionActorListFactory.h"
#include "2s2h/resource/importer/scenecommand/EndMarkerFactory.h"
#include "2s2h/resource/importer/scenecommand/SetAlternateHeadersFactory.h"
#include "2s2h/resource/importer/scenecommand/SetPathwaysFactory.h"
#include "2s2h/resource/importer/scenecommand/SetCutscenesFactory.h"
#include "2s2h/resource/importer/scenecommand/SetLightListFactory.h"
#include "2s2h/resource/importer/scenecommand/SetMeshFactory.h"
#include "2s2h/resource/importer/scenecommand/SetAnimatedMaterialListFactory.h"
#include "2s2h/resource/importer/scenecommand/SetMinimapListFactory.h"
#include "2s2h/resource/importer/scenecommand/SetMinimapChestsFactory.h"
#include "2s2h/resource/importer/scenecommand/SetActorCutsceneListFactory.h"
#include <spdlog/spdlog.h>

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
    sceneCommandFactories[SceneCommandID::SetRoomBehavior] = std::make_shared<SetRoomBehaviorMMFactory>();
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
    // TODO should we use a different custom scene command like cutscenes?
    sceneCommandFactories[SceneCommandID::SetPathways] = std::make_shared<SetPathwaysMMFactory>();
    // sceneCommandFactories[SceneCommandID::SetCutscenes] = std::make_shared<SetCutsceneFactoryMM>();
    sceneCommandFactories[SceneCommandID::SetLightList] = std::make_shared<SetLightListFactory>();
    sceneCommandFactories[SceneCommandID::SetMesh] = std::make_shared<SetMeshFactory>();
    sceneCommandFactories[SceneCommandID::SetCutscenesMM] = std::make_shared<SetCutsceneFactoryMM>();
    sceneCommandFactories[SceneCommandID::SetAnimatedMaterialList] = std::make_shared<SetAnimatedMaterialListFactory>();
    sceneCommandFactories[SceneCommandID::SetMinimapList] = std::make_shared<SetMinimapListFactory>();
    sceneCommandFactories[SceneCommandID::SetMinimapChests] = std::make_shared<SetMinimapChestsFactory>();
    sceneCommandFactories[SceneCommandID::SetActorCutsceneList] = std::make_shared<SetActorCutsceneListFactory>();
}

void ResourceFactoryBinarySceneV0::ParseSceneCommands(std::shared_ptr<Scene> scene,
                                                      std::shared_ptr<Ship::BinaryReader> reader) {
    uint32_t commandCount = reader->ReadUInt32();
    scene->commands.reserve(commandCount);

    for (uint32_t i = 0; i < commandCount; i++) {
        scene->commands.push_back(ParseSceneCommand(scene, reader, i));
    }
}

std::shared_ptr<ISceneCommand>
ResourceFactoryBinarySceneV0::ParseSceneCommand(std::shared_ptr<Scene> scene,
                                                std::shared_ptr<Ship::BinaryReader> reader, uint32_t index) {
    SceneCommandID cmdID = (SceneCommandID)reader->ReadInt32();

    reader->Seek(-sizeof(int32_t), Ship::SeekOffsetType::Current);

    std::shared_ptr<ISceneCommand> result = nullptr;
    auto commandFactory = ResourceFactoryBinarySceneV0::sceneCommandFactories[cmdID];

    if (commandFactory != nullptr) {
        auto initData = std::make_shared<Ship::ResourceInitData>();
        initData->Id = scene->GetInitData()->Id;
        initData->Type = static_cast<uint32_t>(ResourceType::SOH_SceneCommand);
        initData->Path = scene->GetInitData()->Path + "/SceneCommand" + std::to_string(index);
        initData->ResourceVersion = scene->GetInitData()->ResourceVersion;
        result = std::static_pointer_cast<ISceneCommand>(commandFactory->ReadResource(initData, reader));
        // Cache the resource?
    }

    if (result == nullptr) {
        SPDLOG_ERROR("Failed to load scene command of type {} in scene {}", (uint32_t)cmdID,
                     scene->GetInitData()->Path);
    }

    return result;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryBinarySceneV0::ReadResource(std::shared_ptr<Ship::File> file,
                                           std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto scene = std::make_shared<Scene>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    ParseSceneCommands(scene, reader);

    return scene;
};
} // namespace SOH
