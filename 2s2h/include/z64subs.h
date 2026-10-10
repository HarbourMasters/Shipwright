#ifndef Z64SUBS_H
#define Z64SUBS_H

#include "z64actor.h"
#include "z64scene.h"

#include "code/sub_s/sub_s.h"

extern Vec3f gOneVec3f;

#define SUBS_TIME_PATHING_ORDER 3

// NOTE: all on screen checks are bugged, and always evaluate to true, even if the target is off screen
typedef enum {
    /* 0 */ SUBS_OFFER_MODE_NONE,
    /* 1 */ SUBS_OFFER_MODE_GET_ITEM,
    // Further modes all deal with offering talk exchange requests
    /* 2 */ SUBS_OFFER_MODE_NEARBY,
    /* 3 */ SUBS_OFFER_MODE_ONSCREEN,
    /* 4 */ SUBS_OFFER_MODE_AUTO,
    /* 5 */ SUBS_OFFER_MODE_AUTO_TARGETED, // Also checks nearby and on screen
    /* 6 */ SUBS_OFFER_MODE_AUTO_NEARBY_ONSCREEN
} SubSOfferMode;

#define SUBS_OFFER_MODE_MASK                                                                               \
    (SUBS_OFFER_MODE_GET_ITEM | SUBS_OFFER_MODE_NEARBY | SUBS_OFFER_MODE_ONSCREEN | SUBS_OFFER_MODE_AUTO | \
     SUBS_OFFER_MODE_AUTO_TARGETED | SUBS_OFFER_MODE_AUTO_NEARBY_ONSCREEN)

typedef enum {
    /* 0 */ SUBS_CUTSCENE_WITH_PLAYER,
    /* 1 */ SUBS_CUTSCENE_NORMAL,
    /* 2 */ SUBS_CUTSCENE_WITH_PLAYER_SET_FLAG
} SubSCutsceneType;

typedef s32 (*VerifyTalkExchangeActorFunc)(struct PlayState*, Actor*, void*);

typedef s32 (*VerifyFindActorFunc)(struct PlayState*, Actor*, Actor*, void*);

#define SUBS_SHADOW_TEX_WIDTH 64
#define SUBS_SHADOW_TEX_HEIGHT 64
#define SUBS_SHADOW_TEX_SIZE ((s32)sizeof(u8[SUBS_SHADOW_TEX_HEIGHT][SUBS_SHADOW_TEX_WIDTH]))

typedef struct TrackOptions {
    /* 0x0 */ u16 rotMax; // binary angles
    /* 0x2 */ u16 slowness; // larger for slower rotation, cannot be 0
    /* 0x4 */ u16 rotStepMin; // degrees
    /* 0x6 */ u16 rotStepMax; // degrees
} TrackOptions; // size = 0x8

typedef struct TrackOptionsSet {
    /* 0x00 */ TrackOptions headRotX;
    /* 0x08 */ TrackOptions headRotY;
    /* 0x10 */ TrackOptions torsoRotX;
    /* 0x18 */ TrackOptions torsoRotY;
} TrackOptionsSet; // size = 0x20

#define ACTOR_PATHING_RETURN_TO_START (1 << 0)
#define ACTOR_PATHING_SWITCH_DIRECTION (1 << 1)
#define ACTOR_PATHING_MOVE_BACKWARDS (1 << 3)
#define ACTOR_PATHING_REACHED_POINT_PERMANENT (1 << 4)
#define ACTOR_PATHING_REACHED_END_PERMANENT (1 << 5)
#define ACTOR_PATHING_REACHED_POINT_TEMPORARY (1 << 6)
#define ACTOR_PATHING_REACHED_END_TEMPORARY (1 << 7)

#define ACTOR_PATHING_REACHED_TEMPORARY \
    (ACTOR_PATHING_REACHED_POINT_TEMPORARY | ACTOR_PATHING_REACHED_END_TEMPORARY)
#define ACTOR_PATHING_REACHED_POINT \
    (ACTOR_PATHING_REACHED_POINT_PERMANENT | ACTOR_PATHING_REACHED_POINT_TEMPORARY)
#define ACTOR_PATHING_REACHED_END \
    (ACTOR_PATHING_REACHED_END_PERMANENT | ACTOR_PATHING_REACHED_END_TEMPORARY)

struct ActorPathing;
typedef void (*ActorPathingComputeFunc)(struct PlayState*, struct ActorPathing*);
typedef s32 (*ActorPathingUpdateFunc)(struct PlayState*, struct ActorPathing*);

typedef struct ActorPathing {
    /* 0x00 */ Path* setupPathList;
    /* 0x04 */ s32 pathIndex;
    /* 0x08 */ Vec3s* points;
    /* 0x0C */ s32 count;
    /* 0x10 */ s32 curPointIndex;
    /* 0x14 */ s32 begPointIndex;
    /* 0x18 */ s32 endPointIndex;
    /* 0x1C */ u8 flags;
    /* 0x1D */ u8 prevFlags;
    /* 0x20 */ Vec3f curPoint;
    /* 0x2C */ Vec3f pointOffset;
    /* 0x38 */ Vec3f prevPoint;
    /* 0x44 */ Vec3f* worldPos;
    /* 0x48 */ Actor* actor;
    /* 0x4C */ f32 distSqToCurPointXZ;
    /* 0x50 */ f32 distSqToCurPoint;
    /* 0x54 */ Vec3s rotToCurPoint;
    /* 0x5C */ ActorPathingComputeFunc computePointInfoFunc;
    /* 0x60 */ ActorPathingUpdateFunc updateActorInfoFunc; // Return true if should setNextPoint, false if the actor should move forward
    /* 0x64 */ ActorPathingUpdateFunc moveFunc; // Return true if should compute and update again
    /* 0x68 */ ActorPathingUpdateFunc setNextPointFunc; // Return true if should compute and update again
} ActorPathing; // size = 0x6C

struct EnDoor* SubS_FindScheduleDoor(struct PlayState* play, s32 schType);

Gfx* SubS_DrawTransformFlexLimb(struct PlayState* play, s32 limbIndex, void** skeleton, Vec3s* jointTable, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw, TransformLimbDraw transformLimbDraw, Actor* actor, Mtx** mtx, Gfx* gfx);
Gfx* SubS_DrawTransformFlex(struct PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw, TransformLimbDraw transformLimbDraw, Actor* actor, Gfx* gfx);

s32 SubS_InCsMode(struct PlayState* play);

s32 SubS_UpdateLimb(s16 newRotZ, s16 newRotY, Vec3f* pos, Vec3s* rot, s32 stepRot, s32 overrideRot);

void SubS_SetOfferMode(u16* flags, u16 offerMode, u16 mask);

void SubS_TimePathing_FillKnots(f32 knots[], s32 order, s32 numPoints);
s32 SubS_TimePathing_ComputeProgress(f32* progress, s32 elapsedTime, s32 waypointTime, s32 totalTime, s32 pathCount, s32 order, f32 knots[]);
void SubS_TimePathing_ComputeWeights(s32 order, f32 progress, s32 waypoint, f32 knots[], f32 weights[]);
void SubS_TimePathing_ComputeTargetPosXZ(f32* x, f32* z, f32 progress, s32 order, s32 waypoint, Vec3s points[], f32 knots[]);
s32 SubS_TimePathing_Update(Path* path, f32* progress, s32* elapsedTime, s32 waypointTime, s32 totalTime, s32* waypoint, f32 knots[], Vec3f* targetPos, s32 timeSpeed);
void SubS_TimePathing_ComputeInitialY(struct PlayState* play, Path* path, s32 waypoint, Vec3f* targetPos);

Path* SubS_GetAdditionalPath(struct PlayState* play, u8 pathIndex, s32 limit);

Actor* SubS_FindNearestActor(Actor* actor, struct PlayState* play, u8 actorCategory, s16 actorId);

s32 SubS_ChangeAnimationByInfoS(SkelAnime* skelAnime, AnimationInfoS* animationInfo, s32 animIndex);

s32 SubS_HasReachedPoint(Actor* actor, Path* path, s32 pointIndex);

Path* SubS_GetDayDependentPath(struct PlayState* play, u8 pathIndex, u8 pathIndexNone, s32* startPointIndex);

s32 SubS_WeightPathing_ComputePoint(Path* path, s32 waypoint, Vec3f* point, f32 progress, s32 direction);
s32 SubS_WeightPathing_Move(Actor* actor, Path* path, s32* waypoint, f32* progress, s32 direction, s32 returnStart);

s32 SubS_CopyPointFromPathCheckBounds(Path* path, s32 pointIndex, Vec3f* dst);

s32 SubS_Offer(Actor* actor, struct PlayState* play, f32 xzRange, f32 yRange, s32 itemId, SubSOfferMode mode);

void SubS_FillShadowTex(s32 startCol, s32 startRow, u8* tex, s32 size);
void SubS_GenShadowTex(Vec3f bodyPartsPos[], Vec3f* worldPos, u8* tex, f32 weight, u8 bodyPartsNum, u8 sizes[], s8 parentBodyParts[]);
void SubS_DrawShadowTex(Actor* actor, struct GameState* gameState, u8* tex);

s16 SubS_ComputeTrackPointRot(s16* rot, s16 rotMax, s16 target, f32 slowness, f32 stepMin, f32 stepMax);
s32 SubS_TrackPoint(Vec3f* target, Vec3f* focusPos, Vec3s* shapeRot, Vec3s* trackTarget, Vec3s* headRot, Vec3s* torsoRot, TrackOptionsSet* options);

s32 SubS_AngleDiffLessEqual(s16 angleA, s16 threshold, s16 angleB);

Path* SubS_GetPathByIndex(struct PlayState* play, s16 pathIndex, s16 pathIndexNone);
s32 SubS_CopyPointFromPath(Path* path, s32 pointIndex, Vec3f* dst);
s16 SubS_GetDistSqAndOrientPoints(Vec3f* vecA, Vec3f* vecB, f32* distSq);
s32 SubS_MoveActorToPoint(Actor* actor, Vec3f* point, s16 rotStep);
s16 SubS_GetDistSqAndOrientPath(Path* path, s32 pointIndex, Vec3f* pos, f32* distSq);

s8 SubS_IsObjectLoaded(s8 objectSlot, struct PlayState* play);
s8 SubS_GetObjectSlot(s16 objectId, struct PlayState* play);

Actor* SubS_FindActor(struct PlayState* play, Actor* actorListStart, u8 actorCategory, s16 actorId);

s32 SubS_UpdateFidgetTables(struct PlayState* play, s16* fidgetTableY, s16* fidgetTableZ, s32 tableLen);

s32 SubS_IsFloorAbove(struct PlayState* play, Vec3f* pos, f32 distAbove);

s32 SubS_CopyPointFromPathList(Path* paths, s32 pathIndex, s32 pointIndex, Vec3f* dst);
u8 SubS_GetPathCountFromPathList(Path* paths, s32 pathIndex);

void SubS_ActorPathing_Init(struct PlayState* play, Vec3f* worldPos, Actor* actor, ActorPathing* actorPath, Path* paths, s32 pathIndex, s32 begPointIndex, s32 endPointIndex, s32 curPointIndex, u8 flags);
s32 SubS_ActorPathing_Update(struct PlayState* play, ActorPathing* actorPath, ActorPathingComputeFunc computePointInfoFunc, ActorPathingUpdateFunc updateActorInfoFunc, ActorPathingUpdateFunc moveFunc, ActorPathingUpdateFunc setNextPointFunc);
void SubS_ActorPathing_ComputePointInfo(struct PlayState* play, ActorPathing* actorPath);
s32 SubS_ActorPathing_MoveWithGravity(struct PlayState* play, ActorPathing* actorPath);
s32 SubS_ActorPathing_MoveWithoutGravityReverse(struct PlayState* play, ActorPathing* actorPath);
s32 SubS_ActorPathing_SetNextPoint(struct PlayState* play, ActorPathing* actorPath);

void SubS_ChangeAnimationBySpeedInfo(SkelAnime* skelAnime, AnimationSpeedInfo* animationInfo, s32 nextAnimIndex, s32* curAnimIndex);

s32 SubS_StartCutscene(Actor* actor, s16 nextCsId, s16 curCsId, s32 type);
s32 SubS_FillCutscenesList(Actor* actor, s16 csIdList[], s16 numCutscenes);

void SubS_ConstructPlane(Vec3f* point, Vec3f* unitVec, Vec3s* rot, Plane* plane);
s32 SubS_LineSegVsPlane(Vec3f* point, Vec3s* rot, Vec3f* unitVec, Vec3f* linePointA, Vec3f* linePointB, Vec3f* intersect);

Actor* SubS_FindActorCustom(struct PlayState* play, Actor* actor, Actor* actorListStart, u8 actorCategory, s16 actorId, void* verifyData, VerifyFindActorFunc verifyActorFunc);

s32 SubS_OfferTalkExchangeCustom(Actor* actor, struct PlayState* play, f32 xzRange, f32 yRange, s32 exchangeItemAction, void* data, VerifyTalkExchangeActorFunc verifyActorFunc);
s32 SubS_ArePlayerAndActorFacing(struct PlayState* play, Actor* actor, void* data);
s32 SubS_OfferTalkExchangeFacing(Actor* actor, struct PlayState* play, f32 xzRange, f32 yRange, s32 exchangeItemAction, s16 playerYawRange, s16 actorYawRange);

s32 SubS_TrackPointStep(Vec3f* worldPos, Vec3f* focusPos, s16 shapeYRot, Vec3f* yawTarget, Vec3f* pitchTarget, s16* headZRotStep, s16* headXRotStep, s16* torsoZRotStep, s16* torsoXRotStep, u16 headZRotStepMax, u16 headXRotStepMax, u16 torsoZRotStepMax, u16 torsoXRotStepMax);

#endif
