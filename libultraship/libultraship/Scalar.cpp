#include "Scalar.h"
#include "Factories/ResourceLoader.h"

namespace Ship
{
	void ScalarV2::ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader)
	{
		Scalar* scal = (Scalar*)res;

		ScalarType scalType = (ScalarType)reader->ReadUInt32();

		switch (scalType)
		{
		case ScalarType::ZSCALAR_S8:
			scal->scalar.s8 = reader->ReadByte();
			break;
		case ScalarType::ZSCALAR_U8:
			scal->scalar.u8 = reader->ReadUByte();
			break;
		case ScalarType::ZSCALAR_S16:
			scal->scalar.s16 = reader->ReadInt16();
			break;
		case ScalarType::ZSCALAR_U16:
			scal->scalar.u16 = reader->ReadUInt16();
			break;
		case ScalarType::ZSCALAR_S32:
			scal->scalar.s32 = reader->ReadInt32();
			break;
		case ScalarType::ZSCALAR_U32:
			scal->scalar.u32 = reader->ReadUInt32();
			break;
		case ScalarType::ZSCALAR_U64:
			scal->scalar.u64 = reader->ReadUInt64();
			break;
		case ScalarType::ZSCALAR_S64:
			scal->scalar.s64 = reader->ReadInt64();
			break;
		case ScalarType::ZSCALAR_F32:
			scal->scalar.f32 = reader->ReadSingle();
			break;
		case ScalarType::ZSCALAR_F64:
			scal->scalar.f64 = reader->ReadDouble();
			break;
		default:
		}
	}
}
