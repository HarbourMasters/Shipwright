#ifndef Z64LIB_H
#define Z64LIB_H

#include "ultra64.h"

#include "color.h"
#include "z64math.h"

struct Input;

void Lib_GetControlStickData(f32* outMagnitude, s16* outAngle, struct Input* input);
void Color_RGBA8_Copy(Color_RGBA8* dst, Color_RGBA8* src);
void Lib_PlaySfx(u16 sfxId);
void Lib_PlaySfx_2(u16 sfxId);
void Lib_PlaySfx_AtPos(Vec3f* pos, u16 sfxId);
void Lib_Vec3f_TranslateAndRotateY(Vec3f* translation, s16 rotAngle, Vec3f* src, Vec3f* dst);
void Color_RGB8_Lerp(Color_RGB8* from, Color_RGB8* to, f32 lerp, Color_RGB8* dst);
void Lib_Nop801004FC(void);
void* Lib_SegmentedToVirtual(void* ptr);
void* Lib_SegmentedToVirtualNull(void* ptr);
uintptr_t Lib_VirtualToPhysical(void* ptr);
void* Lib_PhysicalToVirtual(uintptr_t ptr);

#endif
