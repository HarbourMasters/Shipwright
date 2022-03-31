#include "ZCollision.h"

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

	uint32_t currentPtr = vtxSegmentOffset;

	for (uint16_t i = 0; i < numVerts; i++)
	{
		ZVector vec(parent);
		vec.ExtractFromBinary(currentPtr, ZScalarType::ZSCALAR_S16, 3);

		currentPtr += vec.GetRawDataSize();
		vertices.push_back(vec);
	}

	for (uint16_t i = 0; i < numPolygons; i++)
		polygons.push_back(PolygonEntry(rawData, polySegmentOffset + (i * 16)));

	uint16_t highestPolyType = 0;

	for (PolygonEntry poly : polygons)
	{
		if (poly.type > highestPolyType)
			highestPolyType = poly.type;
	}

	for (uint16_t i = 0; i < highestPolyType + 1; i++)
		polygonTypes.push_back(
			BitConverter::ToUInt64BE(rawData, polyTypeDefSegmentOffset + (i * 8)));

	if (camDataAddress != 0)
		camData = new CameraDataList(parent, name, rawData, camDataSegmentOffset,
		                             polyTypeDefSegmentOffset, polygonTypes.size());

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
			StringHelper::Sprintf("%s_waterBoxes_%06X", auxName.c_str(), waterBoxSegmentOffset),
			waterBoxes.size(), declaration);
	}

	if (polygons.size() > 0)
	{
		declaration.clear();

		if (!Globals::Instance->otrMode)
		{
			for (size_t i = 0; i < polygons.size(); i++)
			{
				declaration += StringHelper::Sprintf(
					"\t{ 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X },",
					polygons[i].type, polygons[i].vtxA, polygons[i].vtxB, polygons[i].vtxC,
					polygons[i].a, polygons[i].b, polygons[i].c, polygons[i].d);
				if (i + 1 < polygons.size())
					declaration += "\n";
			}
		}

		parent->AddDeclarationArray(
			polySegmentOffset, DeclarationAlignment::Align4, polygons.size() * 16, "CollisionPoly",
			StringHelper::Sprintf("%s_polygons_%08X", auxName.c_str(), polySegmentOffset),
			polygons.size(), declaration);
	}

	declaration.clear();
	if (!Globals::Instance->otrMode)
	{
		for (size_t i = 0; i < polygonTypes.size(); i++)
		{
			declaration += StringHelper::Sprintf("\t{ 0x%08lX, 0x%08lX },", polygonTypes[i] >> 32,
			                                     polygonTypes[i] & 0xFFFFFFFF);

			if (i < polygonTypes.size() - 1)
				declaration += "\n";
		}
	}

	if (polyTypeDefAddress != 0)
		parent->AddDeclarationArray(
			polyTypeDefSegmentOffset, DeclarationAlignment::Align4, polygonTypes.size() * 8,
			"SurfaceType",
			StringHelper::Sprintf("%s_surfaceType_%08X", auxName.c_str(), polyTypeDefSegmentOffset),
			polygonTypes.size(), declaration);

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
				StringHelper::Sprintf("%s_vtx_%08X", auxName.c_str(), vtxSegmentOffset),
				vertices.size(), declaration);
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

PolygonEntry::PolygonEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	const uint8_t* data = rawData.data();

	type = BitConverter::ToUInt16BE(data, rawDataIndex + 0);
	vtxA = BitConverter::ToUInt16BE(data, rawDataIndex + 2);
	vtxB = BitConverter::ToUInt16BE(data, rawDataIndex + 4);
	vtxC = BitConverter::ToUInt16BE(data, rawDataIndex + 6);
	a = BitConverter::ToUInt16BE(data, rawDataIndex + 8);
	b = BitConverter::ToUInt16BE(data, rawDataIndex + 10);
	c = BitConverter::ToUInt16BE(data, rawDataIndex + 12);
	d = BitConverter::ToUInt16BE(data, rawDataIndex + 14);
}

WaterBoxHeader::WaterBoxHeader(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	const uint8_t* data = rawData.data();

	xMin = BitConverter::ToInt16BE(data, rawDataIndex + 0);
	ySurface = BitConverter::ToInt16BE(data, rawDataIndex + 2);
	zMin = BitConverter::ToInt16BE(data, rawDataIndex + 4);
	xLength = BitConverter::ToInt16BE(data, rawDataIndex + 6);
	zLength = BitConverter::ToInt16BE(data, rawDataIndex + 8);

	if (Globals::Instance->game == ZGame::OOT_SW97)
		properties = BitConverter::ToInt16BE(data, rawDataIndex + 10);
	else
		properties = BitConverter::ToInt32BE(data, rawDataIndex + 12);
}

std::string WaterBoxHeader::GetBodySourceCode() const
{
	return StringHelper::Sprintf("%i, %i, %i, %i, %i, 0x%08X", xMin, ySurface, zMin, xLength,
	                             zLength, properties);
}

CameraDataList::CameraDataList(ZFile* parent, const std::string& prefix,
                               const std::vector<uint8_t>& rawData, uint32_t rawDataIndex,
                               uint32_t polyTypeDefSegmentOffset,
                               [[maybe_unused]] uint32_t polygonTypesCnt)
{
	std::string declaration;

	// Parse CameraDataEntries
	int32_t numElements = (polyTypeDefSegmentOffset - rawDataIndex) / 8;
	uint32_t cameraPosDataSeg = rawDataIndex;
	for (int32_t i = 0; i < numElements; i++)
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
			sprintf(camSegLine, "&%s_camPosData_%08X[%i]", prefix.c_str(), cameraPosDataOffset,
			        index);
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
		StringHelper::Sprintf("%s_camDataList_%08X", prefix.c_str(), rawDataIndex), entries.size(),
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
		parent->AddDeclarationArray(
			cameraPosDataIndex, DeclarationAlignment::Align4, entrySize, "Vec3s",
			StringHelper::Sprintf("%s_camPosData_%08X", prefix.c_str(), cameraPosDataIndex),
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
