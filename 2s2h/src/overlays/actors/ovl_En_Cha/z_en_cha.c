/*
 * File: z_en_cha.c
 * Overlay: ovl_En_Cha
 * Description: Laundry Pool Bell
 */

#include "z_en_cha.h"
#include "objects/object_cha/object_cha.h"

#define FLAGS 0x00000000

void EnCha_Init(Actor* thisx, PlayState* play);
void EnCha_Destroy(Actor* thisx, PlayState* play);
void EnCha_Update(Actor* thisx, PlayState* play2);
void EnCha_Draw(Actor* thisx, PlayState* play);

void EnCha_Idle(EnCha* this, PlayState* play);

ActorProfile En_Cha_Profile = {
    /**/ ACTOR_EN_CHA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_CHA,
    /**/ sizeof(EnCha),
    /**/ EnCha_Init,
    /**/ EnCha_Destroy,
    /**/ EnCha_Update,
    /**/ EnCha_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_WOOD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 10, 40, 0, { 0, 0, 0 } },
};

void EnCha_Init(Actor* thisx, PlayState* play) {
    EnCha* this = (EnCha*)thisx;
    s32 pad;

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Collider_UpdateCylinder(&this->actor, &this->collider);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.home.rot.z = 0;
    this->actionFunc = EnCha_Idle;
    this->actor.home.rot.x = this->actor.home.rot.z;
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_60_04);
}

void EnCha_Destroy(Actor* thisx, PlayState* play) {
    EnCha* this = (EnCha*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnCha_Ring(EnCha* this, PlayState* play) {
    EnCha_Idle(this, play);
    if (this->actor.csId == CS_ID_NONE) {
        this->actionFunc = EnCha_Idle;
    } else if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        this->actionFunc = EnCha_Idle;
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void EnCha_Idle(EnCha* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_60_04)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_DOOR_BELL);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_60_04);
        this->actor.home.rot.z = 0x7D0;
    }
    if (this->collider.base.acFlags & AC_HIT) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_DOOR_BELL);
        this->actor.home.rot.z = 0x7D0;
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_51_04)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_51_04);
            this->actionFunc = EnCha_Ring;
        }
    }
    this->actor.home.rot.x += this->actor.home.rot.z;
    this->actor.home.rot.z -= TRUNCF_BINANG(this->actor.home.rot.x * 0.1f);
    this->actor.home.rot.z *= 0.96f;
}

void EnCha_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnCha* this = (EnCha*)thisx;

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    this->actionFunc(this, play);
    if ((this->actor.shape.rot.z > -0x1F40) && (this->actor.shape.rot.z < 0x1F40)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnCha_Draw(Actor* thisx, PlayState* play) {
    EnCha* this = (EnCha*)thisx;

    Gfx_DrawDListOpa(play, object_cha_DL_000710);
    Matrix_Translate(-1094.0f, 4950.0f, 9.0f, MTXMODE_APPLY);
    Matrix_RotateXS(this->actor.home.rot.x, MTXMODE_APPLY);
    Gfx_DrawDListOpa(play, object_cha_DL_000958);
}
