/*
 * File: z_en_boj_04.c
 * Overlay: ovl_En_Boj_04
 * Description: [Empty]
 */

#include "z_en_boj_04.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnBoj04_Init(Actor* thisx, PlayState* play);
void EnBoj04_Destroy(Actor* thisx, PlayState* play);
void EnBoj04_Update(Actor* thisx, PlayState* play);
void EnBoj04_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Boj_04_Profile = {
    /**/ ACTOR_EN_BOJ_04,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnBoj04),
    /**/ EnBoj04_Init,
    /**/ EnBoj04_Destroy,
    /**/ EnBoj04_Update,
    /**/ EnBoj04_Draw,
};

void EnBoj04_Init(Actor* thisx, PlayState* play) {
}

void EnBoj04_Destroy(Actor* thisx, PlayState* play) {
}

void EnBoj04_Update(Actor* thisx, PlayState* play) {
}

void EnBoj04_Draw(Actor* thisx, PlayState* play) {
}
