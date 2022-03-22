#include "SetCutscenes.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetCutscenes::SetCutscenes(ZFile* nParent) : ZRoomCommand(nParent)
{
}

SetCutscenes::~SetCutscenes()
{
	for (ZCutsceneBase* cutscene : cutscenes)
		delete cutscene;
}

void SetCutscenes::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	std::string output;

	numCutscenes = cmdArg1;
	if (Globals::Instance->game == ZGame::OOT_RETAIL || Globals::Instance->game == ZGame::OOT_SW97)
	{
		ZCutscene* cutscene = new ZCutscene(parent);
		cutscene->ExtractFromFile(segmentOffset);

		auto decl = parent->GetDeclaration(segmentOffset);
		if (decl == nullptr)
		{
			cutscene->DeclareVar(zRoom->GetName().c_str(), "");
		}

		cutscenes.push_back(cutscene);
	}
	else
	{
		int32_t currentPtr = segmentOffset;
		std::string declaration;

		for (uint8_t i = 0; i < numCutscenes; i++)
		{
			CutsceneEntry entry(parent->GetRawData(), currentPtr);
			cutsceneEntries.push_back(entry);
			currentPtr += 8;

			// TODO: don't hardcode %sCutsceneData_%06X, look up for the declared name instead
			declaration += StringHelper::Sprintf(
				"    { %sCutsceneData_%06X, 0x%04X, 0x%02X, 0x%02X },", zRoom->GetName().c_str(),
				entry.segmentOffset, entry.exit, entry.entrance, entry.flag);

			if (i < numCutscenes - 1)
				declaration += "\n";

			ZCutsceneMM* cutscene = new ZCutsceneMM(parent);
			cutscene->ExtractFromFile(entry.segmentOffset);
			cutscenes.push_back(cutscene);
		}

		parent->AddDeclarationArray(segmentOffset, DeclarationAlignment::Align4,
		                            cutsceneEntries.size() * 8, "CutsceneEntry",
		                            StringHelper::Sprintf("%sCutsceneEntryList_%06X",
		                                                  zRoom->GetName().c_str(), segmentOffset),
		                            cutsceneEntries.size(), declaration);
	}

	for (ZCutsceneBase* cutscene : cutscenes)
	{
		if (cutscene->GetRawDataIndex() != 0)
		{
			Declaration* decl = parent->GetDeclaration(cutscene->GetRawDataIndex());
			if (decl == nullptr)
			{
				cutscene->GetSourceOutputCode(zRoom->GetName());
			}
			else if (decl->text == "")
			{
				decl->text = cutscene->GetBodySourceCode();
			}
		}
	}
}

std::string SetCutscenes::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "CutsceneData", listName);

	if (Globals::Instance->game == ZGame::MM_RETAIL)
		return StringHelper::Sprintf("SCENE_CMD_CUTSCENE_LIST(%i, %s)", numCutscenes,
		                             listName.c_str());
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
	: segmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 0))),
	  exit(BitConverter::ToInt16BE(rawData, rawDataIndex + 4)), entrance(rawData[rawDataIndex + 6]),
	  flag(rawData[rawDataIndex + 7])
{
}
