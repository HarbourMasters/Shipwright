/*
 * File: z_en_dodongo.c
 * Overlay: ovl_En_Dodongo
 * Description: Dodongo
 */

#include "z_en_dodongo.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Bombf/z_en_bombf.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnDodongo_Init(Actor* thisx, PlayState* play);
void EnDodongo_Destroy(Actor* thisx, PlayState* play);
void EnDodongo_Update(Actor* thisx, PlayState* play2);
void EnDodongo_Draw(Actor* thisx, PlayState* play);

void func_808773C4(EnDodongo* this);
void func_80877424(EnDodongo* this, PlayState* play);
void func_80877494(EnDodongo* this);
void func_80877500(EnDodongo* this, PlayState* play);
void func_808777A8(EnDodongo* this);
void func_8087784C(EnDodongo* this, PlayState* play);
void func_80877D50(EnDodongo* this);
void func_80877D90(EnDodongo* this, PlayState* play);
void func_80877E60(EnDodongo* this, PlayState* play);
void func_80878354(EnDodongo* this);
void func_80878424(EnDodongo* this, PlayState* play);
void func_808785B0(EnDodongo* this, PlayState* play);
void func_8087864C(EnDodongo* this);
void func_808786C8(EnDodongo* this, PlayState* play);
void func_80878724(EnDodongo* this);
void func_808787B0(EnDodongo* this, PlayState* play);

ActorProfile En_Dodongo_Profile = {
    /**/ ACTOR_EN_DODONGO,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DODONGO,
    /**/ sizeof(EnDodongo),
    /**/ EnDodongo_Init,
    /**/ EnDodongo_Destroy,
    /**/ EnDodongo_Update,
    /**/ EnDodongo_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit1[10] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x800CB019, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 7, { { 500, 0, 0 }, 27 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x800CB019, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 21, { { -200, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x800CB019, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 28, { { -200, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x800CB019, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 22, { { 400, -400, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x800CB019, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 29, { { 400, -400, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 16, { { 1000, 0, 0 }, 28 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 14, { { -300, 300, 0 }, 17 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 14, { { 800, 100, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 13, { { 600, 0, 0 }, 12 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 13, { { 1500, 0, 0 }, 12 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit1 = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit1),
    sJntSphElementsInit1,
};

static ColliderJntSphInit sJntSphInit2 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    3,
    sJntSphElementsInit1,
};

static ColliderJntSphElementInit sJntSphElementsInit2[3] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x01, 0x10 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 2, { { 0, -10, 10 }, 25 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x01, 0x10 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 2, { { 0, -5, 50 }, 37 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x01, 0x10 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 2, { { 0, 0, 90 }, 50 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit3 = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit2),
    sJntSphElementsInit2,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x1),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x1),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(2, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(3, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x2),
    /* Ice arrow      */ DMG_ENTRY(1, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(1, 0xE),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x1),
    /* Zora barrier   */ DMG_ENTRY(0, 0x5),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

static CollisionCheckInfoInit sColChkInfoInit = { 3, 100, 100, 80 };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_DODONGO, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 1400, ICHAIN_STOP),
};

void EnDodongo_Init(Actor* thisx, PlayState* play) {
    static EffectBlureInit2 D_80879308 = {
        2,
        EFFECT_BLURE_ELEMENT_FLAG_8,
        0,
        { 255, 255, 255, 255 },
        { 255, 255, 255, 64 },
        { 255, 255, 255, 0 },
        { 255, 255, 255, 0 },
        8,
        0,
        EFF_BLURE_DRAW_MODE_SIMPLE,
        0,
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    };
    EnDodongo* this = (EnDodongo*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->unk_32C.r = 255;
    this->unk_32C.g = 255;
    this->unk_32C.a = 200;
    this->unk_330.r = 255;
    this->unk_330.g = 10;
    this->unk_330.a = 200;
    Math_Vec3f_Copy(&this->unk_314, &gOneVec3f);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 48.0f);
    SkelAnime_Init(play, &this->skelAnime, &object_dodongo_Skel_008318, &object_dodongo_Anim_004C20, this->jointTable,
                   this->morphTable, OBJECT_DODONGO_LIMB_MAX);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Collider_InitAndSetJntSph(play, &this->collider2, &this->actor, &sJntSphInit2, this->collider2Elements);
    Collider_InitAndSetJntSph(play, &this->collider1, &this->actor, &sJntSphInit1, this->collider1Elements);
    Collider_InitAndSetJntSph(play, &this->collider3, &this->actor, &sJntSphInit3, this->collider3Elements);

    for (i = 0; i < ARRAY_COUNT(this->collider2Elements); i++) {
        this->collider2.elements[i].base.elemMaterial = ELEM_MATERIAL_UNK2;
        this->collider2.elements[i].base.acDmgInfo.dmgFlags = 0x77C34FE6;
    }

    Effect_Add(play, &this->unk_338, EFFECT_BLURE2, 0, 0, &D_80879308);
    if (this->actor.params == 0) {
        Actor_SetScale(&this->actor, 3.0f / 160.0f);
        this->unk_334 = 1.0f;
    } else {
        Actor_SetScale(&this->actor, 3.0f / 80.0f);
        this->actor.colChkInfo.health = 12;
        this->unk_334 = 2.0f;

        for (i = 0; i < ARRAY_COUNT(this->collider1Elements); i++) {
            this->collider1.elements[i].dim.modelSphere.radius *= 2;
            this->collider1.elements[i].base.atDmgInfo.damage *= 2;
        }

        for (i = 0; i < ARRAY_COUNT(this->collider2Elements); i++) {
            this->collider2.elements[i].dim.modelSphere.radius *= 2;
        }

        for (i = 0; i < ARRAY_COUNT(this->collider3Elements); i++) {
            this->collider3.elements[i].dim.modelSphere.radius *= 2;
        }
    }

    func_808773C4(this);
}

void EnDodongo_Destroy(Actor* thisx, PlayState* play) {
    EnDodongo* this = (EnDodongo*)thisx;

    Effect_Destroy(play, this->unk_338);
    Collider_DestroyJntSph(play, &this->collider2);
    Collider_DestroyJntSph(play, &this->collider3);
    Collider_DestroyJntSph(play, &this->collider1);
}

void func_80876930(EnDodongo* this, PlayState* play, Vec3f* arg2) {
    static Color_RGBA8 D_8087932C = { 250, 250, 250, 255 };
    static Color_RGBA8 D_80879330 = { 180, 180, 180, 255 };
    static Color_RGBA8 D_80879334 = { 170, 130, 90, 255 };
    static Color_RGBA8 D_80879338 = { 100, 60, 20, 255 };
    static Vec3f D_8087933C = { 0.0f, 0.3f, 0.0f };
    s32 i;
    Vec3f sp88;
    s32 pad2;
    Color_RGBA8* sp80;
    Color_RGBA8* sp7C;
    s32 pad;
    s16 temp1;
    s16 temp2;
    f32 temp3;

    if (SurfaceType_GetMaterial(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) == SURFACE_MATERIAL_SNOW) {
        sp80 = &D_8087932C;
        sp7C = &D_80879330;
    } else {
        sp80 = &D_80879334;
        sp7C = &D_80879338;
    }

    temp1 = this->unk_334 * 300.0f;
    temp2 = this->unk_334 * 15.0f;
    temp3 = this->unk_334 * 30.0f;

    for (i = 0; i < 3; i++) {
        sp88.x = Rand_CenteredFloat(temp3) + arg2->x;
        sp88.y = Rand_ZeroFloat(5.0f) + this->actor.floorHeight;
        sp88.z = Rand_CenteredFloat(temp3) + arg2->z;
        D_8087933C.x = Rand_CenteredFloat(2.0f);
        D_8087933C.z = Rand_CenteredFloat(2.0f);
        func_800B0DE0(play, &sp88, &gZeroVec3f, &D_8087933C, sp80, sp7C, temp1, temp2);
    }
}

void func_80876B08(EnDodongo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS_ALT(yDiff) < 0x4000) {
        if (!(player->stateFlags1 & 0x00800000) && (Player_GetMask(play) != PLAYER_MASK_STONE)) {
            func_808777A8(this);
        } else {
            func_80877494(this);
        }
    } else if (Player_GetMask(play) != PLAYER_MASK_STONE) {
        func_80878354(this);
    } else {
        func_80877494(this);
    }
}

void func_80876BD0(EnDodongo* this, PlayState* play, s32 arg2) {
    if (this->actor.colChkInfo.damageEffect == 2) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffAlpha = 4.0f;
    } else if (this->actor.colChkInfo.damageEffect == 4) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffAlpha = 4.0f;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider1.elements[arg2].base.acDmgInfo.hitPos.x,
                    this->collider1.elements[arg2].base.acDmgInfo.hitPos.y,
                    this->collider1.elements[arg2].base.acDmgInfo.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
    }
}

void func_80876CAC(EnDodongo* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->collider1.base.colMaterial = COL_MATERIAL_HIT3;
    this->drawDmgEffScale = 0.75f;
    this->drawDmgEffFrozenSteamScale = 1.125f;
    this->drawDmgEffAlpha = 1.0f;
    this->timer = 80;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void func_80876D28(EnDodongo* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->timer = 0;
        this->actor.colorFilterTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->collider1.base.colMaterial = COL_MATERIAL_HIT0;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, DODONGO_BODYPART_MAX, 2, this->unk_334 * 0.3f,
                              this->unk_334 * 0.2f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    }
}

void func_80876DC4(EnDodongo* this, PlayState* play) {
    s32 pad;
    Vec3f sp80;
    Vec3f sp74;
    Vec3f sp68;
    s16 sp66;
    s16 sp64;
    s16 sp62;
    f32 temp_f22;
    f32 temp_f20;

    Math_Vec3f_Copy(&sp68, &this->bodyPartsPos[DODONGO_BODYPART_0]);
    sp66 = ((s32)Rand_Next() >> 0x12) + this->actor.shape.rot.y;
    temp_f20 = Math_CosS(sp66);
    temp_f22 = Math_SinS(sp66);

    if (this->unk_32C.r > 30) {
        this->unk_32C.r -= 16;
        this->unk_32C.g += -16;
    }

    if (this->unk_32C.b < 30) {
        this->unk_32C.b += 5;
        this->unk_32C.a += 8;
        this->unk_330.a += 8;
    }

    if (this->unk_330.r != 0) {
        this->unk_330.r -= 15;
    }

    if (this->unk_330.g != 0) {
        this->unk_330.g--;
    }

    sp80.x = temp_f22 * 3.5f * this->unk_334;
    sp80.y = this->unk_330.r * 0.02f * this->unk_334;
    sp80.z = temp_f20 * 3.5f * this->unk_334;
    sp74.x = (Rand_ZeroFloat(0.1f) + 0.15f) * -temp_f22 * this->unk_334;
    sp74.y = this->unk_334 * 0.3f;
    sp74.z = (Rand_ZeroFloat(0.1f) + 0.15f) * -temp_f20 * this->unk_334;
    sp64 = this->unk_334 * 100.0f;
    sp62 = this->unk_334 * 25.0f;
    func_800B0EB0(play, &sp68, &sp80, &sp74, &this->unk_32C, &this->unk_330, sp64, sp62, 0x14);

    sp66 = (Rand_Next() >> 0x13) + this->actor.shape.rot.y;
    temp_f20 = Math_CosS(sp66);
    temp_f22 = Math_SinS(sp66);

    sp68.x -= (temp_f20 * 6.0f) * this->unk_334;
    sp68.z += (temp_f22 * 6.0f) * this->unk_334;

    sp80.x = -temp_f20 * 3.5f * this->unk_334;
    sp80.z = temp_f22 * 3.5f * this->unk_334;

    sp74.x = (Rand_ZeroFloat(0.1f) + 0.15f) * temp_f20 * this->unk_334;
    sp74.z = (Rand_ZeroFloat(0.1f) + 0.15f) * -temp_f22 * this->unk_334;
    func_800B0EB0(play, &sp68, &sp80, &sp74, &this->unk_32C, &this->unk_330, sp64, sp62, 0x14);

    sp68.x = this->bodyPartsPos[DODONGO_BODYPART_0].x + (temp_f20 * 6.0f * this->unk_334);
    sp68.z = this->bodyPartsPos[DODONGO_BODYPART_0].z - (temp_f22 * 6.0f * this->unk_334);
    sp80.x *= -1.0f;
    sp80.z *= -1.0f;
    sp74.x = (Rand_ZeroFloat(0.1f) + 0.15f) * -temp_f20 * this->unk_334;
    sp74.z = (Rand_ZeroFloat(0.1f) + 0.15f) * temp_f22 * this->unk_334;
    func_800B0EB0(play, &sp68, &sp80, &sp74, &this->unk_32C, &this->unk_330, sp64, sp62, 0x14);
}

s32 func_8087721C(EnDodongo* this) {
    if ((this->actionFunc == func_8087784C) && (this->skelAnime.curFrame >= 29.0f) &&
        (this->skelAnime.curFrame <= 43.0f)) {
        return true;
    }
    return false;
}

s32 func_80877278(EnDodongo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;
    Vec3f sp44;

    // Bugfix from OoT
    while (explosive != NULL) {
        if (!explosive->params && (explosive->parent == NULL) && (explosive->update != NULL) &&
            (explosive != player->interactRangeActor) &&
            (((explosive->id == ACTOR_EN_BOM) && (((EnBom*)explosive)->timer > 0)) ||
             ((explosive->id == ACTOR_EN_BOMBF) && (((EnBombf*)explosive)->timer > 0)))) {
            Math_Vec3f_Diff(&explosive->world.pos, &this->unk_308, &sp44);
            if ((fabsf(sp44.x) < 20.0f) && (fabsf(sp44.y) < 10.0f) && (fabsf(sp44.z) < 20.0f)) {
                this->actor.child = explosive;
                explosive->freezeTimer = 30000; // 25 minutes
                return true;
            }
        }
        explosive = explosive->next;
    }
    return false;
}

void func_808773C4(EnDodongo* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_dodongo_Anim_004C20, -4.0f);
    this->actor.speed = 0.0f;
    this->timer = Rand_S16Offset(30, 50);
    this->actionFunc = func_80877424;
}

void func_80877424(EnDodongo* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
            func_80877494(this);
        }
    }
}

void func_80877494(EnDodongo* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_dodongo_Anim_008B1C, -4.0f);
    this->actor.speed = this->unk_334 * 1.5f;
    this->timer = Rand_S16Offset(50, 70);
    this->actionFunc = func_80877500;
}

void func_80877500(EnDodongo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp30;
    s16 temp_v1;

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 19.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_DODO_J_WALK);
        sp30.x = this->collider1Elements[2].dim.worldSphere.center.x;
        sp30.y = this->collider1Elements[2].dim.worldSphere.center.y;
        sp30.z = this->collider1Elements[2].dim.worldSphere.center.z;
        func_80876930(this, play, &sp30);
    } else if (Animation_OnFrame(&this->skelAnime, 39.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_DODO_J_WALK);
        sp30.x = this->collider1Elements[1].dim.worldSphere.center.x;
        sp30.y = this->collider1Elements[1].dim.worldSphere.center.y;
        sp30.z = this->collider1Elements[1].dim.worldSphere.center.z;
        func_80876930(this, play, &sp30);
    }

    if ((Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < 400.0f) &&
        (Player_GetMask(play) != PLAYER_MASK_STONE)) {
        temp_v1 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x1F4);
        if (this->actor.xzDistToPlayer < (100.0f * this->unk_334)) {
            if ((ABS_ALT(temp_v1) < 0x1388) && (this->actor.playerHeightRel < 60.0f) &&
                !(player->stateFlags1 & PLAYER_STATE1_800000)) {
                func_808777A8(this);
            }
        }
    } else {
        if ((Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 150.0f) ||
            (Player_GetMask(play) == PLAYER_MASK_STONE)) {
            Math_ScaledStepToS(&this->actor.world.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                               0x1F4);
        }

        this->timer--;
        if (this->timer == 0) {
            if (Rand_ZeroOne() > 0.7f) {
                this->timer = Rand_S16Offset(50, 70);
            } else {
                func_808773C4(this);
            }
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void func_808777A8(EnDodongo* this) {
    s32 i;
    Sphere16* sph;

    Animation_MorphToPlayOnce(&this->skelAnime, &object_dodongo_Anim_0028F0, -4.0f);
    this->actor.speed = 0.0f;

    for (i = 0; i < ARRAY_COUNT(this->collider3Elements); i++) {
        sph = &this->collider3.elements[i].dim.worldSphere;

        sph->center.x = this->bodyPartsPos[DODONGO_BODYPART_0].x;
        sph->center.y = this->bodyPartsPos[DODONGO_BODYPART_0].y;
        sph->center.z = this->bodyPartsPos[DODONGO_BODYPART_0].z;
        sph->radius = 0;
    }

    this->actionFunc = func_8087784C;
}

void func_8087784C(EnDodongo* this, PlayState* play) {
    static Vec3f D_80879348 = { 0.0f, 0.9f, 0.0f };
    static Vec3f D_80879354 = { 0.0f, 0.0f, 0.0f };
    s16 frame;
    ColliderJntSphElement* jntSphElem;
    s32 i;
    s32 end;
    f32 temp_f2;
    f32 temp_f12;

    if (Animation_OnFrame(&this->skelAnime, 24.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_DODO_J_CRY);
    }

    if (func_8087721C(this)) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_DODO_J_FIRE - SFX_FLAG);
        frame = this->skelAnime.curFrame - 29.0f;
        end = frame >> 1;
        if (end > 3) {
            end = 3;
        }

        jntSphElem = &this->collider3.elements[0];
        temp_f2 = Math_SinS(this->actor.shape.rot.y) * this->unk_334;
        temp_f12 = Math_CosS(this->actor.shape.rot.y) * this->unk_334;

        for (i = 0; i < end; i++, jntSphElem++) {
            jntSphElem->dim.worldSphere.center.x =
                this->bodyPartsPos[DODONGO_BODYPART_0].x + (jntSphElem->dim.modelSphere.center.z * temp_f2);
            jntSphElem->dim.worldSphere.center.y =
                this->bodyPartsPos[DODONGO_BODYPART_0].y + (jntSphElem->dim.modelSphere.center.y * this->unk_334);
            jntSphElem->dim.worldSphere.center.z =
                this->bodyPartsPos[DODONGO_BODYPART_0].z + (jntSphElem->dim.modelSphere.center.z * temp_f12);
            jntSphElem->dim.worldSphere.radius = jntSphElem->dim.modelSphere.radius;
        }

        D_80879354.y = this->unk_334 * -4.5f;
        D_80879348.x = 2.5f * temp_f2;
        D_80879348.y = this->unk_334 * 1.4f;
        D_80879348.z = 2.5f * temp_f12;
        EffectSsDFire_Spawn(play, &this->bodyPartsPos[DODONGO_BODYPART_0], &D_80879354, &D_80879348,
                            this->unk_334 * 100.0f, this->unk_334 * 35.0f, 0xFF - (frame * 10), 5, 0, 8);
    } else if ((this->skelAnime.curFrame >= 2.0f) && (this->skelAnime.curFrame <= 20.0f)) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_DODO_J_BREATH - SFX_FLAG);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        func_80877D50(this);
    }
}

void func_80877D50(EnDodongo* this) {
    Animation_PlayOnce(&this->skelAnime, &object_dodongo_Anim_003088);
    this->actionFunc = func_80877D90;
}

void func_80877D90(EnDodongo* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        func_808773C4(this);
        this->timer = Rand_S16Offset(10, 20);
    }
}

void func_80877DE0(EnDodongo* this) {
    Animation_Change(&this->skelAnime, &object_dodongo_Anim_0028F0, -1.0f, 35.0f, 0.0f, ANIMMODE_ONCE, -4.0f);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->timer = 25;
    this->actionFunc = func_80877E60;
    this->actor.speed = 0.0f;
}

void func_80877E60(EnDodongo* this, PlayState* play) {
    static Vec3f D_80879360 = { 0.0f, 0.6f, 0.0f };
    static Color_RGBA8 D_8087936C = { 255, 255, 255, 255 };
    Vec3f sp84;
    Vec3f sp78;
    s16 i;
    Actor* explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;
    Vec3f sp64;
    s32 pad;
    s16 sp5E;
    s16 sp5C;

    while (explosive != NULL) {
        if (explosive == this->actor.child) {
            break;
        }
        explosive = explosive->next;
    }

    if (explosive == NULL) {
        this->actor.child = NULL;
    }

    if (this->actor.child != NULL) {
        Math_Vec3f_Copy(&this->actor.child->world.pos, &this->unk_308);
        this->actor.child->freezeTimer = 30000; // 25 minutes
    }

    if (Animation_OnFrame(&this->skelAnime, 28.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_DODO_J_EAT);
        if (this->actor.child != NULL) {
            Actor_Kill(this->actor.child);
            this->actor.child = NULL;
        }
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    } else if (this->skelAnime.playSpeed > -0.5f) {
        this->timer--;
        if (this->timer == 10) {
            for (i = 10; i >= 0; i--) {
                sp84.x = Rand_CenteredFloat(10.0f);
                sp84.y = Rand_CenteredFloat(10.0f);
                sp84.z = Rand_CenteredFloat(10.0f);
                sp78.x = sp84.x * -0.1f;
                sp78.y = sp84.y * -0.1f;
                sp78.z = sp84.z * -0.1f;
                sp64.x = this->collider1Elements[0].dim.worldSphere.center.x + sp84.x;
                sp64.y = this->collider1Elements[0].dim.worldSphere.center.y + sp84.y;
                sp64.z = this->collider1Elements[0].dim.worldSphere.center.z + sp84.z;
                func_800B0EB0(play, &sp64, &sp84, &sp78, &this->unk_32C, &this->unk_330, this->unk_334 * 400.0f,
                              this->unk_334 * 10.0f, 10);
            }

            Actor_PlaySfx(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
            if (this->actor.colChkInfo.health <= 4) {
                this->actor.colChkInfo.health = 0;
                Enemy_StartFinishingBlow(play, &this->actor);
            } else {
                this->actor.colChkInfo.health -= 4;
            }
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 120, COLORFILTER_BUFFLAG_OPA, 8);
        }
    } else if (Animation_OnFrame(&this->skelAnime, 24.0f)) {
        this->timer--;
        this->skelAnime.playSpeed = 0.0f;
    }

    if (this->skelAnime.curFrame <= 27.0f) {
        if ((this->skelAnime.curFrame <= 25.0f) && (this->timer < 11)) {
            func_80876DC4(this, play);
        } else {
            sp5E = this->unk_334 * 50.0f;
            sp5C = this->unk_334 * 5.0f;
            Math_Vec3f_Copy(&sp64, &this->bodyPartsPos[DODONGO_BODYPART_0]);
            func_800B0DE0(play, &sp64, &gZeroVec3f, &D_80879360, &D_8087936C, &D_8087936C, sp5E, sp5C);
            sp64.x -= Math_CosS(this->actor.shape.rot.y) * 6.0f * this->unk_334;
            sp64.z += Math_SinS(this->actor.shape.rot.y) * 6.0f * this->unk_334;
            func_800B0DE0(play, &sp64, &gZeroVec3f, &D_80879360, &D_8087936C, &D_8087936C, sp5E, sp5C);
            sp64.x = (2.0f * this->bodyPartsPos[DODONGO_BODYPART_0].x) - sp64.x;
            sp64.z = (2.0f * this->bodyPartsPos[DODONGO_BODYPART_0].z) - sp64.z;
            func_800B0DE0(play, &sp64, &gZeroVec3f, &D_80879360, &D_8087936C, &D_8087936C, sp5E, sp5C);
        }
    }

    this->unk_314.z = Math_SinS((this->actor.colorFilterTimer * 0x1000) >> 1) + 1.0f;
    this->unk_314.x = Math_SinS(this->actor.colorFilterTimer * 0x1000) + 1.0f;
    this->unk_314.y = this->unk_314.z;
    SkelAnime_Update(&this->skelAnime);
    if (this->timer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            func_80878724(this);
        } else {
            func_8087864C(this);
        }
        this->unk_304 = 1;
    }
}

void func_80878354(EnDodongo* this) {
    s32 pad;
    AnimationHeader* anim;
    s16 yDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    this->unk_306 = (0xFFFF - ABS_ALT(yDiff)) / 15;

    if (yDiff >= 0) {
        anim = &object_dodongo_Anim_0042C4;
        this->unk_306 = -this->unk_306;
    } else {
        anim = &object_dodongo_Anim_003B14;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_DODO_J_TAIL);
    Animation_PlayOnceSetSpeed(&this->skelAnime, anim, 2.0f);
    this->timer = 0;
    this->collider1.base.atFlags |= AT_ON;
    this->unk_304 = -1;
    this->actionFunc = func_80878424;
}

void func_80878424(EnDodongo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp20;

    this->timer++;
    if (SkelAnime_Update(&this->skelAnime)) {
        if (!(player->stateFlags1 & PLAYER_STATE1_800000) && (Player_GetMask(play) != PLAYER_MASK_STONE)) {
            this->collider1.base.atFlags &= ~AT_ON;
            func_808777A8(this);
        } else {
            func_80877494(this);
        }
        return;
    }

    this->actor.world.rot.y += this->unk_306;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    sp20.x = this->collider1Elements[1].dim.worldSphere.center.x;
    sp20.y = this->collider1Elements[1].dim.worldSphere.center.y;
    sp20.z = this->collider1Elements[1].dim.worldSphere.center.z;
    func_80876930(this, play, &sp20);
    sp20.x = this->collider1Elements[2].dim.worldSphere.center.x;
    sp20.y = this->collider1Elements[2].dim.worldSphere.center.y;
    sp20.z = this->collider1Elements[2].dim.worldSphere.center.z;
    func_80876930(this, play, &sp20);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
    this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
}

void func_80878594(EnDodongo* this) {
    this->actionFunc = func_808785B0;
    this->actor.speed = 0.0f;
}

void func_808785B0(EnDodongo* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        func_80876D28(this, play);
        if (this->actor.colChkInfo.health == 0) {
            func_80878724(this);
        } else if (this->actor.xzDistToPlayer > (100.0f * this->unk_334)) {
            func_80877494(this);
        } else {
            func_80876B08(this, play);
        }
    }
}

void func_8087864C(EnDodongo* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_dodongo_Anim_001A44, -4.0f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_DODO_J_DAMAGE);
    this->timer = 0;
    this->unk_304 = 0;
    this->actor.speed = 0.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    this->actionFunc = func_808786C8;
}

void func_808786C8(EnDodongo* this, PlayState* play) {
    if (this->unk_304 == 1) {
        func_80876DC4(this, play);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        func_80876B08(this, play);
    }
}

void func_80878724(EnDodongo* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_dodongo_Anim_0013C4, -8.0f);
    this->timer = 0;
    this->unk_304 = 0;
    Actor_PlaySfx(&this->actor, NA_SE_EN_DODO_J_DEAD);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    this->actionFunc = func_808787B0;
}

void func_808787B0(EnDodongo* this, PlayState* play) {
    s32 pad;

    if (this->skelAnime.curFrame < 35.0f) {
        if (this->unk_304 != 0) {
            func_80876DC4(this, play);
        }
    } else if (this->actor.colorFilterTimer == 0) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 120, COLORFILTER_BUFFLAG_OPA, 4);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->timer == 0) {
            EnBom* bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                              this->actor.world.pos.y, this->actor.world.pos.z,
                                              BOMB_EXPLOSIVE_TYPE_BOMB, 0, 0, BOMB_TYPE_BODY);
            if (bomb != NULL) {
                bomb->timer = 0;
            }
            this->timer = 8;
        }
    } else if (Animation_OnFrame(&this->skelAnime, 52.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_GERUDOFT_DOWN);
    }

    if (this->timer != 0) {
        this->timer--;
        if (this->timer == 0) {
            if (this->actor.params == 0) {
                Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x90);
            } else {
                Item_DropCollectible(play, &this->actor.world.pos, ITEM00_RUPEE_PURPLE);
            }
            Actor_Kill(&this->actor);
        }
    }
}

void EnDodongo_UpdateDamage(EnDodongo* this, PlayState* play) {
    Vec3f sp3C;
    s32 i;

    if (this->collider2.base.acFlags & AC_HIT) {
        this->collider2.base.acFlags &= ~AC_HIT;
        this->collider1.base.acFlags &= ~AC_HIT;

        for (i = 0; i < ARRAY_COUNT(this->collider2Elements); i++) {
            if (this->collider2.elements[i].base.acElemFlags & ACELEM_HIT) {
                break;
            }
        }

        if ((i != ARRAY_COUNT(this->collider2Elements)) &&
            ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
             !(this->collider2.elements[i].base.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3))) {
            func_80876D28(this, play);
            Math_Vec3s_ToVec3f(&sp3C, &this->collider2.elements[i].base.acDmgInfo.hitPos);
            if (this->actor.colChkInfo.damageEffect == 0xF) {
                CollisionCheck_BlueBlood(play, NULL, &sp3C);
                EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_WHITE, &sp3C);
            } else if (this->actor.colChkInfo.damageEffect != 0xE) {
                EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_METAL, &sp3C);
                CollisionCheck_SpawnShieldParticlesMetalSound(play, &sp3C, &this->actor.projectedPos);
            }
        }
    } else if (this->collider1.base.acFlags & AC_HIT) {
        this->collider2.base.acFlags &= ~AC_HIT;
        this->collider1.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&this->actor, &this->collider1);

        for (i = 0; i < ARRAY_COUNT(this->collider1Elements); i++) {
            if (this->collider1.elements[i].base.acElemFlags & ACELEM_HIT) {
                break;
            }
        }

        if ((i != ARRAY_COUNT(this->collider1Elements)) &&
            ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
             !(this->collider1.elements[i].base.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3))) {
            func_80876D28(this, play);
            if (this->actor.colChkInfo.damageEffect != 0xF) {
                if (!Actor_ApplyDamage(&this->actor)) {
                    Enemy_StartFinishingBlow(play, &this->actor);
                    if (this->actor.colChkInfo.damageEffect == 3) {
                        func_80876CAC(this);
                        this->timer = 3;
                        this->collider1.base.acFlags &= ~AC_ON;
                        this->collider2.base.acFlags &= ~AC_ON;
                        func_80878594(this);
                    } else {
                        func_80876BD0(this, play, i);
                        func_80878724(this);
                    }
                } else if (this->actor.colChkInfo.damageEffect == 1) {
                    this->timer = 40;
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    func_80878594(this);
                } else if (this->actor.colChkInfo.damageEffect == 5) {
                    this->timer = 40;
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                    this->drawDmgEffScale = 0.75f;
                    this->drawDmgEffAlpha = 2.0f;
                    func_80878594(this);
                } else if (this->actor.colChkInfo.damageEffect == 3) {
                    func_80876CAC(this);
                    func_80878594(this);
                } else {
                    func_80876BD0(this, play, i);
                    func_8087864C(this);
                }
            }
        }
    }
}

void EnDodongo_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnDodongo* this = (EnDodongo*)thisx;

    EnDodongo_UpdateDamage(this, play);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 60.0f, 70.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_GERUDOFT_DOWN);
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
    if (this->actionFunc != func_808787B0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
        if ((this->actionFunc != func_808786C8) && (this->actionFunc != func_80877E60)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
            if ((this->actionFunc != func_8087784C) && func_80877278(this, play)) {
                func_80877DE0(this);
            }
        }
    }

    if (func_8087721C(this)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider3.base);
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.375f;
            this->drawDmgEffScale = (this->drawDmgEffScale > 0.75f) ? 0.75f : this->drawDmgEffScale;
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.75f, 0.01875f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

s32 EnDodongo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDodongo* this = (EnDodongo*)thisx;

    if (limbIndex == OBJECT_DODONGO_LIMB_01) {
        pos->z += 1000.0f;
    } else if ((limbIndex == OBJECT_DODONGO_LIMB_0F) || (limbIndex == OBJECT_DODONGO_LIMB_10)) {
        Matrix_Scale(this->unk_314.x, this->unk_314.y, this->unk_314.z, MTXMODE_APPLY);
    }

    return false;
}

static Vec3f D_80879370 = { 1800.0f, 1200.0f, 0.0f };
static Vec3f D_8087937C = { 1500.0f, 300.0f, 0.0f };

static s8 sLimbToBodyParts[OBJECT_DODONGO_LIMB_MAX] = {
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_NONE
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_01
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_02
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_03
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_04
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_05
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_06
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_07
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_08
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_09
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_0A
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_0B
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_0C
    DODONGO_BODYPART_2, // OBJECT_DODONGO_LIMB_0D
    DODONGO_BODYPART_3, // OBJECT_DODONGO_LIMB_0E
    DODONGO_BODYPART_4, // OBJECT_DODONGO_LIMB_0F
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_10
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_11
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_12
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_13
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_14
    DODONGO_BODYPART_5, // OBJECT_DODONGO_LIMB_15
    DODONGO_BODYPART_6, // OBJECT_DODONGO_LIMB_16
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_17
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_18
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_19
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_1A
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_1B
    DODONGO_BODYPART_7, // OBJECT_DODONGO_LIMB_1C
    DODONGO_BODYPART_8, // OBJECT_DODONGO_LIMB_1D
    BODYPART_NONE,      // OBJECT_DODONGO_LIMB_1E
};

void EnDodongo_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDodongo* this = (EnDodongo*)thisx;

    Collider_UpdateSpheres(limbIndex, &this->collider1);
    Collider_UpdateSpheres(limbIndex, &this->collider2);
    if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
    }

    if (limbIndex == OBJECT_DODONGO_LIMB_07) {
        Matrix_MultVec3f(&D_80879370, &this->unk_308);
        Matrix_MultVec3f(&D_8087937C, &this->bodyPartsPos[DODONGO_BODYPART_0]);
        Matrix_MultZero(&this->actor.focus.pos);
        Matrix_MultVecY(-200.0f, &this->bodyPartsPos[DODONGO_BODYPART_1]);
    } else if (limbIndex == OBJECT_DODONGO_LIMB_0D) {
        Matrix_MultVecX(1600.0f, &this->unk_320);
    }

    if ((limbIndex == OBJECT_DODONGO_LIMB_1E) && (this->actionFunc == func_80878424) &&
        (this->timer != this->unk_304)) {
        EffectBlure_AddVertex(Effect_GetByIndex(this->unk_338), &this->unk_320,
                              &this->bodyPartsPos[DODONGO_BODYPART_4]);
        this->unk_304 = this->timer;
    }
}

void EnDodongo_Draw(Actor* thisx, PlayState* play) {
    EnDodongo* this = (EnDodongo*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnDodongo_OverrideLimbDraw,
                      EnDodongo_PostLimbDraw, &this->actor);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, DODONGO_BODYPART_MAX,
                            this->drawDmgEffScale * this->unk_334, this->drawDmgEffFrozenSteamScale * this->unk_334,
                            this->drawDmgEffAlpha, this->drawDmgEffType);
}
