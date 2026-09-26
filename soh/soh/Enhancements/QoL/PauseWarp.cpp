#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
u8 Randomizer_GetSettingValue(RandomizerSettingKey);
}

static constexpr int32_t CVAR_PAUSE_WARP_DEFAULT = 0;
#define CVAR_PAUSE_WARP_NAME CVAR_ENHANCEMENT("PauseWarp")
#define CVAR_PAUSE_WARP_VALUE CVarGetInteger(CVAR_PAUSE_WARP_NAME, CVAR_PAUSE_WARP_DEFAULT)

static const int songMessageMap[] = {
    TEXT_WARP_MINUET_OF_FOREST,  TEXT_WARP_BOLERO_OF_FIRE,     TEXT_WARP_SERENADE_OF_WATER,
    TEXT_WARP_REQUIEM_OF_SPIRIT, TEXT_WARP_NOCTURNE_OF_SHADOW, TEXT_WARP_PRELUDE_OF_LIGHT,
};

static const int ocarinaSongMap[] = {
    OCARINA_SONG_MINUET,  OCARINA_SONG_BOLERO,   OCARINA_SONG_SERENADE,
    OCARINA_SONG_REQUIEM, OCARINA_SONG_NOCTURNE, OCARINA_SONG_PRELUDE,
};

static const int entranceIndexMap[] = {
    ENTR_SACRED_FOREST_MEADOW_WARP_PAD,  // Minuet
    ENTR_DEATH_MOUNTAIN_CRATER_WARP_PAD, // Bolero
    ENTR_LAKE_HYLIA_WARP_PAD,            // Serenade
    ENTR_DESERT_COLOSSUS_WARP_PAD,       // Requiem
    ENTR_GRAVEYARD_WARP_PAD,             // Nocturne
    ENTR_TEMPLE_OF_TIME_WARP_PAD,        // Prelude
};

static const int songAudioMap[] = {
    NA_BGM_OCA_MINUET,  NA_BGM_OCA_BOLERO,   NA_BGM_OCA_SERENADE,
    NA_BGM_OCA_REQUIEM, NA_BGM_OCA_NOCTURNE, NA_BGM_OCA_LIGHT,
};

static bool isWarpActive = false;

// A song picked while the ocarina momentarily can't come out, e.g. the previous song's state still settling,
// which only advances once unpaused.
static int retrySong = 0;
static int retryTimer = 0;
static constexpr int OCARINA_RETRY_GRACE_FRAMES = 6;

static bool isCannotPlayActive = false;
static constexpr int CANNOT_PLAY_FADE_FRAMES = 60;

static CustomMessage cannotPlayHereMsg = CustomMessage(
    "Can't play the Ocarina here!" + CustomMessage::MESSAGE_END(),
    "Hier kann die Okarina" + CustomMessage::NEWLINE() + "nicht gespielt werden!" + CustomMessage::MESSAGE_END(),
    "Impossible de jouer" + CustomMessage::NEWLINE() + "de l'ocarina ici !" + CustomMessage::MESSAGE_END());

static void PauseWarp_Execute() {
    if (!isWarpActive) {
        return;
    }
    Player* player = GET_PLAYER(gPlayState);
    if (gPlayState->msgCtx.msgMode != MSGMODE_NONE) {
        // A running Link changes action as he stops, and every action change clears this flag.
        player->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
        return;
    }
    isWarpActive = false;
    player->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
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
            Interface_SetSubTimerToFinalSecond(gPlayState);
            return;
        }
    }
    gPlayState->transitionTrigger = TRANS_TRIGGER_OFF;
}

static void ClosePauseMenu(PauseContext* pauseCtx) {
    Interface_SetDoAction(gPlayState, DO_ACTION_NONE);
    pauseCtx->state = 0x12;
    WREG(2) = -6240;
    func_800F64E0(0);
    pauseCtx->unk_1E4 = 0;
}

static void StartWarpPlayback(int song) {
    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
    int idx = song - QUEST_SONG_MINUET;
    gPlayState->msgCtx.lastPlayedSong = ocarinaSongMap[idx];
    Audio_SetSfxBanksMute(0x20);
    Audio_PlayFanfare(songAudioMap[idx]);
    // Where warping is blocked the song still plays, then refuses, as it does from the ocarina.
    if (gPlayState->msgCtx.disableWarpSongs || (gPlayState->interfaceCtx.restrictions.warpSongs == 3 && !IS_RANDO)) {
        Message_StartTextbox(gPlayState, TEXT_CANNOT_WARP_HERE, nullptr);
        return;
    }
    Message_StartTextbox(gPlayState, songMessageMap[idx], nullptr);
    GET_PLAYER(gPlayState)->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    isWarpActive = true;
}

// The footing the vanilla ocarina itself needs (cf. Player_ActionHandler_13). Holding Link for the prompt
// anywhere else, mid-air or swimming, would suspend his physics.
static bool CanPlayOcarina() {
    Player* player = GET_PLAYER(gPlayState);
    if (gPlayState->msgCtx.msgMode != MSGMODE_NONE || gPlayState->msgCtx.ocarinaMode != OCARINA_MODE_00) {
        return false;
    }
    if (gPlayState->interfaceCtx.restrictions.ocarina != 0) {
        return false;
    }
    if (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE |
                               PLAYER_STATE1_TALKING | PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_CARRYING_ACTOR)) {
        return false;
    }
    if (player->stateFlags2 & PLAYER_STATE2_OCARINA_PLAYING) {
        return false;
    }
    return (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) != 0;
}

static void PauseWarp_RetryPlay() {
    if (retryTimer <= 0 || gPlayState->pauseCtx.state != 0 || gPlayState->msgCtx.msgMode != MSGMODE_NONE) {
        return;
    }
    if (CanPlayOcarina()) {
        retryTimer = 0;
        StartWarpPlayback(retrySong);
    } else if (--retryTimer == 0) {
        Message_StartTextbox(gPlayState, TEXT_CANNOT_PLAY_OCARINA_MSG, nullptr);
        isCannotPlayActive = true;
    }
}

// Fades on its own, like a cutscene message, instead of waiting on A.
static void PauseWarp_FadeCannotPlay() {
    if (!isCannotPlayActive) {
        return;
    }
    MessageContext* msgCtx = &gPlayState->msgCtx;
    if (msgCtx->msgMode == MSGMODE_TEXT_DONE && msgCtx->textboxEndType == TEXTBOX_ENDTYPE_DEFAULT) {
        msgCtx->textboxEndType = TEXTBOX_ENDTYPE_FADING;
        msgCtx->stateTimer = CANNOT_PLAY_FADE_FRAMES;
    } else if (msgCtx->msgMode == MSGMODE_NONE) {
        isCannotPlayActive = false;
    }
}

static void PauseWarp_HandleSelection() {
    if (gSaveContext.inventory.items[SLOT_OCARINA] != ITEM_NONE) {
        int aButtonPressed = CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_A);
        int song = gPlayState->pauseCtx.cursorPoint[PAUSE_QUEST];
        if (aButtonPressed && CHECK_QUEST_ITEM(song) && song >= QUEST_SONG_MINUET && song <= QUEST_SONG_PRELUDE &&
            gPlayState->pauseCtx.pageIndex == PAUSE_QUEST && gPlayState->pauseCtx.state == 6 &&
            gPlayState->pauseCtx.cursorSpecialPos == 0) {
            if (gSaveContext.ship.quest.id == QUEST_RANDOMIZER &&
                Randomizer_GetSettingValue(RSK_SHUFFLE_OCARINA_BUTTONS)) {
                bool canplay = false;
                switch (song) {
                    case QUEST_SONG_MINUET:
                        canplay = Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_A) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_LEFT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_RIGHT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_UP);
                        break;
                    case QUEST_SONG_BOLERO:
                        canplay = Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_A) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_RIGHT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_DOWN);
                        break;
                    case QUEST_SONG_SERENADE:
                        canplay = Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_A) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_LEFT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_RIGHT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_DOWN);
                        break;
                    case QUEST_SONG_REQUIEM:
                        canplay = Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_A) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_RIGHT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_DOWN);
                        break;
                    case QUEST_SONG_NOCTURNE:
                        canplay = Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_A) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_LEFT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_RIGHT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_DOWN);
                        break;
                    case QUEST_SONG_PRELUDE:
                        canplay = Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_LEFT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_RIGHT) &&
                                  Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_UP);
                        break;
                }
                if (!canplay) {
                    return;
                }
            }
            ClosePauseMenu(&gPlayState->pauseCtx);
            if (CanPlayOcarina()) {
                StartWarpPlayback(song);
            } else {
                retrySong = song;
                retryTimer = OCARINA_RETRY_GRACE_FRAMES;
            }
        }
    }
}

static void RegisterPauseMenuHooks() {
    COND_HOOK(OnKaleidoUpdate, CVAR_PAUSE_WARP_VALUE, [] {
        if (GameInteractor::IsSaveLoaded()) {
            PauseWarp_HandleSelection();
        }
    });
    COND_HOOK(OnGameFrameUpdate, CVAR_PAUSE_WARP_VALUE, [] {
        if (GameInteractor::IsSaveLoaded()) {
            PauseWarp_Execute();
            PauseWarp_RetryPlay();
            PauseWarp_FadeCannotPlay();
        }
    });
    COND_ID_HOOK(OnOpenText, TEXT_CANNOT_PLAY_OCARINA_MSG, CVAR_PAUSE_WARP_VALUE,
                 [](uint16_t* textId, bool* loadFromMessageTable) {
                     cannotPlayHereMsg.LoadIntoFont();
                     *loadFromMessageTable = false;
                 });
}

static RegisterShipInitFunc initFunc(RegisterPauseMenuHooks, { CVAR_PAUSE_WARP_NAME });
