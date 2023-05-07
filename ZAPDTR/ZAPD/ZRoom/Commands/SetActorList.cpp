#include "SetActorList.h"

#include <cassert>

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZRoom/ZRoom.h"

SetActorList::SetActorList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetActorList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	numActors = cmdArg1;

	actorList = new ZActorList(parent);
	actorList->ExtractFromBinary(segmentOffset, numActors);
}

void SetActorList::DeclareReferences(const std::string& prefix)
{
	if (parent->HasDeclaration(segmentOffset))
	{
		delete actorList;
		actorList = static_cast<ZActorList*>(parent->FindResource(segmentOffset));
		assert(actorList != nullptr);
		assert(actorList->GetResourceType() == ZResourceType::ActorList);
		return;
	}

	if (actorList->GetName() == "")
	{
		actorList->SetName(actorList->GetDefaultName(prefix));
	}
	actorList->DeclareVar(prefix, "");
	parent->AddResource(actorList);
}

std::string SetActorList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "ActorEntry", listName,
	                                       parent->workerID);

	return StringHelper::Sprintf("SCENE_CMD_ACTOR_LIST(%i, %s)", numActors, listName.c_str());
}

std::string SetActorList::GetCommandCName() const
{
	return "SCmdActorList";
}

RoomCommand SetActorList::GetRoomCommand() const
{
	return RoomCommand::SetActorList;
}
