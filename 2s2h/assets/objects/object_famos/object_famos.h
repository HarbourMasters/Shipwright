#ifndef OBJECTS_OBJECT_FAMOS_H
#define OBJECTS_OBJECT_FAMOS_H 1

#include "align_asset_macro.h"

#define dgFamosShakeAnim "__OTR__objects/object_famos/gFamosShakeAnim"
static const ALIGN_ASSET(2) char gFamosShakeAnim[] = dgFamosShakeAnim;

#define dgFamosHelmetDL "__OTR__objects/object_famos/gFamosHelmetDL"
static const ALIGN_ASSET(2) char gFamosHelmetDL[] = dgFamosHelmetDL;

#define dgFamosMainBodyDL "__OTR__objects/object_famos/gFamosMainBodyDL"
static const ALIGN_ASSET(2) char gFamosMainBodyDL[] = dgFamosMainBodyDL;

#define dgFamosShieldDL "__OTR__objects/object_famos/gFamosShieldDL"
static const ALIGN_ASSET(2) char gFamosShieldDL[] = dgFamosShieldDL;

#define dgFamosSwordDL "__OTR__objects/object_famos/gFamosSwordDL"
static const ALIGN_ASSET(2) char gFamosSwordDL[] = dgFamosSwordDL;

#define dgFamosLightSwitchDL "__OTR__objects/object_famos/gFamosLightSwitchDL"
static const ALIGN_ASSET(2) char gFamosLightSwitchDL[] = dgFamosLightSwitchDL;

#define dgFamosBodyTLUT "__OTR__objects/object_famos/gFamosBodyTLUT"
static const ALIGN_ASSET(2) char gFamosBodyTLUT[] = dgFamosBodyTLUT;

#define dgFamosBottomTLUT "__OTR__objects/object_famos/gFamosBottomTLUT"
static const ALIGN_ASSET(2) char gFamosBottomTLUT[] = dgFamosBottomTLUT;

#define dgFamosShieldTLUT "__OTR__objects/object_famos/gFamosShieldTLUT"
static const ALIGN_ASSET(2) char gFamosShieldTLUT[] = dgFamosShieldTLUT;

#define dgFamosSwordHiltTLUT "__OTR__objects/object_famos/gFamosSwordHiltTLUT"
static const ALIGN_ASSET(2) char gFamosSwordHiltTLUT[] = dgFamosSwordHiltTLUT;

#define dgFamosMainBodyTex "__OTR__objects/object_famos/gFamosMainBodyTex"
static const ALIGN_ASSET(2) char gFamosMainBodyTex[] = dgFamosMainBodyTex;

#define dgFamosBottomTex "__OTR__objects/object_famos/gFamosBottomTex"
static const ALIGN_ASSET(2) char gFamosBottomTex[] = dgFamosBottomTex;

#define dgFamosShieldTex "__OTR__objects/object_famos/gFamosShieldTex"
static const ALIGN_ASSET(2) char gFamosShieldTex[] = dgFamosShieldTex;

#define dgFamosSwordHiltTex "__OTR__objects/object_famos/gFamosSwordHiltTex"
static const ALIGN_ASSET(2) char gFamosSwordHiltTex[] = dgFamosSwordHiltTex;

#define dgFamosLightSwitchTex "__OTR__objects/object_famos/gFamosLightSwitchTex"
static const ALIGN_ASSET(2) char gFamosLightSwitchTex[] = dgFamosLightSwitchTex;

#define dgFamosSkel "__OTR__objects/object_famos/gFamosSkel"
static const ALIGN_ASSET(2) char gFamosSkel[] = dgFamosSkel;

#define dgFamosIdleAnim "__OTR__objects/object_famos/gFamosIdleAnim"
static const ALIGN_ASSET(2) char gFamosIdleAnim[] = dgFamosIdleAnim;

#define dgFamosNormalGlowingEmblemTexAnim "__OTR__objects/object_famos/gFamosNormalGlowingEmblemTexAnim"
static const ALIGN_ASSET(2) char gFamosNormalGlowingEmblemTexAnim[] = dgFamosNormalGlowingEmblemTexAnim;

#define dgFamosFlippedGlowingEmblemTexAnim "__OTR__objects/object_famos/gFamosFlippedGlowingEmblemTexAnim"
static const ALIGN_ASSET(2) char gFamosFlippedGlowingEmblemTexAnim[] = dgFamosFlippedGlowingEmblemTexAnim;

typedef enum FamosLimb {
    /* 0x00 */ FAMOS_LIMB_NONE,
    /* 0x01 */ FAMOS_LIMB_BODY,
    /* 0x02 */ FAMOS_LIMB_EMBLEM,
    /* 0x03 */ FAMOS_LIMB_SWORD,
    /* 0x04 */ FAMOS_LIMB_SHIELD,
    /* 0x05 */ FAMOS_LIMB_HEAD,
    /* 0x06 */ FAMOS_LIMB_MAX
} FamosLimb;


#endif // OBJECTS_OBJECT_FAMOS_H
