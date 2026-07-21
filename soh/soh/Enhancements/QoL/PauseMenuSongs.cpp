#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "src/overlays/actors/ovl_En_Okarina_Tag/z_en_okarina_tag.h"
extern PlayState* gPlayState;
u8 Randomizer_GetSettingValue(RandomizerSettingKey);

// Staff-spot (En_Okarina_Tag) idle/listening handlers, not exposed in a header.
void func_80ABEF2C(EnOkarinaTag* tag, PlayState* play);
void func_80ABF0CC(EnOkarinaTag* tag, PlayState* play);
void func_80ABF28C(EnOkarinaTag* tag, PlayState* play);
void func_80ABF4C8(EnOkarinaTag* tag, PlayState* play);
}

static constexpr int32_t CVAR_PAUSE_WARP_DEFAULT = 0;
#define CVAR_PAUSE_WARP_NAME CVAR_ENHANCEMENT("PauseMenuSongs")
#define CVAR_PAUSE_WARP_VALUE CVarGetInteger(CVAR_PAUSE_WARP_NAME, CVAR_PAUSE_WARP_DEFAULT)

// --- Warp songs (QUEST_SONG_MINUET through QUEST_SONG_PRELUDE) ---

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

// --- Non-warp songs (QUEST_SONG_LULLABY through QUEST_SONG_STORMS) ---
// Indexed by (QUEST_SONG_* - QUEST_SONG_LULLABY)

static const int questSongToOcarinaSong[] = {
    OCARINA_SONG_LULLABY, // QUEST_SONG_LULLABY
    OCARINA_SONG_EPONAS,  // QUEST_SONG_EPONA
    OCARINA_SONG_SARIAS,  // QUEST_SONG_SARIA
    OCARINA_SONG_SUNS,    // QUEST_SONG_SUN
    OCARINA_SONG_TIME,    // QUEST_SONG_TIME
    OCARINA_SONG_STORMS,  // QUEST_SONG_STORMS
};

static const int nonWarpSongFanfareMap[] = {
    NA_BGM_OCA_ZELDA, // QUEST_SONG_LULLABY
    NA_BGM_OCA_EPONA, // QUEST_SONG_EPONA
    NA_BGM_OCA_SARIA, // QUEST_SONG_SARIA
    NA_BGM_OCA_SUNS,  // QUEST_SONG_SUN
    NA_BGM_OCA_TIME,  // QUEST_SONG_TIME
    NA_BGM_OCA_STORM, // QUEST_SONG_STORMS
};

// Effect actor IDs indexed by (OCARINA_SONG_* - OCARINA_SONG_SARIAS)
// Mirrors sOcarinaEffectActorIds in z_message_PAL.c
static const int effectActorIds[] = {
    ACTOR_OCEFF_WIPE3, // OCARINA_SONG_SARIAS
    ACTOR_OCEFF_WIPE2, // OCARINA_SONG_EPONAS
    ACTOR_OCEFF_WIPE,  // OCARINA_SONG_LULLABY
    ACTOR_OCEFF_SPOT,  // OCARINA_SONG_SUNS
    ACTOR_OCEFF_WIPE,  // OCARINA_SONG_TIME
    ACTOR_OCEFF_STORM, // OCARINA_SONG_STORMS
};
static const int effectActorParams[] = { 0, 0, 0, 0, 1, 0 };

static bool isWarpActive = false;
static bool isSongActive = false;

static void PauseWarp_Execute() {
    if (!isWarpActive || gPlayState->msgCtx.msgMode != MSGMODE_NONE) {
        return;
    }
    isWarpActive = false;
    GET_PLAYER(gPlayState)->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
    if (gPlayState->msgCtx.choiceIndex != 0) {
        return;
    }

    // Set up respawn destination before spawning the warp actor
    if (IS_RANDO) {
        Entrance_SetWarpSongEntrance();
    } else {
        for (int i = 0; i < ARRAY_COUNT(ocarinaSongMap); i++) {
            if (gPlayState->msgCtx.lastPlayedSong == ocarinaSongMap[i]) {
                gSaveContext.respawn[RESPAWN_MODE_RETURN].entranceIndex = entranceIndexMap[i];
                gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams = 0x5FF;
                gSaveContext.respawn[RESPAWN_MODE_RETURN].data = (s8)gPlayState->msgCtx.lastPlayedSong;
                Interface_SetSubTimerToFinalSecond(gPlayState);
                break;
            }
        }
    }

    // Trigger the warp cutscene (matches Player_Action_8084E3C4's OCARINA_MODE_02 handling)
    Player* player = GET_PLAYER(gPlayState);
    player->csAction = 0;
    player->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
    Player_TryCsAction(gPlayState, NULL, 8);
    gPlayState->mainCamera.unk_14C &= ~8;
    player->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE;
    player->stateFlags2 |= PLAYER_STATE2_OCARINA_PLAYING;

    if (Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0xF) == NULL) {
        Environment_WarpSongLeave(gPlayState);
    }

    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
}

// Fire each in-range staff spot matching the played song by calling its listening handler directly.
// The caller has set ocarinaMode == OCARINA_MODE_03, so the handler runs its full effect/scene logic.
static void PauseSong_ActivateOkarinaTags() {
    Player* player = GET_PLAYER(gPlayState);
    u16 song = gPlayState->msgCtx.lastPlayedSong;
    // Type-7 spots store the song in ocarinaSong as an offset from Saria (Lullaby = 2).
    u8 songIndex = (u8)(song - OCARINA_SONG_SARIAS);

    for (Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_PROP].head; actor != NULL; actor = actor->next) {
        if (actor->id != ACTOR_EN_OKARINA_TAG) {
            continue;
        }
        EnOkarinaTag* tag = (EnOkarinaTag*)actor;
        if ((tag->actor.xzDistToPlayer < (90.0f + tag->interactRange)) &&
            (fabsf(player->actor.world.pos.y - tag->actor.world.pos.y) < 80.0f)) {
            if (tag->actionFunc == func_80ABEF2C && tag->ocarinaSong == songIndex) {
                func_80ABF0CC(tag, gPlayState);
            } else if (tag->actionFunc == func_80ABF28C &&
                       ((song == OCARINA_SONG_LULLABY && (tag->type == 1 || tag->type == 6)) ||
                        (song == OCARINA_SONG_STORMS && tag->type == 2) ||
                        (song == OCARINA_SONG_TIME && tag->type == 4))) {
                func_80ABF4C8(tag, gPlayState);
            }
        }
    }
}

static void PauseSong_Execute() {
    if (!isSongActive || gPlayState->pauseCtx.state != 0 || gPlayState->msgCtx.msgMode != MSGMODE_NONE) {
        return;
    }
    isSongActive = false;

    int song = gPlayState->msgCtx.lastPlayedSong;
    if (song < OCARINA_SONG_SARIAS || song > OCARINA_SONG_STORMS) {
        return;
    }
    int idx = song - OCARINA_SONG_SARIAS;
    if (song == OCARINA_SONG_EPONAS) {
        DREG(53) = 1;
    }
    Player* player = GET_PLAYER(gPlayState);
    Actor_Spawn(&gPlayState->actorCtx, gPlayState, effectActorIds[idx], player->actor.world.pos.x,
                player->actor.world.pos.y, player->actor.world.pos.z, 0, 0, 0, effectActorParams[idx]);

    // Flag the correct-song state, fire matching in-range spots, then return ocarina/message state to
    // rest. The Water Temple triforce leaves msgMode = MSGMODE_PAUSED, which would otherwise block the
    // pause menu (z_play gates on NONE).
    gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_03;
    PauseSong_ActivateOkarinaTags();
    gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_00;
    if (gPlayState->msgCtx.msgMode == MSGMODE_PAUSED) {
        gPlayState->msgCtx.msgMode = MSGMODE_NONE;
    }
}

static void ActivateWarp(PauseContext* pauseCtx, int song) {
    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
    Interface_SetDoAction(gPlayState, DO_ACTION_NONE);
    pauseCtx->state = 0x12;
    WREG(2) = -6240;
    func_800F64E0(0);
    pauseCtx->unk_1E4 = 0;
    int idx = song - QUEST_SONG_MINUET;
    gPlayState->msgCtx.lastPlayedSong = ocarinaSongMap[idx];
    Audio_SetSfxBanksMute(0x20);
    Audio_PlayFanfare(songAudioMap[idx]);
    Message_StartTextbox(gPlayState, songMessageMap[idx], NULL);
    GET_PLAYER(gPlayState)->stateFlags1 |= PLAYER_STATE1_IN_CUTSCENE;
    isWarpActive = true;
}

static void ActivateSong(PauseContext* pauseCtx, int questSong) {
    int idx = questSong - QUEST_SONG_LULLABY;
    Interface_SetDoAction(gPlayState, DO_ACTION_NONE);
    pauseCtx->state = 0x12;
    WREG(2) = -6240;
    func_800F64E0(0);
    pauseCtx->unk_1E4 = 0;
    gPlayState->msgCtx.lastPlayedSong = questSongToOcarinaSong[idx];
    // Intentionally no Audio_SetSoundBanksMute(0x20): it mutes BANK_OCARINA and is only cleared via
    // AudioOcarina_SetInstrument(OFF), which this in-scene path never hits, silencing the next real ocarina.
    Audio_PlayFanfare(nonWarpSongFanfareMap[idx]);
    isSongActive = true;
}

static void PauseMenuSongs_HandleSelection() {
    if (gSaveContext.inventory.items[SLOT_OCARINA] == ITEM_NONE) {
        return;
    }
    int aButtonPressed = CHECK_BTN_ALL(gPlayState->state.input->press.button, BTN_A);
    int song = gPlayState->pauseCtx.cursorPoint[PAUSE_QUEST];
    if (!aButtonPressed || !CHECK_QUEST_ITEM(song) || gPlayState->pauseCtx.pageIndex != PAUSE_QUEST ||
        gPlayState->pauseCtx.state != 6) {
        return;
    }

    if (song >= QUEST_SONG_MINUET && song <= QUEST_SONG_PRELUDE) {
        if (gSaveContext.ship.quest.id == QUEST_RANDOMIZER && Randomizer_GetSettingValue(RSK_SHUFFLE_OCARINA_BUTTONS)) {
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
        ActivateWarp(&gPlayState->pauseCtx, song);
    } else if (song >= QUEST_SONG_LULLABY && song <= QUEST_SONG_STORMS) {
        ActivateSong(&gPlayState->pauseCtx, song);
    }
}

static void RegisterPauseMenuHooks() {
    COND_HOOK(OnKaleidoUpdate, CVAR_PAUSE_WARP_VALUE, [] {
        if (GameInteractor::IsSaveLoaded()) {
            PauseMenuSongs_HandleSelection();
        }
    });
    COND_HOOK(OnGameFrameUpdate, CVAR_PAUSE_WARP_VALUE, [] {
        if (GameInteractor::IsSaveLoaded()) {
            PauseWarp_Execute();
            PauseSong_Execute();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterPauseMenuHooks, { CVAR_PAUSE_WARP_NAME });
