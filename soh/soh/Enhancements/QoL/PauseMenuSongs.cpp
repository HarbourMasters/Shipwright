#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "src/overlays/actors/ovl_En_Okarina_Tag/z_en_okarina_tag.h"
#include "src/overlays/actors/ovl_En_Md/z_en_md.h"
#include "src/overlays/actors/ovl_En_Du/z_en_du.h"
#include "src/overlays/actors/ovl_En_Ma2/z_en_ma2.h"
#include "src/overlays/actors/ovl_Obj_Timeblock/z_obj_timeblock.h"
#include "src/overlays/actors/ovl_Shot_Sun/z_shot_sun.h"
extern PlayState* gPlayState;
u8 Randomizer_GetSettingValue(RandomizerSettingKey);

// Staff-spot (En_Okarina_Tag) idle/listening handlers. These decomp functions have no descriptive names
// yet, so we reference them by their raw symbols:
//   func_80ABEF2C - idle, waiting for the player to approach
//   func_80ABF28C - listening for the ocarina
//   func_80ABF0CC - waiting for the type-7 playback result
//   func_80ABF4C8 - handling a matched song event (types 1/2/4/6)
void func_80ABEF2C(EnOkarinaTag* tag, PlayState* play);
void func_80ABF28C(EnOkarinaTag* tag, PlayState* play);
void func_80ABF0CC(EnOkarinaTag* tag, PlayState* play);
void func_80ABF4C8(EnOkarinaTag* tag, PlayState* play);

// NPC ocarina idle/listen handlers (Mido, Darunia, adult Malon), not exposed in a header.
void EnMd_BlockPath(EnMd* actor, PlayState* play);
void EnMd_ListenToOcarina(EnMd* actor, PlayState* play);
void func_809FE3C0(EnDu* actor, PlayState* play); // Darunia: idle, waiting for the ocarina
void func_809FE4A4(EnDu* actor, PlayState* play); // Darunia: listening for Saria's Song
void EnMa2_WaitForOcarina(EnMa2* actor, PlayState* play);
void EnMa2_WaitForEponasSong(EnMa2* actor, PlayState* play);
void Player_StartTalking(PlayState* play, Actor* actor);

// Song of Time block (Obj_Timeblock) and Great Fairy spawner (Shot_Sun) observers, not exposed in a header.
u8 ObjTimeblock_PlayerIsInRange(ObjTimeblock* timeblock, PlayState* play);
s32 ObjTimeblock_WaitForOcarina(ObjTimeblock* timeblock, PlayState* play);
s32 ObjTimeblock_WaitForSong(ObjTimeblock* timeblock, PlayState* play);
void ShotSun_UpdateFairySpawner(ShotSun* shotSun, PlayState* play);
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
// Returns true if a spot was fired.
static bool PauseSong_ActivateOkarinaTags() {
    Player* player = GET_PLAYER(gPlayState);
    u16 song = gPlayState->msgCtx.lastPlayedSong;
    // Type-7 spots store the song in ocarinaSong as an offset from Saria (Lullaby = 2).
    u8 songIndex = (u8)(song - OCARINA_SONG_SARIAS);
    bool matched = false;

    for (Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_PROP].head; actor != NULL; actor = actor->next) {
        if (actor->id != ACTOR_EN_OKARINA_TAG) {
            continue;
        }
        EnOkarinaTag* tag = (EnOkarinaTag*)actor;
        if ((tag->actor.xzDistToPlayer < (90.0f + tag->interactRange)) &&
            (fabsf(player->actor.world.pos.y - tag->actor.world.pos.y) < 80.0f)) {
            if (tag->actionFunc == func_80ABEF2C && tag->ocarinaSong == songIndex) {
                func_80ABF0CC(tag, gPlayState); // waiting for the playback result
                matched = true;
            } else if (tag->actionFunc == func_80ABF28C) { // listening for the ocarina
                // Type 1/6 react to Zelda's Lullaby, type 2 to Song of Storms, type 4 to Song of Time.
                bool songMatchesType = (((tag->type == 1) || (tag->type == 6)) && (song == OCARINA_SONG_LULLABY)) ||
                                       ((tag->type == 2) && (song == OCARINA_SONG_STORMS)) ||
                                       ((tag->type == 4) && (song == OCARINA_SONG_TIME));
                if (songMatchesType) {
                    // Like the type-7 spots, run the listening handler now while MODE_03 is set so it fires
                    // this frame (its actionFunc is the ocarina-listening state, so it consumes it).
                    func_80ABF4C8(tag, gPlayState); // handle the matched song event
                    matched = true;
                }
            }
        }
    }
    return matched;
}

// --- Deferred hand-off for the actors that can't be driven inline ---
// The staff spots above are safe to run synchronously, but the Song of Time blocks, Great Fairy spawners,
// and the ocarina NPCs (Mido, Darunia, adult Malon) all update *before* this hook and simply poll
// ocarinaMode + lastPlayedSong. Going through the vanilla play-for-actor flow instead
// (PLAYER_STATE2_ATTEMPT_PLAY_FOR_ACTOR) would open a real note-input prompt the player can't satisfy from
// the menu. So for all of them we push the matching in-range actors straight into their song-finished
// state, leave the matching ocarinaMode set, and hold it a couple frames for them to read; then we restore
// it. Mido is the one actor whose follow-up dialogue the player would normally start, so we do that here
// once it has reacted.
static u8 pendingMode = OCARINA_MODE_00;
static Actor* pendingTalkActor = NULL;
static int pendingTimer = 0;

static void PauseSong_HoldMode(u8 mode) {
    gPlayState->msgCtx.ocarinaMode = mode;
    pendingMode = mode;
    pendingTimer = 2;
}

// Restores the held ocarina/message state once the actor has had its frame(s) to read it. Returns true
// while a hand-off is in flight, so the caller skips starting another song. The NPCs flip ocarinaMode to
// MODE_04 when they react, which is how we notice a hand-off completed early.
static bool PauseSong_AdvancePending() {
    if (pendingTimer <= 0) {
        return false;
    }
    bool reacted = gPlayState->msgCtx.ocarinaMode != pendingMode;
    if (--pendingTimer == 0 || reacted) {
        if (pendingTalkActor != NULL && reacted && pendingTalkActor->textId != 0) {
            Player_StartTalking(gPlayState, pendingTalkActor);
        }
        if (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_03 || gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_00;
        }
        if (gPlayState->msgCtx.msgMode == MSGMODE_PAUSED) {
            gPlayState->msgCtx.msgMode = MSGMODE_NONE;
        }
        pendingMode = OCARINA_MODE_00;
        pendingTalkActor = NULL;
        pendingTimer = 0;
    }
    return true;
}

// Hand the played song to a matching in-range NPC. Returns true if one was engaged, leaving MODE_03 held
// for it to consume next frame.
static bool PauseSong_ActivateNpcActors() {
    u16 song = gPlayState->msgCtx.lastPlayedSong;
    s16 scene = gPlayState->sceneNum;

    for (Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].head; actor != NULL; actor = actor->next) {
        // Mido, in the Lost Woods, moves aside for Saria's Song, then offers a dialogue we must start.
        if (actor->id == ACTOR_EN_MD && scene == SCENE_LOST_WOODS && song == OCARINA_SONG_SARIAS) {
            EnMd* mido = (EnMd*)actor;
            if (mido->actionFunc == EnMd_BlockPath && mido->interactInfo.talkState == NPC_TALK_STATE_IDLE &&
                actor->xzDistToPlayer < 100.0f) {
                mido->actionFunc = EnMd_ListenToOcarina;
                pendingTalkActor = actor;
                PauseSong_HoldMode(OCARINA_MODE_03);
                return true;
            }
        }
        // Darunia, in Goron City as a child, dances (a cutscene) for Saria's Song.
        if (actor->id == ACTOR_EN_DU && scene == SCENE_GORON_CITY && song == OCARINA_SONG_SARIAS) {
            EnDu* darunia = (EnDu*)actor;
            if (darunia->actionFunc == func_809FE3C0 && actor->xzDistToPlayer < 120.0f) {
                darunia->actionFunc = func_809FE4A4;
                PauseSong_HoldMode(OCARINA_MODE_03);
                return true;
            }
        }
        // Adult Malon, at Lon Lon Ranch, reacts to Epona's Song (she starts her own dialogue).
        if (actor->id == ACTOR_EN_MA2 && scene == SCENE_LON_LON_RANCH && song == OCARINA_SONG_EPONAS) {
            EnMa2* malon = (EnMa2*)actor;
            if (malon->actionFunc == EnMa2_WaitForOcarina && actor->xzDistToPlayer < 60.0f) {
                malon->actionFunc = EnMa2_WaitForEponasSong;
                PauseSong_HoldMode(OCARINA_MODE_03);
                return true;
            }
        }
    }
    return false;
}

// Engage any in-range Song of Time block (Song of Time) or Great Fairy spawner (Sun's Song / Song of
// Storms) by pushing it into its song-finished state. Returns true if at least one was engaged, leaving
// MODE_04 held for the actor(s) to read.
static bool PauseSong_ActivateSongEventActors() {
    Player* player = GET_PLAYER(gPlayState);
    u16 song = gPlayState->msgCtx.lastPlayedSong;
    bool engaged = false;

    if (song == OCARINA_SONG_TIME) {
        for (Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_ITEMACTION].head; actor != NULL;
             actor = actor->next) {
            if (actor->id != ACTOR_OBJ_TIMEBLOCK) {
                continue;
            }
            ObjTimeblock* timeblock = (ObjTimeblock*)actor;
            // Only idle blocks the player could actually reach; skip ones mid-sequence or out of range.
            if (timeblock->songObserverFunc == ObjTimeblock_WaitForOcarina &&
                ObjTimeblock_PlayerIsInRange(timeblock, gPlayState)) {
                // WaitForSong reports completion once lastPlayedSong is Song of Time and its countdown ends;
                // a non-sentinel prior song plus a 1-frame timer makes it fire on the block's next update.
                timeblock->songObserverFunc = ObjTimeblock_WaitForSong;
                timeblock->unk_172 = OCARINA_SONG_TIME;
                timeblock->songEndTimer = 1;
                engaged = true;
            }
        }
    } else if (song == OCARINA_SONG_SUNS || song == OCARINA_SONG_STORMS) {
        s32 wantParams = (song == OCARINA_SONG_SUNS) ? 0x40 : 0x41;
        for (Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_PROP].head; actor != NULL; actor = actor->next) {
            if (actor->id != ACTOR_SHOT_SUN || (actor->params & 0xFF) != wantParams) {
                continue;
            }
            ShotSun* shotSun = (ShotSun*)actor;
            // fairySpawnerState 2 is the "song finished" state; the actor reads MODE_04 + song from there.
            if (shotSun->actionFunc == ShotSun_UpdateFairySpawner &&
                Math3D_Vec3fDistSq(&actor->world.pos, &player->actor.world.pos) <= 22500.0f) {
                shotSun->fairySpawnerState = 2;
                engaged = true;
            }
        }
    }

    if (engaged) {
        PauseSong_HoldMode(OCARINA_MODE_04);
    }
    return engaged;
}

static void PauseSong_Execute() {
    if (PauseSong_AdvancePending()) {
        return;
    }
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

    // Staff spots are the one category safe to drive inline: calling them here, after every actor has
    // already updated this frame, lets them consume MODE_03 and set their own MODE_04 without it leaking to
    // the MODE_04 readers below. So drive and reset them within this frame. (The Water Temple triforce
    // leaves msgMode = MSGMODE_PAUSED, which would otherwise block the pause menu -- z_play gates on NONE.)
    gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_03;
    if (PauseSong_ActivateOkarinaTags()) {
        gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_00;
        if (gPlayState->msgCtx.msgMode == MSGMODE_PAUSED) {
            gPlayState->msgCtx.msgMode = MSGMODE_NONE;
        }
        return;
    }
    gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_00;

    // Otherwise hand the song to a deferred actor: a Song of Time block / Great Fairy spawner (MODE_04), or
    // a matching NPC (MODE_03). Either holds the mode for the actor to poll next frame; AdvancePending then
    // finishes the hand-off and restores state.
    if (!PauseSong_ActivateSongEventActors()) {
        PauseSong_ActivateNpcActors();
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

// Only inject a played song when Link is in a normal, controllable field state -- the footing the vanilla
// ocarina action itself needs to start (see Player_ActionHandler_13: grounded and not in a blocking state).
// Firing from an item cutscene, a message, mid-song, mid-air, in water, while talking, mounted, or dying
// could leave the ocarina/message system in a bad state, which is the class of corruption we want to avoid.
static bool PauseSong_CanPlayOcarina() {
    Player* player = GET_PLAYER(gPlayState);
    if (gPlayState->msgCtx.msgMode != MSGMODE_NONE || gPlayState->msgCtx.ocarinaMode != OCARINA_MODE_00) {
        return false;
    }
    if (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE |
                               PLAYER_STATE1_TALKING | PLAYER_STATE1_ON_HORSE)) {
        return false;
    }
    if (player->stateFlags2 & PLAYER_STATE2_OCARINA_PLAYING) {
        return false;
    }
    return (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) != 0;
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
        if (!PauseSong_CanPlayOcarina()) {
            return;
        }
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
