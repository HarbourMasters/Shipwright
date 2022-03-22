#pragma once

#include "ZCutscene.h"
#include "ZCutsceneMM.h"
#include "ZRoom/ZRoomCommand.h"

class CutsceneEntry
{
public:
	CutsceneEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	uint32_t segmentOffset;
	uint16_t exit;
	uint8_t entrance;
	uint8_t flag;
};

class SetCutscenes : public ZRoomCommand
{
public:
	std::vector<ZCutsceneBase*> cutscenes;
	std::vector<CutsceneEntry> cutsceneEntries;  // (MM Only)
	uint8_t numCutscenes;                        // (MM Only)

	SetCutscenes(ZFile* nParent);
	~SetCutscenes();

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
