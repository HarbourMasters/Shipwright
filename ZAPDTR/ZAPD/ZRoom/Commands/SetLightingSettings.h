#pragma once

#include "ZRoom/ZRoomCommand.h"

class LightingSettings
{
public:
	uint8_t ambientClrR, ambientClrG, ambientClrB;
	uint8_t diffuseClrA_R, diffuseClrA_G, diffuseClrA_B;
	uint8_t diffuseDirA_X, diffuseDirA_Y, diffuseDirA_Z;
	uint8_t diffuseClrB_R, diffuseClrB_G, diffuseClrB_B;
	uint8_t diffuseDirB_X, diffuseDirB_Y, diffuseDirB_Z;
	uint8_t fogClrR, fogClrG, fogClrB;
	uint16_t unk;
	uint16_t drawDistance;

	LightingSettings(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const;

	size_t GetRawDataSize() const;
};

class SetLightingSettings : public ZRoomCommand
{
public:
	std::vector<LightingSettings> settings;

	SetLightingSettings(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
