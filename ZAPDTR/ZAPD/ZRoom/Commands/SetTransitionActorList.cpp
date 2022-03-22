#include "SetTransitionActorList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZNames.h"
#include "ZRoom/ZRoom.h"

SetTransitionActorList::SetTransitionActorList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetTransitionActorList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	int numActors = cmdArg1;
	uint32_t currentPtr = segmentOffset;

	for (int32_t i = 0; i < numActors; i++)
	{
		TransitionActorEntry entry(parent->GetRawData(), currentPtr);
		transitionActors.push_back(entry);

		currentPtr += 16;
	}
}

void SetTransitionActorList::DeclareReferences(const std::string& prefix)
{
	std::string declaration;

	size_t index = 0;
	for (const auto& entry : transitionActors)
	{
		declaration += StringHelper::Sprintf("    { %s },", entry.GetBodySourceCode().c_str());
		if (index + 1 < transitionActors.size())
		{
			declaration += "\n";
		}

		index++;
	}

	parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::Align4, transitionActors.size() * 16,
		"TransitionActorEntry",
		StringHelper::Sprintf("%sTransitionActorList_%06X", prefix.c_str(), segmentOffset),
		transitionActors.size(), declaration);
}

std::string SetTransitionActorList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "TransitionActorEntry", listName);
	return StringHelper::Sprintf("SCENE_CMD_TRANSITION_ACTOR_LIST(%i, %s)", transitionActors.size(),
	                             listName.c_str());
}

std::string SetTransitionActorList::GetCommandCName() const
{
	return "SCmdTransiActorList";
}

RoomCommand SetTransitionActorList::GetRoomCommand() const
{
	return RoomCommand::SetTransitionActorList;
}

TransitionActorEntry::TransitionActorEntry(const std::vector<uint8_t>& rawData, int rawDataIndex)
{
	frontObjectRoom = rawData[rawDataIndex + 0];
	frontTransitionReaction = rawData[rawDataIndex + 1];
	backObjectRoom = rawData[rawDataIndex + 2];
	backTransitionReaction = rawData[rawDataIndex + 3];
	actorNum = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	posX = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	posY = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	posZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
	rotY = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
	initVar = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
}

std::string TransitionActorEntry::GetBodySourceCode() const
{
	std::string actorStr = ZNames::GetActorName(actorNum);

	return StringHelper::Sprintf("%i, %i, %i, %i, %s, %i, %i, %i, %i, 0x%04X", frontObjectRoom,
	                             frontTransitionReaction, backObjectRoom, backTransitionReaction,
	                             actorStr.c_str(), posX, posY, posZ, rotY, initVar);
}
