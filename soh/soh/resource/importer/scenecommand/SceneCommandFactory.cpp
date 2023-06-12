#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "spdlog/spdlog.h"

namespace LUS {
void SceneCommandVersionFactory::ReadCommandId(std::shared_ptr<ISceneCommand> command, std::shared_ptr<BinaryReader> reader) {
    command->cmdId = (SceneCommandID)reader->ReadInt32();
}
}
    