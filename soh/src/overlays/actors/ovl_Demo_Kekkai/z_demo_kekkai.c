/**
 * File: z_demo_kekkai.c
 * Overlay: ovl_Demo_Kekkai
 * Description: Ganon's castle barriers
 */

#include "z_demo_kekkai.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"
#include "scenes/dungeons/ganontika/ganontika_scene.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void DemoKekkai_Init(Actor* thisx, PlayState* play);
void DemoKekkai_Destroy(Actor* thisx, PlayState* play);
void DemoKekkai_Update(Actor* thisx, PlayState* play);
void DemoKekkai_DrawTowerBarrier(Actor* thisx, PlayState* play);
void DemoKekkai_Reset(void);

void DemoKekkai_TrialBarrierDispel(Actor* thisx, PlayState* play);
void DemoKekkai_TrialBarrierIdle(Actor* thisx, PlayState* play);
void DemoKekkai_DrawTrialBarrier(Actor* thisx, PlayState* play);

void DemoKekkai_TowerBarrier(DemoKekkai* this, PlayState* play);

const ActorInit Demo_Kekkai_InitVars = {
    ACTOR_DEMO_KEKKAI,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_DEMO_KEKKAI,
    sizeof(DemoKekkai),
    (ActorFunc)DemoKekkai_Init,
    (ActorFunc)DemoKekkai_Destroy,
    (ActorFunc)DemoKekkai_Update,
    (ActorFunc)DemoKekkai_DrawTowerBarrier,
    (ActorResetFunc)DemoKekkai_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x07, 0x04 },
        { 0x00002000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 680, 220, 120, { 0, 0, 0 } },
};

static u8 sEnergyColors[] = {
    /* Water   prim */ 170, 255, 255, /* env */ 0,   50,  255,
    /* Light   prim */ 255, 255, 170, /* env */ 200, 255, 0,
    /* Fire    prim */ 255, 255, 170, /* env */ 200, 0,   0,
    /* Shadow  prim */ 255, 170, 255, /* env */ 100, 0,   200,
    /* Spirit  prim */ 255, 255, 170, /* env */ 255, 120, 0,
    /* Forest  prim */ 255, 255, 170, /* env */ 0,   200, 0,
};

// Translates from the barrier's actor params to their corresponding randInf flags.
RandomizerInf trialParamToRandInf(u16 params) {
    switch (params) {
        case KEKKAI_LIGHT:
            return RAND_INF_TRIALS_DONE_LIGHT_TRIAL;
        case KEKKAI_FOREST:
            return RAND_INF_TRIALS_DONE_FOREST_TRIAL;
        case KEKKAI_FIRE:
            return RAND_INF_TRIALS_DONE_FIRE_TRIAL;
        case KEKKAI_WATER:
            return RAND_INF_TRIALS_DONE_WATER_TRIAL;
        case KEKKAI_SPIRIT:
            return RAND_INF_TRIALS_DONE_SPIRIT_TRIAL;
        case KEKKAI_SHADOW:
            return RAND_INF_TRIALS_DONE_SHADOW_TRIAL;
    }
}

s32 DemoKekkai_CheckEventFlag(s32 params) {
    static s32 eventFlags[] = { 0xC3, 0xBC, 0xBF, 0xBE, 0xBD, 0xAD, 0xBB };

    if ((params < KEKKAI_TOWER) || (params > KEKKAI_FOREST)) {
        return true;
    }
    if (gSaveContext.n64ddFlag && params > KEKKAI_TOWER) {
        return Flags_GetRandomizerInf(trialParamToRandInf(params));
    }
    return Flags_GetEventChkInf(eventFlags[params]);
}

u32 TrialsDoneCount() {
    u8 trialCount = 0;

    if (Flags_GetRandomizerInf(RAND_INF_TRIALS_DONE_LIGHT_TRIAL)) {
        trialCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_TRIALS_DONE_FOREST_TRIAL)) {
        trialCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_TRIALS_DONE_FIRE_TRIAL)) {
        trialCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_TRIALS_DONE_WATER_TRIAL)) {
        trialCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_TRIALS_DONE_SPIRIT_TRIAL)) {
        trialCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_TRIALS_DONE_SHADOW_TRIAL)) {
        trialCount++;
    }

    return trialCount;
}

void DemoKekkai_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;

    this->sfxFlag = 0;
    this->energyAlpha = 1.0f;
    Actor_SetScale(thisx, 0.1f);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    Collider_InitCylinder(play, &this->collider1);
    Collider_SetCylinder(play, &this->collider1, thisx, &sCylinderInit);
    Collider_InitCylinder(play, &this->collider2);
    Collider_SetCylinder(play, &this->collider2, thisx, &sCylinderInit);
    Collider_UpdateCylinder(thisx, &this->collider1);
    Collider_UpdateCylinder(thisx, &this->collider2);
    this->timer = 0;
    this->barrierScrollRate = 1.0f;
    this->barrierScroll = 0.0f;
    switch (thisx->params) {
        case KEKKAI_TOWER:
            this->updateFunc = DemoKekkai_TowerBarrier;
            this->collider2.dim.radius = thisx->scale.x * 6100.0f;
            this->collider2.dim.height = thisx->scale.y * 5000.0f;
            this->collider2.dim.yShift = 300;

            if (gSaveContext.n64ddFlag) {
                if (TrialsDoneCount() == NUM_TRIALS) {
                    Actor_Kill(thisx);
                    return;
                }
            }
            break;
        case KEKKAI_WATER:
        case KEKKAI_LIGHT:
        case KEKKAI_FIRE:
        case KEKKAI_SHADOW:
        case KEKKAI_SPIRIT:
        case KEKKAI_FOREST:
            if (gSaveContext.n64ddFlag && Flags_GetRandomizerInf(trialParamToRandInf(thisx->params))) {
                Actor_Kill(thisx);
                return;
            }
            this->energyAlpha = 1.0f;
            this->orbScale = 1.0f;
            Actor_SetScale(thisx, 0.1f);
            thisx->update = DemoKekkai_TrialBarrierIdle;
            thisx->draw = DemoKekkai_DrawTrialBarrier;
            this->collider1.dim.radius = thisx->scale.x * 120.0f;
            this->collider1.dim.height = thisx->scale.y * 2000.0f;
            this->collider1.dim.yShift = 0;
            this->collider2.dim.radius = thisx->scale.x * 320.0f;
            this->collider2.dim.height = thisx->scale.y * 510.0f;
            this->collider2.dim.yShift = 95;
            break;
    }
    if (DemoKekkai_CheckEventFlag(thisx->params)) {
        if (thisx->params == KEKKAI_TOWER) {
            play->envCtx.unk_BF = 1;
        }
        Actor_Kill(thisx);
    }
}

void DemoKekkai_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
}

Vec3f demoKekkaiVel = { 0.0f, 0.0f, 0.0f };
void DemoKekkai_SpawnParticles(DemoKekkai* this, PlayState* play) {
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 lightYellow = { 255, 255, 170, 0 };
    static Color_RGBA8 darkRed = { 200, 0, 0, 0 };
    Vec3f pos;
    s32 i;

    for (i = 0; i < 85; i++) {
        s16 roll = Rand_ZeroFloat(65535.0f);
        s16 yaw = Rand_ZeroFloat(65535.0f);

        demoKekkaiVel.x = Math_SinS(yaw) * Math_CosS(roll) * Rand_ZeroFloat(8.0f);
        demoKekkaiVel.z = Math_CosS(yaw) * Math_CosS(roll) * Rand_ZeroFloat(8.0f);
        demoKekkaiVel.y = Math_SinS(roll) * Rand_ZeroFloat(3.0f);

        pos.x = (demoKekkaiVel.x * 7.0f) + this->actor.world.pos.x;
        pos.y = (demoKekkaiVel.y * 20.0f) + this->actor.world.pos.y + 120.0f;
        pos.z = (demoKekkaiVel.z * 7.0f) + this->actor.world.pos.z;

        EffectSsKiraKira_SpawnFocused(play, &pos, &demoKekkaiVel, &accel, &lightYellow, &darkRed, 3000,
                                      (s32)Rand_ZeroFloat(40.0f) + 45);
    }
}

void DemoKekkai_TowerBarrier(DemoKekkai* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.npcActions[0] != NULL) &&
        (play->csCtx.npcActions[0]->action != 1) && (play->csCtx.npcActions[0]->action == 2)) {
        if (!(this->sfxFlag & 1)) {
            func_800F3F3C(0xC);
            this->sfxFlag |= 1;
        }
        if (this->barrierScrollRate < 7.0f) {
            this->barrierScrollRate += 0.2f;
        } else {
            this->timer++;
            if (this->timer > 100) {
                Flags_SetEventChkInf(0xC3);
                Actor_Kill(&this->actor);
                return;
            } else if (this->timer > 40) {
                this->actor.scale.z = this->actor.scale.x += 0.003f;
            }
        }
    }
    if (!(this->sfxFlag & 1)) {
        func_8002F974(&this->actor, NA_SE_EV_TOWER_BARRIER - SFX_FLAG);
    }
}

void DemoKekkai_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoKekkai* this = (DemoKekkai*)thisx;

    if (this->energyAlpha > 0.99f) {
        if ((this->collider1.base.atFlags & AT_HIT) || (this->collider2.base.atFlags & AT_HIT)) {
            func_8002F71C(play, &this->actor, 6.0f, this->actor.yawTowardsPlayer, 6.0f);
        }
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider2.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider2.base);
    }
    this->updateFunc(this, play);
    this->barrierScroll += this->barrierScrollRate;
    if (this->barrierScroll > 65536.0f) {
        this->barrierScroll -= 65536.0f;
    }
}

void DemoKekkai_TrialBarrierDispel(Actor* thisx, PlayState* play) {
    static s32 eventFlags[] = { 0xC3, 0xBC, 0xBF, 0xBE, 0xBD, 0xAD, 0xBB };
    static u16 csFrames[] = { 0, 280, 280, 280, 280, 280, 280 };
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;

    if (gSaveContext.n64ddFlag) {
        Flags_SetRandomizerInf(trialParamToRandInf(thisx->params));
        // May or may not be needed. Not sure if needed for anything
        // that randoInf isn't already covering. Leaving it for safety.
        Flags_SetEventChkInf(eventFlags[thisx->params]); 
    }

    if (play->csCtx.frames == csFrames[this->actor.params]) {
        func_800F3F3C(0xA);
    }
    if (this->energyAlpha >= 0.05f) {
        this->energyAlpha -= 0.05f;
    } else {
        this->energyAlpha = 0.0f;
    }
    if (this->timer < 40) {
        this->orbScale = ((80 - this->timer) * (f32)this->timer * 0.000625f) + 1.0f;
    } else if (this->timer < 50) {
        this->orbScale = 2.0f;
    } else if (this->timer == 50) {
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_DM_RING_EXPLOSION);
        DemoKekkai_SpawnParticles(this, play);
    } else {
        this->orbScale = 0.0f;
    }
    if (this->orbScale != 0.0f) {
        func_8002F974(&this->actor, NA_SE_EV_TOWER_ENERGY - SFX_FLAG);
    }
    this->timer++;
}

static CutsceneData* sSageCutscenes[] = {
    NULL,
    gWaterTrialSageCs,
    gLightTrialSageCs,
    gFireTrialSageCs,
    gShadowTrialSageCs,
    gSpiritTrialSageCs,
    gForestTrialSageCs,
};

void DemoKekkai_TrialBarrierIdle(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;

    if (this->collider1.base.atFlags & AT_HIT) {
        func_8002F71C(play, &this->actor, 5.0f, this->actor.yawTowardsPlayer, 5.0f);
    }
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
    if (this->collider2.base.acFlags & AC_HIT) {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        // "I got it"
        LOG_STRING("当ったよ");
        this->actor.update = DemoKekkai_TrialBarrierDispel;
        this->timer = 0;
        play->csCtx.segment = SEGMENTED_TO_VIRTUAL(sSageCutscenes[this->actor.params]);
        gSaveContext.cutsceneTrigger = 1;
    }
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
    func_8002F974(&this->actor, NA_SE_EV_TOWER_ENERGY - SFX_FLAG);
}

void DemoKekkai_DrawTrialBarrier(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    s32 frames = play->gameplayFrames & 0xFFFF;
    u8 alphaIndex[102] = {
        1, 1, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 1, 2, 2,
        1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        1, 1, 0, 0, 0, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 0, 0,
    };
    s32 colorIndex;
    DemoKekkai* this = (DemoKekkai*)thisx;
    u8 alphas[3];
    Vtx* energyVtx = ResourceMgr_LoadVtxByName(SEGMENTED_TO_VIRTUAL(gTrialBarrierEnergyVtx));
    s32 i;

    if (this->orbScale != 0.0f) {
        alphas[2] = (s32)(this->energyAlpha * 202.0f);
        alphas[1] = (s32)(this->energyAlpha * 126.0f);
        alphas[0] = 0;
        for (i = 0; i < 102; i++) {
            energyVtx[i].v.cn[3] = alphas[alphaIndex[i]];
        }
        colorIndex = (this->actor.params - 1) * 6;
        OPEN_DISPS(play->state.gfxCtx);
        func_80093D84(play->state.gfxCtx);
        Matrix_Push();
        Matrix_Translate(0.0f, 1200.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(this->orbScale, this->orbScale, this->orbScale, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -1200.0f, 0.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPSegment(POLY_XLU_DISP++, 0x09,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, frames * 5, frames * -10, 0x20, 0x20, 1, frames * 5,
                                    frames * -10, 0x20, 0x20));
        gSPDisplayList(POLY_XLU_DISP++, gTrialBarrierOrbDL);
        Matrix_Pop();
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 50, 0, 100, 255);
        gSPSegment(POLY_XLU_DISP++, 0x0A,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x20, 1, frames, frames, 0x20, 0x20));
        gSPDisplayList(POLY_XLU_DISP++, gTrialBarrierFloorDL);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, sEnergyColors[colorIndex + 0], sEnergyColors[colorIndex + 1],
                        sEnergyColors[colorIndex + 2], 255);
        gDPSetEnvColor(POLY_XLU_DISP++, sEnergyColors[colorIndex + 3], sEnergyColors[colorIndex + 4],
                       sEnergyColors[colorIndex + 5], 128);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, frames * 5, frames * -10, 0x20, 0x20, 1, frames * 5,
                                    frames * -10, 0x20, 0x40));
        gSPDisplayList(POLY_XLU_DISP++, gTrialBarrierEnergyDL);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void DemoKekkai_DrawTowerBarrier(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;
    s32 scroll;

    scroll = (s32)this->barrierScroll & 0xFFFF;
    OPEN_DISPS(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 170, 255, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, scroll * 2, scroll * -4, 0x20, 0x40, 1, scroll * 2,
                                scroll * -4, 0x20, 0x40));
    gSPDisplayList(POLY_XLU_DISP++, gTowerBarrierDL);
    CLOSE_DISPS(play->state.gfxCtx);
}

void DemoKekkai_Reset(void) {
    demoKekkaiVel.x = 0.0f;
    demoKekkaiVel.y = 0.0f;
    demoKekkaiVel.z = 0.0f;
}