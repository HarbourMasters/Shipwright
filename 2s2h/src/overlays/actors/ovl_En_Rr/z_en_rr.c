/*
 * File: z_en_rr.c
 * Overlay: ovl_En_Rr
 * Description: Like Like
 */

#include "z_en_rr.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/object_rr/object_rr.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnRr_Init(Actor* thisx, PlayState* play);
void EnRr_Destroy(Actor* thisx, PlayState* play);
void EnRr_Update(Actor* thisx, PlayState* play);
void EnRr_Draw(Actor* thisx, PlayState* play2);

void func_808FAF94(EnRr* this, PlayState* play);
void func_808FB088(EnRr* this, PlayState* play);
void func_808FB1C0(EnRr* this, PlayState* play);
void func_808FB2C0(EnRr* this, PlayState* play);
void func_808FB42C(EnRr* this, PlayState* play);
void func_808FB680(EnRr* this, PlayState* play);
void func_808FB710(EnRr* this, PlayState* play);

void func_808FAD1C(EnRr* this, PlayState* play);
void func_808FB398(EnRr* this, PlayState* play);

ActorProfile En_Rr_Profile = {
    /**/ ACTOR_EN_RR,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_RR,
    /**/ sizeof(EnRr),
    /**/ EnRr_Init,
    /**/ EnRr_Destroy,
    /**/ EnRr_Update,
    /**/ EnRr_Draw,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x20000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 45, 60, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x20000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_NONE,
    },
    { 30, 45, -30, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(3, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x1),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xE),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(3, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(2, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(2, 0x5),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(2, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

static CollisionCheckInfoInit sColChkInfoInit = { 3, 45, 60, 250 };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_LIKE_LIKE, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void EnRr_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRr* this = (EnRr*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitAndSetCylinder(play, &this->collider1, &this->actor, &sCylinderInit1);
    Collider_InitAndSetCylinder(play, &this->collider2, &this->actor, &sCylinderInit2);
    if (this->actor.params != LIKE_LIKE_PARAM_3) {
        this->actor.scale.y = 0.015f;
        this->actor.scale.x = 0.019f;
        this->actor.scale.z = 0.019f;
    } else {
        this->actor.scale.y = 0.022499999f;
        this->actor.scale.x = 0.028499998f;
        this->actor.scale.z = 0.028499998f;
        this->collider1.dim.radius *= 1.5f;
        this->collider1.dim.height *= 1.5f;
        this->collider2.dim.radius *= 1.5f;
        this->collider2.dim.height *= 1.5f;
        this->collider2.dim.yShift *= 1.5f;
    }

    Collider_UpdateCylinder(&this->actor, &this->collider2);
    Actor_SetFocus(&this->actor, this->actor.scale.y * 2000.0f);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if ((this->actor.params == LIKE_LIKE_PARAM_2) || (this->actor.params == LIKE_LIKE_PARAM_3)) {
        this->actor.colChkInfo.health = 6;
        if (this->actor.params == LIKE_LIKE_PARAM_2) {
            this->actor.colChkInfo.mass = MASS_HEAVY;
        }
    }

    this->actionFunc = func_808FAF94;
    func_808FAD1C(this, play);
}

void EnRr_Destroy(Actor* thisx, PlayState* play) {
    EnRr* this = (EnRr*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
}

void func_808FA01C(EnRr* this, PlayState* play, ColliderCylinder* collider) {
    if (this->actor.colChkInfo.damageEffect == 2) {
        this->drawDmgEffScale = 0.85f;
        this->drawDmgEffAlpha = 4.0f;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    } else if (this->actor.colChkInfo.damageEffect == 4) {
        this->drawDmgEffScale = 0.85f;
        this->drawDmgEffAlpha = 4.0f;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, collider->elem.acDmgInfo.hitPos.x,
                    collider->elem.acDmgInfo.hitPos.y, collider->elem.acDmgInfo.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
    } else if (this->actor.colChkInfo.damageEffect == 5) {
        this->drawDmgEffScale = 0.85f;
        this->drawDmgEffAlpha = 4.0f;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
    }
}

void func_808FA11C(EnRr* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->collider1.base.colMaterial = COL_MATERIAL_HIT3;
    this->collider1.elem.elemMaterial = ELEM_MATERIAL_UNK0;
    this->unk_1EE = 80;
    this->drawDmgEffScale = 0.85f;
    this->drawDmgEffFrozenSteamScale = 1275.0f * 0.001f;
    this->drawDmgEffAlpha = 1.0f;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void func_808FA19C(EnRr* this, PlayState* play) {
    this->unk_1EE = 0;
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->collider1.base.colMaterial = COL_MATERIAL_HIT0;
        this->collider1.elem.elemMaterial = ELEM_MATERIAL_UNK1;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, LIKE_LIKE_BODYPART_MAX, 2,
                              this->actor.scale.y * 23.333334f, this->actor.scale.y * 20.000002f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    }
}

void func_808FA238(EnRr* this, f32 arg1) {
    this->actor.speed = arg1;
    Actor_PlaySfx(&this->actor, NA_SE_EN_LIKE_WALK);
}

void func_808FA260(EnRr* this) {
    static f32 D_808FC1E4[] = { 0.0f, 500.0f, 750.0f, 1000.0f, 1000.0f };
    s32 i;

    this->unk_1E1 = 1;
    this->unk_1E6 = 20;
    this->unk_1F6 = 2500;
    this->unk_210 = 0.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = D_808FC1E4[i];
        this->unk_324[i].unk_14 = 6000;
        this->unk_324[i].unk_18 = 0;
        this->unk_324[i].unk_0C = 0.8f;
    }

    this->actionFunc = func_808FB088;

    Actor_PlaySfx(&this->actor, NA_SE_EN_LIKE_UNARI);
}

void func_808FA344(EnRr* this) {
    s32 i;

    this->unk_1E1 = 0;
    this->unk_210 = 0.0f;
    this->unk_1F6 = 2500;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
        this->unk_324[i].unk_0C = 1.0f;
    }

    if (this->unk_1E2 != 0) {
        this->unk_1E6 = 100;
        this->actionFunc = func_808FB680;
    } else {
        this->unk_1E6 = 60;
        this->actionFunc = func_808FAF94;
    }
}

void func_808FA3F8(EnRr* this, Player* player) {
    s32 i;

    this->unk_1EA = 100;
    this->unk_1FC = 20;
    this->collider1.base.ocFlags1 &= ~OC1_TYPE_PLAYER;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_1F0 = 8;
    this->unk_1E1 = 0;
    this->actor.speed = 0.0f;
    this->unk_218 = 0.0f;
    this->unk_210 = 0.0f;
    this->unk_204 = 0.15f;
    this->unk_20C = 0x200;
    this->unk_1F6 = 5000;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB1C0;
    Actor_PlaySfx(&this->actor, NA_SE_EN_SUISEN_DRINK);
}

void func_808FA4F4(EnRr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u32 sp38;
    s32 sp34;
    f32 sp30;
    f32 sp2C;

    if (player->stateFlags2 & PLAYER_STATE2_80) {
        player->actor.parent = NULL;
        player->av2.actionVar2 = 100;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->unk_1F0 = 110;
        this->unk_1F6 = 2500;
        this->unk_210 = 0.0f;
        this->unk_20C = 0x800;

        if (((this->unk_1E2 == 0) && (GET_PLAYER_FORM == PLAYER_FORM_HUMAN)) &&
            (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_HERO)) {
            sp34 = true;
            this->unk_1E2 = Inventory_DeleteEquipment(play, EQUIP_VALUE_SHIELD_HERO);
        } else {
            sp34 = false;
        }

        if (sp34 && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
            Message_StartTextbox(play, 0xF6, NULL);
        }

        if (this->actor.params == LIKE_LIKE_PARAM_0) {
            sp38 = 8;
        } else {
            sp38 = 16;
        }

        sp30 = this->actor.scale.x * 210.52632f;
        sp2C = this->actor.scale.x * 631.579f;

        player->actor.world.pos.x += sp30 * Math_SinS(this->actor.shape.rot.y);
        player->actor.world.pos.y += sp2C;
        player->actor.world.pos.z += sp30 * Math_CosS(this->actor.shape.rot.y);

        func_800B8D50(play, &this->actor, sp30, this->actor.shape.rot.y, sp2C, sp38);
        Actor_PlaySfx(&this->actor, NA_SE_EN_SUISEN_THROW);
    }
}

void func_808FA6B8(EnRr* this) {
    s32 i;

    this->unk_1E1 = 0;
    if (this->actor.colChkInfo.damageEffect == 5) {
        this->unk_1EC = 80;
    } else {
        this->unk_1EC = 40;
    }
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, this->unk_1EC);

    this->unk_1E6 = 20;
    this->unk_1F6 = 2500;
    this->unk_210 = 0.0f;
    this->unk_204 = 0.0f;
    this->unk_20C = 0.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB398;
    Actor_PlaySfx(&this->actor, NA_SE_EN_LIKE_DAMAGE);
}

void func_808FA7AC(EnRr* this) {
    static f32 D_808FC1F8[] = { 0.0f, 500.0f, 750.0f, 1000.0f, 1000.0f, 0.0f };
    s32 i;

    this->unk_1F6 = 2500;
    this->unk_1E1 = 1;
    this->unk_1E6 = 10;
    this->unk_210 = 0.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = D_808FC1F8[i];
        this->unk_324[i].unk_0C = 0.8f;
        this->unk_324[i].unk_14 = 5000;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB2C0;
}

void func_808FA870(EnRr* this) {
    s32 i;

    this->unk_210 = 0.0f;
    this->unk_204 = 0.15f;
    this->unk_20C = 0x800;
    this->unk_1F6 = 2500;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FAF94;
}

void func_808FA910(EnRr* this) {
    s32 i;

    this->unk_1E4 = 0;
    this->unk_214 = 0.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    this->unk_210 = 0.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_04 = 0.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB42C;
    Actor_PlaySfx(&this->actor, NA_SE_EN_LIKE_DEAD);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void func_808FA9CC(EnRr* this) {
    s32 i;

    this->unk_1F2 = 0;
    this->unk_1F4 = 0;
    this->unk_1F6 = 2500;
    this->unk_1F8 = 0;
    this->unk_1FA = 0;
    this->unk_200 = 0.0f;
    this->unk_204 = 0.15f;
    this->unk_208 = 0.0f;
    this->unk_20C = 0x800;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_08 = 1.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_16 = 0;
        this->unk_324[i].unk_18 = 0;
    }

    this->actionFunc = func_808FB710;
}

s32 func_808FAA94(EnRr* this, PlayState* play) {
    ColliderCylinder* sp2C;
    s32 flag = (this->collider1.base.acFlags & AC_HIT) != 0;

    if (flag || (this->collider2.base.acFlags & AC_HIT)) {
        if (flag) {
            sp2C = &this->collider1;
        } else {
            sp2C = &this->collider2;
        }

        this->collider1.base.acFlags &= ~AC_HIT;
        this->collider2.base.acFlags &= ~AC_HIT;

        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) &&
            (sp2C->elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            return false;
        }

        if (this->actor.colChkInfo.damageEffect == 14) {
            return false;
        }

        Actor_SetDropFlag(&this->actor, &sp2C->elem);
        func_808FA4F4(this, play);
        func_808FA19C(this, play);

        if (!Actor_ApplyDamage(&this->actor)) {
            Enemy_StartFinishingBlow(play, &this->actor);
            if (this->actor.colChkInfo.damageEffect == 3) {
                func_808FA11C(this);
                this->collider1.base.acFlags &= ~AC_ON;
                this->collider2.base.acFlags &= ~AC_ON;
                func_808FA9CC(this);
            } else {
                func_808FA01C(this, play, sp2C);
                func_808FA910(this);
            }
        } else if (this->actor.colChkInfo.damageEffect == 1) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 80);
            this->unk_1EE = 80;
            func_808FA9CC(this);
        } else if (this->actor.colChkInfo.damageEffect == 3) {
            func_808FA11C(this);
            func_808FA9CC(this);
        } else {
            func_808FA01C(this, play, sp2C);
            func_808FA6B8(this);
        }
        return true;
    }
    return false;
}

void func_808FAC80(EnRr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->unk_1F0 == 0) && ((this->collider2.base.atFlags & AT_HIT) || (this->collider1.base.atFlags & AT_HIT))) {
        this->collider1.base.atFlags &= ~AT_HIT;
        this->collider2.base.atFlags &= ~AT_HIT;
        if (play->grabPlayer(play, player)) {
            player->actor.parent = &this->actor;
            func_808FA3F8(this, player);
        }
    }
}

void func_808FAD1C(EnRr* this, PlayState* play) {
    s32 i;
    EnRrStruct* ptr;

    this->unk_1F2 = 0;
    this->unk_1F4 = 0;
    this->unk_1F6 = 2500;
    this->unk_1F8 = 0;
    this->unk_1FA = 0;
    this->unk_200 = 0.0f;
    this->unk_204 = 0.15f;
    this->unk_208 = 0.0f;
    this->unk_20C = 0x800;

    for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_14 = 0;
        this->unk_324[i].unk_16 = 0;
        this->unk_324[i].unk_18 = 0;

        this->unk_324[i].unk_08 = 1.0f;
        this->unk_324[i].unk_0C = 1.0f;
        this->unk_324[i].unk_10 = this->unk_200;
    }

    for (i = 1; i < ARRAY_COUNT(this->unk_324); i++) {
        ptr = &this->unk_324[i];
        ptr->unk_14 = Math_CosS(this->unk_1F8 * i) * this->unk_208;
        ptr->unk_18 = Math_SinS(this->unk_1FA * i) * this->unk_208;
    }
}

void func_808FAE50(EnRr* this, PlayState* play) {
    s32 i;
    EnRrStruct* ptr;

    if (this->actionFunc != func_808FB42C) {
        for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
            ptr = &this->unk_324[i];
            ptr->unk_10 = Math_CosS(this->unk_1F2 + (i * 0x4000)) * this->unk_200;
        }

        if (this->unk_1E1 == 0) {
            for (i = 1; i < ARRAY_COUNT(this->unk_324); i++) {
                ptr = &this->unk_324[i];
                ptr->unk_14 = Math_CosS(this->unk_1F2 + (i * this->unk_1F8)) * this->unk_208;
                ptr->unk_18 = Math_SinS(this->unk_1F2 + (i * this->unk_1FA)) * this->unk_208;
            }
        }
    }

    if (this->unk_1EE == 0) {
        this->unk_1F2 += this->unk_1F4;
    }
}

void func_808FAF94(EnRr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 10, 0x1F4, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((this->unk_1E6 == 0) && !(player->stateFlags2 & PLAYER_STATE2_80) &&
        (Player_GetMask(play) != PLAYER_MASK_STONE) &&
        (this->actor.xzDistToPlayer < (8421.053f * this->actor.scale.x))) {
        func_808FA260(this);
    } else if ((this->actor.xzDistToPlayer < 400.0f) && (this->actor.speed == 0.0f)) {
        func_808FA238(this, 2.0f);
    }
}

void func_808FB088(EnRr* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 10, 0x1F4, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Player_GetMask(play) == PLAYER_MASK_STONE) {
        func_808FA344(this);
        return;
    }

    switch (this->unk_1E1) {
        case 1:
            if (this->unk_1E6 == 0) {
                this->unk_1E1 = 2;
            }
            break;

        case 2:
            if (this->unk_1E6 == 0) {
                this->unk_324[4].unk_0C = 1.5f;
                this->unk_1E6 = 5;
                this->unk_1E1 = 3;
            }
            break;

        case 3:
            if (this->unk_1E6 == 0) {
                this->unk_1E6 = 2;
                this->unk_324[4].unk_04 = 2000.0f;
                this->unk_1E1 = 4;
            }
            break;

        case 4:
            if (this->unk_1E6 == 0) {
                this->unk_324[4].unk_0C = 0.8f;
                this->unk_1E6 = 20;
                this->unk_1E1 = 5;
            }
            break;

        case 5:
            if (this->unk_1E6 == 0) {
                func_808FA344(this);
            }
            break;

        default:
            break;
    }
}

void func_808FB1C0(EnRr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Rumble_Request(this->actor.xyzDistToPlayerSq, 120, 2, 120);
    if (!(this->unk_1E4 & 7)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_DEMO_EYE);
    }

    player->av2.actionVar2 = 0;
    this->unk_1F0 = 8;
    this->unk_1EA--;

    if (this->unk_1EA == 0) {
        func_808FA7AC(this);
    } else {
        Math_StepToF(&player->actor.world.pos.x, this->unk_228.x, 30.0f);
        Math_StepToF(&player->actor.world.pos.y, this->unk_228.y + this->unk_218, 30.0f);
        Math_StepToF(&player->actor.world.pos.z, this->unk_228.z, 30.0f);
        Math_StepToF(&this->unk_218, -(f32)this->collider1.dim.height, 5.0f);
    }
}

void func_808FB2C0(EnRr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_1E6--;
    player->av2.actionVar2 = 0;
    Math_StepToF(&player->actor.world.pos.x, this->unk_228.x, 30.0f);
    Math_StepToF(&player->actor.world.pos.y, this->unk_228.y + this->unk_218, 30.0f);
    Math_StepToF(&player->actor.world.pos.z, this->unk_228.z, 30.0f);
    Math_StepToF(&this->unk_218, -(f32)this->collider1.dim.height, 5.0f);
    if (this->unk_1E6 == 0) {
        this->unk_1E1 = 0;
        func_808FA4F4(this, play);
        func_808FA344(this);
    }
}

void func_808FB398(EnRr* this, PlayState* play) {
    s32 i;
    s16 phi_v1;

    this->unk_1EC--;
    if (this->unk_1EC == 0) {
        func_808FA870(this);
        return;
    }

    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL) {
        if (this->unk_1EC & 2) {
            phi_v1 = 1000;
        } else {
            phi_v1 = -1000;
        }
    } else {
        if (this->unk_1EC & 8) {
            phi_v1 = 5000;
        } else {
            phi_v1 = -5000;
        }
    }

    for (i = 1; i < ARRAY_COUNT(this->unk_324); i++) {
        this->unk_324[i].unk_18 = phi_v1;
    }
}

void func_808FB42C(EnRr* this, PlayState* play) {
    s32 pad;
    s32 i;
    EnRrStruct* ptr;
    f32 temp_f20;

    this->actor.colorFilterTimer = 40;
    if (this->unk_1E4 < 40) {
        for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
            ptr = &this->unk_324[i];
            Math_StepToF(&ptr->unk_04, (i + 59) - (this->unk_1E4 * 25.0f), 50.0f);
            ptr->unk_0C = (SQ((f32)(4 - i)) * this->unk_1E4 * 0.003f) + 1.0f;
        }
        return;
    }

    if (this->unk_1E4 >= 95) {
        if (this->unk_1E2 != 0) {
            Item_DropCollectible(play, &this->actor.world.pos, ITEM00_SHIELD_HERO);
        }
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x90);
        Actor_Kill(&this->actor);
        return;
    }

    if (this->unk_1E4 == 88) {
        Vec3f sp74;

        sp74.x = this->actor.world.pos.x;
        sp74.y = this->actor.world.pos.y + 20.0f;
        sp74.z = this->actor.world.pos.z;
        func_800B3030(play, &sp74, &gZeroVec3f, &gZeroVec3f, 100, 0, 0);
        SoundSource_PlaySfxAtFixedWorldPos(play, &sp74, 11, NA_SE_EN_EXTINCT);
    } else {
        temp_f20 = this->actor.scale.y * 66.66667f;

        Math_StepToF(&this->actor.scale.x, 0.0f, this->unk_214);
        Math_StepToF(&this->unk_214, 0.001f * temp_f20, 0.00001f * temp_f20);
        this->actor.scale.z = this->actor.scale.x;
    }
}

void func_808FB680(EnRr* this, PlayState* play) {
    if (this->unk_1E6 == 0) {
        this->actionFunc = func_808FAF94;
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, BINANG_ROT180(this->actor.yawTowardsPlayer), 10, 0x3E8, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->actor.speed == 0.0f) {
            func_808FA238(this, 2.0f);
        }
    }
}

void func_808FB710(EnRr* this, PlayState* play) {
    this->unk_1EE--;
    if (this->unk_1EE == 0) {
        func_808FA19C(this, play);
        func_808FA870(this);
        this->actionFunc = func_808FAF94;
    } else if ((this->actor.colChkInfo.health == 0) && (this->unk_1EE == 77)) {
        func_808FA19C(this, play);
        func_808FA910(this);
    }
}

void func_808FB794(EnRr* this, PlayState* play) {
    Vec3f sp2C;

    if ((this->actor.depthInWater < this->collider1.dim.height) && (this->actor.depthInWater > 1.0f) &&
        ((play->gameplayFrames % 9) == 0)) {
        sp2C.x = this->actor.world.pos.x;
        sp2C.y = this->actor.world.pos.y + this->actor.depthInWater;
        sp2C.z = this->actor.world.pos.z;
        EffectSsGRipple_Spawn(play, &sp2C, this->actor.scale.x * 34210.527f, this->actor.scale.x * 60526.316f, 0);
    }
}

void EnRr_Update(Actor* thisx, PlayState* play) {
    EnRr* this = (EnRr*)thisx;
    EnRrStruct* ptr;
    s32 i;

    this->unk_1E4++;

    if (this->unk_1EE == 0) {
        this->unk_1E8++;
    }

    if (this->unk_1E6 != 0) {
        this->unk_1E6--;
    }

    if (this->unk_1F0 != 0) {
        this->unk_1F0--;
    }

    Actor_SetFocus(&this->actor, this->actor.scale.y * 2000.0f);
    func_808FAE50(this, play);

    if (!func_808FAA94(this, play)) {
        func_808FAC80(this, play);
    }

    this->actionFunc(this, play);

    if (this->actor.params == LIKE_LIKE_PARAM_2) {
        //! @bug: Actor will constantly play an SFX every frame when player is close enough.
        //! In `func_808FAF94` the check is if the actor has zero speed.
        this->actor.speed = 0.0f;
    } else {
        Math_StepToF(&this->actor.speed, 0.0f, 0.1f);
    }

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, this->collider1.dim.radius, 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10 | UPDBGCHECKINFO_FLAG_40);
    func_808FB794(this, play);

    if (this->unk_1FC > 0) {
        Player* player = GET_PLAYER(play);

        if (!(player->stateFlags2 & PLAYER_STATE2_80)) {
            this->unk_1FC--;
            if (this->unk_1FC == 0) {
                this->collider1.base.ocFlags1 |= OC1_TYPE_PLAYER;
            }
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->collider1);

    if ((this->actionFunc != func_808FB42C) && (this->actionFunc != func_808FB398)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
        if ((this->unk_1F0 == 0) && (this->actionFunc == func_808FB088) && (this->unk_1EE == 0)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider2.base);
        } else {
            this->collider2.base.atFlags &= ~AT_HIT;
            this->collider1.base.atFlags &= ~AT_HIT;
        }
    } else {
        this->collider2.base.atFlags &= ~AT_HIT;
        this->collider1.base.atFlags &= ~AT_HIT;
        this->collider2.base.acFlags &= ~AC_HIT;
        this->collider1.base.acFlags &= ~AC_HIT;
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);

    if (this->unk_1EE == 0) {
        Math_ScaledStepToS(&this->unk_1F4, this->unk_1F6, 50);
        Math_ScaledStepToS(&this->unk_1F8, 0x3000, 0xA4);
        Math_ScaledStepToS(&this->unk_1FA, 0x1000, 0x29);
        Math_StepToF(&this->unk_200, this->unk_204, 0.0015f);
        Math_StepToF(&this->unk_208, this->unk_20C, 20.0f);

        for (i = 0; i < ARRAY_COUNT(this->unk_324); i++) {
            ptr = &this->unk_324[i];
            Math_SmoothStepToS(&ptr->unk_1A.x, ptr->unk_14, 5, this->unk_210 * 0x3E8, 0);
            Math_SmoothStepToS(&ptr->unk_1A.z, ptr->unk_18, 5, this->unk_210 * 0x3E8, 0);
            Math_StepToF(&ptr->unk_08, ptr->unk_0C, this->unk_210 * 0.2f);
            Math_StepToF(&ptr->unk_00, ptr->unk_04, this->unk_210 * 300.0f);
        }

        Math_StepToF(&this->unk_210, 1.0f, 0.2f);
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.425f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.85f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.85f, 0.02125f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

void EnRr_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnRr* this = (EnRr*)thisx;
    Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, 4 * sizeof(Mtx));
    Vec3f* bodyPartPos;
    s32 i;
    EnRrStruct* ptr;
    Vec3f spA4;
    f32 temp_f20;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, mtx);
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (this->unk_1E8 * 0) & 0x7F, (this->unk_1E8 * 0) & 0x3F, 0x20,
                                  0x10, 1, (this->unk_1E8 * 0) & 0x3F, (this->unk_1E8 * -6) & 0x7F, 0x20, 0x10, 0, 0, 0,
                                  -6));

    Matrix_Push();
    f32 likeLikeWidth = CVarGetFloat(CVAR_COSMETIC("Silly.LikeLikeWidth"), 1.0f);

    Matrix_Scale((1.0f + this->unk_324[0].unk_10) * this->unk_324[0].unk_08 * likeLikeWidth, 1.0f,
                 (1.0f + this->unk_324[0].unk_10) * this->unk_324[0].unk_08 * likeLikeWidth, MTXMODE_APPLY);

    bodyPartPos = &this->bodyPartsPos[0];

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    // LIKE_LIKE_BODYPART_0 - LIKE_LIKE_BODYPART_3
    Matrix_MultVecZ(1842.1053f, bodyPartPos++);
    Matrix_MultVecZ(-1842.1053f, bodyPartPos++);
    Matrix_MultVecX(1842.1053f, bodyPartPos++);
    Matrix_MultVecX(-1842.1053f, bodyPartPos++);
    Matrix_Pop();

    for (i = 1; i < ARRAY_COUNT(this->unk_324); i++) {
        temp_f20 = this->unk_324[i].unk_08 * (this->unk_324[i].unk_10 + 1.0f) *
                   CVarGetFloat(CVAR_COSMETIC("Silly.LikeLikeWidth"), 1.0f);
        ptr = &this->unk_324[i];

        Matrix_Translate(0.0f, ptr->unk_00 + 1000.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateZYX(ptr->unk_1A.x, ptr->unk_1A.y, ptr->unk_1A.z, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_Scale(temp_f20, 1.0f, temp_f20, MTXMODE_APPLY);
        Matrix_ToMtx(mtx);

        if ((i & 1) != 0) {
            Matrix_RotateYS(0x2000, MTXMODE_APPLY);
        }

        // LIKE_LIKE_BODYPART_4 - LIKE_LIKE_BODYPART_7
        // LIKE_LIKE_BODYPART_8 - LIKE_LIKE_BODYPART_11
        // LIKE_LIKE_BODYPART_12 - LIKE_LIKE_BODYPART_15
        // LIKE_LIKE_BODYPART_16 - LIKE_LIKE_BODYPART_19
        Matrix_MultVecZ(1842.1053f, bodyPartPos++);
        Matrix_MultVecZ(-1842.1053f, bodyPartPos++);
        Matrix_MultVecX(1842.1053f, bodyPartPos++);
        Matrix_MultVecX(-1842.1053f, bodyPartPos++);
        Matrix_Pop();
        mtx++;
        if (i == 3) {
            Matrix_MultZero(&spA4);
        }
    }

    Matrix_MultZero(&this->unk_228);
    this->collider2.dim.pos.x = LERPIMP(spA4.x, this->unk_228.x, 0.85f);
    this->collider2.dim.pos.y = LERPIMP(spA4.y, this->unk_228.y, 0.85f);
    this->collider2.dim.pos.z = LERPIMP(spA4.z, this->unk_228.z, 0.85f);

    gSPDisplayList(POLY_OPA_DISP++, gLikeLikeDL);

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, LIKE_LIKE_BODYPART_MAX,
                            this->actor.scale.y * 66.66667f * this->drawDmgEffScale, this->drawDmgEffFrozenSteamScale,
                            this->drawDmgEffAlpha, this->drawDmgEffType);

    CLOSE_DISPS(play->state.gfxCtx);
}
