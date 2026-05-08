#include "DmaSizes.hpp"

// --------------------------------------------------------------------------------------------------------------------
// DMA file size loader
//
// Loads misc/dma_sizes from the OTR and caches the result.  The blob was written by OTRExporter during extraction and
// contains per-file VROM sizes from the ROM's DMA table.
// --------------------------------------------------------------------------------------------------------------------

static std::unordered_map<std::string, u32> sDmaFileSizes;
static bool sIsLoaded = false;

static void LoadDmaFileSizes()
{
    if (sIsLoaded)
    {
        return;
    }

    sIsLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile("misc/dma_sizes");
    if (!file || !file->IsLoaded)
    {
        SPDLOG_ERROR("[DmaSizes] Failed to load misc/dma_sizes from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    const u32 entryCount = reader->ReadUInt32();
    for (std::size_t i = 0; i < entryCount; ++i)
    {
        const u32 vromSize = reader->ReadUInt32();
        const std::string name = reader->ReadString();
        sDmaFileSizes[name] = vromSize;
    }

    SPDLOG_INFO("[DmaSizes] Loaded {} DMA file sizes from OTR.", sDmaFileSizes.size());
}

extern "C" u32 DmaSizes_GetFileSize(const char* name)
{
    LoadDmaFileSizes();

    if (const auto i = sDmaFileSizes.find(name); i != sDmaFileSizes.end())
    {
        return i->second;
    }

    SPDLOG_WARN("[DmaSizes] File size not found for '{}'", name);
    return 0;
}
