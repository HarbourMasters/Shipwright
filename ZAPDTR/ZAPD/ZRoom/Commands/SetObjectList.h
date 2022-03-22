#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetObjectList : public ZRoomCommand
{
public:
	std::vector<uint16_t> objects;

	SetObjectList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;
};
