#include "Matrix.h"

namespace Ship
{
	void MatrixV0::ParseFileBinary(BinaryReader* reader, Resource* res) {
		Matrix* mtx = (Matrix*)res;
		ResourceFile::ParseFileBinary(reader, res);

		for (size_t i = 0; i < 4; i++)
			for (size_t j = 0; j < 4; j++)
				mtx->mtx[i][j] = reader->ReadInt32();
	}
}