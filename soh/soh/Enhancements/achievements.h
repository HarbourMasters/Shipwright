#pragma once

typedef enum {
    ACHIEVEMENT_DEFEAT_GOHMA = 0,
    ACHIEVEMENT_DEFEAT_KD = 1,
    ACHIEVEMENT_DEFEAT_BARINADE = 2,
    ACHIEVEMENT_DEFEAT_PG = 3,
    ACHIEVEMENT_DEFEAT_VOLVAGIA = 4,
    ACHIEVEMENT_DEFEAT_MORPHA = 5,
    ACHIEVEMENT_DEFEAT_BONGO = 6,
    ACHIEVEMENT_DEFEAT_TWINROVA = 7,
    ACHIEVEMENT_DEFEAT_GANONDORF = 8,
    ACHIEVEMENT_DEFEAT_GANON = 9,
} AchievementId;

#ifdef __cplusplus
extern "C" {
#endif

void Achievements_Init();
bool Achievements_IsUnlocked(AchievementId id);
void Achievements_TryUnlock(AchievementId id);

#ifdef __cplusplus
}
#endif
