/*
 * File: z_bg_gnd_iceblock.c
 * Overlay: ovl_Bg_Gnd_Iceblock
 * Description: Pushable ice block (Inside Ganon's Castle)
 */

#include "z_bg_gnd_iceblock.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /* 0 */ GNDICE_IDLE,
    /* 1 */ GNDICE_FALL,
    /* 2 */ GNDICE_HOLE
} BgGndIceblockAction;

void BgGndIceblock_Init(Actor* thisx, PlayState* play);
void BgGndIceblock_Destroy(Actor* thisx, PlayState* play);
void BgGndIceblock_Update(Actor* thisx, PlayState* play);
void BgGndIceblock_Draw(Actor* thisx, PlayState* play);

void BgGndIceblock_Idle(BgGndIceblock* this, PlayState* play);
void BgGndIceblock_Slide(BgGndIceblock* this, PlayState* play);

const ActorInit Bg_Gnd_Iceblock_InitVars = {
    ACTOR_BG_GND_ICEBLOCK,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_DEMO_KEKKAI,
    sizeof(BgGndIceblock),
    (ActorFunc)BgGndIceblock_Init,
    (ActorFunc)BgGndIceblock_Destroy,
    (ActorFunc)BgGndIceblock_Update,
    (ActorFunc)BgGndIceblock_Draw,
    NULL,
};

static Color_RGBA8 sWhite = { 250, 250, 250, 255 };
static Color_RGBA8 sGray = { 180, 180, 180, 255 };
static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static u8 sBlockPositions[2];

void BgGndIceblock_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndIceblock* this = (BgGndIceblock*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gWaterTrialIceBlockCol, &colHeader);
    this->targetPos = this->dyna.actor.home.pos;
    this->actionFunc = BgGndIceblock_Idle;
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.actor.world.pos.x == 2792.0f) {
        this->dyna.actor.params = 0;
        sBlockPositions[0] = 7;
    } else if (this->dyna.actor.world.pos.x == 3032.0f) {
        this->dyna.actor.params = 1;
        sBlockPositions[1] = 14;
    } else {
        LOG_FLOAT("thisx->world.position.x", this->dyna.actor.world.pos.x);
        ASSERT(0);
    }
}

void BgGndIceblock_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndIceblock* this = (BgGndIceblock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

/*
 * Diagram of positions in the room:
 *            __
 *  _________|10|_________
 * |*0*     6   *13****16*|
 * |*1*     7          17 |
 * | 2           14    18 |
 * | 3     h8    15    19 |
 * | 4      9 11 XX   *20*|
 * |*5*    XX 12      *21*|
 *  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 * XX are rocks
 * ** are pits
 * h is the hole.
 * Block 0 starts at 7 and block 1 starts at 14
 */

void BgGndIceblock_SetPosition(BgGndIceblock* this, s32 blockPosition) {
    Actor* thisx = &this->dyna.actor;
    u8 xPosIdx[22] = {
        0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 6, 6, 6, 6, 6, 6,
    };
    u8 zPosIdx[22] = {
        5, 4, 3, 2, 1, 0, 5, 4, 2, 1, 6, 1, 0, 5, 3, 2, 5, 4, 3, 2, 1, 0,
    };

    sBlockPositions[thisx->params] = blockPosition;
    this->targetPos.x = 2552.0f + (xPosIdx[blockPosition] * 120.0f);
    this->targetPos.z = -540.0f - (zPosIdx[blockPosition] * 120.0f);
}

s32 BgGndIceblock_CheckForBlock(s32 blockPosition) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if (blockPosition == sBlockPositions[i]) {
            return true;
        }
    }
    return false;
}

s32 BgGndIceblock_NextAction(BgGndIceblock* this) {
    switch (sBlockPositions[this->dyna.actor.params]) {
        case 0:
        case 1:
        case 5:
        case 13:
        case 16:
        case 20:
        case 21:
            return GNDICE_FALL;
        case 8:
            return GNDICE_HOLE;
        default:
            return GNDICE_IDLE;
    }
}

void BgGndIceblock_SetNextPosition(BgGndIceblock* this) {
    if (this->dyna.unk_158 == 0) {
        switch (sBlockPositions[this->dyna.actor.params]) {
            case 3:
            case 4:
                BgGndIceblock_SetPosition(this, 5);
                break;
            case 7:
                if (BgGndIceblock_CheckForBlock(8)) {
                    BgGndIceblock_SetPosition(this, 9);
                } else {
                    BgGndIceblock_SetPosition(this, 8);
                }
                break;
            case 11:
                BgGndIceblock_SetPosition(this, 12);
                break;
            case 14:
                BgGndIceblock_SetPosition(this, 15);
                break;
            case 18:
            case 19:
                BgGndIceblock_SetPosition(this, 20);
                break;
        }
    } else if (this->dyna.unk_158 == -0x8000) {
        switch (sBlockPositions[this->dyna.actor.params]) {
            case 2:
            case 3:
                BgGndIceblock_SetPosition(this, 1);
                break;
            case 7:
            case 9:
                BgGndIceblock_SetPosition(this, 6);
                break;
            case 11:
                BgGndIceblock_SetPosition(this, 10);
                break;
            case 14:
            case 15:
                BgGndIceblock_SetPosition(this, 13);
                break;
            case 17:
                BgGndIceblock_SetPosition(this, 16);
                break;
            case 18:
                if (!BgGndIceblock_CheckForBlock(17)) {
                    BgGndIceblock_SetPosition(this, 16);
                }
                break;
        }
    } else if (this->dyna.unk_158 == 0x4000) {
        switch (sBlockPositions[this->dyna.actor.params]) {
            case 6:
                BgGndIceblock_SetPosition(this, 13);
                break;
            case 7:
                BgGndIceblock_SetPosition(this, 17);
                break;
            case 9:
                BgGndIceblock_SetPosition(this, 11);
                break;
            case 12:
                BgGndIceblock_SetPosition(this, 21);
                break;
            case 14:
                BgGndIceblock_SetPosition(this, 18);
                break;
            case 15:
                BgGndIceblock_SetPosition(this, 19);
                break;
        }
    } else if (this->dyna.unk_158 == -0x4000) {
        switch (sBlockPositions[this->dyna.actor.params]) {
            case 6:
                BgGndIceblock_SetPosition(this, 0);
                break;
            case 7:
                BgGndIceblock_SetPosition(this, 1);
                break;
            case 9:
            case 11:
                BgGndIceblock_SetPosition(this, 4);
                break;
            case 14:
                BgGndIceblock_SetPosition(this, 2);
                break;
            case 15:
                if (BgGndIceblock_CheckForBlock(8)) {
                    BgGndIceblock_SetPosition(this, 3);
                } else {
                    BgGndIceblock_SetPosition(this, 8);
                }
                break;
        }
    }
}

void BgGndIceblock_Idle(BgGndIceblock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~0x10;
        if (this->dyna.unk_150 > 0.0f) {
            BgGndIceblock_SetNextPosition(this);
            if (Actor_WorldDistXZToPoint(&this->dyna.actor, &this->targetPos) > 1.0f) {
                func_8002DF54(play, &this->dyna.actor, 8);
                this->actionFunc = BgGndIceblock_Slide;
            }
        }
        this->dyna.unk_150 = 0.0f;
    }
}

void BgGndIceblock_Reset(BgGndIceblock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* thisx = &this->dyna.actor;

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~0x10;
        this->dyna.unk_150 = 0.0f;
    }
    if (Math_StepToF(&thisx->world.pos.y, thisx->home.pos.y, 1.0f)) {
        this->targetPos = thisx->home.pos;
        thisx->speedXZ = 0.0f;
        this->actionFunc = BgGndIceblock_Idle;
        switch (thisx->params) {
            case 0:
                sBlockPositions[0] = 7;
                break;
            case 1:
                sBlockPositions[1] = 14;
                break;
        }
    }
}

void BgGndIceblock_Fall(BgGndIceblock* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    thisx->velocity.y += 1.0f;
    if (Math_StepToF(&thisx->world.pos.y, thisx->home.pos.y - 300.0f, thisx->velocity.y)) {
        thisx->velocity.y = 0.0f;
        thisx->world.pos.x = thisx->home.pos.x;
        thisx->world.pos.y = thisx->home.pos.y - 100.0f;
        thisx->world.pos.z = thisx->home.pos.z;
        if (Player_InCsMode(play)) {
            func_8002DF54(play, thisx, 7);
        }
        this->actionFunc = BgGndIceblock_Reset;
    }
}

void BgGndIceblock_Hole(BgGndIceblock* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    thisx->velocity.y += 1.0f;
    if (Math_StepToF(&thisx->world.pos.y, thisx->home.pos.y - 100.0f, thisx->velocity.y)) {
        thisx->velocity.y = 0.0f;
        if (Player_InCsMode(play)) {
            func_8002DF54(play, thisx, 7);
        }
        this->actionFunc = BgGndIceblock_Idle;
    }
}

void BgGndIceblock_Slide(BgGndIceblock* this, PlayState* play) {
    s32 atTarget;
    Vec3f pos;
    Vec3f velocity;
    f32 spread;
    Actor* thisx = &this->dyna.actor;

    Math_StepToF(&thisx->speedXZ, 10.0f, 0.5f);
    atTarget = Math_StepToF(&thisx->world.pos.x, this->targetPos.x, thisx->speedXZ);
    atTarget &= Math_StepToF(&thisx->world.pos.z, this->targetPos.z, thisx->speedXZ);
    if (atTarget) {
        thisx->speedXZ = 0.0f;
        this->targetPos.x = thisx->world.pos.x;
        this->targetPos.z = thisx->world.pos.z;
        Audio_PlayActorSound2(thisx, NA_SE_EV_BLOCK_BOUND);
        switch (BgGndIceblock_NextAction(this)) {
            case GNDICE_IDLE:
                this->actionFunc = BgGndIceblock_Idle;
                func_8002DF54(play, thisx, 7);
                break;
            case GNDICE_FALL:
                this->actionFunc = BgGndIceblock_Fall;
                break;
            case GNDICE_HOLE:
                this->actionFunc = BgGndIceblock_Hole;
                break;
        }
    } else if (thisx->speedXZ > 6.0f) {
        spread = Rand_CenteredFloat(120.0f);
        velocity.x = -(1.5f + Rand_ZeroOne()) * Math_SinS(this->dyna.unk_158);
        velocity.y = Rand_ZeroOne() + 1.0f;
        velocity.z = -(1.5f + Rand_ZeroOne()) * Math_CosS(this->dyna.unk_158);
        pos.x = thisx->world.pos.x - (60.0f * Math_SinS(this->dyna.unk_158)) - (Math_CosS(this->dyna.unk_158) * spread);
        pos.z = thisx->world.pos.z - (60.0f * Math_CosS(this->dyna.unk_158)) + (Math_SinS(this->dyna.unk_158) * spread);
        pos.y = thisx->world.pos.y;
        func_8002829C(play, &pos, &velocity, &sZeroVec, &sWhite, &sGray, 250, Rand_S16Offset(40, 15));
        spread = Rand_CenteredFloat(120.0f);
        pos.x = thisx->world.pos.x - (60.0f * Math_SinS(this->dyna.unk_158)) + (Math_CosS(this->dyna.unk_158) * spread);
        pos.z = thisx->world.pos.z - (60.0f * Math_CosS(this->dyna.unk_158)) - (Math_SinS(this->dyna.unk_158) * spread);
        func_8002829C(play, &pos, &velocity, &sZeroVec, &sWhite, &sGray, 250, Rand_S16Offset(40, 15));
        func_8002F974(thisx, NA_SE_PL_SLIP_ICE_LEVEL - SFX_FLAG);
    }
}

void BgGndIceblock_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndIceblock* this = (BgGndIceblock*)thisx;

    this->actionFunc(this, play);
}

void BgGndIceblock_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndIceblock* this = (BgGndIceblock*)thisx;

    Gfx_DrawDListOpa(play, gWaterTrialIceBlockDL);
}
