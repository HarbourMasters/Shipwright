#ifndef OBJECTS_OBJECT_EFC_TW_H
#define OBJECTS_OBJECT_EFC_TW_H 1

#include "align_asset_macro.h"

#define dgTimewarpAnim "__OTR__objects/object_efc_tw/gTimewarpAnim"
static const ALIGN_ASSET(2) char gTimewarpAnim[] = dgTimewarpAnim;

#define dgTimewarpVtx "__OTR__objects/object_efc_tw/gTimewarpVtx"
static const ALIGN_ASSET(2) char gTimewarpVtx[] = dgTimewarpVtx;

#define dgTimewarpDL "__OTR__objects/object_efc_tw/gTimewarpDL"
static const ALIGN_ASSET(2) char gTimewarpDL[] = dgTimewarpDL;

#define dgTimewarpTex "__OTR__objects/object_efc_tw/gTimewarpTex"
static const ALIGN_ASSET(2) char gTimewarpTex[] = dgTimewarpTex;

#define dgTimewarpSkel "__OTR__objects/object_efc_tw/gTimewarpSkel"
static const ALIGN_ASSET(2) char gTimewarpSkel[] = dgTimewarpSkel;

typedef enum TimewarpLimb {
    /* 0x00 */ TIMEWARP_LIMB_NONE,
    /* 0x01 */ TIMEWARP_LIMB_1,
    /* 0x02 */ TIMEWARP_LIMB_2,
    /* 0x03 */ TIMEWARP_LIMB_MAX
} TimewarpLimb;

#endif // OBJECTS_OBJECT_EFC_TW_H
