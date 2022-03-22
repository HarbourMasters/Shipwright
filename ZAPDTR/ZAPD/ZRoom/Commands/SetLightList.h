#pragma once

#include <string>

#include "ZFile.h"
#include "ZRoom/ZRoom.h"
#include "ZRoom/ZRoomCommand.h"

class LightInfo
{
public:
	LightInfo(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const;

	std::string GetSourceTypeName() const;
	size_t GetRawDataSize() const;

public:
	uint8_t type;
	int16_t x, y, z;
	uint8_t r, g, b;
	uint8_t drawGlow;
	int16_t radius;
};

class SetLightList : public ZRoomCommand
{
public:
	uint8_t numLights;
	std::vector<LightInfo> lights;

	SetLightList(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
