#pragma once

#include "2s2h/resource/type/Scene.h"
#include "2s2h/resource/type/scenecommand/SceneCommand.h"
#include "2s2h/resource/importer/scenecommand/SceneCommandFactory.h"
#include <ship/resource/Resource.h>
#include <ship/resource/ResourceFactoryBinary.h>
#include <unordered_map>

namespace SOH {
class ResourceFactoryBinarySceneV0 : public Ship::ResourceFactoryBinary {
  public:
    ResourceFactoryBinarySceneV0();

    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::File> file,
                                                  std::shared_ptr<Ship::ResourceInitData> initData) override;
    void ParseSceneCommands(std::shared_ptr<Scene> scene, std::shared_ptr<Ship::BinaryReader> reader);

    // Doing something very similar to what we do on the ResourceLoader.
    // Eventually, scene commands should be moved up to the ResourceLoader as well.
    // They can not right now because the exporter does not give them a proper resource type enum value,
    // and the exporter does not export the commands with a proper OTR header.
    static inline std::unordered_map<SceneCommandID, std::shared_ptr<SceneCommandFactoryBinaryV0>>
        sceneCommandFactories;

  protected:
    std::shared_ptr<ISceneCommand> ParseSceneCommand(std::shared_ptr<Scene> scene,
                                                     std::shared_ptr<Ship::BinaryReader> reader, uint32_t index);
};
} // namespace SOH
