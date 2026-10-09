#pragma once

#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
typedef struct {
    /* 0x00 */ u16 mapId;
    /* 0x02 */ s16 unk2;
    /* 0x04 */ s16 unk4;
    /* 0x06 */ s16 unk6;
    /* 0x08 */ u16 unk8; // flags; 1 = mirror x? 2 = mirror y?
} MinimapEntryData;      // size = 0xA

typedef struct {
    /* 0x00 */ MinimapEntryData* entry;
    /* 0x04 */ s16 scale;
} MinimapListData; // size  = 0x8

class SetMinimapList : public SceneCommand<MinimapListData> {
  public:
    using SceneCommand::SceneCommand;
    MinimapListData* GetPointer();
    size_t GetPointerSize();

    MinimapListData list;
    std::vector<MinimapEntryData> entries;
};

} // namespace SOH
