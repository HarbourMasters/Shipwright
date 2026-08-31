#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "src/overlays/actors/ovl_En_Skj/z_en_skj.h"
extern PlayState* gPlayState;
void EnSkj_WaitForPlayback(EnSkj* enSkj, PlayState* play);
}

typedef enum {
    OCARINA_GAME_STARTING_NOTES = 3,
    OCARINA_GAME_ROUND_ONE_NOTES = 5,
    OCARINA_GAME_ROUND_TWO_NOTES = 6,
    OCARINA_GAME_ROUND_THREE_NOTES = 8,
} OcarinaGameDefaults;

#define CVAR_CUSTOMIZE_NAME CVAR_ENHANCEMENT("CustomizeOcarinaGame")
#define CVAR_CUSTOMIZE_VALUE CVarGetInteger(CVAR_CUSTOMIZE_NAME, 0)

#define CVAR_STARTING_NOTES_NAME CVAR_ENHANCEMENT("OcarinaGame.StartingNotes")
#define CVAR_STARTING_NOTES_VALUE CVarGetInteger(CVAR_STARTING_NOTES_NAME, OCARINA_GAME_STARTING_NOTES)

#define CVAR_ROUND_ONE_NOTES_NAME CVAR_ENHANCEMENT("OcarinaGame.RoundOneNotes")
#define CVAR_ROUND_ONE_NOTES_VALUE CVarGetInteger(CVAR_ROUND_ONE_NOTES_NAME, OCARINA_GAME_ROUND_ONE_NOTES)

#define CVAR_ROUND_TWO_NOTES_NAME CVAR_ENHANCEMENT("OcarinaGame.RoundTwoNotes")
#define CVAR_ROUND_TWO_NOTES_VALUE CVarGetInteger(CVAR_ROUND_TWO_NOTES_NAME, OCARINA_GAME_ROUND_TWO_NOTES)

#define CVAR_ROUND_THREE_NOTES_NAME CVAR_ENHANCEMENT("OcarinaGame.RoundThreeNotes")
#define CVAR_ROUND_THREE_NOTES_VALUE CVarGetInteger(CVAR_ROUND_THREE_NOTES_NAME, OCARINA_GAME_ROUND_THREE_NOTES)

#define CVAR_NOTE_SPEED_NAME CVAR_ENHANCEMENT("OcarinaGame.NoteSpeed")
#define CVAR_NOTE_SPEED_VALUE CVarGetInteger(CVAR_NOTE_SPEED_NAME, 1)

#define CVAR_INFINITE_TIME_NAME CVAR_ENHANCEMENT("OcarinaUnlimitedFailTime")
#define CVAR_INFINITE_TIME_VALUE CVarGetInteger(CVAR_INFINITE_TIME_NAME, 0)

#define CVAR_INSTANT_WIN_NAME CVAR_ENHANCEMENT("InstantOcarinaGameWin")
#define CVAR_INSTANT_WIN_VALUE CVarGetInteger(CVAR_INSTANT_WIN_NAME, 0)

static void RegisterLostWoodsOcarinaGameRoundNotesSetup() {
    COND_VB_SHOULD(VB_SET_LOST_WOODS_OCARINA_GAME_NOTES, CVAR_CUSTOMIZE_VALUE, {
        s32 minigameRound = va_arg(args, s32);
        u8* roundNotes = va_arg(args, u8*);

        switch (minigameRound) {
            case 0:
                *roundNotes = CVAR_ROUND_ONE_NOTES_VALUE;
                break;
            case 1:
                *roundNotes = CVAR_ROUND_TWO_NOTES_VALUE;
                break;
            default:
                *roundNotes = CVAR_ROUND_THREE_NOTES_VALUE;
                break;
        }

        *should = false;
    });
}

static void RegisterLostWoodsOcarinaGameStartingNotesSetup() {
    COND_VB_SHOULD(VB_SET_LOST_WOODS_OCARINA_GAME_STARTING_NOTES,
                   CVAR_CUSTOMIZE_VALUE && (CVAR_STARTING_NOTES_VALUE != OCARINA_GAME_STARTING_NOTES), {
                       for (u8 i = 0; i < CVAR_STARTING_NOTES_VALUE; i++) {
                           AudioOcarina_MemoryGameNextNote();
                       }
                       *should = false;
                   });
}

static void RegisterLostWoodsOcarinaGameModifyNoteSpeed() {
    COND_VB_SHOULD(VB_MODIFY_LOST_WOODS_OCARINA_GAME_NOTE_SPEED, CVAR_CUSTOMIZE_VALUE && (CVAR_NOTE_SPEED_VALUE != 1), {
        s32 appendPos = va_arg(args, s32);
        sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][appendPos].length /= CVAR_NOTE_SPEED_VALUE;
    });
}

static void RegisterLostWoodsOcarinaGameInfiniteTime() {
    COND_VB_SHOULD(VB_LOST_WOODS_OCARINA_GAME_TIMER_TICK, CVAR_CUSTOMIZE_VALUE && CVAR_INFINITE_TIME_VALUE,
                   { *should = false; });
}

static void RegisterLostWoodsOcarinaGameInstantWin() {
    COND_VB_SHOULD(VB_PLAY_LOST_WOODS_OCARINA_GAME, CVAR_CUSTOMIZE_VALUE && CVAR_INSTANT_WIN_VALUE, {
        EnSkj* enSkj = va_arg(args, EnSkj*);
        gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_0F;
        enSkj->multiuseTimer = 160;
        enSkj->actionFunc = EnSkj_WaitForPlayback;
        *should = false;
    });
}

static RegisterShipInitFunc initFunc_Notes(RegisterLostWoodsOcarinaGameRoundNotesSetup, { CVAR_CUSTOMIZE_NAME });
static RegisterShipInitFunc initFunc_StartingNotes(RegisterLostWoodsOcarinaGameStartingNotesSetup,
                                                   { CVAR_CUSTOMIZE_NAME, CVAR_STARTING_NOTES_NAME });
static RegisterShipInitFunc initFunc_ModifyTime(RegisterLostWoodsOcarinaGameModifyNoteSpeed,
                                                { CVAR_CUSTOMIZE_NAME, CVAR_NOTE_SPEED_NAME });
static RegisterShipInitFunc initFunc_InfiniteTime(RegisterLostWoodsOcarinaGameInfiniteTime,
                                                  { CVAR_CUSTOMIZE_NAME, CVAR_INFINITE_TIME_NAME });
static RegisterShipInitFunc initFunc_InstantWin(RegisterLostWoodsOcarinaGameInstantWin,
                                                { CVAR_CUSTOMIZE_NAME, CVAR_INSTANT_WIN_NAME });