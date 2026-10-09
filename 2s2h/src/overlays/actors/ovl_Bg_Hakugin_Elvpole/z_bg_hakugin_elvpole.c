/*
 * File: z_bg_hakugin_elvpole.c
 * Overlay: ovl_Bg_Hakugin_Elvpole
 * Description: Raisable pillar in Snowhead Temple Map room
 */

#include "z_bg_hakugin_elvpole.h"
#include "objects/object_hakugin_obj/object_hakugin_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgHakuginElvpole_Init(Actor* thisx, PlayState* play);
void BgHakuginElvpole_Destroy(Actor* thisx, PlayState* play);
void BgHakuginElvpole_Update(Actor* thisx, PlayState* play);
void BgHakuginElvpole_Draw(Actor* thisx, PlayState* play);

void func_80ABD92C(BgHakuginElvpole* this, PlayState* play);

ActorProfile Bg_Hakugin_Elvpole_Profile = {
    /**/ ACTOR_BG_HAKUGIN_ELVPOLE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKUGIN_OBJ,
    /**/ sizeof(BgHakuginElvpole),
    /**/ BgHakuginElvpole_Init,
    /**/ BgHakuginElvpole_Destroy,
    /**/ BgHakuginElvpole_Update,
    /**/ BgHakuginElvpole_Draw,
};

void BgHakuginElvpole_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    BgHakuginElvpole* this = (BgHakuginElvpole*)thisx;

    Actor_SetScale(&this->dyna.actor, 0.1f);
    this->actionFunc = func_80ABD92C;
    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&object_hakugin_obj_Colheader_00BF40, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->unk_15C = 0;
    this->unk_160 = false;
    if ((BGHAKUGINELVPOLE_GET_SWITCH_FLAG(&this->dyna.actor) != 0x7F) &&
        Flags_GetSwitch(play, BGHAKUGINELVPOLE_GET_SWITCH_FLAG(&this->dyna.actor))) {
        this->unk_15E = 0x64;
    } else {
        this->unk_15E = -1;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 320.0f;
    }
}

void BgHakuginElvpole_Destroy(Actor* thisx, PlayState* play) {
    BgHakuginElvpole* this = (BgHakuginElvpole*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80ABD92C(BgHakuginElvpole* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    s32 sp28 = false;
    f32 var_fv1;

    if (this->unk_15E > 0) {
        if ((thisx->world.pos.y - thisx->home.pos.y) < -120.0f) {
            thisx->world.pos.y += 4.0f;
            sp28 = true;
        } else if (this->unk_15E > 0) {
            if (this->unk_15E == 100) {
                Actor_PlaySfx(thisx, NA_SE_EV_STONEDOOR_STOP);
            }
            thisx->world.pos.y = thisx->home.pos.y - 120.0f;

            this->unk_15E--;
        }
    } else if (this->unk_15E == 0) {
        if ((thisx->world.pos.y - thisx->home.pos.y) > -320.0f) {
            thisx->world.pos.y -= 4.0f;
            sp28 = true;
        } else {
            thisx->world.pos.y = thisx->home.pos.y - 320.0f;
            Flags_UnsetSwitch(play, BGHAKUGINELVPOLE_GET_SWITCH_FLAG(thisx));
            this->unk_15E = -1;
            Actor_PlaySfx(thisx, NA_SE_EV_STONEDOOR_STOP);
        }
    } else if ((BGHAKUGINELVPOLE_GET_SWITCH_FLAG(thisx) != 0x7F) &&
               Flags_GetSwitch(play, BGHAKUGINELVPOLE_GET_SWITCH_FLAG(thisx))) {
        this->unk_15E = 0x64;
        this->unk_160 = true;
    }

    if (sp28 || (this->unk_15C & 7)) {
        if (this->unk_15C & 1) {
            var_fv1 = 1.0f;
        } else {
            var_fv1 = -1.0f;
        }
        this->unk_15C++;
        thisx->world.pos.x = (Math_SinS(this->unk_15C * 0x2000) * var_fv1) + thisx->home.pos.x;
        thisx->world.pos.z = (Math_CosS(this->unk_15C * 0x2000) * var_fv1) + thisx->home.pos.z;
        Actor_PlaySfx_Flagged(thisx, NA_SE_EV_PLATE_LIFT_LEVEL - SFX_FLAG);
    } else {
        this->unk_15C = 0;
    }

    if (this->unk_160) {
        if (thisx->csId == CS_ID_NONE) {
            this->unk_160 = false;
            return;
        } else if (CutsceneManager_IsNext(thisx->csId)) {
            CutsceneManager_StartWithPlayerCs(thisx->csId, thisx);
            this->unk_160 = false;
            return;
        }
        CutsceneManager_Queue(thisx->csId);
    }
}

void BgHakuginElvpole_Update(Actor* thisx, PlayState* play) {
    BgHakuginElvpole* this = (BgHakuginElvpole*)thisx;

    this->actionFunc(this, play);
}

void BgHakuginElvpole_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, object_hakugin_obj_DL_00ACB8);

    CLOSE_DISPS(play->state.gfxCtx);
}
