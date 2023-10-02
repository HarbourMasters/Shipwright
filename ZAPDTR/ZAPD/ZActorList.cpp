#include "ZActorList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "WarningHandler.h"
#include "ZFile.h"
#include "ZRoom/ZNames.h"

REGISTER_ZFILENODE(ActorList, ZActorList);

ZActorList::ZActorList(ZFile* nParent) : ZResource(nParent)
{
	RegisterRequiredAttribute("Count");
}

void ZActorList::ExtractFromBinary(uint32_t nRawDataIndex, uint8_t nNumActors)
{
	rawDataIndex = nRawDataIndex;
	numActors = nNumActors;

	// Don't parse raw data of external files
	if (parent->GetMode() == ZFileMode::ExternalFile)
		return;

	ParseRawData();
}

void ZActorList::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	numActors = StringHelper::StrToL(registeredAttributes.at("Count").value);

	if (numActors < 1)
	{
		HANDLE_ERROR_RESOURCE(
			WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
			StringHelper::Sprintf("invalid value '%d' found for 'NumPaths' attribute", numActors),
			"Should be at least '1'");
	}
}

void ZActorList::ParseRawData()
{
	ZResource::ParseRawData();

	offset_t currentPtr = rawDataIndex;
	size_t largestlength = 0;

	for (size_t i = 0; i < numActors; i++)
	{
		ActorSpawnEntry entry(parent->GetRawData(), currentPtr);

		currentPtr += entry.GetRawDataSize();
		actors.push_back(entry);

		size_t actorNameLength = ZNames::GetActorName(entry.GetActorId()).size();
		if (actorNameLength > largestlength)
			largestlength = actorNameLength;
	}

	for (auto& entry : actors)
	{
		entry.SetLargestActorName(largestlength);
	}
}

Declaration* ZActorList::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), name, GetActorListArraySize(), bodyStr);
	decl->staticConf = staticConf;

	return decl;
}

std::string ZActorList::GetBodySourceCode() const
{
	std::string declaration;

	size_t index = 0;
	for (auto& entry : actors)
	{
		declaration += StringHelper::Sprintf("\t{ %s },", entry.GetBodySourceCode().c_str());

		if (index < actors.size() - 1)
			declaration += "\n";

		index++;
	}

	return declaration;
}

std::string ZActorList::GetSourceTypeName() const
{
	return actors.front().GetSourceTypeName();
}

ZResourceType ZActorList::GetResourceType() const
{
	return ZResourceType::ActorList;
}

size_t ZActorList::GetRawDataSize() const
{
	return actors.size() * actors.front().GetRawDataSize();
}

size_t ZActorList::GetActorListArraySize() const
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

/* ActorSpawnEntry */

ActorSpawnEntry::ActorSpawnEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	actorNum = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	posX = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	posY = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	posZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	rotX = BitConverter::ToUInt16BE(rawData, rawDataIndex + 8);
	rotY = BitConverter::ToUInt16BE(rawData, rawDataIndex + 10);
	rotZ = BitConverter::ToUInt16BE(rawData, rawDataIndex + 12);
	params = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
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
	body += StringHelper::Sprintf("0x%04X", params);

	return body;
}

std::string ActorSpawnEntry::GetSourceTypeName() const
{
	return "ActorEntry";
}

size_t ActorSpawnEntry::GetRawDataSize() const
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
