#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_En_Owl/z_en_owl.h"
void func_8095AAD0(EnOwl* enOwl, PlayState* play);
void func_8095A920(EnOwl* enOwl, PlayState* play);
void func_8095B9FC(EnOwl* enOwl, PlayState* play);
void func_8095B574(EnOwl* enOwl, PlayState* play);
void func_8095C484(EnOwl* enOwl);
void EnOwl_ChangeMode(EnOwl* enOwl, EnOwlActionFunc actionFunc, EnOwlFunc unkFunc, SkelAnime* skelAnime,
                      AnimationHeader* animation, f32 morphFrames);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void flyOnApproach(EnOwl* enOwl, PlayState* play) {
    func_8095A920(enOwl, play); // Turn to face player
    if (enOwl->actor.xzDistToPlayer < 200.0f) {
        s32 owlType = ENOWL_GET_TYPE(&enOwl->actor);
        if (owlType == ENOWL_GET_TYPE_2) { // Winter Village owl
            // Start flying across the invisible platforms
            EnOwl_ChangeMode(enOwl, func_8095B9FC, func_8095C484, &enOwl->skelAnimeFlying,
                             (AnimationHeader*)&gOwlUnfoldWingsAnim, 0.0f);
        } else if (owlType == ENOWL_GET_TYPE_3) { // Southern Swamp owl
            // Set flag to enable interaction with Song of Soaring engraving
            Flags_SetSwitch(gPlayState, ENOWL_GET_SWITCH_FLAG(&enOwl->actor));
            enOwl->actionFunc = func_8095AAD0; // Fly away
        }
    }
}

void RegisterSkipOwlInteractions() {
    // Quietly change the owl's actionFunc to have it fly away on player proximity
    COND_ID_HOOK(OnActorInit, ACTOR_EN_OWL, CVAR, [](Actor* actor) {
        EnOwl* enOwl = (EnOwl*)actor;
        enOwl->actionFunc = flyOnApproach;
    });

    // Ditto, after the owl has made it across the invisible platforms in the Winter Village
    COND_VB_SHOULD(VB_OWL_TELL_ABOUT_SHRINE, CVAR, {
        EnOwl* enOwl = va_arg(args, EnOwl*);
        EnOwl_ChangeMode(enOwl, flyOnApproach, func_8095C484, &enOwl->skelAnimePerching,
                         (AnimationHeader*)&gOwlPerchAnim, 0.0f);
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipOwlInteractions, { CVAR_NAME });
