#include "EngineReverb.h"

#include <algorithm>

#include <libultraship/libultra.h>
#include "variables.h"

namespace SOH {

namespace {
constexpr int32_t kNativeRate = 32000;

// The engine's mix gain, applied as `out += in * gain >> 15`.
float MixGain(int16_t gain) {
    return static_cast<float>(gain) / 32768.0f;
}
} // namespace

void EngineReverb::Process(const float* send, float* out, uint32_t frameCount, int32_t outputRate) {
    if (gAudioContext.numSynthesisReverbs < 1) {
        return;
    }
    const SynthesisReverb& reverb = gAudioContext.synthesisReverbs[0];
    if (reverb.useReverb == 0) {
        return;
    }

    const int64_t nativeDelay = static_cast<int64_t>(reverb.windowSize) * reverb.downsampleRate;
    const size_t delay = static_cast<size_t>(std::max<int64_t>(1, nativeDelay * outputRate / kNativeRate));
    if (mRing.size() != delay * 2) {
        mRing.assign(delay * 2, 0.0f);
        mRingPos = 0;
    }

    const float returnGain = MixGain(reverb.unk_0A);
    // The engine mixes the ring into itself with this gain, so what feeds back is 1 + gain.
    const float feedback = 1.0f + MixGain(static_cast<int16_t>(static_cast<uint16_t>(reverb.unk_0C + 0x8000)));
    const float leakRtl = MixGain(reverb.leakRtl);
    const float leakLtr = MixGain(reverb.leakLtr);

    for (uint32_t i = 0; i < frameCount; i++) {
        float& ringLeft = mRing.at(mRingPos * 2);
        float& ringRight = mRing.at(mRingPos * 2 + 1);
        out[i * 2] += ringLeft * returnGain;
        out[i * 2 + 1] += ringRight * returnGain;

        const float decayedLeft = ringLeft * feedback;
        const float decayedRight = ringRight * feedback;
        ringLeft = decayedLeft + decayedRight * leakRtl + send[i * 2];
        ringRight = decayedRight + decayedLeft * leakLtr + send[i * 2 + 1];
        mRingPos = (mRingPos + 1) % delay;
    }
}

} // namespace SOH
