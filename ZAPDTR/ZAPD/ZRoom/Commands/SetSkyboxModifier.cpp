#include "SetSkyboxModifier.h"

#include "Utils/StringHelper.h"

SetSkyboxModifier::SetSkyboxModifier(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetSkyboxModifier::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	disableSky = parent->GetRawData().at(rawDataIndex + 0x04);
	disableSunMoon = parent->GetRawData().at(rawDataIndex + 0x05);
}

std::string SetSkyboxModifier::GetBodySourceCode() const
{
	std::string sky = StringHelper::BoolStr(disableSky);
	std::string soonMoon = StringHelper::BoolStr(disableSunMoon);
	return StringHelper::Sprintf("SCENE_CMD_SKYBOX_DISABLES(%s, %s)", sky.c_str(),
	                             soonMoon.c_str());
}

std::string SetSkyboxModifier::GetCommandCName() const
{
	return "SCmdSkyboxDisables";
}

RoomCommand SetSkyboxModifier::GetRoomCommand() const
{
	return RoomCommand::SetSkyboxModifier;
}
