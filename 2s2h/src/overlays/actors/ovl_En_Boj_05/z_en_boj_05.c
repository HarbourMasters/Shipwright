/*
 * File: z_en_boj_05.c
 * Overlay: ovl_En_Boj_05
 * Description: [Empty]
 */

#include "z_en_boj_05.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnBoj05_Init(Actor* thisx, PlayState* play);
void EnBoj05_Destroy(Actor* thisx, PlayState* play);
void EnBoj05_Update(Actor* thisx, PlayState* play);
void EnBoj05_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Boj_05_Profile = {
    /**/ ACTOR_EN_BOJ_05,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnBoj05),
    /**/ EnBoj05_Init,
    /**/ EnBoj05_Destroy,
    /**/ EnBoj05_Update,
    /**/ EnBoj05_Draw,
};

void EnBoj05_Init(Actor* thisx, PlayState* play) {
}

void EnBoj05_Destroy(Actor* thisx, PlayState* play) {
}

void EnBoj05_Update(Actor* thisx, PlayState* play) {
}

void EnBoj05_Draw(Actor* thisx, PlayState* play) {
}
