#pragma once
#include "../Vertex.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
	class VertexFactory
	{
	public:
		static Vertex* ReadVtx(BinaryReader* reader);
	};
}