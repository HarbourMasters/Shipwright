#include "SetSkyboxSettings.h"
#include "Globals.h"
#include "Utils/StringHelper.h"

SetSkyboxSettings::SetSkyboxSettings(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetSkyboxSettings::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	unk1 = cmdArg1;
	skyboxNumber = parent->GetRawData().at(rawDataIndex + 0x04);
	cloudsType = parent->GetRawData().at(rawDataIndex + 0x05);
	isIndoors = parent->GetRawData().at(rawDataIndex + 0x06);
}

std::string SetSkyboxSettings::GetBodySourceCode() const
{
	std::string indoors = StringHelper::BoolStr(isIndoors);
	if (Globals::Instance->game == ZGame::MM_RETAIL)
		return StringHelper::Sprintf("SCENE_CMD_SKYBOX_SETTINGS(0x%02X, %i, %i, %s)", unk1,
		                             skyboxNumber, cloudsType, indoors.c_str());
	return StringHelper::Sprintf("SCENE_CMD_SKYBOX_SETTINGS(%i, %i, %s)", skyboxNumber, cloudsType,
	                             indoors.c_str());
}

std::string SetSkyboxSettings::GetCommandCName() const
{
	return "SCmdSkyboxSettings";
}

RoomCommand SetSkyboxSettings::GetRoomCommand() const
{
	return RoomCommand::SetSkyboxSettings;
}
