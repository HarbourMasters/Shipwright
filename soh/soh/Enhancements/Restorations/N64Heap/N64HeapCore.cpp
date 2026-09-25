#include "N64HeapCore.h"
#include "N64HeapTables.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace n64heap {

static uint32_t Align16(uint32_t value) {
    return (value + 15) & ~15u;
}

const SceneLayout* FindSceneLayout(int16_t scene, uint8_t layer) {
    const SceneLayout* baseLayout = nullptr;
    for (const SceneLayout& layout : kSceneLayouts) {
        if (layout.scene == scene && layout.layer == layer) {
            return &layout;
        }
        if (layout.scene == scene && layout.layer == 0) {
            baseLayout = &layout;
        }
    }
    return baseLayout;
}

void Arena::Init(uint32_t start, uint32_t size) {
    uint32_t first = Align16(start);
    size = (size - (first - start)) & ~15u;
    mStart = first;
    mEnd = first + size;
    mBlocks.clear();
    if (size > kArenaNodeSize) {
        mBlocks.push_back(Block{ first, size - kArenaNodeSize, true });
    } else {
        mEnd = first;
    }
}

uint32_t Arena::Alloc(uint32_t requested, bool reverse) {
    uint32_t size = Align16(requested);
    uint32_t blockSize = size + kArenaNodeSize;
    int count = static_cast<int>(mBlocks.size());
    for (int n = 0; n < count; n++) {
        int i = reverse ? count - 1 - n : n;
        Block& block = mBlocks[i];
        if (!block.free || block.size < size) {
            continue;
        }
        uint32_t node = block.start;
        if (blockSize < block.size) {
            if (reverse) {
                node = block.start + block.size - size;
                block.size -= blockSize;
                mBlocks.insert(mBlocks.begin() + i + 1, Block{ node, size, false });
            } else {
                Block rest{ node + blockSize, block.size - blockSize, true };
                block.size = size;
                block.free = false;
                mBlocks.insert(mBlocks.begin() + i + 1, rest);
            }
        } else {
            block.free = false;
        }
        return node + kArenaNodeSize;
    }
    return 0;
}

bool Arena::Free(uint32_t address) {
    uint32_t node = address - kArenaNodeSize;
    for (size_t i = 0; i < mBlocks.size(); i++) {
        if (mBlocks[i].start != node) {
            continue;
        }
        if (mBlocks[i].free) {
            return false;
        }
        mBlocks[i].free = true;
        if (i + 1 < mBlocks.size() && mBlocks[i + 1].free) {
            mBlocks[i].size += kArenaNodeSize + mBlocks[i + 1].size;
            mBlocks.erase(mBlocks.begin() + i + 1);
        }
        if (i > 0 && mBlocks[i - 1].free) {
            mBlocks[i - 1].size += kArenaNodeSize + mBlocks[i].size;
            mBlocks.erase(mBlocks.begin() + i);
        }
        return true;
    }
    return false;
}

bool Arena::Check() const {
    uint32_t cursor = mStart;
    bool previousFree = false;
    for (const Block& block : mBlocks) {
        if (block.start != cursor || (block.start & 15) || (block.size & 15) || (block.free && previousFree)) {
            return false;
        }
        previousFree = block.free;
        cursor = block.start + kArenaNodeSize + block.size;
    }
    return cursor == mEnd;
}

uint32_t Arena::LargestFree() const {
    uint32_t largest = 0;
    for (const Block& block : mBlocks) {
        if (block.free && block.size > largest) {
            largest = block.size;
        }
    }
    return largest;
}

bool Arena::Locate(uint32_t address, uint32_t* payload, uint32_t* size, bool* free, bool* header) const {
    for (const Block& block : mBlocks) {
        uint32_t end = block.start + kArenaNodeSize + block.size;
        if (address >= block.start && address < end) {
            *payload = block.start + kArenaNodeSize;
            *size = block.size;
            *free = block.free;
            *header = address < *payload;
            return true;
        }
    }
    return false;
}

void Memory::Reserve(uint32_t end) {
    uint32_t size = end - kZeldaArenaStart;
    if (size > mBytes.size()) {
        mBytes.resize(size, 0);
        mKnown.resize(size, false);
        mSources.resize(size, Source{ 0, -1 });
    }
}

void Memory::Forget(uint32_t address, uint32_t size) {
    if (address < kZeldaArenaStart || size == 0) {
        return;
    }
    Reserve(address + size);
    uint32_t start = address - kZeldaArenaStart;
    std::fill(mKnown.begin() + start, mKnown.begin() + start + size, false);
    std::fill(mSources.begin() + start, mSources.begin() + start + size, Source{ 0, -1 });
}

void Memory::Write(uint32_t address, const std::vector<uint8_t>& bytes, const std::vector<bool>& known, int16_t owner) {
    if (address < kZeldaArenaStart || bytes.empty()) {
        return;
    }
    uint32_t size = static_cast<uint32_t>(bytes.size());
    Reserve(address + size);
    uint32_t start = address - kZeldaArenaStart;
    for (uint32_t i = 0; i < size; i++) {
        mBytes[start + i] = bytes[i];
        mKnown[start + i] = known[i];
        mSources[start + i] = Source{ address, owner };
    }
}

bool Memory::ReadWord(uint32_t address, uint32_t* value) const {
    if (address < kZeldaArenaStart || address + 4 - kZeldaArenaStart > mBytes.size()) {
        return false;
    }
    uint32_t start = address - kZeldaArenaStart;
    uint32_t word = 0;
    for (uint32_t i = 0; i < 4; i++) {
        if (!mKnown[start + i]) {
            return false;
        }
        word = (word << 8) | mBytes[start + i];
    }
    *value = word;
    return true;
}

bool Memory::Owner(uint32_t address, int16_t* owner, uint32_t* origin) const {
    if (address < kZeldaArenaStart || address - kZeldaArenaStart >= mSources.size()) {
        return false;
    }
    const Source& source = mSources[address - kZeldaArenaStart];
    *owner = source.owner;
    *origin = source.origin;
    return source.owner >= 0;
}

Core::Core(const HostSizes& hostSizes) : mHost(hostSizes) {
}

void Core::Reset() {
    *this = Core(mHost);
}

void Core::ArenaInit(uint32_t arenaSize, bool skipNextMagicDark) {
    // Memory past a smaller arena holds other scene data, and __osMallocInit writes the first node
    if (arenaSize < mArenaSize) {
        mMemory.Forget(kZeldaArenaStart + arenaSize, mArenaSize - arenaSize);
    }
    mMemory.Forget(kZeldaArenaStart, kArenaNodeSize);
    mActive = true;
    mArenaSize = arenaSize;
    mArena.Init(kZeldaArenaStart, arenaSize);
    mLive.clear();
    mIgnored.clear();
    mOverlays.clear();
    mEffects.clear();
    mSpawn = PendingSpawn{};
    mAbsoluteSpace = 0;
    // SoH's game over sets nayrusLoveTimer to 2000, so Player_Init spawns a Magic_Dark that NTSC 1.2 does not
    mSkipMagicDark = skipNextMagicDark;
    mBodyBreakStep = BodyBreakStep::Matrices;
}

Core::Site Core::Classify(const char* file) {
    if (file == nullptr) {
        return Site::Other;
    }
    const char* base = file;
    for (const char* p = file; *p != '\0'; p++) {
        if (*p == '/' || *p == '\\') {
            base = p + 1;
        }
    }
    struct Rule {
        const char* name;
        Site site;
    };
    static const Rule kRules[] = {
        { "z_actor.c", Site::ActorSpawn },
        { "z_player.c", Site::Player },
        { "z_collision_check.c", Site::Collision },
        { "z_skelanime.c", Site::SkelAnime },
        { "z_fcurve_data_skelanime.c", Site::Curve },
        { "z_skin_awb.c", Site::Skin },
        { "z_camera.c", Site::Camera },
    };
    for (const Rule& rule : kRules) {
        if (strcmp(base, rule.name) == 0) {
            return rule.site;
        }
    }
    return Site::Other;
}

bool Core::ActorSpawn(int16_t actorId) {
    if (!mActive) {
        return true;
    }
    if (mSpawn.active) {
        mStats.unresolvedSpawns++;
    }
    mSpawn = PendingSpawn{ true, 0, actorId };
    if (actorId < 0 || actorId >= kActorCount || !kActors[actorId].valid) {
        return true;
    }
    if (actorId == kActorMagicDark && mSkipMagicDark) {
        mSkipMagicDark = false;
        return true;
    }
    const ActorEntry& entry = kActors[actorId];
    Overlay* overlay = nullptr;
    if (entry.overlaySize != 0) {
        overlay = LoadOverlay(actorId);
        if (overlay == nullptr) {
            mStats.failedSpawns++;
            return false;
        }
    }
    uint32_t address = Allocate(entry.instanceSize, false);
    if (address == 0) {
        FreeUnusedOverlay(actorId);
        mStats.failedSpawns++;
        Check();
        return false;
    }
    if (overlay != nullptr) {
        overlay->count++;
    }
    mSpawn.address = address;
    mStats.allocs++;
    Check();
    return true;
}

Core::Overlay* Core::LoadOverlay(int16_t actorId) {
    auto it = mOverlays.find(actorId);
    if (it != mOverlays.end()) {
        return &it->second;
    }
    const ActorEntry& entry = kActors[actorId];
    uint32_t address = 0;
    switch (entry.allocType) {
        case AllocType::Absolute:
            // Every absolute overlay shares one space, allocated the first time any of them loads
            if (mAbsoluteSpace == 0) {
                mAbsoluteSpace = Allocate(kAbsoluteSpaceSize, true);
            }
            address = mAbsoluteSpace;
            break;
        case AllocType::Persistent:
            address = Allocate(entry.overlaySize, true);
            break;
        case AllocType::Normal:
            address = Allocate(entry.overlaySize, false);
            break;
    }
    if (address == 0) {
        return nullptr;
    }
    return &mOverlays.emplace(actorId, Overlay{ address, 0 }).first->second;
}

void Core::AbortSpawn() {
    mSpawn = PendingSpawn{};
}

bool Core::EffectSpawn(int32_t type) {
    if (!mActive || type < 0 || type >= kEffectCount || kEffectOverlaySizes[type] == 0 || mEffects.count(type) != 0) {
        return true;
    }
    uint32_t address = Allocate(kEffectOverlaySizes[type], true);
    if (address == 0) {
        mStats.failedEffects++;
        return false;
    }
    mEffects.emplace(type, address);
    Check();
    return true;
}

void Core::FreeUnusedOverlay(int16_t actorId) {
    auto it = mOverlays.find(actorId);
    if (it == mOverlays.end() || it->second.count != 0) {
        return;
    }
    AllocType type = kActors[actorId].allocType;
    if (type == AllocType::Persistent) {
        return; // Persistent overlays stay loaded until the scene ends
    }
    if (type == AllocType::Normal) {
        mArena.Free(it->second.address);
    }
    mOverlays.erase(it);
}

// Node headers and new allocations overwrite what the memory held before
uint32_t Core::Allocate(uint32_t size, bool reverse) {
    uint32_t address = mArena.Alloc(size, reverse);
    if (address != 0) {
        mMemory.Forget(address - kArenaNodeSize, kArenaNodeSize + Align16(size) + kArenaNodeSize);
    }
    return address;
}

uint32_t Core::TranslateSize(Site site, size_t size) {
    switch (site) {
        case Site::Player:
            return kGiObjectSegmentSize;
        case Site::Collision: {
            bool jnt = size % mHost.jntSphElement == 0;
            bool tris = size % mHost.trisElement == 0;
            if (jnt && tris) {
                mStats.approximateSizes++;
            }
            if (jnt) {
                return static_cast<uint32_t>(size / mHost.jntSphElement * kJntSphElementSize);
            }
            if (tris) {
                return static_cast<uint32_t>(size / mHost.trisElement * kTrisElementSize);
            }
            mStats.approximateSizes++;
            return static_cast<uint32_t>(size);
        }
        case Site::Skin:
            if (size % mHost.skinLimbVtx == 0 && size % 16 != 0) {
                return static_cast<uint32_t>(size / mHost.skinLimbVtx * kSkinLimbVtxSize);
            }
            if (size % mHost.skinLimbVtx == 0) {
                mStats.approximateSizes++;
            }
            return static_cast<uint32_t>(size);
        case Site::Camera:
            return kCameraSize;
        case Site::ActorSpawn:
            return TranslateBodyBreakSize(size);
        case Site::SkelAnime:
        case Site::Curve:
            return static_cast<uint32_t>(size);
        case Site::Other:
        default:
            mStats.unknownSites++;
            return static_cast<uint32_t>(size);
    }
}

// Other z_actor.c allocations come from BodyBreak_Alloc, which allocates three arrays of the same length
uint32_t Core::TranslateBodyBreakSize(size_t size) {
    const size_t kMatrixSize = 0x40;
    if (mBodyBreakStep == BodyBreakStep::DisplayLists && size == mBodyBreakCount * mHost.pointer) {
        mBodyBreakStep = BodyBreakStep::ObjectIds;
        return mBodyBreakCount * kPointerSize;
    }
    if (mBodyBreakStep == BodyBreakStep::ObjectIds && size == mBodyBreakCount * sizeof(int16_t)) {
        mBodyBreakStep = BodyBreakStep::Matrices;
        return static_cast<uint32_t>(size);
    }
    if (size % kMatrixSize == 0) {
        mBodyBreakCount = static_cast<uint32_t>(size / kMatrixSize);
        mBodyBreakStep = BodyBreakStep::DisplayLists;
        return static_cast<uint32_t>(size);
    }
    mStats.approximateSizes++;
    return static_cast<uint32_t>(size);
}

void Core::OnAlloc(const void* host, size_t size, const char* file, bool reverse) {
    if (!mActive) {
        return;
    }
    Site site = Classify(file);
    if (site == Site::ActorSpawn && mSpawn.active) {
        if (mSpawn.address != 0) {
            mLive[host] = Live{ mSpawn.address, mSpawn.actorId };
        } else {
            mIgnored.insert(host);
        }
        mSpawn = PendingSpawn{};
        return;
    }
    uint32_t address = Allocate(TranslateSize(site, size), reverse);
    if (address == 0) {
        // Only recorded: failing this in SoH could crash where the N64 would misbehave
        mStats.failedAllocs++;
        mIgnored.insert(host);
        return;
    }
    mStats.allocs++;
    mLive[host] = Live{ address, -1 };
    Check();
}

void Core::OnFree(const void* host) {
    if (!mActive) {
        return;
    }
    if (mIgnored.erase(host) != 0) {
        return;
    }
    auto it = mLive.find(host);
    if (it == mLive.end()) {
        mStats.unknownFrees++;
        return;
    }
    Live live = it->second;
    mLive.erase(it);
    mArena.Free(live.address);
    mStats.frees++;
    if (live.actorId >= 0) {
        auto overlay = mOverlays.find(live.actorId);
        if (overlay != mOverlays.end()) {
            overlay->second.count--;
            FreeUnusedOverlay(live.actorId);
        }
    }
    Check();
}

void Core::Check() {
    if (!mArena.Check()) {
        mStats.arenaCorrupt = true;
    }
}

uint32_t Core::LargestFree() const {
    return mArena.LargestFree();
}

std::string Core::DescribeAddress(uint32_t address) const {
    uint32_t payload, size;
    bool free, header;
    char text[128];
    if (!mArena.Locate(address, &payload, &size, &free, &header)) {
        return "outside the actor heap";
    }
    if (header) {
        snprintf(text, sizeof(text), "arena node header of the block at %08X (+0x%X)", payload,
                 address - (payload - kArenaNodeSize));
        return text;
    }
    if (free) {
        int16_t owner;
        uint32_t origin;
        if (mMemory.Owner(address, &owner, &origin)) {
            snprintf(text, sizeof(text), "free block %08X+0x%X (+0x%X, left by actor 0x%03X at %08X +0x%X)", payload,
                     size, address - payload, owner, origin, address - origin);
        } else {
            snprintf(text, sizeof(text), "free block %08X+0x%X (+0x%X, leftover data)", payload, size,
                     address - payload);
        }
        return text;
    }
    for (const auto& overlay : mOverlays) {
        if (overlay.second.address == payload) {
            snprintf(text, sizeof(text), "overlay of actor 0x%03X at %08X +0x%X", overlay.first, payload,
                     address - payload);
            return text;
        }
    }
    for (const auto& effect : mEffects) {
        if (effect.second == payload) {
            snprintf(text, sizeof(text), "overlay of effect 0x%02X at %08X +0x%X", effect.first, payload,
                     address - payload);
            return text;
        }
    }
    if (mAbsoluteSpace == payload) {
        snprintf(text, sizeof(text), "absolute overlay space at %08X +0x%X", payload, address - payload);
        return text;
    }
    for (const auto& live : mLive) {
        if (live.second.address == payload) {
            if (live.second.actorId >= 0) {
                snprintf(text, sizeof(text), "instance of actor 0x%03X at %08X +0x%X", live.second.actorId, payload,
                         address - payload);
            } else {
                snprintf(text, sizeof(text), "allocation %08X+0x%X (+0x%X)", payload, size, address - payload);
            }
            return text;
        }
    }
    snprintf(text, sizeof(text), "untracked block %08X+0x%X (+0x%X)", payload, size, address - payload);
    return text;
}

uint32_t Core::OverlayAddress(int16_t actorId) const {
    auto it = mOverlays.find(actorId);
    return it == mOverlays.end() ? 0 : it->second.address;
}

uint32_t Core::AddressOf(const void* host) const {
    auto it = mLive.find(host);
    return it == mLive.end() ? 0 : it->second.address;
}

const void* Core::FindActorAt(uint32_t address, uint32_t* instanceAddress) const {
    for (const auto& live : mLive) {
        if (live.second.actorId < 0) {
            continue;
        }
        uint32_t start = live.second.address;
        if (address >= start && address < start + kActors[live.second.actorId].instanceSize) {
            *instanceAddress = start;
            return live.first;
        }
    }
    return nullptr;
}

bool Core::FindActor(const void* host, uint32_t* address, int16_t* actorId) const {
    auto it = mLive.find(host);
    if (it == mLive.end() || it->second.actorId < 0) {
        return false;
    }
    *address = it->second.address;
    *actorId = it->second.actorId;
    return true;
}

void Core::RecordLeftover(uint32_t address, const std::vector<uint8_t>& bytes, const std::vector<bool>& known,
                          int16_t actorId) {
    mMemory.Write(address, bytes, known, actorId);
}

bool Core::ReadLeftover(uint32_t address, uint32_t* value) const {
    return mMemory.ReadWord(address, value);
}

bool Core::ResolvePath(const char* path, uint32_t* n64Address, std::string* what) const {
    // SoH passes scene cutscenes as "__OTR__scenes/<mq|nonmq>/<scene file>/<symbol>"
    size_t length = strlen(path);
    for (const ScriptEntry& entry : kScripts) {
        size_t nameLength = strlen(entry.name);
        if (nameLength == 0 || nameLength + 1 > length) {
            continue;
        }
        const char* tail = path + length - nameLength;
        if (tail[-1] == '/' && strcmp(tail, entry.name) == 0) {
            *n64Address = entry.value;
            if (what != nullptr) {
                char text[96];
                snprintf(text, sizeof(text), "scene 0x%02X %s", entry.owner, entry.name);
                *what = text;
            }
            return true;
        }
    }
    return false;
}

const ScriptEntry* Core::FindScript(const int32_t words[4], bool isScene, int16_t sceneId, uint32_t* n64Address,
                                    int* candidates) const {
    const ScriptEntry* found = nullptr;
    *candidates = 0;
    for (const ScriptEntry& entry : kScripts) {
        if (entry.isScene != isScene || memcmp(entry.words, words, sizeof(entry.words)) != 0) {
            continue;
        }
        uint32_t address;
        if (isScene) {
            if (entry.owner != sceneId) {
                continue;
            }
            address = entry.value;
        } else {
            uint32_t overlay = OverlayAddress(entry.owner);
            if (overlay == 0) {
                continue;
            }
            address = overlay + entry.value;
        }
        if (found != nullptr && address == *n64Address) {
            continue;
        }
        (*candidates)++;
        if (found == nullptr) {
            found = &entry;
            *n64Address = address;
        }
    }
    return found;
}

bool Core::ResolveScript(const int32_t words[4], int16_t sceneId, uint32_t* n64Address, std::string* what) const {
    // Actor scripts in loaded overlays take priority over the current scene's scripts
    int candidates = 0;
    const ScriptEntry* entry = FindScript(words, false, sceneId, n64Address, &candidates);
    if (entry == nullptr) {
        entry = FindScript(words, true, sceneId, n64Address, &candidates);
    }
    if (entry == nullptr) {
        return false;
    }
    if (what != nullptr) {
        char text[96];
        if (entry->isScene) {
            snprintf(text, sizeof(text), "scene 0x%02X file data", entry->owner);
        } else {
            snprintf(text, sizeof(text), "actor 0x%03X overlay %08X + 0x%X", entry->owner, *n64Address - entry->value,
                     entry->value);
        }
        *what = text;
        if (candidates > 1) {
            *what += " (ambiguous: " + std::to_string(candidates) + " scripts share this header)";
        }
    }
    return true;
}

} // namespace n64heap
