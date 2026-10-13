/*
 * File: z_bg_goron_oyu.c
 * Overlay: ovl_Bg_Goron_Oyu
 * Description: Goron Hot Spring Water
 */

#include "z_bg_goron_oyu.h"
#include "objects/object_oyu/object_oyu.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgGoronOyu_Init(Actor* thisx, PlayState* play);
void BgGoronOyu_Destroy(Actor* thisx, PlayState* play);
void BgGoronOyu_Update(Actor* thisx, PlayState* play);
void BgGoronOyu_Draw(Actor* thisx, PlayState* play);

void func_80B40100(BgGoronOyu* this, PlayState* play);
void func_80B400C8(BgGoronOyu* this, PlayState* play);
void func_80B401F8(BgGoronOyu* this, PlayState* play);
void BgGoronOyu_UpdateWaterBoxInfo(BgGoronOyu* this, PlayState* play);
void BgGoronOyu_SpawnEffects(BgGoronOyu* this, PlayState* play);
void func_80B40160(BgGoronOyu* this, PlayState* play);

ActorProfile Bg_Goron_Oyu_Profile = {
    /**/ ACTOR_BG_GORON_OYU,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_OYU,
    /**/ sizeof(BgGoronOyu),
    /**/ BgGoronOyu_Init,
    /**/ BgGoronOyu_Destroy,
    /**/ BgGoronOyu_Update,
    /**/ BgGoronOyu_Draw,
};

void func_80B40080(BgGoronOyu* this) {
    this->unk_17E = 1;
    this->actionFunc = func_80B400C8;
}

void func_80B4009C(BgGoronOyu* this) {
    this->unk_17E = 0;
    this->initCsId = this->dyna.actor.csId;
    this->actionFunc = func_80B40100;
    this->unk_164 = 20.0f;
}

void func_80B400C8(BgGoronOyu* this, PlayState* play) {
    BgGoronOyu_UpdateWaterBoxInfo(this, play);
    func_80B401F8(this, play);
}

void func_80B40100(BgGoronOyu* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->initCsId)) {
        CutsceneManager_StartWithPlayerCs(this->initCsId, &this->dyna.actor);
        this->actionFunc = func_80B40160;
    } else {
        CutsceneManager_Queue(this->initCsId);
    }
}

void func_80B40160(BgGoronOyu* this, PlayState* play) {
    static Vec3f D_80B40780 = { 0, 0, 0 };

    Math_StepToF(&this->unk_164, 0.0f, 0.2f);
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - this->unk_164;
    BgGoronOyu_UpdateWaterBoxInfo(this, play);

    if (this->unk_164 <= 0.0f) {
        CutsceneManager_Stop(this->initCsId);
        this->unk_164 = 0.0f;
        func_80B40080(this);
    }

    Audio_PlaySfx_AtPos(&D_80B40780, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
}

void func_80B401F8(BgGoronOyu* this, PlayState* play) {
    Player* player;
    Vec3f dist;

    if (Actor_HasParent(&this->dyna.actor, play)) {
        this->dyna.actor.parent = NULL;
        return;
    }

    player = GET_PLAYER(play);
    Math_Vec3f_DistXYZAndStoreDiff(&this->waterBoxPos, &player->actor.world.pos, &dist);

    if ((dist.x >= 0.0f) && (dist.x <= this->waterBoxXLength) && (dist.z >= 0.0f) &&
        (dist.z <= this->waterBoxZLength) && (fabsf(dist.y) < 100.0f) && (player->actor.depthInWater > 12.0f)) {
        Actor_OfferGetItem(&this->dyna.actor, play, GI_MAX, this->dyna.actor.xzDistToPlayer,
                           fabsf(this->dyna.actor.playerHeightRel));
    }
}

void BgGoronOyu_UpdateWaterBoxInfo(BgGoronOyu* this, PlayState* play) {
    WaterBox* waterBox;
    f32 ySurface;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.z, &ySurface,
                             &waterBox)) {
        Math_Vec3s_ToVec3f(&this->waterBoxPos, &waterBox->minPos);
        this->waterBoxXLength = waterBox->xLength;
        this->waterBoxZLength = waterBox->zLength;
    }
}

void BgGoronOyu_SpawnEffects(BgGoronOyu* this, PlayState* play) {
    s16 scale;
    Vec3f pos1;
    Vec3f pos2;
    CollisionPoly* poly;
    s32 pad;

    if ((play->state.frames % 4) == 0) {
        Vec3f vel1;
        Vec3f vel2;

        pos1.x = this->dyna.actor.world.pos.x;
        pos1.y = this->dyna.actor.world.pos.y + 100.0f;
        pos1.z = this->dyna.actor.world.pos.z;
        vel1.x = 0.0f;
        vel1.y = 2.5f;
        vel1.z = 0.0f;
        scale = -200 - (s32)(Rand_ZeroOne() * 50.0f);

        if (BgCheck_EntityRaycastFloor2(play, &play->colCtx, &poly, &pos1) < this->waterBoxPos.y) {
            pos1.y = this->waterBoxPos.y + 10.0f;
            EffectSsIceSmoke_Spawn(play, &pos1, &vel1, &gZeroVec3f, scale);
        }
        pos2.x = (Rand_ZeroOne() * this->waterBoxXLength) + this->waterBoxPos.x;
        pos2.y = this->waterBoxPos.y + 100.0f;
        pos2.z = (Rand_ZeroOne() * this->waterBoxZLength) + this->waterBoxPos.z;
        vel2.x = 0.0f;
        vel2.y = 0.5f;
        vel2.z = 0.0f;
        scale = -200 - (s32)(Rand_ZeroOne() * 50.0f);

        if (BgCheck_EntityRaycastFloor2(play, &play->colCtx, &poly, &pos2) < this->waterBoxPos.y) {
            pos2.y = this->waterBoxPos.y + 10.0f;
            EffectSsIceSmoke_Spawn(play, &pos2, &vel2, &gZeroVec3f, scale);
        }
    }
}

void BgGoronOyu_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGoronOyu* this = (BgGoronOyu*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_SetScale(&this->dyna.actor, 0.1f);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    CollisionHeader_GetVirtual(&gGoronGraveyardHotSpringWaterCol, &colHeader);

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    BgGoronOyu_UpdateWaterBoxInfo(this, play);

    if (thisx->params != 0) {
        thisx->world.pos.y = thisx->home.pos.y;
        func_80B40080(this);
    } else {
        thisx->world.pos.y = thisx->home.pos.y - 20.0f;
        func_80B4009C(this);
    }
}

void BgGoronOyu_Destroy(Actor* thisx, PlayState* play) {
    BgGoronOyu* this = (BgGoronOyu*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgGoronOyu_Update(Actor* thisx, PlayState* play) {
    BgGoronOyu* this = (BgGoronOyu*)thisx;

    this->actionFunc(this, play);
    BgGoronOyu_SpawnEffects(this, play);
}

void BgGoronOyu_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gGoronGraveyardHotSpringWaterTexAnim));
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, &gGoronGraveyardHotSpringWaterEmptyDL);
    gSPDisplayList(POLY_XLU_DISP++, &gGoronGraveyardHotSpringWaterDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
