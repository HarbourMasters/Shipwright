#pragma once

#include "2s2h/resource/importer/scenecommand/SceneCommandFactory.h"

namespace SOH {
class SetActorCutsceneListFactory : public SceneCommandFactoryBinaryV0 {
    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::ResourceInitData> initData,
                                                  std::shared_ptr<Ship::BinaryReader> reader) override;
};
} // namespace SOH
