#pragma once

#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"

namespace Ship {
class SetSpecialObjectsFactory : public SceneCommandFactory {
  public:
    std::shared_ptr<Resource> ReadResource(uint32_t version, std::shared_ptr<BinaryReader> reader);
};

class SetSpecialObjectsFactoryV0 : public SceneCommandVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace Ship
