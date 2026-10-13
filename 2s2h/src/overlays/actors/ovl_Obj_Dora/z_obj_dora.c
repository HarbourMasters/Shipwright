/*
 * File: z_obj_dora.c
 * Overlay: ovl_Obj_Dora
 * Description: Swordsman's School - Gong
 */

#include "z_obj_dora.h"
#include "assets/objects/object_dora/object_dora.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjDora_Init(Actor* thisx, PlayState* play);
void ObjDora_Destroy(Actor* thisx, PlayState* play);
void ObjDora_Update(Actor* thisx, PlayState* play);
void ObjDora_Draw(Actor* thisx, PlayState* play);

void ObjDora_SetupWait(ObjDora* this);
void ObjDora_Wait(ObjDora* this, PlayState* play);
void ObjDora_SetupMoveGong(ObjDora* this);
void ObjDora_MoveGong(ObjDora* this, PlayState* play);
s32 ObjDora_IsHalfHour(u16 time);
void ObjDora_UpdateCollision(ObjDora* this, PlayState* play);

typedef enum {
    /* 0x0 */ DORA_HIT_NONE,
    /* 0x1 */ DORA_HIT_LIGHT,
    /* 0x2 */ DORA_HIT_STRONG
} ObjDoraHitStrength;

typedef enum {
    /* 0x0 */ DORA_DMGEFF_NONE,
    /* 0xE */ DORA_DMGEFF_STRONG = 0xE,
    /* 0xF */ DORA_DMGEFF_LIGHT
} ObjDoraDamageEffect;

ActorProfile Obj_Dora_Profile = {
    /**/ ACTOR_OBJ_DORA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DORA,
    /**/ sizeof(ObjDora),
    /**/ ObjDora_Init,
    /**/ ObjDora_Destroy,
    /**/ ObjDora_Update,
    /**/ ObjDora_Draw,
};

static ColliderTrisElementInit sTrisElementsInit[6] = {
    {
        {
            ELEM_MATERIAL_UNK5,
            { 0x00000000, 0x00, 0x00 },
            { 0x00100000, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, -35.0f, 0.0f }, { 260.0f, -185.0f, 0.0f }, { 0.0f, -335.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK5,
            { 0x00000000, 0x00, 0x00 },
            { 0x00100000, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 260.0f, -185.0f, 0.0f }, { 260.0f, -485.0f, 0.0f }, { 0.0f, -335.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK5,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 260.0f, -485.0f, 0.0f }, { 0.0f, -635.0f, 0.0f }, { 0.0f, -335.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK5,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, -635.0f, 0.0f }, { -260.0f, -485.0f, 0.0f }, { 0.0f, -335.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK5,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -260.0f, -485.0f, 0.0f }, { -260.0f, -185.0f, 0.0f }, { 0.0f, -335.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK5,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -260.0f, -185.0f, 0.0f }, { 0.0f, -35.0f, 0.0f }, { 0.0f, -335.0f, 0.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(2, DORA_DMGEFF_LIGHT),
    /* Horse trample  */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(0, DORA_DMGEFF_LIGHT),
    /* Normal arrow   */ DMG_ENTRY(0, DORA_DMGEFF_LIGHT),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, DORA_DMGEFF_LIGHT),
    /* Hookshot       */ DMG_ENTRY(0, DORA_DMGEFF_LIGHT),
    /* Goron punch    */ DMG_ENTRY(2, DORA_DMGEFF_STRONG),
    /* Sword          */ DMG_ENTRY(1, DORA_DMGEFF_LIGHT),
    /* Goron pound    */ DMG_ENTRY(1, DORA_DMGEFF_LIGHT),
    /* Fire arrow     */ DMG_ENTRY(0, DORA_DMGEFF_LIGHT),
    /* Ice arrow      */ DMG_ENTRY(0, DORA_DMGEFF_LIGHT),
    /* Light arrow    */ DMG_ENTRY(0, DORA_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, DORA_DMGEFF_LIGHT),
    /* Deku spin      */ DMG_ENTRY(0, DORA_DMGEFF_LIGHT),
    /* Deku bubble    */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Normal shield  */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, DORA_DMGEFF_LIGHT),
    /* Zora punch     */ DMG_ENTRY(0, DORA_DMGEFF_LIGHT),
    /* Spin attack    */ DMG_ENTRY(1, DORA_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(0, DORA_DMGEFF_NONE),
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 8, 0, 0, 0, MASS_HEAVY };

void ObjDora_Init(Actor* thisx, PlayState* play) {
    ObjDora* this = (ObjDora*)thisx;
    s32 i;
    s32 j;
    Vec3f vtx[3];
    s32 buf = 0;
    s32 buff2 = 0;

    Actor_SetScale(&this->actor, 0.1f);
    ActorShape_Init(&this->actor.shape, 0.0f, &ActorShadow_DrawCircle, 36.0f);

    this->gongRotation.x = 0;
    this->gongRotation.z = 0;
    this->gongAngle.x = 0;
    this->gongAngle.z = 0;
    this->lastGongHitType = DORA_HIT_NONE;
    this->rupeeDropTimer = 0;
    this->collisionCooldownTimer = 0;
    this->unk3AA = 0;
    this->gongForce.x = 0.0f;
    this->gongForce.y = 0.0f;
    this->gongForce.z = 0.0f;

    Collider_InitTris(play, &this->colliderTris);
    Collider_SetTris(play, &this->colliderTris, &this->actor, &sTrisInit, this->colliderTrisElements);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                 &this->actor.shape.rot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

    for (i = 0; i < ARRAY_COUNT(this->colliderTrisElements); i++) {
        for (j = 0; j < ARRAY_COUNT(vtx); j++) {
            Matrix_MultVec3f(&sTrisElementsInit[i].dim.vtx[j], &vtx[j]);
        }
        Collider_SetTrisVertices(&this->colliderTris, i, &vtx[0], &vtx[1], &vtx[2]);
    }
    ObjDora_SetupWait(this);
}

void ObjDora_Destroy(Actor* thisx, PlayState* play) {
    ObjDora* this = (ObjDora*)thisx;

    Collider_DestroyTris(play, &this->colliderTris);
}

void ObjDora_SetupWait(ObjDora* this) {
    this->actionFunc = ObjDora_Wait;
}

void ObjDora_Wait(ObjDora* this, PlayState* play) {
}

void ObjDora_SetupMoveGong(ObjDora* this) {
    if (this->lastGongHitType == DORA_HIT_LIGHT) {
        this->gongForce.x = 2.0f;
        this->gongForce.y = 5461.0f;
        this->gongForce.z = 1820.0f;
    } else {
        this->gongForce.x = 4.0f;
        this->gongForce.y = 12743.0f;
        this->gongForce.z = 5461.0f;
    }
    this->gongAngle.x = 0;
    this->gongAngle.z = 0;
    this->actionFunc = ObjDora_MoveGong;
}

void ObjDora_MoveGong(ObjDora* this, PlayState* play) {

    if ((this->gongForce.y < 182.0f) && (this->gongForce.z < 182.0f)) {
        this->lastGongHitType = DORA_HIT_LIGHT;
        ObjDora_SetupWait(this);
    }

    Math_SmoothStepToF(&this->gongForce.x, 0, 0.2f, 0.2f, 0.1f);
    Math_SmoothStepToF(&this->gongForce.y, 0, 0.5f, 54.0f, 18.0f);
    Math_SmoothStepToF(&this->gongForce.z, 0, 0.5f, 54.0f, 18.0f);
    this->gongAngle.x += 0x1555;
    this->gongAngle.z += 0x238E;
    this->gongRotation.x = Math_SinS(this->gongAngle.x) * this->gongForce.y;
    this->gongRotation.z = Math_SinS(this->gongAngle.z) * this->gongForce.z;
}

s32 ObjDora_IsHalfHour(u16 time) {
    f32 timeHalfHour = time;

    timeHalfHour -= (CLOCK_TIME_F(0, 30) * (s32)(time / CLOCK_TIME_F(0, 30)));
    if ((timeHalfHour < CLOCK_TIME_F(0, 1)) || (CLOCK_TIME_F(0, 29) < timeHalfHour)) {
        return true;
    }

    return false;
}

void ObjDora_UpdateCollision(ObjDora* this, PlayState* play) {
    Actor* itemDrop;
    u16 time;

    if (this->colliderTris.base.acFlags & AC_HIT) {
        time = CURRENT_TIME;
        this->colliderTris.base.acFlags &= ~AC_HIT;
        this->collisionCooldownTimer = 5;

        switch (this->actor.colChkInfo.damageEffect) {
            case DORA_DMGEFF_STRONG:
            case DORA_DMGEFF_LIGHT:
                if (this->actor.colChkInfo.damageEffect == DORA_DMGEFF_LIGHT) {
                    Actor_PlaySfx(&this->actor, NA_SE_EV_DORA_S);
                    this->lastGongHitType = DORA_HIT_LIGHT;
                } else {
                    Actor_PlaySfx(&this->actor, NA_SE_EV_DORA_L);
                    this->lastGongHitType = DORA_HIT_STRONG;
                }

                Actor_RequestQuakeAndRumble(&this->actor, play, 5, 10);
                ObjDora_SetupMoveGong(this);

                if ((ObjDora_IsHalfHour(time) == true) && (this->rupeeDropTimer == 0)) {
                    Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
                    if (GameInteractor_Should(VB_GONG_DROP_COLLECTIBLE, true, this)) {
                        itemDrop = Item_DropCollectible(play, &this->actor.world.pos, ITEM00_RUPEE_BLUE);
                        itemDrop->world.rot.y = this->actor.world.rot.y;
                        itemDrop->world.rot.y += (s32)DEG_TO_BINANG_ALT3(Rand_Centered() * 90.0f);
                        itemDrop->velocity.y = 5.0f;
                        itemDrop->gravity = -1.0f;
                    }
                    this->rupeeDropTimer = 40;
                }
                break;
        }
    }

    if (this->rupeeDropTimer > 0) {
        this->rupeeDropTimer--;
    }

    if (this->collisionCooldownTimer > 0) {
        this->collisionCooldownTimer--;
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderTris.base);
    }
}

void ObjDora_Update(Actor* thisx, PlayState* play) {
    ObjDora* this = (ObjDora*)thisx;

    this->actionFunc(this, play);
    ObjDora_UpdateCollision(this, play);
}

void ObjDora_Draw(Actor* thisx, PlayState* play) {
    static Vec3f sPos = { 0.0f, -61.5f, 0.0f };
    ObjDora* this = (ObjDora*)thisx;
    f32 gongForceX;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->actionFunc == ObjDora_MoveGong) {
        gongForceX = this->gongForce.x;
        if ((play->state.frames % 2) != 0) {
            gongForceX *= -1.0f;
        }

        Matrix_Push();
        Matrix_RotateXS(this->gongRotation.x, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, &gDoraChainDL);

        Matrix_Translate(sPos.x, sPos.y + gongForceX, sPos.z + gongForceX, MTXMODE_APPLY);
        Matrix_RotateXS(this->gongRotation.z - this->gongRotation.x, MTXMODE_APPLY);
        Matrix_Translate(-sPos.x, -sPos.y, -sPos.z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, &gDoraGongDL);

        Matrix_Pop();
    } else {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, &gDoraGongDL);
        gSPDisplayList(POLY_OPA_DISP++, &gDoraChainDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
