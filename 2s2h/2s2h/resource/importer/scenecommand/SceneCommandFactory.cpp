#include "2s2h/resource/importer/scenecommand/SceneCommandFactory.h"
#include "2s2h/resource/type/scenecommand/SceneCommand.h"

namespace SOH {
void SceneCommandFactoryBinaryV0::ReadCommandId(std::shared_ptr<SOH::ISceneCommand> command,
                                                std::shared_ptr<Ship::BinaryReader> reader) {
    command->cmdId = (SceneCommandID)reader->ReadInt32();
}
} // namespace SOH
