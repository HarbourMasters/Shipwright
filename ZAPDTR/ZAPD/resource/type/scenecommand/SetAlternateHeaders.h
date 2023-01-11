#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include "Scene.h"
#include "RomFile.h"
#include <libultraship/libultra/types.h>


namespace Ship {
// typedef struct {
//     /* 0x00 */ uintptr_t vromStart;
//     /* 0x04 */ uintptr_t vromEnd;
//     char* fileName;
// } RomFile; // size = 0x8

class SetAlternateHeader : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    uint32_t numHeaders;
    std::vector<std::shared_ptr<Scene>> headers;
};
}; // namespace Ship
