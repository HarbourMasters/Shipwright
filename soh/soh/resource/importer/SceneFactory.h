#pragma once

#include "soh/resource/type/Scene.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"
#include "Resource.h"
#include "ResourceFactory.h"

class ResourceFactoryBinarySceneV0 : public LUS::ResourceFactory {
  public:
    std::shared_ptr<LUS::IResource> ReadResource(std::shared_ptr<LUS::File> file) override;
    void ParseSceneCommands(std::shared_ptr<Scene> scene, std::shared_ptr<BinaryReader> reader);
protected:
    std::shared_ptr<ISceneCommand> ParseSceneCommand(std::shared_ptr<Scene> scene, std::shared_ptr<BinaryReader> reader, uint32_t index);
};
