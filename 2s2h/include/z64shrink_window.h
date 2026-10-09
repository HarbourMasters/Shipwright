#ifndef Z64SHRINK_WINDOW_H
#define Z64SHRINK_WINDOW_H

#include "PR/ultratypes.h"

struct GraphicsContext;

void ShrinkWindow_Letterbox_SetSizeTarget(s32 target);
s32 ShrinkWindow_Letterbox_GetSizeTarget(void);
void ShrinkWindow_Letterbox_SetSize(s32 size);
s32 ShrinkWindow_Letterbox_GetSize(void);

void ShrinkWindow_Pillarbox_SetSizeTarget(s32 target);
s32 ShrinkWindow_Pillarbox_GetSizeTarget(void);
void ShrinkWindow_Pillarbox_SetSize(s32 size);
s32 ShrinkWindow_Pillarbox_GetSize(void);

void ShrinkWindow_Init(void);
void ShrinkWindow_Destroy(void);
void ShrinkWindow_Update(s32 framerateDivisor);
void ShrinkWindow_Draw(struct GraphicsContext* gfxCtx);

#endif
