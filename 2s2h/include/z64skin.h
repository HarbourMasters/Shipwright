#ifndef Z64SKIN_H
#define Z64SKIN_H

#include "z64animation.h"

struct Actor;
struct GraphicsContext;
struct GameState;
struct PlayState;

/**
 * Holds a compact version of a vertex used in the Skin system (doesn't has the x, y, z positions or the flag member)
 * It is used to initialise the Vtx used by an animated limb
 */
typedef struct {
    /* 0x0 */ u16 index;
    /* 0x2 */ s16 s; // s and t are texture coordinates (also known as u and v)
    /* 0x4 */ s16 t;
    /* 0x6 */ s8 normX;
    /* 0x7 */ s8 normY;
    /* 0x8 */ s8 normZ;
    /* 0x9 */ u8 alpha;
} SkinVertex; // size = 0xA

/**
 * Describes a position displacement and a scale to be applied to a limb at index `limbIndex`
 */
typedef struct {
    /* 0x0 */ u8 limbIndex;
    /* 0x2 */ s16 x;
    /* 0x4 */ s16 y;
    /* 0x6 */ s16 z;
    /* 0x8 */ u8 scale;
} SkinTransformation; // size = 0xA

typedef struct {
    /* 0x0 */ u16 vtxCount; // number of vertices in this modif entry
    /* 0x2 */ u16 transformCount;
    /* 0x4 */ u16 unk_04; // index of limbTransformations
    /* 0x8 */ SkinVertex* skinVertices;
    /* 0xC */ SkinTransformation* limbTransformations;
} SkinLimbModif; // size = 0x10

typedef struct {
    /* 0x0 */ u16 totalVtxCount; // total vertex count for all modif entries
    /* 0x2 */ u16 limbModifCount; // count of limbModifCount
    /* 0x4 */ SkinLimbModif* limbModifications;
    /* 0x8 */ Gfx* dlist;
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
    /* 0x0 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
    /* 0x6 */ u8 child;
    /* 0x7 */ u8 sibling;
    /* 0x8 */ s32 segmentType; // Type of data contained in segment
    /* 0xC */ void* segment; // Gfx* if segmentType is SKIN_LIMB_TYPE_NORMAL, SkinAnimatedLimbData if segmentType is SKIN_LIMB_TYPE_ANIMATED, NULL otherwise
} SkinLimb; // size = 0x10

typedef struct {
    /* 0x0 */ u8 index; // alternates every draw cycle
    /* 0x4 */ Vtx* buf[2]; // number of vertices in buffer determined by `totalVtxCount`
} SkinLimbVtx; // size = 0xC

typedef struct {
    /* 0x00 */ SkeletonHeader* skeletonHeader;
    /* 0x04 */ MtxF mtx;
    /* 0x44 */ s32 limbCount;
    /* 0x48 */ SkinLimbVtx* vtxTable; // double buffered list of vertices for each limb
    /* 0x4C */ SkelAnime skelAnime;
} Skin; // size = 0x90

typedef void (*SkinPostDraw)(struct Actor* thisx, struct PlayState* play, Skin* skin);
typedef s32 (*SkinOverrideLimbDraw)(struct Actor* thisx, struct PlayState* play, s32 limbIndex, Skin* skin);

#define SKIN_DRAW_FLAG_CUSTOM_TRANSFORMS (1 << 0)
#define SKIN_DRAW_FLAG_CUSTOM_MATRIX     (1 << 1)


void Skin_UpdateVertices(MtxF* mtx, SkinVertex* skinVertices, SkinLimbModif* modifEntry, Vtx* vtxBuf, Vec3f* pos);
void Skin_ApplyLimbModifications(struct GraphicsContext* gfxCtx, Skin* skin, s32 limbIndex, s32 arg3);
void Skin_DrawAnimatedLimb(struct GraphicsContext* gfxCtx, Skin* skin, s32 limbIndex, s32 arg3, s32 drawFlags);
void Skin_DrawLimb(struct GraphicsContext* gfxCtx, Skin* skin, s32 limbIndex, Gfx* dListOverride, s32 drawFlags);
void func_80138228(struct Actor* actor, struct PlayState* play, Skin* skin, SkinPostDraw postDraw, s32 setTranslation);
void func_80138258(struct Actor* actor, struct PlayState* play, Skin* skin, SkinPostDraw postDraw, SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation);
void func_8013828C(struct Actor* actor, struct PlayState* play, Skin* skin, SkinPostDraw postDraw, SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation, s32 arg6);
void func_801382C4(struct Actor* actor, struct PlayState* play, Skin* skin, SkinPostDraw postDraw, SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation, s32 arg6, s32 drawFlags);
void Skin_GetLimbPos(Skin* skin, s32 limbIndex, Vec3f* offset, Vec3f* dst);
void Skin_GetVertexPos(Skin* skin, s32 limbIndex, s32 vtxIndex, Vec3f* dst);

void Skin_Setup(Skin* skin);
void Skin_InitAnimatedLimb(struct GameState* gameState, Skin* skin, s32 limbIndex);
void Skin_Init(struct GameState* gameState, Skin* skin, SkeletonHeader* skeletonHeader, AnimationHeader* animationHeader);
void Skin_Free(struct GameState* gameState, Skin* skin);
s32 func_801387D4(Skin* skin, SkinLimb** skeleton, MtxF* limbMatrices, u8 parentIndex, u8 limbIndex);
s32 Skin_ApplyAnimTransformations(Skin* skin, MtxF* limbMatrices, struct Actor* actor, s32 setTranslation);

#endif
