/*
 * File: z_en_encount1.c
 * Overlay: ovl_En_Encount1
 * Description: Spawner for Dragonflies, Skullfish, and Wallmasters
 */

#include "z_en_encount1.h"
#include "overlays/actors/ovl_En_Grasshopper/z_en_grasshopper.h"
#include "overlays/actors/ovl_En_Wallmas/z_en_wallmas.h"
#include "overlays/actors/ovl_En_Pr2/z_en_pr2.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_FREEZE_EXCEPTION | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnEncount1_Init(Actor* thisx, PlayState* play);
void EnEncount1_Update(Actor* thisx, PlayState* play);

void EnEncount1_SpawnActor(EnEncount1* this, PlayState* play);

ActorProfile En_Encount1_Profile = {
    /**/ ACTOR_EN_ENCOUNT1,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnEncount1),
    /**/ EnEncount1_Init,
    /**/ NULL,
    /**/ EnEncount1_Update,
    /**/ NULL,
};

static s16 sActorIds[] = {
    ACTOR_EN_GRASSHOPPER, // EN_ENCOUNT1_GRASSHOPPER
    ACTOR_EN_WALLMAS,     // EN_ENCOUNT1_WALLMASTER
    ACTOR_EN_PR2,         // EN_ENCOUNT1_SKULLFISH
    ACTOR_EN_PR2,         // EN_ENCOUNT1_SKULLFISH_2
};

static s16 sActorParams[] = {
    DRAGONFLY_PARAMS(DRAGONFLY_TYPE_GROWS_WHEN_SPAWNED),     // EN_ENCOUNT1_GRASSHOPPER
    WALLMASTER_PARAMS(WALLMASTER_TYPE_TIMER_ONLY, 0, false), // EN_ENCOUNT1_WALLMASTER
    ENPR2_PARAMS(1, 0),                                      // EN_ENCOUNT1_SKULLFISH
    ENPR2_PARAMS(3, 0)                                       // EN_ENCOUNT1_SKULLFISH_2
};

void EnEncount1_Init(Actor* thisx, PlayState* play) {
    EnEncount1* this = (EnEncount1*)thisx;

    if (this->actor.params <= 0) {
        Actor_Kill(&this->actor);
        return;
    }

    this->type = ENENCOUNT1_GET_TYPE(&this->actor);
    this->spawnActiveMax = ENENCOUNT1_GET_SPAWN_ACTIVE_MAX(&this->actor);
    this->spawnTotalMax = ENENCOUNT1_GET_SPAWN_TOTAL_MAX(&this->actor);
    this->spawnTimeMin = ENENCOUNT1_GET_SPAWN_TIME_MIN(&this->actor);
    this->spawnUnusedProp = ENENCOUNT1_GET_SPAWN_UNUSED_PROP(&this->actor);
    this->spawnDistanceMax = (ENENCOUNT1_GET_SPAWN_DISTANCE_MAX(&this->actor) * 40.0f) + 120.0f;

    if (this->spawnTotalMax >= ENENCOUNT1_SPAWNS_TOTAL_MAX_INFINITE) {
        this->spawnTotalMax = -1;
    }
    if (ENENCOUNT1_GET_SPAWN_DISTANCE_MAX(&this->actor) < 0) {
        this->spawnDistanceMax = -1.0f;
    }
    if (this->type == EN_ENCOUNT1_SKULLFISH_2) {
        this->pathIndex = ENENCOUNT1_GET_PATH_INDEX(&this->actor);
        this->path = SubS_GetPathByIndex(play, this->pathIndex, ENENCOUNT1_PATH_INDEX_NONE);
        this->spawnTotalMax = -1;
        this->spawnDistanceMax = -1.0f;
    }
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnEncount1_SpawnActor;
}

void EnEncount1_SpawnActor(EnEncount1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f spawnPos;
    f32 scale;
    f32 floorHeight;
    s16 rotY;
    s16 actorId;
    s32 actorParams;
    CollisionPoly* floorPoly;
    s32 bgId;

    if (((this->spawnActiveCount >= this->spawnActiveMax) ||
         ((this->spawnDistanceMax > 0.0f) && (this->spawnDistanceMax < this->actor.xzDistToPlayer)) ||
         ((this->spawnTotalMax > 0) && (this->spawnTotalMax <= this->spawnTotalCount)))) {
        return;
    }

    if (this->timer != 0) {
        this->timer++;
        if (this->timer < this->spawnTimeMin) {
            return;
        }
    }

    this->timer = 0;
    switch (this->type) {
        case EN_ENCOUNT1_GRASSHOPPER:
            scale = Rand_CenteredFloat(40.0f) + 200.0f;
            rotY = player->actor.shape.rot.y;
            if (this->spawnActiveCount & 1) {
                rotY = -rotY;
                scale = Rand_CenteredFloat(20.0f) + 100.0f;
            }
            spawnPos.x = player->actor.world.pos.x + (Math_SinS(rotY) * scale) + Rand_CenteredFloat(40.0f);
            spawnPos.y = player->actor.floorHeight + 120.0f;
            spawnPos.z = player->actor.world.pos.z + (Math_CosS(rotY) * scale) + Rand_CenteredFloat(40.0f);
            floorHeight = BgCheck_EntityRaycastFloor5(&play->colCtx, &floorPoly, &bgId, &this->actor, &spawnPos);
            if ((floorHeight <= BGCHECK_Y_MIN) ||
                ((player->actor.depthInWater != BGCHECK_Y_MIN) &&
                 (floorHeight < (player->actor.world.pos.y - player->actor.depthInWater)))) {
                return;
            }
            spawnPos.y = floorHeight;
            break;

        case EN_ENCOUNT1_WALLMASTER:
            Math_Vec3f_Copy(&spawnPos, &player->actor.world.pos);
            break;

        case EN_ENCOUNT1_SKULLFISH:
            scale = Rand_CenteredFloat(250.0f) + 500.0f;
            rotY = player->actor.shape.rot.y;
            spawnPos.x = player->actor.world.pos.x + (Math_SinS(rotY) * scale) + Rand_CenteredFloat(40.0f);
            spawnPos.y = player->actor.world.pos.y - Rand_ZeroFloat(20.0f);
            spawnPos.z = player->actor.world.pos.z + (Math_CosS(rotY) * scale) + Rand_CenteredFloat(40.0f);
            floorHeight = BgCheck_EntityRaycastFloor5(&play->colCtx, &floorPoly, &bgId, &this->actor, &spawnPos);
            if (!(player->stateFlags1 & PLAYER_STATE1_8000000) || (floorHeight <= BGCHECK_Y_MIN) ||
                (player->actor.depthInWater < floorHeight)) {
                return;
            }
            break;

        case EN_ENCOUNT1_SKULLFISH_2:
            if ((this->path != NULL) && !SubS_CopyPointFromPath(this->path, 0, &spawnPos)) {
                Actor_Kill(&this->actor);
            }
            break;

        default:
            break;
    }

    actorId = sActorIds[this->type];
    actorParams = sActorParams[this->type];
    if (Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, actorId, spawnPos.x, spawnPos.y, spawnPos.z, 0, 0, 0,
                           actorParams) != NULL) {
        this->spawnActiveCount++;
        if (this->spawnTotalMax > 0) {
            this->spawnTotalCount++;
        }

        if ((this->spawnActiveCount >= this->spawnActiveMax) && (this->spawnTimeMin != 0)) {
            this->timer = 1;
        }
    }
}

void EnEncount1_Update(Actor* thisx, PlayState* play) {
    EnEncount1* this = (EnEncount1*)thisx;

    this->actionFunc(this, play);
}
