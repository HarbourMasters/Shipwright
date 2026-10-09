/*
 * File: z_en_toto.c
 * Overlay: ovl_En_Toto
 * Description: Toto
 */

#include "z_en_toto.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

#define ENTOTO_WEEK_EVENT_FLAGS (CHECK_WEEKEVENTREG(WEEKEVENTREG_50_01) || CHECK_WEEKEVENTREG(WEEKEVENTREG_51_80))

void EnToto_Init(Actor* thisx, PlayState* play);
void EnToto_Destroy(Actor* thisx, PlayState* play);
void EnToto_Update(Actor* thisx, PlayState* play);
void EnToto_Draw(Actor* thisx, PlayState* play);

void func_80BA3930(EnToto* this, PlayState* play);
void func_80BA39C8(EnToto* this, PlayState* play);
void func_80BA3DBC(EnToto* this, PlayState* play);
void func_80BA3BFC(EnToto* this, PlayState* play);
void func_80BA3CC4(EnToto* this, PlayState* play);
void func_80BA3D38(EnToto* this, PlayState* play);
s32 func_80BA3EC0(EnToto* this, PlayState* play);
s32 func_80BA3ED4(EnToto* this, PlayState* play);
s32 func_80BA3EE8(EnToto* this, PlayState* play);
s32 func_80BA3F2C(EnToto* this, PlayState* play);
s32 func_80BA3FB0(EnToto* this, PlayState* play);
s32 func_80BA3FCC(EnToto* this, PlayState* play);
s32 func_80BA402C(EnToto* this, PlayState* play);
s32 func_80BA407C(EnToto* this, PlayState* play);
s32 func_80BA40D4(EnToto* this, PlayState* play);
s32 func_80BA4128(EnToto* this, PlayState* play);
s32 func_80BA415C(EnToto* this, PlayState* play);
s32 func_80BA4204(EnToto* this, PlayState* play);
s32 func_80BA42BC(EnToto* this, PlayState* play);
s32 func_80BA43F4(EnToto* this, PlayState* play);
s32 func_80BA445C(EnToto* this, PlayState* play);
s32 func_80BA44A0(EnToto* this, PlayState* play);
s32 func_80BA4530(EnToto* this, PlayState* play);
s32 func_80BA46D8(EnToto* this, PlayState* play);
s32 func_80BA4740(EnToto* this, PlayState* play);
s32 func_80BA47E0(EnToto* this, PlayState* play);
s32 func_80BA49A4(EnToto* this, PlayState* play);
s32 func_80BA4A00(EnToto* this, PlayState* play);
s32 func_80BA4B24(EnToto* this, PlayState* play);
s32 func_80BA4C0C(EnToto* this, PlayState* play);
s32 func_80BA4C44(EnToto* this, PlayState* play);

ActorProfile En_Toto_Profile = {
    /**/ ACTOR_EN_TOTO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZM,
    /**/ sizeof(EnToto),
    /**/ EnToto_Init,
    /**/ EnToto_Destroy,
    /**/ EnToto_Update,
    /**/ EnToto_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0x01000202, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 60, 0, { 0, 0, 0 } },
};

static EnTotoActionFunc D_80BA501C[] = {
    func_80BA3930,
    func_80BA3BFC,
    func_80BA3D38,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_1, ICHAIN_STOP),
};

static EnTotoText D_80BA502C[] = {
    { 0, 0, 0x2B21 }, { 3, 2, 0 },      { 0, 0, 0x2B23 },  { 2, 1, 0x2B24 }, { 4, 0, 0x2B25 },
    { 3, 2, 0 },      { 4, 0, 0x2B25 }, { 16, 0, 0x2A94 }, { 0, 0, 0x2A95 }, { 4, 0, 0x2A96 },
    { 4, 0, 0x2A97 }, { 0, 0, 0x2A98 }, { 0, 0, 0x2A99 },  { 4, 0, 0x2A9A }, { 4, 0, 0x2A9B },
    { 0, 0, 0x2AE1 }, { 0, 0, 0x2AE2 }, { 4, 0, 0x2AE3 },  { 4, 0, 0x2AE4 },
};

typedef enum EnTotoAnimation {
    /* 0 */ ENTOTO_ANIM_0,
    /* 1 */ ENTOTO_ANIM_1,
    /* 2 */ ENTOTO_ANIM_2,
    /* 3 */ ENTOTO_ANIM_3,
    /* 4 */ ENTOTO_ANIM_MAX
} EnTotoAnimation;

static AnimationHeader* sAnimations[ENTOTO_ANIM_MAX] = {
    &object_zm_Anim_0028B8, // ENTOTO_ANIM_0
    &object_zm_Anim_00B894, // ENTOTO_ANIM_1
    &object_zm_Anim_002F20, // ENTOTO_ANIM_2
    &object_zm_Anim_00BC08, // ENTOTO_ANIM_3
};

static EnTotoText D_80BA5088[] = {
    { 5, 0, 0 },  { 6, 20, 0 }, { 7, 0, 0 },  { 8, 9, 0 },  { 9, 10, 0 },       { 1, 0, 0 },  { 10, 0, 0 },
    { 11, 0, 0 }, { 12, 0, 0 }, { 13, 0, 0 }, { 15, 0, 0 }, { 17, 1, 0 },       { 17, 0, 0 }, { 5, 0, 0 },
    { 6, 20, 0 }, { 8, 5, 0 },  { 12, 0, 0 }, { 13, 0, 0 }, { 14, 20, 0x2B22 }, { 1, 0, 0 },  { 17, 0, 0 },
};

static EnTotoUnkStruct2 D_80BA50DC[] = {
    { 0x2B2F, 0x2B30, 0x2B31, { 0xFF96, 0x0016, 0xFE16 } },
    { 0x2B26, 0x2B27, 0x2B28, { 0x0072, 0x0016, 0xFE3C } },
    { 0x2B29, 0x2B2A, 0x2B2B, { 0xFF67, 0x0016, 0xFE6E } },
    { 0x2B2C, 0x2B2D, 0x2B2E, { 0xFFF1, 0x0016, 0xFE74 } },
};

static Vec3s sPlayerOverrideInputPosList[] = {
    { -186, -8, -192 },
    { -223, -3, -252 },
    { -156, 22, -386 },
};

static u16 sOcarinaActionWindFishPrompts[] = {
    OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN,
    OCARINA_ACTION_PROMPT_WIND_FISH_GORON,
    OCARINA_ACTION_PROMPT_WIND_FISH_ZORA,
    OCARINA_ACTION_PROMPT_WIND_FISH_DEKU,
};

static u8 D_80BA5128[] = { 8, 4, 2, 1 };

static EnTotoUnkFunc D_80BA512C[] = {
    func_80BA3F2C, func_80BA3ED4, func_80BA3F2C, func_80BA3F2C, func_80BA3F2C, func_80BA3ED4,
    func_80BA42BC, func_80BA3ED4, func_80BA44A0, func_80BA3FB0, func_80BA402C, func_80BA46D8,
    func_80BA47E0, func_80BA49A4, func_80BA3FB0, func_80BA402C, func_80BA3F2C, func_80BA3EC0,
};

static EnTotoUnkFunc D_80BA5174[] = {
    func_80BA40D4, func_80BA4128, func_80BA415C, func_80BA3EE8, func_80BA3EE8, func_80BA407C,
    func_80BA43F4, func_80BA445C, func_80BA4530, func_80BA4204, func_80BA407C, func_80BA4740,
    func_80BA407C, func_80BA4A00, func_80BA3FCC, func_80BA407C, func_80BA4B24,
};

static EnTotoActionFunc D_80BA51B8[] = {
    func_80BA39C8,
    func_80BA3CC4,
    func_80BA3DBC,
};

void func_80BA36C0(EnToto* this, PlayState* play, s32 index) {
    this->unk2B7 = false;
    this->actionFuncIndex = index;
    D_80BA501C[this->actionFuncIndex](this, play);
}

void EnToto_Init(Actor* thisx, PlayState* play) {
    EnToto* this = (EnToto*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    if ((play->sceneId == SCENE_MILK_BAR) && (CURRENT_TIME >= CLOCK_TIME(6, 0)) &&
        (CURRENT_TIME < CLOCK_TIME(21, 30))) {
        Actor_Kill(&this->actor);
        return;
    }
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_400;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_zm_Skel_00A978,
                       ((play->sceneId == SCENE_SONCHONOIE) ? &object_zm_Anim_003AA8 : &object_zm_Anim_00C880),
                       this->jointTable, this->morphTable, OBJECT_ZM_LIMB_MAX);
    func_80BA36C0(this, play, 0);
    this->actor.shape.rot.x = 0;
}

void EnToto_Destroy(Actor* thisx, PlayState* play) {
    EnToto* this = (EnToto*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80BA383C(EnToto* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && (this->actionFuncIndex == 1) &&
        (this->skelAnime.animation != &object_zm_Anim_000C80)) {
        if ((play->msgCtx.currentTextId != 0x2A98) && (play->msgCtx.currentTextId != 0x2A99)) {
            if ((this->animIndex & 1) || (Rand_ZeroOne() > 0.5f)) {
                this->animIndex = (this->animIndex + 1) & 3;
            }
        }
        Animation_PlayOnce(&this->skelAnime, sAnimations[this->animIndex]);
    }
    FaceChange_UpdateBlinkingNonHuman(&this->faceChange, 20, 80, 3);
}

void func_80BA3930(EnToto* this, PlayState* play) {
    AnimationHeader* anim = &object_zm_Anim_00C880;

    if (play->sceneId == SCENE_SONCHONOIE) {
        anim = &object_zm_Anim_003AA8;
    }
    Animation_MorphToLoop(&this->skelAnime, anim, -4.0f);
}

s32 func_80BA397C(EnToto* this, s16 arg1) {
    s16 diff;

    diff = this->actor.yawTowardsPlayer - this->actor.home.rot.y;
    if (ABS_ALT(diff) < arg1) {
        return 1;
    }
    return 0;
}

void func_80BA39C8(EnToto* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    func_80BA383C(this, play);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80BA36C0(this, play, 1);
        if (play->sceneId != SCENE_SONCHONOIE) {
            Flags_SetSwitch(play, ENTOTO_GET_SWITCH_FLAG_1(&this->actor));
        } else if (player->transformation == PLAYER_FORM_DEKU) {
            Flags_SetSwitch(play, ENTOTO_GET_SWITCH_FLAG_3(&this->actor));
        }
        this->unk2B6 = 0;
        return;
    }

    if (((play->sceneId == SCENE_MILK_BAR) &&
         !((CURRENT_TIME >= CLOCK_TIME(6, 0)) && (CURRENT_TIME <= (CLOCK_TIME(22, 13) + 7)))) ||
        ((play->sceneId != SCENE_MILK_BAR) && func_80BA397C(this, 0x2000))) {
        if (this->unk2B6 != 0) {
            this->text = &D_80BA502C[6];
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            Actor_OfferTalkExchange(&this->actor, play, 9999.9f, 9999.9f, PLAYER_IA_NONE);
        } else {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            Actor_OfferTalk(&this->actor, play, 50.0f);
            // 2S2H [FD Enhancement] - Do not prompt for song if playing as FD. Default to dialog of cancellation
            if (play->sceneId == SCENE_SONCHONOIE || player->transformation == PLAYER_FORM_FIERCE_DEITY) {
                if (player->transformation == PLAYER_FORM_DEKU) {
                    if (!Flags_GetSwitch(play, ENTOTO_GET_SWITCH_FLAG_3(&this->actor))) {
                        this->text = &D_80BA502C[15];
                    } else {
                        this->text = &D_80BA502C[18];
                    }
                } else {
                    this->text = &D_80BA502C[7];
                }
            } else if (ENTOTO_WEEK_EVENT_FLAGS) {
                this->text = &D_80BA502C[0];
            } else if (!Flags_GetSwitch(play, ENTOTO_GET_SWITCH_FLAG_1(&this->actor))) {
                this->text = &D_80BA502C[2];
            } else {
                this->text = &D_80BA502C[3];
            }
        }

        this->actor.textId = this->text->textId;
    }
}

void func_80BA3BFC(EnToto* this, PlayState* play) {
    if (play->sceneId == SCENE_SONCHONOIE) {
        Animation_MorphToPlayOnce(&this->skelAnime, &object_zm_Anim_000C80, -4.0f);
        this->animIndex = ENTOTO_ANIM_0;
    } else {
        if (this->text->unk0 == 4) {
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_TOTO);
        }
        Animation_MorphToLoop(&this->skelAnime, &object_zm_Anim_00B3E0, -4.0f);
    }
}

void func_80BA3C88(EnToto* this) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xFA0, 0x320);
}

void func_80BA3CC4(EnToto* this, PlayState* play) {
    func_80BA383C(this, play);
    func_80BA3C88(this);
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        func_80BA36C0(this, play, this->text->unk1);
    } else {
        func_80BA4C44(this, play);
    }
}

void func_80BA3D38(EnToto* this, PlayState* play) {
    this->csId = this->actor.csId;
    this->text = ENTOTO_WEEK_EVENT_FLAGS ? &D_80BA5088[13] : &D_80BA5088[0];
    func_80BA4C0C(this, play);
    play->actorCtx.flags |= ACTORCTX_FLAG_5;
    this->faceChange.face = 0;
}

void func_80BA3DBC(EnToto* this, PlayState* play) {
    Player* player;

    func_80BA383C(this, play);
    if (!this->unk2B7) {
        if (!func_80BA4C44(this, play)) {
            return;
        }
        if ((this->text->unk1 != 0) && ENTOTO_WEEK_EVENT_FLAGS) {
            this->unk2B7 = true;
            return;
        }
    } else {
        player = GET_PLAYER(play);
        if ((player->stateFlags1 & PLAYER_STATE1_400) && (player->av1.actionVar1 != 0)) {
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_CIRCUS_LEADERS_MASK);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_TOTO);
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN);
        } else {
            return;
        }
    }

    func_80BA36C0(this, play, 0);
    CutsceneManager_Stop(this->csId);
    play->actorCtx.flags &= ~ACTORCTX_FLAG_5;
}

s32 func_80BA3EC0(EnToto* this, PlayState* play) {
    return 1;
}

s32 func_80BA3ED4(EnToto* this, PlayState* play) {
    return 0;
}

s32 func_80BA3EE8(EnToto* this, PlayState* play) {
    if (this->text->unk1 == 2) {
        Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_WAIT);
    }
    return 0;
}

s32 func_80BA3F2C(EnToto* this, PlayState* play) {
    if (this->text->textId != 0) {
        if (GameInteractor_Should(VB_TOTO_START_SOUND_CHECK, true, this)) {
            Message_ContinueTextbox(play, this->text->textId);
        }
    } else {
        Message_CloseTextbox(play);
        func_80BA3EE8(this, play);
    }
    if (this->text->unk0 == 4) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_TOTO);
    }
    return 0;
}

s32 func_80BA3FB0(EnToto* this, PlayState* play) {
    this->unk2B1 = this->text->unk1;
    return 0;
}

s32 func_80BA3FCC(EnToto* this, PlayState* play) {
    if (DECR(this->unk2B1) == 0) {
        Message_StartTextbox(play, this->text->textId, NULL);
        return 1;
    }
    return 0;
}

s32 func_80BA402C(EnToto* this, PlayState* play) {
    s16 prevCsId = this->csId;

    this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
    CutsceneManager_Queue(this->csId);
    CutsceneManager_Stop(prevCsId);
    return 0;
}

s32 func_80BA407C(EnToto* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_StartWithPlayerCs(this->csId, &GET_PLAYER(play)->actor);
        return 1;
    }

    CutsceneManager_Queue(this->csId);
    return 0;
}

s32 func_80BA40D4(EnToto* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        return 1;
    }
    return 0;
}

s32 func_80BA4128(EnToto* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return 1;
    }
    return 0;
}

s32 func_80BA415C(EnToto* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex != 0) {
            Audio_PlaySfx_MessageCancel();
        } else {
            Audio_PlaySfx_MessageDecide();
        }
        return ((play->msgCtx.choiceIndex != 0) ? 0 : this->text->unk1) + 1;
    }
    return 0;
}

s32 func_80BA4204(EnToto* this, PlayState* play) {
    EnTotoUnkStruct2* temp_v1_2;

    if (DECR(this->unk2B1) == 0) {
        if (!ENTOTO_WEEK_EVENT_FLAGS) {
            temp_v1_2 = &D_80BA50DC[gSaveContext.save.playerForm - 1];
            Message_StartTextbox(play, (this->text->unk0 == 6) ? temp_v1_2->unk0 : temp_v1_2->unk4, NULL);
        }
        return 1;
    }
    return 0;
}

s32 func_80BA42BC(EnToto* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u32 numPoints = 0;
    Vec3s* endPosListPtr = &sPlayerOverrideInputPosList[ARRAY_COUNT(sPlayerOverrideInputPosList)];

    func_80BA3FB0(this, play);
    Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_END);
    if (player->actor.world.pos.z > -310.0f) {
        if ((player->actor.world.pos.x > -150.0f) || (player->actor.world.pos.z > -172.0f)) {
            numPoints = ARRAY_COUNT(sPlayerOverrideInputPosList);
        } else if (player->actor.world.pos.z > -232.0f) {
            numPoints = ARRAY_COUNT(sPlayerOverrideInputPosList) - 1;
        } else {
            numPoints = ARRAY_COUNT(sPlayerOverrideInputPosList) - 2;
        }
    }
    Player_InitOverrideInput(play, &this->overrideInputEntry, numPoints, endPosListPtr - numPoints);
    this->spotlights = Actor_Spawn(&play->actorCtx, play, ACTOR_DM_CHAR07, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0xF02);
    return 0;
}

s32 func_80BA43F4(EnToto* this, PlayState* play) {
    func_80BA3C88(this);
    if (Player_UpdateOverrideInput(play, &this->overrideInputEntry, 60.0f)) {
        Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_19);
        return func_80BA4204(this, play);
    }
    return 0;
}

s32 func_80BA445C(EnToto* this, PlayState* play) {
    if (func_80BA4128(this, play)) {
        Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_END);
        return 1;
    }
    return 0;
}

s32 func_80BA44A0(EnToto* this, PlayState* play) {
    CutsceneManager_Stop(this->csId);
    this->unk2B1 = 0;
    return 0;
}

s32 func_80BA44D4(EnTotoUnkStruct2* arg0, Player* player) {
    Vec3f unk6;

    Math_Vec3s_ToVec3f(&unk6, &arg0->unk6);
    if (Math_Vec3f_DistXZ(&player->actor.world.pos, &unk6) < 10.0f) {
        return 1;
    }
    return 0;
}

s32 func_80BA4530(EnToto* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnTotoUnkStruct2* temp_s0;
    s32 i;

    func_80BA3C88(this);
    if (player->actor.world.pos.z > -270.0f) {
        if (this->spotlights != NULL) {
            Actor_Kill(this->spotlights);
        }
        this->unk2B6 = 1;
        return this->text->unk1;
    }
    if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        temp_s0 = &D_80BA50DC[gSaveContext.save.playerForm - 1];
        if (func_80BA44D4(temp_s0, player)) {
            Math_Vec3s_ToVec3f(&player->actor.world.pos, &temp_s0->unk6);
            player->actor.shape.rot.y = 0;
            player->yaw = 0;
            return func_80BA407C(this, play);
        }
        if (!ENTOTO_WEEK_EVENT_FLAGS) {
            for (i = 0; i < ARRAY_COUNT(D_80BA50DC); i++) {
                if (func_80BA44D4(&D_80BA50DC[i], player)) {
                    if (this->unk2B1 < 10) {
                        this->unk2B1++;
                        if (this->unk2B1 >= 10) {
                            Message_StartTextbox(play, D_80BA50DC[GET_PLAYER_FORM - 1].unk2, NULL);
                        }
                    }
                    return 0;
                }
            }
            this->unk2B1 = 0;
        }
    }
    return 0;
}

s32 func_80BA46D8(EnToto* this, PlayState* play) {
    Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_68);
    Message_DisplayOcarinaStaff(play, sOcarinaActionWindFishPrompts[CUR_FORM]);
    return 0;
}

s32 func_80BA4740(EnToto* this, PlayState* play) {
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_END) {
        GameInteractor_Should(VB_BALLAD_PLAYED_FORM, true);
        if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) {
            SET_WEEKEVENTREG(WEEKEVENTREG_56_10);
        }
        if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
            SET_WEEKEVENTREG(WEEKEVENTREG_56_20);
        }
        if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
            SET_WEEKEVENTREG(WEEKEVENTREG_56_40);
        }
        if (GET_PLAYER_FORM == PLAYER_FORM_GORON) {
            SET_WEEKEVENTREG(WEEKEVENTREG_56_80);
        }
        return 1;
    }
    return 0;
}

s32 func_80BA47E0(EnToto* this, PlayState* play) {
    Vec3f spawnPos;
    s32 i;

    this->unk2B3 = 0;
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_56_10)) {
        this->unk2B3 += 1;
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_56_20)) {
        this->unk2B3 += 2;
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_56_40)) {
        this->unk2B3 += 4;
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_56_80)) {
        this->unk2B3 += 8;
    }
    for (i = 0; i < ARRAY_COUNT(D_80BA50DC); i++) {
        if ((GET_PLAYER_FORM != (i + 1)) && (D_80BA5128[i] & this->unk2B3)) {
            Math_Vec3s_ToVec3f(&spawnPos, &D_80BA50DC[i].unk6);

            Actor_Spawn(&play->actorCtx, play, ACTOR_PLAYER, spawnPos.x, spawnPos.y, spawnPos.z, i + 2, 0, 0,
                        PLAYER_PARAMS(0xFF, PLAYER_START_MODE_F) | 0xFFFFF000);
        }
    }
    func_80BA402C(this, play);
    if (ENTOTO_WEEK_EVENT_FLAGS) {
        func_80BA402C(this, play);
    }
    return 0;
}

s32 func_80BA49A4(EnToto* this, PlayState* play) {
    Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_68);
    Audio_PlayFanfareWithPlayerIOCustomPort(NA_BGM_BALLAD_OF_THE_WIND_FISH, 4, this->unk2B3 ^ 0xF);
    this->unk2B1 = 4;
    return 0;
}

s32 func_80BA4A00(EnToto* this, PlayState* play) {
    Actor* actor;

    if (DECR(this->unk2B1) == 0) {
        if (!Audio_IsSequencePlaying(NA_BGM_BALLAD_OF_THE_WIND_FISH)) {
            actor = &GET_PLAYER(play)->actor;
            actor = actor->next;
            while (actor != NULL) {
                Actor_Kill(actor);
                actor = actor->next;
            }
            if (this->spotlights != NULL) {
                Actor_Kill(this->spotlights);
            }
            Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_69);
            if (this->unk2B3 == 0xF) {
                if (CURRENT_DAY == 1) {
                    SET_WEEKEVENTREG(WEEKEVENTREG_50_01);
                } else {
                    SET_WEEKEVENTREG(WEEKEVENTREG_51_80);
                }
            } else {
                func_80BA402C(this, play);
            }
            return 1;
        }
    }
    return 0;
}

s32 func_80BA4B24(EnToto* this, PlayState* play) {
    Player* player;

    if (func_80BA40D4(this, play)) {
        player = GET_PLAYER(play);
        Animation_MorphToPlayOnce(&this->skelAnime, &object_zm_Anim_0028B8, -4.0f);
        if (player->transformation == PLAYER_FORM_ZORA) {
            if (!Flags_GetSwitch(play, ENTOTO_GET_SWITCH_FLAG_1(&this->actor))) {
                Flags_SetSwitch(play, ENTOTO_GET_SWITCH_FLAG_1(&this->actor));
                return 1;
            } else {
                return 3;
            }
        } else {
            if (!Flags_GetSwitch(play, ENTOTO_GET_SWITCH_FLAG_2(&this->actor))) {
                Flags_SetSwitch(play, ENTOTO_GET_SWITCH_FLAG_2(&this->actor));
                return 4;
            } else {
                return 7;
            }
        }
    }
    return 0;
}

s32 func_80BA4C0C(EnToto* this, PlayState* play) {
    return D_80BA512C[this->text->unk0](this, play);
}

s32 func_80BA4C44(EnToto* this, PlayState* play) {
    s32 ret = D_80BA5174[this->text->unk0](this, play);

    if (ret != 0) {
        this->text += ret;
        return func_80BA4C0C(this, play);
    }
    return 0;
}

void func_80BA4CB4(EnToto* this, PlayState* play) {
    CsCmdActorCue* cue = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_525)];

    if (this->cueId != cue->id) {
        this->cueId = cue->id;
        if (this->cueId != 4) {
            if (this->cueId == 3) {
                Animation_MorphToPlayOnce(&this->skelAnime, &object_zm_Anim_001DF0, -4.0f);
            } else {
                Animation_PlayOnce(&this->skelAnime,
                                   (this->cueId == 1) ? &object_zm_Anim_0016A4 : &object_zm_Anim_0022C8);
                if ((this->cueId == 2) && (this->unk2B3 != 0xF)) {
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_TOTO);
                    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GORMAN);
                }
            }
        }
    }
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x320);
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->cueId != 3) {
            Animation_PlayLoop(&this->skelAnime, (this->cueId == 1) ? &object_zm_Anim_00C880 : &object_zm_Anim_001324);
        }
    }
    if ((this->cueId == 4) && !Actor_HasParent(&this->actor, play)) {
        Actor_OfferGetItem(&this->actor, play, GI_MASK_CIRCUS_LEADER, 9999.9f, 9999.9f);
    }
}

void EnToto_Update(Actor* thisx, PlayState* play) {
    EnToto* this = (EnToto*)thisx;
    s32 pad;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_525)) {
        func_80BA4CB4(this, play);
    } else {
        D_80BA51B8[this->actionFuncIndex](this, play);
    }

    Collider_ResetCylinderAC(play, &this->collider.base);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    Actor_SetFocus(&this->actor, 40.0f);
}

void EnToto_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr eyeTextures[] = { gTotoEyesOpenTex, gTotoEyesHalfTex, gTotoEyesClosedTex };
    EnToto* this = (EnToto*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(eyeTextures[this->faceChange.face]));
    Scene_SetRenderModeXlu(play, 0, 1);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
