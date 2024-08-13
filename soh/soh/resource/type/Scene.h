#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "scenecommand/SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {

class Scene : public Ship::Resource<void> {
public:
  using Resource::Resource;

    Scene() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {}

    void* GetPointer();
  size_t GetPointerSize();

  std::vector<std::shared_ptr<ISceneCommand>> commands;
};
}; // namespace LUS
