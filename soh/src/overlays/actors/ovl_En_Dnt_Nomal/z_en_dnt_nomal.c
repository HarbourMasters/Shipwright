/*
 * File: z_en_dnt_nomal
 * Overlay: ovl_En_Dnt_Nomal
 * Description: Lost Woods minigame scrubs
 */

#include "z_en_dnt_nomal.h"
#include "objects/object_dnk/object_dnk.h"
#include "overlays/actors/ovl_En_Dnt_Demo/z_en_dnt_demo.h"
#include "overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_hintnuts/object_hintnuts.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnDntNomal_Init(Actor* thisx, PlayState* play);
void EnDntNomal_Destroy(Actor* thisx, PlayState* play);
void EnDntNomal_Update(Actor* thisx, PlayState* play);
void EnDntNomal_DrawTargetScrub(Actor* thisx, PlayState* play);
void EnDntNomal_DrawStageScrub(Actor* thisx, PlayState* play);

void EnDntNomal_WaitForObject(EnDntNomal* this, PlayState* play);
void EnDntNomal_SetFlower(EnDntNomal* this, PlayState* play);

void EnDntNomal_SetupTargetWait(EnDntNomal* this, PlayState* play);
void EnDntNomal_SetupTargetUnburrow(EnDntNomal* this, PlayState* play);
void EnDntNomal_SetupTargetWalk(EnDntNomal* this, PlayState* play);
void EnDntNomal_SetupTargetTalk(EnDntNomal* this, PlayState* play);
void EnDntNomal_SetupTargetGivePrize(EnDntNomal* this, PlayState* play);

void EnDntNomal_TargetWait(EnDntNomal* this, PlayState* play);
void EnDntNomal_TargetUnburrow(EnDntNomal* this, PlayState* play);
void EnDntNomal_TargetWalk(EnDntNomal* this, PlayState* play);
void EnDntNomal_TargetFacePlayer(EnDntNomal* this, PlayState* play);
void EnDntNomal_TargetTalk(EnDntNomal* this, PlayState* play);
void EnDntNomal_TargetGivePrize(EnDntNomal* this, PlayState* play);
void EnDntNomal_TargetReturn(EnDntNomal* this, PlayState* play);
void EnDntNomal_TargetBurrow(EnDntNomal* this, PlayState* play);

void EnDntNomal_SetupStageWait(EnDntNomal* this, PlayState* play);
void EnDntNomal_SetupStageCelebrate(EnDntNomal* this, PlayState* play);
void EnDntNomal_SetupStageDance(EnDntNomal* this, PlayState* play);
void EnDntNomal_SetupStageHide(EnDntNomal* this, PlayState* play);
void EnDntNomal_SetupStageAttack(EnDntNomal* this, PlayState* play);
void EnDntNomal_StageSetupReturn(EnDntNomal* this, PlayState* play);

void EnDntNomal_StageWait(EnDntNomal* this, PlayState* play);
void EnDntNomal_StageUp(EnDntNomal* this, PlayState* play);
void EnDntNomal_StageUnburrow(EnDntNomal* this, PlayState* play);
void EnDntNomal_StageCelebrate(EnDntNomal* this, PlayState* play);
void EnDntNomal_StageDance(EnDntNomal* this, PlayState* play);
void EnDntNomal_StageHide(EnDntNomal* this, PlayState* play);
void EnDntNomal_StageAttackHide(EnDntNomal* this, PlayState* play);
void EnDntNomal_StageAttack(EnDntNomal* this, PlayState* play);
void EnDntNomal_StageReturn(EnDntNomal* this, PlayState* play);

const ActorInit En_Dnt_Nomal_InitVars = {
    ACTOR_EN_DNT_NOMAL,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnDntNomal),
    (ActorFunc)EnDntNomal_Init,
    (ActorFunc)EnDntNomal_Destroy,
    (ActorFunc)EnDntNomal_Update,
    NULL,
    NULL,
};

static ColliderCylinderInit sBodyCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 16, 46, 0, { 0, 0, 0 } },
};

static ColliderQuadInit sTargetQuadInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x0001F824, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Color_RGBA8 sLeafColors[] = {
    { 255, 255, 255, 255 }, { 255, 195, 175, 255 }, { 210, 255, 0, 255 },
    { 255, 255, 255, 255 }, { 210, 255, 0, 255 },   { 255, 195, 175, 255 },
    { 255, 255, 255, 255 }, { 255, 195, 175, 255 }, { 210, 255, 0, 255 },
};

void EnDntNomal_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDntNomal* this = (EnDntNomal*)thisx;

    this->type = this->actor.params;
    if (this->type < ENDNTNOMAL_TARGET) {
        this->type = ENDNTNOMAL_TARGET;
    }
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.colChkInfo.mass = 0xFF;
    this->objId = -1;
    if (this->type == ENDNTNOMAL_TARGET) {
        osSyncPrintf("\n\n");
        // "Deku Scrub target"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ デグナッツ的当て ☆☆☆☆☆ \n" VT_RST);
        Collider_InitQuad(play, &this->targetQuad);
        Collider_SetQuad(play, &this->targetQuad, &this->actor, &sTargetQuadInit);
        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->objId = OBJECT_HINTNUTS;
    } else {
        osSyncPrintf("\n\n");
        // "Deku Scrub mask show audience"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ デグナッツお面品評会一般人 ☆☆☆☆☆ \n" VT_RST);
        Collider_InitCylinder(play, &this->bodyCyl);
        Collider_SetCylinder(play, &this->bodyCyl, &this->actor, &sBodyCylinderInit);
        this->objId = OBJECT_DNK;
    }
    if (this->objId >= 0) {
        this->objIndex = Object_GetIndex(&play->objectCtx, this->objId);
        if (this->objIndex < 0) {
            Actor_Kill(&this->actor);
            // "What?"
            osSyncPrintf(VT_FGCOL(PURPLE) " なにみの？ %d\n" VT_RST "\n", this->objIndex);
            // "Bank is funny"
            osSyncPrintf(VT_FGCOL(CYAN) " バンクおかしいしぞ！%d\n" VT_RST "\n", this->actor.params);
            return;
        }
    } else {
        Actor_Kill(&this->actor);
    }
    this->actionFunc = EnDntNomal_WaitForObject;
}

void EnDntNomal_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDntNomal* this = (EnDntNomal*)thisx;

    if (this->type == ENDNTNOMAL_TARGET) {
        Collider_DestroyQuad(play, &this->targetQuad);
    } else {
        Collider_DestroyCylinder(play, &this->bodyCyl);
    }
}

void EnDntNomal_WaitForObject(EnDntNomal* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objIndex)) {
        gSegments[6] = PHYSICAL_TO_VIRTUAL(play->objectCtx.status[this->objIndex].segment);
        this->actor.objBankIndex = this->objIndex;
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
        this->actor.gravity = -2.0f;
        Actor_SetScale(&this->actor, 0.01f);
        if (this->type == ENDNTNOMAL_TARGET) {
            SkelAnime_Init(play, &this->skelAnime, &gHintNutsSkel, &gHintNutsBurrowAnim, this->jointTable,
                           this->morphTable, 10);
            this->actor.draw = EnDntNomal_DrawTargetScrub;
        } else {
            SkelAnime_Init(play, &this->skelAnime, &gDntStageSkel, &gDntStageHideAnim, this->jointTable,
                           this->morphTable, 11);
            this->actor.draw = EnDntNomal_DrawStageScrub;
        }
        this->actionFunc = EnDntNomal_SetFlower;
    }
}

void EnDntNomal_SetFlower(EnDntNomal* this, PlayState* play) {
    if (this->actor.bgCheckFlags & 1) {
        this->flowerPos = this->actor.world.pos;
        if (this->type == ENDNTNOMAL_TARGET) {
            this->actionFunc = EnDntNomal_SetupTargetWait;
        } else {
            this->actionFunc = EnDntNomal_SetupStageWait;
        }
    }
}

void EnDntNomal_SetupTargetWait(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Animation_GetLastFrame(&gHintNutsBurrowAnim);
    Animation_Change(&this->skelAnime, &gHintNutsBurrowAnim, 0.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
    this->skelAnime.curFrame = 8.0f;
    this->actionFunc = EnDntNomal_TargetWait;
}

void EnDntNomal_TargetWait(EnDntNomal* this, PlayState* play) {
    Vec3f scorePos;
    f32 targetX = 1340.0f;
    f32 targetY = 50.0f;
    f32 targetZ = -30.0f;
    f32 dx;
    f32 dy;
    f32 dz;
    Vec3f scoreAccel = { 0.0f, 0.0f, 0.0f };
    Vec3f scoreVel = { 0.0f, 0.0f, 0.0f };

    this->targetVtx[0].x = this->targetVtx[1].x = this->targetVtx[2].x = this->targetVtx[3].x = targetX;

    this->targetVtx[1].y = this->targetVtx[0].y = targetY - 24.0f;

    this->targetVtx[2].z = this->targetVtx[0].z = targetZ + 24.0f;

    this->targetVtx[3].z = this->targetVtx[1].z = targetZ - 24.0f;

    this->targetVtx[3].y = this->targetVtx[2].y = targetY + 24.0f;

    SkelAnime_Update(&this->skelAnime);
    if ((this->targetQuad.base.acFlags & AC_HIT) || BREG(0)) {
        this->targetQuad.base.acFlags &= ~AC_HIT;

        dx = fabsf(targetX - this->targetQuad.info.bumper.hitPos.x);
        dy = fabsf(targetY - this->targetQuad.info.bumper.hitPos.y);
        dz = fabsf(targetZ - this->targetQuad.info.bumper.hitPos.z);

        scoreVel.y = 5.0f;

        if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 8.0f) {
            scorePos.x = this->actor.world.pos.x - 20.0f;
            scorePos.y = this->actor.world.pos.y + 20.0f;
            scorePos.z = this->actor.world.pos.z;
            EffectSsExtra_Spawn(play, &scorePos, &scoreVel, &scoreAccel, 4, 2);
            Audio_StopSfxById(NA_SE_SY_TRE_BOX_APPEAR);
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            // "Big hit"
            osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 大当り ☆☆☆☆☆ %d\n" VT_RST, this->hitCounter);
            if (!LINK_IS_ADULT && !(gSaveContext.itemGetInf[1] & 0x2000)) {
                this->hitCounter++;
                if (this->hitCounter >= 3) {
                    if(gSaveContext.n64ddFlag) {
                        this->actionFunc = EnDntNomal_TargetGivePrize;
                    } else {
                        OnePointCutscene_Init(play, 4140, -99, &this->actor, MAIN_CAM);
                        func_8002DF54(play, &this->actor, 1);
                        this->timer4 = 50;
                        this->actionFunc = EnDntNomal_SetupTargetUnburrow;
                    }
                }
            }
        } else if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 24.0f) {
            scorePos.x = this->actor.world.pos.x;
            scorePos.y = this->actor.world.pos.y + 20.0f;
            scorePos.z = this->actor.world.pos.z;
            EffectSsExtra_Spawn(play, &scorePos, &scoreVel, &scoreAccel, 4, 0);
            this->hitCounter = 0;
        }
    }
}

void EnDntNomal_SetupTargetUnburrow(EnDntNomal* this, PlayState* play) {
    Vec3f spawnPos;

    if (this->timer4 == 0) {
        this->endFrame = (f32)Animation_GetLastFrame(&gHintNutsUnburrowAnim);
        Animation_Change(&this->skelAnime, &gHintNutsUnburrowAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        spawnPos = this->actor.world.pos;
        spawnPos.y = this->actor.world.pos.y + 50.0f;
        EffectSsHahen_SpawnBurst(play, &spawnPos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_UP);
        this->actionFunc = EnDntNomal_TargetUnburrow;
    }
}

void EnDntNomal_TargetUnburrow(EnDntNomal* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);
    if (frame >= this->endFrame) {
        this->actionFunc = EnDntNomal_SetupTargetWalk;
    }
}

void EnDntNomal_SetupTargetWalk(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Animation_GetLastFrame(&gHintNutsRunAnim);
    Animation_Change(&this->skelAnime, &gHintNutsRunAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.speedXZ = 1.0f;
    this->actor.colChkInfo.mass = 0;
    this->actionFunc = EnDntNomal_TargetWalk;
}

void EnDntNomal_TargetWalk(EnDntNomal* this, PlayState* play) {
    f32 dx;
    f32 dz;

    SkelAnime_Update(&this->skelAnime);
    dx = 1340.0f + 3.0f - this->actor.world.pos.x;
    dz = 0.0f - this->actor.world.pos.z;
    Math_SmoothStepToS(&this->actor.shape.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / M_PI), 0x32, 0xBB8, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 6.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if (this->actor.world.pos.z > -30.0f) {
        this->actor.speedXZ = 0.0f;
        this->actionFunc = EnDntNomal_TargetFacePlayer;
    }
}

void EnDntNomal_TargetFacePlayer(EnDntNomal* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 6.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if (fabsf(this->actor.shape.rot.y - this->actor.yawTowardsPlayer) < 30.0f) {
        this->actionFunc = EnDntNomal_SetupTargetTalk;
    }
}

void EnDntNomal_SetupTargetTalk(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Animation_GetLastFrame(&gHintNutsTalkAnim);
    Animation_Change(&this->skelAnime, &gHintNutsTalkAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.textId = 0x10AF;
    Message_StartTextbox(play, this->actor.textId, NULL);
    this->actionFunc = EnDntNomal_TargetTalk;
}

void EnDntNomal_TargetTalk(EnDntNomal* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        func_8005B1A4(GET_ACTIVE_CAM(play));
        GET_ACTIVE_CAM(play)->csId = 0;
        func_8002DF54(play, NULL, 8);
        this->actionFunc = EnDntNomal_SetupTargetGivePrize;
    }
}

void EnDntNomal_SetupTargetGivePrize(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Animation_GetLastFrame(&gHintNutsSpitAnim);
    Animation_Change(&this->skelAnime, &gHintNutsSpitAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = EnDntNomal_TargetGivePrize;
}

void EnDntNomal_TargetGivePrize(EnDntNomal* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);
    if ((frame >= 8.0f) && !this->spawnedItem) {
        f32 itemX = this->mouthPos.x - 10.0f;
        f32 itemY = this->mouthPos.y;
        f32 itemZ = this->mouthPos.z;

        if (Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_EX_ITEM, itemX, itemY, itemZ, 0,
                               0, 0, EXITEM_BULLET_BAG) == NULL) {
            func_8002DF54(play, NULL, 7);
            Actor_Kill(&this->actor);
        }
        this->spawnedItem = true;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_THROW);
    }
    if (frame >= this->endFrame) {
        this->endFrame = (f32)Animation_GetLastFrame(&gHintNutsRunAnim);
        Animation_Change(&this->skelAnime, &gHintNutsRunAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
        this->actionFunc = EnDntNomal_TargetReturn;
    }
}

void EnDntNomal_TargetReturn(EnDntNomal* this, PlayState* play) {
    f32 dx;
    f32 dz;

    SkelAnime_Update(&this->skelAnime);
    dx = this->flowerPos.x - this->actor.world.pos.x;
    dz = -180.0f - this->actor.world.pos.z;

    Math_SmoothStepToS(&this->actor.shape.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / M_PI), 3, 0x1388, 0);
    if (fabsf(this->actor.shape.rot.y - (s16)(Math_FAtan2F(dx, dz) * (0x8000 / M_PI))) < 20.0f) {
        this->actor.speedXZ = 1.0f;
    }
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 6.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (this->actor.world.pos.z < -172.0f) {
        this->endFrame = (f32)Animation_GetLastFrame(&gHintNutsBurrowAnim);
        Animation_Change(&this->skelAnime, &gHintNutsBurrowAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->actor.world.pos.z = -173.0f;
        this->actor.speedXZ = 0.0f;
        this->actionFunc = EnDntNomal_TargetBurrow;
    }
}

void EnDntNomal_TargetBurrow(EnDntNomal* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);
    if (frame >= this->endFrame) {
        this->actionFunc = EnDntNomal_SetupTargetWait;
    }
}

void EnDntNomal_SetupStageWait(EnDntNomal* this, PlayState* play) {
    if (this->timer3 == 0) {
        this->endFrame = (f32)Animation_GetLastFrame(&gDntStageHideAnim);
        Animation_Change(&this->skelAnime, &gDntStageHideAnim, 0.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->skelAnime.curFrame = 8.0f;
        this->isSolid = false;
        this->actionFunc = EnDntNomal_StageWait;
    }
}

void EnDntNomal_StageWait(EnDntNomal* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnDntNomal_SetupStageUp(EnDntNomal* this, PlayState* play) {
    if (this->timer3 == 0) {
        this->endFrame = (f32)Animation_GetLastFrame(&gDntStageUpAnim);
        Animation_Change(&this->skelAnime, &gDntStageUpAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        if (this->action != DNT_ACTION_ATTACK) {
            this->rotDirection = -1;
        }
        EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_UP);
        this->isSolid = true;
        this->actionFunc = EnDntNomal_StageUp;
    }
}

void EnDntNomal_StageUp(EnDntNomal* this, PlayState* play) {
    s16 rotTarget;
    f32 frame = this->skelAnime.curFrame;
    f32 turnMod;

    SkelAnime_Update(&this->skelAnime);
    if ((frame >= this->endFrame) && (this->action == DNT_ACTION_ATTACK)) {
        this->actionFunc = EnDntNomal_SetupStageAttack;
    } else {
        if (this->timer4 == 0) {
            turnMod = 0.0f;
            if (this->stagePrize == DNT_PRIZE_NONE) {
                Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
            } else {
                f32 dx = this->targetPos.x - this->actor.world.pos.x;
                f32 dz = this->targetPos.z - this->actor.world.pos.z;

                Math_SmoothStepToS(&this->actor.shape.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / M_PI), 1, 0xBB8, 0);
                turnMod = 90.0f;
            }
            if ((Rand_ZeroFloat(10.0f + turnMod) < 1.0f) && (this->action != DNT_ACTION_ATTACK)) {
                this->timer4 = (s16)Rand_ZeroFloat(30.0f) + 30;
            }
        } else {
            if (this->timer2 == 0) {
                this->rotDirection++;
                if (this->rotDirection > 1) {
                    this->rotDirection = -1;
                }
                this->timer2 = (s16)Rand_ZeroFloat(10.0f) + 10;
            }
            rotTarget = this->actor.yawTowardsPlayer;
            if (this->rotDirection != 0) {
                rotTarget += this->rotDirection * 0x1388;
            }
            Math_SmoothStepToS(&this->actor.shape.rot.y, rotTarget, 3, 0x1388, 0);
        }
        if (this->actor.xzDistToPlayer < 70.0f) {
            this->actionFunc = EnDntNomal_SetupStageHide;
        }
    }
}

void EnDntNomal_SetupStageUnburrow(EnDntNomal* this, PlayState* play) {
    if (this->timer3 == 0) {
        this->endFrame = (f32)Animation_GetLastFrame(&gDntStageUnburrowAnim);
        Animation_Change(&this->skelAnime, &gDntStageUnburrowAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->isSolid = false;
        EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_UP);
        this->actionFunc = EnDntNomal_StageUnburrow;
    }
}

void EnDntNomal_StageUnburrow(EnDntNomal* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);
    if (frame >= this->endFrame) {
        if (this->action != DNT_ACTION_DANCE) {
            this->timer3 = (s16)Rand_ZeroFloat(2.0f) + (s16)(this->type * 0.5f);
            this->actionFunc = EnDntNomal_SetupStageCelebrate;
        } else {
            this->timer2 = 300;
            this->actionFunc = EnDntNomal_SetupStageDance;
        }
    }
}

void EnDntNomal_SetupStageCelebrate(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Animation_GetLastFrame(&gDntStageWalkAnim);
    Animation_Change(&this->skelAnime, &gDntStageWalkAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.speedXZ = 3.0f;
    this->isSolid = true;
    this->actionFunc = EnDntNomal_StageCelebrate;
}

void EnDntNomal_StageCelebrate(EnDntNomal* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((this->timer1 == 0) && (this->timer3 == 0)) {
        f32 dx = this->targetPos.x - this->actor.world.pos.x;
        f32 dz = this->targetPos.z - this->actor.world.pos.z;

        if ((fabsf(dx) < 10.0f) && (fabsf(dz) < 10.0f) && (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE)) {
            this->action = DNT_ACTION_PRIZE;
            this->actionFunc = EnDntNomal_SetupStageDance;
            this->actor.speedXZ = 0.0f;
            return;
        }
        Math_SmoothStepToS(&this->actor.shape.rot.y, Math_FAtan2F(dx, dz) * (0x8000 / M_PI), 1, 0xBB8, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else {
        if (this->timer1 == 1) {
            this->timer3 = (s16)Rand_ZeroFloat(20.0f) + 20.0f;
        }
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x14, 0x1388, 0);
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
    }
    if (this->timer5 == 0) {
        this->timer5 = 20;
        if ((this->type & 1) == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_DAMAGE);
        }
    } else if ((this->timer5 & 3) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((this->actor.bgCheckFlags & 8) && (this->actor.bgCheckFlags & 1)) {
        this->actor.velocity.y = 7.5f;
    }
}

void EnDntNomal_SetupStageDance(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Animation_GetLastFrame(&gDntStageDanceAnim);
    Animation_Change(&this->skelAnime, &gDntStageDanceAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->isSolid = true;
    this->timer3 = (s16)Rand_ZeroFloat(20.0f) + 20.0f;
    this->rotDirection = -1;
    if (Rand_ZeroFloat(1.99f) < 1.0f) {
        this->rotDirection = 1;
    }
    this->actionFunc = EnDntNomal_StageDance;
}

void EnDntNomal_StageDance(EnDntNomal* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->timer2 == 0) {
        if (this->action == DNT_ACTION_DANCE) {
            this->action = DNT_ACTION_HIGH_RUPEES;
            this->actionFunc = EnDntNomal_SetupStageHide;
        } else {
            this->action = DNT_ACTION_NONE;
            this->actionFunc = EnDntNomal_StageSetupReturn;
        }
    } else if (this->timer3 != 0) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
        if (this->timer3 == 1) {
            this->timer4 = (s16)Rand_ZeroFloat(20.0f) + 20.0f;
            this->rotDirection = -this->rotDirection;
        }
    } else if (this->timer4 != 0) {
        this->actor.shape.rot.y += this->rotDirection * 0x800;
        if (this->timer4 == 1) {
            this->timer3 = (s16)Rand_ZeroFloat(20.0f) + 20.0f;
        }
    }
}

void EnDntNomal_SetupStageHide(EnDntNomal* this, PlayState* play) {
    if (this->timer3 != 0) {
        if ((this->timer3 == 1) && (this->ignore == 1)) {
            func_80078884(NA_SE_SY_ERROR);
        }
    } else {
        this->endFrame = (f32)Animation_GetLastFrame(&gDntStageHideAnim);
        Animation_Change(&this->skelAnime, &gDntStageHideAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->isSolid = false;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_DOWN);
        this->actionFunc = EnDntNomal_StageHide;
    }
}

void EnDntNomal_StageHide(EnDntNomal* this, PlayState* play) {
    EnExRuppy* rupee;
    f32 frame = this->skelAnime.curFrame;
    s16 rupeeColor;

    SkelAnime_Update(&this->skelAnime);
    if (frame >= this->endFrame) {
        EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 4.0f, 0, 10, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_UP);
        switch (this->action) {
            case DNT_ACTION_NONE:
                this->actionFunc = EnDntNomal_SetupStageWait;
                break;
            case DNT_ACTION_ATTACK:
                this->actionFunc = EnDntNomal_StageAttackHide;
                break;
            case DNT_ACTION_LOW_RUPEES:
            case DNT_ACTION_HIGH_RUPEES:
                rupee =
                    (EnExRuppy*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_EX_RUPPY, this->actor.world.pos.x,
                                            this->actor.world.pos.y + 20.0f, this->actor.world.pos.z, 0, 0, 0, 3);
                if (rupee != NULL) {
                    rupeeColor = this->action - DNT_ACTION_LOW_RUPEES;
                    rupee->colorIdx = rupeeColor;
                    if (Rand_ZeroFloat(3.99f) < 1.0f) {
                        rupee->colorIdx = rupeeColor + 1;
                    }
                    rupee->actor.velocity.y = 5.0f;
                    if (rupee->colorIdx == 2) {
                        rupee->actor.velocity.y = 7.0f;
                    }
                    func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                }
                this->action = DNT_ACTION_NONE;
                this->actionFunc = EnDntNomal_SetupStageWait;
                break;
        }
    }
}

void EnDntNomal_StageAttackHide(EnDntNomal* this, PlayState* play) {
    if (this->actor.xzDistToPlayer > 70.0f) {
        this->actionFunc = EnDntNomal_SetupStageUp;
    }
}

void EnDntNomal_SetupStageAttack(EnDntNomal* this, PlayState* play) {
    if (this->timer3 == 0) {
        this->endFrame = (f32)Animation_GetLastFrame(&gDntStageSpitAnim);
        Animation_Change(&this->skelAnime, &gDntStageSpitAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
        this->actor.colChkInfo.mass = 0xFF;
        this->isSolid = true;
        this->timer2 = 0;
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        this->actionFunc = EnDntNomal_StageAttack;
    }
}

void EnDntNomal_StageAttack(EnDntNomal* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* nut;
    f32 frame = this->skelAnime.curFrame;
    f32 dz;
    f32 dx;
    f32 dy;

    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
    dx = player->actor.world.pos.x - this->mouthPos.x;
    dy = player->actor.world.pos.y + 30.0f - this->mouthPos.y;
    dz = player->actor.world.pos.z - this->mouthPos.z;
    Math_SmoothStepToS(&this->actor.shape.rot.x, -(s16)(Math_FAtan2F(dy, sqrtf(SQ(dx) + SQ(dz))) * (0x8000 / M_PI)), 3,
                       0x1388, 0);
    if ((frame >= this->endFrame) && (this->timer2 == 0)) {
        this->timer2 = (s16)Rand_ZeroFloat(10.0f) + 10;
    }
    if (this->timer2 == 1) {
        this->spawnedItem = false;
        this->actionFunc = EnDntNomal_SetupStageAttack;
    } else if (this->actor.xzDistToPlayer < 50.0f) {
        this->action = DNT_ACTION_ATTACK;
        this->actionFunc = EnDntNomal_SetupStageHide;
    } else if ((frame >= 8.0f) && (!this->spawnedItem)) {
        Vec3f baseOffset;
        Vec3f spawnOffset;
        f32 spawnX;
        f32 spawnY;
        f32 spawnZ;

        Matrix_RotateY(this->actor.shape.rot.y / (f32)0x8000 * M_PI, MTXMODE_NEW);
        Matrix_RotateX(this->actor.shape.rot.x / (f32)0x8000 * M_PI, MTXMODE_APPLY);
        baseOffset.x = 0.0f;
        baseOffset.y = 0.0f;
        baseOffset.z = 5.0f;
        Matrix_MultVec3f(&baseOffset, &spawnOffset);
        spawnX = this->mouthPos.x + spawnOffset.x;
        spawnY = this->mouthPos.y + spawnOffset.y;
        spawnZ = this->mouthPos.z + spawnOffset.z;

        nut = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_NUTSBALL, spawnX, spawnY, spawnZ,
                          this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, 4);
        if (nut != NULL) {
            nut->velocity.y = spawnOffset.y * 0.5f;
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_THROW);
        this->spawnedItem = true;
    }
}

void EnDntNomal_StageSetupReturn(EnDntNomal* this, PlayState* play) {
    this->endFrame = (f32)Animation_GetLastFrame(&gDntStageWalkAnim);
    Animation_Change(&this->skelAnime, &gDntStageWalkAnim, 1.5f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.speedXZ = 4.0f;
    this->isSolid = false;
    this->actionFunc = EnDntNomal_StageReturn;
}

void EnDntNomal_StageReturn(EnDntNomal* this, PlayState* play) {
    f32 sp2C;
    f32 sp28;

    SkelAnime_Update(&this->skelAnime);
    sp2C = this->flowerPos.x - this->actor.world.pos.x;
    sp28 = this->flowerPos.z - this->actor.world.pos.z;
    Math_SmoothStepToS(&this->actor.shape.rot.y, Math_FAtan2F(sp2C, sp28) * (0x8000 / M_PI), 1, 0xBB8, 0);
    if (this->timer5 == 0) {
        this->timer5 = 10;
    } else if (!(this->timer5 & 1)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((fabsf(sp2C) < 7.0f) && (fabsf(sp28) < 7.0f)) {
        this->actor.world.pos.x = this->flowerPos.x;
        this->actor.world.pos.z = this->flowerPos.z;
        this->actor.speedXZ = 0.0f;
        this->actionFunc = EnDntNomal_SetupStageHide;
    }
}

void EnDntNomal_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDntNomal* this = (EnDntNomal*)thisx;

    if (this->timer1 != 0) {
        this->timer1--;
    }
    if (this->timer2 != 0) {
        this->timer2--;
    }
    if (this->timer3 != 0) {
        this->timer3--;
    }
    if (this->timer4 != 0) {
        this->timer4--;
    }
    if (this->timer5 != 0) {
        this->timer5--;
    }
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }
    this->actor.world.rot.x = this->actor.shape.rot.x;
    if (this->actionFunc != EnDntNomal_StageCelebrate) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
    this->unkCounter++;
    if (this->type != ENDNTNOMAL_TARGET) {
        switch (this->stageSignal) {
            case DNT_SIGNAL_LOOK:
                if (this->stagePrize == DNT_PRIZE_NONE) {
                    this->actionFunc = EnDntNomal_SetupStageUp;
                } else {
                    this->actionFunc = EnDntNomal_StageUp;
                }
                break;
            case DNT_SIGNAL_CELEBRATE:
                this->action = DNT_ACTION_NONE;
                this->actor.colChkInfo.mass = 0;
                this->timer3 = (s16)Rand_ZeroFloat(3.0f) + (s16)(this->type * 0.5f);
                this->actionFunc = EnDntNomal_SetupStageUnburrow;
                break;
            case DNT_SIGNAL_DANCE:
                this->action = DNT_ACTION_DANCE;
                this->actionFunc = EnDntNomal_SetupStageUnburrow;
                break;
            case DNT_SIGNAL_HIDE:
                this->actionFunc = EnDntNomal_SetupStageHide;
                break;
            case DNT_SIGNAL_RETURN:
                this->actionFunc = EnDntNomal_StageSetupReturn;
                break;
            case DNT_SIGNAL_UNUSED:
                this->actionFunc = EnDntNomal_SetupStageDance;
                break;
            case DNT_SIGNAL_NONE:
                break;
        }
    }
    if (this->stageSignal != DNT_SIGNAL_NONE) {
        this->stageSignal = DNT_SIGNAL_NONE;
    }
    if (this->blinkTimer == 0) {
        this->eyeState++;
        if (this->eyeState >= 3) {
            this->eyeState = 0;
            this->blinkTimer = (s16)Rand_ZeroFloat(60.0f) + 20;
        }
    }
    this->actionFunc(this, play);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 60.0f, 0x1D);
    if (this->type == ENDNTNOMAL_TARGET) {
        Collider_SetQuadVertices(&this->targetQuad, &this->targetVtx[0], &this->targetVtx[1], &this->targetVtx[2],
                                 &this->targetVtx[3]);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->targetQuad.base);
    } else {
        Collider_UpdateCylinder(&this->actor, &this->bodyCyl);
        if (this->isSolid) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCyl.base);
        }
    }
}

s32 EnDntNomal_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnDntNomal* this = (EnDntNomal*)thisx;

    if ((limbIndex == 1) || (limbIndex == 3) || (limbIndex == 4) || (limbIndex == 5) || (limbIndex == 6)) {
        OPEN_DISPS(play->state.gfxCtx);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, sLeafColors[this->type - ENDNTNOMAL_STAGE].r,
                       sLeafColors[this->type - ENDNTNOMAL_STAGE].g, sLeafColors[this->type - ENDNTNOMAL_STAGE].b, 255);
        CLOSE_DISPS(play->state.gfxCtx);
    }
    return false;
}

void EnDntNomal_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnDntNomal* this = (EnDntNomal*)thisx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (this->type == ENDNTNOMAL_TARGET) {
        if (limbIndex == 5) {
            Matrix_MultVec3f(&zeroVec, &this->mouthPos);
        }
    } else if (limbIndex == 7) {
        Matrix_MultVec3f(&zeroVec, &this->mouthPos);
    }
}

void EnDntNomal_DrawStageScrub(Actor* thisx, PlayState* play) {
    static void* blinkTex[] = { gDntStageEyeOpenTex, gDntStageEyeHalfTex, gDntStageEyeShutTex };
    EnDntNomal* this = (EnDntNomal*)thisx;
    Vec3f dustScale = { 0.25f, 0.25f, 0.25f };
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D18(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(blinkTex[this->eyeState]));
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnDntNomal_OverrideLimbDraw,
                      EnDntNomal_PostLimbDraw, this);
    Matrix_Translate(this->flowerPos.x, this->flowerPos.y, this->flowerPos.z, MTXMODE_NEW);
    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, sLeafColors[this->type - ENDNTNOMAL_STAGE].r,
                   sLeafColors[this->type - ENDNTNOMAL_STAGE].g, sLeafColors[this->type - ENDNTNOMAL_STAGE].b, 255);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gDntStageFlowerDL);
    CLOSE_DISPS(play->state.gfxCtx);
    if (this->actionFunc == EnDntNomal_StageCelebrate) {
        func_80033C30(&this->actor.world.pos, &dustScale, 255, play);
    }
}

void EnDntNomal_DrawTargetScrub(Actor* thisx, PlayState* play) {
    EnDntNomal* this = (EnDntNomal*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D18(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, EnDntNomal_PostLimbDraw,
                      this);
    Matrix_Translate(this->flowerPos.x, this->flowerPos.y, this->flowerPos.z, MTXMODE_NEW);
    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gHintNutsFlowerDL);
    CLOSE_DISPS(play->state.gfxCtx);
}
