#ifndef OBJECTS_OBJECT_BB_H
#define OBJECTS_OBJECT_BB_H 1

#include "align_asset_macro.h"

#define dgBubbleAttackAnim "__OTR__objects/object_bb/gBubbleAttackAnim"
static const ALIGN_ASSET(2) char gBubbleAttackAnim[] = dgBubbleAttackAnim;

#define dgBubblePainAnim "__OTR__objects/object_bb/gBubblePainAnim"
static const ALIGN_ASSET(2) char gBubblePainAnim[] = dgBubblePainAnim;

#define dgBubbleFlyingAnim "__OTR__objects/object_bb/gBubbleFlyingAnim"
static const ALIGN_ASSET(2) char gBubbleFlyingAnim[] = dgBubbleFlyingAnim;

#define dgBubbleLeftWingWebbingDL "__OTR__objects/object_bb/gBubbleLeftWingWebbingDL"
static const ALIGN_ASSET(2) char gBubbleLeftWingWebbingDL[] = dgBubbleLeftWingWebbingDL;

#define dgBubbleLeftWingBoneDL "__OTR__objects/object_bb/gBubbleLeftWingBoneDL"
static const ALIGN_ASSET(2) char gBubbleLeftWingBoneDL[] = dgBubbleLeftWingBoneDL;

#define dgBubbleJawDL "__OTR__objects/object_bb/gBubbleJawDL"
static const ALIGN_ASSET(2) char gBubbleJawDL[] = dgBubbleJawDL;

#define dgBubbleCraniumDL "__OTR__objects/object_bb/gBubbleCraniumDL"
static const ALIGN_ASSET(2) char gBubbleCraniumDL[] = dgBubbleCraniumDL;

#define dgBubbleRightWingBoneDL "__OTR__objects/object_bb/gBubbleRightWingBoneDL"
static const ALIGN_ASSET(2) char gBubbleRightWingBoneDL[] = dgBubbleRightWingBoneDL;

#define dgBubbleRightWingWebbingDL "__OTR__objects/object_bb/gBubbleRightWingWebbingDL"
static const ALIGN_ASSET(2) char gBubbleRightWingWebbingDL[] = dgBubbleRightWingWebbingDL;

#define dgBubbleEyeSocketBottomTex "__OTR__objects/object_bb/gBubbleEyeSocketBottomTex"
static const ALIGN_ASSET(2) char gBubbleEyeSocketBottomTex[] = dgBubbleEyeSocketBottomTex;

#define dgBubbleNostrilTex "__OTR__objects/object_bb/gBubbleNostrilTex"
static const ALIGN_ASSET(2) char gBubbleNostrilTex[] = dgBubbleNostrilTex;

#define dgBubbleEyeSocketTopAndJawTex "__OTR__objects/object_bb/gBubbleEyeSocketTopAndJawTex"
static const ALIGN_ASSET(2) char gBubbleEyeSocketTopAndJawTex[] = dgBubbleEyeSocketTopAndJawTex;

#define dgBubbleCraniumTopTex "__OTR__objects/object_bb/gBubbleCraniumTopTex"
static const ALIGN_ASSET(2) char gBubbleCraniumTopTex[] = dgBubbleCraniumTopTex;

#define dgBubbleCraniumSidesAndBackTex "__OTR__objects/object_bb/gBubbleCraniumSidesAndBackTex"
static const ALIGN_ASSET(2) char gBubbleCraniumSidesAndBackTex[] = dgBubbleCraniumSidesAndBackTex;

#define dgBubbleEyeGlowTex "__OTR__objects/object_bb/gBubbleEyeGlowTex"
static const ALIGN_ASSET(2) char gBubbleEyeGlowTex[] = dgBubbleEyeGlowTex;

#define dgBubbleWingBoneTex "__OTR__objects/object_bb/gBubbleWingBoneTex"
static const ALIGN_ASSET(2) char gBubbleWingBoneTex[] = dgBubbleWingBoneTex;

#define dgBubbleWingWebbingTex "__OTR__objects/object_bb/gBubbleWingWebbingTex"
static const ALIGN_ASSET(2) char gBubbleWingWebbingTex[] = dgBubbleWingWebbingTex;

#define dgBubbleSkel "__OTR__objects/object_bb/gBubbleSkel"
static const ALIGN_ASSET(2) char gBubbleSkel[] = dgBubbleSkel;

typedef enum BubbleLimb {
    /* 0x00 */ BUBBLE_LIMB_NONE,
    /* 0x01 */ BUBBLE_LIMB_ROOT,
    /* 0x02 */ BUBBLE_LIMB_CRANIUM_ROOT,
    /* 0x03 */ BUBBLE_LIMB_JAW_ROOT,
    /* 0x04 */ BUBBLE_LIMB_JAW,
    /* 0x05 */ BUBBLE_LIMB_LEFT_WING_ROOT,
    /* 0x06 */ BUBBLE_LIMB_LEFT_WING_WRAPPER,
    /* 0x07 */ BUBBLE_LIMB_LEFT_WING_WEBBING_ROOT,
    /* 0x08 */ BUBBLE_LIMB_LEFT_WING_WEBBING,
    /* 0x09 */ BUBBLE_LIMB_LEFT_WING_BONE,
    /* 0x0A */ BUBBLE_LIMB_RIGHT_WING_ROOT,
    /* 0x0B */ BUBBLE_LIMB_RIGHT_WING_WRAPPER,
    /* 0x0C */ BUBBLE_LIMB_RIGHT_WING_WEBBING_ROOT,
    /* 0x0D */ BUBBLE_LIMB_RIGHT_WING_WEBBING,
    /* 0x0E */ BUBBLE_LIMB_RIGHT_WING_BONE,
    /* 0x0F */ BUBBLE_LIMB_CRANIUM,
    /* 0x10 */ BUBBLE_LIMB_MAX
} BubbleLimb;

#endif // OBJECTS_OBJECT_BB_H
