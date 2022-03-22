#include "CollisionExporter.h"

void ExporterExample_Collision::Save(ZResource* res, [[maybe_unused]] fs::path outPath,
                                     BinaryWriter* writer)
{
	ZCollisionHeader* col = (ZCollisionHeader*)res;

	writer->Write(col->absMinX);
	writer->Write(col->absMinY);
	writer->Write(col->absMinZ);

	writer->Write(col->absMaxX);
	writer->Write(col->absMaxY);
	writer->Write(col->absMaxZ);

	writer->Write(col->numVerts);
	writer->Write(col->vtxAddress);

	writer->Write(col->numPolygons);
	writer->Write(col->polyAddress);
	writer->Write(col->polyTypeDefAddress);
	writer->Write(col->camDataAddress);

	writer->Write(col->numWaterBoxes);
	writer->Write(col->waterBoxAddress);

	writer->Write(col->vtxSegmentOffset);
	writer->Write(col->polySegmentOffset);
	writer->Write(col->polyTypeDefSegmentOffset);
	writer->Write(col->camDataSegmentOffset);
	writer->Write(col->waterBoxSegmentOffset);

	uint32_t oldOffset = writer->GetBaseAddress();

	writer->Seek(col->vtxSegmentOffset, SeekOffsetType::Start);

	for (uint16_t i = 0; i < col->vertices.size(); i++)
	{
		for (uint32_t j = 0; j < col->vertices[i].dimensions; j++)
		{
			writer->Write(col->vertices[i].scalars[j].scalarData.s16);
		}
	}

	writer->Seek(col->polySegmentOffset, SeekOffsetType::Start);

	for (uint16_t i = 0; i < col->polygons.size(); i++)
	{
		writer->Write(col->polygons[i].type);
		writer->Write(col->polygons[i].vtxA);
		writer->Write(col->polygons[i].vtxB);
		writer->Write(col->polygons[i].vtxC);
		writer->Write(col->polygons[i].a);
		writer->Write(col->polygons[i].b);
		writer->Write(col->polygons[i].c);
		writer->Write(col->polygons[i].d);
	}

	writer->Seek(col->polyTypeDefSegmentOffset, SeekOffsetType::Start);

	for (uint16_t i = 0; i < col->polygonTypes.size(); i++)
		writer->Write(col->polygonTypes[i]);

	writer->Seek(col->camDataSegmentOffset, SeekOffsetType::Start);

	for (auto entry : col->camData->entries)
	{
		writer->Write(entry->cameraSType);
		writer->Write(entry->numData);
		writer->Write(entry->cameraPosDataSeg);
	}

	writer->Seek(oldOffset, SeekOffsetType::Start);
}
