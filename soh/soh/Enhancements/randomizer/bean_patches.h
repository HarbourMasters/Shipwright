#pragma once

#include <array>

#include "randomizerTypes.h"
#include "z64scene.h"

// One row per bean patch, shared by logic, savefile setup and the actor hooks.
// swchFlag is the switch flag set once the bean is planted,
// found using the Actor Viewer to get the Obj_Bean parameters & 0x3F.
struct BeanPatch {
    LogicVal plantEvent;
    RandomizerGet soul;
    RandomizerInf soulRandInf;
    SceneID scene;
    uint8_t swchFlag;
};

inline constexpr std::array<BeanPatch, 10> beanPatches = { {
    { LOGIC_PLANT_DEATH_MOUNTAIN_CRATER_BEAN, RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL,
      RAND_INF_DEATH_MOUNTAIN_CRATER_BEAN_SOUL, SCENE_DEATH_MOUNTAIN_CRATER, 3 },
    { LOGIC_PLANT_DEATH_MOUNTAIN_TRAIL_BEAN, RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL, RAND_INF_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL,
      SCENE_DEATH_MOUNTAIN_TRAIL, 6 },
    { LOGIC_PLANT_DESERT_COLOSSUS_BEAN, RG_DESERT_COLOSSUS_BEAN_SOUL, RAND_INF_DESERT_COLOSSUS_BEAN_SOUL,
      SCENE_DESERT_COLOSSUS, 24 },
    { LOGIC_PLANT_GERUDO_VALLEY_BEAN, RG_GERUDO_VALLEY_BEAN_SOUL, RAND_INF_GERUDO_VALLEY_BEAN_SOUL, SCENE_GERUDO_VALLEY,
      3 },
    { LOGIC_PLANT_GRAVEYARD_BEAN, RG_GRAVEYARD_BEAN_SOUL, RAND_INF_GRAVEYARD_BEAN_SOUL, SCENE_GRAVEYARD, 3 },
    { LOGIC_PLANT_KOKIRI_FOREST_BEAN, RG_KOKIRI_FOREST_BEAN_SOUL, RAND_INF_KOKIRI_FOREST_BEAN_SOUL, SCENE_KOKIRI_FOREST,
      9 },
    { LOGIC_PLANT_LAKE_HYLIA_BEAN, RG_LAKE_HYLIA_BEAN_SOUL, RAND_INF_LAKE_HYLIA_BEAN_SOUL, SCENE_LAKE_HYLIA, 1 },
    { LOGIC_PLANT_LOST_WOODS_BRIDGE_BEAN, RG_LOST_WOODS_BRIDGE_BEAN_SOUL, RAND_INF_LOST_WOODS_BRIDGE_BEAN_SOUL,
      SCENE_LOST_WOODS, 4 },
    { LOGIC_PLANT_LOST_WOODS_THEATER_BEAN, RG_LOST_WOODS_BEAN_SOUL, RAND_INF_LOST_WOODS_BEAN_SOUL, SCENE_LOST_WOODS,
      18 },
    { LOGIC_PLANT_ZORAS_RIVER_BEAN, RG_ZORAS_RIVER_BEAN_SOUL, RAND_INF_ZORAS_RIVER_BEAN_SOUL, SCENE_ZORAS_RIVER, 3 },
} };

inline const BeanPatch* FindBeanPatch(LogicVal plantEvent) {
    for (const BeanPatch& patch : beanPatches) {
        if (patch.plantEvent == plantEvent) {
            return &patch;
        }
    }
    return nullptr;
}

// Lost Woods is the only scene with two patches, so swchFlag only has to tell those apart
inline const BeanPatch* FindBeanPatch(int32_t sceneNum, uint8_t swchFlag) {
    for (const BeanPatch& patch : beanPatches) {
        if (patch.scene == sceneNum && (sceneNum != SCENE_LOST_WOODS || patch.swchFlag == swchFlag)) {
            return &patch;
        }
    }
    return nullptr;
}
