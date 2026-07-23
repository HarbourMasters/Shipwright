#ifndef SKIN_MATRIX_H
#define SKIN_MATRIX_H

#include <libultraship/libultra.h>
#include "z64math.h"

#include "extern_c_helper.h"

struct GraphicsContext;

EXTERN_C void SkinMatrix_Vec3fMtxFMultXYZW(MtxF* mf, Vec3f* src, Vec3f* xyzDest, f32* wDest);
EXTERN_C void SkinMatrix_Vec3fMtxFMultXYZ(MtxF* mf, Vec3f* src, Vec3f* dest);
EXTERN_C void SkinMatrix_MtxFMtxFMult(MtxF* mfA, MtxF* mfB, MtxF* dest);
EXTERN_C void SkinMatrix_GetClear(MtxF** mf);
EXTERN_C void SkinMatrix_Clear(MtxF* mf);
EXTERN_C void SkinMatrix_MtxFCopy(MtxF* src, MtxF* dest);
EXTERN_C s32 SkinMatrix_Invert(MtxF* src, MtxF* dest);
EXTERN_C void SkinMatrix_SetScale(MtxF* mf, f32 x, f32 y, f32 z);
EXTERN_C void SkinMatrix_SetRotateZYX(MtxF* mf, s16 x, s16 y, s16 z);
EXTERN_C void SkinMatrix_SetTranslate(MtxF* mf, f32 x, f32 y, f32 z);
EXTERN_C void SkinMatrix_SetTranslateRotateYXZScale(MtxF* dest, f32 scaleX, f32 scaleY, f32 scaleZ, s16 rotX, s16 rotY, s16 rotZ,
                                           f32 translateX, f32 translateY, f32 translateZ);
EXTERN_C void SkinMatrix_SetTranslateRotateZYX(MtxF* dest, s16 rotX, s16 rotY, s16 rotZ, f32 translateX, f32 translateY,
                                      f32 translateZ);
EXTERN_C Mtx* SkinMatrix_MtxFToNewMtx(struct GraphicsContext* gfxCtx, MtxF* src);
EXTERN_C void SkinMatrix_SetRotateAxis(MtxF* mf, s16 angle, f32 axisX, f32 axisY, f32 axisZ);

#endif