#pragma once
#include "libultraship/libultraship.h"
#include "soh/resource/type/AudioSample.h"
class AudioDecoder
{
    LUS::AudioSample* sample;
public:
    AudioDecoder();
    ~AudioDecoder();
    void setSample(LUS::AudioSample* sample);
    size_t decode(int16_t* outBuffer, size_t nbytes);
    size_t decodeToWav(LUS::AudioSample* sample, int16_t** buffer);
};