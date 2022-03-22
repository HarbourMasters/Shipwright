#pragma once

#include "ZRoom/ZRoomCommand.h"

class Unused09 : public ZRoomCommand
{
public:
	Unused09(ZFile* nParent);

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
