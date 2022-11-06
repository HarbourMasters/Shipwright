#ifndef Z64_SKIN_H
#define Z64_SKIN_H

#include "z64animation.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Holds a compact version of a vertex used in the Skin system
 * It is used to initialise the Vtx used by an animated limb
 */
typedef struct {
    /* 0x00 */ u16 index;
    /* 0x02 */ s16 s; // s and t are texture coordinates (also known as u and v)
    /* 0x04 */ s16 t;
    /* 0x06 */ s8 normX;
    /* 0x07 */ s8 normY;
    /* 0x08 */ s8 normZ;
    /* 0x09 */ u8 alpha;
} SkinVertex; // size = 0xA

/**
 * Describes a position displacement and a scale to be applied to a limb at index `limbIndex`
 */
typedef struct {
    /* 0x00 */ u8 limbIndex;
    /* 0x02 */ s16 x;
    /* 0x04 */ s16 y;
    /* 0x06 */ s16 z;
    /* 0x08 */ u8 scale;
} SkinTransformation; // size = 0xA

typedef struct {
    /* 0x00 */ u16 vtxCount; // number of vertices in this modif entry
    /* 0x02 */ u16 transformCount;
    /* 0x04 */ u16 unk_4; // index of limbTransformations?
    /* 0x08 */ SkinVertex* skinVertices;
    /* 0x0C */ SkinTransformation* limbTransformations;
} SkinLimbModif; // size = 0x10

typedef struct {
    /* 0x00 */ u16 totalVtxCount; // total vertex count for all modif entries
    /* 0x02 */ u16 limbModifCount;
    /* 0x04 */ SkinLimbModif* limbModifications;
    /* 0x08 */ Gfx* dlist;
} SkinAnimatedLimbData; // size = 0xC

// ZAPD compatibility typedefs
// TODO: Remove when ZAPD adds support for them
typedef SkinVertex Struct_800A57C0;
typedef SkinTransformation Struct_800A598C_2;
typedef SkinAnimatedLimbData Struct_800A5E28;
typedef SkinLimbModif Struct_800A598C;

#define SKIN_LIMB_TYPE_ANIMATED 4
#define SKIN_LIMB_TYPE_NORMAL 11

typedef struct {
    /* 0x00 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
    /* 0x06 */ u8 child;
    /* 0x07 */ u8 sibling;
    /* 0x08 */ s32 segmentType; // Type of data contained in segment
    /* 0x0C */ void* segment; // Gfx* if segmentType is SKIN_LIMB_TYPE_NORMAL, SkinAnimatedLimbData* if segmentType is SKIN_LIMB_TYPE_ANIMATED, NULL otherwise
} SkinLimb; // size = 0x10

typedef struct {
    /* 0x000 */ u8 index; // alternates every draw cycle
    /* 0x004 */ Vtx* buf[2]; // number of vertices in buffer determined by `totalVtxCount`
} SkinLimbVtx; // size = 0xC

typedef struct {
    /* 0x000 */ SkeletonHeader* skeletonHeader;
    /* 0x004 */ MtxF mtx;
    /* 0x044 */ s32 limbCount;
    /* 0x048 */ SkinLimbVtx* vtxTable; // double buffered list of vertices for each limb
    /* 0x04C */ SkelAnime skelAnime;
} Skin; // size = 0x90

typedef void (*SkinPostDraw)(struct Actor*, struct PlayState*, Skin*);
typedef s32 (*SkinOverrideLimbDraw)(struct Actor*, struct PlayState*, s32, Skin*);

#define SKIN_DRAW_FLAG_CUSTOM_TRANSFORMS (1 << 0)
#define SKIN_DRAW_FLAG_CUSTOM_MATRIX     (1 << 1)

#define SKIN_TRANSFORM_IS_FHG 0x23

#ifdef __cplusplus
};
#endif

#endif
