#ifndef OBJECTS_OBJECT_BOSS04_H
#define OBJECTS_OBJECT_BOSS04_H 1

#include "align_asset_macro.h"

#define dgWartIdleAnim "__OTR__objects/object_boss04/gWartIdleAnim"
static const ALIGN_ASSET(2) char gWartIdleAnim[] = dgWartIdleAnim;

#define dgWartShellDL "__OTR__objects/object_boss04/gWartShellDL"
static const ALIGN_ASSET(2) char gWartShellDL[] = dgWartShellDL;

#define dgWartBottomEyelidDL "__OTR__objects/object_boss04/gWartBottomEyelidDL"
static const ALIGN_ASSET(2) char gWartBottomEyelidDL[] = dgWartBottomEyelidDL;

#define dgWartTopEyelidDL "__OTR__objects/object_boss04/gWartTopEyelidDL"
static const ALIGN_ASSET(2) char gWartTopEyelidDL[] = dgWartTopEyelidDL;

#define dgWartEyeDL "__OTR__objects/object_boss04/gWartEyeDL"
static const ALIGN_ASSET(2) char gWartEyeDL[] = dgWartEyeDL;

#define dgWartShellTLUT "__OTR__objects/object_boss04/gWartShellTLUT"
static const ALIGN_ASSET(2) char gWartShellTLUT[] = dgWartShellTLUT;

#define dgWartRidgesTLUT "__OTR__objects/object_boss04/gWartRidgesTLUT"
static const ALIGN_ASSET(2) char gWartRidgesTLUT[] = dgWartRidgesTLUT;

#define dgWartShellTex "__OTR__objects/object_boss04/gWartShellTex"
static const ALIGN_ASSET(2) char gWartShellTex[] = dgWartShellTex;

#define dgWartRidgesTex "__OTR__objects/object_boss04/gWartRidgesTex"
static const ALIGN_ASSET(2) char gWartRidgesTex[] = dgWartRidgesTex;

#define dgWartEyeTex "__OTR__objects/object_boss04/gWartEyeTex"
static const ALIGN_ASSET(2) char gWartEyeTex[] = dgWartEyeTex;

#define dgWartBubbleOpaqueMaterialDL "__OTR__objects/object_boss04/gWartBubbleOpaqueMaterialDL"
static const ALIGN_ASSET(2) char gWartBubbleOpaqueMaterialDL[] = dgWartBubbleOpaqueMaterialDL;

#define dgWartBubbleMaterialDL "__OTR__objects/object_boss04/gWartBubbleMaterialDL"
static const ALIGN_ASSET(2) char gWartBubbleMaterialDL[] = dgWartBubbleMaterialDL;

#define dgWartBubbleModelDL "__OTR__objects/object_boss04/gWartBubbleModelDL"
static const ALIGN_ASSET(2) char gWartBubbleModelDL[] = dgWartBubbleModelDL;

#define dgWartBubbleTex "__OTR__objects/object_boss04/gWartBubbleTex"
static const ALIGN_ASSET(2) char gWartBubbleTex[] = dgWartBubbleTex;

#define dgWartShadowTex "__OTR__objects/object_boss04/gWartShadowTex"
static const ALIGN_ASSET(2) char gWartShadowTex[] = dgWartShadowTex;

#define dgWartShadowMaterialDL "__OTR__objects/object_boss04/gWartShadowMaterialDL"
static const ALIGN_ASSET(2) char gWartShadowMaterialDL[] = dgWartShadowMaterialDL;

#define dgWartShadowModelDL "__OTR__objects/object_boss04/gWartShadowModelDL"
static const ALIGN_ASSET(2) char gWartShadowModelDL[] = dgWartShadowModelDL;

#define dgWartSkel "__OTR__objects/object_boss04/gWartSkel"
static const ALIGN_ASSET(2) char gWartSkel[] = dgWartSkel;

typedef enum WartLimb {
    /* 0x00 */ WART_LIMB_NONE,
    /* 0x01 */ WART_LIMB_ROOT,
    /* 0x02 */ WART_LIMB_BODY,
    /* 0x03 */ WART_LIMB_EYE_ROOT,
    /* 0x04 */ WART_LIMB_EYE,
    /* 0x05 */ WART_LIMB_TOP_EYELID_ROOT,
    /* 0x06 */ WART_LIMB_TOP_EYELID,
    /* 0x07 */ WART_LIMB_BOTTOM_EYELID_ROOT,
    /* 0x08 */ WART_LIMB_BOTTOM_EYELID,
    /* 0x09 */ WART_LIMB_MAX
} WartLimb;

#endif // OBJECTS_OBJECT_BOSS04_H
