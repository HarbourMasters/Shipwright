#pragma once
#include "randomizerTypes.h"
#include "z64actor_enum.h"
#include "z64scene.h"
#include <string>
#include <vector>
#include <map>

// ABS macro to use since `std::abs` is not constexpr yet
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define TWO_ACTOR_PARAMS(a, b) (ABS(a) << 16) | ABS(b)

namespace RandomizerCheckObjects {
    bool AreaIsDungeon(RandomizerCheckArea area);
    bool AreaIsOverworld(RandomizerCheckArea area);
    std::string GetRCAreaName(RandomizerCheckArea area);
    std::map<RandomizerCheckArea, std::vector<RandomizerCheck>> GetAllRCObjectsByArea();
    std::map<SceneID, RandomizerCheckArea> GetAllRCAreaBySceneID();
    RandomizerCheckArea GetRCAreaBySceneID(SceneID sceneId);
    void UpdateImGuiVisibility();
}
