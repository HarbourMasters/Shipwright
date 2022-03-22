#include "Path.h"

namespace Ship
{
	void PathV0::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		Path* path = (Path*)res;

		ResourceFile::ParseFileBinary(reader, res);

		uint32_t numPaths = reader->ReadUInt32();
		path->paths.reserve(numPaths);

		for (uint32_t k = 0; k < numPaths; k++)
		{
			std::vector<Vec3s> nodes;
			
			uint32_t numNodes = reader->ReadUInt32();
			nodes.reserve(numNodes);

			for (uint32_t i = 0; i < numNodes; i++)
			{
				int16_t x = reader->ReadInt16();
				int16_t y = reader->ReadInt16();
				int16_t z = reader->ReadInt16();

				nodes.push_back(Vec3s(x, y, z));
			}

			path->paths.push_back(nodes);
		}
	}
}
