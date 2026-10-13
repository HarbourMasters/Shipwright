#ifndef OBJECTS_OBJECT_GMO_H
#define OBJECTS_OBJECT_GMO_H 1

#include "align_asset_macro.h"

#define dgNejironIdleAnim "__OTR__objects/object_gmo/gNejironIdleAnim"
static const ALIGN_ASSET(2) char gNejironIdleAnim[] = dgNejironIdleAnim;

#define dgNejironBodyDL "__OTR__objects/object_gmo/gNejironBodyDL"
static const ALIGN_ASSET(2) char gNejironBodyDL[] = dgNejironBodyDL;

#define dgNejironEyesDL "__OTR__objects/object_gmo/gNejironEyesDL"
static const ALIGN_ASSET(2) char gNejironEyesDL[] = dgNejironEyesDL;

#define dgNejironBodyTLUT "__OTR__objects/object_gmo/gNejironBodyTLUT"
static const ALIGN_ASSET(2) char gNejironBodyTLUT[] = dgNejironBodyTLUT;

#define dgNejironBodyTex "__OTR__objects/object_gmo/gNejironBodyTex"
static const ALIGN_ASSET(2) char gNejironBodyTex[] = dgNejironBodyTex;

#define dgNejironEyeOpenTex "__OTR__objects/object_gmo/gNejironEyeOpenTex"
static const ALIGN_ASSET(2) char gNejironEyeOpenTex[] = dgNejironEyeOpenTex;

#define dgNejironEyeHalfTex "__OTR__objects/object_gmo/gNejironEyeHalfTex"
static const ALIGN_ASSET(2) char gNejironEyeHalfTex[] = dgNejironEyeHalfTex;

#define dgNejironEyeClosedTex "__OTR__objects/object_gmo/gNejironEyeClosedTex"
static const ALIGN_ASSET(2) char gNejironEyeClosedTex[] = dgNejironEyeClosedTex;

#define dgNejironSkel "__OTR__objects/object_gmo/gNejironSkel"
static const ALIGN_ASSET(2) char gNejironSkel[] = dgNejironSkel;

typedef enum NejironLimb {
    /* 0x00 */ NEJIRON_LIMB_NONE,
    /* 0x01 */ NEJIRON_LIMB_BODY,
    /* 0x02 */ NEJIRON_LIMB_EYES,
    /* 0x03 */ NEJIRON_LIMB_MAX
} NejironLimb;

#endif // OBJECTS_OBJECT_GMO_H
