/*
 * File: z_en_fr.c
 * Overlay: ovl_En_Fr
 * Description: Invisible spot that triggers a slow camera drift towards the spot while player is moving.
 *              Unused in game.
 */

#include "z_en_fr.h"

#define FLAGS (ACTOR_FLAG_CAMERA_DRIFT_ENABLED)

void EnFr_Init(Actor* thisx, PlayState* play);
void EnFr_Destroy(Actor* thisx, PlayState* play);
void EnFr_Update(Actor* thisx, PlayState* play);

ActorProfile En_Fr_Profile = {
    /**/ ACTOR_EN_FR,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnFr),
    /**/ EnFr_Init,
    /**/ EnFr_Destroy,
    /**/ EnFr_Update,
    /**/ NULL,
};

void EnFr_Init(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    if (Flags_GetSwitch(play, ENFR_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.attentionRangeType = ENFR_GET_ATTENTION_RANGE_TYPE(&this->actor);
}

void EnFr_Destroy(Actor* thisx, PlayState* play) {
}

void EnFr_Update(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    if (Flags_GetSwitch(play, ENFR_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->actor.xyzDistToPlayerSq < SQ(IREG(29))) {
        this->actor.flags &= ~ACTOR_FLAG_CAMERA_DRIFT_ENABLED;
    } else {
        this->actor.flags |= ACTOR_FLAG_CAMERA_DRIFT_ENABLED;
    }
}
