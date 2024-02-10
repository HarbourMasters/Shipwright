#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace SOH {
class SetTimeSettingsFactory : public SceneCommandFactoryBinaryV0 {
  public:
    std::shared_ptr<LUS::IResource>
    ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) override;
};
} // namespace SOH
