/*
 * File: z_bg_umajump.c
 * Overlay: ovl_Bg_Umajump
 * Description: Horse Jumping Fence
 */

#include "z_bg_umajump.h"
#include "assets/objects/object_umajump/object_umajump.h"

#define FLAGS 0x00000000

void BgUmajump_Init(Actor* thisx, PlayState* play);
void BgUmajump_Destroy(Actor* thisx, PlayState* play);
void BgUmajump_Update(Actor* thisx, PlayState* play);
void BgUmajump_Draw(Actor* thisx, PlayState* play);

void func_8091A5A0(Actor* thisx, PlayState* play);

ActorProfile Bg_Umajump_Profile = {
    /**/ ACTOR_BG_UMAJUMP,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(BgUmajump),
    /**/ BgUmajump_Init,
    /**/ BgUmajump_Destroy,
    /**/ BgUmajump_Update,
    /**/ NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_80919F30(BgUmajump* this, PlayState* play) {
    if (this->rotationTimer >= 90) {
        this->rotationTimer = 0;
    }

    this->rotationTimer++;
    this->dyna.actor.shape.yOffset =
        Math_SinS((this->rotationTimer / 90.0f) * 0x10000) * (20.0f / this->dyna.actor.scale.y);
}

void BgUmajump_StopCutscene(BgUmajump* this, PlayState* play) {
    if ((play->csCtx.curFrame >= 6) && !this->hasSoundPlayed) {
        this->hasSoundPlayed = true;
        Audio_PlaySfx(NA_SE_EV_KID_HORSE_NEIGH);
    }

    if (play->csCtx.state == CS_STATE_IDLE) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        this->dyna.actor.update = Actor_Noop;
    }
}

void BgUmajump_PlayCutscene(BgUmajump* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        SET_WEEKEVENTREG(WEEKEVENTREG_89_20);
        this->actionFunc = BgUmajump_StopCutscene;
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void BgUmajump_CheckDistance(BgUmajump* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->horse != NULL) && (player != NULL) &&
        (Math3D_Dist2D(this->horse->world.pos.x, this->horse->world.pos.z, player->actor.world.pos.x,
                       player->actor.world.pos.z) < 1400.0f)) {
        this->actionFunc = BgUmajump_PlayCutscene;
    }
}

void BgUmajump_Init(Actor* thisx, PlayState* play) {
    BgUmajump* this = (BgUmajump*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);

    this->actionFunc = NULL;
    this->horse = NULL;
    this->hasSoundPlayed = false;

    DynaPolyActor_Init(&this->dyna, 0);

    this->pathIndex = BG_UMAJUMP_GET_PATH_INDEX(thisx);
    thisx->params = BG_UMAJUMP_GET_TYPE(thisx);

    if (thisx->params == BG_UMAJUMP_TYPE_2) {
        if ((play->sceneId == SCENE_F01) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_89_20) &&
            !CHECK_QUEST_ITEM(QUEST_SONG_EPONA) && (thisx->csId != CS_ID_NONE)) {
            this->actionFunc = BgUmajump_CheckDistance;
            thisx->update = func_8091A5A0;
            thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->horse = SubS_FindActor(play, this->horse, ACTORCAT_BG, ACTOR_EN_HORSE);
        } else {
            thisx->update = Actor_Noop;
        }
    } else {
        this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_UMAJUMP);

        if (this->objectSlot <= OBJECT_SLOT_NONE) {
            Actor_Kill(thisx);
        }

        if ((thisx->params == BG_UMAJUMP_TYPE_3) && CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KANBAN, thisx->world.pos.x, thisx->world.pos.y,
                        thisx->world.pos.z, thisx->shape.rot.x, thisx->shape.rot.y, thisx->shape.rot.z, 0x3E);
            Actor_Kill(thisx);
        }
    }
}

void BgUmajump_Destroy(Actor* thisx, PlayState* play) {
    BgUmajump* this = (BgUmajump*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgUmajump_Update(Actor* thisx, PlayState* play) {
    BgUmajump* this = (BgUmajump*)thisx;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->dyna.actor.objectSlot = this->objectSlot;
        this->dyna.actor.draw = BgUmajump_Draw;

        Actor_SetObjectDependency(play, &this->dyna.actor);

        if (this->dyna.actor.params == BG_UMAJUMP_TYPE_5) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
                DynaPolyActor_LoadMesh(play, &this->dyna, &object_umajump_Colheader_001558);
            }
        } else if (this->dyna.actor.params == BG_UMAJUMP_TYPE_6) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
                DynaPolyActor_LoadMesh(play, &this->dyna, &object_umajump_Colheader_001558);
            }
        } else if ((this->dyna.actor.params == BG_UMAJUMP_TYPE_4) || (this->dyna.actor.params == BG_UMAJUMP_TYPE_3)) {
            DynaPolyActor_LoadMesh(play, &this->dyna, &object_umajump_Colheader_001558);
        } else {
            DynaPolyActor_LoadMesh(play, &this->dyna, &object_umajump_Colheader_001438);
        }

        if (this->dyna.actor.params == BG_UMAJUMP_TYPE_1) {
            this->rotationTimer = 0;
            this->actionFunc = func_80919F30;
            this->dyna.actor.update = func_8091A5A0;
        } else if (this->dyna.actor.params == BG_UMAJUMP_TYPE_3) {
            this->dyna.actor.update = func_8091A5A0;
        } else if ((this->dyna.actor.params == BG_UMAJUMP_TYPE_5) || (this->dyna.actor.params == BG_UMAJUMP_TYPE_6)) {
            if (this->dyna.actor.params == BG_UMAJUMP_TYPE_5) {
                if ((this->dyna.bgId == BGACTOR_NEG_ONE) && CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
                    DynaPolyActor_LoadMesh(play, &this->dyna, &object_umajump_Colheader_001558);
                }
            } else if ((this->dyna.actor.params == BG_UMAJUMP_TYPE_6) && (this->dyna.bgId == BGACTOR_NEG_ONE) &&
                       (!CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS) ||
                        ((gSaveContext.save.day == 2) && (gSaveContext.save.isNight == true) &&
                         ((CURRENT_TIME >= CLOCK_TIME(5, 30)) && (CURRENT_TIME <= CLOCK_TIME(6, 0)))))) {
                DynaPolyActor_LoadMesh(play, &this->dyna, &object_umajump_Colheader_001558);
            }
            DynaPoly_DisableFloorCollision(play, &play->colCtx.dyna, this->dyna.bgId);
            this->dyna.actor.update = func_8091A5A0;
        } else if (this->dyna.actor.params == BG_UMAJUMP_TYPE_4) {
            DynaPoly_DisableFloorCollision(play, &play->colCtx.dyna, this->dyna.bgId);
            this->dyna.actor.update = Actor_Noop;
        } else {
            this->dyna.actor.update = Actor_Noop;
        }
    }
}

void func_8091A5A0(Actor* thisx, PlayState* play) {
    BgUmajump* this = (BgUmajump*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }

    if ((this->dyna.actor.params == BG_UMAJUMP_TYPE_3) && CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KANBAN, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                    this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y,
                    this->dyna.actor.shape.rot.z, 0x3E);
        Actor_Kill(&this->dyna.actor);
    }

    if (this->dyna.actor.params == BG_UMAJUMP_TYPE_5) {
        if ((this->dyna.bgId == BGACTOR_NEG_ONE) && CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
            DynaPolyActor_LoadMesh(play, &this->dyna, &object_umajump_Colheader_001558);
        } else if ((this->dyna.bgId != BGACTOR_NEG_ONE) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
            DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
        }
    } else if (this->dyna.actor.params == BG_UMAJUMP_TYPE_6) {
        if ((this->dyna.bgId == BGACTOR_NEG_ONE) &&
            (!CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS) ||
             ((gSaveContext.save.day == 2) && (gSaveContext.save.isNight == true) &&
              (CURRENT_TIME >= CLOCK_TIME(5, 30)) && (CURRENT_TIME <= CLOCK_TIME(6, 0))))) {
            DynaPolyActor_LoadMesh(play, &this->dyna, &object_umajump_Colheader_001558);
        } else if ((this->dyna.bgId != BGACTOR_NEG_ONE) && CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS) &&
                   ((gSaveContext.save.day != 2) || (gSaveContext.save.isNight != true) ||
                    (CURRENT_TIME < CLOCK_TIME(5, 30)) || (CURRENT_TIME > CLOCK_TIME(6, 0)))) {
            DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
        }
    }
}

void BgUmajump_Draw(Actor* thisx, PlayState* play) {
    BgUmajump* this = (BgUmajump*)thisx;

    if (this->dyna.bgId != BGACTOR_NEG_ONE) {
        Gfx_DrawDListOpa(play, gHorseJumpFenceDL);
    }
}
