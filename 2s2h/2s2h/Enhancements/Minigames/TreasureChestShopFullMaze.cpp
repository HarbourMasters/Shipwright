#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"

// Re-definitions to avoid modifying source headers
#define TAKARAYA_WALL_ROWS 11
#define TAKARAYA_WALL_COLUMNS 8
#define TAKARAYA_WALL_FRONT_HEIGHT 20.0f
#define TAKARAYA_WALL_BACK_HEIGHT 120.0f

extern "C" {
extern f32 sTakarayaWallHeights[TAKARAYA_WALL_ROWS][TAKARAYA_WALL_COLUMNS];
extern u8 sTakarayaWallStates[TAKARAYA_WALL_ROWS][TAKARAYA_WALL_COLUMNS];
}

// Re-definition to avoid modifying source headers
typedef enum { TAKARAYA_WALL_INACTIVE, TAKARAYA_WALL_RISING, TAKARAYA_WALL_FALLING } TakarayaWallCellState;

#define CVAR_NAME "gEnhancements.Minigames.TreasureChestShopShowFullMaze"
#define CVAR CVarGetInteger(CVAR_NAME, TREASURE_CHEST_SHOP_MAZE_OFF)

static void RegisterTreasureChestShopFullMaze() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_TAKARAYA_WALL, CVAR != TREASURE_CHEST_SHOP_MAZE_OFF, [](Actor* actor) {
        if (gSaveContext.timerStates[TIMER_ID_MINIGAME_2] == TIMER_STATE_OFF) {
            return;
        }

        for (int i = 0; i < TAKARAYA_WALL_ROWS; i++) {
            f32 targetHeight = TAKARAYA_WALL_BACK_HEIGHT;
            if (CVAR == TREASURE_CHEST_SHOP_MAZE_TIERED) {
                f32 distanceFromFront = (f32)(TAKARAYA_WALL_ROWS - 1 - i) / (TAKARAYA_WALL_ROWS - 1);
                targetHeight = TAKARAYA_WALL_FRONT_HEIGHT +
                               ((TAKARAYA_WALL_BACK_HEIGHT - TAKARAYA_WALL_FRONT_HEIGHT) * distanceFromFront);
            }

            for (int j = 0; j < TAKARAYA_WALL_COLUMNS; j++) {
                if (sTakarayaWallHeights[i][j] >= 0.0f) {
                    if (Math_StepToF(&sTakarayaWallHeights[i][j], targetHeight, 15.0f)) {
                        sTakarayaWallStates[i][j] = TAKARAYA_WALL_INACTIVE;
                    } else {
                        sTakarayaWallStates[i][j] = TAKARAYA_WALL_RISING;
                    }
                }
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterTreasureChestShopFullMaze, { CVAR_NAME });
