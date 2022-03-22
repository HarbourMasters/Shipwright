#include "SetLightingSettings.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetLightingSettings::SetLightingSettings(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetLightingSettings::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	uint8_t numLights = cmdArg1;

	for (int i = 0; i < numLights; i++)
		settings.push_back(LightingSettings(parent->GetRawData(), segmentOffset + (i * 22)));
}

void SetLightingSettings::DeclareReferences(const std::string& prefix)
{
	if (settings.size() > 0)
	{
		std::string declaration;

		for (size_t i = 0; i < settings.size(); i++)
		{
			declaration +=
				StringHelper::Sprintf("\t{ %s },", settings.at(i).GetBodySourceCode().c_str());
			if (i + 1 < settings.size())
				declaration += "\n";
		}

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::Align4,
			settings.size() * settings.front().GetRawDataSize(), "LightSettings",
			StringHelper::Sprintf("%sLightSettings0x%06X", prefix.c_str(), segmentOffset),
			settings.size(), declaration);
	}
}

std::string SetLightingSettings::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "LightSettings", listName);
	return StringHelper::Sprintf("SCENE_CMD_ENV_LIGHT_SETTINGS(%i, %s)", settings.size(),
	                             listName.c_str());
}

std::string SetLightingSettings::GetCommandCName() const
{
	return "SCmdLightSettingList";
}

RoomCommand SetLightingSettings::GetRoomCommand() const
{
	return RoomCommand::SetLightingSettings;
}

LightingSettings::LightingSettings(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	ambientClrR = rawData.at(rawDataIndex + 0);
	ambientClrG = rawData.at(rawDataIndex + 1);
	ambientClrB = rawData.at(rawDataIndex + 2);

	diffuseClrA_R = rawData.at(rawDataIndex + 3);
	diffuseClrA_G = rawData.at(rawDataIndex + 4);
	diffuseClrA_B = rawData.at(rawDataIndex + 5);

	diffuseDirA_X = rawData.at(rawDataIndex + 6);
	diffuseDirA_Y = rawData.at(rawDataIndex + 7);
	diffuseDirA_Z = rawData.at(rawDataIndex + 8);

	diffuseClrB_R = rawData.at(rawDataIndex + 9);
	diffuseClrB_G = rawData.at(rawDataIndex + 10);
	diffuseClrB_B = rawData.at(rawDataIndex + 11);

	diffuseDirB_X = rawData.at(rawDataIndex + 12);
	diffuseDirB_Y = rawData.at(rawDataIndex + 13);
	diffuseDirB_Z = rawData.at(rawDataIndex + 14);

	fogClrR = rawData.at(rawDataIndex + 15);
	fogClrG = rawData.at(rawDataIndex + 16);
	fogClrB = rawData.at(rawDataIndex + 17);

	unk = BitConverter::ToInt16BE(rawData, rawDataIndex + 18);
	drawDistance = BitConverter::ToInt16BE(rawData, rawDataIndex + 20);
}

std::string LightingSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf(
		"0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, "
		"0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%04X, 0x%04X",
		ambientClrR, ambientClrG, ambientClrB, diffuseClrA_R, diffuseClrA_G, diffuseClrA_B,
		diffuseDirA_X, diffuseDirA_Y, diffuseDirA_Z, diffuseClrB_R, diffuseClrB_G, diffuseClrB_B,
		diffuseDirB_X, diffuseDirB_Y, diffuseDirB_Z, fogClrR, fogClrG, fogClrB, unk, drawDistance);
}

size_t LightingSettings::GetRawDataSize() const
{
	return 0x16;
}
