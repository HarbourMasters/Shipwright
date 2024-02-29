#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "soh/resource/type/CollisionHeader.h"
// #include <libultraship/libultra/types.h>

namespace SOH {
class SetCollisionHeader : public SceneCommand<CollisionHeaderData> {
  public:
    using SceneCommand::SceneCommand;

    CollisionHeaderData* GetPointer();
    size_t GetPointerSize();

    std::string fileName;

    std::shared_ptr<CollisionHeader> collisionHeader;
};
}; // namespace LUS
