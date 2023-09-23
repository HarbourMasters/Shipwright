#include <stdbool.h>
#include <stdint.h>
#include "soh/Enhancements/gameconsole.h"
#include "global.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"

typedef struct {
    bool warpInitiated, textboxInitiated, inChoosingState, textboxIsOpen, isTextboxClosing;
    int aButtonCooldown, textboxCheckCooldown, textboxOpenCooldown;
} PauseWarpState;

static const int songMessageMap[] = { TEXT_WARP_MINUET_OF_FOREST, TEXT_WARP_BOLERO_OF_FIRE, TEXT_WARP_SERENADE_OF_WATER, TEXT_WARP_REQUIEM_OF_SPIRIT, TEXT_WARP_NOCTURNE_OF_SHADOW, TEXT_WARP_PRELUDE_OF_LIGHT };
static const int songAudioMap[] = { NA_BGM_OCA_MINUET, NA_BGM_OCA_BOLERO, NA_BGM_OCA_SERENADE, NA_BGM_OCA_REQUIEM, NA_BGM_OCA_NOCTURNE, NA_BGM_OCA_LIGHT };

extern bool IsSaveLoadedWrapper();

static PauseWarpState state;

static int lastPlayedSong = -1;

bool IsPauseWarpEnabled() {
    return CVarGetInteger("gPauseWarpEnabled", 0) && IsSaveLoadedWrapper();
}

void ResetStateFlags(PauseWarpState* state) {
    *state = (PauseWarpState){0};
}

void InitiateWarp(PlayState* play, Player* player, int song) {
    int idx = song - QUEST_SONG_MINUET;
    Audio_SetSoundBanksMute(0x20);
    Audio_PlayFanfare(songAudioMap[idx]);
    play->msgCtx.lastPlayedSong = idx;
    play->pauseCtx.state = 0x12;
    Message_StartTextbox(play, songMessageMap[idx], NULL);
    player->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    state = (PauseWarpState){.warpInitiated = true, .textboxOpenCooldown = 10, .aButtonCooldown = 10, .textboxCheckCooldown = 5, .textboxIsOpen = true};
    play->msgCtx.choiceIndex = 0;
}

void HandleWarpConfirmation(PlayState* play, Player* player) {
    if (play->msgCtx.choiceIndex == 0) Entrance_SetWarpSongEntrance();
    player->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
    ResetStateFlags(&state);
}

void HandleCooldowns() {
    if (state.aButtonCooldown > 0) state.aButtonCooldown--;
    if (state.textboxCheckCooldown > 0) state.textboxCheckCooldown--;
    if (state.textboxOpenCooldown > 0) state.textboxOpenCooldown--;
    if (state.isTextboxClosing) ResetStateFlags(&state);
}

void PauseWarp_Main() {
    if (!IsPauseWarpEnabled()) return;
    
    PlayState* play = gPlayState;
    Player* player = GET_PLAYER(play);
    int aButtonPressed = CHECK_BTN_ALL(play->state.input->press.button, BTN_A);

    if (aButtonPressed && !state.aButtonCooldown && !(state.warpInitiated || state.textboxInitiated || state.inChoosingState)) {
        int song = play->pauseCtx.cursorPoint[PAUSE_QUEST];
        if (CHECK_QUEST_ITEM(song) && song >= QUEST_SONG_MINUET && song <= QUEST_SONG_PRELUDE) {
            InitiateWarp(play, player, song);
        }
    }
}

void PauseWarp_Idle() {
    if (!IsPauseWarpEnabled()) return;
    
    PlayState* play = gPlayState;
    Player* player = GET_PLAYER(play);

    if (gSaveContext.inventory.items[SLOT_OCARINA] == ITEM_NONE) return ResetStateFlags(&state);

    int currentSong = play->pauseCtx.cursorPoint[PAUSE_QUEST];
    
    switch (play->msgCtx.msgMode) {
        case 6: 
            if (state.warpInitiated) state.textboxInitiated = true; 
            break;
        case 53: 
            if (state.warpInitiated && state.textboxInitiated) state.inChoosingState = true; 
            break;
        case 54: 
            if (state.warpInitiated && state.textboxInitiated && state.inChoosingState) {
                HandleWarpConfirmation(play, player); 
                lastPlayedSong = currentSong; // Update the last played song
            }
            break;
        case 0: 
            if (currentSong == lastPlayedSong) {
                // Reset state flags if the same song is selected twice
                ResetStateFlags(&state);
            }
            break;
    }

    HandleCooldowns();
}