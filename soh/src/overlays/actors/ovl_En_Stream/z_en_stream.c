/*
 * File: z_en_stream.c
 * Overlay: ovl_En_Stream
 * Description: Water Vortex
 */

#include "z_en_stream.h"
#include "objects/object_stream/object_stream.h"

#define FLAGS ACTOR_FLAG_4

void EnStream_Init(Actor* thisx, PlayState* play);
void EnStream_Destroy(Actor* thisx, PlayState* play);
void EnStream_Update(Actor* thisx, PlayState* play);
void EnStream_Draw(Actor* thisx, PlayState* play);
void EnStream_WaitForPlayer(EnStream* this, PlayState* play);

const ActorInit En_Stream_InitVars = {
    ACTOR_EN_STREAM,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_STREAM,
    sizeof(EnStream),
    (ActorFunc)EnStream_Init,
    (ActorFunc)EnStream_Destroy,
    (ActorFunc)EnStream_Update,
    (ActorFunc)EnStream_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 20, ICHAIN_STOP),
};

void EnStream_SetupAction(EnStream* this, EnStreamActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnStream_Init(Actor* thisx, PlayState* play) {
    EnStream* this = (EnStream*)thisx;

    this->unk_150 = thisx->params & 0xFF;
    Actor_ProcessInitChain(thisx, sInitChain);
    if ((this->unk_150 != 0) && (this->unk_150 == 1)) {
        thisx->scale.y = 0.01f;
    }
    EnStream_SetupAction(this, EnStream_WaitForPlayer);
}

void EnStream_Destroy(Actor* thisx, PlayState* play) {
}

// Checks if the player is in range of the vortex
s32 func_80B0B81C(Vec3f* vortexPosRot, Vec3f* playerPosRot, Vec3f* posDifference, f32 vortexYScale) {
    s32 ret = 0;
    f32 smallConstant = 28.0f;
    f32 upperBounds = 160 * vortexYScale * 50.0f;
    f32 lowerBounds = 0 * vortexYScale * 50.0f;
    f32 xzDist;
    f32 range;

    posDifference->x = playerPosRot->x - vortexPosRot->x;
    posDifference->y = playerPosRot->y - vortexPosRot->y;
    posDifference->z = playerPosRot->z - vortexPosRot->z;
    xzDist = sqrtf(SQ(posDifference->x) + SQ(posDifference->z));

    if (lowerBounds <= posDifference->y && posDifference->y <= upperBounds) {
        posDifference->y -= lowerBounds;

        range = ((75.0f - smallConstant) * (posDifference->y / (upperBounds - lowerBounds))) + 28.0f;
        if (xzDist <= range) {
            ret = 1;
        }
    }

    if ((posDifference->y <= lowerBounds) && (xzDist <= 28.0f)) {
        ret = 2;
    }

    return ret;
}

void EnStream_SuckPlayer(EnStream* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad48;
    Vec3f posDifference;
    f32 xzDist;
    f32 yDistWithOffset;
    s32 pad30;
    s32 pad2C;

    if (func_80B0B81C(&this->actor.world.pos, &player->actor.world.pos, &posDifference, this->actor.scale.y) != 0) {
        xzDist = sqrtf(SQ(posDifference.x) + SQ(posDifference.z));
        yDistWithOffset = player->actor.world.pos.y - (this->actor.world.pos.y - 90.0f);
        player->windDirection = Math_FAtan2F(-posDifference.x, -posDifference.z) * (0x8000 / M_PI);
        if (xzDist > 3.0f) {
            Math_SmoothStepToF(&player->windSpeed, 3.0f, 0.5f, xzDist, 0.0f);
        } else {
            player->windSpeed = 0.0f;
            Math_SmoothStepToF(&player->actor.world.pos.x, this->actor.world.pos.x, 0.5f, 3.0f, 0.0f);
            Math_SmoothStepToF(&player->actor.world.pos.z, this->actor.world.pos.z, 0.5f, 3.0f, 0.0f);
        }
        if (yDistWithOffset > 0.0f) {
            Math_SmoothStepToF(&player->actor.velocity.y, -3.0f, 0.7f, yDistWithOffset, 0.0f);
            if (posDifference.y < -70.0f) {
                player->stateFlags2 |= 0x80000000;
            }
        }
    } else {
        EnStream_SetupAction(this, EnStream_WaitForPlayer);
    }
}

void EnStream_WaitForPlayer(EnStream* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 pad;
    Vec3f temp;

    if (func_80B0B81C(&this->actor.world.pos, &player->actor.world.pos, &temp, this->actor.scale.y) != 0) {
        EnStream_SetupAction(this, EnStream_SuckPlayer);
    }
}

void EnStream_Update(Actor* thisx, PlayState* play) {
    EnStream* this = (EnStream*)thisx;

    this->actionFunc(this, play);
    func_8002F948(thisx, NA_SE_EV_WHIRLPOOL - SFX_FLAG);
}

void EnStream_Draw(Actor* thisx, PlayState* play) {
    u32 multipliedFrames;
    u32 frames = play->gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    multipliedFrames = frames * 20;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, frames * 30, -multipliedFrames, 0x40, 0x40, 1,
                                multipliedFrames, -multipliedFrames, 0x40, 0x40));
    gSPDisplayList(POLY_XLU_DISP++, object_stream_DL_000950);
    CLOSE_DISPS(play->state.gfxCtx);
}
