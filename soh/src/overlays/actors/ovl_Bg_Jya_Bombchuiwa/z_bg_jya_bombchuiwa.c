#include "z_bg_jya_bombchuiwa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/object_jya_obj/object_jya_obj.h"
#define FLAGS ACTOR_FLAG_0

void BgJyaBombchuiwa_Init(Actor* thisx, PlayState* play);
void BgJyaBombchuiwa_Destroy(Actor* thisx, PlayState* play);
void BgJyaBombchuiwa_Update(Actor* thisx, PlayState* play);
void BgJyaBombchuiwa_Draw(Actor* thisx, PlayState* play);

void BgJyaBombchuiwa_WaitForExplosion(BgJyaBombchuiwa* this, PlayState* play);
void BgJyaBombchuiwa_SetupWaitForExplosion(BgJyaBombchuiwa* this, PlayState* play);
void func_808949B8(BgJyaBombchuiwa* this, PlayState* play);
void BgJyaBombchuiwa_CleanUpAfterExplosion(BgJyaBombchuiwa* this, PlayState* play);
void BgJyaBombchuiwa_SpawnLightRay(BgJyaBombchuiwa* this, PlayState* play);

const ActorInit Bg_Jya_Bombchuiwa_InitVars = {
    ACTOR_BG_JYA_BOMBCHUIWA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaBombchuiwa),
    (ActorFunc)BgJyaBombchuiwa_Init,
    (ActorFunc)BgJyaBombchuiwa_Destroy,
    (ActorFunc)BgJyaBombchuiwa_Update,
    (ActorFunc)BgJyaBombchuiwa_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { -300, 0, 0 }, 40 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 3, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgJyaBombchuiwa_SetupCollider(BgJyaBombchuiwa* this, PlayState* play) {
    s32 pad;

    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, &this->colliderItems);
}

void BgJyaBombchuiwa_SetDrawFlags(BgJyaBombchuiwa* this, u8 drawFlags) {
    this->drawFlags &= ~7;
    this->drawFlags |= drawFlags;
}

void BgJyaBombchuiwa_Init(Actor* thisx, PlayState* play) {
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    BgJyaBombchuiwa_SetupCollider(this, play);
    if (Flags_GetSwitch(play, this->actor.params & 0x3F)) {
        BgJyaBombchuiwa_SpawnLightRay(this, play);
    } else {
        BgJyaBombchuiwa_SetupWaitForExplosion(this, play);
    }
    Actor_SetFocus(&this->actor, 0.0f);
}

void BgJyaBombchuiwa_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void BgJyaBombchuiwa_Break(BgJyaBombchuiwa* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    s16 scale;
    s16 arg5;
    s16 arg6;
    s16 arg7;
    s32 i;

    for (i = 0; i < 20; i++) {
        pos.x = Rand_ZeroOne() * 10.0f + this->actor.world.pos.x - 10.0f;
        pos.y = Rand_ZeroOne() * 40.0f + this->actor.world.pos.y - 20.0f;
        pos.z = Rand_ZeroOne() * 50.0f + this->actor.world.pos.z - 25.0f;
        velocity.x = Rand_ZeroOne() * 3.0f - 0.3f;
        velocity.y = Rand_ZeroOne() * 18.0f;
        velocity.z = (Rand_ZeroOne() - 0.5f) * 15.0f;
        scale = (s32)(Rand_ZeroOne() * 20.0f) + 1;
        if (scale > 10) {
            arg5 = 5;
        } else {
            arg5 = 1;
        }
        if (Rand_ZeroOne() < 0.4f) {
            arg5 |= 0x40;
            arg6 = 0xC;
            arg7 = 8;
        } else {
            arg5 |= 0x20;
            arg6 = 0xC;
            arg7 = 8;
            if (scale < 8) {
                arg6 = 0x46;
                arg7 = 0x28;
            }
        }
        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -300, arg5, arg6, arg7, 0, scale, 1, 15, 80,
                             KAKERA_COLOR_NONE, OBJECT_JYA_OBJ, gBombiwaEffectDL);
    }
    func_80033480(play, &this->actor.world.pos, 100.0f, 8, 100, 160, 0);
}

void BgJyaBombchuiwa_SetupWaitForExplosion(BgJyaBombchuiwa* this, PlayState* play) {
    this->actionFunc = BgJyaBombchuiwa_WaitForExplosion;
    BgJyaBombchuiwa_SetDrawFlags(this, 3);
    this->timer = 0;
}

void BgJyaBombchuiwa_WaitForExplosion(BgJyaBombchuiwa* this, PlayState* play) {
    if ((this->collider.base.acFlags & AC_HIT) || (this->timer > 0)) {
        if (this->timer == 0) {
            OnePointCutscene_Init(play, 3410, -99, &this->actor, MAIN_CAM);
        }
        this->timer++;
        if (this->timer > 10) {
            BgJyaBombchuiwa_Break(this, play);
            BgJyaBombchuiwa_CleanUpAfterExplosion(this, play);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
        }
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void BgJyaBombchuiwa_CleanUpAfterExplosion(BgJyaBombchuiwa* this, PlayState* play) {
    this->actionFunc = func_808949B8;
    BgJyaBombchuiwa_SetDrawFlags(this, 4);
    this->lightRayIntensity = 0.3f;
    this->timer = 0;
    this->actor.flags &= ~ACTOR_FLAG_0;
}

void func_808949B8(BgJyaBombchuiwa* this, PlayState* play) {
    this->timer++;
    if (this->timer & 4) {
        func_80033480(play, &this->actor.world.pos, 60.0f, 3, 100, 100, 0);
    }
    if (Math_StepToF(&this->lightRayIntensity, 1.0f, 0.028)) {
        BgJyaBombchuiwa_SpawnLightRay(this, play);
    }
}

void BgJyaBombchuiwa_SpawnLightRay(BgJyaBombchuiwa* this, PlayState* play) {
    this->actionFunc = NULL;
    this->lightRayIntensity = 153.0f;
    BgJyaBombchuiwa_SetDrawFlags(this, 4);
    if (Actor_Spawn(&play->actorCtx, play, ACTOR_MIR_RAY, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 0) == NULL) {
        // "Occurrence failure"
        osSyncPrintf("Ｅｒｒｏｒ : Mir_Ray 発生失敗(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->actor.params);
    }
}

void BgJyaBombchuiwa_Update(Actor* thisx, PlayState* play) {
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void BgJyaBombchuiwa_DrawRock(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gBombchuiwa2DL);
    CLOSE_DISPS(play->state.gfxCtx);
}

void BgJyaBombchuiwa_DrawLight(Actor* thisx, PlayState* play) {
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, CLAMP_MAX((u32)(this->lightRayIntensity * 153.0f), 153));
    gSPDisplayList(POLY_XLU_DISP++, gBombchuiwaLight1DL);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, CLAMP_MAX((u32)(this->lightRayIntensity * 255.0f), 255));
    gSPDisplayList(POLY_XLU_DISP++, gBombchuiwaLight2DL);
    CLOSE_DISPS(play->state.gfxCtx);
}

void BgJyaBombchuiwa_Draw(Actor* thisx, PlayState* play) {
    static Vec3f D_80894F88 = { -920.0f, 480.0f, -889.0f };
    static Vec3s D_80894F94 = { 0, 0, 0 };
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    if (this->drawFlags & 1) {
        Gfx_DrawDListOpa(play, gBombchuiwaDL);
        Collider_UpdateSpheres(0, &this->collider);
    }

    if (this->drawFlags & 2) {
        BgJyaBombchuiwa_DrawRock(play);
    }
    if (this->drawFlags & 4) {
        Matrix_SetTranslateRotateYXZ(D_80894F88.x, D_80894F88.y, D_80894F88.z, &D_80894F94);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        if (this->drawFlags & 4) {
            BgJyaBombchuiwa_DrawLight(thisx, play);
        }
    }
}
