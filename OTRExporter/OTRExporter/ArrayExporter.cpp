#include "ArrayExporter.h"
#include "VtxExporter.h"
#include <ZVector.h>
#include <Globals.h>

void OTRExporter_Array::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeHeader)
{
	ZArray* arr = (ZArray*)res;

	if (writeHeader)
		WriteHeader(res, writer, Ship::ResourceType::Array, Ship::Version::Deckard);

	writer->Write((uint32_t)arr->resList[0]->GetResourceType());
	writer->Write((uint32_t)arr->arrayCnt);

	for (size_t i = 0; i < arr->arrayCnt; i++)
	{
		if (arr->resList[i]->GetResourceType() == ZResourceType::Scalar)
		{
			ZScalar* scal = (ZScalar*)arr->resList[i];
			writer->Write((uint32_t)scal->scalarType);
			WriteScalarType(scal, writer);
		}
		else if (arr->resList[i]->GetResourceType() == ZResourceType::Vector)
		{
			ZVector* vec = (ZVector*)arr->resList[i];
			writer->Write((uint32_t)vec->scalarType);
			writer->Write((uint32_t)vec->dimensions);

			for (int k = 0; k < vec->dimensions; k++)
				WriteScalarType(&vec->scalars[k], writer);
		}
		else if (arr->resList[i]->GetResourceType() == ZResourceType::Vertex)
		{
			ZVtx* vtx = (ZVtx*)arr->resList[i];
			OTRExporter_Vtx::WriteVtx(vtx, writer);
		}
		else
		{
			auto expSet = Globals::Instance->GetExporterSet();
			expSet->exporters[arr->resList[i]->GetResourceType()]->Save(arr->resList[i], outPath, writer, false);
		}
	}
}

void OTRExporter_Array::WriteScalarType(ZScalar* scalar, BinaryWriter* writer)
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
		// OTRTODO: ADD OTHER TYPES
	}
}
