#include "N64HeapCore.h"
#include "N64HeapTables.h"

#include <cstdio>
#include <cstring>

namespace n64heap {

static uint32_t Align16(uint32_t value) {
    return (value + 15) & ~15u;
}

// ---------------------------------------------------------------------------
// Arena: __osMallocInit / __osMalloc / __osMallocR / __osFree (PLATFORM_N64)
// ---------------------------------------------------------------------------

void Arena::Init(uint32_t start, uint32_t size) {
    uint32_t first = Align16(start);
    size = (size - (first - start)) & ~15u;
    mStart = first;
    mEnd = first + size;
    mBlocks.assign(1, Block{ first, size - kArenaNodeSize, true });
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
                node = block.start + block.size - size; // iter + (iter->size - size)
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

// ---------------------------------------------------------------------------
// Core
// ---------------------------------------------------------------------------

Core::Core(const HostSizes& hostSizes) : mHost(hostSizes) {
    mArena.Init(kZeldaArenaStart, kZeldaArenaSize);
}

void Core::ArenaInit(bool skipNextMagicDark) {
    // Play_Init -> ZeldaArena_Init; Actor_InitContext clears every overlay entry.
    // The cutscene pointer is not part of the heap and is kept by the caller.
    mArena.Init(kZeldaArenaStart, kZeldaArenaSize);
    mPending.clear();
    mLive.clear();
    mIgnored.clear();
    mOverlays.clear();
    mAbsoluteSpace = 0;
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

void Core::OnAlloc(const void* host, size_t size, const char* file, bool reverse) {
    mPending.push_back(Event{ true, host, size, Classify(file), reverse, -1, false });
}

void Core::OnFree(const void* host) {
    mPending.push_back(Event{ false, host, 0, Site::Other, false, -1, false });
}

void Core::OnActorSpawn(const void* host, int16_t actorId) {
    // SoH reports the spawn after the actor's init, so nested spawns (Navi in
    // Player_Init) arrive in reverse. The instance allocation itself was
    // buffered at the right position; tag it with the actor id here.
    for (auto it = mPending.rbegin(); it != mPending.rend(); ++it) {
        if (it->isAlloc && it->host == host && it->site == Site::ActorSpawn && it->actorId < 0) {
            it->actorId = actorId;
            bool known = actorId >= 0 && actorId < kActorCount && kActors[actorId].valid;
            if (!known) {
                it->ignore = true; // SoH-only actor: not part of the N64 heap
            } else if (actorId == kActorMagicDark && mSkipMagicDark) {
                it->ignore = true;
                mSkipMagicDark = false;
            }
            return;
        }
    }
    mStats.unresolvedSpawns++;
}

uint32_t Core::TranslateSize(const Event& event) {
    size_t size = event.size;
    switch (event.site) {
        case Site::Player:
            return kGiObjectSegmentSize; // giObjectSegment is the only z_player.c allocation
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
            return static_cast<uint32_t>(size); // Vtx buffers: same size on N64
        case Site::Camera:
            return kCameraSize;
        case Site::ActorSpawn:
            // Unresolved z_actor.c allocations are BodyBreak_Alloc's three arrays:
            // MtxF[count+1] (same size), Gfx*[count+1] (4-byte pointers on N64),
            // s16[count+1] (same size).
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
            return static_cast<uint32_t>(size); // Vec3s / s16 tables: same size on N64
        case Site::Effect:
        case Site::Other:
        default:
            mStats.unknownSites++;
            return static_cast<uint32_t>(size);
    }
}

void Core::ApplyAlloc(const Event& event) {
    if (event.ignore) {
        mIgnored.insert(event.host);
        return;
    }
    Live live{ 0, -1 };
    if (event.site == Site::ActorSpawn && event.actorId >= 0) {
        const ActorEntry& entry = kActors[event.actorId];
        if (entry.overlaySize != 0) {
            // Actor_Spawn (z_actor.c:3198-3240): load the overlay before the instance.
            auto it = mOverlays.find(event.actorId);
            if (it == mOverlays.end()) {
                uint32_t address;
                if (entry.allocType & 1) { // ACTOROVL_ALLOC_ABSOLUTE
                    if (mAbsoluteSpace == 0) {
                        mAbsoluteSpace = mArena.Alloc(kAbsoluteSpaceSize, true);
                    }
                    address = mAbsoluteSpace;
                } else {
                    address = mArena.Alloc(entry.overlaySize, (entry.allocType & 2) != 0);
                }
                if (address == 0) {
                    mStats.failedAllocs++;
                    mIgnored.insert(event.host);
                    return;
                }
                it = mOverlays.emplace(event.actorId, Overlay{ address, 0 }).first;
            }
            it->second.count++;
        }
        live.address = mArena.Alloc(entry.instanceSize, false);
        live.actorId = event.actorId;
    } else {
        live.address = mArena.Alloc(TranslateSize(event), event.reverse);
    }
    if (live.address == 0) {
        mStats.failedAllocs++;
        mIgnored.insert(event.host);
        return;
    }
    mStats.allocs++;
    mLive[event.host] = live;
}

void Core::ApplyFree(const Event& event) {
    if (mIgnored.erase(event.host) != 0) {
        return;
    }
    auto it = mLive.find(event.host);
    if (it == mLive.end()) {
        mStats.unknownFrees++; // allocated before the shadow was reset
        return;
    }
    Live live = it->second;
    mLive.erase(it);
    mArena.Free(live.address);
    mStats.frees++;
    if (live.actorId >= 0) {
        // Actor_Delete -> Actor_FreeOverlay (z_actor.c:3142)
        auto ovl = mOverlays.find(live.actorId);
        if (ovl != mOverlays.end() && --ovl->second.count == 0) {
            uint8_t type = kActors[live.actorId].allocType;
            if (type & 2) {
                // persistent: stays loaded
            } else if (type & 1) {
                mOverlays.erase(ovl); // absolute space stays allocated
            } else {
                mArena.Free(ovl->second.address);
                mOverlays.erase(ovl);
            }
        }
    }
}

void Core::Flush() {
    for (const Event& event : mPending) {
        if (event.isAlloc) {
            ApplyAlloc(event);
        } else {
            ApplyFree(event);
        }
    }
    mPending.clear();
    if (!mArena.Check()) {
        mStats.arenaCorrupt = true;
    }
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
            *n64Address = entry.value; // scene data: absolute N64 address
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
    // Candidates: actor-owned scripts whose overlay is loaded, then scripts in
    // the current scene's file. Scripts that share their first four words
    // cannot be told apart; the result is then flagged as ambiguous.
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
