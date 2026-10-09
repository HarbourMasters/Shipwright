#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/BenPort.h"
#include "2s2h/BenGui/BenMenu.h"
#include "2s2h/Enhancements/Audio/AudioCollection.h"
#include <2s2h/BenGui/Notification.h>

extern "C" {
#include "variables.h"

extern PlayState* gPlayState;
}

#define CVAR_SEQOVERLAY_NAME "gAudioEditor.SeqNameNotification"
#define CVAR_SEQOVERLAY_DEFAULT 0
#define CVAR_SEQOVERLAY_VALUE CVarGetInteger(CVAR_SEQOVERLAY_NAME, CVAR_SEQOVERLAY_DEFAULT)

void NotifySequenceName(int32_t playerIdx, int32_t seqId) {
    // Keep track of the previous sequence/scene so we don't repeat notifications
    static uint16_t previousSeqId = UINT16_MAX;
    static int16_t previousSceneNum = INT16_MAX;

    if (playerIdx == SEQ_PLAYER_BGM_MAIN &&
        (seqId != previousSeqId || (gPlayState != NULL && gPlayState->sceneId != previousSceneNum))) {

        previousSeqId = seqId;
        if (gPlayState != NULL) {
            previousSceneNum = gPlayState->sceneId;
        }
        const char* sequenceName = AudioCollection::Instance->GetSequenceName(seqId);
        if (sequenceName != NULL) {
            Notification::Emit({
                .message = ICON_FA_MUSIC " " + std::string(sequenceName),
                .remainingTime = static_cast<float>(CVarGetInteger("gAudioEditor.SeqNameNotificationDuration", 10)),
                .mute = true,
            });
        }
    }
}

void RegisterAudioNotificationHooks() {
    COND_HOOK(OnSeqPlayerInit, CVAR_SEQOVERLAY_VALUE, NotifySequenceName);
}

static RegisterShipInitFunc initFunc(RegisterAudioNotificationHooks, { CVAR_SEQOVERLAY_NAME });