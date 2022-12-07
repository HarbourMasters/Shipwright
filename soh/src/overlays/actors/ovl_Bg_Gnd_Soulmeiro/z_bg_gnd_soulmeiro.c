/*
 * File: z_bg_gnd_soulmeiro.c
 * Overlay: ovl_Bg_Gnd_Soulmeiro
 * Description: Web-Blocked Ceiling Hole (Inside Ganon's Castle)
 */

#include "z_bg_gnd_soulmeiro.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"
#include "global.h"

#define FLAGS 0

void BgGndSoulmeiro_Init(Actor* thisx, PlayState* play);
void BgGndSoulmeiro_Destroy(Actor* thisx, PlayState* play);
void BgGndSoulmeiro_Update(Actor* thisx, PlayState* play);
void BgGndSoulmeiro_Draw(Actor* thisx, PlayState* play);

void func_8087AF38(BgGndSoulmeiro* this, PlayState* play);
void func_8087B284(BgGndSoulmeiro* this, PlayState* play);
void func_8087B350(BgGndSoulmeiro* this, PlayState* play);

const ActorInit Bg_Gnd_Soulmeiro_InitVars = {
    ACTOR_BG_GND_SOULMEIRO,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_DEMO_KEKKAI,
    sizeof(BgGndSoulmeiro),
    (ActorFunc)BgGndSoulmeiro_Init,
    (ActorFunc)BgGndSoulmeiro_Destroy,
    (ActorFunc)BgGndSoulmeiro_Update,
    (ActorFunc)BgGndSoulmeiro_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0x00020800, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 50, 20, 20, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgGndSoulmeiro_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndSoulmeiro* this = (BgGndSoulmeiro*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actionFunc = NULL;

    switch (this->actor.params & 0xFF) {
        case 0:
            Collider_InitCylinder(play, &this->collider);
            Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
            this->actionFunc = func_8087B284;
            if (Flags_GetSwitch(play, (this->actor.params >> 8) & 0x3F)) {

                Actor_Spawn(&play->actorCtx, play, ACTOR_MIR_RAY, this->actor.world.pos.x,
                            this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 9, true);
                this->actor.draw = NULL;
                Actor_Kill(&this->actor);
                return;
            } else {
                this->actor.draw = BgGndSoulmeiro_Draw;
            }
            break;
        case 1:
        case 2:
            if (Flags_GetSwitch(play, (this->actor.params >> 8) & 0x3F)) {
                this->actor.draw = BgGndSoulmeiro_Draw;
            } else {
                this->actor.draw = NULL;
            }
            this->actionFunc = func_8087B350;
            break;
    }
}

void BgGndSoulmeiro_Destroy(Actor* thisx, PlayState* play) {
    BgGndSoulmeiro* this = (BgGndSoulmeiro*)thisx;

    if ((this->actor.params & 0xFF) == 0) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void func_8087AF38(BgGndSoulmeiro* this, PlayState* play) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f vecA;
    Vec3f vecB;
    Actor* thisx = &this->actor;

    if (this->unk_198 != 0) {
        this->unk_198--;
    }

    if (this->unk_198 == 20) {
        Flags_SetSwitch(play, (thisx->params >> 8) & 0x3F);
        thisx->draw = NULL;
    }

    // This should be this->unk_198 == 0, this is required to match
    if (!this->unk_198) {
        Flags_SetSwitch(play, (thisx->params >> 8) & 0x3F);
        Actor_Kill(&this->actor);
        Actor_Spawn(&play->actorCtx, play, ACTOR_MIR_RAY, thisx->world.pos.x, thisx->world.pos.y,
                    thisx->world.pos.z, 0, 0, 0, 9, true);
    } else if ((this->unk_198 % 6) == 0) {
        s32 i;
        s16 temp_2 = Rand_ZeroOne() * (10922.0f); // This should be: 0x10000 / 6.0f

        vecA.y = 0.0f;
        vecB.y = thisx->world.pos.y;

        for (i = 0; i < 6; i++) {
            s16 temp_1 = Rand_CenteredFloat(0x2800) + temp_2;
            f32 temp_3 = Math_SinS(temp_1);
            f32 temp_4 = Math_CosS(temp_1);
            f32 distXZ;

            vecB.x = thisx->world.pos.x + (120.0f * temp_3);
            vecB.z = thisx->world.pos.z + (120.0f * temp_4);
            distXZ = Math_Vec3f_DistXZ(&thisx->home.pos, &vecB) * (1.0f / 120.0f);
            if (distXZ < 0.7f) {
                temp_3 = Math_SinS(temp_1 + 0x8000);
                temp_4 = Math_CosS(temp_1 + 0x8000);
                vecB.x = thisx->world.pos.x + (120.0f * temp_3);
                vecB.z = thisx->world.pos.z + (120.0f * temp_4);
                distXZ = Math_Vec3f_DistXZ(&thisx->home.pos, &vecB) * (1.0f / 120.0f);
            }

            vecA.x = 4.0f * temp_3 * distXZ;
            vecA.y = 0.0f;
            vecA.z = 4.0f * temp_4 * distXZ;
            EffectSsDeadDb_Spawn(play, &thisx->home.pos, &vecA, &zeroVec, 60, 6, 255, 255, 150, 170, 255, 0, 0, 1,
                                 14, true);
            temp_2 += 0x2AAA;
        }
    }
}

void func_8087B284(BgGndSoulmeiro* this, PlayState* play) {
    s32 pad;

    if (!Flags_GetSwitch(play, (this->actor.params >> 8) & 0x3F)) {
        this->actor.draw = BgGndSoulmeiro_Draw;
        if (this->collider.base.acFlags & AC_HIT) {
            Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->unk_198 = 40;
            this->actionFunc = func_8087AF38;
        } else {
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void func_8087B350(BgGndSoulmeiro* this, PlayState* play) {
    if (Flags_GetSwitch(play, (this->actor.params >> 8) & 0x3F)) {
        this->actor.draw = BgGndSoulmeiro_Draw;
    } else {
        this->actor.draw = NULL;
    }
}

void BgGndSoulmeiro_Update(Actor* thisx, PlayState* play) {
    BgGndSoulmeiro* this = (BgGndSoulmeiro*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void BgGndSoulmeiro_Draw(Actor* thisx, PlayState* play) {
    static Gfx* dLists[] = {
        gSpiritTrialWebDL,
        gSpiritTrialLightSourceDL,
        gSpiritTrialLightFloorDL,
    };
    s32 params = thisx->params & 0xFF;

    switch (params) {
        case 0:
            OPEN_DISPS(play->state.gfxCtx);
            Gfx_SetupDL_25Xlu(play->state.gfxCtx);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, dLists[params]);
            CLOSE_DISPS(play->state.gfxCtx);
            break;
        case 1:
            Gfx_DrawDListXlu(play, dLists[params]);
            break;
        case 2:
            Gfx_DrawDListOpa(play, dLists[params]);
            break;
    }
}
