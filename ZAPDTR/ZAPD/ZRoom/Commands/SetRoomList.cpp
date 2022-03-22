#include "SetRoomList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetRoomList::SetRoomList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetRoomList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	int numRooms = cmdArg1;

	romfile = new RomFile(parent);
	romfile->numRooms = numRooms;
	romfile->ExtractFromFile(segmentOffset);

	parent->resources.push_back(romfile);

	zRoom->roomCount = numRooms;
}

void SetRoomList::DeclareReferences(const std::string& prefix)
{
	ZRoomCommand::DeclareReferences(prefix);

	romfile->DeclareVar(prefix, "");
}

std::string SetRoomList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "RomFile", listName);
	return StringHelper::Sprintf("SCENE_CMD_ROOM_LIST(%i, %s)", romfile->rooms.size(),
	                             listName.c_str());
}

std::string SetRoomList::GetCommandCName() const
{
	return "SCmdRoomList";
}

RoomCommand SetRoomList::GetRoomCommand() const
{
	return RoomCommand::SetRoomList;
}

RomFile::RomFile(ZFile* nParent) : ZResource(nParent)
{
}

void RomFile::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	if (reader->Attribute("NumRooms") != nullptr)
	{
		numRooms = StringHelper::StrToL(std::string(reader->Attribute("NumRooms")));
	}
}

void RomFile::ParseRawData()
{
	ZResource::ParseRawData();

	uint32_t currentPtr = rawDataIndex;

	for (int32_t i = 0; i < numRooms; i++)
	{
		RoomEntry entry(parent->GetRawData(), currentPtr);
		rooms.push_back(entry);

		currentPtr += 8;
	}
}

Declaration* RomFile::DeclareVar(const std::string& prefix, const std::string& body)
{
	std::string auxName = name;
	if (name == "")
		auxName = StringHelper::Sprintf("%sRoomList0x%06X", prefix.c_str(), rawDataIndex);

	return parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align4,
	                                   rooms.size() * rooms.at(0).GetRawDataSize(),
	                                   GetSourceTypeName(), auxName, rooms.size(), body);
}

std::string RomFile::GetBodySourceCode() const
{
	std::string declaration;
	bool isFirst = true;

	for (ZFile* file : Globals::Instance->files)
	{
		for (ZResource* res : file->resources)
		{
			if (res->GetResourceType() == ZResourceType::Room)
			{
				std::string roomName = res->GetName();
				if (!isFirst)
					declaration += "\n";

				declaration +=
					StringHelper::Sprintf("\t{ (u32)_%sSegmentRomStart, (u32)_%sSegmentRomEnd },",
				                          roomName.c_str(), roomName.c_str());
				isFirst = false;
			}
		}
	}

	return declaration;
}

void RomFile::GetSourceOutputCode(const std::string& prefix)
{
	DeclareVar(prefix, GetBodySourceCode());
}

std::string RomFile::GetSourceTypeName() const
{
	return "RomFile";
}

ZResourceType RomFile::GetResourceType() const
{
	// TODO
	return ZResourceType::Error;
}

size_t RomFile::GetRawDataSize() const
{
	return 8 * rooms.size();
}

RoomEntry::RoomEntry(uint32_t nVAS, uint32_t nVAE)
{
	virtualAddressStart = nVAS;
	virtualAddressEnd = nVAE;
}

RoomEntry::RoomEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: RoomEntry(BitConverter::ToInt32BE(rawData, rawDataIndex + 0),
                BitConverter::ToInt32BE(rawData, rawDataIndex + 4))
{
}

size_t RoomEntry::GetRawDataSize() const
{
	return 0x08;
}
