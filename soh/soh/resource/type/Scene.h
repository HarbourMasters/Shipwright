#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "scenecommand/SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {

class Scene : public Resource {
public:
  using Resource::Resource;

  void* GetPointer();
  size_t GetPointerSize();

  std::vector<std::shared_ptr<SceneCommand>> commands;
};
}; // namespace Ship
