#ifndef OLIB_H
#define OLIB_H

#include "z64math.h"

#include "extern_c_helper.h"

EXTERN_C f32 OLib_Vec3fDist(Vec3f* a, Vec3f* b);
EXTERN_C f32 OLib_Vec3fDistXZ(Vec3f* a, Vec3f* b);
EXTERN_C f32 OLib_ClampMinDist(f32 val, f32 min);
EXTERN_C f32 OLib_ClampMaxDist(f32 val, f32 max);
EXTERN_C Vec3f* OLib_Vec3fDistNormalize(Vec3f* dest, Vec3f* a, Vec3f* b);
EXTERN_C Vec3f* OLib_VecSphGeoToVec3f(Vec3f* dest, VecSph* sph);
EXTERN_C VecSph* OLib_Vec3fToVecSph(VecSph* dest, Vec3f* vec);
EXTERN_C VecSph* OLib_Vec3fToVecSphGeo(VecSph* arg0, Vec3f* arg1);
EXTERN_C VecSph* OLib_Vec3fDiffToVecSphGeo(VecSph* arg0, Vec3f* a, Vec3f* b);
EXTERN_C Vec3f* OLib_Vec3fDiffRad(Vec3f* dest, Vec3f* a, Vec3f* b);

#endif