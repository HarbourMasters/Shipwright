#pragma once

#include "ZRoom/ZRoomCommand.h"

class ActorCutsceneEntry
{
protected:
	int16_t priority;
	int16_t length;
	int16_t unk4;
	int16_t unk6;
	int16_t additionalCutscene;
	uint8_t sound;
	uint8_t unkB;
	int16_t unkC;
	uint8_t unkE;
	uint8_t letterboxSize;

public:
	ActorCutsceneEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const;
	std::string GetSourceTypeName() const;
};

class SetActorCutsceneList : public ZRoomCommand
{
public:
	std::vector<ActorCutsceneEntry> cutscenes;

	SetActorCutsceneList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;

private:
};
