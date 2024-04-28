#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include <libultraship/libultra/types.h>

namespace SOH {

enum class SceneCommandID : uint8_t {
    SetStartPositionList = 0x00,
    SetActorList = 0x01,
    SetCsCamera = 0x02,
    SetCollisionHeader = 0x03,
    SetRoomList = 0x04,
    SetWind = 0x05,
    SetEntranceList = 0x06,
    SetSpecialObjects = 0x07,
    SetRoomBehavior = 0x08,
    Unused09 = 0x09,
    SetMesh = 0x0A,
    SetObjectList = 0x0B,
    SetLightList = 0x0C,
    SetPathways = 0x0D,
    SetTransitionActorList = 0x0E,
    SetLightingSettings = 0x0F,
    SetTimeSettings = 0x10,
    SetSkyboxSettings = 0x11,
    SetSkyboxModifier = 0x12,
    SetExitList = 0x13,
    EndMarker = 0x14,
    SetSoundSettings = 0x15,
    SetEchoSettings = 0x16,
    SetCutscenes = 0x17,
    SetAlternateHeaders = 0x18,
    SetCameraSettings = 0x19,

    // MM Commands
    SetWorldMapVisited = 0x19,
    SetAnimatedMaterialList = 0x1A,
    SetActorCutsceneList = 0x1B,
    SetMinimapList = 0x1C,
    Unused1D = 0x1D,
    SetMinimapChests = 0x1E,

    Error = 0xFF
};

class ISceneCommand : public Ship::IResource {
public:
    using IResource::IResource;
    ISceneCommand() : IResource(std::shared_ptr<Ship::ResourceInitData>()) {}
    SceneCommandID cmdId;
};

template <class T> class SceneCommand : public ISceneCommand {
  public:
    using ISceneCommand::ISceneCommand;
    virtual T* GetPointer() = 0;
    void* GetRawPointer() override {
        return static_cast<void*>(GetPointer());
    }
};

}; // namespace LUS
