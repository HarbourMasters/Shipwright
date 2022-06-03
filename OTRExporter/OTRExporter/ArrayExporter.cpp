#include "ArrayExporter.h"
#include "VtxExporter.h"
#include <ZVector.h>
#include <Globals.h>
#include "ScalarExporter.h"

void OTRExporter_Array::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader)
{
	ZArray* arr = (ZArray*)res;

	WriteHeader(res, writer, Ship::ResourceType::Array, Ship::Version::Rachael, writeFullHeader);

	writer->Write((uint32_t)GetResourceType(arr->resList[0]->GetResourceType()));
	writer->Write((uint32_t)arr->arrayCnt);

	auto expSet = Globals::Instance->GetExporterSet();

	for (size_t i = 0; i < arr->arrayCnt; i++)
	{
		if (arr->resList[i]->GetResourceType() == ZResourceType::Vector)
		{
			ZVector* vec = (ZVector*)arr->resList[i];
			writer->Write((uint32_t)vec->dimensions);

			for (int k = 0; k < vec->dimensions; k++)
				expSet->exporters[vec->scalars[k].GetResourceType()]->Save(&vec->scalars[k], outPath, writer, false);
		}
		else
		{
			expSet->exporters[arr->resList[i]->GetResourceType()]->Save(arr->resList[i], outPath, writer, false);
		}
	}
}
