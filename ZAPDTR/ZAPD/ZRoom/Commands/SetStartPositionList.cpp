#include "SetStartPositionList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZNames.h"
#include "ZRoom/ZRoom.h"

SetStartPositionList::SetStartPositionList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetStartPositionList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	uint8_t numActors = cmdArg1;

	uint32_t currentPtr = segmentOffset;

	for (int32_t i = 0; i < numActors; i++)
	{
		actors.push_back(ActorSpawnEntry(parent->GetRawData(), currentPtr));
		currentPtr += 16;
	}
}

void SetStartPositionList::DeclareReferences(const std::string& prefix)
{
	if (!actors.empty())
	{
		std::string declaration;

		size_t index = 0;
		for (const auto& entry : actors)
		{
			declaration += StringHelper::Sprintf("    { %s },", entry.GetBodySourceCode().c_str());
			if (index + 1 < actors.size())
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::Align4, actors.size() * 16, "ActorEntry",
			StringHelper::Sprintf("%sStartPositionList0x%06X", prefix.c_str(), segmentOffset),
			actors.size(), declaration);
	}
}

std::string SetStartPositionList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "ActorEntry", listName);
	return StringHelper::Sprintf("SCENE_CMD_SPAWN_LIST(%i, %s)", actors.size(), listName.c_str());
}

std::string SetStartPositionList::GetCommandCName() const
{
	return "SCmdSpawnList";
}

RoomCommand SetStartPositionList::GetRoomCommand() const
{
	return RoomCommand::SetStartPositionList;
}
