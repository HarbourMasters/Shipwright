#include "Array.h"
#include "Vertex.h"
#include "Factories/ResourceLoader.h"

namespace Ship
{
	void ArrayV2::ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader)
	{
		Array* arr = (Array*)res;

		ResourceFile::ParseFileBinary(reader, res, readFullHeader);

		ResourceType resType = (ResourceType)reader->ReadUInt32();
		uint32_t arrayCnt = reader->ReadUInt32();

		for (uint32_t i = 0; i < arrayCnt; i++)
		{
			if (resType == ResourceType::Vertex)
			{	
				Vertex* vtx = (Vertex*)ResourceLoader::LoadResource(reader, resType, false);
				arr->vertices.push_back(vtx->vtx);
			}
			else if (resType == ResourceType::Vector)
			{
				int iter = reader->ReadUInt32();

				for (int k = 0; k < iter; k++)
				{
 					Scalar* scal = (Scalar*)ResourceLoader::LoadResource(reader, ResourceType::Scalar, false);
					arr->scalars.push_back(scal->scalar);
				}
			}
			else
			{
				Resource* childRes = ResourceLoader::LoadResource(reader, resType, false);

				if (resType == ResourceType::Scalar)
				{
					Scalar* scal = (Scalar*)childRes;
					arr->scalars.push_back(scal->scalar);
				}

				arr->resources.push_back(childRes);
			}
		}
	}

	void ArrayV0::ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader)
	{
		Array* arr = (Array*)res;

		ResourceFile::ParseFileBinary(reader, res, readFullHeader);

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
