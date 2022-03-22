#include "VtxFactory.h"

namespace Ship
{
	Vertex* VertexFactory::ReadVtx(BinaryReader* reader) {
		Vertex* vtx = new Vertex();
		uint32_t version = reader->ReadUInt32();
		switch (version)
		{
		case 0:
		{
			VertexV0 Vtx = VertexV0();
			Vtx.ParseFileBinary(reader, vtx);
		}
		break;
		default:
			//VERSION NOT SUPPORTED
			break;
		}
		return vtx;
	}
}