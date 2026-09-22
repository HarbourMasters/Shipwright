#pragma once

// Model of what NTSC 1.2 holds in the PlayState memory around the object
// space: the scene file, the object space (objects, and the pause screen data
// that KaleidoScope loads over it), and the fixed areas above the scene file.
//
// Memory here is not cleared between scenes, so a stale cutscene pointer
// (for example the title-screen one) reads whatever was written there last.
// The model records the last writer of every address and reads the N64 bytes
// back (through a RomReader) to simulate the cutscene parser.

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace n64heap {

// Reads `size` bytes of NTSC 1.2 file data at `vrom`.
using RomReader = std::function<bool(uint32_t vrom, uint8_t* out, uint32_t size)>;

// Outcome of NTSC 1.2 Cutscene_ProcessScript running the modelled bytes.
struct ScriptSimulation {
    enum class Outcome {
        Unknown,         // bytes not deterministic / not modelled: cannot reproduce
        EndsImmediately, // csFrameCount < 0: stops on the first frame
        NoCommands,      // no recognised command runs; lasts csFrameCount frames
        Hang,            // the s16 loop counters never finish: the N64 locks up
        RunsCommands,    // a recognised command would run: cannot reproduce
    };
    Outcome outcome = Outcome::Unknown;
    bool headerKnown = false; // totalEntries and frameCount were read
    int32_t totalEntries = 0;
    int32_t frameCount = 0;
    std::string source; // what was written at the pointer last
    std::string detail;
};

class ObjectSpace {
  public:
    void SceneInit(int16_t sceneId);
    void SyncObjects(const int16_t* ids, int count);
    void PauseOpened(int linkAge, bool japanese, int16_t worldMapArea);
    void GameOverOpened(bool japanese);
    void PauseClosed(); // Object_ReloadAll

    ScriptSimulation Simulate(uint32_t address, const RomReader& rom) const;
    uint32_t Version() const {
        return mVersion;
    } // changes whenever modelled memory changes

  private:
    enum class Kind { File, GreyIcon, Dynamic };
    struct Span {
        uint32_t end;
        Kind kind;
        uint32_t vrom; // file data: vrom of the byte at the span's file base
        uint32_t base; // N64 address where the file (or icon) starts
        std::string label;
    };
    void Paint(uint32_t start, uint32_t size, Kind kind, uint32_t vrom, uint32_t base, const std::string& label);
    void PaintFile(uint32_t address, uint32_t vrom, uint32_t size, const std::string& label);
    void PaintObject(size_t slot);
    bool ReadWord(uint32_t address, const RomReader& rom, uint32_t* value, std::string* source) const;

    std::map<uint32_t, Span> mSpans; // keyed by start address, non-overlapping
    uint32_t mVersion = 0;
    int16_t mSceneId = -1;
    uint32_t mSpaceStart = 0;
    std::vector<int16_t> mSlotIds;
    std::vector<uint32_t> mSlotAddr;
};

} // namespace n64heap
