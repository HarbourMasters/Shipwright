/*
 * File: z_en_boj_01.c
 * Overlay: ovl_En_Boj_01
 * Description: [Empty]
 */

#include "z_en_boj_01.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnBoj01_Init(Actor* thisx, PlayState* play);
void EnBoj01_Destroy(Actor* thisx, PlayState* play);
void EnBoj01_Update(Actor* thisx, PlayState* play);
void EnBoj01_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Boj_01_Profile = {
    /**/ ACTOR_EN_BOJ_01,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnBoj01),
    /**/ EnBoj01_Init,
    /**/ EnBoj01_Destroy,
    /**/ EnBoj01_Update,
    /**/ EnBoj01_Draw,
};

void EnBoj01_Init(Actor* thisx, PlayState* play) {
}

void EnBoj01_Destroy(Actor* thisx, PlayState* play) {
}

void EnBoj01_Update(Actor* thisx, PlayState* play) {
}

void EnBoj01_Draw(Actor* thisx, PlayState* play) {
}
