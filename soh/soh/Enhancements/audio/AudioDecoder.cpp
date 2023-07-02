#include "AudioDecoder.h"
AudioDecoder::AudioDecoder() {
    sample = NULL;

}
AudioDecoder::~AudioDecoder()
{

}
void AudioDecoder::setSample(LUS::AudioSample* sample)
{
    this->sample = sample;

}
size_t AudioDecoder::decode(int16_t* outBuffer, size_t nbytes)
{
    return 0;

}
size_t AudioDecoder::decodeToWav(LUS::AudioSample* sample, int16_t** buffer)
{
    return 0;

}
