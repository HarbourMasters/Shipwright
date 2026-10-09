#ifndef OBJECTS_OBJECT_RB_H
#define OBJECTS_OBJECT_RB_H 1

#include "align_asset_macro.h"

#define dgLeeverSpinAnim "__OTR__objects/object_rb/gLeeverSpinAnim"
static const ALIGN_ASSET(2) char gLeeverSpinAnim[] = dgLeeverSpinAnim;

#define dgLeeverBottomHalfDL "__OTR__objects/object_rb/gLeeverBottomHalfDL"
static const ALIGN_ASSET(2) char gLeeverBottomHalfDL[] = dgLeeverBottomHalfDL;

#define dgLeeverBottomHalfWrapper2DL "__OTR__objects/object_rb/gLeeverBottomHalfWrapper2DL"
static const ALIGN_ASSET(2) char gLeeverBottomHalfWrapper2DL[] = dgLeeverBottomHalfWrapper2DL;

#define dgLeeverBottomHalfWrapper1DL "__OTR__objects/object_rb/gLeeverBottomHalfWrapper1DL"
static const ALIGN_ASSET(2) char gLeeverBottomHalfWrapper1DL[] = dgLeeverBottomHalfWrapper1DL;

#define dgLeeverTopHalfDL "__OTR__objects/object_rb/gLeeverTopHalfDL"
static const ALIGN_ASSET(2) char gLeeverTopHalfDL[] = dgLeeverTopHalfDL;

#define dgLeeverTopHalfWrapper2DL "__OTR__objects/object_rb/gLeeverTopHalfWrapper2DL"
static const ALIGN_ASSET(2) char gLeeverTopHalfWrapper2DL[] = dgLeeverTopHalfWrapper2DL;

#define dgLeeverTopHalfWrapper1DL "__OTR__objects/object_rb/gLeeverTopHalfWrapper1DL"
static const ALIGN_ASSET(2) char gLeeverTopHalfWrapper1DL[] = dgLeeverTopHalfWrapper1DL;

#define dgLeeverSpike1DL "__OTR__objects/object_rb/gLeeverSpike1DL"
static const ALIGN_ASSET(2) char gLeeverSpike1DL[] = dgLeeverSpike1DL;

#define dgLeeverSpike1Wrapper2DL "__OTR__objects/object_rb/gLeeverSpike1Wrapper2DL"
static const ALIGN_ASSET(2) char gLeeverSpike1Wrapper2DL[] = dgLeeverSpike1Wrapper2DL;

#define dgLeeverSpike1Wrapper1DL "__OTR__objects/object_rb/gLeeverSpike1Wrapper1DL"
static const ALIGN_ASSET(2) char gLeeverSpike1Wrapper1DL[] = dgLeeverSpike1Wrapper1DL;

#define dgLeeverSpike2DL "__OTR__objects/object_rb/gLeeverSpike2DL"
static const ALIGN_ASSET(2) char gLeeverSpike2DL[] = dgLeeverSpike2DL;

#define dgLeeverSpike2Wrapper2DL "__OTR__objects/object_rb/gLeeverSpike2Wrapper2DL"
static const ALIGN_ASSET(2) char gLeeverSpike2Wrapper2DL[] = dgLeeverSpike2Wrapper2DL;

#define dgLeeverSpike2Wrapper1DL "__OTR__objects/object_rb/gLeeverSpike2Wrapper1DL"
static const ALIGN_ASSET(2) char gLeeverSpike2Wrapper1DL[] = dgLeeverSpike2Wrapper1DL;

#define dgLeeverSpike3DL "__OTR__objects/object_rb/gLeeverSpike3DL"
static const ALIGN_ASSET(2) char gLeeverSpike3DL[] = dgLeeverSpike3DL;

#define dgLeeverSpike3Wrapper2DL "__OTR__objects/object_rb/gLeeverSpike3Wrapper2DL"
static const ALIGN_ASSET(2) char gLeeverSpike3Wrapper2DL[] = dgLeeverSpike3Wrapper2DL;

#define dgLeeverSpike3Wrapper1DL "__OTR__objects/object_rb/gLeeverSpike3Wrapper1DL"
static const ALIGN_ASSET(2) char gLeeverSpike3Wrapper1DL[] = dgLeeverSpike3Wrapper1DL;

#define dgLeeverSpike4DL "__OTR__objects/object_rb/gLeeverSpike4DL"
static const ALIGN_ASSET(2) char gLeeverSpike4DL[] = dgLeeverSpike4DL;

#define dgLeeverSpike4Wrapper2DL "__OTR__objects/object_rb/gLeeverSpike4Wrapper2DL"
static const ALIGN_ASSET(2) char gLeeverSpike4Wrapper2DL[] = dgLeeverSpike4Wrapper2DL;

#define dgLeeverSpike4Wrapper1DL "__OTR__objects/object_rb/gLeeverSpike4Wrapper1DL"
static const ALIGN_ASSET(2) char gLeeverSpike4Wrapper1DL[] = dgLeeverSpike4Wrapper1DL;

#define dgLeeverSpikeTex "__OTR__objects/object_rb/gLeeverSpikeTex"
static const ALIGN_ASSET(2) char gLeeverSpikeTex[] = dgLeeverSpikeTex;

#define dgLeeverSideTex "__OTR__objects/object_rb/gLeeverSideTex"
static const ALIGN_ASSET(2) char gLeeverSideTex[] = dgLeeverSideTex;

#define dgLeeverFlowerTex "__OTR__objects/object_rb/gLeeverFlowerTex"
static const ALIGN_ASSET(2) char gLeeverFlowerTex[] = dgLeeverFlowerTex;

#define dgLeeverSkel "__OTR__objects/object_rb/gLeeverSkel"
static const ALIGN_ASSET(2) char gLeeverSkel[] = dgLeeverSkel;

typedef enum LeeverLimb {
    /* 0x00 */ LEEVER_LIMB_NONE,
    /* 0x01 */ OBJECT_RB_LIMB_01,
    /* 0x02 */ OBJECT_RB_LIMB_02,
    /* 0x03 */ OBJECT_RB_LIMB_03,
    /* 0x04 */ LEEVER_LIMB_TOP_HALF,
    /* 0x05 */ OBJECT_RB_LIMB_05,
    /* 0x06 */ OBJECT_RB_LIMB_06,
    /* 0x07 */ LEEVER_LIMB_SPIKE_1,
    /* 0x08 */ OBJECT_RB_LIMB_08,
    /* 0x09 */ OBJECT_RB_LIMB_09,
    /* 0x0A */ LEEVER_LIMB_SPIKE_2,
    /* 0x0B */ OBJECT_RB_LIMB_0B,
    /* 0x0C */ OBJECT_RB_LIMB_0C,
    /* 0x0D */ LEEVER_LIMB_SPIKE_3,
    /* 0x0E */ OBJECT_RB_LIMB_0E,
    /* 0x0F */ OBJECT_RB_LIMB_0F,
    /* 0x10 */ LEEVER_LIMB_SPIKE_4,
    /* 0x11 */ LEEVER_LIMB_BOTTOM_HALF,
    /* 0x12 */ LEEVER_LIMB_MAX
} LeeverLimb;

#endif // OBJECTS_OBJECT_RB_H
