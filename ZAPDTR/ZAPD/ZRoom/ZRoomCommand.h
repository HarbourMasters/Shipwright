#pragma once

#include "tinyxml2.h"

#include <string>
#include <vector>

#include "ZFile.h"
#include "ZResource.h"

class ZRoom;

enum class RoomCommand : uint8_t
{
	SetStartPositionList = 0x00,
	SetActorList = 0x01,
	SetCsCamera = 0x02,
	SetCollisionHeader = 0x03,
	SetRoomList = 0x04,
	SetWind = 0x05,
	SetEntranceList = 0x06,
	SetSpecialObjects = 0x07,
	SetRoomBehavior = 0x08,
	Unused09 = 0x09,
	SetMesh = 0x0A,
	SetObjectList = 0x0B,
	SetLightList = 0x0C,
	SetPathways = 0x0D,
	SetTransitionActorList = 0x0E,
	SetLightingSettings = 0x0F,
	SetTimeSettings = 0x10,
	SetSkyboxSettings = 0x11,
	SetSkyboxModifier = 0x12,
	SetExitList = 0x13,
	EndMarker = 0x14,
	SetSoundSettings = 0x15,
	SetEchoSettings = 0x16,
	SetCutscenes = 0x17,
	SetAlternateHeaders = 0x18,
	SetCameraSettings = 0x19,

	// MM Commands
	SetWorldMapVisited = 0x19,
	SetAnimatedMaterialList = 0x1A,
	SetActorCutsceneList = 0x1B,
	SetMinimapList = 0x1C,
	Unused1D = 0x1D,
	SetMinimapChests = 0x1E,

	Error = 0xFF
};

class ZRoomCommand : public ZResource
{
public:
	int32_t cmdAddress;
	uint32_t cmdIndex;
	uint32_t commandSet;
	RoomCommand cmdID;
	offset_t segmentOffset;

	ZRoomCommand(ZFile* nParent);
	virtual ~ZRoomCommand() = default;

	virtual void ExtractCommandFromRoom(ZRoom* nZRoom, uint32_t nRawDataIndex);

	void ParseRawData() override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	// Getters/Setters
	virtual RoomCommand GetRoomCommand() const = 0;
	size_t GetRawDataSize() const final override;
	virtual std::string GetCommandCName() const;

	virtual std::string GetCommandHex() const;

public:
	ZRoom* zRoom;

	uint8_t cmdArg1;
	segptr_t cmdArg2;
};
