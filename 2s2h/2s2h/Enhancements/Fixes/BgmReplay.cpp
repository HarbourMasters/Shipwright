#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
extern u8 sStartSeqDisabled;
}

#define CVAR_NAME_FASTER_SCENE_TRANSITIONS "gEnhancements.Timesavers.FasterSceneTransitions"
#define CVAR_NAME_PAUSE_SAVE "gEnhancements.Saving.PauseSave"
#define CVAR_NAME_DEBUG_MODE "gDeveloperTools.DebugEnabled"

#define CVAR_FASTER_SCENE_TRANSITIONS CVarGetInteger(CVAR_NAME_FASTER_SCENE_TRANSITIONS, 0)
#define CVAR_PAUSE_SAVE CVarGetInteger(CVAR_NAME_PAUSE_SAVE, 0)
#define CVAR_DEBUG_MODE CVarGetInteger(CVAR_NAME_DEBUG_MODE, 0)

/*
 * When the player transitions from one scene to another, the main BGM sequence may fade out the volume, disable the
 * player when the fadeTimer hits 0, then play a new BGM in the next scene. However, certain 2ship features prevent that
 * fadeTimer from decrementing all the way to 0. This prevents the main BGM sequence player from being disabled in
 * AudioScript_SequencePlayerProcessSound. The next sequence will play fine, but gActiveSeqs[SEQ_PLAYER_BGM_MAIN] will
 * be in an invalid state. The result is that attempts to store and then replay the main BGM will play silence, as seen
 * with mini-boss battles.
 *
 * This fix intercepts the point where the a scene BGM change plays and calls AudioScript_SequencePlayerDisable if any
 * of the enhancements in question are enabled and this is a transition where a different BGM will play.
 */

void RegisterFixBgmReplay() {
    COND_VB_SHOULD(VB_PLAY_SCENE_SEQUENCE, CVAR_FASTER_SCENE_TRANSITIONS || CVAR_PAUSE_SAVE || CVAR_DEBUG_MODE, {
        // The Astral Observatory telescope and moon crash set this flag. No need to meddle further.
        if (sStartSeqDisabled) {
            return;
        }

        u16 sRequestedSceneSeqId = *va_arg(args, u16*);
        u16 sPrevMainBgmSeqId = *va_arg(args, u16*);
        u16 seqId = *va_arg(args, u16*);
        // Entering new scene that has different BGM to play. This is the same condition in Audio_PlaySceneSequence.
        if (sRequestedSceneSeqId != seqId &&
            ((seqId != NA_BGM_FINAL_HOURS) || (sPrevMainBgmSeqId == NA_BGM_DISABLED))) {
            AudioScript_SequencePlayerDisable(&gAudioCtx.seqPlayers[SEQ_PLAYER_BGM_MAIN]);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFixBgmReplay, { CVAR_NAME_FASTER_SCENE_TRANSITIONS, CVAR_NAME_PAUSE_SAVE,
                                                             CVAR_NAME_DEBUG_MODE });
