/*
 * File: z_en_si.c
 * Overlay: En_Si
 * Description: Gold Skulltula token
 */

#include "z_en_si.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_9)

void EnSi_Init(Actor* thisx, GlobalContext* globalCtx);
void EnSi_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnSi_Update(Actor* thisx, GlobalContext* globalCtx);
void EnSi_Draw(Actor* thisx, GlobalContext* globalCtx);

s32 func_80AFB748(EnSi* this, GlobalContext* globalCtx);
void func_80AFB768(EnSi* this, GlobalContext* globalCtx);
void func_80AFB89C(EnSi* this, GlobalContext* globalCtx);
void func_80AFB950(EnSi* this, GlobalContext* globalCtx);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000090, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 18, 2, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 D_80AFBADC = { 0, 0, 0, 0, MASS_IMMOVABLE };

const ActorInit En_Si_InitVars = {
    ACTOR_EN_SI,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_ST,
    sizeof(EnSi),
    (ActorFunc)EnSi_Init,
    (ActorFunc)EnSi_Destroy,
    (ActorFunc)EnSi_Update,
    (ActorFunc)EnSi_Draw,
    NULL,
};

void EnSi_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnSi* this = (EnSi*)thisx;

    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &D_80AFBADC);
    Actor_SetScale(&this->actor, 0.025f);
    this->unk_19C = 0;
    this->actionFunc = func_80AFB768;
    this->actor.shape.yOffset = 42.0f;
}

void EnSi_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnSi* this = (EnSi*)thisx;

    Collider_DestroyCylinder(globalCtx, &this->collider);
}

s32 func_80AFB748(EnSi* this, GlobalContext* globalCtx) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
    }
    return 0;
}

void func_80AFB768(EnSi* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_13)) {
        this->actionFunc = func_80AFB89C;
    } else {
        Math_SmoothStepToF(&this->actor.scale.x, 0.25f, 0.4f, 1.0f, 0.0f);
        Actor_SetScale(&this->actor, this->actor.scale.x);
        this->actor.shape.rot.y += 0x400;

        if (!Player_InCsMode(globalCtx)) {
            func_80AFB748(this, globalCtx);

            if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
                this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
                Item_Give(globalCtx, ITEM_SKULL_TOKEN);
                if (CVar_GetS32("gSkulltulaFreeze", 0) != 1) {
                    player->actor.freezeTimer = 20;
                }
                Message_StartTextbox(globalCtx, 0xB4, NULL);
                Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
                this->actionFunc = func_80AFB950;
            } else {
                Collider_UpdateCylinder(&this->actor, &this->collider);
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
                CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
            }
        }
    }
}

void func_80AFB89C(EnSi* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    Math_SmoothStepToF(&this->actor.scale.x, 0.25f, 0.4f, 1.0f, 0.0f);
    Actor_SetScale(&this->actor, this->actor.scale.x);
    this->actor.shape.rot.y += 0x400;

    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_13)) {
        Item_Give(globalCtx, ITEM_SKULL_TOKEN);
        Message_StartTextbox(globalCtx, 0xB4, NULL);
        Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
        this->actionFunc = func_80AFB950;
    }
}

void func_80AFB950(EnSi* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (Message_GetState(&globalCtx->msgCtx) != TEXT_STATE_CLOSING && CVar_GetS32("gSkulltulaFreeze", 0) != 1) {
        player->actor.freezeTimer = 10;
    } else {
        SET_GS_FLAGS((this->actor.params & 0x1F00) >> 8, this->actor.params & 0xFF);
        Actor_Kill(&this->actor);
    }
}

void EnSi_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnSi* this = (EnSi*)thisx;

    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    this->actionFunc(this, globalCtx);
    Actor_SetFocus(&this->actor, 16.0f);
}

void EnSi_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnSi* this = (EnSi*)thisx;

    if (this->actionFunc != func_80AFB950) {
        func_8002ED80(&this->actor, globalCtx, 0);
        func_8002EBCC(&this->actor, globalCtx, 0);
        GetItem_Draw(globalCtx, GID_SKULL_TOKEN_2);
    }
}
