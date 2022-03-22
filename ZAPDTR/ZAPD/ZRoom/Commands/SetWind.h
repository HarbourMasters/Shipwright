#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetWind : public ZRoomCommand
{
public:
	uint8_t windWest;
	uint8_t windVertical;
	uint8_t windSouth;
	uint8_t clothFlappingStrength;

	SetWind(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;
};
