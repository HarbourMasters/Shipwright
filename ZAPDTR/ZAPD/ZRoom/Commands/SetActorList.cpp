#include "SetActorList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZNames.h"
#include "ZRoom/ZRoom.h"

SetActorList::SetActorList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetActorList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	numActors = cmdArg1;
}

void SetActorList::DeclareReferences(const std::string& prefix)
{
	if (numActors != 0 && cmdArg2 != 0)
	{
		std::string varName =
			StringHelper::Sprintf("%sActorList_%06X", prefix.c_str(), segmentOffset);
		parent->AddDeclarationPlaceholder(segmentOffset, varName);
	}
}

void SetActorList::ParseRawDataLate()
{
	ZRoomCommand::ParseRawDataLate();
	size_t actorsAmount = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 0x10;

	uint32_t currentPtr = segmentOffset;

	for (size_t i = 0; i < actorsAmount; i++)
	{
		ActorSpawnEntry entry(parent->GetRawData(), currentPtr);

		currentPtr += entry.GetRawDataSize();
		actors.push_back(entry);
	}
}

void SetActorList::DeclareReferencesLate(const std::string& prefix)
{
	if (actors.empty())
		return;

	std::string declaration;

	size_t largestlength = 0;
	for (const auto& entry : actors)
	{
		size_t actorNameLength = ZNames::GetActorName(entry.GetActorId()).size();
		if (actorNameLength > largestlength)
			largestlength = actorNameLength;
	}

	size_t index = 0;
	for (auto& entry : actors)
	{
		entry.SetLargestActorName(largestlength);
		declaration += StringHelper::Sprintf("\t{ %s },", entry.GetBodySourceCode().c_str());

		if (index < actors.size() - 1)
			declaration += "\n";

		index++;
	}

	const auto& entry = actors.front();

	std::string varName = StringHelper::Sprintf("%sActorList_%06X", prefix.c_str(), segmentOffset);
	parent->AddDeclarationArray(segmentOffset, DeclarationAlignment::Align4,
	                            actors.size() * entry.GetRawDataSize(), entry.GetSourceTypeName(),
	                            varName, GetActorListArraySize(), declaration);
}

std::string SetActorList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "ActorEntry", listName);
	if (numActors != actors.size())
	{
		printf("%s: numActors(%i) ~ actors(%li)\n", parent->GetName().c_str(), numActors,
		       actors.size());
	}
	return StringHelper::Sprintf("SCENE_CMD_ACTOR_LIST(%i, %s)", numActors, listName.c_str());
}

size_t SetActorList::GetActorListArraySize() const
{
	size_t actorCount = 0;

	// Doing an else-if here so we only do the loop when the game is SW97.
	// Actor 0x22 is removed from SW97, so we need to ensure that we don't increment the actor count
	// for it.
	if (Globals::Instance->game == ZGame::OOT_SW97)
	{
		actorCount = 0;

		for (const auto& entry : actors)
			if (entry.GetActorId() != 0x22)
				actorCount++;
	}
	else
	{
		actorCount = actors.size();
	}

	return actorCount;
}

std::string SetActorList::GetCommandCName() const
{
	return "SCmdActorList";
}

RoomCommand SetActorList::GetRoomCommand() const
{
	return RoomCommand::SetActorList;
}

ActorSpawnEntry::ActorSpawnEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	actorNum = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	posX = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	posY = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	posZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	rotX = BitConverter::ToUInt16BE(rawData, rawDataIndex + 8);
	rotY = BitConverter::ToUInt16BE(rawData, rawDataIndex + 10);
	rotZ = BitConverter::ToUInt16BE(rawData, rawDataIndex + 12);
	initVar = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
}

std::string ActorSpawnEntry::GetBodySourceCode() const
{
	std::string body;

	std::string actorNameFmt = StringHelper::Sprintf("%%-%zus ", largestActorName + 1);
	body =
		StringHelper::Sprintf(actorNameFmt.c_str(), (ZNames::GetActorName(actorNum) + ",").c_str());

	body += StringHelper::Sprintf("{ %6i, %6i, %6i }, ", posX, posY, posZ);
	if (Globals::Instance->game == ZGame::MM_RETAIL)
		body += StringHelper::Sprintf("{ SPAWN_ROT_FLAGS(%#5hX, 0x%04X)"
		                              ", SPAWN_ROT_FLAGS(%#5hX, 0x%04X)"
		                              ", SPAWN_ROT_FLAGS(%#5hX, 0x%04X) }, ",
		                              (rotX >> 7) & 0b111111111, rotX & 0b1111111,
		                              (rotY >> 7) & 0b111111111, rotY & 0b1111111,
		                              (rotZ >> 7) & 0b111111111, rotZ & 0b1111111);
	else
		body += StringHelper::Sprintf("{ %#6hX, %#6hX, %#6hX }, ", rotX, rotY, rotZ);
	body += StringHelper::Sprintf("0x%04X", initVar);

	return body;
}

std::string ActorSpawnEntry::GetSourceTypeName() const
{
	return "ActorEntry";
}

int32_t ActorSpawnEntry::GetRawDataSize() const
{
	return 16;
}

uint16_t ActorSpawnEntry::GetActorId() const
{
	return actorNum;
}

void ActorSpawnEntry::SetLargestActorName(size_t nameSize)
{
	largestActorName = nameSize;
}
