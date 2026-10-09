#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void SOH_StreamedMusic_RegisterSequence(uint16_t seqId, uint8_t isStreamed, const char* resourcePath);
void SOH_StreamedMusic_SequenceStarted(uint8_t playerIdx, uint16_t resolvedSeqId);

// Nonzero when the module handles the stop's fade: the caller must not queue its own stop.
uint8_t SOH_StreamedMusic_SequenceStopped(uint8_t playerIdx, uint16_t fadeTimer);

// Nonzero when the start is deferred until the outgoing track is quiet: the caller must do nothing.
// Otherwise a zero `*fadeTimer` may be set to the incoming track's fade-in.
uint8_t SOH_StreamedMusic_PrepareStart(uint8_t playerIdx, uint8_t seqId, uint8_t seqArgs, uint16_t* fadeTimer);

void SOH_StreamedMusic_Update();
void SOH_StreamedMusic_Reset();

// A streamed track is one note held for the whole song: once its channel is stopped, nothing
// issues the note again.
uint8_t SOH_StreamedMusic_ShouldSplitChannels(uint8_t playerIdx);

#ifdef __cplusplus
}
#endif
