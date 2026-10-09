#ifndef OBJECTS_OBJECT_PST_H
#define OBJECTS_OBJECT_PST_H 1

#include "align_asset_macro.h"

#define dgPostboxIdleAnim "__OTR__objects/object_pst/gPostboxIdleAnim"
static const ALIGN_ASSET(2) char gPostboxIdleAnim[] = dgPostboxIdleAnim;

#define dgPostboxFrameDL "__OTR__objects/object_pst/gPostboxFrameDL"
static const ALIGN_ASSET(2) char gPostboxFrameDL[] = dgPostboxFrameDL;

#define dgPostboxMailSlotDL "__OTR__objects/object_pst/gPostboxMailSlotDL"
static const ALIGN_ASSET(2) char gPostboxMailSlotDL[] = dgPostboxMailSlotDL;

#define dgPostboxWoodRoofTex "__OTR__objects/object_pst/gPostboxWoodRoofTex"
static const ALIGN_ASSET(2) char gPostboxWoodRoofTex[] = dgPostboxWoodRoofTex;

#define dgPostboxWoodBodyTex "__OTR__objects/object_pst/gPostboxWoodBodyTex"
static const ALIGN_ASSET(2) char gPostboxWoodBodyTex[] = dgPostboxWoodBodyTex;

#define dgPostboxMetalTex "__OTR__objects/object_pst/gPostboxMetalTex"
static const ALIGN_ASSET(2) char gPostboxMetalTex[] = dgPostboxMetalTex;

#define dgPostboxSlotHoleTex "__OTR__objects/object_pst/gPostboxSlotHoleTex"
static const ALIGN_ASSET(2) char gPostboxSlotHoleTex[] = dgPostboxSlotHoleTex;

#define dgPostboxSkel "__OTR__objects/object_pst/gPostboxSkel"
static const ALIGN_ASSET(2) char gPostboxSkel[] = dgPostboxSkel;

typedef enum PostboxLimb {
    /* 0x00 */ POSTBOX_LIMB_NONE,
    /* 0x01 */ POSTBOX_LIMB_FRAME,
    /* 0x02 */ POSTBOX_LIMB_MAIL_SLOT,
    /* 0x03 */ POSTBOX_LIMB_MAX
} PostboxLimb;

#endif // OBJECTS_OBJECT_PST_H
