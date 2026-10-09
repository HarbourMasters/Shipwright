/*
 * File: z_en_ending_hero.c
 * Overlay: ovl_En_Ending_Hero
 * Description: Mayor Dotour at wedding during the credits
 */

#include "z_en_ending_hero.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnEndingHero_Init(Actor* thisx, PlayState* play);
void EnEndingHero_Destroy(Actor* thisx, PlayState* play);
void EnEndingHero_Update(Actor* thisx, PlayState* play);
void EnEndingHero_Draw(Actor* thisx, PlayState* play);

void EnEndingHero1_SetupIdle(EnEndingHero* this);
void EnEndingHero1_Idle(EnEndingHero* this, PlayState* play);

ActorProfile En_Ending_Hero_Profile = {
    /**/ ACTOR_EN_ENDING_HERO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DT,
    /**/ sizeof(EnEndingHero),
    /**/ EnEndingHero_Init,
    /**/ EnEndingHero_Destroy,
    /**/ EnEndingHero_Update,
    /**/ EnEndingHero_Draw,
};

void EnEndingHero_Init(Actor* thisx, PlayState* play) {
    EnEndingHero* this = (EnEndingHero*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -3.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_dt_Skel_00B0CC, &gDotourUprightAnim, this->jointTable,
                       this->morphTable, OBJECT_DT_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    EnEndingHero1_SetupIdle(this);
}

void EnEndingHero_Destroy(Actor* thisx, PlayState* play) {
}

void EnEndingHero1_SetupIdle(EnEndingHero* this) {
    this->isIdle = true;
    this->actionFunc = EnEndingHero1_Idle;
}

void EnEndingHero1_Idle(EnEndingHero* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnEndingHero_Update(Actor* thisx, PlayState* play) {
    EnEndingHero* this = (EnEndingHero*)thisx;

    if (this->unk240 == 0) {
        this->unk242++;
        if (this->unk242 > 2) {
            this->unk242 = 0;
            this->unk240 = TRUNCF_BINANG(Rand_ZeroFloat(60.0f)) + 0x14;
        }
    }
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
}

static TexturePtr sEyeTextures[] = {
    gDotourEyeShockTex, gDotourEyeOpenTex, gDotourEyeClosedTex, gDotourEyeLookDownTex, gDotourEyeSquintTex,
};
static TexturePtr sEyebrowTextures[] = {
    gDotourEyebrowHighTex,
    gDotourEyebrowMidTex,
    gDotourEyebrowLowTex,
};

void EnEndingHero_Draw(Actor* thisx, PlayState* play) {
    EnEndingHero* this = (EnEndingHero*)thisx;
    s32 index = 0;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->unk242]));

    if (this->unk242 < 3) {
        index = this->unk242;
    }

    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyebrowTextures[index]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
