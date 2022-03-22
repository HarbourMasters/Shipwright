#pragma once
#include "ZRoom/ZRoomCommand.h"

class ZRoomCommandUnk : public ZRoomCommand
{
public:
	ZRoomCommandUnk(ZFile* nParent);

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
};
