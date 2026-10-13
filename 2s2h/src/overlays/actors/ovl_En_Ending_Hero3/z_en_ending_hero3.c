/*
 * File: z_en_ending_hero3.c
 * Overlay: ovl_En_Ending_Hero3
 * Description: Mutoh watching moon disappearance and wedding
 */

#include "z_en_ending_hero3.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnEndingHero3_Init(Actor* thisx, PlayState* play);
void EnEndingHero3_Destroy(Actor* thisx, PlayState* play);
void EnEndingHero3_Update(Actor* thisx, PlayState* play);
void EnEndingHero3_Draw(Actor* thisx, PlayState* play);

void EnEndingHero3_SetupIdle(EnEndingHero3* this);
void EnEndingHero3_Idle(EnEndingHero3* this, PlayState* play);

ActorProfile En_Ending_Hero3_Profile = {
    /**/ ACTOR_EN_ENDING_HERO3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TORYO,
    /**/ sizeof(EnEndingHero3),
    /**/ EnEndingHero3_Init,
    /**/ EnEndingHero3_Destroy,
    /**/ EnEndingHero3_Update,
    /**/ EnEndingHero3_Draw,
};

void EnEndingHero3_Init(Actor* thisx, PlayState* play) {
    EnEndingHero3* this = (EnEndingHero3*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -3.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_toryo_Skel_007150, &object_toryo_Anim_000E50, this->jointTable,
                       this->morphTable, OBJECT_TORYO_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    EnEndingHero3_SetupIdle(this);
}

void EnEndingHero3_Destroy(Actor* thisx, PlayState* play) {
}

void EnEndingHero3_SetupIdle(EnEndingHero3* this) {
    this->isIdle = true;
    this->actionFunc = EnEndingHero3_Idle;
}

void EnEndingHero3_Idle(EnEndingHero3* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnEndingHero3_Update(Actor* thisx, PlayState* play) {
    EnEndingHero3* this = (EnEndingHero3*)thisx;

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
}

void EnEndingHero3_Draw(Actor* thisx, PlayState* play) {
    EnEndingHero3* this = (EnEndingHero3*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);
}
