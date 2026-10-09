#ifndef OBJECTS_OBJECT_CROW_H
#define OBJECTS_OBJECT_CROW_H 1

#include "align_asset_macro.h"

#define dgGuayFlyAnim "__OTR__objects/object_crow/gGuayFlyAnim"
static const ALIGN_ASSET(2) char gGuayFlyAnim[] = dgGuayFlyAnim;

#define dgGuayBodyDL "__OTR__objects/object_crow/gGuayBodyDL"
static const ALIGN_ASSET(2) char gGuayBodyDL[] = dgGuayBodyDL;

#define dgGuayRightWingTipDL "__OTR__objects/object_crow/gGuayRightWingTipDL"
static const ALIGN_ASSET(2) char gGuayRightWingTipDL[] = dgGuayRightWingTipDL;

#define dgGuayRightWingBodyDL "__OTR__objects/object_crow/gGuayRightWingBodyDL"
static const ALIGN_ASSET(2) char gGuayRightWingBodyDL[] = dgGuayRightWingBodyDL;

#define dgGuayLeftWingTipDL "__OTR__objects/object_crow/gGuayLeftWingTipDL"
static const ALIGN_ASSET(2) char gGuayLeftWingTipDL[] = dgGuayLeftWingTipDL;

#define dgGuayLeftWingBodyDL "__OTR__objects/object_crow/gGuayLeftWingBodyDL"
static const ALIGN_ASSET(2) char gGuayLeftWingBodyDL[] = dgGuayLeftWingBodyDL;

#define dgGuayTailDL "__OTR__objects/object_crow/gGuayTailDL"
static const ALIGN_ASSET(2) char gGuayTailDL[] = dgGuayTailDL;

#define dgGuayUpperTailDL "__OTR__objects/object_crow/gGuayUpperTailDL"
static const ALIGN_ASSET(2) char gGuayUpperTailDL[] = dgGuayUpperTailDL;

#define dgGuayBodyTex "__OTR__objects/object_crow/gGuayBodyTex"
static const ALIGN_ASSET(2) char gGuayBodyTex[] = dgGuayBodyTex;

#define dgGuayEyeTex "__OTR__objects/object_crow/gGuayEyeTex"
static const ALIGN_ASSET(2) char gGuayEyeTex[] = dgGuayEyeTex;

#define dgGuayTailTex "__OTR__objects/object_crow/gGuayTailTex"
static const ALIGN_ASSET(2) char gGuayTailTex[] = dgGuayTailTex;

#define dgGuaySkel "__OTR__objects/object_crow/gGuaySkel"
static const ALIGN_ASSET(2) char gGuaySkel[] = dgGuaySkel;

typedef enum ObjectCrowLimb {
    /* 0x00 */ OBJECT_CROW_LIMB_NONE,
    /* 0x01 */ OBJECT_CROW_LIMB_ROOT,
    /* 0x02 */ OBJECT_CROW_LIMB_BODY,
    /* 0x03 */ OBJECT_CROW_LIMB_RIGHT_WING_BODY,
    /* 0x04 */ OBJECT_CROW_LIMB_RIGHT_WING_TIP,
    /* 0x05 */ OBJECT_CROW_LIMB_LEFT_WING_BODY,
    /* 0x06 */ OBJECT_CROW_LIMB_LEFT_WING_TIP,
    /* 0x07 */ OBJECT_CROW_LIMB_UPPER_TAIL,
    /* 0x08 */ OBJECT_CROW_LIMB_TAIL,
    /* 0x09 */ OBJECT_CROW_LIMB_MAX
} ObjectCrowLimb;

#endif // OBJECTS_OBJECT_CROW_H
