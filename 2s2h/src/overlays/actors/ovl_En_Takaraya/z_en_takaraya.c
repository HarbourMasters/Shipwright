/*
 * File: z_en_takaraya.c
 * Overlay: ovl_En_Takaraya
 * Description: Treasure Chest Shop Gal
 */

#include "z_en_takaraya.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnTakaraya_Init(Actor* thisx, PlayState* play);
void EnTakaraya_Destroy(Actor* thisx, PlayState* play);
void EnTakaraya_Update(Actor* thisx, PlayState* play);
void EnTakaraya_Draw(Actor* thisx, PlayState* play);

void EnTakaraya_SetupWait(EnTakaraya* this);
void EnTakaraya_Wait(EnTakaraya* this, PlayState* play);
void EnTakaraya_SetupTalk(EnTakaraya* this);
void EnTakaraya_Talk(EnTakaraya* this, PlayState* play);
void func_80ADF2D4(EnTakaraya* this);
void func_80ADF338(EnTakaraya* this, PlayState* play);
void func_80ADF4E0(EnTakaraya* this);
void func_80ADF520(EnTakaraya* this, PlayState* play);
void func_80ADF608(EnTakaraya* this, PlayState* play);
void func_80ADF654(EnTakaraya* this, PlayState* play);
void func_80ADF6DC(EnTakaraya* this);
void func_80ADF730(EnTakaraya* this, PlayState* play);
void func_80ADF7B8(EnTakaraya* this);
void func_80ADF7CC(EnTakaraya* this, PlayState* play);

ActorProfile En_Takaraya_Profile = {
    /**/ ACTOR_EN_TAKARAYA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BG,
    /**/ sizeof(EnTakaraya),
    /**/ EnTakaraya_Init,
    /**/ EnTakaraya_Destroy,
    /**/ EnTakaraya_Update,
    /**/ EnTakaraya_Draw,
};

TexturePtr sEyesUpTextures[] = {
    gTreasureChestShopGalEyeOpenUpTex,
    gTreasureChestShopGalEyeHalfUpTex,
    gTreasureChestShopGalEyeClosedTex,
    gTreasureChestShopGalEyeHalfUpTex,
};

TexturePtr sEyesDownTextures[] = {
    gTreasureChestShopGalEyeOpenDownTex,
    gTreasureChestShopGalEyeHalfDownTex,
    gTreasureChestShopGalEyeClosedTex,
    gTreasureChestShopGalEyeHalfDownTex,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_6, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 1000, ICHAIN_STOP),
};

static s32 sTexturesDesegmented = false;

u16 D_80ADFB2C[PLAYER_FORM_MAX] = {
    0x76D, // PLAYER_FORM_FIERCE_DEITY
    0x76E, // PLAYER_FORM_GORON
    0x76F, // PLAYER_FORM_ZORA
    0x76C, // PLAYER_FORM_DEKU
    0x76D, // PLAYER_FORM_HUMAN
};

u8 D_80ADFB38[PLAYER_FORM_MAX][2] = {
    { GI_RUPEE_PURPLE, GI_RUPEE_PURPLE }, // PLAYER_FORM_FIERCE_DEITY
    { GI_HEART_PIECE, GI_RUPEE_PURPLE },  // PLAYER_FORM_GORON
    { GI_RUPEE_RED, GI_RUPEE_RED },       // PLAYER_FORM_ZORA
    { GI_DEKU_NUTS_10, GI_DEKU_NUTS_5 },  // PLAYER_FORM_DEKU
    { GI_RUPEE_PURPLE, GI_RUPEE_PURPLE }, // PLAYER_FORM_HUMAN
};

u16 D_80ADFB44[PLAYER_FORM_MAX] = {
    0x775, // PLAYER_FORM_FIERCE_DEITY
    0x776, // PLAYER_FORM_GORON
    0x777, // PLAYER_FORM_ZORA
    0x774, // PLAYER_FORM_DEKU
    0x775, // PLAYER_FORM_HUMAN
};

u16 D_80ADFB50[PLAYER_FORM_MAX] = {
    0x771, // PLAYER_FORM_FIERCE_DEITY
    0x772, // PLAYER_FORM_GORON
    0x773, // PLAYER_FORM_ZORA
    0x770, // PLAYER_FORM_DEKU
    0x771, // PLAYER_FORM_HUMAN
};

void EnTakaraya_Init(Actor* thisx, PlayState* play) {
    EnTakaraya* this = (EnTakaraya*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, -60.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gTreasureChestShopGalSkel, &object_bg_Anim_009890, this->jointTable,
                       this->morphTable, TREASURE_CHEST_SHOP_GAL_LIMB_MAX);
    this->switchFlag = EN_TAKARAYA_GET_SWITCH_FLAG(thisx);
    thisx->params &= 0xFF;

    if (!sTexturesDesegmented) {
        for (i = 0; i < ARRAY_COUNT(sEyesDownTextures); i++) {
            sEyesUpTextures[i] = Lib_SegmentedToVirtual(sEyesUpTextures[i]);
            sEyesDownTextures[i] = Lib_SegmentedToVirtual(sEyesDownTextures[i]);
        }
        sTexturesDesegmented = true;
    }

    this->eyeTexIndex = 2;
    if (gSaveContext.save.entrance == ENTRANCE(TREASURE_CHEST_SHOP, 1)) {
        Audio_StopSubBgm();
        if (gSaveContext.timerStates[TIMER_ID_MINIGAME_2] == TIMER_STATE_6) {
            this->actor.textId = 0x77A;
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
            func_80ADF6DC(this);
        } else {
            this->actor.textId = 0x77C;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                EnTakaraya_SetupWait(this);
            } else {
                func_80ADF6DC(this);
            }
        }
    } else {
        EnTakaraya_SetupWait(this);
    }
}

void EnTakaraya_Destroy(Actor* thisx, PlayState* play) {
    EnTakaraya* this = (EnTakaraya*)thisx;

    Flags_UnsetSwitch(play, 5);
    if (!this->unk2AD) {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
        gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
    }
}

void EnTakaraya_Blink(EnTakaraya* this) {
    if (this->eyeTexIndex != 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex == 4) {
            this->eyeTexIndex = 0;
        }
    } else if (Rand_ZeroOne() < 0.02f) {
        this->eyeTexIndex++;
    }
}

void EnTakaraya_SetupWait(EnTakaraya* this) {
    if (this->skelAnime.animation == &object_bg_Anim_001384) {
        Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00AD98, 5.0f);
    }
    this->actionFunc = EnTakaraya_Wait;
}

void EnTakaraya_Wait(EnTakaraya* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->skelAnime.animation == &object_bg_Anim_00A280) {
            Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00AD98, 5.0f);
        } else {
            Animation_MorphToLoop(&this->skelAnime, &object_bg_Anim_009890, -4.0f);
        }
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (Text_GetFaceReaction(play, FACE_REACTION_SET_TREASURE_CHEST_SHOP_GAL) == 0) {
            Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00A280, -4.0f);
        }
        EnTakaraya_SetupTalk(this);
    } else if (Actor_IsFacingPlayer(&this->actor, 0x2000)) {
        this->actor.textId = Text_GetFaceReaction(play, FACE_REACTION_SET_TREASURE_CHEST_SHOP_GAL);
        if (this->actor.textId == 0) {
            this->actor.textId = D_80ADFB2C[GET_PLAYER_FORM];
        }
        this->formSwitchFlag = GET_PLAYER_FORM + this->switchFlag;
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void EnTakaraya_SpawnWalls(EnTakaraya* this, PlayState* play) {
    u8 getItemId;

    if (Flags_GetSwitch(play, this->formSwitchFlag)) {
        getItemId = D_80ADFB38[GET_PLAYER_FORM][1];
    } else {
        getItemId = D_80ADFB38[GET_PLAYER_FORM][0];
    }
    Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_OBJ_TAKARAYA_WALL, 0.0f, 0.0f, 0.0f, 0, 0, 5,
                                  (ENBOX_TYPE_BIG_SWITCH_FLAG << 12) + (getItemId << 5) + this->actor.params,
                                  this->actor.csId, HALFDAYBIT_ALL, NULL);
}

void EnTakaraya_SetupTalk(EnTakaraya* this) {
    this->actionFunc = EnTakaraya_Talk;
}

void EnTakaraya_Talk(EnTakaraya* this, PlayState* play) {
    u8 talkState;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->skelAnime.animation == &object_bg_Anim_00AD98) {
            Animation_PlayOnce(&this->skelAnime, &object_bg_Anim_000968);
        } else if (this->skelAnime.animation == &object_bg_Anim_00A280) {
            Animation_PlayLoop(&this->skelAnime, &object_bg_Anim_001384);
        } else {
            Animation_PlayLoop(&this->skelAnime, &object_bg_Anim_009890);
        }
    }

    talkState = Message_GetState(&play->msgCtx);

    if ((talkState == TEXT_STATE_CLOSING) || (talkState == TEXT_STATE_DONE)) {
        if (this->actor.textId == 0x778) {
            func_80ADF2D4(this);
        } else {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
            EnTakaraya_SetupWait(this);
        }
    } else if ((talkState == TEXT_STATE_NEXT) && (this->actor.textId != 0x778)) {
        if (Message_ShouldAdvance(play)) {
            Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00AD98, 5.0f);
        }
    } else if ((talkState == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex == 0) {
            if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
                this->actor.textId = 0x77B;
                if (this->skelAnime.animation == &object_bg_Anim_009890) {
                    Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_000968, 5.0f);
                }
                Audio_PlaySfx(NA_SE_SY_ERROR);
            } else {
                Audio_PlaySfx_MessageDecide();
                Rupees_ChangeBy(-play->msgCtx.unk1206C);
                EnTakaraya_SpawnWalls(this, play);
                this->actor.textId = 0x778;
                if (this->skelAnime.animation != &object_bg_Anim_009890) {
                    Animation_MorphToLoop(&this->skelAnime, &object_bg_Anim_009890, 5.0f);
                }
            }
        } else {
            Audio_PlaySfx_MessageCancel();
            this->actor.textId = D_80ADFB44[GET_PLAYER_FORM];
            if (this->skelAnime.animation == &object_bg_Anim_009890) {
                Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_000968, 5.0f);
            }
        }
        Message_ContinueTextbox(play, this->actor.textId);
    }
}

void func_80ADF2D4(EnTakaraya* this) {
    Actor_PlaySfx_FlaggedCentered1(&this->actor, NA_SE_SY_FOUND);
    Audio_PlaySubBgm(NA_BGM_TIMED_MINI_GAME);
    this->timer = 145;
    SET_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
    this->actionFunc = func_80ADF338;
}

void func_80ADF338(EnTakaraya* this, PlayState* play) {
    Actor* chest = play->actorCtx.actorLists[ACTORCAT_CHEST].first;
    Player* player = GET_PLAYER(play);
    Vec3f subCamEye;
    Vec3f subCamAt;
    f32 sp2C;
    s16 subCamId;

    SkelAnime_Update(&this->skelAnime);
    if ((CutsceneManager_GetCurrentCsId() == this->actor.csId) && (chest != NULL)) {
        this->timer--;
        subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
        if (this->timer >= 26) {
            sp2C = 250.0f;
        } else {
            sp2C = ((chest->xzDistToPlayer - 250.0f) * (25 - this->timer) * 0.04f) + 250.0f;
        }
        subCamEye.x = chest->world.pos.x + (Math_SinS(chest->yawTowardsPlayer) * sp2C);
        subCamEye.y = player->actor.world.pos.y + 120.0f;
        subCamEye.z = chest->world.pos.z + (Math_CosS(chest->yawTowardsPlayer) * sp2C);
        subCamAt.x = subCamEye.x - (Math_SinS(chest->yawTowardsPlayer) * 250.0f);
        subCamAt.y = subCamEye.y - 90.0f;
        subCamAt.z = subCamEye.z - (Math_CosS(chest->yawTowardsPlayer) * 250.0f);
        Play_SetCameraAtEye(play, subCamId, &subCamAt, &subCamEye);
    } else if (this->timer < 145) {
        func_80ADF4E0(this);
    }
}

void func_80ADF4E0(EnTakaraya* this) {
    Interface_StartTimer(TIMER_ID_MINIGAME_2, 45);
    this->timer = 1;
    this->actionFunc = func_80ADF520;
}

void func_80ADF520(EnTakaraya* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (!Play_InCsMode(play)) {
        if (Flags_GetTreasure(play, this->actor.params)) {
            Flags_SetSwitch(play, this->formSwitchFlag);
            play->actorCtx.sceneFlags.chest &= ~TAKARAYA_GET_TREASURE_FLAG(&this->actor);
            this->timer = 0;
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_6;
            func_80ADF608(this, play);
        } else if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] == TIMER_STATE_OFF) {
            this->timer = 50;
            Message_StartTextbox(play, 0x77D, &this->actor);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
            func_80ADF608(this, play);
        }
    }
}

void func_80ADF608(EnTakaraya* this, PlayState* play) {
    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
    this->unk2AD = true;
    this->actionFunc = func_80ADF654;
}

void func_80ADF654(EnTakaraya* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        if (gSaveContext.timerStates[TIMER_ID_MINIGAME_2] == TIMER_STATE_6) {
            play->transitionType = TRANS_TYPE_80;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        } else {
            play->transitionType = TRANS_TYPE_64;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
        }
        gSaveContext.nextCutsceneIndex = 0;
        play->nextEntrance = ENTRANCE(TREASURE_CHEST_SHOP, 1);
        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

void func_80ADF6DC(EnTakaraya* this) {
    Animation_PlayLoop(&this->skelAnime, &object_bg_Anim_001384);
    this->eyeTexIndex = 0;
    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    this->actionFunc = func_80ADF730;
}

void func_80ADF730(EnTakaraya* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        func_80ADF7B8(this);
    } else {
        this->formSwitchFlag = GET_PLAYER_FORM + this->switchFlag;
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void func_80ADF7B8(EnTakaraya* this) {
    this->actionFunc = func_80ADF7CC;
}

void func_80ADF7CC(EnTakaraya* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->actor.textId == 0x77A) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
                Message_CloseTextbox(play);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                EnTakaraya_SetupWait(this);
            } else {
                this->actor.textId = 0x77C;
                Message_ContinueTextbox(play, this->actor.textId);
            }
        } else {
            this->actor.textId = D_80ADFB50[GET_PLAYER_FORM];
            Message_ContinueTextbox(play, this->actor.textId);
            Animation_MorphToPlayOnce(&this->skelAnime, &object_bg_Anim_00AD98, 5.0f);
            EnTakaraya_SetupTalk(this);
        }
    }
}

void EnTakaraya_Update(Actor* thisx, PlayState* play) {
    EnTakaraya* this = (EnTakaraya*)thisx;
    s32 pad;
    Vec3s torsoRot;

    this->actionFunc(this, play);
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &torsoRot, this->actor.focus.pos);
    EnTakaraya_Blink(this);
}

s32 EnTakaraya_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnTakaraya* this = (EnTakaraya*)thisx;

    if (limbIndex == TREASURE_CHEST_SHOP_GAL_LIMB_HEAD) {
        rot->x += this->headRot.y;
    }
    return false;
}

void EnTakaraya_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnTakaraya* this = (EnTakaraya*)thisx;

    if (limbIndex == TREASURE_CHEST_SHOP_GAL_LIMB_HEAD) {
        Matrix_RotateYS(0x400 - this->headRot.x, MTXMODE_APPLY);
        Matrix_MultVecX(500.0f, &thisx->focus.pos);
    }
}

void EnTakaraya_Draw(Actor* thisx, PlayState* play) {
    EnTakaraya* this = (EnTakaraya*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if ((GET_PLAYER_FORM == PLAYER_FORM_DEKU) || (GET_PLAYER_FORM == PLAYER_FORM_HUMAN)) {
        gSPSegment(POLY_OPA_DISP++, 0x08, sEyesDownTextures[this->eyeTexIndex]);
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, sEyesUpTextures[this->eyeTexIndex]);
    }
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnTakaraya_OverrideLimbDraw, NULL,
                                   EnTakaraya_TransformLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
