#ifndef OBJECTS_OBJECT_BEE_H
#define OBJECTS_OBJECT_BEE_H 1

#include "align_asset_macro.h"

#define dgBeeFlyingAnim "__OTR__objects/object_bee/gBeeFlyingAnim"
static const ALIGN_ASSET(2) char gBeeFlyingAnim[] = dgBeeFlyingAnim;

#define dgBeeBodyDL "__OTR__objects/object_bee/gBeeBodyDL"
static const ALIGN_ASSET(2) char gBeeBodyDL[] = dgBeeBodyDL;

#define dgBeeHeadDL "__OTR__objects/object_bee/gBeeHeadDL"
static const ALIGN_ASSET(2) char gBeeHeadDL[] = dgBeeHeadDL;

#define dgBeeAntennaeDL "__OTR__objects/object_bee/gBeeAntennaeDL"
static const ALIGN_ASSET(2) char gBeeAntennaeDL[] = dgBeeAntennaeDL;

#define dgBeeRightWingDL "__OTR__objects/object_bee/gBeeRightWingDL"
static const ALIGN_ASSET(2) char gBeeRightWingDL[] = dgBeeRightWingDL;

#define dgBeeLeftWingDL "__OTR__objects/object_bee/gBeeLeftWingDL"
static const ALIGN_ASSET(2) char gBeeLeftWingDL[] = dgBeeLeftWingDL;

#define dgBeeWingTLUT "__OTR__objects/object_bee/gBeeWingTLUT"
static const ALIGN_ASSET(2) char gBeeWingTLUT[] = dgBeeWingTLUT;

#define dgBeeHeadTLUT "__OTR__objects/object_bee/gBeeHeadTLUT"
static const ALIGN_ASSET(2) char gBeeHeadTLUT[] = dgBeeHeadTLUT;

#define dgBeeBodyTLUT "__OTR__objects/object_bee/gBeeBodyTLUT"
static const ALIGN_ASSET(2) char gBeeBodyTLUT[] = dgBeeBodyTLUT;

#define dgBeeAntennaeTLUT "__OTR__objects/object_bee/gBeeAntennaeTLUT"
static const ALIGN_ASSET(2) char gBeeAntennaeTLUT[] = dgBeeAntennaeTLUT;

#define dgBeeWingTex "__OTR__objects/object_bee/gBeeWingTex"
static const ALIGN_ASSET(2) char gBeeWingTex[] = dgBeeWingTex;

#define dgBeeHeadTex "__OTR__objects/object_bee/gBeeHeadTex"
static const ALIGN_ASSET(2) char gBeeHeadTex[] = dgBeeHeadTex;

#define dgBeeBodyTex "__OTR__objects/object_bee/gBeeBodyTex"
static const ALIGN_ASSET(2) char gBeeBodyTex[] = dgBeeBodyTex;

#define dgBeeAntennaeTex "__OTR__objects/object_bee/gBeeAntennaeTex"
static const ALIGN_ASSET(2) char gBeeAntennaeTex[] = dgBeeAntennaeTex;

#define dgBeeSkel "__OTR__objects/object_bee/gBeeSkel"
static const ALIGN_ASSET(2) char gBeeSkel[] = dgBeeSkel;

typedef enum ObjectBeeLimb {
    /* 0x00 */ OBJECT_BEE_LIMB_NONE,
    /* 0x01 */ OBJECT_BEE_LIMB_ROOT,
    /* 0x02 */ OBJECT_BEE_LIMB_WINGS_ROOT,
    /* 0x03 */ OBJECT_BEE_LIMB_LEFT_WING_ROOT,
    /* 0x04 */ OBJECT_BEE_LIMB_LEFT_WING,
    /* 0x05 */ OBJECT_BEE_LIMB_RIGHT_WING_ROOT,
    /* 0x06 */ OBJECT_BEE_LIMB_RIGHT_WING,
    /* 0x07 */ OBJECT_BEE_LIMB_BODY,
    /* 0x08 */ OBJECT_BEE_LIMB_ANTENNAE,
    /* 0x09 */ OBJECT_BEE_LIMB_HEAD,
    /* 0x0A */ OBJECT_BEE_LIMB_MAX
} ObjectBeeLimb;

#endif // OBJECTS_OBJECT_BEE_H
