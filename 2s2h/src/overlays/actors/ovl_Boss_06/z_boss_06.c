/*
 * File: z_boss_06.c
 * Overlay: ovl_Boss_06
 * Description: Igos du Ikana window - curtains and ray effects
 */

#include "prevent_bss_reordering.h"
#include "z_boss_06.h"
#include "z64shrink_window.h"
#include "overlays/actors/ovl_En_Knight/z_en_knight.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_knight/object_knight.h"

#include "2s2h/BenPort.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Boss06_Init(Actor* thisx, PlayState* play);
void Boss06_Destroy(Actor* thisx, PlayState* play);
void Boss06_Update(Actor* thisx, PlayState* play);
void Boss06_Draw(Actor* thisx, PlayState* play2);
void Boss06_Reset(void);

void Boss06_SetupCurtainBurningCutscene(Boss06* this);
void Boss06_CurtainBurningCutscene(Boss06* this, PlayState* play);
void Boss06_SetupCloseCurtain(Boss06* this, PlayState* play);
void Boss06_CloseCurtain(Boss06* this, PlayState* play);
void Boss06_SetupCurtainClosed(Boss06* this, PlayState* play);
void Boss06_CurtainClosed(Boss06* this, PlayState* play);
void Boss06_SetupCurtainDestroyed(Boss06* this, PlayState* play);
void Boss06_CurtainDestroyed(Boss06* this, PlayState* play);

Vec3f sCurtainFireEffectPositions[128];
static EnKnight* sIgosInstance;
s32 sBoss06Seed1;
s32 sBoss06Seed2;
s32 sBoss06Seed3;

typedef enum {
    /*  0 */ BOSS06_DMGEFF_0 = 0,
    /*  2 */ BOSS06_DMGEFF_FIRE_ARROW = 2,
    /*  3 */ BOSS06_DMGEFF_ICE_ARROW,
    /*  4 */ BOSS06_DMGEFF_LIGHT_ARROW,
    /* 15 */ BOSS06_DMGEFF_F = 15
} Boss06DamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Deku Stick     */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Horse trample  */ DMG_ENTRY(0, BOSS06_DMGEFF_0),
    /* Explosives     */ DMG_ENTRY(1, BOSS06_DMGEFF_F),
    /* Zora boomerang */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Normal arrow   */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, BOSS06_DMGEFF_0),
    /* Hookshot       */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Goron punch    */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Sword          */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Goron pound    */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Fire arrow     */ DMG_ENTRY(1, BOSS06_DMGEFF_FIRE_ARROW),
    /* Ice arrow      */ DMG_ENTRY(0, BOSS06_DMGEFF_ICE_ARROW),
    /* Light arrow    */ DMG_ENTRY(0, BOSS06_DMGEFF_LIGHT_ARROW),
    /* Goron spikes   */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Deku spin      */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Deku bubble    */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Deku launch    */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Zora barrier   */ DMG_ENTRY(0, BOSS06_DMGEFF_0),
    /* Normal shield  */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Light ray      */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Thrown object  */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Zora punch     */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Spin attack    */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* Sword beam     */ DMG_ENTRY(0, BOSS06_DMGEFF_0),
    /* Normal Roll    */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, BOSS06_DMGEFF_0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, BOSS06_DMGEFF_0),
    /* Unblockable    */ DMG_ENTRY(0, BOSS06_DMGEFF_0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, BOSS06_DMGEFF_0),
    /* Powder Keg     */ DMG_ENTRY(0, BOSS06_DMGEFF_F),
};

ActorProfile Boss_06_Profile = {
    /**/ ACTOR_BOSS_06,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_KNIGHT,
    /**/ sizeof(Boss06),
    /**/ Boss06_Init,
    /**/ Boss06_Destroy,
    /**/ Boss06_Update,
    /**/ Boss06_Draw,
    /**/ Boss06_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK3,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7FFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 90, 140, 10, { 0, 0, 0 } },
};

static Vec3f sCurtainLocations[] = {
    { 1081.0f, 235.0f, 3224.0f },
    { 676.0f, 235.0f, 3224.0f },
};

#define BOSS06_DRAWFLAG_CURTAIN (1 << 0)
#define BOSS06_DRAWFLAG_LIGHT_RAY (1 << 1)

typedef enum {
    /* 0 */ BOSS06_CS_STATE_BEGIN,
    /* 1 */ BOSS06_CS_STATE_SHOW_BURNING_AND_REACTIONS,
    /* 2 */ BOSS06_CS_STATE_PAN_OVER_LIGHT_RAY
} Boss06CutsceneState;

void Boss06_InitRand(s32 seedInit1, s32 seedInit2, s32 seedInit3) {
    sBoss06Seed1 = seedInit1;
    sBoss06Seed2 = seedInit2;
    sBoss06Seed3 = seedInit3;
}

f32 Boss06_RandZeroOne(void) {
    // Wichmann-Hill algorithm
    f32 randFloat;

    sBoss06Seed1 = (sBoss06Seed1 * 171) % 30269;
    sBoss06Seed2 = (sBoss06Seed2 * 172) % 30307;
    sBoss06Seed3 = (sBoss06Seed3 * 170) % 30323;

    randFloat = (sBoss06Seed1 / 30269.0f) + (sBoss06Seed2 / 30307.0f) + (sBoss06Seed3 / 30323.0f);
    while (randFloat >= 1.0f) {
        randFloat -= 1.0f;
    }
    return fabsf(randFloat);
}

void Boss06_Init(Actor* thisx, PlayState* play) {
    Boss06* this = (Boss06*)thisx;
    u8* curtainTexture;
    s32 i;

    sIgosInstance = (EnKnight*)this->actor.parent;
    this->actor.colChkInfo.damageTable = &sDamageTable;

    if ((KREG(64) != 0) || CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_IGOS_DU_IKANA)) {
        this->actionFunc = Boss06_SetupCurtainClosed;
    } else {
        this->actionFunc = Boss06_SetupCloseCurtain;
    }

    Actor_SetScale(&this->actor, 0.1f);
    Math_Vec3f_Copy(&this->actor.world.pos, &sCurtainLocations[ENBOSS06_GET_PARAMS(&this->actor)]);
    this->actor.shape.rot.y = -0x8000;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    curtainTexture = ResourceMgr_LoadTexOrDListByName(&gIkanaThroneRoomCurtainTex);
    for (i = 0; i < ARRAY_COUNT(this->curtainTexture); i++) {
        this->curtainTexture[i] = curtainTexture[i];
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void Boss06_Destroy(Actor* thisx, PlayState* play) {
}

void Boss06_UpdateDamage(Boss06* this) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;

        if ((this->csState == BOSS06_CS_STATE_BEGIN) && (sIgosInstance->subCamId == CAM_ID_MAIN)) {
            if (this->actor.colChkInfo.damageEffect == BOSS06_DMGEFF_FIRE_ARROW) {
                Boss06_SetupCurtainBurningCutscene(this);
                Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);

                this->arrowHitPos.x = -(this->actor.world.pos.x - this->collider.elem.acDmgInfo.hitPos.x);
                this->arrowHitPosScaled.x = this->arrowHitPos.x * 0.35f;

                this->arrowHitPos.y = -((this->actor.world.pos.y + 80.0f) - this->collider.elem.acDmgInfo.hitPos.y);
                this->arrowHitPosScaled.y = this->arrowHitPos.y * -0.35f;
            }
        }
    }
}

void Boss06_SetupCurtainBurningCutscene(Boss06* this) {
    this->actionFunc = Boss06_CurtainBurningCutscene;
    this->maxStepScale = 0.0f;
    this->lensFlareYOffset = 0.0f;
}

void Boss06_CurtainBurningCutscene(Boss06* this, PlayState* play) {
    s16 frameTarget = 0;
    Player* player = GET_PLAYER(play);
    Actor* searchArrow;
    Actor* searchKnight;
    Actor* child;

    this->csFrameCount++;

    switch (this->csState) {
        case BOSS06_CS_STATE_BEGIN:
            if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            sIgosInstance->inCurtainCutscene = true;
            this->csState = BOSS06_CS_STATE_SHOW_BURNING_AND_REACTIONS;
            this->updateFireEffects = true;
            this->lightOrbAlphaFactor = 255.0f;
            this->lightOrbScale = 0.0f;

            // Kill all arrow actors
            for (searchArrow = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first; searchArrow != NULL;
                 searchArrow = searchArrow->next) {
                if (searchArrow->id == ACTOR_EN_ARROW) {
                    Actor_Kill(searchArrow);
                }
            }

        case BOSS06_CS_STATE_SHOW_BURNING_AND_REACTIONS:
            if (this->csFrameCount >= 10) {
                Math_ApproachF(&this->lensFlareScale, 30.0f, 0.2f, 1.0f);
                play->envCtx.fillScreen = true;
                play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] = play->envCtx.screenFillColor[2] = 0;
                play->envCtx.screenFillColor[3] = this->screenFillAlpha;
                Math_ApproachF(&this->screenFillAlpha, 75.0f, 1.0f, 3.0f);
            }

            if (this->csFrameCount >= 30) {
                Audio_PlaySfx(NA_SE_EV_S_STONE_FLASH);
            }

            if (this->csFrameCount >= 60) {
                play->envCtx.fillScreen = false;
                this->updateFireEffects = false;
                this->lightOrbScale = 0.0f;
                this->fireEffectScale = 0.0f;

                if (this->csFrameCount == 60) {
                    sIgosInstance->roomLightingState++;
                    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_132);
                    player->actor.shape.rot.y = 0;
                    player->actor.world.rot.y = player->actor.shape.rot.y;
                }

                this->subCamEye.x = player->actor.world.pos.x + 20.0f;
                this->subCamEye.y = player->actor.world.pos.y + 20.0f;
                this->subCamEye.z = player->actor.world.pos.z + 30.0f;

                this->subCamAt.x = player->actor.world.pos.x;
                this->subCamAt.y = player->actor.world.pos.y + 26.0f;
                this->subCamAt.z = player->actor.world.pos.z;

                if (this->csFrameCount >= 75) {
                    for (searchKnight = play->actorCtx.actorLists[ACTORCAT_BOSS].first; searchKnight != NULL;
                         searchKnight = searchKnight->next) {
                        if ((searchKnight->id == ACTOR_EN_KNIGHT) && (&sIgosInstance->actor == searchKnight) &&
                            sIgosInstance->inCurtainReaction) {
                            this->subCamEye.x = 1307.0f;
                            this->subCamEye.y = 142.0f;
                            this->subCamEye.z = 2897.0f;

                            this->subCamAt.x = 1376.0f;
                            this->subCamAt.y = 132.0f;
                            this->subCamAt.z = 2860.0f;

                            if (this->csFrameCount == 75) {
                                sIgosInstance->subAction = KNIGHT_SUB_ACTION_CURTAIN_REACT_BEGIN;
                            }
                            frameTarget = 17;
                            break;
                        }
                    }
                }

                if (this->csFrameCount > (s16)(frameTarget + 80)) {
                    this->csState = BOSS06_CS_STATE_PAN_OVER_LIGHT_RAY;
                    this->csFrameCount = 0;
                    this->arrowHitPos.y = 0.0f;
                    this->arrowHitPos.x = 0.0f;
                    this->drawFlags = BOSS06_DRAWFLAG_LIGHT_RAY;
                    this->lightRayBaseOffsetZ = 0.0f;
                    this->lightRayTopVerticesOffset = 0.0f;
                    this->lightOrbScale = 18.0f;
                    this->lightOrbAlphaFactor = 255.0f;
                    this->lightRayBrightness = 1.0f;

                    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_MIR_RAY2, this->actor.world.pos.x,
                                       this->actor.world.pos.y - 200.0f, this->actor.world.pos.z - 170.0f, 15, 0, 0, 1);

                    if (ENBOSS06_GET_PARAMS(&this->actor) == 0) {
                        this->subCamEye.x = this->actor.world.pos.x - 200.0f;
                    } else {
                        this->subCamEye.x = this->actor.world.pos.x + 200.0f;
                    }
                    this->subCamEye.y = (this->actor.world.pos.y - 200.0f) + 100.0f;
                    this->subCamEye.z = this->actor.world.pos.z - 200.0f;

                    this->subCamAt.x = this->actor.world.pos.x;
                    this->subCamAt.y = this->actor.world.pos.y + 80.0f;
                    this->subCamAt.z = this->actor.world.pos.z;
                }
            } else {
                this->subCamEye.x = this->actor.world.pos.x;
                this->subCamEye.y = this->actor.world.pos.y + 60.0f;
                this->subCamEye.z = this->actor.world.pos.z - 210.0f;

                this->subCamAt.x = this->actor.world.pos.x;
                this->subCamAt.y = this->actor.world.pos.y + 80.0f;
                this->subCamAt.z = this->actor.world.pos.z;
            }
            break;

        case BOSS06_CS_STATE_PAN_OVER_LIGHT_RAY:
            child = this->actor.child;

            if (this->csFrameCount == 1) {
                this->subCamMaxStep.x = fabsf(this->subCamAt.x - child->world.pos.x);
                this->subCamMaxStep.y = fabsf(this->subCamAt.y - child->world.pos.y);
                this->subCamMaxStep.z = fabsf(this->subCamAt.z - child->world.pos.z);
            }

            Math_ApproachF(&this->subCamAt.x, child->world.pos.x, 0.15f, this->subCamMaxStep.x * this->maxStepScale);
            Math_ApproachF(&this->subCamAt.y, child->world.pos.y, 0.15f, this->subCamMaxStep.y * this->maxStepScale);
            Math_ApproachF(&this->subCamAt.z, child->world.pos.z, 0.15f, this->subCamMaxStep.z * this->maxStepScale);
            Math_ApproachF(&this->maxStepScale, 1.0f, 1.0f, 0.001f);

            if (this->csFrameCount > 80) {
                Boss06_SetupCurtainDestroyed(this, play);
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                sIgosInstance->inCurtainCutscene = false;
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        ShrinkWindow_Letterbox_SetSizeTarget(27);
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void Boss06_SetupCloseCurtain(Boss06* this, PlayState* play) {
    this->actionFunc = Boss06_CloseCurtain;
    this->drawFlags = BOSS06_DRAWFLAG_LIGHT_RAY | BOSS06_DRAWFLAG_CURTAIN;
    this->curtainHeight = 110.0f;
    this->lightOrbAlphaFactor = 200;
    this->lightOrbScale = 15.0f;
    this->lensFlareScale = 30.0f;
    this->lensFlareYOffset = 0.0f;
    this->lightRayBrightness = 1.0f;
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_MIR_RAY2, this->actor.world.pos.x,
                       this->actor.world.pos.y - 200.0f, this->actor.world.pos.z - 170.0f, 15, 0, 0, 1);
    sIgosInstance->roomLightingState++;
}

void Boss06_CloseCurtain(Boss06* this, PlayState* play) {
    if (sIgosInstance->closeCurtainAction != KNIGHT_CLOSE_CURTAIN_ACTION_0) {
        this->maxStepScale = 0.008f;
        Math_ApproachF(&this->lensFlareScale, 0.0f, 0.2f, this->maxStepScale * 30.0f);
        Math_ApproachF(&this->lensFlareYOffset, -70.0f, 0.2f, this->maxStepScale * 70.0f);
        Math_ApproachF(&this->curtainHeight, 0.0f, 0.2f, this->maxStepScale * 110.0f);
        Math_ApproachF(&this->lightRayTopVerticesOffset, -900.0f, 0.2f, this->maxStepScale * 900.0f);
        Math_ApproachF(&this->lightRayBaseOffsetZ, 1350.0f, 0.2f, this->maxStepScale * 1350.0f);
        Math_ApproachF(&this->lightOrbAlphaFactor, 100.0f, 0.2f, this->maxStepScale * 100.0f);

        if (this->lensFlareScale < 5.0f) {
            Math_ApproachF(&this->lightRayBrightness, 0.0f, 1.0f, 0.03f);
        }

        if ((this->lensFlareScale > 0.1f) && ENBOSS06_GET_PARAMS(&this->actor) == 0) {
            // Plays the sfx for just one of the curtains
            Audio_PlaySfx(NA_SE_EV_CURTAIN_DOWN - SFX_FLAG);
        }
    }

    if (sIgosInstance->closeCurtainAction == KNIGHT_CLOSE_CURTAIN_ACTION_2) {
        Actor_Kill(this->actor.child);
        this->actor.child = NULL;
        Boss06_SetupCurtainClosed(this, play);
    }
}

void Boss06_SetupCurtainClosed(Boss06* this, PlayState* play) {
    this->actionFunc = Boss06_CurtainClosed;
    this->drawFlags = BOSS06_DRAWFLAG_CURTAIN;
}

void Boss06_CurtainClosed(Boss06* this, PlayState* play) {
    this->lightOrbAlphaFactor = 200.0f;
    this->lightOrbScale = 15.0f;
    Boss06_UpdateDamage(this);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    this->collider.dim.pos.z = (this->actor.world.pos.z - 50.0f) + 100.0f;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void Boss06_SetupCurtainDestroyed(Boss06* this, PlayState* play) {
    this->actionFunc = Boss06_CurtainDestroyed;
}

void Boss06_CurtainDestroyed(Boss06* this, PlayState* play) {
    this->lightOrbAlphaFactor = 100.0f;
    this->lightOrbScale = 13.0f;
}

void Boss06_Update(Actor* thisx, PlayState* play) {
    Boss06* this = (Boss06*)thisx;

    this->actionFunc(this, play);

    if (this->unusedTimer1 != 0) {
        this->unusedTimer1--;
    }

    if (this->unusedTimer2 != 0) {
        this->unusedTimer2--;
    }

    if (this->lensFlareScale > 0.0f) {
        if (ENBOSS06_GET_PARAMS(&this->actor) == 0) {
            gCustomLensFlare1On = true;
            gCustomLensFlare1Pos.x = this->actor.world.pos.x + this->arrowHitPos.x;
            gCustomLensFlare1Pos.y = this->actor.world.pos.y + 80.0f + this->arrowHitPos.y + this->lensFlareYOffset;
            gCustomLensFlare1Pos.z = this->actor.world.pos.z;
            D_801F4E44 = this->lensFlareScale;
            D_801F4E48 = 10.0f;
            D_801F4E4C = 0;
        } else {
            gCustomLensFlare2On = true;
            gCustomLensFlare2Pos.x = this->actor.world.pos.x + this->arrowHitPos.x;
            gCustomLensFlare2Pos.y = this->actor.world.pos.y + 80.0f + this->arrowHitPos.y + this->lensFlareYOffset;
            gCustomLensFlare2Pos.z = this->actor.world.pos.z;
            D_801F4E5C = this->lensFlareScale;
            D_801F4E60 = 10.0f;
            D_801F4E64 = 0;
        }
    } else {
        if (ENBOSS06_GET_PARAMS(&this->actor) == 0) {
            gCustomLensFlare1On = false;
        } else {
            gCustomLensFlare2On = false;
        }
    }

    if (this->updateFireEffects && this->updateFireEffects) {
        s32 i;
        Vec3f texCoords;
        f32 distFromHitPos;
        f32 angle0;
        f32 angle1;

        Audio_PlaySfx(NA_SE_EV_FIRE_PLATE - SFX_FLAG);
        this->fireEffectDistanceAdd += 0.6f;
        this->fireEffectDistanceScale2 += 0.1f;
        this->fireEffectDistanceScale1 += 20.0f * 0.001f;
        this->fireEffectScale += 0.00016f;
        this->lightOrbScale += 0.4f;
        Math_ApproachZeroF(&this->arrowHitPos.x, 1.0f, 0.7f);
        Math_ApproachZeroF(&this->arrowHitPos.y, 1.0f, 0.7f);

        angle0 = 0.0f;
        angle1 = 0.0f;

        for (i = 0; i < 8 * ARRAY_COUNT(sCurtainFireEffectPositions); i++) {
            distFromHitPos = (sinf(angle1) * this->fireEffectDistanceScale1 + 1.0f) *
                                 (sinf(angle0) * this->fireEffectDistanceScale2 + this->fireEffectDistanceScale2) +
                             this->fireEffectDistanceAdd;

            angle0 += M_PIf / 64;
            angle1 += 5 * M_PIf / 512;

            Matrix_RotateZF(i * (M_PIf / 512), MTXMODE_NEW);
            Matrix_MultVecY(distFromHitPos, &texCoords);

            texCoords.x += (BOSS06_CURTAIN_TEX_WIDTH / 2) + this->arrowHitPosScaled.x;
            texCoords.y += (BOSS06_CURTAIN_TEX_HEIGHT / 2) + this->arrowHitPosScaled.y;

            if ((i % 8) == 0) {
                Math_Vec3f_Copy(&sCurtainFireEffectPositions[i / 8], &texCoords);
            }

            if ((texCoords.x >= 0.0f) && (texCoords.y >= 0.0f) && (texCoords.x < BOSS06_CURTAIN_TEX_WIDTH) &&
                (texCoords.y < BOSS06_CURTAIN_TEX_HEIGHT)) {
                s32 x = texCoords.x;
                s32 idx = ((s32)texCoords.y * (BOSS06_CURTAIN_TEX_WIDTH / 2));

                idx += (x / 2);

                // The texture format is CI4, only zero one pixel
                if ((x % 2) != 0) {
                    this->curtainTexture[idx] &= 0xF0;
                } else {
                    this->curtainTexture[idx] &= 0xF;
                }

                // 2S2H [Port] Map the coordinates to the blended mask
                this->curtainTexMask[((s32)texCoords.y * 64) + x] = true;
            }
        }
    }
}

void Boss06_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss06* this = (Boss06*)thisx;
    s32 i;
    f32 lightOrbOffsetZ = 0.0f;
    s16 lightRayBaseX;
    s16 lightRayBaseZ;
    Vtx* vertices;
    u16 time;
    u16 pad;
    u8 lightRayAlpha;
    u8 floorLightAlpha;
    s32 lightRayGreenFactor = 255;
    f32 lightRayBlueFactor;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    time = CURRENT_TIME;
    if (time > CLOCK_TIME(12, 0)) {
        time = (DAY_LENGTH - 1) - time;
    }
    lightRayBlueFactor = (f32)time / 0x8000;
    lightRayAlpha = (10.0f * lightRayBlueFactor + 105.0f) * this->lightRayBrightness;
    floorLightAlpha = (40.0f * lightRayBlueFactor + 55.0f) * this->lightRayBrightness;

    if (this->drawFlags & BOSS06_DRAWFLAG_LIGHT_RAY) {
        lightRayBaseX = Math_SinS(sIgosInstance->curtainsLightRayAngle) * 1000.0f;
        lightRayBaseZ = Math_CosS(sIgosInstance->curtainsLightRayAngle) * -2000.0f - 2000.0f;
        // #region 2S2H fix a crash caused by modifying what would be a string. Load the verticies here so the correct
        // thing is modified.
        vertices = ResourceMgr_LoadVtxByName(gIkanaThroneRoomLightRayVtx);
        vertices[0].v.ob[1] = TRUNCF_BINANG(this->lightRayTopVerticesOffset) + 0xE92;
        vertices[3].v.ob[1] = TRUNCF_BINANG(this->lightRayTopVerticesOffset) + 0xE92;
        vertices[4].v.ob[1] = TRUNCF_BINANG(this->lightRayTopVerticesOffset) + 0xE92;
        vertices[7].v.ob[1] = TRUNCF_BINANG(this->lightRayTopVerticesOffset) + 0xE92;

        /*
         * Vertices:
         *  [ 0]    ( 300, 3730,  1112)
         *  [ 1]    ( 300, 2830,  1112)
         *  [ 2]    (-300, 2830,  1112)
         *  [ 3]    (-300, 3730,  1112)
         * Triangles:
         *          ( 0,  1,  2), ( 2,  3,  0)
         * Diagrams:
         *    -300         300
         *     (3)---------(0) 3730
         *      |        .  |
         *      |     .     |
         *  y   |  .        |
         *  ^  (2)---------(1) 2830
         *  |
         *  o--> x
         *
         *
         * Vertices:
         *  [ 4]    ( 300, 3730,  1112)     Shared with above
         *  [ 5]    ( 675,    0, -1800)
         *  [ 6]    (-675,    0, -1800)
         *  [ 7]    (-300, 3730,  1112)     Shared with above
         *  [ 8]    ( 300, 2830,  1112)     Shared with above
         *  [ 9]    ( 675,    0, - 450)
         *  [10]    (-300, 2830,  1112)     Shared with above
         *  [11]    (-675,    0, - 450)
         *  [12]    ( 675,    0, - 450)
         *  [13]    ( 300, 2830,  1112)     Shared with above
         * Triangles:
         *          ( 4,  5,  6), ( 4,  6,  7)
         *          ( 8,  9,  5), ( 8,  5,  4)
         *          (10, 11, 12), (10, 12, 13)
         *          ( 7,  6, 11), ( 7, 11, 10)
         * Diagrams:
         *    -675         675
         *    (11)---------(12) -450
         *      |           |
         *      |           |
         *  z   |           |
         *  ^  (6)---------(5) -1800
         *  |
         *  o--> x
         *    -300         300
         *     (7)---------(4) 3730
         *      |           |
         *      |           |
         *  y   |           |
         *  ^ (10)---------(13) 2830
         *  |
         *  o--> x
         *
         *
         * Vertices:
         *  [14]    (-825,    0, -1950)
         *  [15]    (-825,    0,     0)
         *  [16]    ( 825,    0,     0)
         *  [17]    ( 825,    0, -1950)
         * Triangles:
         *          (14, 15, 16), (14, 16, 17)
         * Diagram:
         *    -825         825
         *    (15)---------(16) 0
         *      |        .  |
         *      |     .     |
         *  z   |  .        |
         *  ^ (14)---------(17) -1950
         *  |
         *  o--> x
         */

        // Vertices 0-3 form a rectangle on the wall.
        // Vertices 4-13 are the 4 faces of the ray.
        // Vertices 14-17 form a rectangle on the floor.

        // Vertices 1,2,8,10,13 don't need to move, these are the vertices at the lower part of the [0,1,2,3] rectangle

        // These vertices are the 4 highest vertices, move these up and down based on the curtain height.
        vertices[0].v.ob[1] = 3730 + (s16)(s32)this->lightRayTopVerticesOffset;
        vertices[3].v.ob[1] = 3730 + (s16)(s32)this->lightRayTopVerticesOffset;
        vertices[4].v.ob[1] = 3730 + (s16)(s32)this->lightRayTopVerticesOffset;
        vertices[7].v.ob[1] = 3730 + (s16)(s32)this->lightRayTopVerticesOffset;

        // Move the ray based on the angle
        vertices[5].v.ob[0] = lightRayBaseX + 675;
        vertices[5].v.ob[2] = lightRayBaseZ + (s16)(s32)this->lightRayBaseOffsetZ - 1800;
        vertices[6].v.ob[0] = lightRayBaseX - 675;
        vertices[6].v.ob[2] = lightRayBaseZ + (s16)(s32)this->lightRayBaseOffsetZ - 1800;
        vertices[9].v.ob[0] = lightRayBaseX + 675;
        vertices[9].v.ob[2] = lightRayBaseZ - 450;
        vertices[11].v.ob[0] = lightRayBaseX - 675;
        vertices[11].v.ob[2] = lightRayBaseZ - 450;
        vertices[12].v.ob[0] = lightRayBaseX + 675;
        vertices[12].v.ob[2] = lightRayBaseZ - 450;

        // Move the rectangle cast on the floor based on the ray angle
        vertices[14].v.ob[0] = lightRayBaseX - 825;
        vertices[14].v.ob[2] = lightRayBaseZ - 1950;
        vertices[15].v.ob[0] = lightRayBaseX - 825;
        vertices[15].v.ob[2] = lightRayBaseZ;
        vertices[16].v.ob[0] = lightRayBaseX + 825;
        vertices[16].v.ob[2] = lightRayBaseZ;
        vertices[17].v.ob[0] = lightRayBaseX + 825;
        vertices[17].v.ob[2] = lightRayBaseZ - 1950;

        if (this->actor.child != NULL) {
            Actor* child = this->actor.child;

            child->world.pos.x = this->actor.world.pos.x + (lightRayBaseX * 0.1f);
            child->world.pos.z = (this->actor.world.pos.z - 170.0f) + (lightRayBaseZ * 0.1f);
        }

        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(&gIkanaThroneRoomLightRayTexAnim));
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y - 234.0f, this->actor.world.pos.z + 30.0f,
                         MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, -1112.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 155, 255, lightRayGreenFactor, (u8)(140.0f * lightRayBlueFactor + 115.0f),
                        lightRayAlpha);
        gSPDisplayList(POLY_XLU_DISP++, gIkanaThroneRoomLightRayDL);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 255, 255, lightRayGreenFactor, (u8)(100.0f * lightRayBlueFactor + 65.0f),
                        floorLightAlpha);
        gSPDisplayList(POLY_XLU_DISP++, gIkanaThroneRoomLightOnFloorDL);

        //! FAKE:
        if (1) {}
    }

    if (this->drawFlags & BOSS06_DRAWFLAG_CURTAIN) {
        // 2S2H [Port][HD Textures] To support CPU modified textures for any arbitrary texture size and alt asset
        // toggling, we need to set the original texture resource as the segment value and register each curtain's mask
        // as a blended texture. The blended tex opcode is necessary since there are two curtains in the scene, but only
        // one mask can be registered to a resource at a time.
        if (true) {
            gSPRegisterBlendedTex(POLY_OPA_DISP++, gIkanaThroneRoomCurtainTex, this->curtainTexMask, NULL);
            gSPInvalidateTexCache(POLY_OPA_DISP++, this->curtainTexMask);
            gSPInvalidateTexCache(POLY_OPA_DISP++, gIkanaThroneRoomCurtainTex);
            gSPSegment(POLY_OPA_DISP++, 0x08, gIkanaThroneRoomCurtainTex);
        } else {
            gSPInvalidateTexCache(POLY_OPA_DISP++, &this->curtainTexture);
            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(&this->curtainTexture));
        }

        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + this->curtainHeight,
                         this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, 0.0f, MTXMODE_APPLY);
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gIkanaThroneRoomCurtainDL);

        if (this->fireEffectScale > 0.0f) {
            Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + 84.0f, this->actor.world.pos.z - 2.0f,
                             MTXMODE_NEW);
            Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
            Boss06_InitRand(1, 29093, 9786);

            gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, 0);
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 128, 255, 255, 0, 230);

            for (i = 0; i < ARRAY_COUNT(sCurtainFireEffectPositions); i++) {
                if ((fabsf(sCurtainFireEffectPositions[i].x - 32.0f) < 30.0f) &&
                    (fabsf(sCurtainFireEffectPositions[i].y - 32.0f) < 30.0f)) {
                    FrameInterpolation_RecordOpenChild(this, i);
                    Matrix_Push();

                    gSPSegment(POLY_XLU_DISP++, 0x08,
                               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                                  ((play->gameplayFrames + (i * 10)) * -20) % 512, 32, 128, 0, 0, 0,
                                                  -20));

                    Matrix_Translate((sCurtainFireEffectPositions[i].x - 32.0f) * -2.4f,
                                     (sCurtainFireEffectPositions[i].y - 32.0f) * -2.4f, 0.0f, MTXMODE_APPLY);
                    Matrix_RotateZF(i * (M_PIf / 64), MTXMODE_APPLY);

                    if (Boss06_RandZeroOne() < 0.5f) {
                        Matrix_RotateYF(M_PIf, MTXMODE_APPLY);
                    }

                    Matrix_Scale(-0.02f / 10.0f, -this->fireEffectScale, 1.0f, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

                    Matrix_Pop();
                    FrameInterpolation_RecordCloseChild();
                }
            }
        } else {
            lightOrbOffsetZ = 7.0f;
        }
    }

    if (this->lightOrbScale > 0.0f) {
        u8 lightOrbAlpha = (this->lightOrbAlphaFactor - 50.0f) + (50.0f * lightRayBlueFactor);

        Matrix_Translate(this->actor.world.pos.x + this->arrowHitPos.x,
                         this->actor.world.pos.y + 84.0f + this->arrowHitPos.y,
                         (this->actor.world.pos.z - 2.0f) + lightOrbOffsetZ, MTXMODE_NEW);

        gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, (u8)(140.0f * lightRayBlueFactor + 115.0f), lightOrbAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 205, (u8)(100.0f * lightRayBlueFactor + 65.0f), 128);

        Matrix_Scale(this->lightOrbScale, this->lightOrbScale, 1.0f, MTXMODE_APPLY);
        Matrix_RotateZS(play->gameplayFrames * 64, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss06_Reset(void) {
    sIgosInstance = NULL;
    sBoss06Seed1 = 0;
    sBoss06Seed2 = 0;
    sBoss06Seed3 = 0;

    Gfx_UnregisterBlendedTexture(gIkanaThroneRoomCurtainTex);
}
