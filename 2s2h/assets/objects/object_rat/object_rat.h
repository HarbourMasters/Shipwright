#ifndef OBJECTS_OBJECT_RAT_H
#define OBJECTS_OBJECT_RAT_H 1

#include "align_asset_macro.h"

#define dgRealBombchuSpotAnim "__OTR__objects/object_rat/gRealBombchuSpotAnim"
static const ALIGN_ASSET(2) char gRealBombchuSpotAnim[] = dgRealBombchuSpotAnim;

#define dgRealBombchuRunAnim "__OTR__objects/object_rat/gRealBombchuRunAnim"
static const ALIGN_ASSET(2) char gRealBombchuRunAnim[] = dgRealBombchuRunAnim;

#define dgRealBombchuBodyDL "__OTR__objects/object_rat/gRealBombchuBodyDL"
static const ALIGN_ASSET(2) char gRealBombchuBodyDL[] = dgRealBombchuBodyDL;

#define dgRealBombchuBackRightLegDL "__OTR__objects/object_rat/gRealBombchuBackRightLegDL"
static const ALIGN_ASSET(2) char gRealBombchuBackRightLegDL[] = dgRealBombchuBackRightLegDL;

#define dgRealBombchuFrontRightLegDL "__OTR__objects/object_rat/gRealBombchuFrontRightLegDL"
static const ALIGN_ASSET(2) char gRealBombchuFrontRightLegDL[] = dgRealBombchuFrontRightLegDL;

#define dgRealBombchuBackLeftLegDL "__OTR__objects/object_rat/gRealBombchuBackLeftLegDL"
static const ALIGN_ASSET(2) char gRealBombchuBackLeftLegDL[] = dgRealBombchuBackLeftLegDL;

#define dgRealBombchuFrontLeftLegDL "__OTR__objects/object_rat/gRealBombchuFrontLeftLegDL"
static const ALIGN_ASSET(2) char gRealBombchuFrontLeftLegDL[] = dgRealBombchuFrontLeftLegDL;

#define dgRealBombchuTailBaseDL "__OTR__objects/object_rat/gRealBombchuTailBaseDL"
static const ALIGN_ASSET(2) char gRealBombchuTailBaseDL[] = dgRealBombchuTailBaseDL;

#define dgRealBombchuTailMiddleDL "__OTR__objects/object_rat/gRealBombchuTailMiddleDL"
static const ALIGN_ASSET(2) char gRealBombchuTailMiddleDL[] = dgRealBombchuTailMiddleDL;

#define dgRealBombchuTailEndDL "__OTR__objects/object_rat/gRealBombchuTailEndDL"
static const ALIGN_ASSET(2) char gRealBombchuTailEndDL[] = dgRealBombchuTailEndDL;

#define dgRealBombchuHeadDL "__OTR__objects/object_rat/gRealBombchuHeadDL"
static const ALIGN_ASSET(2) char gRealBombchuHeadDL[] = dgRealBombchuHeadDL;

#define dgRealBombchuSkinTex "__OTR__objects/object_rat/gRealBombchuSkinTex"
static const ALIGN_ASSET(2) char gRealBombchuSkinTex[] = dgRealBombchuSkinTex;

#define dgRealBombchuMouthTex "__OTR__objects/object_rat/gRealBombchuMouthTex"
static const ALIGN_ASSET(2) char gRealBombchuMouthTex[] = dgRealBombchuMouthTex;

#define dgRealBombchuEyeTex "__OTR__objects/object_rat/gRealBombchuEyeTex"
static const ALIGN_ASSET(2) char gRealBombchuEyeTex[] = dgRealBombchuEyeTex;

#define dgRealBombchuSkel "__OTR__objects/object_rat/gRealBombchuSkel"
static const ALIGN_ASSET(2) char gRealBombchuSkel[] = dgRealBombchuSkel;

typedef enum RealBombchuLimb {
    /* 0x00 */ REAL_BOMBCHU_LIMB_NONE,
    /* 0x01 */ REAL_BOMBCHU_LIMB_BODY,
    /* 0x02 */ REAL_BOMBCHU_LIMB_HEAD,
    /* 0x03 */ REAL_BOMBCHU_LIMB_TAIL_BASE,
    /* 0x04 */ REAL_BOMBCHU_LIMB_TAIL_MIDDLE,
    /* 0x05 */ REAL_BOMBCHU_LIMB_TAIL_END,
    /* 0x06 */ REAL_BOMBCHU_LIMB_FRONT_LEFT_LEG,
    /* 0x07 */ REAL_BOMBCHU_LIMB_BACK_LEFT_LEG,
    /* 0x08 */ REAL_BOMBCHU_LIMB_FRONT_RIGHT_LEG,
    /* 0x09 */ REAL_BOMBCHU_LIMB_BACK_RIGHT_LEG,
    /* 0x0A */ REAL_BOMBCHU_LIMB_MAX
} RealBombchuLimb;

#endif // OBJECTS_OBJECT_RAT_H
