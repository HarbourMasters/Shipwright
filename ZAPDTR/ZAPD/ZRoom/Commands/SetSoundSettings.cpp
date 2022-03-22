#include "SetSoundSettings.h"
#include "Utils/StringHelper.h"

SetSoundSettings::SetSoundSettings(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetSoundSettings::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	reverb = cmdArg1;
	nightTimeSFX = parent->GetRawData().at(rawDataIndex + 0x06);
	musicSequence = parent->GetRawData().at(rawDataIndex + 0x07);
}

std::string SetSoundSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_SOUND_SETTINGS(%i, %i, %i)", reverb, nightTimeSFX,
	                             musicSequence);
}

std::string SetSoundSettings::GetCommandCName() const
{
	return "SCmdSoundSettings";
}

RoomCommand SetSoundSettings::GetRoomCommand() const
{
	return RoomCommand::SetSoundSettings;
}
