/*
 * File: z_bg_jya_kanaami.c
 * Overlay: ovl_Bg_Jya_Kanaami
 * Description: Climbable grating/bridge (Spirit Temple)
 */

#include "z_bg_jya_kanaami.h"
#include "objects/object_jya_obj/object_jya_obj.h"

#define FLAGS 0

void BgJyaKanaami_Init(Actor* thisx, PlayState* play);
void BgJyaKanaami_Destroy(Actor* thisx, PlayState* play);
void BgJyaKanaami_Update(Actor* thisx, PlayState* play);
void BgJyaKanaami_Draw(Actor* thisx, PlayState* play);

void func_80899880(BgJyaKanaami* this);
void func_80899894(BgJyaKanaami* this, PlayState* play);
void func_8089993C(BgJyaKanaami* this);
void func_80899950(BgJyaKanaami* this, PlayState* play);
void func_80899A08(BgJyaKanaami* this);

const ActorInit Bg_Jya_Kanaami_InitVars = {
    ACTOR_BG_JYA_KANAAMI,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJyaKanaami),
    (ActorFunc)BgJyaKanaami_Init,
    (ActorFunc)BgJyaKanaami_Destroy,
    (ActorFunc)BgJyaKanaami_Update,
    (ActorFunc)BgJyaKanaami_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgJyaKanaami_InitDynaPoly(BgJyaKanaami* this, PlayState* play, CollisionHeader* collision, s32 flag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&this->dyna, flag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void BgJyaKanaami_Init(Actor* thisx, PlayState* play) {
    BgJyaKanaami* this = (BgJyaKanaami*)thisx;

    BgJyaKanaami_InitDynaPoly(this, play, &gKanaamiCol, DPM_UNK);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    if (Flags_GetSwitch(play, this->dyna.actor.params & 0x3F)) {
        func_80899A08(this);
    } else {
        func_80899880(this);
    }
    osSyncPrintf("(jya 金網)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void BgJyaKanaami_Destroy(Actor* thisx, PlayState* play) {
    BgJyaKanaami* this = (BgJyaKanaami*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80899880(BgJyaKanaami* this) {
    this->actionFunc = func_80899894;
    this->unk_16A = 0;
}

void func_80899894(BgJyaKanaami* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->dyna.actor.params & 0x3F) || this->unk_16A > 0) {
        if (this->dyna.actor.world.pos.x > -1000.0f && this->unk_16A == 0) {
            OnePointCutscene_Init(play, 3450, -99, &this->dyna.actor, MAIN_CAM);
        }
        this->unk_16A += 1;
        if (this->unk_16A >= 0xA) {
            func_8089993C(this);
        }
    }
}

void func_8089993C(BgJyaKanaami* this) {
    this->actionFunc = func_80899950;
    this->unk_168 = 0;
}

void func_80899950(BgJyaKanaami* this, PlayState* play) {
    s32 pad[2];
    s32 quakeId;

    this->unk_168 += 0x20;
    if (Math_ScaledStepToS(&this->dyna.actor.world.rot.x, 0x4000, this->unk_168)) {
        func_80899A08(this);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_TRAP_BOUND);
        quakeId = Quake_Add(GET_ACTIVE_CAM(play), 3);
        Quake_SetSpeed(quakeId, 25000);
        Quake_SetQuakeValues(quakeId, 2, 0, 0, 0);
        Quake_SetCountdown(quakeId, 16);
    }
}

void func_80899A08(BgJyaKanaami* this) {
    this->actionFunc = 0;
    this->dyna.actor.world.rot.x = 0x4000;
}

void BgJyaKanaami_Update(Actor* thisx, PlayState* play) {
    BgJyaKanaami* this = (BgJyaKanaami*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
    this->dyna.actor.shape.rot.x = this->dyna.actor.world.rot.x;
}

void BgJyaKanaami_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gKanaamiDL);
}
