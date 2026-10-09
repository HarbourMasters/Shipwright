#ifndef OBJECTS_OBJECT_HATA_H
#define OBJECTS_OBJECT_HATA_H 1

#include "align_asset_macro.h"

#define dgFlagpoleCol "__OTR__objects/object_hata/gFlagpoleCol"
static const ALIGN_ASSET(2) char gFlagpoleCol[] = dgFlagpoleCol;

#define dgFlagpoleFlapAnim "__OTR__objects/object_hata/gFlagpoleFlapAnim"
static const ALIGN_ASSET(2) char gFlagpoleFlapAnim[] = dgFlagpoleFlapAnim;

#define dgFlagpolePoleTex "__OTR__objects/object_hata/gFlagpolePoleTex"
static const ALIGN_ASSET(2) char gFlagpolePoleTex[] = dgFlagpolePoleTex;

#define dgFlagpoleTopTex "__OTR__objects/object_hata/gFlagpoleTopTex"
static const ALIGN_ASSET(2) char gFlagpoleTopTex[] = dgFlagpoleTopTex;

#define dgFlagpoleFlagTex "__OTR__objects/object_hata/gFlagpoleFlagTex"
static const ALIGN_ASSET(2) char gFlagpoleFlagTex[] = dgFlagpoleFlagTex;

#define dgFlagpolePoleDL "__OTR__objects/object_hata/gFlagpolePoleDL"
static const ALIGN_ASSET(2) char gFlagpolePoleDL[] = dgFlagpolePoleDL;

#define dgFlagpoleFlag1FlyEndDL "__OTR__objects/object_hata/gFlagpoleFlag1FlyEndDL"
static const ALIGN_ASSET(2) char gFlagpoleFlag1FlyEndDL[] = dgFlagpoleFlag1FlyEndDL;

#define dgFlagpoleFlag1FlyMidDL "__OTR__objects/object_hata/gFlagpoleFlag1FlyMidDL"
static const ALIGN_ASSET(2) char gFlagpoleFlag1FlyMidDL[] = dgFlagpoleFlag1FlyMidDL;

#define dgFlagpoleFlag1HoistMidDL "__OTR__objects/object_hata/gFlagpoleFlag1HoistMidDL"
static const ALIGN_ASSET(2) char gFlagpoleFlag1HoistMidDL[] = dgFlagpoleFlag1HoistMidDL;

#define dgFlagpoleFlag1HoistEndDL "__OTR__objects/object_hata/gFlagpoleFlag1HoistEndDL"
static const ALIGN_ASSET(2) char gFlagpoleFlag1HoistEndDL[] = dgFlagpoleFlag1HoistEndDL;

#define dgFlagpoleFlag2FlyEndDL "__OTR__objects/object_hata/gFlagpoleFlag2FlyEndDL"
static const ALIGN_ASSET(2) char gFlagpoleFlag2FlyEndDL[] = dgFlagpoleFlag2FlyEndDL;

#define dgFlagpoleFlag2FlyMidDL "__OTR__objects/object_hata/gFlagpoleFlag2FlyMidDL"
static const ALIGN_ASSET(2) char gFlagpoleFlag2FlyMidDL[] = dgFlagpoleFlag2FlyMidDL;

#define dgFlagpoleFlag2HoistMidDL "__OTR__objects/object_hata/gFlagpoleFlag2HoistMidDL"
static const ALIGN_ASSET(2) char gFlagpoleFlag2HoistMidDL[] = dgFlagpoleFlag2HoistMidDL;

#define dgFlagpoleFlag2HoistEndDL "__OTR__objects/object_hata/gFlagpoleFlag2HoistEndDL"
static const ALIGN_ASSET(2) char gFlagpoleFlag2HoistEndDL[] = dgFlagpoleFlag2HoistEndDL;

#define dgFlagpoleSkel "__OTR__objects/object_hata/gFlagpoleSkel"
static const ALIGN_ASSET(2) char gFlagpoleSkel[] = dgFlagpoleSkel;

typedef enum FlagpoleLimb {
    /* 0x00 */ FLAGPOLE_LIMB_NONE,
    /* 0x01 */ FLAGPOLE_LIMB_BASE,
    /* 0x02 */ FLAGPOLE_LIMB_POLE,
    /* 0x03 */ FLAGPOLE_LIMB_FLAG1_BASE,
    /* 0x04 */ FLAGPOLE_LIMB_FLAG1_HOIST_END_BASE,
    /* 0x05 */ FLAGPOLE_LIMB_FLAG1_HOIST_MID_BASE,
    /* 0x06 */ FLAGPOLE_LIMB_FLAG1_FLY_MID_BASE,
    /* 0x07 */ FLAGPOLE_LIMB_FLAG1_FLY_END_BASE,
    /* 0x08 */ FLAGPOLE_LIMB_FLAG1_FLY_END,
    /* 0x09 */ FLAGPOLE_LIMB_FLAG1_FLY_MID,
    /* 0x0A */ FLAGPOLE_LIMB_FLAG1_HOIST_MID,
    /* 0x0B */ FLAGPOLE_LIMB_FLAG1_HOIST_END,
    /* 0x0C */ FLAGPOLE_LIMB_FLAG2_BASE,
    /* 0x0D */ FLAGPOLE_LIMB_FLAG2_HOIST_END_BASE,
    /* 0x0E */ FLAGPOLE_LIMB_FLAG2_HOIST_MID_BASE,
    /* 0x0F */ FLAGPOLE_LIMB_FLAG2_FLY_MID_BASE,
    /* 0x10 */ FLAGPOLE_LIMB_FLAG2_FLY_END_BASE,
    /* 0x11 */ FLAGPOLE_LIMB_FLAG2_FLY_END,
    /* 0x12 */ FLAGPOLE_LIMB_FLAG2_FLY_MID,
    /* 0x13 */ FLAGPOLE_LIMB_FLAG2_HOIST_MID,
    /* 0x14 */ FLAGPOLE_LIMB_FLAG2_HOIST_END,
    /* 0x15 */ FLAGPOLE_LIMB_MAX
} FlagpoleLimb;

#endif // OBJECTS_OBJECT_HATA_H
