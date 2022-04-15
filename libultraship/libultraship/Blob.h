#pragma once

#include "Resource.h"

namespace Ship
{
	class BlobV0 : public ResourceFile
	{
	public:

		void ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader) override;
	};

	class Blob : public Resource
	{
	public:
		std::vector<uint8_t> data;
	};
};