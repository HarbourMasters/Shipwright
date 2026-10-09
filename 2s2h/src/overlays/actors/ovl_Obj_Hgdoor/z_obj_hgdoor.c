/*
 * File: z_obj_hgdoor.c
 * Overlay: ovl_Obj_Hgdoor
 * Description: Music Box House - Closet Door
 */

#include "z_obj_hgdoor.h"
#include "objects/object_hgdoor/object_hgdoor.h"

#define FLAGS (ACTOR_FLAG_FREEZE_EXCEPTION)

void ObjHgdoor_Init(Actor* thisx, PlayState* play);
void ObjHgdoor_Destroy(Actor* thisx, PlayState* play);
void ObjHgdoor_Update(Actor* thisx, PlayState* play);
void ObjHgdoor_Draw(Actor* thisx, PlayState* play);

void ObjHgdoor_SetupIdle(ObjHgdoor* this);
void ObjHgdoor_Idle(ObjHgdoor* this, PlayState* play);
void ObjHgdoor_SetupCutscene(ObjHgdoor* this);
void ObjHgdoor_PlayCutscene(ObjHgdoor* this, PlayState* play);
void ObjHgdoor_SetupHandleCutscene(ObjHgdoor* this);
void ObjHgdoor_HandleCutscene(ObjHgdoor* this, PlayState* play);
void ObjHgdoor_SetupStopCs(ObjHgdoor* this);
void ObjHgdoor_StopCs(ObjHgdoor* this, PlayState* play);
s32 ObjHgdoor_Rotate(ObjHgdoor* this, PlayState* play);

ActorProfile Obj_Hgdoor_Profile = {
    /**/ ACTOR_OBJ_HGDOOR,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_HGDOOR,
    /**/ sizeof(ObjHgdoor),
    /**/ ObjHgdoor_Init,
    /**/ ObjHgdoor_Destroy,
    /**/ ObjHgdoor_Update,
    /**/ ObjHgdoor_Draw,
};

static s16 sOpenFlag = 0;

void ObjHgdoor_SetChild(ObjHgdoor* this, PlayState* play) {
    Actor* actorIterator = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (actorIterator) {
        if ((actorIterator->id == ACTOR_OBJ_HGDOOR) && (&this->dyna.actor != actorIterator)) {
            this->dyna.actor.child = actorIterator;
            break;
        }
        actorIterator = actorIterator->next;
    }
}

void ObjHgdoor_SetParent(ObjHgdoor* this, PlayState* play) {
    Actor* actorIterator = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (actorIterator) {
        if (actorIterator->id == ACTOR_EN_HG) {
            this->dyna.actor.parent = actorIterator;
            break;
        }
        actorIterator = actorIterator->next;
    }
}

void ObjHgdoor_Init(Actor* thisx, PlayState* play) {
    ObjHgdoor* this = (ObjHgdoor*)thisx;
    s32 pad;
    CollisionHeader* header = NULL;

    Actor_SetScale(&this->dyna.actor, 0.1f);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    if (OBJHGDOOR_IS_RIGHT_DOOR(&this->dyna.actor)) {
        CollisionHeader_GetVirtual(&object_hgdoor_Colheader_001D10, &header);
    } else {
        CollisionHeader_GetVirtual(&object_hgdoor_Colheader_0018C0, &header);
    }
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, header);
    this->rotation = 0;
    this->timer = 0;
    this->csId = this->dyna.actor.csId;
    ObjHgdoor_SetupIdle(this);
}

void ObjHgdoor_Destroy(Actor* thisx, PlayState* play) {
    ObjHgdoor* this = (ObjHgdoor*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjHgdoor_SetupIdle(ObjHgdoor* this) {
    this->actionFunc = ObjHgdoor_Idle;
}

void ObjHgdoor_Idle(ObjHgdoor* this, PlayState* play) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_75_20) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE) &&
        (this->dyna.actor.xzDistToPlayer < 100.0f) && (this->dyna.actor.playerHeightRel < 40.0f) &&
        OBJHGDOOR_IS_RIGHT_DOOR(&this->dyna.actor)) {
        ObjHgdoor_SetChild(this, play);
        ObjHgdoor_SetParent(this, play);
        ObjHgdoor_SetupCutscene(this);
    }
}

void ObjHgdoor_SetupCutscene(ObjHgdoor* this) {
    this->actionFunc = ObjHgdoor_PlayCutscene;
}

void ObjHgdoor_PlayCutscene(ObjHgdoor* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_Start(this->csId, &this->dyna.actor);
        ObjHgdoor_SetupHandleCutscene(this);
        ObjHgdoor_SetupHandleCutscene((ObjHgdoor*)this->dyna.actor.child);
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(this->csId);
    }
}

void ObjHgdoor_SetupHandleCutscene(ObjHgdoor* this) {
    this->cueId = 99;
    this->actionFunc = ObjHgdoor_HandleCutscene;
}

void ObjHgdoor_HandleCutscene(ObjHgdoor* this, PlayState* play) {
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_483)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_483);
        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;
            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WOOD_DOOR_OPEN_SPEEDY);
                    if ((this->dyna.actor.parent != NULL) && (this->dyna.actor.parent->id == ACTOR_EN_HG)) {
                        this->dyna.actor.parent->colChkInfo.health = 1;
                    }
                    sOpenFlag = 1;
                    break;

                case 2:
                    break;
            }
        }
        if ((sOpenFlag == 1) && ObjHgdoor_Rotate(this, play)) {
            ObjHgdoor_SetupStopCs(this);
        }
    } else {
        this->cueId = 99;
    }
}

void ObjHgdoor_SetupStopCs(ObjHgdoor* this) {
    this->timer = 0;
    this->actionFunc = ObjHgdoor_StopCs;
}

void ObjHgdoor_StopCs(ObjHgdoor* this, PlayState* play) {
    if (this->timer++ > 80) {
        if (!CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_Stop(this->csId);
        }
    }
}

/**
 * @brief Function to increment the rotation angle of the door
 *
 * @return true when door is fully open, false otherwise
 */
s32 ObjHgdoor_Rotate(ObjHgdoor* this, PlayState* play) {
    if (this->rotation < 0x5555) {
        this->rotation += 0x1388;
    } else {
        return true;
    }

    return false;
}

void ObjHgdoor_Open(ObjHgdoor* this) {
    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y;
    if (OBJHGDOOR_IS_RIGHT_DOOR(&this->dyna.actor)) {
        this->dyna.actor.shape.rot.y += this->rotation;
    } else {
        this->dyna.actor.shape.rot.y -= this->rotation;
    }
}

void ObjHgdoor_Update(Actor* thisx, PlayState* play) {
    ObjHgdoor* this = (ObjHgdoor*)thisx;

    this->actionFunc(this, play);
    ObjHgdoor_Open(this);
}

void ObjHgdoor_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    if (OBJHGDOOR_IS_RIGHT_DOOR(thisx)) {
        gSPDisplayList(POLY_OPA_DISP++, object_hgdoor_DL_001AB0);
        gSPDisplayList(POLY_OPA_DISP++, object_hgdoor_DL_001BA8);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, object_hgdoor_DL_001670);
        gSPDisplayList(POLY_OPA_DISP++, object_hgdoor_DL_001768);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
