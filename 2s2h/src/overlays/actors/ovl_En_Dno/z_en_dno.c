/*
 * File: z_en_dno.c
 * Overlay: ovl_En_Dno
 * Description: Deku Butler
 *
 * This actor's name is probably short for "Dekunuts Ou (king)". This is likely a misnamed actor,
 * since it uses assets from object_dnj (probably short for "Dekunuts Jii (grandfather, elderly
 * person"). Note that the Deku Princess and King both call the Butler "Jii" in the Japanese text.
 */

#include "z_en_dno.h"
#include "overlays/actors/ovl_Bg_Crace_Movebg/z_bg_crace_movebg.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnDno_Init(Actor* thisx, PlayState* play);
void EnDno_Destroy(Actor* thisx, PlayState* play);
void EnDno_Update(Actor* thisx, PlayState* play);
void EnDno_Draw(Actor* thisx, PlayState* play);

void func_80A71B04(EnDno* this, PlayState* play);
void EnDno_DoNothing(EnDno* this, PlayState* play);
void func_80A71B68(EnDno* this, PlayState* play);
void func_80A71C3C(EnDno* this, PlayState* play);
void func_80A71E54(EnDno* this, PlayState* play);
void func_80A71F18(EnDno* this, PlayState* play);
void func_80A72438(EnDno* this, PlayState* play);
void func_80A724B8(EnDno* this, PlayState* play);
void func_80A725E0(EnDno* this, PlayState* play);
void func_80A725F8(EnDno* this, PlayState* play);
void func_80A72AE4(EnDno* this, PlayState* play);
void func_80A72B3C(EnDno* this, PlayState* play);
void func_80A72B84(EnDno* this, PlayState* play);
void func_80A72BA4(EnDno* this, PlayState* play);
void func_80A72C04(EnDno* this, PlayState* play);
void func_80A72CF8(EnDno* this, PlayState* play);
void func_80A730A0(EnDno* this, PlayState* play);
void func_80A73244(EnDno* this, PlayState* play);
void func_80A732C8(EnDno* this, PlayState* play);
s32 EnDno_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx);
void EnDno_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx);

typedef enum {
    /* -1 */ EN_DNO_ANIM_NONE = -1,
    /*  0 */ EN_DNO_ANIM_START_RACE_START,
    /*  1 */ EN_DNO_ANIM_START_RACE_END,
    /*  2 */ EN_DNO_ANIM_FLY,
    /*  3 */ EN_DNO_ANIM_FAREWELL,
    /*  4 */ EN_DNO_ANIM_GREETING,
    /*  5 */ EN_DNO_ANIM_GREETING_WITH_CANDLE,
    /*  6 */ EN_DNO_ANIM_PRAYER_LOOP,
    /*  7 */ EN_DNO_ANIM_CLOSE_PARASOL,
    /*  8 */ EN_DNO_ANIM_OPEN_PARASOL,
    /*  9 */ EN_DNO_ANIM_IMPLORE_LOOP,
    /* 10 */ EN_DNO_ANIM_IMPLORE_END,
    /* 11 */ EN_DNO_ANIM_TALK,
    /* 12 */ EN_DNO_ANIM_TALK_WITH_PARSOL_AND_CANDLE,
    /* 13 */ EN_DNO_ANIM_IDLE,
    /* 14 */ EN_DNO_ANIM_IDLE_WITH_CANDLE,
    /* 15 */ EN_DNO_ANIM_PRAYER_START,
    /* 16 */ EN_DNO_ANIM_IMPLORE_START,
    /* 17 */ EN_DNO_ANIM_SHOCK_START,
    /* 18 */ EN_DNO_ANIM_SHOCK_LOOP,
    /* 19 */ EN_DNO_ANIM_GRIEVE,
    /* 20 */ EN_DNO_ANIM_MAX
} EnDnoAnimation;

static AnimationSpeedInfo sAnimationSpeedInfo[EN_DNO_ANIM_MAX] = {
    { &gDekuButlerStartRaceStartAnim, 1.0f, ANIMMODE_ONCE, 0.0f },           // EN_DNO_ANIM_START_RACE_START
    { &gDekuButlerStartRaceEndAnim, 1.0f, ANIMMODE_ONCE, 0.0f },             // EN_DNO_ANIM_START_RACE_END
    { &gDekuButlerFlyAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                      // EN_DNO_ANIM_FLY
    { &gDekuButlerFarewellAnim, 1.0f, ANIMMODE_ONCE, 0.0f },                 // EN_DNO_ANIM_FAREWELL
    { &gDekuButlerGreetingAnim, 1.0f, ANIMMODE_ONCE, 0.0f },                 // EN_DNO_ANIM_GREETING
    { &gDekuButlerGreetingWithCandleAnim, 1.0f, ANIMMODE_ONCE, 0.0f },       // EN_DNO_ANIM_GREETING_WITH_CANDLE
    { &gDekuButlerPrayerLoopAnim, 1.0f, ANIMMODE_LOOP, 0.0f },               // EN_DNO_ANIM_PRAYER_LOOP
    { &gDekuButlerCloseParasolAnim, 1.0f, ANIMMODE_ONCE, 0.0f },             // EN_DNO_ANIM_CLOSE_PARASOL
    { &gDekuButlerOpenParasolAnim, 1.0f, ANIMMODE_ONCE, 0.0f },              // EN_DNO_ANIM_OPEN_PARASOL
    { &gDekuButlerImploreLoopAnim, 1.0f, ANIMMODE_LOOP, 0.0f },              // EN_DNO_ANIM_IMPLORE_LOOP
    { &gDekuButlerImploreEndAnim, 1.0f, ANIMMODE_ONCE, 0.0f },               // EN_DNO_ANIM_IMPLORE_END
    { &gDekuButlerTalkAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                     // EN_DNO_ANIM_TALK
    { &gDekuButlerTalkWithParasolAndCandleAnim, 1.0f, ANIMMODE_LOOP, 0.0f }, // EN_DNO_ANIM_TALK_WITH_PARSOL_AND_CANDLE
    { &gDekuButlerIdleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                     // EN_DNO_ANIM_IDLE
    { &gDekuButlerIdleWithCandleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },           // EN_DNO_ANIM_IDLE_WITH_CANDLE
    { &gDekuButlerPrayerStartAnim, 1.0f, ANIMMODE_ONCE, 0.0f },              // EN_DNO_ANIM_PRAYER_START
    { &gDekuButlerImploreStartAnim, 1.0f, ANIMMODE_ONCE, 0.0f },             // EN_DNO_ANIM_IMPLORE_START
    { &gDekuButlerShockStartAnim, 1.0f, ANIMMODE_ONCE, 0.0f },               // EN_DNO_ANIM_SHOCK_START
    { &gDekuButlerShockLoopAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                // EN_DNO_ANIM_SHOCK_LOOP
    { &gDekuButlerGrieveAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                   // EN_DNO_ANIM_GRIEVE
};

ActorProfile En_Dno_Profile = {
    /**/ ACTOR_EN_DNO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DNJ,
    /**/ sizeof(EnDno),
    /**/ EnDno_Init,
    /**/ EnDno_Destroy,
    /**/ EnDno_Update,
    /**/ EnDno_Draw,
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
    { 17, 58, 0, { 0, 0, 0 } },
};

static Vec3f D_80A73B2C = { 0.0f, 0.0f, 1.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDownward, 80, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_STOP),
};

void func_80A711D0(EnDno* this, PlayState* play, Vec3f* vec) {
    f32 rand = Rand_ZeroOne() * 0.5f;

    Lights_PointGlowSetInfo(&this->lightInfo, vec->x, vec->y, vec->z, (127.5f * rand) + 127.5f,
                            (100.0f * rand) + 100.0f, (40.0f * rand) + 40.0f, 320);
}

s32 func_80A71424(s16* arg0, s16 arg1, s16 yawToPlayer, s16 rotY, s16 arg4, s16 arg5) {
    s16 temp_v0 = yawToPlayer - rotY;
    s32 ret;

    if (arg4 >= ABS(temp_v0)) {
        ret = Math_ScaledStepToS(arg0, arg1 + temp_v0, arg5);
    } else {
        ret = Math_ScaledStepToS(arg0, arg1, arg5);
    }

    return ret;
}

void func_80A714B4(EnDno* this, PlayState* play) {
    Actor* actor = NULL;

    do {
        actor = SubS_FindActor(play, actor, ACTORCAT_BG, ACTOR_BG_CRACE_MOVEBG);
        if (actor != NULL) {
            if (BG_CRACE_MOVEBG_GET_TYPE(actor) == BG_CRACE_MOVEBG_TYPE_OPENING) {
                Flags_SetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(actor));
            }
            actor = actor->next;
        }
    } while (actor != NULL);
}

// Unused?
void func_80A7153C(EnDno* this, Vec3f* arg1, Vec3f* arg2) {
    f32 sp1C = Math_CosS(this->actor.home.rot.y);
    f32 sp18 = Math_SinS(this->actor.home.rot.y);
    f32 temp_f2 = arg2->x - this->actor.home.pos.x;
    f32 temp_f12 = arg2->z - this->actor.home.pos.z;

    arg1->x = (temp_f2 * sp1C) - (temp_f12 * sp18);
    arg1->z = (temp_f12 * sp1C) + (temp_f2 * sp18);
    arg1->y = arg2->y - this->actor.home.pos.y;
}

void func_80A715DC(EnDno* this, PlayState* play) {
    BgCraceMovebg* crace = NULL;
    s32 pad[2];
    Vec3f sp88;
    Vec3f sp7C;
    Vec3f sp70;

    do {
        crace = (BgCraceMovebg*)SubS_FindActor(play, &crace->dyna.actor, ACTORCAT_BG, ACTOR_BG_CRACE_MOVEBG);
        if (crace != NULL) {
            if (BG_CRACE_MOVEBG_GET_TYPE(&crace->dyna.actor) == BG_CRACE_MOVEBG_TYPE_CLOSING &&
                !(crace->stateFlags & BG_CRACE_MOVEBG_FLAG_BUTLER_IS_BEYOND_DOOR)) {
                if (SubS_LineSegVsPlane(&crace->dyna.actor.home.pos, &crace->dyna.actor.home.rot, &D_80A73B2C,
                                        &this->actor.prevPos, &this->actor.world.pos, &sp88)) {
                    Math_Vec3f_Diff(&this->actor.world.pos, &crace->dyna.actor.home.pos, &sp7C);
                    Matrix_RotateYS(-crace->dyna.actor.home.rot.y, MTXMODE_NEW);
                    Matrix_MultVec3f(&sp7C, &sp70);
                    if ((fabsf(sp70.x) < 100.0f) && (sp70.y >= -10.0f) && (sp70.y <= 180.0f) && (sp70.z < 0.0f)) {
                        crace->stateFlags |= BG_CRACE_MOVEBG_FLAG_BUTLER_IS_BEYOND_DOOR;
                    }
                }
            }
            crace = (BgCraceMovebg*)crace->dyna.actor.next;
        }
    } while (crace != NULL);
}

void func_80A71788(EnDno* this, PlayState* play) {
    Actor* actor = NULL;

    do {
        actor = SubS_FindActor(play, actor, ACTORCAT_BG, ACTOR_BG_CRACE_MOVEBG);
        if (actor != NULL) {
            Flags_UnsetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(actor));
            actor = actor->next;
        }
    } while (actor != NULL);
}

void EnDno_Init(Actor* thisx, PlayState* play) {
    EnDno* this = (EnDno*)thisx;
    s32 pad;
    Actor* actor = NULL;

    while (true) {
        actor = SubS_FindActor(play, actor, ACTORCAT_NPC, ACTOR_EN_DNO);
        if (actor != NULL) {
            if (actor != thisx) {
                Actor_Kill(thisx);
                break;
            }
            actor = actor->next;
        }

        if (actor == NULL) {
            Actor_ProcessInitChain(thisx, sInitChain);
            ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 21.0f);
            SkelAnime_InitFlex(play, &this->skelAnime, &gDekuButlerSkel, &gDekuButlerIdleAnim, this->jointTable,
                               this->morphTable, DEKU_BUTLER_LIMB_MAX);
            Collider_InitCylinder(play, &this->collider);
            Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);
            Actor_UpdateBgCheckInfo(play, thisx, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
            Animation_Change(&this->skelAnime, sAnimationSpeedInfo[EN_DNO_ANIM_IDLE_WITH_CANDLE].animation, 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationSpeedInfo[EN_DNO_ANIM_IDLE_WITH_CANDLE].animation),
                             sAnimationSpeedInfo[EN_DNO_ANIM_IDLE_WITH_CANDLE].mode,
                             sAnimationSpeedInfo[EN_DNO_ANIM_IDLE_WITH_CANDLE].morphFrames);
            this->unk_3BE = 0x3E93;
            this->unk_3C0 = 60.0f;
            this->unk_3B0 = 0;
            this->cueId = 99;
            this->skelAnime.playSpeed = 0.0f;

            switch (EN_DNO_GET_C000(thisx)) {
                case EN_DNO_GET_C000_0:
                    func_80A71788(this, play);
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20) || CHECK_WEEKEVENTREG(WEEKEVENTREG_93_02)) {
                        Actor_Kill(thisx);
                    } else {
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                        EN_DNO_ANIM_IDLE_WITH_CANDLE, &this->animIndex);
                        thisx->room = -1;
                        gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_STOP;
                        this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
                        this->unk_3B0 |= 1;
                        this->actionFunc = func_80A72438;
                        this->unk_454 = 0.0f;
                    }
                    break;

                case EN_DNO_GET_C000_1:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
                        Actor_Kill(thisx);
                    } else {
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IDLE,
                                                        &this->animIndex);
                        this->unk_460 = SubS_FindActor(play, NULL, ACTORCAT_NPC, ACTOR_EN_DNQ);
                        if (this->unk_460 == NULL) {
                            Actor_Kill(thisx);
                        } else {
                            this->actionFunc = func_80A71B68;
                        }
                    }
                    break;

                default:
                    this->actionFunc = func_80A71B04;
                    break;
            }
            break;
        }
    }
}

void EnDno_Destroy(Actor* thisx, PlayState* play) {
    EnDno* this = (EnDno*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

void func_80A71B04(EnDno* this, PlayState* play) {
    this->unk_452 = 0;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_GRIEVE, &this->animIndex);
    this->actionFunc = EnDno_DoNothing;
}

void EnDno_DoNothing(EnDno* this, PlayState* play) {
}

void func_80A71B68(EnDno* this, PlayState* play) {
    this->unk_452 = 0;
    this->actor.textId = 0;
    if (CHECK_QUEST_ITEM(QUEST_SONG_SONATA)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_27_01)) {
            if (!(this->unk_3B0 & 0x20)) {
                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_PRAYER_LOOP,
                                                &this->animIndex);
                this->actor.shape.rot.y = Actor_WorldYawTowardActor(&this->actor, this->unk_460);
            }
        } else {
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IDLE, &this->animIndex);
        }
    } else {
        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IDLE, &this->animIndex);
    }
    this->actionFunc = func_80A71C3C;
}

void func_80A71C3C(EnDno* this, PlayState* play) {
    switch (this->animIndex) {
        case EN_DNO_ANIM_IMPLORE_LOOP:
        case EN_DNO_ANIM_IMPLORE_START:
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IMPLORE_END,
                                            &this->animIndex);
            break;

        case EN_DNO_ANIM_FAREWELL:
        case EN_DNO_ANIM_IMPLORE_END:
            if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IDLE,
                                                &this->animIndex);
            }
            break;

        case EN_DNO_ANIM_TALK:
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_PRAYER_START,
                                            &this->animIndex);
            break;

        case EN_DNO_ANIM_PRAYER_START:
            if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_PRAYER_LOOP,
                                                &this->animIndex);
            }

        case EN_DNO_ANIM_PRAYER_LOOP:
            Math_SmoothStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardActor(&this->actor, this->unk_460), 2,
                               0xE38, 0x222);
            break;

        default:
            break;
    }

    if ((this->animIndex == EN_DNO_ANIM_IDLE) && (this->actor.xzDistToPlayer <= 120.0f)) {
        func_80A71424(&this->unk_466, 0, this->actor.yawTowardsPlayer, this->actor.shape.rot.y, 0x2000, 0x16C);
    } else {
        Math_ScaledStepToS(&this->unk_466, 0, 0x16C);
    }

    if ((this->animIndex != EN_DNO_ANIM_FAREWELL) && (this->animIndex != EN_DNO_ANIM_PRAYER_START) &&
        (this->animIndex != EN_DNO_ANIM_PRAYER_LOOP)) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x222);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        play->msgCtx.msgMode = MSGMODE_NONE;
        play->msgCtx.msgLength = 0;
        func_80A71E54(this, play);
    } else if (this->actor.xzDistToPlayer < 60.0f) {
        Actor_OfferTalk(&this->actor, play, 60.0f);
    }
}

void func_80A71E54(EnDno* this, PlayState* play) {
    if (CHECK_QUEST_ITEM(QUEST_SONG_SONATA)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_27_01)) {
            this->textId = 0x811;
        } else {
            this->textId = 0x80F;
            SET_WEEKEVENTREG(WEEKEVENTREG_27_01);
        }
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_26_80)) {
        this->textId = 0x80B;
    } else {
        this->textId = 0x80C;
        SET_WEEKEVENTREG(WEEKEVENTREG_26_80);
    }

    if (this->textId != 0x811) {
        this->unk_3B0 |= 0x10;
    } else {
        this->unk_3B0 &= ~0x10;
    }

    this->actionFunc = func_80A71F18;
}

void func_80A71F18(EnDno* this, PlayState* play) {
    Math_ScaledStepToS(&this->unk_466, 0, 0x16C);
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
            if (!(this->unk_3B0 & 0x10) ||
                Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xE38)) {
                switch (this->textId) {
                    case 0x80B:
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                        EN_DNO_ANIM_IMPLORE_START, &this->animIndex);

                    case 0x811:
                        Message_StartTextbox(play, this->textId, &this->actor);
                        break;

                    case 0x80C:
                    case 0x80F:
                        if (this->animIndex == EN_DNO_ANIM_IDLE) {
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_GREETING,
                                                            &this->animIndex);
                        } else if ((this->animIndex == EN_DNO_ANIM_GREETING) &&
                                   (this->skelAnime.curFrame == this->skelAnime.endFrame)) {
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_TALK,
                                                            &this->animIndex);
                            Message_StartTextbox(play, this->textId, &this->actor);
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case TEXT_STATE_FADING:
            if (play->msgCtx.currentTextId == 0x80B) {
                switch (this->animIndex) {
                    case EN_DNO_ANIM_IMPLORE_START:
                        if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                            EN_DNO_ANIM_IMPLORE_LOOP, &this->animIndex);
                        }
                        Math_ScaledStepToS(&this->actor.shape.rot.y,
                                           Actor_WorldYawTowardActor(&this->actor, this->unk_460), 0x71C);
                        break;

                    case EN_DNO_ANIM_IMPLORE_END:
                        if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                            Message_CloseTextbox(play);
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IDLE,
                                                            &this->animIndex);
                            func_80A71B68(this, play);
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
            switch (play->msgCtx.currentTextId) {
                case 0x80B:
                    switch (this->animIndex) {
                        case EN_DNO_ANIM_IMPLORE_START:
                            if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                                EN_DNO_ANIM_IMPLORE_LOOP, &this->animIndex);
                            }
                            break;

                        case EN_DNO_ANIM_IMPLORE_LOOP:
                            if (Message_ShouldAdvance(play)) {
                                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                                EN_DNO_ANIM_IMPLORE_END, &this->animIndex);
                                play->msgCtx.msgMode = MSGMODE_PAUSED;
                            }
                            break;

                        case EN_DNO_ANIM_IMPLORE_END:
                            if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                                Message_CloseTextbox(play);
                                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IDLE,
                                                                &this->animIndex);
                                func_80A71B68(this, play);
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                case 0x80C:
                    if (Message_ShouldAdvance(play)) {
                        Message_ContinueTextbox(play, 0x80D);
                    }
                    break;

                case 0x80D:
                    if (Message_ShouldAdvance(play)) {
                        Message_ContinueTextbox(play, 0x80E);
                    }
                    break;

                case 0x80E:
                    if (this->animIndex == EN_DNO_ANIM_TALK) {
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_FAREWELL,
                                                        &this->animIndex);
                    } else if (this->animIndex == EN_DNO_ANIM_FAREWELL) {
                        if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IDLE,
                                                            &this->animIndex);
                        }
                    } else if ((this->animIndex == EN_DNO_ANIM_IDLE) && Message_ShouldAdvance(play)) {
                        Message_CloseTextbox(play);
                        func_80A71B68(this, play);
                    }
                    break;

                case 0x80F:
                    if (Message_ShouldAdvance(play)) {
                        Message_ContinueTextbox(play, 0x810);
                    }
                    break;

                case 0x810:
                    if (Message_ShouldAdvance(play)) {
                        this->unk_3B0 |= 0x20;
                        Message_CloseTextbox(play);
                        func_80A71B68(this, play);
                        break;
                    }

                    switch (this->animIndex) {
                        case EN_DNO_ANIM_TALK:
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                            EN_DNO_ANIM_PRAYER_START, &this->animIndex);
                            break;

                        case EN_DNO_ANIM_PRAYER_START:
                            if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                                EN_DNO_ANIM_PRAYER_LOOP, &this->animIndex);
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                case 0x811:
                    if (Message_ShouldAdvance(play)) {
                        Message_CloseTextbox(play);
                        func_80A71B68(this, play);
                    }
                    break;

                default:
                    break;
            }
            break;
    }
}

void func_80A72438(EnDno* this, PlayState* play) {
    this->unk_452 = 1;
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IDLE_WITH_CANDLE,
                                    &this->animIndex);
    this->actor.textId = 0;
    if (Flags_GetSwitch(play, EN_DNO_GET_RACE_STARTED_SWITCH_FLAG(&this->actor))) {
        this->unk_454 = 1.0f;
    }
    this->actionFunc = func_80A724B8;
}

void func_80A724B8(EnDno* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 120.0f) {
        func_80A71424(&this->unk_466, 0, this->actor.yawTowardsPlayer, this->actor.home.rot.y, 0x2000, 0x2D8);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80A725E0(this, play);
    } else if (this->actor.xzDistToPlayer < 60.0f) {
        Actor_OfferTalk(&this->actor, play, 60.0f);
    }
}

void func_80A7256C(EnDno* this, PlayState* play) {
    Actor_OfferTalkExchange(&this->actor, play, this->actor.xzDistToPlayer, this->actor.playerHeightRel,
                            PLAYER_IA_MINUS1);
}

void func_80A72598(EnDno* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80A725E0(this, play);
    } else {
        func_80A7256C(this, play);
    }
}

void func_80A725E0(EnDno* this, PlayState* play) {
    this->actionFunc = func_80A725F8;
}

void func_80A725F8(EnDno* this, PlayState* play) {
    s32 pad[2];

    func_80A71424(&this->unk_466, 0, 0, 0, 0x2000, 0x16C);
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
            switch (this->unk_328) {
                case 0:
                    if (this->animIndex == EN_DNO_ANIM_IDLE_WITH_CANDLE) {
                        if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x2D8)) {
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                            EN_DNO_ANIM_GREETING_WITH_CANDLE, &this->animIndex);
                        }
                    } else if ((this->animIndex == EN_DNO_ANIM_GREETING_WITH_CANDLE) &&
                               Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        if (Flags_GetSwitch(play, EN_DNO_GET_RACE_STARTED_SWITCH_FLAG(&this->actor))) {
                            Message_StartTextbox(play, 0x801, &this->actor);
                        } else if (Player_GetMask(play) == PLAYER_MASK_SCENTS) {
                            Message_StartTextbox(play, 0x806, &this->actor);
                        } else {
                            Message_StartTextbox(play, 0x800, &this->actor);
                        }
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                        EN_DNO_ANIM_IDLE_WITH_CANDLE, &this->animIndex);
                    }
                    break;

                case 2:
                    if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x2D8)) {
                        SET_WEEKEVENTREG(WEEKEVENTREG_93_02);
                        Message_StartTextbox(play, 0x802, &this->actor);
                    }
                    break;

                case 3:
                    Message_StartTextbox(play, 0x804, &this->actor);
                    break;

                default:
                    break;
            }
            break;

        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
            if (((play->msgCtx.currentTextId == 0x800) || (play->msgCtx.currentTextId == 0x801)) &&
                (this->animIndex == EN_DNO_ANIM_OPEN_PARASOL)) {
                Math_SmoothStepToF(&this->unk_454, 1.0f, 1.0f, 0.1f, 0.01f);
                if (this->skelAnime.curFrame <= 23.0f) {
                    this->unk_452 = 3;
                    if (Animation_OnFrame(&this->skelAnime, 23.0f)) {
                        Actor_PlaySfx(&this->actor, NA_SE_EV_OPEN_AMBRELLA);
                    }
                } else if (this->skelAnime.curFrame <= 24.0f) {
                    this->unk_452 = 4;
                } else if (this->skelAnime.curFrame >= 25.0f) {
                    this->unk_452 = 2;
                }

                if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                    EN_DNO_ANIM_TALK_WITH_PARSOL_AND_CANDLE, &this->animIndex);
                    Message_StartTextbox(play, 0x803, &this->actor);
                }
            }
            break;

        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
            switch (play->msgCtx.currentTextId) {
                case 0x800:
                case 0x801:
                    if (Message_ShouldAdvance(play)) {
                        play->msgCtx.msgMode = MSGMODE_PAUSED;
                        this->unk_452 = 1;
                        this->unk_454 = 0.0f;
                        SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_OPEN_PARASOL,
                                                        &this->animIndex);
                    }
                    break;

                case 0x802:
                    if (Message_ShouldAdvance(play)) {
                        if (INV_CONTENT(ITEM_MASK_SCENTS) == ITEM_MASK_SCENTS) {
                            this->getItemId = GI_RUPEE_RED;
                        } else {
                            this->getItemId = GI_MASK_SCENTS;
                        }
                        Actor_OfferGetItem(&this->actor, play, this->getItemId, 60.0f, 60.0f);
                        Message_CloseTextbox(play);
                        func_80A72B84(this, play);
                    }
                    break;

                case 0x803:
                    if (Message_ShouldAdvance(play)) {
                        func_80A72AE4(this, play);
                    }
                    break;

                case 0x804:
                    if (this->animIndex == EN_DNO_ANIM_IDLE_WITH_CANDLE) {
                        if (Message_ShouldAdvance(play)) {
                            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo,
                                                            EN_DNO_ANIM_GREETING_WITH_CANDLE, &this->animIndex);
                            if (!(this->unk_3B0 & 0x40)) {
                                func_80A72CF8(this, play);
                                this->unk_3B0 |= 0x40;
                            }
                        }
                    } else if ((this->animIndex == EN_DNO_ANIM_GREETING_WITH_CANDLE) &&
                               (this->skelAnime.curFrame == this->skelAnime.endFrame)) {
                        Message_CloseTextbox(play);
                        func_80A72438(this, play);
                    }
                    break;

                case 0x806:
                    if (Message_ShouldAdvance(play)) {
                        Message_ContinueTextbox(play, 0x800);
                    }
                    break;

                default:
                    func_80A72438(this, play);
                    break;
            }
            break;
    }
}

void func_80A72AE4(EnDno* this, PlayState* play) {
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_START_RACE_START,
                                    &this->animIndex);
    func_80A714B4(this, play);
    this->actionFunc = func_80A72B3C;
}

void func_80A72B3C(EnDno* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->unk_44E = 0;
        func_80A72C04(this, play);
    }
}

void func_80A72B84(EnDno* this, PlayState* play) {
    this->unk_328 = 3;
    this->actionFunc = func_80A72BA4;
}

void func_80A72BA4(EnDno* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80A72598;
    } else {
        Actor_OfferGetItem(&this->actor, play, this->getItemId, 60.0f, 60.0f);
    }
}

void func_80A72C04(EnDno* this, PlayState* play) {
    SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_START_RACE_START,
                                    &this->animIndex);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    Math_Vec3f_Copy(&this->unk_334, &this->actor.world.pos);
    SubS_ActorPathing_Init(play, &this->unk_334, &this->actor, &this->actorPath, play->setupPathList,
                           EN_DNO_GET_7F(&this->actor), 1, 0, 1, 0);
    SubS_ActorPathing_ComputePointInfo(play, &this->actorPath);

    this->actor.world.rot.y = this->actorPath.rotToCurPoint.y;
    this->actor.world.rot.x = this->actorPath.rotToCurPoint.x;

    Flags_SetSwitch(play, EN_DNO_GET_RACE_STARTED_SWITCH_FLAG(&this->actor));
    this->actionFunc = func_80A730A0;
}

void func_80A72CF8(EnDno* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, this->actor.world.pos.x + 80.0f,
                       this->actor.floorHeight, this->actor.world.pos.z, 0, 0, 0, 0x201);
}

s32 EnDno_ActorPathing_UpdateActorInfo(PlayState* play, ActorPathing* actorPath) {
    Actor* thisx = actorPath->actor;
    s32 pad;
    s32 ret = false;
    f32 sp38;
    s16 temp_v0;
    s32 temp_v0_2;
    s32 sp2C;

    thisx->gravity = 0.0f;
    temp_v0 = thisx->yawTowardsPlayer - thisx->world.rot.y;
    if ((temp_v0 <= 0x4000) && (temp_v0 >= -0x4000)) {
        Math_SmoothStepToF(&thisx->speed, 15.0f, 0.8f, 1.0f, 0.01f);
    } else {
        if (thisx->xzDistToPlayer <= 80.0f) {
            Math_SmoothStepToF(&thisx->speed, 8.0f, 0.5f, 0.5f, 0.01f);
        } else if (thisx->xzDistToPlayer <= 360.0f) {
            Math_SmoothStepToF(&thisx->speed, 7.0f, 0.5f, 0.5f, 0.01f);
        } else {
            Math_SmoothStepToF(&thisx->speed, 3.5f, 0.5f, 0.5f, 0.01f);
        }
    }

    if (actorPath->distSqToCurPoint < SQ(thisx->speed)) {
        ret = true;
    } else {
        sp38 = thisx->speed / sqrtf(actorPath->distSqToCurPointXZ);
        sp2C = ABS(actorPath->rotToCurPoint.x - thisx->world.rot.x);
        temp_v0_2 = sp2C;
        temp_v0_2 *= sp38;
        temp_v0_2 += 0x71C;
        sp2C = ABS(actorPath->rotToCurPoint.y - thisx->world.rot.y);

        Math_ScaledStepToS(&thisx->world.rot.x, actorPath->rotToCurPoint.x, temp_v0_2);
        Math_ScaledStepToS(&thisx->world.rot.y, actorPath->rotToCurPoint.y, (s32)(sp2C * sp38) + 0x71C);
    }

    return ret;
}

s32 EnDno_ActorPathing_Move(PlayState* play, ActorPathing* actorPath) {
    Actor* thisx = actorPath->actor;
    EnDno* this = (EnDno*)thisx;
    f32 sp24 = Math_CosS(-thisx->world.rot.x) * thisx->speed;
    f32 sp20 = gFramerateDivisorHalf;

    thisx->velocity.x = Math_SinS(thisx->world.rot.y) * sp24;
    thisx->velocity.y = Math_SinS(-thisx->world.rot.x) * thisx->speed;
    thisx->velocity.z = Math_CosS(thisx->world.rot.y) * sp24;

    this->unk_334.x += (this->actor.velocity.x * sp20) + this->actor.colChkInfo.displacement.x;
    this->unk_334.y += (this->actor.velocity.y * sp20) + this->actor.colChkInfo.displacement.y;
    this->unk_334.z += (this->actor.velocity.z * sp20) + this->actor.colChkInfo.displacement.z;

    return false;
}

void func_80A730A0(EnDno* this, PlayState* play) {
    s32 nextAnimIndex;

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        nextAnimIndex = EN_DNO_ANIM_NONE;
        switch (this->animIndex) {
            case EN_DNO_ANIM_START_RACE_START:
                if (this->unk_44E >= 20) {
                    nextAnimIndex = EN_DNO_ANIM_START_RACE_END;
                } else {
                    this->unk_44E = 20;
                }
                break;

            case EN_DNO_ANIM_START_RACE_END:
                nextAnimIndex = EN_DNO_ANIM_FLY;
                break;

            default:
                break;
        }

        if (nextAnimIndex > EN_DNO_ANIM_NONE) {
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, nextAnimIndex, &this->animIndex);
        }
    }

    SubS_ActorPathing_Update(play, &this->actorPath, SubS_ActorPathing_ComputePointInfo,
                             EnDno_ActorPathing_UpdateActorInfo, EnDno_ActorPathing_Move,
                             SubS_ActorPathing_SetNextPoint);
    this->unk_45C += 6553;
    this->actorPath.pointOffset.x = 0.0f;
    this->actorPath.pointOffset.y = 0.0f;
    this->actorPath.pointOffset.z = 0.0f;
    Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_334);
    this->actor.world.pos.y += Math_SinS(this->unk_3AC) * (4.0f + Math_SinS(this->unk_3AE));
    this->unk_3AC += 4500;
    this->unk_3AE += 1000;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    func_80A715DC(this, play);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_BUTLER_FRY - SFX_FLAG);
    if (this->actorPath.flags & ACTOR_PATHING_REACHED_END_PERMANENT) {
        Math_Vec3f_Copy(&this->actor.world.pos, &this->actorPath.curPoint);
        this->actor.speed = 0.0f;
        this->actor.velocity.x = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.z = 0.0f;
        func_80A73244(this, play);
    }
}

void func_80A73244(EnDno* this, PlayState* play) {
    this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->unk_328 = 2;
    this->actor.speed = 0.0f;
    Flags_UnsetSwitch(play, EN_DNO_GET_RACE_STARTED_SWITCH_FLAG(&this->actor));
    gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_STOP;
    this->unk_44E = 0;
    this->actionFunc = func_80A732C8;
}

void func_80A732C8(EnDno* this, PlayState* play) {
    s32 pad;

    if (this->unk_44E == 0) {
        if (Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x71C)) {
            this->unk_3B0 |= 4;
            this->unk_44E = 3;
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_CLOSE_PARASOL,
                                            &this->animIndex);
        }
    } else if (this->unk_44E == 3) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_IDLE_WITH_CANDLE,
                                            &this->animIndex);
            func_80A72438(this, play);
        } else {
            if (this->skelAnime.curFrame >= 20.0f) {
                Math_SmoothStepToF(&this->unk_454, 0.0f, 1.0f, 0.125f, 0.01f);
            }

            if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_CLOSE_AMBRELLA);
                this->unk_452 = 4;
            } else if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
                this->unk_452 = 3;
            }
        }
    }
    Actor_MoveWithGravity(&this->actor);
}

void func_80A73408(EnDno* this, PlayState* play) {
    s32 nextAnimIndex;
    u8 changeAnim = true;
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_475)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_475);
        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    nextAnimIndex = EN_DNO_ANIM_IDLE;
                    break;

                case 2:
                    GameInteractor_Should(VB_DEKU_BUTLER_CHANGE_SHOCK_ANIMATION, true, this, &changeAnim);
                    nextAnimIndex = EN_DNO_ANIM_SHOCK_START;
                    break;

                default:
                    nextAnimIndex = EN_DNO_ANIM_START_RACE_START;
                    changeAnim = false;
                    break;
            }

            if (changeAnim) {
                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, nextAnimIndex, &this->animIndex);
            }
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        switch (this->animIndex) {
            case EN_DNO_ANIM_SHOCK_START:
                SubS_ChangeAnimationBySpeedInfo(&this->skelAnime, sAnimationSpeedInfo, EN_DNO_ANIM_SHOCK_LOOP,
                                                &this->animIndex);
                break;

            default:
                break;
        }
    }
}

void EnDno_Update(Actor* thisx, PlayState* play) {
    EnDno* this = (EnDno*)thisx;
    s32 pad;

    SkelAnime_Update(&this->skelAnime);
    func_80A73408(this, play);
    this->actionFunc(this, play);
    if (this->unk_3B0 & 4) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    }
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_SetFocus(&this->actor, 51.0f);
}

void EnDno_Draw(Actor* thisx, PlayState* play) {
    EnDno* this = (EnDno*)thisx;

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDno_OverrideLimbDraw, EnDno_PostLimbDraw, &this->actor);
}

s32 EnDno_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDno* this = (EnDno*)thisx;

    *dList = NULL;
    if (limbIndex == DEKU_BUTLER_LIMB_EYES) {
        rot->x += this->unk_466;
    }
    return false;
}

void EnDno_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80A73B40 = { 0.0f, 0.0f, 0.0f };
    Gfx* gfxOpa;
    Gfx* gfxXlu;
    Vec3f sp84;
    EnDno* this = (EnDno*)thisx;
    s32 pad;
    s32 phi_v0 = false;

    if (*dList != NULL) {
        switch (this->unk_452) {
            case 0:
                if ((limbIndex != DEKU_BUTLER_LIMB_CANDLE) && (limbIndex != DEKU_BUTLER_LIMB_CANDLE_WICK) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_HANDLE) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_LOWER_PETALS) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_MIDDLE_PETALS) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_UPPER_PETALS)) {
                    phi_v0 = true;
                }
                break;

            case 1:
                if ((limbIndex != DEKU_BUTLER_LIMB_CANDLE_WICK) && (limbIndex != DEKU_BUTLER_LIMB_PARASOL_HANDLE) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_LOWER_PETALS) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_MIDDLE_PETALS) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_UPPER_PETALS)) {
                    phi_v0 = true;
                }
                break;

            case 2:
                if ((limbIndex != DEKU_BUTLER_LIMB_PARASOL_LOWER_PETALS) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_MIDDLE_PETALS)) {
                    phi_v0 = true;
                }
                break;

            case 3:
                if ((limbIndex != DEKU_BUTLER_LIMB_PARASOL_MIDDLE_PETALS) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_UPPER_PETALS)) {
                    phi_v0 = true;
                }
                break;

            case 4:
                if ((limbIndex != DEKU_BUTLER_LIMB_PARASOL_LOWER_PETALS) &&
                    (limbIndex != DEKU_BUTLER_LIMB_PARASOL_UPPER_PETALS)) {
                    phi_v0 = true;
                }
                break;

            default:
                break;
        }
    }

    if (phi_v0 == true) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        if (limbIndex == DEKU_BUTLER_LIMB_PARASOL_HANDLE) {
            Matrix_Scale(this->unk_454, this->unk_454, this->unk_454, MTXMODE_APPLY);
            Matrix_RotateXS(this->unk_45C, MTXMODE_APPLY);
        }

        gfxOpa = POLY_OPA_DISP;
        MATRIX_FINALIZE_AND_LOAD(gfxOpa, play->state.gfxCtx);
        gSPDisplayList(&gfxOpa[1], *dList);

        POLY_OPA_DISP = &gfxOpa[2];
        CLOSE_DISPS(play->state.gfxCtx);
    }

    if ((this->unk_3B0 & 1) && (limbIndex == DEKU_BUTLER_LIMB_CANDLE_WICK)) {
        u32 frames;

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        frames = play->gameplayFrames;
        Matrix_MultVec3f(&D_80A73B40, &sp84);
        func_80A711D0(this, play, &sp84);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(0.15f, 0.15f, 1.0f, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -3200.0f, 0.0f, MTXMODE_APPLY);
        gfxXlu = Gfx_SetupDL71(POLY_XLU_DISP);

        MATRIX_FINALIZE_AND_LOAD(gfxXlu, play->state.gfxCtx);
        gSPSegment(
            &gfxXlu[1], 0x08,
            Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, -frames * 20, 0x20, 0x80, 0, 0, 0, -20));
        gDPSetPrimColor(&gfxXlu[2], 0x80, 0x80, 255, 255, 0, 255);
        gDPSetEnvColor(&gfxXlu[3], 255, 0, 0, 0);
        gSPDisplayList(&gfxXlu[4], gEffFire1DL);

        POLY_XLU_DISP = &gfxXlu[5];

        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
