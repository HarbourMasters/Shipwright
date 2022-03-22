#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetSkyboxModifier : public ZRoomCommand
{
public:
	uint8_t disableSky;
	uint8_t disableSunMoon;

	SetSkyboxModifier(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;
};
