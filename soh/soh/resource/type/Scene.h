#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "scenecommand/SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace LUS {

class Scene : public Resource<void> {
public:
  using Resource::Resource;

    Scene() : Resource(std::shared_ptr<ResourceInitData>()) {}

    void* GetPointer();
  size_t GetPointerSize();

  std::vector<std::shared_ptr<ISceneCommand>> commands;
};
}; // namespace LUS
