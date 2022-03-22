#include "ZResource.h"
#include "ZMtx.h"
#include "Exporter.h"
#include <Utils/BinaryWriter.h>

class OTRExporter_MtxExporter : public OTRExporter
{
public:
	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) override;
};
