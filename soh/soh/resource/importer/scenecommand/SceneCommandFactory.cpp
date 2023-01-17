#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "spdlog/spdlog.h"

namespace Ship {
void SceneCommandVersionFactory::ReadCommandId(std::shared_ptr<SceneCommand> command, std::shared_ptr<BinaryReader> reader) {
    command->cmdId = (SceneCommandID)reader->ReadInt32();
}
}
    