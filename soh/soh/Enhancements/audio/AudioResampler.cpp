#include "soh/Enhancements/audio/AudioResampler.h"

#include <cmath>
#include <cassert>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SOH {

AudioResampler::AudioResampler(int32_t inRate, int32_t outRate, int32_t numChannels)
    : mNumChannels(numChannels), mPhase(0) {

    /* One output frame advances the input by at most one frame below. */
    assert(outRate >= inRate);

    int32_t g = GCD(inRate, outRate);
    mP = outRate / g; /* upsample factor   (e.g. 3 for 32k->48k) */
    mQ = inRate / g;  /* downsample factor (e.g. 2 for 32k->48k) */
    mNumPhases = mP;

    BuildFilter();

    /* Zeroed so the first output frames fade in from silence rather than a step. */
    mHistory.assign((kTapsPerPhase - 1) * mNumChannels, 0.0f);
}

float AudioResampler::BesselI0(float x) {
    /* Series expansion; 30 terms converge for the beta values used here. */
    float sum = 1.0f;
    float term = 1.0f;
    float half_x = x * 0.5f;
    for (int k = 1; k <= 30; k++) {
        term *= (half_x / (float)k);
        term *= (half_x / (float)k);
        sum += term;
        if (term < 1e-12f * sum)
            break;
    }
    return sum;
}

float AudioResampler::KaiserWindow(int n, int N, float beta) {
    /* beta=6 trades transition width for about 60 dB of stopband rejection. */
    float r = 2.0f * (float)n / (float)N - 1.0f; /* normalise to [-1, 1] */
    float inside = 1.0f - r * r;
    if (inside < 0.0f)
        inside = 0.0f;
    return BesselI0(beta * sqrtf(inside)) / BesselI0(beta);
}

float AudioResampler::Sinc(float x) {
    if (fabsf(x) < 1e-8f)
        return 1.0f;
    float px = (float)M_PI * x;
    return sinf(px) / px;
}

void AudioResampler::BuildFilter() {
    /* Cutoff sits at the lower of the two Nyquist limits, expressed against the
     * interpolated rate P*inRate, so nothing aliases in either direction. */
    const int totalTaps = mNumPhases * kTapsPerPhase;
    const float fc = 0.5f / (float)std::max(mP, mQ);
    const float beta = 6.0f;
    const int N = totalTaps - 1;

    std::vector<float> h(totalTaps);

    for (int i = 0; i < totalTaps; i++) {
        float x = (float)i - (float)N * 0.5f;
        h[i] = 2.0f * fc * Sinc(2.0f * fc * x) * KaiserWindow(i, N, beta);
    }

    /* Taps go in reversed because Process() walks the window forwards rather than
     * backwards as a convolution would. Without the flip each phase carries the
     * fractional delay of its opposite and they partially cancel: 2.6 dB down at
     * 8 kHz, 14 dB at 12 kHz, measured on a 32k -> 48k sweep. */
    mCoeffs.resize(mNumPhases * kTapsPerPhase);
    for (int phase = 0; phase < mNumPhases; phase++) {
        for (int tap = 0; tap < kTapsPerPhase; tap++) {
            mCoeffs[phase * kTapsPerPhase + tap] = h[phase + (kTapsPerPhase - 1 - tap) * mNumPhases] * (float)mP;
        }
    }
}

void AudioResampler::Reset() {
    std::fill(mHistory.begin(), mHistory.end(), 0.0f);
    mPhase = 0;
}

int32_t AudioResampler::MaxOutputFrames(int32_t inFrames) const {
    /* ceil((inFrames * P) / Q) */
    return (int32_t)(((int64_t)inFrames * mP + mQ - 1) / mQ);
}

// Polyphase rational resample: conceptually upsample by P (zero-stuff), lowpass,
// downsample by Q -- done without the zeros by walking phases and advancing the
// input only after Q phases. Per output: filter bank[mPhase], then mPhase += Q.

int32_t AudioResampler::Process(const float* inBuf, int32_t inFrames, float* outBuf, int32_t maxOutFrames) {
    const int histLen = kTapsPerPhase - 1;
    const int ch = mNumChannels;

    const int windowFrames = histLen + inFrames;
    if ((int)mWindow.size() < windowFrames * ch) {
        mWindow.resize(windowFrames * ch);
    }
    std::vector<float>& window = mWindow;

    for (int i = 0; i < histLen * ch; i++) {
        window[i] = mHistory[i];
    }

    for (int i = 0; i < inFrames * ch; i++) {
        window[histLen * ch + i] = inBuf[i];
    }

    int32_t outFrames = 0;
    int32_t inPos = 0;
    int32_t phase = mPhase;

    while (inPos + kTapsPerPhase <= windowFrames && outFrames < maxOutFrames) {
        const float* coeffs = &mCoeffs[phase * kTapsPerPhase];

        for (int c = 0; c < ch; c++) {
            float acc = 0.0f;
            for (int tap = 0; tap < kTapsPerPhase; tap++) {
                acc += window[(inPos + tap) * ch + c] * coeffs[tap];
            }
            /* Unclamped: where the filter's ringing gets limited is the caller's. */
            outBuf[outFrames * ch + c] = acc;
        }
        outFrames++;

        phase += mQ;
        if (phase >= mP) {
            phase -= mP;
            inPos++;
        }
    }

    /* Holds only while maxOutFrames was at least MaxOutputFrames(inFrames);
     * a tighter cap would strand input frames here instead of carrying them. */
    assert(inPos == windowFrames - histLen);
    for (int i = 0; i < histLen * ch; i++) {
        mHistory[i] = window[(windowFrames - histLen) * ch + i];
    }

    mPhase = phase;
    return outFrames;
}

int32_t AudioResampler::Process(const int16_t* inBuf, int32_t inFrames, int16_t* outBuf, int32_t maxOutFrames) {
    const int ch = mNumChannels;
    const int totalIn = inFrames * ch;
    const int totalOut = maxOutFrames * ch;

    if (static_cast<int>(mInScratch.size()) < totalIn) {
        mInScratch.resize(totalIn);
    }
    if (static_cast<int>(mOutScratch.size()) < totalOut) {
        mOutScratch.resize(totalOut);
    }

    constexpr float kS16ToFloat = 1.0f / 32768.0f;
    for (int i = 0; i < totalIn; i++) {
        mInScratch[i] = static_cast<float>(inBuf[i]) * kS16ToFloat;
    }

    const int32_t outFrames = Process(mInScratch.data(), inFrames, mOutScratch.data(), maxOutFrames);

    /* Clamp because the filter's ringing can push a near-full-scale sample past it. */
    const int outSamples = outFrames * ch;
    for (int i = 0; i < outSamples; i++) {
        float v = mOutScratch[i] * 32767.0f;
        if (v > 32767.0f)
            v = 32767.0f;
        if (v < -32768.0f)
            v = -32768.0f;
        outBuf[i] = static_cast<int16_t>(lrintf(v));
    }
    return outFrames;
}

} // namespace SOH
