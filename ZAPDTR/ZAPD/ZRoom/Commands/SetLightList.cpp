#include "SetLightList.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

SetLightList::SetLightList(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetLightList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	std::string declarations;

	numLights = cmdArg1;
	int32_t currentPtr = segmentOffset;
	for (int i = 0; i < this->numLights; i++)
	{
		LightInfo light(parent->GetRawData(), currentPtr);

		currentPtr += light.GetRawDataSize();
		lights.push_back(light);
	}
}

void SetLightList::DeclareReferences(const std::string& prefix)
{
	if (!lights.empty())
	{
		std::string declarations;

		for (size_t i = 0; i < lights.size(); i++)
		{
			declarations +=
				StringHelper::Sprintf("\t{ %s },", lights.at(i).GetBodySourceCode().c_str());

			if (i < lights.size() - 1)
				declarations += "\n";
		}

		const auto& light = lights.front();

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::Align4, lights.size() * light.GetRawDataSize(),
			light.GetSourceTypeName(),
			StringHelper::Sprintf("%sLightInfo0x%06X", prefix.c_str(), segmentOffset),
			lights.size(), declarations);
	}
}

std::string SetLightList::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "LightInfo", listName);
	return StringHelper::Sprintf("SCENE_CMD_LIGHT_LIST(%i, %s)", numLights, listName.c_str());
}

std::string SetLightList::GetCommandCName() const
{
	return "SCmdLightList";
}

RoomCommand SetLightList::GetRoomCommand() const
{
	return RoomCommand::SetLightList;
}

LightInfo::LightInfo(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	type = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0);
	x = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	y = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	z = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	r = BitConverter::ToUInt8BE(rawData, rawDataIndex + 8);
	g = BitConverter::ToUInt8BE(rawData, rawDataIndex + 9);
	b = BitConverter::ToUInt8BE(rawData, rawDataIndex + 10);
	drawGlow = BitConverter::ToUInt8BE(rawData, rawDataIndex + 11);
	radius = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
}

std::string LightInfo::GetBodySourceCode() const
{
	return StringHelper::Sprintf(
		"0x%02X, { %i, %i, %i, { 0x%02X, 0x%02X, 0x%02X }, 0x%02X, 0x%04X }", type, x, y, z, r, g,
		b, drawGlow, radius);
}

std::string LightInfo::GetSourceTypeName() const
{
	return "LightInfo";
}

size_t LightInfo::GetRawDataSize() const
{
	return 0x0E;
}
