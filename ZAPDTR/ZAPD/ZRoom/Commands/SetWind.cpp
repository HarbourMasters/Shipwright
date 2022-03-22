#include "SetWind.h"
#include "Utils/StringHelper.h"

SetWind::SetWind(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetWind::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	auto& parentRawData = parent->GetRawData();
	windWest = parentRawData.at(rawDataIndex + 0x04);
	windVertical = parentRawData.at(rawDataIndex + 0x05);
	windSouth = parentRawData.at(rawDataIndex + 0x06);
	clothFlappingStrength = parentRawData.at(rawDataIndex + 0x07);
}

std::string SetWind::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_WIND_SETTINGS(%i, %i, %i, %i)", windWest, windVertical,
	                             windSouth, clothFlappingStrength);
}

std::string SetWind::GetCommandCName() const
{
	return "SCmdWindSettings";
}

RoomCommand SetWind::GetRoomCommand() const
{
	return RoomCommand::SetWind;
}
