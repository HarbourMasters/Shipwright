#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

static constexpr s32 TARGET_CVAR_DEFAULT = 20;
#define TARGET_CVAR_NAME "gEnhancements.Minigames.FishermanJumpingGame.TargetScore"
#define TARGET_CVAR CVarGetInteger(TARGET_CVAR_NAME, TARGET_CVAR_DEFAULT)

#define EARLY_WIN_CVAR_NAME "gEnhancements.Minigames.FishermanJumpingGame.EarlyWin"
#define EARLY_WIN_CVAR CVarGetInteger(EARLY_WIN_CVAR_NAME, 0)

#define TORCH_TIME_CVAR_NAME "gEnhancements.Minigames.FishermanJumpingGame.TorchTimeLimit"
#define TORCH_TIME_CVAR CVarGetInteger(TORCH_TIME_CVAR_NAME, TORCH_TIME_LIMIT_NORMAL)

static constexpr u16 TEXT_TARGET_SCORE = 0x109A;

static constexpr s16 DOUBLE_TIME_LIMIT = 200;

static bool ShouldWinJumpingGame() {
    return gSaveContext.minigameScore >= TARGET_CVAR;
}

static void ModifyTargetScore(u16* textId, bool* loadFromMessageTable) {
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    CustomMessage::Replace(&entry.msg,
                           "\x01"
                           "20",
                           "%r" + std::to_string(TARGET_CVAR));
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

static void RegisterJumpingGameEarlyWin() {
    COND_VB_SHOULD(VB_JUMPING_GAME_END, EARLY_WIN_CVAR, {
        if (ShouldWinJumpingGame()) {
            gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] = SECONDS_TO_TIMER(0);
            *should = true;
        }
    });
}

static void RegisterJumpingGameTargetScore() {
    bool isTargetChanged = TARGET_CVAR != TARGET_CVAR_DEFAULT;
    COND_VB_SHOULD(VB_FAIL_FISHERMAN_JUMPING_GAME, isTargetChanged, { *should = !ShouldWinJumpingGame(); });
    COND_ID_HOOK(OnOpenText, TEXT_TARGET_SCORE, isTargetChanged, ModifyTargetScore);
}

static void RegisterJumpingGameTorchTime() {
    COND_VB_SHOULD(VB_JUMPING_GAME_TORCH_RUN_OUT, TORCH_TIME_CVAR == TORCH_TIME_LIMIT_DOUBLE, {
        s32 burnTime = va_arg(args, s32);
        *should = (burnTime > DOUBLE_TIME_LIMIT);
    });

    COND_VB_SHOULD(VB_JUMPING_GAME_TORCH_RUN_OUT, TORCH_TIME_CVAR == TORCH_TIME_LIMIT_INFINITY, { *should = false; });
}

static RegisterShipInitFunc initFunc_EarlyWin(RegisterJumpingGameEarlyWin, { EARLY_WIN_CVAR_NAME });
static RegisterShipInitFunc initFunc_Target(RegisterJumpingGameTargetScore, { TARGET_CVAR_NAME });
static RegisterShipInitFunc initFunc_TorchTime(RegisterJumpingGameTorchTime, { TORCH_TIME_CVAR_NAME });
