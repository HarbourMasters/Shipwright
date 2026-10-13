#ifndef OBJECTS_OBJECT_FIREFLY_H
#define OBJECTS_OBJECT_FIREFLY_H 1

#include "align_asset_macro.h"

#define dgFireKeeseFlyAnim "__OTR__objects/object_firefly/gFireKeeseFlyAnim"
static const ALIGN_ASSET(2) char gFireKeeseFlyAnim[] = dgFireKeeseFlyAnim;

#define dgFireKeeseBodyTex "__OTR__objects/object_firefly/gFireKeeseBodyTex"
static const ALIGN_ASSET(2) char gFireKeeseBodyTex[] = dgFireKeeseBodyTex;

#define dgFireKeeseEyeTex "__OTR__objects/object_firefly/gFireKeeseEyeTex"
static const ALIGN_ASSET(2) char gFireKeeseEyeTex[] = dgFireKeeseEyeTex;

#define dgFireKeeseEarTex "__OTR__objects/object_firefly/gFireKeeseEarTex"
static const ALIGN_ASSET(2) char gFireKeeseEarTex[] = dgFireKeeseEarTex;

#define dgFireKeeseTalonTex "__OTR__objects/object_firefly/gFireKeeseTalonTex"
static const ALIGN_ASSET(2) char gFireKeeseTalonTex[] = dgFireKeeseTalonTex;

#define dgFireKeeseWingTex "__OTR__objects/object_firefly/gFireKeeseWingTex"
static const ALIGN_ASSET(2) char gFireKeeseWingTex[] = dgFireKeeseWingTex;

#define dgKeeseRedEyeTex "__OTR__objects/object_firefly/gKeeseRedEyeTex"
static const ALIGN_ASSET(2) char gKeeseRedEyeTex[] = dgKeeseRedEyeTex;

#define dgKeeseBodyTex "__OTR__objects/object_firefly/gKeeseBodyTex"
static const ALIGN_ASSET(2) char gKeeseBodyTex[] = dgKeeseBodyTex;

#define dgKeeseTalonTex "__OTR__objects/object_firefly/gKeeseTalonTex"
static const ALIGN_ASSET(2) char gKeeseTalonTex[] = dgKeeseTalonTex;

#define dgKeeseWingTex "__OTR__objects/object_firefly/gKeeseWingTex"
static const ALIGN_ASSET(2) char gKeeseWingTex[] = dgKeeseWingTex;

#define dgKeeseEarTex "__OTR__objects/object_firefly/gKeeseEarTex"
static const ALIGN_ASSET(2) char gKeeseEarTex[] = dgKeeseEarTex;

#define dgFireKeeseHeadDL "__OTR__objects/object_firefly/gFireKeeseHeadDL"
static const ALIGN_ASSET(2) char gFireKeeseHeadDL[] = dgFireKeeseHeadDL;

#define dgFireKeeseBodyDL "__OTR__objects/object_firefly/gFireKeeseBodyDL"
static const ALIGN_ASSET(2) char gFireKeeseBodyDL[] = dgFireKeeseBodyDL;

#define dgFireKeeseLeftFootDL "__OTR__objects/object_firefly/gFireKeeseLeftFootDL"
static const ALIGN_ASSET(2) char gFireKeeseLeftFootDL[] = dgFireKeeseLeftFootDL;

#define dgFireKeeseRightFootDL "__OTR__objects/object_firefly/gFireKeeseRightFootDL"
static const ALIGN_ASSET(2) char gFireKeeseRightFootDL[] = dgFireKeeseRightFootDL;

#define dgFireKeeseRightWingStartDL "__OTR__objects/object_firefly/gFireKeeseRightWingStartDL"
static const ALIGN_ASSET(2) char gFireKeeseRightWingStartDL[] = dgFireKeeseRightWingStartDL;

#define dgFireKeeseRightWingMidDL "__OTR__objects/object_firefly/gFireKeeseRightWingMidDL"
static const ALIGN_ASSET(2) char gFireKeeseRightWingMidDL[] = dgFireKeeseRightWingMidDL;

#define dgFireKeeseRightWingEndDL "__OTR__objects/object_firefly/gFireKeeseRightWingEndDL"
static const ALIGN_ASSET(2) char gFireKeeseRightWingEndDL[] = dgFireKeeseRightWingEndDL;

#define dgFireKeeseLeftWingEndDL "__OTR__objects/object_firefly/gFireKeeseLeftWingEndDL"
static const ALIGN_ASSET(2) char gFireKeeseLeftWingEndDL[] = dgFireKeeseLeftWingEndDL;

#define dgFireKeeseLeftWingMidDL "__OTR__objects/object_firefly/gFireKeeseLeftWingMidDL"
static const ALIGN_ASSET(2) char gFireKeeseLeftWingMidDL[] = dgFireKeeseLeftWingMidDL;

#define dgFireKeeseLeftWingStartDL "__OTR__objects/object_firefly/gFireKeeseLeftWingStartDL"
static const ALIGN_ASSET(2) char gFireKeeseLeftWingStartDL[] = dgFireKeeseLeftWingStartDL;

#define dgKeeseRedEyesDL "__OTR__objects/object_firefly/gKeeseRedEyesDL"
static const ALIGN_ASSET(2) char gKeeseRedEyesDL[] = dgKeeseRedEyesDL;

#define dgFireKeeseSkel "__OTR__objects/object_firefly/gFireKeeseSkel"
static const ALIGN_ASSET(2) char gFireKeeseSkel[] = dgFireKeeseSkel;

typedef enum FireKeeseLimb {
    /* 0x00 */ FIRE_KEESE_LIMB_NONE,
    /* 0x01 */ FIRE_KEESE_LIMB_ROOT,
    /* 0x02 */ FIRE_KEESE_LIMB_ROOT_WRAPPER,
    /* 0x03 */ FIRE_KEESE_LIMB_FEET_ROOT,
    /* 0x04 */ FIRE_KEESE_LIMB_RIGHT_FOOT_ROOT,
    /* 0x05 */ FIRE_KEESE_LIMB_RIGHT_FOOT_WRAPPER,
    /* 0x06 */ FIRE_KEESE_LIMB_RIGHT_FOOT,
    /* 0x07 */ FIRE_KEESE_LIMB_LEFT_FOOT_ROOT,
    /* 0x08 */ FIRE_KEESE_LIMB_LEFT_FOOT_WRAPPER,
    /* 0x09 */ FIRE_KEESE_LIMB_LEFT_FOOT,
    /* 0x0A */ FIRE_KEESE_LIMB_BODY,
    /* 0x0B */ FIRE_KEESE_LIMB_LEFT_WING_ROOT,
    /* 0x0C */ FIRE_KEESE_LIMB_LEFT_WING_WRAPPER,
    /* 0x0D */ FIRE_KEESE_LIMB_LEFT_WING_END_MID_ROOT,
    /* 0x0E */ FIRE_KEESE_LIMB_LEFT_WING_END_ROOT,
    /* 0x0F */ FIRE_KEESE_LIMB_LEFT_WING_END,
    /* 0x10 */ FIRE_KEESE_LIMB_LEFT_WING_MID,
    /* 0x11 */ FIRE_KEESE_LIMB_LEFT_WING_START,
    /* 0x12 */ FIRE_KEESE_LIMB_RIGHT_WING_ROOT,
    /* 0x13 */ FIRE_KEESE_LIMB_RIGHT_WING_WRAPPER,
    /* 0x14 */ FIRE_KEESE_LIMB_RIGHT_WING_END_MID_ROOT,
    /* 0x15 */ FIRE_KEESE_LIMB_RIGHT_WING_END_ROOT,
    /* 0x16 */ FIRE_KEESE_LIMB_RIGHT_WING_END,
    /* 0x17 */ FIRE_KEESE_LIMB_RIGHT_WING_MID,
    /* 0x18 */ FIRE_KEESE_LIMB_RIGHT_WING_START,
    /* 0x19 */ FIRE_KEESE_LIMB_HEAD_ROOT,
    /* 0x1A */ FIRE_KEESE_LIMB_HEAD_WRAPPER,
    /* 0x1B */ FIRE_KEESE_LIMB_HEAD,
    /* 0x1C */ FIRE_KEESE_LIMB_MAX
} FireKeeseLimb;

#endif // OBJECTS_OBJECT_FIREFLY_H
