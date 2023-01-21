/*
 * File: z_bg_spot17_bakudankabe.c
 * Overlay: ovl_Bg_Spot17_Bakudankabe
 * Description: Death Mountain Crater Bombable Wall
 */

#include "z_bg_spot17_bakudankabe.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "objects/object_spot17_obj/object_spot17_obj.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS 0

void BgSpot17Bakudankabe_Init(Actor* thisx, PlayState* play);
void BgSpot17Bakudankabe_Destroy(Actor* thisx, PlayState* play);
void BgSpot17Bakudankabe_Update(Actor* thisx, PlayState* play);
void BgSpot17Bakudankabe_Draw(Actor* thisx, PlayState* play);

const ActorInit Bg_Spot17_Bakudankabe_InitVars = {
    ACTOR_BG_SPOT17_BAKUDANKABE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT17_OBJ,
    sizeof(BgSpot17Bakudankabe),
    (ActorFunc)BgSpot17Bakudankabe_Init,
    (ActorFunc)BgSpot17Bakudankabe_Destroy,
    (ActorFunc)BgSpot17Bakudankabe_Update,
    (ActorFunc)BgSpot17Bakudankabe_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void func_808B6BC0(BgSpot17Bakudankabe* this, PlayState* play) {
    s32 pad[2];
    s32 i;
    Vec3f burstDepthY;
    Vec3f burstDepthX;
    f32 sinY;
    f32 cosY;

    sinY = Math_SinS(this->dyna.actor.shape.rot.y);
    cosY = Math_CosS(this->dyna.actor.shape.rot.y);

    burstDepthX.z = 0.0f;
    burstDepthX.x = 0.0f;

    for (i = 0; i < 20; i++) {
        s16 gravityInfluence;
        s16 scale;
        f32 temp1;
        f32 temp2;
        s32 rotationSpeed;

        temp1 = (Rand_ZeroOne() - 0.5f) * 140.0f;
        temp2 = (Rand_ZeroOne() - 0.5f) * 20.0f;

        burstDepthY.x = this->dyna.actor.world.pos.x + temp2 * sinY + (temp1 * cosY);
        burstDepthY.y = this->dyna.actor.world.pos.y + 30.0f + (i * 6.5f);
        burstDepthY.z = this->dyna.actor.world.pos.z + temp2 * cosY - (temp1 * sinY);

        burstDepthX.y = (Rand_ZeroOne() - 0.2f) * 12.0f;
        scale = Rand_ZeroOne() * 55.0f + 8.0f;

        if (scale < 20) {
            gravityInfluence = -300;
        } else if (scale < 35) {
            gravityInfluence = -360;
        } else {
            gravityInfluence = -420;
        }

        if (Rand_ZeroOne() < 0.4f) {
            rotationSpeed = 65;
        } else {
            rotationSpeed = 33;
        }
        EffectSsKakera_Spawn(play, &burstDepthY, &burstDepthX, &burstDepthY, gravityInfluence, rotationSpeed, 0x1E,
                             4, 0, scale, 1, 3, 80, KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_FIELD_KEEP, gFieldKakeraDL);
    }
    Math_Vec3f_Copy(&burstDepthY, &this->dyna.actor.world.pos);
    func_80033480(play, &burstDepthY, 60.0f, 4, 110, 160, 1);
    burstDepthY.y += 40.0f;
    func_80033480(play, &burstDepthY, 60.0f, 4, 120, 160, 1);
    burstDepthY.y += 40.0f;
    func_80033480(play, &burstDepthY, 60.0f, 4, 110, 160, 1);
}

void BgSpot17Bakudankabe_Init(Actor* thisx, PlayState* play) {
    BgSpot17Bakudankabe* this = (BgSpot17Bakudankabe*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    if (Flags_GetSwitch(play, (this->dyna.actor.params & 0x3F))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    CollisionHeader_GetVirtual(&gCraterBombableWallCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
}

void BgSpot17Bakudankabe_Destroy(Actor* thisx, PlayState* play) {
    BgSpot17Bakudankabe* this = (BgSpot17Bakudankabe*)thisx;
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgSpot17Bakudankabe_Update(Actor* thisx, PlayState* play) {
    BgSpot17Bakudankabe* this = (BgSpot17Bakudankabe*)thisx;
    if (this->dyna.actor.xzDistToPlayer < 650.0f && func_80033684(play, &this->dyna.actor) != NULL) {
        func_808B6BC0(this, play);
        Flags_SetSwitch(play, (this->dyna.actor.params & 0x3F));
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        Actor_Kill(&this->dyna.actor);
    }
}

void BgSpot17Bakudankabe_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    s8 r = coss(play->gameplayFrames * 1500) >> 8;
    s8 g = coss(play->gameplayFrames * 1500) >> 8;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    r = (r >> 1) + 0xC0;
    g = (g >> 1) + 0xC0;

    gDPSetEnvColor(POLY_OPA_DISP++, r, g, 255, 128);

    gSPDisplayList(POLY_OPA_DISP++, gCraterBombableWallDL);

    CLOSE_DISPS(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gCraterBombableWallCracksDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
