#ifndef MEMPAK_H
#define MEMPAK_H

#include <libultraship/libultra.h>

#include "extern_c_helper.h"

EXTERN_C s32 Mempak_Init(s32 controllerNb);
EXTERN_C s32 Mempak_GetFreeBytes(s32 controllerNb);
EXTERN_C s32 Mempak_FindFile(s32 controllerNb, char start, char end);
EXTERN_C s32 Mempak_Write(s32 controllerNb, char idx, void* buffer, s32 offset, ptrdiff_t size);
EXTERN_C s32 Mempak_Read(s32 controllerNb, char idx, void* buffer, s32 offset, ptrdiff_t size);
// TODO: Rename to Mempak_CreateFile
EXTERN_C s32 Mempak_Alloc(s32 controllerNb, char* idx, ptrdiff_t size);
EXTERN_C s32 Mempak_DeleteFile(s32 controllerNb, char idx);
EXTERN_C s32 Mempak_GetFileSize(s32 controllerNb, char idx);

// TODO: MEMPAK macros

#endif