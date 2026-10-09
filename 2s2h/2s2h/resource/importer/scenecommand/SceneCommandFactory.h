#pragma once

#include <memory>
#include <ship/resource/Resource.h>
#include <ship/resource/ResourceFactory.h>
#include "2s2h/resource/type/scenecommand/SceneCommand.h"

namespace SOH {
class SceneCommandFactoryBinaryV0 {
  public:
    virtual std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                          std::shared_ptr<Ship::BinaryReader> reader) = 0;

  protected:
    void ReadCommandId(std::shared_ptr<ISceneCommand> command, std::shared_ptr<Ship::BinaryReader> reader);
};
} // namespace SOH
