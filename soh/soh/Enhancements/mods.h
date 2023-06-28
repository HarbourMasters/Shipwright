#include <stdint.h>
#include <nlohmann/json.hpp>

#ifndef MODS_H
#define MODS_H

#ifdef __cplusplus
extern "C" {
#endif

void PushSaveStateToRemote();
void RequestSaveStateFromRemote();
void ParseSaveStateFromRemote(nlohmann::json payload);
void UpdateDirtPathFixState(int32_t sceneNum);
void UpdateMirrorModeState(int32_t sceneNum);
void InitMods();

#ifdef __cplusplus
}
#endif

#endif
