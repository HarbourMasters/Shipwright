#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace LUS {
class SetActorListFactory : public SceneCommandFactory {
  public:
    std::shared_ptr<Resource>
    ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) override;
};

class SetActorListFactoryV0 : public SceneCommandVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace LUS
