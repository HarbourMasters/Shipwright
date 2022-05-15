#include "Array.h"
#include "Vertex.h"

namespace Ship
{
	void ArrayV0::ParseFileBinary(BinaryReader* reader, Resource* res)
	{
		Array* arr = (Array*)res;

		ResourceFile::ParseFileBinary(reader, res);

		ZResourceType resType = (ZResourceType)reader->ReadUInt32();
		uint32_t arrayCnt = reader->ReadUInt32();

		for (uint32_t i = 0; i < arrayCnt; i++)
		{
			if (resType == ZResourceType::Vertex)
			{
				Vtx data;
				data.x = reader->ReadInt16();
				data.y = reader->ReadInt16();
				data.z = reader->ReadInt16();
				data.flag = reader->ReadUInt16();
				data.s = reader->ReadInt16();
				data.t = reader->ReadInt16();
				data.r = reader->ReadUByte();
				data.g = reader->ReadUByte();
				data.b = reader->ReadUByte();
				data.a = reader->ReadUByte();
				arr->vertices.push_back(data);
			}
			else
			{
				ScalarType scalType = (ScalarType)reader->ReadUInt32();

				int iter = 1;

				if (resType == ZResourceType::Vector)
					iter = reader->ReadUInt32();

				for (int k = 0; k < iter; k++)
				{
					ScalarData data;

					switch (scalType)
					{
					case ScalarType::ZSCALAR_S16:
						data.s16 = reader->ReadInt16();
						break;
					case ScalarType::ZSCALAR_U16:
						data.u16 = reader->ReadUInt16();
						break;
						// OTRTODO: IMPLEMENT OTHER TYPES!
					default:
						break;
					}

					arr->scalars.push_back(data);
				}
			}
		}
	}
}
