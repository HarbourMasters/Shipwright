/*
 * File: z_bg_spot12_saku.c
 * Overlay: ovl_Bg_Spot12_Saku
 * Description:
 */

#include "z_bg_spot12_saku.h"
#include "objects/object_spot12_obj/object_spot12_obj.h"

#define FLAGS 0

void BgSpot12Saku_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot12Saku_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot12Saku_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot12Saku_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808B3550(BgSpot12Saku* this);
void func_808B357C(BgSpot12Saku* this, GlobalContext* globalCtx);
void func_808B35E4(BgSpot12Saku* this);
void func_808B3604(BgSpot12Saku* this, GlobalContext* globalCtx);
void func_808B3714(BgSpot12Saku* this);
void func_808B37AC(BgSpot12Saku* this, GlobalContext* globalCtx);

const ActorInit Bg_Spot12_Saku_InitVars = {
    ACTOR_BG_SPOT12_SAKU,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT12_OBJ,
    sizeof(BgSpot12Saku),
    (ActorFunc)BgSpot12Saku_Init,
    (ActorFunc)BgSpot12Saku_Destroy,
    (ActorFunc)BgSpot12Saku_Update,
    (ActorFunc)BgSpot12Saku_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void func_808B3420(BgSpot12Saku* this, GlobalContext* globalCtx, CollisionHeader* collision, s32 flags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&this->dyna, flags);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void BgSpot12Saku_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot12Saku* this = (BgSpot12Saku*)thisx;

    // If ER is on, force the gate to always use its permanent flag
    // (which it only uses in Child Gerudo Fortress in the vanilla game)
    if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_DUNGEONS_ENTRANCES)) {
        thisx->params = 0x0002;
    }

    func_808B3420(this, globalCtx, &gGerudoFortressGTGShutterCol, DPM_UNK);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    if (Flags_GetSwitch(globalCtx, this->dyna.actor.params & 0x3F)) {
        func_808B3714(this);
    } else {
        func_808B3550(this);
    }
}

void BgSpot12Saku_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot12Saku* this = (BgSpot12Saku*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void func_808B3550(BgSpot12Saku* this) {
    this->actionFunc = func_808B357C;
    this->dyna.actor.scale.x = 0.1f;
    this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
}

void func_808B357C(BgSpot12Saku* this, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, this->dyna.actor.params & 0x3F)) {
        func_808B35E4(this);
        this->timer = 20;
        OnePointCutscene_Init(globalCtx, 4170, -99, &this->dyna.actor, MAIN_CAM);
    }
}

void func_808B35E4(BgSpot12Saku* this) {
    if (this->timer == 0) {
        this->actionFunc = func_808B3604;
    }
}

void func_808B3604(BgSpot12Saku* this, GlobalContext* globalCtx) {
    f32 temp_ret = Math_SmoothStepToF(&this->dyna.actor.scale.x, 0.001f / 0.14f, 0.16f, 0.0022f, 0.001f);
    f32 temp_f18 = ((0.1f - this->dyna.actor.scale.x) * 840.0f);

    this->dyna.actor.world.pos.x =
        this->dyna.actor.home.pos.x - (Math_SinS(this->dyna.actor.shape.rot.y + 0x4000) * temp_f18);
    this->dyna.actor.world.pos.z =
        this->dyna.actor.home.pos.z - (Math_CosS(this->dyna.actor.shape.rot.y + 0x4000) * temp_f18);
    if (fabsf(temp_ret) < 0.0001f) {
        func_808B3714(this);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
    } else {
        func_8002F974(&this->dyna.actor, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
    }
}

void func_808B3714(BgSpot12Saku* this) {
    this->actionFunc = func_808B37AC;
    this->dyna.actor.scale.x = 0.001f / 0.14f;
    this->dyna.actor.world.pos.x =
        this->dyna.actor.home.pos.x - (Math_SinS(this->dyna.actor.shape.rot.y + 0x4000) * 78.0f);
    this->dyna.actor.world.pos.z =
        this->dyna.actor.home.pos.z - (Math_CosS(this->dyna.actor.shape.rot.y + 0x4000) * 78.0f);
}

void func_808B37AC(BgSpot12Saku* this, GlobalContext* globalCtx) {
}

void BgSpot12Saku_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot12Saku* this = (BgSpot12Saku*)thisx;

    // If ER is on, when the guard opens the GtG gate its permanent flag will be set.
    if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_DUNGEONS_ENTRANCES) &&
        globalCtx->actorCtx.flags.tempSwch & 0x04000000) {
        globalCtx->actorCtx.flags.swch |= 0x00000004;
    }

    if (this->timer > 0) {
        this->timer--;
    }
    this->actionFunc(this, globalCtx);
}

void BgSpot12Saku_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gGerudoFortressGTGShutterDL);
}
