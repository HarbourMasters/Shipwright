/*
 * File: z_bg_jya_zurerukabe.c
 * Overlay: ovl_Bg_Jya_Zurerukabe
 * Description: Sliding, Climbable Brick Wall
 */

#include "z_bg_jya_zurerukabe.h"
#include "objects/object_jya_obj/object_jya_obj.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void BgJyaZurerukabe_Init(Actor* thisx, PlayState* play);
void BgJyaZurerukabe_Destroy(Actor* thisx, PlayState* play);
void BgJyaZurerukabe_Update(Actor* thisx, PlayState* play);
void BgJyaZurerukabe_Draw(Actor* thisx, PlayState* play);

void func_8089B4C8(BgJyaZurerukabe* this, PlayState* play);
void func_8089B7B4(BgJyaZurerukabe* this);
void func_8089B7C4(BgJyaZurerukabe* this, PlayState* play);
void func_8089B80C(BgJyaZurerukabe* this);
void func_8089B870(BgJyaZurerukabe* this, PlayState* play);

static f32 D_8089B9C0[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

const ActorInit Bg_Jya_Zurerukabe_InitVars = {
    ACTOR_BG_JYA_ZURERUKABE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaZurerukabe),
    (ActorFunc)BgJyaZurerukabe_Init,
    (ActorFunc)BgJyaZurerukabe_Destroy,
    (ActorFunc)BgJyaZurerukabe_Update,
    (ActorFunc)BgJyaZurerukabe_Draw,
    NULL,
};

static s16 D_8089B9F0[4] = { 943, 1043, 1243, 1343 };

static s16 D_8089B9F8[4] = { -1, 1, -1, 1 };

static s16 D_8089BA00[4] = { 48, 48, 36, 36 };

static f32 D_8089BA08[4] = { 8.0f, 8.0f, 10.0f, 10.0f };

static s16 D_8089BA18[6][2] = {
    { 0x0388, 0x0395 }, { 0x03EA, 0x03FF }, { 0x0454, 0x0467 },
    { 0x04B4, 0x04C1 }, { 0x0518, 0x0528 }, { 0x0581, 0x0590 },
};

static s16 D_8089BA30[6] = {
    0, 0, 1, 2, 2, 3,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgJyaZurerukabe_InitDynaPoly(BgJyaZurerukabe* this, PlayState* play, CollisionHeader* collision,
                                  s32 flag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&this->dyna, flag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void func_8089B4C8(BgJyaZurerukabe* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 == 0x200000) && (player->actor.wallPoly != NULL)) {
        s32 i;

        for (i = 0; i < ARRAY_COUNT(D_8089BA18); i++) {
            f32 posY = player->actor.world.pos.y;

            if ((posY >= D_8089BA18[i][0]) && (posY <= D_8089BA18[i][1])) {
                break;
            }
        }

        switch (i) {
            case 0:
            case 2:
            case 3:
            case 5:
                if (fabsf(D_8089B9C0[D_8089BA30[i]]) > 1.0f) {
                    func_8002F6D4(play, &this->dyna.actor, 1.5f, this->dyna.actor.shape.rot.y, 0.0f, 0);
                }
                break;
            case 1:
            case 4:
                if (fabsf(D_8089B9C0[D_8089BA30[i]] - D_8089B9C0[D_8089BA30[i + 1]]) > 1.0f) {
                    func_8002F6D4(play, &this->dyna.actor, 1.5f, this->dyna.actor.shape.rot.y, 0.0f, 0);
                }
                break;
        }
    }
}

void BgJyaZurerukabe_Init(Actor* thisx, PlayState* play) {
    BgJyaZurerukabe* this = (BgJyaZurerukabe*)thisx;
    s32 i;

    BgJyaZurerukabe_InitDynaPoly(this, play, &gZurerukabeCol, DPM_UNK);
    Actor_ProcessInitChain(thisx, sInitChain);

    for (i = 0; i < ARRAY_COUNT(D_8089B9F0); i++) {
        if (fabsf(D_8089B9F0[i] - this->dyna.actor.home.pos.y) < 1.0f) {
            this->unk_168 = i;
            break;
        }
    }

    if (i == ARRAY_COUNT(D_8089B9F0)) {
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("home pos が変更されたみたい(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.params);
        osSyncPrintf(VT_RST);
    }

    this->unk_16E = D_8089B9F8[this->unk_168];
    func_8089B7B4(this);
    osSyncPrintf("(jya ずれる壁)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void BgJyaZurerukabe_Destroy(Actor* thisx, PlayState* play) {
    BgJyaZurerukabe* this = (BgJyaZurerukabe*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    D_8089B9C0[this->unk_168] = 0.0f;
}

void func_8089B7B4(BgJyaZurerukabe* this) {
    this->actionFunc = func_8089B7C4;
}

void func_8089B7C4(BgJyaZurerukabe* this, PlayState* play) {
    if (this->unk_16A <= 0) {
        func_8089B80C(this);
    }
    D_8089B9C0[this->unk_168] = 0.0f;
}

void func_8089B80C(BgJyaZurerukabe* this) {
    this->actionFunc = func_8089B870;
    this->unk_16A = D_8089BA00[this->unk_168];
    if (ABS(this->unk_16C) == 4) {
        this->unk_16E = -this->unk_16E;
    }
    this->unk_16C += this->unk_16E;
}

void func_8089B870(BgJyaZurerukabe* this, PlayState* play) {
    if (Math_StepToF(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x + (this->unk_16C * 75),
                     D_8089BA08[this->unk_168])) {
        func_8089B7B4(this);
    }

    D_8089B9C0[this->unk_168] = D_8089BA08[this->unk_168] * this->unk_16E;
    func_8002F974(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
}

void BgJyaZurerukabe_Update(Actor* thisx, PlayState* play) {
    BgJyaZurerukabe* this = (BgJyaZurerukabe*)thisx;

    if (this->unk_16A > 0) {
        this->unk_16A--;
    }

    this->actionFunc(this, play);

    if (this->unk_168 == 0) {
        func_8089B4C8(this, play);
    }
}

void BgJyaZurerukabe_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gZurerukabeDL);
}
