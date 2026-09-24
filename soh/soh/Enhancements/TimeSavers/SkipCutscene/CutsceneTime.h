#pragma once

#include <cstdint>
#include <functional>

// Stands in for actors that change the time during a cutscene. Runs each frame before the cutscene commands.
using CutsceneTimeActorUpdate = std::function<void(uint16_t& dayTime, uint16_t& timeSpeed)>;

// Simulate cutscene's time of day, returning dayTime at end.
// fadeInFrames is how many frames time stays paused while the scene fades in.
uint16_t CutsceneTime_Simulate(int32_t sceneNum, uint16_t cutsceneIndex, uint16_t dayTime, int32_t fadeInFrames,
                               const CutsceneTimeActorUpdate& actorUpdate = nullptr);
