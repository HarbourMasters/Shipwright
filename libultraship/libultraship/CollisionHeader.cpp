#include "CollisionHeader.h"

void Ship::CollisionHeaderV0::ParseFileBinary(BinaryReader* reader, Resource* res)
{
	CollisionHeader* col = (CollisionHeader*)res;

	ResourceFile::ParseFileBinary(reader, res);

	col->absMinX = reader->ReadInt16();
	col->absMinY = reader->ReadInt16();
	col->absMinZ = reader->ReadInt16();

	col->absMaxX = reader->ReadInt16();
	col->absMaxY = reader->ReadInt16();
	col->absMaxZ = reader->ReadInt16();

	uint32_t vtxCnt = reader->ReadInt32();
	col->vertices.reserve(vtxCnt);

	for (uint32_t i = 0; i < vtxCnt; i++)
	{
		float x = reader->ReadInt16();
		float y = reader->ReadInt16();
		float z = reader->ReadInt16();
		col->vertices.push_back(Vec3f(x, y, z));
	}

	uint32_t polyCnt = reader->ReadUInt32();
	col->polygons.reserve(polyCnt);

	for (uint32_t i = 0; i < polyCnt; i++)
		col->polygons.push_back(Ship::PolygonEntry(reader));

	uint32_t polyTypesCnt = reader->ReadUInt32();
	col->polygonTypes.reserve(polyTypesCnt);

	for (uint32_t i = 0; i < polyTypesCnt; i++)
		col->polygonTypes.push_back(reader->ReadUInt64());

	col->camData = new CameraDataList();

	uint32_t camEntriesCnt = reader->ReadUInt32();
	col->camData->entries.reserve(camEntriesCnt);

	for (uint32_t i = 0; i < camEntriesCnt; i++)
	{
		Ship::CameraDataEntry* entry = new Ship::CameraDataEntry();
		entry->cameraSType = reader->ReadUInt16();
		entry->numData = reader->ReadInt16();
		entry->cameraPosDataIdx = reader->ReadInt32();
		col->camData->entries.push_back(entry);
	}

	uint32_t camPosCnt = reader->ReadInt32();
	col->camData->cameraPositionData.reserve(camPosCnt);

	for (uint32_t i = 0; i < camPosCnt; i++)
	{
		Ship::CameraPositionData* entry = new Ship::CameraPositionData();
		entry->x = reader->ReadInt16();
		entry->y = reader->ReadInt16();
		entry->z = reader->ReadInt16();
		col->camData->cameraPositionData.push_back(entry);
	}

	uint32_t waterBoxCnt = reader->ReadInt32();
	col->waterBoxes.reserve(waterBoxCnt);

	for (uint32_t i = 0; i < waterBoxCnt; i++)
	{
		Ship::WaterBoxHeader waterBox;
		waterBox.xMin = reader->ReadInt16();
		waterBox.ySurface = reader->ReadInt16();
		waterBox.zMin = reader->ReadInt16();
		waterBox.xLength = reader->ReadInt16();
		waterBox.zLength = reader->ReadInt16();
		waterBox.properties = reader->ReadInt32();

		col->waterBoxes.push_back(waterBox);
	}
}

Ship::PolygonEntry::PolygonEntry(BinaryReader* reader)
{
	type = reader->ReadUInt16();
	
	vtxA = reader->ReadUInt16();
	vtxB = reader->ReadUInt16();
	vtxC = reader->ReadUInt16();
	
	a = reader->ReadUInt16();
	b = reader->ReadUInt16();
	c = reader->ReadUInt16();
	d = reader->ReadUInt16();
}

Ship::WaterBoxHeader::WaterBoxHeader()
{
}