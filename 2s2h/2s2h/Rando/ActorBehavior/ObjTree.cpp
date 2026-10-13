#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomItem/CustomItem.h"
#include "assets/2s2h_assets.h"
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
}

std::map<std::tuple<s16, s16, s16>, RandoCheckId> treeActorIdMap = {
    // Beneath the Well
    { { SCENE_REDEAD, 9, 3 }, RC_BENEATH_THE_WELL_TREE },

    // North Clock Town
    { { SCENE_BACKTOWN, 0, 21 }, RC_CLOCK_TOWN_NORTH_TREE_01 },

    // Cucco Shack
    { { SCENE_F01C, 0, 12 }, RC_CUCCO_SHACK_TREE },

    // Goron Racetrack
    { { SCENE_GORONRACE, 0, 59 }, RC_GORON_RACETRACK_TREE_01 },
    { { SCENE_GORONRACE, 0, 60 }, RC_GORON_RACETRACK_TREE_02 },
    { { SCENE_GORONRACE, 0, 61 }, RC_GORON_RACETRACK_TREE_03 },
    { { SCENE_GORONRACE, 0, 62 }, RC_GORON_RACETRACK_TREE_04 },
    { { SCENE_GORONRACE, 0, 63 }, RC_GORON_RACETRACK_TREE_05 },
    { { SCENE_GORONRACE, 0, 64 }, RC_GORON_RACETRACK_TREE_06 },
    { { SCENE_GORONRACE, 0, 65 }, RC_GORON_RACETRACK_TREE_07 },
    { { SCENE_GORONRACE, 0, 66 }, RC_GORON_RACETRACK_TREE_08 },
    { { SCENE_GORONRACE, 0, 67 }, RC_GORON_RACETRACK_TREE_09 },
    { { SCENE_GORONRACE, 0, 68 }, RC_GORON_RACETRACK_TREE_10 },

    // Great Bay Coast (pre-temple clear)
    { { SCENE_30GYOSON, 0, 10 }, RC_GREAT_BAY_COAST_TREE_01 },
    { { SCENE_30GYOSON, 0, 11 }, RC_GREAT_BAY_COAST_TREE_02 },
    { { SCENE_30GYOSON, 0, 12 }, RC_GREAT_BAY_COAST_TREE_03 },
    { { SCENE_30GYOSON, 0, 13 }, RC_GREAT_BAY_COAST_TREE_04 },

    // Great Bay Coast (post-temple clear)
    { { SCENE_30GYOSON, 0, 31 }, RC_GREAT_BAY_COAST_TREE_01 },
    { { SCENE_30GYOSON, 0, 33 }, RC_GREAT_BAY_COAST_TREE_02 },
    { { SCENE_30GYOSON, 0, 34 }, RC_GREAT_BAY_COAST_TREE_03 },
    { { SCENE_30GYOSON, 0, 32 }, RC_GREAT_BAY_COAST_TREE_04 },

    // Path to Mountain Village
    { { SCENE_13HUBUKINOMITI, 0, 14 }, RC_PATH_TO_MOUNTAIN_VILLAGE_TREE_01 },
    { { SCENE_13HUBUKINOMITI, 0, 15 }, RC_PATH_TO_MOUNTAIN_VILLAGE_TREE_02 },
    { { SCENE_13HUBUKINOMITI, 0, 16 }, RC_PATH_TO_MOUNTAIN_VILLAGE_TREE_03 },
    { { SCENE_13HUBUKINOMITI, 0, 17 }, RC_PATH_TO_MOUNTAIN_VILLAGE_TREE_04 },

    // Path to Snowhead
    { { SCENE_14YUKIDAMANOMITI, 0, 22 }, RC_PATH_TO_SNOWHEAD_TREE_01 },
    { { SCENE_14YUKIDAMANOMITI, 0, 23 }, RC_PATH_TO_SNOWHEAD_TREE_02 },
    { { SCENE_14YUKIDAMANOMITI, 0, 24 }, RC_PATH_TO_SNOWHEAD_TREE_03 },
    { { SCENE_14YUKIDAMANOMITI, 0, 25 }, RC_PATH_TO_SNOWHEAD_TREE_04 },

    // Road to Southern Swamp
    { { SCENE_24KEMONOMITI, 0, 26 }, RC_ROAD_TO_SOUTHERN_SWAMP_TREE_01 },
    { { SCENE_24KEMONOMITI, 0, 27 }, RC_ROAD_TO_SOUTHERN_SWAMP_TREE_02 },
    { { SCENE_24KEMONOMITI, 0, 28 }, RC_ROAD_TO_SOUTHERN_SWAMP_TREE_03 },
    { { SCENE_24KEMONOMITI, 0, 29 }, RC_ROAD_TO_SOUTHERN_SWAMP_TREE_04 },
    { { SCENE_24KEMONOMITI, 0, 30 }, RC_ROAD_TO_SOUTHERN_SWAMP_TREE_05 },
    { { SCENE_24KEMONOMITI, 0, 31 }, RC_ROAD_TO_SOUTHERN_SWAMP_TREE_06 },

    // Romani Ranch
    { { SCENE_F01, 0, 20 }, RC_ROMANI_RANCH_TREE_01 },
    { { SCENE_F01, 0, 21 }, RC_ROMANI_RANCH_TREE_02 },
    { { SCENE_F01, 0, 22 }, RC_ROMANI_RANCH_TREE_03 },
    { { SCENE_F01, 0, 23 }, RC_ROMANI_RANCH_TREE_04 },
    { { SCENE_F01, 0, 24 }, RC_ROMANI_RANCH_TREE_05 },
    { { SCENE_F01, 0, 25 }, RC_ROMANI_RANCH_TREE_06 },
    { { SCENE_F01, 0, 26 }, RC_ROMANI_RANCH_TREE_07 },

    // Termina Field
    // RC_TERMINA_FIELD_MAN_IN_THE_TREE_RUPEE_01 also handles RC_TERMINA_FIELD_MAN_IN_THE_TREE_RUPEE_02
    { { SCENE_00KEIKOKU, 0, 167 }, RC_TERMINA_FIELD_MAN_IN_THE_TREE_RUPEE_01 },
    { { SCENE_00KEIKOKU, 0, 168 }, RC_TERMINA_FIELD_TREE_01 },
    { { SCENE_00KEIKOKU, 0, 169 }, RC_TERMINA_FIELD_TREE_02 },

    // Path to Goron Village
    { { SCENE_17SETUGEN2, 0, 7 }, RC_TWIN_ISLANDS_SPRING_TREE_01 },
    { { SCENE_17SETUGEN2, 0, 8 }, RC_TWIN_ISLANDS_SPRING_TREE_02 },
    { { SCENE_17SETUGEN2, 0, 9 }, RC_TWIN_ISLANDS_SPRING_TREE_03 },
    { { SCENE_17SETUGEN, 0, 7 }, RC_TWIN_ISLANDS_TREE_01 },
    { { SCENE_17SETUGEN, 0, 8 }, RC_TWIN_ISLANDS_TREE_02 },
    { { SCENE_17SETUGEN, 0, 9 }, RC_TWIN_ISLANDS_TREE_03 },

    // Zora Cape
    { { SCENE_31MISAKI, 0, 24 }, RC_ZORA_CAPE_TREE_01 },
    { { SCENE_31MISAKI, 0, 25 }, RC_ZORA_CAPE_TREE_02 },
    { { SCENE_31MISAKI, 0, 26 }, RC_ZORA_CAPE_TREE_03 },
    { { SCENE_31MISAKI, 0, 27 }, RC_ZORA_CAPE_TREE_04 },
    { { SCENE_31MISAKI, 0, 28 }, RC_ZORA_CAPE_TREE_05 },
};

std::unordered_map<RandoCheckId, EnItem00*> currentlySpawnedItems;

void ApplyGravityToSpawnedItem(RandoCheckId randoCheckId) {
    auto found = currentlySpawnedItems.find(randoCheckId);
    if (found != currentlySpawnedItems.end()) {
        Actor* actor = &found->second->actor;
        actor->gravity = -1.4f;
        CUSTOM_ITEM_FLAGS &= ~(CustomItem::STOP_BOBBING | CustomItem::STOP_SPINNING);
    }
}

EnItem00* SpawnTreeDrop(Actor* actor) {
    int customItemFlags = CustomItem::KILL_ON_TOUCH | CustomItem::ABLE_TO_ZORA_RANG | CustomItem::STOP_SPINNING |
                          CustomItem::STOP_BOBBING;
    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);

    // Pick a random direction to spawn the item, should be radius of 20 units from the tree trunk
    float angle = Rand_ZeroOne() * 2.0f * M_PI;
    float radius = 25.0f;
    float offsetX = radius * cosf(angle);
    float offsetZ = radius * sinf(angle);
    float offsetY = 220.0f;

    switch (actor->id) {
        case ACTOR_EN_WOOD02:
            if (actor->scale.x == 0.6f) {
                offsetY = 100.0f;
            }
            break;
        case ACTOR_OBJ_TREE:
            if (actor->scale.x == 0.1f) {
                offsetY = 140.0f;
            }
            break;
        case ACTOR_OBJ_YASI:
            offsetY = 270.0f;
            break;
        case ACTOR_EN_SNOWWD:
            offsetY = 150.0f;
            offsetX *= 1.5f;
            offsetZ *= 1.5f;
            break;
        default:
            break;
    }

    return CustomItem::Spawn(
        actor->world.pos.x + offsetX, actor->world.pos.y + offsetY, actor->world.pos.z + offsetZ, 0, customItemFlags,
        randoCheckId,
        [](Actor* actor, PlayState* play) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            randoSaveCheck.eligible = true;

            currentlySpawnedItems.erase((RandoCheckId)CUSTOM_ITEM_PARAM);
        },
        [](Actor* actor, PlayState* play) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                            (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
        });
}

void IdentifyTreeAndSpawnItem(Actor* actor) {
    s16 actorListIndex = GetActorListIndex(actor);
    RandoCheckId randoCheckId = RC_UNKNOWN;

    auto it = treeActorIdMap.find({ gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, actorListIndex });
    if (it != treeActorIdMap.end()) {
        randoCheckId = it->second;
    }

    // For the man in the tree rupees, attempt to spawn an extra item
    if (randoCheckId == RC_TERMINA_FIELD_MAN_IN_THE_TREE_RUPEE_01) {
        RandoCheckId otherRandoCheckId = RC_TERMINA_FIELD_MAN_IN_THE_TREE_RUPEE_02;

        if (RANDO_SAVE_CHECKS[randoCheckId].shuffled && !RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            Rando::ActorBehavior::SetObjectRandoCheckId(actor, otherRandoCheckId);
            EnItem00* spawnedItem = SpawnTreeDrop(actor);
            if (spawnedItem != nullptr) {
                currentlySpawnedItems.insert({ otherRandoCheckId, spawnedItem });
            }
        }
    }

    if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
        return;
    }

    Rando::ActorBehavior::SetObjectRandoCheckId(actor, randoCheckId);
    EnItem00* spawnedItem = SpawnTreeDrop(actor);
    if (spawnedItem != nullptr) {
        currentlySpawnedItems.insert({ randoCheckId, spawnedItem });
    }
}

void Rando::ActorBehavior::InitObjTreeBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_TREE_DROPS] != RO_GENERIC_OFF;

    COND_ID_HOOK(OnActorInit, ACTOR_EN_WOOD02, shouldRegister, IdentifyTreeAndSpawnItem);
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_TREE, shouldRegister, IdentifyTreeAndSpawnItem);
    COND_ID_HOOK(OnActorInit, ACTOR_OBJ_YASI, shouldRegister, IdentifyTreeAndSpawnItem);
    COND_ID_HOOK(OnActorInit, ACTOR_EN_SNOWWD, shouldRegister, IdentifyTreeAndSpawnItem);

    COND_VB_SHOULD(VB_APPLY_BONK_TO_ACTOR, shouldRegister, {
        Actor* tree = va_arg(args, Actor*);
        if (tree == nullptr) {
            return;
        }

        if (tree->id != ACTOR_EN_WOOD02 && tree->id != ACTOR_OBJ_TREE && tree->id != ACTOR_OBJ_YASI &&
            tree->id != ACTOR_EN_SNOWWD) {
            return;
        }

        RandoCheckId randoCheckId = GetObjectRandoCheckId(tree);
        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        // Apply gravity to the other rupee if applicable
        if (randoCheckId == RC_TERMINA_FIELD_MAN_IN_THE_TREE_RUPEE_01) {
            ApplyGravityToSpawnedItem(RC_TERMINA_FIELD_MAN_IN_THE_TREE_RUPEE_02);
        } else if (randoCheckId == RC_TERMINA_FIELD_MAN_IN_THE_TREE_RUPEE_02) {
            ApplyGravityToSpawnedItem(RC_TERMINA_FIELD_MAN_IN_THE_TREE_RUPEE_01);
        }

        ApplyGravityToSpawnedItem(randoCheckId);
    });

    COND_VB_SHOULD(VB_TREE_DROP_COLLECTIBLE, shouldRegister, {
        Actor* tree = va_arg(args, Actor*);
        if (tree == NULL) {
            return;
        }

        RandoCheckId randoCheckId = GetObjectRandoCheckId(tree);
        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        ApplyGravityToSpawnedItem(randoCheckId);
        *should = false;
    });

    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_SCOPECOIN, shouldRegister, [](Actor* actor, bool* should) {
        if (gPlayState->sceneId == SCENE_00KEIKOKU && actor->params == 162) {
            *should = false;
        }
    });

    COND_HOOK(OnSceneInit, IS_RANDO, [](s16 sceneId, s8 spawnNum) { currentlySpawnedItems.clear(); });
}
