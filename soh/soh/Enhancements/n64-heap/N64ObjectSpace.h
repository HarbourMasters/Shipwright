#pragma once

// Tracks the last N64 data written around the object space, which a stale cutscene pointer can read

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace n64heap {

using RomReader = std::function<bool(uint32_t vrom, uint8_t* out, uint32_t size)>;

struct ScriptSimulation {
    enum class Outcome {
        Unknown,
        EndsImmediately,
        NoCommands,
        Hang,
        RunsCommands,
    };
    Outcome outcome = Outcome::Unknown;
    bool headerKnown = false;
    int32_t totalEntries = 0;
    int32_t frameCount = 0;
    std::string source;
    std::string detail;
};

class ObjectSpace {
  public:
    void SceneInit(int16_t sceneId, uint8_t sceneLayer);
    void SyncObjects(const int16_t* ids, int count);
    void PauseOpened(int linkAge, bool japanese, int16_t worldMapArea);
    void GameOverOpened(bool japanese);
    void PauseClosed();

    ScriptSimulation Simulate(uint32_t address, const RomReader& rom) const;
    uint32_t Version() const {
        return mVersion;
    }

  private:
    enum class Kind { File, GreyIcon, Dynamic };
    struct Span {
        uint32_t end;
        Kind kind;
        uint32_t vrom;
        uint32_t base;
        std::string label;
    };
    void Paint(uint32_t start, uint32_t size, Kind kind, uint32_t vrom, uint32_t base, const std::string& label);
    void PaintFile(uint32_t address, uint32_t vrom, uint32_t size, const std::string& label);
    void PaintObject(size_t slot);
    bool ReadWord(uint32_t address, const RomReader& rom, uint32_t* value, std::string* source) const;

    std::map<uint32_t, Span> mSpans;
    uint32_t mVersion = 0;
    int16_t mSceneId = -1;
    uint32_t mSpaceStart = 0;
    std::vector<int16_t> mSlotIds;
    std::vector<uint32_t> mSlotAddr;
};

} // namespace n64heap
