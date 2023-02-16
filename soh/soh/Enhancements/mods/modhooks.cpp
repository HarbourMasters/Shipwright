#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include <libultraship/bridge.h>

extern "C" {
#include <z64.h>
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void Play_PerformSave(PlayState* play);
}

void RegisterAutoSaveOnReceiveItem() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnReceiveItem>([](u8 item) {
        if (CVarGetInteger("gAutosave", 0) && (gPlayState != NULL) && (gPlayState->sceneNum != SCENE_KENJYANOMA) && (gSaveContext.pendingSale == ITEM_NONE) && (gPlayState->sceneNum != SCENE_GANON_DEMO)) {
            if (CVarGetInteger("gAutosaveAllItems", 0)) {
                Play_PerformSave(gPlayState);
            } else if (CVarGetInteger("gAutosaveMajorItems", 1)) {
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

extern "C" void RegisterModHooks() {
    RegisterAutoSaveOnReceiveItem();
}
