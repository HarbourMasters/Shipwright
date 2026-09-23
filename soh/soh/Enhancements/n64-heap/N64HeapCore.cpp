#include "N64HeapCore.h"
#include "N64HeapTables.h"

#include <cstdio>
#include <cstring>

namespace n64heap {

static uint32_t Align16(uint32_t value) {
    return (value + 15) & ~15u;
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

Core::Core(const HostSizes& hostSizes) : mHost(hostSizes) {
}

void Core::ArenaInit(uint32_t arenaSize, bool skipNextMagicDark) {
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
    mBodyBreakStage = 0;
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
        { "z_effect_soft_sprite.c", Site::Effect },
    };
    for (const Rule& rule : kRules) {
        if (strcmp(base, rule.name) == 0) {
            return rule.site;
        }
    }
    return Site::Other;
}

bool Core::ActorSpawn(int16_t actorId) {
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
        auto it = mOverlays.find(actorId);
        if (it == mOverlays.end()) {
            uint32_t address;
            if (entry.allocType & 1) {
                if (mAbsoluteSpace == 0) {
                    mAbsoluteSpace = mArena.Alloc(kAbsoluteSpaceSize, true);
                }
                address = mAbsoluteSpace;
            } else {
                address = mArena.Alloc(entry.overlaySize, (entry.allocType & 2) != 0);
            }
            if (address == 0) {
                mStats.failedSpawns++;
                return false;
            }
            it = mOverlays.emplace(actorId, Overlay{ address, 0 }).first;
        }
        overlay = &it->second;
    }
    uint32_t address = mArena.Alloc(entry.instanceSize, false);
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

void Core::AbortSpawn() {
    mSpawn = PendingSpawn{};
}

bool Core::EffectSpawn(int32_t type) {
    if (type < 0 || type >= kEffectCount || kEffectOverlaySizes[type] == 0 || mEffects.count(type) != 0) {
        return true;
    }
    uint32_t address = mArena.Alloc(kEffectOverlaySizes[type], true);
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
    uint8_t type = kActors[actorId].allocType;
    if (type & 2) {
        return; // Persistent overlays stay loaded until the scene ends
    }
    if (!(type & 1)) {
        mArena.Free(it->second.address);
    }
    mOverlays.erase(it);
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
            // BodyBreak_Alloc: MtxF[n] and s16[n] match N64, Gfx*[n] uses 4 byte pointers there
            if (mBodyBreakStage == 1 && size == mBodyBreakCount * mHost.pointer) {
                mBodyBreakStage = 2;
                return mBodyBreakCount * kPointerSize;
            }
            if (mBodyBreakStage == 2 && size == mBodyBreakCount * 2) {
                mBodyBreakStage = 0;
                return static_cast<uint32_t>(size);
            }
            if (size % 0x40 == 0) {
                mBodyBreakCount = static_cast<uint32_t>(size / 0x40);
                mBodyBreakStage = 1;
                return static_cast<uint32_t>(size);
            }
            mStats.approximateSizes++;
            return static_cast<uint32_t>(size);
        case Site::SkelAnime:
        case Site::Curve:
            return static_cast<uint32_t>(size);
        case Site::Effect:
        case Site::Other:
        default:
            mStats.unknownSites++;
            return static_cast<uint32_t>(size);
    }
}

void Core::OnAlloc(const void* host, size_t size, const char* file, bool reverse) {
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
    uint32_t address = mArena.Alloc(TranslateSize(site, size), reverse);
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
        snprintf(text, sizeof(text), "free block %08X+0x%X (+0x%X, leftover data)", payload, size, address - payload);
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

bool Core::ResolvePath(const char* path, uint32_t* n64Address, std::string* what) const {
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
                *what = std::string("scene 0x") + "0123456789ABCDEF"[(entry.owner >> 4) & 0xF] +
                        "0123456789ABCDEF"[entry.owner & 0xF] + " " + entry.name;
            }
            return true;
        }
    }
    return false;
}

bool Core::ResolveScript(const int32_t words[4], int16_t sceneId, uint32_t* n64Address, std::string* what) const {
    // Actor scripts in loaded overlays are checked before the current scene's scripts
    for (int pass = 0; pass < 2; pass++) {
        bool found = false;
        int candidates = 0;
        char text[96] = "";
        for (const ScriptEntry& entry : kScripts) {
            if ((pass == 0) != (entry.isScene == 0) || memcmp(entry.words, words, sizeof(entry.words)) != 0) {
                continue;
            }
            uint32_t value;
            if (entry.isScene) {
                if (entry.owner != sceneId) {
                    continue;
                }
                value = entry.value;
            } else {
                uint32_t base = OverlayAddress(entry.owner);
                if (base == 0) {
                    continue;
                }
                value = base + entry.value;
            }
            if (found && value == *n64Address) {
                continue;
            }
            candidates++;
            if (!found) {
                found = true;
                *n64Address = value;
                if (entry.isScene) {
                    snprintf(text, sizeof(text), "scene 0x%02X file data", entry.owner);
                } else {
                    snprintf(text, sizeof(text), "actor 0x%03X overlay %08X + 0x%X", entry.owner, value - entry.value,
                             entry.value);
                }
            }
        }
        if (found) {
            if (what != nullptr) {
                *what = text;
                if (candidates > 1) {
                    *what += " (ambiguous: " + std::to_string(candidates) + " scripts share this header)";
                }
            }
            return true;
        }
    }
    return false;
}

} // namespace n64heap
