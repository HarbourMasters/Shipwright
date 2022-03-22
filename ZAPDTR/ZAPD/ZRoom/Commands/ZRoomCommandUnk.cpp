#include "ZRoomCommandUnk.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZRoom/ZRoom.h"

ZRoomCommandUnk::ZRoomCommandUnk(ZFile* nParent) : ZRoomCommand(nParent)
{
}

std::string ZRoomCommandUnk::GetBodySourceCode() const
{
	return StringHelper::Sprintf("{ %s, 0x%02X, 0x%06X } /* WARNING: "
	                             "UNIMPLEMENTED ROOM COMMAND */",
	                             GetCommandHex().c_str(), cmdArg1, cmdArg2);
}

RoomCommand ZRoomCommandUnk::GetRoomCommand() const
{
	return RoomCommand::Error;
}
