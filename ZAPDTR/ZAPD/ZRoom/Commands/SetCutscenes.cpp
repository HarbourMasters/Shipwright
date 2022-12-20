#include "SetCutscenes.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetCutscenes::SetCutscenes(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetCutscenes::ParseRawData()
{
	ZRoomCommand::ParseRawData();

	numCutscenes = cmdArg1;

	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		int32_t currentPtr = segmentOffset;

		cutsceneEntries.reserve(numCutscenes);
		for (uint8_t i = 0; i < numCutscenes; i++)
		{
			CutsceneEntry entry(parent->GetRawData(), currentPtr);
			cutsceneEntries.push_back(entry);
			currentPtr += 8;
		}
	}
}

void SetCutscenes::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = name;
	if (varPrefix == "")
		varPrefix = prefix;

	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		std::string declaration;
		size_t i = 0;

		for (const auto& entry : cutsceneEntries)
		{
			if (entry.segmentPtr != SEGMENTED_NULL &&
			    GETSEGNUM(entry.segmentPtr) == parent->segment)
			{
				offset_t csOffset = Seg2Filespace(entry.segmentPtr, parent->baseAddress);
				if (!parent->HasDeclaration(csOffset))
				{
					auto* cutscene = new ZCutscene(parent);
					cutscene->ExtractFromFile(csOffset);
					cutscene->SetName(cutscene->GetDefaultName(varPrefix));
					cutscene->DeclareVar(varPrefix, "");
					cutscene->DeclareReferences(varPrefix);
					parent->AddResource(cutscene);
				}
			}

			std::string csName;
			Globals::Instance->GetSegmentedPtrName(entry.segmentPtr, parent, "CutsceneData",
			                                       csName);

			declaration +=
				StringHelper::Sprintf("    { %s, 0x%04X, 0x%02X, 0x%02X },", csName.c_str(),
			                          entry.exit, entry.entrance, entry.flag);

			if (i + 1 < numCutscenes)
				declaration += "\n";

			i++;
		}

		parent->AddDeclarationArray(segmentOffset, DeclarationAlignment::Align4,
		                            cutsceneEntries.size() * 8, "CutsceneEntry",
		                            StringHelper::Sprintf("%sCutsceneEntryList_%06X",
		                                                  zRoom->GetName().c_str(), segmentOffset),
		                            cutsceneEntries.size(), declaration);
	}
	else
	{
		if (cmdArg2 != SEGMENTED_NULL && GETSEGNUM(cmdArg2) == parent->segment)
		{
			offset_t csOffset = Seg2Filespace(cmdArg2, parent->baseAddress);
			if (!parent->HasDeclaration(csOffset))
			{
				auto* cutscene = new ZCutscene(parent);
				cutscene->ExtractFromFile(csOffset);
				cutscene->SetName(cutscene->GetDefaultName(varPrefix));
				cutscene->DeclareVar(varPrefix, "");
				cutscene->DeclareReferences(varPrefix);
				parent->AddResource(cutscene);
			}
		}
	}
}

std::string SetCutscenes::GetBodySourceCode() const
{
	std::string listName;
<<<<<<< HEAD
=======
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "CutsceneData", listName,
	                                       parent->workerID);
>>>>>>> e55b9ea (Create test.txt)

	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "CutsceneEntry", listName);
		return StringHelper::Sprintf("SCENE_CMD_CUTSCENE_LIST(%i, %s)", numCutscenes,
		                             listName.c_str());
	}

	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "CutsceneData", listName);
	return StringHelper::Sprintf("SCENE_CMD_CUTSCENE_DATA(%s)", listName.c_str());
}

std::string SetCutscenes::GetCommandCName() const
{
	return "SCmdCutsceneData";
}

RoomCommand SetCutscenes::GetRoomCommand() const
{
	return RoomCommand::SetCutscenes;
}

CutsceneEntry::CutsceneEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: segmentPtr(BitConverter::ToInt32BE(rawData, rawDataIndex + 0)),
	  exit(BitConverter::ToInt16BE(rawData, rawDataIndex + 4)), entrance(rawData[rawDataIndex + 6]),
	  flag(rawData[rawDataIndex + 7])
{
}
