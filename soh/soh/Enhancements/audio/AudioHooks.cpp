#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "AudioCollection.h"
#include <soh/Notification/Notification.h>
#include <soh/SohGui/ImGuiUtils.h>

extern "C" {
#include "variables.h"

extern PlayState* gPlayState;
}

#define CVAR_SEQOVERLAY_NAME CVAR_AUDIO("SeqNameOverlay")
#define CVAR_SEQOVERLAY_DEFAULT 0
#define CVAR_SEQOVERLAY_VALUE CVarGetInteger(CVAR_SEQOVERLAY_NAME, CVAR_SEQOVERLAY_DEFAULT)

void NotifySequenceName(int32_t playerIdx, int32_t seqId) {
    // Keep track of the previous sequence/scene so we don't repeat notifications
    static uint16_t previousSeqId = UINT16_MAX;
    static int16_t previousSceneNum = INT16_MAX;
    if (playerIdx == SEQ_PLAYER_BGM_MAIN &&
        (seqId != previousSeqId || (gPlayState != NULL && gPlayState->sceneNum != previousSceneNum))) {
        
        previousSeqId = seqId;
        if (gPlayState != NULL) {
            previousSceneNum = gPlayState->sceneNum;
        }
        const char* sequenceName = AudioCollection::Instance->GetSequenceName(seqId);
        if (sequenceName != NULL) {
            Notification::Emit({
                .itemIcon = GetTextureForItemId(ITEM_SONG_LULLABY),
                .message = sequenceName,
                .mute = true,
            });
        }
    }
}

void RegisterAudioHooks() {
    COND_HOOK(OnSeqPlayerInit, CVAR_SEQOVERLAY_VALUE, NotifySequenceName);
}

static RegisterShipInitFunc initFunc(RegisterAudioHooks, { CVAR_SEQOVERLAY_NAME });