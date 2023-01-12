#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace Ship {
class SetEchoSettingsFactory : public SceneCommandFactory {
  public:
    std::shared_ptr<Resource> ReadResource(std::shared_ptr<BinaryReader> reader);
};

class SetEchoSettingsFactoryV0 : public SceneCommandVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace Ship
