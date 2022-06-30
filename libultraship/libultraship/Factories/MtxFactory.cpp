#include "MtxFactory.h"

namespace Ship
{
	Matrix* MtxFactory::ReadMtx(BinaryReader* reader, bool readFullHeader) {
		Matrix* mtx = new Matrix();
		uint32_t version = reader->ReadUInt32();
		switch (version)
		{
		case 0:
		{
			MatrixV0 Mtx = MatrixV0();
			Mtx.ParseFileBinary(reader, mtx, readFullHeader);
		}
		break;
		default:
			//VERSION NOT SUPPORTED
			break;
		}
		return mtx;
	}
}