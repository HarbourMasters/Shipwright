#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include "soh/resource/type/Scene.h"
#include "RomFile.h"
#include <libultraship/libultra/types.h>


namespace Ship {

class SetAlternateHeaders : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    uint32_t numHeaders;
    std::vector<std::shared_ptr<Scene>> headers;
};
}; // namespace Ship
