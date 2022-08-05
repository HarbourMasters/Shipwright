#include "Matrix.h"

namespace Ship
{
	void MatrixV0::ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader) {
		Matrix* mtx = (Matrix*)res;
		ResourceFile::ParseFileBinary(reader, res, readFullHeader);

		for (size_t i = 0; i < 4; i++)
			for (size_t j = 0; j < 4; j++)
				mtx->mtx[i][j] = reader->ReadInt32();
	}
}