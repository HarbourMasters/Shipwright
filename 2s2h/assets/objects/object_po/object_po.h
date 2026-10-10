#ifndef OBJECTS_OBJECT_PO_H
#define OBJECTS_OBJECT_PO_H 1

#include "align_asset_macro.h"

#define dgPoeAttackAnim "__OTR__objects/object_po/gPoeAttackAnim"
static const ALIGN_ASSET(2) char gPoeAttackAnim[] = dgPoeAttackAnim;

#define dgPoeDamagedAnim "__OTR__objects/object_po/gPoeDamagedAnim"
static const ALIGN_ASSET(2) char gPoeDamagedAnim[] = dgPoeDamagedAnim;

#define dgPoeFleeAnim "__OTR__objects/object_po/gPoeFleeAnim"
static const ALIGN_ASSET(2) char gPoeFleeAnim[] = dgPoeFleeAnim;

#define dgPoeFloatAnim "__OTR__objects/object_po/gPoeFloatAnim"
static const ALIGN_ASSET(2) char gPoeFloatAnim[] = dgPoeFloatAnim;

#define dgPoeAppearAnim "__OTR__objects/object_po/gPoeAppearAnim"
static const ALIGN_ASSET(2) char gPoeAppearAnim[] = dgPoeAppearAnim;

#define dgPoeDisappearAnim "__OTR__objects/object_po/gPoeDisappearAnim"
static const ALIGN_ASSET(2) char gPoeDisappearAnim[] = dgPoeDisappearAnim;

#define dgPoeRightUpperArmDL "__OTR__objects/object_po/gPoeRightUpperArmDL"
static const ALIGN_ASSET(2) char gPoeRightUpperArmDL[] = dgPoeRightUpperArmDL;

#define dgPoeLeftForearmDL "__OTR__objects/object_po/gPoeLeftForearmDL"
static const ALIGN_ASSET(2) char gPoeLeftForearmDL[] = dgPoeLeftForearmDL;

#define dgPoeLeftUpperArmDL "__OTR__objects/object_po/gPoeLeftUpperArmDL"
static const ALIGN_ASSET(2) char gPoeLeftUpperArmDL[] = dgPoeLeftUpperArmDL;

#define dgPoeRightForearmDL "__OTR__objects/object_po/gPoeRightForearmDL"
static const ALIGN_ASSET(2) char gPoeRightForearmDL[] = dgPoeRightForearmDL;

#define dgPoeRightHandDL "__OTR__objects/object_po/gPoeRightHandDL"
static const ALIGN_ASSET(2) char gPoeRightHandDL[] = dgPoeRightHandDL;

#define dgPoeCloak1DL "__OTR__objects/object_po/gPoeCloak1DL"
static const ALIGN_ASSET(2) char gPoeCloak1DL[] = dgPoeCloak1DL;

#define dgPoeBurnDL "__OTR__objects/object_po/gPoeBurnDL"
static const ALIGN_ASSET(2) char gPoeBurnDL[] = dgPoeBurnDL;

#define dgPoeFaceDL "__OTR__objects/object_po/gPoeFaceDL"
static const ALIGN_ASSET(2) char gPoeFaceDL[] = dgPoeFaceDL;

#define dgPoeLanternDL "__OTR__objects/object_po/gPoeLanternDL"
static const ALIGN_ASSET(2) char gPoeLanternDL[] = dgPoeLanternDL;

#define dgPoeCloak2DL "__OTR__objects/object_po/gPoeCloak2DL"
static const ALIGN_ASSET(2) char gPoeCloak2DL[] = dgPoeCloak2DL;

#define dgPoeSoulTex "__OTR__objects/object_po/gPoeSoulTex"
static const ALIGN_ASSET(2) char gPoeSoulTex[] = dgPoeSoulTex;

#define dgPoeSoulDL "__OTR__objects/object_po/gPoeSoulDL"
static const ALIGN_ASSET(2) char gPoeSoulDL[] = dgPoeSoulDL;

#define dgPoeArmTex "__OTR__objects/object_po/gPoeArmTex"
static const ALIGN_ASSET(2) char gPoeArmTex[] = dgPoeArmTex;

#define dgPoeCloak1Tex "__OTR__objects/object_po/gPoeCloak1Tex"
static const ALIGN_ASSET(2) char gPoeCloak1Tex[] = dgPoeCloak1Tex;

#define dgPoeCloak2Tex "__OTR__objects/object_po/gPoeCloak2Tex"
static const ALIGN_ASSET(2) char gPoeCloak2Tex[] = dgPoeCloak2Tex;

#define dgPoeCloak3Tex "__OTR__objects/object_po/gPoeCloak3Tex"
static const ALIGN_ASSET(2) char gPoeCloak3Tex[] = dgPoeCloak3Tex;

#define dgPoeEyeTex "__OTR__objects/object_po/gPoeEyeTex"
static const ALIGN_ASSET(2) char gPoeEyeTex[] = dgPoeEyeTex;

#define dgPoeLanternTopTex "__OTR__objects/object_po/gPoeLanternTopTex"
static const ALIGN_ASSET(2) char gPoeLanternTopTex[] = dgPoeLanternTopTex;

#define dgPoeLanternBottomTex "__OTR__objects/object_po/gPoeLanternBottomTex"
static const ALIGN_ASSET(2) char gPoeLanternBottomTex[] = dgPoeLanternBottomTex;

#define dgPoeLanternMiddleTex "__OTR__objects/object_po/gPoeLanternMiddleTex"
static const ALIGN_ASSET(2) char gPoeLanternMiddleTex[] = dgPoeLanternMiddleTex;

#define dgPoeBurnTex "__OTR__objects/object_po/gPoeBurnTex"
static const ALIGN_ASSET(2) char gPoeBurnTex[] = dgPoeBurnTex;

#define dgPoeBurnEyeTex "__OTR__objects/object_po/gPoeBurnEyeTex"
static const ALIGN_ASSET(2) char gPoeBurnEyeTex[] = dgPoeBurnEyeTex;

#define dgPoeSkel "__OTR__objects/object_po/gPoeSkel"
static const ALIGN_ASSET(2) char gPoeSkel[] = dgPoeSkel;

typedef enum PoeLimb {
    /* 0x00 */ POE_LIMB_NONE,
    /* 0x01 */ POE_LIMB_ROOT,
    /* 0x02 */ POE_LIMB_ROOT_WRAPPER,
    /* 0x03 */ POE_LIMB_BOTTOM_CLOAK_ROOT,
    /* 0x04 */ POE_LIMB_BOTTOM_CLOAK,
    /* 0x05 */ POE_LIMB_TOP_CLOAK,
    /* 0x06 */ POE_LIMB_LEFT_ARM_ROOT,
    /* 0x07 */ POE_LIMB_LEFT_ARM_WRAPPER,
    /* 0x08 */ POE_LIMB_LEFT_FOREARM_ROOT,
    /* 0x09 */ POE_LIMB_LEFT_FOREARM,
    /* 0x0A */ POE_LIMB_LEFT_UPPER_ARM,
    /* 0x0B */ POE_LIMB_FACE,
    /* 0x0C */ POE_LIMB_RIGHT_ARM_ROOT,
    /* 0x0D */ POE_LIMB_RIGHT_ARM_WRAPPER,
    /* 0x0E */ POE_LIMB_RIGHT_FOREARM_ROOT,
    /* 0x0F */ POE_LIMB_RIGHT_ARM_HAND_ROOT,
    /* 0x10 */ POE_LIMB_RIGHT_ARM_HAND,
    /* 0x11 */ POE_LIMB_LANTERN_ROOT,
    /* 0x12 */ POE_LIMB_LANTERN,
    /* 0x13 */ POE_LIMB_RIGHT_FOREARM,
    /* 0x14 */ POE_LIMB_RIGHT_UPPER_ARM,
    /* 0x15 */ POE_LIMB_MAX
} PoeLimb;

#endif // OBJECTS_OBJECT_PO_H
