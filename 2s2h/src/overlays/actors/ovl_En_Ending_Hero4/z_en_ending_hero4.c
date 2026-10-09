/*
 * File: z_en_ending_hero4.c
 * Overlay: ovl_En_Ending_Hero4
 * Description: Soldiers watching moon disappearance and Indigo-Go's
 */

#include "z_en_ending_hero4.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnEndingHero4_Init(Actor* thisx, PlayState* play);
void EnEndingHero4_Destroy(Actor* thisx, PlayState* play);
void EnEndingHero4_Update(Actor* thisx, PlayState* play);
void EnEndingHero4_Draw(Actor* thisx, PlayState* play);

void EnEndingHero4_SetupIdle(EnEndingHero4* this);
void EnEndingHero4_Idle(EnEndingHero4* this, PlayState* play);

ActorProfile En_Ending_Hero4_Profile = {
    /**/ ACTOR_EN_ENDING_HERO4,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SDN,
    /**/ sizeof(EnEndingHero4),
    /**/ EnEndingHero4_Init,
    /**/ EnEndingHero4_Destroy,
    /**/ EnEndingHero4_Update,
    /**/ EnEndingHero4_Draw,
};

void EnEndingHero4_Init(Actor* thisx, PlayState* play) {
    EnEndingHero4* this = (EnEndingHero4*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -3.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &gSoldierSkel, &gSoldierCheerWithSpearAnim, this->jointTable,
                       this->morphTable, SOLDIER_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    EnEndingHero4_SetupIdle(this);
}

void EnEndingHero4_Destroy(Actor* thisx, PlayState* play) {
}

void EnEndingHero4_SetupIdle(EnEndingHero4* this) {
    this->isIdle = true;
    this->actionFunc = EnEndingHero4_Idle;
}

void EnEndingHero4_Idle(EnEndingHero4* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnEndingHero4_Update(Actor* thisx, PlayState* play) {
    EnEndingHero4* this = (EnEndingHero4*)thisx;

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
}

void EnEndingHero4_Draw(Actor* thisx, PlayState* play) {
    EnEndingHero4* this = (EnEndingHero4*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);
}
