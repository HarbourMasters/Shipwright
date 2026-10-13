#ifndef OBJECTS_OBJECT_NIW_H
#define OBJECTS_OBJECT_NIW_H 1

#include "align_asset_macro.h"

#define dgNiwIdleAnim "__OTR__objects/object_niw/gNiwIdleAnim"
static const ALIGN_ASSET(2) char gNiwIdleAnim[] = dgNiwIdleAnim;

#define dgNiwNeckDL "__OTR__objects/object_niw/gNiwNeckDL"
static const ALIGN_ASSET(2) char gNiwNeckDL[] = dgNiwNeckDL;

#define dgNiwHeadDL "__OTR__objects/object_niw/gNiwHeadDL"
static const ALIGN_ASSET(2) char gNiwHeadDL[] = dgNiwHeadDL;

#define dgNiwTailDL "__OTR__objects/object_niw/gNiwTailDL"
static const ALIGN_ASSET(2) char gNiwTailDL[] = dgNiwTailDL;

#define dgNiwWingLeftDL "__OTR__objects/object_niw/gNiwWingLeftDL"
static const ALIGN_ASSET(2) char gNiwWingLeftDL[] = dgNiwWingLeftDL;

#define dgNiwFootLeftDL "__OTR__objects/object_niw/gNiwFootLeftDL"
static const ALIGN_ASSET(2) char gNiwFootLeftDL[] = dgNiwFootLeftDL;

#define dgNiwWingRightDL "__OTR__objects/object_niw/gNiwWingRightDL"
static const ALIGN_ASSET(2) char gNiwWingRightDL[] = dgNiwWingRightDL;

#define dgNiwFootRightDL "__OTR__objects/object_niw/gNiwFootRightDL"
static const ALIGN_ASSET(2) char gNiwFootRightDL[] = dgNiwFootRightDL;

#define dgNiwEyeTex "__OTR__objects/object_niw/gNiwEyeTex"
static const ALIGN_ASSET(2) char gNiwEyeTex[] = dgNiwEyeTex;

#define dgNiwHeadFeathersTex "__OTR__objects/object_niw/gNiwHeadFeathersTex"
static const ALIGN_ASSET(2) char gNiwHeadFeathersTex[] = dgNiwHeadFeathersTex;

#define dgNiwBaseFeathersTex "__OTR__objects/object_niw/gNiwBaseFeathersTex"
static const ALIGN_ASSET(2) char gNiwBaseFeathersTex[] = dgNiwBaseFeathersTex;

#define dgNiwBeakTex "__OTR__objects/object_niw/gNiwBeakTex"
static const ALIGN_ASSET(2) char gNiwBeakTex[] = dgNiwBeakTex;

#define dgNiwCombTex "__OTR__objects/object_niw/gNiwCombTex"
static const ALIGN_ASSET(2) char gNiwCombTex[] = dgNiwCombTex;

#define dgNiwTailFeathersTex "__OTR__objects/object_niw/gNiwTailFeathersTex"
static const ALIGN_ASSET(2) char gNiwTailFeathersTex[] = dgNiwTailFeathersTex;

#define dgNiwFootTex "__OTR__objects/object_niw/gNiwFootTex"
static const ALIGN_ASSET(2) char gNiwFootTex[] = dgNiwFootTex;

#define dgNiwFeatherTex "__OTR__objects/object_niw/gNiwFeatherTex"
static const ALIGN_ASSET(2) char gNiwFeatherTex[] = dgNiwFeatherTex;

#define dgNiwFeatherMaterialDL "__OTR__objects/object_niw/gNiwFeatherMaterialDL"
static const ALIGN_ASSET(2) char gNiwFeatherMaterialDL[] = dgNiwFeatherMaterialDL;

#define dgNiwFeatherDL "__OTR__objects/object_niw/gNiwFeatherDL"
static const ALIGN_ASSET(2) char gNiwFeatherDL[] = dgNiwFeatherDL;

#define dgNiwSkel "__OTR__objects/object_niw/gNiwSkel"
static const ALIGN_ASSET(2) char gNiwSkel[] = dgNiwSkel;

typedef enum ObjectNiwLimb {
    /* 0x00 */ NIW_LIMB_NONE,
    /* 0x01 */ NIW_LIMB_ROOT,
    /* 0x02 */ NIW_LIMB_2,
    /* 0x03 */ NIW_LIMB_3,
    /* 0x04 */ NIW_LIMB_UNDER_SIDE,
    /* 0x05 */ NIW_LIMB_LEFT_FOOT_ROOT,
    /* 0x06 */ NIW_LIMB_LEFT_FOOT,
    /* 0x07 */ NIW_LIMB_LEFT_WING_ROOT,
    /* 0x08 */ NIW_LIMB_LEFT_WING,
    /* 0x09 */ NIW_LIMB_RIGHT_FOOT_ROOT,
    /* 0x0A */ NIW_LIMB_RIGHT_FOOT,
    /* 0x0B */ NIW_LIMB_RIGHT_WING_ROOT,
    /* 0x0C */ NIW_LIMB_RIGHT_WING,
    /* 0x0D */ NIW_LIMB_UPPER_BODY,
    /* 0x0E */ NIW_LIMB_NECK,
    /* 0x0F */ NIW_LIMB_HEAD,
    /* 0x10 */ NIW_LIMB_MAX
} ObjectNiwLimb;

#endif // OBJECTS_OBJECT_NIW_H
