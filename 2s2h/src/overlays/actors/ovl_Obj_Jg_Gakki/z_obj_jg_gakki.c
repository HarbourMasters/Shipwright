/*
 * File: z_obj_jg_gakki.c
 * Overlay: ovl_Obj_Jg_Gakki
 * Description: Goron Elder's Drum
 */

#include "z_obj_jg_gakki.h"
#include "objects/object_jg/object_jg.h"

#define FLAGS (ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjJgGakki_Init(Actor* thisx, PlayState* play2);
void ObjJgGakki_Destroy(Actor* thisx, PlayState* play);
void ObjJgGakki_Update(Actor* thisx, PlayState* play);
void ObjJgGakki_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Jg_Gakki_Profile = {
    /**/ ACTOR_OBJ_JG_GAKKI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_JG,
    /**/ sizeof(ObjJgGakki),
    /**/ ObjJgGakki_Init,
    /**/ ObjJgGakki_Destroy,
    /**/ ObjJgGakki_Update,
    /**/ ObjJgGakki_Draw,
};

void ObjJgGakki_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjJgGakki* this = (ObjJgGakki*)thisx;
    f32 endFrame = Animation_GetLastFrame(&gGoronElderDrumTakeOutAnim);

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_Init(play, &this->skelAnime, &gGoronElderDrumSkel, NULL, NULL, NULL, 0);

    if (((play->sceneId == SCENE_SPOT00) && (gSaveContext.sceneLayer == 7)) && (play->csCtx.scriptIndex == 0)) {
        Animation_Change(&this->skelAnime, &gGoronElderDrumTakeOutAnim, 1.0f, endFrame, endFrame, ANIMMODE_ONCE, 0.0f);
    } else if ((play->sceneId == SCENE_17SETUGEN) || (play->sceneId == SCENE_10YUKIYAMANOMURA)) {
        Animation_Change(&this->skelAnime, &gGoronElderDrumTakeOutAnim, 1.0f, 0.0f, endFrame, ANIMMODE_ONCE, 0.0f);
    } else {
        Actor_Kill(&this->actor);
    }
    Actor_SetScale(&this->actor, 0.01f);
}

void ObjJgGakki_Destroy(Actor* thisx, PlayState* play) {
    ObjJgGakki* this = (ObjJgGakki*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void ObjJgGakki_Update(Actor* thisx, PlayState* play) {
    ObjJgGakki* this = (ObjJgGakki*)thisx;

    SkelAnime_Update(&this->skelAnime);
}

void ObjJgGakki_Draw(Actor* thisx, PlayState* play) {
    ObjJgGakki* this = (ObjJgGakki*)thisx;

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, &this->actor);
}
