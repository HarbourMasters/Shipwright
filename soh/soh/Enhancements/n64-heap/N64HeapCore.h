#pragma once

// N64 (NTSC 1.2) actor-heap shadow.
//
// Ship of Harkinian has no actor overlays and uses PC struct sizes, so the
// N64 addresses of actor-heap allocations never exist in its memory. This core
// replays SoH's Zelda-arena traffic into an emulation of the retail N64
// allocator (__osMalloc_n64.c) using N64 sizes from N64HeapTables.h, and adds
// the overlay allocations that the N64 Actor_Spawn performs.
//
// It has no SoH dependencies; N64Heap.cpp connects it to the game.

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace n64heap {

// PC sizes of the structs whose N64 size differs (sizeof in the host build).
struct HostSizes {
    size_t jntSphElement;
    size_t trisElement;
    size_t skinLimbVtx;
    size_t pointer;
};

class Arena {
  public:
    void Init(uint32_t start, uint32_t size);
    uint32_t Alloc(uint32_t size, bool reverse); // payload address, 0 on failure
    bool Free(uint32_t address);
    bool Check() const;

  private:
    struct Block {
        uint32_t start; // node address
        uint32_t size;  // payload size
        bool free;
    };
    uint32_t mStart = 0;
    uint32_t mEnd = 0;
    std::vector<Block> mBlocks;
};

struct Stats {
    uint32_t allocs = 0;
    uint32_t frees = 0;
    uint32_t failedAllocs = 0;
    uint32_t unknownFrees = 0;
    uint32_t unresolvedSpawns = 0;
    uint32_t approximateSizes = 0;
    uint32_t unknownSites = 0;
    bool arenaCorrupt = false;
};

class Core {
  public:
    explicit Core(const HostSizes& hostSizes);

    // ZeldaArena_Init. skipNextMagicDark: SoH's game over sets
    // nayrusLoveTimer = 2000 (PAL 1.1+ behaviour); NTSC 1.2 sets it to 0, so
    // the Magic_Dark that Player_Init spawns must not exist in the shadow.
    void ArenaInit(bool skipNextMagicDark);
    void OnAlloc(const void* host, size_t size, const char* file, bool reverse);
    void OnFree(const void* host);
    void OnActorSpawn(const void* host, int16_t actorId); // after Actor_Spawn, actor->id
    void Flush();                                         // apply buffered events in order

    // Resolve a cutscene script from its first four CutsceneData words.
    bool ResolveScript(const int32_t words[4], int16_t sceneId, uint32_t* n64Address, std::string* what) const;
    // Resolve a SoH resource path ("__OTR__scenes/<mq|nonmq>/<scene>/<symbol>").
    bool ResolvePath(const char* path, uint32_t* n64Address, std::string* what) const;

    uint32_t OverlayAddress(int16_t actorId) const; // 0 if not loaded
    const Stats& GetStats() const {
        return mStats;
    }

  private:
    enum class Site { ActorSpawn, Player, Collision, SkelAnime, Curve, Skin, Camera, Effect, Other };
    struct Event {
        bool isAlloc;
        const void* host;
        size_t size;
        Site site;
        bool reverse;
        int16_t actorId;
        bool ignore;
    };
    struct Live {
        uint32_t address;
        int16_t actorId; // instance of this actor, or -1
    };
    struct Overlay {
        uint32_t address;
        int count;
    };

    static Site Classify(const char* file);
    uint32_t TranslateSize(const Event& event);
    void ApplyAlloc(const Event& event);
    void ApplyFree(const Event& event);

    HostSizes mHost;
    Arena mArena;
    std::vector<Event> mPending;
    std::unordered_map<const void*, Live> mLive;
    std::unordered_set<const void*> mIgnored;
    std::unordered_map<int16_t, Overlay> mOverlays;
    uint32_t mAbsoluteSpace = 0;
    bool mSkipMagicDark = false;
    uint32_t mBodyBreakCount = 0; // element count of the last BodyBreak matrices allocation
    int mBodyBreakStage = 0;
    Stats mStats;
};

} // namespace n64heap
