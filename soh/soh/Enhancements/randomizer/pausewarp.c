// Importing necessary libraries and headers
#include <stdbool.h>
#include <stdint.h>
#include "soh/Enhancements/gameconsole.h"
#include "global.h"
#include "../custom-message/CustomMessageTypes.h"

// Defining the structure for the pause warp state, which holds different flags and cooldowns
typedef struct {
    bool warpInitiated, textboxInitiated, inChoosingState, textboxIsOpen, isTextboxClosing;
    int aButtonCooldown, textboxCheckCooldown, textboxOpenCooldown;
} PauseWarpState;

// Mapping the song messages. Each song corresponds to a specific text message.
static const int songMessageMap[] = { TEXT_WARP_MINUET_OF_FOREST, TEXT_WARP_BOLERO_OF_FIRE, TEXT_WARP_SERENADE_OF_WATER, TEXT_WARP_REQUIEM_OF_SPIRIT, TEXT_WARP_NOCTURNE_OF_SHADOW, TEXT_WARP_PRELUDE_OF_LIGHT };

// Mapping the song audio. Each song corresponds to a specific audio ID.
static const int songAudioMap[] = { NA_BGM_OCA_MINUET, NA_BGM_OCA_BOLERO, NA_BGM_OCA_SERENADE, NA_BGM_OCA_REQUIEM, NA_BGM_OCA_NOCTURNE, NA_BGM_OCA_LIGHT };

// Forward declaring the functions used in this file
bool IsStateValid(PlayState* play, Player* player, PauseWarpState* state);
void ResetStateFlags(PauseWarpState* state);
void InitiateWarp(PlayState* play, Player* player, int song, PauseWarpState* state);
void HandleWarpConfirmation(PlayState* play, Player* player, PauseWarpState* state);
void HandleCooldowns(PauseWarpState* state);

// Checking if the state is valid. This is a sanity check to ensure we're not operating on null pointers.
bool IsStateValid(PlayState* play, Player* player, PauseWarpState* state) {
    return play && player && state;
}

// Resetting all the flags in the state to their default values (which is mostly 'false' for booleans and '0' for integers)
void ResetStateFlags(PauseWarpState* state) {
    *state = (PauseWarpState){0};
}

// Initiating the warp process. Here we set all the required flags and disable player input.
void InitiateWarp(PlayState* play, Player* player, int song, PauseWarpState* state) {
    int idx = song - QUEST_SONG_MINUET;  // Calculating the song index
    Audio_SetSoundBanksMute(0x20);       //Mute current BGM
    Audio_PlayFanfare(songAudioMap[idx]);  // Play the corresponding fanfare
    play->msgCtx.lastPlayedSong = idx;   // Storing the last played song
    play->pauseCtx.state = 0x12;         // Setting the pause state
    Message_StartTextbox(play, songMessageMap[idx], NULL);  // Starting the textbox with the appropriate message
    player->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;  // Disabling player input
    *state = (PauseWarpState){.warpInitiated = true, .textboxOpenCooldown = 10, .aButtonCooldown = 10, .textboxCheckCooldown = 5, .textboxIsOpen = true};  // Setting the flags for warp
    play->msgCtx.choiceIndex = 0;  // Resetting the choice index
}

// Handling the warp confirmation. This is the part where the player actually gets teleported if they confirmed.
void HandleWarpConfirmation(PlayState* play, Player* player, PauseWarpState* state) {
    if (play->msgCtx.choiceIndex == 0) Entrance_SetWarpSongEntrance();  // Teleporting the player if 'ok' was selected
    player->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;  // Re-enabling player input
    ResetStateFlags(state);  // Resetting the state flags
}

// Managing the cooldowns for different actions and transitions
void HandleCooldowns(PauseWarpState* state) {
    // Decreasing the cooldowns if they're greater than zero
    if (state->aButtonCooldown > 0) state->aButtonCooldown--;
    if (state->textboxCheckCooldown > 0) state->textboxCheckCooldown--;
    if (state->textboxOpenCooldown > 0) state->textboxOpenCooldown--;

    // If the textbox is closing, reset the flag
    if (state->isTextboxClosing) *state = (PauseWarpState){.isTextboxClosing = false};
}

// The main function that gets called every frame
void PauseWarp_Main() {
    static PauseWarpState state;  // The state is static so it retains its value between function calls

    // Checking if the pause warp feature is enabled
    int pauseWarpEnabled = CVarGetInteger("gPauseWarpEnabled", 0);
    PlayState* play = gPlayState;
    Player* player = play ? GET_PLAYER(play) : NULL;

    // If pause warp is not enabled or the state is invalid, reset the state and exit
    if (!pauseWarpEnabled || !IsStateValid(play, player, &state)) return ResetStateFlags(&state);

    // Check if a Ocarina in the Ocarina slot
    if (gSaveContext.inventory.items[SLOT_OCARINA] == ITEM_NONE) return ResetStateFlags(&state);

    // Checking if the 'A' button is pressed
    int aButtonPressed = CHECK_BTN_ALL(play->state.input->press.button, BTN_A);

    // If 'A' is pressed and the cooldowns are zero, and we're not already warping, initiate the warp
    if (aButtonPressed && !state.aButtonCooldown && !(state.warpInitiated || state.textboxInitiated || state.inChoosingState)) {
        int song = play->pauseCtx.cursorPoint[PAUSE_QUEST];
        
        // Check if the player has the selected warp song
        if (!CHECK_QUEST_ITEM(song)) return;

        // Initiate warp if the song is within the valid range
        if (song >= QUEST_SONG_MINUET && song <= QUEST_SONG_PRELUDE) InitiateWarp(play, player, song, &state);
    }

    // Depending on the message mode, update the state flags
    switch (play->msgCtx.msgMode) {
        case 6: if (state.warpInitiated) state.textboxInitiated = true; break;
        case 53: if (state.warpInitiated && state.textboxInitiated) state.inChoosingState = true; break;
        case 54: if (state.warpInitiated && state.textboxInitiated && state.inChoosingState) HandleWarpConfirmation(play, player, &state); break;
        case 0: ResetStateFlags(&state); break;
    }

    // Finally, handle any cooldowns for the next frame
    HandleCooldowns(&state);
}