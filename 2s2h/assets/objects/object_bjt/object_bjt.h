#ifndef OBJECTS_OBJECT_BJT_H
#define OBJECTS_OBJECT_BJT_H 1

#include "align_asset_macro.h"

#define dgToiletHandWaggingFingerAnim "__OTR__objects/object_bjt/gToiletHandWaggingFingerAnim"
static const ALIGN_ASSET(2) char gToiletHandWaggingFingerAnim[] = dgToiletHandWaggingFingerAnim;

#define dgToiletHandFistAnim "__OTR__objects/object_bjt/gToiletHandFistAnim"
static const ALIGN_ASSET(2) char gToiletHandFistAnim[] = dgToiletHandFistAnim;

#define dgToiletHandThumbsUpAnim "__OTR__objects/object_bjt/gToiletHandThumbsUpAnim"
static const ALIGN_ASSET(2) char gToiletHandThumbsUpAnim[] = dgToiletHandThumbsUpAnim;

#define dgToiletHandOpenHandAnim "__OTR__objects/object_bjt/gToiletHandOpenHandAnim"
static const ALIGN_ASSET(2) char gToiletHandOpenHandAnim[] = dgToiletHandOpenHandAnim;

#define dgToiletHandWaitingAnim "__OTR__objects/object_bjt/gToiletHandWaitingAnim"
static const ALIGN_ASSET(2) char gToiletHandWaitingAnim[] = dgToiletHandWaitingAnim;

#define dgToiletHandIndexFingerDL "__OTR__objects/object_bjt/gToiletHandIndexFingerDL"
static const ALIGN_ASSET(2) char gToiletHandIndexFingerDL[] = dgToiletHandIndexFingerDL;

#define dgToiletHandThumbDL "__OTR__objects/object_bjt/gToiletHandThumbDL"
static const ALIGN_ASSET(2) char gToiletHandThumbDL[] = dgToiletHandThumbDL;

#define dgToiletHandOtherFingersDL "__OTR__objects/object_bjt/gToiletHandOtherFingersDL"
static const ALIGN_ASSET(2) char gToiletHandOtherFingersDL[] = dgToiletHandOtherFingersDL;

#define dgToiletHandPalmDL "__OTR__objects/object_bjt/gToiletHandPalmDL"
static const ALIGN_ASSET(2) char gToiletHandPalmDL[] = dgToiletHandPalmDL;

#define dgToiletHandArmDL "__OTR__objects/object_bjt/gToiletHandArmDL"
static const ALIGN_ASSET(2) char gToiletHandArmDL[] = dgToiletHandArmDL;

#define dgToiletHandFingersTex "__OTR__objects/object_bjt/gToiletHandFingersTex"
static const ALIGN_ASSET(2) char gToiletHandFingersTex[] = dgToiletHandFingersTex;

#define dgToiletHandArmSkinTex "__OTR__objects/object_bjt/gToiletHandArmSkinTex"
static const ALIGN_ASSET(2) char gToiletHandArmSkinTex[] = dgToiletHandArmSkinTex;

#define dgToiletHandSkel "__OTR__objects/object_bjt/gToiletHandSkel"
static const ALIGN_ASSET(2) char gToiletHandSkel[] = dgToiletHandSkel;

typedef enum ToiletHandLimb {
    /* 0x00 */ TOILET_HAND_LIMB_NONE,
    /* 0x01 */ TOILET_HAND_LIMB_ROOT,
    /* 0x02 */ TOILET_HAND_LIMB_ARM,
    /* 0x03 */ TOILET_HAND_LIMB_PALM,
    /* 0x04 */ TOILET_HAND_LIMB_OTHER_FINGERS,
    /* 0x05 */ TOILET_HAND_LIMB_THUMB,
    /* 0x06 */ TOILET_HAND_LIMB_INDEX_FINGER,
    /* 0x07 */ TOILET_HAND_LIMB_MAX
} ToiletHandLimb;

#endif // OBJECTS_OBJECT_BJT_H
