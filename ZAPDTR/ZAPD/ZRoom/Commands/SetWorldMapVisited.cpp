#include "SetWorldMapVisited.h"

#include "Utils/StringHelper.h"

SetWorldMapVisited::SetWorldMapVisited(ZFile* nParent) : ZRoomCommand(nParent)
{
}

std::string SetWorldMapVisited::GetBodySourceCode() const
{
	return "SCENE_CMD_MISC_SETTINGS()";
}

std::string SetWorldMapVisited::GetCommandCName() const
{
	return "SCmdWorldMapVisited";
}

RoomCommand SetWorldMapVisited::GetRoomCommand() const
{
	return RoomCommand::SetWorldMapVisited;
}
