#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace SOH {

// The engine's reverb unit 0, for audio the engine does not synthesise itself. It reads the unit's
// live parameters, so it follows the audio spec, and runs at the output rate.
class EngineReverb {
  public:
    // Adds the reverb of `send` into `out`, both stereo interleaved.
    void Process(const float* send, float* out, uint32_t frameCount, int32_t outputRate);

  private:
    std::vector<float> mRing;
    size_t mRingPos = 0;
};

} // namespace SOH
