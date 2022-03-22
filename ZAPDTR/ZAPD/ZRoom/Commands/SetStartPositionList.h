#pragma once

#include "SetActorList.h"
#include "ZRoom/ZRoomCommand.h"

class SetStartPositionList : public ZRoomCommand
{
public:
	std::vector<ActorSpawnEntry> actors;

	SetStartPositionList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
