#pragma once

// Tracks the last N64 data written around the object space, which a stale cutscene pointer can read

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace n64heap {

struct RomFile;

using RomReader = std::function<bool(uint32_t vrom, uint8_t* out, uint32_t size)>;
// Reads one N64 word and describes where it came from; false when the value is not known
using WordReader = std::function<bool(uint32_t address, uint32_t* value, std::string* source)>;

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
    // For RunsCommands: the commands the N64 parser runs, in the layout SoH's cutscene importer produces
    std::vector<int32_t> script;
};

ScriptSimulation SimulateCutscene(uint32_t address, const WordReader& read);

class ObjectSpace {
  public:
    void SceneInit(int16_t sceneId, uint8_t sceneLayer);
    void SyncObjects(const int16_t* ids, int count);
    void PauseOpened(int linkAge, bool japanese, int16_t worldMapArea);
    void GameOverOpened(bool japanese);
    void PauseClosed();

    bool ReadWord(uint32_t address, const RomReader& rom, uint32_t* value, std::string* source) const;
    uint32_t Version() const {
        return mVersion;
    }

  private:
    enum class Kind { File, GreyIcon, Runtime };
    struct Span {
        uint32_t end;
        Kind kind;
        uint32_t vrom;
        uint32_t base;
        std::string label;
    };
    void Paint(uint32_t start, uint32_t size, Kind kind, uint32_t vrom, uint32_t base, const std::string& label);
    void PaintFile(uint32_t address, const RomFile& file, const std::string& label);
    void PaintRuntimeData(uint32_t address, uint32_t size, const std::string& label);
    void PaintObject(size_t slot);
    uint32_t PaintIconFiles(uint32_t address, const RomFile& areaIcons, const char* areaLabel, bool japanese);

    std::map<uint32_t, Span> mSpans;
    uint32_t mVersion = 0;
    int16_t mSceneId = -1;
    uint32_t mSpaceStart = 0;
    std::vector<int16_t> mSlotIds;
    std::vector<uint32_t> mSlotAddresses;
};

} // namespace n64heap
