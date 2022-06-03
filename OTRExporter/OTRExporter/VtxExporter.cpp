#include "VtxExporter.h"
#include "Resource.h"
#include "VersionInfo.h"

void OTRExporter_Vtx::WriteVtx(ZVtx* vtx, BinaryWriter* writer)
{
	writer->Write(vtx->x);
	writer->Write(vtx->y);
	writer->Write(vtx->z);
	writer->Write(vtx->flag);
	writer->Write(vtx->s);
	writer->Write(vtx->t);
	writer->Write(vtx->r);
	writer->Write(vtx->g);
	writer->Write(vtx->b);
	writer->Write(vtx->a);
}

void OTRExporter_Vtx::SaveArr(ZResource* res, const fs::path& outPath, const std::vector<ZResource*>& vec, BinaryWriter* writer, bool writeFullHeader)
{
	WriteHeader(res, writer, Ship::ResourceType::Vertex, Ship::Version::Rachael, writeFullHeader);
	
	writer->Write((uint32_t)vec.size());
	
	for (auto& res: vec) {
		ZVtx* vtx = (ZVtx*)res;
		WriteVtx(vtx, writer);
	}
}

void OTRExporter_Vtx::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer, bool writeFullHeader)
{
	ZVtx* vtx = (ZVtx*)res;

	WriteHeader(res, writer, Ship::ResourceType::Vertex, Ship::Version::Rachael, writeFullHeader);

	WriteVtx(vtx, writer);

	//std::vector<ZResource*> vec;
	//vec.push_back(vtx);

	//SaveArr(res, outPath, vec, writer, writeHeader);
}