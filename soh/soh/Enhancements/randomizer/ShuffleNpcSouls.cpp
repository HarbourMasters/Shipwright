/**
 * NPC Souls: an NPC whose soul hasn't been found yet is a "statue". It finishes initialising
 * normally, then freezes (no update, so no talking, trading or minigames), is drawn in greyscale and
 * keeps a solid collision cylinder so it still blocks the path it blocked before. Z-targeting it
 * and pressing C-up gets Navi's "They aren't moving anymore..." line. Picking up the soul wakes the
 * statue on the spot.
 */
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Enhancements/randomizer/npc_souls.h"
#include "soh/ShipInit.hpp"

#include <unordered_map>

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "overlays/actors/ovl_En_Ossan/z_en_ossan.h"
#include "overlays/actors/ovl_En_Hy/z_en_hy.h"
#include "overlays/actors/ovl_En_Ko/z_en_ko.h"
#include "overlays/actors/ovl_En_Go2/z_en_go2.h"
#include "overlays/actors/ovl_En_Ge1/z_en_ge1.h"
#include "overlays/actors/ovl_Fishing/z_fishing.h"
extern PlayState* gPlayState;
}

namespace {

constexpr s32 kAnyParams = -1;
constexpr s16 kAnyScene = -1;

// One way to recognise an NPC. `paramMask`/`paramValue` compare `actor->params & paramMask` to
// `paramValue` (kAnyParams matches every params). `solid` keeps a collision cylinder on the statue.
struct NpcMatcher {
    s16 actorId;
    s32 paramMask;
    s32 paramValue;
    s16 scene;
    RandomizerGet soul;
    bool solid;
};

// clang-format off
const NpcMatcher sNpcMatchers[] = {
    // Merchants
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_KOKIRI,           kAnyScene, RG_KOKIRI_SHOPKEEPER_SOUL,  true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_KAKARIKO_POTION,  kAnyScene, RG_POTION_SHOPKEEPER_SOUL,  true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_MARKET_POTION,    kAnyScene, RG_POTION_SHOPKEEPER_SOUL,  true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_BAZAAR,           kAnyScene, RG_BAZAAR_SHOPKEEPER_SOUL,  true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_ADULT,            kAnyScene, RG_BAZAAR_SHOPKEEPER_SOUL,  true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_GORON,            kAnyScene, RG_GORON_SHOPKEEPER_SOUL,   true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_ZORA,             kAnyScene, RG_ZORA_SHOPKEEPER_SOUL,    true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_BOMBCHUS,         kAnyScene, RG_BOMBCHU_SHOPKEEPER_SOUL, true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_MASK,             kAnyScene, RG_MASK_SALESMAN_SOUL,      true  },
    { ACTOR_EN_MS,           0,          0,                           kAnyScene, RG_BEAN_SALESMAN_SOUL,      true  },
    { ACTOR_EN_JS,           0,          0,                           kAnyScene, RG_CARPET_SALESMAN_SOUL,    true  },
    { ACTOR_EN_GM,           0,          0,                           kAnyScene, RG_MEDIGORON_SOUL,          true  },
    { ACTOR_EN_DS,           0,          0,                           kAnyScene, RG_GRANNY_SOUL,             true  },
    { ACTOR_EN_HY,           0x7F,       ENHY_TYPE_BOJ_5,             kAnyScene, RG_BEGGAR_SOUL,             true  },
    { ACTOR_EN_GB,           0,          0,                           kAnyScene, RG_POE_COLLECTOR_SOUL,      true  },
    { ACTOR_EN_HS,           0,          0,                           kAnyScene, RG_GROG_SOUL,               true  },
    { ACTOR_EN_KO,           0xFF,       ENKO_TYPE_CHILD_FADO,        kAnyScene, RG_FADO_SOUL,               true  },
    { ACTOR_EN_TORYO,        0,          0,                           kAnyScene, RG_MUTOH_SOUL,              true  },
    { ACTOR_EN_GO2,          0x1F,       GORON_DMT_BIGGORON,          kAnyScene, RG_BIGGORON_SOUL,           true  },
    // Minigames
    { ACTOR_EN_SYATEKI_MAN,  0,          0,                           kAnyScene, RG_SHOOTING_GALLERY_MAN_SOUL, true },
    { ACTOR_EN_BOM_BOWL_MAN, 0,          0,                           kAnyScene, RG_BOMBCHU_BOWLING_LADY_SOUL, true },
    { ACTOR_EN_TAKARA_MAN,   0,          0,                           kAnyScene, RG_TREASURE_CHEST_MAN_SOUL, true  },
    { ACTOR_EN_DIVING_GAME,  0,          0,                           kAnyScene, RG_DIVING_GAME_ZORA_SOUL,   true  },
    { ACTOR_EN_TK,           0,          0,                           kAnyScene, RG_DAMPE_SOUL,              true  },
    { ACTOR_EN_PO_RELAY,     0,          0,                           kAnyScene, RG_DAMPE_SOUL,              false },
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_HORSEBACK_ARCHERY,  kAnyScene, RG_HORSEBACK_ARCHER_SOUL,   true  },
    { ACTOR_FISHING,         kAnyParams, EN_FISH_OWNER,               kAnyScene, RG_FISHING_POND_OWNER_SOUL, true  },
    { ACTOR_EN_MK,           0,          0,                           kAnyScene, RG_SCIENTIST_SOUL,          true  },
    { ACTOR_EN_GO2,          0x1F,       GORON_CITY_ROLLING_BIG,      kAnyScene, RG_HOT_RODDER_SOUL,         false },
    { ACTOR_EN_SKJ,          0,          0,                           kAnyScene, RG_SKULL_KID_SOUL,          true  },
    { ACTOR_EN_FR,           0,          0,                           kAnyScene, RG_FROGS_SOUL,              false },
    { ACTOR_EN_NIW_LADY,     0,          0,                           kAnyScene, RG_ANJU_SOUL,               true  },
    // Townsfolk
    { ACTOR_EN_FU,           0,          0,                           kAnyScene, RG_WINDMILL_MAN_SOUL,       true  },
    { ACTOR_EN_ANI,          0,          0,                           kAnyScene, RG_MAN_ON_ROOF_SOUL,        true  },
    { ACTOR_EN_HY,           0x7F,       ENHY_TYPE_AOB,      SCENE_DOG_LADY_HOUSE, RG_DOG_LADY_SOUL,         true  },
    { ACTOR_EN_SSH,          0,          0,                           kAnyScene, RG_SKULLTULA_FAMILY_SOUL,   false },
    { ACTOR_EN_STH,          0,          0,                           kAnyScene, RG_SKULLTULA_FAMILY_SOUL,   true  },
    { ACTOR_EN_HEISHI2,      0xFF,       2,                           kAnyScene, RG_HYLIAN_GUARDS_SOUL,      true  }, // castle gate guard
    { ACTOR_EN_MM,           0,          0,                           kAnyScene, RG_RUNNING_MAN_SOUL,        true  },
    { ACTOR_EN_MM2,          0,          0,                           kAnyScene, RG_RUNNING_MAN_SOUL,        true  },
    { ACTOR_EN_GO2,          0x1F,       GORON_CITY_LINK,             kAnyScene, RG_GORON_LINK_SOUL,         false },
};
// clang-format on

// Navi's C-up text for an actor is message 0x0600 + naviEnemyId. This id is unused by the game.
constexpr u8 kStatueNaviEnemyId = 0xF0;
constexpr u16 kStatueNaviTextId = 0x0600 + kStatueNaviEnemyId;

// Updates a statue gets before it freezes, so actors that finish setting up in their first
// updates (waiting for their object, picking an action, setting `draw`) end up drawable.
constexpr s32 kUpdatesBeforeFreeze = 2;
constexpr s32 kMaxUpdatesBeforeFreeze = 30;

constexpr s16 kDefaultStatueRadius = 22;
constexpr s16 kDefaultStatueHeight = 50;

struct Statue {
    RandomizerGet soul;
    bool solid;
    bool frozen = false;
    s32 updates = 0;
    u8 savedNaviEnemyId = 0xFF;
    bool hasCollider = false;
    ColliderCylinder collider{};
};

std::unordered_map<Actor*, Statue> sStatues;

ColliderCylinderInit sStatueCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { kDefaultStatueRadius, kDefaultStatueHeight, 0, { 0, 0, 0 } },
};

bool IsSoulMissing(RandomizerGet rg) {
    const Rando::NpcSoul* soul = Rando::GetNpcSoul(rg);
    return soul != nullptr && Rando::IsNpcSoulShuffled(*soul) && !Flags_GetRandomizerInf(soul->randInf);
}

const NpcMatcher* FindMatcher(Actor* actor) {
    for (const NpcMatcher& matcher : sNpcMatchers) {
        if (actor->id != matcher.actorId) {
            continue;
        }
        if (matcher.scene != kAnyScene && (gPlayState == nullptr || gPlayState->sceneNum != matcher.scene)) {
            continue;
        }
        s32 params = (matcher.paramMask == kAnyParams) ? actor->params : (actor->params & matcher.paramMask);
        if (params != matcher.paramValue) {
            continue;
        }
        return &matcher;
    }
    return nullptr;
}

Statue* GetStatue(Actor* actor) {
    auto it = sStatues.find(actor);
    return it != sStatues.end() ? &it->second : nullptr;
}

void ReleaseStatue(Actor* actor, Statue& statue) {
    if (statue.hasCollider && gPlayState != nullptr) {
        Collider_DestroyCylinder(gPlayState, &statue.collider);
    }
    actor->naviEnemyId = statue.savedNaviEnemyId;
    // Talking to Navi about the statue leaves the talk flag set and its textId pointing at Navi's line;
    // clear both so the NPC doesn't start that conversation the moment it wakes up.
    if (actor->textId == kStatueNaviTextId) {
        actor->textId = 0;
    }
    actor->flags &= ~ACTOR_FLAG_TALK;
}

void Freeze(Actor* actor, Statue& statue) {
    statue.frozen = true;
    if (statue.solid && gPlayState != nullptr) {
        Collider_InitCylinder(gPlayState, &statue.collider);
        Collider_SetCylinder(gPlayState, &statue.collider, actor, &sStatueCylinderInit);
        if (actor->colChkInfo.cylRadius > 0 && actor->colChkInfo.cylHeight > 0) {
            statue.collider.dim.radius = actor->colChkInfo.cylRadius;
            statue.collider.dim.height = actor->colChkInfo.cylHeight;
        }
        statue.hasCollider = true;
    }
}

void OnActorInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);
    if (actor->update == nullptr) {
        return; // killed itself during init
    }
    const NpcMatcher* matcher = FindMatcher(actor);
    if (matcher == nullptr || !IsSoulMissing(matcher->soul)) {
        return;
    }
    Statue statue;
    statue.soul = matcher->soul;
    statue.solid = matcher->solid;
    statue.savedNaviEnemyId = actor->naviEnemyId;
    actor->naviEnemyId = kStatueNaviEnemyId;
    sStatues[actor] = statue;
}

void OnActorUpdate(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);
    Statue* statue = GetStatue(actor);
    if (statue == nullptr || statue->frozen) {
        return;
    }
    statue->updates++;
    if ((actor->draw != nullptr && statue->updates >= kUpdatesBeforeFreeze) ||
        statue->updates >= kMaxUpdatesBeforeFreeze) {
        Freeze(actor, *statue);
    }
}

void ShouldActorUpdate(void* actorRef, bool* result) {
    Statue* statue = GetStatue(static_cast<Actor*>(actorRef));
    if (statue != nullptr && statue->frozen) {
        *result = false;
    }
}

void OnPlayerUpdate() {
    if (gPlayState == nullptr) {
        return;
    }
    for (auto& [actor, statue] : sStatues) {
        if (statue.frozen && statue.hasCollider) {
            Collider_UpdateCylinder(actor, &statue.collider);
            CollisionCheck_SetOC(gPlayState, &gPlayState->colChkCtx, &statue.collider.base);
        }
    }
}

void OnActorDestroy(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);
    auto it = sStatues.find(actor);
    if (it != sStatues.end()) {
        ReleaseStatue(actor, it->second);
        sStatues.erase(it);
    }
}

void OnItemReceive(GetItemEntry itemEntry) {
    if (itemEntry.modIndex != MOD_RANDOMIZER || !Rando::IsNpcSoul((RandomizerGet)itemEntry.getItemId)) {
        return;
    }
    RandomizerGet soul = (RandomizerGet)itemEntry.getItemId;
    for (auto it = sStatues.begin(); it != sStatues.end();) {
        if (it->second.soul == soul) {
            ReleaseStatue(it->first, it->second);
            it = sStatues.erase(it);
        } else {
            ++it;
        }
    }
}

void BuildStatueNaviMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("They aren't moving anymore...", TEXTBOX_TYPE_BLUE);
    msg.AutoFormat();
    *loadFromMessageTable = false;
    msg.LoadIntoFont();
}

} // namespace

void RegisterShuffleNpcSouls() {
    bool shouldRegister = IS_RANDO && Rando::AreNpcSoulsEnabled();

    sStatues.clear();

    COND_HOOK(OnSceneInit, shouldRegister, [](int16_t sceneNum) { sStatues.clear(); });
    COND_HOOK(OnActorInit, shouldRegister, OnActorInit);
    COND_HOOK(OnActorUpdate, shouldRegister, OnActorUpdate);
    COND_HOOK(ShouldActorUpdate, shouldRegister, ShouldActorUpdate);
    COND_HOOK(OnPlayerUpdate, shouldRegister, OnPlayerUpdate);
    COND_HOOK(OnActorDestroy, shouldRegister, OnActorDestroy);
    COND_HOOK(OnItemReceive, shouldRegister, OnItemReceive);
    COND_ID_HOOK(OnOpenText, kStatueNaviTextId, shouldRegister, BuildStatueNaviMessage);

    COND_VB_SHOULD(VB_DRAW_ACTOR_IN_GRAYSCALE, shouldRegister, {
        Actor* actor = va_arg(args, Actor*);
        if (GetStatue(actor) != nullptr) {
            *should = true;
        }
    });

    // A statue that hasn't frozen yet may still offer to talk during its first updates.
    COND_VB_SHOULD(VB_SPEAK, shouldRegister, {
        Actor* talkActor = GET_PLAYER(gPlayState)->talkActor;
        if (talkActor != nullptr && GetStatue(talkActor) != nullptr) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterShuffleNpcSouls, { "IS_RANDO" });
