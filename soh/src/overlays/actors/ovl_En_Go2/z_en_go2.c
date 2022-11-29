#include "z_en_go2.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

/*
FLAGS

gSaveContext.eventChkInf[2] & 0x8 - DC entrance boulder blown up as child

InfTable

gSaveContext.infTable[11] & 0x10
gSaveContext.infTable[14] & 0x1 - Talked to DMT Goron at DC entrance (Before DC is opened as child)
gSaveContext.infTable[14] & 0x8 - Talked to GC Goron in bottom level stairwell
gSaveContext.infTable[14] & 0x40 - Talked to GC Goron at LW entrance (Before LW shortcut is opened)
gSaveContext.infTable[14] & 0x800 - Talked to DMT Goron at Bomb Flower with goron bracelet
gSaveContext.infTable[15] & 0x1 - Talked to Goron at GC Entrance (Before goron ruby is obtained)
gSaveContext.infTable[15] & 0x10 - Talked to Goron at GC Island (Before goron ruby is obtained)
gSaveContext.infTable[15] & 0x100 - (not on cloud modding) Talked to GC Goron outside Darunias door (after opening door,
before getting goron bracelet) gSaveContext.infTable[16] & 0x200 - Obtained Fire Tunic from Goron Link
gSaveContext.infTable[16] & 0x400 - (not on cloud modding)
gSaveContext.infTable[16] & 0x800 - Spoke to Goron Link About Volvagia
gSaveContext.infTable[16] & 0x1000 - Stopped Goron Link's Rolling
gSaveContext.infTable[16] & 0x2000 - EnGo Exclusive
gSaveContext.infTable[16] & 0x4000 - Spoke to Goron Link
gSaveContext.infTable[16] & 0x8000 - (not on cloud modding)

gSaveContext.infTable[17] & 0x4000 - Bomb bag upgrade obtained from rolling Goron

EnGo
pathIndex: this->actor.params & 0xF
Goron: this->actor.params & 0xF0

EnGo2
(this->actor.params & 0x3E0) >> 5
(this->actor.params & 0xFC00) >> 0xA - Gorons in Fire Temple
this->actor.params & 0x1F

Gorons only move when this->unk_194.unk_00 == 0
*/

void EnGo2_Init(Actor* thisx, PlayState* play);
void EnGo2_Destroy(Actor* thisx, PlayState* play);
void EnGo2_Update(Actor* thisx, PlayState* play);
void EnGo2_Draw(Actor* thisx, PlayState* play);

void EnGo2_StopRolling(EnGo2* this, PlayState* play);
void EnGo2_CurledUp(EnGo2* this, PlayState* play);

void func_80A46B40(EnGo2* this, PlayState* play);
void EnGo2_GoronDmtBombFlowerAnimation(EnGo2* this, PlayState* play);
void EnGo2_GoronRollingBigContinueRolling(EnGo2* this, PlayState* play);
void EnGo2_ContinueRolling(EnGo2* this, PlayState* play);
void EnGo2_SlowRolling(EnGo2* this, PlayState* play);
void EnGo2_GroundRolling(EnGo2* this, PlayState* play);

void EnGo2_ReverseRolling(EnGo2* this, PlayState* play);
void EnGo2_SetupGetItem(EnGo2* this, PlayState* play);
void EnGo2_SetGetItem(EnGo2* this, PlayState* play);
void EnGo2_BiggoronEyedrops(EnGo2* this, PlayState* play);
void EnGo2_GoronLinkStopRolling(EnGo2* this, PlayState* play);
void EnGo2_GoronFireGenericAction(EnGo2* this, PlayState* play);

static void* sDustTex[] = { gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex };

static Vec3f sPos = { 0.0f, 0.0f, 0.0f };
static Vec3f sVelocity = { 0.0f, 0.0f, 0.0f };
static Vec3f sAccel = { 0.0f, 0.3f, 0.0f };

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 40, 65, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = {
    0, 0, 0, 0, MASS_IMMOVABLE,
};

const ActorInit En_Go2_InitVars = {
    ACTOR_EN_GO2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OF1D_MAP,
    sizeof(EnGo2),
    (ActorFunc)EnGo2_Init,
    (ActorFunc)EnGo2_Destroy,
    (ActorFunc)EnGo2_Update,
    (ActorFunc)EnGo2_Draw,
    NULL,
};

static EnGo2DataStruct1 D_80A4816C[14] = {
    { 0, 0, 0, 68, 148 }, { 0, 0, 0, 24, 52 }, { 0, 320, 380, 400, 120 }, { 0, 0, 0, 30, 68 }, { 0, 0, 0, 46, 90 },
    { 0, 0, 0, 30, 68 },  { 0, 0, 0, 30, 68 }, { 0, 0, 0, 30, 68 },       { 0, 0, 0, 30, 68 }, { 0, 0, 0, 30, 68 },
    { 0, 0, 0, 30, 68 },  { 0, 0, 0, 30, 68 }, { 0, 0, 0, 30, 68 },       { 0, 0, 0, 30, 68 },
};

static EnGo2DataStruct2 D_80A481F8[14] = {
    { 30.0f, 0.026f, 6, 60.0f }, { 24.0f, 0.008f, 6, 30.0f }, { 28.0f, 0.16f, 5, 380.0f }, { 28.0f, 0.01f, 7, 40.0f },
    { 30.0f, 0.015f, 6, 30.0f }, { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },
    { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },
    { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },
};

static f32 D_80A482D8[14][2] = {
    { 80.0f, 80.0f }, { -10.0f, -10.0f }, { 800.0f, 800.0f }, { 0.0f, 0.0f },   { 20.0f, 40.0f },
    { 20.0f, 20.0f }, { 20.0f, 20.0f },   { 20.0f, 20.0f },   { 20.0f, 20.0f }, { 20.0f, 20.0f },
    { 20.0f, 20.0f }, { 20.0f, 20.0f },   { 20.0f, 20.0f },   { 20.0f, 20.0f },
};

typedef enum {
    /*  0 */ ENGO2_ANIM_0,
    /*  1 */ ENGO2_ANIM_1,
    /*  2 */ ENGO2_ANIM_2,
    /*  3 */ ENGO2_ANIM_3,
    /*  4 */ ENGO2_ANIM_4,
    /*  5 */ ENGO2_ANIM_5,
    /*  6 */ ENGO2_ANIM_6,
    /*  7 */ ENGO2_ANIM_7,
    /*  8 */ ENGO2_ANIM_8,
    /*  9 */ ENGO2_ANIM_9,
    /* 10 */ ENGO2_ANIM_10,
    /* 11 */ ENGO2_ANIM_11,
    /* 12 */ ENGO2_ANIM_12,
    /* 13 */ ENGO2_ANIM_13, // Added to fix spinning goron issue for biggoron
} EnGo2Animation;

static AnimationInfo sAnimationInfo[] = {
    { &gGoronAnim_004930, 0.0f, 0.0f, -1.0f, 0x00, 0.0f },  { &gGoronAnim_004930, 0.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_0029A8, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_010590, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_003768, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_0038E4, 1.0f, 0.0f, -1.0f, 0x02, -8.0f },
    { &gGoronAnim_002D80, 1.0f, 0.0f, -1.0f, 0x02, -8.0f }, { &gGoronAnim_00161C, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_001A00, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_0021D0, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_004930, 0.0f, 0.0f, -1.0f, 0x01, -8.0f }, { &gGoronAnim_000750, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_000D5C, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_004930, 0.0f, 1.0f, -1.0f, 0x01,  0.0f },
};

static EnGo2DustEffectData sDustEffectData[2][4] = {
    {
        { 12, 0.2f, 0.2f, 1, 18.0f, 0.0f },
        { 12, 0.1f, 0.2f, 12, 26.0f, 0.0f },
        { 12, 0.1f, 0.3f, 4, 10.0f, 0.0f },
        { 12, 0.2f, 0.2f, 1, 18.0f, 0.0f },
    },
    {
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
    },
};

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

void EnGo2_AddDust(EnGo2* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 initialTimer, f32 scale, f32 scaleStep) {
    EnGoEffect* dustEffect = this->dustEffects;
    s16 i;
    s16 timer;

    for (i = 0; i < ARRAY_COUNT(this->dustEffects); i++, dustEffect++) {
        if (dustEffect->type != 1) {
            dustEffect->epoch++;
            dustEffect->scale = scale;
            dustEffect->scaleStep = scaleStep;
            timer = initialTimer;
            dustEffect->timer = timer;
            dustEffect->type = 1;
            dustEffect->initialTimer = initialTimer;
            dustEffect->pos = *pos;
            dustEffect->accel = *accel;
            dustEffect->velocity = *velocity;
            return;
        }
    }
}

void EnGo2_UpdateDust(EnGo2* this) {
    EnGoEffect* dustEffect = this->dustEffects;
    f32 randomNumber;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->dustEffects); i++, dustEffect++) {
        if (dustEffect->type) {
            dustEffect->timer--;
            if (dustEffect->timer == 0) {
                dustEffect->type = 0;
            }
            dustEffect->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
            randomNumber = Rand_ZeroOne() * 0.4f;
            dustEffect->accel.z = randomNumber - 0.2f;
            dustEffect->pos.x += dustEffect->velocity.x;
            dustEffect->pos.y += dustEffect->velocity.y;
            dustEffect->pos.z += dustEffect->velocity.z;
            dustEffect->velocity.x += dustEffect->accel.x;
            dustEffect->velocity.y += dustEffect->accel.y;
            dustEffect->velocity.z += randomNumber - 0.2f;
            dustEffect->scale += dustEffect->scaleStep;
        }
    }
}

void EnGo2_DrawDust(EnGo2* this, PlayState* play) {
    EnGoEffect* dustEffect = this->dustEffects;
    s16 alpha;
    s16 firstDone;
    s16 index;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);

    firstDone = false;
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(this->dustEffects); i++, dustEffect++) {
        if (dustEffect->type) {
            if (!firstDone) {
                POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD40);
                gDPSetEnvColor(POLY_XLU_DISP++, 100, 60, 20, 0);
                firstDone = true;
            }

            FrameInterpolation_RecordOpenChild(dustEffect, dustEffect->epoch);
            alpha = dustEffect->timer * (255.0f / dustEffect->initialTimer);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 130, 90, alpha);
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_Translate(dustEffect->pos.x, dustEffect->pos.y, dustEffect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(dustEffect->scale, dustEffect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            index = dustEffect->timer * (8.0f / dustEffect->initialTimer);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sDustTex[index]));
            gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD50);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnGo2_SpawnDust(EnGo2* this, u8 initialTimer, f32 scale, f32 scaleStep, s32 numDustEffects, f32 radius,
                    f32 yAccel) {
    Vec3f pos = sPos;
    Vec3f velocity = sVelocity;
    Vec3f accel = sAccel;
    s32 i;
    s16 angle;

    pos = this->actor.world.pos; // overwrites sPos data
    pos.y = this->actor.floorHeight;
    angle = (Rand_ZeroOne() - 0.5f) * 0x10000;
    i = numDustEffects;
    while (i >= 0) {
        accel.y += Rand_ZeroOne() * yAccel;
        pos.x = (Math_SinS(angle) * radius) + this->actor.world.pos.x;
        pos.z = (Math_CosS(angle) * radius) + this->actor.world.pos.z;
        EnGo2_AddDust(this, &pos, &velocity, &accel, initialTimer, scale, scaleStep);
        angle += (s16)(0x10000 / numDustEffects);
        i--;
    }
    return 0;
}

void EnGo2_GetItem(EnGo2* this, PlayState* play, s32 getItemId) {
    this->getItemId = getItemId;
    func_8002F434(&this->actor, play, getItemId, this->actor.xzDistToPlayer + 1.0f,
                  fabsf(this->actor.yDistToPlayer) + 1.0f);
}

void EnGo2_GetItemEntry(EnGo2* this, PlayState* play, GetItemEntry getItemEntry) {
    this->getItemId = getItemEntry.getItemId;
    this->getItemEntry = getItemEntry;
    GiveItemEntryFromActor(&this->actor, play, getItemEntry, this->actor.xzDistToPlayer + 1.0f,
                  fabsf(this->actor.yDistToPlayer) + 1.0f);
}

s32 EnGo2_GetDialogState(EnGo2* this, PlayState* play) {
    s16 dialogState = Message_GetState(&play->msgCtx);

    if ((this->dialogState == TEXT_STATE_AWAITING_NEXT) || (this->dialogState == TEXT_STATE_EVENT) ||
        (this->dialogState == TEXT_STATE_CLOSING) || (this->dialogState == TEXT_STATE_DONE_HAS_NEXT)) {
        if (dialogState != this->dialogState) {
            this->unk_20C++;
        }
    }

    this->dialogState = dialogState;
    return dialogState;
}

u16 EnGo2_GoronFireGenericGetTextId(EnGo2* this) {
    switch ((this->actor.params & 0xFC00) >> 0xA) {
        case 3:
            return 0x3069;
        case 5:
            return 0x306A;
        case 4:
            return 0x306B;
        case 2:
            return 0x306C;
        case 10:
            return 0x306D;
        case 8:
            return 0x306E;
        case 11:
            return 0x306F;
        case 1:
            return 0x3070;
        default:
            return 0x3052;
    }
}

u16 EnGo2_GetTextIdGoronCityRollingBig(PlayState* play, EnGo2* this) {
    if (gSaveContext.infTable[17] & 0x4000) {
        return 0x3013;
    } else if ((CUR_CAPACITY(UPG_BOMB_BAG) >= 20 || gSaveContext.n64ddFlag) && this->waypoint > 7 && this->waypoint < 12) {
        return 0x3012;
    } else {
        return 0x3011;
    }
}

s16 EnGo2_GetStateGoronCityRollingBig(PlayState* play, EnGo2* this) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            return 2;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                if (this->actor.textId == 0x3012) {
                    this->actionFunc = EnGo2_SetupGetItem;
                    if(!gSaveContext.n64ddFlag) {
                        EnGo2_GetItem(this, play, CUR_CAPACITY(UPG_BOMB_BAG) == 30 ? GI_BOMB_BAG_40 : GI_BOMB_BAG_30);
                    } else {
                        EnGo2_GetItemEntry(this, play, Randomizer_GetItemFromKnownCheck(RC_GC_ROLLING_GORON_AS_CHILD, GI_BOMB_BAG_40));
                    }
                    Message_CloseTextbox(play);
                    gSaveContext.infTable[17] |= 0x4000;
                    return 2;
                } else {
                    return 2;
                }
            }
        default:
            return 1;
    }
}

u16 EnGo2_GetTextIdGoronDmtBombFlower(PlayState* play, EnGo2* this) {
    return CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? 0x3027 : 0x300A;
}

// DMT Goron by Bomb Flower Choice
s16 EnGo2_GetStateGoronDmtBombFlower(PlayState* play, EnGo2* this) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            if ((this->actor.textId == 0x300B) && (gSaveContext.infTable[14] & 0x800) == 0) {
                gSaveContext.infTable[14] |= 0x800;
                return 2;
            } else {
                return 0;
            }
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                // Ask question to DMT Goron by bomb flower
                if (this->actor.textId == 0x300A) {
                    if (play->msgCtx.choiceIndex == 0) {
                        this->actor.textId = CUR_UPG_VALUE(UPG_STRENGTH) != 0 ? 0x300B : 0x300C;
                    } else {
                        this->actor.textId = 0x300D;
                    }
                    Message_ContinueTextbox(play, this->actor.textId);
                }
                return 1;
            }
        default:
            return 1;
    }
}

u16 EnGo2_GetTextIdGoronDmtRollingSmall(PlayState* play, EnGo2* this) {
    if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 0x3027;
    } else {
        return (gSaveContext.eventChkInf[2] & 0x8) ? 0x3026 : 0x3009;
    }
}

s16 EnGo2_GetStateGoronDmtRollingSmall(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronDmtDcEntrance(PlayState* play, EnGo2* this) {
    if (((!gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) ||
         (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE))) && LINK_IS_ADULT) {
        return 0x3043;
    } else if ((!gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) ||
               (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DODONGOS_CAVERN))) {
        return 0x3027;
    } else {
        return gSaveContext.eventChkInf[2] & 0x8 ? 0x3021 : gSaveContext.infTable[14] & 0x1 ? 0x302A : 0x3008;
    }
}

s16 EnGo2_GetStateGoronDmtDcEntrance(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3008) {
            gSaveContext.infTable[14] |= 0x1;
        }
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityEntrance(PlayState* play, EnGo2* this) {
    if (((!gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) ||
         (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE))) && LINK_IS_ADULT) {
        return 0x3043;
    } else if ((!gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) ||
               (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DODONGOS_CAVERN))) {
        return 0x3027;
    } else {
        return gSaveContext.infTable[15] & 0x1 ? 0x3015 : 0x3014;
    }
}

s16 EnGo2_GetStateGoronCityEntrance(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3014) {
            gSaveContext.infTable[15] |= 0x1;
        }
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityIsland(PlayState* play, EnGo2* this) {
    if (((!gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) ||
         (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE))) && LINK_IS_ADULT) {
        return 0x3043;
    } else if ((!gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) ||
               (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DODONGOS_CAVERN))) {
        return 0x3027;
    } else {
        return gSaveContext.infTable[15] & 0x10 ? 0x3017 : 0x3016;
    }
}

s16 EnGo2_GetStateGoronCityIsland(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3016) {
            gSaveContext.infTable[15] |= 0x10;
        }
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityLowestFloor(PlayState* play, EnGo2* this) {
    if (((!gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) ||
         (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE))) && LINK_IS_ADULT) {
        return 0x3043;
    } else if ((!gSaveContext.n64ddFlag && CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) ||
               (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DODONGOS_CAVERN))) {
        return 0x3027;
    } else {
        return CUR_UPG_VALUE(UPG_STRENGTH) != 0    ? 0x302C
               : !Flags_GetSwitch(play, 0x1B) ? 0x3017
               : gSaveContext.infTable[15] & 0x100 ? 0x3019
                                                   : 0x3018;
    }
}

s16 EnGo2_GetStateGoronCityLowestFloor(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3018) {
            gSaveContext.infTable[15] |= 0x100;
        }
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityLink(PlayState* play, EnGo2* this) {
    if(gSaveContext.n64ddFlag) {
        return 0x3036;
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
        return gSaveContext.infTable[16] & 0x8000 ? 0x3042 : 0x3041;
    } else if (CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
        return gSaveContext.infTable[16] & 0x4000 ? 0x3038 : 0x3037;
    } else if (gSaveContext.infTable[16] & 0x1000) {
        this->unk_20C = 0;
        this->dialogState = TEXT_STATE_NONE;
        return gSaveContext.infTable[16] & 0x400 ? 0x3033 : 0x3032;
    } else {
        return 0x3030;
    }
}

s16 EnGo2_GetStateGoronCityLink(PlayState* play, EnGo2* this) {
    switch (EnGo2_GetDialogState(this, play)) {
        case TEXT_STATE_CLOSING:
            if(!gSaveContext.n64ddFlag) {
                switch (this->actor.textId) {
                    case 0x3036:
                        EnGo2_GetItem(this, play, GI_TUNIC_GORON);
                        this->actionFunc = EnGo2_SetupGetItem;
                        return 2;
                    case 0x3037:
                        gSaveContext.infTable[16] |= 0x4000;
                    default:
                        return 0;
                }
            } else {
                if (Flags_GetTreasure(play, 0x1F)) {
                    return 0;
                }
                
                gSaveContext.infTable[16] |= 0x200;
                EnGo2_GetItemEntry(this, play, Randomizer_GetItemFromKnownCheck(RC_GC_ROLLING_GORON_AS_ADULT, GI_TUNIC_GORON));
                this->actionFunc = EnGo2_SetupGetItem;
                Flags_SetTreasure(play, 0x1F);
                return 2;
            }

        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                if (this->actor.textId == 0x3034) {
                    if (play->msgCtx.choiceIndex == 0) {
                        this->actor.textId = gSaveContext.infTable[16] & 0x800 ? 0x3033 : 0x3035;
                        if (this->actor.textId == 0x3035) {
                            Audio_StopSfxById(NA_SE_EN_GOLON_CRY);
                        }
                    } else {
                        this->actor.textId = gSaveContext.infTable[16] & 0x800 ? 0x3036 : 0x3033;
                        if (this->actor.textId == 0x3036) {
                            Audio_StopSfxById(NA_SE_EN_GOLON_CRY);
                        }
                    }
                    Message_ContinueTextbox(play, this->actor.textId);
                    this->unk_20C = 0;
                }
            } else {
                break;
            }
            return 1;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (this->actor.textId) {
                    case 0x3035:
                        gSaveContext.infTable[16] |= 0x800;
                    case 0x3032:
                    case 0x3033:
                        this->actor.textId = 0x3034;
                        Message_ContinueTextbox(play, this->actor.textId);
                        return 1;
                    default:
                        return 2;
                }
            }
    }
    return 1;
}

u16 EnGo2_GetTextIdGoronDmtBiggoron(PlayState* play, EnGo2* this) {
    Player* player = GET_PLAYER(play);

    if (!gSaveContext.n64ddFlag && gSaveContext.bgsFlag) {
        player->exchangeItemId = EXCH_ITEM_CLAIM_CHECK;
        return 0x305E;
    } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_CLAIM_CHECK) {
        player->exchangeItemId = EXCH_ITEM_CLAIM_CHECK;
        return 0x305E;
    } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_PRESCRIPTION) {
        player->exchangeItemId = EXCH_ITEM_EYEDROPS;
        return 0x3058;
    } else {
        player->exchangeItemId = EXCH_ITEM_SWORD_BROKEN;
        return 0x3053;
    }
}

s16 EnGo2_GetStateGoronDmtBiggoron(PlayState* play, EnGo2* this) {
    s32 unusedPad;
    u8 dialogState = this->dialogState;

    switch (EnGo2_GetDialogState(this, play)) {
        case TEXT_STATE_DONE:
            if (this->actor.textId == 0x305E) {
                if((!gSaveContext.n64ddFlag && gSaveContext.bgsFlag) || (gSaveContext.n64ddFlag && Flags_GetTreasure(play, 0x1F))) {
                    return 0;
                }
                
                if(gSaveContext.n64ddFlag) {
                    EnGo2_GetItemEntry(this, play, Randomizer_GetItemFromKnownCheck(RC_DMT_TRADE_CLAIM_CHECK, GI_SWORD_BGS));
                    Flags_SetTreasure(play, 0x1F);
                } else {
                    EnGo2_GetItem(this, play, GI_SWORD_BGS);
                }
                this->actionFunc = EnGo2_SetupGetItem;
                return 2;
            } else {
                return 0;
            }
        case TEXT_STATE_DONE_FADING:
            switch (this->actor.textId) {
                case 0x305E:
                    if (func_8002F368(play) != EXCH_ITEM_CLAIM_CHECK) {
                        break;
                    }
                case 0x3059:
                    if (dialogState == TEXT_STATE_NONE) {
                        func_800F4524(&D_801333D4, NA_SE_EN_GOLON_WAKE_UP, 60);
                    }
                case 0x3054:
                    if (dialogState == TEXT_STATE_NONE) {
                        Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                    }
            }
            return 1;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                if ((this->actor.textId == 0x3054) || (this->actor.textId == 0x3055)) {
                    if (play->msgCtx.choiceIndex == 0) {
                        if (gSaveContext.n64ddFlag) {
                            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_DMT_TRADE_BROKEN_SWORD, GI_PRESCRIPTION);
                            Randomizer_ConsumeAdultTradeItem(play, ITEM_SWORD_BROKEN);
                            EnGo2_GetItemEntry(this, play, getItemEntry);
                        } else {
                            u32 getItemId = GI_PRESCRIPTION;
                            EnGo2_GetItem(this, play, getItemId);
                        }
                        this->actionFunc = EnGo2_SetupGetItem;
                        return 2;
                    }
                    this->actor.textId = 0x3056;
                    Message_ContinueTextbox(play, this->actor.textId);
                }
                return 1;
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                if (this->actor.textId == 0x3059) {
                    play->msgCtx.msgMode = MSGMODE_PAUSED;
                    this->actionFunc = EnGo2_BiggoronEyedrops;
                }
                return 2;
            }
    }
    return 1;
}

u16 EnGo2_GetTextIdGoronFireGeneric(PlayState* play, EnGo2* this) {
    if (Flags_GetSwitch(play, (this->actor.params & 0xFC00) >> 0xA)) {
        return 0x3071;
    } else {
        return 0x3051;
    }
}

s16 EnGo2_GetStateGoronFireGeneric(PlayState* play, EnGo2* this) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            return 0;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                if (this->actor.textId == 0x3071) {
                    this->actor.textId = EnGo2_GoronFireGenericGetTextId(this);
                    Message_ContinueTextbox(play, this->actor.textId);
                }
                return 1;
            }
        default:
            return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityStairwell(PlayState* play, EnGo2* this) {
    return !LINK_IS_ADULT ? gSaveContext.infTable[14] & 0x8 ? 0x3022 : 0x300E : 0x3043;
}

s16 EnGo2_GetStateGoronCityStairwell(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x300E) {
            gSaveContext.infTable[14] |= 0x8;
        }
        return 0;
    } else {
        return 1;
    }
}

// Goron in child market bazaar after obtaining Goron Ruby
u16 EnGo2_GetTextIdGoronMarketBazaar(PlayState* play, EnGo2* this) {
    return 0x7122;
}

s16 EnGo2_GetStateGoronMarketBazaar(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextIdGoronCityLostWoods(PlayState* play, EnGo2* this) {
    if (!LINK_IS_ADULT) {
        if (Flags_GetSwitch(play, 0x1C)) {
            return 0x302F;
        } else {
            return gSaveContext.infTable[14] & 0x40 ? 0x3025 : 0x3024;
        }
    } else {
        return 0x3043;
    }
}

s16 EnGo2_GetStateGoronCityLostWoods(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (this->actor.textId == 0x3024) {
            gSaveContext.infTable[14] |= 0x40;
        }
        return 0;
    } else {
        return 1;
    }
}

// Goron at base of DMT summit
u16 EnGo2_GetTextIdGoronDmtFairyHint(PlayState* play, EnGo2* this) {
    if (!LINK_IS_ADULT) {
        return CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? 0x3065 : 0x3064;
    } else {
        return 0x3043;
    }
}

s16 EnGo2_GetStateGoronDmtFairyHint(PlayState* play, EnGo2* this) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return 0;
    } else {
        return 1;
    }
}

u16 EnGo2_GetTextId(PlayState* play, Actor* thisx) {
    EnGo2* this = (EnGo2*)thisx;
    u16 faceReaction = Text_GetFaceReaction(play, 0x20);

    if (faceReaction) {
        return faceReaction;
    } else {
        switch (this->actor.params & 0x1F) {
            case GORON_CITY_ROLLING_BIG:
                return EnGo2_GetTextIdGoronCityRollingBig(play, this);
            case GORON_CITY_LINK:
                return EnGo2_GetTextIdGoronCityLink(play, this);
            case GORON_DMT_BIGGORON:
                return EnGo2_GetTextIdGoronDmtBiggoron(play, this);
            case GORON_FIRE_GENERIC:
                return EnGo2_GetTextIdGoronFireGeneric(play, this);
            case GORON_DMT_BOMB_FLOWER:
                return EnGo2_GetTextIdGoronDmtBombFlower(play, this);
            case GORON_DMT_ROLLING_SMALL:
                return EnGo2_GetTextIdGoronDmtRollingSmall(play, this);
            case GORON_DMT_DC_ENTRANCE:
                return EnGo2_GetTextIdGoronDmtDcEntrance(play, this);
            case GORON_CITY_ENTRANCE:
                return EnGo2_GetTextIdGoronCityEntrance(play, this);
            case GORON_CITY_ISLAND:
                return EnGo2_GetTextIdGoronCityIsland(play, this);
            case GORON_CITY_LOWEST_FLOOR:
                return EnGo2_GetTextIdGoronCityLowestFloor(play, this);
            case GORON_CITY_STAIRWELL:
                return EnGo2_GetTextIdGoronCityStairwell(play, this);
            case GORON_CITY_LOST_WOODS:
                return EnGo2_GetTextIdGoronCityLostWoods(play, this);
            case GORON_DMT_FAIRY_HINT:
                return EnGo2_GetTextIdGoronDmtFairyHint(play, this);
            case GORON_MARKET_BAZAAR:
                return EnGo2_GetTextIdGoronMarketBazaar(play, this);
        }
    }
}

s16 EnGo2_GetState(PlayState* play, Actor* thisx) {
    EnGo2* this = (EnGo2*)thisx;
    switch (this->actor.params & 0x1F) {
        case GORON_CITY_ROLLING_BIG:
            return EnGo2_GetStateGoronCityRollingBig(play, this);
        case GORON_CITY_LINK:
            return EnGo2_GetStateGoronCityLink(play, this);
        case GORON_DMT_BIGGORON:
            return EnGo2_GetStateGoronDmtBiggoron(play, this);
        case GORON_FIRE_GENERIC:
            return EnGo2_GetStateGoronFireGeneric(play, this);
        case GORON_DMT_BOMB_FLOWER:
            return EnGo2_GetStateGoronDmtBombFlower(play, this);
        case GORON_DMT_ROLLING_SMALL:
            return EnGo2_GetStateGoronDmtRollingSmall(play, this);
        case GORON_DMT_DC_ENTRANCE:
            return EnGo2_GetStateGoronDmtDcEntrance(play, this);
        case GORON_CITY_ENTRANCE:
            return EnGo2_GetStateGoronCityEntrance(play, this);
        case GORON_CITY_ISLAND:
            return EnGo2_GetStateGoronCityIsland(play, this);
        case GORON_CITY_LOWEST_FLOOR:
            return EnGo2_GetStateGoronCityLowestFloor(play, this);
        case GORON_CITY_STAIRWELL:
            return EnGo2_GetStateGoronCityStairwell(play, this);
        case GORON_CITY_LOST_WOODS:
            return EnGo2_GetStateGoronCityLostWoods(play, this);
        case GORON_DMT_FAIRY_HINT:
            return EnGo2_GetStateGoronDmtFairyHint(play, this);
        case GORON_MARKET_BAZAAR:
            return EnGo2_GetStateGoronMarketBazaar(play, this);
    }
}

s32 func_80A44790(EnGo2* this, PlayState* play) {
    if ((this->actor.params & 0x1F) != GORON_DMT_BIGGORON && (this->actor.params & 0x1F) != GORON_CITY_ROLLING_BIG) {
        return func_800343CC(play, &this->actor, &this->unk_194.unk_00, this->unk_218, EnGo2_GetTextId,
                             EnGo2_GetState);
    } else if (((this->actor.params & 0x1F) == GORON_DMT_BIGGORON) && ((this->collider.base.ocFlags2 & 1) == 0)) {
        return false;
    } else {
        if (Actor_ProcessTalkRequest(&this->actor, play)) {
            this->unk_194.unk_00 = 1;
            return true;
        } else if (this->unk_194.unk_00 != 0) {
            this->unk_194.unk_00 = EnGo2_GetState(play, &this->actor);
            return false;
        } else if (func_8002F2CC(&this->actor, play, this->unk_218)) {
            this->actor.textId = EnGo2_GetTextId(play, &this->actor);
        }
        return false;
    }
}

void EnGo2_SetColliderDim(EnGo2* this) {
    u8 index = this->actor.params & 0x1F;

    this->collider.dim.radius = D_80A4816C[index].radius;
    this->collider.dim.height = D_80A4816C[index].height;
}

void EnGo2_SetShape(EnGo2* this) {
    u8 index = this->actor.params & 0x1F;

    this->actor.shape.shadowScale = D_80A481F8[index].shape_unk_10;
    Actor_SetScale(&this->actor, D_80A481F8[index].scale);
    this->actor.targetMode = D_80A481F8[index].actor_unk_1F;
    this->unk_218 = D_80A481F8[index].unk_218;
    this->unk_218 += this->collider.dim.radius;
}

void EnGo2_CheckCollision(EnGo2* this, PlayState* play) {
    Vec3s pos;
    f32 xzDist;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z = this->actor.world.pos.z;
    xzDist = D_80A4816C[this->actor.params & 0x1F].xzDist;
    pos.x += (s16)(xzDist * Math_SinS(this->actor.shape.rot.y));
    pos.z += (s16)(xzDist * Math_CosS(this->actor.shape.rot.y));
    pos.y += D_80A4816C[this->actor.params & 0x1F].yDist;
    this->collider.dim.pos = pos;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void EnGo2_SwapInitialFrameAnimFrameCount(EnGo2* this) {
    f32 initialFrame;

    initialFrame = this->skelAnime.startFrame;
    this->skelAnime.startFrame = this->skelAnime.endFrame;
    this->skelAnime.endFrame = initialFrame;
}

s32 func_80A44AB0(EnGo2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 arg2;

    if ((this->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        return false;
    } else {
        if ((this->actionFunc != EnGo2_SlowRolling) && (this->actionFunc != EnGo2_ReverseRolling) &&
            (this->actionFunc != EnGo2_ContinueRolling)) {
            return false;
        } else {
            if (this->collider.base.acFlags & 2) {
                Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->actor.flags &= ~ACTOR_FLAG_24;
                this->collider.base.acFlags &= ~0x2;
                EnGo2_StopRolling(this, play);
                return true;
            }
            if (player->invincibilityTimer <= 0) {
                this->collider.base.ocFlags1 |= 8;
            } else {
                return false;
            }
            if (this->collider.base.ocFlags2 & 1) {
                this->collider.base.ocFlags2 &= ~1;

                arg2 = this->actionFunc == EnGo2_ContinueRolling ? 1.5f : this->actor.speedXZ * 1.5f;

                play->damagePlayer(play, -4);
                func_8002F71C(play, &this->actor, arg2, this->actor.yawTowardsPlayer, 6.0f);
                Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
                this->collider.base.ocFlags1 &= ~0x8;
            }
        }
    }
    return false;
}

s32 EnGo2_UpdateWaypoint(EnGo2* this, PlayState* play) {
    s32 change;

    if (this->path == NULL) {
        return 0;
    }

    change = (u8)(this->path->count - 1);
    if (this->reverse) {
        this->waypoint--;
        if (this->waypoint < 0) {
            this->waypoint = change - 1;
        }
    } else {
        this->waypoint++;
        if (this->waypoint >= change) {
            this->waypoint = 0;
        }
    }

    return 1;
}

s32 EnGo2_Orient(EnGo2* this, PlayState* play) {
    s16 targetYaw;
    f32 waypointDistSq = Path_OrientAndGetDistSq(&this->actor, this->path, this->waypoint, &targetYaw);

    Math_SmoothStepToS(&this->actor.world.rot.y, targetYaw, 6, 4000, 1);
    if (waypointDistSq > 0.0f && waypointDistSq < SQ(30.0f)) {
        return EnGo2_UpdateWaypoint(this, play);
    } else {
        return 0;
    }
}

s32 func_80A44D84(EnGo2* this) {
    s16 targetYaw;

    Path_OrientAndGetDistSq(&this->actor, this->path, this->waypoint, &targetYaw);
    this->actor.world.rot.y = targetYaw;
    return 1;
}

s32 EnGo2_IsWakingUp(EnGo2* this) {
    s16 yawDiff;
    f32 xyzDist = (this->actor.params & 0x1F) == GORON_DMT_BIGGORON ? 800.0f : 200.0f;
    f32 yDist = (this->actor.params & 0x1F) == GORON_DMT_BIGGORON ? 400.0f : 60.0f;
    s16 yawDiffAbs;

    if ((this->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        if ((this->collider.base.ocFlags2 & 1) == 0) {
            this->actor.flags &= ~ACTOR_FLAG_0;
            return false;
        } else {
            this->actor.flags |= ACTOR_FLAG_0;
            return true;
        }
    }

    xyzDist = SQ(xyzDist);
    yawDiff = (f32)this->actor.yawTowardsPlayer - (f32)this->actor.shape.rot.y;
    yawDiffAbs = ABS(yawDiff);
    if (this->actor.xyzDistToPlayerSq <= xyzDist && fabsf(this->actor.yDistToPlayer) < yDist && yawDiffAbs < 0x2AA8) {
        return true;
    } else {
        return false;
    }
}

s32 EnGo2_IsRollingOnGround(EnGo2* this, s16 arg1, f32 arg2, s16 arg3) {
    if ((this->actor.bgCheckFlags & 1) == 0 || this->actor.velocity.y > 0.0f) {
        return false;
    }

    if (DECR(this->unk_590)) {
        if (!arg3) {
            return true;
        } else {
            this->actor.world.pos.y =
                (this->unk_590 & 1) ? this->actor.world.pos.y + 1.5f : this->actor.world.pos.y - 1.5f;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
            return true;
        }
    }

    if (this->unk_59C >= 2) {
        Audio_PlayActorSound2(&this->actor, (this->actor.params & 0x1F) == GORON_CITY_ROLLING_BIG
                                                ? NA_SE_EN_GOLON_LAND_BIG
                                                : NA_SE_EN_DODO_M_GND);
    }

    this->unk_59C--;
    if (this->unk_59C <= 0) {
        if (this->unk_59C == 0) {
            this->unk_590 = Rand_S16Offset(60, 30);
            this->unk_59C = 0;
            this->actor.velocity.y = 0.0f;
            return true;
        } else {
            this->unk_59C = arg1;
        }
    }

    this->actor.velocity.y = ((f32)this->unk_59C / (f32)arg1) * arg2;
    return true;
}

void EnGo2_BiggoronSetTextId(EnGo2* this, PlayState* play, Player* player) {
    u16 textId;

    if ((this->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        if ((!gSaveContext.n64ddFlag && gSaveContext.bgsFlag)) {
            if (func_8002F368(play) == EXCH_ITEM_CLAIM_CHECK) {
                this->actor.textId = 0x3003;
            } else {
                this->actor.textId = 0x305E;
            }
            player->actor.textId = this->actor.textId;

        } else if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_CLAIM_CHECK) {
            if (func_8002F368(play) == EXCH_ITEM_CLAIM_CHECK) {
                if (gSaveContext.n64ddFlag && Flags_GetTreasure(play, 0x1F)) {
                    textId = 0x3003;
                } else if (Environment_GetBgsDayCount() >= CVar_GetS32("gForgeTime", 3)) {
                    textId = 0x305E;
                } else {
                    textId = 0x305D;
                }
                this->actor.textId = textId;
            } else {
                if (gSaveContext.n64ddFlag && Flags_GetTreasure(play, 0x1F)) {
                    textId = 0x305E;
                } else if (Environment_GetBgsDayCount() >= CVar_GetS32("gForgeTime", 3)) {
                    textId = 0x3002;
                } else {
                    textId = 0x305D;
                }
                this->actor.textId = textId;
            }
            player->actor.textId = this->actor.textId;

        } else if ((INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_PRESCRIPTION) &&
                   (INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_CLAIM_CHECK)) {
            if (func_8002F368(play) == EXCH_ITEM_EYEDROPS) {
                this->actor.textId = 0x3059;
            } else {
                this->actor.textId = 0x3058;
            }
            if (this->actor.textId == 0x3059) {
                gSaveContext.timer2State = 0;
            }
            player->actor.textId = this->actor.textId;

        } else if (INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_SWORD_BROKEN) {
            if (func_8002F368(play) == EXCH_ITEM_SWORD_BROKEN) {
                if (gSaveContext.infTable[11] & 0x10) {
                    textId = 0x3055;
                } else {
                    textId = 0x3054;
                }
                this->actor.textId = textId;
            } else {
                this->actor.textId = 0x3053;
            }
            player->actor.textId = this->actor.textId;
        } else {
            this->actor.textId = 0x3053;
            player->actor.textId = this->actor.textId;
        }
    }
}

void func_80A45288(EnGo2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 linkAge;

    if (this->actionFunc != EnGo2_GoronFireGenericAction) {
        this->unk_194.unk_18 = player->actor.world.pos;
        linkAge = gSaveContext.linkAge;
        this->unk_194.unk_14 = D_80A482D8[this->actor.params & 0x1F][linkAge];
        func_80034A14(&this->actor, &this->unk_194, 4, this->unk_26E);
    }
    if ((this->actionFunc != EnGo2_SetGetItem) && (this->isAwake == true)) {
        if (func_80A44790(this, play)) {
            EnGo2_BiggoronSetTextId(this, play, player);
        }
    }
}

void func_80A45360(EnGo2* this, f32* alpha) {
    f32 alphaTarget =
        (this->skelAnime.animation == &gGoronAnim_004930) && (this->skelAnime.curFrame <= 32.0f) ? 0.0f : 255.0f;

    Math_ApproachF(alpha, alphaTarget, 0.4f, 100.0f);
    this->actor.shape.shadowAlpha = (u8)(u32)*alpha;
}

void EnGo2_RollForward(EnGo2* this) {
    f32 speedXZ = this->actor.speedXZ;

    if (this->unk_194.unk_00 != 0) {
        this->actor.speedXZ = 0.0f;
    }

    if (this->actionFunc != EnGo2_ContinueRolling) {
        Actor_MoveForward(&this->actor);
    }

    this->actor.speedXZ = speedXZ;
}

void func_80A454CC(EnGo2* this) {
    switch (this->actor.params & 0x1F) {
        case GORON_CITY_ROLLING_BIG:
        case GORON_DMT_DC_ENTRANCE:
        case GORON_CITY_ENTRANCE:
        case GORON_CITY_STAIRWELL:
        case GORON_DMT_FAIRY_HINT:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_9);
            break;
        case GORON_DMT_BIGGORON:
            if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_SWORD_BROKEN && INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_EYEDROPS) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_4);
                break;
            }
        default:
            this->skelAnime.playSpeed = 0.0f;
            break;
    }
}

f32 EnGo2_GetTargetXZSpeed(EnGo2* this) {
    f32 yDist = (this->actor.params & 0x1F) == GORON_DMT_BIGGORON ? 400.0f : 60.0f;
    s32 index = this->actor.params & 0x1F;

    if (index == GORON_CITY_LINK && (fabsf(this->actor.yDistToPlayer) < yDist) &&
        (this->actor.xzDistToPlayer < 400.0f)) {
        return 9.0f;
    } else {
        return index == GORON_CITY_ROLLING_BIG ? 3.6000001f : 6.0f;
    }
}

s32 EnGo2_IsCameraModified(EnGo2* this, PlayState* play) {
    Camera* camera = play->cameraPtrs[MAIN_CAM];

    if ((this->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        if (EnGo2_IsWakingUp(this)) {
            Camera_ChangeSetting(camera, CAM_SET_DIRECTED_YAW);
            func_8005AD1C(camera, 4);
        } else if (!EnGo2_IsWakingUp(this) && (camera->setting == CAM_SET_DIRECTED_YAW)) {
            Camera_ChangeSetting(camera, CAM_SET_DUNGEON1);
            func_8005ACFC(camera, 4);
        }
    }

    if ((this->actor.params & 0x1F) == GORON_FIRE_GENERIC || (this->actor.params & 0x1F) == GORON_CITY_ROLLING_BIG ||
        (this->actor.params & 0x1F) == GORON_CITY_STAIRWELL || (this->actor.params & 0x1F) == GORON_DMT_BIGGORON ||
        (this->actor.params & 0x1F) == GORON_MARKET_BAZAAR) {
        return true;
    } else if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
        return true;
    } else {
        return false;
    }
}

void EnGo2_DefaultWakingUp(EnGo2* this) {
    if (EnGo2_IsWakingUp(this)) {
        this->unk_26E = 2;
    } else {
        this->unk_26E = 1;
    }

    if (this->unk_194.unk_00 != 0) {
        this->unk_26E = 4;
    }

    this->isAwake = true;
}

void EnGo2_WakingUp(EnGo2* this) {
    f32 xyzDist = (this->actor.params & 0x1F) == GORON_DMT_BIGGORON ? 800.0f : 200.0f;
    s32 isTrue = true;

    xyzDist = SQ(xyzDist);
    this->unk_26E = 1;
    if ((this->actor.xyzDistToPlayerSq <= xyzDist) || (this->unk_194.unk_00 != 0)) {
        this->unk_26E = 4;
    }

    this->isAwake = isTrue;
}

void EnGo2_BiggoronWakingUp(EnGo2* this) {
    if (EnGo2_IsWakingUp(this) || this->unk_194.unk_00 != 0) {
        this->unk_26E = 2;
        this->isAwake = true;
    } else {
        this->unk_26E = 1;
        this->isAwake = false;
    }
}

void EnGo2_SelectGoronWakingUp(EnGo2* this) {
    switch (this->actor.params & 0x1F) {
        case GORON_DMT_BOMB_FLOWER:
            this->isAwake = true;
            this->unk_26E = EnGo2_IsWakingUp(this) ? 2 : 1;
            break;
        case GORON_FIRE_GENERIC:
            EnGo2_WakingUp(this);
            break;
        case GORON_DMT_BIGGORON:
            EnGo2_BiggoronWakingUp(this);
            break;
        case GORON_CITY_LINK:
            if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
                EnGo2_WakingUp(this);
                break;
            }
        default:
            EnGo2_DefaultWakingUp(this);
    }
}

void EnGo2_EyeMouthTexState(EnGo2* this) {
    switch (this->eyeMouthTexState) {
        case 1:
            this->blinkTimer = 0;
            this->eyeTexIndex = 0;
            this->mouthTexIndex = 0;
            break;
        case 2:
            this->blinkTimer = 0;
            this->eyeTexIndex = 1;
            this->mouthTexIndex = 0;
            break;
        // case 3 only when biggoron is given eyedrops. Biggoron smiles. (only use of second mouth texture)
        case 3:
            this->blinkTimer = 0;
            this->eyeTexIndex = 0;
            this->mouthTexIndex = 1;
            break;
        default:
            if (DECR(this->blinkTimer) == 0) {
                this->eyeTexIndex++;
                if (this->eyeTexIndex >= 4) {
                    this->blinkTimer = Rand_S16Offset(30, 30);
                    this->eyeTexIndex = 1;
                }
            }
    }
}

void EnGo2_SitDownAnimation(EnGo2* this) {
    if ((this->skelAnime.playSpeed != 0.0f) && (this->skelAnime.animation == &gGoronAnim_004930)) {
        if (this->skelAnime.playSpeed > 0.0f && this->skelAnime.curFrame == 14.0f) {
            if ((this->actor.params & 0x1F) != GORON_DMT_BIGGORON) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOLON_SIT_DOWN);
            } else {
                func_800F4524(&D_801333D4, NA_SE_EN_GOLON_SIT_DOWN, 60);
            }
        }
        if (this->skelAnime.playSpeed < 0.0f) {
            if (this->skelAnime.curFrame == 1.0f) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
            }
            if (this->skelAnime.curFrame == 40.0f) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOLON_SIT_DOWN);
            }
        }
    }
}

void EnGo2_GetDustData(EnGo2* this, s32 index2) {
    s32 index1 = (this->actor.params & 0x1F) == GORON_CITY_ROLLING_BIG ? 1 : 0;
    EnGo2DustEffectData* dustEffectData = &sDustEffectData[index1][index2];

    EnGo2_SpawnDust(this, dustEffectData->initialTimer, dustEffectData->scale, dustEffectData->scaleStep,
                    dustEffectData->numDustEffects, dustEffectData->radius, dustEffectData->yAccel);
}

void EnGo2_RollingAnimation(EnGo2* this, PlayState* play) {
    if ((this->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        this->actor.flags &= ~ACTOR_FLAG_0;
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_10);
        this->skelAnime.playSpeed = -0.5f;
    } else {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_1);
        this->skelAnime.playSpeed = -1.0f;
    }
    EnGo2_SwapInitialFrameAnimFrameCount(this);
    this->unk_26E = 1;
    this->unk_211 = false;
    this->isAwake = false;
    this->actionFunc = EnGo2_CurledUp;
}

void EnGo2_WakeUp(EnGo2* this, PlayState* play) {
    if (CVar_GetS32("gUnfixGoronSpin", 0)) {
        // Trick SkelAnime into thinking the current animation is changing so that it morphs between the same position,
        // making the goron do a spin
        this->skelAnime.animation = NULL;
    }
    if (this->skelAnime.playSpeed == 0.0f) {
        if ((this->actor.params & 0x1F) != GORON_DMT_BIGGORON) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOLON_WAKE_UP);
        } else {
            func_800F4524(&D_801333D4, NA_SE_EN_GOLON_WAKE_UP, 60);
        }
    }
    if ((this->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
        OnePointCutscene_Init(play, 4200, -99, &this->actor, MAIN_CAM);
        // There is an issue interpolating between ENGO2_ANIM_0 and ENGO2_ANIM_1/10, the goron
        // is technically in the same position at the end of ANIM_0 and beginning of ANIM_1/10
        // but something isn't getting translated correctly causing the 360 degree spin before
        // then continuing the wake up animation like normal. One solution is to use ANIM_0
        // which uses the same frame data as ANIM_1/10 but no morph frames, but only when the
        // current animation frame is at 0, meaning no morphing is necessary anyway.
        // ANIM_13 is ANIM_0 but with the startFrame and mode adjusted for biggoron.
        if (this->skelAnime.curFrame == 0.0f && !CVar_GetS32("gUnfixGoronSpin", 0)) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_13);
        } else {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_10);
        }
        this->skelAnime.playSpeed = 0.5f;
    } else {
        if (this->skelAnime.curFrame == 0.0f && !CVar_GetS32("gUnfixGoronSpin", 0)) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_0);
        } else {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_1);
        }
        this->skelAnime.playSpeed = 1.0f;
    }
    this->actionFunc = func_80A46B40;
}

void EnGo2_GetItemAnimation(EnGo2* this, PlayState* play) {
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_1);
    this->unk_211 = true;
    this->actionFunc = func_80A46B40;
    this->skelAnime.playSpeed = 0.0f;
    this->actor.speedXZ = 0.0f;
    this->skelAnime.curFrame = this->skelAnime.endFrame;
}

void EnGo2_SetupRolling(EnGo2* this, PlayState* play) {
    if ((this->actor.params & 0x1F) == GORON_CITY_ROLLING_BIG || (this->actor.params & 0x1F) == GORON_CITY_LINK) {
        this->collider.info.bumperFlags = 1;
        this->actor.speedXZ = gSaveContext.infTable[17] & 0x4000 ? 6.0f : 3.6000001f;
    } else {
        this->actor.speedXZ = 6.0f;
    }
    this->actor.flags |= ACTOR_FLAG_24;
    this->animTimer = 10;
    this->actor.shape.yOffset = 1800.0f;
    this->actor.speedXZ += this->actor.speedXZ; // Speeding up
    this->actionFunc = EnGo2_ContinueRolling;
}

void EnGo2_StopRolling(EnGo2* this, PlayState* play) {
    EnBom* bomb;

    if (((this->actor.params & 0x1F) != GORON_CITY_ROLLING_BIG) && ((this->actor.params & 0x1F) != GORON_CITY_LINK)) {
        if ((this->actor.params & 0x1F) == GORON_DMT_ROLLING_SMALL) {
            bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
            if (bomb != NULL) {
                bomb->timer = 0;
            }
        }
    } else {
        this->collider.info.bumperFlags = 0;
    }

    this->actor.shape.rot = this->actor.world.rot;
    this->unk_59C = 0;
    this->unk_590 = 0;
    this->actionFunc = EnGo2_GroundRolling;
    this->actor.shape.yOffset = 0.0f;
    this->actor.speedXZ = 0.0f;
}

s32 EnGo2_IsFreeingGoronInFire(EnGo2* this, PlayState* play) {
    if ((this->actor.params & 0x1F) != GORON_FIRE_GENERIC) {
        return false;
    }

    // shaking curled up
    this->actor.world.pos.x += (play->state.frames & 1) ? 1.0f : -1.0f;
    if (Flags_GetSwitch(play, (this->actor.params & 0xFC00) >> 0xA)) {
        return true;
    }
    return false;
}

s32 EnGo2_IsGoronDmtBombFlower(EnGo2* this) {
    if ((this->actor.params & 0x1F) != GORON_DMT_BOMB_FLOWER || this->unk_194.unk_00 != 2) {
        return false;
    }

    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_3);
    this->unk_194.unk_00 = 0;
    this->isAwake = false;
    this->unk_26E = 1;
    this->actionFunc = EnGo2_GoronDmtBombFlowerAnimation;
    return true;
}

s32 EnGo2_IsGoronRollingBig(EnGo2* this, PlayState* play) {
    if ((this->actor.params & 0x1F) != GORON_CITY_ROLLING_BIG || (this->unk_194.unk_00 != 2)) {
        return false;
    }
    this->unk_194.unk_00 = 0;
    EnGo2_RollingAnimation(this, play);
    this->actionFunc = EnGo2_GoronRollingBigContinueRolling;
    return true;
}

s32 EnGo2_IsGoronFireGeneric(EnGo2* this) {
    if ((this->actor.params & 0x1F) != GORON_FIRE_GENERIC || this->unk_194.unk_00 == 0) {
        return false;
    }
    this->actionFunc = EnGo2_GoronFireGenericAction;
    return true;
}

s32 EnGo2_IsGoronLinkReversing(EnGo2* this) {
    if ((this->actor.params & 0x1F) != GORON_CITY_LINK || (this->waypoint >= this->unk_216) ||
        !EnGo2_IsWakingUp(this)) {
        return false;
    }
    return true;
}

s32 EnGo2_IsRolling(EnGo2* this) {
    if (this->unk_194.unk_00 == 0 || this->actor.speedXZ < 1.0f) {
        return false;
    }
    if (EnGo2_IsRollingOnGround(this, 2, 20.0 / 3.0f, 0)) {
        if ((this->unk_590 >= 9) && (this->unk_59C == 0)) {
            this->unk_590 = 8;
        }
        EnGo2_GetDustData(this, 0);
    }
    return true;
}

void EnGo2_GoronLinkAnimation(EnGo2* this, PlayState* play) {
    s32 animation = ARRAY_COUNT(sAnimationInfo);

    if ((this->actor.params & 0x1F) == GORON_CITY_LINK) {
        if ((this->actor.textId == 0x3035 && this->unk_20C == 0) ||
            (this->actor.textId == 0x3036 && this->unk_20C == 0)) {
            if (this->skelAnime.animation != &gGoronAnim_000D5C) {
                animation = ENGO2_ANIM_12;
                this->eyeMouthTexState = 0;
            }
        }

        if ((this->actor.textId == 0x3032 && this->unk_20C == 12) || (this->actor.textId == 0x3033) ||
            (this->actor.textId == 0x3035 && this->unk_20C == 6)) {
            if (this->skelAnime.animation != &gGoronAnim_000750) {
                animation = ENGO2_ANIM_11;
                this->eyeMouthTexState = 1;
            }
        }

        if (this->skelAnime.animation == &gGoronAnim_000750) {
            if (this->skelAnime.curFrame == 20.0f) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOLON_CRY);
            }
        }

        if (animation != ARRAY_COUNT(sAnimationInfo)) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, animation);
        }
    }
}

void EnGo2_GoronFireCamera(EnGo2* this, PlayState* play) {
    s16 yaw;

    this->camId = Play_CreateSubCamera(play);
    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, this->camId, CAM_STAT_ACTIVE);
    Path_CopyLastPoint(this->path, &this->at);
    yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->at) + 0xE38;
    this->eye.x = Math_SinS(yaw) * 100.0f + this->actor.world.pos.x;
    this->eye.z = Math_CosS(yaw) * 100.0f + this->actor.world.pos.z;
    this->eye.y = this->actor.world.pos.y + 20.0f;
    this->at.x = this->actor.world.pos.x;
    this->at.y = this->actor.world.pos.y + 40.0f;
    this->at.z = this->actor.world.pos.z;
    Play_CameraSetAtEye(play, this->camId, &this->at, &this->eye);
}

void EnGo2_GoronFireClearCamera(EnGo2* this, PlayState* play) {
    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
    Play_ClearCamera(play, this->camId);
}

void EnGo2_BiggoronAnimation(EnGo2* this) {
    if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_SWORD_BROKEN && INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_EYEDROPS &&
        (this->actor.params & 0x1F) == GORON_DMT_BIGGORON && this->unk_194.unk_00 == 0) {
        if (DECR(this->animTimer) == 0) {
            this->animTimer = Rand_S16Offset(30, 30);
            func_800F4524(&D_801333D4, NA_SE_EN_GOLON_EYE_BIG, 60);
        }
    }
}

void EnGo2_Init(Actor* thisx, PlayState* play) {
    EnGo2* this = (EnGo2*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 28.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGoronSkel, NULL, this->jointTable, this->morphTable, 18);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    // Not GORON_CITY_ROLLING_BIG, GORON_CITY_LINK, GORON_DMT_BIGGORON
    switch (this->actor.params & 0x1F) {
        case GORON_FIRE_GENERIC:
        case GORON_DMT_BOMB_FLOWER:
        case GORON_DMT_ROLLING_SMALL:
        case GORON_DMT_DC_ENTRANCE:
        case GORON_CITY_ENTRANCE:
        case GORON_CITY_ISLAND:
        case GORON_CITY_LOWEST_FLOOR:
        case GORON_CITY_STAIRWELL:
        case GORON_CITY_LOST_WOODS:
        case GORON_DMT_FAIRY_HINT:
        case GORON_MARKET_BAZAAR:
            this->actor.flags &= ~ACTOR_FLAG_4;
            this->actor.flags &= ~ACTOR_FLAG_5;
    }

    EnGo2_SetColliderDim(this);
    EnGo2_SetShape(this);
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_0);
    this->actor.gravity = -1.0f;
    this->alpha = this->actor.shape.shadowAlpha = 0;
    this->reverse = 0;
    this->isAwake = false;
    this->unk_211 = false;
    this->goronState = 0;
    this->waypoint = 0;
    this->unk_216 = this->actor.shape.rot.z;
    this->unk_26E = 1;
    this->path = Path_GetByIndex(play, (this->actor.params & 0x3E0) >> 5, 0x1F);
    this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    switch (this->actor.params & 0x1F) {
        case GORON_CITY_ENTRANCE:
        case GORON_CITY_ISLAND:
        case GORON_CITY_LOWEST_FLOOR:
        case GORON_CITY_STAIRWELL:
        case GORON_CITY_LOST_WOODS:
            if (((!gSaveContext.n64ddFlag && !CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) ||
                 (gSaveContext.n64ddFlag && !Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE))) && LINK_IS_ADULT) {
                Actor_Kill(&this->actor);
            }
            this->actionFunc = EnGo2_CurledUp;
            break;
        case GORON_MARKET_BAZAAR:
            if ((LINK_IS_ADULT) || !CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                Actor_Kill(&this->actor);
            }
            EnGo2_GetItemAnimation(this, play);
            break;
        case GORON_CITY_LINK:
            if ((gSaveContext.infTable[16] & 0x200)) {
                Path_CopyLastPoint(this->path, &this->actor.world.pos);
                this->actor.home.pos = this->actor.world.pos;
                if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
                    EnGo2_GetItemAnimation(this, play);
                } else {
                    this->actionFunc = EnGo2_CurledUp;
                }
            } else {
                gSaveContext.infTable[16] &= ~0x1000;
                this->collider.dim.height = (D_80A4816C[this->actor.params & 0x1F].height * 0.6f);
                EnGo2_SetupRolling(this, play);
                this->isAwake = true;
            }
            break;
        case GORON_CITY_ROLLING_BIG:
        case GORON_DMT_ROLLING_SMALL:
            this->collider.dim.height = (D_80A4816C[this->actor.params & 0x1F].height * 0.6f);
            EnGo2_SetupRolling(this, play);
            break;
        case GORON_FIRE_GENERIC:
            if (Flags_GetSwitch(play, (this->actor.params & 0xFC00) >> 0xA)) {
                Actor_Kill(&this->actor);
            } else {
                this->isAwake = true;
                this->actionFunc = EnGo2_CurledUp;
            }
            break;
        case GORON_DMT_BIGGORON:
            this->actor.shape.shadowDraw = NULL;
            this->actor.flags &= ~ACTOR_FLAG_0;
            if ((INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_SWORD_BROKEN) &&
                (INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_EYEDROPS)) {
                this->eyeMouthTexState = 1;
            }
            this->collider.base.acFlags = 0;
            this->collider.base.ocFlags1 = 0xD; // OC_PLAYER | OC_NO_PUSH | OC_ON
            this->actionFunc = EnGo2_CurledUp;
            break;
        case GORON_DMT_BOMB_FLOWER:
            if (gSaveContext.infTable[14] & 0x800) {
                Path_CopyLastPoint(this->path, &this->actor.world.pos);
                this->actor.home.pos = this->actor.world.pos;
            }
        case GORON_DMT_DC_ENTRANCE:
        case GORON_DMT_FAIRY_HINT:
        default:
            this->actionFunc = EnGo2_CurledUp;
    }
}

void EnGo2_Destroy(Actor* thisx, PlayState* play) {
}

void EnGo2_CurledUp(EnGo2* this, PlayState* play) {
    u8 index = this->actor.params & 0x1F;
    s16 height;
    s32 quake;

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if ((this->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
            quake = Quake_Add(GET_ACTIVE_CAM(play), 3);
            Quake_SetSpeed(quake, -0x3CB0);
            Quake_SetQuakeValues(quake, 8, 0, 0, 0);
            Quake_SetCountdown(quake, 16);
        } else {
            EnGo2_GetDustData(this, 1);
        }
        this->skelAnime.playSpeed = 0.0f;
    }

    if ((s32)this->skelAnime.curFrame == 0) {
        this->collider.dim.height = (D_80A4816C[index].height * 0.6f);
    } else {
        height = D_80A4816C[index].height;
        this->collider.dim.height =
            ((D_80A4816C[index].height * 0.4f * (this->skelAnime.curFrame / this->skelAnime.startFrame)) +
             (height * 0.6f));
    }
    if (EnGo2_IsFreeingGoronInFire(this, play)) {
        this->isAwake = false;
        EnGo2_WakeUp(this, play);
    }
    if (((this->actor.params & 0x1F) != GORON_FIRE_GENERIC) && EnGo2_IsWakingUp(this)) {
        EnGo2_WakeUp(this, play);
    }
}

void func_80A46B40(EnGo2* this, PlayState* play) {
    u8 index = (this->actor.params & 0x1F);
    f32 height;

    if (this->unk_211 == true) {
        EnGo2_BiggoronAnimation(this);
        EnGo2_GoronLinkAnimation(this, play);
        EnGo2_SelectGoronWakingUp(this);

        if (!EnGo2_IsGoronRollingBig(this, play) && !EnGo2_IsGoronFireGeneric(this)) {
            if (EnGo2_IsGoronDmtBombFlower(this)) {
                return;
            }
        } else {
            return;
        }
    } else {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            if ((this->actor.params & 0x1F) == GORON_DMT_BIGGORON) {
                this->actor.flags |= ACTOR_FLAG_0;
            }
            func_80A454CC(this);
            this->unk_211 = true;
            this->collider.dim.height = D_80A4816C[index].height;
        } else {
            height = D_80A4816C[index].height;
            this->collider.dim.height =
                (s16)((height * 0.4f * (this->skelAnime.curFrame / this->skelAnime.endFrame)) + (height * 0.6f));
        }
    }
    if ((!EnGo2_IsCameraModified(this, play)) && (!EnGo2_IsWakingUp(this))) {
        EnGo2_RollingAnimation(this, play);
    }
}

void EnGo2_GoronDmtBombFlowerAnimation(EnGo2* this, PlayState* play) {
    f32 float1 = this->skelAnime.endFrame;
    f32 float2 = this->skelAnime.curFrame * ((f32)0x8000 / float1);

    this->actor.speedXZ = Math_SinS(float2);
    if ((EnGo2_Orient(this, play)) && (this->waypoint == 0)) {
        EnGo2_GetItemAnimation(this, play);
    }
}

void EnGo2_GoronRollingBigContinueRolling(EnGo2* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnGo2_GetDustData(this, 1);
        this->skelAnime.playSpeed = 0.0f;
        EnGo2_SetupRolling(this, play);
    }
}

void EnGo2_ContinueRolling(EnGo2* this, PlayState* play) {
    f32 float1 = 1000.0f;

    if (((this->actor.params & 0x1F) != GORON_DMT_ROLLING_SMALL || !(this->actor.xyzDistToPlayerSq > SQ(float1))) &&
        DECR(this->animTimer) == 0) {
        this->actionFunc = EnGo2_SlowRolling;
        this->actor.speedXZ *= 0.5f; // slowdown
    }
    EnGo2_GetDustData(this, 2);
}

void EnGo2_SlowRolling(EnGo2* this, PlayState* play) {
    s32 orientation;
    s32 index;

    if (!EnGo2_IsRolling(this)) {
        if (EnGo2_IsRollingOnGround(this, 4, 8.0f, 1) == true) {
            if (EnGo2_IsGoronLinkReversing(this)) {
                this->actionFunc = EnGo2_ReverseRolling;
                return;
            }
            EnGo2_GetDustData(this, 3);
        }
        orientation = EnGo2_Orient(this, play);
        index = this->actor.params & 0x1F;
        if (index != GORON_CITY_LINK) {
            if ((index == GORON_DMT_ROLLING_SMALL) && (orientation == 1) && (this->waypoint == 0)) {
                EnGo2_StopRolling(this, play);
                return;
            }
        } else if ((orientation == 2) && (this->waypoint == 1)) {
            EnGo2_StopRolling(this, play);
            return;
        }
        Math_ApproachF(&this->actor.speedXZ, EnGo2_GetTargetXZSpeed(this), 0.4f, 0.6f);
        this->actor.shape.rot = this->actor.world.rot;
    }
}

void EnGo2_GroundRolling(EnGo2* this, PlayState* play) {
    if (EnGo2_IsRollingOnGround(this, 4, 8.0f, 0)) {
        EnGo2_GetDustData(this, 0);
        if (this->unk_59C == 0) {
            switch (this->actor.params & 0x1F) {
                case GORON_CITY_LINK:
                    this->goronState = 0;
                    this->actionFunc = EnGo2_GoronLinkStopRolling;
                    break;
                case GORON_CITY_ROLLING_BIG:
                    EnGo2_WakeUp(this, play);
                    break;
                default:
                    this->actionFunc = EnGo2_CurledUp;
            }
        }
    }
}

void EnGo2_ReverseRolling(EnGo2* this, PlayState* play) {
    if (!EnGo2_IsRolling(this)) {
        Math_ApproachF(&this->actor.speedXZ, 0.0f, 0.6f, 0.8f);
        if (this->actor.speedXZ >= 1.0f) {
            EnGo2_GetDustData(this, 3);
        }
        if ((s32)this->actor.speedXZ == 0) {
            this->actor.world.rot.y ^= 0x8000;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            this->reverse ^= 1;
            EnGo2_UpdateWaypoint(this, play);
            EnGo2_SetupRolling(this, play);
        }
    }
}

void EnGo2_SetupGetItem(EnGo2* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = EnGo2_SetGetItem;
    } else {
        if (!gSaveContext.n64ddFlag || this->getItemEntry.getItemId == GI_NONE) {
            func_8002F434(&this->actor, play, this->getItemId, this->actor.xzDistToPlayer + 1.0f, fabsf(this->actor.yDistToPlayer) + 1.0f);
        } else {
            GiveItemEntryFromActor(&this->actor, play, this->getItemEntry, this->actor.xzDistToPlayer + 1.0f, fabsf(this->actor.yDistToPlayer) + 1.0f);
        }
    }
}

void EnGo2_SetGetItem(EnGo2* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        this->unk_194.unk_00 = 0;
        switch (this->getItemId) {
            case GI_CLAIM_CHECK:
                Environment_ClearBgsDayCount();
                EnGo2_GetItemAnimation(this, play);
                return;
            case GI_TUNIC_GORON:
                if (!gSaveContext.n64ddFlag) {
                    gSaveContext.infTable[16] |= 0x200;
                }
                EnGo2_GetItemAnimation(this, play);
                return;
            case GI_SWORD_BGS:
                gSaveContext.bgsFlag = true;
                break;
            case GI_BOMB_BAG_30:
            case GI_BOMB_BAG_40:
                EnGo2_RollingAnimation(this, play);
                this->actionFunc = EnGo2_GoronRollingBigContinueRolling;
                return;
        }

        if (gSaveContext.n64ddFlag) {
            // Resolves #1301. unk_13EE is used to set the opacity of the HUD. The trade sequence discussion with Biggoron 
            // sets the HUD to transparent, and it is restored at z_message_PAL:3549, but by specifically watching for 
            // trade sequence items, this leaves it transparent for non-trade sequence items (in rando) so we fix that here
            gSaveContext.unk_13EE = 0x32;
        }
        this->actionFunc = func_80A46B40;
    }
}

void EnGo2_BiggoronEyedrops(EnGo2* this, PlayState* play) {
    switch (this->goronState) {
        case 0:
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_5);
            this->actor.flags &= ~ACTOR_FLAG_0;
            this->actor.shape.rot.y += 0x5B0;
            this->unk_26E = 1;
            this->animTimer = gSaveContext.n64ddFlag ? 0 : (this->skelAnime.endFrame + 60.0f + 60.0f); // eyeDrops animation timer
            this->eyeMouthTexState = 2;
            this->unk_20C = 0;
            this->goronState++;
            func_800F483C(0x28, 5);
            if (!gSaveContext.n64ddFlag) {
                OnePointCutscene_Init(play, 4190, -99, &this->actor, MAIN_CAM);
            }
            break;
        case 1:
            if (DECR(this->animTimer)) {
                if (this->animTimer == 60 || this->animTimer == 120) {
                    func_8005B1A4(GET_ACTIVE_CAM(play));
                    func_800F4524(&D_801333D4, NA_SE_EV_GORON_WATER_DROP, 60);
                }
            } else {
                func_800F4524(&D_801333D4, NA_SE_EN_GOLON_GOOD_BIG, 60);
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_6);
                Message_ContinueTextbox(play, 0x305A);
                this->eyeMouthTexState = 3;
                this->goronState++;
                func_800F483C(0x7F, 5);
            }
            break;
        case 2:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                this->eyeMouthTexState = 0;
            }
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_1);
                this->actor.flags |= ACTOR_FLAG_0;
                this->unk_26E = 2;
                this->skelAnime.playSpeed = 0.0f;
                this->skelAnime.curFrame = this->skelAnime.endFrame;
                if (gSaveContext.n64ddFlag) {
                    GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_DMT_TRADE_EYEDROPS, GI_CLAIM_CHECK);
                    Randomizer_ConsumeAdultTradeItem(play, ITEM_EYEDROPS);
                    EnGo2_GetItemEntry(this, play, getItemEntry);
                } else {
                    u32 getItemId = GI_CLAIM_CHECK;
                    EnGo2_GetItem(this, play, getItemId);
                }
                this->actionFunc = EnGo2_SetupGetItem;
                this->goronState = 0;
            }
            break;
    }
}

void EnGo2_GoronLinkStopRolling(EnGo2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->goronState) {
        case 0:
            if (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE) {
                return;
            } else {
                Message_StartTextbox(play, 0x3031, NULL);
                player->actor.freezeTimer = 10;
                this->goronState++;
            }
        case 1:
            break;
        default:
            return;
    }

    if (Message_GetState(&play->msgCtx) != TEXT_STATE_CLOSING) {
        player->actor.freezeTimer = 10;
    } else {
        gSaveContext.infTable[16] |= 0x1000;
        this->unk_26E = 1;
        this->unk_211 = false;
        this->isAwake = false;
        this->actionFunc = EnGo2_CurledUp;
    }
}

void EnGo2_GoronFireGenericAction(EnGo2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3s D_80A4854C = { 0x00, 0x00, 0x00 };

    switch (this->goronState) {
        case 0: // Wake up
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                EnGo2_GoronFireCamera(this, play);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENGO2_ANIM_2);
                this->waypoint = 1;
                this->skelAnime.playSpeed = 2.0f;
                func_80A44D84(this);
                this->actor.shape.rot = this->actor.world.rot;
                this->animTimer = 60;
                this->actor.gravity = 0.0f;
                this->actor.speedXZ = 2.0f;
                this->unk_194.unk_08 = D_80A4854C;
                this->unk_194.unk_0E = D_80A4854C;
                this->goronState++;
                this->goronState++;
                player->actor.world.rot.y = this->actor.world.rot.y;
                player->actor.shape.rot.y = this->actor.world.rot.y;
                player->actor.world.pos.x =
                    (f32)((Math_SinS(this->actor.world.rot.y) * -30.0f) + this->actor.world.pos.x);
                player->actor.world.pos.z =
                    (f32)((Math_CosS(this->actor.world.rot.y) * -30.0f) + this->actor.world.pos.z);
                func_8002DF54(play, &this->actor, 8);
                Audio_PlayFanfare(NA_BGM_APPEAR);
            }
            break;
        case 2: // Walking away
            if (DECR(this->animTimer)) {
                if (!(this->animTimer % 8)) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_WALK);
                }
                Actor_MoveForward(&this->actor);
            } else {
                this->animTimer = 0;
                this->actor.speedXZ = 0.0f;
                if ((((this->actor.params & 0xFC00) >> 0xA) != 1) && (((this->actor.params & 0xFC00) >> 0xA) != 2) &&
                    (((this->actor.params & 0xFC00) >> 0xA) != 4) && (((this->actor.params & 0xFC00) >> 0xA) != 5) &&
                    (((this->actor.params & 0xFC00) >> 0xA) != 9) && (((this->actor.params & 0xFC00) >> 0xA) != 11)) {
                    this->goronState++;
                }
                this->goronState++;
            }
            break;
        case 3: // Walking away
            this->animTimer++;
            if (!(this->animTimer % 8) && (this->animTimer < 10)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_MORIBLIN_WALK);
            }
            if (this->animTimer == 10) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_IRON_DOOR_OPEN);
            }
            if (this->animTimer > 44) {
                SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_IRON_DOOR_CLOSE);
            } else {
                break;
            }
        case 4: // Finalize walking away
            Message_CloseTextbox(play);
            EnGo2_GoronFireClearCamera(this, play);
            func_8002DF54(play, &this->actor, 7);
            Actor_Kill(&this->actor);
            break;
        case 1:
            break;
    }
}

void EnGo2_Update(Actor* thisx, PlayState* play) {
    EnGo2* this = (EnGo2*)thisx;

    func_80A45360(this, &this->alpha);
    EnGo2_SitDownAnimation(this);
    SkelAnime_Update(&this->skelAnime);
    EnGo2_RollForward(this);
    Actor_UpdateBgCheckInfo(play, &this->actor, (f32)this->collider.dim.height * 0.5f,
                            (f32)this->collider.dim.radius * 0.6f, 0.0f, 5);
    if (this->unk_194.unk_00 == 0) {
        func_80A44AB0(this, play);
    }
    this->actionFunc(this, play);
    if (this->unk_211 == true) {
        func_80034F54(play, this->unk_226, this->unk_24A, 18);
    }
    func_80A45288(this, play);
    EnGo2_EyeMouthTexState(this);
    EnGo2_CheckCollision(this, play);
}

s32 EnGo2_DrawCurledUp(EnGo2* this, PlayState* play) {
    Vec3f D_80A48554 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00BD80);
    CLOSE_DISPS(play->state.gfxCtx);
    Matrix_MultVec3f(&D_80A48554, &this->actor.focus.pos);

    return 1;
}

s32 EnGo2_DrawRolling(EnGo2* this, PlayState* play) {
    s32 pad;
    Vec3f D_80A48560 = { 0.0f, 0.0f, 0.0f };
    f32 speedXZ;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    speedXZ = this->actionFunc == EnGo2_ReverseRolling ? 0.0f : this->actor.speedXZ;
    Matrix_RotateZYX((play->state.frames * ((s16)speedXZ * 1400)), 0, this->actor.shape.rot.z, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00C140);
    CLOSE_DISPS(play->state.gfxCtx);
    Matrix_MultVec3f(&D_80A48560, &this->actor.focus.pos);
    return 1;
}

s32 EnGo2_OverrideLimbDraw(PlayState* play, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGo2* this = (EnGo2*)thisx;
    Vec3s vec1;
    f32 float1;

    if (limb == 17) {
        Matrix_Translate(2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec1 = this->unk_194.unk_08;
        float1 = (vec1.y / (f32)0x8000) * M_PI;
        Matrix_RotateX(float1, MTXMODE_APPLY);
        float1 = (vec1.x / (f32)0x8000) * M_PI;
        Matrix_RotateZ(float1, MTXMODE_APPLY);
        Matrix_Translate(-2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limb == 10) {
        vec1 = this->unk_194.unk_0E;
        float1 = (vec1.y / (f32)0x8000) * M_PI;
        Matrix_RotateY(float1, MTXMODE_APPLY);
        float1 = (vec1.x / (f32)0x8000) * M_PI;
        Matrix_RotateX(float1, MTXMODE_APPLY);
    }
    if ((limb == 10) || (limb == 11) || (limb == 14)) {
        float1 = Math_SinS(this->unk_226[limb]);
        rot->y += float1 * 200.0f;
        float1 = Math_CosS(this->unk_24A[limb]);
        rot->z += float1 * 200.0f;
    }
    return 0;
}

void EnGo2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGo2* this = (EnGo2*)thisx;
    Vec3f D_80A4856C = { 600.0f, 0.0f, 0.0f };

    if (limbIndex == 17) {
        Matrix_MultVec3f(&D_80A4856C, &this->actor.focus.pos);
    }
}

void EnGo2_Draw(Actor* thisx, PlayState* play) {
    EnGo2* this = (EnGo2*)thisx;
    void* eyeTextures[] = { gGoronCsEyeClosed2Tex, gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    void* mouthTextures[] = { gGoronCsMouthNeutralTex, gGoronCsMouthSmileTex };

    EnGo2_UpdateDust(this);
    Matrix_Push();
    EnGo2_DrawDust(this, play);
    Matrix_Pop();

    if ((this->actionFunc == EnGo2_CurledUp) && (this->skelAnime.playSpeed == 0.0f) &&
        (this->skelAnime.curFrame == 0.0f)) {
        EnGo2_DrawCurledUp(this, play);
    } else if (this->actionFunc == EnGo2_SlowRolling || this->actionFunc == EnGo2_ReverseRolling ||
               this->actionFunc == EnGo2_ContinueRolling) {
        EnGo2_DrawRolling(this, play);
    } else {
        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL_25Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTextures[this->mouthTexIndex]));

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, EnGo2_OverrideLimbDraw, EnGo2_PostLimbDraw, this);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}
