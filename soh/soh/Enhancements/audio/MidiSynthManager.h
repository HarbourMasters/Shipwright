#pragma once
#include "IMidiSynth.h"
#include <memory>
#include <mutex>

namespace SOH {

// Owns the optional IMidiSynth that, when present, replaces the engine's
// native audio synthesis. When no synth is installed, GetActiveSynth()
// returns nullptr and the audio thread should fall back to the native
// path.
class MidiSynthManager {
  public:
    static MidiSynthManager& Instance();

    // Install or remove the active synth. Thread-safe.
    // Must NOT be called from the audio thread.
    // Passing nullptr uninstalls (native synthesis takes over).
    void SetSynth(std::shared_ptr<IMidiSynth> synth);

    // Returns the installed synth, or nullptr if native synthesis is active.
    std::shared_ptr<IMidiSynth> GetActiveSynth();

  private:
    MidiSynthManager() = default;
    std::shared_ptr<IMidiSynth> mSynth;
    std::mutex mMutex;
};

} // namespace SOH
