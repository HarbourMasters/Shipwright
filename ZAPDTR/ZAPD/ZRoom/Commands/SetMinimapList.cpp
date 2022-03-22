#include "SetMinimapList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetMinimapList::SetMinimapList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetMinimapList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	listSegmentAddr = BitConverter::ToInt32BE(parent->GetRawData(), segmentOffset);
	listSegmentOffset = GETSEGOFFSET(listSegmentAddr);
	unk4 = BitConverter::ToInt32BE(parent->GetRawData(), segmentOffset + 4);

	int32_t currentPtr = listSegmentOffset;

	for (int32_t i = 0; i < zRoom->roomCount; i++)
	{
		MinimapEntry entry(parent->GetRawData(), currentPtr);
		minimaps.push_back(entry);

		currentPtr += 10;
	}
}

void SetMinimapList::DeclareReferences(const std::string& prefix)
{
	{
		std::string declaration;

		size_t index = 0;
		for (const auto& entry : minimaps)
		{
			declaration += StringHelper::Sprintf("    { %s },", entry.GetBodySourceCode().c_str());

			if (index < minimaps.size() - 1)
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(
			listSegmentOffset, DeclarationAlignment::Align4, minimaps.size() * 10, "MinimapEntry",
			StringHelper::Sprintf("%sMinimapEntryList0x%06X", prefix.c_str(), listSegmentOffset),
			minimaps.size(), declaration);
	}

	{
		std::string listName;
		Globals::Instance->GetSegmentedPtrName(listSegmentAddr, parent, "MinimapEntry", listName);
		std::string declaration = StringHelper::Sprintf("\n\t%s, 0x%08X\n", listName.c_str(), unk4);

		parent->AddDeclaration(
			segmentOffset, DeclarationAlignment::Align4, 8, "MinimapList",
			StringHelper::Sprintf("%sMinimapList0x%06X", prefix.c_str(), segmentOffset),
			declaration);
	}
}

std::string SetMinimapList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "MinimapList", listName);
	return StringHelper::Sprintf("SCENE_CMD_MINIMAP_INFO(%s)", listName.c_str());
}

std::string SetMinimapList::GetCommandCName() const
{
	return "SCmdMinimapSettings";
}

RoomCommand SetMinimapList::GetRoomCommand() const
{
	return RoomCommand::SetMinimapList;
}

MinimapEntry::MinimapEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: unk0(BitConverter::ToUInt16BE(rawData, rawDataIndex + 0)),
	  unk2(BitConverter::ToUInt16BE(rawData, rawDataIndex + 2)),
	  unk4(BitConverter::ToUInt16BE(rawData, rawDataIndex + 4)),
	  unk6(BitConverter::ToUInt16BE(rawData, rawDataIndex + 6)),
	  unk8(BitConverter::ToUInt16BE(rawData, rawDataIndex + 8))
{
}

std::string MinimapEntry::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X", unk0, unk2, unk4, unk6,
	                             unk8);
}
