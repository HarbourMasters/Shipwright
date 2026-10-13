/*
 * File: z_obj_tokei_turret.c
 * Overlay: ovl_Obj_Tokei_Turret
 * Description: South Clock Town - Flags & Carnival Platform
 */

#include "z_obj_tokei_turret.h"
#include "objects/object_tokei_turret/object_tokei_turret.h"

#define FLAGS 0x00000000

void ObjTokeiTurret_Init(Actor* thisx, PlayState* play);
void ObjTokeiTurret_Destroy(Actor* thisx, PlayState* play);
void ObjTokeiTurret_Update(Actor* thisx, PlayState* play);
void ObjTokeiTurret_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Tokei_Turret_Profile = {
    /**/ ACTOR_OBJ_TOKEI_TURRET,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_TOKEI_TURRET,
    /**/ sizeof(ObjTokeiTurret),
    /**/ ObjTokeiTurret_Init,
    /**/ ObjTokeiTurret_Destroy,
    /**/ ObjTokeiTurret_Update,
    /**/ ObjTokeiTurret_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void ObjTokeiTurret_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjTokeiTurret* this = (ObjTokeiTurret*)thisx;
    s32 tier;

    tier = OBJ_TOKEI_TURRET_TIER_TYPE(thisx);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);

    if ((tier == TURRET_TIER_BASE) || (tier == TURRET_TIER_TOP)) {
        this->dyna.actor.cullingVolumeDownward = this->dyna.actor.cullingVolumeScale = 240.0f;

        if (tier == TURRET_TIER_BASE) {
            DynaPolyActor_LoadMesh(play, &this->dyna, &gClockTownTurretBaseCol);
        } else {
            DynaPolyActor_LoadMesh(play, &this->dyna, &gClockTownTurretPlatformCol);
        }
    } else {
        this->dyna.actor.cullingVolumeDownward = this->dyna.actor.cullingVolumeScale = 1300.0f;
    }
}

void ObjTokeiTurret_Destroy(Actor* thisx, PlayState* play) {
    ObjTokeiTurret* this = (ObjTokeiTurret*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjTokeiTurret_Update(Actor* thisx, PlayState* play) {
}

void ObjTokeiTurret_Draw(Actor* thisx, PlayState* play) {
    ObjTokeiTurret* this = (ObjTokeiTurret*)thisx;
    Gfx* gfx;

    if (OBJ_TOKEI_TURRET_TIER_TYPE(thisx) == TURRET_TIER_TOP) {
        OPEN_DISPS(play->state.gfxCtx);

        gfx = POLY_OPA_DISP;
        gSPDisplayList(gfx++, gSetupDLs[SETUPDL_25]);
        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
        gSPDisplayList(gfx++, gClockTownTurretPlatformTopDL);
        POLY_OPA_DISP = gfx;

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (OBJ_TOKEI_TURRET_TIER_TYPE(thisx) == TURRET_TIER_BASE) {
        Gfx_DrawDListOpa(play, gClockTownTurretPlatformBaseDL);
    } else {
        Gfx_DrawDListOpa(play, gClockTownFlagsDL);
    }
}
