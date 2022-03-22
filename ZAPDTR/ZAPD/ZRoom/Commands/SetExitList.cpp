#include "SetExitList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetExitList::SetExitList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetExitList::DeclareReferences([[maybe_unused]] const std::string& prefix)
{
	if (segmentOffset != 0)
	{
		std::string varName =
			StringHelper::Sprintf("%sExitList_%06X", prefix.c_str(), segmentOffset);
		parent->AddDeclarationPlaceholder(segmentOffset, varName);
	}
}

void SetExitList::ParseRawDataLate()
{
	// Parse Entrances and Generate Declaration
	int numEntrances = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 2;
	uint32_t currentPtr = segmentOffset;

	for (int32_t i = 0; i < numEntrances; i++)
	{
		uint16_t exit = BitConverter::ToUInt16BE(parent->GetRawData(), currentPtr);
		exits.push_back(exit);

		currentPtr += 2;
	}
}

void SetExitList::DeclareReferencesLate([[maybe_unused]] const std::string& prefix)
{
	if (!exits.empty())
	{
		std::string declaration;

		for (size_t i = 0; i < exits.size(); i++)
		{
			declaration += StringHelper::Sprintf("    0x%04X,", exits.at(i));
			if (i + 1 < exits.size())
				declaration += "\n";
		}

		std::string varName =
			StringHelper::Sprintf("%sExitList_%06X", prefix.c_str(), segmentOffset);
		parent->AddDeclarationArray(segmentOffset, DeclarationAlignment::Align4, exits.size() * 2,
		                            "u16", varName, exits.size(), declaration);
	}
}

std::string SetExitList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "u16", listName);
	return StringHelper::Sprintf("SCENE_CMD_EXIT_LIST(%s)", listName.c_str());
}

std::string SetExitList::GetCommandCName() const
{
	return "SCmdExitList";
}

RoomCommand SetExitList::GetRoomCommand() const
{
	return RoomCommand::SetExitList;
}
