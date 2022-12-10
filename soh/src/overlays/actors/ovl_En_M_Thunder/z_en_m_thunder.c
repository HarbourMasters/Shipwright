#include "z_en_m_thunder.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS 0

void EnMThunder_Init(Actor* thisx, PlayState* play);
void EnMThunder_Destroy(Actor* thisx, PlayState* play);
void EnMThunder_Update(Actor* thisx, PlayState* play);
void EnMThunder_Draw(Actor* thisx, PlayState* play);

void func_80A9F314(PlayState* play, f32 arg1);
void func_80A9F408(EnMThunder* this, PlayState* play);
void func_80A9F9B4(EnMThunder* this, PlayState* play);

const ActorInit En_M_Thunder_InitVars = {
    ACTOR_EN_M_THUNDER,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnMThunder),
    (ActorFunc)EnMThunder_Init,
    (ActorFunc)EnMThunder_Destroy,
    (ActorFunc)EnMThunder_Update,
    (ActorFunc)EnMThunder_Draw,
    NULL,
};

static ColliderCylinderInit D_80AA0420 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000001, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 200, 200, 0, { 0, 0, 0 } },
};

static u32 D_80AA044C[] = { 0x01000000, 0x00400000, 0x00800000 };
static u32 D_80AA0458[] = { 0x08000000, 0x02000000, 0x04000000 };

static u16 sSfxIds[] = {
    NA_SE_IT_ROLLING_CUT_LV2,
    NA_SE_IT_ROLLING_CUT_LV1,
    NA_SE_IT_ROLLING_CUT_LV2,
    NA_SE_IT_ROLLING_CUT_LV1,
};

// Setup action
void func_80A9EFE0(EnMThunder* this, EnMThunderActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnMThunder_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnMThunder* this = (EnMThunder*)thisx;
    Player* player = GET_PLAYER(play);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &D_80AA0420);
    this->unk_1C7 = (this->actor.params & 0xFF) - 1;
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 255, 255, 255, 0);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    this->collider.dim.radius = 0;
    this->collider.dim.height = 40;
    this->collider.dim.yShift = -20;
    this->unk_1C4 = 8;
    this->unk_1B4 = 0.0f;
    this->actor.world.pos = player->bodyPartsPos[0];
    this->unk_1AC = 0.0f;
    this->unk_1BC = 0.0f;
    this->actor.shape.rot.y = player->actor.shape.rot.y + 0x8000;
    this->actor.room = -1;
    Actor_SetScale(&this->actor, 0.1f);
    this->unk_1CA = 0;

    if (player->stateFlags2 & 0x20000) {
        if (!gSaveContext.isMagicAcquired || gSaveContext.magicState ||
            (((this->actor.params & 0xFF00) >> 8) &&
             !(func_80087708(play, (this->actor.params & 0xFF00) >> 8, 0)))) {
            Audio_PlaySoundGeneral(NA_SE_IT_ROLLING_CUT, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING_HARD, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            Actor_Kill(&this->actor);
            return;
        }

        player->stateFlags2 &= ~0x20000;
        this->unk_1CA = 1;
        this->collider.info.toucher.dmgFlags = D_80AA044C[this->unk_1C7];
        this->unk_1C6 = 1;
        this->unk_1C9 = ((this->unk_1C7 == 1) ? 2 : 4);
        func_80A9EFE0(this, func_80A9F9B4);
        this->unk_1C4 = 8;
        Audio_PlaySoundGeneral(NA_SE_IT_ROLLING_CUT_LV1, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        this->unk_1AC = 1.0f;
    } else {
        func_80A9EFE0(this, func_80A9F408);
    }
    this->actor.child = NULL;
}

void EnMThunder_Destroy(Actor* thisx, PlayState* play) {
    EnMThunder* this = (EnMThunder*)thisx;

    if (this->unk_1CA != 0) {
        func_800876C8(play);
    }

    Collider_DestroyCylinder(play, &this->collider);
    func_80A9F314(play, 0.0f);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

void func_80A9F314(PlayState* play, f32 arg1) {
    Environment_AdjustLights(play, arg1, 850.0f, 0.2f, 0.0f);
}

void func_80A9F350(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & 0x20000) {
        if (player->meleeWeaponAnimation >= 0x18) {
            Audio_PlaySoundGeneral(NA_SE_IT_ROLLING_CUT, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING_HARD, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }

        Actor_Kill(&this->actor);
        return;
    }

    if (!(player->stateFlags1 & 0x1000)) {
        Actor_Kill(&this->actor);
    }
}

void func_80A9F408(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* child = this->actor.child;

    this->unk_1B8 = player->unk_858;
    this->actor.world.pos = player->bodyPartsPos[0];
    this->actor.shape.rot.y = player->actor.shape.rot.y + 0x8000;

    if (this->unk_1CA == 0) {
        if (player->unk_858 >= 0.1f) {
            if ((gSaveContext.magicState) || (((this->actor.params & 0xFF00) >> 8) &&
                                            !(func_80087708(play, (this->actor.params & 0xFF00) >> 8, 4)))) {
                func_80A9F350(this, play);
                func_80A9EFE0(this, func_80A9F350);
                this->unk_1C8 = 0;
                this->unk_1BC = 0.0;
                this->unk_1AC = 0.0f;
                return;
            }

            this->unk_1CA = 1;
        }
    }

    if (player->unk_858 >= 0.1f) {
        func_800AA000(0.0f, (s32)(player->unk_858 * 150.0f) & 0xFF, 2, (s32)(player->unk_858 * 150.0f) & 0xFF);
    }

    if (player->stateFlags2 & 0x20000) {
        if ((child != NULL) && (child->update != NULL)) {
            child->parent = NULL;
        }

        if (player->unk_858 <= 0.15f) {
            if ((player->unk_858 >= 0.1f) && (player->meleeWeaponAnimation >= 0x18)) {
                Audio_PlaySoundGeneral(NA_SE_IT_ROLLING_CUT, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_SWING_HARD, &player->actor.projectedPos, 4, &D_801333E0,
                                       &D_801333E0, &D_801333E8);
            }
            Actor_Kill(&this->actor);
            return;
        } else {
            player->stateFlags2 &= ~0x20000;
            if ((this->actor.params & 0xFF00) >> 8) {
                gSaveContext.magicState = 1;
            }
            if (player->unk_858 < 0.85f) {
                this->collider.info.toucher.dmgFlags = D_80AA044C[this->unk_1C7];
                this->unk_1C6 = 1;
                this->unk_1C9 = ((this->unk_1C7 == 1) ? 2 : 4);
            } else {
                this->collider.info.toucher.dmgFlags = D_80AA0458[this->unk_1C7];
                this->unk_1C6 = 0;
                this->unk_1C9 = ((this->unk_1C7 == 1) ? 4 : 8);
            }

            func_80A9EFE0(this, func_80A9F9B4);
            this->unk_1C4 = 8;
            Audio_PlaySoundGeneral(sSfxIds[this->unk_1C6], &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            this->unk_1AC = 1.0f;
            return;
        }
    }

    if (!(player->stateFlags1 & 0x1000)) {
        if (this->actor.child != NULL) {
            this->actor.child->parent = NULL;
        }
        Actor_Kill(&this->actor);
        return;
    }

    if (player->unk_858 > 0.15f) {
        this->unk_1C8 = 255;
        if (this->actor.child == NULL) {
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EFF_DUST, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                               this->unk_1C7 + 2);
        }
        this->unk_1BC += ((((player->unk_858 - 0.15f) * 1.5f) - this->unk_1BC) * 0.5f);

    } else if (player->unk_858 > .1f) {
        this->unk_1C8 = (s32)((player->unk_858 - .1f) * 255.0f * 20.0f);
        this->unk_1AC = (player->unk_858 - .1f) * 10.0f;
    } else {
        this->unk_1C8 = 0;
    }

    if (player->unk_858 > 0.85f) {
        func_800F4254(&player->actor.projectedPos, 2);
    } else if (player->unk_858 > 0.15f) {
        func_800F4254(&player->actor.projectedPos, 1);
    } else if (player->unk_858 > 0.1f) {
        func_800F4254(&player->actor.projectedPos, 0);
    }

    if (Play_InCsMode(play)) {
        Actor_Kill(&this->actor);
    }
}

void func_80A9F938(EnMThunder* this, PlayState* play) {
    if (this->unk_1C4 < 2) {
        if (this->unk_1C8 < 40) {
            this->unk_1C8 = 0;
        } else {
            this->unk_1C8 -= 40;
        }
    }

    this->unk_1B4 += 2.0f * this->unk_1B0;

    if (this->unk_1BC < this->unk_1AC) {
        this->unk_1BC += ((this->unk_1AC - this->unk_1BC) * 0.1f);
    } else {
        this->unk_1BC = this->unk_1AC;
    }
}

void func_80A9F9B4(EnMThunder* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Math_StepToF(&this->unk_1AC, 0.0f, 1 / 16.0f)) {
        Actor_Kill(&this->actor);
    } else {
        Math_SmoothStepToF(&this->actor.scale.x, (s32)this->unk_1C9, 0.6f, 0.8f, 0.0f);
        Actor_SetScale(&this->actor, this->actor.scale.x);
        this->collider.dim.radius = (this->actor.scale.x * 25.0f);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->unk_1C4 > 0) {
        this->actor.world.pos.x = player->bodyPartsPos[0].x;
        this->actor.world.pos.z = player->bodyPartsPos[0].z;
        this->unk_1C4--;
    }

    if (this->unk_1AC > 0.6f) {
        this->unk_1B0 = 1.0f;
    } else {
        this->unk_1B0 = this->unk_1AC * (5.0f / 3.0f);
    }

    func_80A9F938(this, play);

    if (Play_InCsMode(play)) {
        Actor_Kill(&this->actor);
    }
}

void EnMThunder_Update(Actor* thisx, PlayState* play) {
    EnMThunder* this = (EnMThunder*)thisx;
    f32 blueRadius;
    s32 redGreen;

    this->actionFunc(this, play);
    func_80A9F314(play, this->unk_1BC);
    blueRadius = this->unk_1AC;
    redGreen = (u32)(blueRadius * 255.0f) & 0xFF;
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, redGreen, redGreen, (u32)(blueRadius * 100.0f),
                              (s32)(blueRadius * 800.0f));
}

void EnMThunder_Draw(Actor* thisx, PlayState* play2) {
    static f32 D_80AA046C[] = { 0.1f, 0.15f, 0.2f, 0.25f, 0.3f, 0.25f, 0.2f, 0.15f };
    PlayState* play = play2;
    EnMThunder* this = (EnMThunder*)thisx;
    Player* player = GET_PLAYER(play);
    f32 phi_f14;
    s32 phi_t1;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    Matrix_Scale(0.02f, 0.02f, 0.02f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    switch (this->unk_1C6) {
        case 0:
        case 1:
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0xFF - ((u8)(s32)(this->unk_1B4 * 30) & 0xFF), 0,
                                        0x40, 0x20, 1, 0xFF - ((u8)(s32)(this->unk_1B4 * 20) & 0xFF), 0, 8, 8));
            break;
    }

    switch (this->unk_1C6) {
        case 0:
            if (CVar_GetS32("gCosmetics.SpinAttack_Level2Primary.Changed", 0)) {
                Color_RGB8 color = CVar_GetRGB("gCosmetics.SpinAttack_Level2Primary.Value", (Color_RGB8){255, 255, 170});
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, color.r, color.g, color.b, (u8)(this->unk_1B0 * 255));
            } else {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 170, (u8)(this->unk_1B0 * 255));
            }
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttack3DL);
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttack4DL);
            break;
        case 1:
             if (CVar_GetS32("gCosmetics.SpinAttack_Level1Primary.Changed", 0)) {
                Color_RGB8 color = CVar_GetRGB("gCosmetics.SpinAttack_Level1Primary.Value", (Color_RGB8){170, 255, 255});
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, color.r, color.g, color.b, (u8)(this->unk_1B0 * 255));
            } else {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, (u8)(this->unk_1B0 * 255));
            }
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttack1DL);
            gSPDisplayList(POLY_XLU_DISP++, gSpinAttack2DL);
            break;
    }

    Matrix_Mult(&player->mf_9E0, MTXMODE_NEW);

    switch (this->unk_1C7) {
        case 1:
            Matrix_Translate(0.0f, 220.0f, 0.0f, MTXMODE_APPLY);
            Matrix_Scale(-0.7f, -0.6f, -0.4f, MTXMODE_APPLY);
            Matrix_RotateX(16384.0f, MTXMODE_APPLY);
            break;
        case 0:
            Matrix_Translate(0.0f, 300.0f, -100.0f, MTXMODE_APPLY);
            Matrix_Scale(-1.2f, -1.0f, -0.7f, MTXMODE_APPLY);
            Matrix_RotateX(16384.0f, MTXMODE_APPLY);
            break;
        case 2:
            Matrix_Translate(200.0f, 350.0f, 0.0f, MTXMODE_APPLY);
            Matrix_Scale(-1.8f, -1.4f, -0.7f, MTXMODE_APPLY);
            Matrix_RotateX(16384.0f, MTXMODE_APPLY);
            break;
    }

    if (this->unk_1B8 >= 0.85f) {
        phi_f14 = (D_80AA046C[(play->gameplayFrames & 7)] * 6.0f) + 1.0f;
        if (CVar_GetS32("gCosmetics.SpinAttack_Level2Primary.Changed", 0)) {
            Color_RGB8 color = CVar_GetRGB("gCosmetics.SpinAttack_Level2Primary.Value", (Color_RGB8){255, 255, 170});
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, color.r, color.g, color.b, this->unk_1C8);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 170, this->unk_1C8);
        }
        if (CVar_GetS32("gCosmetics.SpinAttack_Level2Secondary.Changed", 0)) {
            Color_RGB8 color = CVar_GetRGB("gCosmetics.SpinAttack_Level2Secondary.Value", (Color_RGB8){255, 100, 0});
            gDPSetEnvColor(POLY_XLU_DISP++, color.r, color.g, color.b, 128);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 100, 0, 128);
        }
        phi_t1 = 0x28;
    } else {
        phi_f14 = (D_80AA046C[play->gameplayFrames & 7] * 2.0f) + 1.0f;
        if (CVar_GetS32("gCosmetics.SpinAttack_Level1Primary.Changed", 0)) {
            Color_RGB8 color = CVar_GetRGB("gCosmetics.SpinAttack_Level1Primary.Value", (Color_RGB8){170, 255, 255});
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, color.r, color.g, color.b, this->unk_1C8);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, this->unk_1C8);
        }
        if (CVar_GetS32("gCosmetics.SpinAttack_Level1Secondary.Changed", 0)) {
            Color_RGB8 color = CVar_GetRGB("gCosmetics.SpinAttack_Level1Secondary.Value", (Color_RGB8){0, 100, 255});
            gDPSetEnvColor(POLY_XLU_DISP++, color.r, color.g, color.b, 128);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, 128);
        }
        phi_t1 = 0x14;
    }
    Matrix_Scale(1.0f, phi_f14, phi_f14, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPSegment(POLY_XLU_DISP++, 0x09,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, (play->gameplayFrames * 5) & 0xFF, 0, 0x20, 0x20, 1,
                                (play->gameplayFrames * 20) & 0xFF, (play->gameplayFrames * phi_t1) & 0xFF, 8,
                                8));

    gSPDisplayList(POLY_XLU_DISP++, gSpinAttackChargingDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
