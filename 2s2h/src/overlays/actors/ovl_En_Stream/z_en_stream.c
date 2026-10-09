/*
 * File: z_en_stream.c
 * Overlay: ovl_En_Stream
 * Description: Unused water vortex from OoT
 */

#include "z_en_stream.h"
#include "objects/object_stream/object_stream.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnStream_Init(Actor* thisx, PlayState* play);
void EnStream_Destroy(Actor* thisx, PlayState* play);
void EnStream_Update(Actor* thisx, PlayState* play);
void EnStream_Draw(Actor* thisx, PlayState* play);

void EnStream_WaitForPlayer(EnStream* this, PlayState* play);

ActorProfile En_Stream_Profile = {
    /**/ ACTOR_EN_STREAM,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_STREAM,
    /**/ sizeof(EnStream),
    /**/ EnStream_Init,
    /**/ EnStream_Destroy,
    /**/ EnStream_Update,
    /**/ EnStream_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 20, ICHAIN_STOP),
};

void EnStream_SetupAction(EnStream* this, EnStreamActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnStream_Init(Actor* thisx, PlayState* play) {
    EnStream* this = (EnStream*)thisx;

    this->size = EN_STREAM_SIZE(&this->actor);
    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (this->size != EN_STREAM_SIZE_NORMAL && this->size == EN_STREAM_SIZE_SMALL) {
        this->actor.scale.y = 0.01f;
    }
    EnStream_SetupAction(this, EnStream_WaitForPlayer);
}

void EnStream_Destroy(Actor* thisx, PlayState* play) {
}

s32 EnStream_PlayerIsInRange(Vec3f* vortexWorldPos, Vec3f* playerWorldPos, Vec3f* posDifference, f32 vortexYScale) {
    s32 ret = EN_STREAM_PLAYER_OUTSIDE_RANGE;
    f32 smallConstant = 28.0f;
    f32 upperBounds = 160 * vortexYScale * 50.0f;
    f32 lowerBounds = 0 * vortexYScale * 50.0f;
    f32 xzDist;
    f32 range;

    posDifference->x = playerWorldPos->x - vortexWorldPos->x;
    posDifference->y = playerWorldPos->y - vortexWorldPos->y;
    posDifference->z = playerWorldPos->z - vortexWorldPos->z;
    xzDist = sqrtf(SQ(posDifference->x) + SQ(posDifference->z));

    if (lowerBounds <= posDifference->y && posDifference->y <= upperBounds) {
        posDifference->y -= lowerBounds;

        range = ((75.0f - smallConstant) * (posDifference->y / (upperBounds - lowerBounds))) + 28.0f;
        if (xzDist <= range) {
            ret = EN_STREAM_PLAYER_WITHIN_RANGE_INSIDE_VORTEX;
        }
    }

    if (posDifference->y <= lowerBounds && xzDist <= 28.0f) {
        ret = EN_STREAM_PLAYER_WITHIN_RANGE_BELOW_VORTEX;
    }

    return ret;
}

void EnStream_SuckPlayer(EnStream* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad48;
    Vec3f posDifference;
    f32 xzDist;
    f32 yDistWithOffset;
    s32 pad30[2];

    if (EnStream_PlayerIsInRange(&this->actor.world.pos, &player->actor.world.pos, &posDifference,
                                 this->actor.scale.y) != EN_STREAM_PLAYER_OUTSIDE_RANGE) {
        xzDist = sqrtf(SQ(posDifference.x) + SQ(posDifference.z));
        yDistWithOffset = player->actor.world.pos.y - (this->actor.world.pos.y - 90.0f);
        player->pushedYaw = Math_Atan2S(-posDifference.x, -posDifference.z);
        if (xzDist > 3.0f) {
            Math_SmoothStepToF(&player->pushedSpeed, 3.0f, 0.5f, xzDist, 0.0f);
        } else {
            player->pushedSpeed = 0.0f;
            Math_SmoothStepToF(&player->actor.world.pos.x, this->actor.world.pos.x, 0.5f, 3.0f, 0.0f);
            Math_SmoothStepToF(&player->actor.world.pos.z, this->actor.world.pos.z, 0.5f, 3.0f, 0.0f);
        }
        if (yDistWithOffset > 0.0f) {
            Math_SmoothStepToF(&player->actor.velocity.y, -3.0f, 0.7f, yDistWithOffset, 0.0f);
            if (posDifference.y < -70.0f) {
                player->stateFlags2 |= PLAYER_STATE2_80000000;
            }
        }
    } else {
        EnStream_SetupAction(this, EnStream_WaitForPlayer);
    }
}

void EnStream_WaitForPlayer(EnStream* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    Vec3f temp;

    if (EnStream_PlayerIsInRange(&this->actor.world.pos, &player->actor.world.pos, &temp, this->actor.scale.y) !=
        EN_STREAM_PLAYER_OUTSIDE_RANGE) {
        EnStream_SetupAction(this, EnStream_SuckPlayer);
    }
}

void EnStream_Update(Actor* thisx, PlayState* play) {
    EnStream* this = (EnStream*)thisx;

    this->actionFunc(this, play);
    Actor_PlaySfx_FlaggedCentered2(&this->actor, NA_SE_EV_WHIRLPOOL - SFX_FLAG);
}

void EnStream_Draw(Actor* thisx, PlayState* play) {
    u32 multipliedFrames;
    u32 frames = play->gameplayFrames;
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gfx = POLY_XLU_DISP;
    MATRIX_FINALIZE_AND_LOAD(&gfx[0], play->state.gfxCtx);
    multipliedFrames = frames * 20;
    gSPSegment(&gfx[1], 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, frames * 30, -multipliedFrames, 64, 64, 1, multipliedFrames,
                                  -multipliedFrames, 64, 64, 30, -20, 20, -20));
    gSPDisplayList(&gfx[2], gWaterVortexDL);
    POLY_XLU_DISP = &gfx[3];

    CLOSE_DISPS(play->state.gfxCtx);
}
