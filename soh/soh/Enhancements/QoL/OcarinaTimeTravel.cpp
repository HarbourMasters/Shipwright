#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/ShipInit.hpp"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/SwitchAge.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_Obj_Timeblock/z_obj_timeblock.h"
#include "src/overlays/actors/ovl_Obj_Warp2block/z_obj_warp2block.h"
#include "macros.h"
#include "variables.h"
void ObjTimeblock_Normal(ObjTimeblock* self, PlayState* play);
s32 ObjTimeblock_WaitForSong(ObjTimeblock* self, PlayState* play);
void func_80BA2610(ObjWarp2block* self, PlayState* play);
s32 func_80BA228C(ObjWarp2block* self, PlayState* play);
}

extern "C" PlayState* gPlayState;

#define CVAR_OCARINA_TIME_TRAVEL_DEFAULT 0
#define CVAR_OCARINA_TIME_TRAVEL_NAME CVAR_ENHANCEMENT("TimeTravel")
#define CVAR_OCARINA_TIME_TRAVEL_VALUE CVarGetInteger(CVAR_OCARINA_TIME_TRAVEL_NAME, CVAR_OCARINA_TIME_TRAVEL_DEFAULT)

static const CustomMessage timeTravelPrompt =
    CustomMessage("Travel through time?&&" + CustomMessage::TWO_WAY_CHOICE() + "%gYes&No%w",
                  "Durch die Zeit reisen?&&" + CustomMessage::TWO_WAY_CHOICE() + "%gJa&Nein%w",
                  "Voyager dans le temps ?&&" + CustomMessage::TWO_WAY_CHOICE() + "%gOui&Non%w");

enum class TimeTravelState { None, Prompting, WarpingOut, Leaving, Arriving, Materializing };
static TimeTravelState timeTravelState = TimeTravelState::None;
static u16 arrivalFrames = 0;
static bool arrivalLoaded = false;
// False when the warp-out was skipped: no cutscene, but the surface move still applies.
static bool arrivalCutscene = false;
static u16 arrivalSummonFrame = 0;
static ActorFunc arrivalSavedDraw = nullptr;

// The arrival animation spawned by the warp-song start mode
static constexpr s16 DEMOKANKYO_WARP_IN_PARAM = 0x10;

static void ArrivalShowPlayer() {
    if (arrivalSavedDraw != nullptr) {
        if (gPlayState != nullptr) {
            Player* player = GET_PLAYER(gPlayState);
            if (player != nullptr) {
                player->actor.draw = arrivalSavedDraw;
            }
        }
        arrivalSavedDraw = nullptr;
    }
}

static void ArrivalSummon() {
    if (!arrivalCutscene) {
        timeTravelState = TimeTravelState::None;
        return;
    }
    arrivalSummonFrame = arrivalFrames;
    timeTravelState = TimeTravelState::Materializing;
    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0,
                DEMOKANKYO_WARP_IN_PARAM);
}

// The era change lands while the old scene is still fading out, so the warp-in is summoned only
// once the arrival load has spawned its actors.
static void OnArrivalSceneSpawnActors() {
    if (timeTravelState != TimeTravelState::Arriving) {
        return;
    }
    arrivalLoaded = true;
    Player* player = GET_PLAYER(gPlayState);
    if (arrivalCutscene) {
        // Hidden from the load, so he materialises into the arrival instead of walking in visible.
        arrivalSavedDraw = player->actor.draw;
        player->actor.draw = nullptr;
    }
    if (!(player->stateFlags1 & PLAYER_STATE1_IN_WATER)) {
        return; // dry spawn: the walk-in wait in the update decides the summon
    }
    // Arrive at the water surface instead of wherever under it the preserved position was: the
    // game's swim-band centre (age thresholds unk_24..unk_2C) is where swim physics is stable and
    // UNDERWATER is unset, so the cutscene park accepts him and its exit restores the surface swim.
    f32 ySurface;
    WaterBox* waterbox;
    if (WaterBox_GetSurface1(gPlayState, &gPlayState->colCtx, player->actor.world.pos.x, player->actor.world.pos.z,
                             &ySurface, &waterbox)) {
        player->actor.world.pos.y = ySurface - ((player->ageProperties->unk_24 + player->ageProperties->unk_2C) / 2);
        player->actor.velocity.y = 0.0f;
    }
    ArrivalSummon();
}

// A block in range arms itself when the ocarina comes out; only an armed block takes the Song of Time.
static bool SongWakesABlock() {
    for (Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_ITEMACTION].head; actor != nullptr;
         actor = actor->next) {
        if (actor->id == ACTOR_OBJ_TIMEBLOCK) {
            auto* block = reinterpret_cast<ObjTimeblock*>(actor);
            if (block->actionFunc == ObjTimeblock_Normal && block->songObserverFunc == ObjTimeblock_WaitForSong) {
                return true;
            }
        } else if (actor->id == ACTOR_OBJ_WARP2BLOCK) {
            auto* block = reinterpret_cast<ObjWarp2block*>(actor);
            if (block->actionFunc == func_80BA2610 && block->func_168 == func_80BA228C) {
                return true;
            }
        }
    }
    return false;
}

/// Offers to change eras the way a warp song offers to warp.
void OcarinaTimeTravel() {
    if (!GameInteractor::IsSaveLoaded(true)) {
        return;
    }

    Actor* player = &GET_PLAYER(gPlayState)->actor;
    Actor* nearbyOcarinaSpot = Actor_FindNearby(gPlayState, player, ACTOR_EN_OKARINA_TAG, ACTORCAT_PROP, 120.0f);
    Actor* nearbyDoorOfTime = Actor_FindNearby(gPlayState, player, ACTOR_DOOR_TOKI, ACTORCAT_BG, 500.0f);
    Actor* nearbyFrogs = Actor_FindNearby(gPlayState, player, ACTOR_EN_FR, ACTORCAT_NPC, 300.0f);
    Actor* nearbyGossipStone = Actor_FindNearby(gPlayState, player, ACTOR_EN_GS, ACTORCAT_NPC, 300.0f);
    bool justPlayedSoT = gPlayState->msgCtx.lastPlayedSong == OCARINA_SONG_TIME;
    bool notNearAnySource =
        !SongWakesABlock() && !nearbyOcarinaSpot && !nearbyDoorOfTime && !nearbyFrogs && !nearbyGossipStone;
    bool hasOcarinaOfTime = (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME);
    bool hasMasterSword = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER);
    int timeTravelSetting = CVarGetInteger(CVAR_ENHANCEMENT("TimeTravel"), 0);
    bool meetsTimeTravelRequirements = false;

    switch (timeTravelSetting) {
        case TIME_TRAVEL_ANY:
            meetsTimeTravelRequirements = true;
            break;
        case TIME_TRAVEL_ANY_MS:
            meetsTimeTravelRequirements = hasMasterSword;
            break;
        case TIME_TRAVEL_OOT_MS:
            meetsTimeTravelRequirements = hasMasterSword && hasOcarinaOfTime;
            break;
        case TIME_TRAVEL_OOT:
        default:
            meetsTimeTravelRequirements = hasOcarinaOfTime;
            break;
    }

    if (justPlayedSoT && notNearAnySource && meetsTimeTravelRequirements &&
        gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        // As for a warp prompt, OCARINA_MODE_01 makes Yes answer OCARINA_MODE_02; opening a textbox clears it.
        Message_StartTextbox(gPlayState, TEXT_TIME_TRAVEL_PROMPT, nullptr);
        gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_01;
        timeTravelState = TimeTravelState::Prompting;
    }
}

// Applied after every OnWarpSongLeave handler, so none of them can redirect or cancel it.
static void ChangeEra(bool cutscene) {
    timeTravelState = TimeTravelState::None;
    arrivalLoaded = false;
    arrivalSavedDraw = nullptr;
    arrivalCutscene = cutscene;
    // Environment_WarpSongLeave's warp-in respawn; SwitchAge leaves it alone at Hyrule Castle.
    gSaveContext.respawnFlag = 0;
    SwitchAge();
    gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;

    if (gSaveContext.respawnFlag == 1) {
        // Demo_Kankyo colours the warp-in from the return respawn's song.
        if (cutscene) {
            gSaveContext.respawn[RESPAWN_MODE_RETURN].data = OCARINA_SONG_PRELUDE;
        }
        // The load keeps the IDLE spawn path Skip Warp Animation gets, which validates the position
        // before anything else happens. The warp-song start mode instead hides the player and waits
        // for its arrival cutscene to park him; that park is refused for underwater spawns and the
        // wait has no timeout, so the cutscene is summoned after the load only where it can hold him.
        arrivalFrames = 0;
        timeTravelState = TimeTravelState::Arriving;
    }
}

// The player's own OCARINA_MODE_02 handling indexes a warp-song table with lastPlayedSong, so the warp-out
// is started here instead (z_player.c, Player_Action_8084E3C4).
static void StartWarpOut() {
    Player* player = GET_PLAYER(gPlayState);

    gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_00;
    // Demo_Kankyo colours its sparkles by warp song.
    gPlayState->msgCtx.lastPlayedSong = OCARINA_SONG_PRELUDE;

    player->csAction = 0;
    player->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
    Player_TryCsAction(gPlayState, nullptr, 8);
    gPlayState->mainCamera.unk_14C &= ~8;
    player->stateFlags1 |= PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE;
    player->stateFlags2 |= PLAYER_STATE2_OCARINA_PLAYING;

    timeTravelState = TimeTravelState::WarpingOut;
    if (Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0xF) == nullptr) {
        Environment_WarpSongLeave(gPlayState);
    }
    // Leaving before the warp-out has played a frame means it was skipped, so the arrival is skipped too.
    if (timeTravelState == TimeTravelState::Leaving) {
        ChangeEra(false);
    }

    gSaveContext.seqId = static_cast<u8>(NA_BGM_DISABLED);
    gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
}

static void OcarinaTimeTravelUpdate() {
    switch (timeTravelState) {
        case TimeTravelState::Prompting:
            if (gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_02) {
                StartWarpOut();
            } else if (gPlayState->msgCtx.ocarinaMode != OCARINA_MODE_01) {
                timeTravelState = TimeTravelState::None;
            }
            break;
        case TimeTravelState::Leaving:
            ChangeEra(true);
            break;
        case TimeTravelState::Arriving: {
            arrivalFrames++;
            Player* player = GET_PLAYER(gPlayState);

            if (!arrivalLoaded) {
                break;
            }

            // A cutscene already running and a load that refuses to settle within 150 frames fall
            // back to the plain load.
            if (arrivalFrames > 150 || gPlayState->csCtx.state != CS_STATE_IDLE) {
                ArrivalShowPlayer();
                timeTravelState = TimeTravelState::None;
            } else if (!(player->stateFlags1 & PLAYER_STATE1_IN_WATER) &&
                       (gSaveContext.respawn[RESPAWN_MODE_DOWN].data == 0)) {
                // respawn[DOWN].data is 1 until the entrance walk-in completes, which is when real
                // physics has settled him. Ground at the height the era change left him at hosts the
                // cutscene; ground whose shape the era moved (holes, drops) does not.
                f32 heightDiff = player->actor.world.pos.y - gSaveContext.respawn[RESPAWN_MODE_DOWN].pos.y;
                if ((heightDiff > -40.0f) && (heightDiff < 40.0f)) {
                    ArrivalSummon();
                } else {
                    ArrivalShowPlayer();
                    timeTravelState = TimeTravelState::None;
                }
            }
            break;
        }
        case TimeTravelState::Materializing:
            arrivalFrames++;
            // Vanilla materialises the player about 40 frames into the arrival; the cap covers a
            // cutscene that never ran.
            if ((arrivalFrames >= (arrivalSummonFrame + 40)) || (arrivalFrames > 150)) {
                ArrivalShowPlayer();
                timeTravelState = TimeTravelState::None;
            }
            break;
        default:
            break;
    }
}

static void OnWarpSongLeave() {
    if (timeTravelState == TimeTravelState::WarpingOut) {
        timeTravelState = TimeTravelState::Leaving;
    }
}

static void RegisterOcarinaTimeTravel() {
    timeTravelState = TimeTravelState::None;
    arrivalFrames = 0;
    arrivalLoaded = false;
    arrivalCutscene = false;
    arrivalSummonFrame = 0;
    // Re-registering can happen mid-arrival (the setting toggled); never leave him hidden.
    ArrivalShowPlayer();
    COND_HOOK(OnOcarinaSongAction, CVAR_OCARINA_TIME_TRAVEL_VALUE, OcarinaTimeTravel);
    COND_HOOK(OnSceneSpawnActors, CVAR_OCARINA_TIME_TRAVEL_VALUE, OnArrivalSceneSpawnActors);
    COND_HOOK(OnGameFrameUpdate, CVAR_OCARINA_TIME_TRAVEL_VALUE, [] {
        if (GameInteractor::IsSaveLoaded(true)) {
            OcarinaTimeTravelUpdate();
        }
    });
    COND_HOOK(OnWarpSongLeave, CVAR_OCARINA_TIME_TRAVEL_VALUE, OnWarpSongLeave);
    COND_ID_HOOK(OnOpenText, TEXT_TIME_TRAVEL_PROMPT, CVAR_OCARINA_TIME_TRAVEL_VALUE,
                 [](uint16_t* textId, bool* loadFromMessageTable) {
                     // Formatting reads tables that may not exist yet during static initialisation.
                     CustomMessage prompt = timeTravelPrompt;
                     prompt.Format();
                     prompt.LoadIntoFont();
                     *loadFromMessageTable = false;
                 });
}

static RegisterShipInitFunc initFunc(RegisterOcarinaTimeTravel, { CVAR_OCARINA_TIME_TRAVEL_NAME });
