#pragma once

#include <cassert>

#include "soh/Enhancements/randomizer/location_access.h"
#include "soh/Enhancements/randomizer/dungeon.h"
#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/trial.h"

inline uint8_t setting(const RandomizerSettingKey key) {
    return ctx->GetOption(key).Get();
}

inline bool has(const RandomizerGet item) {
    return logic->HasItem(item);
}

inline bool flag(const LogicVal key) {
    return logic->Get(key);
}

inline bool trick(const RandomizerTrick key) {
    return (bool)ctx->GetTrickOption(key);
}

inline bool is_child() {
    return logic->IsChild;
}

inline bool is_adult() {
    return logic->IsAdult;
}

inline bool at_day() {
    return logic->AtDay;
}

inline bool at_night() {
    return logic->AtNight;
}

inline Rando::DungeonKey GetDungeonKey(SceneID scene) {
    switch (scene) {
        case SCENE_DEKU_TREE:
            return Rando::DEKU_TREE;
        case SCENE_DODONGOS_CAVERN:
            return Rando::DODONGOS_CAVERN;
        case SCENE_JABU_JABU:
            return Rando::JABU_JABUS_BELLY;
        case SCENE_FOREST_TEMPLE:
            return Rando::FOREST_TEMPLE;
        case SCENE_FIRE_TEMPLE:
            return Rando::FIRE_TEMPLE;
        case SCENE_WATER_TEMPLE:
            return Rando::WATER_TEMPLE;
        case SCENE_SPIRIT_TEMPLE:
            return Rando::SPIRIT_TEMPLE;
        case SCENE_SHADOW_TEMPLE:
            return Rando::SHADOW_TEMPLE;
        case SCENE_BOTTOM_OF_THE_WELL:
            return Rando::BOTTOM_OF_THE_WELL;
        case SCENE_ICE_CAVERN:
            return Rando::ICE_CAVERN;
        case SCENE_GERUDO_TRAINING_GROUND:
            return Rando::GERUDO_TRAINING_GROUND;
        case SCENE_INSIDE_GANONS_CASTLE:
            return Rando::GANONS_CASTLE;
        default:
            assert(false);
            return Rando::DEKU_TREE;
    }
}

// SCENE_ID_MAX means the dungeon of the current region
inline bool is_vanilla(SceneID scene = SCENE_ID_MAX) {
    if (scene == SCENE_ID_MAX) {
        scene = areaTable[logic->CurrentRegionKey].scene;
    }
    return ctx->GetDungeon(GetDungeonKey(scene))->IsVanilla();
}

inline bool is_mq(SceneID scene = SCENE_ID_MAX) {
    if (scene == SCENE_ID_MAX) {
        scene = areaTable[logic->CurrentRegionKey].scene;
    }
    return ctx->GetDungeon(GetDungeonKey(scene))->IsMQ();
}

inline bool any_age(ConditionFn condition) {
    return AnyAgeTime(condition);
}

inline bool spirit_shared(
    RandomizerRegion region, ConditionFn condition, bool anyAge = false, RandomizerRegion otherRegion = RR_NONE,
    ConditionFn otherCondition = [] { return false; }, RandomizerRegion thirdRegion = RR_NONE,
    ConditionFn thirdCondition = [] { return false; }) {
    return SpiritShared(region, condition, anyAge, otherRegion, otherCondition, thirdRegion, thirdCondition);
}

inline bool spirit_certain_access(RandomizerRegion region) {
    return SpiritCertainAccess(region);
}

inline bool can_use(const RandomizerGet item) {
    return logic->CanUse(item);
}

inline bool item_use_allowed(const RandomizerGet item) {
    return logic->ItemUseAllowed(item);
}

inline bool b_allowed() {
    return logic->BAllowed();
}

inline bool can_plant_bean(const RandomizerGet bean) {
    return CanPlantBean(bean);
}

inline bool bean_planted(const LogicVal beanEvent) {
    return logic->BeanPlanted(beanEvent);
}

inline bool can_open_large_chest() {
    return logic->CanOpenLargeChest();
}

inline bool shop_item_not_for_sale(const RandomizerCheck check) {
    return logic->ShopItemNotForSale(check);
}

inline int bottle_count() {
    return logic->BottleCount();
}

inline int ocarina_buttons() {
    return logic->OcarinaButtons();
}

inline bool bombchus_enabled() {
    return logic->BombchusEnabled();
}

inline int health() {
    return logic->Health();
}

inline int effective_health() {
    return logic->EffectiveHealth();
}

inline int dungeon_count() {
    return logic->DungeonCount();
}

inline int stone_count() {
    return logic->StoneCount();
}

inline int medallion_count() {
    return logic->MedallionCount();
}

inline int fire_timer() {
    return logic->FireTimer();
}

inline int water_timer() {
    return logic->WaterTimer();
}

inline int get_gs_count() {
    return logic->GetGSCount();
}

inline int get_big_poe_count() {
    return logic->BigPoes;
}

inline int check_price(const RandomizerCheck check = RC_UNKNOWN_CHECK) {
    return GetCheckPrice(check);
}

inline bool trial_skipped(TrialKey key) {
    return ctx->GetTrial(key)->IsSkipped();
}

inline bool can_build_rainbow_bridge() {
    return logic->CanBuildRainbowBridge();
}

inline bool can_trigger_gbk() {
    return logic->CanTriggerGBK();
}

inline bool can_trigger_ganons_soul() {
    return logic->CanTriggerGanonsSoul();
}

inline bool can_trigger_wincon() {
    return logic->CanTriggerWincon();
}

inline bool small_keys(SceneID scene, int requiredAmount) {
    return logic->SmallKeys(scene, requiredAmount);
}
