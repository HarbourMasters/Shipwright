/*
 * File: z_en_hgo.c
 * Overlay: ovl_En_Hgo
 * Description: Pamela's Father (Human)
 */

#include "z_en_hgo.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnHgo_Init(Actor* thisx, PlayState* play);
void EnHgo_Destroy(Actor* thisx, PlayState* play);
void EnHgo_Update(Actor* thisx, PlayState* play);
void EnHgo_Draw(Actor* thisx, PlayState* play);

void EnHgo_SetupDoNothing(EnHgo* this);
void EnHgo_DoNothing(EnHgo* this, PlayState* play);
void EnHgo_UpdateCollision(EnHgo* this, PlayState* play);
void EnHgo_SetupTalk(EnHgo* this);
void EnHgo_Talk(EnHgo* this, PlayState* play);
void EnHgo_SetupDialogueHandler(EnHgo* this);
void EnHgo_DefaultDialogueHandler(EnHgo* this, PlayState* play);
void EnHgo_HandlePlayerChoice(EnHgo* this, PlayState* play);

#define TALK_FLAG_NONE 0
#define TALK_FLAG_HAS_SPOKEN_WITH_HUMAN (1 << 0)
#define TALK_FLAG_HAS_SPOKEN_WITH_OTHER_MASK_FORM (1 << 1)
#define TALK_FLAG_HAS_SPOKEN_WITH_GIBDO_MASK (1 << 2)

typedef enum {
    /* 0 */ HGO_EYE_OPEN,
    /* 1 */ HGO_EYE_HALF,
    /* 2 */ HGO_EYE_CLOSED
} EyeState;

ActorProfile En_Hgo_Profile = {
    /**/ ACTOR_EN_HGO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_HARFGIBUD,
    /**/ sizeof(EnHgo),
    /**/ EnHgo_Init,
    /**/ EnHgo_Destroy,
    /**/ EnHgo_Update,
    /**/ EnHgo_Draw,
};

typedef enum {
    /* 0 */ HGO_ANIM_ARMS_FOLDED,
    /* 1 */ HGO_ANIM_ASTONISHED,
    /* 2 */ HGO_ANIM_KNEEL_DOWN_AND_HUG,
    /* 3 */ HGO_ANIM_CONSOLE,
    /* 4 */ HGO_ANIM_CONSOLE_HEAD_UP,
    /* 5 */ HGO_ANIM_REACH_DOWN_TO_LIFT,
    /* 6 */ HGO_ANIM_TOSS,
    /* 7 */ HGO_ANIM_MAX
} HgoAnimation;

static AnimationInfo sAnimationInfo[HGO_ANIM_MAX] = {
    { &gPamelasFatherArmsFoldedAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },     // HGO_ANIM_ARMS_FOLDED
    { &gPamelasFatherAstonishedAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },      // HGO_ANIM_ASTONISHED
    { &gPamelasFatherKneelDownAndHugAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f }, // HGO_ANIM_KNEEL_DOWN_AND_HUG
    { &gPamelasFatherConsoleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },         // HGO_ANIM_CONSOLE
    { &gPamelasFatherConsoleHeadUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // HGO_ANIM_CONSOLE_HEAD_UP
    { &gPamelasFatherReachDownToLiftAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f }, // HGO_ANIM_REACH_DOWN_TO_LIFT
    { &gPamelasFatherTossAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },            // HGO_ANIM_TOSS
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

void EnHgo_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHgo* this = (EnHgo*)thisx;

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gPamelasFatherHumanSkel, &gPamelasFatherArmsFoldedAnim,
                       this->jointTable, this->morphTable, PAMELAS_FATHER_HUMAN_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisx->colChkInfo, NULL, &sColChkInfoInit);
    thisx->attentionRangeType = ATTENTION_RANGE_6;

    this->eyeIndex = 0;
    this->blinkTimer = 0;
    this->textId = 0;
    this->talkFlags = TALK_FLAG_NONE;
    this->isInCutscene = false;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_75_20) || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE)) {
        EnHgo_SetupTalk(this);
    } else {
        thisx->draw = NULL;
        EnHgo_SetupDoNothing(this);
    }
}

void EnHgo_Destroy(Actor* thisx, PlayState* play) {
    EnHgo* this = (EnHgo*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnHgo_SetupDoNothing(EnHgo* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnHgo_DoNothing;
}

void EnHgo_DoNothing(EnHgo* this, PlayState* play) {
}

void EnHgo_SetupInitCollision(EnHgo* this) {
    this->actionFunc = EnHgo_UpdateCollision;
}

void EnHgo_UpdateCollision(EnHgo* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.focus.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.focus.pos.z;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnHgo_SetupTalk(EnHgo* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HGO_ANIM_ARMS_FOLDED);
    this->actionFunc = EnHgo_Talk;
}

void EnHgo_Talk(EnHgo* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (Player_GetMask(play) == PLAYER_MASK_GIBDO) {
            if (!(this->talkFlags & TALK_FLAG_HAS_SPOKEN_WITH_GIBDO_MASK)) {
                this->talkFlags |= TALK_FLAG_HAS_SPOKEN_WITH_GIBDO_MASK;
                Message_StartTextbox(play, 0x15A5, &this->actor);
                this->textId = 0x15A5; // That mask is a gibdo

            } else {
                Message_StartTextbox(play, 0x15A7, &this->actor);
                this->textId = 0x15A7; // can I research that mask
            }
        } else if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) {
            if (!(this->talkFlags & TALK_FLAG_HAS_SPOKEN_WITH_HUMAN)) {
                this->talkFlags |= TALK_FLAG_HAS_SPOKEN_WITH_HUMAN;
                Message_StartTextbox(play, 0x158F, &this->actor);
                this->textId = 0x158F; // Isn't this a fairy
            } else {
                Message_StartTextbox(play, 0x1593, &this->actor);
                this->textId = 0x1593; // Never seen a fairy this lively
            }
        } else {
            if (!(this->talkFlags & TALK_FLAG_HAS_SPOKEN_WITH_OTHER_MASK_FORM)) {
                this->talkFlags |= TALK_FLAG_HAS_SPOKEN_WITH_OTHER_MASK_FORM;
                Message_StartTextbox(play, 0x1595, &this->actor);
                this->textId = 0x1595; // ghost radar is reacting
            } else {
                Message_StartTextbox(play, 0x1598, &this->actor);
                this->textId = 0x1598; // you seem to be similar to a ghost
            }
        }
        EnHgo_SetupDialogueHandler(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void EnHgo_SetupDialogueHandler(EnHgo* this) {
    this->actionFunc = EnHgo_DefaultDialogueHandler;
}

void EnHgo_DefaultDialogueHandler(EnHgo* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
        case TEXT_STATE_CHOICE:
            break;

        case TEXT_STATE_EVENT:
            EnHgo_HandlePlayerChoice(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                EnHgo_SetupTalk(this);
            }
            break;

        default:
            break;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x71C, 0xB6);
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

// Handles the next dialogue based on player's selection
void EnHgo_HandlePlayerChoice(EnHgo* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x158F:
                Message_StartTextbox(play, 0x1590, &this->actor);
                this->textId = 0x1590;
                break;

            case 0x1590:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
                    Message_StartTextbox(play, 0x1591, &this->actor);
                    this->textId = 0x1591;
                    break;
                }

                Message_StartTextbox(play, 0x1592, &this->actor);
                this->textId = 0x1592;
                break;

            case 0x1591:
                Message_StartTextbox(play, 0x1592, &this->actor);
                this->textId = 0x1592;
                break;

            case 0x1593:
                Message_StartTextbox(play, 0x1594, &this->actor);
                this->textId = 0x1594;
                break;

            case 0x1595:
                Message_StartTextbox(play, 0x1596, &this->actor);
                this->textId = 0x1596;
                break;

            case 0x1596:
                Message_StartTextbox(play, 0x1597, &this->actor);
                this->textId = 0x1597;
                break;

            case 0x1598:
                Message_StartTextbox(play, 0x1599, &this->actor);
                this->textId = 0x1599;
                break;

            case 0x15A5:
                Message_StartTextbox(play, 0x15A6, &this->actor);
                this->textId = 0x15A6;
                break;

            case 0x15A6:
                Message_StartTextbox(play, 0x15A7, &this->actor);
                this->textId = 0x15A7;
                break;

            case 0x15A7:
                Message_CloseTextbox(play);
                EnHgo_SetupTalk(this);
                break;

            default:
                break;
        }
    }
}

s32 EnHgo_HandleCutscene(EnHgo* this, PlayState* play) {
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_486)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_486);
        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;
            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    this->animIndex = HGO_ANIM_ARMS_FOLDED;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HGO_ANIM_ARMS_FOLDED);
                    break;

                case 2:
                    this->actor.draw = EnHgo_Draw;
                    this->animIndex = HGO_ANIM_ASTONISHED;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HGO_ANIM_ASTONISHED);
                    break;

                case 3:
                    this->animIndex = HGO_ANIM_KNEEL_DOWN_AND_HUG;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HGO_ANIM_KNEEL_DOWN_AND_HUG);
                    break;

                case 4:
                    this->animIndex = HGO_ANIM_CONSOLE;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HGO_ANIM_CONSOLE);
                    break;

                case 5:
                    this->animIndex = HGO_ANIM_CONSOLE_HEAD_UP;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HGO_ANIM_CONSOLE_HEAD_UP);
                    break;

                case 6:
                    this->animIndex = HGO_ANIM_REACH_DOWN_TO_LIFT;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HGO_ANIM_REACH_DOWN_TO_LIFT);
                    break;

                default:
                    break;
            }
        } else if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            switch (this->animIndex) {
                case HGO_ANIM_ASTONISHED:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) && !this->isInCutscene) {
                        this->isInCutscene = true;
                        if ((gSaveContext.sceneLayer == 0) &&
                            ((play->csCtx.scriptIndex == 2) || (play->csCtx.scriptIndex == 4))) {
                            Actor_PlaySfx(&this->actor, NA_SE_VO_GBVO02);
                        }
                    }
                    break;

                case HGO_ANIM_KNEEL_DOWN_AND_HUG:
                    this->animIndex = HGO_ANIM_CONSOLE;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HGO_ANIM_CONSOLE);
                    break;

                case HGO_ANIM_REACH_DOWN_TO_LIFT:
                    this->animIndex = HGO_ANIM_TOSS;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HGO_ANIM_TOSS);

                default:
                    break;
            }
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        return true;
    }

    if ((play->csCtx.state == CS_STATE_IDLE) && CHECK_WEEKEVENTREG(WEEKEVENTREG_75_20) &&
        (this->actionFunc == EnHgo_DoNothing)) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Actor_Spawn(&play->actorCtx, play, ACTOR_ELF_MSG2, this->actor.focus.pos.x, this->actor.focus.pos.y,
                    this->actor.focus.pos.z, 7, 0, 0, 0x7F5A);
        EnHgo_SetupInitCollision(this);
    }

    this->cueId = 99;
    return false;
}

void EnHgo_UpdateModel(EnHgo* this, PlayState* play) {
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    if (this->blinkTimer > 2) {
        this->blinkTimer--;
    } else if (this->blinkTimer == 2) {
        this->eyeIndex = HGO_EYE_HALF;
        this->blinkTimer = 1;
    } else if (this->blinkTimer == 1) {
        this->eyeIndex = HGO_EYE_CLOSED;
        this->blinkTimer = 0;
    } else {
        this->eyeIndex = HGO_EYE_OPEN;
        this->blinkTimer = 60;
    }
}

void EnHgo_Update(Actor* thisx, PlayState* play) {
    EnHgo* this = (EnHgo*)thisx;
    s32 pad;

    this->actionFunc(this, play);
    SkelAnime_Update(&this->skelAnime);
    if (EnHgo_HandleCutscene(this, play)) {
        Actor_TrackNone(&this->headRot, &this->torsoRot);
    } else if (this->actionFunc != EnHgo_DoNothing) {
        if (this->actionFunc != EnHgo_UpdateCollision) {
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            EnHgo_UpdateModel(this, play);
        }
    }
}

s32 EnHgo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnHgo* this = (EnHgo*)thisx;

    if (limbIndex == PAMELAS_FATHER_HUMAN_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }
    return false;
}

void EnHgo_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* pos, Actor* thisx) {
    EnHgo* this = (EnHgo*)thisx;

    if (limbIndex == PAMELAS_FATHER_HUMAN_LIMB_HEAD) {
        Matrix_Get(&this->mf);
        Matrix_MultZero(&this->actor.focus.pos);
    }
}

static TexturePtr sEyeTextures[] = {
    gPamelasFatherHumanEyeOpenTex,
    gPamelasFatherHumanEyeHalfTex,
    gPamelasFatherHumanEyeClosedTex,
};

void EnHgo_Draw(Actor* thisx, PlayState* play) {
    EnHgo* this = (EnHgo*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnHgo_OverrideLimbDraw, EnHgo_PostLimbDraw, &this->actor);
    Matrix_Put(&this->mf);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gPamelasFatherHumanEyebrowsDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
