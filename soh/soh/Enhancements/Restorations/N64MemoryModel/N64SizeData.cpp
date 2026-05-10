#include "N64SizeData.hpp"

// --------------------------------------------------------------------------------------------------------------------
// DMA file sizes (misc/dma_sizes)
//
// Format: u32 entryCount, then per entry: u32 vromSize, length-prefixed string name
// --------------------------------------------------------------------------------------------------------------------

static std::unordered_map<std::string, u32> sDmaFileSizes;
static bool sIsDmaLoaded = false;

static void LoadDmaFileSizes() {
    if (sIsDmaLoaded) {
        return;
    }

    sIsDmaLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile("misc/dma_sizes");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/dma_sizes from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    const u32 entryCount = reader->ReadUInt32();
    for (std::size_t i = 0; i < entryCount; ++i) {
        const u32 vromSize = reader->ReadUInt32();
        const std::string name = reader->ReadString();
        sDmaFileSizes[name] = vromSize;
    }

    SPDLOG_INFO("[N64SizeData] Loaded {} DMA file sizes from OTR.", sDmaFileSizes.size());
}

// --------------------------------------------------------------------------------------------------------------------
// Actor overlay VRAM sizes (misc/actor_overlay_sizes)
//
// Format: u32 entryCount, then entryCount consecutive u32 values indexed by actor ID
// --------------------------------------------------------------------------------------------------------------------

static std::vector<u32> sActorOverlaySizes;
static bool sIsActorOverlayLoaded = false;

static void LoadActorOverlaySizes() {
    if (sIsActorOverlayLoaded) {
        return;
    }

    sIsActorOverlayLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile("misc/actor_overlay_sizes");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/actor_overlay_sizes from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    const u32 entryCount = reader->ReadUInt32();
    sActorOverlaySizes.resize(entryCount);

    for (std::size_t i = 0; i < entryCount; ++i) {
        sActorOverlaySizes.at(i) = reader->ReadUInt32();
    }

    SPDLOG_INFO("[N64SizeData] Loaded {} actor overlay sizes.", sActorOverlaySizes.size());
}

// --------------------------------------------------------------------------------------------------------------------
// Effect overlay VRAM sizes (misc/effect_overlay_sizes)
//
// Format: u32 entryCount, then entryCount consecutive u32 values indexed by effect type
// --------------------------------------------------------------------------------------------------------------------

static std::vector<u32> sEffectOverlaySizes;
static bool sIsEffectOverlayLoaded = false;

static void LoadEffectOverlaySizes() {
    if (sIsEffectOverlayLoaded) {
        return;
    }

    sIsEffectOverlayLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile("misc/effect_overlay_sizes");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/effect_overlay_sizes from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    const u32 entryCount = reader->ReadUInt32();
    sEffectOverlaySizes.resize(entryCount);

    for (std::size_t i = 0; i < entryCount; ++i) {
        sEffectOverlaySizes.at(i) = reader->ReadUInt32();
    }

    SPDLOG_INFO("[N64SizeData] Loaded {} effect overlay sizes.", sEffectOverlaySizes.size());
}

// --------------------------------------------------------------------------------------------------------------------
// Actor instance sizes (misc/actor_instance_sizes)
//
// Format: u32 entryCount, then entryCount consecutive u32 values indexed by actor ID
// Each value is the N64 sizeof the actor's instance struct, read from ActorProfile.instanceSize.
// --------------------------------------------------------------------------------------------------------------------

static std::vector<u32> sActorInstanceSizes;
static bool sIsActorInstanceLoaded = false;

static void LoadActorInstanceSizes() {
    if (sIsActorInstanceLoaded) {
        return;
    }

    sIsActorInstanceLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile("misc/actor_instance_sizes");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/actor_instance_sizes from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    const u32 entryCount = reader->ReadUInt32();
    sActorInstanceSizes.resize(entryCount);

    for (std::size_t i = 0; i < entryCount; ++i) {
        sActorInstanceSizes.at(i) = reader->ReadUInt32();
    }

    SPDLOG_INFO("[N64SizeData] Loaded {} actor instance sizes.", sActorInstanceSizes.size());
}

// --------------------------------------------------------------------------------------------------------------------
// API
// --------------------------------------------------------------------------------------------------------------------

extern "C" u32 N64SizeData_GetDmaFileSize(const char* name) {
    LoadDmaFileSizes();

    if (const auto i = sDmaFileSizes.find(name); i != sDmaFileSizes.end()) {
        return i->second;
    }

    SPDLOG_WARN("[N64SizeData] DMA file size not found for '{}'", name);
    return 0;
}

extern "C" u32 N64SizeData_GetActorOverlaySize(u16 actorId) {
    LoadActorOverlaySizes();

    if (actorId < sActorOverlaySizes.size()) {
        return sActorOverlaySizes.at(actorId);
    }

    return 0;
}

extern "C" u32 N64SizeData_GetEffectOverlaySize(u16 effectType) {
    LoadEffectOverlaySizes();

    if (effectType < sEffectOverlaySizes.size()) {
        return sEffectOverlaySizes.at(effectType);
    }

    return 0;
}

extern "C" u32 N64SizeData_GetActorInstanceSize(u16 actorId) {
    LoadActorInstanceSizes();

    if (actorId < sActorInstanceSizes.size()) {
        return sActorInstanceSizes.at(actorId);
    }

    return 0;
}

// --------------------------------------------------------------------------------------------------------------------
// Kaleido overlay max VRAM size (misc/kaleido_vram_size)
//
// Format: single u32 -- max(ovl_kaleido_scope VRAM, ovl_player_actor VRAM)
// --------------------------------------------------------------------------------------------------------------------

static u32 sKaleidoVramSize = 0;
static bool sIsKaleidoLoaded = false;

static void LoadKaleidoVramSize() {
    if (sIsKaleidoLoaded) {
        return;
    }

    sIsKaleidoLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile("misc/kaleido_vram_size");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/kaleido_vram_size from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    sKaleidoVramSize = reader->ReadUInt32();

    SPDLOG_INFO("[N64SizeData] Kaleido max VRAM size: 0x{:X}.", sKaleidoVramSize);
}

extern "C" u32 N64SizeData_GetKaleidoVramSize() {
    LoadKaleidoVramSize();
    return sKaleidoVramSize;
}