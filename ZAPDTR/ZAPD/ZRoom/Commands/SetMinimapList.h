#pragma once

#include "ZRoom/ZRoomCommand.h"

class MinimapEntry
{
public:
	MinimapEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const;

protected:
	uint16_t unk0;
	uint16_t unk2;
	uint16_t unk4;
	uint16_t unk6;
	uint16_t unk8;
};

class SetMinimapList : public ZRoomCommand
{
public:
	std::vector<MinimapEntry> minimaps;

	SetMinimapList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
	segptr_t listSegmentAddr;
	uint32_t listSegmentOffset;
	uint32_t unk4;
};
