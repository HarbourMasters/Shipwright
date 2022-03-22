#pragma once

#include "ZRoom/ZRoomCommand.h"

class EndMarker : public ZRoomCommand
{
public:
	EndMarker(ZFile* nParent);

	std::string GetBodySourceCode() const override;
	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;
};