#ifndef OBJECTS_OBJECT_UCH_H
#define OBJECTS_OBJECT_UCH_H 1

#include "align_asset_macro.h"

#define dgAlienEyeBeamDL "__OTR__objects/object_uch/gAlienEyeBeamDL"
static const ALIGN_ASSET(2) char gAlienEyeBeamDL[] = dgAlienEyeBeamDL;

#define dgAlienEyeBeamMaskTex "__OTR__objects/object_uch/gAlienEyeBeamMaskTex"
static const ALIGN_ASSET(2) char gAlienEyeBeamMaskTex[] = dgAlienEyeBeamMaskTex;

#define dgAlienEyeBeamTexAnim "__OTR__objects/object_uch/gAlienEyeBeamTexAnim"
static const ALIGN_ASSET(2) char gAlienEyeBeamTexAnim[] = dgAlienEyeBeamTexAnim;

#define dgAlienEmptyTexAnim "__OTR__objects/object_uch/gAlienEmptyTexAnim"
static const ALIGN_ASSET(2) char gAlienEmptyTexAnim[] = dgAlienEmptyTexAnim;

#define dgAlienDeathAnim "__OTR__objects/object_uch/gAlienDeathAnim"
static const ALIGN_ASSET(2) char gAlienDeathAnim[] = dgAlienDeathAnim;

#define dgAlienJerkingAnim "__OTR__objects/object_uch/gAlienJerkingAnim"
static const ALIGN_ASSET(2) char gAlienJerkingAnim[] = dgAlienJerkingAnim;

#define dgAlienDeathFlashDL "__OTR__objects/object_uch/gAlienDeathFlashDL"
static const ALIGN_ASSET(2) char gAlienDeathFlashDL[] = dgAlienDeathFlashDL;

#define dgAlienDeathFlashMaskTex "__OTR__objects/object_uch/gAlienDeathFlashMaskTex"
static const ALIGN_ASSET(2) char gAlienDeathFlashMaskTex[] = dgAlienDeathFlashMaskTex;

#define dgAlienHoldingCowAnim "__OTR__objects/object_uch/gAlienHoldingCowAnim"
static const ALIGN_ASSET(2) char gAlienHoldingCowAnim[] = dgAlienHoldingCowAnim;

#define dgAlienFloatAnim "__OTR__objects/object_uch/gAlienFloatAnim"
static const ALIGN_ASSET(2) char gAlienFloatAnim[] = dgAlienFloatAnim;

#define dgAlienHeadDL "__OTR__objects/object_uch/gAlienHeadDL"
static const ALIGN_ASSET(2) char gAlienHeadDL[] = dgAlienHeadDL;

#define dgAlienRightEyeDL "__OTR__objects/object_uch/gAlienRightEyeDL"
static const ALIGN_ASSET(2) char gAlienRightEyeDL[] = dgAlienRightEyeDL;

#define dgAlienLeftEyeDL "__OTR__objects/object_uch/gAlienLeftEyeDL"
static const ALIGN_ASSET(2) char gAlienLeftEyeDL[] = dgAlienLeftEyeDL;

#define dgAlienLeftShoulderDL "__OTR__objects/object_uch/gAlienLeftShoulderDL"
static const ALIGN_ASSET(2) char gAlienLeftShoulderDL[] = dgAlienLeftShoulderDL;

#define dgAlienLeftUpperArmDL "__OTR__objects/object_uch/gAlienLeftUpperArmDL"
static const ALIGN_ASSET(2) char gAlienLeftUpperArmDL[] = dgAlienLeftUpperArmDL;

#define dgAlienLeftForearmDL "__OTR__objects/object_uch/gAlienLeftForearmDL"
static const ALIGN_ASSET(2) char gAlienLeftForearmDL[] = dgAlienLeftForearmDL;

#define dgAlienLeftHandDL "__OTR__objects/object_uch/gAlienLeftHandDL"
static const ALIGN_ASSET(2) char gAlienLeftHandDL[] = dgAlienLeftHandDL;

#define dgAlienTorsoDL "__OTR__objects/object_uch/gAlienTorsoDL"
static const ALIGN_ASSET(2) char gAlienTorsoDL[] = dgAlienTorsoDL;

#define dgAlienRightShoulderDL "__OTR__objects/object_uch/gAlienRightShoulderDL"
static const ALIGN_ASSET(2) char gAlienRightShoulderDL[] = dgAlienRightShoulderDL;

#define dgAlienRightUpperArmDL "__OTR__objects/object_uch/gAlienRightUpperArmDL"
static const ALIGN_ASSET(2) char gAlienRightUpperArmDL[] = dgAlienRightUpperArmDL;

#define dgAlienRightForearmDL "__OTR__objects/object_uch/gAlienRightForearmDL"
static const ALIGN_ASSET(2) char gAlienRightForearmDL[] = dgAlienRightForearmDL;

#define dgAlienRightHandDL "__OTR__objects/object_uch/gAlienRightHandDL"
static const ALIGN_ASSET(2) char gAlienRightHandDL[] = dgAlienRightHandDL;

#define dgAlienTLUT "__OTR__objects/object_uch/gAlienTLUT"
static const ALIGN_ASSET(2) char gAlienTLUT[] = dgAlienTLUT;

#define dgAlienTorsoTex "__OTR__objects/object_uch/gAlienTorsoTex"
static const ALIGN_ASSET(2) char gAlienTorsoTex[] = dgAlienTorsoTex;

#define dgAlienHandsAndTorsoUndersideTex "__OTR__objects/object_uch/gAlienHandsAndTorsoUndersideTex"
static const ALIGN_ASSET(2) char gAlienHandsAndTorsoUndersideTex[] = dgAlienHandsAndTorsoUndersideTex;

#define dgAlienHeadFrontAndArmsTex "__OTR__objects/object_uch/gAlienHeadFrontAndArmsTex"
static const ALIGN_ASSET(2) char gAlienHeadFrontAndArmsTex[] = dgAlienHeadFrontAndArmsTex;

#define dgAlienHeadBackTex "__OTR__objects/object_uch/gAlienHeadBackTex"
static const ALIGN_ASSET(2) char gAlienHeadBackTex[] = dgAlienHeadBackTex;

#define dgAlienEyeTex "__OTR__objects/object_uch/gAlienEyeTex"
static const ALIGN_ASSET(2) char gAlienEyeTex[] = dgAlienEyeTex;

#define dgAlienSkel "__OTR__objects/object_uch/gAlienSkel"
static const ALIGN_ASSET(2) char gAlienSkel[] = dgAlienSkel;

typedef enum AlienLimb {
    /* 0x00 */ ALIEN_LIMB_NONE,
    /* 0x01 */ ALIEN_LIMB_ROOT,
    /* 0x02 */ ALIEN_LIMB_TORSO,
    /* 0x03 */ ALIEN_LIMB_RIGHT_SHOULDER,
    /* 0x04 */ ALIEN_LIMB_RIGHT_UPPER_ARM,
    /* 0x05 */ ALIEN_LIMB_RIGHT_FOREARM,
    /* 0x06 */ ALIEN_LIMB_RIGHT_HAND,
    /* 0x07 */ ALIEN_LIMB_LEFT_SHOULDER,
    /* 0x08 */ ALIEN_LIMB_LEFT_UPPER_ARM,
    /* 0x09 */ ALIEN_LIMB_LEFT_FOREARM,
    /* 0x0A */ ALIEN_LIMB_LEFT_HAND,
    /* 0x0B */ ALIEN_LIMB_HEAD,
    /* 0x0C */ ALIEN_LIMB_LEFT_EYE,
    /* 0x0D */ ALIEN_LIMB_RIGHT_EYE,
    /* 0x0E */ ALIEN_LIMB_MAX
} AlienLimb;

#endif // OBJECTS_OBJECT_UCH_H
