#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"


class SetRoomListFactory : public SceneCommandFactoryBinaryV0 {
  public:
    std::shared_ptr<LUS::IResource>
    ReadResource(std::shared_ptr<LUS::ResourceInitData> initData, std::shared_ptr<LUS::BinaryReader> reader) override;
};

class SetRoomListFactoryV0 : public SceneCommandVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<LUS::BinaryReader> reader, std::shared_ptr<LUS::IResource> resource) override;
};
}; // namespace LUS
