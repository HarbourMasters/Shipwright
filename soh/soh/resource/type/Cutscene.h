#pragma once

#include <vector>
#include <string>
#include "Resource.h"

namespace SOH {

enum class CutsceneCommands {
    Cmd00 = 0x0000,
    SetCameraPos = 0x0001,
    SetCameraFocus = 0x0002,
    SpecialAction = 0x0003,
    SetLighting = 0x0004,
    SetCameraPosLink = 0x0005,
    SetCameraFocusLink = 0x0006,
    Cmd07 = 0x0007,
    Cmd08 = 0x0008,
    Cmd09 = 0x0009,
    Unknown = 0x001A,
    Textbox = 0x0013,
    SetActorAction0 = 0x000A,
    SetActorAction1 = 0x000F,
    SetActorAction2 = 0x000E,
    SetActorAction3 = 0x0019,
    SetActorAction4 = 0x001D,
    SetActorAction5 = 0x001E,
    SetActorAction6 = 0x002C,
    SetActorAction7 = 0x001F,
    SetActorAction8 = 0x0031,
    SetActorAction9 = 0x003E,
    SetActorAction10 = 0x008F,
    SetSceneTransFX = 0x002D,
    Nop = 0x000B,
    PlayBGM = 0x0056,
    StopBGM = 0x0057,
    FadeBGM = 0x007C,
    SetTime = 0x008C,
    Terminator = 0x03E8,
    End = 0xFFFF,
    Error = 0xFEAF,
};

class Cutscene : public Ship::Resource<uint32_t> {
  public:
    using Resource::Resource;

    Cutscene() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {}

    uint32_t* GetPointer();
    size_t GetPointerSize();

    uint32_t numCommands;
    uint32_t endFrame;
    std::vector<uint32_t> commands;
};
} // namespace SOH


/////////////

// class CutsceneCommand {
//   public:
//     uint32_t commandID;
//     uint32_t commandIndex;

//     CutsceneCommand(){};
// };
