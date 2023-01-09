#pragma once

#include <memory>
#include "Resource.h"
#include "ResourceFactory.h"
#include "resource/type/SceneCommand.h"

namespace Ship {
class SceneCommandFactory : public ResourceFactory {};

class SceneCommandVersionFactory : public ResourceVersionFactory {
protected:
    void ReadCommandId(std::shared_ptr<SceneCommand> command, std::shared_ptr<BinaryReader> reader);
};
}; // namespace Ship
