#pragma once
#include "stdint.h"

void InitAudioEditor();
void AudioEditor_RandomizeAll();
void AudioEditor_ResetAll();

#ifndef __cplusplus
const char* AudioEditor_GetSequenceName(u16 seqId);
void AudioEditor_AddSequence(char *otrPath, uint16_t seqNum);
#endif