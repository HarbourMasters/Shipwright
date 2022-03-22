#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetSoundSettings : public ZRoomCommand
{
public:
	uint8_t reverb;
	uint8_t nightTimeSFX;
	uint8_t musicSequence;

	SetSoundSettings(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
