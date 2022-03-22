#pragma once

#include "ZRoom/ZRoomCommand.h"

class TransitionActorEntry
{
public:
	uint8_t frontObjectRoom;
	uint8_t frontTransitionReaction;
	uint8_t backObjectRoom;
	uint8_t backTransitionReaction;
	uint16_t actorNum;
	int16_t posX, posY, posZ;
	int16_t rotY;
	uint16_t initVar;

	TransitionActorEntry(const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() const;
};

class SetTransitionActorList : public ZRoomCommand
{
public:
	std::vector<TransitionActorEntry> transitionActors;

	SetTransitionActorList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
