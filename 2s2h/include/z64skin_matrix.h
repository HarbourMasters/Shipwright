#ifndef Z64SKIN_MATRIX_H
#define Z64SKIN_MATRIX_H

#include "ultra64.h"
#include "z64math.h"

struct GraphicsContext;

void SkinMatrix_Vec3fMtxFMultXYZW(MtxF* mf, Vec3f* src, Vec3f* xyzDest, f32* wDest);
void SkinMatrix_Vec3fMtxFMultXYZ(MtxF* mf, Vec3f* src, Vec3f* dest);
void SkinMatrix_MtxFMtxFMult(MtxF* mfB, MtxF* mfA, MtxF* dest);
void SkinMatrix_GetClear(MtxF** mfp);
void SkinMatrix_MtxFCopy(MtxF* src, MtxF* dest);
s32 SkinMatrix_Invert(MtxF* src, MtxF* dest);
void SkinMatrix_SetScale(MtxF* mf, f32 x, f32 y, f32 z);
void SkinMatrix_SetRotateRPY(MtxF* mf, s16 roll, s16 pitch, s16 yaw);
void SkinMatrix_SetRotateYRP(MtxF* mf, s16 yaw, s16 roll, s16 pitch);
void SkinMatrix_SetTranslate(MtxF* mf, f32 x, f32 y, f32 z);
void SkinMatrix_SetScaleRotateRPYTranslate(MtxF* mf, f32 scaleX, f32 scaleY, f32 scaleZ, s16 roll, s16 pitch, s16 yaw, f32 dx, f32 dy, f32 dz);
void SkinMatrix_SetScaleRotateYRPTranslate(MtxF* mf, f32 scaleX, f32 scaleY, f32 scaleZ, s16 yaw, s16 roll, s16 pitch, f32 dx, f32 dy, f32 dz);
void SkinMatrix_SetRotateRPYTranslate(MtxF* mf, s16 roll, s16 pitch, s16 yaw, f32 dx, f32 dy, f32 dz);
void SkinMatrix_Vec3fToVec3s(Vec3f* src, Vec3s* dest);
void SkinMatrix_Vec3sToVec3f(Vec3s* src, Vec3f* dest);
void SkinMatrix_MtxFToMtx(MtxF* src, Mtx* dest);
Mtx* SkinMatrix_MtxFToNewMtx(struct GraphicsContext* gfxCtx, MtxF* src);
void SkinMatrix_SetRotateAroundVec(MtxF* mf, s16 a, f32 x, f32 y, f32 z);
void SkinMatrix_SetXRotation(MtxF* mf, s16 a);
void SkinMatrix_MulXRotation(MtxF* mf, s16 a);
void SkinMatrix_SetYRotation(MtxF* mf, s16 a);
void SkinMatrix_MulYRotation(MtxF* mf, s16 a);
void SkinMatrix_SetZRotation(MtxF* mf, s16 a);

#endif
