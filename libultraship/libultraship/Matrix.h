#pragma once

#include "Resource.h"

namespace Ship 
{
	class MatrixV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class Matrix : public Resource
	{
	public:
		std::array<std::array<int32_t, 4>, 4> mtx;
	};
}
