#ifndef OBJECTS_OBJECT_COW_H
#define OBJECTS_OBJECT_COW_H 1

#include "align_asset_macro.h"

#define dgCowChewAnim "__OTR__objects/object_cow/gCowChewAnim"
static const ALIGN_ASSET(2) char gCowChewAnim[] = dgCowChewAnim;

#define dgCowBodyDL "__OTR__objects/object_cow/gCowBodyDL"
static const ALIGN_ASSET(2) char gCowBodyDL[] = dgCowBodyDL;

#define dgCowHeadDL "__OTR__objects/object_cow/gCowHeadDL"
static const ALIGN_ASSET(2) char gCowHeadDL[] = dgCowHeadDL;

#define dgCowNoseDL "__OTR__objects/object_cow/gCowNoseDL"
static const ALIGN_ASSET(2) char gCowNoseDL[] = dgCowNoseDL;

#define dgCowNoseRingDL "__OTR__objects/object_cow/gCowNoseRingDL"
static const ALIGN_ASSET(2) char gCowNoseRingDL[] = dgCowNoseRingDL;

#define dgCowJawDL "__OTR__objects/object_cow/gCowJawDL"
static const ALIGN_ASSET(2) char gCowJawDL[] = dgCowJawDL;

#define dgCowTLUT "__OTR__objects/object_cow/gCowTLUT"
static const ALIGN_ASSET(2) char gCowTLUT[] = dgCowTLUT;

#define dgCowUdderTex "__OTR__objects/object_cow/gCowUdderTex"
static const ALIGN_ASSET(2) char gCowUdderTex[] = dgCowUdderTex;

#define dgCowNoseRingTex "__OTR__objects/object_cow/gCowNoseRingTex"
static const ALIGN_ASSET(2) char gCowNoseRingTex[] = dgCowNoseRingTex;

#define dgCowNoseTex "__OTR__objects/object_cow/gCowNoseTex"
static const ALIGN_ASSET(2) char gCowNoseTex[] = dgCowNoseTex;

#define dgCowSpotsTex "__OTR__objects/object_cow/gCowSpotsTex"
static const ALIGN_ASSET(2) char gCowSpotsTex[] = dgCowSpotsTex;

#define dgCowEarTex "__OTR__objects/object_cow/gCowEarTex"
static const ALIGN_ASSET(2) char gCowEarTex[] = dgCowEarTex;

#define dgCowEyelidTex "__OTR__objects/object_cow/gCowEyelidTex"
static const ALIGN_ASSET(2) char gCowEyelidTex[] = dgCowEyelidTex;

#define dgCowLegAndTailTex "__OTR__objects/object_cow/gCowLegAndTailTex"
static const ALIGN_ASSET(2) char gCowLegAndTailTex[] = dgCowLegAndTailTex;

#define dgCowSkel "__OTR__objects/object_cow/gCowSkel"
static const ALIGN_ASSET(2) char gCowSkel[] = dgCowSkel;

#define dgCowMooAnim "__OTR__objects/object_cow/gCowMooAnim"
static const ALIGN_ASSET(2) char gCowMooAnim[] = dgCowMooAnim;

#define dgCowTailIdleAnim "__OTR__objects/object_cow/gCowTailIdleAnim"
static const ALIGN_ASSET(2) char gCowTailIdleAnim[] = dgCowTailIdleAnim;

#define dgCowTailBaseDL "__OTR__objects/object_cow/gCowTailBaseDL"
static const ALIGN_ASSET(2) char gCowTailBaseDL[] = dgCowTailBaseDL;

#define dgCowTailUpperDL "__OTR__objects/object_cow/gCowTailUpperDL"
static const ALIGN_ASSET(2) char gCowTailUpperDL[] = dgCowTailUpperDL;

#define dgCowTailMiddleDL "__OTR__objects/object_cow/gCowTailMiddleDL"
static const ALIGN_ASSET(2) char gCowTailMiddleDL[] = dgCowTailMiddleDL;

#define dgCowTailLowerDL "__OTR__objects/object_cow/gCowTailLowerDL"
static const ALIGN_ASSET(2) char gCowTailLowerDL[] = dgCowTailLowerDL;

#define dgCowTailEndDL "__OTR__objects/object_cow/gCowTailEndDL"
static const ALIGN_ASSET(2) char gCowTailEndDL[] = dgCowTailEndDL;

#define dgCowTailSkel "__OTR__objects/object_cow/gCowTailSkel"
static const ALIGN_ASSET(2) char gCowTailSkel[] = dgCowTailSkel;

#define dgCowTailSwishAnim "__OTR__objects/object_cow/gCowTailSwishAnim"
static const ALIGN_ASSET(2) char gCowTailSwishAnim[] = dgCowTailSwishAnim;

typedef enum CowLimb {
    /* 0x00 */ COW_LIMB_NONE,
    /* 0x01 */ COW_LIMB_BODY,
    /* 0x02 */ COW_LIMB_HEAD,
    /* 0x03 */ COW_LIMB_JAW,
    /* 0x04 */ COW_LIMB_NOSE,
    /* 0x05 */ COW_LIMB_NOSE_RING,
    /* 0x06 */ COW_LIMB_MAX
} CowLimb;


typedef enum CowTailLimb {
    /* 0x00 */ COW_TAIL_LIMB_NONE,
    /* 0x01 */ COW_TAIL_LIMB_BASE,
    /* 0x02 */ COW_TAIL_LIMB_UPPER,
    /* 0x03 */ COW_TAIL_LIMB_MIDDLE,
    /* 0x04 */ COW_TAIL_LIMB_LOWER,
    /* 0x05 */ COW_TAIL_LIMB_END,
    /* 0x06 */ COW_TAIL_LIMB_MAX
} CowTailLimb;

#endif // OBJECTS_OBJECT_COW_H
