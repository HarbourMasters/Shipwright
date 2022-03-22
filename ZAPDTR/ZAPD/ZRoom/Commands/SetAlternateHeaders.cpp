#include "SetAlternateHeaders.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

SetAlternateHeaders::SetAlternateHeaders(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetAlternateHeaders::DeclareReferences([[maybe_unused]] const std::string& prefix)
{
	if (cmdArg2 != 0)
	{
		std::string varName =
			StringHelper::Sprintf("%sAlternateHeaders0x%06X", prefix.c_str(), segmentOffset);
		parent->AddDeclarationPlaceholder(segmentOffset, varName);
	}
}

void SetAlternateHeaders::ParseRawDataLate()
{
	int numHeaders = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 4;

	for (int32_t i = 0; i < numHeaders; i++)
	{
		int32_t address = BitConverter::ToInt32BE(parent->GetRawData(), segmentOffset + (i * 4));
		headers.push_back(address);

		if (address != 0 && parent->GetDeclaration(GETSEGOFFSET(address)) == nullptr)
		{
			ZRoom* altheader = new ZRoom(parent);
			altheader->ExtractFromBinary(GETSEGOFFSET(address), zRoom->GetResourceType());
			altheader->DeclareReferences(parent->GetName());

			parent->resources.push_back(altheader);
		}
	}
}

void SetAlternateHeaders::DeclareReferencesLate(const std::string& prefix)
{
	if (!headers.empty())
	{
		std::string declaration;

		for (size_t i = 0; i < headers.size(); i++)
		{
			std::string altHeaderName;
			Globals::Instance->GetSegmentedPtrName(headers.at(i), parent, "", altHeaderName);

			declaration += StringHelper::Sprintf("\t%s,", altHeaderName.c_str());

			if (i + 1 < headers.size())
				declaration += "\n";
		}

		std::string varName =
			StringHelper::Sprintf("%sAlternateHeaders0x%06X", prefix.c_str(), segmentOffset);
		parent->AddDeclarationArray(segmentOffset, GetDeclarationAlignment(), headers.size() * 4,
		                            "SceneCmd*", varName, headers.size(), declaration);
	}
}

std::string SetAlternateHeaders::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "SceneCmd*", listName);
	return StringHelper::Sprintf("SCENE_CMD_ALTERNATE_HEADER_LIST(%s)", listName.c_str());
}

std::string SetAlternateHeaders::GetCommandCName() const
{
	return "SCmdAltHeaders";
}

RoomCommand SetAlternateHeaders::GetRoomCommand() const
{
	return RoomCommand::SetAlternateHeaders;
}
