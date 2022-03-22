#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetEchoSettings : public ZRoomCommand
{
public:
	uint8_t echo;

	SetEchoSettings(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;
};
