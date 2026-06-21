#include "soh/Enhancements/achievements.h"
#include "soh/SaveManager.h"
#include "soh/Notification/Notification.h"
#include "soh/ShipInit.hpp"
#include "z64.h"
#include "functions.h"
#include "variables.h"
#include <fast/Fast3dGui.h>

extern "C" SaveContext gSaveContext;

static RegisterShipInitFunc initFunc(Achievements_Init);

void Achievements_InitDefaults(bool isDebug);
void Achievements_Load();
void Achievements_Save(SaveContext* saveContext, int sectionID, bool fullSave);

void Achievements_LoadIcons() {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());

    gui->LoadTextureFromRawImage("gAchieveGohmaDefeat", "textures/achievement_icons/gAchieveGohmaDefeat.png");
    gui->LoadTextureFromRawImage("gAchieveGohmaDefeat_Grayed", "textures/achievement_icons/gAchieveGohmaDefeat_Grayed.png");
    gui->LoadTextureFromRawImage("gAchieveKDDefeat", "textures/achievement_icons/gAchieveKDDefeat.png");
    gui->LoadTextureFromRawImage("gAchieveKDDefeat_Grayed", "textures/achievement_icons/gAchieveKDDefeat_Grayed.png");
    gui->LoadTextureFromRawImage("gAchieveBarinadeDefeat", "textures/achievement_icons/gAchieveBarinadeDefeat.png");
    gui->LoadTextureFromRawImage("gAchieveBarinadeDefeat_Grayed", "textures/achievement_icons/gAchieveBarinadeDefeat_Grayed.png");
    gui->LoadTextureFromRawImage("gAchievePGDefeat", "textures/achievement_icons/gAchievePGDefeat.png");
    gui->LoadTextureFromRawImage("gAchievePGDefeat_Grayed", "textures/achievement_icons/gAchievePGDefeat_Grayed.png");
    gui->LoadTextureFromRawImage("gAchieveVolvagiaDefeat", "textures/achievement_icons/gAchieveVolvagiaDefeat.png");
    gui->LoadTextureFromRawImage("gAchieveVolvagiaDefeat_Grayed", "textures/achievement_icons/gAchieveVolvagiaDefeat_Grayed.png");
    gui->LoadTextureFromRawImage("gAchieveMorphaDefeat", "textures/achievement_icons/gAchieveMorphaDefeat.png");
    gui->LoadTextureFromRawImage("gAchieveMorphaDefeat_Grayed", "textures/achievement_icons/gAchieveMorphaDefeat_Grayed.png");
    gui->LoadTextureFromRawImage("gAchieveBongoDefeat", "textures/achievement_icons/gAchieveBongoDefeat.png");
    gui->LoadTextureFromRawImage("gAchieveBongoDefeat_Grayed", "textures/achievement_icons/gAchieveBongoDefeat_Grayed.png");
    gui->LoadTextureFromRawImage("gAchieveTwinrovaDefeat", "textures/achievement_icons/gAchieveTwinrovaDefeat.png");
    gui->LoadTextureFromRawImage("gAchieveTwinrovaDefeat_Grayed", "textures/achievement_icons/gAchieveTwinrovaDefeat_Grayed.png");
    gui->LoadTextureFromRawImage("gAchieveGanondorfDefeat", "textures/achievement_icons/gAchieveGanondorfDefeat.png");
    gui->LoadTextureFromRawImage("gAchieveGanondorfDefeat_Grayed", "textures/achievement_icons/gAchieveGanondorfDefeat_Grayed.png");
    gui->LoadTextureFromRawImage("gAchieveGanonDefeat", "textures/achievement_icons/gAchieveGanonDefeat.png");
    gui->LoadTextureFromRawImage("gAchieveGanonDefeat_Grayed", "textures/achievement_icons/gAchieveGanonDefeat_Grayed.png");
}

void Achievements_Init() {
    Achievements_LoadIcons();
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
            .itemIcon = info->icon,
            .prefix = "Achievement Unlocked!",
            .prefixColor = ImVec4(1.0f, 0.85f, 0.0f, 1.0f),
            .message = info->name,
            .messageColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
            .achievement = true,
        });
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

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
    { ACHIEVEMENT_DEFEAT_GOHMA, "Parasitic Armored Arachnid", "gAchieveGohmaDefeat", "gAchieveGohmaDefeat_Grayed" },
    { ACHIEVEMENT_DEFEAT_KD, "Infernal Dinosaur", "gAchieveKDDefeat", "gAchieveKDDefeat_Grayed" },
    { ACHIEVEMENT_DEFEAT_BARINADE, "Bio-Electric Anemone", "gAchieveBarinadeDefeat", "gAchieveBarinadeDefeat_Grayed" },
    { ACHIEVEMENT_DEFEAT_PG, "Evil Spirit from Beyond", "gAchievePGDefeat", "gAchievePGDefeat_Grayed" },
    { ACHIEVEMENT_DEFEAT_VOLVAGIA, "Subterranean Lava Dragon", "gAchieveVolvagiaDefeat", "gAchieveVolvagiaDefeat_Grayed" },
    { ACHIEVEMENT_DEFEAT_MORPHA, "Giant Aquatic Amoeba", "gAchieveMorphaDefeat", "gAchieveMorphaDefeat_Grayed" },
    { ACHIEVEMENT_DEFEAT_BONGO, "Phantom Shadow Beast", "gAchieveBongoDefeat", "gAchieveBongoDefeat_Grayed" },
    { ACHIEVEMENT_DEFEAT_TWINROVA, "Sorceress Sisters", "gAchieveTwinrovaDefeat", "gAchieveTwinrovaDefeat_Grayed" },
    { ACHIEVEMENT_DEFEAT_GANONDORF, "Great King of Evil", "gAchieveGanondorfDefeat", "gAchieveGanondorfDefeat_Grayed" },
    { ACHIEVEMENT_DEFEAT_GANON, "Ganon", "gAchieveGanonDefeat", "gAchieveGanonDefeat_Grayed" },
};

const size_t gAchievementCount =
    sizeof(gAchievements) / sizeof(gAchievements[0]);