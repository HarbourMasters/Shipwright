/**
 * NPC Souls: an NPC whose soul hasn't been found yet is a "statue". It finishes initialising
 * normally, then freezes (no update, so no talking, trading or minigames), is drawn in greyscale and
 * keeps a solid collision cylinder so it still blocks the path it blocked before. Z-targeting it
 * and pressing C-up gets Navi's "They aren't moving anymore..." line. Picking up the soul wakes the
 * statue on the spot.
 *
 * Gatekeepers that block a path by side-stepping in front of Link (Mido, the Kokiri at the forest
 * exit) keep doing that as statues: they slide around the same circle they use in vanilla, so the
 * path stays shut exactly as before, without talking or moving aside.
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
#include "overlays/actors/ovl_En_Md/z_en_md.h"
extern PlayState* gPlayState;
}

namespace {

constexpr s32 kAnyParams = -1;
constexpr s16 kAnyScene = -1;

// One way to recognise an NPC. `paramMask`/`paramValue` compare `actor->params & paramMask` to
// `paramValue` (kAnyParams matches every params). `solid` keeps a collision cylinder on the statue.
// The optional fields:
//   radius/height  size of that cylinder (0: the actor's colChkInfo size, or a default).
//   orbit          the NPC blocks a path by staying `orbit` units from its home, between home and
//                  Link. If it was doing that when it froze, the statue keeps doing it.
//   blockHeight    cylinder height while orbit-blocking (0: same as `height`).
//   freezeOnInit   freeze before the first update. For NPCs whose first update would start a forced
//                  conversation or a cutscene, which a statue would then never finish.
//   active         when set, the NPC only counts as an NPC while this returns true.
struct NpcMatcher {
    s16 actorId;
    s32 paramMask;
    s32 paramValue;
    s16 scene;
    RandomizerGet soul;
    bool solid;
    s16 radius = 0;
    s16 height = 0;
    f32 orbit = 0.0f;
    s16 blockHeight = 0;
    bool freezeOnInit = false;
    bool (*active)() = nullptr;
};

// Until Link has the membership card, the fortress Gerudo aren't NPCs but lookouts who throw him in
// jail; a statue would make sneaking past them free. They become statues once they are friendly.
bool GerudosFriendly() {
    return CHECK_QUEST_ITEM(QUEST_GERUDO_CARD);
}

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
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_HORSEBACK_ARCHERY,  kAnyScene, RG_HORSEBACK_ARCHER_SOUL,   true, 0, 0, 0.0f, 0, false, GerudosFriendly },
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
    // Gatekeepers
    { ACTOR_EN_HEISHI2,      0xFF,       5,                           kAnyScene, RG_KAKARIKO_GATEKEEPER_SOUL, true, 15, 70 }, // Kakariko gate guard
    { ACTOR_EN_MD,           0,          0,                           kAnyScene, RG_MIDO_SOUL,               true, 36, 46, 60.0f },
    { ACTOR_EN_KO,           0xFF,       ENKO_TYPE_CHILD_3, SCENE_KOKIRI_FOREST, RG_KOKIRI_EXIT_GUARD_SOUL,  true, 20, 46, 80.0f, 246 },
    { ACTOR_EN_KZ,           0,          0,                           kAnyScene, RG_KING_ZORA_SOUL,          true, 80, 120 },
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_GATE_OPERATOR,      kAnyScene, RG_WASTELAND_GATEKEEPER_SOUL, true, 0, 0, 0.0f, 0, false, GerudosFriendly },
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_TRAINING_GROUND_GUARD, kAnyScene, RG_TRAINING_GROUND_GATEKEEPER_SOUL, true, 0, 0, 0.0f, 0, false, GerudosFriendly },
    { ACTOR_EN_GE3,          0,          0,                           kAnyScene, RG_GERUDO_CARD_GIVER_SOUL,  true, 20, 50, 0.0f, 0, true },
    { ACTOR_EN_GE2,          0xFF,       2,                           kAnyScene, RG_GERUDO_CARD_GIVER_SOUL,  true, 20, 50, 0.0f, 0, true },
    // Carpenters, hideout and Kakariko alike: params & 3 is 0 Ichiro, 1 Sabooro, 2 Jiro, 3 Shiro
    { ACTOR_EN_DAIKU,        3,          0,                           kAnyScene, RG_ICHIRO_SOUL,             true  },
    { ACTOR_EN_DAIKU,        3,          1,                           kAnyScene, RG_SABOORO_SOUL,            true  },
    { ACTOR_EN_DAIKU,        3,          2,                           kAnyScene, RG_JIRO_SOUL,               true  },
    { ACTOR_EN_DAIKU,        3,          3,                           kAnyScene, RG_SHIRO_SOUL,              true  },
    { ACTOR_EN_DAIKU_KAKARIKO, 3,        0,                           kAnyScene, RG_ICHIRO_SOUL,             true  },
    { ACTOR_EN_DAIKU_KAKARIKO, 3,        1,                           kAnyScene, RG_SABOORO_SOUL,            true  },
    { ACTOR_EN_DAIKU_KAKARIKO, 3,        2,                           kAnyScene, RG_JIRO_SOUL,               true  },
    { ACTOR_EN_DAIKU_KAKARIKO, 3,        3,                           kAnyScene, RG_SHIRO_SOUL,              true  },
    { ACTOR_EN_TA,           0,          0,                           kAnyScene, RG_TALON_SOUL,              true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_TALON,            kAnyScene, RG_TALON_SOUL,              true  },
    { ACTOR_EN_MA1,          0,          0,                           kAnyScene, RG_MALON_SOUL,              true  },
    { ACTOR_EN_MA2,          0,          0,                           kAnyScene, RG_MALON_SOUL,              true  },
    { ACTOR_EN_MA3,          0,          0,                           kAnyScene, RG_MALON_SOUL,              true  },
    { ACTOR_EN_IN,           0,          0,                           kAnyScene, RG_INGO_SOUL,               true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_INGO,             kAnyScene, RG_INGO_SOUL,               true  },
    { ACTOR_EN_OWL,          0,          0,                           kAnyScene, RG_KAEPORA_GAEBORA_SOUL,    false, 0, 0, 0.0f, 0, true },
    // More townsfolk
    { ACTOR_EN_KO,           0,          0,      SCENE_KNOW_IT_ALL_BROS_HOUSE, RG_KNOW_IT_ALL_BROTHERS_SOUL, true },
    { ACTOR_EN_TG,           0,          0,                           kAnyScene, RG_HONEY_AND_DARLING_SOUL,  true  },
    { ACTOR_EN_HS2,          0,          0,                           kAnyScene, RG_GROG_SOUL,               true  }, // Grog as a child
    { ACTOR_EN_HEISHI4,      0,          0,                           kAnyScene, RG_HYLIAN_GUARDS_SOUL,      true  },
    { ACTOR_EN_KAKASI,       0,          0,                           kAnyScene, RG_PIERRE_SOUL,             true  },
    { ACTOR_EN_KAKASI2,      0,          0,                           kAnyScene, RG_PIERRE_SOUL,             false, 0, 0, 0.0f, 0, true }, // where Pierre pops up
    { ACTOR_EN_KAKASI3,      0,          0,                           kAnyScene, RG_BONOORU_SOUL,            true  },
    // One soul per people, for everyone not matched above
    { ACTOR_EN_KO,           0,          0,                           kAnyScene, RG_KOKIRI_SOUL,             true  },
    { ACTOR_EN_HY,           0,          0,                           kAnyScene, RG_HYLIANS_SOUL,            true  },
    { ACTOR_EN_MU,           0,          0,                           kAnyScene, RG_HYLIANS_SOUL,            true  },
    { ACTOR_EN_CS,           0,          0,                           kAnyScene, RG_HYLIANS_SOUL,            true  },
    { ACTOR_EN_NIW_GIRL,     0,          0,                           kAnyScene, RG_HYLIANS_SOUL,            true  },
    { ACTOR_EN_GUEST,        0,          0,                           kAnyScene, RG_HYLIANS_SOUL,            true  },
    { ACTOR_EN_GO2,          0x1F,       GORON_DMT_ROLLING_SMALL,     kAnyScene, RG_GORONS_SOUL,             false },
    { ACTOR_EN_GO2,          0,          0,                           kAnyScene, RG_GORONS_SOUL,             true  },
    { ACTOR_EN_ZO,           0,          0,                           kAnyScene, RG_ZORAS_SOUL,              true  },
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_GATE_GUARD,         kAnyScene, RG_GERUDO_SOUL,             true  },
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_VALLEY_FLOOR,       kAnyScene, RG_GERUDO_SOUL,             true  },
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_NORMAL,             kAnyScene, RG_GERUDO_SOUL,             true, 0, 0, 0.0f, 0, false, GerudosFriendly },
    // Business scrubs hide in the ground (EnShopnuts) and pop out as EnDns once stunned; the Deku
    // Theater controller (EnDntDemo) runs the judges and the stage. In the Deku Tree, the three scrubs
    // of the order puzzle before Gohma (EnHintnuts 1-3) are statues too, keeping the boss door shut.
    // The Deku Tree's lone hint scrubs (EnHintnuts 0) stay enemies: they hold rooms whose bars only
    // open once they're beaten, so a statue would lock Link in. The Lost Woods target game
    // (EnDntNomal) is left alone too.
    { ACTOR_EN_SHOPNUTS,     0,          0,                           kAnyScene, RG_DEKU_SCRUBS_SOUL,        false },
    { ACTOR_EN_DNS,          0,          0,                           kAnyScene, RG_DEKU_SCRUBS_SOUL,        false },
    { ACTOR_EN_HINTNUTS,     0xFF,       1,                           kAnyScene, RG_DEKU_SCRUBS_SOUL,        false },
    { ACTOR_EN_HINTNUTS,     0xFF,       2,                           kAnyScene, RG_DEKU_SCRUBS_SOUL,        false },
    { ACTOR_EN_HINTNUTS,     0xFF,       3,                           kAnyScene, RG_DEKU_SCRUBS_SOUL,        false },
    { ACTOR_EN_DNT_DEMO,     0,          0,                           kAnyScene, RG_DEKU_SCRUBS_SOUL,        false, 0, 0, 0.0f, 0, true },
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
    const NpcMatcher* matcher = nullptr;
    bool blocking = false; // sliding around `matcher->orbit` to keep a path shut
    bool frozen = false;
    u32 frozenFrames = 0; // frame counters at the freeze, replayed while drawing the statue
    u32 frozenGameplayFrames = 0;
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
        if (matcher.active != nullptr && !matcher.active()) {
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

// Puts an orbit-blocking statue where its vanilla self would stand this frame: `orbit` units from
// home, on the line towards Link, facing him.
void SlideBlocker(Actor* actor, const Statue& statue) {
    Player* player = GET_PLAYER(gPlayState);
    s16 yaw = Math_Vec3f_Yaw(&actor->home.pos, &player->actor.world.pos);
    actor->world.pos.x = actor->home.pos.x + statue.matcher->orbit * Math_SinS(yaw);
    actor->world.pos.z = actor->home.pos.z + statue.matcher->orbit * Math_CosS(yaw);
    actor->shape.rot.y = actor->world.rot.y = Actor_WorldYawTowardActor(actor, &player->actor);
}

void Freeze(Actor* actor, Statue& statue) {
    statue.frozen = true;
    if (gPlayState != nullptr) {
        statue.frozenFrames = gPlayState->state.frames;
        statue.frozenGameplayFrames = gPlayState->gameplayFrames;
    }
    const NpcMatcher* matcher = statue.matcher;

    // An orbit-blocker standing still on its circle is guarding its path (one walking away has speed).
    if (matcher->orbit > 0.0f && actor->speedXZ == 0.0f) {
        f32 fromHome = Math_Vec3f_DistXZ(&actor->home.pos, &actor->world.pos);
        statue.blocking = fabsf(fromHome - matcher->orbit) < 0.5f;
    }

    // Kokiri and Mido fade in by distance over their first updates; a statue stays fully visible
    // and targetable whatever point of the fade it froze at.
    if (actor->id == ACTOR_EN_KO) {
        reinterpret_cast<EnKo*>(actor)->modelAlpha = 255.0f;
        actor->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    } else if (actor->id == ACTOR_EN_MD) {
        reinterpret_cast<EnMd*>(actor)->alpha = 255;
        actor->shape.shadowAlpha = 255;
        actor->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }

    if (statue.solid && gPlayState != nullptr) {
        Collider_InitCylinder(gPlayState, &statue.collider);
        Collider_SetCylinder(gPlayState, &statue.collider, actor, &sStatueCylinderInit);
        if (matcher->radius > 0 && matcher->height > 0) {
            statue.collider.dim.radius = matcher->radius;
            statue.collider.dim.height = matcher->height;
        } else if (actor->colChkInfo.cylRadius > 0 && actor->colChkInfo.cylHeight > 0) {
            statue.collider.dim.radius = actor->colChkInfo.cylRadius;
            statue.collider.dim.height = actor->colChkInfo.cylHeight;
        }
        if (statue.blocking && matcher->blockHeight > 0) {
            statue.collider.dim.height = matcher->blockHeight;
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
    statue.matcher = matcher;
    statue.savedNaviEnemyId = actor->naviEnemyId;
    actor->naviEnemyId = kStatueNaviEnemyId;
    Statue& stored = sStatues[actor] = statue;
    if (matcher->freezeOnInit) {
        Freeze(actor, stored);
    }
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

// The Gerudo who hands over the membership card (EnGe3) is spawned once, by the last carpenter to be
// freed, and is gone when Link leaves the room. If her soul was still missing then, she comes back the
// next time Link is in the hideout with her soul, so the card can still be collected.
s8 sCardGiverRoomTried = -1;

void MaybeRespawnCardGiver() {
    if (gPlayState->sceneNum != SCENE_THIEVES_HIDEOUT) {
        return;
    }
    const Rando::NpcSoul* soul = Rando::GetNpcSoul(RG_GERUDO_CARD_GIVER_SOUL);
    if (soul == nullptr || !Rando::IsNpcSoulShuffled(*soul) || !Flags_GetRandomizerInf(soul->randInf) ||
        !GET_EVENTCHKINF_CARPENTERS_FREE_ALL() || Flags_GetRandomizerInf(RAND_INF_TH_ITEM_FROM_LEADER_OF_FORTRESS)) {
        return;
    }
    // One try per room visit: a room without her object loaded can't spawn her.
    s8 room = gPlayState->roomCtx.curRoom.num;
    if (room == sCardGiverRoomTried) {
        return;
    }
    Player* player = GET_PLAYER(gPlayState);
    if (player->stateFlags1 & PLAYER_STATE1_IN_CUTSCENE) {
        return;
    }
    sCardGiverRoomTried = room;
    for (Actor* npc = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].head; npc != nullptr; npc = npc->next) {
        if (npc->id == ACTOR_EN_GE3) {
            return; // already here (a statue that just woke up)
        }
    }
    s16 facing = player->actor.shape.rot.y;
    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_GE3, player->actor.world.pos.x + 80.0f * Math_SinS(facing),
                player->actor.world.pos.y, player->actor.world.pos.z + 80.0f * Math_CosS(facing), 0,
                static_cast<s16>(facing + 0x8000), 0, 2);
}

void OnPlayerUpdate() {
    if (gPlayState == nullptr) {
        return;
    }
    for (auto& [actor, statue] : sStatues) {
        if (!statue.frozen) {
            continue;
        }
        if (statue.blocking) {
            SlideBlocker(actor, statue);
        }
        if (statue.hasCollider) {
            Collider_UpdateCylinder(actor, &statue.collider);
            CollisionCheck_SetOC(gPlayState, &gPlayState->colChkCtx, &statue.collider.base);
        }
    }
    MaybeRespawnCardGiver();
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

    COND_HOOK(OnSceneInit, shouldRegister, [](int16_t sceneNum) {
        sStatues.clear();
        sCardGiverRoomTried = -1;
    });
    COND_HOOK(OnActorInit, shouldRegister, OnActorInit);
    COND_HOOK(OnActorUpdate, shouldRegister, OnActorUpdate);
    COND_HOOK(ShouldActorUpdate, shouldRegister, ShouldActorUpdate);
    COND_HOOK(OnPlayerUpdate, shouldRegister, OnPlayerUpdate);
    COND_HOOK(OnActorDestroy, shouldRegister, OnActorDestroy);
    COND_HOOK(OnItemReceive, shouldRegister, OnItemReceive);
    COND_ID_HOOK(OnOpenText, kStatueNaviTextId, shouldRegister, BuildStatueNaviMessage);

    COND_VB_SHOULD(VB_DRAW_ACTOR_IN_GRAYSCALE, shouldRegister, {
        Actor* actor = va_arg(args, Actor*);
        Statue* statue = GetStatue(actor);
        if (statue != nullptr) {
            *should = true;
            // Some NPCs sway their limbs from the frame counter in their draw function (Zoras, Gerudo,
            // Anju, Guru-Guru...); replaying the frame they froze on keeps the statue still.
            if (statue->frozen) {
                gPlayState->state.frames = statue->frozenFrames;
                gPlayState->gameplayFrames = statue->frozenGameplayFrames;
            }
        }
    });

    // Kaepora Gaebora calls Link over by himself; a statue owl doesn't.
    COND_VB_SHOULD(VB_OWL_INTERACTION, shouldRegister, {
        Actor* owl = va_arg(args, Actor*);
        if (GetStatue(owl) != nullptr) {
            *should = false;
        }
    });

    // With a closed forest, the Kokiri at the exit stands aside once the Deku Tree is done, but only
    // once he has his soul back. (He only blocks child Link, as logic expects.)
    COND_VB_SHOULD(VB_OPEN_KOKIRI_FOREST, shouldRegister, {
        if (LINK_IS_CHILD && RAND_GET_OPTION(RSK_FOREST).Is(RO_CLOSED_FOREST_ON) &&
            IsSoulMissing(RG_KOKIRI_EXIT_GUARD_SOUL)) {
            *should = false;
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
