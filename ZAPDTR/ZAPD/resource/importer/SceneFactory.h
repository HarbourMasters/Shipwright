#pragma once

#include "resource/type/Scene.h"
#include "resource/type/scenecommand/SceneCommand.h"
#include "resource/importer/scenecommand/SceneCommandFactory.h"
#include "Resource.h"
#include "ResourceFactory.h"

namespace Ship {
class SceneFactory : public ResourceFactory {
  public:
    std::shared_ptr<Resource> ReadResource(std::shared_ptr<BinaryReader> reader);

    // Doing something very similar to what we do on the ResourceLoader.
    // Eventually, scene commands should be moved up to the ResourceLoader as well.
    // They can not right now because the exporter does not give them a proper resource type enum value,
    // and the exporter does not export the commands with a proper OTR header.
    static std::unordered_map<SceneCommandID, std::shared_ptr<SceneCommandFactory>> sceneCommandFactories;
};

class SceneFactoryV0 : public ResourceVersionFactory {
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<Resource> resource) override;
    std::shared_ptr<SceneCommand> ParseSceneCommand(std::shared_ptr<BinaryReader> reader);
};
}; // namespace Ship
