#include "SetActorCutsceneList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetActorCutsceneList::SetActorCutsceneList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetActorCutsceneList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	int numCutscenes = cmdArg1;
	int32_t currentPtr = segmentOffset;

	for (int32_t i = 0; i < numCutscenes; i++)
	{
		ActorCutsceneEntry entry(parent->GetRawData(), currentPtr);
		cutscenes.push_back(entry);

		currentPtr += 16;
	}
}

void SetActorCutsceneList::DeclareReferences(const std::string& prefix)
{
	if (cutscenes.size() > 0)
	{
		std::string declaration;

		for (size_t i = 0; i < cutscenes.size(); i++)
		{
			const auto& entry = cutscenes.at(i);
			declaration += StringHelper::Sprintf("    { %s },", entry.GetBodySourceCode().c_str());

			if (i + 1 < cutscenes.size())
			{
				declaration += "\n";
			}
		}

		std::string typeName = cutscenes.at(0).GetSourceTypeName();

		parent->AddDeclarationArray(
			segmentOffset, GetDeclarationAlignment(), cutscenes.size() * 16, typeName,
			StringHelper::Sprintf("%s%sList_%06X", prefix.c_str(), typeName.c_str(), segmentOffset),
			cutscenes.size(), declaration);
	}
}

std::string SetActorCutsceneList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "ActorCutscene", listName);
	return StringHelper::Sprintf("SCENE_CMD_ACTOR_CUTSCENE_LIST(%i, %s)", cutscenes.size(),
	                             listName.c_str());
}

std::string SetActorCutsceneList::GetCommandCName() const
{
	return "SCmdCutsceneActorList";
}

RoomCommand SetActorCutsceneList::GetRoomCommand() const
{
	return RoomCommand::SetActorCutsceneList;
}

ActorCutsceneEntry::ActorCutsceneEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: priority(BitConverter::ToInt16BE(rawData, rawDataIndex + 0)),
	  length(BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
	  unk4(BitConverter::ToInt16BE(rawData, rawDataIndex + 4)),
	  unk6(BitConverter::ToInt16BE(rawData, rawDataIndex + 6)),
	  additionalCutscene(BitConverter::ToInt16BE(rawData, rawDataIndex + 8)),
	  sound(rawData[rawDataIndex + 0xA]), unkB(rawData[rawDataIndex + 0xB]),
	  unkC(BitConverter::ToInt16BE(rawData, rawDataIndex + 0xC)), unkE(rawData[rawDataIndex + 0xE]),
	  letterboxSize(rawData[rawDataIndex + 0xF])
{
}

std::string ActorCutsceneEntry::GetBodySourceCode() const
{
	return StringHelper::Sprintf("%i, %i, %i, %i, %i, %i, %i, %i, %i, %i", priority, length, unk4,
	                             unk6, additionalCutscene, sound, unkB, unkC, unkE, letterboxSize);
}

std::string ActorCutsceneEntry::GetSourceTypeName() const
{
	return "ActorCutscene";
}
