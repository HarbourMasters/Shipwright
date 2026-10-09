/*
 * File: z_en_stone_heishi.c
 * Overlay: ovl_En_Stone_heishi
 * Description: Shiro
 */

#include "z_en_stone_heishi.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_REACT_TO_LENS)

void EnStoneheishi_Init(Actor* thisx, PlayState* play);
void EnStoneheishi_Destroy(Actor* thisx, PlayState* play);
void EnStoneheishi_Update(Actor* thisx, PlayState* play);
void EnStoneheishi_Draw(Actor* thisx, PlayState* play);

void func_80BC9560(EnStoneheishi* this, PlayState* play);
void func_80BC9680(EnStoneheishi* this, PlayState* play);
void EnStoneheishi_CheckGivenItem(EnStoneheishi* this, PlayState* play);
void EnStoneheishi_DrinkBottleProcess(EnStoneheishi* this, PlayState* play);
void func_80BC9D28(EnStoneheishi* this, PlayState* play);
void func_80BC9E50(EnStoneheishi* this, PlayState* play);
void func_80BC94B0(EnStoneheishi* this);
void func_80BC9660(EnStoneheishi* this);
void EnStoneheishi_SetupCheckGivenItem(EnStoneheishi*);
void EnStoneheishi_GiveItemReward(EnStoneheishi* this, PlayState* play);
void EnStoneheishi_SetupDrinkBottleProcess(EnStoneheishi* this);

ActorProfile En_Stone_heishi_Profile = {
    /**/ ACTOR_EN_STONE_HEISHI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SDN,
    /**/ sizeof(EnStoneheishi),
    /**/ EnStoneheishi_Init,
    /**/ EnStoneheishi_Destroy,
    /**/ EnStoneheishi_Update,
    /**/ EnStoneheishi_Draw,
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
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 15, 70, 0, { 0, 0, 0 } },
};

static u16 sEnStoneHeishiTextIds[] = { 0x1473, 0x1474, 0x1475, 0x1476, 0x1477, 0x1478, 0x1479, 0x147A, 0x1472 };

typedef enum EnStoneHeishiAnimation {
    /* 0 */ EN_STONE_HEISHI_ANIM_STAND_HAND_ON_HIP,
    /* 1 */ EN_STONE_HEISHI_ANIM_DRINK_1,
    /* 2 */ EN_STONE_HEISHI_ANIM_CHEER_WITH_SPEAR,
    /* 3 */ EN_STONE_HEISHI_ANIM_WAVE,
    /* 4 */ EN_STONE_HEISHI_ANIM_SIT_AND_REACH,
    /* 5 */ EN_STONE_HEISHI_ANIM_DRINK_2,
    /* 6 */ EN_STONE_HEISHI_ANIM_STAND_UP,
    /* 7 */ EN_STONE_HEISHI_ANIM_MAX
} EnStoneHeishiAnimation;

static AnimationHeader* sAnimations[EN_STONE_HEISHI_ANIM_MAX] = {
    &gSoldierStandHandOnHipAnim, // EN_STONE_HEISHI_ANIM_STAND_HAND_ON_HIP
    &gSoldierDrinkAnim,          // EN_STONE_HEISHI_ANIM_DRINK_1
    &gSoldierCheerWithSpearAnim, // EN_STONE_HEISHI_ANIM_CHEER_WITH_SPEAR
    &gSoldierWaveAnim,           // EN_STONE_HEISHI_ANIM_WAVE
    &gSoldierSitAndReachAnim,    // EN_STONE_HEISHI_ANIM_SIT_AND_REACH
    &gSoldierDrinkAnim,          // EN_STONE_HEISHI_ANIM_DRINK_2
    &gSoldierStandUpAnim,        // EN_STONE_HEISHI_ANIM_STAND_UP
};

static u8 sAnimationModes[EN_STONE_HEISHI_ANIM_MAX] = {
    ANIMMODE_LOOP, // EN_STONE_HEISHI_ANIM_STAND_HAND_ON_HIP
    ANIMMODE_ONCE, // EN_STONE_HEISHI_ANIM_DRINK_1
    ANIMMODE_LOOP, // EN_STONE_HEISHI_ANIM_CHEER_WITH_SPEAR
    ANIMMODE_LOOP, // EN_STONE_HEISHI_ANIM_WAVE
    ANIMMODE_LOOP, // EN_STONE_HEISHI_ANIM_SIT_AND_REACH
    ANIMMODE_LOOP, // EN_STONE_HEISHI_ANIM_DRINK_2
    ANIMMODE_ONCE, // EN_STONE_HEISHI_ANIM_STAND_UP
};

typedef enum {
    /* 0 */ EN_STONE_ACTION_0,
    /* 1 */ EN_STONE_ACTION_1,
    /* 2 */ EN_STONE_ACTION_CHECK_ITEM,
    /* 3 */ EN_STONE_ACTION_DRINK_BOTTLE,
    /* 4 */ EN_STONE_ACTION_4
} EnStoneHeishiAction;

typedef enum {
    /* 0 */ EN_STONE_DRINK_BOTTLE_INITIAL,
    /* 1 */ EN_STONE_DRINK_BOTTLE_DRINKING,
    /* 2 */ EN_STONE_DRINK_BOTTLE_EMPTY,
    /* 3 */ EN_STONE_DRINK_BOTTLE_STAND_UP,
    /* 4 */ EN_STONE_DRINK_BOTTLE_STANDING
} EnStoneHeishiDrinkBottle;

typedef enum {
    /* 0 */ EN_STONE_BOTTLE_NONE,
    /* 1 */ EN_STONE_BOTTLE_RED_POTION,
    /* 2 */ EN_STONE_BOTTLE_EMPTY,
    /* 3 */ EN_STONE_BOTTLE_BLUE_POTION
} EnStoneHeishiBottle;

void EnStoneheishi_Init(Actor* thisx, PlayState* play) {
    EnStoneheishi* this = (EnStoneheishi*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gSoldierSkel, &gSoldierWaveAnim, this->jointTable, this->morphTable,
                       SOLDIER_LIMB_MAX);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -3.0f;

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->collider.dim.radius = 40;
    this->collider.dim.height = 40;
    this->collider.dim.yShift = 0;

    func_80BC94B0(this);
}

void EnStoneheishi_Destroy(Actor* thisx, PlayState* play) {
    EnStoneheishi* this = (EnStoneheishi*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnStoneheishi_ChangeAnim(EnStoneheishi* this, s32 animIndex) {
    f32 startFrame = 0.0f;

    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);

    // This will never pass since this animation index is never used.
    if (animIndex == EN_STONE_HEISHI_ANIM_DRINK_2) {
        startFrame = 55.0f;
    }

    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, startFrame, this->animEndFrame,
                     sAnimationModes[this->animIndex], -10.0f);
}

void EnStoneheishi_TrackPlayer(EnStoneheishi* this) {
    s32 yawDiff = ABS_ALT((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y));

    this->targetHeadRot.y = 0;

    if ((this->actor.xzDistToPlayer < 200.0f) && (yawDiff < 0x4E20)) {
        this->targetHeadRot.y = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
        if (this->targetHeadRot.y > 0x2710) {
            this->targetHeadRot.y = 0x2710;
        } else if (this->targetHeadRot.y < -0x2710) {
            this->targetHeadRot.y = -0x2710;
        }
    }
}

void func_80BC94B0(EnStoneheishi* this) {
    this->textIdIndex = 0;
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_41_40)) { // After drinking bottle
        EnStoneheishi_ChangeAnim(this, EN_STONE_HEISHI_ANIM_CHEER_WITH_SPEAR);
        this->textIdIndex = 8;
        this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
    } else { // Initial configuration
        EnStoneheishi_ChangeAnim(this, EN_STONE_HEISHI_ANIM_WAVE);
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_41_80)) {
            this->textIdIndex = 2;
        }
    }
    this->actor.textId = sEnStoneHeishiTextIds[this->textIdIndex];
    this->action = EN_STONE_ACTION_0;
    this->actionFunc = func_80BC9560;
}

void func_80BC9560(EnStoneheishi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 yawDiff;

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80BC9660(this);
        return;
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_41_40) && (play->actorCtx.lensMaskSize != 100)) {
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        return;
    }

    SkelAnime_Update(&this->skelAnime);

    this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;

    yawDiff = ABS_ALT((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y));

    if ((yawDiff <= 0x18F0) && !(player->stateFlags1 & PLAYER_STATE1_800000)) {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void func_80BC9660(EnStoneheishi* this) {
    this->textIdSet = false;
    this->action = EN_STONE_ACTION_1;
    this->actionFunc = func_80BC9680;
}

void func_80BC9680(EnStoneheishi* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->textIdIndex == 0) || (this->textIdIndex == 2)) {
        if (this->animIndex != EN_STONE_HEISHI_ANIM_SIT_AND_REACH) {
            if (fabsf(this->headRot.x - this->targetHeadRot.x) < 50.0f) {
                EnStoneheishi_ChangeAnim(this, EN_STONE_HEISHI_ANIM_SIT_AND_REACH);
            }
            return;
        }
    } else if (this->textIdIndex == 3) {
        if (this->animIndex != EN_STONE_HEISHI_ANIM_WAVE) {
            if ((this->timer == 0) && (fabsf(this->headRot.x - this->targetHeadRot.x) < 50.0f)) {
                EnStoneheishi_ChangeAnim(this, EN_STONE_HEISHI_ANIM_WAVE);
            }
            return;
        } else if (!this->textIdSet && (curFrame >= this->animEndFrame)) {
            Player* player = GET_PLAYER(play);

            this->textIdSet = true;
            player->actor.textId = sEnStoneHeishiTextIds[this->textIdIndex];

            Message_ContinueTextbox(play, sEnStoneHeishiTextIds[this->textIdIndex]);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_SHIRO);
        }
    }

    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);

        if (this->textIdIndex == 2) {
            Message_ContinueTextbox(play, 0xFF);
            EnStoneheishi_SetupCheckGivenItem(this);
        } else if (this->textIdIndex == 3) {
            func_80BC94B0(this);
        } else if (this->textIdIndex == 6) {
            EnStoneheishi_GiveItemReward(this, play);
        } else {
            if (this->textIdIndex < 7) {
                this->textIdIndex++;
            } else {
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_SHIRO);
                func_80BC94B0(this);
                return;
            }
            Message_ContinueTextbox(play, sEnStoneHeishiTextIds[this->textIdIndex]);
        }
    }
}

void EnStoneheishi_SetupCheckGivenItem(EnStoneheishi* this) {
    this->action = EN_STONE_ACTION_CHECK_ITEM;
    this->actionFunc = EnStoneheishi_CheckGivenItem;
}

void EnStoneheishi_CheckGivenItem(EnStoneheishi* this, PlayState* play) {
    PlayerItemAction itemAction;

    SkelAnime_Update(&this->skelAnime);

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_PAUSE_MENU) {
        itemAction = func_80123810(play);

        if (itemAction > PLAYER_IA_NONE) {
            this->timer = 40;
            Message_CloseTextbox(play);

            if ((itemAction == PLAYER_IA_BOTTLE_POTION_RED) || (itemAction == PLAYER_IA_BOTTLE_POTION_BLUE)) {
                this->playerGivesBluePotion = false;
                if (itemAction == PLAYER_IA_BOTTLE_POTION_BLUE) {
                    this->playerGivesBluePotion = true;
                }
                EnStoneheishi_SetupDrinkBottleProcess(this);
            } else {
                Player* player = GET_PLAYER(play);

                this->textIdIndex = 3;
                player->actor.textId = 0;
                SET_WEEKEVENTREG(WEEKEVENTREG_41_80);
                this->action = EN_STONE_ACTION_1;
                this->actionFunc = func_80BC9680;
            }
        } else if (itemAction <= PLAYER_IA_MINUS1) {
            Message_CloseTextbox(play);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_SHIRO);
            func_80BC94B0(this);
        }
    }
}

void EnStoneheishi_SetupDrinkBottleProcess(EnStoneheishi* this) {
    this->action = EN_STONE_ACTION_DRINK_BOTTLE;
    this->actionFunc = EnStoneheishi_DrinkBottleProcess;
}

void EnStoneheishi_DrinkBottleProcess(EnStoneheishi* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    switch (this->drinkBottleState) {
        case EN_STONE_DRINK_BOTTLE_INITIAL:
            if (this->timer == 0) {
                this->textIdIndex = 4;
                Message_ContinueTextbox(play, sEnStoneHeishiTextIds[this->textIdIndex]);
                player->actor.textId = sEnStoneHeishiTextIds[this->textIdIndex];
                this->drinkBottleState++;
            }
            break;

        case EN_STONE_DRINK_BOTTLE_DRINKING:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Player* player = GET_PLAYER(play);

                play->msgCtx.msgLength = 0;
                player->actor.textId = 0;
                player->exchangeItemAction = PLAYER_IA_NONE;
                this->bottleDisplay = EN_STONE_BOTTLE_RED_POTION;

                if (this->playerGivesBluePotion) {
                    this->bottleDisplay = EN_STONE_BOTTLE_BLUE_POTION;
                }

                Player_SetModels(player, 3);
                EnStoneheishi_ChangeAnim(this, EN_STONE_HEISHI_ANIM_DRINK_1);
                this->timer = 30;
                this->drinkBottleState++;
            }
            break;

        case EN_STONE_DRINK_BOTTLE_EMPTY:
            if (this->timer != 0) {
                if ((this->timer < 10) && (this->bottleDisplay != EN_STONE_BOTTLE_EMPTY)) {
                    this->bottleDisplay = EN_STONE_BOTTLE_EMPTY;
                    Actor_PlaySfx(&this->actor, NA_SE_VO_NP_DRINK);
                    Player_UpdateBottleHeld(play, GET_PLAYER(play), ITEM_BOTTLE, PLAYER_IA_BOTTLE_EMPTY);
                }
            } else {
                this->drinkBottleState++;
            }
            break;

        case EN_STONE_DRINK_BOTTLE_STAND_UP:
            if (curFrame >= this->animEndFrame) {
                // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't seem to
                // anything bad.
                Audio_PlayFanfare(NA_BGM_GET_ITEM);
                this->bottleDisplay = EN_STONE_BOTTLE_NONE;
                EnStoneheishi_ChangeAnim(this, EN_STONE_HEISHI_ANIM_STAND_UP);
                this->drinkBottleState++;
            }
            break;

        case EN_STONE_DRINK_BOTTLE_STANDING:
            if (curFrame >= this->animEndFrame) {
                this->textIdIndex = 5;
                Message_ContinueTextbox(play, sEnStoneHeishiTextIds[this->textIdIndex]);
                player->actor.textId = sEnStoneHeishiTextIds[this->textIdIndex];
                EnStoneheishi_ChangeAnim(this, EN_STONE_HEISHI_ANIM_STAND_HAND_ON_HIP);
                this->action = EN_STONE_ACTION_1;
                this->actionFunc = func_80BC9680;
            }
            break;

        default:
            break;
    }
}

void EnStoneheishi_GiveItemReward(EnStoneheishi* this, PlayState* play) {
    Message_CloseTextbox(play);

    if (INV_CONTENT(ITEM_MASK_STONE) == ITEM_MASK_STONE) {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_BLUE, 300.0f, 300.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MASK_STONE, 300.0f, 300.0f);
    }

    if (GameInteractor_Should(VB_STONE_HEISHI_SET_ACTION, true)) {
        this->action = EN_STONE_ACTION_4;
        this->actionFunc = func_80BC9D28;
    }
}

void func_80BC9D28(EnStoneheishi* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->textIdIndex++;
        this->actor.textId = sEnStoneHeishiTextIds[this->textIdIndex];
        SET_WEEKEVENTREG(WEEKEVENTREG_41_40);
        Actor_TalkOfferAccepted(&this->actor, &play->state);
        Actor_OfferTalkExchange(&this->actor, play, 400.0f, 400.0f, PLAYER_IA_MINUS1);
        this->actionFunc = func_80BC9E50;
    } else if (INV_CONTENT(ITEM_MASK_STONE) == ITEM_MASK_STONE) {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_BLUE, 300.0f, 300.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MASK_STONE, 300.0f, 300.0f);
    }
}

void func_80BC9E50(EnStoneheishi* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_STONE_MASK);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_SHIRO);
        this->action = EN_STONE_ACTION_1;
        this->actionFunc = func_80BC9680;
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 400.0f, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnStoneheishi_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnStoneheishi* this = (EnStoneheishi*)thisx;
    Player* player = GET_PLAYER(play);

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actionFunc(this, play);

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Actor_SetScale(&this->actor, 0.01f);

    if ((CHECK_WEEKEVENTREG(WEEKEVENTREG_41_40) || (play->actorCtx.lensMaskSize == 100)) &&
        !(player->stateFlags1 & PLAYER_STATE1_800000)) {
        if ((this->animIndex != EN_STONE_HEISHI_ANIM_WAVE) &&
            ((((this->action == EN_STONE_ACTION_0) || (this->action == EN_STONE_ACTION_1)) ||
              (this->action == EN_STONE_ACTION_CHECK_ITEM)) ||
             ((this->action == EN_STONE_ACTION_DRINK_BOTTLE) &&
              (this->drinkBottleState <= EN_STONE_DRINK_BOTTLE_DRINKING)))) {
            EnStoneheishi_TrackPlayer(this);
        } else {
            this->targetHeadRot.y = 0;
        }

        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_41_40)) {
            Actor_SetFocus(&this->actor, 30.0f);
        } else {
            Actor_SetFocus(&this->actor, 60.0f);
        }

        Math_SmoothStepToS(&this->headRot.y, this->targetHeadRot.y, 1, 0xBB8, 0);
        Math_SmoothStepToS(&this->headRot.x, this->targetHeadRot.x, 1, 0x3E8, 0);

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 EnStoneheishi_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                                   Gfx** gfxP) {
    EnStoneheishi* this = (EnStoneheishi*)thisx;

    if (limbIndex == SOLDIER_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->y += this->headRot.x;
        rot->z += this->headRot.z;
    }

    return false;
}

void EnStoneheishi_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfxP) {
    static Vec3f sLeftHandPos = { 0.0f, 0.0f, 0.0f };
    EnStoneheishi* this = (EnStoneheishi*)thisx;
    Gfx* gfx;

    if ((limbIndex == SOLDIER_LIMB_LEFT_HAND) && (this->bottleDisplay != EN_STONE_BOTTLE_NONE)) {
        gfx = Gfx_SetupDL71(*gfxP);

        sLeftHandPos.x = 320.0f;
        sLeftHandPos.y = 210.0f;
        sLeftHandPos.z = 440.0f;

        Matrix_Translate(sLeftHandPos.x, sLeftHandPos.y, sLeftHandPos.z, MTXMODE_APPLY);

        gDPPipeSync(gfx++);

        Matrix_RotateYS(-0x1770, MTXMODE_APPLY);
        Matrix_RotateXS(0x7D0, MTXMODE_APPLY);
        Matrix_RotateZS(-0x7530, MTXMODE_APPLY);

        if (this->bottleDisplay != EN_STONE_BOTTLE_EMPTY) {
            if (this->bottleDisplay == EN_STONE_BOTTLE_BLUE_POTION) {
                gDPSetEnvColor(gfx++, 0, 0, 200, 0); // Blue Potion
            } else {
                gDPSetEnvColor(gfx++, 200, 0, 0, 0); // Red Potion
            }
            MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
            gSPDisplayList(gfx++, gSoldierBottleContentsDL);
        }

        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
        gSPDisplayList(gfx++, gSoldierBottleDL);

        *gfxP = gfx++;
    }
}

void EnStoneheishi_Draw(Actor* thisx, PlayState* play) {
    EnStoneheishi* this = (EnStoneheishi*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_41_40)) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnStoneheishi_OverrideLimbDraw, EnStoneheishi_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    } else {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        POLY_OPA_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnStoneheishi_OverrideLimbDraw, EnStoneheishi_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
