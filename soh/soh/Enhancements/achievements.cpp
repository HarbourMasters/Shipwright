#include "soh/Enhancements/achievements.h"
#include "soh/SaveManager.h"
#include "soh/Notification/Notification.h"
#include "soh/ShipInit.hpp"
#include "z64.h"

extern "C" SaveContext gSaveContext;

static RegisterShipInitFunc initFunc(Achievements_Init);

void Achievements_Load();
void Achievements_Save(SaveContext* saveContext, int sectionID, bool fullSave);
void Achievements_InitDefaults(bool isDebug);
const char* Achievements_GetName(AchievementId id);

void Achievements_Init() {
    SaveManager::Instance->AddLoadFunction("achievements", 1, Achievements_Load);
    SaveManager::Instance->AddSaveFunction("achievements", 1, Achievements_Save, true, SECTION_PARENT_NONE);
    SaveManager::Instance->AddInitFunction(Achievements_InitDefaults);
}

void Achievements_InitDefaults(bool isDebug) {
    gSaveContext.ship.achievements.achievementFlags = 0;
}

void Achievements_Load() {
    SaveManager::Instance->LoadData("achievementFlags", gSaveContext.ship.achievements.achievementFlags);
}

void Achievements_Save(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveManager::Instance->SaveData("achievementFlags", saveContext->ship.achievements.achievementFlags);
}

extern "C" bool Achievements_IsUnlocked(AchievementId id) {
    return (gSaveContext.ship.achievements.achievementFlags & (1ULL << id)) != 0;
}

extern "C" void Achievements_TryUnlock(AchievementId id) {
    if (!Achievements_IsUnlocked(id)) {
        gSaveContext.ship.achievements.achievementFlags |= (1ULL << id);
        Notification::Emit({
            .itemIcon = "__OTR__textures/icon_item_24_static/gQuestIconGoldSkulltulaTex",
            .prefix = "",
            .message = Achievements_GetName(id),
            .suffix = "",
        });
    }
}

const char* Achievements_GetName(AchievementId id) {
    switch (id) {
        case ACHIEVEMENT_DEFEAT_GOHMA:
            return "Defeat Gohma";
        case ACHIEVEMENT_DEFEAT_KD:
            return "Defeat King Dodongo";
        case ACHIEVEMENT_DEFEAT_BARINADE:
            return "Defeat Barinade";
        case ACHIEVEMENT_DEFEAT_PG:
            return "Defeat Phantom Ganon";
        case ACHIEVEMENT_DEFEAT_VOLVAGIA:
            return "Defeat Volvagia";
        case ACHIEVEMENT_DEFEAT_MORPHA:
            return "Defeat Morpha";
        case ACHIEVEMENT_DEFEAT_BONGO:
            return "Defeat Bongo Bongo";
        case ACHIEVEMENT_DEFEAT_TWINROVA:
            return "Defeat Twinrova";
        case ACHIEVEMENT_DEFEAT_GANONDORF:
            return "Defeat Ganondorf";
        case ACHIEVEMENT_DEFEAT_GANON:
            return "Defeat Ganon";
        default:
            return "Unknown Achievement";
    }
}