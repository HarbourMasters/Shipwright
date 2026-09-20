#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void SOH_StreamedMusic_SetStreamed(uint16_t seqId, uint8_t isStreamed);
void SOH_StreamedMusic_SequenceStarted(uint8_t playerIdx, uint16_t seqId);
void SOH_StreamedMusic_SequenceStopped(uint8_t playerIdx);

// A streamed track is one note held for the whole song: once its channel is stopped, nothing
// issues the note again.
uint8_t SOH_StreamedMusic_ShouldSplitChannels(uint8_t playerIdx);

#ifdef __cplusplus
}
#endif
