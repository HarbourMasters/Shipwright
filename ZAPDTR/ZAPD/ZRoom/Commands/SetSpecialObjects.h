#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetSpecialObjects : public ZRoomCommand
{
public:
	uint8_t elfMessage;
	uint16_t globalObject;

	SetSpecialObjects(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
