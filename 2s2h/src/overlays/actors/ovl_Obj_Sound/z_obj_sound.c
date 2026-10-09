/*
 * File: z_obj_sound.c
 * Overlay: ovl_Obj_Sound
 * Description: An invisible point-source to play sounds, including sfx and sequences.
 */

#include "z_obj_sound.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjSound_Init(Actor* thisx, PlayState* play);
void ObjSound_Destroy(Actor* thisx, PlayState* play);
void ObjSound_Update(Actor* thisx, PlayState* play);
void ObjSound_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Sound_Profile = {
    /**/ ACTOR_OBJ_SOUND,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjSound),
    /**/ ObjSound_Init,
    /**/ ObjSound_Destroy,
    /**/ ObjSound_Update,
    /**/ NULL,
};

void ObjSound_Init(Actor* thisx, PlayState* play) {
    ObjSound* this = (ObjSound*)thisx;

    this->unk_144 = false;
    this->soundType = OBJ_SOUND_GET_TYPE(&this->actor);
    this->sfxType = OBJ_SOUND_GET_SFX_TYPE(&this->actor);
    this->actor.params &= OBJ_SOUND_ID_MASK;
    if (this->soundType == OBJ_SOUND_TYPE_FANFARE) {
        this->actor.draw = ObjSound_Draw;
    }
}

void ObjSound_Destroy(Actor* thisx, PlayState* play) {
    ObjSound* this = (ObjSound*)thisx;

    if (this->soundType == OBJ_SOUND_TYPE_BGM) {
        Audio_PlayObjSoundBgm(NULL, NA_BGM_GENERAL_SFX);
    }
}

void ObjSound_Update(Actor* thisx, PlayState* play) {
    ObjSound* this = (ObjSound*)thisx;

    if (this->soundType == OBJ_SOUND_TYPE_SFX) {
        if (this->sfxType != 0) {
            Actor_PlaySfx_Flagged(&this->actor, gAudioEnvironmentalSfx[this->actor.params]);
        } else {
            Actor_PlaySfx_FlaggedCentered2(&this->actor, gAudioEnvironmentalSfx[this->actor.params]);
        }
    } else if (this->unk_144) {
        if (this->soundType == OBJ_SOUND_TYPE_BGM) {
            Audio_PlayObjSoundBgm(&this->actor.projectedPos, this->actor.params);
        } else if (this->soundType == OBJ_SOUND_TYPE_FIXED_SFX) {
            Audio_PlaySfx_AtFixedPos(&this->actor.projectedPos, gAudioEnvironmentalSfx[this->actor.params]);
        }
    } else {
        this->unk_144 = true;
    }
}

void ObjSound_Draw(Actor* thisx, PlayState* play) {
    ObjSound* this = (ObjSound*)thisx;

    if (CHECK_EVENTINF(EVENTINF_41) || CHECK_EVENTINF(EVENTINF_35)) {
        Audio_PlayObjSoundFanfare(&this->actor.projectedPos, this->actor.params);
    }
}
