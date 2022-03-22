#include "SetMinimapChests.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetMinimapChests::SetMinimapChests(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetMinimapChests::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	int numChests = cmdArg1;

	int32_t currentPtr = segmentOffset;

	for (int32_t i = 0; i < numChests; i++)
	{
		MinimapChest chest(parent->GetRawData(), currentPtr);
		chests.push_back(chest);

		currentPtr += 10;
	}
}

void SetMinimapChests::DeclareReferences(const std::string& prefix)
{
	std::string declaration;

	size_t index = 0;
	for (const auto& chest : chests)
	{
		declaration += StringHelper::Sprintf("    { %s },", chest.GetBodySourceCode().c_str());

		if (index < chests.size() - 1)
			declaration += "\n";

		index++;
	}

	parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::Align4, chests.size() * 10, "MinimapChest",
		StringHelper::Sprintf("%sMinimapChests0x%06X", prefix.c_str(), segmentOffset),
		chests.size(), declaration);
}

std::string SetMinimapChests::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "MinimapChest", listName);
	return StringHelper::Sprintf("SCENE_CMD_MINIMAP_COMPASS_ICON_INFO(0x%02X, %s)", chests.size(),
	                             listName.c_str());
}

std::string SetMinimapChests::GetCommandCName() const
{
	return "SCmdMinimapChests";
}

RoomCommand SetMinimapChests::GetRoomCommand() const
{
	return RoomCommand::SetMinimapChests;
}

MinimapChest::MinimapChest(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: unk0(BitConverter::ToUInt16BE(rawData, rawDataIndex + 0)),
	  unk2(BitConverter::ToUInt16BE(rawData, rawDataIndex + 2)),
	  unk4(BitConverter::ToUInt16BE(rawData, rawDataIndex + 4)),
	  unk6(BitConverter::ToUInt16BE(rawData, rawDataIndex + 6)),
	  unk8(BitConverter::ToUInt16BE(rawData, rawDataIndex + 8))
{
}

std::string MinimapChest::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X", unk0, unk2, unk4, unk6,
	                             unk8);
}
