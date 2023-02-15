#pragma once
#include "stdint.h"

void InitSfxEditor();
void SfxEditor_RandomizeAll();
void SfxEditor_ResetAll();

#ifndef __cplusplus
const char* SfxEditor_GetSequenceName(u16 seqId);
void SfxEditor_AddSequence(char *otrPath, uint16_t seqNum);
#endif