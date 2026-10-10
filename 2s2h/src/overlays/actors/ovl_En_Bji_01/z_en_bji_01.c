/*
 * File: z_en_bji_01.c
 * Overlay: ovl_En_Bji_01
 * Description: Professor Shikashi (Astral Observatory Proprietor)
 */

#include "z_en_bji_01.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnBji01_Init(Actor* thisx, PlayState* play);
void EnBji01_Destroy(Actor* thisx, PlayState* play);
void EnBji01_Update(Actor* thisx, PlayState* play);
void EnBji01_Draw(Actor* thisx, PlayState* play);

void func_809CCEE8(EnBji01* this, PlayState* play);
void func_809CD028(EnBji01* this, PlayState* play);
void EnBji01_DialogueHandler(EnBji01* this, PlayState* play);
void func_809CD634(EnBji01* this, PlayState* play);
void EnBji01_DoNothing(EnBji01* this, PlayState* play);
void func_809CD6C0(EnBji01* this, PlayState* play);
void func_809CD70C(EnBji01* this, PlayState* play);
void func_809CD77C(EnBji01* this, PlayState* play);

ActorProfile En_Bji_01_Profile = {
    /**/ ACTOR_EN_BJI_01,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BJI,
    /**/ sizeof(EnBji01),
    /**/ EnBji01_Init,
    /**/ EnBji01_Destroy,
    /**/ EnBji01_Update,
    /**/ EnBji01_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 64, 0, { 0, 0, 0 } },
};

typedef enum {
    /* -1 */ SHIKASHI_ANIM_NONE = -1,
    /*  0 */ SHIKASHI_ANIM_LOOK_THROUGH_TELESCOPE,
    /*  1 */ SHIKASHI_ANIM_BREATHE,
    /*  2 */ SHIKASHI_ANIM_TALK,
    /*  3 */ SHIKASHI_ANIM_SCRATCH_CHIN,
    /*  4 */ SHIKASHI_ANIM_MAX
} ShikashiAnimation;

static AnimationSpeedInfo sAnimationSpeedInfo[SHIKASHI_ANIM_MAX] = {
    { &object_bji_Anim_000FDC, 1.0f, ANIMMODE_LOOP, 0.0f },  // SHIKASHI_ANIM_LOOK_THROUGH_TELESCOPE
    { &object_bji_Anim_005B58, 1.0f, ANIMMODE_LOOP, 10.0f }, // SHIKASHI_ANIM_BREATHE
    { &object_bji_Anim_000AB0, 1.0f, ANIMMODE_LOOP, 0.0f },  // SHIKASHI_ANIM_TALK
    { &object_bji_Anim_00066C, 1.0f, ANIMMODE_ONCE, -5.0f }, // SHIKASHI_ANIM_SCRATCH_CHIN
};

void func_809CCDE0(EnBji01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f pitchTarget;
    s32 pad[2];

    Math_Vec3f_Copy(&pitchTarget, &player->actor.world.pos);
    pitchTarget.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    SubS_TrackPointStep(&this->actor.world.pos, &this->actor.focus.pos, this->actor.shape.rot.y,
                        &player->actor.world.pos, &pitchTarget, &this->headZRotStep, &this->headXRotStep,
                        &this->torsoZRotStep, &this->torsoXRotStep, 0x1554, 0x1FFE, 0xE38, 0x1C70);
}

void func_809CCE98(EnBji01* this, PlayState* play) {
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SHIKASHI_ANIM_LOOK_THROUGH_TELESCOPE,
                                    &this->animIndex);
    this->actor.textId = 0;
    this->actionFunc = func_809CCEE8;
}

void func_809CCEE8(EnBji01* this, PlayState* play) {
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x444);
    if (this->actor.params == SHIKASHI_TYPE_DEFAULT) {
        if ((this->actor.xzDistToPlayer <= 60.0f) && (this->actor.playerHeightRel <= 10.0f)) {
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        } else {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        }
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        play->msgCtx.msgMode = MSGMODE_NONE;
        play->msgCtx.msgLength = 0;
        func_809CD028(this, play);
    } else {
        if (this->moonsTear != NULL) {
            if (this->moonsTear->actor.colChkInfo.health == 1) {
                func_809CD6C0(this, play);
                return;
            }
        } else {
            this->moonsTear = (ObjMoonStone*)SubS_FindActor(play, NULL, ACTORCAT_PROP, ACTOR_OBJ_MOON_STONE);
        }
        Actor_OfferTalkExchange(&this->actor, play, 60.0f, 10.0f, PLAYER_IA_NONE);
    }
}

void func_809CD028(EnBji01* this, PlayState* play) {
    f32 timeUntilMoonCrash;

    switch (this->actor.params) {
        case SHIKASHI_TYPE_DEFAULT:
        case SHIKASHI_TYPE_FINISHED_CONVERSATION:
            switch (GET_PLAYER_FORM) {
                case PLAYER_FORM_DEKU:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_17_10)) {
                        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_74_80)) {
                            this->textId = 0x5F4;
                        } else {
                            this->textId = 0x5E2;
                        }
                    } else {
                        this->textId = 0x5EC;
                        SET_WEEKEVENTREG(WEEKEVENTREG_17_10);
                    }
                    break;

                case PLAYER_FORM_HUMAN:
                    if (Player_GetMask(play) == PLAYER_MASK_KAFEIS_MASK) {
                        this->textId = 0x236A;
                    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_74_10)) {
                        this->textId = 0x5F6;
                    } else {
                        this->textId = 0x5F5;
                        SET_WEEKEVENTREG(WEEKEVENTREG_74_10);
                    }
                    break;

                case PLAYER_FORM_GORON:
                case PLAYER_FORM_ZORA:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_75_08)) {
                        this->textId = 0x5E4;
                    } else {
                        this->textId = 0x5DC;
                        SET_WEEKEVENTREG(WEEKEVENTREG_75_08);
                    }
                    break;

                default:
                    break;
            }
            break;

        case SHIKASHI_TYPE_LOOKED_THROUGH_TELESCOPE:
            switch (GET_PLAYER_FORM) {
                case PLAYER_FORM_DEKU:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_74_80)) {
                        this->textId = 0x5F2;
                    } else {
                        this->textId = 0x5F1;
                    }
                    Actor_OfferTalkExchange(&this->actor, play, this->actor.xzDistToPlayer, this->actor.playerHeightRel,
                                            PLAYER_IA_NONE);
                    break;

                case PLAYER_FORM_HUMAN:
                    this->textId = 0x5F7;
                    break;

                case PLAYER_FORM_GORON:
                case PLAYER_FORM_ZORA:
                    switch (CURRENT_DAY) {
                        case 1:
                            this->textId = 0x5E9;
                            break;

                        case 2:
                            this->textId = 0x5EA;
                            break;

                        case 3:
                            if (GameInteractor_Should(VB_TIME_UNTIL_MOON_CRASH_CALCULATION, true,
                                                      &timeUntilMoonCrash)) {
                                timeUntilMoonCrash = TIME_UNTIL_MOON_CRASH;
                            }
                            if (timeUntilMoonCrash < CLOCK_TIME_F(1, 0)) {
                                this->textId = 0x5E8;
                            } else {
                                this->textId = 0x5EB;
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SHIKASHI_ANIM_TALK, &this->animIndex);
    this->actionFunc = EnBji01_DialogueHandler;
}

void EnBji01_DialogueHandler(EnBji01* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
            Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x444);
            func_809CCDE0(this, play);
            if (this->actor.shape.rot.y == this->actor.yawTowardsPlayer) {
                Message_StartTextbox(play, this->textId, &this->actor);
            }
            break;

        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                this->actor.params = SHIKASHI_TYPE_FINISHED_CONVERSATION;
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        Audio_PlaySfx_MessageDecide();
                        Message_CloseTextbox(play);
                        func_809CD634(this, play);
                        break;

                    case 1:
                        Audio_PlaySfx_MessageCancel();
                        switch (GET_PLAYER_FORM) {
                            case PLAYER_FORM_DEKU:
                                Message_ContinueTextbox(play, 0x5F0);
                                break;

                            case PLAYER_FORM_HUMAN:
                                Message_ContinueTextbox(play, 0x5F8);
                                break;

                            case PLAYER_FORM_GORON:
                            case PLAYER_FORM_ZORA:
                                Message_ContinueTextbox(play, 0x5E1);
                                break;

                            default:
                                break;
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                switch (play->msgCtx.currentTextId) {
                    case 0x5DE:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                        SHIKASHI_ANIM_SCRATCH_CHIN, &this->animIndex);
                        Message_ContinueTextbox(play, 0x5DF);
                        break;

                    case 0x5E4:
                        Message_ContinueTextbox(play, 0x5E7);
                        break;

                    case 0x5E5:
                        Message_ContinueTextbox(play, 0x5E0);
                        break;

                    case 0x5E7:
                        Message_ContinueTextbox(play, 0x5E5);
                        break;

                    case 0x5DC:
                    case 0x5DD:
                    case 0x5DF:
                    case 0x5EC:
                    case 0x5ED:
                    case 0x5EE:
                    case 0x5F2:
                    case 0x5F5:
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x5F0:
                    case 0x5F6:
                        Message_ContinueTextbox(play, 0x5EF);
                        break;

                    case 0x5E1:
                    case 0x5E8:
                    case 0x5E9:
                    case 0x5EA:
                    case 0x5EB:
                    case 0x5F1:
                    case 0x5F3:
                    case 0x5F4:
                    case 0x5F7:
                    case 0x5F8:
                        Message_CloseTextbox(play);
                        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                        this->actor.params = SHIKASHI_TYPE_FINISHED_CONVERSATION;
                        func_809CCE98(this, play);
                        break;

                    default:
                        break;
                }
            }
            break;

        case TEXT_STATE_DONE:
            this->actor.params = SHIKASHI_TYPE_FINISHED_CONVERSATION;
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            func_809CCE98(this, play);
            break;

        default:
            break;
    }
    if ((this->animIndex == SHIKASHI_ANIM_SCRATCH_CHIN) && (this->skelAnime.curFrame == this->skelAnime.endFrame)) {
        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SHIKASHI_ANIM_TALK, &this->animIndex);
    }
}

void func_809CD634(EnBji01* this, PlayState* play) {
    AudioSfx_MuteBanks((1 << BANK_PLAYER) | (1 << BANK_ITEM) | (1 << BANK_ENV) | (1 << BANK_ENEMY) |
                       (1 << BANK_OCARINA) | (1 << BANK_VOICE));
    SEQCMD_DISABLE_PLAY_SEQUENCES(true);
    play->nextEntrance = ENTRANCE(TERMINA_FIELD, 10); /* Telescope entrance */
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = play->nextEntrance;
    func_80169EFC(play); /* Load new entrance? */
    gSaveContext.respawnFlag = -2;
    this->actionFunc = EnBji01_DoNothing;
}

void EnBji01_DoNothing(EnBji01* this, PlayState* play) {
}

void func_809CD6C0(EnBji01* this, PlayState* play) {
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, SHIKASHI_ANIM_TALK, &this->animIndex);
    this->actionFunc = func_809CD70C;
}

void func_809CD70C(EnBji01* this, PlayState* play) {
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x444);
    func_809CCDE0(this, play);
    if (this->actor.shape.rot.y == this->actor.yawTowardsPlayer) {
        Actor_ChangeFocus(&this->moonsTear->actor, play, &this->actor); /* Z-Target the Moon's Tear? */
        this->actionFunc = func_809CD77C;
    }
}

void func_809CD77C(EnBji01* this, PlayState* play) {
    if (this->moonsTear->actor.colChkInfo.health == 0) {
        func_809CCE98(this, play);
    }
}

void EnBji01_Init(Actor* thisx, PlayState* play) {
    EnBji01* this = (EnBji01*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gShikashiSkel, &object_bji_Anim_000FDC, this->jointTable,
                       this->morphTable, SHIKASHI_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->actor.child = NULL;
    this->animIndex = SHIKASHI_ANIM_NONE;

    Actor_SetScale(&this->actor, 0.01f);
    SubS_FillCutscenesList(&this->actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    this->moonsTear = (ObjMoonStone*)SubS_FindActor(play, NULL, ACTORCAT_PROP, ACTOR_OBJ_MOON_STONE);

    switch (gSaveContext.save.entrance) {
        case ENTRANCE(ASTRAL_OBSERVATORY, 0): /* ECT entrance*/
        case ENTRANCE(ASTRAL_OBSERVATORY, 1): /* TF door entrance */
            this->actor.params = SHIKASHI_TYPE_DEFAULT;
            func_809CCE98(this, play);
            break;

        case ENTRANCE(ASTRAL_OBSERVATORY, 2): /* Telescope entrance */
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            AudioSfx_MuteBanks(0);
            SEQCMD_DISABLE_PLAY_SEQUENCES(false);
            this->actor.params = SHIKASHI_TYPE_LOOKED_THROUGH_TELESCOPE;
            func_809CCE98(this, play);
            break;

        default:
            Actor_Kill(&this->actor);
            break;
    }
}

void EnBji01_Destroy(Actor* thisx, PlayState* play) {
    EnBji01* this = (EnBji01*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnBji01_Update(Actor* thisx, PlayState* play) {
    static s16 sBlinkSequence[] = { 0, 1, 2, 1, 0, 0 };
    EnBji01* this = (EnBji01*)thisx;
    s32 pad;

    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    SkelAnime_Update(&this->skelAnime);

    if (this->blinkTimer-- <= 0) {
        this->blinkSeqIndex--;
        if (this->blinkSeqIndex < 0) {
            this->blinkSeqIndex = 4;
            this->blinkTimer = (Rand_ZeroOne() * 60.0f) + 20.0f;
        } else {
            this->eyeTexIndex = sBlinkSequence[this->blinkSeqIndex];
        }
    }

    Actor_SetFocus(&this->actor, 40.0f);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnBji01_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBji01* this = (EnBji01*)thisx;

    if ((limbIndex == SHIKASHI_LIMB_NONE) && ((play->gameplayFrames % 2) != 0)) {
        *dList = NULL;
    }
    if (limbIndex == SHIKASHI_LIMB_NONE) {
        // Set to itself
        rot->x = rot->x;
        rot->y = rot->y;
        rot->z = rot->z;
    }

    switch (limbIndex) {
        case SHIKASHI_LIMB_TORSO:
            rot->x += this->torsoXRotStep;
            rot->z += this->torsoZRotStep;
            break;

        case SHIKASHI_LIMB_HEAD:
            rot->x += this->headXRotStep;
            rot->z += this->headZRotStep;
            break;

        default:
            break;
    }
    return false;
}

void EnBji01_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_809CDCC8 = { 1088.0f, 1200.0f, 0.0f };
    EnBji01* this = (EnBji01*)thisx;
    Vec3f sp20;
    s32 temp_f4 = 0;

    if (limbIndex == SHIKASHI_LIMB_HEAD) {
        Math_Vec3f_Copy(&sp20, &D_809CDCC8);
        sp20.x += temp_f4 * 0.1f;
        sp20.y += temp_f4 * 0.1f;
        sp20.z += temp_f4 * 0.1f;
        Matrix_MultVec3f(&sp20, &this->actor.focus.pos);
    }
}

void EnBji01_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = { object_bji_Tex_0049F0, object_bji_Tex_004E70, object_bji_Tex_005270 };
    EnBji01* this = (EnBji01*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBji01_OverrideLimbDraw, EnBji01_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
