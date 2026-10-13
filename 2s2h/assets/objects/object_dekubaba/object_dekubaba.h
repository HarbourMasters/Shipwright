#ifndef OBJECTS_OBJECT_DEKUBABA_H
#define OBJECTS_OBJECT_DEKUBABA_H 1

#include "align_asset_macro.h"

#define dgDekuBabaPauseChompAnim "__OTR__objects/object_dekubaba/gDekuBabaPauseChompAnim"
static const ALIGN_ASSET(2) char gDekuBabaPauseChompAnim[] = dgDekuBabaPauseChompAnim;

#define dgDekuBabaFastChompAnim "__OTR__objects/object_dekubaba/gDekuBabaFastChompAnim"
static const ALIGN_ASSET(2) char gDekuBabaFastChompAnim[] = dgDekuBabaFastChompAnim;

#define dgDekuBabaLowerJawDL "__OTR__objects/object_dekubaba/gDekuBabaLowerJawDL"
static const ALIGN_ASSET(2) char gDekuBabaLowerJawDL[] = dgDekuBabaLowerJawDL;

#define dgDekuBabaUpperJawDL "__OTR__objects/object_dekubaba/gDekuBabaUpperJawDL"
static const ALIGN_ASSET(2) char gDekuBabaUpperJawDL[] = dgDekuBabaUpperJawDL;

#define dgDekuBabaBaseLeavesDL "__OTR__objects/object_dekubaba/gDekuBabaBaseLeavesDL"
static const ALIGN_ASSET(2) char gDekuBabaBaseLeavesDL[] = dgDekuBabaBaseLeavesDL;

#define dgDekuBabaStemTopDL "__OTR__objects/object_dekubaba/gDekuBabaStemTopDL"
static const ALIGN_ASSET(2) char gDekuBabaStemTopDL[] = dgDekuBabaStemTopDL;

#define dgDekuBabaStemMiddleDL "__OTR__objects/object_dekubaba/gDekuBabaStemMiddleDL"
static const ALIGN_ASSET(2) char gDekuBabaStemMiddleDL[] = dgDekuBabaStemMiddleDL;

#define dgDekuBabaStemBaseDL "__OTR__objects/object_dekubaba/gDekuBabaStemBaseDL"
static const ALIGN_ASSET(2) char gDekuBabaStemBaseDL[] = dgDekuBabaStemBaseDL;

#define dgDekuBabaJawOuterTex "__OTR__objects/object_dekubaba/gDekuBabaJawOuterTex"
static const ALIGN_ASSET(2) char gDekuBabaJawOuterTex[] = dgDekuBabaJawOuterTex;

#define dgDekuBabaJawInnerTex "__OTR__objects/object_dekubaba/gDekuBabaJawInnerTex"
static const ALIGN_ASSET(2) char gDekuBabaJawInnerTex[] = dgDekuBabaJawInnerTex;

#define dgDekuBabaLeafTex "__OTR__objects/object_dekubaba/gDekuBabaLeafTex"
static const ALIGN_ASSET(2) char gDekuBabaLeafTex[] = dgDekuBabaLeafTex;

#define dgDekuBabaBulbTex "__OTR__objects/object_dekubaba/gDekuBabaBulbTex"
static const ALIGN_ASSET(2) char gDekuBabaBulbTex[] = dgDekuBabaBulbTex;

#define dgDekuBabaStemTex "__OTR__objects/object_dekubaba/gDekuBabaStemTex"
static const ALIGN_ASSET(2) char gDekuBabaStemTex[] = dgDekuBabaStemTex;

#define dgDekuBabaSkel "__OTR__objects/object_dekubaba/gDekuBabaSkel"
static const ALIGN_ASSET(2) char gDekuBabaSkel[] = dgDekuBabaSkel;

#define dgDekuBabaStickDropDL "__OTR__objects/object_dekubaba/gDekuBabaStickDropDL"
static const ALIGN_ASSET(2) char gDekuBabaStickDropDL[] = dgDekuBabaStickDropDL;

#define dgDekuBabaStickDropTex "__OTR__objects/object_dekubaba/gDekuBabaStickDropTex"
static const ALIGN_ASSET(2) char gDekuBabaStickDropTex[] = dgDekuBabaStickDropTex;

typedef enum DekuBabaLimb {
    /* 0x00 */ DEKUBABA_LIMB_NONE,
    /* 0x01 */ DEKUBABA_LIMB_ROOT,
    /* 0x02 */ DEKUBABA_LIMB_UPPER_JAW_ROOT,
    /* 0x03 */ DEKUBABA_LIMB_UPPER_JAW_WRAPPER,
    /* 0x04 */ DEKUBABA_LIMB_UPPER_JAW,
    /* 0x05 */ DEKUBABA_LIMB_LOWER_JAW_ROOT,
    /* 0x06 */ DEKUBABA_LIMB_LOWER_JAW_WRAPPER,
    /* 0x07 */ DEKUBABA_LIMB_LOWER_JAW,
    /* 0x08 */ DEKUBABA_LIMB_MAX
} DekuBabaLimb;

#endif // OBJECTS_OBJECT_DEKUBABA_H
