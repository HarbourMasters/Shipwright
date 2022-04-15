#include "Vertex.h"

namespace Ship
{
	void VertexV1::ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader)
	{
		Vertex* vtx = (Vertex*)res;
		ResourceFile::ParseFileBinary(reader, res, readFullHeader);

		vtx->vtx.x = reader->ReadInt16();
		vtx->vtx.y = reader->ReadInt16();
		vtx->vtx.z = reader->ReadInt16();
		vtx->vtx.flag = reader->ReadUInt16();
		vtx->vtx.s = reader->ReadInt16();
		vtx->vtx.t = reader->ReadInt16();
		vtx->vtx.r = reader->ReadUByte();
		vtx->vtx.g = reader->ReadUByte();
		vtx->vtx.b = reader->ReadUByte();
		vtx->vtx.a = reader->ReadUByte();
	}

	void VertexV0::ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader)
	{
		Vertex* vtx = (Vertex*)res;
		ResourceFile::ParseFileBinary(reader, res, readFullHeader);

		uint32_t count = reader->ReadUInt32();
		//vtx->vtxList.reserve(count);

		for (uint32_t i = 0; i < count; i++) 
		{
			Vtx data;
			data.x = reader->ReadInt16();
			data.y = reader->ReadInt16();
			data.z = reader->ReadInt16();
			data.flag = reader->ReadUInt16();
			data.s = reader->ReadInt16();
			data.t = reader->ReadInt16();
			data.r = reader->ReadUByte();
			data.g = reader->ReadUByte();
			data.b = reader->ReadUByte();
			data.a = reader->ReadUByte();
			vtx->vtx = data;
			break;
			//vtx->vtxList.push_back(data);
		}
	}
}