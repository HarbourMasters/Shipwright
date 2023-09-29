#pragma once
#include "randomizerTypes.h"
#include <string>
#include <map>

// Forward Declarations to avoid duplicate definition issues
enum ActorID : int;
enum SceneID : int;

#define TWO_ACTOR_PARAMS(a, b) (abs(a) << 16) | abs(b)

#define RC_OBJECT(rc, rc_v_or_mq, rc_type, rc_area, actor_id, scene_id, actor_params, og_item_id, rc_shortname, rc_spoilername, vanillaCompletion) \
    { rc, {rc, rc_v_or_mq, rc_type, rc_area, actor_id, scene_id, actor_params, og_item_id, false, rc_shortname, rc_spoilername, vanillaCompletion} }

typedef struct {
    RandomizerCheck rc;
    RandomizerCheckVanillaOrMQ vOrMQ;
    RandomizerCheckType rcType;
    RandomizerCheckArea rcArea;
    ActorID actorId;
    SceneID sceneId;
    int32_t actorParams;
    GetItemID ogItemId;
    bool visibleInImgui;
    std::string rcShortName;
    std::string rcSpoilerName;
    bool vanillaCompletion;
} RandomizerCheckObject;

namespace RandomizerCheckObjects {
    bool AreaIsDungeon(RandomizerCheckArea area);
    bool AreaIsOverworld(RandomizerCheckArea area);
    std::string GetRCAreaName(RandomizerCheckArea area);
    std::map<RandomizerCheck, RandomizerCheckObject> GetAllRCObjects();
    std::map<RandomizerCheckArea, std::map<RandomizerCheck, RandomizerCheckObject*>> GetAllRCObjectsByArea();
    std::map<SceneID, RandomizerCheckArea> GetAllRCAreaBySceneID();
    RandomizerCheckArea GetRCAreaBySceneID(SceneID sceneId);
    void UpdateImGuiVisibility();
}
