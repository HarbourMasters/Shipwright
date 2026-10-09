#ifndef Z64ANIMATION_H
#define Z64ANIMATION_H

#include "PR/ultratypes.h"
#include "PR/gbi.h"
#include "z64dma.h"
#include "z64math.h"

struct GameState;
struct PlayState;
struct Actor;
struct SkelAnime;
struct PlayerAnimationFrame;

// for indexing `jointTable[]` and `morphTable[]`
#define LIMB_ROOT_POS 0 // Translation/Offset of the root limb
#define LIMB_ROOT_ROT 1 // Rotation of the root limb

#define LIMB_DONE 0xFF
#define BODYPART_NONE -1

#define ANIM_FLAG_1         (1 << 0)
#define ANIM_FLAG_UPDATE_Y  (1 << 1)
#define ANIM_FLAG_4         (1 << 2)

// When this flag is set, ActorMovement tasks will be queued.
//
// Note that individual actors are responsible for implementing the functionality of this flag.
// In practice, Player is the only actor who implements this flag.
// It is possible to bypass the need for this flag by manually calling `AnimTaskQueue_AddActorMovement`
// when it is needed.
#define ANIM_FLAG_ENABLE_MOVEMENT (1 << 3)

#define ANIM_FLAG_NOMOVE    (1 << 4)
#define ANIM_FLAG_80        (1 << 7)
#define ANIM_FLAG_100       (1 << 8)
#define ANIM_FLAG_200       (1 << 9)

typedef enum AnimationMode {
    /* 0 */ ANIMMODE_LOOP,
    /* 1 */ ANIMMODE_LOOP_INTERP,
    /* 2 */ ANIMMODE_ONCE,
    /* 3 */ ANIMMODE_ONCE_INTERP,
    /* 4 */ ANIMMODE_LOOP_PARTIAL,
    /* 5 */ ANIMMODE_LOOP_PARTIAL_INTERP
} AnimationMode;

typedef enum AnimationTaper {
    /* -1 */ ANIMTAPER_DECEL = -1,
    /*  0 */ ANIMTAPER_NONE,
    /*  1 */ ANIMTAPER_ACCEL
} AnimationTaper;

typedef struct {
    /* 0x0 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
    /* 0x6 */ u8 child;       // The first child's index into the limb table.
    /* 0x7 */ u8 sibling;     // The parent limb's next limb index into the limb table.
    /* 0x8 */ Gfx* dList;     // Display lists for the limb. Index 0 is the normal display list, index 1 is the
                               // far model display list.
} StandardLimb; // size = 0xC

// Model has limbs with only rigid meshes
typedef struct {
    /* 0x0 */ Vec3s jointPos; // Root is position in model space, children are relative to parent
    /* 0x6 */ u8 child;
    /* 0x7 */ u8 sibling;
    /* 0x8 */ Gfx* dLists[2]; // Near and far
} LodLimb; // size = 0x10

// Model has limbs with only rigid meshes
typedef struct {
    /* 0x0 */ void** segment;
    /* 0x4 */ u8 limbCount;
} SkeletonHeader; // size = 0x8

// Model has limbs with flexible meshes
typedef struct {
    /* 0x0 */ SkeletonHeader sh;
    /* 0x8 */ u8 dListCount;
} FlexSkeletonHeader; // size = 0xC

// Index into the frame data table. 
typedef struct {
    /* 0x0 */ u16 x;
    /* 0x2 */ u16 y;
    /* 0x4 */ u16 z;
} JointIndex; // size = 0x6

typedef struct {
    /* 0x0 */ s16 frameCount;
} AnimationHeaderCommon; // size = 0x2

typedef struct {
    /* 0x0 */ AnimationHeaderCommon common;
    /* 0x4 */ s16* frameData; // referenced as tbl
    /* 0x8 */ JointIndex* jointIndices; // referenced as ref_tbl
    /* 0xC */ u16 staticIndexMax;
} AnimationHeader; // size = 0x10

typedef enum AnimTaskType {
    /* 0 */ ANIMTASK_LOAD_PLAYER_FRAME,
    /* 1 */ ANIMTASK_COPY,
    /* 2 */ ANIMTASK_INTERP,
    /* 3 */ ANIMTASK_COPY_USING_MAP,
    /* 4 */ ANIMTASK_COPY_USING_MAP_INVERTED,
    /* 5 */ ANIMTASK_ACTOR_MOVE,
    /* 6 */ ANIMTASK_MAX
} AnimTaskType;

typedef struct {
    /* 0x00 */ DmaRequest req;
    /* 0x20 */ OSMesgQueue msgQueue;
    /* 0x38 */ OSMesg msg[1];
} AnimTaskLoadPlayerFrame; // size = 0x3C

typedef struct {
    /* 0x0 */ u8 group;
    /* 0x1 */ u8 vecCount;
    /* 0x4 */ Vec3s* dest;
    /* 0x8 */ Vec3s* src;
} AnimTaskCopy; // size = 0xC

typedef struct {
    /* 0x0 */ u8 group;
    /* 0x1 */ u8 vecCount;
    /* 0x4 */ Vec3s* base;
    /* 0x8 */ Vec3s* mod;
    /* 0xC */ f32 weight;
} AnimTaskInterp; // size = 0x10

typedef struct {
    /* 0x0 */ u8 group;
    /* 0x1 */ u8 vecCount;
    /* 0x4 */ Vec3s* dest;
    /* 0x8 */ Vec3s* src;
    /* 0xC */ u8* limbCopyMap;
} AnimTaskCopyUsingMap; // size = 0x10

typedef struct {
    /* 0x0 */ u8 group;
    /* 0x1 */ u8 vecCount;
    /* 0x4 */ Vec3s* dest;
    /* 0x8 */ Vec3s* src;
    /* 0xC */ u8* limbCopyMap;
} AnimTaskCopyUsingMapInverted; // size = 0x10

typedef struct {
    /* 0x0 */ struct Actor* actor;
    /* 0x4 */ struct SkelAnime* skelAnime;
    /* 0x8 */ f32 diffScale;
} AnimTaskActorMovement; // size = 0xC

typedef union {
    AnimTaskLoadPlayerFrame loadPlayerFrame;
    AnimTaskCopy copy;
    AnimTaskInterp interp;
    AnimTaskCopyUsingMap copyUsingMap;
    AnimTaskCopyUsingMapInverted copyUsingMapInverted;
    AnimTaskActorMovement actorMovement;
} AnimTaskData; // size = 0x3C

typedef struct {
    /* 0x0 */ u8 type;
    /* 0x4 */ AnimTaskData data;
} AnimTask; // size = 0x40

#define ANIM_TASK_QUEUE_MAX 50

typedef struct AnimTaskQueue {
    /* 0x0 */ s16 count;
    /* 0x4 */ AnimTask tasks[ANIM_TASK_QUEUE_MAX];
} AnimTaskQueue; // size = 0xC84

typedef struct {
    /* 0x0 */ AnimationHeaderCommon common;
    /* 0x4 */ union {
                void* segmentVoid;
                struct PlayerAnimationFrame* linkAnimSegment;
            };
} PlayerAnimationHeader; // size = 0x8

typedef struct SkelAnime {
    /* 0x00 */ u8 limbCount;      // Number of limbs in the skeleton
    /* 0x01 */ u8 mode;           // 0: loop, 2: play once, 4: partial loop. +1 to interpolate between frames.
    /* 0x02 */ u8 dListCount;     // Number of display lists in a flexible skeleton
    /* 0x03 */ s8 taper;          // Tapering to use when morphing between animations. Only used by Door_Warp1.
    /* 0x04 */ void** skeleton;   // An array of pointers to limbs. Can be StandardLimb, LodLimb, or SkinLimb.
    /* 0x08 */ void* animation;   // Can be an AnimationHeader or PlayerAnimationHeader.
    /* 0x0C */ f32 startFrame;    // In mode 4, start of partial loop.
    /* 0x10 */ f32 endFrame;      // In mode 2, Update returns true when curFrame is equal to this. In mode 4, end of partial loop.
    /* 0x14 */ f32 animLength;    // Total number of frames in the current animation's file.
    /* 0x18 */ f32 curFrame;      // Current frame in the animation
    /* 0x1C */ f32 playSpeed;     // Multiplied by R_UPDATE_RATE / 3 to get the animation's frame rate.
    /* 0x20 */ Vec3s* jointTable; // Current translation of model and rotations of all limbs
    /* 0x24 */ Vec3s* morphTable; // Table of values used to morph between animations
    /* 0x28 */ f32 morphWeight;   // Weight of the current animation morph as a fraction in [0,1]
    /* 0x2C */ f32 morphRate;     // Reciprocal of the number of frames in the morph
    /* 0x30 */ union {
                    s32 (*normal)(struct SkelAnime*);// Can be Loop, Partial loop, Play once, Morph, or Tapered morph
                    s32 (*player)(struct PlayState*, struct SkelAnime*); // Loop, Play once, and Morph
                } update;
    /* 0x34 */ s8 initFlags;      // Flags used when initializing Player's skeleton
    /* 0x35 */ u8 movementFlags;  // Flags used for animations that move the actor in worldspace.
    /* 0x36 */ s16 prevYaw;       // Previous rotation in worldspace.
    /* 0x38 */ Vec3s prevTransl;  // Previous modelspace translation.
    /* 0x3E */ Vec3s baseTransl;  // Base modelspace translation.
} SkelAnime; // size = 0x44

typedef s32 (*OverrideLimbDrawOpa)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                   struct Actor* thisx);

typedef void (*PostLimbDrawOpa)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot,
                                struct Actor* thisx);

typedef s32 (*OverrideLimbDraw)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                struct Actor* thisx, Gfx** gfx);

typedef void (*PostLimbDraw)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot,
                             struct Actor* thisx, Gfx** gfx);

typedef s32 (*OverrideLimbDrawFlex)(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                    struct Actor* thisx);

typedef void (*PostLimbDrawFlex)(struct PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot,
                                 struct Actor* thisx);

typedef void (*TransformLimbDrawOpa)(struct PlayState* play, s32 limbIndex, struct Actor* thisx);

typedef void (*TransformLimbDraw)(struct PlayState* play, s32 limbIndex, struct Actor* thisx, Gfx** gfx);

typedef struct {
    /* 0x00 */ AnimationHeader* animation;
    /* 0x04 */ f32 playSpeed;
    /* 0x08 */ f32 startFrame;
    /* 0x0C */ f32 frameCount;
    /* 0x10 */ u8 mode;
    /* 0x14 */ f32 morphFrames;
} AnimationInfo; // size = 0x18

typedef struct {
    /* 0x0 */ AnimationHeader* animation;
    /* 0x4 */ f32 playSpeed;
    /* 0x8 */ s16 startFrame;
    /* 0xA */ s16 frameCount;
    /* 0xC */ u8 mode;
    /* 0xE */ s16 morphFrames;
} AnimationInfoS; // size = 0x10

typedef struct AnimationSpeedInfo {
    /* 0x0 */ AnimationHeader* animation;
    /* 0x4 */ f32 playSpeed;
    /* 0x8 */ u8 mode;
    /* 0xC */ f32 morphFrames;
} AnimationSpeedInfo; // size = 0x10

void SkelAnime_DrawLod(struct PlayState* play, void** skeleton, Vec3s* jointTable, OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, struct Actor* actor, s32 lod);
void SkelAnime_DrawFlexLod(struct PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, OverrideLimbDrawFlex overrideLimbDraw, PostLimbDrawFlex postLimbDraw, struct Actor* actor, s32 lod);
void SkelAnime_DrawOpa(struct PlayState* play, void** skeleton, Vec3s* jointTable, OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, struct Actor* actor);
void SkelAnime_DrawFlexOpa(struct PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, struct Actor* actor);
void SkelAnime_DrawTransformFlexOpa(struct PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, TransformLimbDrawOpa transformLimbDraw, struct Actor* actor);
s16 Animation_GetLastFrame(void* animation);
Gfx* SkelAnime_Draw(struct PlayState* play, void** skeleton, Vec3s* jointTable, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw, struct Actor* actor, Gfx* gfx);
Gfx* SkelAnime_DrawFlex(struct PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw, struct Actor* actor, Gfx* gfx);

void AnimTaskQueue_Reset(AnimTaskQueue* animTaskQueue);
void AnimTaskQueue_SetNextGroup(struct PlayState* play);
void AnimTaskQueue_DisableTransformTasksForGroup(struct PlayState* play);
void AnimTaskQueue_AddLoadPlayerFrame(struct PlayState* play, PlayerAnimationHeader* animation, s32 frame, s32 limbCount, Vec3s* frameTable);
void AnimTaskQueue_AddCopy(struct PlayState* play, s32 vecCount, Vec3s* dest, Vec3s* src);
void AnimTaskQueue_AddInterp(struct PlayState* play, s32 vecCount, Vec3s* base, Vec3s* mod, f32 weight);
void AnimTaskQueue_AddCopyUsingMap(struct PlayState* play, s32 vecCount, Vec3s* dest, Vec3s* src, u8* limbCopyMap);
void AnimTaskQueue_AddCopyUsingMapInverted(struct PlayState* play, s32 vecCount, Vec3s* dest, Vec3s* src, u8* limbCopyMap);
void AnimTaskQueue_AddActorMovement(struct PlayState* play, struct Actor* actor, SkelAnime* skelAnime, f32 moveDiffScale);
void AnimTaskQueue_Update(struct PlayState* play, AnimTaskQueue* animTaskQueue);

void SkelAnime_InitPlayer(struct PlayState* play, SkelAnime* skelAnime, FlexSkeletonHeader* skeletonHeaderSeg, PlayerAnimationHeader* animation, s32 flags, void* jointTableBuffer, void* morphTableBuffer, s32 limbBufCount);
void PlayerAnimation_SetUpdateFunction(SkelAnime* skelAnime);
s32 PlayerAnimation_Update(struct PlayState* play, SkelAnime* skelAnime);
void PlayerAnimation_AnimateFrame(struct PlayState* play, SkelAnime* skelAnime);
void Animation_SetMorph(struct PlayState* play, SkelAnime* skelAnime, f32 morphFrames);
void PlayerAnimation_Change(struct PlayState* play, SkelAnime* skelAnime, PlayerAnimationHeader* animation, f32 playSpeed, f32 startFrame, f32 endFrame, u8 mode, f32 morphFrames);
void PlayerAnimation_PlayOnce(struct PlayState* play, SkelAnime* skelAnime, PlayerAnimationHeader* animation);
void PlayerAnimation_PlayOnceSetSpeed(struct PlayState* play, SkelAnime* skelAnime, PlayerAnimationHeader* animation, f32 playSpeed);
void PlayerAnimation_PlayLoop(struct PlayState* play, SkelAnime* skelAnime, PlayerAnimationHeader* animation);
void PlayerAnimation_PlayLoopSetSpeed(struct PlayState* play, SkelAnime* skelAnime, PlayerAnimationHeader* animation, f32 playSpeed);
void PlayerAnimation_CopyJointToMorph(struct PlayState* play, SkelAnime* skelAnime);
void PlayerAnimation_CopyMorphToJoint(struct PlayState* play, SkelAnime* skelAnime);
void PlayerAnimation_LoadToMorph(struct PlayState* play, SkelAnime* skelAnime, PlayerAnimationHeader* animation, f32 frame);
void PlayerAnimation_LoadToJoint(struct PlayState* play, SkelAnime* skelAnime, PlayerAnimationHeader* animation, f32 frame);
void PlayerAnimation_InterpJointMorph(struct PlayState* play, SkelAnime* skelAnime, f32 weight);
void PlayerAnimation_BlendToJoint(struct PlayState* play, SkelAnime* skelAnime, PlayerAnimationHeader* animation1, f32 frame1, PlayerAnimationHeader* animation2, f32 frame2, f32 blendWeight, void* blendTableBuffer);
void PlayerAnimation_BlendToMorph(struct PlayState* play, SkelAnime* skelAnime, PlayerAnimationHeader* animation1, f32 frame1, PlayerAnimationHeader* animation2, f32 frame2, f32 blendWeight, void* blendTableBuffer);
void PlayerAnimation_EndLoop(SkelAnime* skelAnime);
s32 PlayerAnimation_OnFrame(SkelAnime* skelAnime, f32 frame);

void SkelAnime_Init(struct PlayState* play, SkelAnime* skelAnime, SkeletonHeader* skeletonHeaderSeg, AnimationHeader* animation, Vec3s* jointTable, Vec3s* morphTable, s32 limbCount);
void SkelAnime_InitFlex(struct PlayState* play, SkelAnime* skelAnime, FlexSkeletonHeader* skeletonHeaderSeg, AnimationHeader* animation, Vec3s* jointTable, Vec3s* morphTable, s32 limbCount);
void SkelAnime_InitSkin(struct GameState* gameState, SkelAnime* skelAnime, SkeletonHeader* skeletonHeaderSeg, AnimationHeader* animation);

s32 SkelAnime_Update(SkelAnime* skelAnime);
void Animation_ChangeImpl(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed, f32 startFrame, f32 endFrame, u8 mode, f32 morphFrames, s8 taper);
void Animation_Change(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed, f32 startFrame, f32 endFrame, u8 mode, f32 morphFrames);
void Animation_PlayOnce(SkelAnime* skelAnime, AnimationHeader* animation);
void Animation_MorphToPlayOnce(SkelAnime* skelAnime, AnimationHeader* animation, f32 morphFrames);
void Animation_PlayOnceSetSpeed(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed);
void Animation_PlayLoop(SkelAnime* skelAnime, AnimationHeader* animation);
void Animation_MorphToLoop(SkelAnime* skelAnime, AnimationHeader* animation, f32 morphFrames);
void Animation_PlayLoopSetSpeed(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed);
void Animation_EndLoop(SkelAnime* skelAnime);
void Animation_Reverse(SkelAnime* skelAnime);
void SkelAnime_CopyFrameTableTrue(SkelAnime* skelAnime, Vec3s* dst, Vec3s* src, u8* copyFlag);
void SkelAnime_CopyFrameTableFalse(SkelAnime* skelAnime, Vec3s* dst, Vec3s* src, u8* copyFlag);
void SkelAnime_UpdateTranslation(SkelAnime* skelAnime, Vec3f* diff, s16 angle);
s32 Animation_OnFrame(SkelAnime* skelAnime, f32 frame);
void SkelAnime_Free(SkelAnime* skelAnime, struct PlayState* play);

// ZAPD compatibility typedefs
// TODO: Remove when ZAPD adds support for them
typedef PlayerAnimationHeader LinkAnimationHeader;

#endif
