#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace SOH {
class SetRoomBehaviorFactory : public SceneCommandFactoryBinaryV0 {
  public:
    std::shared_ptr<Ship::IResource>
    ReadResource(std::shared_ptr<Ship::ResourceInitData> initData, std::shared_ptr<Ship::BinaryReader> reader) override;
};
} // namespace SOH
