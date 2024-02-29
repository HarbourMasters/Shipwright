#pragma once

#include "soh/resource/type/Scene.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"
#include "Resource.h"
#include "ResourceFactoryBinary.h"

namespace SOH {
class ResourceFactoryBinarySceneV0 : public LUS::ResourceFactoryBinary {
  public:
    ResourceFactoryBinarySceneV0();

    std::shared_ptr<LUS::IResource> ReadResource(std::shared_ptr<LUS::File> file) override;
    void ParseSceneCommands(std::shared_ptr<Scene> scene, std::shared_ptr<LUS::BinaryReader> reader);

    // Doing something very similar to what we do on the ResourceLoader.
    // Eventually, scene commands should be moved up to the ResourceLoader as well.
    // They can not right now because the exporter does not give them a proper resource type enum value,
    // and the exporter does not export the commands with a proper OTR header.
    static inline std::unordered_map<SceneCommandID, std::shared_ptr<SceneCommandFactoryBinaryV0>> sceneCommandFactories;
protected:
    std::shared_ptr<ISceneCommand> ParseSceneCommand(std::shared_ptr<Scene> scene, std::shared_ptr<LUS::BinaryReader> reader, uint32_t index);
};
} // namespace SOH
