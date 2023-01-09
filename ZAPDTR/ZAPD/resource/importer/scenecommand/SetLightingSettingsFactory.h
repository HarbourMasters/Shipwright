#pragma once

#include "resource/importer/scenecommand/SceneCommandFactory.h"

namespace Ship {
class SetLightingSettingsFactory : public SceneCommandFactory {
  public:
    std::shared_ptr<Resource> ReadResource(std::shared_ptr<BinaryReader> reader);
};

class SetLightingSettingsV0 : public SceneCommandVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
};
}; // namespace Ship
