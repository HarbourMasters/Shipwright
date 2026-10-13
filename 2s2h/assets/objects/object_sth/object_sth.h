#ifndef OBJECTS_OBJECT_STH_H
#define OBJECTS_OBJECT_STH_H 1

#include "align_asset_macro.h"

#define dgSthUnusedHeadVtx "__OTR__objects/object_sth/gSthUnusedHeadVtx"
static const ALIGN_ASSET(2) char gSthUnusedHeadVtx[] = dgSthUnusedHeadVtx;

#define dgSthHeadDL "__OTR__objects/object_sth/gSthHeadDL"
static const ALIGN_ASSET(2) char gSthHeadDL[] = dgSthHeadDL;

#define dgSthRightHandDL "__OTR__objects/object_sth/gSthRightHandDL"
static const ALIGN_ASSET(2) char gSthRightHandDL[] = dgSthRightHandDL;

#define dgSthRightForearmDL "__OTR__objects/object_sth/gSthRightForearmDL"
static const ALIGN_ASSET(2) char gSthRightForearmDL[] = dgSthRightForearmDL;

#define dgSthRightUpperArmDL "__OTR__objects/object_sth/gSthRightUpperArmDL"
static const ALIGN_ASSET(2) char gSthRightUpperArmDL[] = dgSthRightUpperArmDL;

#define dgSthLeftHandDL "__OTR__objects/object_sth/gSthLeftHandDL"
static const ALIGN_ASSET(2) char gSthLeftHandDL[] = dgSthLeftHandDL;

#define dgSthLeftForearmDL "__OTR__objects/object_sth/gSthLeftForearmDL"
static const ALIGN_ASSET(2) char gSthLeftForearmDL[] = dgSthLeftForearmDL;

#define dgSthLeftUpperArmDL "__OTR__objects/object_sth/gSthLeftUpperArmDL"
static const ALIGN_ASSET(2) char gSthLeftUpperArmDL[] = dgSthLeftUpperArmDL;

#define dgSthTorsoDL "__OTR__objects/object_sth/gSthTorsoDL"
static const ALIGN_ASSET(2) char gSthTorsoDL[] = dgSthTorsoDL;

#define dgSthRightFootDL "__OTR__objects/object_sth/gSthRightFootDL"
static const ALIGN_ASSET(2) char gSthRightFootDL[] = dgSthRightFootDL;

#define dgSthRightShinDL "__OTR__objects/object_sth/gSthRightShinDL"
static const ALIGN_ASSET(2) char gSthRightShinDL[] = dgSthRightShinDL;

#define dgSthRightThighDL "__OTR__objects/object_sth/gSthRightThighDL"
static const ALIGN_ASSET(2) char gSthRightThighDL[] = dgSthRightThighDL;

#define dgSthLeftFootDL "__OTR__objects/object_sth/gSthLeftFootDL"
static const ALIGN_ASSET(2) char gSthLeftFootDL[] = dgSthLeftFootDL;

#define dgSthLeftShinDL "__OTR__objects/object_sth/gSthLeftShinDL"
static const ALIGN_ASSET(2) char gSthLeftShinDL[] = dgSthLeftShinDL;

#define dgSthLeftThighDL "__OTR__objects/object_sth/gSthLeftThighDL"
static const ALIGN_ASSET(2) char gSthLeftThighDL[] = dgSthLeftThighDL;

#define dgSthPelvisDL "__OTR__objects/object_sth/gSthPelvisDL"
static const ALIGN_ASSET(2) char gSthPelvisDL[] = dgSthPelvisDL;

#define dgSthTLUT "__OTR__objects/object_sth/gSthTLUT"
static const ALIGN_ASSET(2) char gSthTLUT[] = dgSthTLUT;

#define dgSthSkinTex "__OTR__objects/object_sth/gSthSkinTex"
static const ALIGN_ASSET(2) char gSthSkinTex[] = dgSthSkinTex;

#define dgSthFingerTex "__OTR__objects/object_sth/gSthFingerTex"
static const ALIGN_ASSET(2) char gSthFingerTex[] = dgSthFingerTex;

#define dgSthShirtTex "__OTR__objects/object_sth/gSthShirtTex"
static const ALIGN_ASSET(2) char gSthShirtTex[] = dgSthShirtTex;

#define dgSthBeltBuckleTex "__OTR__objects/object_sth/gSthBeltBuckleTex"
static const ALIGN_ASSET(2) char gSthBeltBuckleTex[] = dgSthBeltBuckleTex;

#define dgSthPantsTex "__OTR__objects/object_sth/gSthPantsTex"
static const ALIGN_ASSET(2) char gSthPantsTex[] = dgSthPantsTex;

#define dgSthSkel "__OTR__objects/object_sth/gSthSkel"
static const ALIGN_ASSET(2) char gSthSkel[] = dgSthSkel;

typedef enum ObjectSthLimb {
    /* 0x00 */ STH_LIMB_NONE,
    /* 0x01 */ STH_LIMB_PELVIS,
    /* 0x02 */ STH_LIMB_LEFT_THIGH,
    /* 0x03 */ STH_LIMB_LEFT_SHIN,
    /* 0x04 */ STH_LIMB_LEFT_FOOT,
    /* 0x05 */ STH_LIMB_RIGHT_THIGH,
    /* 0x06 */ STH_LIMB_RIGHT_SHIN,
    /* 0x07 */ STH_LIMB_RIGHT_FOOT,
    /* 0x08 */ STH_LIMB_CHEST,
    /* 0x09 */ STH_LIMB_LEFT_UPPER_ARM,
    /* 0x0A */ STH_LIMB_LEFT_FOREARM,
    /* 0x0B */ STH_LIMB_LEFT_HAND,
    /* 0x0C */ STH_LIMB_RIGHT_UPPER_ARM,
    /* 0x0D */ STH_LIMB_RIGHT_FOREARM,
    /* 0x0E */ STH_LIMB_RIGHT_HAND,
    /* 0x0F */ STH_LIMB_HEAD,
    /* 0x10 */ STH_LIMB_MAX
} ObjectSthLimb;

#endif // OBJECTS_OBJECT_STH_H
