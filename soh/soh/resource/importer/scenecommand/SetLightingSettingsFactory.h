#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace SOH {
class SetLightingSettingsFactory : public SceneCommandFactoryBinaryV0 {
  public:
    std::shared_ptr<ShipDK::IResource>
    ReadResource(std::shared_ptr<ShipDK::ResourceInitData> initData, std::shared_ptr<ShipDK::BinaryReader> reader) override;
};
} // namespace SOH
