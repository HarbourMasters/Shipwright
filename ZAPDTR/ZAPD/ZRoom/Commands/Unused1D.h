#pragma once

#include "ZRoom/ZRoomCommand.h"

class Unused1D : public ZRoomCommand
{
public:
	Unused1D(ZFile* nParent);

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
