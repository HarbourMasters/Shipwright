/*
 * File: z_en_wf.c
 * Overlay: ovl_En_Wf
 * Description: Wolfos and White Wolfos
 */

#include "z_en_wf.h"
#include "overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_Obj_Ice_Poly/z_obj_ice_poly.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnWf_Init(Actor* thisx, PlayState* play);
void EnWf_Destroy(Actor* thisx, PlayState* play);
void EnWf_Update(Actor* thisx, PlayState* play);
void EnWf_Draw(Actor* thisx, PlayState* play);

void func_80990F0C(EnWf* this);
void func_80990F50(EnWf* this, PlayState* play);
void func_80990FC8(EnWf* this);
void func_80991040(EnWf* this, PlayState* play);
void func_80991174(EnWf* this, PlayState* play);
void func_80991200(EnWf* this);
void func_80991280(EnWf* this, PlayState* play);
void func_80991438(EnWf* this);
void func_8099149C(EnWf* this, PlayState* play);
void func_80991738(EnWf* this);
void func_8099177C(EnWf* this, PlayState* play);
void func_80991948(EnWf* this);
void func_809919F4(EnWf* this, PlayState* play);
void func_80991C04(EnWf* this);
void func_80991C80(EnWf* this, PlayState* play);
void func_80992068(EnWf* this, PlayState* play);
void func_8099223C(EnWf* this);
void func_809922B4(EnWf* this, PlayState* play);
void func_809923E4(EnWf* this, PlayState* play);
void func_809924EC(EnWf* this, PlayState* play);
void func_809926D0(EnWf* this);
void func_80992784(EnWf* this, PlayState* play);
void func_8099282C(EnWf* this);
void func_809928CC(EnWf* this, PlayState* play);
void func_80992A74(EnWf* this, PlayState* play);
void func_80992B8C(EnWf* this, PlayState* play);
void func_80992D6C(EnWf* this);
void func_80992E0C(EnWf* this, PlayState* play);
void func_80992FD4(EnWf* this);
void func_80993018(EnWf* this, PlayState* play);
void func_80993148(EnWf* this);
void func_80993194(EnWf* this, PlayState* play);
void func_80993350(EnWf* this);
void func_809933A0(EnWf* this, PlayState* play);
void func_80993524(EnWf* this);
void func_8099357C(EnWf* this, PlayState* play);
s32 func_8099408C(PlayState* play, EnWf* this);

ActorProfile En_Wf_Profile = {
    /**/ ACTOR_EN_WF,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_WF,
    /**/ sizeof(EnWf),
    /**/ EnWf_Init,
    /**/ EnWf_Destroy,
    /**/ EnWf_Update,
    /**/ EnWf_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[4] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 15, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 21, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 17, { { 800, 0, 0 }, 25 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK1,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 12, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 15, 20, -15, { 0, 0, 0 } },
};

static DamageTable sDamageTable1 = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x1),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(2, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(3, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(1, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
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

static DamageTable sDamageTable2 = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x1),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(3, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
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

static CollisionCheckInfoInit sColChkInfoInit = { 8, 50, 100, MASS_HEAVY };

static TexturePtr sNormalEyeTextures[] = {
    gWolfosNormalEyeOpenTex,
    gWolfosNormalEyeHalfTex,
    gWolfosNormalEyeNarrowTex,
    gWolfosNormalEyeHalfTex,
};

static TexturePtr sWhiteEyeTextures[] = {
    gWolfosWhiteEyeOpenTex,
    gWolfosWhiteEyeHalfTex,
    gWolfosWhiteEyeNarrowTex,
    gWolfosWhiteEyeHalfTex,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void EnWf_Init(Actor* thisx, PlayState* play) {
    static s32 sTexturesDesegmented = false;
    EnWf* this = (EnWf*)thisx;
    s32 i;
    s32 temp_s0;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 70.0f);
    this->actor.shape.shadowAlpha = 150;
    Collider_InitAndSetJntSph(play, &this->collider1, &this->actor, &sJntSphInit, this->collider1Elements);
    Collider_InitAndSetCylinder(play, &this->collider2, &this->actor, &sCylinderInit1);
    Collider_InitAndSetCylinder(play, &this->collider3, &this->actor, &sCylinderInit2);
    Math_Vec3f_Copy(&this->actor.focus.pos, &this->actor.world.pos);

    if (this->actor.shape.rot.x <= 0) {
        this->unk_2A8 = 10000.0f;
    } else {
        this->unk_2A8 = this->actor.shape.rot.x * 40.0f;
    }

    this->actor.shape.rot.x = 0;
    this->actor.world.rot.x = 0;

    if (ENWF_GET_80(&this->actor)) {
        this->unk_295 = true;
    } else {
        this->unk_295 = false;
    }

    if (this->unk_295) {
        temp_s0 = this->actor.shape.rot.z + 100;
        temp_s0 = CLAMP(temp_s0, 1, 255);
        this->actor.shape.rot.z = 0;
        this->actor.world.rot.z = 0;
        this->actor.child = Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_ICE_POLY, this->actor.world.pos.x,
                                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                                        OBJICEPOLY_PARAMS_ALT(temp_s0, OBJICEPOLY_SWITCH_FLAG_NONE));
        if (this->actor.child != NULL) {
            Player* player = GET_PLAYER(play);

            this->actor.child->xzDistToPlayer = Actor_WorldDistXZToActor(&this->actor, &player->actor);
            this->unk_2A4 = (temp_s0 * 0.5f) + 45.0f;
            this->unk_29C = 0x32000 / this->unk_2A4;
            this->actor.shape.rot.y = Actor_WorldYawTowardActor(&this->actor, &player->actor);
            this->actor.world.pos.x -= this->unk_2A4 * Math_SinS(this->actor.shape.rot.y);
            this->actor.world.pos.z -= this->unk_2A4 * Math_CosS(this->actor.shape.rot.y);
        }
    }

    if (Flags_GetClear(play, this->actor.room)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.params &= 0x3F;
    Actor_SetScale(&this->actor, 0.0075f);

    if (this->actor.params == 0) {
        SkelAnime_InitFlex(play, &this->skelAnime, &gWolfosNormalSkel, &gWolfosWaitAnim, this->jointTable,
                           this->morphTable, WOLFOS_NORMAL_LIMB_MAX);
        this->actor.hintId = TATL_HINT_ID_WOLFOS;
        CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable2, &sColChkInfoInit);
        this->collider1.elements[0].base.atDmgInfo.damage = 8;
        this->collider1.elements[1].base.atDmgInfo.damage = 8;
        this->actor.colChkInfo.health = 6;
    } else {
        SkelAnime_InitFlex(play, &this->skelAnime, &gWolfosWhiteSkel, &gWolfosWaitAnim, this->jointTable,
                           this->morphTable, WOLFOS_WHITE_LIMB_MAX);
        this->actor.hintId = TATL_HINT_ID_WHITE_WOLFOS;
        CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable1, &sColChkInfoInit);
    }

    if (this->actor.child == NULL) {
        func_80990F0C(this);
        this->unk_2A0 = 0;
    } else {
        func_80992FD4(this);
    }

    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);

    if (!sTexturesDesegmented) {
        for (i = 0; i < ARRAY_COUNT(sNormalEyeTextures); i++) {
            sNormalEyeTextures[i] = Lib_SegmentedToVirtual(sNormalEyeTextures[i]);
            sWhiteEyeTextures[i] = Lib_SegmentedToVirtual(sWhiteEyeTextures[i]);
        }
        sTexturesDesegmented = true;
    }
    this->unk_2A2 = Rand_ZeroFloat(96.0f);
}

void EnWf_Destroy(Actor* thisx, PlayState* play) {
    EnWf* this = (EnWf*)thisx;

    Collider_DestroyJntSph(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
    Collider_DestroyCylinder(play, &this->collider3);
}

void func_809907D4(EnWf* this) {
    this->drawDmgEffScale = 0.75f;
    this->drawDmgEffFrozenSteamScale = 1.125f;
    this->drawDmgEffAlpha = 1.0f;
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->collider2.base.colMaterial = COL_MATERIAL_HIT3;
    this->collider3.base.colMaterial = COL_MATERIAL_HIT3;
    this->unk_2A0 = 80;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void func_80990854(EnWf* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->collider2.base.colMaterial = COL_MATERIAL_HIT5;
        this->collider3.base.colMaterial = COL_MATERIAL_HIT5;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, WOLFOS_BODYPART_MAX, 2, 0.3f, 0.2f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    }
}

void EnWf_Blink(EnWf* this) {
    if (this->eyeIndex != 0) {
        this->eyeIndex++;
        if (this->eyeIndex == 4) {
            this->eyeIndex = 0;
        }
    } else if (Rand_ZeroOne() < 0.05f) {
        this->eyeIndex = 1;
    }
}

s32 func_80990948(PlayState* play, EnWf* this, s16 arg2) {
    Player* player = GET_PLAYER(play);
    s32 sp38 = ABS_ALT(BINANG_SUB(this->actor.wallYaw, this->actor.shape.rot.y));
    s32 pad[2];
    s16 sp2E;
    Actor* actor;

    if ((!this->unk_295 ||
         ((this->unk_295 == true) && (this->actor.child != NULL) && (this->actor.child->update != NULL))) &&
        (this->actor.xzDistToPlayer > 160.0f) &&
        (this->unk_2A8 < Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos))) {
        func_80993524(this);
        return true;
    }

    if (func_800BE184(play, &this->actor, 100.0f, 10000, 12000, this->actor.shape.rot.y) &&
        ((player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) || ((play->gameplayFrames % 2) != 0))) {
        func_8099282C(this);
        return true;
    }

    if (func_800BE184(play, &this->actor, 100.0f, 24000, 0x2AA8, this->actor.shape.rot.y)) {
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (sp38 < 12000)) {
            func_809926D0(this);
            return true;
        }

        if ((player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) || ((play->gameplayFrames % 2) != 0)) {
            func_8099282C(this);
            return true;
        }

        func_8099223C(this);
        return true;
    }

    actor = func_800BC444(play, &this->actor, 80.0f);
    if ((actor != NULL) && (actor->id == ACTOR_EN_BOM_CHU)) {
        EnBomChu* bomchu = (EnBomChu*)actor;

        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        if (BINANG_ROT180(this->actor.shape.rot.y - bomchu->actor.world.rot.y) < 16000) {
            func_809926D0(this);
        }
        return true;
    }

    if (arg2) {
        if (!Actor_IsFacingPlayer(&this->actor, 7000)) {
            func_80992A74(this, play);
            return true;
        }

        sp2E = player->actor.shape.rot.y - this->actor.shape.rot.y;
        if ((this->actor.xzDistToPlayer <= 80.0f) && !Actor_OtherIsTargeted(play, &this->actor)) {
            if (((play->gameplayFrames % 8) != 0) || (ABS_ALT(sp2E) < 0x38E0)) {
                func_80991C04(this);
                return true;
            }
        }

        func_80991948(this);
        return true;
    }
    return false;
}

void func_80990C6C(EnWf* this, PlayState* play, s32 arg2) {
    static Vec3f sAccel = { 0.0f, 0.1f, 0.0f };
    static Color_RGBA8 D_809942E8 = { 185, 140, 70, 255 };
    static Color_RGBA8 D_809942EC = { 255, 255, 255, 255 };
    s32 i;
    Vec3f pos;
    FloorType floorType;
    Color_RGBA8* phi_s1;
    s16 phi_s6;

    if (this->actor.floorPoly != NULL) {
        floorType = SurfaceType_GetFloorType(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

        if (floorType != FLOOR_TYPE_5) {
            if ((floorType == FLOOR_TYPE_15) || (floorType == FLOOR_TYPE_14)) {
                phi_s1 = &D_809942EC;
                phi_s6 = Rand_ZeroFloat(150.0f) + 350.0f;
                arg2 += 2;
            } else {
                phi_s1 = &D_809942E8;
                phi_s6 = Rand_ZeroFloat(100.0f) + 250.0f;
            }

            for (i = 0; i < arg2; i++) {
                pos.x = Rand_CenteredFloat(50.0f) + this->actor.world.pos.x;
                pos.y = Rand_ZeroFloat(5.0f) + this->actor.world.pos.y;
                pos.z = Rand_CenteredFloat(50.0f) + this->actor.world.pos.z;
                func_800B0F18(play, &pos, &gZeroVec3f, &sAccel, phi_s1, phi_s1, phi_s6, 5,
                              Rand_ZeroFloat(5.0f) + 14.0f);
            }
        }
    }
}

void func_80990E4C(EnWf* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 1.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        func_80990C6C(this, play, 2);
        Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_WALK);
    }
}

s32 func_80990EAC(EnWf* this) {
    ObjIcePoly* icePoly = (ObjIcePoly*)this->actor.child;

    if (icePoly->unk_148 < 200) {
        return true;
    }
    return false;
}

void func_80990ED4(EnWf* this) {
    this->actor.child = NULL;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_APPEAR);
    func_80991438(this);
}

void func_80990F0C(EnWf* this) {
    this->collider2.base.acFlags &= ~AC_ON;
    this->actor.shape.shadowScale = 0.0f;
    this->actor.scale.y = 0.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_2A0 = 60;
    this->actionFunc = func_80990F50;
}

void func_80990F50(EnWf* this, PlayState* play) {
    f32 phi_f2 = 240.0f;

    if (this->unk_2A0 != 0) {
        this->unk_2A0--;
    }

    if (this->unk_2A0 <= 0) {
        if (this->unk_2A8 < 240.0f) {
            phi_f2 = this->unk_2A8;
        }

        if (this->actor.xzDistToPlayer < phi_f2) {
            func_80990FC8(this);
        }
    }
}

void func_80990FC8(EnWf* this) {
    Animation_Change(&this->skelAnime, &gWolfosRearUpFallOverAnim, 0.5f, 0.0f, 7.0f, ANIMMODE_ONCE_INTERP, 0.0f);
    this->unk_2A0 = 5;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = func_80991040;
}

void func_80991040(EnWf* this, PlayState* play) {
    if (this->unk_2A0 != 0) {
        this->actor.scale.y += this->actor.scale.x * 0.2f;
        Math_StepToF(&this->actor.shape.shadowScale, 70.0f, 14.0f);
        this->unk_2A0--;
        if (this->unk_2A0 == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_APPEAR);
        }
    } else if (SkelAnime_Update(&this->skelAnime)) {
        this->actor.scale.y = this->actor.scale.x;
        func_80991200(this);
    }
}

void func_809910F0(EnWf* this) {
    this->collider2.base.acFlags &= ~AC_ON;
    this->actor.speed = 0.0f;
    Animation_Change(&this->skelAnime, &gWolfosRearUpFallOverAnim, 0.5f, 0.0f, 7.0f, ANIMMODE_ONCE_INTERP, -5.0f);
    this->unk_2A0 = 5;
    this->actionFunc = func_80991174;
}

void func_80991174(EnWf* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_2A0 > 0) {
            this->actor.scale.y -= this->actor.scale.x * 0.2f;
            Math_StepToF(&this->actor.shape.shadowScale, 0.0f, 14.0f);
            this->unk_2A0--;
        } else {
            func_80990F0C(this);
        }
    }
}

void func_80991200(EnWf* this) {
    this->collider2.base.acFlags |= AC_ON;
    Animation_MorphToLoop(&this->skelAnime, &gWolfosWaitAnim, -4.0f);
    this->unk_2A0 = (s32)Rand_ZeroFloat(10.0f) + 2;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = func_80991280;
    this->actor.speed = 0.0f;
}

void func_80991280(EnWf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 temp_v0;
    s32 phi_v1;

    SkelAnime_Update(&this->skelAnime);
    if (this->unk_298 != 0) {
        temp_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y - this->unk_29E;
        if (ABS_ALT(temp_v0) > 0x2000) {
            this->unk_298--;
            return;
        }
        this->unk_298 = 0;
    }

    if (!func_8099408C(play, this) && !func_80990948(play, this, false)) {
        phi_v1 = ABS_ALT(BINANG_SUB(player->actor.shape.rot.y, this->actor.shape.rot.y));
        if ((this->actor.xzDistToPlayer < 80.0f) && (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) &&
            (phi_v1 >= 0x1F40)) {
            this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            func_80991948(this);
        } else {
            this->unk_2A0--;
            if (this->unk_2A0 == 0) {
                if (Actor_IsFacingPlayer(&this->actor, 0x1555)) {
                    if (Rand_ZeroOne() > 0.3f) {
                        func_80991438(this);
                    } else {
                        func_80991948(this);
                    }
                } else {
                    func_80991738(this);
                }

                if (this->unk_2A2 == 0) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_CRY);
                }
            }
        }
    }
}

void func_80991438(EnWf* this) {
    this->collider2.base.acFlags |= AC_ON;
    Animation_MorphToLoop(&this->skelAnime, &gWolfosRunAnim, -4.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.speed = 8.0f;
    this->actionFunc = func_8099149C;
}

void func_8099149C(EnWf* this, PlayState* play) {
    f32 sp2C;
    s32 sp28;
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (!func_8099408C(play, this)) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x1F4);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (Actor_OtherIsTargeted(play, &this->actor)) {
            sp2C = 150.0f;
        } else {
            sp2C = 0.0f;
        }

        sp28 = ABS_ALT(BINANG_SUB(player->actor.shape.rot.y, this->actor.shape.rot.y));

        if ((this->actor.xzDistToPlayer < (150.0f + sp2C)) &&
            (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) && (sp28 >= 0x1F40)) {
            this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            if (Rand_ZeroOne() > 0.7f) {
                func_80991948(this);
                return;
            }
        }

        SkelAnime_Update(&this->skelAnime);
        if (!func_80990948(play, this, false)) {
            if (!Actor_IsFacingPlayer(&this->actor, 0x11C7)) {
                if (Rand_ZeroOne() > 0.5f) {
                    func_80991948(this);
                } else {
                    func_80991200(this);
                }
            } else if (this->actor.xzDistToPlayer < (90.0f + sp2C)) {
                if (!Actor_OtherIsTargeted(play, &this->actor) &&
                    ((Rand_ZeroOne() > 0.03f) || ((this->actor.xzDistToPlayer <= 80.0f) && (sp28 < 0x38E0)))) {
                    func_80991C04(this);
                } else if (Actor_OtherIsTargeted(play, &this->actor) && (Rand_ZeroOne() > 0.5f)) {
                    func_8099223C(this);
                } else {
                    func_80991948(this);
                }
            }

            if (this->unk_2A2 == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_CRY);
            }

            func_80990E4C(this, play);
        }
    }
}

void func_80991738(EnWf* this) {
    Animation_MorphToLoop(&this->skelAnime, &gWolfosSidestepAnim, -4.0f);
    this->actionFunc = func_8099177C;
}

void func_8099177C(EnWf* this, PlayState* play) {
    s16 temp_v0;
    s16 phi_v1;
    f32 phi_f2;
    f32 phi_f12;

    if (!func_8099408C(play, this) && !func_80990948(play, this, false)) {
        temp_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        if (temp_v0 > 0) {
            phi_v1 = (temp_v0 * 0.25f) + 2000.0f;
        } else {
            phi_v1 = (temp_v0 * 0.25f) - 2000.0f;
        }

        this->actor.shape.rot.y += phi_v1;
        this->actor.world.rot.y = this->actor.shape.rot.y;

        if (temp_v0 > 0) {
            phi_f2 = phi_v1 * 0.5f;
            phi_f12 = 1.0f;
            if (phi_f2 > 1.0f) {
                phi_f2 = phi_f12;
            }
        } else {
            phi_f2 = phi_v1 * 0.5f;
            phi_f12 = -1.0f;
            if (phi_f2 < -1.0f) {
                phi_f2 = phi_f12;
            }
        }

        this->skelAnime.playSpeed = -phi_f2;
        SkelAnime_Update(&this->skelAnime);

        if (Actor_IsFacingPlayer(&this->actor, 0x1555)) {
            if (Rand_ZeroOne() > 0.8f) {
                func_80991948(this);
            } else {
                func_80991438(this);
            }
        }
        if (this->unk_2A2 == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_CRY);
        }
    }
}

void func_80991948(EnWf* this) {
    this->collider2.base.acFlags |= AC_ON;
    Animation_MorphToLoop(&this->skelAnime, &gWolfosRunAnim, -4.0f);
    if (Rand_ZeroOne() > 0.5f) {
        this->unk_29A = 16000;
    } else {
        this->unk_29A = -16000;
    }
    this->actor.speed = 6.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->unk_2A0 = (s32)Rand_ZeroFloat(30.0f) + 30;
    this->actionFunc = func_809919F4;
}

void func_809919F4(EnWf* this, PlayState* play) {
    s16 sp26;
    Player* player = GET_PLAYER(play);
    s16 temp_v0;

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + this->unk_29A, 0x9C4);

    if (!func_8099408C(play, this) && !func_80990948(play, this, false)) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        sp26 = BINANG_ROT180(player->actor.shape.rot.y + this->unk_29A);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            temp_v0 = (this->actor.wallYaw - this->actor.yawTowardsPlayer) - this->unk_29A;
            if (ABS_ALT(temp_v0) > 0x2EE0) {
                this->unk_29A = -this->unk_29A;
            }
        }

        SkelAnime_Update(&this->skelAnime);
        func_80990E4C(this, play);

        if (this->unk_2A2 == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_CRY);
        }

        if ((Math_CosS(sp26 - this->actor.shape.rot.y) < -0.85f) && !Actor_OtherIsTargeted(play, &this->actor) &&
            (this->actor.xzDistToPlayer <= 80.0f)) {
            func_80991C04(this);
        } else {
            this->unk_2A0--;
            if (this->unk_2A0 == 0) {
                if (Actor_OtherIsTargeted(play, &this->actor) && (Rand_ZeroOne() > 0.5f)) {
                    func_8099223C(this);
                } else {
                    func_80991200(this);
                    this->unk_2A0 = (s32)Rand_ZeroFloat(3.0f) + 1;
                }
            }
        }
    }
}

void func_80991C04(EnWf* this) {
    this->collider2.base.acFlags |= AC_ON;
    Animation_PlayOnce(&this->skelAnime, &gWolfosSlashAnim);
    this->collider1.base.atFlags &= ~AT_BOUNCED;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->unk_2A0 = 7;
    this->skelAnime.endFrame = 20.0f;
    this->actor.speed = 0.0f;
    this->actionFunc = func_80991C80;
}

void func_80991C80(EnWf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 sp30;
    s32 onAnim15thFrame;
    s16 sp2A;

    sp2A = BINANG_SUB(player->actor.shape.rot.y, this->actor.shape.rot.y);
    sp30 = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y));

    this->actor.speed = 0.0f;

    if (((this->skelAnime.curFrame >= 9.0f) && (this->skelAnime.curFrame < 13.0f)) ||
        ((this->skelAnime.curFrame >= 17.0f) && (this->skelAnime.curFrame < 20.0f))) {
        if (!(this->collider1.base.atFlags & AT_ON)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_ATTACK);
        }
        this->collider1.base.atFlags |= AT_ON;
    } else {
        this->collider1.base.atFlags &= ~AT_ON;
    }

    onAnim15thFrame = Animation_OnFrame(&this->skelAnime, 15.0f);

    if ((onAnim15thFrame && !Actor_IsTargeted(play, &this->actor) &&
         (!Actor_IsFacingPlayer(&this->actor, 0x2000) || (this->actor.xzDistToPlayer >= 100.0f))) ||
        SkelAnime_Update(&this->skelAnime)) {
        if (!onAnim15thFrame && (this->unk_2A0 != 0)) {
            this->actor.shape.rot.y += TRUNCF_BINANG(0xCCC * (1.5f + ((this->unk_2A0 - 4) * 0.4f)));
            func_80990C6C(this, play, 1);
            this->unk_2A0--;
        } else if (!Actor_IsFacingPlayer(&this->actor, 0x1554) && !onAnim15thFrame) {
            func_80991200(this);
            this->unk_2A0 = (s32)Rand_ZeroFloat(5.0f) + 5;
            if (sp30 >= 0x32C9) {
                this->unk_298 = 7;
            }
        } else if ((Rand_ZeroOne() > 0.7f) || (this->actor.xzDistToPlayer >= 120.0f)) {
            func_80991200(this);
            this->unk_2A0 = (s32)Rand_ZeroFloat(5.0f) + 5;
        } else {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            if (Rand_ZeroOne() > 0.7f) {
                func_80992A74(this, play);
            } else if (ABS_ALT(sp2A) < 0x2711) {
                if (sp30 >= 0x3E81) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    func_80991948(this);
                } else {
                    func_80990948(play, this, true);
                }
            } else {
                func_80991948(this);
            }
        }
    }
}

void func_80991FD8(EnWf* this) {
    f32 phi_f0 = 1.0f;

    if (this->skelAnime.curFrame > 15.0f) {
        phi_f0 = 15.0f;
    }
    Animation_Change(&this->skelAnime, &gWolfosSlashAnim, -0.5f, this->skelAnime.curFrame - 1.0f, phi_f0,
                     ANIMMODE_ONCE_INTERP, 0.0f);
    this->collider1.base.atFlags &= ~AT_ON;
    this->actionFunc = func_80992068;
}

void func_80992068(EnWf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 sp28;
    s16 sp26;

    sp26 = BINANG_SUB(player->actor.shape.rot.y, this->actor.shape.rot.y);
    sp28 = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y));

    if (SkelAnime_Update(&this->skelAnime)) {
        if (!Actor_IsFacingPlayer(&this->actor, 0x1554)) {
            func_80991200(this);
            this->unk_2A0 = (s32)Rand_ZeroFloat(5.0f) + 5;
            if (sp28 > 0x32C8) {
                this->unk_298 = 30;
            }
        } else if ((Rand_ZeroOne() > 0.7f) || (this->actor.xzDistToPlayer >= 120.0f)) {
            func_80991200(this);
            this->unk_2A0 = (s32)Rand_ZeroFloat(5.0f) + 5;
        } else {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            if (Rand_ZeroOne() > 0.7f) {
                func_80992A74(this, play);
            } else if (ABS_ALT(sp26) < 0x2711) {
                if (sp28 > 0x3E80) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    func_80991948(this);
                } else {
                    func_80990948(play, this, true);
                }
            } else {
                func_80991948(this);
            }
        }
    }
}

void func_8099223C(EnWf* this) {
    this->collider2.base.acFlags &= ~AC_ON;
    Animation_MorphToPlayOnce(&this->skelAnime, &gWolfosBackflipAnim, -3.0f);
    this->unk_2A0 = 0;
    this->actor.speed = -6.0f;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Actor_PlaySfx(&this->actor, NA_SE_EN_TEKU_JUMP);
    this->actionFunc = func_809922B4;
}

void func_809922B4(EnWf* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (!Actor_OtherIsTargeted(play, &this->actor) && (this->actor.xzDistToPlayer < 170.0f) &&
            (this->actor.xzDistToPlayer > 140.0f) && (Rand_ZeroOne() < 0.2f)) {
            func_80991438(this);
        } else if ((play->gameplayFrames % 2) != 0) {
            func_80992A74(this, play);
        } else {
            func_80991200(this);
        }
    }
    if (this->unk_2A2 == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_CRY);
    }
}

void func_809923B0(EnWf* this) {
    this->actor.speed = 0.0f;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }
    this->actionFunc = func_809923E4;
}

void func_809923E4(EnWf* this, PlayState* play) {
    if (this->unk_2A0 != 0) {
        this->unk_2A0--;
    }

    if (this->unk_2A0 == 0) {
        func_80990854(this, play);
        if (this->actor.colChkInfo.health == 0) {
            func_80992D6C(this);
        } else {
            func_80990948(play, this, true);
        }
    }
}

void func_8099245C(EnWf* this) {
    this->collider2.base.acFlags &= ~AC_ON;
    Animation_MorphToPlayOnce(&this->skelAnime, &gWolfosDamagedAnim, -4.0f);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = -4.0f;
    }
    this->unk_298 = 0;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_DAMAGE);
    this->actionFunc = func_809924EC;
}

void func_809924EC(EnWf* this, PlayState* play) {
    s16 sp26;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.speed < 0.0f)) {
        this->actor.speed += 0.05f;
    }

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xBB8);

    if (SkelAnime_Update(&this->skelAnime)) {
        sp26 = this->actor.wallYaw - this->actor.shape.rot.y;

        if (func_80990948(play, this, false)) {
            return;
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (ABS_ALT(sp26) < 0x2EE0) &&
            (this->actor.xzDistToPlayer < 120.0f)) {
            func_809926D0(this);
        } else if (!func_8099408C(play, this)) {
            if ((this->actor.xzDistToPlayer <= 80.0f) && !Actor_OtherIsTargeted(play, &this->actor) &&
                ((play->gameplayFrames % 8) != 0)) {
                func_80991C04(this);
            } else if (Rand_ZeroOne() > 0.5f) {
                func_80991200(this);
                this->unk_2A0 = (s32)Rand_ZeroFloat(5.0f) + 5;
                this->unk_298 = 30;
            } else {
                func_8099223C(this);
            }
        }
    }
}

void func_809926D0(EnWf* this) {
    this->collider2.base.acFlags &= ~AC_ON;
    Animation_Change(&this->skelAnime, &gWolfosBackflipAnim, -1.0f, Animation_GetLastFrame(&gWolfosBackflipAnim), 0.0f,
                     ANIMMODE_ONCE, -3.0f);
    this->unk_2A0 = 0;
    this->actor.speed = 6.5f;
    this->actor.velocity.y = 15.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_TEKU_JUMP);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = func_80992784;
}

void func_80992784(EnWf* this, PlayState* play) {
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x9C4);
    if (SkelAnime_Update(&this->skelAnime) &&
        (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH))) {
        this->actor.shape.rot.x = 0;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        this->actor.world.pos.y = this->actor.floorHeight;
        if (!Actor_OtherIsTargeted(play, &this->actor)) {
            func_80991C04(this);
        } else {
            func_80991200(this);
        }
    }
}

void func_8099282C(EnWf* this) {
    this->collider2.base.acFlags |= AC_ON;
    this->collider1.base.atFlags &= ~AT_ON;
    this->unk_2A0 = 10;
    this->actor.speed = 0.0f;
    Animation_Change(&this->skelAnime, &gWolfosBlockAnim, -1.0f, Animation_GetLastFrame(&gWolfosBlockAnim), 0.0f,
                     ANIMMODE_ONCE, -2.0f);
    this->actionFunc = func_809928CC;
}

void func_809928CC(EnWf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 sp2A;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_2A0 != 0) {
            this->unk_2A0--;
        } else if (func_800BE184(play, &this->actor, 100.0f, 10000, 0x4000, this->actor.shape.rot.y)) {
            if ((player->meleeWeaponAnimation != PLAYER_MWA_JUMPSLASH_START) || ((play->gameplayFrames % 2) != 0)) {
                this->unk_2A0 = 10;
            } else {
                func_8099223C(this);
            }
        } else if (Actor_IsFacingPlayer(&this->actor, 0x4000) && (this->actor.xzDistToPlayer < 60.0f) &&
                   (fabsf(this->actor.playerHeightRel) < 50.0f)) {
            sp2A = player->actor.shape.rot.y - this->actor.shape.rot.y;
            if (!Actor_OtherIsTargeted(play, &this->actor) &&
                (((play->gameplayFrames % 2) != 0) || (ABS_ALT(sp2A) < 0x38E0))) {
                func_80991C04(this);
            } else {
                func_80991948(this);
            }
        } else {
            func_80991948(this);
        }
    }
}

void func_80992A74(EnWf* this, PlayState* play) {
    Player* player;
    f32 temp_f0;

    this->collider2.base.acFlags |= AC_ON;
    Animation_MorphToLoop(&this->skelAnime, &gWolfosRunAnim, -4.0f);
    player = GET_PLAYER(play);
    temp_f0 = Math_SinS((player->actor.shape.rot.y + this->unk_29A) - this->actor.yawTowardsPlayer);

    if (temp_f0 > 0.0f) {
        this->unk_29A = 0x3E80;
    } else if (temp_f0 < 0.0f) {
        this->unk_29A = -0x3E80;
    } else if (Rand_ZeroOne() > 0.5f) {
        this->unk_29A = 0x3E80;
    } else {
        this->unk_29A = -0x3E80;
    }

    this->actor.speed = 6.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->unk_2A0 = (s32)Rand_ZeroFloat(10.0f) + 5;
    this->actionFunc = func_80992B8C;
}

void func_80992B8C(EnWf* this, PlayState* play) {
    s32 sp28;
    s32 sp2C;
    Player* player = GET_PLAYER(play);

    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + this->unk_29A, 0x7D0);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        sp2C = BINANG_SUB((this->actor.wallYaw - this->actor.yawTowardsPlayer), this->unk_29A);
        if (ABS_ALT(sp2C) > 0x2EE0) {
            this->unk_29A = -this->unk_29A;
        }
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    SkelAnime_Update(&this->skelAnime);

    if (!func_80990948(play, this, false)) {
        this->unk_2A0--;
        if (this->unk_2A0 == 0) {
            sp28 = ABS_ALT(BINANG_SUB(player->actor.shape.rot.y, this->actor.yawTowardsPlayer));
            if (sp28 >= 0x3A98) {
                func_80991200(this);
                this->unk_2A0 = (s32)Rand_ZeroFloat(3.0f) + 1;
            } else {
                this->actor.world.rot.y = this->actor.shape.rot.y;
                if ((this->actor.xzDistToPlayer <= 80.0f) && !Actor_OtherIsTargeted(play, &this->actor) &&
                    (((play->gameplayFrames % 4) == 0) || (sp28 < 0x38E0))) {
                    func_80991C04(this);
                } else {
                    func_80991438(this);
                }
            }
        }

        func_80990E4C(this, play);

        if (this->unk_2A2 == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_CRY);
        }
    }
}

void func_80992D6C(EnWf* this) {
    this->collider2.base.acFlags &= ~AC_ON;
    Animation_MorphToPlayOnce(&this->skelAnime, &gWolfosRearUpFallOverAnim, -4.0f);
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = -6.0f;
    }
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_2A0 = 25;
    Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_DEAD);
    this->actionFunc = func_80992E0C;
}

void func_80992E0C(EnWf* this, PlayState* play) {
    static Vec3f D_809942F0 = { 0.0f, 0.5f, 0.0f };

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_295) {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x70);
        } else {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x60);
        }
        Actor_Kill(&this->actor);
    } else {
        s32 i;
        Vec3f sp60;

        if (this->unk_2A0 == 25) {
            SoundSource_PlaySfxEachFrameAtFixedWorldPos(play, &this->actor.world.pos, 36,
                                                        NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
        }

        this->unk_2A0--;

        for (i = (25 - this->unk_2A0) >> 1; i >= 0; i--) {
            sp60.x = Rand_CenteredFloat(60.0f) + this->actor.world.pos.x;
            sp60.z = Rand_CenteredFloat(60.0f) + this->actor.world.pos.z;
            sp60.y = Rand_CenteredFloat(50.0f) + (this->actor.world.pos.y + 20.0f);
            func_800B3030(play, &sp60, &D_809942F0, &D_809942F0, 100, 0, 2);
        }

        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
    }
}

void func_80992FD4(EnWf* this) {
    Animation_MorphToLoop(&this->skelAnime, &gWolfosWaitAnim, -4.0f);
    this->actionFunc = func_80993018;
}

void func_80993018(EnWf* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
        if (func_80990EAC(this)) {
            func_80990ED4(this);
        } else if (this->actor.child->xzDistToPlayer < (80.0f + this->unk_2A4)) {
            func_80993350(this);
        } else {
            s16 temp_v0 = Actor_WorldYawTowardActor(this->actor.child, &this->actor);
            s16 temp_v1 = BINANG_SUB(temp_v0 - this->actor.child->yawTowardsPlayer, 0x8000);

            if (ABS_ALT(temp_v1) > 0x800) {
                func_80993148(this);
            } else {
                Math_ScaledStepToS(&this->actor.shape.rot.y, BINANG_ROT180(temp_v0), 0x800);
            }
        }
    } else {
        func_80990ED4(this);
    }
}

void func_80993148(EnWf* this) {
    Animation_MorphToLoop(&this->skelAnime, &gWolfosRunAnim, -4.0f);
    this->actor.speed = 0.0f;
    this->actionFunc = func_80993194;
}

void func_80993194(EnWf* this, PlayState* play) {
    s16 sp36;
    s16 sp34;
    s16 sp32;

    SkelAnime_Update(&this->skelAnime);
    func_80990E4C(this, play);

    if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
        if (func_80990EAC(this)) {
            func_80990ED4(this);
        } else if (this->actor.child->xzDistToPlayer < (80.0f + this->unk_2A4)) {
            func_80993350(this);
        } else {
            sp36 = Actor_WorldYawTowardActor(this->actor.child, &this->actor);
            sp34 = Math_SmoothStepToS(&sp36, BINANG_ROT180(this->actor.child->yawTowardsPlayer), 10, this->unk_29C, 16);
            this->actor.world.pos.x = (Math_SinS(sp36) * this->unk_2A4) + this->actor.child->world.pos.x;
            this->actor.world.pos.z = (Math_CosS(sp36) * this->unk_2A4) + this->actor.child->world.pos.z;
            if (sp34 < -0x800) {
                sp32 = sp36 - 0x4000;
            } else if (sp34 > 0x800) {
                sp32 = sp36 + 0x4000;
            } else {
                sp32 = sp36 + 0x8000;
                func_80992FD4(this);
            }
            Math_ScaledStepToS(&this->actor.shape.rot.y, sp32, 0x800);
        }
    } else {
        func_80990ED4(this);
    }

    if (this->unk_2A2 == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_CRY);
    }
}

void func_80993350(EnWf* this) {
    Animation_MorphToLoop(&this->skelAnime, &gWolfosRunAnim, -4.0f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_APPEAR);
    this->actionFunc = func_809933A0;
}

void func_809933A0(EnWf* this, PlayState* play) {
    s16 sp2E;
    s16 sp2C;
    s16 phi_a1;

    SkelAnime_Update(&this->skelAnime);
    func_80990E4C(this, play);

    if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
        if (func_80990EAC(this)) {
            func_80990ED4(this);
        } else {
            sp2E = Actor_WorldYawTowardActor(this->actor.child, &this->actor);
            Math_ScaledStepToS(&sp2E, this->actor.child->yawTowardsPlayer, this->unk_29C >> 2);
            sp2C = sp2E - this->actor.child->yawTowardsPlayer;
            this->actor.world.pos.x = (Math_SinS(sp2E) * this->unk_2A4) + this->actor.child->world.pos.x;
            this->actor.world.pos.z = (Math_CosS(sp2E) * this->unk_2A4) + this->actor.child->world.pos.z;
            if (sp2C > 0) {
                phi_a1 = sp2E - 0x4000;
            } else {
                phi_a1 = sp2E + 0x4000;
            }
            Math_ScaledStepToS(&this->actor.shape.rot.y, phi_a1, 0x800);
            if (ABS_ALT(sp2C) < 0x3000) {
                func_80991438(this);
            }
        }
    } else {
        func_80990ED4(this);
    }

    if (this->unk_2A2 == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_CRY);
    }
}

void func_80993524(EnWf* this) {
    Animation_MorphToLoop(&this->skelAnime, &gWolfosRunAnim, -4.0f);
    this->actor.speed = 6.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = func_8099357C;
}

void func_8099357C(EnWf* this, PlayState* play) {
    s16 sp2E;

    SkelAnime_Update(&this->skelAnime);
    func_80990E4C(this, play);

    if (this->unk_295) {
        if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
            sp2E = Actor_WorldYawTowardActor(&this->actor, this->actor.child);
            if (func_80990EAC(this)) {
                this->actor.child = NULL;
                func_80991738(this);
                return;
            }

            if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                if (BINANG_SUB(sp2E, this->actor.wallYaw) > 0) {
                    sp2E = this->actor.wallYaw - 0x4000;
                } else {
                    sp2E = this->actor.wallYaw + 0x4000;
                }
            }

            Math_SmoothStepToS(&this->actor.shape.rot.y, sp2E, 10, 0x800, 0x10);
            this->actor.world.rot.y = this->actor.shape.rot.y;

            if (Actor_WorldDistXZToActor(&this->actor, this->actor.child) < (this->unk_2A4 + 10.0f)) {
                func_80993148(this);
            }
        } else {
            this->actor.child = NULL;
            func_80991738(this);
        }
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos), 10,
                           0x800, 0x10);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) < 40.0f) {
            func_809910F0(this);
        }
    }

    if (this->unk_2A2 == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_CRY);
    }
}

void func_80993738(EnWf* this, PlayState* play) {
    if ((this->actionFunc == func_80991280) && (this->unk_298 != 0)) {
        this->unk_29E = Math_SinS(this->unk_298 * 4200) * 8920.0f;
    } else if ((this->actionFunc == func_80991C80) || (this->actionFunc == func_8099357C) ||
               (this->actionFunc == func_80991040) || (this->actionFunc == func_80991174)) {
        Math_ScaledStepToS(&this->unk_29E, 0, 0x3E8);
    } else if (this->actionFunc != func_809923E4) {
        Math_ScaledStepToS(&this->unk_29E, this->actor.yawTowardsPlayer - this->actor.shape.rot.y, 0x3E8);
        this->unk_29E = CLAMP(this->unk_29E, -0x3127, 0x3127);
    }
}

void func_8099386C(EnWf* this, PlayState* play) {
    s32 pad;
    ColliderCylinder* collider;

    if (this->collider1.base.acFlags & AC_BOUNCED) {
        this->collider1.base.acFlags &= ~(AC_BOUNCED | AC_HIT);
        this->collider2.base.acFlags &= ~AC_HIT;
        this->collider3.base.acFlags &= ~AC_HIT;
        return;
    }

    if ((this->collider2.base.acFlags & AC_HIT) || (this->collider3.base.acFlags & AC_HIT)) {
        if ((!(this->collider2.base.acFlags & AC_HIT) && (this->collider3.base.acFlags & AC_HIT)) ||
            !Actor_IsFacingPlayer(&this->actor, 0x4A38)) {
            this->actor.colChkInfo.damage *= 4;
        }

        if (this->collider2.base.acFlags & AC_HIT) {
            Actor_SetDropFlag(&this->actor, &this->collider2.elem);
            collider = &this->collider2;
        } else {
            Actor_SetDropFlag(&this->actor, &this->collider3.elem);
            collider = &this->collider3;
        }

        this->collider2.base.acFlags &= ~AC_HIT;
        this->collider3.base.acFlags &= ~AC_HIT;
        this->collider1.base.atFlags &= ~AT_ON;

        if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
             !(collider->elem.acHitElem->atDmgInfo.dmgFlags &
               (0x80000 | 0x40000 | 0x10000 | 0x8000 | 0x2000 | 0x1000 | 0x80 | 0x20 | 0x10 | 0x2 | 0x1))) &&
            (this->actor.colChkInfo.damageEffect != 0xF)) {
            if (!Actor_ApplyDamage(&this->actor)) {
                Enemy_StartFinishingBlow(play, &this->actor);
            }

            func_80990854(this, play);

            if (this->actor.colChkInfo.damageEffect == 1) {
                this->unk_2A0 = 40;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                func_809923B0(this);
            } else if (this->actor.colChkInfo.damageEffect == 5) {
                this->unk_2A0 = 40;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                this->drawDmgEffScale = 0.75f;
                this->drawDmgEffAlpha = 2.0f;
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                func_809923B0(this);
            } else if (this->actor.colChkInfo.damageEffect == 3) {
                func_809907D4(this);
                if (this->actor.colChkInfo.health == 0) {
                    this->unk_2A0 = 3;
                    this->collider2.base.acFlags &= ~AC_ON;
                }
                func_809923B0(this);
            } else {
                if (this->actor.colChkInfo.damageEffect == 2) {
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                    this->drawDmgEffScale = 0.75f;
                    this->drawDmgEffAlpha = 4.0f;
                } else if (this->actor.colChkInfo.damageEffect == 4) {
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    this->drawDmgEffScale = 0.75f;
                    this->drawDmgEffAlpha = 4.0f;
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, collider->elem.acDmgInfo.hitPos.x,
                                collider->elem.acDmgInfo.hitPos.y, collider->elem.acDmgInfo.hitPos.z, 0, 0, 0,
                                CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                }

                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);

                if (this->actor.colChkInfo.health == 0) {
                    func_80992D6C(this);
                } else {
                    func_8099245C(this);
                }
            }
        }
    }
}

void EnWf_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWf* this = (EnWf*)thisx;

    if (this->unk_2A2 == 0) {
        this->unk_2A2 = 96;
    } else {
        this->unk_2A2--;
    }

    func_8099386C(this, play);
    if (this->actionFunc != func_809923E4) {
        EnWf_Blink(this);
    }

    this->actionFunc(this, play);

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 32.0f, 30.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    func_80993738(this, play);

    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
        func_800BE3D0(&this->actor, this->actor.shape.rot.y, &this->actor.shape.rot);
    } else {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x258);
        Math_ScaledStepToS(&this->actor.shape.rot.z, 0, 0x258);
    }

    if (this->actionFunc != func_80990F50) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
    }

    if (this->collider2.base.acFlags & AC_ON) {
        Collider_UpdateCylinder(&this->actor, &this->collider2);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider3.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
    }

    if (this->actionFunc == func_809928CC) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
    }

    if (this->collider1.base.atFlags & AT_ON) {
        if (!(this->collider1.base.atFlags & AT_BOUNCED)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
        } else {
            func_80991FD8(this);
        }
    }

    Actor_SetFocus(&this->actor, 25.0f);

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.375f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.75f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.75f, 0.01875f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

s32 EnWf_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnWf* this = (EnWf*)thisx;

    if ((limbIndex == WOLFOS_NORMAL_LIMB_HEAD) || (limbIndex == WOLFOS_NORMAL_LIMB_EYES)) {
        rot->y -= this->unk_29E;
    }
    return false;
}

static s8 sLimbToBodyParts[WOLFOS_NORMAL_LIMB_MAX] = {
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_NONE
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_ROOT
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_BACK_LEFT_THIGH
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_BACK_LEFT_SHIN
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_BACK_LEFT_PASTERN
    WOLFOS_BODYPART_BACK_LEFT_PAW,         // WOLFOS_NORMAL_LIMB_BACK_LEFT_PAW
    WOLFOS_BODYPART_TAIL,                  // WOLFOS_NORMAL_LIMB_TAIL
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_ABDOMEN
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_BACK_RIGHT_THIGH
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_BACK_RIGHT_SHIN
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_BACK_RIGHT_PASTERN
    WOLFOS_BODYPART_BACK_RIGHT_PAW,        // WOLFOS_NORMAL_LIMB_BACK_RIGHT_PAW
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_THORAX
    WOLFOS_BODYPART_FRONT_RIGHT_UPPER_LEG, // WOLFOS_NORMAL_LIMB_FRONT_RIGHT_UPPER_LEG
    WOLFOS_BODYPART_FRONT_RIGHT_LOWER_LEG, // WOLFOS_NORMAL_LIMB_FRONT_RIGHT_LOWER_LEG
    WOLFOS_BODYPART_FRONT_RIGHT_CLAW,      // WOLFOS_NORMAL_LIMB_FRONT_RIGHT_CLAW
    WOLFOS_BODYPART_HEAD_ROOT,             // WOLFOS_NORMAL_LIMB_HEAD_ROOT
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_HEAD
    BODYPART_NONE,                         // WOLFOS_NORMAL_LIMB_EYES
    WOLFOS_BODYPART_FRONT_LEFT_UPPER_LEG,  // WOLFOS_NORMAL_LIMB_FRONT_LEFT_UPPER_LEG
    WOLFOS_BODYPART_FRONT_LEFT_LOWER_LEG,  // WOLFOS_NORMAL_LIMB_FRONT_LEFT_LOWER_LEG
    WOLFOS_BODYPART_FRONT_LEFT_CLAW,       // WOLFOS_NORMAL_LIMB_FRONT_LEFT_CLAW
};

void EnWf_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnWf* this = (EnWf*)thisx;
    Vec3f sp20;

    Collider_UpdateSpheres(limbIndex, &this->collider1);

    if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
    }

    if (limbIndex == WOLFOS_NORMAL_LIMB_TAIL) {
        Matrix_MultVecX(1200.0f, &sp20);
        this->collider3.dim.pos.x = sp20.x;
        this->collider3.dim.pos.y = sp20.y;
        this->collider3.dim.pos.z = sp20.z;
    }
}

void EnWf_Draw(Actor* thisx, PlayState* play) {
    EnWf* this = (EnWf*)thisx;

    if (this->actionFunc != func_80990F50) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        if (this->actor.params == 0) {
            gSPSegment(POLY_OPA_DISP++, 0x08, sNormalEyeTextures[this->eyeIndex]);
        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, sWhiteEyeTextures[this->eyeIndex]);
        }

        CLOSE_DISPS(play->state.gfxCtx);

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnWf_OverrideLimbDraw, EnWf_PostLimbDraw, &this->actor);
        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, WOLFOS_BODYPART_MAX, this->drawDmgEffScale,
                                this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);
    }
}

s32 func_8099408C(PlayState* play, EnWf* this) {
    Actor* temp_v0 = func_800BC270(play, &this->actor, 80.0f, 0x138B0);
    s16 temp_v1;

    if (temp_v0 != NULL) {
        temp_v1 = Actor_WorldYawTowardActor(&this->actor, temp_v0) - this->actor.shape.rot.y - this->unk_29E;
        if (ABS_ALT(temp_v1) < 0x3000) {
            if (Rand_ZeroOne() < 0.5f) {
                func_8099223C(this);
            } else {
                func_8099282C(this);
            }
            return true;
        }
    }
    return false;
}
