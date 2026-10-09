/*
 * File: z_en_si.c
 * Overlay: ovl_En_Si
 * Description: Golden Skulltula Token
 */

#include "z_en_si.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void EnSi_Init(Actor* thisx, PlayState* play);
void EnSi_Destroy(Actor* thisx, PlayState* play);
void EnSi_Update(Actor* thisx, PlayState* play);
void EnSi_Draw(Actor* thisx, PlayState* play);

void EnSi_DraggedByHookshot(EnSi* this, PlayState* play);

ActorProfile En_Si_Profile = {
    /**/ ACTOR_EN_SI,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_ST,
    /**/ sizeof(EnSi),
    /**/ EnSi_Init,
    /**/ EnSi_Destroy,
    /**/ EnSi_Update,
    /**/ EnSi_Draw,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 10 }, 100 },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
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

void EnSi_UpdateCollision(EnSi* this, PlayState* play) {
    this->collider.dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.dim.worldSphere.center.y = this->actor.world.pos.y;
    this->collider.dim.worldSphere.center.z = this->actor.world.pos.z;
    this->collider.dim.worldSphere.radius = this->collider.dim.modelSphere.radius * this->collider.dim.scale;
    if (this->actor.colChkInfo.health > 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnSi_GiveToken(EnSi* this, PlayState* play) {
    s32 chestFlag = ENSI_GET_CHEST_FLAG(&this->actor);

    if ((chestFlag < 0x20) && (chestFlag >= 0)) {
        Flags_SetTreasure(play, chestFlag);
    }

    if (!GameInteractor_Should(VB_GIVE_ITEM_FROM_SI, true, this)) {
        return;
    }

    Item_Give(play, ITEM_SKULL_TOKEN);
    if (Inventory_GetSkullTokenCount(play->sceneId) >= SPIDER_HOUSE_TOKENS_REQUIRED) {
        Message_StartTextbox(play, 0xFC, NULL); // You collected all tokens, curse lifted
        // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't seem to
        // anything bad.
        Audio_PlayFanfare(NA_BGM_GET_ITEM);
    } else {
        Message_StartTextbox(play, 0x52, NULL); // You got one more gold token, your [count] one!
        Audio_PlayFanfare(NA_BGM_GET_SMALL_ITEM);
    }
}

void EnSi_Wait(EnSi* this, PlayState* play) {
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        this->actionFunc = EnSi_DraggedByHookshot;
    } else if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        EnSi_GiveToken(this, play);
        Actor_Kill(&this->actor);
        return;
    }
    this->actor.shape.rot.y += 0x38E;
}

void EnSi_DraggedByHookshot(EnSi* this, PlayState* play) {
    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        EnSi_GiveToken(this, play);
        Actor_Kill(&this->actor);
    }
}

void EnSi_Init(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    Collider_InitSphere(play, &this->collider);
    Collider_SetSphere(play, &this->collider, &this->actor, &sSphereInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.25f);
    this->actionFunc = EnSi_Wait;
}

void EnSi_Destroy(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    Collider_DestroySphere(play, &this->collider);
}

void EnSi_Update(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    this->actionFunc(this, play);
    EnSi_UpdateCollision(this, play);
    Actor_SetFocus(&this->actor, 0.0f);
}

void EnSi_Draw(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    func_800B8118(&this->actor, play, 0);
    func_800B8050(&this->actor, play, 0);
    GetItem_Draw(play, GID_SKULL_TOKEN_2);
}
