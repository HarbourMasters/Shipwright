#include "EndMarker.h"

EndMarker::EndMarker(ZFile* nParent) : ZRoomCommand(nParent)
{
}

std::string EndMarker::GetBodySourceCode() const
{
	return "SCENE_CMD_END()";
}

std::string EndMarker::GetCommandCName() const
{
	return "SCmdEndMarker";
}

RoomCommand EndMarker::GetRoomCommand() const
{
	return RoomCommand::EndMarker;
}
