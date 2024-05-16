#include "soh/resource/importer/scenecommand/SceneCommandFactory.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "spdlog/spdlog.h"

namespace SOH {
void SceneCommandFactoryBinaryV0::ReadCommandId(std::shared_ptr<ISceneCommand> command, std::shared_ptr<Ship::BinaryReader> reader) {
    command->cmdId = (SceneCommandID)reader->ReadInt32();
}
} // namespace SOH
