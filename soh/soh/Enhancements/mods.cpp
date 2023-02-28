#include "mods.h"
#include <libultraship/bridge.h>
#include "game-interactor/GameInteractor.h"

extern "C" {
#include <z64.h>
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void Play_PerformSave(PlayState* play);
}

void RegisterAutoSaveOnReceiveItemHook() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnReceiveItem>([](u8 item) {

        // Don't autosave immediately after buying items from shops to prevent getting them for free!
        // Don't autosave in the Chamber of Sages since resuming from that map breaks the game
        // Don't autosave during the Ganon fight when picking up the Master Sword
        if ((CVarGetInteger("gAutosave", 0) > 0) && (gPlayState != NULL) && (gSaveContext.pendingSale == ITEM_NONE) &&
            (gPlayState->sceneNum != SCENE_KENJYANOMA) && (gPlayState->sceneNum != SCENE_GANON_DEMO)) {
            if ((CVarGetInteger("gAutosave", 0) == 2) || (CVarGetInteger("gAutosave", 0) == 5)) {
                // Autosave for all items
                Play_PerformSave(gPlayState);

            } else if ((CVarGetInteger("gAutosave", 0) == 1) || (CVarGetInteger("gAutosave", 0) == 4)) {
                // Autosave for major items
                switch (item) {
                    case ITEM_STICK:
                    case ITEM_NUT:
                    case ITEM_BOMB:
                    case ITEM_BOW:
                    case ITEM_SEEDS:
                    case ITEM_FISHING_POLE:
                    case ITEM_MAGIC_SMALL:
                    case ITEM_MAGIC_LARGE:
                    case ITEM_INVALID_4:
                    case ITEM_INVALID_5:
                    case ITEM_INVALID_6:
                    case ITEM_INVALID_7:
                    case ITEM_HEART:
                    case ITEM_RUPEE_GREEN:
                    case ITEM_RUPEE_BLUE:
                    case ITEM_RUPEE_RED:
                    case ITEM_RUPEE_PURPLE:
                    case ITEM_RUPEE_GOLD:
                    case ITEM_INVALID_8:
                    case ITEM_STICKS_5:
                    case ITEM_STICKS_10:
                    case ITEM_NUTS_5:
                    case ITEM_NUTS_10:
                    case ITEM_BOMBS_5:
                    case ITEM_BOMBS_10:
                    case ITEM_BOMBS_20:
                    case ITEM_BOMBS_30:
                    case ITEM_ARROWS_SMALL:
                    case ITEM_ARROWS_MEDIUM:
                    case ITEM_ARROWS_LARGE:
                    case ITEM_SEEDS_30:
                        break;
                    case ITEM_BOMBCHU:
                    case ITEM_BOMBCHUS_5:
                    case ITEM_BOMBCHUS_20:
                        if (!CVarGetInteger("gBombchuDrops", 0)) {
                            Play_PerformSave(gPlayState);
                        }
                        break;
                    default:
                        Play_PerformSave(gPlayState);
                        break;
                }
            }
        }
    });
}

void RegisterRupeeDash() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!CVarGetInteger("gRupeeDash", 0)) {
            return;
        }
        
        // Initialize Timer
        static uint16_t rupeeDashTimer = 0;
        uint16_t rdmTime = CVarGetInteger("gDashInterval", 5) * 20;
        
        // Did time change by DashInterval?
        if (rupeeDashTimer >= rdmTime) {
            rupeeDashTimer = 0;
            if (gSaveContext.rupees > 0) {
                Rupees_ChangeBy(-1);
            } else {
                if (gSaveContext.health > 0) {
                    gSaveContext.health = std::max(gSaveContext.health - 16, 0);
                }
            }
        } else {
            rupeeDashTimer++;
        }
    });
}

void InitMods() {
    RegisterAutoSaveOnReceiveItemHook();
}
