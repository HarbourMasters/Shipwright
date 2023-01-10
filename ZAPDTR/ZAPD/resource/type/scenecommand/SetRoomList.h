#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

typedef struct {
    /* 0x00 */ uintptr_t vromStart;
    /* 0x04 */ uintptr_t vromEnd;
    char* fileName;
} RomFile; // size = 0x8

namespace Ship {
class SetExitList : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    uint32_t numRooms;

    std::vector<std::string> fileNames;
    std::vector<RomFile> rooms;
};
}; // namespace Ship
