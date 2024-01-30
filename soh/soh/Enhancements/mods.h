#include <stdint.h>

#ifndef MODS_H
#define MODS_H

#ifdef __cplusplus
extern "C" {
#endif

void UpdateDirtPathFixState(int32_t sceneNum);
void UpdateMirrorModeState(int32_t sceneNum);
void PatchToTMedallions();
void UpdatePermanentHeartLossState();
void InitMods();
void UpdatePatchChildHylianShield();
void UpdateChildHylianShieldState();

#ifdef __cplusplus
}
#endif

#endif
