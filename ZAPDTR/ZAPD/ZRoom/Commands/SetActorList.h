#pragma once

#include "ZActorList.h"
#include "ZRoom/ZRoomCommand.h"

class SetActorList : public ZRoomCommand
{
public:
	uint32_t numActors;
	ZActorList* actorList = nullptr;

	SetActorList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

protected:
	size_t GetActorListArraySize() const;
};
