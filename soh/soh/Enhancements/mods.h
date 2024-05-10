#include <stdint.h>

#ifndef MODS_H
#define MODS_H

#ifdef __cplusplus
extern "C" {
#endif

void UpdateDirtPathFixState(int32_t sceneNum);
void UpdateMirrorModeState(int32_t sceneNum);
void UpdateHurtContainerModeState(bool newState);
void PatchToTMedallions();
void PatchCompasses();
void UpdatePermanentHeartLossState();
void UpdateHyperEnemiesState();
void UpdateHyperBossesState();
void InitMods();
void UpdatePatchHand(); 

#ifdef __cplusplus
}
#endif

#endif
