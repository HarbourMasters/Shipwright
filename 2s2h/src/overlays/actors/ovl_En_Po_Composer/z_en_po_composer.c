/*
 * File: z_en_po_composer.c
 * Overlay: ovl_En_Po_Composer
 * Description: Poe Composers Flat and Sharp
 */

#include "z_en_po_composer.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_FREEZE_EXCEPTION | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnPoComposer_Init(Actor* thisx, PlayState* play);
void EnPoComposer_Destroy(Actor* thisx, PlayState* play);
void EnPoComposer_Update(Actor* thisx, PlayState* play);
void EnPoComposer_Draw(Actor* thisx, PlayState* play);

void EnPoComposer_AwaitPlayer(EnPoComposer* this, PlayState* play);
void EnPoComposer_StartCutscene(EnPoComposer* this, PlayState* play);
void EnPoComposer_StartedCutscene(EnPoComposer* this, PlayState* play);
void EnPoComposer_PlayCurse(EnPoComposer* this, PlayState* play);
void EnPoComposer_IdleStill(EnPoComposer* this, PlayState* play);
void EnPoComposer_Appear(EnPoComposer* this, PlayState* play);
void EnPoComposer_Idle(EnPoComposer* this, PlayState* play);
void EnPoComposer_RaiseArms(EnPoComposer* this, PlayState* play);
void EnPoComposer_LowerArms(EnPoComposer* this, PlayState* play);
void EnPoComposer_CutscenePlayCurse(EnPoComposer* this, PlayState* play);
void EnPoComposer_Roll(EnPoComposer* this, PlayState* play);

void EnPoComposer_SetupAwaitPlayer(EnPoComposer* this);
void EnPoComposer_SetupStartCutscene(EnPoComposer* this);
void EnPoComposer_SetupStartedCutscene(EnPoComposer* this);
void EnPoComposer_StepLightAlpha(EnPoComposer* this);

ActorProfile En_Po_Composer_Profile = {
    /**/ ACTOR_EN_PO_COMPOSER,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_PO_COMPOSER,
    /**/ sizeof(EnPoComposer),
    /**/ EnPoComposer_Init,
    /**/ EnPoComposer_Destroy,
    /**/ EnPoComposer_Update,
    /**/ EnPoComposer_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, 20, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { POE_COMPOSER_LIMB_LANTERN, { { 0, -1500, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 4, 25, 50, 40 };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(2, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(2, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x1),
    /* Normal arrow   */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(2, 0x0),
    /* Hookshot       */ DMG_ENTRY(2, 0x1),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(2, 0x0),
    /* Goron pound    */ DMG_ENTRY(4, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x0),
    /* Ice arrow      */ DMG_ENTRY(2, 0x0),
    /* Light arrow    */ DMG_ENTRY(2, 0x0),
    /* Goron spikes   */ DMG_ENTRY(2, 0x0),
    /* Deku spin      */ DMG_ENTRY(2, 0x0),
    /* Deku bubble    */ DMG_ENTRY(2, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(4, 0x0),
    /* Spin attack    */ DMG_ENTRY(2, 0x0),
    /* Sword beam     */ DMG_ENTRY(2, 0x0),
    /* Normal Roll    */ DMG_ENTRY(8, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(4, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(4, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

typedef enum {
    /*  0 */ POE_COMPOSER_ANIM_STILL,
    /*  1 */ POE_COMPOSER_ANIM_APPEAR_DISAPPEAR,
    /*  2 */ POE_COMPOSER_ANIM_IDLE,
    /*  3 */ POE_COMPOSER_ANIM_RAISE_ARMS,
    /*  4 */ POE_COMPOSER_ANIM_ARMS_RAISED,
    /*  5 */ POE_COMPOSER_ANIM_LOWER_ARMS,
    /*  6 */ POE_COMPOSER_ANIM_LOOKING_DOWN,
    /*  7 */ POE_COMPOSER_ANIM_BEGIN_PLAYING_CURSE,
    /*  8 */ POE_COMPOSER_ANIM_PLAYING_CURSE,
    /*  9 */ POE_COMPOSER_ANIM_BEGIN_ROLLING,
    /* 10 */ POE_COMPOSER_ANIM_ROLLING,
    /* 11 */ POE_COMPOSER_ANIM_FLEE,
    /* 12 */ POE_COMPOSER_ANIM_ATTACK,
    /* 13 */ POE_COMPOSER_ANIM_MAX
} EnPoComposerAnim;

static AnimationInfo sAnimationInfo[POE_COMPOSER_ANIM_MAX] = {
    /*  0 */ { &gPoeComposerStillAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /*  1 */ { &gPoeComposerAppearDisappearAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },
    /*  2 */ { &gPoeComposerIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /*  3 */ { &gPoeComposerRaiseArmsAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /*  4 */ { &gPoeComposerArmsRaisedAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /*  5 */ { &gPoeComposerLowerArmsAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /*  6 */ { &gPoeComposerLookDownAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /*  7 */ { &gPoeComposerBeginPlayAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /*  8 */ { &gPoeComposerPlayAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /*  9 */ { &gPoeComposerBeginRollAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /* 10 */ { &gPoeComposerRollAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /* 11 */ { &gPoeComposerFleeAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    /* 12 */ { &gPoeComposerAttackAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },
};

static Color_RGBA8 sLightColorInit = { 255, 255, 210, 0 };

static Color_RGBA8 sEnvColorInit = { 0, 0, 0, 255 };

static Color_RGBA8 sSharpClothingColor1 = { 75, 20, 25, 255 };

static Color_RGBA8 sFlatClothingColor1 = { 80, 110, 90, 255 };

static Color_RGBA8 sSharpClothingColor2 = { 90, 85, 50, 255 };

static Color_RGBA8 sFlatClothingColor2 = { 100, 90, 100, 255 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 3200, ICHAIN_STOP),
};

static s32 sPlayerIsPlayingOcarina = false;

void EnPoComposer_Init(Actor* thisx, PlayState* play) {
    EnPoComposer* this = (EnPoComposer*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gPoeComposerSkel, &gPoeComposerIdleAnim, this->jointTable,
                       this->morphTable, POE_COMPOSER_LIMB_MAX);
    Collider_InitJntSph(play, &this->lanternCollider);
    Collider_SetJntSph(play, &this->lanternCollider, &this->actor, &sJntSphInit, this->lanternColliderElements);
    Collider_InitCylinder(play, &this->bodyCollider);
    Collider_SetCylinder(play, &this->bodyCollider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Lights_PointGlowSetInfo(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z,
                            255, 255, 255, 0);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->lightColor = sLightColorInit;
    this->envColor = sEnvColorInit;
    this->cueId = POE_COMPOSER_CUEID_NONE;

    if (POE_COMPOSER_IS_FLAT(&this->actor)) {
        this->sharpCsNum = 0;
        this->inCutscene = true;
        EnPoComposer_SetupStartedCutscene(this);
    } else if (POE_COMPOSER_4000(&this->actor)) {
        if (gSaveContext.save.entrance != ENTRANCE(IKANA_CANYON, 10)) {
            Actor_Kill(&this->actor);
            return;
        }
        this->sharpCsNum = SHARP_CS_ENCOUNTER_FIRST;
        this->inCutscene = true;
        EnPoComposer_SetupStartedCutscene(this);
    } else {
        s32 i;
        s16 csId = this->actor.csId;

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
            Actor_Kill(&this->actor);
            return;
        }

        for (i = 0; i < ARRAY_COUNT(this->csIds); i++) {
            if (csId == CS_ID_NONE) {
                break;
            }
            this->csIds[i] = csId;
            csId = CutsceneManager_GetAdditionalCsId(csId);
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_02)) {
            this->sharpCsNum = SHARP_CS_ENCOUNTER_OTHER;
        } else {
            this->sharpCsNum = SHARP_CS_ENCOUNTER_FIRST;
        }
        this->inCutscene = false;
        EnPoComposer_SetupAwaitPlayer(this);
    }
}

void EnPoComposer_Destroy(Actor* thisx, PlayState* play) {
    EnPoComposer* this = (EnPoComposer*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    Collider_DestroyJntSph(play, &this->lanternCollider);
    Collider_DestroyCylinder(play, &this->bodyCollider);
}

void EnPoComposer_SetupAwaitPlayer(EnPoComposer* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_STILL);
    this->actionFunc = EnPoComposer_AwaitPlayer;
}

void EnPoComposer_AwaitPlayer(EnPoComposer* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 200.0f) {
        EnPoComposer_SetupStartCutscene(this);
    }
}

void EnPoComposer_SetupStartCutscene(EnPoComposer* this) {
    this->inCutscene = true;
    this->actionFunc = EnPoComposer_StartCutscene;
}

void EnPoComposer_StartCutscene(EnPoComposer* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csIds[this->sharpCsNum])) {
        CutsceneManager_Start(this->csIds[this->sharpCsNum], &this->actor);
        EnPoComposer_SetupStartedCutscene(this);
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(this->csIds[this->sharpCsNum]);
    }
}

void EnPoComposer_SetupStartedCutscene(EnPoComposer* this) {
    this->actionFunc = EnPoComposer_StartedCutscene;
}

// #region 2S2H [Port] There is an issue with MSVC where this function is removed in release mode and the check in
// EnPoComposer_UpdateAction fails. We need to do something in this function that has a side effect which will cause
// MSVC to not remove it. This modification seemed cleaner than `volatile int a = 5`. __declspec(noinlne) did not work.
void EnPoComposer_StartedCutscene(EnPoComposer* this, PlayState* play) {
    sPlayerIsPlayingOcarina = false;
}

void EnPoComposer_SetupPlayCurse(EnPoComposer* this) {
    this->actionTimer = 0;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_PLAYING_CURSE);
    this->actionFunc = EnPoComposer_PlayCurse;
}

void EnPoComposer_PlayCurse(EnPoComposer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // Damage the player every second
    if (this->actionTimer == 20) {
        func_800B8D10(play, &this->actor, 0.0f, 0, 0.0f, 1, 4);
        this->actionTimer = 0;
    } else {
        this->actionTimer++;
    }

    // Ocarina check
    if (player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) {
        if (!sPlayerIsPlayingOcarina) {
            // Play sound whenever the player begins playing the Ocarina
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        }
        sPlayerIsPlayingOcarina = true;
    } else {
        sPlayerIsPlayingOcarina = false;
    }

    // Song check, check if player just played song of storms or song of healing
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) {
        switch (play->msgCtx.lastPlayedSong) {
            case OCARINA_SONG_STORMS:
                this->sharpCsNum = SHARP_CS_SONG_STORMS;
                EnPoComposer_SetupStartCutscene(this);
                break;

            case OCARINA_SONG_HEALING:
                this->sharpCsNum = SHARP_CS_SONG_HEALING;
                EnPoComposer_SetupStartCutscene(this);
                break;

            default:
                break;
        }
    }

    // Play sharp's curse bgm
    Audio_PlaySequenceAtPos(SEQ_PLAYER_BGM_SUB, &this->actor.projectedPos, NA_BGM_SHARPS_CURSE, 2000.0f);
}

void EnPoComposer_SetupIdleStill(EnPoComposer* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_STILL);
    this->actionFunc = EnPoComposer_IdleStill;
}

void EnPoComposer_IdleStill(EnPoComposer* this, PlayState* play) {
}

void EnPoComposer_SetupAppear(EnPoComposer* this) {
    this->actionTimer = 0;
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_APPEAR);
    Actor_PlaySfx(&this->actor, NA_SE_EN_PO_LAUGH);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_APPEAR_DISAPPEAR);
    this->actionFunc = EnPoComposer_Appear;
}

void EnPoComposer_Appear(EnPoComposer* this, PlayState* play) {
    this->actionTimer++;
    this->actor.shape.rot.y += 0x1000;
    if (this->actionTimer == 32) {
        EnPoComposer_SetupIdleStill(this);
    }
}

void EnPoComposer_SetupIdle(EnPoComposer* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_IDLE);
    this->actionFunc = EnPoComposer_Idle;
}

void EnPoComposer_Idle(EnPoComposer* this, PlayState* play) {
}

void EnPoComposer_SetupRaiseArms(EnPoComposer* this) {
    this->csCueTimer = 0;
    Actor_PlaySfx(&this->actor, NA_SE_EN_SHARP_REACTION);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_RAISE_ARMS);
    this->actionFunc = EnPoComposer_RaiseArms;
}

void EnPoComposer_RaiseArms(EnPoComposer* this, PlayState* play) {
    if ((this->csCueTimer == 0) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->csCueTimer++;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_ARMS_RAISED);
    }
}

void EnPoComposer_SetupLowerArms(EnPoComposer* this) {
    this->csCueTimer = 0;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_LOWER_ARMS);
    this->actionFunc = EnPoComposer_LowerArms;
}

void EnPoComposer_LowerArms(EnPoComposer* this, PlayState* play) {
    if ((this->csCueTimer == 0) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->csCueTimer++;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_LOOKING_DOWN);
    }
}

void EnPoComposer_SetupCutscenePlayCurse(EnPoComposer* this) {
    this->csCueTimer = 0;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_BEGIN_PLAYING_CURSE);
    this->actionFunc = EnPoComposer_CutscenePlayCurse;
}

void EnPoComposer_CutscenePlayCurse(EnPoComposer* this, PlayState* play) {
    if ((this->csCueTimer == 0) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->csCueTimer++;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_PLAYING_CURSE);
    }
}

void EnPoComposer_SetupRoll(EnPoComposer* this) {
    this->csCueTimer = 0;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_BEGIN_ROLLING);
    this->actionFunc = EnPoComposer_Roll;
}

void EnPoComposer_Roll(EnPoComposer* this, PlayState* play) {
    if (this->csCueTimer == 0) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->csCueTimer++;
            Actor_PlaySfx(&this->actor, NA_SE_EN_SHARP_REACTION);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, POE_COMPOSER_ANIM_ROLLING);
        }
    } else if (Animation_OnFrame(&this->skelAnime, 1.0f) && (this->lightColor.a > 32)) {
        if (this->csCueTimer == 1) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_DEAD_WIND2_OLD);
            this->csCueTimer = 2;
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_LAST3_DEAD_WIND3_OLD);
            this->csCueTimer = 1;
        }
    }
}

void EnPoComposer_SharpCutsceneDone(EnPoComposer* this) {
    SET_WEEKEVENTREG(WEEKEVENTREG_14_04);
    Actor_Kill(&this->actor);
}

s32 EnPoComposer_UpdateAction(EnPoComposer* this, PlayState* play) {
    u16 cueType = POE_COMPOSER_IS_FLAT(&this->actor) ? CS_CMD_ACTOR_CUE_482 : CS_CMD_ACTOR_CUE_481;
    s32 cueChannel;

    if (!this->inCutscene) {
        return false;
    }

    if (Cutscene_IsCueInChannel(play, cueType)) {
        cueChannel = Cutscene_GetCueChannel(play, cueType);

        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;

            switch (play->csCtx.actorCues[cueChannel]->id) {
                case POE_COMPOSER_CUEID_IDLE: // loop idle
                    EnPoComposer_SetupIdleStill(this);
                    break;

                case POE_COMPOSER_CUEID_APPEAR_SPIN: // appear/disappear -> loop idle
                    EnPoComposer_SetupAppear(this);
                    break;

                case POE_COMPOSER_CUEID_IDLE2: // loop sharp idle
                    EnPoComposer_SetupIdle(this);
                    break;

                case POE_COMPOSER_CUEID_RAISE_ARMS: // raise arms -> loop arms raised
                    EnPoComposer_SetupRaiseArms(this);
                    break;

                case POE_COMPOSER_CUEID_LOWER_ARMS: // lower arms -> loop idle
                    EnPoComposer_SetupLowerArms(this);
                    break;

                case POE_COMPOSER_CUEID_PLAY_CURSE: // begin playing -> loop playing
                    EnPoComposer_SetupCutscenePlayCurse(this);
                    break;

                case POE_COMPOSER_CUEID_ROLL: // begin rolling -> loop rolling
                    EnPoComposer_SetupRoll(this);
                    break;

                case POE_COMPOSER_CUEID_APPEAR: // appear
                    this->visible = true;
                    break;

                case POE_COMPOSER_CUEID_DISAPPEAR: // disappear
                    this->visible = false;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_PO_DISAPPEAR);
                    break;

                case POE_COMPOSER_CUEID_DONE:
                    if (cueType == CS_CMD_ACTOR_CUE_481) {
                        EnPoComposer_SharpCutsceneDone(this);
                    } else {
                        Actor_Kill(&this->actor);
                    }
                    break;

                default:
                    break;
            }
        }

        if (this->actionFunc != EnPoComposer_Appear) {
            Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        } else {
            Cutscene_ActorTranslate(&this->actor, play, cueChannel);
        }

        EnPoComposer_StepLightAlpha(this);

        if ((this->sharpCsNum == SHARP_CS_SONG_STORMS) && (play->csCtx.curFrame == 204)) {
            Audio_PlaySfx(NA_SE_SY_DIZZY_EFFECT);
        }

        if (this->visible == true) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_SHARP_FLOAT - SFX_FLAG);
        }
        return true;
    }

    if ((this->actionFunc != EnPoComposer_StartedCutscene) && (this->actionFunc != EnPoComposer_StartCutscene) &&
        (this->actionFunc != EnPoComposer_PlayCurse) && (play->csCtx.state == CS_STATE_IDLE)) {

        if (POE_COMPOSER_IS_FLAT(&this->actor)) {
            EnPoComposer_SetupStartedCutscene(this);
        } else if (POE_COMPOSER_4000(&this->actor)) {
            EnPoComposer_SetupStartedCutscene(this);
        } else {
            this->inCutscene = false;

            if (this->sharpCsNum < SHARP_CS_SONG_STORMS) {
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_14_02)) {
                    SET_WEEKEVENTREG(WEEKEVENTREG_14_02);
                }
                EnPoComposer_SetupPlayCurse(this);
            } else {
                EnPoComposer_SetupStartedCutscene(this);
            }
        }
    }

    this->cueId = POE_COMPOSER_CUEID_NONE;
    return false;
}

void EnPoComposer_StepLightAlpha(EnPoComposer* this) {
    if ((this->visible == true) && (this->lightColor.a != 255)) {
        if (this->lightColor.a > 247) {
            this->lightColor.a = 255;
        } else {
            this->lightColor.a += 7;
        }
    } else if (!this->visible && (this->lightColor.a != 0)) {
        if (this->lightColor.a < 8) {
            this->lightColor.a = 0;
        } else {
            this->lightColor.a -= 7;
        }
    }
}

void EnPoComposer_StepLightColor(EnPoComposer* this) {
    s16 color;

    color = this->lightColor.r + 5;
    this->lightColor.r = CLAMP_MAX(color, 255);

    color = this->lightColor.g + 5;
    this->lightColor.g = CLAMP_MAX(color, 255);

    if (this->lightColor.b > 210) {
        color = this->lightColor.b - 5;
        this->lightColor.b = CLAMP_MIN(color, 210);
    } else {
        color = this->lightColor.b + 5;
        this->lightColor.b = CLAMP_MAX(color, 210);
    }
}

void EnPoComposer_UpdateEnvColor(EnPoComposer* this) {
    if ((this->actionFunc == EnPoComposer_Appear) && (this->skelAnime.curFrame < 12.0f)) {
        // Step rgb from 55 to 255 and alpha from 0 to 200 over the first 12 frames of the appear anim
        this->envColor.r = this->envColor.g = this->envColor.b = 55.0f + this->skelAnime.curFrame * 16.66f;
        this->envColor.a = this->skelAnime.curFrame * (200.0f / 12.0f);
    } else {
        f32 rand = Rand_ZeroOne();

        this->envColor.r = 225 + (s32)(rand * 30.0f);
        this->envColor.g = 155 + (s32)(rand * 100.0f);
        this->envColor.b = 95 + (s32)(rand * 160.0f);
        this->envColor.a = 200;
    }
    if (this->lightColor.a == 0) {
        this->envColor.a = 0;
    }
}

void EnPoComposer_UpdateCollision(EnPoComposer* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->bodyCollider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->lanternCollider.base);
}

void EnPoComposer_Update(Actor* thisx, PlayState* play) {
    EnPoComposer* this = (EnPoComposer*)thisx;

    EnPoComposer_UpdateEnvColor(this);
    SkelAnime_Update(&this->skelAnime);
    Actor_SetFocus(&this->actor, 42.0f);

    this->actionFunc(this, play);

    if (!EnPoComposer_UpdateAction(this, play)) {
        EnPoComposer_UpdateCollision(this, play);
    }
}

s32 EnPoComposer_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                                  Gfx** gfx) {
    EnPoComposer* this = (EnPoComposer*)thisx;

    if ((this->lightColor.a == 0) || (limbIndex == POE_COMPOSER_LIMB_LANTERN)) {
        *dList = NULL;
    } else if (POE_COMPOSER_IS_FLAT(&this->actor) && (limbIndex == POE_COMPOSER_LIMB_HEAD)) {
        *dList = gPoeComposerFlatHeadDL;
    }

    if (limbIndex == 19) {
        //! @bug skeleton does not have a limb 19, this is leftover from OoT Poes
        gDPPipeSync((*gfx)++);
        gDPSetEnvColor((*gfx)++, this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
    }

    return false;
}

void EnPoComposer_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnPoComposer* this = (EnPoComposer*)thisx;

    Collider_UpdateSpheres(limbIndex, &this->lanternCollider);
    if (limbIndex == POE_COMPOSER_LIMB_LANTERN) {
        Matrix_Get(&this->lanternMtxF);
    }
}

void EnPoComposer_Draw(Actor* thisx, PlayState* play) {
    EnPoComposer* this = (EnPoComposer*)thisx;
    s32 pad;
    Gfx* dl;
    Color_RGBA8* clothingColor;
    Color_RGBA8* var_t0;
    Vec3f lightOffset;
    Vec3s lightPos;

    OPEN_DISPS(play->state.gfxCtx);

    EnPoComposer_StepLightColor(this);

    // Select colors

    if (POE_COMPOSER_IS_FLAT(&this->actor)) {
        clothingColor = &sFlatClothingColor1;
        var_t0 = &sFlatClothingColor2;
    } else {
        clothingColor = &sSharpClothingColor1;
        var_t0 = &sSharpClothingColor2;
    }

    // Draw skeleton

    if ((this->lightColor.a == 255) || (this->lightColor.a == 0)) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08,
                   Gfx_EnvColor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                this->lightColor.a));
        gSPSegment(
            POLY_OPA_DISP++, 0x0A,
            Gfx_EnvColor(play->state.gfxCtx, clothingColor->r, clothingColor->g, clothingColor->b, this->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0B,
                   Gfx_EnvColor(play->state.gfxCtx, var_t0->r, var_t0->g, var_t0->b, this->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0C, D_801AEFA0);

        POLY_OPA_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnPoComposer_OverrideLimbDraw, EnPoComposer_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_EnvColor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                this->lightColor.a));
        gSPSegment(
            POLY_XLU_DISP++, 0x0A,
            Gfx_EnvColor(play->state.gfxCtx, clothingColor->r, clothingColor->g, clothingColor->b, this->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0B,
                   Gfx_EnvColor(play->state.gfxCtx, var_t0->r, var_t0->g, var_t0->b, this->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0C, D_801AEF88);

        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnPoComposer_OverrideLimbDraw, EnPoComposer_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }

    if (this->lightColor.a != 0) {
        // Draw lantern if visible

        if (this->lightColor.a == 255) {
            Scene_SetRenderModeXlu(play, 0, 1);
            dl = POLY_OPA_DISP;
        } else {
            Scene_SetRenderModeXlu(play, 1, 2);
            dl = POLY_XLU_DISP;
        }

        gDPPipeSync(&dl[0]);
        gDPSetEnvColor(&dl[1], this->envColor.r, this->envColor.g, this->envColor.b, this->lightColor.a);

        Matrix_Put(&this->lanternMtxF);
        MATRIX_FINALIZE_AND_LOAD(&dl[2], play->state.gfxCtx);

        gSPDisplayList(&dl[3], gPoeComposerLanternBaseDL);
        gSPDisplayList(&dl[4], gPoeComposerLanternGlassDL);

        gDPPipeSync(&dl[5]);
        gDPSetEnvColor(&dl[6], clothingColor->r, clothingColor->g, clothingColor->b, this->lightColor.a);
        gSPDisplayList(&dl[7], gPoeComposerLanternTopDL);

        if (this->lightColor.a == 255) {
            POLY_OPA_DISP = &dl[8];
        } else {
            POLY_XLU_DISP = &dl[8];
        }
    }

    Matrix_Put(&play->billboardMtxF);
    Matrix_MultVecZ(15.0f, &lightOffset);
    lightPos = this->lanternCollider.elements[0].dim.worldSphere.center;
    lightPos.x += TRUNCF_BINANG(lightOffset.x);
    lightPos.y += TRUNCF_BINANG(lightOffset.y);
    lightPos.z += TRUNCF_BINANG(lightOffset.z);

    Lights_PointGlowSetInfo(&this->lightInfo, lightPos.x, lightPos.y, lightPos.z, this->envColor.r, this->envColor.g,
                            this->envColor.b, this->envColor.a * (200.0f / 255.0f));

    CLOSE_DISPS(play->state.gfxCtx);
}
