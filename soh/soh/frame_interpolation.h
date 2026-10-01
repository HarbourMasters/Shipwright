#pragma once

#include "include/z64math.h"

#ifdef __cplusplus

#include <unordered_map>

std::unordered_map<Mtx*, MtxF> FrameInterpolation_Interpolate(float step);

extern "C" {

#endif

void FrameInterpolation_StartRecord(void);

void FrameInterpolation_StopRecord(void);

void FrameInterpolation_RecordOpenChild(const void* a, int b);

void FrameInterpolation_RecordCloseChild(void);

void FrameInterpolation_DontInterpolateCamera(void);

int FrameInterpolation_GetCameraEpoch(void);

void FrameInterpolation_RecordActorPosRotMatrix(void);

void FrameInterpolation_RecordMatrixPush(void);

void FrameInterpolation_RecordMatrixPop(void);

void FrameInterpolation_RecordMatrixPut(MtxF* src);

void FrameInterpolation_RecordMatrixMult(MtxF* mf, u8 mode);

void FrameInterpolation_RecordMatrixTranslate(f32 x, f32 y, f32 z, u8 mode);

void FrameInterpolation_RecordMatrixScale(f32 x, f32 y, f32 z, u8 mode);

void FrameInterpolation_RecordMatrixRotate1Coord(u32 coord, f32 value, u8 mode);

void FrameInterpolation_RecordMatrixRotateZYX(s16 x, s16 y, s16 z, u8 mode);

void FrameInterpolation_RecordMatrixTranslateRotateZYX(Vec3f* translation, Vec3s* rotation);

void FrameInterpolation_RecordMatrixSetTranslateRotateYXZ(f32 translateX, f32 translateY, f32 translateZ, Vec3s* rot);

void FrameInterpolation_RecordMatrixMtxFToMtx(MtxF* src, Mtx* dest);

void FrameInterpolation_RecordMatrixToMtx(Mtx* dest, char* file, s32 line);

void FrameInterpolation_RecordMatrixReplaceRotation(MtxF* mf);

void FrameInterpolation_RecordMatrixRotateAxis(f32 angle, Vec3f* axis, u8 mode);

void FrameInterpolation_RecordSkinMatrixMtxFToMtx(MtxF* src, Mtx* dest);

/**
 * Records a CPU written vertex block whose newest end is a moving sample: swept ribbon
 * trails (blure: sword slashes, enemy slashes, boomerang, ...). `dest` holds the vertices
 * the game just wrote for this logical frame. `pairs` lists `pairCount` (destination,
 * source) vertex index pairs, `source` being the vertex the destination has to collapse
 * onto at the start of the logical frame. Only the newest segment of a ribbon moves, and
 * only its newest end moves within that segment, so every other vertex of the block is
 * simply left out of `pairs` and stays where the game put it.
 *
 * Only record a block for a logical frame in which the game actually appended a sample:
 * a ribbon that did not grow is a fixed shape and must be left untouched, otherwise the
 * blend keeps collapsing its newest segment onto the previous sample on every frame.
 */
void FrameInterpolation_RecordRibbonHead(void* key, int index, void* dest, u32 vtxCount, u32 pairCount,
                                         const s16* pairs);

/**
 * Blends the recorded ribbon ends for the displayed frame of factor `step`.
 * Call once per displayed frame, right before running the commands.
 */
void FrameInterpolation_UpdateRibbonHeads(f32 step);

#ifdef __cplusplus
}
#endif
