#pragma once

#include <cstdint>
#include <vector>

namespace SOH {

/*
 * AudioResampler -- polyphase sinc resampler for rational ratios.
 *
 * Designed for the specific case of console audio upsampling from 32000 Hz
 * to 48000 Hz (ratio 3/2 exact), and works for any rational ratio P/Q with
 * P = outRate / gcd(outRate, inRate) and Q = inRate / gcd(outRate, inRate).
 * Upsampling only: outRate must be at least inRate, so that one output frame
 * never advances the input by more than one frame.
 *
 * Architecture:
 *   - Polyphase decomposition of a windowed-sinc lowpass filter.
 *   - Filter cutoff at min(inRate, outRate) / 2 to prevent aliasing.
 *   - Kaiser window (beta=6) for a good stopband attenuation (~60 dB).
 *   - For 32k->48k: P=3, Q=2, 8 taps per phase -> 24 total filter coefficients.
 *
 * Usage:
 *   AudioResampler r(32000, 48000, numChannels);
 *   r.Process(inFloat, inFrames, outFloat, outFrames);
 *
 * Process() returns the number of output frames actually written.
 * State (history samples) is preserved between calls, so a stream can be fed in
 * arbitrary batches without a seam at the joins. The filter is unity-gain, but
 * its ringing can carry a sample that was already near full scale past it, so
 * the float overload can return values slightly outside [-1, 1]; the s16
 * overload clamps.
 */
class AudioResampler {
  public:
    AudioResampler(int32_t inRate, int32_t outRate, int32_t numChannels);

    /* Resample inFrames input frames into outBuf.
     * Returns the number of output frames written.
     * outBuf must hold MaxOutputFrames(inFrames) frames. A smaller cap does not
     * overrun anything, but the input frames that did not fit are dropped rather
     * than carried over, so it is the only value worth passing.
     *
     * Two overloads:
     *  - int16_t in / int16_t out is what the engine bus uses. It converts at the
     *    boundaries and clamps back into the s16 range.
     *  - float in / float out is the core that the s16 overload wraps, taking
     *    interleaved samples in the nominal [-1, 1] range. Filtering happens in
     *    float either way, so the two differ only at the boundaries.
     *
     * Neither allocates once the scratch buffers have grown to the largest batch
     * seen, which is what makes them safe to call from the audio thread. */
    int32_t Process(const float* inBuf, int32_t inFrames, float* outBuf, int32_t maxOutFrames);
    int32_t Process(const int16_t* inBuf, int32_t inFrames, int16_t* outBuf, int32_t maxOutFrames);

    /* Maximum output frames for a given number of input frames. */
    int32_t MaxOutputFrames(int32_t inFrames) const;

    /* Reset history (e.g. on stream discontinuity). */
    void Reset();

  private:
    int32_t mNumChannels;

    /* Rational ratio P/Q after GCD reduction */
    int32_t mP; /* upsample factor */
    int32_t mQ; /* downsample factor */

    /* Polyphase filter -- mNumPhases phases x kTapsPerPhase taps */
    static constexpr int kTapsPerPhase = 8;
    int32_t mNumPhases;         /* = P */
    std::vector<float> mCoeffs; /* [phase * kTapsPerPhase + tap] */

    /* Current phase index in [0, P) */
    int32_t mPhase;

    /* History buffer: kTapsPerPhase-1 frames per channel for convolution state */
    std::vector<float> mHistory; /* [(kTapsPerPhase-1) * numChannels] */

    /* Scratch window (history + current input), kept as a member so Process()
     * does not allocate on the audio thread. */
    std::vector<float> mWindow;

    /* Float staging for the s16 overload, member-held for the same reason. */
    std::vector<float> mInScratch;
    std::vector<float> mOutScratch;

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
