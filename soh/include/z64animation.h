#ifndef Z64_ANIMATION_H
#define Z64_ANIMATION_H

#include "ultra64.h"
#include "z64dma.h"
#include "z64math.h"

struct PlayState;
struct Actor;
struct SkelAnime;

#define LINK_ANIMATION_OFFSET(addr, offset) \
    (((uintptr_t)_link_animetionSegmentRomStart) + ((uintptr_t)addr) - ((uintptr_t)_link_animetionSegmentStart) + ((uintptr_t)offset))
#define LIMB_DONE 0xFF
#define ANIMATION_ENTRY_MAX 50
#define ANIM_FLAG_UPDATEY (1 << 1)
#define ANIM_FLAG_NOMOVE (1 << 4)

typedef enum {
    /* 0 */ ANIMMODE_LOOP,
    /* 1 */ ANIMMODE_LOOP_INTERP,
    /* 2 */ ANIMMODE_ONCE,
    /* 3 */ ANIMMODE_ONCE_INTERP,
    /* 4 */ ANIMMODE_LOOP_PARTIAL,
    /* 5 */ ANIMMODE_LOOP_PARTIAL_INTERP
} AnimationMode;

typedef enum { 
    /* -1 */ ANIMTAPER_DECEL = -1, 
    /*  0 */ ANIMTAPER_NONE, 
    /*  1 */ ANIMTAPER_ACCEL
} AnimationTapers;

typedef struct {
    /* 0x00 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
    /* 0x06 */ u8 child;
    /* 0x07 */ u8 sibling;
    /* 0x08 */ Gfx* dList;
} StandardLimb; // size = 0xC

typedef struct {
    /* 0x00 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
    /* 0x06 */ u8 child;
    /* 0x07 */ u8 sibling;
    /* 0x08 */ Gfx* dLists[2]; // Near and far
} LodLimb; // size = 0x10

typedef struct LegacyLimb {
    /* 0x000 */ Gfx* dList;
    /* 0x004 */ Vec3f trans;
    /* 0x010 */ Vec3s rot;
    /* 0x018 */ struct LegacyLimb* sibling;
    /* 0x01C */ struct LegacyLimb* child;
} LegacyLimb; // size = 0x20

// Model has limbs with only rigid meshes
typedef struct {
    /* 0x00 */ void** segment;
    /* 0x04 */ u8 limbCount;
} SkeletonHeader; // size = 0x8

// Model has limbs with flexible meshes
typedef struct {
    /* 0x00 */ SkeletonHeader sh;
    /* 0x08 */ u8 dListCount;
} FlexSkeletonHeader; // size = 0xC

// Index into the frame data table. 
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 z;
} JointIndex; // size = 0x06

typedef struct {
    /* 0x00 */ s16 frameCount;
} AnimationHeaderCommon;

typedef struct {
    /* 0x00 */ AnimationHeaderCommon common;
    /* 0x04 */ void* segment;
} LinkAnimationHeader; // size = 0x8

typedef struct {
    /* 0x00 */ AnimationHeaderCommon common;
    /* 0x04 */ s16* frameData; // "tbl"
    /* 0x08 */ JointIndex* jointIndices; // "ref_tbl"
    /* 0x0C */ u16 staticIndexMax;
} AnimationHeader; // size = 0x10

// Unused
typedef struct {
    /* 0x00 */ s16 xMax;
    /* 0x02 */ s16 x;
    /* 0x04 */ s16 yMax;
    /* 0x06 */ s16 y;
    /* 0x08 */ s16 zMax;
    /* 0x0A */ s16 z;
} JointKey; // size = 0x0C

// Unused
typedef struct {
    /* 0x00 */ s16 frameCount;
    /* 0x02 */ s16 limbCount;
    /* 0x04 */ s16* frameData;
    /* 0x08 */ JointKey* jointKey;
} LegacyAnimationHeader; // size = 0xC

typedef s32 (*OverrideLimbDrawOpa)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                   void*);

typedef void (*PostLimbDrawOpa)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void*);

typedef s32 (*OverrideLimbDraw)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void*, Gfx** gfx);

typedef void (*PostLimbDraw)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void*, Gfx** gfx);

typedef enum {
    ANIMENTRY_LOADFRAME,
    ANIMENTRY_COPYALL,
    ANIMENTRY_INTERP,
    ANIMENTRY_COPYTRUE,
    ANIMENTRY_COPYFALSE,
    ANIMENTRY_MOVEACTOR
} AnimationType;

typedef struct {
    /* 0x000 */ DmaRequest req;
    /* 0x020 */ OSMesgQueue msgQueue;
    /* 0x038 */ OSMesg msg;
} AnimEntryLoadFrame; // size = 0x3C

typedef struct {
    /* 0x000 */ u8 queueFlag;
    /* 0x001 */ u8 vecCount;
    /* 0x004 */ Vec3s* dst;
    /* 0x008 */ Vec3s* src;
} AnimEntryCopyAll; // size = 0xC

typedef struct {
    /* 0x000 */ u8 queueFlag;
    /* 0x001 */ u8 vecCount;
    /* 0x004 */ Vec3s* base;
    /* 0x008 */ Vec3s* mod;
    /* 0x00C */ f32 weight;
} AnimEntryInterp; // size = 0x10

typedef struct {
    /* 0x000 */ u8 queueFlag;
    /* 0x001 */ u8 vecCount;
    /* 0x004 */ Vec3s* dst;
    /* 0x008 */ Vec3s* src;
    /* 0x00C */ u8* copyFlag;
} AnimEntryCopyTrue; // size = 0x10

typedef struct {
    /* 0x000 */ u8 queueFlag;
    /* 0x001 */ u8 vecCount;
    /* 0x004 */ Vec3s* dst;
    /* 0x008 */ Vec3s* src;
    /* 0x00C */ u8* copyFlag;
} AnimEntryCopyFalse; // size = 0x10

typedef struct {
    /* 0x000 */ struct Actor* actor;
    /* 0x004 */ struct SkelAnime* skelAnime;
    /* 0x008 */ f32 unk_08;
} AnimEntryMoveActor; // size = 0xC

typedef union {
    AnimEntryLoadFrame load;
    AnimEntryCopyAll copy;
    AnimEntryInterp interp;
    AnimEntryCopyTrue copy1;
    AnimEntryCopyFalse copy0;
    AnimEntryMoveActor move;
} AnimationEntryData; // size = 0x3C

typedef struct {
    /* 0x00 */ u8 type;
    /* 0x04 */ AnimationEntryData data;
} AnimationEntry; // size = 0x40

typedef struct AnimationContext {
    s16 animationCount;
    AnimationEntry entries[ANIMATION_ENTRY_MAX];
} AnimationContext; // size = 0xC84

typedef void (*AnimationEntryCallback)(struct PlayState* play, AnimationEntryData* data);

// fcurve_skelanime structs
typedef struct {
    /* 0x0000 */ u16 unk_00; // appears to be flags
    /* 0x0002 */ s16 unk_02;
    /* 0x0004 */ s16 unk_04;
    /* 0x0006 */ s16 unk_06;
    /* 0x0008 */ f32 unk_08;
} TransformData; // size = 0xC

typedef struct {
    /* 0x0000 */ u8* refIndex;
    /* 0x0004 */ TransformData* transformData;
    /* 0x0008 */ s16* copyValues;
    /* 0x000C */ s16 unk_0C;
    /* 0x000E */ s16 unk_0E;
} TransformUpdateIndex; // size = 0x10

typedef struct {
    /* 0x0000 */ u8 firstChildIdx;
    /* 0x0001 */ u8 nextLimbIdx;
    /* 0x0004 */ Gfx* dList[2];
} SkelCurveLimb; // size = 0xC

typedef struct {
    /* 0x0000 */ SkelCurveLimb** limbs;
    /* 0x0004 */ u8 limbCount;
} SkelCurveLimbList; // size = 0x8

typedef struct {
    /* 0x0000 */ Vec3s scale;
    /* 0x0006 */ Vec3s rot;
    /* 0x000C */ Vec3s pos;
} LimbTransform; // size = 0x12

typedef struct {
    /* 0x0000 */ u8 limbCount;
    /* 0x0004 */ SkelCurveLimb** limbList;
    /* 0x0008 */ TransformUpdateIndex* transUpdIdx;
    /* 0x000C */ f32 unk_0C; // seems to be unused
    /* 0x0010 */ f32 animFinalFrame;
    /* 0x0014 */ f32 animSpeed;
    /* 0x0018 */ f32 animCurFrame;
    /* 0x001C */ LimbTransform* transforms;
} SkelAnimeCurve; // size = 0x20

typedef s32 (*OverrideCurveLimbDraw)(struct PlayState* play, SkelAnimeCurve* skelCurve, s32 limbIndex, void*);
typedef void (*PostCurveLimbDraw)(struct PlayState* play, SkelAnimeCurve* skelCurve, s32 limbIndex, void*);

typedef s32 (*AnimUpdateFunc)();

typedef struct SkelAnime {
    /* 0x00 */ u8 limbCount;      // Number of limbs in the skeleton
    /* 0x01 */ u8 mode;           // See `AnimationMode`
    /* 0x02 */ u8 dListCount;     // Number of display lists in a flexible skeleton
    /* 0x03 */ s8 taper;          // Tapering to use when morphing between animations. Only used by Door_Warp1.
    /* 0x04 */ void** skeleton;   // An array of pointers to limbs. Can be StandardLimb, LodLimb, or SkinLimb.
    /* 0x08 */ void* animation;   // Can be an AnimationHeader or LinkAnimationHeader.
    /* 0x0C */ f32 startFrame;    // In mode ANIMMODE_LOOP_PARTIAL*, start of partial loop.
    /* 0x10 */ f32 endFrame;      // In mode ANIMMODE_ONCE*, Update returns true when curFrame is equal to this. In mode ANIMMODE_LOOP_PARTIAL*, end of partial loop.
    /* 0x14 */ f32 animLength;    // Total number of frames in the current animation.
    /* 0x18 */ f32 curFrame;      // Current frame in the animation
    /* 0x1C */ f32 playSpeed;     // Multiplied by R_UPDATE_RATE / 3 to get the animation's frame rate.
    /* 0x20 */ Vec3s* jointTable; // Current translation of model and rotations of all limbs
    /* 0x24 */ Vec3s* morphTable; // Table of values used to morph between animations
    /* 0x28 */ f32 morphWeight;   // Weight of the current animation morph as a fraction in [0,1]
    /* 0x2C */ f32 morphRate;     // Reciprocal of the number of frames in the morph
    /* 0x30 */ s32 (*update)();   // Can be Loop, Partial loop, Play once, Morph, or Tapered morph. Link only has Loop, Play once, and Morph.
    /* 0x34 */ s8 initFlags;      // Flags used when initializing Link's skeleton
    /* 0x35 */ u8 moveFlags;      // Flags used for animations that move the actor in worldspace.
    /* 0x36 */ s16 prevRot;       // Previous rotation in worldspace.
    /* 0x38 */ Vec3s prevTransl;  // Previous modelspace translation.
    /* 0x3E */ Vec3s baseTransl;  // Base modelspace translation.
} SkelAnime; // size = 0x44

#endif
