#pragma once

#include "ZCollision.h"
#include "ZRoom/ZRoomCommand.h"

class SetCollisionHeader : public ZRoomCommand
{
public:
	ZCollisionHeader* collisionHeader;

	SetCollisionHeader(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;
};
