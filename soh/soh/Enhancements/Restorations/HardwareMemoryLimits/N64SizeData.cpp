#include "N64SizeData.hpp"

// --------------------------------------------------------------------------------------------------------------------
// DMA file sizes (misc/dma_sizes)
//
// Format: uint32_t entryCount, then per entry: uint32_t vromSize, length-prefixed string name
// --------------------------------------------------------------------------------------------------------------------

static std::unordered_map<std::string, uint32_t> sDmaFileSizes;
static bool sIsDmaLoaded = false;

static void LoadDmaFileSizes() {
    if (sIsDmaLoaded) {
        return;
    }

    sIsDmaLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile("misc/n64_memory/dma_sizes");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/n64_memory/dma_sizes from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    const uint32_t entryCount = reader->ReadUInt32();
    for (std::size_t i = 0; i < entryCount; ++i) {
        const uint32_t vromSize = reader->ReadUInt32();
        const std::string name = reader->ReadString();
        sDmaFileSizes[name] = vromSize;
    }

    SPDLOG_INFO("[N64SizeData] Loaded {} DMA file sizes from OTR.", sDmaFileSizes.size());
}

// --------------------------------------------------------------------------------------------------------------------
// Actor overlay VRAM sizes (misc/actor_overlay_sizes)
//
// Format: uint32_t entryCount, then entryCount consecutive uint32_t values indexed by actor ID
// --------------------------------------------------------------------------------------------------------------------

static std::vector<uint32_t> sActorOverlaySizes;
static bool sIsActorOverlayLoaded = false;

static void LoadActorOverlaySizes() {
    if (sIsActorOverlayLoaded) {
        return;
    }

    sIsActorOverlayLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile(
            "misc/n64_memory/actor_overlay_sizes");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/n64_memory/actor_overlay_sizes from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    const uint32_t entryCount = reader->ReadUInt32();
    sActorOverlaySizes.resize(entryCount);

    for (std::size_t i = 0; i < entryCount; ++i) {
        sActorOverlaySizes.at(i) = reader->ReadUInt32();
    }

    SPDLOG_INFO("[N64SizeData] Loaded {} actor overlay sizes.", sActorOverlaySizes.size());
}

// --------------------------------------------------------------------------------------------------------------------
// Effect overlay VRAM sizes (misc/effect_overlay_sizes)
//
// Format: uint32_t entryCount, then entryCount consecutive uint32_t values indexed by effect type
// --------------------------------------------------------------------------------------------------------------------

static std::vector<uint32_t> sEffectOverlaySizes;
static bool sIsEffectOverlayLoaded = false;

static void LoadEffectOverlaySizes() {
    if (sIsEffectOverlayLoaded) {
        return;
    }

    sIsEffectOverlayLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile(
            "misc/n64_memory/effect_overlay_sizes");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/n64_memory/effect_overlay_sizes from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    const uint32_t entryCount = reader->ReadUInt32();
    sEffectOverlaySizes.resize(entryCount);

    for (std::size_t i = 0; i < entryCount; ++i) {
        sEffectOverlaySizes.at(i) = reader->ReadUInt32();
    }

    SPDLOG_INFO("[N64SizeData] Loaded {} effect overlay sizes.", sEffectOverlaySizes.size());
}

// --------------------------------------------------------------------------------------------------------------------
// Actor instance sizes (misc/actor_instance_sizes)
//
// Format: uint32_t entryCount, then entryCount consecutive uint32_t values indexed by actor ID
// Each value is the N64 sizeof the actor's instance struct, read from ActorProfile.instanceSize.
// --------------------------------------------------------------------------------------------------------------------

static std::vector<uint32_t> sActorInstanceSizes;
static bool sIsActorInstanceLoaded = false;

static void LoadActorInstanceSizes() {
    if (sIsActorInstanceLoaded) {
        return;
    }

    sIsActorInstanceLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile(
            "misc/n64_memory/actor_instance_sizes");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/n64_memory/actor_instance_sizes from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    const uint32_t entryCount = reader->ReadUInt32();
    sActorInstanceSizes.resize(entryCount);

    for (std::size_t i = 0; i < entryCount; ++i) {
        sActorInstanceSizes.at(i) = reader->ReadUInt32();
    }

    SPDLOG_INFO("[N64SizeData] Loaded {} actor instance sizes.", sActorInstanceSizes.size());
}

// --------------------------------------------------------------------------------------------------------------------
// API
// --------------------------------------------------------------------------------------------------------------------

extern "C" uint32_t N64SizeData_GetDmaFileSize(const char* name) {
    LoadDmaFileSizes();

    if (const auto i = sDmaFileSizes.find(name); i != sDmaFileSizes.end()) {
        return i->second;
    }

    SPDLOG_WARN("[N64SizeData] DMA file size not found for '{}'", name);
    return 0;
}

extern "C" uint32_t N64SizeData_GetActorOverlaySize(uint16_t actorId) {
    LoadActorOverlaySizes();

    if (actorId < sActorOverlaySizes.size()) {
        return sActorOverlaySizes.at(actorId);
    }

    return 0;
}

extern "C" uint32_t N64SizeData_GetEffectOverlaySize(uint16_t effectType) {
    LoadEffectOverlaySizes();

    if (effectType < sEffectOverlaySizes.size()) {
        return sEffectOverlaySizes.at(effectType);
    }

    return 0;
}

extern "C" uint32_t N64SizeData_GetActorInstanceSize(uint16_t actorId) {
    LoadActorInstanceSizes();

    if (actorId < sActorInstanceSizes.size()) {
        return sActorInstanceSizes.at(actorId);
    }

    return 0;
}

// --------------------------------------------------------------------------------------------------------------------
// Kaleido overlay max VRAM size (misc/kaleido_vram_size)
//
// Format: single uint32_t -- max(ovl_kaleido_scope VRAM, ovl_player_actor VRAM)
// --------------------------------------------------------------------------------------------------------------------

static uint32_t sKaleidoVramSize = 0;
static bool sIsKaleidoLoaded = false;

static void LoadKaleidoVramSize() {
    if (sIsKaleidoLoaded) {
        return;
    }

    sIsKaleidoLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile(
            "misc/n64_memory/kaleido_vram_size");
    if (!file || !file->IsLoaded) {
        SPDLOG_ERROR("[N64SizeData] Failed to load misc/n64_memory/kaleido_vram_size from OTR.");
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    sKaleidoVramSize = reader->ReadUInt32();

    SPDLOG_INFO("[N64SizeData] Kaleido max VRAM size: 0x{:X}.", sKaleidoVramSize);
}

extern "C" uint32_t N64SizeData_GetKaleidoVramSize() {
    LoadKaleidoVramSize();
    return sKaleidoVramSize;
}

// --------------------------------------------------------------------------------------------------------------------
// Arena node size
//
// Format: single uint32_t -- ArenaNode size for this ROM version (0x10 retail, 0x30 debug)
// --------------------------------------------------------------------------------------------------------------------

static uint32_t sArenaNodeSize = 0x30; // Default to debug (safe fallback -- over-estimates node overhead)
static bool sIsArenaNodeSizeLoaded = false;

static void LoadArenaNodeSize() {
    if (sIsArenaNodeSizeLoaded) {
        return;
    }

    sIsArenaNodeSizeLoaded = true;

    const auto file =
        Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->LoadFile(
            "misc/n64_memory/arena_node_size");
    if (!file || !file->IsLoaded) {
        SPDLOG_WARN("[N64SizeData] Failed to load misc/n64_memory/arena_node_size from OTR, defaulting to 0x{:X}.",
                    sArenaNodeSize);
        return;
    }

    auto stream = std::make_shared<Ship::MemoryStream>(file->Buffer->data(), file->Buffer->size());
    const auto reader = std::make_shared<Ship::BinaryReader>(stream);
    reader->SetEndianness(Ship::Endianness::Big);

    sArenaNodeSize = reader->ReadUInt32();

    SPDLOG_INFO("[N64SizeData] Arena node size: 0x{:X}.", sArenaNodeSize);
}

extern "C" uint32_t N64SizeData_GetArenaNodeSize() {
    LoadArenaNodeSize();
    return sArenaNodeSize;
}