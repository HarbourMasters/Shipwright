#ifndef OBJECTS_OBJECT_VM_H
#define OBJECTS_OBJECT_VM_H 1

#include "align_asset_macro.h"

#define dgBeamosAnim "__OTR__objects/object_vm/gBeamosAnim"
static const ALIGN_ASSET(2) char gBeamosAnim[] = dgBeamosAnim;

#define dgBeamosBodyDL "__OTR__objects/object_vm/gBeamosBodyDL"
static const ALIGN_ASSET(2) char gBeamosBodyDL[] = dgBeamosBodyDL;

#define dgBeamosTopEyelidDL "__OTR__objects/object_vm/gBeamosTopEyelidDL"
static const ALIGN_ASSET(2) char gBeamosTopEyelidDL[] = dgBeamosTopEyelidDL;

#define dgBeamosBottomEyelidDL "__OTR__objects/object_vm/gBeamosBottomEyelidDL"
static const ALIGN_ASSET(2) char gBeamosBottomEyelidDL[] = dgBeamosBottomEyelidDL;

#define dgBeamosEyeDL "__OTR__objects/object_vm/gBeamosEyeDL"
static const ALIGN_ASSET(2) char gBeamosEyeDL[] = dgBeamosEyeDL;

#define dgBeamosLaserDL "__OTR__objects/object_vm/gBeamosLaserDL"
static const ALIGN_ASSET(2) char gBeamosLaserDL[] = dgBeamosLaserDL;

#define dgBeamosBodyGradientTex "__OTR__objects/object_vm/gBeamosBodyGradientTex"
static const ALIGN_ASSET(2) char gBeamosBodyGradientTex[] = dgBeamosBodyGradientTex;

#define dgBeamosGrayMetalTex "__OTR__objects/object_vm/gBeamosGrayMetalTex"
static const ALIGN_ASSET(2) char gBeamosGrayMetalTex[] = dgBeamosGrayMetalTex;

#define dgBeamosBoltAndMetalTex "__OTR__objects/object_vm/gBeamosBoltAndMetalTex"
static const ALIGN_ASSET(2) char gBeamosBoltAndMetalTex[] = dgBeamosBoltAndMetalTex;

#define dgBeamosInnerEyeLidTex "__OTR__objects/object_vm/gBeamosInnerEyeLidTex"
static const ALIGN_ASSET(2) char gBeamosInnerEyeLidTex[] = dgBeamosInnerEyeLidTex;

#define dgBeamosEyeOutlineTex "__OTR__objects/object_vm/gBeamosEyeOutlineTex"
static const ALIGN_ASSET(2) char gBeamosEyeOutlineTex[] = dgBeamosEyeOutlineTex;

#define dgBeamosEyeTex "__OTR__objects/object_vm/gBeamosEyeTex"
static const ALIGN_ASSET(2) char gBeamosEyeTex[] = dgBeamosEyeTex;

#define dgBeamosBodyTex "__OTR__objects/object_vm/gBeamosBodyTex"
static const ALIGN_ASSET(2) char gBeamosBodyTex[] = dgBeamosBodyTex;

#define dgBeamosLaserTex "__OTR__objects/object_vm/gBeamosLaserTex"
static const ALIGN_ASSET(2) char gBeamosLaserTex[] = dgBeamosLaserTex;

#define dgBeamosTeethTex "__OTR__objects/object_vm/gBeamosTeethTex"
static const ALIGN_ASSET(2) char gBeamosTeethTex[] = dgBeamosTeethTex;

#define dgBeamosSkel "__OTR__objects/object_vm/gBeamosSkel"
static const ALIGN_ASSET(2) char gBeamosSkel[] = dgBeamosSkel;

typedef enum BeamosLimb {
    /* 0x00 */ BEAMOS_LIMB_NONE,
    /* 0x01 */ BEAMOS_LIMB_ROOT,
    /* 0x02 */ BEAMOS_LIMB_HEAD_ROOT,
    /* 0x03 */ BEAMOS_LIMB_TOP_EYELID_ROOT,
    /* 0x04 */ BEAMOS_LIMB_TOP_EYELID_WRAPPER,
    /* 0x05 */ BEAMOS_LIMB_TOP_EYELID,
    /* 0x06 */ BEAMOS_LIMB_BOTTOM_EYELID_ROOT,
    /* 0x07 */ BEAMOS_LIMB_BOTTOM_EYELID_WRAPPER,
    /* 0x08 */ BEAMOS_LIMB_BOTTEM_EYELID,
    /* 0x09 */ BEAMOS_LIMB_EYE,
    /* 0x0A */ BEAMOS_LIMB_BODY,
    /* 0x0B */ BEAMOS_LIMB_MAX
} BeamosLimb;

#endif // OBJECTS_OBJECT_VM_H
