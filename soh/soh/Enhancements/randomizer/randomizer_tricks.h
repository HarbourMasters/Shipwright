#pragma once
#include "randomizerTypes.h"
#include <string>
#include <map>

typedef enum {
    RTAREA_GENERAL,
    RTAREA_BK_SKIPS,
    RTAREA_KOKIRI_FOREST,
    RTAREA_LOST_WOODS,
    RTAREA_SACRED_FOREST_MEADOW,
    RTAREA_HYRULE_FIELD,
    RTAREA_LAKE_HYLIA,
    RTAREA_GERUDO_VALLEY,
    RTAREA_GERUDO_FORTRESS,
    RTAREA_WASTELAND,
    RTAREA_DESERT_COLOSSUS,
    RTAREA_MARKET,
    RTAREA_HYRULE_CASTLE,
    RTAREA_KAKARIKO_VILLAGE,
    RTAREA_GRAVEYARD,
    RTAREA_DEATH_MOUNTAIN_TRAIL,
    RTAREA_GORON_CITY,
    RTAREA_DEATH_MOUNTAIN_CRATER,
    RTAREA_ZORAS_RIVER,
    RTAREA_ZORAS_DOMAIN,
    RTAREA_ZORAS_FOUNTAIN,
    RTAREA_LON_LON_RANCH,
    RTAREA_DEKU_TREE,
    RTAREA_DODONGOS_CAVERN,
    RTAREA_JABU_JABUS_BELLY,
    RTAREA_FOREST_TEMPLE,
    RTAREA_FIRE_TEMPLE,
    RTAREA_WATER_TEMPLE,
    RTAREA_SPIRIT_TEMPLE,
    RTAREA_SHADOW_TEMPLE,
    RTAREA_BOTTOM_OF_THE_WELL,
    RTAREA_ICE_CAVERN,
    RTAREA_GERUDO_TRAINING_GROUND,
    RTAREA_GANONS_CASTLE,
    RTAREA_INVALID
} RandomizerTrickArea;

typedef enum {
    RTVORMQ_VANILLA,
    RTVORMQ_MQ,
    RTVORMQ_BOTH
} RandomizerTrickVanillaOrMQ;

//todo tags
#define RT_OBJECT(rt, rt_v_or_mq, rt_area, rt_glitch, rt_shortname, rt_desc) \
    { rt, {rt, rt_v_or_mq, rt_area, rt_glitch, true, rt_shortname, rt_desc} }

typedef struct {
    RandomizerTrick rt;
    RandomizerTrickVanillaOrMQ vOrMQ;
    RandomizerTrickArea rtArea;
    //SceneID sceneId;
    bool rtGlitch;
    bool visibleInImgui;
    std::string rtShortName;
    std::string rtDesc;
} RandomizerTrickObject;

namespace RandomizerTricks {
    std::map<RandomizerTrickArea, std::map<RandomizerTrick,RandomizerTrickObject>> GetAllRTObjectsByArea();
    std::string GetRTAreaName(RandomizerTrickArea area);
}
