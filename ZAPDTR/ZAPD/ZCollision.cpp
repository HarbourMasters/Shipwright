#include "ZCollision.h"

#include <cassert>
#include <cstdint>
#include <string>

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

REGISTER_ZFILENODE(Collision, ZCollisionHeader);

ZCollisionHeader::ZCollisionHeader(ZFile* nParent) : ZResource(nParent)
{
	genOTRDef = true;
}

ZCollisionHeader::~ZCollisionHeader()
{
	delete camData;
}

void ZCollisionHeader::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	absMinX = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	absMinY = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	absMinZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);

	absMaxX = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	absMaxY = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	absMaxZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);

	numVerts = BitConverter::ToUInt16BE(rawData, rawDataIndex + 12);
	vtxAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 16);

	numPolygons = BitConverter::ToUInt16BE(rawData, rawDataIndex + 20);
	polyAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 24);
	polyTypeDefAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 28);
	camDataAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 32);

	numWaterBoxes = BitConverter::ToUInt16BE(rawData, rawDataIndex + 36);
	waterBoxAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 40);

	vtxSegmentOffset = Seg2Filespace(vtxAddress, parent->baseAddress);
	polySegmentOffset = Seg2Filespace(polyAddress, parent->baseAddress);
	polyTypeDefSegmentOffset = Seg2Filespace(polyTypeDefAddress, parent->baseAddress);
	camDataSegmentOffset = Seg2Filespace(camDataAddress, parent->baseAddress);
	waterBoxSegmentOffset = Seg2Filespace(waterBoxAddress, parent->baseAddress);

	vertices.reserve(numVerts);
	polygons.reserve(numPolygons);

	offset_t currentPtr = vtxSegmentOffset;

	for (uint16_t i = 0; i < numVerts; i++)
	{
		ZVector vec(parent);
		vec.ExtractFromBinary(currentPtr, ZScalarType::ZSCALAR_S16, 3);

		currentPtr += vec.GetRawDataSize();
		vertices.push_back(vec);
	}

	for (uint16_t i = 0; i < numPolygons; i++)
	{
		ZCollisionPoly poly(parent);
		poly.SetRawDataIndex(polySegmentOffset + (i * 16));
		poly.ParseRawData();
		polygons.push_back(poly);
	}

	uint16_t highestPolyType = 0;

	for (const ZCollisionPoly& poly : polygons)
	{
		if (poly.type > highestPolyType)
			highestPolyType = poly.type;
	}

	for (uint16_t i = 0; i < highestPolyType + 1; i++)
	{
		ZSurfaceType surfaceType(parent);
		surfaceType.SetRawDataIndex(polyTypeDefSegmentOffset + (i * 8));
		surfaceType.ParseRawData();
		PolygonTypes.push_back(surfaceType);
	}
	// polygonTypes.push_back(
	//	BitConverter::ToUInt64BE(rawData, polyTypeDefSegmentOffset + (i * 8)));

	if (camDataAddress != SEGMENTED_NULL)
	{
		// Try to guess how many elements the CamDataList array has.
		// The "guessing algorithm" is basically a "best effort" one and it
		// is error-prone.
		// This is based mostly on observation of how CollisionHeader data is
		// usually ordered. If for some reason the data was in some other funny
		// order, this would probably break.
		// The most common ordering is:
		// - *CamData*
		// - SurfaceType
		// - CollisionPoly
		// - Vertices
		// - WaterBoxes
		// - CollisionHeader
		offset_t upperCameraBoundary = polyTypeDefSegmentOffset;
		if (upperCameraBoundary == 0)
		{
			upperCameraBoundary = polySegmentOffset;
		}
		if (upperCameraBoundary == 0)
		{
			upperCameraBoundary = vtxSegmentOffset;
		}
		if (upperCameraBoundary == 0)
		{
			upperCameraBoundary = waterBoxSegmentOffset;
		}
		if (upperCameraBoundary == 0)
		{
			upperCameraBoundary = rawDataIndex;
		}

		camData =
			new CameraDataList(parent, name, rawData, camDataSegmentOffset, upperCameraBoundary);
	}

	for (uint16_t i = 0; i < numWaterBoxes; i++)
		waterBoxes.push_back(WaterBoxHeader(
			rawData,
			waterBoxSegmentOffset + (i * (Globals::Instance->game == ZGame::OOT_SW97 ? 12 : 16))));
}

void ZCollisionHeader::DeclareReferences(const std::string& prefix)
{
	std::string declaration = "";
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	if (waterBoxes.size() > 0)
	{
		if (!Globals::Instance->otrMode)
		{
			for (size_t i = 0; i < waterBoxes.size(); i++)
			{
				declaration +=
					StringHelper::Sprintf("\t{ %s },", waterBoxes[i].GetBodySourceCode().c_str());
				if (i + 1 < waterBoxes.size())
					declaration += "\n";
			}
		}

		parent->AddDeclarationArray(
			waterBoxSegmentOffset, DeclarationAlignment::Align4, 16 * waterBoxes.size(), "WaterBox",
			StringHelper::Sprintf("%sWaterBoxes", auxName.c_str()), waterBoxes.size(), declaration);
	}

	if (polygons.size() > 0)
	{
		declaration.clear();

		if (!Globals::Instance->otrMode)
		{
			for (size_t i = 0; i < polygons.size(); i++)
			{
				declaration +=
					StringHelper::Sprintf("\t%s,", polygons[i].GetBodySourceCode().c_str());
				if (i + 1 < polygons.size())
					declaration += "\n";
			}
		}

		parent->AddDeclarationArray(polySegmentOffset, DeclarationAlignment::Align4,
		                            polygons.size() * 16, polygons[0].GetSourceTypeName().c_str(),
		                            StringHelper::Sprintf("%sPolygons", auxName.c_str()),
		                            polygons.size(), declaration);
	}

	declaration.clear();
	for (const auto& polyType : PolygonTypes)
	{
		declaration += StringHelper::Sprintf("\t%s,", polyType.GetBodySourceCode().c_str());
	}

	if (polyTypeDefAddress != SEGMENTED_NULL)
		parent->AddDeclarationArray(polyTypeDefSegmentOffset, DeclarationAlignment::Align4,
		                            PolygonTypes.size() * 8,
		                            PolygonTypes[0].GetSourceTypeName().c_str(),
		                            StringHelper::Sprintf("%sSurfaceType", auxName.c_str()),
		                            PolygonTypes.size(), declaration);

	declaration.clear();

	if (vertices.size() > 0)
	{
		declaration.clear();

		if (!Globals::Instance->otrMode)
		{
			for (size_t i = 0; i < vertices.size(); i++)
			{
				declaration +=
					StringHelper::Sprintf("\t{ %s },", vertices[i].GetBodySourceCode().c_str());

				if (i < vertices.size() - 1)
					declaration += "\n";
			}
		}

		const auto& first = vertices.front();
		if (vtxAddress != 0)
			parent->AddDeclarationArray(
				vtxSegmentOffset, first.GetDeclarationAlignment(),
				vertices.size() * first.GetRawDataSize(), first.GetSourceTypeName(),
				StringHelper::Sprintf("%sVertices", auxName.c_str()), vertices.size(), declaration);
	}
}

std::string ZCollisionHeader::GetBodySourceCode() const
{
	std::string declaration = "";

	if (Globals::Instance->otrMode)
		return declaration;

	declaration += "\n";

	declaration += StringHelper::Sprintf("\t{ %i, %i, %i },\n", absMinX, absMinY, absMinZ);
	declaration += StringHelper::Sprintf("\t{ %i, %i, %i },\n", absMaxX, absMaxY, absMaxZ);

	std::string vtxName;
	Globals::Instance->GetSegmentedPtrName(vtxAddress, parent, "Vec3s", vtxName, parent->workerID);
	declaration += StringHelper::Sprintf("\t%i,\n\t%s,\n", numVerts, vtxName.c_str());

	std::string polyName;
	Globals::Instance->GetSegmentedPtrName(polyAddress, parent, "CollisionPoly", polyName,
	                                       parent->workerID);
	declaration += StringHelper::Sprintf("\t%i,\n\t%s,\n", numPolygons, polyName.c_str());

	std::string surfaceName;
	Globals::Instance->GetSegmentedPtrName(polyTypeDefAddress, parent, "SurfaceType", surfaceName,
	                                       parent->workerID);
	declaration += StringHelper::Sprintf("\t%s,\n", surfaceName.c_str());

	std::string camName;
	Globals::Instance->GetSegmentedPtrName(camDataAddress, parent, "CamData", camName,
	                                       parent->workerID);
	declaration += StringHelper::Sprintf("\t%s,\n", camName.c_str());

	std::string waterBoxName;
	Globals::Instance->GetSegmentedPtrName(waterBoxAddress, parent, "WaterBox", waterBoxName,
	                                       parent->workerID);
	declaration += StringHelper::Sprintf("\t%i,\n\t%s\n", numWaterBoxes, waterBoxName.c_str());

	return declaration;
}

std::string ZCollisionHeader::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sCol_%06X", prefix.c_str(), rawDataIndex);
}

std::string ZCollisionHeader::GetSourceTypeName() const
{
	return "CollisionHeader";
}

ZResourceType ZCollisionHeader::GetResourceType() const
{
	return ZResourceType::CollisionHeader;
}

size_t ZCollisionHeader::GetRawDataSize() const
{
	return 44;
}

WaterBoxHeader::WaterBoxHeader(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	xMin = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	ySurface = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	zMin = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	xLength = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	zLength = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);

	if (Globals::Instance->game == ZGame::OOT_SW97)
		properties = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
	else
		properties = BitConverter::ToInt32BE(rawData, rawDataIndex + 12);
}

std::string WaterBoxHeader::GetBodySourceCode() const
{
	return StringHelper::Sprintf("%i, %i, %i, %i, %i, 0x%08X", xMin, ySurface, zMin, xLength,
	                             zLength, properties);
}

CameraDataList::CameraDataList(ZFile* parent, const std::string& prefix,
                               const std::vector<uint8_t>& rawData, offset_t rawDataIndex,
                               offset_t upperCameraBoundary)
{
	std::string declaration;

	// Parse CameraDataEntries
	size_t numElements = (upperCameraBoundary - rawDataIndex) / 8;
	assert(numElements < 10000);

	offset_t cameraPosDataSeg = rawDataIndex;
	for (size_t i = 0; i < numElements; i++)
	{
		CameraDataEntry* entry = new CameraDataEntry();

		entry->cameraSType =
			BitConverter::ToInt16BE(rawData, rawDataIndex + (entries.size() * 8) + 0);
		entry->numData = BitConverter::ToInt16BE(rawData, rawDataIndex + (entries.size() * 8) + 2);
		entry->cameraPosDataSeg =
			BitConverter::ToInt32BE(rawData, rawDataIndex + (entries.size() * 8) + 4);

		if (entry->cameraPosDataSeg != 0 && GETSEGNUM(entry->cameraPosDataSeg) != SEGMENT_SCENE)
		{
			cameraPosDataSeg = rawDataIndex + (entries.size() * 8);
			break;
		}

		if (entry->cameraPosDataSeg != 0 && cameraPosDataSeg > (entry->cameraPosDataSeg & 0xFFFFFF))
			cameraPosDataSeg = (entry->cameraPosDataSeg & 0xFFFFFF);

		entries.push_back(entry);
	}

	// Setting cameraPosDataAddr to rawDataIndex give a pos list length of 0
	uint32_t cameraPosDataOffset = cameraPosDataSeg & 0xFFFFFF;
	for (size_t i = 0; i < entries.size(); i++)
	{
		char camSegLine[2048];

		if (entries[i]->cameraPosDataSeg != 0)
		{
			int32_t index =
				((entries[i]->cameraPosDataSeg & 0x00FFFFFF) - cameraPosDataOffset) / 0x6;
			sprintf(camSegLine, "&%sCamPosData[%i]", prefix.c_str(), index);
		}
		else
			sprintf(camSegLine, "NULL");

		declaration +=
			StringHelper::Sprintf("    { 0x%04X, %i, %s },", entries[i]->cameraSType,
		                          entries[i]->numData, camSegLine, rawDataIndex + (i * 8));

		if (i < entries.size() - 1)
			declaration += "\n";
	}

	parent->AddDeclarationArray(
		rawDataIndex, DeclarationAlignment::Align4, entries.size() * 8, "CamData",
		StringHelper::Sprintf("%sCamDataList", prefix.c_str(), rawDataIndex), entries.size(),
		declaration);

	uint32_t numDataTotal = (rawDataIndex - cameraPosDataOffset) / 0x6;

	if (numDataTotal > 0)
	{
		declaration.clear();
		for (uint32_t i = 0; i < numDataTotal; i++)
		{
			CameraPositionData* data =
				new CameraPositionData(rawData, cameraPosDataOffset + (i * 6));
			cameraPositionData.push_back(data);

			declaration += StringHelper::Sprintf("\t{ %6i, %6i, %6i },", data->x, data->y, data->z);
			if (i + 1 < numDataTotal)
				declaration += "\n";
		}

		int32_t cameraPosDataIndex = GETSEGOFFSET(cameraPosDataSeg);
		uint32_t entrySize = numDataTotal * 0x6;
		parent->AddDeclarationArray(cameraPosDataIndex, DeclarationAlignment::Align4, entrySize,
		                            "Vec3s", StringHelper::Sprintf("%sCamPosData", prefix.c_str()),
		                            numDataTotal, declaration);
	}
}

CameraDataList::~CameraDataList()
{
	for (auto entry : entries)
		delete entry;

	for (auto camPosData : cameraPositionData)
		delete camPosData;
}

CameraPositionData::CameraPositionData(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	x = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	y = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	z = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
}
