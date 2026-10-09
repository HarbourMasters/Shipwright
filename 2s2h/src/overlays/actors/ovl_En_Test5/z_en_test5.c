/*
 * File: z_en_test5.c
 * Overlay: ovl_En_Test5
 * Description: Spring Water Modifier
 */

#include "z_en_test5.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnTest5_Init(Actor* thisx, PlayState* play2);
void EnTest5_Destroy(Actor* thisx, PlayState* play);
void EnTest5_Update(Actor* thisx, PlayState* play2);
void EnTest5_HandleBottleAction(EnTest5* this, PlayState* play);
void EnTest5_SetupAction(EnTest5* this, EnTest5ActionFunc actionFunc);

ActorProfile En_Test5_Profile = {
    /**/ ACTOR_EN_TEST5,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnTest5),
    /**/ EnTest5_Init,
    /**/ EnTest5_Destroy,
    /**/ EnTest5_Update,
    /**/ NULL,
};

void EnTest5_SetupAction(EnTest5* this, EnTest5ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnTest5_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest5* this = (EnTest5*)thisx;
    WaterBox* waterBox;
    f32 ySurface;

    // If not spawned above a water source, immediately despawn
    if (!WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &ySurface,
                              &waterBox)) {
        Actor_Kill(&this->actor);
        return;
    }

    Math_Vec3s_ToVec3f(&this->minPos, &waterBox->minPos);
    this->xLength = (f32)waterBox->xLength;
    this->zLength = (f32)waterBox->zLength;

    EnTest5_SetupAction(this, EnTest5_HandleBottleAction);
}

void EnTest5_Destroy(Actor* thisx, PlayState* play) {
}

void EnTest5_HandleBottleAction(EnTest5* this, PlayState* play) {
    Player* player;
    Vec3f playerPosRelativeToWater;

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        return;
    }

    player = GET_PLAYER(play);

    if ((player->interactRangeActor == NULL) || (player->getItemId != GI_MAX)) {
        Math_Vec3f_DistXYZAndStoreDiff(&this->minPos, &player->actor.world.pos, &playerPosRelativeToWater);

        // Make sure that the player is within the bounds of the water and deep enough to grab some
        if ((playerPosRelativeToWater.x >= 0.0f) && (playerPosRelativeToWater.x <= this->xLength) &&
            (playerPosRelativeToWater.z >= 0.0f) && (playerPosRelativeToWater.z <= this->zLength) &&
            (fabsf(playerPosRelativeToWater.y) < 100.0f) && (player->actor.depthInWater > 12.0f)) {
            Actor_OfferGetItem(&this->actor, play, GI_MAX, this->actor.xzDistToPlayer,
                               fabsf(this->actor.playerHeightRel));
        }
    }
}

void EnTest5_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest5* this = (EnTest5*)thisx;
    Vec3f steamPos;
    CollisionPoly* poly;
    s32 pad;

    this->actionFunc(this, play);

    // If it's the hot spring variant, generate steam clouds
    if (ENTEST5_IS_HOT_SPRING(&this->actor) && ((play->state.frames % 4) == 0)) {
        steamPos.x = this->minPos.x + (Rand_ZeroOne() * this->xLength);
        steamPos.y = this->minPos.y + 100.0f;
        steamPos.z = this->minPos.z + (Rand_ZeroOne() * this->zLength);

        if ((BgCheck_EntityRaycastFloor2(play, &play->colCtx, &poly, &steamPos) + 10.0f) < this->minPos.y) {
            Vec3f steamVel;

            steamPos.y = this->minPos.y + 10.0f;
            steamVel.y = 0.5f;
            steamVel.x = 0.0f;
            steamVel.z = 0.0f;

            EffectSsIceSmoke_Spawn(play, &steamPos, &steamVel, &gZeroVec3f, -200 - (s32)(Rand_ZeroOne() * 50.0f));
        }
    }
}
