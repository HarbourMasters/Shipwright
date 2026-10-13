/*
 * File: z_en_maruta.c
 * Overlay: ovl_En_Maruta
 * Description: Swordsman's School - Practice Log
 */

#include "z_en_maruta.h"
#include "overlays/actors/ovl_En_Kendo_Js/z_en_kendo_js.h"
#include "objects/object_maruta/object_maruta.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnMaruta_Init(Actor* thisx, PlayState* play);
void EnMaruta_Destroy(Actor* thisx, PlayState* play);
void EnMaruta_Update(Actor* thisx, PlayState* play);
void EnMaruta_Draw(Actor* thisx, PlayState* play);

void func_80B372B8(EnMaruta* this);
void func_80B372CC(EnMaruta* this, PlayState* play);
void func_80B37364(EnMaruta* this);
void func_80B3738C(EnMaruta* this, PlayState* play);
void func_80B373F4(EnMaruta* this);
void func_80B37428(EnMaruta* this, PlayState* play);
void func_80B374FC(EnMaruta* this, PlayState* play);
void func_80B37590(EnMaruta* this, PlayState* play);
void func_80B37950(EnMaruta* this, PlayState* play);
void func_80B379C0(EnMaruta* this, PlayState* play);
void func_80B37A14(EnMaruta* this);
void func_80B37A64(EnMaruta* this, PlayState* play);
void func_80B37AA0(EnMaruta* this, PlayState* play);
void func_80B37C04(s16* arg0);
void func_80B37EC0(EnMaruta* this, PlayState* play);
void func_80B38060(EnMaruta* this, Vec3f* arg1);
void func_80B3828C(Vec3f* arg0, Vec3f* arg1, s16 arg2, s16 arg3, s32 arg4);
void func_80B382E4(PlayState* play, Vec3f arg1);

ActorProfile En_Maruta_Profile = {
    /**/ ACTOR_EN_MARUTA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_MARUTA,
    /**/ sizeof(EnMaruta),
    /**/ EnMaruta_Init,
    /**/ EnMaruta_Destroy,
    /**/ EnMaruta_Update,
    /**/ EnMaruta_Draw,
};

Gfx* D_80B386A0[] = {
    object_maruta_DL_002220, object_maruta_DL_0023D0, object_maruta_DL_002568, object_maruta_DL_002660,
    object_maruta_DL_002758, object_maruta_DL_002850, object_maruta_DL_002948, object_maruta_DL_002AE0,
};

u8 D_80B386C0[] = {
    0xFF, 0x2B, 0xD4, 0x17, 0xE8, 0x55, 0xAA, 0x0F, 0xF0,
};

s32 D_80B386CC[] = {
    5, // PLAYER_MWA_FORWARD_SLASH_1H
    5, // PLAYER_MWA_FORWARD_SLASH_2H
    3, // PLAYER_MWA_FORWARD_COMBO_1H
    3, // PLAYER_MWA_FORWARD_COMBO_2H
    7, // PLAYER_MWA_RIGHT_SLASH_1H
    7, // PLAYER_MWA_RIGHT_SLASH_2H
    7, // PLAYER_MWA_RIGHT_COMBO_1H
    7, // PLAYER_MWA_RIGHT_COMBO_2H
    3, // PLAYER_MWA_LEFT_SLASH_1H
    3, // PLAYER_MWA_LEFT_SLASH_2H
    3, // PLAYER_MWA_LEFT_COMBO_1H
    3, // PLAYER_MWA_LEFT_COMBO_2H
    7, // PLAYER_MWA_STAB_1H
    7, // PLAYER_MWA_STAB_2H
    3, // PLAYER_MWA_STAB_COMBO_1H
    3, // PLAYER_MWA_STAB_COMBO_2H
    0, // PLAYER_MWA_FLIPSLASH_START
    0, // PLAYER_MWA_JUMPSLASH_START
    0, // PLAYER_MWA_ZORA_JUMPKICK_START
    0, // PLAYER_MWA_FLIPSLASH_FINISH
    5, // PLAYER_MWA_JUMPSLASH_FINISH
    0, // PLAYER_MWA_ZORA_JUMPKICK_FINISH
    0, // PLAYER_MWA_BACKSLASH_RIGHT
    0, // PLAYER_MWA_BACKSLASH_LEFT
    0, // PLAYER_MWA_GORON_PUNCH_LEFT
    0, // PLAYER_MWA_GORON_PUNCH_RIGHT
    0, // PLAYER_MWA_GORON_PUNCH_BUTT
    0, // PLAYER_MWA_ZORA_PUNCH_LEFT
    0, // PLAYER_MWA_ZORA_PUNCH_COMBO
    0, // PLAYER_MWA_ZORA_PUNCH_KICK
    7, // PLAYER_MWA_SPIN_ATTACK_1H
    7, // PLAYER_MWA_SPIN_ATTACK_2H
    7, // PLAYER_MWA_BIG_SPIN_1H
    7  // PLAYER_MWA_BIG_SPIN_2H
};

Vec3f D_80B38754 = { -2.0f, 3.0f, 0.0f };
Vec3f D_80B38760 = { -2.0f, 3.0f, 0.0f };
Vec3f D_80B3876C = { 2.0f, 3.0f, 0.0f };
Vec3f D_80B38778 = { -2.0f, 10.0f, 0.0f };
Vec3f D_80B38784 = { -2.5f, 5.0f, 0.0f };
Vec3f D_80B38790 = { -3.0f, 10.0f, 0.0f };
Vec3f D_80B3879C = { 2.5f, 5.0f, 0.0f };
Vec3f D_80B387A8 = { 3.0f, 10.0f, 0.0f };
Vec3f D_80B387B4 = { -1.0f, 7.0f, -1.0f };
Vec3f D_80B387C0 = { 1.0f, 7.0f, -1.0f };
Vec3f D_80B387CC = { 0.0f, 8.0f, -1.5f };
Vec3f D_80B387D8 = { 0.0f, 8.0f, -1.5f };

Vec3f D_80B387E4[] = {
    { 0.0f, 0.0f, 0.0f },    { 0.0f, 0.0f, 0.0f },    { -10.0f, 460.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },    { 10.0f, 460.0f, 0.0f }, { -40.0f, 315.0f, 0.0f },
    { 40.0f, 315.0f, 0.0f }, { 0.0f, 140.0f, 0.0f },  { 0.0f, 445.0f, 0.0f },
};

Vec3f D_80B38850[] = {
    { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f },
};

Vec3f D_80B388BC[] = {
    { 90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, 78.0f }, { 90.0f, 630.0f, 78.0f },
    { 90.0f, 350.0f, -78.0f }, { -90.0f, 170.0f, -78.0f }, { -90.0f, 170.0f, 78.0f }, { 90.0f, 350.0f, 78.0f },
};

Vec3f D_80B3891C[] = {
    { 90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, 78.0f }, { 90.0f, 630.0f, 78.0f },
    { 90.0f, 170.0f, -78.0f }, { -90.0f, 350.0f, -78.0f }, { -90.0f, 350.0f, 78.0f }, { 90.0f, 170.0f, 78.0f },
};

Vec3f D_80B3897C[] = {
    { 0.0f, 630.0f, -104.0f }, { -90.0f, 630.0f, -52.0f }, { -90.0f, 630.0f, 52.0f }, { 0.0f, 630.0f, 104.0f },
    { 0.0f, 0.0f, -104.0f },   { -90.0f, 0.0f, -52.0f },   { -90.0f, 0.0f, 52.0f },   { 0.0f, 0.0f, 104.0f },
};

Vec3f D_80B389DC[] = {
    { 0.0f, 630.0f, -104.0f }, { 90.0f, 630.0f, -52.0f }, { 90.0f, 630.0f, 52.0f }, { 0.0f, 630.0f, 104.0f },
    { 0.0f, 0.0f, -104.0f },   { 90.0f, 0.0f, -52.0f },   { 90.0f, 0.0f, 52.0f },   { 0.0f, 0.0f, 104.0f },
};

Vec3f D_80B38A3C[] = {
    { 90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, -78.0f }, { -90.0f, 630.0f, 78.0f }, { 90.0f, 630.0f, 78.0f },
    { 90.0f, 260.0f, -78.0f }, { -90.0f, 260.0f, -78.0f }, { -90.0f, 260.0f, 78.0f }, { 90.0f, 260.0f, 78.0f },
};

Vec3f D_80B38A9C[] = {
    { 90.0f, 260.0f, -78.0f }, { -90.0f, 260.0f, -78.0f }, { -90.0f, 260.0f, 78.0f }, { 90.0f, 260.0f, 78.0f },
    { 90.0f, 20.0f, -78.0f },  { -90.0f, 20.0f, -78.0f },  { -90.0f, 20.0f, 78.0f },  { 90.0f, 20.0f, 78.0f },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 12, 65, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, 0xF),
    /* Hookshot       */ DMG_ENTRY(0, 0x1),
    /* Goron punch    */ DMG_ENTRY(2, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(1, 0xF),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0xF),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 8, 0, 0, 0, MASS_HEAVY };

Vec3f D_80B38B54 = { 0.0f, 0.0f, 0.0f };

void EnMaruta_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMaruta* this = (EnMaruta*)thisx;
    s32 i;

    Actor_SetScale(&this->actor, 0.1f);

    this->actor.attentionRangeType = ATTENTION_RANGE_6;

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 50.0f;

    this->actor.colChkInfo.health = 1;
    this->actor.world.pos.y -= 4.0f;
    this->actor.home.pos.y -= 4.0f;

    this->unk_210 = ENMARUTA_GET_FF00(&this->actor);
    this->unk_218 = 0;
    this->unk_21A = 0;
    this->unk_1A0 = NULL;
    this->unk_21C = 0;

    this->unk_194.x = 0.0f;
    this->unk_194.y = 0.0f;
    this->unk_194.z = 1.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_1A4); i++) {
        this->unk_1A4[i] = D_80B38B54;
    }

    this->unk_214 = -1;
    this->unk_220 = 0;

    if (this->unk_210 == 0) {
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    }

    if (this->unk_210 == 0) {
        if (ENMARUTA_GET_FF(&this->actor) == 0xFF) {
            func_80B372B8(this);
        } else {
            func_80B37364(this);
        }
    } else {
        func_80B37590(this, play);
    }
}

void EnMaruta_Destroy(Actor* thisx, PlayState* play) {
    EnMaruta* this = (EnMaruta*)thisx;

    if (this->unk_210 == 0) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void func_80B372B8(EnMaruta* this) {
    this->actionFunc = func_80B372CC;
}

void func_80B372CC(EnMaruta* this, PlayState* play) {
    s16 temp_v1 = BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y);

    if (temp_v1 > 0x1555) {
        this->actor.shape.rot.y += 0x2AAA;
    } else if (temp_v1 < -0x1555) {
        this->actor.shape.rot.y -= 0x2AAA;
    }

    if (this->unk_220 == 1) {
        func_80B373F4(this);
    }

    if ((this->actor.parent != NULL) && (this->actor.parent->id == ACTOR_EN_KENDO_JS)) {
        EnKendoJs* kendoJs = (EnKendoJs*)this->actor.parent;

        kendoJs->unk_292 = this->actor.isLockedOn;
    }
}

void func_80B37364(EnMaruta* this) {
    this->actor.world.pos.y = this->actor.home.pos.y - 630.0f;
    this->actionFunc = func_80B3738C;
}

void func_80B3738C(EnMaruta* this, PlayState* play) {
    if (Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.4f, 100.0f, 10.0f) == 0.0f) {
        func_80B372B8(this);
    }
}

void func_80B373F4(EnMaruta* this) {
    this->collider.base.acFlags |= AC_HIT;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -2.0f;
    this->actionFunc = func_80B37428;
}

void func_80B37428(EnMaruta* this, PlayState* play) {
    if ((this->actor.floorHeight - 630.0f) < this->actor.world.pos.y) {
        this->actor.velocity.y += this->actor.gravity;
        this->actor.world.pos.y += this->actor.velocity.y;
    } else {
        if ((this->actor.parent != NULL) && (this->actor.parent->id == ACTOR_EN_KENDO_JS)) {
            EnKendoJs* kendoJs = (EnKendoJs*)this->actor.parent;

            kendoJs->unk_28C--;
        }
        Actor_Kill(&this->actor);
    }
}

void func_80B374B8(EnMaruta* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (this->actionFunc != func_80B37428) {
        this->unk_21E = 0;
        this->actor.gravity = -2.0f;
        this->actionFunc = func_80B374FC;
    }
}

void func_80B374FC(EnMaruta* this, PlayState* play) {
    if (this->unk_21E == 40) {
        Actor_Kill(&this->actor);
        return;
    }

    if (((this->actor.floorHeight - 630.0f) < this->actor.world.pos.y) && (this->unk_21E > 30)) {
        this->actor.velocity.y += this->actor.gravity;
        this->actor.world.pos.y += this->actor.velocity.y;
    }

    this->unk_21E++;
}

void func_80B37590(EnMaruta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp48;
    s16 sp46;
    Vec3f sp38;

    this->unk_21E = 0;
    this->unk_21C = 0;
    this->actor.gravity = -2.0f;

    switch (this->unk_210) {
        case 2:
            sp48 = D_80B38754;
            this->unk_1A0 = D_80B388BC;
            break;

        case 4:
            if (player->meleeWeaponAnimation == PLAYER_MWA_LEFT_SLASH_1H) {
                sp48 = D_80B3876C;
            } else {
                sp48 = D_80B38778;
            }
            this->unk_1A0 = D_80B3891C;
            break;

        case 5:
            if (player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_SLASH_1H) {
                sp48 = D_80B38784;
            } else {
                sp48 = D_80B38790;
            }
            this->unk_1A0 = D_80B3897C;
            break;

        case 6:
            if (player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_SLASH_1H) {
                sp48 = D_80B3879C;
            } else {
                sp48 = D_80B387A8;
            }
            this->unk_1A0 = D_80B389DC;
            break;

        case 8:
            if (player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_SLASH_1H) {
                sp48 = D_80B387B4;
            } else {
                sp48 = D_80B387C0;
            }
            this->unk_1A0 = D_80B38A3C;
            break;

        case 7:
            sp48 = D_80B387CC;
            this->unk_1A0 = D_80B38A9C;
            break;

        default:
            sp48 = D_80B38B54;
            break;
    }

    sp46 = this->actor.shape.rot.y;
    this->actor.velocity.x = (sp48.x * Math_CosS(sp46) + (Math_SinS(sp46) * sp48.z));
    this->actor.velocity.y = sp48.y;
    this->actor.velocity.z = (-sp48.x * Math_SinS(sp46) + (Math_CosS(sp46) * sp48.z));

    sp38 = D_80B38850[this->unk_210];

    func_80B3828C(&sp38, &this->unk_194, Rand_Next() & 0xFFF, Rand_Next() & 0xFFF, 0);
    this->unk_21A = Rand_Next() & 0x7FF;

    if (this->unk_210 == 7) {
        this->unk_21A |= 0x3F;
    } else {
        this->unk_21A |= 0xFF;
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = func_80B37950;
}

void func_80B37950(EnMaruta* this, PlayState* play) {
    this->unk_218 += this->unk_21A;
    this->actor.velocity.y += this->actor.gravity;
    func_80B37EC0(this, play);
    Actor_UpdatePos(&this->actor);
}

void func_80B37998(EnMaruta* this) {
    this->unk_21E = 0;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->unk_21A = 0;
    this->actionFunc = func_80B379C0;
}

void func_80B379C0(EnMaruta* this, PlayState* play) {
    if (this->unk_21E == 40) {
        func_80B37A14(this);
    } else {
        this->unk_21E++;
    }
    func_80B37C04(&this->unk_218);
}

void func_80B37A14(EnMaruta* this) {
    s16 temp = this->unk_218 & 0x7FFF;

    if (((this->unk_210 == 5) || (this->unk_210 == 6)) && !temp) {
        this->unk_21E = 100;
    } else {
        this->unk_21E = 0;
    }
    this->actionFunc = func_80B37A64;
}

void func_80B37A64(EnMaruta* this, PlayState* play) {
    if (this->unk_21E > 100) {
        this->actor.colChkInfo.health = 0;
    } else {
        this->unk_21E++;
    }
}

void func_80B37A8C(EnMaruta* this) {
    this->actionFunc = func_80B37AA0;
}

void func_80B37AA0(EnMaruta* this, PlayState* play) {
    if (this->actor.scale.y == 0.0f) {
        if (this->actor.scale.x == 0.0f) {
            Actor_Kill(&this->actor);
            return;
        }
        Math_SmoothStepToF(&this->actor.scale.x, 0.0f, 0.2f, 0.01f, 0.001f);
        Math_SmoothStepToF(&this->actor.scale.z, 0.0f, 0.2f, 0.01f, 0.001f);
    } else {
        Math_SmoothStepToF(&this->actor.scale.y, 0.0f, 0.2f, 0.003f, 0.001f);
    }
}

s32 func_80B37B78(EnMaruta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 temp_v1 = BINANG_SUB(this->actor.yawTowardsPlayer, 0x8000);

    temp_v1 = BINANG_SUB(temp_v1, player->actor.shape.rot.y);
    if ((ABS_ALT(temp_v1) < 0x1555) || ((player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) &&
                                        ((player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_SLASH_1H) ||
                                         (player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_COMBO_1H) ||
                                         (player->meleeWeaponAnimation == PLAYER_MWA_SPIN_ATTACK_1H) ||
                                         (player->meleeWeaponAnimation == PLAYER_MWA_BIG_SPIN_1H)))) {
        return true;
    }
    return false;
}

void func_80B37C04(s16* arg0) {
    s16 temp_a1 = *arg0 & 0xC000;
    s16 temp = *arg0 & 0x3FFF;

    if (temp > 0x2000) {
        temp_a1 += 0x4000;
    }
    Math_SmoothStepToS(arg0, temp_a1, 1, 0xAAA, 0xB6);
}

void func_80B37C60(EnMaruta* this) {
    if ((this->actionFunc != func_80B37AA0) && (this->actor.colChkInfo.health == 0)) {
        func_80B37A8C(this);
    }
}

void func_80B37CA0(EnMaruta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->actionFunc == func_80B372CC) || (this->actionFunc == func_80B3738C) ||
        (this->actionFunc == func_80B374FC) || (this->actionFunc == func_80B37AA0) ||
        ((this->actionFunc == func_80B37428) && !(this->actor.world.pos.y < (this->actor.floorHeight - 20.0f)))) {
        if ((this->collider.base.acFlags & AC_HIT) && (this->actionFunc == func_80B372CC)) {
            this->collider.base.acFlags &= ~AC_HIT;
            Actor_PlaySfx(&this->actor, NA_SE_IT_SWORD_STRIKE);

            if (D_80B386CC[player->meleeWeaponAnimation] != 0) {
                s32 temp = D_80B386CC[player->meleeWeaponAnimation] + 1;

                temp = (temp << 8) & 0xFF00;
                this->unk_210 = D_80B386CC[player->meleeWeaponAnimation];
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_MARUTA, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                                   temp);
                this->actor.world.rot.y = this->actor.shape.rot.y;
                if ((this->unk_210 == 5) ||
                    ((this->unk_210 == 7) && (player->meleeWeaponAnimation == PLAYER_MWA_STAB_1H))) {
                    func_80B37590(this, play);
                } else {
                    func_80B374B8(this);
                }

                if ((this->actor.parent != NULL) && (this->actor.parent->id == ACTOR_EN_KENDO_JS)) {
                    EnKendoJs* kendoJs = (EnKendoJs*)this->actor.parent;

                    kendoJs->unk_28E = 1;
                    kendoJs->unk_28C--;
                }
                return;
            }
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

        if (func_80B37B78(this, play) && (this->actionFunc == func_80B372CC)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void func_80B37EC0(EnMaruta* this, PlayState* play) {
    Vec3f sp34 = this->unk_204;
    s32 phi_a2 = -1;
    s32 i;
    f32 temp;

    for (i = 0; i < ARRAY_COUNT(this->unk_1A4); i++) {
        if (this->unk_1A4[i].y < sp34.y) {
            sp34 = this->unk_1A4[i];
            phi_a2 = i;
        }
        this->unk_214 = phi_a2;
    }

    if (sp34.y < this->actor.floorHeight) {
        this->unk_218 -= this->unk_21A;

        temp = this->actor.floorHeight - sp34.y;
        this->actor.velocity.y -= this->actor.gravity;
        this->actor.velocity.x *= 0.6f;
        this->actor.velocity.z *= 0.6f;
        this->actor.world.pos.y += temp;

        if (this->actor.velocity.y < -this->actor.gravity) {
            func_80B382E4(play, sp34);
            Actor_PlaySfx(&this->actor, NA_SE_EV_LOG_BOUND);
            this->actor.velocity.y *= -0.6f;
            func_80B38060(this, &sp34);
        }
    }
}

f32 func_80B38028(Vec3f* arg0, Vec3f* arg1, Vec3f* arg2) {
    f32 x0 = arg0->x;
    f32 z0 = arg0->z;
    f32 dx = arg2->x - arg1->x;
    f32 dz = arg2->z - arg1->z;

    return z0 * dx - x0 * dz;
}

void func_80B38060(EnMaruta* this, Vec3f* arg1) {
    Vec3f sp44;
    s32 pad;
    f32 temp_f0;
    f32 phi_f2;

    func_80B3828C(&this->unk_194, &sp44, 0, this->actor.shape.rot.y, 0);
    temp_f0 = func_80B38028(&sp44, &this->unk_204, arg1);

    if ((this->unk_210 == 5) || (this->unk_210 == 6)) {
        phi_f2 = 8.0f;
    } else if (this->unk_210 == 4) {
        phi_f2 = 5.0f;
    } else {
        phi_f2 = 2.0f;
    }

    if ((temp_f0 < phi_f2) && (-phi_f2 < temp_f0)) {
        phi_f2 = 3.0f;
    } else {
        phi_f2 = 1.2f;
    }

    if (temp_f0 > 0.0f) {
        if (this->unk_21A > 0) {
            this->unk_21A *= phi_f2;
        } else {
            this->unk_21A *= -0.8f;
        }
    } else {
        if (this->unk_21A > 0) {
            this->unk_21A *= -0.8f;
        } else {
            this->unk_21A *= phi_f2;
        }
    }

    if ((ABS_ALT(this->unk_21A) < 0x38E) &&
        (((this->unk_218 & 0x3FFF) < 0x71C) || ((this->unk_218 & 0x3FFF) >= 0x38E4))) {
        this->actor.gravity *= 0.8f;
        this->unk_21C++;
        if (this->unk_21C == 2) {
            func_80B37998(this);
        }
    } else if (ABS_ALT(this->unk_21A) > 0x38E) {
        if (this->unk_21A < 0) {
            this->unk_21A = -0x38E;
        } else {
            this->unk_21A = 0x38E;
        }
    }
}

void func_80B3828C(Vec3f* arg0, Vec3f* arg1, s16 arg2, s16 arg3, s32 arg4) {
    Matrix_Push();
    Matrix_RotateZYX(arg2, arg3, arg4, MTXMODE_NEW);
    Matrix_MultVec3f(arg0, arg1);
    Matrix_Pop();
}

void func_80B382E4(PlayState* play, Vec3f arg1) {
    Vec3f pos = arg1;
    Vec3f velocity;
    Vec3f accel;
    Color_RGBA8 primColor = { 170, 130, 90, 255 };
    Color_RGBA8 envColor = { 100, 60, 20, 255 };
    s32 i;

    accel.y = 0.0f;
    pos.y += 15.0f;

    for (i = 0; i < 10; i++) {
        velocity.x = Rand_Centered() * 10.0f;
        velocity.y = 2.0f * Rand_ZeroOne();
        velocity.z = Rand_Centered() * 10.0f;
        accel.x = -0.2f * velocity.x;
        accel.z = -0.2f * velocity.z;
        func_800B0EB0(play, &pos, &velocity, &accel, &primColor, &envColor, 60, 20, 10);
    }
}

void EnMaruta_Update(Actor* thisx, PlayState* play) {
    EnMaruta* this = (EnMaruta*)thisx;

    this->actionFunc(this, play);

    func_80B37CA0(this, play);
    func_80B37C60(this);
}

void EnMaruta_Draw(Actor* thisx, PlayState* play) {
    EnMaruta* this = (EnMaruta*)thisx;
    Vec3f sp50;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->unk_210 == 0) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_maruta_DL_002EC0);
    } else {
        sp50 = D_80B387E4[this->unk_210];

        Matrix_Push();
        Matrix_Translate(sp50.x, sp50.y, sp50.z, MTXMODE_APPLY);
        Matrix_RotateAxisS(this->unk_218, &this->unk_194, MTXMODE_APPLY);
        Matrix_Translate(-sp50.x, -sp50.y, -sp50.z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        for (i = 0; i < 8; i++) {
            if (D_80B386C0[this->unk_210] & (1 << i)) {
                gSPDisplayList(POLY_OPA_DISP++, D_80B386A0[i]);
            }
        }

        if (this->unk_1A0 != NULL) {
            for (i = 0; i < ARRAY_COUNT(this->unk_1A4); i++) {
                Matrix_MultVec3f(&this->unk_1A0[i], &this->unk_1A4[i]);
            }
            Matrix_MultVec3f(&sp50, &this->unk_204);
        }

        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
