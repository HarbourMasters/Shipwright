#include "SetRoomBehavior.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

SetRoomBehavior::SetRoomBehavior(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetRoomBehavior::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	gameplayFlags = cmdArg1;
	gameplayFlags2 = BitConverter::ToInt32BE(parent->GetRawData(), rawDataIndex + 0x04);

	currRoomUnk2 = gameplayFlags2 & 0xFF;

	currRoomUnk5 = showInvisActors = (gameplayFlags2 >> 8) & 1;

	msgCtxUnk = (gameplayFlags2 >> 10) & 1;

	enablePosLights = (gameplayFlags2 >> 11) & 1;
	kankyoContextUnkE2 = (gameplayFlags2 >> 12) & 1;
}

std::string SetRoomBehavior::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		std::string enableLights = StringHelper::BoolStr(enablePosLights);
		return StringHelper::Sprintf("SCENE_CMD_ROOM_BEHAVIOR(0x%02X, 0x%02X, %i, %i, %s, %i)",
		                             gameplayFlags, currRoomUnk2, currRoomUnk5, msgCtxUnk,
		                             enableLights.c_str(), kankyoContextUnkE2);
	}
	std::string showInvisible = StringHelper::BoolStr(showInvisActors);
	std::string disableWarps = StringHelper::BoolStr(msgCtxUnk);
	return StringHelper::Sprintf("SCENE_CMD_ROOM_BEHAVIOR(0x%02X, 0x%02X, %s, %s)", gameplayFlags,
	                             currRoomUnk2, showInvisible.c_str(), disableWarps.c_str());
}

std::string SetRoomBehavior::GetCommandCName() const
{
	return "SCmdRoomBehavior";
}

RoomCommand SetRoomBehavior::GetRoomCommand() const
{
	return RoomCommand::SetRoomBehavior;
}
