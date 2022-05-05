/*
 * File: z_demo_6k.c
 * Overlay: ovl_Demo_6K
 * Description: Sages, balls of light (cutscene)
 */

#include "z_demo_6k.h"
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_demo_6k/object_demo_6k.h"
#include "objects/object_gnd_magic/object_gnd_magic.h"
#include "overlays/actors/ovl_Eff_Dust/z_eff_dust.h"

#define FLAGS ACTOR_FLAG_4

void Demo6K_Init(Actor* thisx, GlobalContext* globalCtx);
void Demo6K_Destroy(Actor* thisx, GlobalContext* globalCtx);
void Demo6K_Update(Actor* thisx, GlobalContext* globalCtx);
void Demo6K_Reset(void);


void func_80966DB0(Demo6K* this, GlobalContext* globalCtx);
void func_80966E04(Demo6K* this, GlobalContext* globalCtx);
void func_80966E98(Demo6K* this, GlobalContext* globalCtx);
void func_80966F84(Demo6K* this, GlobalContext* globalCtx);
void func_8096712C(Demo6K* this, GlobalContext* globalCtx);
void func_80967410(Demo6K* this, GlobalContext* globalCtx);
void func_809674E0(Demo6K* this, GlobalContext* globalCtx);
void func_8096784C(Demo6K* this, GlobalContext* globalCtx);
void func_80967A04(Demo6K* this, s32 i);
void func_80967AD0(Demo6K* this, GlobalContext* globalCtx);
void func_80967DBC(Demo6K* this, GlobalContext* globalCtx);
void func_80967F10(Demo6K* this, GlobalContext* globalCtx);
void func_80967FFC(Actor* thisx, GlobalContext* globalCtx);
void func_80968298(Actor* thisx, GlobalContext* globalCtx);
void func_8096865C(Actor* thisx, GlobalContext* globalCtx);
void func_809688C4(Actor* thisx, GlobalContext* globalCtx);
void func_80968B70(Actor* thisx, GlobalContext* globalCtx);
void func_80968FB0(Actor* thisx, GlobalContext* globalCtx);
void func_809691BC(Demo6K* this, GlobalContext* globalCtx, s32 params);

const ActorInit Demo_6K_InitVars = {
    ACTOR_DEMO_6K,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(Demo6K),
    (ActorFunc)Demo6K_Init,
    (ActorFunc)Demo6K_Destroy,
    (ActorFunc)Demo6K_Update,
    NULL,
    Demo6K_Reset,
};

static s16 sObjectIds[] = {
    OBJECT_GAMEPLAY_KEEP, OBJECT_DEMO_6K,       OBJECT_DEMO_6K,       OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP,
    OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP,
    OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GND_MAGIC,     OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP,
    OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP, OBJECT_GAMEPLAY_KEEP,
};
static Color_RGB8 sEnvColors[] = {
    { 255, 50, 0 }, { 0, 200, 0 }, { 200, 255, 0 }, { 200, 50, 255 }, { 255, 150, 0 }, { 0, 150, 255 },
};
static f32 D_8096930C[] = { 1.0f, 1.04f, 1.0f, 0.96f };
static f32 D_8096931C[] = { 1.1f, 1.0f, 0.9f, 0.8f };

void Demo6K_SetupAction(Demo6K* this, Demo6KActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Demo6K_Init(Actor* thisx, GlobalContext* globalCtx) {
    Demo6K* this = (Demo6K*)thisx;
    s32 pad;
    s32 params = this->actor.params;
    s32 objBankIndex;
    s32 i;

    osSyncPrintf("no = %d\n", params);

    if (sObjectIds[params] != OBJECT_GAMEPLAY_KEEP) {
        objBankIndex = Object_GetIndex(&globalCtx->objectCtx, sObjectIds[params]);
    } else {
        objBankIndex = 0;
    }

    osSyncPrintf("bank_ID = %d\n", objBankIndex);

    if (objBankIndex < 0) {
        ASSERT(0, "0", "../z_demo_6k.c", 334);
    } else {
        this->objBankIndex = objBankIndex;
    }

    Demo6K_SetupAction(this, func_80966DB0);
    this->timer1 = 0;
    this->flags = 0;
    this->timer2 = 0;

    switch (params) {
        case 0:
            this->drawFunc = func_809688C4;
            this->initActionFunc = func_80967AD0;
            Actor_SetScale(&this->actor, 1.0f);

            for (i = 0; i < 16; i++) {
                func_80967A04(this, i);
                this->unk_1B4[i] = 0.0f;
            }

            this->unk_170 = 0.0f;
            break;
        case 1:
            this->drawFunc = func_80967FFC;
            this->initActionFunc = func_80966E04;
            Actor_SetScale(&this->actor, 0.228f);
            break;
        case 2:
            this->drawFunc = func_80968298;
            this->initActionFunc = func_80966F84;
            Actor_SetScale(&this->actor, 0.1f);
            this->unk_164 = 1.0f;
            this->unk_168 = 1.0f;
            this->unk_16C = 0.0f;
            this->unk_170 = 0.0f;
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            this->drawFunc = func_8096865C;
            this->initActionFunc = func_8096712C;
            Actor_SetScale(&this->actor, 0.0f);
            this->unk_293 = params - 3;
            break;
        case 9:
        case 10:
            this->drawFunc = func_8096865C;
            Actor_SetScale(&this->actor, 0.0f);
            this->initActionFunc = func_809674E0;
            break;
        case 11:
            this->drawFunc = func_8096865C;
            Actor_SetScale(&this->actor, 0.0f);
            this->initActionFunc = func_8096784C;
            this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_NABALL_VANISH);
            break;
        case 12:
            Actor_SetScale(&this->actor, 0.0f);
            this->initActionFunc = func_80967F10;
            this->drawFunc = func_80968B70;
            Actor_ChangeCategory(globalCtx, &globalCtx->actorCtx, &this->actor, ACTORCAT_ITEMACTION);
            break;
        case 13:
            Actor_SetScale(&this->actor, 0.14f);
            Demo6K_SetupAction(this, func_80967DBC);
            this->actor.draw = func_80968FB0;
            this->unk_293 = 0;
            break;
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            this->actor.flags |= ACTOR_FLAG_5;
            this->drawFunc = func_8096865C;
            this->initActionFunc = func_80967410;
            this->flags |= 1;
            Actor_SetScale(&this->actor, 0.2f);
            this->unk_293 = params - 14;
            break;
        default:
            ASSERT(0, "0", "../z_demo_6k.c", 435);
            break;
    }

    switch (params) {
        case 9:
            this->unk_293 = 0;
            break;
        case 10:
            this->unk_293 = 5;
            break;
        case 11:
            this->unk_293 = 4;
            break;
    }

    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 255, 255, 255, 100);
    this->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &this->lightInfo);
}

void Demo6K_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    Demo6K* this = (Demo6K*)thisx;

    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, this->lightNode);
}

void func_80966DB0(Demo6K* this, GlobalContext* globalCtx) {
    if (Object_IsLoaded(&globalCtx->objectCtx, this->objBankIndex)) {
        this->actor.objBankIndex = this->objBankIndex;
        this->actor.draw = this->drawFunc;
        this->actionFunc = this->initActionFunc;
    }
}

void func_80966E04(Demo6K* this, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.frames > 214) {
        func_8002F948(&this->actor, NA_SE_EV_LIGHT_GATHER - SFX_FLAG);
    }

    if (globalCtx->csCtx.frames > 264) {
        func_8002F948(&this->actor, NA_SE_EV_GOD_LIGHTBALL_2 - SFX_FLAG);
    }

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[6] != NULL) &&
        (globalCtx->csCtx.npcActions[6]->action == 2)) {
        Demo6K_SetupAction(this, func_80966E98);
    }
}

void func_80966E98(Demo6K* this, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.frames < 353) {
        func_8002F948(&this->actor, NA_SE_EV_LIGHT_GATHER - SFX_FLAG);
        func_8002F948(&this->actor, NA_SE_EV_GOD_LIGHTBALL_2 - SFX_FLAG);
    }

    if (globalCtx->csCtx.frames == 342) {
        func_800F3F3C(2);
    }

    if (this->timer1 == 39) {
        func_800788CC(NA_SE_EV_CONSENTRATION);
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_6K, this->actor.world.pos.x,
                    this->actor.world.pos.y + 10.0f, this->actor.world.pos.z, 0, 0, 0, 2);
    }

    if (this->timer1 == 64) {
        Actor_Kill(&this->actor);
    }

    this->timer1++;
}

void func_80966F84(Demo6K* this, GlobalContext* globalCtx) {
    if (this->timer1 < 5) {
        this->unk_168 = D_8096930C[this->timer1 & 3];
    } else if (this->timer1 < 15) {
        this->actor.scale.x += 0.012f;
        Actor_SetScale(&this->actor, this->actor.scale.x);
        Math_StepToF(&this->unk_170, 0.6f, 0.05f);
        this->unk_168 = 1.0f;
    } else {
        if (this->timer1 == 15) {
            Actor_SpawnAsChild(&globalCtx->actorCtx, &this->actor, globalCtx, ACTOR_EFF_DUST, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, EFF_DUST_TYPE_1);
        }
        Math_StepToF(&this->unk_16C, 1.0f, 0.02f);
        this->unk_168 = D_8096930C[this->timer1 & 1];
    }

    this->timer1++;
}

void func_809670AC(Demo6K* this, GlobalContext* globalCtx) {
    this->timer2++;

    if (this->timer1 < 10) {
        this->timer1++;
    } else if (this->actor.scale.x > 0.0f) {
        this->actor.scale.x -= 1.0f / 120.0f;
        Actor_SetScale(&this->actor, this->actor.scale.x);
    } else {
        Actor_Kill(&this->actor);
    }
}

void func_8096712C(Demo6K* this, GlobalContext* globalCtx) {
    static u16 D_8096932C[] = { 275, 275, 275, 275, 275, 275 };
    u32 frames = globalCtx->state.frames;

    if (this->actor.scale.x < 0.1f) {
        this->actor.scale.x += 0.0017f;
    } else if (frames & 1) {
        this->actor.scale.x = 0.1f * 1.04f;
    } else {
        this->actor.scale.x = 0.1f;
    }

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[6] != NULL) &&
        (globalCtx->csCtx.npcActions[6]->action == 2)) {
        Demo6K_SetupAction(this, func_809670AC);
        this->timer1 = 0;
        this->actor.scale.x = 0.1f;
    }

    Actor_SetScale(&this->actor, this->actor.scale.x);

    this->timer2++;

    if ((globalCtx->sceneNum == SCENE_GANONTIKA) && (globalCtx->csCtx.frames < D_8096932C[this->actor.params - 3])) {
        func_8002F974(&this->actor, NA_SE_EV_LIGHT_GATHER - SFX_FLAG);
    }
}

Vec3f sDemo6kVelocity = { 0.0f, 0.0f, 0.0f };
void func_80967244(Demo6K* this, GlobalContext* globalCtx) {
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 0 };
    static Color_RGBA8 envColor = { 255, 150, 0, 0 };
    Vec3f pos;
    s16 rand1;
    s16 rand2;
    s32 scale;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z = this->actor.world.pos.z;

    rand1 = Rand_ZeroFloat(0xFFFF);
    rand2 = Rand_ZeroFloat(0xFFFF);

    sDemo6kVelocity.x = Math_SinS(rand2) * Math_CosS(rand1) * 20.0f;
    sDemo6kVelocity.z = Math_CosS(rand2) * Math_CosS(rand1) * 20.0f;
    sDemo6kVelocity.y = Math_SinS(rand1) * 20.0f;

    accel.y = 0.0f;

    envColor.r = sEnvColors[this->unk_293].r;
    envColor.g = sEnvColors[this->unk_293].g;
    envColor.b = sEnvColors[this->unk_293].b;

    if (globalCtx->sceneNum == SCENE_TOKINOMA) {
        scale = 6000;
    } else if (globalCtx->csCtx.frames < 419) {
        scale = 6000;
    } else {
        scale = 18000;
    }

    EffectSsKiraKira_SpawnFocused(globalCtx, &pos, &sDemo6kVelocity, &accel, &primColor, &envColor, scale, 20);
}

void func_80967410(Demo6K* this, GlobalContext* globalCtx) {
    s32 params = this->actor.params - 14;

    this->timer2++;

    Actor_SetScale(&this->actor, 0.2f);

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[params] != NULL)) {
        func_809691BC(this, globalCtx, params);

        if (globalCtx->csCtx.npcActions[params]->action == 3) {
            this->flags &= ~1;
            func_80967244(this, globalCtx);
        } else {
            this->flags |= 1;
        }
    } else {
        this->flags |= 1;
    }
}

void func_809674E0(Demo6K* this, GlobalContext* globalCtx) {
    u32 frames = globalCtx->state.frames;

    if (this->actor.scale.x < 0.05f) {
        this->actor.scale.x += 0.005f;
    } else if (frames & 1) {
        this->actor.scale.x = 0.05f * 1.04f;
    } else {
        this->actor.scale.x = 0.05f;
    }

    Actor_SetScale(&this->actor, this->actor.scale.x);

    this->timer2++;

    if (this->timer2 > 47) {
        Actor_Kill(&this->actor);
    } else if (this->timer2 > 39) {
        f32 dTimer = this->timer2 - 39;
        f32 temp = 1.0f / (9.0f - dTimer);

        this->actor.world.pos.x += (-1611.0f - this->actor.world.pos.x) * temp;
        this->actor.world.pos.y += (19.0f - this->actor.world.pos.y) * temp;
        this->actor.world.pos.z += (1613.0f - this->actor.world.pos.z) * temp;

        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_FIRE - SFX_FLAG);
    }

    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, sEnvColors[this->unk_293].r, sEnvColors[this->unk_293].g,
                              sEnvColors[this->unk_293].b, this->actor.scale.x * 4000.0f);
}

void func_809676A4(Demo6K* this, GlobalContext* globalCtx) {
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 0 };
    static Color_RGBA8 envColor = { 255, 150, 0, 0 };
    Vec3f pos;
    f32 temp = this->actor.scale.x * 500.0f;
    s32 i;

    for (i = 0; i < 8; i++) {
        pos.x = this->actor.world.pos.x + Rand_CenteredFloat(temp);
        pos.y = this->actor.world.pos.y + Rand_CenteredFloat(temp);
        pos.z = this->actor.world.pos.z + Rand_CenteredFloat(temp);

        velocity.x = Rand_CenteredFloat(2.0f);
        velocity.y = (Rand_ZeroFloat(-10.0f) - 5.0f) * 0.1f;
        velocity.z = Rand_CenteredFloat(2.0f);

        accel.y = 0.0f;

        EffectSsKiraKira_SpawnFocused(globalCtx, &pos, &velocity, &accel, &primColor, &envColor, 500, 20);
    }
}

void func_8096784C(Demo6K* this, GlobalContext* globalCtx) {
    u32 frames = globalCtx->state.frames;

    this->timer2++;

    if (this->timer2 > 24) {
        Actor_Kill(&this->actor);
    } else if (this->timer2 > 4) {
        this->actor.velocity.x += Rand_CenteredFloat(0.2f);
        this->actor.velocity.y += 0.12f;
        this->actor.velocity.z += Rand_CenteredFloat(0.2f);

        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;

        this->actor.scale.x -= 0.0015f;

        func_809676A4(this, globalCtx);
    } else if (frames & 1) {
        this->actor.scale.x = 0.033f;
    } else {
        this->actor.scale.x = 0.03f;
    }

    Actor_SetScale(&this->actor, this->actor.scale.x);
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, sEnvColors[this->unk_293].r, sEnvColors[this->unk_293].g,
                              sEnvColors[this->unk_293].b, this->actor.scale.x * 4000.0f);
}

void func_80967A04(Demo6K* this, s32 i) {
    this->unk_174[i] = (i * 10.0f) - 80.0f;
    this->unk_1B4[i] = Rand_ZeroFloat(100.0f) + 100.0f;
    this->unk_1F4[i] = -3.0f - Rand_ZeroFloat(6.0f);
    this->unk_274[i] = (s32)Rand_ZeroFloat(6.0f);
    this->unk_234[i] = Rand_ZeroFloat(0.02f) + 0.01f;
}

void func_80967AD0(Demo6K* this, GlobalContext* globalCtx) {
    s32 i;

    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[1] != NULL)) {
        if (globalCtx->csCtx.npcActions[1]->action == 2) {
            this->unk_170++;
            func_8002F948(&this->actor, NA_SE_EV_RAINBOW_SHOWER - SFX_FLAG);
        }

        func_809691BC(this, globalCtx, 1);
    }

    for (i = 0; (i < (s32)this->unk_170) && (i < 16); i++) {
        this->unk_1B4[i] += this->unk_1F4[i];
        if (this->unk_1B4[i] < 0.0f) {
            func_80967A04(this, i);
        }
    }

    this->timer1++;
}

void func_80967BF8(Player* player, GlobalContext* globalCtx) {
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 0 };
    static Color_RGBA8 envColor = { 255, 200, 0, 0 };
    Vec3f pos;
    s32 i;

    for (i = 0; i < 150; i++) {
        pos.x = Rand_CenteredFloat(15.0f) + player->actor.world.pos.x;
        pos.y = Rand_CenteredFloat(15.0f) + player->actor.world.pos.y + 30.0f;
        pos.z = Rand_CenteredFloat(15.0f) + player->actor.world.pos.z;

        velocity.x = Rand_CenteredFloat(8.0f) + 1.0f;
        velocity.y = Rand_CenteredFloat(4.0f);
        velocity.z = Rand_CenteredFloat(8.0f) + 2.0f;

        accel.y = 0.0f;

        EffectSsKiraKira_SpawnFocused(globalCtx, &pos, &velocity, &accel, &primColor, &envColor, 1000,
                                      (s32)Rand_ZeroFloat(60.0f) + 60);
    }
}

void func_80967DBC(Demo6K* this, GlobalContext* globalCtx) {
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_ATTACK_DEMO - SFX_FLAG);

    this->timer2++;

    if (this->timer2 > 44) {
        if (this->unk_293 < 236) {
            this->unk_293 += 20;
        } else {
            this->unk_293 = 255;
        }

        if (this->timer2 > 104) {
            func_80967BF8(GET_PLAYER(globalCtx), globalCtx);
            Actor_Kill(&this->actor);
            Audio_PlayActorSound2(&GET_PLAYER(globalCtx)->actor, NA_SE_EN_FANTOM_HIT_THUNDER);
        } else if (this->timer2 > 94) {
            Actor_SetScale(&this->actor, this->actor.scale.x + 0.03f);

            if (this->timer2 == 95) {
                osSyncPrintf(VT_FGCOL(CYAN) "  NA_SE_EN_GANON_FIRE_DEMO\n" VT_RST);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_GANON_FIRE_DEMO);
            }
        }

        Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 255, 200, 0, this->unk_293);
    }
}

void func_80967F10(Demo6K* this, GlobalContext* globalCtx) {
    if (this->timer2 == 0) {
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_6K, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 13);
    }

    this->timer2++;

    if (this->timer2 > 60) {
        Actor_Kill(&this->actor);
    }

    Actor_SetScale(&this->actor, 0.05f - (this->timer2 * 0.00075f));
}

void Demo6K_Update(Actor* thisx, GlobalContext* globalCtx) {
    Demo6K* this = (Demo6K*)thisx;

    this->actionFunc(this, globalCtx);
}

void func_80967FFC(Actor* thisx, GlobalContext* globalCtx) {
    Demo6K* this = (Demo6K*)thisx;
    s32 pad;
    u16 timer1 = this->timer1;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1070);

    func_80093D84(globalCtx->state.gfxCtx);
    Matrix_RotateX(-M_PI / 2, MTXMODE_APPLY);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0x7FFF - ((timer1 * 8) & 0x7FFF), 16, 512, 1, 0,
                                0x7FFF - ((timer1 * 8) & 0x7FFF), 16, 32));

    {
        s32 i;
        s32 pad;
        Color_RGB8 colors[6][2] = {
            { { 255, 170, 255 }, { 255, 0, 100 } }, { { 255, 255, 170 }, { 0, 255, 0 } },
            { { 255, 255, 170 }, { 255, 255, 0 } }, { { 255, 170, 255 }, { 50, 0, 255 } },
            { { 255, 255, 170 }, { 255, 100, 0 } }, { { 170, 255, 255 }, { 0, 100, 255 } },
        };

        Matrix_RotateZ(-M_PI / 2, MTXMODE_APPLY);

        for (i = 0; i < 6; i++) {
            Matrix_RotateZ(M_PI / 3, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1115),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, colors[i][0].r, colors[i][0].g, colors[i][0].b, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, colors[i][1].r, colors[i][1].g, colors[i][1].b, 255);
            gSPDisplayList(POLY_XLU_DISP++, object_demo_6k_DL_0022B0);
        }

        // required to avoid optimizing out i
        if ((s16)i) {}
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1127);
}

void func_80968298(Actor* thisx, GlobalContext* globalCtx) {
    static u8 skipIndices[] = { 6, 7, 11, 16, 20, 24, 28, 33, 35, 41, 45, 50, 57, 58, 62, 255 };
    Demo6K* this = (Demo6K*)thisx;
    s32 pad;
    u32 timer1 = this->timer1;
    f32 scale = this->unk_164 * this->unk_168;
    Vtx* vertices = ResourceMgr_LoadVtxByName(SEGMENTED_TO_VIRTUAL(object_demo_6kVtx_0035E0));
    s32 i;
    s32 i2;
    u8 alpha;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1145);

    alpha = (s32)(this->unk_170 * 255.0f);
    POLY_XLU_DISP = func_800937C0(POLY_XLU_DISP);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, alpha);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
    gDPFillRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    func_80093D84(globalCtx->state.gfxCtx);

    alpha = (s32)(this->unk_16C * 255.0f);
    for (i2 = 0, i = 0; i < 63; i++) {
        if (i == skipIndices[i2]) {
            i2++;
        } else {
            vertices[i].v.cn[3] = alpha;
        }
    }

    Matrix_RotateX(-M_PI / 2, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1170),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 210, 210, 210, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 100, 100, 100, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (0xFFF - (timer1 * 6)) & 0xFFF, (timer1 * 12) & 0xFFF, 128,
                                64, 1, (0xFFF - (timer1 * 6)) & 0xFFF, (timer1 * 12) & 0xFFF, 64, 32));
    gSPDisplayList(POLY_XLU_DISP++, object_demo_6k_DL_0039D0);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1189),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    func_80093D84(globalCtx->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 50, 50, 50, 255);
    gSPDisplayList(POLY_XLU_DISP++, object_demo_6k_DL_001040);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1198);
}

void func_8096865C(Actor* thisx, GlobalContext* globalCtx) {
    Demo6K* this = (Demo6K*)thisx;
    s32 pad;
    Gfx* displayList;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1208);

    if (!(this->flags & 1)) {
        if (this->actor.params > 8) {
            displayList = gEffFlash1DL;
        } else {
            displayList = gEffFlash2DL;
        }

        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, sEnvColors[this->unk_293].r, sEnvColors[this->unk_293].g,
                       sEnvColors[this->unk_293].b, 255);
        Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_RotateZ((f32)(this->timer2 * 6) * (M_PI / 180.0f), MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1230),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, displayList);
        Matrix_Pop();
        Matrix_RotateZ(-(f32)(this->timer2 * 6) * (M_PI / 180.0f), MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1236),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, displayList);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1242);
}

void func_809688C4(Actor* thisx, GlobalContext* globalCtx2) {
    Demo6K* this = (Demo6K*)thisx;
    GlobalContext* globalCtx = globalCtx2;
    u32 frames = globalCtx->state.frames;
    s32 i;

    if ((i = (globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[1] != NULL)) &&
        (globalCtx->csCtx.npcActions[1]->action != 1)) {
        OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1277);

        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, 255);
        Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000) * (M_PI / 0x8000), MTXMODE_APPLY);

        for (i = 0; i < 16; i++) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, sEnvColors[this->unk_274[i]].r, sEnvColors[this->unk_274[i]].g,
                           sEnvColors[this->unk_274[i]].b, 255);
            Matrix_Push();
            Matrix_Translate(this->unk_174[i], this->unk_1B4[i], 0.0f, MTXMODE_APPLY);
            Matrix_Scale(this->unk_234[i] * D_8096931C[(frames + i) & 3],
                         this->unk_234[i] * D_8096931C[(frames + i) & 3],
                         this->unk_234[i] * D_8096931C[(frames + i) & 3], MTXMODE_APPLY);
            Matrix_ReplaceRotation(&globalCtx->billboardMtxF);
            gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1297),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
            Matrix_Pop();
        }

        gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1305);
    }
}

void func_80968B70(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    Demo6K* this = (Demo6K*)thisx;
    u32 timer2 = this->timer2;
    u8 primColor[4];
    u8 envColor[3];

    if (1) {}

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1316);

    func_80093D84(globalCtx->state.gfxCtx);
    Matrix_Mult(&globalCtx->billboardMtxF, MTXMODE_APPLY);
    Matrix_RotateX(M_PI / 2, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1322),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0xFF - ((timer2 * 2) & 0xFF), 0, 32, 32, 1,
                                0xFF - ((timer2 * 2) & 0xFF), (timer2 * 15) & 0x3FF, 16, 64));

    if (this->timer2 < 40) {
        primColor[0] = primColor[2] = 100 - (this->timer2 * 2.5f);
        primColor[1] = envColor[1] = 0;
        envColor[0] = 100 - primColor[2];
        primColor[3] = this->timer2 * 6.375f;
        envColor[2] = envColor[0] * 2;
    } else if (this->timer2 < 50) {
        primColor[0] = (this->timer2 * 5) - 200;
        primColor[1] = primColor[2] = 0;
        primColor[3] = 255;
        envColor[0] = 100 - primColor[0];
        envColor[1] = primColor[0] * 2;
        envColor[2] = 200 - (primColor[0] * 4);
    } else {
        primColor[2] = (this->timer2 * 5) - 250;
        envColor[2] = 0;
        primColor[1] = primColor[2] * 3;
        primColor[3] = 255;
        primColor[0] = envColor[0] = (primColor[2] * 2) + 50;
        envColor[1] = 100 - primColor[2];
    }

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, primColor[0], primColor[1], primColor[2], primColor[3]);
    gDPSetEnvColor(POLY_XLU_DISP++, envColor[0], envColor[1], envColor[2], 128);
    gSPDisplayList(POLY_XLU_DISP++, object_gnd_magic_DL_001190);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1368);
}

void func_80968FB0(Actor* thisx, GlobalContext* globalCtx) {
    static u8 D_809693CC[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 7, 6, 5, 4, 3, 2, 1 };
    Demo6K* this = (Demo6K*)thisx;
    Gfx* displayList = Graph_Alloc(globalCtx->state.gfxCtx, 4 * sizeof(Gfx));
    u16 frames = globalCtx->gameplayFrames;
    f32 scaleFactor;
    s32 pad;

    if (1) {}

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1386);

    func_80093D84(globalCtx->state.gfxCtx);
    scaleFactor = ((s16)D_809693CC[(frames * 4) & 0xF] * 0.01f) + 1.0f;
    Matrix_Scale(this->actor.scale.x * scaleFactor, this->actor.scale.y * scaleFactor,
                 this->actor.scale.z * scaleFactor, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1394),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_XLU_DISP++, 0x08, displayList);
    gDPPipeSync(displayList++);
    gDPSetPrimColor(displayList++, 0, 0x80, 255, 255, 255, this->unk_293);
    gDPSetRenderMode(displayList++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
    gSPEndDisplayList(displayList++);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 255);
    gSPDisplayList(POLY_XLU_DISP++, gGlowCircleSmallDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_demo_6k.c", 1411);
}

void func_809691BC(Demo6K* this, GlobalContext* globalCtx, s32 params) {
    Vec3f startPos;
    Vec3f endPos;
    f32 temp;
    CsCmdActorAction* csAction = globalCtx->csCtx.npcActions[params];

    startPos.x = csAction->startPos.x;
    startPos.y = csAction->startPos.y;
    startPos.z = csAction->startPos.z;

    endPos.x = csAction->endPos.x;
    endPos.y = csAction->endPos.y;
    endPos.z = csAction->endPos.z;

    temp = Environment_LerpWeight(csAction->endFrame, csAction->startFrame, globalCtx->csCtx.frames);

    this->actor.world.pos.x = (((endPos.x - startPos.x) * temp) + startPos.x);
    this->actor.world.pos.y = (((endPos.y - startPos.y) * temp) + startPos.y);
    this->actor.world.pos.z = (((endPos.z - startPos.z) * temp) + startPos.z);
}

void Demo6K_Reset(void) {
    sDemo6kVelocity.x = 0.0f;
    sDemo6kVelocity.y = 0.0f;
    sDemo6kVelocity.z = 0.0f;
}