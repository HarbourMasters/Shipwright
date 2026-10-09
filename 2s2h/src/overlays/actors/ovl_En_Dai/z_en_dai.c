/*
 * File: z_en_dai.c
 * Overlay: ovl_En_Dai
 * Description: Biggoron
 */

#include "z_en_dai.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnDai_Init(Actor* thisx, PlayState* play);
void EnDai_Destroy(Actor* thisx, PlayState* play);
void EnDai_Update(Actor* thisx, PlayState* play);
void EnDai_Draw(Actor* thisx, PlayState* play);

void func_80B3F00C(EnDai* this, PlayState* play);
void func_80B3EF90(EnDai* this, PlayState* play);

ActorProfile En_Dai_Profile = {
    /**/ ACTOR_EN_DAI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DAI,
    /**/ sizeof(EnDai),
    /**/ EnDai_Init,
    /**/ EnDai_Destroy,
    /**/ EnDai_Update,
    /**/ EnDai_Draw,
};

static Vec3f D_80B3FBF0 = { 1.0f, 1.0f, 1.0f };

EnDaiEffect* func_80B3DFF0(EnDaiEffect* effect, Vec3f arg1, Vec3f arg2, Vec3f arg3, f32 arg4, f32 arg5, s32 arg6) {
    s32 i;

    for (i = 0; i < EN_DAI_EFFECT_COUNT; i++, effect++) {
        if (!effect->isEnabled) {
            effect->isEnabled = true;
            effect->unk_01 = (Rand_ZeroOne() * (2.0f * (arg6 / 3.0f))) + (arg6 / 3.0f);
            effect->unk_02 = effect->unk_01;
            effect->unk_10 = arg1;
            effect->unk_1C = arg2;
            effect->unk_28 = arg3;
            effect->unk_34 = arg4;
            effect->unk_38 = arg5;
            return effect;
        }
    }

    return NULL;
}

void func_80B3E168(EnDaiEffect* effect, PlayState* play2) {
    PlayState* play = play2;
    s32 pad;
    s32 isDisplayListSet = false;
    s32 i;
    f32 alpha;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < EN_DAI_EFFECT_COUNT; i++, effect++) {
        if (effect->isEnabled == true) {
            FrameInterpolation_RecordOpenChild(effect, 0);
            gDPPipeSync(POLY_XLU_DISP++);

            if (!isDisplayListSet) {
                gSPDisplayList(POLY_XLU_DISP++, object_dai_DL_000230);
                isDisplayListSet = true;
            }

            Matrix_Push();

            alpha = (effect->unk_02 / (f32)effect->unk_01);
            alpha *= 255.0f;

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (u8)alpha);

            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, (effect->unk_02 + (i * 3)) * 3,
                                          (effect->unk_02 + (i * 3)) * 15, 0x20, 0x40, 1, 0, 0, 0x20, 0x20, 3, 15, 0,
                                          0));

            Matrix_Translate(effect->unk_10.x, effect->unk_10.y, effect->unk_10.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->unk_34, effect->unk_34, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, object_dai_DL_0002E8);

            Matrix_Pop();
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 func_80B3E460(EnDai* this) {
    EnDaiEffect* effect = this->effects;
    s32 i;
    s32 count;

    for (i = 0, count = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled && effect->unk_02) {
            effect->unk_10.x += effect->unk_28.x;
            effect->unk_02--;
            effect->unk_10.y += effect->unk_28.y;
            effect->unk_10.z += effect->unk_28.z;
            effect->unk_28.x += effect->unk_1C.x;
            effect->unk_28.y += effect->unk_1C.y;
            effect->unk_28.z += effect->unk_1C.z;
            effect->unk_34 += effect->unk_38;
            count++;
        } else if (effect->isEnabled) {
            effect->isEnabled = false;
        }
    }

    return count;
}

s32 func_80B3E5B4(EnDai* this, PlayState* play) {
    func_80B3E168(this->effects, play);
    return 0;
}

typedef enum {
    /* -1 */ ENDAI_ANIM_NONE = -1,
    /*  0 */ ENDAI_ANIM_0,
    /*  1 */ ENDAI_ANIM_1,
    /*  2 */ ENDAI_ANIM_2,
    /*  3 */ ENDAI_ANIM_3,
    /*  4 */ ENDAI_ANIM_4,
    /*  5 */ ENDAI_ANIM_5,
    /*  6 */ ENDAI_ANIM_6,
    /*  7 */ ENDAI_ANIM_7,
    /*  8 */ ENDAI_ANIM_8,
    /*  9 */ ENDAI_ANIM_MAX
} EnDaiAnimation;

static AnimationInfoS sAnimationInfo[ENDAI_ANIM_MAX] = {
    { &object_dai_Anim_0079E4, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENDAI_ANIM_0
    { &object_dai_Anim_0079E4, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENDAI_ANIM_1
    { &object_dai_Anim_007354, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDAI_ANIM_2
    { &object_dai_Anim_000CEC, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDAI_ANIM_3
    { &object_dai_Anim_0069DC, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDAI_ANIM_4
    { &object_dai_Anim_00563C, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },  // ENDAI_ANIM_5
    { &object_dai_Anim_00563C, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDAI_ANIM_6
    { &object_dai_Anim_002E58, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENDAI_ANIM_7
    { &object_dai_Anim_006590, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENDAI_ANIM_8
};

s32 EnDai_ChangeAnim(EnDai* this, s32 animIndex) {
    s32 changeAnim = false;
    s32 didAnimChange = false;

    switch (animIndex) {
        case ENDAI_ANIM_0:
        case ENDAI_ANIM_1:
            if ((this->animIndex != ENDAI_ANIM_0) && (this->animIndex != ENDAI_ANIM_1)) {
                changeAnim = true;
            }
            break;

        case ENDAI_ANIM_5:
        case ENDAI_ANIM_6:
            if ((this->animIndex != ENDAI_ANIM_5) && (this->animIndex != ENDAI_ANIM_6)) {
                changeAnim = true;
            }
            break;

        default:
            if (this->animIndex != animIndex) {
                changeAnim = true;
            }
            break;
    }

    if (changeAnim) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
    }

    return didAnimChange;
}

s32 func_80B3E69C(EnDai* this, PlayState* play) {
    s32 ret = false;

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->sceneId == SCENE_12HAKUGINMAE) &&
        (play->csCtx.scriptIndex == 0) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_30_01)) {
        if (!(this->unk_1CE & 0x10)) {
            Flags_SetSwitch(play, 20);
            this->unk_1CE |= (0x80 | 0x10);
            this->unk_1CE &= ~(0x100 | 0x20);
            this->cueId = 255;
            this->unk_1DC = 0;
            this->unk_1CD = 0;
            this->unk_1F0 = D_80B3FBF0;
            this->unk_1FC = D_80B3FBF0;
        }
        ret = true;
    } else if (this->unk_1CE & 0x10) {
        this->unk_1CE &= ~0x10;
        this->unk_1CE |= 0x200;
        SET_WEEKEVENTREG(WEEKEVENTREG_30_01);
        this->actionFunc = func_80B3F00C;
    }

    return ret;
}

s32 func_80B3E7C8(EnDai* this, PlayState* play) {
    s32 ret = false;

    if (((this->unk_1CE & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        SubS_SetOfferMode(&this->unk_1CE, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->actionFunc = func_80B3EF90;
        ret = true;
    }

    return ret;
}

void func_80B3E834(EnDai* this) {
    s16 phi_v1;

    if (this->unk_1CE & 0x80) {
        if (this->unk_1D8 == 0) {
            phi_v1 = 0;
        } else {
            this->unk_1D8--;
            phi_v1 = this->unk_1D8;
        }

        if (phi_v1 == 0) {
            this->unk_1D6++;
            if (this->unk_1D6 >= 5) {
                this->unk_1D8 = Rand_S16Offset(20, 20);
                this->unk_1D6 = 0;
            }
        }
    }
}

static MsgScript sMsgScript[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_GORON(0x0009 - 0x0003),
    /* 0x0003 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C90),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0008 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0009 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_85_20, 0x001F - 0x000E),
    /* 0x000E 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C91),
    /* 0x0011 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0012 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0C92),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x0C93),
    /* 0x0019 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001A 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x001B 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_85_20),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x001F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x0C94),
    /* 0x0022 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0023 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0024 0x01 */ MSCRIPT_CMD_DONE(),
};

static f32 D_80B3FCB4[] = {
    1.0f, 6.0f, 16.0f, 19.0f, 46.0f, 48.0f, 50.0f, 52.0f, 54.0f, 1.0f, 6.0f, 36.0f,
};

static f32 D_80B3FCE4[] = {
    5.0f, 15.0f, 18.0f, 45.0f, 47.0f, 49.0f, 51.0f, 53.0f, 55.0f, 5.0f, 35.0f, 45.0f,
};

static s16 D_80B3FD14[] = {
    4, 5, 2, 0, 4, 5, 2, 5, 4, 4, 5, 2,
};

s16 func_80B3E8BC(EnDai* this, s32 cueId) {
    s32 i;
    s32 end;

    if (cueId == 3) {
        end = 9;
        i = 0;
    } else if (cueId == 4) {
        end = 12;
        i = 9;
    } else {
        return 2;
    }

    while (i < end) {
        if ((this->skelAnime.curFrame >= D_80B3FCB4[i]) && (this->skelAnime.curFrame <= D_80B3FCE4[i])) {
            return D_80B3FD14[i];
        }
        i++;
    }

    return this->unk_1D6;
}

s32 func_80B3E96C(EnDai* this, PlayState* play) {
    Vec3f sp74;
    Vec3f sp68;
    Vec3f sp5C;
    Vec3f sp50;

    if ((DECR(this->unk_1DA) == 0) &&
        ((this->unk_1DC == 0) || Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame))) {
        switch (this->unk_1DC) {
            case 0:
                EnDai_ChangeAnim(this, ENDAI_ANIM_2);
                this->unk_1DC++;
                break;

            case 1:
                Actor_PlaySfx(&this->actor, NA_SE_EV_SNOWSTORM_HARD);
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WEATHER_TAG, this->actor.world.pos.x,
                            this->actor.world.pos.y, this->actor.world.pos.z, 0x1388, 0x708, 0x3E8, 0);
                EnDai_ChangeAnim(this, ENDAI_ANIM_3);
                this->unk_1DC++;
                break;

            case 2:
                EnDai_ChangeAnim(this, ENDAI_ANIM_4);
                this->unk_1DC++;
                break;

            case 3:
                EnDai_ChangeAnim(this, ENDAI_ANIM_1);
                this->unk_1F0 = D_80B3FBF0;
                this->unk_1FC = D_80B3FBF0;
                this->unk_1DC = 0;
                this->unk_1DA = Rand_S16Offset(20, 20);
                break;

            default:
                break;
        }
    } else if (this->unk_1DC == 2) {
        sp50.x = sp50.y = 0.0f;
        sp50.z = 200.0f;

        sp68.x = sp68.z = 0.0f;
        sp68.y = 6.0f;

        sp5C.x = Math_SinS(this->unk_1D4) * 80.0f;
        sp5C.z = Math_CosS(this->unk_1D4) * 80.0f;
        sp5C.y = -40.0f;

        Lib_Vec3f_TranslateAndRotateY(&this->unk_1E4, this->unk_1D4, &sp50, &sp74);
        func_80B3DFF0(this->effects, sp74, sp68, sp5C, 0.03f, 0.04f, 0x10);
    }

    return 0;
}

void func_80B3EC10(f32 arg0, Vec3f arg1, f32 arg2, Vec3f arg3, f32 arg4, Vec3f* arg5) {
    f32 temp_f0 = (arg4 - arg0) / (arg2 - arg0);

    arg5->x = ((arg3.x - arg1.x) * temp_f0) + arg1.x;
    arg5->y = ((arg3.y - arg1.y) * temp_f0) + arg1.y;
    arg5->z = ((arg3.z - arg1.z) * temp_f0) + arg1.z;
}

static f32 D_80B3FD2C[] = {
    6.0f, 11.0f, 15.0f, 0.0f, 12.0f, 17.0f, 0.0f, 3.0f,
};

static f32 D_80B3FD4C[] = {
    8.0f, 15.0f, 24.0f, 4.0f, 17.0f, 26.0f, 3.0f, 7.0f,
};

static s16 D_80B3FD6C[] = {
    1, 1, 1, 2, 2, 2, 3, 3,
};

static Vec3f D_80B3FD7C[] = {
    { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.8f, 0.8f }, { 1.0f, 1.1f, 1.1f }, { 1.0f, 1.3f, 1.3f }, { 1.0f, 0.7f, 0.9f },
    { 1.0f, 0.8f, 0.9f }, { 1.0f, 0.7f, 0.9f }, { 1.0f, 0.8f, 0.9f }, { 1.0f, 1.0f, 1.0f },
};

s32 func_80B3EC84(EnDai* this) {
    s32 i;
    s32 ret = false;

    for (i = 0; i < ARRAY_COUNT(D_80B3FD6C); i++) {
        if ((this->unk_1DC == D_80B3FD6C[i]) && (this->skelAnime.curFrame > D_80B3FD2C[i]) &&
            (this->skelAnime.curFrame <= D_80B3FD4C[i])) {
            func_80B3EC10(D_80B3FD2C[i], D_80B3FD7C[i], D_80B3FD4C[i], D_80B3FD7C[i + 1], this->skelAnime.curFrame,
                          &this->unk_1F0);
            ret = true;
            break;
        }
    }

    return ret;
}

static f32 D_80B3FDE8[] = { 22.0f, 0.0f, 0.0f };

static f32 D_80B3FDF4[] = { 29.0f, 3.0f, 9.0f };

static s16 D_80B3FE00[] = { 1, 2, 3 };

static Vec3f D_80B3FE08[] = {
    { 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.2f, 1.2f },
    { 1.0f, 0.7f, 0.8f },
    { 1.0f, 1.0f, 1.0f },
};

s32 func_80B3ED88(EnDai* this) {
    s32 i;
    s32 ret = false;

    for (i = 0; i < ARRAY_COUNT(D_80B3FE00); i++) {
        if ((this->unk_1DC == D_80B3FE00[i]) && (D_80B3FDE8[i] < this->skelAnime.curFrame) &&
            (D_80B3FDF4[i] >= this->skelAnime.curFrame)) {
            func_80B3EC10(D_80B3FDE8[i], D_80B3FE08[i], D_80B3FDF4[i], D_80B3FE08[i + 1], this->skelAnime.curFrame,
                          &this->unk_1FC);
            ret = true;
            break;
        }
    }

    return ret;
}

void func_80B3EE8C(EnDai* this, PlayState* play) {
    s16 csId = this->actor.csId;

    if (CutsceneManager_IsNext(csId)) {
        CutsceneManager_StartWithPlayerCs(csId, &this->actor);
    } else {
        CutsceneManager_Queue(csId);
    }
}

void func_80B3EEDC(EnDai* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (GameInteractor_Should(VB_OPEN_SNOWHEAD_FROM_SONG,
                              (player->transformation == PLAYER_FORM_GORON) &&
                                  (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
                                  (play->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY),
                              this)) {
        EnDai_ChangeAnim(this, ENDAI_ANIM_1);
        this->actionFunc = func_80B3EE8C;
    } else if (!(player->stateFlags2 & PLAYER_STATE2_USING_OCARINA)) {
        func_80B3E96C(this, play);
        this->unk_A6C = 0;
    } else if (this->unk_A6C == 0) {
        Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        this->unk_A6C = 1;
    }
}

void func_80B3EF90(EnDai* this, PlayState* play) {
    if (MsgEvent_RunScript(&this->actor, play, sMsgScript, NULL, &this->msgScriptPos)) {
        SubS_SetOfferMode(&this->unk_1CE, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->msgScriptPos = 0;
        this->actionFunc = func_80B3F00C;
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0x2AA8);
    }
}

void func_80B3F00C(EnDai* this, PlayState* play) {
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x2AA8);
}

void EnDai_HandleCutscene(EnDai* this, PlayState* play) {
    static s32 sCsAnimIndex[] = {
        ENDAI_ANIM_0, ENDAI_ANIM_0, ENDAI_ANIM_6, ENDAI_ANIM_7, ENDAI_ANIM_8,
    };
    s32 cueChannel = 0;
    s32 cueId;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_472)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_472);
        cueId = play->csCtx.actorCues[cueChannel]->id;
        if (this->cueId != (u8)cueId) {
            EnDai_ChangeAnim(this, sCsAnimIndex[cueId]);
            switch (cueId) {
                case 1:
                    this->unk_1E0 = 0.0f;
                    break;

                case 2:
                    Actor_PlaySfx(&this->actor, NA_SE_EV_ROLL_AND_FALL);
                    break;

                case 3:
                    this->unk_1CE &= ~0x80;
                    break;

                default:
                    break;
            }
        }
        this->cueId = cueId;
    }

    switch (this->cueId) {
        case 1:
            this->unk_1E0 += 1.0f;
            if (this->unk_1E0 < 30.0f) {
                this->unk_1CD = (this->unk_1E0 / 30.0f) * 255.0f;
            } else {
                this->unk_1CD = 0xFF;
            }
            break;

        case 2:
            if (play->csCtx.curFrame == 360) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_DAIGOLON_SLEEP3 - SFX_FLAG);
            }
            if (Animation_OnFrame(&this->skelAnime, 43.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_GORON_BOUND_1);
            }
            this->unk_1D6 = func_80B3E8BC(this, this->cueId);
            break;

        case 3:
            if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_DAIGOLON_SLEEP1);
            }
            this->unk_1D6 = func_80B3E8BC(this, this->cueId);
            break;

        case 4:
            if (Animation_OnFrame(&this->skelAnime, 30.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_DAIGOLON_SLEEP2);
            }
            if (Animation_OnFrame(&this->skelAnime, 35.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_GORON_BOUND_0);
            }
            this->unk_1D6 = func_80B3E8BC(this, this->cueId);
            break;

        default:
            break;
    }

    Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
}

void EnDai_Init(Actor* thisx, PlayState* play) {
    EnDai* this = (EnDai*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_dai_Skel_0130D0, NULL, this->jointTable, this->morphTable,
                       OBJECT_DAI_LIMB_MAX);
    this->animIndex = ENDAI_ANIM_NONE;
    EnDai_ChangeAnim(this, ENDAI_ANIM_0);
    Actor_SetScale(&this->actor, 0.2f);
    this->actor.attentionRangeType = ATTENTION_RANGE_10;
    this->unk_1F0 = D_80B3FBF0;
    this->unk_1FC = D_80B3FBF0;
    this->unk_1CE = 0;
    this->unk_1D6 = 0;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
        SubS_SetOfferMode(&this->unk_1CE, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_1CE |= 0x80;
        this->unk_1CD = 0xFF;
        this->actionFunc = func_80B3F00C;
        return;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_30_01)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->unk_1CD = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_1CE |= (0x100 | 0x20);
    this->unk_1CE |= 0x80;
    this->actionFunc = func_80B3EEDC;
}

void EnDai_Destroy(Actor* thisx, PlayState* play) {
}

void EnDai_Update(Actor* thisx, PlayState* play) {
    EnDai* this = (EnDai*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (!func_80B3E7C8(this, play) && func_80B3E69C(this, play)) {
        EnDai_HandleCutscene(this, play);
        SkelAnime_Update(&this->skelAnime);
        func_80B3E834(this);
        func_80B3E460(this);
    } else {
        this->actionFunc(this, play);
        if (!(player->stateFlags2 & PLAYER_STATE2_USING_OCARINA)) {
            SkelAnime_Update(&this->skelAnime);
            func_80B3E834(this);
            if (!(this->unk_1CE & 0x200)) {
                SubS_Offer(&this->actor, play, 0.0f, 0.0f, PLAYER_IA_NONE, this->unk_1CE & SUBS_OFFER_MODE_MASK);
            }
            func_80B3E460(this);
        }
    }
}

s32 EnDai_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                           Gfx** gfx) {
    EnDai* this = (EnDai*)thisx;

    if (!(this->unk_1CE & 0x40)) {
        *dList = NULL;
    }

    if (limbIndex == OBJECT_DAI_LIMB_0B) {
        Matrix_MultVec3f(&gZeroVec3f, &this->unk_1E4);
    }

    if (limbIndex == OBJECT_DAI_LIMB_0A) {
        *dList = NULL;
    }

    return false;
}

void EnDai_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    static Vec3f D_80B3FE4C = { 0.0f, 0.0f, 0.0f };

    EnDai* this = (EnDai*)thisx;
    Vec3s sp64;
    MtxF sp24;

    switch (limbIndex) {
        case OBJECT_DAI_LIMB_0B:
            D_80B3FE4C.x = sREG(0);
            D_80B3FE4C.y = sREG(1);
            D_80B3FE4C.z = sREG(2);
            Matrix_MultVec3f(&D_80B3FE4C, &this->actor.focus.pos);
            Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);
            Matrix_Get(&sp24);
            Matrix_MtxFToYXZRot(&sp24, &sp64, false);
            this->unk_1D4 = BINANG_SUB(sp64.y, 0x4000);
            break;

        case OBJECT_DAI_LIMB_0A:
            Matrix_Get(&this->unk_18C);
            break;

        default:
            break;
    }
}

void EnDai_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx, Gfx** gfx) {
    EnDai* this = (EnDai*)thisx;

    switch (limbIndex) {
        case OBJECT_DAI_LIMB_09:
            if (this->unk_1CE & 0x100) {
                func_80B3EC84(this);
            }
            Matrix_Scale(this->unk_1F0.x, this->unk_1F0.y, this->unk_1F0.z, MTXMODE_APPLY);
            break;

        case OBJECT_DAI_LIMB_02:
            if (this->unk_1CE & 0x100) {
                func_80B3ED88(this);
            }
            Matrix_Scale(this->unk_1FC.x, this->unk_1FC.y, this->unk_1FC.z, MTXMODE_APPLY);
            break;

        default:
            break;
    }
}

void func_80B3F78C(EnDai* this, PlayState* play) {
    static TexturePtr D_80B3FE58[] = {
        object_dai_Tex_0107B0, object_dai_Tex_010FB0, object_dai_Tex_0117B0,
        object_dai_Tex_010FB0, object_dai_Tex_011FB0, object_dai_Tex_0127B0,
    };
    s32 pad;

    if (play->actorCtx.lensActorsDrawn) {
        this->unk_1CE |= 0x40;
    } else {
        Actor_AddToLensActors(play, &this->actor);
        this->unk_1CE &= ~0x40;
    }

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    Scene_SetRenderModeXlu(play, 2, 2);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(D_80B3FE58[this->unk_1D6]));

    POLY_XLU_DISP = SubS_DrawTransformFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, EnDai_OverrideLimbDraw, EnDai_PostLimbDraw,
                                           EnDai_TransformLimbDraw, &this->actor, POLY_XLU_DISP);
    if (this->unk_1CE & 0x40) {
        Matrix_Put(&this->unk_18C);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, object_dai_DL_00C538);
    }

    func_80B3E5B4(this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80B3F920(EnDai* this, PlayState* play) {
    static TexturePtr D_80B3FE70[] = {
        object_dai_Tex_0107B0, object_dai_Tex_010FB0, object_dai_Tex_0117B0,
        object_dai_Tex_010FB0, object_dai_Tex_011FB0, object_dai_Tex_0127B0,
    };
    s32 pad;

    this->unk_1CE |= 0x40;
    if (this->unk_1CD == 0xFF) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        OPEN_DISPS(play->state.gfxCtx);

        Scene_SetRenderModeXlu(play, 0, 1);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80B3FE70[this->unk_1D6]));

        POLY_OPA_DISP = SubS_DrawTransformFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                               this->skelAnime.dListCount, EnDai_OverrideLimbDraw, EnDai_PostLimbDraw,
                                               EnDai_TransformLimbDraw, &this->actor, POLY_OPA_DISP);
        Matrix_Put(&this->unk_18C);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_dai_DL_00C538);

        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        OPEN_DISPS(play->state.gfxCtx);

        Scene_SetRenderModeXlu(play, 2, 2);

        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->unk_1CD);
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(D_80B3FE70[this->unk_1D6]));

        POLY_XLU_DISP = SubS_DrawTransformFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                               this->skelAnime.dListCount, EnDai_OverrideLimbDraw, EnDai_PostLimbDraw,
                                               EnDai_TransformLimbDraw, &this->actor, POLY_XLU_DISP);
        Matrix_Put(&this->unk_18C);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, object_dai_DL_00C538);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    func_80B3E5B4(this, play);
}

void EnDai_Draw(Actor* thisx, PlayState* play) {
    EnDai* this = (EnDai*)thisx;

    if (!(this->unk_1CE & 0x200)) {
        if (this->unk_1CE & 0x20) {
            func_80B3F78C(this, play);
        } else {
            func_80B3F920(this, play);
        }
    }
}
