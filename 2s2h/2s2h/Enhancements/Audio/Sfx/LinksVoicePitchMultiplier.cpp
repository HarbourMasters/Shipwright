#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

// For Link's voice pitch SFX modifier
static f32 freqMultiplier = 1;

#define CVAR_NAME "gAudioEditor.LinkVoiceFreqMultiplier.Enable"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterLinksVoicePitchMultiplier() {
    COND_VB_SHOULD(VB_LINK_VOICE_PITCH_MULTIPLIER, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        u16 sfxId = *va_arg(args, u16*);

        if (sfxId >= NA_SE_VO_LI_SWORD_N && sfxId <= NA_SE_VO_DEMO_394 || sfxId == NA_SE_PL_TRANSFORM_VOICE) {

            *should = false;
            freqMultiplier = CVarGetFloat("gAudioEditor.LinkVoiceFreqMultiplier.Scale", 1.0);
            if (freqMultiplier <= 0) {
                freqMultiplier = 1;
            }

            AudioSfx_PlaySfx(sfxId, &player->actor.projectedPos, 4, &freqMultiplier, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultReverb);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterLinksVoicePitchMultiplier, { CVAR_NAME });
