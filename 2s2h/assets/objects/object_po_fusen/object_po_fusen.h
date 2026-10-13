#ifndef OBJECTS_OBJECT_PO_FUSEN_H
#define OBJECTS_OBJECT_PO_FUSEN_H 1

#include "align_asset_macro.h"

#define dgPoeBalloonEmptyAnim "__OTR__objects/object_po_fusen/gPoeBalloonEmptyAnim"
static const ALIGN_ASSET(2) char gPoeBalloonEmptyAnim[] = dgPoeBalloonEmptyAnim;

#define dgPoeBalloonChainAndLanternDL "__OTR__objects/object_po_fusen/gPoeBalloonChainAndLanternDL"
static const ALIGN_ASSET(2) char gPoeBalloonChainAndLanternDL[] = dgPoeBalloonChainAndLanternDL;

#define dgPoeBalloonLeftHandDL "__OTR__objects/object_po_fusen/gPoeBalloonLeftHandDL"
static const ALIGN_ASSET(2) char gPoeBalloonLeftHandDL[] = dgPoeBalloonLeftHandDL;

#define dgPoeBalloonLeftForearmDL "__OTR__objects/object_po_fusen/gPoeBalloonLeftForearmDL"
static const ALIGN_ASSET(2) char gPoeBalloonLeftForearmDL[] = dgPoeBalloonLeftForearmDL;

#define dgPoeBalloonLeftUpperArmDL "__OTR__objects/object_po_fusen/gPoeBalloonLeftUpperArmDL"
static const ALIGN_ASSET(2) char gPoeBalloonLeftUpperArmDL[] = dgPoeBalloonLeftUpperArmDL;

#define dgPoeBalloonRightHandDL "__OTR__objects/object_po_fusen/gPoeBalloonRightHandDL"
static const ALIGN_ASSET(2) char gPoeBalloonRightHandDL[] = dgPoeBalloonRightHandDL;

#define dgPoeBalloonRightForearmDL "__OTR__objects/object_po_fusen/gPoeBalloonRightForearmDL"
static const ALIGN_ASSET(2) char gPoeBalloonRightForearmDL[] = dgPoeBalloonRightForearmDL;

#define dgPoeBalloonRightUpperArmDL "__OTR__objects/object_po_fusen/gPoeBalloonRightUpperArmDL"
static const ALIGN_ASSET(2) char gPoeBalloonRightUpperArmDL[] = dgPoeBalloonRightUpperArmDL;

#define dgPoeBalloonBodyDL "__OTR__objects/object_po_fusen/gPoeBalloonBodyDL"
static const ALIGN_ASSET(2) char gPoeBalloonBodyDL[] = dgPoeBalloonBodyDL;

#define dgPoeBalloonTLUT "__OTR__objects/object_po_fusen/gPoeBalloonTLUT"
static const ALIGN_ASSET(2) char gPoeBalloonTLUT[] = dgPoeBalloonTLUT;

#define dgPoeBalloonTatteredChinTex "__OTR__objects/object_po_fusen/gPoeBalloonTatteredChinTex"
static const ALIGN_ASSET(2) char gPoeBalloonTatteredChinTex[] = dgPoeBalloonTatteredChinTex;

#define dgPoeBalloonLanternTopTex "__OTR__objects/object_po_fusen/gPoeBalloonLanternTopTex"
static const ALIGN_ASSET(2) char gPoeBalloonLanternTopTex[] = dgPoeBalloonLanternTopTex;

#define dgPoeBalloonLanternStandTex "__OTR__objects/object_po_fusen/gPoeBalloonLanternStandTex"
static const ALIGN_ASSET(2) char gPoeBalloonLanternStandTex[] = dgPoeBalloonLanternStandTex;

#define dgPoeBalloonLanternGlassTex "__OTR__objects/object_po_fusen/gPoeBalloonLanternGlassTex"
static const ALIGN_ASSET(2) char gPoeBalloonLanternGlassTex[] = dgPoeBalloonLanternGlassTex;

#define dgPoeBalloonHandTex "__OTR__objects/object_po_fusen/gPoeBalloonHandTex"
static const ALIGN_ASSET(2) char gPoeBalloonHandTex[] = dgPoeBalloonHandTex;

#define dgPoeBalloonOrangeSkinTex "__OTR__objects/object_po_fusen/gPoeBalloonOrangeSkinTex"
static const ALIGN_ASSET(2) char gPoeBalloonOrangeSkinTex[] = dgPoeBalloonOrangeSkinTex;

#define dgPoeBalloonFaceTex "__OTR__objects/object_po_fusen/gPoeBalloonFaceTex"
static const ALIGN_ASSET(2) char gPoeBalloonFaceTex[] = dgPoeBalloonFaceTex;

#define dgPoeBalloonChainLinkTex "__OTR__objects/object_po_fusen/gPoeBalloonChainLinkTex"
static const ALIGN_ASSET(2) char gPoeBalloonChainLinkTex[] = dgPoeBalloonChainLinkTex;

#define dgPoeBalloonSkel "__OTR__objects/object_po_fusen/gPoeBalloonSkel"
static const ALIGN_ASSET(2) char gPoeBalloonSkel[] = dgPoeBalloonSkel;

typedef enum PoeBalloonLimb {
    /* 0x00 */ POE_BALLOON_LIMB_NONE,
    /* 0x01 */ POE_BALLOON_LIMB_ROOT,
    /* 0x02 */ POE_BALLOON_LIMB_BODY,
    /* 0x03 */ POE_BALLOON_RIGHT_UPPER_ARM,
    /* 0x04 */ POE_BALLOON_RIGHT_FOREARM,
    /* 0x05 */ POE_BALLOON_RIGHT_HAND,
    /* 0x06 */ POE_BALLOON_LEFT_UPPER_ARM,
    /* 0x07 */ POE_BALLOON_LEFT_FOREARM,
    /* 0x08 */ POE_BALLOON_LEFT_HAND,
    /* 0x09 */ POE_BALLOON_LIMB_CHAIN_AND_LANTERN,
    /* 0x0A */ POE_BALLOON_LIMB_MAX
} PoeBalloonLimb;

#endif // OBJECTS_OBJECT_PO_FUSEN_H
