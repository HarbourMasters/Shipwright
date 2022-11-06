/*
 * File: z_bg_bowl_wall.c
 * Overlay: Bg_Bowl_Wall
 * Description: Bombchu Bowling Alley Wall
 */

#include "z_bg_bowl_wall.h"
#include "overlays/actors/ovl_En_Wall_Tubo/z_en_wall_tubo.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_bowl/object_bowl.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgBowlWall_Init(Actor* thisx, PlayState* play);
void BgBowlWall_Destroy(Actor* thisx, PlayState* play);
void BgBowlWall_Update(Actor* thisx, PlayState* play);
void BgBowlWall_Draw(Actor* thisx, PlayState* play);

void BgBowlWall_SpawnBullseyes(BgBowlWall* this, PlayState* play);
void BgBowlWall_WaitForHit(BgBowlWall* this, PlayState* play);
void BgBowlWall_FallDoEffects(BgBowlWall* this, PlayState* play);
void BgBowlWall_FinishFall(BgBowlWall* this, PlayState* play);
void BgBowlWall_Reset(BgBowlWall* this, PlayState* play);

const ActorInit Bg_Bowl_Wall_InitVars = {
    ACTOR_BG_BOWL_WALL,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_BOWL,
    sizeof(BgBowlWall),
    (ActorFunc)BgBowlWall_Init,
    (ActorFunc)BgBowlWall_Destroy,
    (ActorFunc)BgBowlWall_Update,
    (ActorFunc)BgBowlWall_Draw,
    NULL,
};

static Vec3f sBullseyeOffset[] = {
    { 0.0f, 210.0f, -20.0f },
    { 0.0f, 170.0f, -20.0f },
    { -170.0f, 0.0f, -20.0f },
    { 170.0f, 0.0f, -20.0f },
};

static s16 sTargetRot[] = { 0x0000, 0x0000, 0x3FFF, -0x3FFF };

void BgBowlWall_Init(Actor* thisx, PlayState* play) {
    BgBowlWall* this = (BgBowlWall*)thisx;
    s32 pad1;
    s32 pad2;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, DPM_UNK);

    if (this->dyna.actor.params == 0) {
        CollisionHeader_GetVirtual(&gBowlingFirstAndFinalRoundCol, &colHeader);
    } else {
        CollisionHeader_GetVirtual(&gBowlingSecondRoundCol, &colHeader);
    }

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->initPos = this->dyna.actor.world.pos;
    osSyncPrintf("\n\n");
    osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ ボーリングおじゃま壁発生 ☆☆☆☆☆ %d\n" VT_RST, this->dyna.actor.params);
    this->actionFunc = BgBowlWall_SpawnBullseyes;
    this->dyna.actor.scale.x = this->dyna.actor.scale.y = this->dyna.actor.scale.z = 1.0f;
}

void BgBowlWall_Destroy(Actor* thisx, PlayState* play) {
    BgBowlWall* this = (BgBowlWall*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgBowlWall_SpawnBullseyes(BgBowlWall* this, PlayState* play) {
    s32 pad;
    EnWallTubo* bullseye;
    Actor* lookForGirl;
    s16 type;

    type = this->dyna.actor.params;
    if (type != 0) {
        type += (s16)Rand_ZeroFloat(2.99f);
        this->dyna.actor.shape.rot.z = this->dyna.actor.world.rot.z = sTargetRot[type];
        osSyncPrintf("\n\n");
    }
    this->bullseyeCenter.x = sBullseyeOffset[type].x + this->dyna.actor.world.pos.x;
    this->bullseyeCenter.y = sBullseyeOffset[type].y + this->dyna.actor.world.pos.y;
    this->bullseyeCenter.z = sBullseyeOffset[type].z + this->dyna.actor.world.pos.z;
    bullseye = (EnWallTubo*)Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_EN_WALL_TUBO,
                                               this->bullseyeCenter.x, this->bullseyeCenter.y, this->bullseyeCenter.z,
                                               0, 0, 0, this->dyna.actor.params);
    if (bullseye != NULL) {
        bullseye->explosionCenter = this->bullseyeCenter;
        if (type != 0) {
            bullseye->explosionCenter = this->bullseyeCenter = this->dyna.actor.world.pos;
        }
        if (this->chuGirl == NULL) {
            lookForGirl = play->actorCtx.actorLists[ACTORCAT_NPC].head;
            while (lookForGirl != NULL) {
                if (lookForGirl->id != ACTOR_EN_BOM_BOWL_MAN) {
                    lookForGirl = lookForGirl->next;
                } else {
                    this->chuGirl = (EnBomBowlMan*)lookForGirl;
                    break;
                }
            }
        }
        this->actionFunc = BgBowlWall_WaitForHit;
    }
}

void BgBowlWall_WaitForHit(BgBowlWall* this, PlayState* play) {
    if (this->isHit) {
        this->actionFunc = BgBowlWall_FallDoEffects;
    }
}

void BgBowlWall_FallDoEffects(BgBowlWall* this, PlayState* play) {
    s16 pad;
    Vec3f effectAccel = { 0.0f, 0.1f, 0.0f };
    Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f effectPos;
    s16 quakeIndex;
    s32 wallFallen;
    s32 i;

    wallFallen = false;

    if (this->dyna.actor.params == 0) { // wall collapses backwards
        Math_SmoothStepToS(&this->dyna.actor.shape.rot.x, -0x3E80, 3, 500, 0);
        this->dyna.actor.world.rot.x = this->dyna.actor.shape.rot.x;
        if (this->dyna.actor.shape.rot.x < -0x3C1E) {
            wallFallen = true;
        }
    } else { // wall slides downwards
        Math_ApproachF(&this->dyna.actor.world.pos.y, this->initPos.y - 450.0f, 0.3f, 10.0f);
        if (this->dyna.actor.world.pos.y < (this->initPos.y - 400.0f)) {
            wallFallen = true;
        }
    }

    if (wallFallen) {
        for (i = 0; i < 15; i++) {
            effectPos.x = Rand_CenteredFloat(300.0f) + this->bullseyeCenter.x;
            effectPos.y = -100.0f;
            effectPos.z = Rand_CenteredFloat(400.0f) + this->bullseyeCenter.z;
            EffectSsBomb2_SpawnLayered(play, &effectPos, &effectVelocity, &effectAccel, 100, 30);
            effectPos.y = -50.0f;
            EffectSsHahen_SpawnBurst(play, &effectPos, 10.0f, 0, 50, 15, 3, HAHEN_OBJECT_DEFAULT, 10, NULL);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_IT_BOMB_EXPLOSION);
        }
        quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 1);
        Quake_SetSpeed(quakeIndex, 0x7FFF);
        Quake_SetQuakeValues(quakeIndex, 300, 0, 0, 0);
        Quake_SetCountdown(quakeIndex, 30);
        this->timer = 20;
        this->actionFunc = BgBowlWall_FinishFall;
    }
}

void BgBowlWall_FinishFall(BgBowlWall* this, PlayState* play) {
    if (this->timer >= 2) {
        if (this->dyna.actor.params == 0) {
            Math_SmoothStepToS(&this->dyna.actor.shape.rot.x, -0x3E80, 1, 200, 0);
        } else {
            Math_ApproachF(&this->dyna.actor.world.pos.y, this->initPos.y - 450.0f, 0.3f, 10.0f);
        }
    } else if (this->timer == 1) {
        this->dyna.actor.world.rot.x = this->dyna.actor.shape.rot.x = 0;
        this->dyna.actor.world.pos.y = this->initPos.y - 450.0f;
        this->chuGirl->wallStatus[this->dyna.actor.params] = 2;
        this->actionFunc = BgBowlWall_Reset;
    }
}

void BgBowlWall_Reset(BgBowlWall* this, PlayState* play) {
    if (this->chuGirl->wallStatus[this->dyna.actor.params] != 2) {
        Math_ApproachF(&this->dyna.actor.world.pos.y, this->initPos.y, 0.3f, 50.0f);
        if (fabsf(this->dyna.actor.world.pos.y - this->initPos.y) <= 10.0f) {
            this->dyna.actor.world.pos.y = this->initPos.y;
            this->isHit = false;
            this->actionFunc = BgBowlWall_SpawnBullseyes;
        }
    }
}

void BgBowlWall_Update(Actor* thisx, PlayState* play) {
    BgBowlWall* this = (BgBowlWall*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actionFunc(this, play);
}

void BgBowlWall_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgBowlWall* this = (BgBowlWall*)thisx;
    u32 frames;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D84(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x8,
               Gfx_TexScroll(play->state.gfxCtx, 0, -2 * (frames = play->state.frames), 16, 16));
    gDPPipeSync(POLY_OPA_DISP++);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (this->dyna.actor.params == 0) {
        gSPDisplayList(POLY_OPA_DISP++, gBowlingRound1WallDL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gBowlingRound2WallDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
