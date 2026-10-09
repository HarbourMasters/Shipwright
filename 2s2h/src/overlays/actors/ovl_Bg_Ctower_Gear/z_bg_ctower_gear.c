/*
 * File: z_bg_ctower_gear.c
 * Overlay: ovl_Bg_Ctower_Gear
 * Description: Different Cogs/Organ inside Clock Tower
 */

#include "z_bg_ctower_gear.h"
#include "objects/object_ctower_rot/object_ctower_rot.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgCtowerGear_Init(Actor* thisx, PlayState* play);
void BgCtowerGear_Destroy(Actor* thisx, PlayState* play);
void BgCtowerGear_Update(Actor* thisx, PlayState* play);
void BgCtowerGear_Draw(Actor* thisx, PlayState* play);

void BgCtowerGear_UpdateOrgan(Actor* thisx, PlayState* play);
void BgCtowerGear_DrawOrgan(Actor* thisx, PlayState* play);

ActorProfile Bg_Ctower_Gear_Profile = {
    /**/ ACTOR_BG_CTOWER_GEAR,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_CTOWER_ROT,
    /**/ sizeof(BgCtowerGear),
    /**/ BgCtowerGear_Init,
    /**/ BgCtowerGear_Destroy,
    /**/ BgCtowerGear_Update,
    /**/ BgCtowerGear_Draw,
};

static Vec3f sExitSplashOffsets[] = {
    { -70.0f, -60.0f, 8.0f },
    { -60.0f, -60.0f, -9.1f },
    { -75.0f, -60.0f, -9.1f },
    { -70.0f, -60.0f, -26.2f },
};

static Vec3f sEnterSplashOffsets[] = {
    { 85.0f, -60.0f, 8.0f },
    { 80.0f, -60.0f, -9.1f },
    { 85.0f, -60.0f, -26.2f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_STOP),
};

static InitChainEntry sInitChainCenterCog[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2000, ICHAIN_STOP),
};

static InitChainEntry sInitChainOrgan[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 420, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 570, ICHAIN_STOP),
};

static Gfx* sDLists[] = { gClockTowerCeilingCogDL, gClockTowerCenterCogDL, gClockTowerWaterWheelDL };

void BgCtowerGear_Splash(BgCtowerGear* this, PlayState* play) {
    s32 i;
    s32 flag40 = this->dyna.actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME;
    Vec3f splashSpawnPos;
    Vec3f splashOffset;
    s32 pad;
    s32 j;
    s16 rotZ = this->dyna.actor.shape.rot.z & 0x1FFF;

    if (flag40 && (rotZ < 0x1B58) && (rotZ >= 0x1388)) {
        Matrix_RotateYS(this->dyna.actor.home.rot.y, MTXMODE_NEW);
        Matrix_RotateXS(this->dyna.actor.home.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->dyna.actor.home.rot.z, MTXMODE_APPLY);
        for (i = 0; i < 4; i++) {
            if (Rand_Next() >= 0x40000000) {
                splashOffset.x = sExitSplashOffsets[i].x - (Rand_ZeroOne() * 30.0f);
                splashOffset.y = sExitSplashOffsets[i].y;
                splashOffset.z = sExitSplashOffsets[i].z;
                Matrix_MultVec3f(&splashOffset, &splashSpawnPos);
                splashSpawnPos.x += this->dyna.actor.world.pos.x + ((Rand_ZeroOne() * 20.0f) - 10.0f);
                splashSpawnPos.y += this->dyna.actor.world.pos.y;
                splashSpawnPos.z += this->dyna.actor.world.pos.z + ((Rand_ZeroOne() * 20.0f) - 10.0f);
                EffectSsGSplash_Spawn(play, &splashSpawnPos, NULL, NULL, 0, (Rand_Next() >> 25) + 340);
            }
        }
    }
    if ((rotZ < 0x1F4) && (rotZ >= 0)) {
        if (flag40) {
            Matrix_RotateYS(this->dyna.actor.home.rot.y, MTXMODE_NEW);
            Matrix_RotateXS(this->dyna.actor.home.rot.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->dyna.actor.home.rot.z, MTXMODE_APPLY);
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 2; j++) {
                    splashOffset.x = sEnterSplashOffsets[i].x + (Rand_ZeroOne() * 10.0f);
                    splashOffset.y = sEnterSplashOffsets[i].y;
                    splashOffset.z = sEnterSplashOffsets[i].z;
                    Matrix_MultVec3f(&splashOffset, &splashSpawnPos);
                    splashSpawnPos.x += this->dyna.actor.world.pos.x + ((Rand_ZeroOne() * 20.0f) - 10.0f);
                    splashSpawnPos.y += this->dyna.actor.world.pos.y;
                    splashSpawnPos.z += this->dyna.actor.world.pos.z + ((Rand_ZeroOne() * 20.0f) - 10.0f);
                    EffectSsGSplash_Spawn(play, &splashSpawnPos, NULL, NULL, 0, (Rand_Next() >> 25) + 280);
                }
            }
        }
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WATERWHEEL_LEVEL);
    }
}

void BgCtowerGear_Init(Actor* thisx, PlayState* play) {
    BgCtowerGear* this = (BgCtowerGear*)thisx;
    s32 type = BGCTOWERGEAR_GET_TYPE(&this->dyna.actor);

    Actor_SetScale(&this->dyna.actor, 0.1f);
    if (type == BGCTOWERGEAR_CENTER_COG) {
        Actor_ProcessInitChain(&this->dyna.actor, sInitChainCenterCog);
    } else if (type == BGCTOWERGEAR_ORGAN) {
        Actor_ProcessInitChain(&this->dyna.actor, sInitChainOrgan);
        this->dyna.actor.draw = NULL;
        this->dyna.actor.update = BgCtowerGear_UpdateOrgan;
    } else {
        Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    }
    if (type == BGCTOWERGEAR_WATER_WHEEL) {
        DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
        DynaPolyActor_LoadMesh(play, &this->dyna, &gClockTowerWaterWheelCol);
    } else if (type == BGCTOWERGEAR_ORGAN) {
        DynaPolyActor_Init(&this->dyna, 0);
        DynaPolyActor_LoadMesh(play, &this->dyna, &gClockTowerOrganCol);
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void BgCtowerGear_Destroy(Actor* thisx, PlayState* play) {
    BgCtowerGear* this = (BgCtowerGear*)thisx;
    s32 type = BGCTOWERGEAR_GET_TYPE(&this->dyna.actor);

    if ((type == BGCTOWERGEAR_WATER_WHEEL) || (type == BGCTOWERGEAR_ORGAN)) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void BgCtowerGear_Update(Actor* thisx, PlayState* play) {
    BgCtowerGear* this = (BgCtowerGear*)thisx;
    s32 type = BGCTOWERGEAR_GET_TYPE(&this->dyna.actor);

    if (type == BGCTOWERGEAR_CEILING_COG) {
        this->dyna.actor.shape.rot.x -= 0x1F4;
    } else if (type == BGCTOWERGEAR_CENTER_COG) {
        this->dyna.actor.shape.rot.y += 0x1F4;
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_WINDMILL_LEVEL - SFX_FLAG);
    } else if (type == BGCTOWERGEAR_WATER_WHEEL) {
        this->dyna.actor.shape.rot.z -= 0x1F4;
        BgCtowerGear_Splash(this, play);
    }
}

void BgCtowerGear_UpdateOrgan(Actor* thisx, PlayState* play) {
    BgCtowerGear* this = (BgCtowerGear*)thisx;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_104)) {
        switch (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_104)]->id) {
            case 1:
                this->dyna.actor.draw = NULL;
                DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
                break;

            case 2:
                this->dyna.actor.draw = BgCtowerGear_DrawOrgan;
                DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
                break;

            case 3:
                Actor_Kill(&this->dyna.actor);
                break;

            default:
                break;
        }
    }
}

void BgCtowerGear_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, sDLists[BGCTOWERGEAR_GET_TYPE(thisx)]);
}

void BgCtowerGear_DrawOrgan(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gClockTowerOrganDL);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gClockTowerOrganPipesDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
