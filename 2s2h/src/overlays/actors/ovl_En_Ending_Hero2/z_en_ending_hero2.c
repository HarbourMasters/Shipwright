/*
 * File: z_en_ending_hero2.c
 * Overlay: ovl_En_Ending_Hero2
 * Description: Viscen watching moon disappearance and wedding
 */

#include "z_en_ending_hero2.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnEndingHero2_Init(Actor* thisx, PlayState* play);
void EnEndingHero2_Destroy(Actor* thisx, PlayState* play);
void EnEndingHero2_Update(Actor* thisx, PlayState* play);
void EnEndingHero2_Draw(Actor* thisx, PlayState* play);

void EnEndingHero2_SetupIdle(EnEndingHero2* this);
void EnEndingHero2_Idle(EnEndingHero2* this, PlayState* play);

ActorProfile En_Ending_Hero2_Profile = {
    /**/ ACTOR_EN_ENDING_HERO2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BAI,
    /**/ sizeof(EnEndingHero2),
    /**/ EnEndingHero2_Init,
    /**/ EnEndingHero2_Destroy,
    /**/ EnEndingHero2_Update,
    /**/ EnEndingHero2_Draw,
};

void EnEndingHero2_Init(Actor* thisx, PlayState* play) {
    EnEndingHero2* this = (EnEndingHero2*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -3.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_bai_Skel_007908, &object_bai_Anim_0011C0, this->jointTable,
                       this->morphTable, OBJECT_BAI_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    EnEndingHero2_SetupIdle(this);
}

void EnEndingHero2_Destroy(Actor* thisx, PlayState* play) {
}

void EnEndingHero2_SetupIdle(EnEndingHero2* this) {
    this->isIdle = true;
    this->actionFunc = EnEndingHero2_Idle;
}

void EnEndingHero2_Idle(EnEndingHero2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnEndingHero2_Update(Actor* thisx, PlayState* play) {
    EnEndingHero2* this = (EnEndingHero2*)thisx;

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
}

void EnEndingHero2_Draw(Actor* thisx, PlayState* play) {
    EnEndingHero2* this = (EnEndingHero2*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);
}
