#pragma once

#include <cstdint>
#include <vector>

namespace SOH {

/*
 * AudioResampler — polyphase sinc resampler for integer ratios.
 *
 * Designed for the specific case of console audio upsampling from 32000 Hz
 * to 48000 Hz (ratio 3/2 exact). Works for any integer ratio P/Q where
 * P = outRate / gcd(outRate, inRate) and Q = inRate / gcd(outRate, inRate).
 *
 * Architecture:
 *   - Polyphase decomposition of a windowed-sinc lowpass filter.
 *   - Filter cutoff at min(inRate, outRate) / 2 to prevent aliasing.
 *   - Kaiser window (beta=6) for a good stopband attenuation (~60 dB).
 *   - For 32k→48k: P=3, Q=2, 8 taps per phase → 24 total filter coefficients.
 *
 * Usage:
 *   AudioResampler r(32000, 48000, numChannels);
 *   r.Process(inFloat, inFrames, outFloat, outFrames);
 *
 * Process() returns the number of output frames actually written.
 * State (history samples) is preserved between calls for continuous streams.
 * Samples are interleaved float in nominal [-1, 1] range; the polyphase
 * filter is unity-gain so peaks slightly above 1.0 may pass through and
 * should be soft-clipped by the caller (or before reaching the backend).
 */
class AudioResampler {
  public:
    AudioResampler(int32_t inRate, int32_t outRate, int32_t numChannels);

    /* Resample inFrames input frames into outBuf.
     * Returns number of output frames written.
     * outBuf must be large enough for ceil(inFrames * outRate / inRate) frames.
     *
     * Two overloads:
     *  - float in / float out is the canonical path used by the float audio
     *    pipeline. Samples are interleaved float in nominal [-1, 1].
     *  - int16_t in / int16_t out is the legacy entry point preserved for
     *    libultraship consumers still on the s16 path. It converts at the
     *    boundaries and clamps the output to the s16 range; the inner DSP
     *    is identical (the filter coefficients live in float either way). */
    int32_t Process(const float* inBuf, int32_t inFrames, float* outBuf, int32_t maxOutFrames);
    int32_t Process(const int16_t* inBuf, int32_t inFrames, int16_t* outBuf, int32_t maxOutFrames);

    /* Maximum output frames for a given number of input frames. */
    int32_t MaxOutputFrames(int32_t inFrames) const;

    /* Reset history (e.g. on stream discontinuity). */
    void Reset();

  private:
    int32_t mInRate;
    int32_t mOutRate;
    int32_t mNumChannels;

    /* Rational ratio P/Q after GCD reduction */
    int32_t mP; /* upsample factor */
    int32_t mQ; /* downsample factor */

    /* Polyphase filter — mNumPhases phases × mTapsPerPhase taps */
    static constexpr int kTapsPerPhase = 8;
    int32_t mNumPhases;         /* = P */
    std::vector<float> mCoeffs; /* [phase * kTapsPerPhase + tap] */

    /* Current phase index in [0, P) */
    int32_t mPhase;

    /* History buffer: kTapsPerPhase-1 frames per channel for convolution state */
    std::vector<float> mHistory; /* [(kTapsPerPhase-1) * numChannels] */

    void BuildFilter();
    static float BesselI0(float x);
    static float KaiserWindow(int n, int N, float beta);
    static float Sinc(float x);

    static inline int32_t GCD(int32_t a, int32_t b) {
        while (b) {
            int32_t t = b;
            b = a % b;
            a = t;
        }
        return a;
    }
};

} // namespace SOH
