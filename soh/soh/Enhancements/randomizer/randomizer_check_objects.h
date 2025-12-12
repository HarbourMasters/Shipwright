#pragma once

#ifndef RANDOCHECKOBJECTS_H
#define RANDOCHECKOBJECTS_H

#include "randomizerTypes.h"
#include "z64actor_enum.h"
#include "z64scene.h"
#include <string>
#include <vector>
#include <map>

namespace RandomizerCheckObjects {
bool AreaIsDungeon(RandomizerCheckArea area);
bool AreaIsOverworld(RandomizerCheckArea area);
std::string GetRCAreaName(RandomizerCheckArea area);
std::map<RandomizerCheckArea, std::vector<RandomizerCheck>> GetAllRCObjectsByArea();
std::map<SceneID, RandomizerCheckArea> GetAllRCAreaBySceneID();
RandomizerCheckArea GetRCAreaBySceneID(SceneID sceneId);
void UpdateImGuiVisibility();
} // namespace RandomizerCheckObjects
#endif // RANDOCHECKOBJECTS_H
