#include "SetEntranceList.h"

#include "Globals.h"
#include "SetStartPositionList.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetEntranceList::SetEntranceList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetEntranceList::DeclareReferences([[maybe_unused]] const std::string& prefix)
{
	if (segmentOffset != 0)
	{
		std::string varName =
			StringHelper::Sprintf("%sEntranceList0x%06X", prefix.c_str(), segmentOffset);
		parent->AddDeclarationPlaceholder(segmentOffset, varName);
	}
}

void SetEntranceList::ParseRawDataLate()
{
	// Parse Entrances and Generate Declaration
	int numEntrances = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 2;
	uint32_t currentPtr = segmentOffset;

	for (int32_t i = 0; i < numEntrances; i++)
	{
		EntranceEntry entry(parent->GetRawData(), currentPtr);
		entrances.push_back(entry);

		currentPtr += 2;
	}
}

void SetEntranceList::DeclareReferencesLate([[maybe_unused]] const std::string& prefix)
{
	if (!entrances.empty())
	{
		std::string declaration;

		size_t index = 0;
		for (const auto& entry : entrances)
		{
			declaration += StringHelper::Sprintf("    { %s },", entry.GetBodySourceCode().c_str());
			if (index + 1 < entrances.size())
				declaration += "\n";

			index++;
		}

		std::string varName =
			StringHelper::Sprintf("%sEntranceList0x%06X", prefix.c_str(), segmentOffset);
		parent->AddDeclarationArray(segmentOffset, DeclarationAlignment::Align4,
		                            entrances.size() * 2, "EntranceEntry", varName,
		                            entrances.size(), declaration);
	}
}

std::string SetEntranceList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "EntranceEntry", listName);
	return StringHelper::Sprintf("SCENE_CMD_ENTRANCE_LIST(%s)", listName.c_str());
}

std::string SetEntranceList::GetCommandCName() const
{
	return "SCmdEntranceList";
}

RoomCommand SetEntranceList::GetRoomCommand() const
{
	return RoomCommand::SetEntranceList;
}

EntranceEntry::EntranceEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	startPositionIndex = rawData.at(rawDataIndex + 0);
	roomToLoad = rawData.at(rawDataIndex + 1);
}

std::string EntranceEntry::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%02X, 0x%02X", startPositionIndex, roomToLoad);
}
