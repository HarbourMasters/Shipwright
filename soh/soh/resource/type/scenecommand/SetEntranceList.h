#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
typedef struct {
    /* 0x00 */ u8 spawn;
    /* 0x01 */ u8 room;
} EntranceEntry;

class SetEntranceList : public SceneCommand<EntranceEntry> {
  public:
    using SceneCommand::SceneCommand;

    EntranceEntry* GetPointer();
    size_t GetPointerSize();

    uint32_t numEntrances;

    std::vector<EntranceEntry> entrances;
};
}; // namespace LUS
