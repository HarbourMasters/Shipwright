#include "resource/importer/SceneCommandFactory.h"
#include "resource/type/SceneCommand.h"
#include "spdlog/spdlog.h"

namespace Ship {
void SceneCommandFactory::ReadCommandId(std::shared_ptr<SceneCommand> command, std::shared_ptr<BinaryReader> reader) {
    command->cmdId = (SceneCommandID)reader->ReadInt32();
}
}
    