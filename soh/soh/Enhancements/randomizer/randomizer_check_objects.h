#pragma once
#include "randomizerTypes.h"
#include "z64actor_enum.h"
#include "z64scene.h"
#include <string>
#include <vector>
#include <map>

#define TWO_ACTOR_PARAMS(a, b) (abs(a) << 16) | abs(b)
#define THREE_ACTOR_PARAMS(a, b, c) (abs(a) << 10) | (abs(a) << 20) | abs(c)

namespace RandomizerCheckObjects {
    bool AreaIsDungeon(RandomizerCheckArea area);
    bool AreaIsOverworld(RandomizerCheckArea area);
    std::string GetRCAreaName(RandomizerCheckArea area);
    std::map<RandomizerCheckArea, std::vector<RandomizerCheck>> GetAllRCObjectsByArea();
    std::map<SceneID, RandomizerCheckArea> GetAllRCAreaBySceneID();
    RandomizerCheckArea GetRCAreaBySceneID(SceneID sceneId);
    void UpdateImGuiVisibility();
}
