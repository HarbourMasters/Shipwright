#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "SceneCommand.h"
#include "RomFile.h"

namespace SOH {
class SetRoomList : public SceneCommand<RomFile> {
  public:
    using SceneCommand::SceneCommand;

    RomFile* GetPointer();
    size_t GetPointerSize();

    uint32_t numRooms;

    std::vector<std::string> fileNames;
    std::vector<RomFile> rooms;
};
}; // namespace SOH
