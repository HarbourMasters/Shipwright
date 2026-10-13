/*
 * File: z_bg_keikoku_saku.c
 * Overlay: ovl_Bg_Keikoku_Saku
 * Description: Termina Field Spiked Fence
 */

#include "z_bg_keikoku_saku.h"
#include "objects/object_keikoku_obj/object_keikoku_obj.h"

#define FLAGS 0x00000000

void BgKeikokuSaku_Init(Actor* thisx, PlayState* play);
void BgKeikokuSaku_Destroy(Actor* thisx, PlayState* play);
void BgKeikokuSaku_Update(Actor* thisx, PlayState* play);
void BgKeikokuSaku_Draw(Actor* thisx, PlayState* play);

void func_80A5389C(BgKeikokuSaku* this, PlayState* play);
void func_80A538E0(BgKeikokuSaku* this, PlayState* play);
void func_80A53994(BgKeikokuSaku* this, PlayState* play);

ActorProfile Bg_Keikoku_Saku_Profile = {
    /**/ ACTOR_BG_KEIKOKU_SAKU,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_KEIKOKU_OBJ,
    /**/ sizeof(BgKeikokuSaku),
    /**/ BgKeikokuSaku_Init,
    /**/ BgKeikokuSaku_Destroy,
    /**/ BgKeikokuSaku_Update,
    /**/ BgKeikokuSaku_Draw,
};

void BgKeikokuSaku_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgKeikokuSaku* this = (BgKeikokuSaku*)thisx;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&object_keikoku_obj_Colheader_002300, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->switchFlag = BGKEIKOKUSAKU_GET_SWITCH_FLAG(thisx);
    if (Flags_GetSwitch(play, this->switchFlag)) {
        this->dyna.actor.world.pos.z = 2659.0f;
    } else {
        this->actionFunc = func_80A5389C;
    }
}

void BgKeikokuSaku_Destroy(Actor* thisx, PlayState* play) {
    BgKeikokuSaku* this = (BgKeikokuSaku*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80A5389C(BgKeikokuSaku* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->switchFlag)) {
        this->actionFunc = func_80A538E0;
    }
}

void func_80A538E0(BgKeikokuSaku* this, PlayState* play) {
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
    this->dyna.actor.world.pos.z -= 2.0f + BREG(8);
    if (this->dyna.actor.world.pos.z < (BREG(9) + 2660.0f)) {
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
        this->timer = 30;
        this->actionFunc = func_80A53994;
    }
}

void func_80A53994(BgKeikokuSaku* this, PlayState* play) {
    if (this->timer == 0) {
        this->actionFunc = func_80A5389C;
    }
}

void BgKeikokuSaku_Update(Actor* thisx, PlayState* play) {
    BgKeikokuSaku* this = (BgKeikokuSaku*)thisx;

    if (this->timer) {
        this->timer--;
    }
    this->dyna.actor.world.pos.x = BREG(5) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.y = BREG(6) + this->dyna.actor.home.pos.y;
    this->dyna.actor.world.pos.z = BREG(7) + this->dyna.actor.home.pos.z;
    this->dyna.actor.scale.x = (BREG(10) / 1000.0f) + 0.1f;
    this->dyna.actor.scale.y = (BREG(11) / 1000.0f) + 0.1f;
    this->dyna.actor.scale.z = (BREG(12) / 1000.0f) + 0.1f;
    this->actionFunc(this, play);
}

void BgKeikokuSaku_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, object_keikoku_obj_DL_001640);

    CLOSE_DISPS(play->state.gfxCtx);
}
