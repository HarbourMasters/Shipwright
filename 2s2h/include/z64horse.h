#ifndef Z64_HORSE_H
#define Z64_HORSE_H

#include "ultra64.h"
#include "z64.h"


s32 Horse_GetJumpingFencePathIndex(PlayState* play);
s32 Horse_CopyPointFromPathList(PlayState* play, s32 pathIndex, s32 pointIndex, Vec3s* dst, s16* arg4);
s32 Horse_IsValidSpawn(s16 sceneId);
void Horse_ResetHorseData(PlayState* play);
void Horse_Spawn(PlayState* play, Player* player);
void Horse_RotateToPoint(Actor* actor, Vec3f* pos, s16 turnYaw);
s32 Horse_IsActive(PlayState* play, ActorContext* actorCtx);


extern s32 gHorseIsMounted;
extern s32 D_801BDAA0;
extern s32 gHorsePlayedEponasSong;

#endif
