#include "Array.h"
#include "Vertex.h"
#include "Factories/ResourceLoader.h"

namespace Ship
{
	void ArrayV1::ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader)
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
			else if (resType == ResourceType::Vector || resType == ResourceType::Scalar)
			{
				ScalarType scalType = (ScalarType)reader->ReadUInt32();

				int iter = 1;

				if (resType == ResourceType::Vertex)
					iter = reader->ReadUInt32();

				for (int k = 0; k < iter; k++)
				{
					ScalarData data;

					switch (scalType)
					{
					case ScalarType::ZSCALAR_S8:
						data.s8 = reader->ReadByte();
						break;
					case ScalarType::ZSCALAR_U8:
						data.u8 = reader->ReadUByte();
						break;
					case ScalarType::ZSCALAR_S16:
						data.s16 = reader->ReadInt16();
						break;
					case ScalarType::ZSCALAR_U16:
						data.u16 = reader->ReadUInt16();
						break;
					case ScalarType::ZSCALAR_S32:
						data.s32 = reader->ReadInt32();
						break;
					case ScalarType::ZSCALAR_U32:
						data.u32 = reader->ReadUInt32();
						break;
					case ScalarType::ZSCALAR_F32:
						data.f32 = reader->ReadSingle();
						break;
					case ScalarType::ZSCALAR_F64:
						data.f64 = reader->ReadDouble();
						break;
					}

					arr->scalars.push_back(data);
				}
			}
			else
			{
				Resource* childRes = ResourceLoader::LoadResource(reader, resType, false);
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
