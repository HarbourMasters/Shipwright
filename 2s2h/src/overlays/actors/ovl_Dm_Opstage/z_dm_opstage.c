/*
 * File: z_dm_opstage.c
 * Overlay: ovl_Dm_Opstage
 * Description: Lost Woods Cutscene Trees and Floor
 */

#include "z_dm_opstage.h"
#include "objects/object_keikoku_demo/object_keikoku_demo.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmOpstage_Init(Actor* thisx, PlayState* play);
void DmOpstage_Destroy(Actor* thisx, PlayState* play);
void DmOpstage_Update(Actor* thisx, PlayState* play);
void DmOpstage_Draw(Actor* thisx, PlayState* play);

void DmOpstage_HandleCutscene(DmOpstage* this, PlayState* play);

ActorProfile Dm_Opstage_Profile = {
    /**/ ACTOR_DM_OPSTAGE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_KEIKOKU_DEMO,
    /**/ sizeof(DmOpstage),
    /**/ DmOpstage_Init,
    /**/ DmOpstage_Destroy,
    /**/ DmOpstage_Update,
    /**/ DmOpstage_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_STOP),
};

void DmOpstage_SetupAction(DmOpstage* this, DmOpstageActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void DmOpstage_Init(Actor* thisx, PlayState* play) {
    DmOpstage* this = (DmOpstage*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DmOpstage_SetupAction(this, DmOpstage_HandleCutscene);
    Actor_SetScale(&this->dyna.actor, 0.1f);

    if (DMOPSTAGE_GET_TYPE(&this->dyna.actor) == DMOPSTAGE_TYPE_GROUND) {
        DynaPolyActor_Init(&this->dyna, 0);
        DynaPolyActor_LoadMesh(play, &this->dyna, &gKeikokuDemoFloorCol);
    }

    if (DMOPSTAGE_GET_TYPE(&this->dyna.actor) > DMOPSTAGE_TYPE_GROUND) {
        // trees move their world pos to map origin/center, then draw at their starting position
        this->drawOffset.x = this->dyna.actor.world.pos.x;
        this->drawOffset.y = this->dyna.actor.world.pos.y;
        this->drawOffset.z = this->dyna.actor.world.pos.z;
        this->dyna.actor.world.pos.x = 0.0f;
        this->dyna.actor.world.pos.y = 0.0f;
        this->dyna.actor.world.pos.z = 0.0f;
    }
}

void DmOpstage_Destroy(Actor* thisx, PlayState* play) {
    DmOpstage* this = (DmOpstage*)thisx;

    if (DMOPSTAGE_GET_TYPE(&this->dyna.actor) == DMOPSTAGE_TYPE_GROUND) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void DmOpstage_HandleCutscene(DmOpstage* this, PlayState* play) {
    s32 cueChannel;

    if (DMOPSTAGE_GET_TYPE(&this->dyna.actor) == DMOPSTAGE_TYPE_GROUND) {
        if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_115)) {
            cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_115);
            if (play->csCtx.actorCues[cueChannel]->id == 2) {
                this->dyna.actor.scale.x = 0.075f;
                this->dyna.actor.scale.z = 0.3f;
            } else {
                this->dyna.actor.scale.x = 0.1f;
                this->dyna.actor.scale.z = 0.1f;
            }
            Cutscene_ActorTranslateAndYaw(&this->dyna.actor, play, cueChannel);
        }
    } else if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_116 + DMOPSTAGE_GET_CUE_TYPE_OFFSET(&this->dyna.actor))) {
        Cutscene_ActorTranslateAndYaw(
            &this->dyna.actor, play,
            Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_116 + DMOPSTAGE_GET_CUE_TYPE_OFFSET(&this->dyna.actor)));
    }
}

void DmOpstage_Update(Actor* thisx, PlayState* play) {
    DmOpstage* this = (DmOpstage*)thisx;

    this->actionFunc(this, play);
    if ((play->sceneId == SCENE_SPOT00) && (gSaveContext.sceneLayer == 0) && (play->csCtx.curFrame == 480)) {
        // This actor is responsible for playing the fairy sound during the exposition in the intro,
        // during the transition to Lost Woods, before Ocarina gets stolen.
        Audio_PlaySfx_2(NA_SE_EV_NAVY_FLY_REBIRTH);
    }
}

void DmOpstage_Draw(Actor* thisx, PlayState* play) {
    DmOpstage* this = (DmOpstage*)thisx;

    if (DMOPSTAGE_GET_TYPE(&this->dyna.actor) > DMOPSTAGE_TYPE_GROUND) {
        // Assumption: worldPos is being manipulated by cutscene
        Matrix_Translate(this->dyna.actor.world.pos.x + this->drawOffset.x,
                         this->dyna.actor.world.pos.y + this->drawOffset.y,
                         this->dyna.actor.world.pos.z + this->drawOffset.z, MTXMODE_NEW);
        Matrix_RotateYS(this->dyna.actor.world.rot.y, MTXMODE_APPLY);
        Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    }

    switch (DMOPSTAGE_GET_TYPE(&this->dyna.actor)) {
        case DMOPSTAGE_TYPE_GROUND:
            Gfx_DrawDListOpa(play, gKeikokuDemoFloorDL);
            Gfx_DrawDListXlu(play, gKeikokuDemoFloorEmptyDL);
            break;

        case DMOPSTAGE_TYPE_ROOT_TREE:
            Gfx_DrawDListOpa(play, gKeikokuDemoTallTreeWithRootBaseDL);
            Gfx_DrawDListXlu(play, gKeikokuDemoTallTreeWithRootBaseEmptyDL);
            break;

        case DMOPSTAGE_TYPE_CUT_TREE:
            Gfx_DrawDListOpa(play, gKeikokuDemoTallTreeCutDL);
            Gfx_DrawDListXlu(play, gKeikokuDemoTallTreeCutEmptyDL);
            break;

        case DMOPSTAGE_TYPE_STRAIGHT_TREE:
            Gfx_DrawDListOpa(play, gKeikokuDemoTallTreeStraightDL);
            Gfx_DrawDListXlu(play, gKeikokuDemoTallTreeStraightEmptyDL);
            break;

        default:
            break;
    }
}
