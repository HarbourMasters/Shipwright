#pragma once

#include "ZRoom/ZRoomCommand.h"
#include "ZVector.h"

class CsCameraEntry
{
public:
	CsCameraEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetSourceTypeName() const;
	int32_t GetRawDataSize() const;

	int16_t GetNumPoints() const;
	segptr_t GetCamAddress() const;
	uint32_t GetSegmentOffset() const;

	int baseOffset;
	int16_t type;
	int16_t numPoints;
	segptr_t camAddress;
	uint32_t segmentOffset;
};

class SetCsCamera : public ZRoomCommand
{
public:
	std::vector<CsCameraEntry> cameras;
	std::vector<ZVector> points;

	SetCsCamera(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
