/*
 * File: z_dm_ravine.c
 * Overlay: ovl_Dm_Ravine
 * Description: Lost Woods Cutscene - Tree Trunk
 */

#include "z_dm_ravine.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmRavine_Init(Actor* thisx, PlayState* play);
void DmRavine_Destroy(Actor* thisx, PlayState* play);
void DmRavine_DoNothing(DmRavine* this, PlayState* play);
void DmRavine_Update(Actor* thisx, PlayState* play);
void DmRavine_Draw(Actor* thisx, PlayState* play);

ActorProfile Dm_Ravine_Profile = {
    /**/ ACTOR_DM_RAVINE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_KEIKOKU_DEMO,
    /**/ sizeof(DmRavine),
    /**/ DmRavine_Init,
    /**/ DmRavine_Destroy,
    /**/ DmRavine_Update,
    /**/ DmRavine_Draw,
};

void DmRavine_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    DmRavine* this = (DmRavine*)thisx;

    if (CHECK_WEEKEVENTREG_ALT(WEEKEVENTREG_ENTERED_GORMAN_TRACK) | cREG(0)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->isActive = false;
    play->roomCtx.unk7A[0] = 1;
    play->roomCtx.unk7A[1] = 0;
    this->state = 0;
    Actor_SetScale(&this->actor, 1.0f);
    this->actionFunc = DmRavine_DoNothing;
}

void DmRavine_Destroy(Actor* thisx, PlayState* play) {
}

void DmRavine_DoNothing(DmRavine* this, PlayState* play) {
}

void DmRavine_Update(Actor* thisx, PlayState* play) {
    DmRavine* this = (DmRavine*)thisx;
    RoomContext* roomCtx;

    switch (this->state) {
        case DM_RAVINE_STATE_INITIALIZED:
            break;

        case DM_RAVINE_STATE_ACTIVE:
            this->isActive = true;
            play->roomCtx.unk7A[1]++;
            if (play->roomCtx.unk7A[1] > 254) {
                play->roomCtx.unk7A[1] = 254;
                if (play->csCtx.curFrame > 700) {
                    play->roomCtx.unk7A[1] = 255;
                    play->roomCtx.unk7A[0] = 0;
                    this->state++; // -> DM_RAVINE_STATE_PENDING_DEATH
                }
            }
            break;

        case DM_RAVINE_STATE_PENDING_DEATH:
            Actor_Kill(&this->actor);
            break;

        default:
            break;
    }
}

void DmRavine_Draw(Actor* thisx, PlayState* play) {
}
