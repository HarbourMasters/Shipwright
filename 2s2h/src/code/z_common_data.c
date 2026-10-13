#include "z64save.h"

#include "alignment.h"
#include "sequence.h"
#include "libc/stdbool.h"
#include "z64environment.h"
#include "z64transition.h"
#include <string.h>

SaveContext gSaveContext ALIGNED(16);

void SaveContext_Init(void) {
    memset(&gSaveContext, 0, sizeof(SaveContext));

    gSaveContext.save.playerForm = 0;
    gSaveContext.seqId = NA_BGM_DISABLED;
    gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
    gSaveContext.forcedSeqId = NA_BGM_GENERAL_SFX;
    gSaveContext.nextCutsceneIndex = 0xFFEF;
    gSaveContext.cutsceneTrigger = 0;
    gSaveContext.chamberCutsceneNum = 0;
    gSaveContext.nextDayTime = NEXT_TIME_NONE;
    gSaveContext.skyboxTime = 0;
    gSaveContext.dogIsLost = true;
    gSaveContext.nextTransitionType = TRANS_NEXT_TYPE_DEFAULT;
    gSaveContext.prevHudVisibility = HUD_VISIBILITY_ALL;

    gSaveContext.options.language = LANGUAGE_ENG;
    gSaveContext.options.audioSetting = SAVE_AUDIO_STEREO;
    gSaveContext.options.zTargetSetting = 0;
}
