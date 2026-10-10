#pragma once

#include <memory>
#include <string>
#include "2s2h/resource/type/scenecommand/SceneCommand.h"
#include "2s2h/resource/type/CollisionHeader.h"

namespace SOH {
class SetCollisionHeader : public SceneCommand<CollisionHeaderData> {
  public:
    using SceneCommand::SceneCommand;

    CollisionHeaderData* GetPointer();
    size_t GetPointerSize();

    std::string fileName;

    std::shared_ptr<CollisionHeader> collisionHeader;
};
}; // namespace SOH
