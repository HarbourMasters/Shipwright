#pragma once
#include <ship/resource/Resource.h>
#include "SceneCommand.h"

namespace SOH {
typedef struct {
    int16_t unk_0;
    int16_t unk_2;
    int16_t unk_4;
    int16_t unk_6;
    int16_t unk_8;
} MinimapChestData;

class SetMinimapChests : public SceneCommand<MinimapChestData> {
  public:
    using SceneCommand::SceneCommand;

    MinimapChestData* GetPointer();
    size_t GetPointerSize();
    std::vector<MinimapChestData> chests;
};

} // namespace SOH
