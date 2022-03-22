#include "MtxFactory.h"

namespace Ship
{
	Matrix* MtxFactory::ReadMtx(BinaryReader* reader) {
		Matrix* mtx = new Matrix();
		uint32_t version = reader->ReadUInt32();
		switch (version)
		{
		case 0:
		{
			MatrixV0 Mtx = MatrixV0();
			Mtx.ParseFileBinary(reader, mtx);
		}
		break;
		default:
			//VERSION NOT SUPPORTED
			break;
		}
		return mtx;
	}
}