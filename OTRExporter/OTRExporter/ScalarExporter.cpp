#include "ScalarExporter.h"
#include "Resource.h"
#include "VersionInfo.h"

void OTRExporter_Scalar::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader)
{
	ZScalar* scalar = (ZScalar*)res;

	WriteHeader(res, writer, Ship::ResourceType::Scalar, Ship::Version::Rachael, writeFullHeader);

	writer->Write((uint32_t)scalar->scalarType);

	WriteScalarType(scalar, writer);
}

void OTRExporter_Scalar::WriteScalarType(ZScalar* scalar, BinaryWriter* writer)
{
	switch (scalar->scalarType)
	{
	case ZScalarType::ZSCALAR_U8:
		writer->Write(scalar->scalarData.u8);
		break;
	case ZScalarType::ZSCALAR_S8:
		writer->Write(scalar->scalarData.s8);
		break;
	case ZScalarType::ZSCALAR_U16:
		writer->Write(scalar->scalarData.u16);
		break;
	case ZScalarType::ZSCALAR_S16:
		writer->Write(scalar->scalarData.s16);
		break;
	case ZScalarType::ZSCALAR_S32:
		writer->Write(scalar->scalarData.s32);
		break;
	case ZScalarType::ZSCALAR_U32:
		writer->Write(scalar->scalarData.u32);
		break;
	case ZScalarType::ZSCALAR_S64:
		writer->Write(scalar->scalarData.s64);
		break;
	case ZScalarType::ZSCALAR_U64:
		writer->Write(scalar->scalarData.u64);
		break;
	case ZScalarType::ZSCALAR_F32:
		writer->Write(scalar->scalarData.f32);
		break;
	case ZScalarType::ZSCALAR_F64:
		writer->Write(scalar->scalarData.f64);
		break;
	default:
		break;
	}
}