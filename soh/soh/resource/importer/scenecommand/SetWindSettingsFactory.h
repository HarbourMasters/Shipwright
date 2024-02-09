#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"


class SetWindSettingsFactory : public SceneCommandFactory {
  public:
    std::shared_ptr<LUS::IResource>
    ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) override;
};

class SetWindSettingsFactoryV0 : public SceneCommandVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<LUS::IResource> resource) override;
};
}; // namespace LUS
