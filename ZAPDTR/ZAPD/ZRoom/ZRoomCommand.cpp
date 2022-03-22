#include "ZRoomCommand.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZRoom.h"

ZRoomCommand::ZRoomCommand(ZFile* nParent) : ZResource(nParent)
{
}

void ZRoomCommand::ExtractCommandFromRoom(ZRoom* nZRoom, uint32_t nRawDataIndex)
{
	zRoom = nZRoom;
	rawDataIndex = nRawDataIndex;

	ParseRawData();
}

void ZRoomCommand::ParseRawData()
{
	auto& parentRawData = parent->GetRawData();
	cmdID = static_cast<RoomCommand>(parentRawData.at(rawDataIndex));
	cmdAddress = rawDataIndex;

	cmdArg1 = parentRawData.at(rawDataIndex + 1);
	cmdArg2 = BitConverter::ToUInt32BE(parentRawData, rawDataIndex + 4);
	segmentOffset = Seg2Filespace(cmdArg2, parent->baseAddress);
}

RoomCommand ZRoomCommand::GetRoomCommand() const
{
	return RoomCommand::Error;
}

size_t ZRoomCommand::GetRawDataSize() const
{
	return 0x08;
}

std::string ZRoomCommand::GetSourceTypeName() const
{
	return GetCommandCName();
}

ZResourceType ZRoomCommand::GetResourceType() const
{
	return ZResourceType::RoomCommand;
}

std::string ZRoomCommand::GetCommandCName() const
{
	return "SceneCmd";
}

std::string ZRoomCommand::GetCommandHex() const
{
	return StringHelper::Sprintf("0x%02X", static_cast<uint8_t>(cmdID));
}
