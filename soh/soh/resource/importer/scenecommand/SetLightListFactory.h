#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace Ship {
class SetLightListFactory : public SceneCommandFactory {
  public:
    std::shared_ptr<Resource> ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                           std::shared_ptr<ResourceInitData> initData,
                                           std::shared_ptr<BinaryReader> reader) override;
};

class SetLightListFactoryV0 : public SceneCommandVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace Ship
