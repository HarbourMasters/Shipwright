#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/ObjectExtension/ActorListIndex.h"

extern "C" {
#include "overlays/actors/ovl_En_Invisible_Ruppe/z_en_invisible_ruppe.h"
#include "overlays/actors/ovl_En_Gakufu/z_en_gakufu.h"
#include "overlays/actors/ovl_En_Hit_Tag/z_en_hit_tag.h"
#include "overlays/actors/ovl_Obj_Dora/z_obj_dora.h"

extern Vec3f sRewardDropsSpawnTerminaFieldPos;
extern u8 sRewardDropsIndex[];
extern void EnGakufu_PlayRewardCutscene(EnGakufu* enGakufu, PlayState* play);
}

extern EnItem00* spawnReplacementItem(Vec3f& pos, Rando::StaticData::RandoStaticCheck& randoStaticCheck);
extern int isDropActorAtPosition(PlayState* play, Actor* callingActor_, Actor* actor, void* verifyData);
extern void SpawnDropItem(Vec3f position, RandoCheckId randoCheckId);

// clang-format off
std::map<std::tuple<s16, s16, s16, s16>, RandoCheckId> enWonderItemMap = {
    // Pirates' Fortress
    { { ACTOR_EN_HIT_TAG, SCENE_KAIZOKU, 0, 24 }, RC_PIRATE_FORTRESS_PLAZA_WONDER_ITEM_01 },
    { { ACTOR_EN_HIT_TAG, SCENE_KAIZOKU, 0, 25 }, RC_PIRATE_FORTRESS_PLAZA_WONDER_ITEM_04 },
    { { ACTOR_EN_HIT_TAG, SCENE_PIRATE,  3, 9  }, RC_PIRATE_FORTRESS_INTERIOR_WONDER_ITEM_01 },
    // Swamp Spider House
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_KINSTA1, 4, 25 }, RC_SWAMP_SPIDER_HOUSE_WONDER_ITEM_01 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_KINSTA1, 4, 26 }, RC_SWAMP_SPIDER_HOUSE_WONDER_ITEM_02 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_KINSTA1, 4, 27 }, RC_SWAMP_SPIDER_HOUSE_WONDER_ITEM_03 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_KINSTA1, 4, 28 }, RC_SWAMP_SPIDER_HOUSE_WONDER_ITEM_04 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_KINSTA1, 4, 29 }, RC_SWAMP_SPIDER_HOUSE_WONDER_ITEM_05 },
    { { ACTOR_EN_HIT_TAG,         SCENE_KINSTA1, 1, 10 }, RC_SWAMP_SPIDER_HOUSE_WONDER_ITEM_06 },
    { { ACTOR_EN_HIT_TAG,         SCENE_KINSTA1, 1, 11 }, RC_SWAMP_SPIDER_HOUSE_WONDER_ITEM_09 },
    { { ACTOR_EN_HIT_TAG,         SCENE_KINSTA1, 1, 12 }, RC_SWAMP_SPIDER_HOUSE_WONDER_ITEM_12 },
    { { ACTOR_EN_HIT_TAG,         SCENE_KINSTA1, 1, 13 }, RC_SWAMP_SPIDER_HOUSE_WONDER_ITEM_15 },
    // Oceanside Spider House
    { { ACTOR_EN_HIT_TAG, SCENE_KINDAN2, 1, 0 }, RC_OCEAN_SPIDER_HOUSE_WONDER_ITEM_01 },
    { { ACTOR_EN_HIT_TAG, SCENE_KINDAN2, 1, 1 }, RC_OCEAN_SPIDER_HOUSE_WONDER_ITEM_04 },
    { { ACTOR_EN_HIT_TAG, SCENE_KINDAN2, 1, 2 }, RC_OCEAN_SPIDER_HOUSE_WONDER_ITEM_07 },
    // Termina Field
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 89  }, RC_TERMINA_FIELD_WONDER_ITEM_01 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 90  }, RC_TERMINA_FIELD_WONDER_ITEM_02 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 91  }, RC_TERMINA_FIELD_WONDER_ITEM_03 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 92  }, RC_TERMINA_FIELD_WONDER_ITEM_04 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 93  }, RC_TERMINA_FIELD_WONDER_ITEM_05 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 94  }, RC_TERMINA_FIELD_WONDER_ITEM_06 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 95  }, RC_TERMINA_FIELD_WONDER_ITEM_07 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 96  }, RC_TERMINA_FIELD_WONDER_ITEM_08 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 97  }, RC_TERMINA_FIELD_WONDER_ITEM_09 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 98  }, RC_TERMINA_FIELD_WONDER_ITEM_10 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_00KEIKOKU, 0, 99  }, RC_TERMINA_FIELD_WONDER_ITEM_11 },
    { { ACTOR_EN_HIT_TAG,         SCENE_00KEIKOKU, 0, 100 }, RC_TERMINA_FIELD_WONDER_ITEM_12 },
    { { ACTOR_EN_HIT_TAG,         SCENE_00KEIKOKU, 0, 101 }, RC_TERMINA_FIELD_WONDER_ITEM_15 },
    { { ACTOR_EN_HIT_TAG,         SCENE_00KEIKOKU, 0, 102 }, RC_TERMINA_FIELD_WONDER_ITEM_18 },
    // Romani Ranch
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_F01, 0, 42 }, RC_ROMANI_RANCH_WONDER_ITEM_01 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_F01, 0, 43 }, RC_ROMANI_RANCH_WONDER_ITEM_02 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_F01, 0, 44 }, RC_ROMANI_RANCH_WONDER_ITEM_03 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_F01, 0, 45 }, RC_ROMANI_RANCH_WONDER_ITEM_04 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_F01, 0, 46 }, RC_ROMANI_RANCH_WONDER_ITEM_05 },
    { { ACTOR_EN_INVISIBLE_RUPPE, SCENE_F01, 0, 47 }, RC_ROMANI_RANCH_WONDER_ITEM_06 },
    // Cucco Shack
    { { ACTOR_EN_HIT_TAG, SCENE_F01C, 0, 17 }, RC_CUCCO_SHACK_WONDER_ITEM_01 },
    { { ACTOR_EN_HIT_TAG, SCENE_F01C, 0, 18 }, RC_CUCCO_SHACK_WONDER_ITEM_04 },
    // Ikana Graveyard
    { { ACTOR_EN_HIT_TAG, SCENE_BOTI, 0, 10 }, RC_IKANA_GRAVEYARD_WONDER_ITEM_01 },
    { { ACTOR_EN_HIT_TAG, SCENE_BOTI, 0, 11 }, RC_IKANA_GRAVEYARD_WONDER_ITEM_04 },
    { { ACTOR_EN_HIT_TAG, SCENE_BOTI, 0, 12 }, RC_IKANA_GRAVEYARD_WONDER_ITEM_07 },
    { { ACTOR_EN_HIT_TAG, SCENE_BOTI, 1, 9  }, RC_IKANA_GRAVEYARD_WONDER_ITEM_10 },
    // Stock Pot Inn
    { { ACTOR_EN_HIT_TAG, SCENE_YADOYA, 0, 14 }, RC_STOCK_POT_INN_WONDER_ITEM_01 },
    // East Clock Town
    { { ACTOR_EN_HIT_TAG, SCENE_TOWN, 0, 42 }, RC_CLOCK_TOWN_EAST_WONDER_ITEM_01 },
    { { ACTOR_EN_HIT_TAG, SCENE_TOWN, 0, 43 }, RC_CLOCK_TOWN_EAST_WONDER_ITEM_04 },
    { { ACTOR_EN_HIT_TAG, SCENE_TOWN, 0, 44 }, RC_CLOCK_TOWN_EAST_WONDER_ITEM_07 },
    // South Clock Town
    { { ACTOR_EN_HIT_TAG, SCENE_CLOCKTOWER, 0, 29 }, RC_CLOCK_TOWN_SOUTH_WONDER_ITEM_01 },
};
// clang-format on

void Rando::ActorBehavior::SpawnWonderItemSparkle(Vec3f* pos) {
    static Vec3f sVelocity = { 0.0f, 1.2f, 0.0f };
    static Vec3f sAccel = { 0.0f, -0.05f, 0.0f };
    static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 255, 200, 64, 255 };

    Vec3f sparklePos = {
        Rand_CenteredFloat(20.0f) + pos->x,
        (Rand_ZeroOne() * 25.0f) + pos->y,
        Rand_CenteredFloat(20.0f) + pos->z,
    };

    EffectSsKirakira_SpawnDispersed(gPlayState, &sparklePos, &sVelocity, &sAccel, &sPrimColor, &sEnvColor, 1600, 16);
}

void Rando::ActorBehavior::DrawWonderItemSparkle(Actor* actor, PlayState* play) {
    if ((gGameState->frames % 4) != 0) {
        return;
    }

    if (!RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM].obtained) {
        SpawnWonderItemSparkle(&actor->world.pos);
    }
}

void EnHitTag_DrawWonderItemSparkle(Actor* actor, PlayState* play) {
    if ((gGameState->frames % 4) != 0) {
        return;
    }

    RandoCheckId baseRandoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);
    for (s32 i = 0; i < 3; i++) {
        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[baseRandoCheckId + i];
        if (randoSaveCheck.shuffled && !randoSaveCheck.obtained) {
            Rando::ActorBehavior::SpawnWonderItemSparkle(&actor->world.pos);
            // (returning here cause we only want to render one sparkle)
            return;
        }
    }
}

bool SpawnDroppedWonderItems(Vec3f position, u32 params) {
    Actor* dropActor = SubS_FindActorCustom(gPlayState, NULL, NULL, ACTORCAT_ITEMACTION, ACTOR_EN_HIT_TAG, &position,
                                            isDropActorAtPosition);

    if (dropActor == nullptr) {
        dropActor = Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_GAKUFU, ACTORCAT_ITEMACTION,
                                     99999.9f);
        if (dropActor == nullptr || ((EnGakufu*)dropActor)->actionFunc != EnGakufu_PlayRewardCutscene) {
            return false;
        }
    }

    s16 actorListIndex = GetActorListIndex(dropActor);

    // Cheesing it to avoid dupe spawns
    if (actorListIndex == -1) {
        return true;
    }

    RandoCheckId baseRandoCheckId = RC_UNKNOWN;
    if (dropActor->id == ACTOR_EN_GAKUFU) {
        s32 hour = TIME_TO_HOURS_F(CURRENT_TIME);
        baseRandoCheckId = RandoCheckId(RC_TERMINA_FIELD_WALL_WONDER_ITEM_01 + sRewardDropsIndex[hour]);
        position = sRewardDropsSpawnTerminaFieldPos;
    } else {
        auto it = enWonderItemMap.find(
            { ACTOR_EN_HIT_TAG, gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, actorListIndex });
        if (it != enWonderItemMap.end()) {
            baseRandoCheckId = it->second;
        }
    }

    if (baseRandoCheckId == RC_UNKNOWN) {
        return false;
    }

    if (!RANDO_SAVE_CHECKS[baseRandoCheckId].cycleObtained)
        SpawnDropItem(position, baseRandoCheckId);
    if (!RANDO_SAVE_CHECKS[baseRandoCheckId + 1].cycleObtained)
        SpawnDropItem(position, RandoCheckId(baseRandoCheckId + 1));
    if (!RANDO_SAVE_CHECKS[baseRandoCheckId + 2].cycleObtained)
        SpawnDropItem(position, RandoCheckId(baseRandoCheckId + 2));
    // Set this as a quick and dirty condition to avoid spawning dupes
    SetActorListIndex(dropActor, -1);
    return true;
}

void WonderItemInit(Actor* actor, RandoCheckId randoCheckId, bool isInvisible, bool* should) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

    if (!randoSaveCheck.shuffled || randoSaveCheck.cycleObtained) {
        return;
    }

    Actor* customActor = (Actor*)spawnReplacementItem(actor->world.pos, Rando::StaticData::Checks[randoCheckId]);
    if (isInvisible) {
        customActor->draw = Rando::ActorBehavior::DrawWonderItemSparkle;
    }

    *should = false;
}

void Rando::ActorBehavior::InitWonderItemsBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_WONDER_ITEMS];

    COND_VB_SHOULD(VB_DROP_COLLECTIBLE, shouldRegister, {
        Vec3f position = va_arg(args, Vec3f);
        u32 params = va_arg(args, u32);
        if (SpawnDroppedWonderItems(position, params)) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_GONG_DROP_COLLECTIBLE, shouldRegister, {
        ObjDora* objDora = va_arg(args, ObjDora*);
        if (!RANDO_SAVE_CHECKS[RC_SWORDSMAN_SCHOOL_WONDER_ITEM].cycleObtained) {
            SpawnDropItem(objDora->actor.world.pos, RC_SWORDSMAN_SCHOOL_WONDER_ITEM);
            *should = false;
        }
    });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_HIT_TAG, shouldRegister, [](Actor* actor) {
        if (actor->update == NULL) {
            return;
        }

        auto it =
            enWonderItemMap.find({ ACTOR_EN_HIT_TAG, gPlayState->sceneId, actor->room, GetActorListIndex(actor) });
        if (it == enWonderItemMap.end()) {
            return;
        }

        SetObjectRandoCheckId(actor, it->second);
        actor->draw = EnHitTag_DrawWonderItemSparkle;
    });

    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_INVISIBLE_RUPPE, shouldRegister, [](Actor* actor, bool* should) {
        RandoCheckId randoCheckId = RC_UNKNOWN;
        auto it = enWonderItemMap.find(
            { actor->id, gPlayState->sceneId, gPlayState->roomCtx.curRoom.num, GetActorListIndex(actor) });
        if (it != enWonderItemMap.end()) {
            randoCheckId = it->second;
        }

        if (randoCheckId != RC_UNKNOWN && RANDO_SAVE_CHECKS[randoCheckId].shuffled ||
            RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            WonderItemInit(actor, randoCheckId, true, should);
        }
    });

    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_SCOPECOIN, shouldRegister, [](Actor* actor, bool* should) {
        if (GetActorListIndex(actor) == 207) { // Pillar Rupee in Termina Field
            WonderItemInit(actor, RC_TERMINA_FIELD_WONDER_ITEM_21,
                           !(gPlayState->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON), should);
        }
    });
}
