#pragma once

#include "Resource.h"

namespace Ship 
{
	class Vtx 
	{
	public:
		int16_t x, y, z;
		uint16_t flag;
		int16_t s, t;
		uint8_t r, g, b, a;
	};

	class VertexV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class Vertex : public Resource
	{
	public:
		std::vector<Vtx> vtxList;
	};
}
