#include "soh/Enhancements/achievements.h"
#include "soh/SaveManager.h"
#include "soh/Notification/Notification.h"
#include "soh/ShipInit.hpp"
#include "z64.h"

extern "C" SaveContext gSaveContext;

static RegisterShipInitFunc initFunc(Achievements_Init);

void Achievements_InitDefaults(bool isDebug);
void Achievements_Load();
void Achievements_Save(SaveContext* saveContext, int sectionID, bool fullSave);
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

    SPDLOG_INFO("Loaded achievementFlags={}",gSaveContext.ship.achievements.achievementFlags);
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
        const AchievementInfo* info = Achievements_GetInfo(id);
        Notification::Emit({
            .itemIcon = "__OTR__textures/object_goma/gGohmaTitleCardENGTex",
            .message = info->name,
        });

        SPDLOG_INFO("Unlock {} -> flags={}", static_cast<int>(id), gSaveContext.ship.achievements.achievementFlags);
    }
}

const AchievementInfo* Achievements_GetInfo(AchievementId id) {
    for (size_t i = 0; i < gAchievementCount; i++) {
        if (gAchievements[i].id == id) {
            return &gAchievements[i];
        }
    }
    return NULL;
}

const AchievementInfo gAchievements[] = {
    { ACHIEVEMENT_DEFEAT_GOHMA, "Defeat Gohma" },
    { ACHIEVEMENT_DEFEAT_KD, "Defeat King Dodongo" },
    { ACHIEVEMENT_DEFEAT_BARINADE, "Defeat Barinade" },
    { ACHIEVEMENT_DEFEAT_PG, "Defeat Phantom Ganon" },
    { ACHIEVEMENT_DEFEAT_VOLVAGIA, "Defeat Volvagia" },
    { ACHIEVEMENT_DEFEAT_MORPHA, "Defeat Morpha" },
    { ACHIEVEMENT_DEFEAT_BONGO, "Defeat Bongo Bongo" },
    { ACHIEVEMENT_DEFEAT_TWINROVA, "Defeat Twinrova" },
    { ACHIEVEMENT_DEFEAT_GANONDORF, "Defeat Ganondorf" },
    { ACHIEVEMENT_DEFEAT_GANON, "Defeat Ganon" },
};

const size_t gAchievementCount =
    sizeof(gAchievements) / sizeof(gAchievements[0]);