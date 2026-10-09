/*
 * File: z_en_viewer.c
 * Overlay: ovl_En_Viewer
 * Description: Cutscene Actors?
 */

#include "z_en_viewer.h"

#define FLAGS                                                                \
    (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_SOARING_AND_SOT_CS)

void EnViewer_Init(Actor* thisx, PlayState* play);
void EnViewer_Destroy(Actor* thisx, PlayState* play);
void EnViewer_Update(Actor* thisx, PlayState* play2);
void EnViewer_Draw(Actor* thisx, PlayState* play);

void func_8089F17C(EnViewer* this, PlayState* play);
void func_8089F218(EnViewer* this, PlayState* play);
void func_8089F2C4(EnViewer* this, PlayState* play);

static u8 D_8089F3E0 = 0;

static u8 D_8089F3E4 = 0;

ActorProfile En_Viewer_Profile = {
    /**/ ACTOR_EN_VIEWER,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnViewer),
    /**/ EnViewer_Init,
    /**/ EnViewer_Destroy,
    /**/ EnViewer_Update,
    /**/ EnViewer_Draw,
};

u32 D_8089F4D0;

void EnViewer_SetupAction(EnViewer* this, EnViewerActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnViewer_Init(Actor* thisx, PlayState* play) {
    EnViewer* this = (EnViewer*)thisx;

    this->unk_154 = D_8089F3E0;
    D_8089F3E0++;
    switch (ENVIEWER_GET_3(&this->actor)) {
        case ENVIEWER_PARAM_0:
            this->unk_148 = ENVIEWER_GET_8000(&this->actor) * 40.0f;
            this->unk_150 = (u8)ENVIEWER_GET_1FC(&this->actor) * 40.0f;
            EnViewer_SetupAction(this, func_8089F17C);
            break;

        case ENVIEWER_PARAM_1:
            this->unk_148 = (u16)ENVIEWER_GET_FE00(&this->actor) * 40.0f;
            this->unk_150 = (u8)ENVIEWER_GET_1FC(&this->actor) * 40.0f;
            this->unk_14C = (u16)(this->actor.world.rot.y) * 40.0f;
            EnViewer_SetupAction(this, func_8089F218);
            break;

        case ENVIEWER_PARAM_2:
            this->unk_148 = (u16)ENVIEWER_GET_FE00(&this->actor) * 40.0f;
            this->unk_14C = (u8)ENVIEWER_GET_1FC(&this->actor) * 40.0f;
            EnViewer_SetupAction(this, func_8089F2C4);
            break;

        default:
            break;
    }
}

void EnViewer_Destroy(Actor* thisx, PlayState* play) {
    D_8089F3E0--;
}

void func_8089F014(EnViewer* this, PlayState* play, f32 arg2) {
    if (arg2 > 1.0f) {
        arg2 = 1.0f;
    } else if (arg2 < 0.0f) {
        arg2 = 0.0f;
    }

    play->envCtx.lightSettingOverride = 0;
    play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;
    play->envCtx.lightSetting = this->actor.world.rot.x;
    play->envCtx.prevLightSetting = this->actor.world.rot.z;
    play->envCtx.lightBlend = arg2;
}

void func_8089F0A0(EnViewer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    D_8089F3E4++;
    if ((D_8089F3E4 == D_8089F3E0) && (play->envCtx.lightSettingOverride != LIGHT_SETTING_OVERRIDE_NONE)) {
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
        play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_NONE;
        play->envCtx.lightBlend = 1.0f;
        Environment_ChangeLightSetting(
            play, SurfaceType_GetLightSettingIndex(&play->colCtx, player->actor.floorPoly, player->actor.floorBgId));
        play->envCtx.lightBlend = 1.0f;
        play->envCtx.prevLightSetting = play->envCtx.lightSetting;
    }
}

void func_8089F17C(EnViewer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 xzDist = fabsf(this->actor.xzDistToPlayer);

    if ((((play->roomCtx.curRoom.num == this->actor.room) && (xzDist <= this->unk_148)) &&
         (this->actor.playerHeightRel <= this->unk_150)) &&
        (this->actor.world.pos.y <= player->actor.world.pos.y)) {
        xzDist = this->actor.playerHeightRel / this->unk_150;
        func_8089F014(this, play, xzDist);
    } else {
        func_8089F0A0(this, play);
    }
}

void func_8089F218(EnViewer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 xzDist = fabsf(this->actor.xzDistToPlayer);
    f32 temp;

    if ((play->roomCtx.curRoom.num == this->actor.room) && (xzDist <= this->unk_148) &&
        (this->actor.playerHeightRel <= this->unk_150) && (this->actor.world.pos.y <= player->actor.world.pos.y)) {
        temp = (xzDist - this->unk_14C) / (this->unk_148 - this->unk_14C);
        func_8089F014(this, play, temp);
    } else {
        func_8089F0A0(this, play);
    }
}

void func_8089F2C4(EnViewer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp20;
    f32 temp;

    Actor_WorldToActorCoords(&this->actor, &sp20, &player->actor.world.pos);

    if (this->unk_14C == 0.0f) {
        this->unk_14C = 0.1f;
    }

    if ((play->roomCtx.curRoom.num == this->actor.room) && (sp20.x < fabsf(this->unk_148))) {
        temp = sp20.z / this->unk_14C;
        func_8089F014(this, play, temp);
    } else {
        func_8089F0A0(this, play);
    }
}

void EnViewer_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnViewer* this = (EnViewer*)thisx;

    if (D_8089F4D0 != play->state.frames) {
        D_8089F4D0 = play->state.frames;
        D_8089F3E4 = 0;
    }

    this->actionFunc(this, play);
}

void EnViewer_Draw(Actor* thisx, PlayState* play) {
}
