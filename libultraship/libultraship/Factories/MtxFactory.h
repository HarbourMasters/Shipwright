#pragma once
#include "../Matrix.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
	class MtxFactory
	{
	public:
		static Matrix* ReadMtx(BinaryReader* reader);
	};
}