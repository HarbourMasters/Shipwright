/*
 * File: z_en_wall_tubo.c
 * Overlay: ovl_En_Wall_Tubo
 * Description: Bombchu Bowling Alley Bullseyes/Pits
 */

#include "z_en_wall_tubo.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.h"
#include "overlays/actors/ovl_Bg_Bowl_Wall/z_bg_bowl_wall.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS ACTOR_FLAG_4

void EnWallTubo_Init(Actor* thisx, PlayState* play);
void EnWallTubo_Destroy(Actor* thisx, PlayState* play);
void EnWallTubo_Update(Actor* thisx, PlayState* play);

void EnWallTubo_FindGirl(EnWallTubo* this, PlayState* play);
void EnWallTubo_DetectChu(EnWallTubo* this, PlayState* play);
void EnWallTubo_SetWallFall(EnWallTubo* this, PlayState* play);

const ActorInit En_Wall_Tubo_InitVars = {
    ACTOR_EN_WALL_TUBO,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnWallTubo),
    (ActorFunc)EnWallTubo_Init,
    (ActorFunc)EnWallTubo_Destroy,
    (ActorFunc)EnWallTubo_Update,
    NULL,
    NULL,
};

void EnWallTubo_Init(Actor* thisx, PlayState* play) {
    EnWallTubo* this = (EnWallTubo*)thisx;

    osSyncPrintf("\n\n");
    // "Wall Target"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 壁のツボ ☆☆☆☆☆ \n" VT_RST);
    this->unk_164 = this->actor.world.pos;
    this->actionFunc = EnWallTubo_FindGirl;
}

void EnWallTubo_Destroy(Actor* thisx, PlayState* play) {
}

void EnWallTubo_FindGirl(EnWallTubo* this, PlayState* play) {
    Actor* lookForGirl;

    lookForGirl = play->actorCtx.actorLists[ACTORCAT_NPC].head;

    while (lookForGirl != NULL) {
        if (lookForGirl->id != ACTOR_EN_BOM_BOWL_MAN) {
            lookForGirl = lookForGirl->next;
        } else {
            this->chuGirl = (EnBomBowlMan*)lookForGirl;
            break;
        }
    }

    this->actionFunc = EnWallTubo_DetectChu;
}

void EnWallTubo_DetectChu(EnWallTubo* this, PlayState* play) {
    EnBomChu* chu;
    s32 pad;
    Vec3f effAccel = { 0.0f, 0.1f, 0.0f };
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f chuPosDiff;
    s16 quakeIndex;

    if (this->chuGirl->minigamePlayStatus != 0) {
        if (play->cameraPtrs[MAIN_CAM]->setting == CAM_SET_CHU_BOWLING) {
            chu = (EnBomChu*)play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

            while (chu != NULL) {
                if ((&chu->actor == &this->actor) || (chu->actor.id != ACTOR_EN_BOM_CHU)) {
                    chu = (EnBomChu*)chu->actor.next;
                    continue;
                }

                chuPosDiff.x = chu->actor.world.pos.x - this->actor.world.pos.x;
                chuPosDiff.y = chu->actor.world.pos.y - this->actor.world.pos.y;
                chuPosDiff.z = chu->actor.world.pos.z - this->actor.world.pos.z;

                if (((fabsf(chuPosDiff.x) < 40.0f) || (BREG(2))) && ((fabsf(chuPosDiff.y) < 40.0f) || (BREG(2))) &&
                    (fabsf(chuPosDiff.z) < 40.0f || (BREG(2)))) {
                    this->chuGirl->wallStatus[this->actor.params] = 1;
                    chu->timer = 2;
                    func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                    this->timer = 60;
                    EffectSsBomb2_SpawnLayered(play, &this->explosionCenter, &effVelocity, &effAccel, 200, 40);
                    quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 1);
                    Quake_SetSpeed(quakeIndex, 0x7FFF);
                    Quake_SetQuakeValues(quakeIndex, 100, 0, 0, 0);
                    Quake_SetCountdown(quakeIndex, 100);
                    this->actionFunc = EnWallTubo_SetWallFall;
                    break;
                }

                chu = (EnBomChu*)chu->actor.next;
            }
        }
    }
}

void EnWallTubo_SetWallFall(EnWallTubo* this, PlayState* play) {
    BgBowlWall* wall;
    Vec3f effAccel = { 0.0f, 0.1f, 0.0f };
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;

    if ((play->gameplayFrames & 1) == 0) {
        effPos.x = this->explosionCenter.x + Rand_CenteredFloat(300.0f);
        effPos.y = this->explosionCenter.y + Rand_CenteredFloat(300.0f);
        effPos.z = this->explosionCenter.z;
        EffectSsBomb2_SpawnLayered(play, &effPos, &effVelocity, &effAccel, 100, 30);
        EffectSsHahen_SpawnBurst(play, &effPos, 10.0f, 0, 50, 15, 3, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
    }

    if (this->timer == 0) {
        wall = (BgBowlWall*)this->actor.parent;

        if ((wall != NULL) && (wall->dyna.actor.update != NULL)) {
            wall->isHit = true;
            // "You did it field!" (repeated 5 times)
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            osSyncPrintf(VT_FGCOL(BLUE) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
            osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆ やった原！ ☆☆☆☆☆ \n" VT_RST);
        }

        Actor_Kill(&this->actor);
    }
}

void EnWallTubo_Update(Actor* thisx, PlayState* play) {
    EnWallTubo* this = (EnWallTubo*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actionFunc(this, play);

    if (BREG(0)) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 0, 0, 255, 255, 4, play->state.gfxCtx);
    }
}
