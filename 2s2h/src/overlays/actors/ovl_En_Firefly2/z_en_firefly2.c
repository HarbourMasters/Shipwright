/*
 * File: z_en_firefly2.c
 * Overlay: ovl_En_Firefly2
 * Description: [Empty]
 */

#include "z_en_firefly2.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void EnFirefly2_Init(Actor* thisx, PlayState* play);
void EnFirefly2_Destroy(Actor* thisx, PlayState* play);
void EnFirefly2_Update(Actor* thisx, PlayState* play);
void EnFirefly2_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Firefly2_Profile = {
    /**/ ACTOR_EN_FIREFLY2,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FIREFLY,
    /**/ sizeof(EnFirefly2),
    /**/ EnFirefly2_Init,
    /**/ EnFirefly2_Destroy,
    /**/ EnFirefly2_Update,
    /**/ EnFirefly2_Draw,
};

void EnFirefly2_Init(Actor* thisx, PlayState* play) {
}
void EnFirefly2_Destroy(Actor* thisx, PlayState* play) {
}
void EnFirefly2_Update(Actor* thisx, PlayState* play) {
}
void EnFirefly2_Draw(Actor* thisx, PlayState* play) {
}
