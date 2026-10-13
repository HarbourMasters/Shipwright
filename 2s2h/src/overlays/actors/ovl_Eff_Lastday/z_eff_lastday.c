/*
 * File: z_eff_lastday.c
 * Overlay: ovl_Eff_Lastday
 * Description: Moon Crash Cutscene Fire Wall
 */

#include "z_eff_lastday.h"
#include "objects/object_lastday/object_lastday.h"

#include "2s2h/BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EffLastday_Init(Actor* thisx, PlayState* play2);
void EffLastday_Destroy(Actor* thisx, PlayState* play);
void EffLastday_Update(Actor* thisx, PlayState* play);
void EffLastday_Draw(Actor* thisx, PlayState* play);

void func_80BEBD0C(EffLastday* this, PlayState* play);
void func_80BEBDF8(EffLastday* this, PlayState* play);
void func_80BEBEB8(EffLastday* this, PlayState* play);
void func_80BEBF78(EffLastday* this, PlayState* play);

typedef enum EffLastDayAction {
    /* 0 */ EFFLASTDAY_ACTION_NONE,
    /* 1 */ EFFLASTDAY_ACTION_1,
    /* 2 */ EFFLASTDAY_ACTION_2,
    /* 3 */ EFFLASTDAY_ACTION_3
} EffLastDayAction;

ActorProfile Eff_Lastday_Profile = {
    /**/ ACTOR_EFF_LASTDAY,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_LASTDAY,
    /**/ sizeof(EffLastday),
    /**/ EffLastday_Init,
    /**/ EffLastday_Destroy,
    /**/ EffLastday_Update,
    /**/ EffLastday_Draw,
};

void EffLastday_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EffLastday* this = (EffLastday*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
    switch (EFFLASTDAY_GET_F(&this->actor)) {
        case EFFLASTDAY_PARAM_1:
            this->dList = object_lastday_DL_000510;
            this->matAnim = Lib_SegmentedToVirtual(object_lastday_Matanimheader_000608);
            this->cueType = CS_CMD_ACTOR_CUE_508;
            this->actionFunc = func_80BEBDF8;
            Actor_SetScale(&this->actor, 1.0f);
            break;

        case EFFLASTDAY_PARAM_2:
            this->dList = object_lastday_DL_000210;
            this->matAnim = Lib_SegmentedToVirtual(object_lastday_Matanimheader_000308);
            this->cueType = CS_CMD_ACTOR_CUE_509;
            this->actionFunc = func_80BEBEB8;
            Actor_SetScale(&this->actor, 0.5f);
            break;

        case EFFLASTDAY_PARAM_3:
            this->dList = object_lastday_DL_000060;
            this->matAnim = Lib_SegmentedToVirtual(object_lastday_Matanimheader_000148);
            this->cueType = CS_CMD_ACTOR_CUE_510;
            this->actionFunc = func_80BEBF78;
            Actor_SetScale(&this->actor, 0.2f);
            this->actor.home.rot.z = 0;
            break;

        default:
            this->dList = object_lastday_DL_000370;
            this->matAnim = Lib_SegmentedToVirtual(object_lastday_Matanimheader_000448);
            this->cueType = CS_CMD_ACTOR_CUE_507;
            this->actionFunc = func_80BEBD0C;
            Actor_SetScale(&this->actor, 1.0f);
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EFF_LASTDAY, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.world.rot.y, 0,
                               EFFLASTDAY_PARAM_1);
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EFF_LASTDAY, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.world.rot.y, 0,
                               EFFLASTDAY_PARAM_2);
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EFF_LASTDAY, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.world.rot.y, 0,
                               EFFLASTDAY_PARAM_3);
            break;
    }
}

void EffLastday_Destroy(Actor* thisx, PlayState* play) {
}

void func_80BEBD0C(EffLastday* this, PlayState* play) {
    u16 cueId;

    if ((Cutscene_IsCueInChannel(play, this->cueType))) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        switch (cueId) {
            default:
                this->actor.draw = NULL;
                this->alpha = 0;
                this->step = 0;
                break;

            case EFFLASTDAY_ACTION_2:
                this->actor.draw = NULL;
                this->actor.draw = EffLastday_Draw;
                if (this->alpha < 255) {
                    this->alpha += 10;
                }
                if (this->alpha > 255) {
                    this->alpha = 255;
                }
                this->step++;
                break;

            case EFFLASTDAY_ACTION_3:
                this->actor.draw = EffLastday_Draw;
                break;
        }
    } else {
        this->actor.draw = NULL;
    }
}

void func_80BEBDF8(EffLastday* this, PlayState* play) {
    u16 cueId;

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        switch (cueId) {
            default:
                this->actor.draw = NULL;
                this->step = 0;
                break;

            case EFFLASTDAY_ACTION_2:
                this->actor.draw = EffLastday_Draw;
                this->step++;
                break;

            case EFFLASTDAY_ACTION_3:
                this->actor.draw = EffLastday_Draw;
                break;
        }
    } else {
        this->actor.draw = NULL;
    }
}

void func_80BEBEB8(EffLastday* this, PlayState* play) {
    u16 cueId;

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        switch (cueId) {
            default:
                this->actor.draw = NULL;
                this->step = 0;
                break;

            case EFFLASTDAY_ACTION_2:
                this->actor.draw = EffLastday_Draw;
                this->step++;
                break;

            case EFFLASTDAY_ACTION_3:
                this->actor.draw = EffLastday_Draw;
                break;
        }
    } else {
        this->actor.draw = NULL;
    }
}

void func_80BEBF78(EffLastday* this, PlayState* play) {
    u16 cueId;

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
        switch (cueId) {
            default:
                this->actor.draw = NULL;
                this->alpha = 0;
                this->step = 0;
                break;

            case EFFLASTDAY_ACTION_2:
                if (!this->actor.home.rot.z) {
                    this->actor.home.rot.z = true;
                    Actor_PlaySfx(&this->actor, NA_SE_EV_COMING_FIRE);
                }
                this->actor.draw = EffLastday_Draw;
                if (this->alpha < 255) {
                    this->alpha += 5;
                }
                if (this->alpha > 255) {
                    this->alpha = 255;
                }
                this->step++;
                break;

            case EFFLASTDAY_ACTION_3:

                this->actor.draw = EffLastday_Draw;
                break;
        }
    } else {
        this->actor.draw = NULL;
    }
}

void EffLastday_Update(Actor* thisx, PlayState* play) {
    EffLastday* this = (EffLastday*)thisx;

    this->actionFunc(this, play);
}

void EffLastday_SetVtxAlpha(s16 alpha) {
    Vtx* vtx = ResourceMgr_LoadVtxByName(Lib_SegmentedToVirtual(object_lastday_Vtx_000000));

    vtx[0].v.cn[3] = alpha;
    vtx[3].v.cn[3] = alpha;
    vtx[4].v.cn[3] = alpha;
    vtx[5].v.cn[3] = alpha;
}

void EffLastday_Draw(Actor* thisx, PlayState* play) {
    EffLastday* this = (EffLastday*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    switch (EFFLASTDAY_GET_F(thisx)) {
        case EFFLASTDAY_PARAM_1:
        case EFFLASTDAY_PARAM_2:
            break;
        case EFFLASTDAY_PARAM_3:
            EffLastday_SetVtxAlpha(this->alpha);
            break;
        default:
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 170, this->alpha);
            break;
    }
    AnimatedMat_DrawAlphaStep(play, this->matAnim, 1.0f, this->step);
    Gfx_DrawDListXlu(play, this->dList);

    CLOSE_DISPS(play->state.gfxCtx);
}
