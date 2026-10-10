/*
 * File: z_bg_haka_tomb.c
 * Overlay: ovl_Bg_Haka_Tomb
 * Description: Flat's Tomb
 */

#include "z_bg_haka_tomb.h"
#include "objects/object_haka_obj/object_haka_obj.h"

#define FLAGS 0x00000000

void BgHakaTomb_Init(Actor* thisx, PlayState* play);
void BgHakaTomb_Destroy(Actor* thisx, PlayState* play);
void BgHakaTomb_Update(Actor* thisx, PlayState* play);
void BgHakaTomb_Draw(Actor* thisx, PlayState* play);

void func_80BD6624(BgHakaTomb* this);
void func_80BD66AC(BgHakaTomb* this, PlayState* play);
void func_80BD6754(BgHakaTomb* this);
void func_80BD6768(BgHakaTomb* this, PlayState* play);
void BgHakaTomb_SetupDoNothing(BgHakaTomb* this);
void BgHakaTomb_DoNothing(BgHakaTomb* this, PlayState* play);

ActorProfile Bg_Haka_Tomb_Profile = {
    /**/ ACTOR_BG_HAKA_TOMB,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKA_OBJ,
    /**/ sizeof(BgHakaTomb),
    /**/ BgHakaTomb_Init,
    /**/ BgHakaTomb_Destroy,
    /**/ BgHakaTomb_Update,
    /**/ BgHakaTomb_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static Vec3f D_80BD68A4 = { 30.0f, 90.0f, 0.0f };

void BgHakaTomb_Init(Actor* thisx, PlayState* play) {
    BgHakaTomb* this = (BgHakaTomb*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_haka_obj_Colheader_000EE8);
    SubS_FillCutscenesList(&this->dyna.actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    func_80BD6624(this);
}

void BgHakaTomb_Destroy(Actor* thisx, PlayState* play) {
    BgHakaTomb* this = (BgHakaTomb*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80BD6624(BgHakaTomb* this) {
    this->actionFunc = func_80BD66AC;
}

s32 func_80BD6638(s16* csId, s16* csIdList, s32 numCutscenes) {
    s32 pad;
    s32 retVal = false;
    s32 i;

    *csId = CutsceneManager_GetCurrentCsId();
    if (*csId > CS_ID_NONE) {
        for (i = 0; i < numCutscenes; i++) {
            if (*csId == csIdList[i]) {
                retVal = true;
                break;
            }
        }
    }

    return retVal;
}

void func_80BD66AC(BgHakaTomb* this, PlayState* play) {
    s16 csId;

    if (Flags_GetClear(play, this->dyna.actor.room)) {
        this->dyna.actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }
    if (!func_80BD6638(&csId, this->csIdList, ARRAY_COUNT(this->csIdList)) && (csId <= CS_ID_NONE) &&
        Flags_GetClear(play, this->dyna.actor.room)) {
        this->dyna.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        if (this->dyna.actor.isLockedOn) {
            func_80BD6754(this);
        }
    } else {
        this->dyna.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void func_80BD6754(BgHakaTomb* this) {
    this->actionFunc = func_80BD6768;
}

void func_80BD6768(BgHakaTomb* this, PlayState* play) {
    if (SubS_StartCutscene(&this->dyna.actor, this->csIdList[0], CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
        BgHakaTomb_SetupDoNothing(this);
    }
}

void BgHakaTomb_SetupDoNothing(BgHakaTomb* this) {
    this->actionFunc = BgHakaTomb_DoNothing;
}

void BgHakaTomb_DoNothing(BgHakaTomb* this, PlayState* play) {
}

void BgHakaTomb_Update(Actor* thisx, PlayState* play) {
    BgHakaTomb* this = (BgHakaTomb*)thisx;
    s32 pad;
    Vec3f vec;

    this->actionFunc(this, play);
    Matrix_RotateYS(this->dyna.actor.world.rot.y, MTXMODE_NEW);
    Matrix_RotateXS(this->dyna.actor.world.rot.x, MTXMODE_APPLY);
    Matrix_RotateZS(this->dyna.actor.world.rot.z, MTXMODE_APPLY);
    Matrix_MultVec3f(&D_80BD68A4, &vec);
    Math_Vec3f_Sum(&this->dyna.actor.world.pos, &vec, &this->dyna.actor.focus.pos);
}

void BgHakaTomb_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_haka_obj_DL_0007B0);
}
