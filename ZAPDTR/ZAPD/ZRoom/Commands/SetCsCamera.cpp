#include "SetCsCamera.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetCsCamera::SetCsCamera(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetCsCamera::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	int numCameras = cmdArg1;

	uint32_t currentPtr = segmentOffset;
	int32_t numPoints = 0;

	for (int32_t i = 0; i < numCameras; i++)
	{
		CsCameraEntry entry(parent->GetRawData(), currentPtr);
		numPoints += entry.GetNumPoints();

		currentPtr += entry.GetRawDataSize();
		cameras.push_back(entry);
	}

	if (numPoints > 0)
	{
		uint32_t currentPtr = cameras.at(0).GetSegmentOffset();

		for (int32_t i = 0; i < numPoints; i++)
		{
			ZVector vec(parent);
			vec.ExtractFromBinary(currentPtr, ZScalarType::ZSCALAR_S16, 3);

			currentPtr += vec.GetRawDataSize();
			points.push_back(vec);
		}
	}
}

void SetCsCamera::DeclareReferences(const std::string& prefix)
{
	if (points.size() > 0)
	{
		std::string declaration;
		size_t index = 0;
		for (auto& point : points)
		{
			declaration += StringHelper::Sprintf("\t{ %s },", point.GetBodySourceCode().c_str());

			if (index < points.size() - 1)
				declaration += "\n";

			index++;
		}

		uint32_t segOffset = cameras.at(0).GetSegmentOffset();

		parent->AddDeclarationArray(
			segOffset, DeclarationAlignment::Align4, points.size() * points.at(0).GetRawDataSize(),
			points.at(0).GetSourceTypeName().c_str(),
			StringHelper::Sprintf("%sCsCameraPoints_%06X", prefix.c_str(), segOffset),
			points.size(), declaration);
	}

	if (!cameras.empty())
	{
		std::string camPointsName;
		Globals::Instance->GetSegmentedPtrName(cameras.at(0).GetCamAddress(), parent, "Vec3s",
		                                       camPointsName);
		std::string declaration;

		size_t index = 0;
		size_t pointsIndex = 0;
		for (const auto& entry : cameras)
		{
			declaration +=
				StringHelper::Sprintf("\t{ %i, %i, &%s[%i] },", entry.type, entry.numPoints,
			                          camPointsName.c_str(), pointsIndex);

			if (index < cameras.size() - 1)
				declaration += "\n";

			index++;
			pointsIndex += entry.GetNumPoints();
		}

		const auto& entry = cameras.front();
		std::string camTypename = entry.GetSourceTypeName();

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::Align4, cameras.size() * entry.GetRawDataSize(),
			camTypename,
			StringHelper::Sprintf("%s%s_%06X", prefix.c_str(), camTypename.c_str(), segmentOffset),
			cameras.size(), declaration);
	}
}

std::string SetCsCamera::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "CsCameraEntry", listName);
	return StringHelper::Sprintf("SCENE_CMD_ACTOR_CUTSCENE_CAM_LIST(%i, %s)", cameras.size(),
	                             listName.c_str());
}

std::string SetCsCamera::GetCommandCName() const
{
	return "SCmdCsCameraList";
}

RoomCommand SetCsCamera::GetRoomCommand() const
{
	return RoomCommand::SetCsCamera;
}

CsCameraEntry::CsCameraEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: baseOffset(rawDataIndex), type(BitConverter::ToInt16BE(rawData, rawDataIndex + 0)),
	  numPoints(BitConverter::ToInt16BE(rawData, rawDataIndex + 2))
{
	camAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
	segmentOffset = GETSEGOFFSET(camAddress);
}

std::string CsCameraEntry::GetSourceTypeName() const
{
	return "CsCameraEntry";
}

int32_t CsCameraEntry::GetRawDataSize() const
{
	return 8;
}

int16_t CsCameraEntry::GetNumPoints() const
{
	return numPoints;
}

segptr_t CsCameraEntry::GetCamAddress() const
{
	return camAddress;
}

uint32_t CsCameraEntry::GetSegmentOffset() const
{
	return segmentOffset;
}
