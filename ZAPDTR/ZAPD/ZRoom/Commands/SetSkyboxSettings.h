#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetSkyboxSettings : public ZRoomCommand
{
public:
	uint8_t unk1;  // (MM Only)
	uint8_t skyboxNumber;
	uint8_t cloudsType;
	uint8_t isIndoors;

	SetSkyboxSettings(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;
};
