#pragma once

#include "ZCutscene.h"
#include "ZRoom/ZRoomCommand.h"

class CutsceneEntry
{
public:
	CutsceneEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	segptr_t segmentPtr;
	uint16_t exit;
	uint8_t entrance;
	uint8_t flag;
};

class SetCutscenes : public ZRoomCommand
{
public:
	std::vector<CutsceneEntry> cutsceneEntries;  // (MM Only)
	uint8_t numCutscenes;                        // (MM Only)

	SetCutscenes(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
