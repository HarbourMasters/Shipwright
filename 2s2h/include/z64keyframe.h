#ifndef Z64_KEYFRAME_H
#define Z64_KEYFRAME_H

#include "ultra64.h"
#include "z64math.h"

struct PlayState;

struct KFSkelAnimeFlex;
struct KFSkelAnime;

typedef s32 (*OverrideKeyframeDraw)(struct PlayState* play, struct KFSkelAnime* kfSkelAnime, s32 limbIndex,
                                    Gfx** dList, u8* flags, void* arg, Vec3s* rot, Vec3f* pos);
typedef s32 (*PostKeyframeDraw)(struct PlayState* play, struct KFSkelAnime* kfSkelAnime, s32 limbIndex,
                                Gfx** dList, u8* flags, void* arg, Vec3s* rot, Vec3f* pos);

typedef s32 (*OverrideKeyframeDrawScaled)(struct PlayState* play, struct KFSkelAnimeFlex* kfSkelAnime, s32 limbIndex,
                                          Gfx** dList, u8* flags, void* arg, Vec3f* scale, Vec3s* rot, Vec3f* pos);
typedef s32 (*PostKeyframeDrawScaled)(struct PlayState* play, struct KFSkelAnimeFlex* kfSkelAnime, s32 limbIndex,
                                      Gfx** dList, u8* flags, void* arg, Vec3f* scale, Vec3s* rot, Vec3f* pos);

typedef s32 (*KeyframeTransformCallback)(struct PlayState* play, struct KFSkelAnimeFlex* kfSkelAnime, s32 limbIndex,
                                         Gfx** dList, u8* flags, void* arg);

// These flags are mutually exclusive, if XLU is set OPA is not set and vice-versa.
#define KEYFRAME_DRAW_OPA (0 << 0)
#define KEYFRAME_DRAW_XLU (1 << 0)

typedef enum {
    /* 0 */ KEYFRAME_NOT_DONE,
    /* 1 */ KEYFRAME_DONE_ONCE,
    /* 2 */ KEYFRAME_DONE_LOOP
} KeyFrameDoneType;

#define KF_CALLBACK_INDEX_NONE 0xFF

typedef struct {
    /* 0x00 */ Gfx* dList;     // Display list for this limb
    /* 0x04 */ u8 numChildren; // Number of children for this limb
    /* 0x05 */ u8 drawFlags;   // KEYFRAME_DRAW_*, determines which render layer the matrix + display list will be appended to
    /* 0x06 */ Vec3s jointPos; // For the root limb this is the position in model space; for child limbs it is the relative position to the parent
} KeyFrameStandardLimb; // size = 0xC

typedef struct {
    /* 0x00 */ u8 limbCount;                // Number of limbs in this skeleton
    /* 0x01 */ u8 dListCount;               // Number of limbs with a non-NULL display list, used to know how many matrices to allocate for drawing
    /* 0x04 */ KeyFrameStandardLimb* limbs; // Pointer to standard limb array
} KeyFrameSkeleton; // Size = 0x8

typedef struct {
    /* 0x00 */ Gfx* dList;       // Display list for this limb
    /* 0x04 */ u8 numChildren;   // Number of children for this limb
    /* 0x05 */ u8 drawFlags;     // KEYFRAME_DRAW_*, determines which render layer the matrix + display list will be appended to
    /* 0x06 */ u8 callbackIndex; // Transform callback function index, KF_CALLBACK_INDEX_NONE indicates no callback
} KeyFrameFlexLimb; // size = 0x8

typedef struct {
    /* 0x00 */ u8 limbCount;            // Number of limbs in this skeleton
    /* 0x01 */ u8 dListCount;           // Number of limbs with a non-NULL display list, used to know how many matrices to allocate for drawing
    /* 0x04 */ KeyFrameFlexLimb* limbs; // Pointer to flex limb array
} KeyFrameFlexSkeleton; // Size = 0x8

typedef struct {
    /* 0x00 */ s16 frame;    // Frame number for this keyframe
    /* 0x02 */ s16 value;    // Value (any of translation, rotation, scale)
    /* 0x04 */ s16 velocity; // The instantaneous rate of change of the value
} KeyFrame; // Size = 0x6

typedef struct {
    // Array of bitflags for each limb indicating whether to do keyframe interpolation
    // or pull from fixed values that do not change throughout the animation.
    union {
        // For standard the bit layout in each array element is:
        // [5] X Translation (root limb only)
        // [4] Y Translation (root limb only)
        // [3] Z Translation (root limb only)
        // [2] X Rotation (all limbs)
        // [1] Y Rotation (all limbs)
        // [0] Z Rotation (all limbs)
    /* 0x00 */ u8* standard;
        // For flex the bit layout in each array element is:
        // [8] X Scale
        // [7] Y Scale
        // [6] Z Scale
        // [5] X Rotation
        // [4] Y Rotation
        // [3] Z Rotation
        // [2] X Translation
        // [1] Y Translation
        // [0] Z Translation
    /* 0x00 */ u16* flex;
    } bitFlags;
    /* 0x04 */ KeyFrame* keyFrames; // Array of keyframes determining the motion, grouped by limb
    /* 0x08 */ s16* kfNums;         // Array containing how many keyframes belong to each limb
    /* 0x0C */ s16* fixedValues;    // Array of fixed rotation (standard skeleton) or scale/rotation/translation (flex skeleton) values
    /* 0x10 */ UNK_TYPE2 unk_10;
    /* 0x12 */ s16 frameCount;      // Length of the animation in 30fps frames
} KeyFrameAnimation; // Size = 0x14

typedef enum {
    /* 0 */ KEYFRAME_ANIM_ONCE, // Play once and stop
    /* 1 */ KEYFRAME_ANIM_LOOP  // Play in a loop
} KeyFrameAnimMode;

typedef struct {
    /* 0x00 */ f32 start;      // Current animation start frame number
    /* 0x04 */ f32 end;        // Current animation end frame number
    /* 0x08 */ f32 frameCount; // Current animation total frame count
    /* 0x0C */ f32 speed;      // Current play speed
    /* 0x10 */ f32 curTime;    // Current play frame number
    /* 0x14 */ s32 animMode;   // Current play mode (see FrameAnimMode)
} FrameControl; // Size = 0x18

typedef struct KFSkelAnime {
    /* 0x00 */ FrameControl frameCtrl;       // Current play state
    /* 0x18 */ KeyFrameSkeleton* skeleton;   // Skeleton to animate
    /* 0x1C */ KeyFrameAnimation* animation; // Currently active animation
    /* 0x20 */ f32 morphFrames;              // Number of frames in which to morph between the previous pose and the current animation
    /* 0x24 */ Vec3s* jointTable;            // Array of data describing the current pose
                                             //     size = 1 + limbCount, one root translation followed by rotations for each limb
    /* 0x28 */ Vec3s* morphTable;            // Array of data describing the current morph pose to interpolate with
                                             //     size = 1 + limbCount, one root translation followed by rotations for each limb
    /* 0x2C */ Vec3s* rotOffsetsTable;       // Table of rotations to add to the current pose, may be NULL so that no additional rotations are added
                                             //     size = limbCount
} KFSkelAnime; // Size = 0x30

typedef struct KFSkelAnimeFlex {
    /* 0x00 */ FrameControl frameCtrl;                        // Current play state
    /* 0x18 */ KeyFrameFlexSkeleton* skeleton;                // Skeleton to animate
    /* 0x1C */ KeyFrameAnimation* animation;                  // Currently active animation
    /* 0x20 */ KeyframeTransformCallback* transformCallbacks; // Pointer to array of limb transform callbacks, indexed by callbackIndex in KeyFrameFlexLimb
    /* 0x24 */ f32 morphFrames;                               // Number of frames in which to morph between the previous pose and the current animation
    /* 0x28 */ Vec3s* jointTable;                             // Array of data describing the current pose
                                                              //    size = 3 * limbCount in order of (scale, rotation, translation) for each limb
    /* 0x2C */ Vec3s* morphTable;                             // Array of data describing the current morph pose to interpolate with
                                                              //    size = 3 * limbCount in order of (scale, rotation, translation) for each limb
} KFSkelAnimeFlex; // Size = 0x30

void FrameCtrl_Reset(FrameControl* frameCtrl);
void FrameCtrl_Init(FrameControl* frameCtrl);
void FrameCtrl_SetProperties(FrameControl* frameCtrl, f32 startTime, f32 endTime, f32 frameCount, f32 t, f32 speed,
                             s32 animMode);
s32 FrameCtrl_PassCheck(FrameControl* frameCtrl, f32 t, f32* remainingTime);
s32 FrameCtrl_UpdateOnce(FrameControl* frameCtrl);
s32 FrameCtrl_UpdateLoop(FrameControl* frameCtrl);
s32 FrameCtrl_Update(FrameControl* frameCtrl);

void Keyframe_ResetFlex(KFSkelAnimeFlex* kfSkelAnime);
void Keyframe_InitFlex(KFSkelAnimeFlex* kfSkelAnime, KeyFrameFlexSkeleton* skeleton, KeyFrameAnimation* animation,
                       Vec3s* jointTable, Vec3s* morphTable, KeyframeTransformCallback* transformCallbacks);
void Keyframe_DestroyFlex(KFSkelAnimeFlex* kfSkelAnime);
void Keyframe_FlexPlayOnce(KFSkelAnimeFlex* kfSkelAnime, KeyFrameAnimation* animation);
void Keyframe_FlexPlayOnceSetSpeed(KFSkelAnimeFlex* kfSkelAnime, KeyFrameAnimation* animation, f32 speed);
void Keyframe_FlexMorphToPlayOnce(KFSkelAnimeFlex* kfSkelAnime, KeyFrameAnimation* animation, f32 morphFrames);
void Keyframe_FlexPlayLoop(KFSkelAnimeFlex* kfSkelAnime, KeyFrameAnimation* animation);
void Keyframe_FlexPlayLoopSetSpeed(KFSkelAnimeFlex* kfSkelAnime, KeyFrameAnimation* animation, f32 speed);
void Keyframe_FlexMorphToPlayLoop(KFSkelAnimeFlex* kfSkelAnime, KeyFrameAnimation* animation, f32 morphFrames);
void Keyframe_FlexChangeAnim(KFSkelAnimeFlex* kfSkelAnime, KeyFrameFlexSkeleton* skeleton, KeyFrameAnimation* animation,
                             f32 startTime, f32 endTime, f32 t, f32 speed, f32 morphFrames, s32 animMode);
void Keyframe_FlexChangeAnimQuick(KFSkelAnimeFlex* kfSkelAnime, KeyFrameAnimation* animation);
f32 Keyframe_Interpolate(f32 t, f32 delta, f32 x0, f32 x1, f32 v0, f32 v1);
s16 Keyframe_KeyCalc(s16 kfStart, s16 kfNum, KeyFrame* keyFrames, f32 t);
void Keyframe_MorphInterpolateRotation(f32 t, s16* out, s16 rot1, s16 rot2);
void Keyframe_MorphInterpolateLinear(s16* jointData, s16* morphData, f32 t);
void Keyframe_FlexMorphInterpolation(KFSkelAnimeFlex* kfSkelAnime);
s32 Keyframe_UpdateFlex(KFSkelAnimeFlex* kfSkelAnime);
void Keyframe_DrawFlex(struct PlayState* play, KFSkelAnimeFlex* kfSkelAnime, Mtx* mtxStack,
                       OverrideKeyframeDrawScaled overrideKeyframeDraw, PostKeyframeDrawScaled postKeyframeDraw,
                       void* arg);

void Keyframe_ResetStandard(KFSkelAnime* kfSkelAnime);
void Keyframe_InitStandard(KFSkelAnime* kfSkelAnime, KeyFrameSkeleton* skeleton, KeyFrameAnimation* animation,
                           Vec3s* jointTable, Vec3s* morphTable);
void Keyframe_DestroyStandard(KFSkelAnime* kfSkelAnime);
void Keyframe_StandardPlayOnce(KFSkelAnime* kfSkelAnime, KeyFrameAnimation* animation, Vec3s* rotOffsetsTable);
void Keyframe_StandardPlayOnceSetSpeed(KFSkelAnime* kfSkelAnime, KeyFrameAnimation* animation, Vec3s* rotOffsetsTable,
                                       f32 speed);
void Keyframe_StandardMorphToPlayOnce(KFSkelAnime* kfSkelAnime, KeyFrameAnimation* animation, Vec3s* rotOffsetsTable,
                                      f32 morphFrames);
void Keyframe_StandardPlayLoop(KFSkelAnime* kfSkelAnime, KeyFrameAnimation* animation, Vec3s* rotOffsetsTable);
void Keyframe_StandardPlayLoopSetSpeed(KFSkelAnime* kfSkelAnime, KeyFrameAnimation* animation, Vec3s* rotOffsetsTable,
                                       f32 speed);
void Keyframe_StandardMorphToPlayLoop(KFSkelAnime* kfSkelAnime, KeyFrameAnimation* animation, Vec3s* rotOffsetsTable,
                                      f32 morphFrames);
void Keyframe_StandardChangeAnim(KFSkelAnime* kfSkelAnime, KeyFrameSkeleton* skeleton, KeyFrameAnimation* animation,
                                 f32 startTime, f32 endTime, f32 t, f32 speed, f32 morphFrames, s32 animMode,
                                 Vec3s* rotOffsetsTable);
void Keyframe_StandardChangeAnimQuick(KFSkelAnime* kfSkelAnime, KeyFrameAnimation* animation);
void Keyframe_StandardMorphInterpolation(KFSkelAnime* kfSkelAnime);
s32 Keyframe_UpdateStandard(KFSkelAnime* kfSkelAnime);
void Keyframe_DrawStandardLimb(struct PlayState* play, KFSkelAnime* kfSkelAnime, s32* limbIndex,
                               OverrideKeyframeDraw overrideKeyframeDraw, PostKeyframeDraw postKeyframeDraw, void* arg,
                               Mtx** mtxStack);
void Keyframe_DrawStandard(struct PlayState* play, KFSkelAnime* kfSkelAnime, Mtx* mtxStack,
                           OverrideKeyframeDraw overrideKeyframeDraw, PostKeyframeDraw postKeyframeDraw, void* arg);

void Keyframe_FlexGetScale(KFSkelAnimeFlex* kfSkelAnime, s32 targetLimbIndex, Vec3s* scale);

#endif
