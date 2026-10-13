#ifndef OBJECTS_OBJECT_TANRON4_H
#define OBJECTS_OBJECT_TANRON4_H 1

#include "align_asset_macro.h"

#define dgSeagullGlideAnim "__OTR__objects/object_tanron4/gSeagullGlideAnim"
static const ALIGN_ASSET(2) char gSeagullGlideAnim[] = dgSeagullGlideAnim;

#define dgSeagullFlapAnim "__OTR__objects/object_tanron4/gSeagullFlapAnim"
static const ALIGN_ASSET(2) char gSeagullFlapAnim[] = dgSeagullFlapAnim;

#define dgSeagullBodyDL "__OTR__objects/object_tanron4/gSeagullBodyDL"
static const ALIGN_ASSET(2) char gSeagullBodyDL[] = dgSeagullBodyDL;

#define dgSeagullRightWingEndDL "__OTR__objects/object_tanron4/gSeagullRightWingEndDL"
static const ALIGN_ASSET(2) char gSeagullRightWingEndDL[] = dgSeagullRightWingEndDL;

#define dgSeagullRightWingStartDL "__OTR__objects/object_tanron4/gSeagullRightWingStartDL"
static const ALIGN_ASSET(2) char gSeagullRightWingStartDL[] = dgSeagullRightWingStartDL;

#define dgSeagullLeftWingEndDL "__OTR__objects/object_tanron4/gSeagullLeftWingEndDL"
static const ALIGN_ASSET(2) char gSeagullLeftWingEndDL[] = dgSeagullLeftWingEndDL;

#define dgSeagullLeftWingStartDL "__OTR__objects/object_tanron4/gSeagullLeftWingStartDL"
static const ALIGN_ASSET(2) char gSeagullLeftWingStartDL[] = dgSeagullLeftWingStartDL;

#define dgSeagullBodyTex "__OTR__objects/object_tanron4/gSeagullBodyTex"
static const ALIGN_ASSET(2) char gSeagullBodyTex[] = dgSeagullBodyTex;

#define dgSeagullWingTex "__OTR__objects/object_tanron4/gSeagullWingTex"
static const ALIGN_ASSET(2) char gSeagullWingTex[] = dgSeagullWingTex;

#define dgSeagullSkel "__OTR__objects/object_tanron4/gSeagullSkel"
static const ALIGN_ASSET(2) char gSeagullSkel[] = dgSeagullSkel;

typedef enum ObjectTanron4Limb {
    /* 0x00 */ SEAGULL_LIMB_RIGHT_WING_NONE,
    /* 0x01 */ SEAGULL_LIMB_ROOT,
    /* 0x02 */ SEAGULL_LIMB_BODY,
    /* 0x03 */ SEAGULL_LIMB_LEFT_WING_ROOT,
    /* 0x04 */ SEAGULL_LIMB_LEFT_WING_WRAPPER,
    /* 0x05 */ SEAGULL_LIMB_LEFT_WING_START,
    /* 0x06 */ SEAGULL_LIMB_LEFT_WING_END,
    /* 0x07 */ SEAGULL_LIMB_RIGHT_WING_ROOT,
    /* 0x08 */ SEAGULL_LIMB_RIGHT_WING_WRAPPER,
    /* 0x09 */ SEAGULL_LIMB_RIGHT_WING_START,
    /* 0x0A */ SEAGULL_LIMB_RIGHT_WING_END,
    /* 0x0B */ SEAGULL_LIMB_RIGHT_WING_MAX
} ObjectTanron4Limb;

#endif // OBJECTS_OBJECT_TANRON4_H
