#include "Unused1D.h"
#include "Utils/StringHelper.h"

Unused1D::Unused1D(ZFile* nParent) : ZRoomCommand(nParent)
{
}

std::string Unused1D::GetBodySourceCode() const
{
	return StringHelper::Sprintf("{ %s, 0x00, 0x00 }", GetCommandHex().c_str());
}

std::string Unused1D::GetCommandCName() const
{
	return "SceneCmd";
}

RoomCommand Unused1D::GetRoomCommand() const
{
	return RoomCommand::Unused1D;
}
