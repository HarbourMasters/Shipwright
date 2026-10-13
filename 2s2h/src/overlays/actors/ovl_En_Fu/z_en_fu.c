/*
 * File: z_en_fu.c
 * Overlay: ovl_En_Fu
 * Description: Honey & Darling
 */

#include "z_en_fu.h"
#include "overlays/actors/ovl_En_Fu_Mato/z_en_fu_mato.h"
#include "overlays/actors/ovl_En_Fu_Kago/z_en_fu_kago.h"
#include "overlays/actors/ovl_Bg_Fu_Kaiten/z_bg_fu_kaiten.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnFu_Init(Actor* thisx, PlayState* play);
void EnFu_Destroy(Actor* thisx, PlayState* play);
void EnFu_Update(Actor* thisx, PlayState* play);
void EnFu_Draw(Actor* thisx, PlayState* play);

void func_809622FC(EnFu* this);
void func_80962340(EnFu* this, PlayState* play);
void func_809628BC(EnFu* this);
void func_809628D0(EnFu* this, PlayState* play);
void func_809629F8(EnFu* this);
void func_80962A10(EnFu* this, PlayState* play);
void func_80962BA8(EnFu* this);
void func_80962BCC(EnFu* this, PlayState* play);
void func_80962D48(EnFu* this);
void func_80962D60(EnFu* this, PlayState* play);
void func_80962F10(EnFu* this);
void func_80962F4C(EnFu* this, PlayState* play);
void func_8096326C(EnFu* this, PlayState* play);
void func_809632D0(EnFu* this);
void func_80963350(EnFu* this, PlayState* play);
void func_80963540(EnFu* this);
void func_80963560(EnFu* this, PlayState* play);
void func_80963610(EnFu* this);
void func_80963630(EnFu* this, PlayState* play);
s32 EnFu_MovePlayerToPos(PlayState* play, Vec3f targetPos);
s32 func_809638F8(PlayState* play);
void func_809639D0(EnFu* this, PlayState* play);
void func_80963DE4(EnFu* this, PlayState* play);
void func_80963EAC(EnFu* this, PlayState* play);
void func_80963F44(EnFu* this, PlayState* play);
void func_80963F88(EnFu* this, PlayState* play);
void func_80963FF8(EnFu* this, PlayState* play);
void func_8096413C(EnFu* this, PlayState* play);
void func_80964190(EnFu* this, PlayState* play);
void func_8096426C(EnFu* this, PlayState* play);
void func_80964694(EnFu* this, EnFuUnkStruct* ptr, Vec3f* arg2, s32 len);
void func_809647EC(PlayState* play, EnFuUnkStruct* ptr, s32 len);
void func_80964950(PlayState* play, EnFuUnkStruct* ptr, s32 len);

ActorProfile En_Fu_Profile = {
    /**/ ACTOR_EN_FU,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MU,
    /**/ sizeof(EnFu),
    /**/ EnFu_Init,
    /**/ EnFu_Destroy,
    /**/ EnFu_Update,
    /**/ EnFu_Draw,
};

static s32 D_80964B00[] = { 300, 100, 100 };

static Vec3f D_80964B0C = { 0.0f, 60.0f, -8.0f };
static Vec3f D_80964B18 = { 0.0f, 55.0f, 12.0f };
static Vec3f D_80964B24 = { 0.0f, 60.0f, 0.0f };

typedef enum {
    /* 0 */ HONEY_DARLING_ANIM_IDLE,
    /* 1 */ HONEY_DARLING_ANIM_CUP_CHEEKS,
    /* 2 */ HONEY_DARLING_ANIM_HUG,
    /* 3 */ HONEY_DARLING_ANIM_GAME_DANCE,
    /* 4 */ HONEY_DARLING_ANIM_HOLD_HANDS_MORPH,
    /* 5 */ HONEY_DARLING_ANIM_HOLD_HANDS,
    /* 6 */ HONEY_DARLING_ANIM_SURPRISE,
    /* 7 */ HONEY_DARLING_ANIM_MAX
} HoneyAndDarlingAnimation;

static AnimationInfo sAnimationInfo[HONEY_DARLING_ANIM_MAX] = {
    // HONEY_DARLING_ANIM_IDLE
    { &gHoneyAndDarlingIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    // HONEY_DARLING_ANIM_CUP_CHEEKS
    { &gHoneyAndDarlingCupCheeksLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    // HONEY_DARLING_ANIM_HUG
    { &gHoneyAndDarlingHugLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    // HONEY_DARLING_ANIM_GAME_DANCE
    { &gHoneyAndDarlingGameDanceLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },
    // HONEY_DARLING_ANIM_HOLD_HANDS_MORPH
    { &gHoneyAndDarlingHoldHandsLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },
    // HONEY_DARLING_ANIM_HOLD_HANDS
    { &gHoneyAndDarlingHoldHandsLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },
    // HONEY_DARLING_ANIM_SURPRISE
    { &gHoneyAndDarlingSurpiseAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },
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
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

void func_809616E0(EnFu* this, PlayState* play) {
    s32 i;
    f32 temp_f20;
    f32 temp_f22;
    s16 atan;
    s16 spA0 = false;

    if ((GET_PLAYER_FORM == PLAYER_FORM_DEKU) && (CURRENT_DAY == 3)) {
        spA0 = true;
    }
    this->unk_54C = 0;

    for (i = 0; i < this->unk_520; i++) {
        temp_f20 = this->actor.world.pos.x - this->pathPoints[i].x;
        temp_f22 = this->actor.world.pos.z - this->pathPoints[i].z;
        atan = Math_Atan2S_XY(temp_f22, temp_f20);

        if (!spA0 || ((i % 2) != 0)) {
            Actor_Spawn(&play->actorCtx, play, this->unk_544, this->pathPoints[i].x, this->pathPoints[i].y,
                        this->pathPoints[i].z, 0, atan, 0, i);
            this->unk_54C++;
        }

        if (this->unk_546 == 1) {
            Vec3f pos;
            Vec3f velocity = { 0.0f, 0.0f, 0.0f };
            Vec3f accel = { 0.0f, 0.2f, 0.0f };
            Color_RGBA8 primColor = { 255, 255, 255, 255 };
            Color_RGBA8 envColor = { 198, 198, 198, 255 };

            pos.x = this->pathPoints[i].x;
            pos.y = this->pathPoints[i].y;
            pos.z = this->pathPoints[i].z;
            func_800B0EB0(play, &pos, &velocity, &accel, &primColor, &envColor, 100, 150, 10);
            pos.x -= 0.1f * temp_f20;
            pos.z -= 0.1f * temp_f22;
            func_800B3030(play, &pos, &velocity, &accel, 100, 0, 3);
        }
    }
}

void func_809619D0(EnFu* this, PlayState* play) {
    s32 i;
    Path* path = &play->setupPathList[ENFU_GET_PATH_INDEX(&this->actor)];

    switch (CURRENT_DAY) {
        case 1:
            this->unk_542 = 2;
            this->unk_544 = ACTOR_EN_FU_MATO;
            break;

        case 2:
            this->unk_542 = 1;
            this->unk_544 = ACTOR_EN_FU_KAGO;
            break;

        case 3:
            this->unk_542 = 0;
            this->unk_544 = ACTOR_EN_FU_MATO;
            break;

        default:
            this->unk_544 = ACTOR_EN_FU_MATO;
            this->unk_542 = 2;
            break;
    }

    for (i = 0; i < this->unk_542; i++) {
        path = &play->setupPathList[path->additionalPathIndex];
    }

    this->unk_520 = path->count;
    this->pathPoints = Lib_SegmentedToVirtual(path->points);
    func_809616E0(this, play);
}

void EnFu_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFu* this = (EnFu*)thisx;
    Actor* fuKaiten = play->actorCtx.actorLists[ACTORCAT_BG].first;

    while (fuKaiten != NULL) {
        if (fuKaiten->id == ACTOR_BG_FU_KAITEN) {
            this->actor.child = fuKaiten;
            break;
        }
        fuKaiten = fuKaiten->next;
    }

    if (fuKaiten != NULL) {
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gHoneyAndDarlingSkel, &gHoneyAndDarlingCupCheeksLoopAnim,
                           this->jointTable, this->morphTable, HONEY_AND_DARLING_LIMB_MAX);
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        Actor_SetScale(&this->actor, 0.01f);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.gravity = -0.2f;
        this->actor.shape.rot.y += 0x4000;
        this->actor.world.rot = this->actor.shape.rot;
        this->unk_546 = 0;
        this->unk_54A = 1;
        this->unk_54C = 0;
        this->unk_53C = 0;
        this->unk_53E = 0;
        this->unk_540 = 0;
        this->unk_54E = 0;
        this->unk_550 = 0;
        func_809622FC(this);
        this->actor.attentionRangeType = ATTENTION_RANGE_6;
        func_809619D0(this, play);
        if (CURRENT_DAY == 2) {
            Vec3f sp40 = this->actor.child->home.pos;

            this->unk_2D4 =
                (BgFuMizu*)Actor_Spawn(&play->actorCtx, play, ACTOR_BG_FU_MIZU, sp40.x, sp40.y, sp40.z, 0, 0, 0, 0);
        } else {
            this->unk_2D4 = NULL;
        }
    }
}

void EnFu_Destroy(Actor* thisx, PlayState* play) {
    EnFu* this = (EnFu*)thisx;

    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_08_01);
    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80961D10(EnFu* this) {
    if ((this->collider.base.acFlags & AC_HIT) && (this->unk_542 == 0)) {
        return true;
    }

    if ((this->collider.base.ocFlags1 & OC1_HIT) && (this->unk_542 == 2)) {
        Actor* actor = this->collider.base.oc;

        if (actor->id == ACTOR_EN_BOM_CHU) {
            return true;
        }
    }

    return false;
}

void func_80961D7C(PlayState* play) {
    Actor* explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;

    while (explosive != NULL) {
        if ((explosive->id == ACTOR_EN_BOM) && (explosive->bgCheckFlags & BGCHECKFLAG_GROUND)) {
            EnBom* bomb = (EnBom*)explosive;

            if (bomb->actor.floorBgId != BGCHECK_SCENE) {
                DynaPolyActor* fuKago = DynaPoly_GetActor(&play->colCtx, bomb->actor.floorBgId);

                if ((fuKago != NULL) && (fuKago->actor.id == ACTOR_EN_FU_KAGO)) {
                    Math_SmoothStepToF(&bomb->actor.world.pos.x, fuKago->actor.world.pos.x, 0.1f, 1.0f, 0.5f);
                    Math_SmoothStepToF(&bomb->actor.world.pos.z, fuKago->actor.world.pos.z, 0.1f, 1.0f, 0.5f);
                }
            }
        }
        explosive = explosive->next;
    }
}

void func_80961E88(PlayState* play) {
    Actor* explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;

    while (explosive != NULL) {
        Actor_Kill(explosive);
        explosive = explosive->next;
    }
}

void func_80961EC8(PlayState* play) {
    Actor* fuMato = play->actorCtx.actorLists[ACTORCAT_BG].first;

    while (fuMato != NULL) {
        if (fuMato->id == ACTOR_EN_FU_MATO) {
            ((EnFuMato*)fuMato)->unk_30A = 1;
        }
        fuMato = fuMato->next;
    }
}

void func_80961F00(PlayState* play) {
    Actor* fuKago = play->actorCtx.actorLists[ACTORCAT_BG].first;

    while (fuKago != NULL) {
        if (fuKago->id == ACTOR_EN_FU_KAGO) {
            ((EnFuKago*)fuKago)->unk_33C = 1;
        }
        fuKago = fuKago->next;
    }
}

void func_80961F38(PlayState* play, Vec3f* arg1, s16* arg2, s16 arg3, s16 arg4, s16 arg5) {
    Player* player = GET_PLAYER(play);
    s16 sp42;
    s16 sp40;
    Vec3f sp34 = player->actor.focus.pos;

    sp40 = Math_Vec3f_Yaw(arg1, &sp34) - arg3;

    if (arg5 < ABS_ALT(sp40)) {
        Math_SmoothStepToS(&arg2[0], 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&arg2[1], 0, 6, 0x1838, 0x64);
    } else {
        sp42 = Math_Vec3f_Pitch(arg1, &sp34);

        Math_SmoothStepToS(&arg2[0], sp42, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&arg2[1], sp40, 6, 0x1838, 0x64);
    }

    if (arg4 < ABS_ALT(arg2[1])) {
        if (arg2[1] > 0) {
            arg2[1] = arg4;
        } else {
            arg2[1] = -arg4;
        }
    }

    Math_SmoothStepToS(&arg2[2], 0, 6, 0x7D0, 0x64);
}

void func_8096209C(EnFu* this, PlayState* play) {
    Vec3f sp34;

    if (this->unk_53C & 1) {
        func_80961F38(play, &this->unk_508, this->unk_524, this->actor.shape.rot.y, 0x38E3, 0x6AAA);
    } else {
        Math_SmoothStepToS(&this->unk_524[0], 0, 6, 0x1770, 0x64);
        Math_SmoothStepToS(&this->unk_524[1], 0, 6, 0x1770, 0x64);
        Math_SmoothStepToS(&this->unk_524[2], 0, 6, 0x1770, 0x64);
    }

    if (this->unk_53C & 2) {
        s16 rotY = BINANG_SUB(this->actor.shape.rot.y, 0x8000);

        func_80961F38(play, &this->unk_514, this->unk_52A, rotY, 0x38E3, 0x5555);
    } else {
        Math_SmoothStepToS(&this->unk_52A[0], 0, 6, 0x1770, 0x64);
        Math_SmoothStepToS(&this->unk_52A[1], 0, 6, 0x1770, 0x64);
        Math_SmoothStepToS(&this->unk_52A[2], 0, 6, 0x1770, 0x64);
    }

    if ((this->unk_53C & 1) && (this->unk_53C & 2)) {
        sp34 = D_80964B24;
    } else if (this->unk_53C & 1) {
        sp34 = D_80964B0C;
    } else if (this->unk_53C & 2) {
        sp34 = D_80964B18;
    } else {
        sp34 = D_80964B24;
    }

    Matrix_Push();
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
    Matrix_MultVec3f(&sp34, &this->actor.focus.pos);
    Matrix_Pop();
    this->actor.focus.pos.x += this->actor.world.pos.x;
    this->actor.focus.pos.y += this->actor.world.pos.y;
    this->actor.focus.pos.z += this->actor.world.pos.z;
}

void func_809622FC(EnFu* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HONEY_DARLING_ANIM_CUP_CHEEKS);
    this->actionFunc = func_80962340;
}

void func_80962340(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_54A == 2) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (this->unk_54A == 2) {
            if (this->unk_552 == 0x287D) {
                if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
                    Message_StartTextbox(play, 0x287E, &this->actor);
                    this->unk_552 = 0x287E;
                } else if ((CURRENT_DAY == 3) && CHECK_WEEKEVENTREG(WEEKEVENTREG_22_10) &&
                           CHECK_WEEKEVENTREG(WEEKEVENTREG_22_20)) {
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_22_40)) {
                        Message_StartTextbox(play, 0x2883, &this->actor);
                        this->unk_552 = 0x2883;
                    } else {
                        Message_StartTextbox(play, 0x2880, &this->actor);
                        this->unk_552 = 0x2880;
                    }
                } else {
                    Message_StartTextbox(play, 0x287E, &this->actor);
                    this->unk_552 = 0x287E;
                }
            } else if ((gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] == SECONDS_TO_TIMER(0)) &&
                       (this->unk_552 != 0x2888)) {
                Message_StartTextbox(play, 0x2886, &this->actor);
                this->unk_552 = 0x2886;
            } else {
                Message_StartTextbox(play, 0x2889, &this->actor);
                this->unk_552 = 0x2889;
            }
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            player->stateFlags1 &= ~PLAYER_STATE1_20;
            this->unk_54A = 1;
        } else {
            Message_StartTextbox(play, 0x283C, &this->actor);
            this->unk_552 = 0x283C;
        }
        func_809628BC(this);
    } else if (this->unk_54A == 2) {
        Actor_OfferTalk(&this->actor, play, 500.0f);
    } else {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, BINANG_SUB(this->actor.child->shape.rot.y, 0x4000), 10, 3000, 0x64);
}

void func_80962588(EnFu* this, PlayState* play) {
    if (!Message_ShouldAdvance(play) || (this->unk_552 != 0x2871)) {
        return;
    }

    if (play->msgCtx.choiceIndex == 0) {
        if (gSaveContext.save.saveInfo.playerData.rupees >= 10) {
            Audio_PlaySfx_MessageDecide();
            Rupees_ChangeBy(-10);
            func_80963DE4(this, play);
        } else {
            Audio_PlaySfx(NA_SE_SY_ERROR);
            Message_StartTextbox(play, 0x2873, &this->actor);
            this->unk_552 = 0x2873;
        }
    } else {
        Audio_PlaySfx_MessageCancel();
        Message_StartTextbox(play, 0x2872, &this->actor);
        this->unk_552 = 0x2872;
    }
}

void func_80962660(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Message_ShouldAdvance(play)) {
        switch (this->unk_552) {
            case 0x283C:
                func_80963F44(this, play);
                break;

            case 0x283D:
                func_809639D0(this, play);
                break;

            case 0x283E:
            case 0x283F:
            case 0x2841:
            case 0x2843:
            case 0x2845:
                func_80963F44(this, play);
                break;

            case 0x2846:
                Message_StartTextbox(play, 0x2849, &this->actor);
                this->unk_552 = 0x2849;
                break;

            case 0x2847:
            case 0x2849:
            case 0x284B:
            case 0x284D:
            case 0x284F:
            case 0x2851:
            case 0x2853:
            case 0x2855:
            case 0x2857:
            case 0x2859:
            case 0x285B:
            case 0x285D:
            case 0x285F:
            case 0x2861:
            case 0x2863:
            case 0x2865:
            case 0x2867:
            case 0x2869:
            case 0x286B:
            case 0x286D:
            case 0x286F:
            case 0x2873:
            case 0x2875:
            case 0x2877:
            case 0x2879:
            case 0x287B:
                func_80963F44(this, play);
                break;

            case 0x2848:
            case 0x284A:
            case 0x284E:
            case 0x2850:
            case 0x2852:
            case 0x2856:
            case 0x2858:
            case 0x285A:
            case 0x285C:
            case 0x285E:
            case 0x2860:
            case 0x2862:
            case 0x2864:
            case 0x2866:
            case 0x2868:
            case 0x286A:
            case 0x286C:
            case 0x286E:
                Message_StartTextbox(play, 0x2871, &this->actor);
                this->unk_552 = 0x2871;
                break;

            case 0x2876:
            case 0x2878:
            case 0x287A:
            case 0x287C:
                Message_StartTextbox(play, 0x287D, &this->actor);
                this->unk_552 = 0x287D;
                break;

            case 0x287D:
                SET_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                Message_CloseTextbox(play);
                player->stateFlags1 |= PLAYER_STATE1_20;
                this->unk_53C = 0;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HONEY_DARLING_ANIM_GAME_DANCE);
                Audio_PlaySubBgm(NA_BGM_TIMED_MINI_GAME);
                if (this->unk_542 == 0) {
                    if (this->unk_546 == 1) {
                        func_80961EC8(play);
                    }
                    func_809629F8(this);
                } else if (this->unk_542 == 1) {
                    if (this->unk_546 == 1) {
                        func_80961F00(play);
                    }
                    func_80962BA8(this);
                } else if (this->unk_542 == 2) {
                    if (this->unk_546 == 1) {
                        func_80961EC8(play);
                    }
                    func_80962D48(this);
                }
                break;

            case 0x287E:
            case 0x2880:
            case 0x2883:
                Message_CloseTextbox(play);
                func_80963540(this);
                func_80963560(this, play);
                break;

            case 0x2886:
            case 0x2889:
                func_80963F44(this, play);
                break;
        }
    }
}

void func_809628BC(EnFu* this) {
    this->actionFunc = func_809628D0;
}

void func_809628D0(EnFu* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    switch (talkState) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
            break;

        case TEXT_STATE_CHOICE:
            func_80962588(this, play);
            break;

        case TEXT_STATE_EVENT:
            func_80962660(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                this->unk_54A = 1;
                switch (this->unk_552) {
                    case 0x287F:
                    case 0x2881:
                    case 0x2882:
                    case 0x2884:
                    case 0x2887:
                    case 0x288A:
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                        CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                        func_809622FC(this);
                        break;

                    default:
                        func_809622FC(this);
                        break;
                }
            }
            break;

        default:
            break;
    }

    if (talkState != TEXT_STATE_FADING) {
        func_80964190(this, play);
        func_8096426C(this, play);
    }

    func_8096413C(this, play);
}

void func_809629F8(EnFu* this) {
    this->unk_54A = 0;
    this->actionFunc = func_80962A10;
}

void func_80962A10(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    BgFuKaiten* fuKaiten = (BgFuKaiten*)this->actor.child;

    this->unk_53C = 0;
    if ((fuKaiten->rotationSpeed < 300) || (fuKaiten->bounceHeight < 40.0f) || (fuKaiten->bounceSpeed < 600)) {
        Math_SmoothStepToS(&fuKaiten->rotationSpeed, 300, 10, 5, 5);
        Math_SmoothStepToS(&fuKaiten->bounceSpeed, 600, 20, 10, 10);
        Math_SmoothStepToF(&fuKaiten->bounceHeight, 40.0f, 0.1f, 1.0f, 1.0f);
        return;
    }

    Audio_PlaySfx(NA_SE_SY_FOUND);
    player->stateFlags1 &= ~PLAYER_STATE1_20;
    Interface_StartTimer(TIMER_ID_MINIGAME_2, 60);
    if (this->unk_546 == 1) {
        func_809616E0(this, play);
    } else {
        this->unk_546 = 1;
    }

    if ((GET_PLAYER_FORM == PLAYER_FORM_DEKU) && gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
        Magic_Add(play, MAGIC_FILL_TO_CAPACITY);
    }

    func_80962F10(this);
}

void func_80962BA8(EnFu* this) {
    BgFuMizu* mizu = this->unk_2D4;

    this->unk_54A = 0;
    mizu->unk_160 = 1;
    this->actionFunc = func_80962BCC;
}

void func_80962BCC(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    BgFuKaiten* fuKaiten = (BgFuKaiten*)this->actor.child;

    if ((fuKaiten->rotationSpeed < 100) || (fuKaiten->bounceHeight < 30.0f) || (fuKaiten->bounceSpeed < 600)) {
        Math_SmoothStepToS(&fuKaiten->rotationSpeed, 100, 10, 5, 5);
        Math_SmoothStepToS(&fuKaiten->bounceSpeed, 600, 20, 10, 10);
        Math_SmoothStepToF(&fuKaiten->elevation, 10.0f, 0.1f, 1.0f, 1.0f);
        Math_SmoothStepToF(&fuKaiten->bounceHeight, 30.0f, 0.1f, 1.0f, 1.0f);
        return;
    }

    Audio_PlaySfx(NA_SE_SY_FOUND);
    player->stateFlags1 &= ~PLAYER_STATE1_20;
    player->stateFlags3 |= PLAYER_STATE3_400000;
    Interface_StartTimer(TIMER_ID_MINIGAME_2, 60);

    if (this->unk_546 == 1) {
        func_809616E0(this, play);
    } else {
        this->unk_546 = 1;
    }

    play->unk_1887E = 30;
    func_80962F10(this);
}

void func_80962D48(EnFu* this) {
    this->unk_54A = 0;
    this->actionFunc = func_80962D60;
}

void func_80962D60(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    BgFuKaiten* fuKaiten = (BgFuKaiten*)this->actor.child;

    if ((fuKaiten->rotationSpeed < 100) || (fuKaiten->bounceHeight < 40.0f) || (fuKaiten->bounceSpeed < 600)) {
        Math_SmoothStepToS(&fuKaiten->rotationSpeed, 100, 10, 5, 5);
        Math_SmoothStepToS(&fuKaiten->bounceSpeed, 600, 20, 10, 10);
        Math_SmoothStepToF(&fuKaiten->bounceHeight, 40.0f, 0.1f, 1.0f, 1.0f);
        return;
    }

    Audio_PlaySfx(NA_SE_SY_FOUND);
    player->stateFlags1 &= ~PLAYER_STATE1_20;
    player->stateFlags3 |= PLAYER_STATE3_400000;
    Interface_StartTimer(TIMER_ID_MINIGAME_2, 60);

    if (this->unk_546 == 1) {
        func_809616E0(this, play);
    } else {
        this->unk_546 = 1;
    }

    play->unk_1887D = 30;
    func_80962F10(this);
}

void func_80962EBC(EnFu* this, PlayState* play) {
    if (this->unk_542 != 0) {
        if (this->actor.csId != CS_ID_NONE) {
            Camera_ChangeActorCsCamIndex(play->cameraPtrs[CAM_ID_MAIN],
                                         CutsceneManager_GetCutsceneEntry(this->actor.csId)->csCamId);
        }
    }
}

void func_80962F10(EnFu* this) {
    this->unk_548 = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    SET_WEEKEVENTREG(WEEKEVENTREG_08_01);
    this->actionFunc = func_80962F4C;
}

void func_80962F4C(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    BgFuKaiten* fuKaiten = (BgFuKaiten*)this->actor.child;

    switch (this->unk_542) {
        case 0:
            if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) {
                player->stateFlags3 |= PLAYER_STATE3_400;
            }
            break;

        case 1:
            play->unk_1887E = 30;
            break;

        case 2:
            play->unk_1887D = 30;
            break;

        default:
            break;
    }

    if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] < SECONDS_TO_TIMER(20)) {
        s16 val = D_80964B00[this->unk_542] + 200;

        Math_SmoothStepToS(&fuKaiten->rotationSpeed, val, 10, 5, 5);
    } else if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] < SECONDS_TO_TIMER(40)) {
        s16 val = D_80964B00[this->unk_542] + 100;

        Math_SmoothStepToS(&fuKaiten->rotationSpeed, val, 10, 5, 5);
    }

    if (func_80961D10(this)) {
        Message_StartTextbox(play, 0x288B, &this->actor);
    }

    if (GameInteractor_Should(VB_HONEY_AND_DARLING_MINIGAME_FINISH,
                              (!DynaPolyActor_IsPlayerAbove((DynaPolyActor*)this->actor.child) &&
                               (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) ||
                                  (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] <= SECONDS_TO_TIMER(0)) ||
                                  (this->unk_548 == this->unk_54C),
                              this)) {
        player->stateFlags3 &= ~PLAYER_STATE3_400000;
        func_80961E88(play);
        player->stateFlags1 |= PLAYER_STATE1_20;
        if (this->unk_548 < this->unk_54C) {
            if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] == SECONDS_TO_TIMER(0)) {
                Message_StartTextbox(play, 0x2885, &this->actor);
                this->unk_552 = 0x2885;
            } else {
                Message_StartTextbox(play, 0x2888, &this->actor);
                this->unk_552 = 0x2888;
            }
            Audio_StopSubBgm();
            gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] = SECONDS_TO_TIMER(0);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
            this->unk_548 = 0;
            func_809632D0(this);
        } else {
            this->unk_548 = 0;
            Audio_StopSubBgm();
            gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] = SECONDS_TO_TIMER(0);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
            // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't seem to do
            // anything bad.
            Audio_PlayFanfare(NA_BGM_GET_ITEM);
            Interface_SetPerfectLetters(play, PERFECT_LETTERS_TYPE_1);
            this->unk_54A = 3;
            func_809632D0(this);
        }
    }
    func_80962EBC(this, play);
}

void func_80963258(EnFu* this) {
    this->actionFunc = func_8096326C;
}

void func_8096326C(EnFu* this, PlayState* play) {
    func_80963FF8(this, play);
    if (EnFu_MovePlayerToPos(play, this->actor.world.pos)) {
        func_809622FC(this);
    }
}

void func_809632D0(EnFu* this) {
    if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
        Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    }

    CLEAR_WEEKEVENTREG(WEEKEVENTREG_08_01);

    if (this->unk_2D4 != NULL) {
        BgFuMizu* mizu = this->unk_2D4;

        mizu->unk_160 = 0;
    }

    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = func_80963350;
}

void func_80963350(EnFu* this, PlayState* play) {
    static s32 D_80964C24 = 0;
    BgFuKaiten* fuKaiten = (BgFuKaiten*)this->actor.child;

    if ((this->unk_54A == 0) &&
        (((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) ||
         ((Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) && (play->msgCtx.stateTimer == 1)))) {
        Message_CloseTextbox(play);
        this->unk_54A = 2;
        D_80964C24 = 1;
    }

    if ((this->unk_54A == 3) && !play->interfaceCtx.perfectLettersOn) {
        this->unk_54A = 2;
        D_80964C24 = 1;
    }

    if ((fuKaiten->rotationSpeed != 0) || (fuKaiten->bounceSpeed != 0) || (fuKaiten->bounceHeight > 0.0f) ||
        !func_809638F8(play)) {
        Math_SmoothStepToS(&fuKaiten->rotationSpeed, 0, 10, 10, 5);
        Math_SmoothStepToS(&fuKaiten->bounceSpeed, 0, 10, 15, 5);
        Math_SmoothStepToF(&fuKaiten->bounceHeight, 0.0f, 0.1f, 1.0f, 1.0f);
        Math_SmoothStepToF(&fuKaiten->elevation, 0.0f, 0.1f, 1.0f, 1.0f);
        func_80962EBC(this, play);
    } else if (D_80964C24 == 1) {
        D_80964C24 = 0;
        fuKaiten->bounce = 0;
        func_80963F88(this, play);
        play->actorCtx.isOverrideInputOn = true;
        func_80963258(this);
    }
}

void func_80963540(EnFu* this) {
    this->actor.child->freezeTimer = 10;
    this->actionFunc = func_80963560;
}

void func_80963560(EnFu* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        func_80963610(this);
    } else if ((this->unk_552 == 0x2880) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_HONEY_AND_DARLING_HEART_PIECE)) {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 500.0f, 100.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_PURPLE, 500.0f, 100.0f);
    }
    this->actor.child->freezeTimer = 10;
}

void func_80963610(EnFu* this) {
    this->actor.child->freezeTimer = 10;
    this->actionFunc = func_80963630;
}

void func_80963630(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_22_10) && CHECK_WEEKEVENTREG(WEEKEVENTREG_22_20) && (CURRENT_DAY == 3) &&
            (GET_PLAYER_FORM == PLAYER_FORM_HUMAN)) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_22_40)) {
                Message_StartTextbox(play, 0x2884, &this->actor);
                this->unk_552 = 0x2884;
            } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_HONEY_AND_DARLING_HEART_PIECE)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_HONEY_AND_DARLING_HEART_PIECE);
                Message_StartTextbox(play, 0x2882, &this->actor);
                this->unk_552 = 0x2882;
            } else {
                Message_StartTextbox(play, 0x2881, &this->actor);
                this->unk_552 = 0x2881;
            }
        } else {
            Message_StartTextbox(play, 0x287F, &this->actor);
            this->unk_552 = 0x287F;
        }

        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actor.child->freezeTimer = 0;
        func_809628BC(this);

        if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) {
            switch (CURRENT_DAY) {
                case 1:
                    SET_WEEKEVENTREG(WEEKEVENTREG_22_10);
                    break;

                case 2:
                    SET_WEEKEVENTREG(WEEKEVENTREG_22_20);
                    break;

                case 3:
                    SET_WEEKEVENTREG(WEEKEVENTREG_22_40);
                    break;

                default:
                    break;
            }
        }
        player->stateFlags1 &= ~PLAYER_STATE1_20;
    } else {
        this->actor.child->freezeTimer = 10;
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 500.0f, PLAYER_IA_MINUS1);
    }
}

s32 EnFu_MovePlayerToPos(PlayState* play, Vec3f targetPos) {
    Player* player = GET_PLAYER(play);
    f32 distXZ;
    f32 controlStickMagnitude;
    s16 controlStickAngle;

    controlStickAngle = Math_Vec3f_Yaw(&player->actor.world.pos, &targetPos);
    distXZ = Math_Vec3f_DistXZ(&player->actor.world.pos, &targetPos);

    if (distXZ < 80.0f) {
        controlStickMagnitude = 10.0f;
    } else if (distXZ < 90.0f) {
        controlStickMagnitude = 40.0f;
    } else {
        controlStickMagnitude = 80.0f;
    }

    play->actorCtx.isOverrideInputOn = true;
    Actor_SetControlStickData(play, &play->actorCtx.overrideInput, controlStickMagnitude, controlStickAngle);

    if (distXZ < 80.0f) {
        return true;
    }
    return false;
}

s32 func_809638F8(PlayState* play) {
    s32 ret = true;

    if (play->envCtx.adjLightSettings.light1Color[0] > 25) {
        play->envCtx.adjLightSettings.light1Color[0] -= 25;
        ret = false;
    } else {
        play->envCtx.adjLightSettings.light1Color[0] = 0;
    }

    if (play->envCtx.adjLightSettings.light1Color[1] > 25) {
        play->envCtx.adjLightSettings.light1Color[1] -= 25;
        ret = false;
    } else {
        play->envCtx.adjLightSettings.light1Color[1] = 0;
    }

    if (play->envCtx.adjLightSettings.light1Color[2] > 25) {
        play->envCtx.adjLightSettings.light1Color[2] -= 25;
        ret = false;
    } else {
        play->envCtx.adjLightSettings.light1Color[2] = 0;
    }

    if (play->envCtx.adjLightSettings.ambientColor[0] > 25) {
        play->envCtx.adjLightSettings.ambientColor[0] -= 25;
        ret = false;
    } else {
        play->envCtx.adjLightSettings.ambientColor[0] = 0;
    }

    if (play->envCtx.adjLightSettings.ambientColor[1] > 25) {
        play->envCtx.adjLightSettings.ambientColor[1] -= 25;
        ret = false;
    } else {
        play->envCtx.adjLightSettings.ambientColor[1] = 0;
    }

    if (play->envCtx.adjLightSettings.ambientColor[2] > 25) {
        play->envCtx.adjLightSettings.ambientColor[2] -= 25;
        ret = false;
    } else {
        play->envCtx.adjLightSettings.ambientColor[2] = 0;
    }
    return ret;
}

void func_809639D0(EnFu* this, PlayState* play) {
    switch (CURRENT_DAY) {
        case 1:
            if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) {
                if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
                    Message_StartTextbox(play, 0x2853, &this->actor);
                    this->unk_552 = 0x2853;
                } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_22_10)) {
                    Message_StartTextbox(play, 0x284D, &this->actor);
                    this->unk_552 = 0x284D;
                } else if (this->unk_53E == 1) {
                    Message_StartTextbox(play, 0x284F, &this->actor);
                    this->unk_552 = 0x284F;
                } else {
                    this->unk_53E = 1;
                    Message_StartTextbox(play, 0x2851, &this->actor);
                    this->unk_552 = 0x2851;
                }
            } else {
                Message_StartTextbox(play, 0x286F, &this->actor);
                this->unk_552 = 0x286F;
            }
            break;

        case 2:
            if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
                Message_StartTextbox(play, 0x286F, &this->actor);
                this->unk_552 = 0x286F;
            } else if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
                Message_StartTextbox(play, 0x2853, &this->actor);
                this->unk_552 = 0x2853;
            } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_22_10)) {
                if (this->unk_53E == 1) {
                    Message_StartTextbox(play, 0x285B, &this->actor);
                    this->unk_552 = 0x285B;
                } else {
                    this->unk_53E = 1;
                    Message_StartTextbox(play, 0x285D, &this->actor);
                    this->unk_552 = 0x285D;
                }
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_22_20)) {
                Message_StartTextbox(play, 0x2855, &this->actor);
                this->unk_552 = 0x2855;
            } else if (this->unk_53E == 1) {
                Message_StartTextbox(play, 0x2857, &this->actor);
                this->unk_552 = 0x2857;
            } else {
                this->unk_53E = 1;
                Message_StartTextbox(play, 0x2859, &this->actor);
                this->unk_552 = 0x2859;
            }
            break;

        case 3:
            if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
                if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
                    func_80963EAC(this, play);
                } else {
                    Message_StartTextbox(play, 0x2841, &this->actor);
                    this->unk_552 = 0x2841;
                }
            } else if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
                Message_StartTextbox(play, 0x284B, &this->actor);
                this->unk_552 = 0x284B;
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_22_40)) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_22_10) && CHECK_WEEKEVENTREG(WEEKEVENTREG_22_20)) {
                    Message_StartTextbox(play, 0x285F, &this->actor);
                    this->unk_552 = 0x285F;
                } else {
                    Message_StartTextbox(play, 0x2861, &this->actor);
                    this->unk_552 = 0x2861;
                }
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_22_10) && CHECK_WEEKEVENTREG(WEEKEVENTREG_22_20)) {
                if (this->unk_53E == 1) {
                    Message_StartTextbox(play, 0x2863, &this->actor);
                    this->unk_552 = 0x2863;
                } else {
                    this->unk_53E = 1;
                    Message_StartTextbox(play, 0x2865, &this->actor);
                    this->unk_552 = 0x2865;
                }
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_22_10) || CHECK_WEEKEVENTREG(WEEKEVENTREG_22_20)) {
                if (this->unk_53E == 1) {
                    Message_StartTextbox(play, 0x2867, &this->actor);
                    this->unk_552 = 0x2867;
                } else {
                    this->unk_53E = 1;
                    Message_StartTextbox(play, 0x2869, &this->actor);
                    this->unk_552 = 0x2869;
                }
            } else if (this->unk_53E == 1) {
                Message_StartTextbox(play, 0x286B, &this->actor);
                this->unk_552 = 0x286B;
            } else {
                this->unk_53E = 1;
                Message_StartTextbox(play, 0x286D, &this->actor);
                this->unk_552 = 0x286D;
            }
            break;

        default:
            break;
    }
}

void func_80963DE4(EnFu* this, PlayState* play) {
    switch (this->unk_542) {
        case 0:
            if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
                Message_StartTextbox(play, 0x2875, &this->actor);
                this->unk_552 = 0x2875;
            } else {
                Message_StartTextbox(play, 0x2877, &this->actor);
                this->unk_552 = 0x2877;
            }
            break;

        case 1:
            Message_StartTextbox(play, 0x2879, &this->actor);
            this->unk_552 = 0x2879;
            break;

        case 2:
            Message_StartTextbox(play, 0x287B, &this->actor);
            this->unk_552 = 0x287B;
            break;

        default:
            break;
    }
}

void func_80963EAC(EnFu* this, PlayState* play) {
    if (gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
        if (this->unk_540 == 1) {
            Message_StartTextbox(play, 0x2847, &this->actor);
            this->unk_552 = 0x2847;
        } else {
            this->unk_540 = 1;
            Message_StartTextbox(play, 0x2845, &this->actor);
            this->unk_552 = 0x2845;
        }
    } else {
        Message_StartTextbox(play, 0x2843, &this->actor);
        this->unk_552 = 0x2843;
    }
}

void func_80963F44(EnFu* this, PlayState* play) {
    u16 sp1E = this->unk_552 + 1;

    Message_StartTextbox(play, sp1E, &this->actor);
    this->unk_552 = sp1E;
}

void func_80963F88(EnFu* this, PlayState* play) {
    if (this->unk_542 == 1) {
        Camera_ChangeSetting(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_HONEY_AND_DARLING_2);
        play->unk_1887E = 0;
    } else if (this->unk_542 == 2) {
        play->unk_1887D = 0;
        Camera_ChangeSetting(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_HONEY_AND_DARLING_2);
    }
}

void func_80963FF8(EnFu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & PLAYER_STATE1_100000) {
        play->actorCtx.isOverrideInputOn = true;
        play->actorCtx.overrideInput.press.button = BTN_A;
    } else {
        play->actorCtx.isOverrideInputOn = true;
    }
}

void func_80964034(EnFu* this, PlayState* play) {
    Vec3f sp2C;

    if (DECR(this->unk_54E) == 0) {
        this->unk_54E = 12;
        sp2C = this->actor.world.pos;
        sp2C.y += 62.0f;
        func_80964694(this, this->unk_2D8, &sp2C, ARRAY_COUNT(this->unk_2D8));
    }
    func_809647EC(play, this->unk_2D8, ARRAY_COUNT(this->unk_2D8));
}

void func_809640D8(EnFu* this, PlayState* play) {
    if ((this->actionFunc == func_80962A10) || (this->actionFunc == func_80962BCC) ||
        (this->actionFunc == func_80962D60) || (this->actionFunc == func_80962F4C) ||
        (this->actionFunc == func_80963350)) {
        this->actor.shape.rot.y += 0x400;
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void func_8096413C(EnFu* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, BINANG_SUB(this->actor.yawTowardsPlayer, 0x4000), 5, 0x3E8, 0x64);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void func_80964190(EnFu* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        switch (this->unk_552) {
            case 0x2842:
            case 0x2844:
            case 0x2848:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HONEY_DARLING_ANIM_CUP_CHEEKS);
                break;

            case 0x2840:
            case 0x2841:
            case 0x2843:
            case 0x2847:
            case 0x2849:
            case 0x284B:
            case 0x284D:
            case 0x284F:
            case 0x2851:
            case 0x2853:
            case 0x2855:
            case 0x2857:
            case 0x2859:
            case 0x285B:
            case 0x285D:
            case 0x2861:
            case 0x2863:
            case 0x2865:
            case 0x2867:
            case 0x2869:
            case 0x286B:
            case 0x286D:
            case 0x2871:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HONEY_DARLING_ANIM_HOLD_HANDS_MORPH);
                break;

            case 0x2860:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HONEY_DARLING_ANIM_HOLD_HANDS);
                break;

            case 0x285F:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HONEY_DARLING_ANIM_SURPRISE);
                break;

            case 0x287E:
            case 0x2880:
            case 0x2883:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, HONEY_DARLING_ANIM_HUG);
                break;

            default:
                break;
        }
    }
}

void func_8096426C(EnFu* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        switch (this->unk_552) {
            case 0x2840:
            case 0x2841:
            case 0x2843:
            case 0x2847:
            case 0x2849:
            case 0x284D:
            case 0x284F:
            case 0x2851:
            case 0x2853:
            case 0x2855:
            case 0x2857:
            case 0x2859:
            case 0x285B:
            case 0x285D:
            case 0x2861:
            case 0x2863:
            case 0x2865:
            case 0x2867:
            case 0x2869:
            case 0x286B:
            case 0x286D:
            case 0x2871:
            case 0x2875:
            case 0x2877:
            case 0x2879:
            case 0x287B:
            case 0x287D:
                this->unk_53C = 1;
                break;

            case 0x2848:
            case 0x284A:
            case 0x284E:
            case 0x2850:
            case 0x2852:
            case 0x2856:
            case 0x2858:
            case 0x285A:
            case 0x285C:
            case 0x285E:
            case 0x2860:
            case 0x2862:
            case 0x2864:
            case 0x2866:
            case 0x2868:
            case 0x286A:
            case 0x286C:
            case 0x286E:
                this->unk_53C = 2;
                break;

            default:
                this->unk_53C = 0;
                break;
        }
    }
}

void func_809642E0(EnFu* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    if (this->unk_542 == 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnFu_Update(Actor* thisx, PlayState* play) {
    EnFu* this = (EnFu*)thisx;

    this->actionFunc(this, play);

    func_809642E0(this, play);
    Actor_MoveWithGravity(&this->actor);
    func_8096209C(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    SkelAnime_Update(&this->skelAnime);
    func_80961D7C(play);
    func_809640D8(this, play);
    func_80964034(this, play);
}

s32 EnFu_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnFu* this = (EnFu*)thisx;

    if (limbIndex == HONEY_AND_DARLING_LIMB_MAN_HEAD) {
        Matrix_Translate(1600.0f, 300.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_524[1], MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_524[0], MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_524[2], MTXMODE_APPLY);
        Matrix_Translate(-1600.0f, -300.0f, 0.0f, MTXMODE_APPLY);
    } else if (limbIndex == HONEY_AND_DARLING_LIMB_WOMAN_HEAD) {
        Matrix_Translate(1800.0f, 200.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_52A[1], MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_52A[0], MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_52A[2], MTXMODE_APPLY);
        Matrix_Translate(-1800.0f, -200.0f, 0.0f, MTXMODE_APPLY);
    }
    return false;
}

void EnFu_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80964C28 = { -2500.0f, 0.0f, 0.0f };
    static Vec3f D_80964C34 = { -3500.0f, 0.0f, 0.0f };
    EnFu* this = (EnFu*)thisx;

    if (limbIndex == HONEY_AND_DARLING_LIMB_MAN_HEAD) {
        Matrix_MultVec3f(&D_80964C28, &this->unk_508);
    } else if (limbIndex == HONEY_AND_DARLING_LIMB_WOMAN_HEAD) {
        Matrix_MultVec3f(&D_80964C34, &this->unk_514);
    }
}

void EnFu_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFu* this = (EnFu*)thisx;

    Matrix_Push();
    func_80964950(play, this->unk_2D8, ARRAY_COUNT(this->unk_2D8));
    Matrix_Pop();

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_EnvColor(play->state.gfxCtx, 0, 50, 160, 0));
    gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_EnvColor(play->state.gfxCtx, 255, 255, 255, 0));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnFu_OverrideLimbDraw, EnFu_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80964694(EnFu* this, EnFuUnkStruct* ptr, Vec3f* arg2, s32 len) {
    Vec3f sp2C = { 0.0f, 1.5f, 0.0f };
    Vec3f sp20 = { 0.0f, 0.0f, 0.0f };
    s32 i = 0;

    while ((i < len) && (ptr->unk_36 != 0)) {
        i++;
        ptr++;
    }

    if (i < len) {
        ptr->unk_36 = 1;
        ptr->unk_08 = *arg2;
        ptr->unk_20 = sp2C;
        ptr->unk_14 = sp20;
        ptr->unk_00 = 0.01f;

        ptr->unk_08.x += 4.0f * Math_SinS(this->actor.shape.rot.y);
        ptr->unk_08.z += 4.0f * Math_CosS(this->actor.shape.rot.y);
        ptr->unk_37 = 16;
    }
}

void func_809647EC(PlayState* play, EnFuUnkStruct* ptr, s32 len) {
    Vec3f sp44 = { 0.0f, 0.0f, 0.0f };
    s16 yaw = Camera_GetInputDirYaw(GET_ACTIVE_CAM(play));
    s32 i;

    for (i = 0; i < len; i++, ptr++) {
        if (ptr->unk_36 == 1) {
            if (DECR(ptr->unk_37) == 0) {
                ptr->unk_36 = false;
            }
            ptr->unk_08.y += ptr->unk_20.y;
            ptr->unk_08.x += 2.0f * Math_SinS(ptr->unk_2C);
            ptr->unk_08.z += 2.0f * Math_CosS(ptr->unk_2C);
            Matrix_Push();
            Matrix_Translate(ptr->unk_08.x, ptr->unk_08.y, ptr->unk_08.z, MTXMODE_NEW);
            Matrix_RotateYS(yaw, MTXMODE_APPLY);
            Matrix_MultVec3f(&sp44, &ptr->unk_08);
            Matrix_Pop();
            ptr->unk_2C += 6000;
        }
    }
}

void func_80964950(PlayState* play, EnFuUnkStruct* ptr, s32 len) {
    s32 i;
    s32 flag = false;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

    for (i = 0; i < len; i++, ptr++) {
        if (ptr->unk_36 == 1) {
            FrameInterpolation_RecordOpenChild(ptr, i);
            if (!flag) {
                gSPDisplayList(POLY_OPA_DISP++, gHoneyAndDarlingHeartMaterialDL);
                flag = true;
            }
            Matrix_Translate(ptr->unk_08.x, ptr->unk_08.y, ptr->unk_08.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(ptr->unk_00, ptr->unk_00, ptr->unk_00, MTXMODE_APPLY);

            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gDropRecoveryHeartTex));
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gHoneyAndDarlingHeartModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
