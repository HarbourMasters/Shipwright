/*
 * File: z_en_zl1.c
 * Overlay: ovl_En_Zl1
 * Description: [Empty]
 */

#include "z_en_zl1.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnZl1_Init(Actor* thisx, PlayState* play);
void EnZl1_Destroy(Actor* thisx, PlayState* play);
void EnZl1_Update(Actor* thisx, PlayState* play);
void EnZl1_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Zl1_Profile = {
    /**/ ACTOR_EN_ZL1,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZL1,
    /**/ sizeof(EnZl1),
    /**/ EnZl1_Init,
    /**/ EnZl1_Destroy,
    /**/ EnZl1_Update,
    /**/ EnZl1_Draw,
};

void EnZl1_Init(Actor* thisx, PlayState* play) {
}
void EnZl1_Destroy(Actor* thisx, PlayState* play) {
}
void EnZl1_Update(Actor* thisx, PlayState* play) {
}
void EnZl1_Draw(Actor* thisx, PlayState* play) {
}
