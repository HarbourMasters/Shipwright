#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"


class SetSkyboxSettingsFactory : public SceneCommandFactory {
  public:
    std::shared_ptr<LUS::IResource>
    ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) override;
};

class SetSkyboxSettingsFactoryV0 : public SceneCommandVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<LUS::IResource> resource) override;
};
}; // namespace LUS
