#include "SetTimeSettings.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

SetTimeSettings::SetTimeSettings(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetTimeSettings::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	hour = parent->GetRawData().at(rawDataIndex + 4);
	min = parent->GetRawData().at(rawDataIndex + 5);
	unk = parent->GetRawData().at(rawDataIndex + 6);
}

std::string SetTimeSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_TIME_SETTINGS(%i, %i, %i)", hour, min, unk);
}

std::string SetTimeSettings::GetCommandCName() const
{
	return "SCmdTimeSettings";
}

RoomCommand SetTimeSettings::GetRoomCommand() const
{
	return RoomCommand::SetTimeSettings;
}
