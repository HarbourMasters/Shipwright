#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetCameraSettings : public ZRoomCommand
{
public:
	uint8_t cameraMovement;
	uint32_t mapHighlight;

	SetCameraSettings(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;

private:
};
