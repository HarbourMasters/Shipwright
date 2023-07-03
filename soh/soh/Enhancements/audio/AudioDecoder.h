#pragma once
//A standalone, incremental audio sample decoder.
//Based on the ADPCM decoding routines in mixer.c.

#include "libultraship/libultraship.h"
#include "soh/resource/type/AudioSample.h"
class AudioDecoder
{
    LUS::AudioSample* sample;
    uint8_t *in, * inStart, * inEnd;
    int16_t adpcm_table[8][2][8];
    int16_t prev1, prev2;//The internal decoder takes an array of 16 shorts which it calls ADPCM_STATE. Strictly speaking only the last two decoded samples are needed for continuation.
  public:
    AudioDecoder();
    ~AudioDecoder();
    void setSample(LUS::AudioSample* sample);
    size_t decode(int16_t* out, size_t nSamples);
    size_t decodeToWav(LUS::AudioSample* sample, int16_t** buffer);
};