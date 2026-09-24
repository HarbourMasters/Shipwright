#pragma once

// Mirrors SoH's Zelda arena traffic into an emulation of the NTSC 1.2 allocator using N64 sizes

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace n64heap {

struct SceneLayout;
struct ScriptEntry;

// Layers a scene does not define use its base layout
const SceneLayout* FindSceneLayout(int16_t scene, uint8_t layer);

// Host sizes of the structs whose size differs on N64
struct HostSizes {
    size_t jntSphElement;
    size_t trisElement;
    size_t skinLimbVtx;
    size_t pointer;
};

class Arena {
  public:
    void Init(uint32_t start, uint32_t size);
    uint32_t Alloc(uint32_t size, bool reverse); // 0 when it does not fit
    bool Free(uint32_t address);
    bool Check() const;
    uint32_t LargestFree() const;
    bool Locate(uint32_t address, uint32_t* payload, uint32_t* size, bool* free, bool* header) const;

  private:
    struct Block {
        uint32_t start;
        uint32_t size;
        bool free;
    };
    uint32_t mStart = 0;
    uint32_t mEnd = 0;
    std::vector<Block> mBlocks;
};

// The last bytes written to each address of the Zelda arena, which the N64 never clears
class Memory {
  public:
    void Forget(uint32_t address, uint32_t size);
    // owner is the id of the actor whose instance held the bytes
    void Write(uint32_t address, const std::vector<uint8_t>& bytes, const std::vector<bool>& known, int16_t owner);
    bool ReadWord(uint32_t address, uint32_t* value) const;
    bool Owner(uint32_t address, int16_t* owner, uint32_t* origin) const;

  private:
    struct Source {
        uint32_t origin;
        int16_t owner;
    };
    void Reserve(uint32_t end);
    std::vector<uint8_t> mBytes;
    std::vector<bool> mKnown;
    std::vector<Source> mSources;
};

struct Stats {
    uint32_t allocs = 0;
    uint32_t frees = 0;
    uint32_t failedAllocs = 0;
    uint32_t failedSpawns = 0;
    uint32_t failedEffects = 0;
    uint32_t unknownFrees = 0;
    uint32_t unresolvedSpawns = 0;
    uint32_t approximateSizes = 0;
    uint32_t unknownSites = 0;
    bool arenaCorrupt = false;
};

class Core {
  public:
    explicit Core(const HostSizes& hostSizes);

    void ArenaInit(uint32_t arenaSize, bool skipNextMagicDark);
    uint32_t ArenaSize() const {
        return mArenaSize;
    }

    // Places the N64 overlay and instance for a spawn; the next z_actor.c allocation is SoH's instance
    bool ActorSpawn(int16_t actorId);
    void AbortSpawn();
    bool EffectSpawn(int32_t type);
    void OnAlloc(const void* host, size_t size, const char* file, bool reverse);
    void OnFree(const void* host);

    bool ResolveScript(const int32_t words[4], int16_t sceneId, uint32_t* n64Address, std::string* what) const;
    bool ResolvePath(const char* path, uint32_t* n64Address, std::string* what) const;
    uint32_t AddressOf(const void* host) const;
    // The host actor whose N64 instance contains address, or nullptr
    const void* FindActorAt(uint32_t address, uint32_t* instanceAddress) const;
    bool FindActor(const void* host, uint32_t* address, int16_t* actorId) const;
    // Keeps the N64 bytes of an instance about to be freed, as the memory still holds them afterwards
    void RecordLeftover(uint32_t address, const std::vector<uint8_t>& bytes, const std::vector<bool>& known,
                        int16_t actorId);
    bool ReadLeftover(uint32_t address, uint32_t* value) const;
    uint32_t LargestFree() const;
    std::string DescribeAddress(uint32_t address) const;
    const Stats& GetStats() const {
        return mStats;
    }

  private:
    enum class Site { ActorSpawn, Player, Collision, SkelAnime, Curve, Skin, Camera, Other };
    struct Live {
        uint32_t address;
        int16_t actorId;
    };
    struct Overlay {
        uint32_t address;
        int count;
    };
    struct PendingSpawn {
        bool active = false;
        uint32_t address = 0;
        int16_t actorId = -1;
    };
    enum class BodyBreakStep { Matrices, DisplayLists, ObjectIds };

    static Site Classify(const char* file);
    uint32_t Allocate(uint32_t size, bool reverse);
    uint32_t TranslateSize(Site site, size_t size);
    uint32_t TranslateBodyBreakSize(size_t size);
    Overlay* LoadOverlay(int16_t actorId);
    void FreeUnusedOverlay(int16_t actorId);
    uint32_t OverlayAddress(int16_t actorId) const;
    const ScriptEntry* FindScript(const int32_t words[4], bool isScene, int16_t sceneId, uint32_t* n64Address,
                                  int* candidates) const;
    void Check();

    HostSizes mHost;
    Arena mArena;
    Memory mMemory;
    uint32_t mArenaSize = 0;
    std::unordered_map<const void*, Live> mLive;
    std::unordered_set<const void*> mIgnored;
    std::unordered_map<int16_t, Overlay> mOverlays;
    std::unordered_map<int32_t, uint32_t> mEffects;
    PendingSpawn mSpawn;
    uint32_t mAbsoluteSpace = 0;
    bool mSkipMagicDark = false;
    uint32_t mBodyBreakCount = 0;
    BodyBreakStep mBodyBreakStep = BodyBreakStep::Matrices;
    Stats mStats;
};

} // namespace n64heap
