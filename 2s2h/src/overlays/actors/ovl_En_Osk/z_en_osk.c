/*
 * File: z_en_osk.c
 * Overlay: ovl_En_Osk
 * Description: Igos du Ikana's and his lackey's floating heads
 */

#include "z_en_osk.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnOsk_Init(Actor* thisx, PlayState* play);
void EnOsk_Destroy(Actor* thisx, PlayState* play);
void EnOsk_Update(Actor* thisx, PlayState* play);
void EnOsk_Draw(Actor* thisx, PlayState* play);

void func_80BF5F60(EnOsk* this, PlayState* play);
void func_80BF61EC(EnOsk* this, PlayState* play);
void func_80BF656C(EnOsk* this, PlayState* play);
void func_80BF6A20(EnOsk* this, PlayState* play);

ActorProfile En_Osk_Profile = {
    /**/ ACTOR_EN_OSK,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_IKN_DEMO,
    /**/ sizeof(EnOsk),
    /**/ EnOsk_Init,
    /**/ EnOsk_Destroy,
    /**/ EnOsk_Update,
    /**/ EnOsk_Draw,
};

typedef enum OskType0Animation {
    /* 0 */ OSK_TYPE0_ANIM_0,
    /* 1 */ OSK_TYPE0_ANIM_1,
    /* 2 */ OSK_TYPE0_ANIM_2,
    /* 3 */ OSK_TYPE0_ANIM_3,
    /* 4 */ OSK_TYPE0_ANIM_4,
    /* 5 */ OSK_TYPE0_ANIM_MAX
} OskType0Animation;

AnimationHeader* sAnimationsType0[OSK_TYPE0_ANIM_MAX] = {
    &object_ikn_demo_Anim_003CAC, // OSK_TYPE0_ANIM_0
    &object_ikn_demo_Anim_004208, // OSK_TYPE0_ANIM_1
    &object_ikn_demo_Anim_004514, // OSK_TYPE0_ANIM_2
    &object_ikn_demo_Anim_004A58, // OSK_TYPE0_ANIM_3
    &object_ikn_demo_Anim_0065D8, // OSK_TYPE0_ANIM_4
};

typedef enum OskType1Animation {
    /*  0 */ OSK_TYPE1_ANIM_0,
    /*  1 */ OSK_TYPE1_ANIM_1,
    /*  2 */ OSK_TYPE1_ANIM_2,
    /*  3 */ OSK_TYPE1_ANIM_3,
    /*  4 */ OSK_TYPE1_ANIM_4,
    /*  5 */ OSK_TYPE1_ANIM_5,
    /*  6 */ OSK_TYPE1_ANIM_6,
    /*  7 */ OSK_TYPE1_ANIM_7,
    /*  8 */ OSK_TYPE1_ANIM_8,
    /*  9 */ OSK_TYPE1_ANIM_9,
    /* 10 */ OSK_TYPE1_ANIM_10,
    /* 11 */ OSK_TYPE1_ANIM_11,
    /* 12 */ OSK_TYPE1_ANIM_MAX
} OskType1Animation;

AnimationHeader* sAnimationsType1[OSK_TYPE1_ANIM_MAX] = {
    &object_ikn_demo_Anim_0067B8, // OSK_TYPE1_ANIM_0
    &object_ikn_demo_Anim_0083F4, // OSK_TYPE1_ANIM_1
    &object_ikn_demo_Anim_0081A8, // OSK_TYPE1_ANIM_2
    &object_ikn_demo_Anim_008AF4, // OSK_TYPE1_ANIM_3
    &object_ikn_demo_Anim_008724, // OSK_TYPE1_ANIM_4
    &object_ikn_demo_Anim_008F1C, // OSK_TYPE1_ANIM_5
    &object_ikn_demo_Anim_008CEC, // OSK_TYPE1_ANIM_6
    &object_ikn_demo_Anim_009254, // OSK_TYPE1_ANIM_7
    &object_ikn_demo_Anim_009044, // OSK_TYPE1_ANIM_8
    &object_ikn_demo_Anim_0098E4, // OSK_TYPE1_ANIM_9
    &object_ikn_demo_Anim_009CC8, // OSK_TYPE1_ANIM_10
    &object_ikn_demo_Anim_009A44, // OSK_TYPE1_ANIM_11
};

typedef enum OskType2Animation {
    /*  0 */ OSK_TYPE2_ANIM_0,
    /*  1 */ OSK_TYPE2_ANIM_1,
    /*  2 */ OSK_TYPE2_ANIM_2,
    /*  3 */ OSK_TYPE2_ANIM_3,
    /*  4 */ OSK_TYPE2_ANIM_4,
    /*  5 */ OSK_TYPE2_ANIM_5,
    /*  6 */ OSK_TYPE2_ANIM_6,
    /*  7 */ OSK_TYPE2_ANIM_7,
    /*  8 */ OSK_TYPE2_ANIM_8,
    /*  9 */ OSK_TYPE2_ANIM_9,
    /* 10 */ OSK_TYPE2_ANIM_10,
    /* 11 */ OSK_TYPE2_ANIM_11,
    /* 12 */ OSK_TYPE2_ANIM_12,
    /* 13 */ OSK_TYPE2_ANIM_MAX
} OskType2Animation;

AnimationHeader* sAnimationsType2[OSK_TYPE2_ANIM_MAX] = {
    &object_ikn_demo_Anim_009EA8, // OSK_TYPE2_ANIM_0
    &object_ikn_demo_Anim_00B8C0, // OSK_TYPE2_ANIM_1
    &object_ikn_demo_Anim_00B704, // OSK_TYPE2_ANIM_2
    &object_ikn_demo_Anim_00BDB4, // OSK_TYPE2_ANIM_3
    &object_ikn_demo_Anim_00B9E4, // OSK_TYPE2_ANIM_4
    &object_ikn_demo_Anim_00C154, // OSK_TYPE2_ANIM_5
    &object_ikn_demo_Anim_00BF04, // OSK_TYPE2_ANIM_6
    &object_ikn_demo_Anim_00CA74, // OSK_TYPE2_ANIM_7
    &object_ikn_demo_Anim_00C804, // OSK_TYPE2_ANIM_8
    &object_ikn_demo_Anim_00D044, // OSK_TYPE2_ANIM_9
    &object_ikn_demo_Anim_00CD28, // OSK_TYPE2_ANIM_10
    &object_ikn_demo_Anim_00C59C, // OSK_TYPE2_ANIM_11
    &object_ikn_demo_Anim_00C2C4, // OSK_TYPE2_ANIM_12
};

void EnOsk_Init(Actor* thisx, PlayState* play) {
    EnOsk* this = (EnOsk*)thisx;

    Actor_SetScale(&this->actor, 0.013f);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 10.0f);

    this->actionFunc = func_80BF5F60;
    this->animIndex = -1;
    this->cueId = -1;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    switch (ENOSK_GET_TYPE(&this->actor)) {
        case ENOSK_TYPE_1:
            SkelAnime_InitFlex(play, &this->skelAnime, &object_ikn_demo_Skel_007B48, &object_ikn_demo_Anim_006808,
                               this->jointTable, this->morphTable, OBJECT_IKN_DEMO_2_LIMB_MAX);
            Animation_PlayLoop(&this->skelAnime, &object_ikn_demo_Anim_006808);
            this->actionFunc = func_80BF656C;
            this->cueType = CS_CMD_ACTOR_CUE_528;
            break;

        case ENOSK_TYPE_2:
            SkelAnime_InitFlex(play, &this->skelAnime, &object_ikn_demo_Skel_00B490, &object_ikn_demo_Anim_009F00,
                               this->jointTable, this->morphTable, OBJECT_IKN_DEMO_3_LIMB_MAX);
            Animation_PlayLoop(&this->skelAnime, &object_ikn_demo_Anim_006808);
            this->actionFunc = func_80BF6A20;
            this->cueType = CS_CMD_ACTOR_CUE_529;
            break;

        default: // ENOSK_TYPE_0
            Actor_SetScale(&this->actor, 0.017f);
            SkelAnime_InitFlex(play, &this->skelAnime, &object_ikn_demo_Skel_0038F0, &object_ikn_demo_Anim_0000B8,
                               this->jointTable, this->morphTable, OBJECT_IKN_DEMO_1_LIMB_MAX);
            Animation_PlayLoop(&this->skelAnime, &object_ikn_demo_Anim_0000B8);
            this->actionFunc = func_80BF61EC;
            this->cueType = CS_CMD_ACTOR_CUE_530;
            this->actor.home.rot.z = 0;
            this->animSfxFrame = 0.0f;
            break;
    }
}

void EnOsk_Destroy(Actor* thisx, PlayState* play) {
}

void EnOsk_ChangeAnimLoop(EnOsk* this, AnimationHeader** animations, s16 animIndex, f32 morphFrame) {
    if (this->animIndex == animIndex) {
        return;
    }
    Animation_MorphToLoop(&this->skelAnime, animations[animIndex], morphFrame);
    this->animIndex = animIndex;
}

void EnOsk_ChangeAnimOnce(EnOsk* this, AnimationHeader** animations, s16 animIndex, f32 playSpeed) {
    Animation_MorphToPlayOnce(&this->skelAnime, animations[animIndex], playSpeed);
    this->animIndex = animIndex;
}

void func_80BF5EBC(EnOsk* this, PlayState* play) {
    static Vec3f D_80BF7018 = { 0.0f, 0.5f, 0.0f };
    Vec3f sp2C;

    sp2C.x = Rand_CenteredFloat(30.0f) + this->actor.world.pos.x;
    sp2C.z = Rand_CenteredFloat(30.0f) + this->actor.world.pos.z;
    sp2C.y = Rand_CenteredFloat(30.0f) + this->actor.world.pos.y;

    func_800B3030(play, &sp2C, &D_80BF7018, &D_80BF7018, 100, 0, 2);
}

void func_80BF5F60(EnOsk* this, PlayState* play) {
}

void func_80BF5F70(EnOsk* this) {
    if ((this->cueId != 1) && (this->cueId != 6)) {
        this->actor.draw = EnOsk_Draw;
        Actor_SetScale(&this->actor, 0.017f);
    }

    switch (this->cueId) {
        case 1:
            this->actor.draw = NULL;
            break;

        case 2:
            EnOsk_ChangeAnimLoop(this, sAnimationsType0, OSK_TYPE0_ANIM_3, -5.0f);
            break;

        case 3:
            EnOsk_ChangeAnimLoop(this, sAnimationsType0, OSK_TYPE0_ANIM_2, -5.0f);
            break;

        case 4:
            EnOsk_ChangeAnimLoop(this, sAnimationsType0, OSK_TYPE0_ANIM_1, -5.0f);
            break;

        case 5:
            EnOsk_ChangeAnimLoop(this, sAnimationsType0, OSK_TYPE0_ANIM_0, -5.0f);
            break;

        case 6:
            EnOsk_ChangeAnimLoop(this, sAnimationsType0, OSK_TYPE0_ANIM_1, -5.0f);
            break;

        case 7:
            EnOsk_ChangeAnimLoop(this, sAnimationsType0, OSK_TYPE0_ANIM_4, -5.0f);
            this->actor.home.rot.z = 1;
            break;

        default:
            break;
    }
}

void func_80BF609C(EnOsk* this, PlayState* play) {
    if (this->actor.draw != NULL) {
        if (this->actor.home.rot.z != 0) {
            if (play->msgCtx.currentTextId == 0x1531) {
                this->actor.home.rot.z = 0;
            }
        } else {
            if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
                this->animSfxFrame = Rand_ZeroFloat(3.0f);
            }

            switch (this->animIndex) {
                case OSK_TYPE0_ANIM_0:
                    if (Animation_OnFrame(&this->skelAnime, this->animSfxFrame) ||
                        Animation_OnFrame(&this->skelAnime, this->animSfxFrame + 8.0f)) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_TALK);
                    }
                    break;

                case OSK_TYPE0_ANIM_1:
                    if (Animation_OnFrame(&this->skelAnime, this->animSfxFrame)) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_TALK);
                    }
                    break;

                case OSK_TYPE0_ANIM_2:
                    if (Animation_OnFrame(&this->skelAnime, this->animSfxFrame) ||
                        Animation_OnFrame(&this->skelAnime, this->animSfxFrame + 6.0f)) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_BOSU_TALK);
                    }
                    break;

                default:
                    break;
            }
        }
    }
}

void func_80BF61EC(EnOsk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
        if (this->cueId != play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id) {
            this->cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
            func_80BF5F70(this);
        }
        func_80BF609C(this, play);
    } else {
        this->actor.draw = NULL;
    }

    if (this->cueId == 6) {
        if (this->actor.scale.x > 0.85f * 0.001f) {
            this->actor.scale.x -= 0.85f * 0.001f;
            Actor_SetScale(&this->actor, this->actor.scale.x);
            func_80BF5EBC(this, play);
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
        } else {
            this->actor.draw = NULL;
        }
    }
}

void func_80BF6314(EnOsk* this) {
    if ((this->cueId != 1) && (this->cueId != 9)) {
        this->actor.draw = EnOsk_Draw;
        Actor_SetScale(&this->actor, 0.013f);
    }

    switch (this->cueId) {
        case 1:
            this->actor.draw = NULL;
            break;

        case 2:
            EnOsk_ChangeAnimLoop(this, sAnimationsType1, OSK_TYPE1_ANIM_9, -5.0f);
            Actor_SetScale(&this->actor, 0.0f);
            break;

        case 3:
            EnOsk_ChangeAnimOnce(this, sAnimationsType1, OSK_TYPE1_ANIM_2, -5.0f);
            break;

        case 4:
            EnOsk_ChangeAnimOnce(this, sAnimationsType1, OSK_TYPE1_ANIM_4, 0);
            break;

        case 5:
            EnOsk_ChangeAnimOnce(this, sAnimationsType1, OSK_TYPE1_ANIM_8, -5.0f);
            break;

        case 6:
            EnOsk_ChangeAnimOnce(this, sAnimationsType1, OSK_TYPE1_ANIM_11, -5.0f);
            break;

        case 7:
            EnOsk_ChangeAnimOnce(this, sAnimationsType1, OSK_TYPE1_ANIM_6, -5.0f);
            break;

        case 8:
            EnOsk_ChangeAnimLoop(this, sAnimationsType1, OSK_TYPE1_ANIM_0, -5.0f);
            break;

        case 9:
            EnOsk_ChangeAnimLoop(this, sAnimationsType1, OSK_TYPE1_ANIM_0, -5.0f);
            break;

        default:
            break;
    }
}

void func_80BF6478(EnOsk* this) {
    if (this->actor.draw != NULL) {
        switch (this->animIndex) {
            case OSK_TYPE1_ANIM_1:
                if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 6.0f) ||
                    Animation_OnFrame(&this->skelAnime, 11.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_DEBU_HEAD_UP);
                }
                break;

            case OSK_TYPE1_ANIM_3:
                if (Animation_OnFrame(&this->skelAnime, 11.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_DEBU_HEAD_UP);
                }
                break;

            case OSK_TYPE1_ANIM_4:
                if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_DEBU_HEAD_UP);
                }
                break;

            case OSK_TYPE1_ANIM_10:
            case OSK_TYPE1_ANIM_11:
                Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_YASE_LAUGH_K - SFX_FLAG);
                break;

            default:
                break;
        }
    }
}

void func_80BF656C(EnOsk* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        switch (this->animIndex) {
            case OSK_TYPE1_ANIM_2:
                EnOsk_ChangeAnimLoop(this, sAnimationsType1, OSK_TYPE1_ANIM_1, 0.0f);
                break;

            case OSK_TYPE1_ANIM_4:
                EnOsk_ChangeAnimLoop(this, sAnimationsType1, OSK_TYPE1_ANIM_3, 0.0f);
                break;

            case OSK_TYPE1_ANIM_8:
                EnOsk_ChangeAnimLoop(this, sAnimationsType1, OSK_TYPE1_ANIM_7, -5.0f);
                break;

            case OSK_TYPE1_ANIM_11:
                EnOsk_ChangeAnimLoop(this, sAnimationsType1, OSK_TYPE1_ANIM_10, -5.0f);
                break;

            case OSK_TYPE1_ANIM_6:
                EnOsk_ChangeAnimLoop(this, sAnimationsType1, OSK_TYPE1_ANIM_5, -5.0f);
                break;

            default:
                break;
        }
    }

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
        if (this->cueId != play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id) {
            this->cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
            func_80BF6314(this);
        }
        func_80BF6478(this);
    } else {
        this->actor.draw = NULL;
    }

    if ((this->cueId == 2) && (this->actor.scale.x < (13.0f * 0.001f))) {
        this->actor.scale.x += 0.65f * 0.001f;
        Actor_SetScale(&this->actor, this->actor.scale.x);
    }

    if (this->cueId == 9) {
        if (this->actor.scale.x > (0.65f * 0.001f)) {
            this->actor.scale.x -= 0.65f * 0.001f;
            Actor_SetScale(&this->actor, this->actor.scale.x);
            func_80BF5EBC(this, play);
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
        } else {
            this->actor.draw = NULL;
        }
    }
}

void func_80BF67A8(EnOsk* this) {
    if ((this->cueId != 1) && (this->cueId != 8)) {
        this->actor.draw = EnOsk_Draw;
        Actor_SetScale(&this->actor, 0.013f);
    }

    switch (this->cueId) {
        case 1:
            this->actor.draw = NULL;
            break;

        case 2:
            EnOsk_ChangeAnimOnce(this, sAnimationsType2, OSK_TYPE2_ANIM_2, -5.0f);
            break;

        case 3:
            EnOsk_ChangeAnimOnce(this, sAnimationsType2, OSK_TYPE2_ANIM_8, -5.0f);
            break;

        case 4:
            EnOsk_ChangeAnimOnce(this, sAnimationsType2, OSK_TYPE2_ANIM_4, -5.0f);
            break;

        case 5:
            EnOsk_ChangeAnimOnce(this, sAnimationsType2, OSK_TYPE2_ANIM_6, -5.0f);
            break;

        case 6:
            EnOsk_ChangeAnimOnce(this, sAnimationsType2, OSK_TYPE2_ANIM_10, -5.0f);
            break;

        case 7:
        case 8:
            EnOsk_ChangeAnimLoop(this, sAnimationsType2, OSK_TYPE2_ANIM_0, -5.0f);
            break;

        case 9:
            EnOsk_ChangeAnimOnce(this, sAnimationsType2, OSK_TYPE2_ANIM_12, -5.0f);
            break;

        default:
            break;
    }
}

void func_80BF68E0(EnOsk* this) {
    if (this->actor.draw != NULL) {
        switch (this->animIndex) {
            case OSK_TYPE2_ANIM_1:
            case OSK_TYPE2_ANIM_7:
                if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 6.0f) ||
                    Animation_OnFrame(&this->skelAnime, 11.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_DEBU_HEAD_UP);
                }
                break;

            case OSK_TYPE2_ANIM_2:
                if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_DEBU_HEAD_UP);
                }
                break;

            case OSK_TYPE2_ANIM_4:
                if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_DEBU_HEAD_UP);
                }
                break;

            case OSK_TYPE2_ANIM_5:
                if (Animation_OnFrame(&this->skelAnime, 6.0f) || Animation_OnFrame(&this->skelAnime, 11.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_DEBU_HEAD_UP);
                }
                break;

            case OSK_TYPE2_ANIM_8:
                if (Animation_OnFrame(&this->skelAnime, 13.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_DEBU_HEAD_UP);
                }
                break;

            default:
                break;
        }
    }
}

void func_80BF6A20(EnOsk* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        switch (this->animIndex) {
            case OSK_TYPE2_ANIM_2:
                EnOsk_ChangeAnimLoop(this, sAnimationsType2, OSK_TYPE2_ANIM_1, -5.0f);
                break;

            case OSK_TYPE2_ANIM_8:
                EnOsk_ChangeAnimLoop(this, sAnimationsType2, OSK_TYPE2_ANIM_7, -5.0f);
                break;

            case OSK_TYPE2_ANIM_4:
                EnOsk_ChangeAnimLoop(this, sAnimationsType2, OSK_TYPE2_ANIM_3, -5.0f);
                break;

            case OSK_TYPE2_ANIM_6:
                EnOsk_ChangeAnimLoop(this, sAnimationsType2, OSK_TYPE2_ANIM_5, -5.0f);
                break;

            case OSK_TYPE2_ANIM_10:
                EnOsk_ChangeAnimLoop(this, sAnimationsType2, OSK_TYPE2_ANIM_9, -5.0f);
                break;

            case OSK_TYPE2_ANIM_12:
                EnOsk_ChangeAnimLoop(this, sAnimationsType2, OSK_TYPE2_ANIM_11, -5.0f);
                break;

            default:
                break;
        }
    }

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));
        if (this->cueId != play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id) {
            this->cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id;
            func_80BF67A8(this);
        }
        func_80BF68E0(this);
    } else {
        this->actor.draw = NULL;
    }

    if (this->cueId == 8) {
        if (this->actor.scale.x > 0.65f * 0.001f) {
            this->actor.scale.x -= 0.65f * 0.001f;
            Actor_SetScale(&this->actor, this->actor.scale.x);
            func_80BF5EBC(this, play);
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
        } else {
            this->actor.draw = NULL;
        }
    }
}

void EnOsk_Update(Actor* thisx, PlayState* play) {
    EnOsk* this = (EnOsk*)thisx;

    this->actionFunc(this, play);
}

void EnOsk_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80BF7024 = { 0.0f, 0.0f, 0.0f };
    EnOsk* this = (EnOsk*)thisx;

    if (limbIndex == 1) {
        // OBJECT_IKN_DEMO_1_LIMB_01
        // OBJECT_IKN_DEMO_2_LIMB_01
        // OBJECT_IKN_DEMO_3_LIMB_01
        Matrix_MultVec3f(&D_80BF7024, &this->actor.focus.pos);
    }
}

void EnOsk_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOsk* this = (EnOsk*)thisx;
    Gfx* gfx;
    Vec3f sp80;
    s32 pad2[4];

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_PrimColor(play->state.gfxCtx, 0x80, 255, 255, 255, 255));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          EnOsk_PostLimbDraw, &this->actor);

    {
        s16 sp62;
        s16 sp60;
        Vec3f sp54;

        sp54 = GET_ACTIVE_CAM(play)->eye;

        sp62 = Math_Vec3f_Yaw(&sp54, &this->actor.focus.pos);
        sp60 = -Math_Vec3f_Pitch(&sp54, &this->actor.focus.pos);

        sp80.x = -(15.0f * (Math_SinS(sp62)) * Math_CosS(sp60));
        sp80.y = -(15.0f * Math_SinS(sp60));
        sp80.z = -(15.0f * (Math_CosS(sp62)) * Math_CosS(sp60));

        Matrix_Translate(this->actor.focus.pos.x + sp80.x, this->actor.focus.pos.y + sp80.y,
                         sp80.z = this->actor.focus.pos.z + sp80.z, MTXMODE_NEW);

        sp80.z = Math_SinS(play->gameplayFrames * 0x4000);
        sp80.z = ((sp80.z + 1.0f) * 0.1f) + 2.0f;
        Matrix_Scale(this->actor.scale.x * sp80.z, this->actor.scale.y * sp80.z, this->actor.scale.z * sp80.z,
                     MTXMODE_APPLY);
    }

    gfx = POLY_XLU_DISP;
    gfx = Gfx_SetupDL20_NoCD(gfx);

    gDPSetDither(gfx++, G_CD_NOISE);
    gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
    gSPDisplayList(gfx++, gameplay_keep_DL_029CB0);
    gDPSetPrimColor(gfx++, 0, 0, 130, 0, 255, 100);
    MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
    gSPDisplayList(gfx++, gameplay_keep_DL_029CF0);

    POLY_XLU_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}
