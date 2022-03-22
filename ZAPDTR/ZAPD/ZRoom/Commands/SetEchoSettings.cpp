#include "SetEchoSettings.h"
#include "Utils/StringHelper.h"

SetEchoSettings::SetEchoSettings(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetEchoSettings::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	echo = parent->GetRawData().at(rawDataIndex + 0x07);
}

std::string SetEchoSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_ECHO_SETTINGS(%i)", echo);
}

std::string SetEchoSettings::GetCommandCName() const
{
	return "SCmdEchoSettings";
}

RoomCommand SetEchoSettings::GetRoomCommand() const
{
	return RoomCommand::SetEchoSettings;
}
