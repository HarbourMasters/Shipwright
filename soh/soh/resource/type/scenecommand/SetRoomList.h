#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include "RomFile.h"
#include <libultraship/libultra/types.h>


namespace SOH {
// typedef struct {
//     /* 0x00 */ uintptr_t vromStart;
//     /* 0x04 */ uintptr_t vromEnd;
//     char* fileName;
// } RomFile; // size = 0x8

class SetRoomList : public SceneCommand<RomFile> {
  public:
    using SceneCommand::SceneCommand;

    RomFile* GetPointer();
    size_t GetPointerSize();

    uint32_t numRooms;

    std::vector<std::string> fileNames;
    std::vector<RomFile> rooms;
};
}; // namespace LUS
