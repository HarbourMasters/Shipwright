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

	class VertexV1 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader) override;
	};

	class VertexV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader) override;
	};

	class Vertex : public Resource
	{
	public:
		Vtx vtx;
		//std::vector<Vtx> vtxList; // Note: V0 never properly used this list...
	};
}
