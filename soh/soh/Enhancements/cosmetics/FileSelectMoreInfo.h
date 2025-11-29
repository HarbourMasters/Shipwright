#pragma once

#include "libultraship/libultra/types.h"

struct FileChooseContext;

#ifdef __cplusplus
extern "C" {
#endif
void DrawMoreInfo(struct FileChooseContext* thisx, s16 fileIndex, u8 alpha);
#ifdef __cplusplus
}
#endif
