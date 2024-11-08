#include "custom-message/CustomMessageTypes.h"
#include "global.h"
#include "z64.h"
#include "game-interactor/GameInteractor.h"

static const int songMessageMap[] = { 
    TEXT_WARP_MINUET_OF_FOREST, 
    TEXT_WARP_BOLERO_OF_FIRE, 
    TEXT_WARP_SERENADE_OF_WATER, 
    TEXT_WARP_REQUIEM_OF_SPIRIT, 
    TEXT_WARP_NOCTURNE_OF_SHADOW, 
    TEXT_WARP_PRELUDE_OF_LIGHT 
};

static const int ocarinaSongMap[] = {
    OCARINA_SONG_MINUET, 
    OCARINA_SONG_BOLERO, 
    OCARINA_SONG_SERENADE, 
    OCARINA_SONG_REQUIEM, 
    OCARINA_SONG_NOCTURNE, 
    OCARINA_SONG_PRELUDE
};

static const int entranceIndexMap[] = {
    ENTR_SACRED_FOREST_MEADOW_WARP_PAD, // Minuet
    ENTR_DEATH_MOUNTAIN_CRATER_WARP_PAD, // Bolero
    ENTR_LAKE_HYLIA_WARP_PAD, // Serenade
    ENTR_DESERT_COLOSSUS_WARP_PAD, // Requiem
    ENTR_GRAVEYARD_WARP_PAD, // Nocturne
    ENTR_TEMPLE_OF_TIME_WARP_PAD // Prelude
};

static const int songAudioMap[] = { 
    NA_BGM_OCA_MINUET, 
    NA_BGM_OCA_BOLERO, 
    NA_BGM_OCA_SERENADE, 
    NA_BGM_OCA_REQUIEM, 
    NA_BGM_OCA_NOCTURNE, 
    NA_BGM_OCA_LIGHT 
};

static bool isWarpActive = false; 

void PauseWarp_Execute() {
    if (!isWarpActive || gPlayState->msgCtx.msgMode != MSGMODE_NONE) {
        return;
    }
    isWarpActive = false; 
    GET_PLAYER(gPlayState)->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
    if (gPlayState->msgCtx.choiceIndex != 0) {
        return;
    }
    if (IS_RANDO) {
        Entrance_SetWarpSongEntrance();
        return;
    }
    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
    for (int i = 0; i < ARRAY_COUNT(ocarinaSongMap); i++) {
        if (gPlayState->msgCtx.lastPlayedSong == ocarinaSongMap[i]) {
            gPlayState->nextEntranceIndex = entranceIndexMap[i];
            return;
        }
    }
    gPlayState->transitionTrigger = TRANS_TRIGGER_OFF;
}

void ActivateWarp(PauseContext* pauseCtx, int song) {
    Audio_OcaSetInstrument(0);
    Interface_SetDoAction(gPlayState, DO_ACTION_NONE);
    pauseCtx->state = 0x12;
    WREG(2) = -6240;
    func_800F64E0(0);
    pauseCtx->unk_1E4 = 0;
    int idx = song - QUEST_SONG_MINUET;
    gPlayState->msgCtx.lastPlayedSong = ocarinaSongMap[idx];
    Audio_SetSoundBanksMute(0x20);
    Audio_PlayFanfare(songAudioMap[idx]);
    Message_StartTextbox(gPlayState, songMessageMap[idx], NULL);
    GET_PLAYER(gPlayState)->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    isWarpActive = true;
}

void PauseWarp_HandleSelection() {
    if (gSaveContext.inventory.items[SLOT_OCARINA] != ITEM_NONE) {
        int aButtonPressed = CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_A);
        int song = gPlayState->pauseCtx.cursorPoint[PAUSE_QUEST];
        if (aButtonPressed && CHECK_QUEST_ITEM(song) && song >= QUEST_SONG_MINUET && song <= QUEST_SONG_PRELUDE &&
            gPlayState->pauseCtx.pageIndex == PAUSE_QUEST && gPlayState->pauseCtx.state == 6) {
            ActivateWarp(&gPlayState->pauseCtx, song);
        }
    }
}
