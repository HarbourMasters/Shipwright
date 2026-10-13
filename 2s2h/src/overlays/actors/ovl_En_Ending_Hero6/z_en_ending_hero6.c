/*
 * File: z_en_ending_hero6.c
 * Overlay: ovl_En_Ending_Hero6
 * Description:
 */

#include "z_en_ending_hero6.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnEndingHero6_Init(Actor* thisx, PlayState* play);
void EnEndingHero6_Destroy(Actor* thisx, PlayState* play);
void EnEndingHero6_Update(Actor* thisx, PlayState* play);
void EnEndingHero6_Draw(Actor* thisx, PlayState* play);

void EnEndingHero6_SetupIdle(EnEndingHero6* this);
void EnEndingHero6_Idle(EnEndingHero6* this, PlayState* play);

ActorProfile En_Ending_Hero6_Profile = {
    /**/ ACTOR_EN_ENDING_HERO6,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnEndingHero6),
    /**/ EnEndingHero6_Init,
    /**/ EnEndingHero6_Destroy,
    /**/ EnEndingHero6_Update,
    /**/ EnEndingHero6_Draw,
};

typedef enum {
    /* 0 */ ENDING_HERO6_TYPE_DT,
    /* 1 */ ENDING_HERO6_TYPE_BAI,
    /* 2 */ ENDING_HERO6_TYPE_TORYO,
    /* 3 */ ENDING_HERO6_TYPE_SOLDIER,
    /* 4 */ ENDING_HERO6_TYPE_DAIKU_RED,
    /* 5 */ ENDING_HERO6_TYPE_DAIKU_BLUE,
    /* 6 */ ENDING_HERO6_TYPE_DAIKU_GREEN,
    /* 7 */ ENDING_HERO6_TYPE_DAIKU_PURPLE,
    /* 8 */ ENDING_HERO6_TYPE_DAIKU_ORANGE,
    /* 9 */ ENDING_HERO6_TYPE_MAX
} EndingHero6Type;

static FlexSkeletonHeader* sSkeletons[ENDING_HERO6_TYPE_MAX] = {
    &object_dt_Skel_00B0CC,    // ENDING_HERO6_TYPE_DT
    &object_bai_Skel_007908,   // ENDING_HERO6_TYPE_BAI
    &object_toryo_Skel_007150, // ENDING_HERO6_TYPE_TORYO
    &gSoldierSkel,             // ENDING_HERO6_TYPE_SOLDIER
    &object_daiku_Skel_00A850, // ENDING_HERO6_TYPE_DAIKU_RED
    &object_daiku_Skel_00A850, // ENDING_HERO6_TYPE_DAIKU_BLUE
    &object_daiku_Skel_00A850, // ENDING_HERO6_TYPE_DAIKU_GREEN
    &object_daiku_Skel_00A850, // ENDING_HERO6_TYPE_DAIKU_PURPLE
    &object_daiku_Skel_00A850, // ENDING_HERO6_TYPE_DAIKU_ORANGE
};

static AnimationHeader* sAnimations[ENDING_HERO6_TYPE_MAX] = {
    &gDotourUprightAnim,         // ENDING_HERO6_TYPE_DT
    &object_bai_Anim_0011C0,     // ENDING_HERO6_TYPE_BAI
    &object_toryo_Anim_000E50,   // ENDING_HERO6_TYPE_TORYO
    &gSoldierCheerWithSpearAnim, // ENDING_HERO6_TYPE_SOLDIER
    &object_daiku_Anim_002FA0,   // ENDING_HERO6_TYPE_DAIKU_RED
    &object_daiku_Anim_002FA0,   // ENDING_HERO6_TYPE_DAIKU_BLUE
    &object_daiku_Anim_002FA0,   // ENDING_HERO6_TYPE_DAIKU_GREEN
    &object_daiku_Anim_002FA0,   // ENDING_HERO6_TYPE_DAIKU_PURPLE
    &object_daiku_Anim_002FA0,   // ENDING_HERO6_TYPE_DAIKU_ORANGE
};

static s32 sLimbCounts[ENDING_HERO6_TYPE_MAX] = {
    OBJECT_DT_LIMB_MAX,    // ENDING_HERO6_TYPE_DT
    OBJECT_BAI_LIMB_MAX,   // ENDING_HERO6_TYPE_BAI
    OBJECT_TORYO_LIMB_MAX, // ENDING_HERO6_TYPE_TORYO
    SOLDIER_LIMB_MAX,      // ENDING_HERO6_TYPE_SOLDIER
    OBJECT_DAIKU_LIMB_MAX, // ENDING_HERO6_TYPE_DAIKU_RED
    OBJECT_DAIKU_LIMB_MAX, // ENDING_HERO6_TYPE_DAIKU_BLUE
    OBJECT_DAIKU_LIMB_MAX, // ENDING_HERO6_TYPE_DAIKU_GREEN
    OBJECT_DAIKU_LIMB_MAX, // ENDING_HERO6_TYPE_DAIKU_PURPLE
    OBJECT_DAIKU_LIMB_MAX, // ENDING_HERO6_TYPE_DAIKU_ORANGE
};

void EnEndingHero6_Init(Actor* thisx, PlayState* play) {
    EnEndingHero6* this = (EnEndingHero6*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -3.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, sSkeletons[this->type], sAnimations[this->type], this->jointTable,
                       this->morphTable, sLimbCounts[this->type]);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    EnEndingHero6_SetupIdle(this);
}

void EnEndingHero6_Destroy(Actor* thisx, PlayState* play) {
}

void EnEndingHero6_ChangeAnim(EnEndingHero6* this, s32 type) {
    this->animIndex = type;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[type]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.f, this->animEndFrame, ANIMMODE_LOOP,
                     0.0f);
}

void EnEndingHero6_SetupIdle(EnEndingHero6* this) {
    EnEndingHero6_ChangeAnim(this, this->type);
    this->isIdle = true;
    this->actionFunc = EnEndingHero6_Idle;
}

void EnEndingHero6_Idle(EnEndingHero6* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnEndingHero6_Update(Actor* thisx, PlayState* play) {
    EnEndingHero6* this = (EnEndingHero6*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->blinkTimer == 0) {
        this->eyeState++;
        if (this->eyeState >= 3) {
            this->eyeState = 0;
            this->blinkTimer = TRUNCF_BINANG(Rand_ZeroFloat(60.0f)) + 20;
        }
    }

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
}

void EnEndingHero6_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Gfx* D_80C2426C[] = {
        object_daiku_DL_0070C0, // ENDING_HERO6_TYPE_DAIKU_RED
        object_daiku_DL_006FB0, // ENDING_HERO6_TYPE_DAIKU_BLUE
        object_daiku_DL_006E80, // ENDING_HERO6_TYPE_DAIKU_GREEN
        object_daiku_DL_006D70, // ENDING_HERO6_TYPE_DAIKU_PURPLE
        object_daiku_DL_00A390, // ENDING_HERO6_TYPE_DAIKU_ORANGE
    };
    EnEndingHero6* this = (EnEndingHero6*)thisx;
    s32 daikuIndex;

    OPEN_DISPS(play->state.gfxCtx);

    if ((this->type >= ENDING_HERO6_TYPE_DAIKU_RED) && (limbIndex == OBJECT_DAIKU_LIMB_0F)) {
        daikuIndex = this->type - ENDING_HERO6_TYPE_DAIKU_RED;
        gSPDisplayList(POLY_OPA_DISP++, D_80C2426C[daikuIndex]);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnEndingHero6_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = { gDotourEyeShockTex, gDotourEyeOpenTex, gDotourEyeClosedTex,
                                         gDotourEyeLookDownTex, gDotourEyeSquintTex };
    static TexturePtr sEyebrowTextures[] = { gDotourEyebrowHighTex, gDotourEyebrowMidTex, gDotourEyebrowLowTex };
    s32 pad;
    EnEndingHero6* this = (EnEndingHero6*)thisx;
    s32 index = 0;

    if (this->isIdle == true) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        if ((this->objectSlot > OBJECT_SLOT_NONE) && Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->objectSlot].segment);

            switch (this->type) {
                case ENDING_HERO6_TYPE_DAIKU_RED:
                    gDPSetEnvColor(POLY_OPA_DISP++, 170, 10, 70, 255);
                    break;

                case ENDING_HERO6_TYPE_DAIKU_BLUE:
                    gDPSetEnvColor(POLY_OPA_DISP++, 170, 200, 255, 255);
                    break;

                case ENDING_HERO6_TYPE_DAIKU_GREEN:
                    gDPSetEnvColor(POLY_OPA_DISP++, 0, 230, 70, 255);
                    break;

                case ENDING_HERO6_TYPE_DAIKU_PURPLE:
                    gDPSetEnvColor(POLY_OPA_DISP++, 200, 0, 150, 255);
                    break;

                case ENDING_HERO6_TYPE_DAIKU_ORANGE:
                    gDPSetEnvColor(POLY_OPA_DISP++, 245, 155, 0, 255);
                    break;

                default:
                    break;
            }

            if (this->type == ENDING_HERO6_TYPE_DT) {
                gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeState]));

                if (this->eyeState < 3) {
                    index = this->eyeState;
                }

                gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyebrowTextures[index]));
            }

            SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, NULL, EnEndingHero6_PostLimbDraw, &this->actor);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
