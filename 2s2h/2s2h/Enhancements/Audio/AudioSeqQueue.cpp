#if 0
#include "AudioSeqQueue.h"

#include "resource/type/AudioSequence.h"
#include <ship/Context.h>

static SafeQueue<char*> audioQueue;

extern "C" {

void AudioQueue_Enqueue(char* seqId) {
    audioQueue.enqueue(seqId);
}

char* AudioQueue_Dequeue(void) {
    return audioQueue.dequeue();
}

int32_t AudioQueue_IsEmpty(void) {
    return audioQueue.isEmpty();
}

void AudioQueue_GetSeqInfo(const char* path, uint64_t* numFrames, uint32_t* numChannels, uint32_t* sampleRate,
                           int16_t** sampleData) {
    auto seqData =
        static_pointer_cast<SOH::AudioSequence>(Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(path));
    if (numFrames != nullptr) {
        *numFrames = seqData->sequence.seqDataSize / sizeof(uint16_t);
    }

    if (numChannels != nullptr) {
        *numChannels = seqData->numChannels;
    }

    if (numChannels != nullptr) {
        *sampleRate = seqData->sampleRate;
    }
    *sampleData = (s16*)seqData->sequence.seqData;
}
}
#endif