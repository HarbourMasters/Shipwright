#include "VtxFactory.h"

namespace Ship
{
	Vertex* VertexFactory::ReadVtx(BinaryReader* reader, bool readFullHeader) {
		Vertex* vtx = new Vertex();
		uint32_t version = reader->ReadUInt32();
		switch (version)
		{
		case (uint32_t)Version::Rachael:
		{
			VertexV2 Vtx = VertexV2();
			Vtx.ParseFileBinary(reader, vtx, readFullHeader);
		}
		break;
		case (uint32_t)Version::Deckard:
		{
			VertexV0 Vtx = VertexV0();
			Vtx.ParseFileBinary(reader, vtx, readFullHeader);
		}
		break;
		default:
			//VERSION NOT SUPPORTED
			break;
		}
		return vtx;
	}
}