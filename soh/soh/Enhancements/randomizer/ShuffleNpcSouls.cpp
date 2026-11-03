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

#include <cstddef>
#include <cstring>
#include <unordered_map>
#include <vector>

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
#include "overlays/actors/ovl_En_Nb/z_en_nb.h"
#include "overlays/actors/ovl_En_Xc/z_en_xc.h"
#include "overlays/actors/ovl_En_Sa/z_en_sa.h"
#include "overlays/actors/ovl_En_Du/z_en_du.h"
#include "overlays/actors/ovl_En_Ru1/z_en_ru1.h"
#include "overlays/actors/ovl_En_Zl4/z_en_zl4.h"
#include "overlays/actors/ovl_En_Zl1/z_en_zl1.h"
#include "overlays/actors/ovl_Demo_Im/z_demo_im.h"
#include "overlays/actors/ovl_En_Ge3/z_en_ge3.h"
#include "overlays/actors/ovl_En_Ge2/z_en_ge2.h"
#include "overlays/actors/ovl_En_Heishi1/z_en_heishi1.h"
#include "overlays/actors/ovl_En_Heishi3/z_en_heishi3.h"
#include "overlays/actors/ovl_En_Ru2/z_en_ru2.h"
#include "overlays/actors/ovl_En_Rl/z_en_rl.h"
#include "overlays/actors/ovl_En_Zl2/z_en_zl2.h"
#include "overlays/actors/ovl_Demo_Du/z_demo_du.h"
#include "overlays/actors/ovl_Demo_Go/z_demo_go.h"
#include "overlays/actors/ovl_Demo_Sa/z_demo_sa.h"
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
//   sentry         when set and returning true, the statue is a lookout that still catches Link: it
//                  keeps running its own logic (so its vision is vanilla), but its position, facing and
//                  pose are put back after every update, also while it throws Link out.
//   puppet         the statue still plays its part in cutscenes: while one is running it keeps
//                  updating, so it follows its scripted path, but its pose is put back after every
//                  update. A statue gliding through the scene. Outside cutscenes it's fully frozen.
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
    bool (*sentry)() = nullptr;
    bool puppet = false;
};

bool AlwaysSentry() {
    return true;
}

// Gerudo only watch for Link until he has the membership card; then they're friendly NPCs.
bool SentryWithoutCard() {
    return !CHECK_QUEST_ITEM(QUEST_GERUDO_CARD);
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
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_HORSEBACK_ARCHERY,  kAnyScene, RG_HORSEBACK_ARCHER_SOUL,   true, 0, 0, 0.0f, 0, false, SentryWithoutCard },
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
    { ACTOR_EN_HEISHI2,      0xFF,       5,                           kAnyScene, RG_HYLIAN_GUARDS_SOUL,      true, 15, 70 }, // Kakariko gate guard
    { ACTOR_EN_MD,           0,          0,                           kAnyScene, RG_MIDO_SOUL,               true, 36, 46, 60.0f },
    { ACTOR_EN_KO,           0xFF,       ENKO_TYPE_CHILD_3, SCENE_KOKIRI_FOREST, RG_KOKIRI_SOUL,             true, 20, 46, 80.0f, 246 },
    { ACTOR_EN_KZ,           0,          0,                           kAnyScene, RG_KING_ZORA_SOUL,          true, 80, 120 },
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_GATE_OPERATOR,      kAnyScene, RG_GERUDO_SOUL,             true, 0, 0, 0.0f, 0, false, SentryWithoutCard },
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_TRAINING_GROUND_GUARD, kAnyScene, RG_GERUDO_SOUL,          true, 0, 0, 0.0f, 0, false, SentryWithoutCard },
    { ACTOR_EN_GE3,          0,          0,                           kAnyScene, RG_GERUDO_SOUL,             true, 20, 50, 0.0f, 0, true },
    { ACTOR_EN_GE2,          0xFF,       2,                           kAnyScene, RG_GERUDO_SOUL,             true, 20, 50, 0.0f, 0, true },
    // Carpenters, hideout and Kakariko alike: params & 3 is 0 Ichiro (1 torch cell), 1 Shiro (steep slope),
    // 2 Jiro (double cell), 3 Sabooro (dead end)
    { ACTOR_EN_DAIKU,        3,          0,                           kAnyScene, RG_ICHIRO_SOUL,             true  },
    { ACTOR_EN_DAIKU,        3,          1,                           kAnyScene, RG_SHIRO_SOUL,              true  },
    { ACTOR_EN_DAIKU,        3,          2,                           kAnyScene, RG_JIRO_SOUL,               true  },
    { ACTOR_EN_DAIKU,        3,          3,                           kAnyScene, RG_SABOORO_SOUL,            true  },
    { ACTOR_EN_DAIKU_KAKARIKO, 3,        0,                           kAnyScene, RG_ICHIRO_SOUL,             true  },
    { ACTOR_EN_DAIKU_KAKARIKO, 3,        1,                           kAnyScene, RG_SHIRO_SOUL,              true  },
    { ACTOR_EN_DAIKU_KAKARIKO, 3,        2,                           kAnyScene, RG_JIRO_SOUL,               true  },
    { ACTOR_EN_DAIKU_KAKARIKO, 3,        3,                           kAnyScene, RG_SABOORO_SOUL,            true  },
    { ACTOR_EN_TA,           0,          0,                           kAnyScene, RG_TALON_SOUL,              true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_TALON,            kAnyScene, RG_TALON_SOUL,              true  },
    { ACTOR_EN_MA1,          0,          0,                           kAnyScene, RG_MALON_SOUL,              true  },
    { ACTOR_EN_MA2,          0,          0,                           kAnyScene, RG_MALON_SOUL,              true  },
    { ACTOR_EN_MA3,          0,          0,                           kAnyScene, RG_MALON_SOUL,              true  },
    { ACTOR_EN_IN,           0,          0,                           kAnyScene, RG_INGO_SOUL,               true  },
    { ACTOR_EN_OSSAN,        kAnyParams, OSSAN_TYPE_INGO,             kAnyScene, RG_INGO_SOUL,               true  },
    { ACTOR_EN_OWL,          0,          0,                           kAnyScene, RG_KAEPORA_GAEBORA_SOUL,    false, 0, 0, 0.0f, 0, true },
    // Story. These all freeze before their first update: each one starts a cutscene or a forced
    // conversation by itself (Saria's greeting, Zelda noticing Link, Sheik's songs, Ruto's intro...).
    // They're puppets: in a running cutscene they still play their part, as statues.
    { ACTOR_EN_SA,           0,          0,                           kAnyScene, RG_SARIA_SOUL,              true, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_DU,           0,          0,                           kAnyScene, RG_DARUNIA_SOUL,            true, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_RU1,          0,          0,                           kAnyScene, RG_RUTO_SOUL,               true, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_NB,           0xFF,       NB_TYPE_CRAWLSPACE, SCENE_SPIRIT_TEMPLE, RG_NABOORU_SOUL,           true, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_NB,           0,          0,                           kAnyScene, RG_NABOORU_SOUL,            false, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_ZL4,          0,          0,                           kAnyScene, RG_ZELDA_SOUL,              true, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_ZL1,          0,          0,                           kAnyScene, RG_ZELDA_SOUL,              true, 0, 0, 0.0f, 0, true, nullptr, true },
    // Zelda's cutscene self; the Ganon's Tower escape Zelda (EnZl3) is gameplay and stays normal
    { ACTOR_EN_ZL2,          0,          0,                           kAnyScene, RG_ZELDA_SOUL,              false, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_XC,           kAnyParams, SHEIK_TYPE_MINUET,           kAnyScene, RG_SHEIK_SOUL,              true, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_XC,           kAnyParams, SHEIK_TYPE_BOLERO,           kAnyScene, RG_SHEIK_SOUL,              true, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_XC,           kAnyParams, SHEIK_TYPE_SERENADE,         kAnyScene, RG_SHEIK_SOUL,              true, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_XC,           kAnyParams, SHEIK_TYPE_RANDO,            kAnyScene, RG_SHEIK_SOUL,              true, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_XC,           0,          0,                           kAnyScene, RG_SHEIK_SOUL,              false, 0, 0, 0.0f, 0, true, nullptr, true }, // her cutscene types
    // Cutscene-only versions of story NPCs
    { ACTOR_DEMO_IM,         0,          0,                           kAnyScene, RG_IMPA_SOUL,               false, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_DEMO_SA,         0,          0,                           kAnyScene, RG_SARIA_SOUL,              false, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_DEMO_DU,         0,          0,                           kAnyScene, RG_DARUNIA_SOUL,            false, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_DEMO_GO,         0,          0,                           kAnyScene, RG_GORONS_SOUL,             false, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_RU2,          0,          0,                           kAnyScene, RG_RUTO_SOUL,               false, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_EN_RL,           0,          0,                           kAnyScene, RG_RAURU_SOUL,              false, 0, 0, 0.0f, 0, true, nullptr, true },
    { ACTOR_BG_DY_YOSEIZO,   0,          0,                           kAnyScene, RG_GREAT_FAIRIES_SOUL,      false, 0, 0, 0.0f, 0, true },
    // More townsfolk
    { ACTOR_EN_KO,           0,          0,      SCENE_KNOW_IT_ALL_BROS_HOUSE, RG_KNOW_IT_ALL_BROTHERS_SOUL, true },
    { ACTOR_EN_TG,           0,          0,                           kAnyScene, RG_HONEY_AND_DARLING_SOUL,  true  },
    { ACTOR_EN_HS2,          0,          0,                           kAnyScene, RG_GROG_SOUL,               true  }, // Grog as a child
    { ACTOR_EN_HEISHI4,      0,          0,                           kAnyScene, RG_HYLIAN_GUARDS_SOUL,      true  },
    // The castle guards who catch child Link sneaking in: frozen sentries that still see him
    { ACTOR_EN_HEISHI1,      0,          0,                           kAnyScene, RG_HYLIAN_GUARDS_SOUL,      true, 0, 0, 0.0f, 0, false, AlwaysSentry },
    { ACTOR_EN_HEISHI3,      0,          0,                           kAnyScene, RG_HYLIAN_GUARDS_SOUL,      true, 0, 0, 0.0f, 0, false, AlwaysSentry },
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
    // The fortress lookouts are frozen sentries until Link has the card: they still see him. The Gerudo
    // warriors (EnGeldB) stay enemies: they're fought for the cell keys.
    { ACTOR_EN_GE2,          0xFF,       0,                           kAnyScene, RG_GERUDO_SOUL,             true, 0, 0, 0.0f, 0, false, SentryWithoutCard }, // patrolling
    { ACTOR_EN_GE2,          0xFF,       1,                           kAnyScene, RG_GERUDO_SOUL,             true, 0, 0, 0.0f, 0, false, SentryWithoutCard }, // stationary
    { ACTOR_EN_GE1,          0xFF,       GE1_TYPE_NORMAL,             kAnyScene, RG_GERUDO_SOUL,             true, 0, 0, 0.0f, 0, false, SentryWithoutCard },
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
    bool blocking = false;  // sliding around `matcher->orbit` to keep a path shut
    bool sentry = false;    // a lookout: still updates, but is put back to its frozen pose every frame
    bool capturing = false; // a sentry that has caught Link and is seeing the capture through
    bool puppet = false;    // plays its part in cutscenes, pose pinned
    bool captureDone = false;
    bool frozen = false;
    u32 frozenFrames = 0; // frame counters at the freeze, replayed while drawing the statue
    u32 frozenGameplayFrames = 0;
    s32 updates = 0;
    s32 drawnUpdates = 0; // updates that ended with the actor drawable
    u8 savedNaviEnemyId = 0xFF;
    bool hasCollider = false;
    ColliderCylinder collider{};
    // A sentry's pose at the freeze
    Vec3f pinPos{};
    Vec3s pinWorldRot{};
    Vec3s pinShapeRot{};
    std::vector<Vec3s> pinJoints;
    bool jointsPinned = false;
    std::vector<u8> pinFace; // eye and mouth texture state, see GetFaceFields
    Vec3s pinHead[2]{};
    f32 pinHeadAngle = 0.0f;
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

// Puts an orbit-blocking statue where its vanilla self would stand this frame: `orbit` units from
// home, on the line towards Link, facing him.
void SlideBlocker(Actor* actor, const Statue& statue) {
    Player* player = GET_PLAYER(gPlayState);
    s16 yaw = Math_Vec3f_Yaw(&actor->home.pos, &player->actor.world.pos);
    actor->world.pos.x = actor->home.pos.x + statue.matcher->orbit * Math_SinS(yaw);
    actor->world.pos.z = actor->home.pos.z + statue.matcher->orbit * Math_CosS(yaw);
    actor->shape.rot.y = actor->world.rot.y = Actor_WorldYawTowardActor(actor, &player->actor);
}

// An actor frozen before its first update has an animation set but no pose computed yet, so it would be
// drawn in its skeleton's bind pose (Saria lying half in the ground). Compute the pose once, without
// blending from that bind pose.
template <typename T> SkelAnime* SkelAnimeOf(Actor* actor) {
    return &reinterpret_cast<T*>(actor)->skelAnime;
}

// The skeleton of a statue whose pose is snapped or pinned, or nullptr.
SkelAnime* GetStatueSkelAnime(Actor* actor) {
    switch (actor->id) {
        case ACTOR_EN_SA:
            return SkelAnimeOf<EnSa>(actor);
        case ACTOR_EN_DU:
            return SkelAnimeOf<EnDu>(actor);
        case ACTOR_EN_RU1:
            return SkelAnimeOf<EnRu1>(actor);
        case ACTOR_EN_RU2:
            return SkelAnimeOf<EnRu2>(actor);
        case ACTOR_EN_NB:
            return SkelAnimeOf<EnNb>(actor);
        case ACTOR_EN_ZL1:
            return SkelAnimeOf<EnZl1>(actor);
        case ACTOR_EN_ZL2:
            return SkelAnimeOf<EnZl2>(actor);
        case ACTOR_EN_ZL4:
            return SkelAnimeOf<EnZl4>(actor);
        case ACTOR_EN_XC:
            return SkelAnimeOf<EnXc>(actor);
        case ACTOR_EN_RL:
            return SkelAnimeOf<EnRl>(actor);
        case ACTOR_DEMO_IM:
            return SkelAnimeOf<DemoIm>(actor);
        case ACTOR_DEMO_SA:
            return SkelAnimeOf<DemoSa>(actor);
        case ACTOR_DEMO_DU:
            return SkelAnimeOf<DemoDu>(actor);
        case ACTOR_DEMO_GO:
            return SkelAnimeOf<DemoGo>(actor);
        case ACTOR_EN_GE3:
            return SkelAnimeOf<EnGe3>(actor);
        case ACTOR_EN_HEISHI1:
            return SkelAnimeOf<EnHeishi1>(actor);
        case ACTOR_EN_HEISHI3:
            return SkelAnimeOf<EnHeishi3>(actor);
        case ACTOR_EN_GE1:
            return SkelAnimeOf<EnGe1>(actor);
        case ACTOR_EN_GE2:
            return SkelAnimeOf<EnGe2>(actor);
        default:
            return nullptr;
    }
}

// An actor frozen before its first update has an animation set but no pose computed yet, so it would be
// drawn in its skeleton's bind pose (Saria lying half in the ground). Compute the pose once, without
// blending from that bind pose.
void SnapPoseBeforeFirstUpdate(Actor* actor) {
    SkelAnime* skelAnime = GetStatueSkelAnime(actor);
    if (skelAnime != nullptr && skelAnime->skeleton != nullptr && skelAnime->animation != nullptr) {
        skelAnime->morphWeight = 0.0f;
        SkelAnime_Update(skelAnime);
    }
}

// The fields an actor picks its eye and mouth textures from. Its update changes them (blinking, talking),
// so a statue that still updates (a sentry or a puppet) has them put back with its pose.
struct FaceField {
    size_t offset;
    size_t size;
};
static constexpr FaceField MakeFaceField(size_t offset, size_t size) {
    return { offset, size };
}
#define FACE_FIELD(type, field) MakeFaceField(offsetof(type, field), sizeof(type::field))

std::vector<FaceField> GetFaceFields(Actor* actor) {
    switch (actor->id) {
        case ACTOR_EN_SA:
            return { FACE_FIELD(EnSa, rightEyeIndex), FACE_FIELD(EnSa, leftEyeIndex), FACE_FIELD(EnSa, mouthIndex) };
        case ACTOR_EN_DU:
            return { FACE_FIELD(EnDu, eyeTexIndex), FACE_FIELD(EnDu, mouthTexIndex) };
        case ACTOR_EN_RU1:
            return { FACE_FIELD(EnRu1, eyeIndex), FACE_FIELD(EnRu1, mouthIndex) };
        case ACTOR_EN_RU2:
            return { FACE_FIELD(EnRu2, eyeIndex) };
        case ACTOR_EN_NB:
            return { FACE_FIELD(EnNb, eyeIdx) };
        case ACTOR_EN_ZL1:
            return { FACE_FIELD(EnZl1, unk_1EC), FACE_FIELD(EnZl1, unk_1F4), FACE_FIELD(EnZl1, unk_1F8) };
        case ACTOR_EN_ZL2:
            return { FACE_FIELD(EnZl2, eyeTexIndex), FACE_FIELD(EnZl2, eyeTexIndex2),
                     FACE_FIELD(EnZl2, mouthTexIndex) };
        case ACTOR_EN_ZL4:
            return { FACE_FIELD(EnZl4, leftEyeState), FACE_FIELD(EnZl4, rightEyeState), FACE_FIELD(EnZl4, mouthState) };
        case ACTOR_EN_XC:
            return { FACE_FIELD(EnXc, eyeIdx) };
        case ACTOR_EN_RL:
            return { FACE_FIELD(EnRl, eyeTextureIndex) };
        case ACTOR_DEMO_IM:
            return { FACE_FIELD(DemoIm, eyeIndex) };
        case ACTOR_DEMO_SA:
            return { FACE_FIELD(DemoSa, eyeIndex), FACE_FIELD(DemoSa, mouthIndex) };
        case ACTOR_DEMO_DU:
            return { FACE_FIELD(DemoDu, eyeTexIndex), FACE_FIELD(DemoDu, mouthTexIndex) };
        case ACTOR_DEMO_GO:
            return { FACE_FIELD(DemoGo, unk_190) }; // eye texture index
        case ACTOR_EN_GE1:
            return { FACE_FIELD(EnGe1, eyeIndex) };
        case ACTOR_EN_GE2:
            return { FACE_FIELD(EnGe2, eyeIndex) };
        case ACTOR_EN_GE3:
            return { FACE_FIELD(EnGe3, eyeIndex) };
        default:
            return {};
    }
}
#undef FACE_FIELD

// Pins the statue's pose once it has one. Some cutscene actors spawn without an animation and only get
// their first pose when their part starts (Darunia, falling into the Goron's Ruby cutscene); pinning the
// empty pose before that would draw nothing. A pose still blending in from another one isn't kept either.
void PinJoints(Actor* actor, Statue& statue) {
    SkelAnime* skelAnime = GetStatueSkelAnime(actor);
    if (skelAnime == nullptr || skelAnime->jointTable == nullptr || skelAnime->animation == nullptr ||
        skelAnime->morphWeight > 0.0f) {
        return;
    }
    statue.pinJoints.assign(skelAnime->jointTable, skelAnime->jointTable + skelAnime->limbCount);
    statue.jointsPinned = true;
    statue.pinFace.clear();
    for (const FaceField& field : GetFaceFields(actor)) {
        const u8* src = reinterpret_cast<const u8*>(actor) + field.offset;
        statue.pinFace.insert(statue.pinFace.end(), src, src + field.size);
    }
}

// Puts the pinned pose back, or pins it if the statue has only just got one.
void RestoreJoints(Actor* actor, Statue& statue) {
    if (!statue.jointsPinned) {
        PinJoints(actor, statue);
        return;
    }
    SkelAnime* skelAnime = GetStatueSkelAnime(actor);
    if (skelAnime != nullptr && skelAnime->jointTable != nullptr &&
        statue.pinJoints.size() == static_cast<size_t>(skelAnime->limbCount)) {
        std::copy(statue.pinJoints.begin(), statue.pinJoints.end(), skelAnime->jointTable);
    }
    size_t at = 0;
    for (const FaceField& field : GetFaceFields(actor)) {
        if (at + field.size > statue.pinFace.size()) {
            break;
        }
        memcpy(reinterpret_cast<u8*>(actor) + field.offset, statue.pinFace.data() + at, field.size);
        at += field.size;
    }
}

bool IsCutsceneRunning() {
    return gPlayState != nullptr && gPlayState->csCtx.state != CS_STATE_IDLE;
}

// Kokiri are drawable from the start but only build their skeleton once their objects have loaded;
// freezing one before that would leave it without a skeleton.
bool IsReadyToFreeze(Actor* actor) {
    if (actor->id == ACTOR_EN_KO) {
        return reinterpret_cast<EnKo*>(actor)->skelAnime.skeleton != nullptr;
    }
    return true;
}

// An orbit-blocker standing still on its circle is guarding its path (one walking away has speed).
bool IsOnBlockingCircle(Actor* actor, const NpcMatcher* matcher) {
    return matcher->orbit > 0.0f && actor->speedXZ == 0.0f &&
           fabsf(Math_Vec3f_DistXZ(&actor->home.pos, &actor->world.pos) - matcher->orbit) < 0.5f;
}

// The head and torso angles each lookout adds in its draw function (it turns them in its update).
void CopySentryHead(Actor* actor, Statue& statue, bool save) {
    auto copy = [save](auto& field, auto& pinned) {
        if (save) {
            pinned = field;
        } else {
            field = pinned;
        }
    };
    switch (actor->id) {
        case ACTOR_EN_HEISHI1:
            copy(reinterpret_cast<EnHeishi1*>(actor)->headAngle, statue.pinHeadAngle);
            break;
        case ACTOR_EN_HEISHI3: {
            EnHeishi3* heishi = reinterpret_cast<EnHeishi3*>(actor);
            copy(heishi->unk_262, statue.pinHead[0].x);
            copy(heishi->unk_264, statue.pinHead[0].y);
            copy(heishi->unk_26E, statue.pinHead[0].z);
            break;
        }
        case ACTOR_EN_GE1:
            copy(reinterpret_cast<EnGe1*>(actor)->headRot, statue.pinHead[0]);
            break;
        case ACTOR_EN_GE2:
            copy(reinterpret_cast<EnGe2*>(actor)->headRot, statue.pinHead[0]);
            copy(reinterpret_cast<EnGe2*>(actor)->unk_2EE, statue.pinHead[1]);
            break;
        default:
            break;
    }
}

void PinSentryPose(Actor* actor, Statue& statue) {
    statue.pinPos = actor->world.pos;
    statue.pinWorldRot = actor->world.rot;
    statue.pinShapeRot = actor->shape.rot;
    PinJoints(actor, statue);
    CopySentryHead(actor, statue, true);
}

void RestoreSentryPose(Actor* actor, Statue& statue) {
    actor->world.pos = statue.pinPos;
    // While capturing, its facing (world.rot) is left alone: a Gerudo only charges once she faces Link.
    // What's drawn (shape.rot) stays put.
    if (!statue.capturing) {
        actor->world.rot = statue.pinWorldRot;
    }
    actor->shape.rot = statue.pinShapeRot;
    actor->speedXZ = 0.0f;
    RestoreJoints(actor, statue);
    CopySentryHead(actor, statue, false);
}

// A sentry that caught Link stays a statue. Most captures end on a timer or once the text is dismissed,
// which still happens. The courtyard guard only throws Link out after walking up to him, which a statue
// can't, so his throw-out is done here when his "Hey you!" text is dismissed, as his kick would.
void FinishSentryCapture(Actor* actor, Statue& statue) {
    if (actor->id != ACTOR_EN_HEISHI1 || statue.captureDone) {
        return;
    }
    if ((Message_GetState(&gPlayState->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(gPlayState)) {
        Message_CloseTextbox(gPlayState);
        Flags_SetEventChkInf(EVENTCHKINF_CAUGHT_BY_CASTLE_GUARDS);
        gPlayState->nextEntranceIndex = ENTR_HYRULE_CASTLE_3;
        gPlayState->transitionTrigger = TRANS_TRIGGER_START;
        gPlayState->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
        gSaveContext.nextTransitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
        statue.captureDone = true;
    }
}

void Freeze(Actor* actor, Statue& statue) {
    statue.frozen = true;
    if (gPlayState != nullptr) {
        statue.frozenFrames = gPlayState->state.frames;
        statue.frozenGameplayFrames = gPlayState->gameplayFrames;
    }
    const NpcMatcher* matcher = statue.matcher;

    statue.blocking = IsOnBlockingCircle(actor, matcher);
    if (matcher->sentry != nullptr && matcher->sentry()) {
        // A sentry keeps its own collider (it still updates), so it needs none of ours.
        statue.sentry = true;
        statue.solid = false;
        PinSentryPose(actor, statue);
    }
    if (matcher->puppet) {
        statue.puppet = true;
        PinJoints(actor, statue);
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
        SnapPoseBeforeFirstUpdate(actor);
        Freeze(actor, stored);
    }
}

void OnActorUpdate(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);
    Statue* statue = GetStatue(actor);
    if (statue == nullptr) {
        return;
    }
    if (statue->frozen) {
        if (statue->sentry) {
            if (gPlayState != nullptr && GET_PLAYER(gPlayState)->csActor == actor) {
                statue->capturing = true;
            }
            if (statue->capturing) {
                FinishSentryCapture(actor, *statue);
            }
            RestoreSentryPose(actor, *statue);
        } else if (statue->puppet) {
            RestoreJoints(actor, *statue); // it moved along the cutscene; its pose stays
        }
        return;
    }
    if (!IsReadyToFreeze(actor)) {
        return; // keeps updating (still can't be talked to) until it can be frozen
    }
    // The update that makes an actor drawable is often the one where it finishes setting up, before it
    // has acted once. Count from there so it gets one real update.
    statue->updates++;
    if (actor->draw != nullptr) {
        statue->drawnUpdates++;
    }
    bool settled = statue->drawnUpdates >= kUpdatesBeforeFreeze;
    // A path blocker takes its spot on its circle in its first updates; give it the time to.
    if (settled && statue->matcher->orbit > 0.0f && !IsOnBlockingCircle(actor, statue->matcher) &&
        statue->updates < kMaxUpdatesBeforeFreeze) {
        settled = false;
    }
    if (settled || statue->updates >= kMaxUpdatesBeforeFreeze) {
        Freeze(actor, *statue);
    }
}

void ShouldActorUpdate(void* actorRef, bool* result) {
    Statue* statue = GetStatue(static_cast<Actor*>(actorRef));
    if (statue != nullptr && statue->frozen && !statue->sentry && !(statue->puppet && IsCutsceneRunning())) {
        *result = false;
    }
}

// The Gerudo who hands over the membership card (EnGe3) is spawned once, by the last carpenter to be
// freed, and is gone when Link leaves the room. If the Gerudo soul was still missing then, she comes back the
// next time Link is in the hideout with it, so the card can still be collected.
s8 sCardGiverRoomTried = -1;

void MaybeRespawnCardGiver() {
    if (gPlayState->sceneNum != SCENE_THIEVES_HIDEOUT) {
        return;
    }
    const Rando::NpcSoul* soul = Rando::GetNpcSoul(RG_GERUDO_SOUL);
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
        if (LINK_IS_CHILD && RAND_GET_OPTION(RSK_FOREST).Is(RO_CLOSED_FOREST_ON) && IsSoulMissing(RG_KOKIRI_SOUL)) {
            *should = false;
        }
    });

    // Story checks that aren't handed over by an actor we can freeze (or that are decided before its first
    // update) are switched off at their eligibility check instead.
#define NO_SOUL_NO_EVENT(vb, soul)       \
    COND_VB_SHOULD(vb, shouldRegister, { \
        if (IsSoulMissing(soul)) {       \
            *should = false;             \
        }                                \
    })
    // A sage still holding their reward hands nothing over in the cutscene, so it shows nothing either.
    COND_VB_SHOULD(VB_DRAW_CUTSCENE_REWARD_ITEM, shouldRegister, {
        RandomizerCheck rc = static_cast<RandomizerCheck>(va_arg(args, int));
        RandomizerGet sageSoul = Rando::GetSageRewardSoul(rc);
        if (sageSoul != RG_NONE && IsSoulMissing(sageSoul)) {
            *should = false;
        }
    });

    NO_SOUL_NO_EVENT(VB_BE_ELIGIBLE_FOR_SARIAS_SONG, RG_SARIA_SOUL);
    NO_SOUL_NO_EVENT(VB_BE_ELIGIBLE_FOR_SARIAS_GIFT, RG_SARIA_SOUL);
    NO_SOUL_NO_EVENT(VB_BE_ELIGIBLE_FOR_DARUNIAS_JOY_REWARD, RG_DARUNIA_SOUL);
    NO_SOUL_NO_EVENT(VB_BE_ELIGIBLE_FOR_OCARINA_OF_TIME, RG_ZELDA_SOUL);
    NO_SOUL_NO_EVENT(VB_BE_ELIGIBLE_FOR_PRELUDE_OF_LIGHT, RG_SHEIK_SOUL);
    NO_SOUL_NO_EVENT(VB_BE_ELIGIBLE_FOR_SERENADE_OF_WATER, RG_SHEIK_SOUL);
    NO_SOUL_NO_EVENT(VB_BE_ELIGIBLE_FOR_REQUIEM_OF_SPIRIT, RG_SHEIK_SOUL);
    NO_SOUL_NO_EVENT(VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW, RG_SHEIK_SOUL);
    NO_SOUL_NO_EVENT(VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS, RG_ZELDA_SOUL);
#undef NO_SOUL_NO_EVENT

    // A statue that hasn't frozen yet may still offer to talk during its first updates.
    COND_VB_SHOULD(VB_SPEAK, shouldRegister, {
        Actor* talkActor = GET_PLAYER(gPlayState)->talkActor;
        if (talkActor != nullptr && GetStatue(talkActor) != nullptr) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterShuffleNpcSouls, { "IS_RANDO" });
