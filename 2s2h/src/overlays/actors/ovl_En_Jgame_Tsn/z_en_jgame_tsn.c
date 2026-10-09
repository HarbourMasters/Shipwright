/*
 * File: z_en_jgame_tsn.c
 * Overlay: ovl_En_Jgame_Tsn
 * Description: Fisherman's Jumping Game - Fisherman
 */

#include "z_en_jgame_tsn.h"
#include "overlays/actors/ovl_Obj_Jgame_Light/z_obj_jgame_light.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnJgameTsn_Init(Actor* thisx, PlayState* play);
void EnJgameTsn_Destroy(Actor* thisx, PlayState* play);
void EnJgameTsn_Update(Actor* thisx, PlayState* play);
void EnJgameTsn_Draw(Actor* thisx, PlayState* play);

void func_80C13A2C(EnJgameTsn* this, PlayState* play);
void func_80C13B74(EnJgameTsn* this);
void func_80C13BB8(EnJgameTsn* this, PlayState* play);
void func_80C13E6C(EnJgameTsn* this);
void func_80C13E90(EnJgameTsn* this, PlayState* play);
void func_80C13F9C(EnJgameTsn* this, PlayState* play);
void func_80C14030(EnJgameTsn* this);
void func_80C14044(EnJgameTsn* this, PlayState* play);
void func_80C1418C(EnJgameTsn* this, PlayState* play);
void func_80C141DC(EnJgameTsn* this);
void func_80C14230(EnJgameTsn* this, PlayState* play);
void func_80C144F8(EnJgameTsn* this, PlayState* play);
void func_80C14554(EnJgameTsn* this, PlayState* play);
void func_80C145FC(EnJgameTsn* this);
void func_80C14610(EnJgameTsn* this, PlayState* play);
void func_80C14684(EnJgameTsn* this, PlayState* play);
void func_80C147B4(EnJgameTsn* this, PlayState* play);
s32 func_80C149B0(PlayState* play, EnJgameTsnStruct* arg1);
s32 func_80C14BCC(EnJgameTsn* this, PlayState* play);

ActorProfile En_Jgame_Tsn_Profile = {
    /**/ ACTOR_EN_JGAME_TSN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TSN,
    /**/ sizeof(EnJgameTsn),
    /**/ EnJgameTsn_Init,
    /**/ EnJgameTsn_Destroy,
    /**/ EnJgameTsn_Update,
    /**/ EnJgameTsn_Draw,
};

typedef enum EnJgameTsnAnimation {
    /* -1 */ ENJGAMETSN_ANIM_NONE = -1,
    /*  0 */ ENJGAMETSN_ANIM_0,
    /*  1 */ ENJGAMETSN_ANIM_1,
    /*  2 */ ENJGAMETSN_ANIM_2,
    /*  3 */ ENJGAMETSN_ANIM_MAX
} EnJgameTsnAnimation;

static AnimationInfo sAnimationInfo[ENJGAMETSN_ANIM_MAX] = {
    { &object_tsn_Anim_0092FC, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f }, // ENJGAMETSN_ANIM_0
    { &object_tsn_Anim_000964, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f }, // ENJGAMETSN_ANIM_1
    { &object_tsn_Anim_001198, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f }, // ENJGAMETSN_ANIM_2
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
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
    { 30, 40, 0, { 0, 0, 0 } },
};

TexturePtr D_80C150A4[] = {
    object_tsn_Tex_0073B8,
    object_tsn_Tex_0085B8,
};

void EnJgameTsn_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnJgameTsn* this = (EnJgameTsn*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_tsn_Skel_008AB8, &object_tsn_Anim_0092FC, this->jointTable,
                       this->morphTable, OBJECT_TSN_LIMB_MAX);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.velocity.y = 0.0f;

    if (gSaveContext.save.entrance == ENTRANCE(GREAT_BAY_COAST, 13)) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }

    this->unk_2F8 = 0;
    this->unk_2FA = 0;
    this->unk_21C = 0;
    this->unk_2FE = 0;

    func_80C13A2C(this, play);
    func_80C13B74(this);
}

void func_80C13A2C(EnJgameTsn* this, PlayState* play) {
    Path* path = &play->setupPathList[ENJGAMETSN_GET_PATH_INDEX(&this->actor)];
    s32 i;

    if (path == NULL) {
        Actor_Kill(&this->actor);
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_1D8); i++) {
        this->unk_1D8[i].points = Lib_SegmentedToVirtual(path->points);
        this->unk_1D8[i].count = path->count;

        path = &play->setupPathList[path->additionalPathIndex];
        if (path == NULL) {
            Actor_Kill(&this->actor);
        }
    }

    this->unk_1F8.points = Lib_SegmentedToVirtual(path->points);
    this->unk_1F8.count = path->count;

    path = &play->setupPathList[path->additionalPathIndex];
    if (path == NULL) {
        Actor_Kill(&this->actor);
    }

    this->unk_200.points = Lib_SegmentedToVirtual(path->points);
    this->unk_200.count = path->count;
}

void EnJgameTsn_Destroy(Actor* thisx, PlayState* play) {
    EnJgameTsn* this = (EnJgameTsn*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_90_20);
}

void func_80C13B74(EnJgameTsn* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_0);
    this->actionFunc = func_80C13BB8;
}

void func_80C13BB8(EnJgameTsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (this->actor.flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED) {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] > SECONDS_TO_TIMER(0)) {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_1);
                Message_StartTextbox(play, 0x10A2, &this->actor);
                this->unk_300 = 0x10A2;
            } else if (GameInteractor_Should(VB_FAIL_FISHERMAN_JUMPING_GAME, gSaveContext.minigameScore < 20)) {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_1);
                Message_StartTextbox(play, 0x10A2, &this->actor);
                this->unk_300 = 0x10A2;
            } else {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_2);
                Message_StartTextbox(play, 0x10A3, &this->actor);
                this->unk_300 = 0x10A3;
            }
        } else if (((CURRENT_TIME > CLOCK_TIME(4, 0)) && (CURRENT_TIME < CLOCK_TIME(7, 0))) ||
                   ((CURRENT_TIME > CLOCK_TIME(16, 0)) && (CURRENT_TIME < CLOCK_TIME(19, 0)))) {
            Message_StartTextbox(play, 0x1094, &this->actor);
            this->unk_300 = 0x1094;
        } else if (this->unk_2F8 == 0) {
            this->unk_2F8 = 1;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_1);
            Message_StartTextbox(play, 0x1095, &this->actor);
            this->unk_300 = 0x1095;
        } else {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_1);
            Message_StartTextbox(play, 0x1096, &this->actor);
            this->unk_300 = 0x1096;
        }
        func_80C14030(this);
    } else if (this->actor.flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED) {
        Actor_OfferTalk(&this->actor, play, 200.0f);
    } else {
        Actor_OfferTalk(&this->actor, play, 80.0f);
    }

    if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !(player->stateFlags1 & PLAYER_STATE1_2000) &&
        (this->unk_2FE == 0) && (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) && func_80C149B0(play, &this->unk_1F8)) {
        this->unk_2FE = 1;
        func_80C13E6C(this);
    } else if (!(player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->unk_2FE = 0;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 5, 0x71C, 0xB6);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void func_80C13E6C(EnJgameTsn* this) {
    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    this->actionFunc = func_80C13E90;
}

void func_80C13E90(EnJgameTsn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        if (((CURRENT_TIME > CLOCK_TIME(4, 0)) && (CURRENT_TIME < CLOCK_TIME(7, 0))) ||
            ((CURRENT_TIME > CLOCK_TIME(16, 0)) && (CURRENT_TIME < CLOCK_TIME(19, 0)))) {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_2);
            Message_StartTextbox(play, 0x1094, &this->actor);
            this->unk_300 = 0x1094;
        } else {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_1);
            Message_StartTextbox(play, 0x1098, &this->actor);
            this->unk_300 = 0x1098;
        }
        func_80C14030(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 1000.0f);
    }
}

void func_80C13F88(EnJgameTsn* this) {
    this->actionFunc = func_80C13F9C;
}

void func_80C13F9C(EnJgameTsn* this, PlayState* play) {
    if (this->actor.csId != CS_ID_NONE) {
        if (CutsceneManager_IsNext(this->actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
            func_80C14030(this);
        } else {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            }
            CutsceneManager_Queue(this->actor.csId);
        }
    } else {
        func_80C14030(this);
    }
}

void func_80C14030(EnJgameTsn* this) {
    this->actionFunc = func_80C14044;
}

void func_80C14044(EnJgameTsn* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
            break;

        case TEXT_STATE_CHOICE:
            func_80C14684(this, play);
            break;

        case TEXT_STATE_EVENT:
            func_80C147B4(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                if (CutsceneManager_GetCurrentCsId() == this->actor.csId) {
                    CutsceneManager_Stop(this->actor.csId);
                }
                func_80C13B74(this);
            }
            break;

        default:
            break;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x71C, 0xB6);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void func_80C1410C(EnJgameTsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags1 |= PLAYER_STATE1_20;
    Audio_PlaySubBgm(NA_BGM_TIMED_MINI_GAME);
    play->interfaceCtx.minigameState = MINIGAME_STATE_COUNTDOWN_SETUP_3;
    Interface_InitMinigame(play);
    SET_WEEKEVENTREG(WEEKEVENTREG_90_20);
    Interface_StartTimer(TIMER_ID_MINIGAME_2, 120);
    this->actionFunc = func_80C1418C;
}

void func_80C1418C(EnJgameTsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->interfaceCtx.minigameState == MINIGAME_STATE_COUNTDOWN_GO) {
        func_80C141DC(this);
        player->stateFlags1 &= ~PLAYER_STATE1_20;
    }
}

void func_80C141DC(EnJgameTsn* this) {
    this->unk_218 = Rand_Next() & 3;
    this->unk_2FC = 0;
    *this->unk_208[this->unk_218] |= OBJLUPYGAMELIFT_IGNITE_FIRE;
    this->actionFunc = func_80C14230;
}

void func_80C14230(EnJgameTsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    s32 rand;

    if (GameInteractor_Should(VB_JUMPING_GAME_TORCH_RUN_OUT, this->unk_2FC > 100, this->unk_2FC) ||
        func_80C14BCC(this, play)) {
        rand = Rand_Next() % 3;

        this->unk_2FC = 0;
        if (rand < this->unk_218) {
            this->unk_218 = rand;
        } else {
            this->unk_218 = rand + 1;
        }

        for (i = 0; i < ARRAY_COUNT(this->unk_208); i++) {
            if (i == this->unk_218) {
                *this->unk_208[i] |= OBJLUPYGAMELIFT_IGNITE_FIRE;
                *this->unk_208[i] &= ~OBJLUPYGAMELIFT_SNUFF_FIRE;
            } else {
                *this->unk_208[i] |= OBJLUPYGAMELIFT_SNUFF_FIRE;
            }
        }
    }

    this->unk_2FC++;

    if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) && func_80C149B0(play, &this->unk_200)) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_2);
        Message_StartTextbox(play, 0x109F, &this->actor);
        this->unk_300 = 0x109F;
        player->stateFlags1 |= PLAYER_STATE1_20;
        *this->unk_208[this->unk_218] &= ~OBJLUPYGAMELIFT_IGNITE_FIRE;
        Audio_StopSubBgm();
        func_80C14030(this);
    } else if ((player->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) ||
               (player->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_2);
        Message_StartTextbox(play, 0x10A0, &this->actor);
        this->unk_300 = 0x10A0;
        player->stateFlags1 |= PLAYER_STATE1_20;
        *this->unk_208[this->unk_218] &= ~OBJLUPYGAMELIFT_IGNITE_FIRE;
        Audio_StopSubBgm();
        func_80C14030(this);
    }

    if (GameInteractor_Should(VB_JUMPING_GAME_END,
                              gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] == SECONDS_TO_TIMER(0))) {
        Message_StartTextbox(play, 0x10A1, &this->actor);
        this->unk_300 = 0x10A1;
        player->stateFlags1 |= PLAYER_STATE1_20;
        *this->unk_208[this->unk_218] &= ~OBJLUPYGAMELIFT_IGNITE_FIRE;
        Audio_StopSubBgm();
        func_80C14030(this);
    }
}

void func_80C144E4(EnJgameTsn* this) {
    this->actionFunc = func_80C144F8;
}

void func_80C144F8(EnJgameTsn* this, PlayState* play) {
    play->nextEntrance = ENTRANCE(GREAT_BAY_COAST, 13);
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_80;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
}

void func_80C14540(EnJgameTsn* this) {
    this->actionFunc = func_80C14554;
}

void func_80C14554(EnJgameTsn* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_FISHERMANS_JUMPING_GAME_HEART_PIECE)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_FISHERMANS_JUMPING_GAME_HEART_PIECE);
        }
        func_80C145FC(this);
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_FISHERMANS_JUMPING_GAME_HEART_PIECE)) {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_PURPLE, 500.0f, 100.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 500.0f, 100.0f);
    }
}

void func_80C145FC(EnJgameTsn* this) {
    this->actionFunc = func_80C14610;
}

void func_80C14610(EnJgameTsn* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, 0x10A4, &this->actor);
        this->unk_300 = 0x10A4;
        func_80C14030(this);
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 200.0f, PLAYER_IA_MINUS1);
    }
}

void func_80C14684(EnJgameTsn* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex == 0) {
            if (gSaveContext.save.saveInfo.playerData.rupees >= 20) {
                Message_StartTextbox(play, 0x109E, &this->actor);
                this->unk_300 = 0x109E;
                Rupees_ChangeBy(-20);
            } else {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_2);
                Message_StartTextbox(play, 0x109D, &this->actor);
                this->unk_300 = 0x109D;
            }
        } else {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_2);
            Message_StartTextbox(play, 0x109C, &this->actor);
            this->unk_300 = 0x109C;
        }
    }
}

void func_80C1476C(EnJgameTsn* this, PlayState* play) {
    Actor* prop = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (prop != NULL) {
        if (prop->id == ACTOR_OBJ_JGAME_LIGHT) {
            this->unk_208[OBJJGAMELIGHT_GET_7F(prop)] = &prop->colChkInfo.health;
        }
        prop = prop->next;
    }
}

void func_80C147B4(EnJgameTsn* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        switch (this->unk_300) {
            case 0x1095:
                Message_StartTextbox(play, 0x1096, &this->actor);
                this->unk_300 = 0x1096;
                break;

            case 0x1096:
                Message_StartTextbox(play, 0x1097, &this->actor);
                this->unk_300 = 0x1097;
                break;

            case 0x1098:
                Message_StartTextbox(play, 0x1099, &this->actor);
                this->unk_300 = 0x1099;
                func_80C13F88(this);
                break;

            case 0x1099:
                Message_StartTextbox(play, 0x109A, &this->actor);
                this->unk_300 = 0x109A;
                break;

            case 0x109A:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENJGAMETSN_ANIM_0);
                Message_StartTextbox(play, 0x109B, &this->actor);
                this->unk_300 = 0x109B;
                break;

            case 0x109E:
                if (CutsceneManager_GetCurrentCsId() == this->actor.csId) {
                    CutsceneManager_Stop(this->actor.csId);
                }
                Message_CloseTextbox(play);
                func_80C1476C(this, play);
                func_80C1410C(this, play);
                break;

            case 0x109F:
            case 0x10A0:
            case 0x10A1:
                Message_CloseTextbox(play);
                gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_90_20);
                func_80C144E4(this);
                break;

            case 0x10A3:
                Message_CloseTextbox(play);
                func_80C14540(this);
                func_80C14554(this, play);
                break;

            default:
                break;
        }
    }
}

s32 func_80C14960(Vec2f arg0, Vec2f arg1) {
    s32 phi_v1;

    if ((arg1.x * arg0.z) < (arg0.x * arg1.z)) {
        phi_v1 = 1;
    } else {
        phi_v1 = -1;
    }
    return phi_v1;
}

s32 func_80C149B0(PlayState* play, EnJgameTsnStruct* arg1) {
    s32 i = 1;
    s32 temp_s3;
    Player* player = GET_PLAYER(play);
    Vec2f sp64;
    Vec2f sp5C;
    s32 sp58 = true;
    s32 pad;
    s32 sp50 = 0;
    f32 temp_f20 = player->actor.world.pos.z;
    f32 temp_f22 = player->actor.world.pos.x;

    sp64.x = arg1->points[0].z - temp_f20;
    sp64.z = arg1->points[0].x - temp_f22;
    sp5C.x = arg1->points[1].z - temp_f20;
    sp5C.z = arg1->points[1].x - temp_f22;
    temp_s3 = func_80C14960(sp64, sp5C);

    while (i != 0) {
        sp50++;
        if (i < (arg1->count - 1)) {
            i++;
        } else {
            i = 0;
        }

        sp64.x = arg1->points[sp50].z - temp_f20;
        sp64.z = arg1->points[sp50].x - temp_f22;
        sp5C.x = arg1->points[i].z - temp_f20;
        sp5C.z = arg1->points[i].x - temp_f22;

        if (func_80C14960(sp64, sp5C) != temp_s3) {
            sp58 = false;
            break;
        }
    }

    return sp58;
}

s32 func_80C14BCC(EnJgameTsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    s32 phi_s3 = -1;

    if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        for (i = 0; i < ARRAY_COUNT(this->unk_1D8); i++) {
            if (func_80C149B0(play, &this->unk_1D8[i])) {
                phi_s3 = i;
            }
        }

        if (phi_s3 == -1) {
            return false;
        }

        if (phi_s3 == this->unk_218) {
            Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
            *this->unk_208[phi_s3] |= OBJLUPYGAMELIFT_DISPLAY_CORRECT;
            play->interfaceCtx.minigamePoints = 1;
            return true;
        }

        if (*this->unk_208[phi_s3] & OBJLUPYGAMELIFT_IGNITE_FIRE) {
            Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
            *this->unk_208[phi_s3] |= OBJLUPYGAMELIFT_DISPLAY_CORRECT;
            play->interfaceCtx.minigamePoints = 1;
        } else {
            *this->unk_208[phi_s3] |= OBJLUPYGAMELIFT_DISPLAY_INCORRECT;
            Actor_PlaySfx(&this->actor, NA_SE_SY_ERROR);
        }
    }

    return false;
}

void func_80C14D14(EnJgameTsn* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_80C14D58(EnJgameTsn* this, PlayState* play) {
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);

    if (DECR(this->unk_2FA) == 0) {
        this->unk_2FA = Rand_S16Offset(60, 60);
    }

    if ((this->unk_2FA == 1) || (this->unk_2FA == 3)) {
        this->unk_21C = 1;
    } else {
        this->unk_21C = 0;
    }
}

void EnJgameTsn_Update(Actor* thisx, PlayState* play) {
    EnJgameTsn* this = (EnJgameTsn*)thisx;

    this->actionFunc(this, play);

    SkelAnime_Update(&this->skelAnime);
    func_80C14D14(this, play);
    func_80C14D58(this, play);
}

s32 EnJgamesTsn_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnJgameTsn* this = (EnJgameTsn*)thisx;
    s16 temp_v0 = this->headRot.x >> 1;

    if (limbIndex == OBJECT_TSN_LIMB_0F) {
        rot->x += this->headRot.y;
        rot->z += temp_v0;
    } else if (limbIndex == OBJECT_TSN_LIMB_08) {
        rot->x += this->torsoRot.y;
        rot->z += temp_v0;
    }
    return false;
}

void EnJgamesTsn_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    if (limbIndex == OBJECT_TSN_LIMB_0F) {
        Matrix_MultZero(&thisx->focus.pos);
    }
}

void EnJgameTsn_Draw(Actor* thisx, PlayState* play) {
    EnJgameTsn* this = (EnJgameTsn*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80C150A4[this->unk_21C]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(D_80C150A4[this->unk_21C]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnJgamesTsn_OverrideLimbDraw, EnJgamesTsn_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
