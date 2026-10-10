/*
 * File: z_en_m_thunder.c
 * Overlay: ovl_En_M_Thunder
 * Description: Spin attack and sword beams
 */

#include "z_en_m_thunder.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_Eff_Dust/z_eff_dust.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "2s2h/BenGui/CosmeticEditor.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnMThunder_Init(Actor* thisx, PlayState* play);
void EnMThunder_Destroy(Actor* thisx, PlayState* play);
void EnMThunder_Update(Actor* thisx, PlayState* play);
void EnMThunder_Draw(Actor* thisx, PlayState* play2);

void EnMThunder_UnkType_Update(Actor* thisx, PlayState* play);

void EnMThunder_AdjustLights(PlayState* play, f32 arg1);

void EnMThunder_Charge(EnMThunder* this, PlayState* play);
void EnMThunder_Spin_Attack(EnMThunder* this, PlayState* play);
void EnMThunder_SwordBeam_Attack(EnMThunder* this, PlayState* play);
void EnMThunder_UnkType_Attack(EnMThunder* this, PlayState* play);

#define ENMTHUNDER_TYPE_MAX 4

ActorProfile En_M_Thunder_Profile = {
    /**/ ACTOR_EN_M_THUNDER,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnMThunder),
    /**/ EnMThunder_Init,
    /**/ EnMThunder_Destroy,
    /**/ EnMThunder_Update,
    /**/ EnMThunder_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x01000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 200, 200, 0, { 0, 0, 0 } },
};

static u8 sDamages[] = {
    1, 2, 3, 4, // Regular
    1, 2, 3, 4, // Great Spin
};

typedef enum {
    /* 0 */ ENMTHUNDER_SUBTYPE_SPIN_GREAT,
    /* 1 */ ENMTHUNDER_SUBTYPE_SPIN_REGULAR,
    /* 2 */ ENMTHUNDER_SUBTYPE_SWORDBEAM_GREAT,
    /* 3 */ ENMTHUNDER_SUBTYPE_SWORDBEAM_REGULAR,
    /* 4 */ ENMTHUNDER_SUBTYPE_MAX
} EnMThunderSubType;

static u16 sChargingSfxIds[ENMTHUNDER_SUBTYPE_MAX] = {
    NA_SE_IT_ROLLING_CUT_LV2, // ENMTHUNDER_SUBTYPE_SPIN_GREAT
    NA_SE_IT_ROLLING_CUT_LV1, // ENMTHUNDER_SUBTYPE_SPIN_REGULAR
    NA_SE_IT_ROLLING_CUT_LV2, // ENMTHUNDER_SUBTYPE_SWORDBEAM_GREAT
    NA_SE_IT_ROLLING_CUT_LV1, // ENMTHUNDER_SUBTYPE_SWORDBEAM_REGULAR
};

static f32 sScales[] = { 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.25f, 0.2f, 0.15f, 0.0f };

void EnMThunder_UnkType_Setup(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.update = EnMThunder_UnkType_Update;
    this->isCharging = false;
    this->subtype = ENMTHUNDER_SUBTYPE_SPIN_REGULAR;
    this->scaleTarget = 2;
    this->actionFunc = EnMThunder_UnkType_Attack;
    this->timer = 8;
    this->lightColorFrac = 1.0f;
    AudioSfx_PlaySfx(NA_SE_IT_ROLLING_CUT_LV1, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    this->actor.child = NULL;
}

void EnMThunder_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMThunder* this = (EnMThunder*)thisx;
    Player* player = GET_PLAYER(play);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->type = ENMTHUNDER_GET_TYPE(&this->actor);
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 255, 255, 255, 0);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);

    if (this->type == ENMTHUNDER_TYPE_UNK) {
        EnMThunder_UnkType_Setup(this, play);
        return;
    }

    this->collider.dim.radius = 0;
    this->collider.dim.height = 40;
    this->collider.dim.yShift = -20;
    this->timer = 8;
    this->scroll = 0.0f;
    this->actor.world.pos = player->bodyPartsPos[PLAYER_BODYPART_WAIST];
    this->lightColorFrac = 0.0f;
    this->adjustLightsArg1 = 0.0f;
    this->actor.shape.rot.y = player->actor.shape.rot.y + 0x8000;
    this->actor.shape.rot.x = -this->actor.world.rot.x;
    this->actor.room = -1;
    Actor_SetScale(&this->actor, 0.1f);
    this->isCharging = false;

    if (player->stateFlags2 & PLAYER_STATE2_20000) {
        if (!gSaveContext.save.saveInfo.playerData.isMagicAcquired || (gSaveContext.magicState != MAGIC_STATE_IDLE) ||
            ((ENMTHUNDER_GET_MAGIC_COST(&this->actor) != 0) &&
             !Magic_Consume(play, ENMTHUNDER_GET_MAGIC_COST(&this->actor), MAGIC_CONSUME_NOW))) {
            AudioSfx_PlaySfx(NA_SE_IT_ROLLING_CUT, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            AudioSfx_PlaySfx(NA_SE_IT_SWORD_SWING_HARD, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            Actor_Kill(&this->actor);
            return;
        }

        player->stateFlags2 &= ~PLAYER_STATE2_20000;
        this->isCharging = false;

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK)) {
            player->unk_B08 = 1.0f;
            this->collider.elem.atDmgInfo.damage = sDamages[this->type + ENMTHUNDER_TYPE_MAX];
            this->subtype = ENMTHUNDER_SUBTYPE_SPIN_GREAT;
            if (this->type == ENMTHUNDER_TYPE_GREAT_FAIRYS_SWORD) {
                this->scaleTarget = 6;
            } else if (this->type == ENMTHUNDER_TYPE_GILDED_SWORD) {
                this->scaleTarget = 4;
            } else {
                this->scaleTarget = 3;
            }
        } else {
            player->unk_B08 = 0.5f;
            this->collider.elem.atDmgInfo.damage = sDamages[this->type];
            this->subtype = ENMTHUNDER_SUBTYPE_SPIN_REGULAR;
            if (this->type == ENMTHUNDER_TYPE_GREAT_FAIRYS_SWORD) {
                this->scaleTarget = 4;
            } else if (this->type == ENMTHUNDER_TYPE_GILDED_SWORD) {
                this->scaleTarget = 3;
            } else {
                this->scaleTarget = 2;
            }
        }

        if (player->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) {
            this->subtype += ENMTHUNDER_SUBTYPE_SWORDBEAM_GREAT;
            this->actionFunc = EnMThunder_SwordBeam_Attack;
            this->timer = 1;
            this->scaleTarget = 12;
            this->collider.elem.atDmgInfo.dmgFlags = DMG_SWORD_BEAM;
            this->collider.elem.atDmgInfo.damage = 3;
        } else {
            this->actionFunc = EnMThunder_Spin_Attack;
            this->timer = 8;
        }

        AudioSfx_PlaySfx(NA_SE_IT_ROLLING_CUT_LV1, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

        this->lightColorFrac = 1.0f;
    } else {
        this->actionFunc = EnMThunder_Charge;
    }

    this->actor.child = NULL;
}

void EnMThunder_Destroy(Actor* thisx, PlayState* play) {
    EnMThunder* this = (EnMThunder*)thisx;

    if (this->isCharging) {
        Magic_Reset(play);
    }

    Collider_DestroyCylinder(play, &this->collider);
    EnMThunder_AdjustLights(play, 0.0f);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

void EnMThunder_AdjustLights(PlayState* play, f32 arg1) {
    Environment_AdjustLights(play, arg1, 850.0f, 0.2f, 0.0f);
}

void EnMThunder_Spin_AttackNoMagic(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & PLAYER_STATE2_20000) {
        if (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) {
            AudioSfx_PlaySfx(NA_SE_IT_ROLLING_CUT, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            AudioSfx_PlaySfx(NA_SE_IT_SWORD_SWING_HARD, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
        Actor_Kill(&this->actor);
        return;
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK)) {
        Actor_Kill(&this->actor);
    }
}

void EnMThunder_Charge(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* child = this->actor.child;

    this->unk1B0 = player->unk_B08;
    this->actor.world.pos = player->bodyPartsPos[PLAYER_BODYPART_WAIST];
    this->actor.shape.rot.y = player->actor.shape.rot.y + 0x8000;

    if (!this->isCharging && (player->unk_B08 >= 0.1f)) {
        if ((gSaveContext.magicState != MAGIC_STATE_IDLE) ||
            ((ENMTHUNDER_GET_MAGIC_COST(&this->actor) != 0) &&
             !Magic_Consume(play, ENMTHUNDER_GET_MAGIC_COST(&this->actor), MAGIC_CONSUME_WAIT_PREVIEW))) {
            EnMThunder_Spin_AttackNoMagic(this, play);
            this->actionFunc = EnMThunder_Spin_AttackNoMagic;
            this->chargingAlpha = 0;
            this->adjustLightsArg1 = 0.0f;
            this->lightColorFrac = 0.0f;
            return;
        }
        this->isCharging = true;
    }

    if (player->unk_B08 >= 0.1f) {
        Rumble_Request(0.0f, (s32)(player->unk_B08 * 150.0f), 2, (s32)(player->unk_B08 * 150.0f));
    }

    if (player->stateFlags2 & PLAYER_STATE2_20000) {
        if ((child != NULL) && (child->update != NULL)) {
            child->parent = NULL;
        }

        if (player->unk_B08 <= 0.15f) {
            if ((player->unk_B08 >= 0.1f) && (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H)) {
                AudioSfx_PlaySfx(NA_SE_IT_ROLLING_CUT, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                AudioSfx_PlaySfx(NA_SE_IT_SWORD_SWING_HARD, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            }
            Actor_Kill(&this->actor);
            return;
        }

        player->stateFlags2 &= ~PLAYER_STATE2_20000;

        if (ENMTHUNDER_GET_MAGIC_COST(&this->actor) != 0) {
            gSaveContext.magicState = MAGIC_STATE_CONSUME_SETUP;
        }

        if (player->unk_B08 < 0.85f) {
            this->collider.elem.atDmgInfo.damage = sDamages[this->type];
            this->subtype = ENMTHUNDER_SUBTYPE_SPIN_REGULAR;
            if (this->type == ENMTHUNDER_TYPE_GREAT_FAIRYS_SWORD) {
                this->scaleTarget = 4;
            } else if (this->type == ENMTHUNDER_TYPE_GILDED_SWORD) {
                this->scaleTarget = 3;
            } else {
                this->scaleTarget = 2;
            }
        } else {
            this->collider.elem.atDmgInfo.damage = sDamages[this->type + ENMTHUNDER_TYPE_MAX];
            this->subtype = ENMTHUNDER_SUBTYPE_SPIN_GREAT;
            if (this->type == ENMTHUNDER_TYPE_GREAT_FAIRYS_SWORD) {
                this->scaleTarget = 6;
            } else if (this->type == ENMTHUNDER_TYPE_GILDED_SWORD) {
                this->scaleTarget = 4;
            } else {
                this->scaleTarget = 3;
            }
        }

        if (player->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) {
            this->subtype += ENMTHUNDER_SUBTYPE_SWORDBEAM_GREAT;
            this->actionFunc = EnMThunder_SwordBeam_Attack;
            this->timer = 1;
        } else {
            this->actionFunc = EnMThunder_Spin_Attack;
            this->timer = 8;
        }

        AudioSfx_PlaySfx(sChargingSfxIds[this->subtype], &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

        this->lightColorFrac = 1.0f;

        return;
    }

    if (!(player->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK)) {
        if (this->actor.child != NULL) {
            this->actor.child->parent = NULL;
        }
        Actor_Kill(&this->actor);
        return;
    }

    if (player->unk_B08 > 0.15f) {
        this->chargingAlpha = 255;
        if (this->actor.child == NULL) {
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EFF_DUST, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                               EFF_DUST_TYPE_SPIN_ATTACK_CHARGE);
        }
        this->adjustLightsArg1 += (((player->unk_B08 - 0.15f) * 1.5f) - this->adjustLightsArg1) * 0.5f;
    } else if (player->unk_B08 > .1f) {
        this->chargingAlpha = (s32)((player->unk_B08 - .1f) * 255.0f * 20.0f);
        this->lightColorFrac = (player->unk_B08 - .1f) * 10.0f;
    } else {
        this->chargingAlpha = 0;
    }

    if (player->unk_B08 > 0.85f) {
        Audio_PlaySfx_SwordCharge(&player->actor.projectedPos, 2);
    } else if (player->unk_B08 > 0.15f) {
        Audio_PlaySfx_SwordCharge(&player->actor.projectedPos, 1);
    } else if (player->unk_B08 > 0.1f) {
        Audio_PlaySfx_SwordCharge(&player->actor.projectedPos, 0);
    }

    if (Play_InCsMode(play)) {
        Actor_Kill(&this->actor);
    }
}

void func_808B5EEC(EnMThunder* this, PlayState* play) {
    if (this->timer < 2) {
        if (this->chargingAlpha < 40) {
            this->chargingAlpha = 0;
        } else {
            this->chargingAlpha -= 40;
        }
    }

    this->scroll += 2.0f * this->alphaFrac;

    if (this->adjustLightsArg1 < this->lightColorFrac) {
        this->adjustLightsArg1 = F32_LERPIMP(this->adjustLightsArg1, this->lightColorFrac, 0.1f);
    } else {
        this->adjustLightsArg1 = this->lightColorFrac;
    }
}

void EnMThunder_Spin_Attack(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Math_StepToF(&this->lightColorFrac, 0.0f, 0.0625f)) {
        Actor_Kill(&this->actor);
    } else {
        Math_SmoothStepToF(&this->actor.scale.x, (s32)this->scaleTarget, 0.6f, 0.8f, 0.0f);
        Actor_SetScale(&this->actor, this->actor.scale.x);
        this->collider.dim.radius = this->actor.scale.x * 30.0f;
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->timer > 0) {
        this->actor.world.pos.x = player->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
        this->actor.world.pos.z = player->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
        this->timer--;
    }

    if (this->lightColorFrac > (6.0f / 10.0f)) {
        this->alphaFrac = 1.0f;
    } else {
        this->alphaFrac = this->lightColorFrac * (10.0f / 6.0f);
    }

    func_808B5EEC(this, play);

    if (Play_InCsMode(play)) {
        Actor_Kill(&this->actor);
    }
}

void EnMThunder_SwordBeam_Attack(EnMThunder* this, PlayState* play) {
    s32 pad[2];
    f32 sp2C;

    if (this->lightColorFrac > (9.0f / 10.0f)) {
        this->alphaFrac = 1.0f;
    } else {
        this->alphaFrac = this->lightColorFrac * (10.0f / 9.0f);
    }

    if (Math_StepToF(&this->lightColorFrac, 0.0f, 0.05f)) {
        Actor_Kill(&this->actor);
    } else {
        sp2C = -80.0f * Math_CosS(this->actor.world.rot.x);

        this->actor.world.pos.x += sp2C * Math_SinS(this->actor.shape.rot.y);
        this->actor.world.pos.z += sp2C * Math_CosS(this->actor.shape.rot.y);
        this->actor.world.pos.y += -80.0f * Math_SinS(this->actor.world.rot.x);

        Math_SmoothStepToF(&this->actor.scale.x, this->scaleTarget, 0.6f, 2.0f, 0.0f);
        Actor_SetScale(&this->actor, this->actor.scale.x);

        this->collider.dim.radius = this->actor.scale.x * 5.0f;

        this->collider.dim.pos.x = this->actor.world.pos.x;
        this->collider.dim.pos.y = this->actor.world.pos.y;
        this->collider.dim.pos.z = this->actor.world.pos.z;

        this->collider.dim.pos.x =
            (Math_SinS(this->actor.shape.rot.y) * -5.0f * this->actor.scale.x) + this->actor.world.pos.x;
        this->collider.dim.pos.y = this->actor.world.pos.y;
        this->collider.dim.pos.z =
            (Math_CosS(this->actor.shape.rot.y) * -5.0f * this->actor.scale.z) + this->actor.world.pos.z;

        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->timer > 0) {
        this->timer--;
    }

    func_808B5EEC(this, play);
}

void EnMThunder_UnkType_Attack(EnMThunder* this, PlayState* play) {
    if (Math_StepToF(&this->lightColorFrac, 0.0f, 0.0625f)) {
        Actor_Kill(&this->actor);
    } else {
        Math_SmoothStepToF(&this->actor.scale.x, (s32)this->scaleTarget, 0.6f, 0.8f, 0.0f);
        Actor_SetScale(&this->actor, this->actor.scale.x);
    }

    if (this->lightColorFrac > (6.0f / 10.0f)) {
        this->alphaFrac = 1.0f;
    } else {
        this->alphaFrac = this->lightColorFrac * (10.0f / 6.0f);
    }

    func_808B5EEC(this, play);
}

void EnMThunder_Update(Actor* thisx, PlayState* play) {
    EnMThunder* this = (EnMThunder*)thisx;

    this->actionFunc(this, play);
    EnMThunder_AdjustLights(play, this->adjustLightsArg1);
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, this->lightColorFrac * 255.0f, this->lightColorFrac * 255.0f,
                              this->lightColorFrac * 100.0f, this->lightColorFrac * 800.0f);
}

void EnMThunder_UnkType_Update(Actor* thisx, PlayState* play) {
    EnMThunder* this = (EnMThunder*)thisx;

    this->actionFunc(this, play);
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, this->lightColorFrac * 255.0f, this->lightColorFrac * 255.0f,
                              this->lightColorFrac * 100.0f, this->lightColorFrac * 800.0f);
}

void EnMThunder_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnMThunder* this = (EnMThunder*)thisx;
    Player* player = GET_PLAYER(play);
    f32 scale;
    s32 y2Scroll;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Matrix_Scale(0.02f, 0.02f, 0.02f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    switch (this->subtype) {
        case ENMTHUNDER_SUBTYPE_SPIN_GREAT:
        case ENMTHUNDER_SUBTYPE_SPIN_REGULAR:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0xFF - ((u16)(s32)(this->scroll * 30.0f) & 0xFF), 0,
                                          64, 32, 1, 0xFF - ((u16)(s32)(this->scroll * 20.0f) & 0xFF), 0, 8, 8, -30, 0,
                                          -20, 0));
            break;

        case ENMTHUNDER_SUBTYPE_SWORDBEAM_GREAT:
        case ENMTHUNDER_SUBTYPE_SWORDBEAM_REGULAR:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 16, 64, 1, 0,
                                          0x1FF - ((u16)(s32)(this->scroll * 10.0f) & 0x1FF), 32, 128, 0, 0, 0, -10));
            break;

        default:
            break;
    }

    switch (this->subtype) {
        case ENMTHUNDER_SUBTYPE_SPIN_GREAT:
            gDPSetPrimColorOverride(POLY_XLU_DISP++, 0, 0x80, 255, 255, 170, (u16)(this->alphaFrac * 255.0f),
                                    COSMETIC_ID("Effects.GreatSpinBurst"));
            gSPDisplayList(POLY_XLU_DISP++, gGreatSpinAttackDiskDL);
            gSPDisplayList(POLY_XLU_DISP++, gGreatSpinAttackCylinderDL);
            break;

        case ENMTHUNDER_SUBTYPE_SPIN_REGULAR:
            gDPSetPrimColorOverride(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, (u16)(this->alphaFrac * 255.0f),
                                    COSMETIC_ID("Effects.SpinSlashBurst"));
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttackDiskDL);
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttackCylinderDL);
            break;

        case ENMTHUNDER_SUBTYPE_SWORDBEAM_REGULAR:
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, (u16)(this->alphaFrac * 255.0f));
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, 128);
            gSPDisplayList(POLY_XLU_DISP++, gSwordBeamDL);
            break;

        case ENMTHUNDER_SUBTYPE_SWORDBEAM_GREAT:
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 0, 255, 255, (u16)(this->alphaFrac * 255.0f));
            gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 128);
            gSPDisplayList(POLY_XLU_DISP++, gSwordBeamDL);
            break;

        default:
            break;
    }

    Matrix_Mult(&player->leftHandMf, MTXMODE_NEW);

    if (GameInteractor_Should(VB_TRANSFORM_THUNDER_MATRIX, true, this)) {
        if (this->type == ENMTHUNDER_TYPE_GILDED_SWORD) {
            Matrix_Translate(0.0f, 220.0f, 0.0f, MTXMODE_APPLY);
            Matrix_Scale(-1.2f, -0.8f, -0.6f, MTXMODE_APPLY);
            Matrix_RotateXS(0x4000, MTXMODE_APPLY);
        } else {
            Matrix_Translate(0.0f, 220.0f, 0.0f, MTXMODE_APPLY);
            Matrix_Scale(-0.7f, -0.6f, -0.4f, MTXMODE_APPLY);
            Matrix_RotateXS(0x4000, MTXMODE_APPLY);
        }
    }

    if (this->unk1B0 >= 0.85f) {
        scale = (sScales[play->gameplayFrames & 7] * 6.0f) + 1.0f;
        gDPSetPrimColorOverride(POLY_XLU_DISP++, 0, 0x80, 255, 255, 170, this->chargingAlpha,
                                COSMETIC_ID("Effects.GreatSpinCharge"));
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 100, 0, 128);
        y2Scroll = 40;
    } else {
        scale = (sScales[play->gameplayFrames & 7] * 2.0f) + 1.0f;
        gDPSetPrimColorOverride(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, this->chargingAlpha,
                                COSMETIC_ID("Effects.SpinSlashCharge"));
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, 128);
        y2Scroll = 20;
    }

    Matrix_Scale(1.0f, scale, scale, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x09,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (play->gameplayFrames * 5) & 0xFF, 0, 32, 32, 1,
                                  (play->gameplayFrames * 20) & 0xFF, (play->gameplayFrames * y2Scroll) & 0xFF, 8, 8, 5,
                                  0, 20, y2Scroll));
    gSPDisplayList(POLY_XLU_DISP++, gSpinAttackChargingDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
