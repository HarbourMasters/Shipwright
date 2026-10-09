#ifndef OBJECTS_OBJECT_HS_H
#define OBJECTS_OBJECT_HS_H 1

#include "align_asset_macro.h"

#define dgHsShiveringAnim "__OTR__objects/object_hs/gHsShiveringAnim"
static const ALIGN_ASSET(2) char gHsShiveringAnim[] = dgHsShiveringAnim;

#define dgHsPleadingAnim "__OTR__objects/object_hs/gHsPleadingAnim"
static const ALIGN_ASSET(2) char gHsPleadingAnim[] = dgHsPleadingAnim;

#define dgHsIdleAnim "__OTR__objects/object_hs/gHsIdleAnim"
static const ALIGN_ASSET(2) char gHsIdleAnim[] = dgHsIdleAnim;

#define dobject_hs_DL_003760 "__OTR__objects/object_hs/object_hs_DL_003760"
static const ALIGN_ASSET(2) char object_hs_DL_003760[] = dobject_hs_DL_003760;

#define dobject_hs_DL_003AB0 "__OTR__objects/object_hs/object_hs_DL_003AB0"
static const ALIGN_ASSET(2) char object_hs_DL_003AB0[] = dobject_hs_DL_003AB0;

#define dobject_hs_DL_004140 "__OTR__objects/object_hs/object_hs_DL_004140"
static const ALIGN_ASSET(2) char object_hs_DL_004140[] = dobject_hs_DL_004140;

#define dobject_hs_DL_0042A0 "__OTR__objects/object_hs/object_hs_DL_0042A0"
static const ALIGN_ASSET(2) char object_hs_DL_0042A0[] = dobject_hs_DL_0042A0;

#define dobject_hs_DL_004380 "__OTR__objects/object_hs/object_hs_DL_004380"
static const ALIGN_ASSET(2) char object_hs_DL_004380[] = dobject_hs_DL_004380;

#define dobject_hs_DL_004728 "__OTR__objects/object_hs/object_hs_DL_004728"
static const ALIGN_ASSET(2) char object_hs_DL_004728[] = dobject_hs_DL_004728;

#define dobject_hs_DL_004860 "__OTR__objects/object_hs/object_hs_DL_004860"
static const ALIGN_ASSET(2) char object_hs_DL_004860[] = dobject_hs_DL_004860;

#define dobject_hs_DL_004960 "__OTR__objects/object_hs/object_hs_DL_004960"
static const ALIGN_ASSET(2) char object_hs_DL_004960[] = dobject_hs_DL_004960;

#define dobject_hs_DL_004AB8 "__OTR__objects/object_hs/object_hs_DL_004AB8"
static const ALIGN_ASSET(2) char object_hs_DL_004AB8[] = dobject_hs_DL_004AB8;

#define dobject_hs_DL_004BF0 "__OTR__objects/object_hs/object_hs_DL_004BF0"
static const ALIGN_ASSET(2) char object_hs_DL_004BF0[] = dobject_hs_DL_004BF0;

#define dgHsEndDL "__OTR__objects/object_hs/gHsEndDL"
static const ALIGN_ASSET(2) char gHsEndDL[] = dgHsEndDL;

#define dobject_hs_DL_004CF8 "__OTR__objects/object_hs/object_hs_DL_004CF8"
static const ALIGN_ASSET(2) char object_hs_DL_004CF8[] = dobject_hs_DL_004CF8;

#define dobject_hs_TLUT_004E50 "__OTR__objects/object_hs/object_hs_TLUT_004E50"
static const ALIGN_ASSET(2) char object_hs_TLUT_004E50[] = dobject_hs_TLUT_004E50;

#define dgHsRibsTex "__OTR__objects/object_hs/gHsRibsTex"
static const ALIGN_ASSET(2) char gHsRibsTex[] = dgHsRibsTex;

#define dgHsEarTex "__OTR__objects/object_hs/gHsEarTex"
static const ALIGN_ASSET(2) char gHsEarTex[] = dgHsEarTex;

#define dgHsMouthTex "__OTR__objects/object_hs/gHsMouthTex"
static const ALIGN_ASSET(2) char gHsMouthTex[] = dgHsMouthTex;

#define dgHsEyeTex "__OTR__objects/object_hs/gHsEyeTex"
static const ALIGN_ASSET(2) char gHsEyeTex[] = dgHsEyeTex;

#define dgHsPantsTex "__OTR__objects/object_hs/gHsPantsTex"
static const ALIGN_ASSET(2) char gHsPantsTex[] = dgHsPantsTex;

#define dgHsFingersTex "__OTR__objects/object_hs/gHsFingersTex"
static const ALIGN_ASSET(2) char gHsFingersTex[] = dgHsFingersTex;

#define dgHsBraceletTex "__OTR__objects/object_hs/gHsBraceletTex"
static const ALIGN_ASSET(2) char gHsBraceletTex[] = dgHsBraceletTex;

#define dgHsCuccoFeedBagTex "__OTR__objects/object_hs/gHsCuccoFeedBagTex"
static const ALIGN_ASSET(2) char gHsCuccoFeedBagTex[] = dgHsCuccoFeedBagTex;

#define dgHsChairBoxTex "__OTR__objects/object_hs/gHsChairBoxTex"
static const ALIGN_ASSET(2) char gHsChairBoxTex[] = dgHsChairBoxTex;

#define dgHsAbsTex "__OTR__objects/object_hs/gHsAbsTex"
static const ALIGN_ASSET(2) char gHsAbsTex[] = dgHsAbsTex;

#define dgHsBackTex "__OTR__objects/object_hs/gHsBackTex"
static const ALIGN_ASSET(2) char gHsBackTex[] = dgHsBackTex;

#define dgHsSkel "__OTR__objects/object_hs/gHsSkel"
static const ALIGN_ASSET(2) char gHsSkel[] = dgHsSkel;

typedef enum ObjectHsLimb {
    /* 0x00 */ OBJECT_HS_LIMB_NONE,
    /* 0x01 */ HS_LIMB_ROOT,
    /* 0x02 */ HS_LIMB_LEFT_UPPER_ARM,
    /* 0x03 */ HS_LIMB_LEFT_FOREARM,
    /* 0x04 */ HS_LIMB_LEFT_HAND,
    /* 0x05 */ HS_LIMB_RIGHT_UPPER_ARM,
    /* 0x06 */ HS_LIMB_RIGHT_FOREARM,
    /* 0x07 */ HS_LIMB_RIGHT_HAND,
    /* 0x08 */ HS_LIMB_HEAD_ROOT,
    /* 0x09 */ HS_LIMB_HEAD,
    /* 0x0A */ HS_LIMB_HAIR_SPIKES,
    /* 0x0B */ HS_LIMB_HIDDEN_HAIR,
    /* 0x0C */ OBJECT_HS_LIMB_0C,
    /* 0x0D */ OBJECT_HS_LIMB_0D,
    /* 0x0E */ HS_LIMB_LOWER_BODY_PLUS_BOX,
    /* 0x0F */ HS_LIMB_CUCCO_FEED_BAGS,
    /* 0x10 */ OBJECT_HS_LIMB_MAX
} ObjectHsLimb;

#endif // OBJECTS_OBJECT_HS_H
