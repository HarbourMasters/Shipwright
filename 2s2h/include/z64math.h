#ifndef Z64MATH_H
#define Z64MATH_H

#include "PR/ultratypes.h"
#include <math.h>
#define VEC_SET(V,X,Y,Z) V.x=X;V.y=Y;V.z=Z

typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 z;
} Vec2s; // size = 0x4

typedef struct {
    /* 0x0 */ f32 x;
    /* 0x4 */ f32 z;
} Vec2f; // size = 0x8

typedef struct {
    /* 0x0 */ f32 x;
    /* 0x4 */ f32 y;
    /* 0x8 */ f32 z;
} Vec3f; // size = 0xC

typedef struct {
    /* 0x0 */ u16 x;
    /* 0x2 */ u16 y;
    /* 0x4 */ u16 z;
} Vec3us; // size = 0x6

typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
} Vec3s; // size = 0x6

typedef struct {
    /* 0x0 */ s32 x;
    /* 0x4 */ s32 y;
    /* 0x8 */ s32 z;
} Vec3i; // size = 0xC

typedef struct {
    /* 0x0 */ s16 distance;
    /* 0x2 */ s16 angle;
} VecPolarS; // size = 0x4

typedef struct {
    /* 0x0 */ f32 distance;
    /* 0x4 */ s16 angle;
} VecPolar; // size = 0x8

typedef struct {
    /* 0x0 */ Vec3s center;
    /* 0x6 */ s16   radius;
} Sphere16; // size = 0x8

typedef struct {
    /* 0x0 */ Vec3f center;
    /* 0xC */ f32   radius;
} Spheref; // size = 0x10

/*
The plane paramaters are of form `ax + by + cz + d = 0`
where `(a,b,c)` is the plane's normal vector and d is the originDist
 */
typedef struct {
    /* 0x0 */ Vec3f normal;
    /* 0xC */ f32   originDist;
} Plane; // size = 0x10

typedef struct {
    /* 0x00 */ Vec3f vtx[3];
    /* 0x24 */ Plane plane;
} TriNorm; // size = 0x34

typedef struct {
    /* 0x0 */ s16   radius;
    /* 0x2 */ s16   height;
    /* 0x4 */ s16   yShift;
    /* 0x6 */ Vec3s pos;
} Cylinder16; // size = 0xC

typedef struct {
    /* 0x00 */ f32   radius;
    /* 0x04 */ f32   height;
    /* 0x08 */ f32   yShift;
    /* 0x0C */ Vec3f pos;
} Cylinderf; // size = 0x18

typedef struct {
    /* 0x00 */ Vec3f point;
    /* 0x0C */ Vec3f dir;
} InfiniteLine; // size = 0x18

typedef struct {
    /* 0x00 */ Vec3f a;
    /* 0x0C */ Vec3f b;
} Linef; // size = 0x18

typedef struct {
    /* 0x0 */ f32 r; // radius
    /* 0x4 */ s16 pitch; // depends on coordinate system. See below.
    /* 0x6 */ s16 yaw; // azimuthal angle
} VecSphGeo; // size = 0x8

// Defines a point in the spherical coordinate system.
// Pitch is 0 along the positive y-axis (up)
typedef VecSphGeo VecSph;

// Defines a point in the geographic coordinate system.
// Pitch is 0 along the xz-plane (horizon)
typedef VecSphGeo VecGeo;

// To be used with OLib_Vec3fAdd()
typedef enum {
    /* 0 */ OLIB_ADD_COPY, // Copy `b` to dest
    /* 1 */ OLIB_ADD_OFFSET, // Add `a` and `b` to dest, and also add the yaw of `a` to the dest
    /* 2 */ OLIB_ADD // Add `a` and `b` to dest
} OlibVec3fAdd;

typedef enum {
    /* 0 */ OLIB_DIFF_COPY, // Copy `b` to dest
    /* 1 */ OLIB_DIFF_OFFSET, // Sub `a` and `b` to dest, and also subs the yaw of `a` to the dest
    /* 2 */ OLIB_DIFF // Sub `a` and `b` to dest
} OlibVec3fDiff;

#if 0
typedef float MtxF_t[4][4];
typedef union {
    MtxF_t mf;
    struct {
        float xx, yx, zx, wx,
              xy, yy, zy, wy,
              xz, yz, zz, wz,
              xw, yw, zw, ww;
    };
} MtxF; // size = 0x40
#endif

#ifdef __cplusplus
extern "C"
{
#endif

extern Vec3f gZeroVec3f;
extern Vec3s gZeroVec3s;

#ifdef __cplusplus
}
#endif

#define LERPIMP(v0, v1, t) ((v0) + (((v1) - (v0)) * (t)))
#define LERPIMP_ALT(v0, v1, t) (((v1) - (v0)) * (t) + (v0))
#define S16_LERP(v0, v1, t) ((s16)(((v1) - (v0)) * (t)) + (v0))
#define F32_LERP(v0, v1, t) ((1.0f - (t)) * (f32)(v0) + (t) * (f32)(v1))
#define F32_LERP_ALT(v0, v1, t) ((f32)(v0) * (1.0f - (t)) + (t) * (f32)(v1))
#define F32_LERPIMP(v0, v1, t) ((f32)(v0) + (((f32)(v1) - (f32)(v0)) * (t)))
#define F32_LERPIMPINV(v0, v1, t) ((f32)(v0) + (((f32)(v1) - (f32)(v0)) / (t)))
#define BINANG_LERPIMP(v0, v1, t) ((v0) + (s16)(BINANG_SUB((v1), (v0)) * (t)))
#define BINANG_LERPIMPINV(v0, v1, t) ((v0) + BINANG_SUB((v1), (v0)) / (t))

#define LERPWEIGHT(val, prev, next) (((val) - (prev)) / ((next) - (prev)))
#define F32_LERPWEIGHT(val, prev, next) (((f32)(val) - (f32)(prev)) / ((f32)(next) - (f32)(prev)))

#define VEC3F_LERPIMPDST(dst, v0, v1, t)                \
    {                                                   \
        (dst)->x = (v0)->x + (((v1)->x - (v0)->x) * t); \
        (dst)->y = (v0)->y + (((v1)->y - (v0)->y) * t); \
        (dst)->z = (v0)->z + (((v1)->z - (v0)->z) * t); \
    }                                                   \
    (void)0

#define IS_ZERO(f) (fabsf(f) < 0.008f)

#define SQ(x) ((x) * (x))
#define CB(x) ((x) * (x) * (x))
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define ABS_ALT(x) ((x) < 0 ? -(x) : (x))

// Casting a float to an integer, when the float value is larger than what the integer type can hold,
// leads to undefined behavior. For example (f32)0x8000 doesn't fit in a s16, so it cannot be cast to s16.
// This isn't an issue with IDO, but is one with for example GCC.
// A partial workaround is to cast to s32 then s16, hoping all binang values used will fit a s32.
#define TRUNCF_BINANG(f) (s16)(s32)(f)

// Angle conversion macros
#define DEG_TO_RAD(degrees) ((degrees) * (M_PIf / 180.0f))
#define DEG_TO_BINANG(degrees) TRUNCF_BINANG((degrees) * (0x8000 / 180.0f))
#define DEG_TO_BINANG_ALT(degrees) TRUNCF_BINANG(((degrees) / 180.0f) * 0x8000)
#define DEG_TO_BINANG_ALT2(degrees) TRUNCF_BINANG(((degrees) * 0x10000) / 360.0f)
#define DEG_TO_BINANG_ALT3(degrees) ((degrees) * (0x8000 / 180.0f))

#define RAD_TO_DEG(radians) ((radians) * (180.0f / M_PIf))
#define RAD_TO_BINANG(radians) TRUNCF_BINANG((radians) * (0x8000 / M_PIf))
#define RAD_TO_BINANG_ALT(radians) TRUNCF_BINANG(((radians) / M_PIf) * 0x8000)
#define RAD_TO_BINANG_ALT2(radians) TRUNCF_BINANG(((radians) * 0x8000) / M_PIf)

#define BINANG_TO_DEG(binang) ((f32)(binang) * (180.0f / 0x8000))
#define BINANG_TO_RAD(binang) ((f32)(binang) * (M_PIf / 0x8000))
#define BINANG_TO_RAD_ALT(binang) (((f32)(binang) / 0x8000) * M_PIf)
#define BINANG_TO_RAD_ALT2(binang) (((f32)(binang) * M_PIf) / 0x8000)

// Angle arithmetic macros
#define BINANG_ROT180(angle) ((s16)(angle + 0x8000))
#define BINANG_SUB(a, b) ((s16)(a - b))
#define BINANG_ADD(a, b) ((s16)(a + b))

// Vector macros
#define SQXZ(vec) ((vec.x) * (vec.x) + (vec.z) * (vec.z))
#define DOTXZ(vec1, vec2) ((vec1.x) * (vec2.x) + (vec1.z) * (vec2.z))
#define SQXYZ(vec) ((vec.x) * (vec.x) + (vec.y) * (vec.y) + (vec.z) * (vec.z))
#define DOTXYZ(vec1, vec2) ((vec1.x) * (vec2.x) + (vec1.y) * (vec2.y) + (vec1.z) * (vec2.z))

f32 Math_CosS(s16 angle);
f32 Math_SinS(s16 angle);
s32 Math_StepToIImpl(s32 start, s32 target, s32 step);
void Math_StepToIGet(s32* pValue, s32 target, s32 step);
s32 Math_StepToI(s32* pValue, s32 target, s32 step);
s32 Math_ScaledStepToS(s16* pValue, s16 target, s16 step);
s32 Math_StepToS(s16* pValue, s16 target, s16 step);
s32 Math_StepToC(s8* pValue, s8 target, s8 step);
s32 Math_StepToF(f32* pValue, f32 target, f32 step);
s32 Math_StepUntilAngleS(s16* pValue, s16 target, s16 step);
s32 Math_StepToAngleS(s16* pValue, s16 target, s16 step);
s32 Math_AsymStepToS(s16* pValue, s16 target, s16 incrStep, s16 decrStep);
s32 Math_StepUntilF(f32* pValue, f32 limit, f32 step);
s32 Math_AsymStepToF(f32* pValue, f32 target, f32 incrStep, f32 decrStep);
s16 Rand_S16Offset(s16 base, s16 range);
s16 Rand_S16OffsetStride(s16 base, s16 stride, s16 range);
void Math_Vec3f_Copy(Vec3f* dest, Vec3f* src);
void Math_Vec3s_Copy(Vec3s* dest, Vec3s* src);
void Math_Vec3s_ToVec3f(Vec3f* dest, Vec3s* src);
void Math_Vec3f_ToVec3s(Vec3s* dest, Vec3f* src);
void Math_Vec3f_Sum(Vec3f* l, Vec3f* r, Vec3f* dest);
void Math_Vec3f_Diff(Vec3f* l, Vec3f* r, Vec3f* dest);
void Math_Vec3s_DiffToVec3f(Vec3f* dest, Vec3s* l, Vec3s* r);
void Math_Vec3f_Scale(Vec3f* vec, f32 scale);
void Math_Vec3f_ScaleAndStore(Vec3f* vec, f32 scale, Vec3f* dest);
void Math_Vec3f_Lerp(Vec3f* a, Vec3f* b, f32 t, Vec3f* dest);
void Math_Vec3f_SumScaled(Vec3f* a, Vec3f* b, f32 scale, Vec3f* dest);
void Math_Vec3f_AddRand(Vec3f* orig, f32 scale, Vec3f* dest);
void Math_Vec3f_DistXYZAndStoreNormDiff(Vec3f* a, Vec3f* b, f32 scale, Vec3f* dest);
f32 Math_Vec3f_DistXYZ(Vec3f* a, Vec3f* b);
f32 Math_Vec3f_DistXYZAndStoreDiff(Vec3f* a, Vec3f* b, Vec3f* dest);
f32 Math_Vec3f_DistXZ(Vec3f* a, Vec3f* b);
f32 Math_Vec3f_DistXZAndStore(Vec3f* a, Vec3f* b, f32* dx, f32* dz);
f32 Math_Vec3f_StepToXZ(Vec3f* start, Vec3f* target, f32 speed);
f32 Math_Vec3f_DiffY(Vec3f* a, Vec3f* b);
s16 Math_Vec3f_Yaw(Vec3f* a, Vec3f* b);
s16 Math_Vec3f_Pitch(Vec3f* a, Vec3f* b);
f32 Math_SmoothStepToF(f32* pValue, f32 target, f32 fraction, f32 step, f32 minStep);
void Math_ApproachF(f32* pValue, f32 target, f32 scale, f32 maxStep);
void Math_ApproachZeroF(f32* pValue, f32 scale, f32 maxStep);
s16 Math_SmoothStepToS(s16* pValue, s16 target, s16 scale, s16 step, s16 minStep);
void Math_ApproachS(s16* pValue, s16 target, s16 scale, s16 maxStep);
f32 Math_Vec3f_StepTo(Vec3f* start, Vec3f* target, f32 speed);

f32 Math_FactorialF(f32 n);
f32 Math_Factorial(s32 n);
f32 Math_PowF(f32 base, s32 exp);
f32 Math_SinF(f32 rad);
f32 Math_CosF(f32 rad);
f32 Rand_ZeroFloat(f32 scale);
f32 Rand_CenteredFloat(f32 scale);

f32 Math_FTanF(f32 x);
f32 Math_FFloorF(f32 x);
f32 Math_FCeilF(f32 x);
f32 Math_FRoundF(f32 x);
f32 Math_FTruncF(f32 x);
f32 Math_FNearbyIntF(f32 x);
f32 Math_FAtanTaylorQF(f32 x);
f32 Math_FAtanTaylorF(f32 x);
f32 Math_FAtanContFracF(f32 x);
f32 Math_FAtanF(f32 x);
f32 Math_FAtan2F(f32 y, f32 x);
f32 Math_FAsinF(f32 x);
f32 Math_FAcosF(f32 x);

s16 Math_Atan2S(f32 y, f32 x);
f32 Math_Atan2F(f32 y, f32 x);
s16 Math_Atan2S_XY(f32 x, f32 y);
f32 Math_Atan2F_XY(f32 x, f32 y);

f32 Math3D_Normalize(Vec3f* vec);
s32 Math3D_PlaneVsLineSegClosestPoint(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB, f32 planeBC, f32 planeBDist, Vec3f* linePointA, Vec3f* linePointB, Vec3f* closestPoint);
s32 Math3D_LineSegMakePerpLineSeg(Vec3f* lineAPointA, Vec3f* lineAPointB, Vec3f* lineBPointA, Vec3f* lineBPointB, Vec3f* lineAClosestToB, Vec3f* lineBClosestToA);
f32 Math3D_LineClosestToPoint(InfiniteLine* line, Vec3f* pos, Vec3f* closestPoint);
void Math3D_FindPointOnPlaneIntersect(float planeAAxis1Norm, f32 planeAAxis2Norm, f32 planeBAxis1Norm, f32 planeBAxis2Norm, f32 axis3Direction, f32 planeADist, f32 planeBDist, float* axis1Point, float* axis2Point);
s32 Math3D_PlaneVsPlaneNewLine(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB, f32 planeBC, f32 planeBDist, InfiniteLine* intersect);
s32 Math3D_PlaneVsPlaneVsLineClosestPoint(f32 planeAA, f32 planeAB, f32 planeAC, f32 planeADist, f32 planeBA, f32 planeBB, f32 planeBC, f32 planeBDist, Vec3f* point, Vec3f* closestPoint);
void Math3D_PointOnDirectedLine(Vec3f* v0, Vec3f* dir, f32 scale, Vec3f* ret);
void Math3D_LineSplitRatio(Vec3f* v0, Vec3f* v1, f32 ratio, Vec3f* ret);
f32 Math3D_Cos(Vec3f* a, Vec3f* b);
s32 Math3D_CosOut(Vec3f* a, Vec3f* b, f32* dst);
void Math3D_Vec3fReflect(Vec3f* vec, Vec3f* normal, Vec3f* reflVec);
s32 Math3D_PointInSquare2D(f32 upperLeftX, f32 lowerRightX, f32 upperLeftY, f32 lowerRightY, f32 x, f32 y);
s32 Math3D_CirSquareVsTriSquareXY(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 centerX, f32 centerY, f32 radius);
s32 Math3D_CirSquareVsTriSquareYZ(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 centerY, f32 centerZ, f32 radius);
s32 Math3D_CirSquareVsTriSquareZX(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 centerZ, f32 centerX, f32 radius);
s32 Math3D_SphCubeVsTriCube(Vec3f* v0, Vec3f* v1, Vec3f* v2, Vec3f* center, f32 radius);
f32 Math3D_Dist1DSq(f32 a, f32 b);
f32 Math3D_Dist1D(f32 a, f32 b);
f32 Math3D_Dist2DSq(f32 x0, f32 y0, f32 x1, f32 y1);
f32 Math3D_Dist2D(f32 x0, f32 y0, f32 x1, f32 y1);
f32 Math3D_Vec3fMagnitudeSq(Vec3f* vec);
f32 Math3D_Vec3fMagnitude(Vec3f* vec);
f32 Math3D_Vec3fDistSq(Vec3f* a, Vec3f* b);
f32 Math3D_Vec3f_DistXYZ(Vec3f* a, Vec3f* b);
f32 Math3D_DistXYZ16toF(Vec3s* a, Vec3f* b);
f32 Math3D_Vec3fDiff_CrossZ(Vec3f* a, Vec3f* b, f32 dx, f32 dy);
f32 Math3D_Vec3fDiff_CrossX(Vec3f* a, Vec3f* b, f32 dy, f32 dz);
f32 Math3D_Vec3fDiff_CrossY(Vec3f* a, Vec3f* b, f32 dz, f32 dx);
void Math3D_Vec3f_Cross(Vec3f* a, Vec3f* b, Vec3f* ret);
void Math3D_SurfaceNorm(Vec3f* va, Vec3f* vb, Vec3f* vc, Vec3f* normal);
s32 Math3D_PointRelativeToCubeFaces(Vec3f* point, Vec3f* min, Vec3f* max);
s32 Math3D_PointRelativeToCubeEdges(Vec3f* point, Vec3f* min, Vec3f* max);
s32 Math3D_PointRelativeToCubeVertices(Vec3f* point, Vec3f* min, Vec3f* max);
s32 Math3D_LineVsCube(Vec3f* min, Vec3f* max, Vec3f* a, Vec3f* b);
s32 Math3D_LineVsCubeShort(Vec3s* min, Vec3s* max, Vec3s* a, Vec3s* b);
void Math3D_RotateXZPlane(Vec3f* pointOnPlane, s16 angle, f32* a, f32* c, f32* d);
void Math3D_DefPlane(Vec3f* va, Vec3f* vb, Vec3f* vc, f32* nx, f32* ny, f32* nz, f32* originDist);
f32 Math3D_PlaneF(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* pointOnPlane);
f32 Math3D_Plane(Plane* plane, Vec3f* pointOnPlane);
f32 Math3D_UDistPlaneToPos(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* p);
f32 Math3D_DistPlaneToPos(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* p);
s32 Math3D_TriChkPointParaYImpl(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x, f32 detMax, f32 chkDist, f32 ny);
s32 Math3D_TriChkPointParaYDeterminate(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x, f32 detMax, f32 ny);
s32 Math3D_TriChkPointParaYSlopedY(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x);
s32 Math3D_TriChkPointParaYIntersectDist(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z, f32 x, f32* yIntersect, f32 chkDist);
s32 Math3D_TriChkPointParaYIntersectInsideTri(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z, f32 x, f32* yIntersect, f32 chkDist);
s32 Math3D_TriChkPointParaY(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 ny, f32 z, f32 x);
s32 Math3D_TriChkLineSegParaYIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z, f32 x, f32* yIntersect, f32 y0, f32 y1);
s32 Math3D_TriChkPointParaYDist(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 z, f32 x, f32 chkDist);
s32 Math3D_TriChkPointParaYImplNoCheckRange(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 z, f32 x, f32 detMax, f32 chkDist, f32 ny);
s32 Math3D_TriChkPointParaYNoRangeCheckIntersectInsideTri(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 z, f32 x, f32* yIntersect, f32 chkDist);
s32 Math3D_TriChkPointParaXImpl(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 y, f32 z, f32 detMax, f32 chkDist, f32 ny);
s32 Math3D_TriChkPointParaXDeterminate(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 y, f32 z, f32 detMax, f32 nx);
s32 Math3D_TriChkPointParaXIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 y, f32 z, f32* xIntersect);
s32 Math3D_TriChkPointParaX(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 y, f32 z);
s32 Math3D_TriChkLineSegParaXIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 y, f32 z, f32* xIntersect, f32 x0, f32 x1);
s32 Math3D_TriChkLineSegParaXDist(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 y, f32 z, f32 chkDist);
s32 Math3D_TriChkPointParaZImpl(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 x, f32 y, f32 detMax, f32 chkDist, f32 nz);
s32 Math3D_TriChkPointParaZDeterminate(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 y, f32 z, f32 detMax, f32 nx);
s32 Math3D_TriChkPointParaZIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 x, f32 y, f32* zIntersect);
s32 Math3D_TriChkPointParaZ(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 y, f32 z);
s32 Math3D_TriChkLineSegParaZIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, f32 x, f32 y, f32* zIntersect, f32 z0, f32 z1);
s32 Math3D_TriChkLineSegParaZDist(Vec3f* v0, Vec3f* v1, Vec3f* v2, Plane* plane, f32 x, f32 y, f32 chkDist);
s32 Math3D_LineSegFindPlaneIntersect(f32 pointADist, f32 pointBDist, Vec3f* pointA, Vec3f* pointB, Vec3f* intersect);
s32 Math3D_LineSegVsPlane(f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* linePointA, Vec3f* linePointB, Vec3f* intersect, s32 fromFront);
s32 Math3D_TriLineIntersect(Vec3f* v0, Vec3f* v1, Vec3f* v2, f32 nx, f32 ny, f32 nz, f32 originDist, Vec3f* linePointA, Vec3f* linePointB, Vec3f* intersect, s32 fromFront) ;
void Math3D_TriNorm(TriNorm* tri, Vec3f* va, Vec3f* vb, Vec3f* vc);
s32 Math3D_PointInSph(Sphere16* sphere, Vec3f* point);
s32 Math3D_PointDistSqToLine2DImpl(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32* perpXOut, f32* perpYOut, f32* lineLenSq); // returns boolean
s32 Math3D_PointDistSqToLine2D(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2, f32* lineLenSq);
s32 Math3D_PointDistSqToLineXY(f32 x0, f32 y0, Vec3f* p1, Vec3f* p2, f32* lineLenSq);
s32 Math3D_PointDistSqToLineYZ(f32 y0, f32 z0, Vec3f* p1, Vec3f* p2, f32* lineLenSq);
s32 Math3D_PointDistSqToLineZX(f32 z0, f32 x0, Vec3f* p1, Vec3f* p2, f32* lineLenSq);
s32 Math3D_LineVsSph(Sphere16* sphere, Linef* line);
void Math3D_GetSphVsTriIntersectPoint(Sphere16* sphere, TriNorm* tri, Vec3f* intersectPoint);
s32 Math3D_TriVsSphIntersect(Sphere16* sphere, TriNorm* tri, Vec3f* intersectPoint);
s32 Math3D_PointInCyl(Cylinder16* cyl, Vec3f* point);
s32 Math3D_CylVsLineSeg(Cylinder16* cyl, Vec3f* linePointA, Vec3f* linePointB, Vec3f* intersectA, Vec3f* intersectB);
s32 Math3D_CylTriVsIntersect(Cylinder16* cyl, TriNorm* tri, Vec3f* intersect);
s32 Math3D_CylVsTri(Cylinder16* cyl, TriNorm* tri);
s32 Math3D_SphVsSph(Sphere16* sphereA, Sphere16* sphereB);
s32 Math3D_SphVsSphOverlap(Sphere16* sphereA, Sphere16* sphereB, f32* overlapSize);
s32 Math3D_SphVsSphOverlapCenterDist(Sphere16* sphereA, Sphere16* sphereB, f32* overlapSize, f32* centerDist);
s32 Math3D_SphVsCylOverlap(Sphere16* sph, Cylinder16* cyl, f32* overlapSize);
s32 Math3D_SphVsCylOverlapCenterDist(Sphere16* sph, Cylinder16* cyl, f32* overlapSize, f32* centerDist);
s32 Math3D_CylVsCylOverlap(Cylinder16* ca, Cylinder16* cb, f32* overlapSize);
s32 Math3D_CylVsCylOverlapCenterDist(Cylinder16* ca, Cylinder16* cb, f32* overlapSize, f32* centerDist);
s32 Math3D_TriVsTriIntersect(TriNorm* ta, TriNorm* tb, Vec3f* intersect);
s32 Math3D_XZInSphere(Sphere16* sphere, f32 x, f32 z);
s32 Math3D_XYInSphere(Sphere16* sphere, f32 x, f32 y);
s32 Math3D_YZInSphere(Sphere16* sphere, f32 y, f32 z);
s32 Math3D_CircleLineIntersections(f32 centreX, f32 centerY, f32 radius, f32 pointX, f32 pointY, f32 dirX, f32 dirY, f32* intersectAX, f32* intersectAY, f32* intersectBX, f32* intersectBY);
void func_8017FD44(Vec3f* arg0, Vec3f* arg1, Vec3f* dst, f32 arg3);

#endif
