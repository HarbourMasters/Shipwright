/**
 * File: SetAnimatedMaterialList.cpp
 * Description: Defines a class SetAnimatedMaterialList to enable ZRoom to declare
 * ZTextureAnimations, using that ZResource to do the work.
 */
#include "SetAnimatedMaterialList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"
#include "ZTextureAnimation.h"

SetAnimatedMaterialList::SetAnimatedMaterialList(ZFile* nParent)
	: ZRoomCommand(nParent), textureAnimation(nParent)
{
}

void SetAnimatedMaterialList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	textureAnimation.ExtractFromFile(segmentOffset);
}

void SetAnimatedMaterialList::DeclareReferences(const std::string& prefix)
{
	textureAnimation.SetName(textureAnimation.GetDefaultName(prefix.c_str()));
	textureAnimation.DeclareReferences(prefix);
	textureAnimation.GetSourceOutputCode(prefix);
}

std::string SetAnimatedMaterialList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "AnimatedMaterial", listName);
	return StringHelper::Sprintf("SCENE_CMD_ANIMATED_MATERIAL_LIST(%s)", listName.c_str());
}

std::string SetAnimatedMaterialList::GetCommandCName() const
{
	return "SCmdTextureAnimations";
}

RoomCommand SetAnimatedMaterialList::GetRoomCommand() const
{
	return RoomCommand::SetAnimatedMaterialList;
}
