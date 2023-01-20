#include "VtxExporter.h"
#include <libultraship/bridge.h>
#include "VersionInfo.h"


void OTRExporter_Vtx::SaveArr(ZResource* res, const fs::path& outPath, const std::vector<ZResource*>& vec, BinaryWriter* writer)
{
	WriteHeader(res, outPath, writer, Ship::ResourceType::Vertex);
	
	for (auto& res: vec) {
		ZVtx* vtx = (ZVtx*)res;
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

}

void OTRExporter_Vtx::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZVtx* vtx = (ZVtx*)res;

	WriteHeader(res, outPath, writer, Ship::ResourceType::Vertex);

	writer->Write((uint32_t)1); //Yes I'm hard coding it to one, it *should* be fine.

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
