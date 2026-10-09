/*
 * File: z_en_jc_mato.c
 * Overlay: ovl_En_Jc_Mato
 * Description: Boat Cruise Target
 */

#include "z_en_jc_mato.h"
#include "objects/object_tru/object_tru.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void EnJcMato_Init(Actor* thisx, PlayState* play);
void EnJcMato_Destroy(Actor* thisx, PlayState* play);
void EnJcMato_Update(Actor* thisx, PlayState* play);
void EnJcMato_Draw(Actor* thisx, PlayState* play);

s32 EnJcMato_CheckForHit(EnJcMato* this, PlayState* play);
void EnJcMato_SetupIdle(EnJcMato* this);
void EnJcMato_Idle(EnJcMato* this, PlayState* play);

ActorProfile En_Jc_Mato_Profile = {
    /**/ ACTOR_EN_JC_MATO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_TRU,
    /**/ sizeof(EnJcMato),
    /**/ EnJcMato_Init,
    /**/ EnJcMato_Destroy,
    /**/ EnJcMato_Update,
    /**/ EnJcMato_Draw,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 15 }, 100 },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0xF),
    /* Ice arrow      */ DMG_ENTRY(1, 0xF),
    /* Light arrow    */ DMG_ENTRY(1, 0xF),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

s32 EnJcMato_CheckForHit(EnJcMato* this, PlayState* play) {
    this->collider.dim.worldSphere.center.x = this->pos.x;
    this->collider.dim.worldSphere.center.y = this->pos.y;
    this->collider.dim.worldSphere.center.z = this->pos.z;
    if ((this->collider.base.acFlags & AC_HIT) && !this->hitFlag && (this->actor.colChkInfo.damageEffect == 0xF)) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
        play->interfaceCtx.minigamePoints = 1;
        this->hitFlag = true;
        return 1;
    } else {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        return 0;
    }
}

void EnJcMato_SetupIdle(EnJcMato* this) {
    this->actionFunc = EnJcMato_Idle;
}

void EnJcMato_Idle(EnJcMato* this, PlayState* play) {
    if (this->hitFlag) {
        if (DECR(this->despawnTimer) == 0) {
            Actor_Kill(&this->actor);
        }
    }
}

void EnJcMato_Init(Actor* thisx, PlayState* play) {
    EnJcMato* this = (EnJcMato*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    Collider_InitSphere(play, &this->collider);
    Collider_SetSphere(play, &this->collider, &this->actor, &sSphereInit);
    this->collider.dim.worldSphere.radius = 15;
    this->actor.colChkInfo.damageTable = &sDamageTable;
    Actor_SetScale(&this->actor, 0.008f);
    this->hitFlag = false;
    this->despawnTimer = 25;
    EnJcMato_SetupIdle(this);
}

void EnJcMato_Destroy(Actor* thisx, PlayState* play) {
    EnJcMato* this = (EnJcMato*)thisx;

    Collider_DestroySphere(play, &this->collider);
}

void EnJcMato_Update(Actor* thisx, PlayState* play) {
    EnJcMato* this = (EnJcMato*)thisx;

    this->actionFunc(this, play);
    if (!CHECK_EVENTINF(EVENTINF_40)) {
        EnJcMato_CheckForHit(this, play);
    }
}

void EnJcMato_Draw(Actor* thisx, PlayState* play) {
    static Vec3f sOffset = { 0.0f, -2500.0f, 0.0f };
    EnJcMato* this = (EnJcMato*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gKoumeTargetDL);
    Matrix_MultVec3f(&sOffset, &this->pos);

    CLOSE_DISPS(play->state.gfxCtx);
}
