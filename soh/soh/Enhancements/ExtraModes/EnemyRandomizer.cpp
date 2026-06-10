#include "functions.h"
#include "macros.h"
#include "soh/ShipUtils.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/ObjectExtension/ObjectExtension.h"
#include "variables.h"
#include "soh/cvar_prefixes.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/SohGui/MenuTypes.h"
#include "soh/SohGui/SohMenu.h"

extern "C" {
#include <z64.h>
#include "src/overlays/actors/ovl_Bg_Haka/z_bg_haka.h"
#include "src/overlays/actors/ovl_Bg_Haka_Huta/z_bg_haka_huta.h"
#include "src/overlays/actors/ovl_Bg_Haka_Tubo/z_bg_haka_tubo.h"
#include "src/overlays/actors/ovl_Bg_Mori_Bigst/z_bg_mori_bigst.h"
#include "src/overlays/actors/ovl_En_Blkobj/z_en_blkobj.h"
#include "src/overlays/actors/ovl_En_Encount1/z_en_encount1.h"
#include "src/overlays/actors/ovl_En_GeldB/z_en_geldb.h"
#include "src/overlays/actors/ovl_En_Peehat/z_en_peehat.h"
#include "src/overlays/actors/ovl_En_Rr/z_en_rr.h"
#include "src/overlays/actors/ovl_En_Vali/z_en_vali.h"

extern PlayState* gPlayState;
}

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
}

#define CVAR_ENEMY_RANDOMIZER_NAME CVAR_ENHANCEMENT("RandomizedEnemies")
#define CVAR_ENEMY_RANDOMIZER_DEFAULT ENEMY_RANDOMIZER_OFF
#define CVAR_ENEMY_RANDOMIZER_VALUE CVarGetInteger(CVAR_ENEMY_RANDOMIZER_NAME, CVAR_ENEMY_RANDOMIZER_DEFAULT)
#define ENEMY_RANDOMIZER_ENABLED CVAR_ENEMY_RANDOMIZER_VALUE != CVAR_ENEMY_RANDOMIZER_DEFAULT

typedef struct EnemyEntry {
    const char* cvar;
    const char* name;
    s16 id;
    s16 params;
} EnemyEntry;

// clang-format off
static EnemyEntry randomizedEnemySpawnTable[] = {
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Anubis"),           "Anubis",                ACTOR_EN_ANUBICE_TAG,      1 }, // Anubis
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Armos"),            "Armos",                 ACTOR_EN_AM,              -1 }, // Armos
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Arwing"),           "Arwing",                ACTOR_EN_CLEAR_TAG,        1 }, // Arwing
    { CVAR_ENHANCEMENT("RandomizedEnemyList.BabyDodongo"),      "Baby Dodongo",          ACTOR_EN_DODOJR,           0 }, // Baby Dodongo
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Bari"),             "Bari",                  ACTOR_EN_VALI,            -1 }, // Bari (big jellyfish)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Beamos"),           "Beamos",                ACTOR_EN_VM,            1280 }, // Beamos
    { CVAR_ENHANCEMENT("RandomizedEnemyList.BigSkulltula"),     "Big Skulltula",         ACTOR_EN_ST,               1 }, // Skulltula (big)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.BigStalchild"),     "Stalchild (Big)",       ACTOR_EN_SKB,             20 }, // Stalchild (big)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Biri"),             "Biri",                  ACTOR_EN_BILI,             0 }, // Biri (jellyfish)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.BlackKnuckle"),     "Iron Knuckle (Black)",  ACTOR_EN_IK,               2 }, // Iron Knuckle (black, standing)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.BlueTektite"),      "Blue Tektite",          ACTOR_EN_TITE,            -2 }, // Tektite (blue)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Bubble"),           "Bubble",                ACTOR_EN_BB,              -1 }, // Bubble (flying skull enemy) (blue)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.ClubMoblin"),       "Club Moblin",           ACTOR_EN_MB,               0 }, // Club Moblin
    { CVAR_ENHANCEMENT("RandomizedEnemyList.DarkLink"),         "Dark Link",             ACTOR_EN_TORCH2,           0 }, // Dark Link
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Dinolfos"),         "Dinolfos",              ACTOR_EN_ZF,              -2 }, // Dinolfos
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Dodongo"),          "Dodongo",               ACTOR_EN_DODONGO,         -1 }, // Dodongo
    { CVAR_ENHANCEMENT("RandomizedEnemyList.FireKeese"),        "Fire Keese",            ACTOR_EN_FIREFLY,          1 }, // Fire Keese
    // { CVAR_ENHANCEMENT("RandomizedEnemyList.FlareDancer"),      "Flare Dancer",          ACTOR_EN_FD,               0 }, // Flare Dancer (possible cause of crashes because of spawning flame actors on sloped ground or overloading)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.FloorTile"),        "Floor Tile",            ACTOR_EN_YUKABYUN,         0 }, // Flying Floor Tile
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Floormaster"),      "Floormaster",           ACTOR_EN_FLOORMAS,         0 }, // Floormaster
    { CVAR_ENHANCEMENT("RandomizedEnemyList.FlyingPeahat"),     "Flying Peahat",         ACTOR_EN_PEEHAT,          -1 }, // Flying Peahat (big grounded, doesn't spawn larva)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.FlyingPot"),        "Flying Pot",            ACTOR_EN_TUBO_TRAP,        0 }, // Flying pot
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Freezard"),         "Freezard",              ACTOR_EN_FZ,               0 }, // Freezard
    { CVAR_ENHANCEMENT("RandomizedEnemyList.GerudoFighter"),    "Gerudo Fighter",        ACTOR_EN_GELDB,            0 }, // Gerudo Fighter
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Gibdo"),            "Gibdo",                 ACTOR_EN_RD,           32766 }, // Gibdo (standing)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.GohmaLarva"),       "Gohma Larva",           ACTOR_EN_GOMA,             7 }, // Gohma Larva (Non-Gohma rooms)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Guay"),             "Guay",                  ACTOR_EN_CROW,             0 }, // Guay
    { CVAR_ENHANCEMENT("RandomizedEnemyList.IceKeese"),         "Ice Keese",             ACTOR_EN_FIREFLY,          4 }, // Ice Keese
    { CVAR_ENHANCEMENT("RandomizedEnemyList.InvisFireKeese"),   "Invisible Fire Keese",  ACTOR_EN_FIREFLY,     0x8001 }, // Fire Keese (invisible)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.InvisFloormaster"), "Invisible Floormaster", ACTOR_EN_FLOORMAS,    0x8000 }, // Floormaster (invisible)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.InvisIceKeese"),    "Invisible Ice Keese",   ACTOR_EN_FIREFLY,     0x8004 }, // Ice Keese (invisible)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.InvisKeese"),       "Invisible Keese",       ACTOR_EN_FIREFLY,     0x8002 }, // Keese (invisible)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.InvisRedead"),      "Invisible Redead",      ACTOR_EN_RD,               3 }, // Redead (invisible)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.InvisSkulltula"),   "Invisible Skulltula",   ACTOR_EN_ST,               2 }, // Skulltula (invisible)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.InvisStalfos"),     "Invisible Stalfos",     ACTOR_EN_TEST,             0 }, // Stalfos (invisible)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Keese"),            "Keese",                 ACTOR_EN_FIREFLY,          2 }, // Regular Keese
    { CVAR_ENHANCEMENT("RandomizedEnemyList.LargeBaba"),        "Large Deku Baba",       ACTOR_EN_DEKUBABA,         1 }, // Deku Baba (large)
    // { CVAR_ENHANCEMENT("RandomizedEnemyList.Leever"),           "Leever",                ACTOR_EN_REEBA,            0 }, // Leever       Doesn't work (reliant on surface, without a spawner it kills itself too quickly)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.LikeLike"),         "Like-Like",             ACTOR_EN_RR,               0 }, // Like-Like
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Lizalfos"),         "Lizalfos",              ACTOR_EN_ZF,              -1 }, // Lizalfos
    { CVAR_ENHANCEMENT("RandomizedEnemyList.MadScrub"),         "Mad Scrub",             ACTOR_EN_DEKUNUTS,       768 }, // Mad Scrub (triple attack) (projectiles don't work)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.NormalWolfos"),     "Wolfos (Normal)",       ACTOR_EN_WF,               0 }, // Wolfos (normal)
    // { CVAR_ENHANCEMENT("RandomizedEnemyList.Octorok"),          "Octorok",               ACTOR_EN_OKUTA,            0 }, // Octorok                 Doesn't work (actor directly uses water box collision to handle hiding/popping up)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.PeahatLarva"),      "Peahat Larva",          ACTOR_EN_PEEHAT,           1 }, // Flying Peahat Larva
    // { CVAR_ENHANCEMENT("RandomizedEnemyList.Poe"),              "Poe",                   ACTOR_EN_POH,              0 }, // Poe                         Doesn't work (Seems to rely on other objects?)
    // { CVAR_ENHANCEMENT("RandomizedEnemyList.Poe.Sharp"),        "Poe (Sharp)",           ACTOR_EN_POH,              2 }, // Poe (composer Sharp)        Doesn't work (Seems to rely on other objects?)
    // { CVAR_ENHANCEMENT("RandomizedEnemyList.Poe.Flat"),         "Poe (Flat)",            ACTOR_EN_POH,              3 }, // Poe (composer Flat)         Doesn't work (Seems to rely on other objects?)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Redead"),           "Redead",                ACTOR_EN_RD,               1 }, // Redead (standing)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.RedTektite"),       "Red Tektite",           ACTOR_EN_TITE,            -1 }, // Tektite (red)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Shabom"),           "Shabom",                ACTOR_EN_BUBBLE,           0 }, // Shabom (bubble)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.ShellBlade"),       "Shell Blade",           ACTOR_EN_SB,               0 }, // Shell Blade
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Skulltula"),        "Skulltula",             ACTOR_EN_ST,               0 }, // Skulltula (normal)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.SkullKid"),         "Skull Kid",             ACTOR_EN_SKJ,           4159 }, // Skull Kid
    { CVAR_ENHANCEMENT("RandomizedEnemyList.SmallBaba"),        "Small Deku Baba",       ACTOR_EN_DEKUBABA,         0 }, // Deku Baba (small)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.SmallStalchild"),   "Stalchild (Small)",     ACTOR_EN_SKB,              1 }, // Stalchild (small)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.SpearMoblin"),      "Spear Moblin",          ACTOR_EN_MB,              -1 }, // Spear Moblin
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Spike"),            "Spike",                 ACTOR_EN_NY,               0 }, // Spike (rolling enemy)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Stalfos"),          "Stalfos",               ACTOR_EN_TEST,             2 }, // Stalfos
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Stinger"),          "Stinger",               ACTOR_EN_EIYER,           10 }, // Stinger (land) (One in formation, sink under floor and do not activate)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Tailparasan"),      "Tailpasaran",           ACTOR_EN_TP,              -1 }, // Electric Tailpasaran
    { CVAR_ENHANCEMENT("RandomizedEnemyList.TorchSlug"),        "Torch Slug",            ACTOR_EN_BW,               0 }, // Torch Slug
    { CVAR_ENHANCEMENT("RandomizedEnemyList.Wallmaster"),       "Wallmaster",            ACTOR_EN_WALLMAS,          1 }, // Wallmaster
    { CVAR_ENHANCEMENT("RandomizedEnemyList.WhiteKnuckle"),     "Iron Knuckle (White)",  ACTOR_EN_IK,               3 }, // Iron Knuckle (white, standing)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.WhiteWolfos"),      "Wolfos (White)",        ACTOR_EN_WF,               1 }, // Wolfos (white)
    { CVAR_ENHANCEMENT("RandomizedEnemyList.WitheredBaba"),     "Withered Deku Baba",    ACTOR_EN_KAREBABA,         0 }, // Withered Deku Baba
};
// clang-format on

static int enemiesToRandomize[] = {
    ACTOR_EN_ANUBICE_TAG, // Anubis
    ACTOR_EN_FIREFLY,     // Keese (including fire/ice)
    ACTOR_EN_TEST,        // Stalfos
    ACTOR_EN_TITE,        // Tektite
    ACTOR_EN_POH,         // Poe (normal, blue rupee, composers)
    ACTOR_EN_OKUTA,       // Octorok
    ACTOR_EN_WALLMAS,     // Wallmaster
    ACTOR_EN_DODONGO,     // Dodongo
    ACTOR_EN_PEEHAT,      // Flying Peahat, big one spawning larva, larva
    ACTOR_EN_ZF,          // Lizalfos, Dinolfos
    ACTOR_EN_GOMA,        // Gohma Larva (normal, eggs, gohma eggs)
    ACTOR_EN_BUBBLE,      // Shabom (bubble)
    ACTOR_EN_DODOJR,      // Baby Dodongo
    ACTOR_EN_TORCH2,      // Dark Link
    ACTOR_EN_BILI,        // Biri (small jellyfish)
    ACTOR_EN_TP,          // Electric Tailpasaran
    ACTOR_EN_ST,          // Skulltula (normal, big, invisible)
    ACTOR_EN_BW,          // Torch Slug
    ACTOR_EN_EIYER,       // Stinger (land)
    ACTOR_EN_MB,          // Moblins (Club, spear)
    ACTOR_EN_DEKUBABA,    // Deku Baba (small, large)
    ACTOR_EN_AM,          // Armos (enemy variant)
    ACTOR_EN_DEKUNUTS,    // Mad Scrub (single attack, triple attack)
    ACTOR_EN_VALI,        // Bari (big jellyfish) (spawns very high up)
    ACTOR_EN_BB,          // Bubble (flying skull enemy) (all colors)
    ACTOR_EN_YUKABYUN,    // Flying Floor Tile
    ACTOR_EN_VM,          // Beamos
    ACTOR_EN_FLOORMAS,    // Floormaster
    ACTOR_EN_RD,          // Redead, Gibdo
    ACTOR_EN_SW,          // Skullwalltula
    ACTOR_EN_FD,          // Flare Dancer
    ACTOR_EN_SB,          // Shell Blade
    ACTOR_EN_KAREBABA,    // Withered Deku Baba
    ACTOR_EN_RR,          // Like-Like
    ACTOR_EN_NY,          // Spike (rolling enemy)
    ACTOR_EN_IK,          // Iron Knuckle
    ACTOR_EN_TUBO_TRAP,   // Flying pot
    ACTOR_EN_FZ,          // Freezard
    ACTOR_EN_WEIYER,      // Stinger (Water)
    ACTOR_EN_HINTNUTS,    // Hint Deku Scrubs
    ACTOR_EN_WF,          // Wolfos
    ACTOR_EN_SKB,         // Stalchild
    ACTOR_EN_CROW,        // Guay
    ACTOR_EN_SKJ,         // Skull Kid
    // ACTOR_EN_REEBA,      // Leever (reliant on spawner (z_en_encount1.c))
};

static bool IsExcludedFromClearRooms(s16 enemyId, s16 enemyParams) {
    switch (enemyId) {
        // Freezard - Child Link can only kill this with Deku Stick jumpslash or other equipment like bombs
        case ACTOR_EN_FZ:
        // Beamos - Needs bombs
        case ACTOR_EN_VM:
        // Shell Blade - It's annoying to kill these as Child Link with sword or Deku Stick
        case ACTOR_EN_SB:
        // Spike - Child Link can't kill these with sword or Deku Stick
        case ACTOR_EN_NY:
        // Arwing - Goes out of bounds way too easily, softlocking the player
        case ACTOR_EN_CLEAR_TAG:
        // Wallmaster - Not easily visible, often makes players think they're softlocked and that there's no enemies
        // left
        case ACTOR_EN_WALLMAS:
        // Dark Link - Goes out of bounds way too easily, softlocking the player
        case ACTOR_EN_TORCH2:
        // Flare dancer - Goes out of bounds way too easily, softlocking the player
        case ACTOR_EN_FD:
        // Anubis - Needs fire
        case ACTOR_EN_ANUBICE_TAG:
            return true;
        case ACTOR_EN_MB:
            return enemyParams == 0;
        default:
            return false;
    }
}

static bool IsExcludedFromTimedRooms(s16 enemyId, s16 enemyParams) {
    switch (enemyId) {
        // Bari - Spawns 3 more enemies, potentially extremely difficult in timed rooms
        case ACTOR_EN_VALI:
            return true;
        default:
            return IsExcludedFromClearRooms(enemyId, enemyParams);
    }
}

static bool IsClearRoom(bool mq, s16 sceneNum, s8 roomNum) {
    switch (sceneNum) {
        case SCENE_DEKU_TREE:
            if (mq) {
                return roomNum == 4 || roomNum == 6 || roomNum == 9 || roomNum == 10;
            } else {
                return roomNum == 1 || roomNum == 9;
            }
        case SCENE_DODONGOS_CAVERN:
            if (mq) {
                return roomNum == 5 || roomNum == 6 || roomNum == 13 || roomNum == 14;
            } else {
                return roomNum == 15;
            }
        case SCENE_JABU_JABU:
            if (mq) {
                return roomNum == 11 || roomNum == 13 || roomNum == 14;
            } else {
                return roomNum == 8 || roomNum == 9;
            }
        case SCENE_FOREST_TEMPLE:
            if (mq) {
                return roomNum == 5 || roomNum == 6 || roomNum == 18 || roomNum == 21;
            } else {
                return roomNum == 6 || roomNum == 10 || roomNum == 18 || roomNum == 21;
            }
        case SCENE_FIRE_TEMPLE:
            if (mq) {
                return roomNum == 15 || roomNum == 17 || roomNum == 18;
            } else {
                return roomNum == 15;
            }
        case SCENE_WATER_TEMPLE:
            if (mq) {
                return roomNum == 13 || roomNum == 18;
            } else {
                return roomNum == 13 || roomNum == 18 || roomNum == 19;
            }
        case SCENE_SPIRIT_TEMPLE:
            if (mq) {
                return roomNum == 1 || roomNum == 2 || roomNum == 4 || roomNum == 10 || roomNum == 15 ||
                       roomNum == 19 || roomNum == 20;
            } else {
                return roomNum == 1 || roomNum == 10 || roomNum == 17 || roomNum == 20 || roomNum == 27;
            }
        case SCENE_SHADOW_TEMPLE:
            if (mq) {
                return roomNum == 1 || roomNum == 6 || roomNum == 7 || roomNum == 11 || roomNum == 14 || roomNum == 20;
            } else {
                return roomNum == 1 || roomNum == 7 || roomNum == 11 || roomNum == 14 || roomNum == 16 ||
                       roomNum == 17 || roomNum == 19 || roomNum == 20;
            }
        case SCENE_INSIDE_GANONS_CASTLE:
            if (mq) {
                return roomNum == 0 || roomNum == 2 || roomNum == 5 || roomNum == 9;
            } else {
                return roomNum == 2 || roomNum == 5 || roomNum == 9;
            }
        case SCENE_ICE_CAVERN:
            if (mq) {
                return roomNum == 3 || roomNum == 7;
            } else {
                return roomNum == 1 || roomNum == 7;
            }
        case SCENE_GERUDO_TRAINING_GROUND:
            if (mq) {
                return roomNum == 10;
            } else {
                return roomNum == 3 || roomNum == 5 || roomNum == 10;
            }
        case SCENE_GANONS_TOWER:
            return true;
        case SCENE_GROTTOS:
            return roomNum == 2 || roomNum == 7;
        case SCENE_ROYAL_FAMILYS_TOMB:
            return roomNum == 0;
        default:
            return false;
    }
}

static bool IsTimedRoom(bool mq, s16 sceneNum, s8 roomNum) {
    switch (sceneNum) {
        case SCENE_JABU_JABU:
            return !mq && roomNum == 12;
        case SCENE_GERUDO_TRAINING_GROUND:
            if (mq) {
                return roomNum == 1 || roomNum == 3 || roomNum == 5 || roomNum == 7;
            } else {
                return roomNum == 1 || roomNum == 7;
            }
        case SCENE_GANONS_TOWER_COLLAPSE_INTERIOR:
            return roomNum == 1;
        default:
            return false;
    }
}

static bool IsEnemyAllowedToSpawn(s16 sceneNum, s8 roomNum, EnemyEntry enemy, s16 posY, bool fromBari) {
    bool mq = ResourceMgr_IsSceneMasterQuest(sceneNum);

    if (IsExcludedFromClearRooms(enemy.id, enemy.params) && IsClearRoom(mq, sceneNum, roomNum)) {
        return false;
    }

    if (IsExcludedFromTimedRooms(enemy.id, enemy.params) && IsTimedRoom(mq, sceneNum, roomNum)) {
        return false;
    }

    // Don't allow Lizalfos or Baris in Ganon's Tower because they would spawn up on the ceiling, becoming impossible to
    // kill.
    if (sceneNum == SCENE_GANONS_TOWER &&
        (enemy.id == ACTOR_EN_VALI || (enemy.id == ACTOR_EN_ZF && enemy.params == -1))) {
        return false;
    }

    // Don't allow Lizalfos in the first room of the interior of the castle collapse
    if (sceneNum == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR && roomNum == 1 && enemy.id == ACTOR_EN_ZF &&
        enemy.params == -1) {
        return false;
    }

    // Don't allow big Stalchildren, big Peahats and Baris (big jellyfish) during the Gohma fight because they can clip
    // into Gohma and it crashes the game. Likely because Gohma on the ceiling can't handle collision with other
    // enemies.
    if (sceneNum == SCENE_DEKU_TREE_BOSS &&
        ((enemy.id == ACTOR_EN_SKB && enemy.params == 20) || (enemy.id == ACTOR_EN_PEEHAT && enemy.params == -1) ||
         (enemy.id == ACTOR_EN_VALI))) {
        return false;
    }

    // Don't allow the following enemies in the first spawn of the first room in MQ Fire Temple loop as when spawned and
    // they get stuck in the room above
    // - Lizalfos/Dinolfos, Bari: they drop in
    // - Skulltulla: they appear above
    // - Flying Peehat: they rise above the ceiling
    if (mq && sceneNum == SCENE_FIRE_TEMPLE && roomNum == 15 && posY == 64 &&
        (enemy.id == ACTOR_EN_ZF || enemy.id == ACTOR_EN_VALI || enemy.id == ACTOR_EN_ST ||
         enemy.id == ACTOR_EN_PEEHAT)) {
        return false;
    }

    // Don't allow Stalfos in the child spirit clear room as they jump out of bounds frequently
    if (sceneNum == SCENE_SPIRIT_TEMPLE && roomNum == 1 && enemy.id == ACTOR_EN_TEST) {
        return false;
    }

    // Don't allow baris to spawn another bari
    if (fromBari && enemy.id == ACTOR_EN_VALI) {
        return false;
    }

    return true;
}

static std::vector<EnemyEntry> selectedEnemyList;

static void UpdateSelectedEnemies() {
    selectedEnemyList.clear();

    for (int i = 0; i < ARRAY_COUNT(randomizedEnemySpawnTable); i++) {
        if (CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemyList.All"), 0)) {
            selectedEnemyList.push_back(randomizedEnemySpawnTable[i]);
        } else if (CVarGetInteger(randomizedEnemySpawnTable[i].cvar, 1)) {
            selectedEnemyList.push_back(randomizedEnemySpawnTable[i]);
        }
    }

    if (selectedEnemyList.size() == 0) {
        selectedEnemyList.push_back(randomizedEnemySpawnTable[0]);
    }
}

static EnemyEntry GetRandomizedEnemyEntry(u32 seed, PlayState* play, s16 posY, bool fromBari) {
    std::vector<EnemyEntry> filteredEnemyList = {};

    if (selectedEnemyList.size() == 0) {
        UpdateSelectedEnemies();
    }

    for (EnemyEntry enemy : selectedEnemyList) {
        if (IsEnemyAllowedToSpawn(play->sceneNum, play->roomCtx.curRoom.num, enemy, posY, fromBari)) {
            filteredEnemyList.push_back(enemy);
        }
    }

    if (filteredEnemyList.size() == 0) {
        filteredEnemyList = selectedEnemyList;
    }

    if (CVAR_ENEMY_RANDOMIZER_VALUE == ENEMY_RANDOMIZER_RANDOM_SEEDED) {
        uint64_t randomState = 0;

        ShipUtils::RandInit(
            seed + (IS_RANDO ? Rando::Context::GetInstance()->GetSeed() : gSaveContext.ship.stats.fileCreatedAt),
            &randomState);

        return ShipUtils::RandomElement(filteredEnemyList, false, &randomState);
    }

    return ShipUtils::RandomElement(filteredEnemyList, false);
}

static bool IsEnemyFoundToRandomize(s16 sceneNum, s8 roomNum, s16 actorId, s16 params, f32 posX) {
    u32 isMQ = ResourceMgr_IsSceneMasterQuest(sceneNum);

    for (int i = 0; i < ARRAY_COUNT(enemiesToRandomize); i++) {
        if (actorId != enemiesToRandomize[i]) {
            continue;
        }

        switch (actorId) {
            // Only randomize the main component of Electric Tailparasans, not the tail segments they spawn.
            case ACTOR_EN_TP:
                return params == -1;
            // Only randomize the initial Deku Scrub actor (single and triple attack), not the flower they spawn.
            case ACTOR_EN_DEKUNUTS:
                return params == -256 || params == 768;
            // Don't randomize the OoB wallmaster in the Silver Rupee room because it's only there to
            // not trigger unlocking the door after killing the other wallmaster in authentic gameplay.
            case ACTOR_EN_WALLMAS:
                return !(!isMQ && sceneNum == SCENE_GERUDO_TRAINING_GROUND && roomNum == 2 && posX == -2345);
            // Only randomize initial Floormaster actor (it can split and does some spawning on init).
            case ACTOR_EN_FLOORMAS:
                return params == 0 || params == -32768;
            // Only randomize the initial eggs, not the enemies that spawn from them.
            case ACTOR_EN_GOMA:
                return params >= 0 && params <= 9;
            // Only randomize Skullwalltulas, not Golden Skulltulas.
            case ACTOR_EN_SW:
                return params == 0;
            // Don't randomize Nabooru because it'll break the cutscene and the door.
            // Don't randomize Iron Knuckle in MQ Spirit Trial because it's needed to
            // break the thrones in the room to access a button.
            case ACTOR_EN_IK:
                return params != 1280 && !(isMQ && sceneNum == SCENE_INSIDE_GANONS_CASTLE && roomNum == 17);
            // Only randomize the initial spawn of the huge jellyfish. It spawns another copy when hit with a sword.
            case ACTOR_EN_VALI:
                return params == -1;
            // Don't randomize Lizalfos in Dodongo's Cavern because the gates won't work correctly otherwise.
            case ACTOR_EN_ZF:
                return params != 1280 && params != 1281 && params != 1536 && params != 1537;
            // Don't randomize the right baby dodongo on the first tunnel in Dodongo's Cavern as in vanilla you use them
            // isntead of bombs to blow up a wall
            case ACTOR_EN_DODOJR:
                return !(sceneNum == SCENE_DODONGOS_CAVERN && roomNum == 1 && posX == 1972);
            // Don't randomize the Wolfos in SFM because it's needed to open the gate.
            case ACTOR_EN_WF:
                return params != 7936;
            // Don't randomize the Stalfos in Forest Temple because other enemies fall through the hole and don't
            // trigger the platform. Don't randomize the Stalfos spawning on the boat in Shadow Temple, as
            // randomizing them places the new enemies down in the river.
            case ACTOR_EN_TEST:
                return params != 1 && !(sceneNum == SCENE_SHADOW_TEMPLE && roomNum == 21);
            // Only randomize the enemy variant of Armos Statue.
            // Leave one Armos unrandomized in the Spirit Temple room where an armos is needed to push down a
            // button.
            case ACTOR_EN_AM:
                return (params == -1 || params == 255) && !(sceneNum == SCENE_SPIRIT_TEMPLE && posX == 2141);
            // Don't randomize Shell Blades and Spikes in the underwater portion in Water Temple as it's impossible
            // to kill most other enemies underwater with just hookshot and they're required to be killed for a
            // grate to open.
            case ACTOR_EN_SB:
            case ACTOR_EN_NY:
                return !(!isMQ && sceneNum == SCENE_WATER_TEMPLE && roomNum == 2);
            // Don't randomize Skull Kids in Lost Woods as child as they're not enemies
            case ACTOR_EN_SKJ:
                return !(sceneNum == SCENE_LOST_WOODS && LINK_IS_CHILD);
            default:
                return true;
        }
    }

    // If no enemy is found, don't randomize the actor.
    return false;
}

static u8 GetRandomizedEnemy(PlayState* play, s16* actorId, s16* posX, s16* posY, s16* posZ, s16* rotX, s16* rotY,
                             s16* rotZ, s16* params, s16 offset = 0, bool fromBari = false) {
    u32 isMQ = ResourceMgr_IsSceneMasterQuest(play->sceneNum);

    // Hack to remove enemies that wrongfully spawn because of bypassing object dependency with enemy randomizer on.
    // This should probably be handled on OTR generation in the future when object dependency is fully removed.
    // Remove bats and Skulltulas from graveyard.
    // Remove Octorok in Lost Woods.
    // Remove signs in Gerudo Fortress as child
    if (((*actorId == ACTOR_EN_FIREFLY || (*actorId == ACTOR_EN_SW && *params == 0)) &&
         play->sceneNum == SCENE_GRAVEYARD) ||
        (*actorId == ACTOR_EN_OKUTA && play->sceneNum == SCENE_LOST_WOODS) ||
        (*actorId == ACTOR_EN_KANBAN && play->sceneNum == SCENE_GERUDOS_FORTRESS && LINK_IS_CHILD)) {
        return 0;
    }

    // Hack to change a pot in Spirit Temple that holds a Deku Shield to not hold anything.
    // This should probably be handled on OTR generation in the future when object dependency is fully removed.
    // This Deku Shield doesn't normally spawn in authentic gameplay because of object dependency.
    if (*actorId == ACTOR_OBJ_TSUBO && *params == 24597) {
        *params = 24067;
    }

    // Lengthen timer in non-MQ Jabu Jabu bubble room.
    if (!isMQ && *actorId == ACTOR_OBJ_ROOMTIMER && *params == 30760 && play->sceneNum == SCENE_JABU_JABU &&
        play->roomCtx.curRoom.num == 12) {
        *params = (*params & ~0x3FF) | 120;
    }

    if (IsEnemyFoundToRandomize(play->sceneNum, play->roomCtx.curRoom.num, *actorId, *params, *posX)) {
        // When replacing Iron Knuckles in Spirit Temple, move them away from the throne because
        // some enemies can get stuck on the throne.
        if (*actorId == ACTOR_EN_IK && play->sceneNum == SCENE_SPIRIT_TEMPLE) {
            if (*params == 6657) {
                *posX = *posX + 150;
            } else if (*params == 6401) {
                *posX = *posX - 150;
            }
        }

        // Move like-likes in MQ Jabu Jabu down into the room as they otherwise get stuck on Song of Time blocks.
        if (*actorId == ACTOR_EN_RR && play->sceneNum == SCENE_JABU_JABU && play->roomCtx.curRoom.num == 11) {
            if (*posX == 1003) {
                *posX = *posX - 75;
            } else {
                *posX = *posX + 75;
            }
            *posY = *posY - 200;
        }

        // Do a raycast from the original position of the actor to find the ground below it, then try to place
        // the new actor on the ground. This way enemies don't spawn very high in the sky, and gives us control
        // over height offsets per enemy from a proven grounded position.
        CollisionPoly poly;
        Vec3f pos;
        f32 raycastResult;

        pos.x = *posX;
        pos.y = *posY + 50;
        pos.z = *posZ;

        // the forest temple second twisted hallway spawns after the enemies so we need to "find the floor" manually
        if (play->sceneNum == SCENE_FOREST_TEMPLE && play->roomCtx.curRoom.num == 20 && *posZ > -3000) {
            // when hallway is twisted (play->actorCtx.flags.tempSwch & 1), one spawn has the floor at 1235.165 &
            // the other at 1239.094 but that changes based on the player position
            // when not twisted, the whole floor is at 1228

            *posY = 1228.0;
        } else {
            raycastResult = BgCheck_AnyRaycastFloor1(&play->colCtx, &poly, &pos);

            // If ground is found below actor, move actor to that height.
            if (raycastResult > BGCHECK_Y_MIN) {
                *posY = raycastResult;
            }
        }

        // Get randomized enemy ID and parameter.
        u32 seed =
            play->sceneNum + *actorId + (int)*posX + (int)*posY + (int)*posZ + *rotX + *rotY + *rotZ + *params + offset;
        EnemyEntry randomEnemy = GetRandomizedEnemyEntry(seed, play, *posY, fromBari);

        *actorId = randomEnemy.id;
        *params = randomEnemy.params;

        // Straighten out enemies so they aren't flipped on their sides when the original spawn is.
        *rotX = 0;

        switch (*actorId) {
            // When spawning big jellyfish, spawn it up high.
            case ACTOR_EN_VALI:
                *posY = *posY + 300;
                break;
            // Spawn Peahat off the ground, otherwise it kills itself by colliding with the ground.
            case ACTOR_EN_PEEHAT:
                if (*params == 1) {
                    *posY = *posY + 100;
                }
                break;
            // Spawn Skulltulas off the ground.
            case ACTOR_EN_ST:
                *posY = *posY + 200;
                break;
            // Spawn flying enemies off the ground.
            case ACTOR_EN_FIREFLY:
            case ACTOR_EN_BILI:
            case ACTOR_EN_BB:
            case ACTOR_EN_CLEAR_TAG:
            case ACTOR_EN_CROW:
                *posY = *posY + 75;
                break;
            default:
                break;
        }
    }

    // Enemy finished randomization process.
    return 1;
}

void FixClubMoblinScale(void* ptr) {
    Actor* actor = (Actor*)ptr;
    if (actor->params == -1) {
        Actor_SetScale(actor, 0.014f);
    }
}

static void OnGerudoFighterDefeat(void* refActor) {
    EnGeldB* enGeldB = reinterpret_cast<EnGeldB*>(refActor);

    if (enGeldB->keyFlag == 0) {
        Item_DropCollectibleRandom(gPlayState, &enGeldB->actor, &enGeldB->actor.world.pos, 0xC0);
    }
}

struct CustomStalfosPairFightData {
    BgMoriBigst* moriBigst = nullptr;
    ActorFunc originalDestroy = nullptr;
};

static ObjectExtension::Register<CustomStalfosPairFightData> CustomStalfosPairFightDataRegister;

void CustomStalfosPairFightDestroy(Actor* thisx, PlayState* play) {
    assert(ObjectExtension::GetInstance().Has<CustomStalfosPairFightData>(thisx));

    CustomStalfosPairFightData* customStalfosPairFightData =
        ObjectExtension::GetInstance().Get<CustomStalfosPairFightData>(thisx);

    customStalfosPairFightData->moriBigst->dyna.actor.home.rot.z -= 1;

    customStalfosPairFightData->originalDestroy(thisx, play);

    ObjectExtension::GetInstance().Remove<CustomStalfosPairFightData>(thisx);
}

struct CustomPeehatLarvaData {
    EnPeehat* peehat = nullptr;
    ActorFunc originalDestroy = nullptr;
};

static ObjectExtension::Register<CustomPeehatLarvaData> CustomPeehatLarvaDataRegister;

void CustomPeehatLarvaDestroy(Actor* thisx, PlayState* play) {
    assert(ObjectExtension::GetInstance().Has<CustomPeehatLarvaData>(thisx));

    CustomPeehatLarvaData* customPeehatLarvaData = ObjectExtension::GetInstance().Get<CustomPeehatLarvaData>(thisx);

    customPeehatLarvaData->peehat->unk_2FA -= 1;

    customPeehatLarvaData->originalDestroy(thisx, play);

    ObjectExtension::GetInstance().Remove<CustomPeehatLarvaData>(thisx);
}

void RegisterEnemyRandomizer() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_MB, ENEMY_RANDOMIZER_ENABLED, FixClubMoblinScale);

    // prevent dark link from triggering a voidout
    COND_VB_SHOULD(VB_TRIGGER_VOIDOUT, ENEMY_RANDOMIZER_ENABLED, {
        Actor* actor = va_arg(args, Actor*);

        if (actor->category != ACTORCAT_PLAYER) {
            *should = false;
            Actor_Kill(actor);
        }
    });

    // prevent dark link dealing fall damage to the player
    COND_VB_SHOULD(VB_RECIEVE_FALL_DAMAGE, ENEMY_RANDOMIZER_ENABLED, {
        Actor* actor = va_arg(args, Actor*);

        if (actor->category != ACTORCAT_PLAYER) {
            *should = false;
        }
    });

    // prevent dark link from interfering with HESS/recoil/etc when at more than 100 away from him
    COND_VB_SHOULD(VB_TORCH2_HANDLE_CLANKING, ENEMY_RANDOMIZER_ENABLED, {
        Actor* darkLink = va_arg(args, Actor*);

        if (darkLink->xzDistToPlayer > 100.0f) {
            *should = false;
        }
    });

    // prevent dark link from interfering with ice floors
    COND_VB_SHOULD(VB_SET_STATIC_PREV_FLOOR_TYPE, ENEMY_RANDOMIZER_ENABLED, {
        Player* playerOrDarkLink = va_arg(args, Player*);

        if (playerOrDarkLink->actor.id != ACTOR_PLAYER) {
            *should = false;
        }
    });

    // prevent dark link from interfering with ice floors
    COND_VB_SHOULD(VB_SET_STATIC_FLOOR_TYPE, ENEMY_RANDOMIZER_ENABLED, {
        Player* playerOrDarkLink = va_arg(args, Player*);

        if (playerOrDarkLink->actor.id != ACTOR_PLAYER) {
            *should = false;
        }
    });

    // prevent dark link from being grabbed by like likes and therefore grabbing the player
    COND_VB_SHOULD(VB_LIKE_LIKE_GRAB_PLAYER, ENEMY_RANDOMIZER_ENABLED, {
        EnRr* likeLike = va_arg(args, EnRr*);

        if (!(likeLike->collider1.base.oc != NULL && likeLike->collider1.base.oc->category == ACTORCAT_PLAYER) &&
            !(likeLike->collider2.base.oc != NULL && likeLike->collider2.base.oc->category == ACTORCAT_PLAYER)) {
            *should = false;
        }
    });

    // Allow Random Gerudo Fighters (contain no keys) to spawn without any switch flags
    COND_VB_SHOULD(VB_GERUDO_FIGHTER_CONTINUE_WAITING, ENEMY_RANDOMIZER_ENABLED, {
        EnGeldB* enGeldB = va_arg(args, EnGeldB*);

        if (enGeldB->keyFlag == 0) {
            if (!enGeldB->invisible || enGeldB->actor.xzDistToPlayer <= 300.0f) {
                *should = false;
            }
        }
    });

    // Don't play Miniboss music for Random Gerudo Fighters
    COND_VB_SHOULD(VB_GERUDO_FIGHTER_PLAY_MINIBOSS_MUSIC, ENEMY_RANDOMIZER_ENABLED, {
        EnGeldB* enGeldB = va_arg(args, EnGeldB*);

        if (enGeldB->keyFlag == 0) {
            *should = false;
        }
    });

    // If Random Gerudo Fighters knock Link down, void him out like Wallmasters
    COND_VB_SHOULD(VB_GERUDO_FIGHTER_THROW_LINK_TO_JAIL, ENEMY_RANDOMIZER_ENABLED, {
        EnGeldB* enGeldB = va_arg(args, EnGeldB*);

        if (enGeldB->keyFlag == 0) {
            *should = false;
            Sfx_PlaySfxCentered(NA_SE_OC_ABYSS);
            Play_TriggerRespawn(gPlayState);
        }
    });

    // If Random Gerudo Fighters are defeated, drop some items
    COND_ID_HOOK(OnEnemyDefeat, ACTOR_EN_GELDB, ENEMY_RANDOMIZER_ENABLED, OnGerudoFighterDefeat);

    COND_VB_SHOULD(VB_SPAWN_ACTOR_ENTRY, ENEMY_RANDOMIZER_ENABLED, {
        ActorContext* actorCtx = va_arg(args, ActorContext*);
        ActorEntry* actorEntry = va_arg(args, ActorEntry*);
        PlayState* play = va_arg(args, PlayState*);
        Actor** actor = va_arg(args, Actor**);

        s16 actorId = actorEntry->id;
        s16 posX = actorEntry->pos.x;
        s16 posY = actorEntry->pos.y;
        s16 posZ = actorEntry->pos.z;
        s16 rotX = actorEntry->rot.x;
        s16 rotY = actorEntry->rot.y;
        s16 rotZ = actorEntry->rot.z;
        s16 params = actorEntry->params;

        *should = false;
        if (GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            *actor = Actor_Spawn(actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);
        }
    });

    COND_VB_SHOULD(VB_ADULT_ZELDA_SPAWN_STALFOS_IN_COLLAPSE, ENEMY_RANDOMIZER_ENABLED, {
        PlayState* play = va_arg(args, PlayState*);
        Vec3f* playerPos = va_arg(args, Vec3f*);
        double posX = va_arg(args, double);
        double posY = va_arg(args, double);
        double posZ = va_arg(args, double);

        s16 actorId = ACTOR_EN_TEST;
        s16 posX2 = posX;
        s16 posY2 = posY;
        s16 posZ2 = posZ;
        s16 rotX = 0;
        s16 rotY = Math_FAtan2F(playerPos->x - posX, playerPos->z - posZ) * (0x8000 / M_PI);
        s16 rotZ = 0;
        s16 params = 5;

        if (!GetRandomizedEnemy(play, &actorId, &posX2, &posY2, &posZ2, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        Actor_Spawn(&play->actorCtx, play, actorId, posX2, posY2, posZ2, rotX, rotY, rotZ, params);

        *should = false;
    });

    COND_VB_SHOULD(VB_BLKOBJ_SPAWN_DARK_LINK, ENEMY_RANDOMIZER_ENABLED, {
        if (!*should) {
            return;
        }

        EnBlkobj* blkobj = va_arg(args, EnBlkobj*);
        PlayState* play = va_arg(args, PlayState*);

        s16 actorId = ACTOR_EN_TORCH2;
        s16 posX = blkobj->dyna.actor.world.pos.x;
        s16 posY = blkobj->dyna.actor.world.pos.y;
        s16 posZ = blkobj->dyna.actor.world.pos.z;
        s16 rotX = 0;
        s16 rotY = blkobj->dyna.actor.yawTowardsPlayer;
        s16 rotZ = 0;
        s16 params = 0;

        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

        EnBlkobj_SetupAction(blkobj, EnBlkobj_DarkLinkFight);

        *should = false;
    });

    COND_VB_SHOULD(VB_HAKA_TUBO_SPAWN_KEESE, ENEMY_RANDOMIZER_ENABLED, {
        BgHakaTubo* hakaTubo = va_arg(args, BgHakaTubo*);
        PlayState* play = va_arg(args, PlayState*);

        s16 actorId = ACTOR_EN_FIREFLY;
        s16 posX = hakaTubo->dyna.actor.world.pos.x;
        s16 posY = hakaTubo->dyna.actor.world.pos.y + 80.0f;
        s16 posZ = hakaTubo->dyna.actor.world.pos.z;
        s16 rotX = 0;
        s16 rotY = hakaTubo->dyna.actor.shape.rot.y;
        s16 rotZ = 0;
        s16 params = 2;

        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

        *should = false;
    });

    COND_VB_SHOULD(VB_HAKA_SPAWN_POE, ENEMY_RANDOMIZER_ENABLED, {
        if (!*should) {
            return;
        }

        BgHaka* haka = va_arg(args, BgHaka*);
        PlayState* play = va_arg(args, PlayState*);

        s16 actorId = ACTOR_EN_POH;
        s16 posX = haka->dyna.actor.world.pos.x;
        s16 posY = haka->dyna.actor.world.pos.y;
        s16 posZ = haka->dyna.actor.world.pos.z;
        s16 rotX = 0;
        s16 rotY = haka->dyna.actor.shape.rot.y;
        s16 rotZ = 0;
        s16 params = 1;

        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

        *should = false;
    });

    COND_VB_SHOULD(VB_BIRI_SPAWN_JELLYFISH_UPON_DEATH, ENEMY_RANDOMIZER_ENABLED, {
        EnVali* vali = va_arg(args, EnVali*);
        PlayState* play = va_arg(args, PlayState*);

        s16 actorId = ACTOR_EN_BILI;
        s16 posX = vali->actor.world.pos.x;
        s16 posY = vali->actor.world.pos.y;
        s16 posZ = vali->actor.world.pos.z;
        s16 rotX = 0;
        s16 rotY = vali->actor.world.rot.y;
        s16 rotZ = 0;
        s16 params = 0;

        s16 homePosX = vali->actor.home.pos.x;
        s16 homePosY = vali->actor.home.pos.y;
        s16 homePosZ = vali->actor.home.pos.z;

        s16 homeRotX = vali->actor.home.rot.x;
        s16 homeRotY = vali->actor.home.rot.y;
        s16 homeRotZ = vali->actor.home.rot.z;

        for (s32 i = 0; i < 3; i++) {
            // use the home pos & rot to make it consistent
            if (!GetRandomizedEnemy(play, &actorId, &homePosX, &homePosY, &homePosZ, &homeRotX, &homeRotY, &homeRotZ,
                                    &params, i * 1000, true)) {
                assert(false);
            }

            Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

            rotY += 0x10000 / 3;
        }

        *should = false;
    });

    COND_VB_SHOULD(VB_ENCOUNT1_SPAWN_STALCHILD_OR_WOLFOS, ENEMY_RANDOMIZER_ENABLED, {
        EnEncount1* encount1 = va_arg(args, EnEncount1*);
        PlayState* play = va_arg(args, PlayState*);

        // have to use int instead of s16 in the va_arg call due to integer promotion
        s16 actorId = va_arg(args, int);
        Vec3f spawnPos = va_arg(args, Vec3f);
        s16 posX = spawnPos.x;
        s16 posY = spawnPos.y;
        s16 posZ = spawnPos.z;
        s16 rotX = 0;
        s16 rotY = 0;
        s16 rotZ = 0;
        // have to use int instead of s16 in the va_arg call due to integer promotion
        s16 params = va_arg(args, int);

        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        if (Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params)) {
            encount1->curNumSpawn++;
            if (encount1->curNumSpawn >= encount1->maxCurSpawns) {
                encount1->fieldSpawnTimer = 100;
            }
            if (play->sceneNum != SCENE_HYRULE_FIELD) {
                encount1->totalNumSpawn++;
            }
        }

        *should = false;
    });

    COND_VB_SHOULD(VB_MORI_BIGST_SUMMON_STALFOS_PAIR, ENEMY_RANDOMIZER_ENABLED, {
        BgMoriBigst* moriBigst = va_arg(args, BgMoriBigst*);
        PlayState* play = va_arg(args, PlayState*);

        s16 actorId = ACTOR_EN_TEST;
        s16 posX = 70.0f;
        s16 posY = 827.0f;
        s16 posZ = -3383.0f;
        s16 rotX = 0;
        s16 rotY = 0;
        s16 rotZ = 0;
        s16 params = 5;

        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        Actor* enemy1 = Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

        actorId = ACTOR_EN_TEST;
        posX = 170.0f;
        posY = 827.0f;
        posZ = -3260.0f;
        rotX = 0;
        rotY = 0;
        rotZ = 0;
        params = 5;

        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        Actor* enemy2 = Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

        moriBigst->dyna.actor.home.rot.z = 2;

        ObjectExtension::GetInstance().Set<CustomStalfosPairFightData>(
            enemy1, CustomStalfosPairFightData{ .moriBigst = moriBigst, .originalDestroy = enemy1->destroy });
        ObjectExtension::GetInstance().Set<CustomStalfosPairFightData>(
            enemy2, CustomStalfosPairFightData{ .moriBigst = moriBigst, .originalDestroy = enemy2->destroy });

        enemy1->destroy = CustomStalfosPairFightDestroy;
        enemy2->destroy = CustomStalfosPairFightDestroy;

        *should = false;
    });

    COND_VB_SHOULD(VB_HAKA_HUTA_SPAWN_KEESE, ENEMY_RANDOMIZER_ENABLED, {
        BgHakaHuta* hakaHuta = va_arg(args, BgHakaHuta*);
        PlayState* play = va_arg(args, PlayState*);

        s16 actorId = ACTOR_EN_FIREFLY;
        s16 posX = hakaHuta->dyna.actor.world.pos.x + (-25.0f) * Math_CosS(hakaHuta->dyna.actor.shape.rot.y) +
                   40.0f * Math_SinS(hakaHuta->dyna.actor.shape.rot.y);
        s16 posY = hakaHuta->dyna.actor.world.pos.y - 10.0f;
        s16 posZ = hakaHuta->dyna.actor.world.pos.z - (-25.0f) * Math_SinS(hakaHuta->dyna.actor.shape.rot.y) +
                   40.0f * Math_CosS(hakaHuta->dyna.actor.shape.rot.y);
        s16 rotX = 0;
        s16 rotY = hakaHuta->dyna.actor.shape.rot.y + 0x8000;
        s16 rotZ = 0;
        s16 params = 2;

        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

        actorId = ACTOR_EN_FIREFLY;
        posX = hakaHuta->dyna.actor.world.pos.x + (-25.0f) * Math_CosS(hakaHuta->dyna.actor.shape.rot.y) +
               80.0f * Math_SinS(hakaHuta->dyna.actor.shape.rot.y);
        posY = hakaHuta->dyna.actor.world.pos.y - 10.0f;
        posZ = hakaHuta->dyna.actor.world.pos.z - (-25.0f) * Math_SinS(hakaHuta->dyna.actor.shape.rot.y) +
               80.0f * Math_CosS(hakaHuta->dyna.actor.shape.rot.y);
        rotX = 0;
        rotY = hakaHuta->dyna.actor.shape.rot.y;
        rotZ = 0;
        params = 2;

        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

        *should = false;
    });

    COND_VB_SHOULD(VB_HAKA_HUTA_SPAWN_REDEAD, ENEMY_RANDOMIZER_ENABLED, {
        BgHakaHuta* hakaHuta = va_arg(args, BgHakaHuta*);
        PlayState* play = va_arg(args, PlayState*);

        s16 actorId = ACTOR_EN_RD;
        s16 posX = hakaHuta->dyna.actor.home.pos.x + (-25.0f) * Math_CosS(hakaHuta->dyna.actor.shape.rot.y) +
                   100.0f * Math_SinS(hakaHuta->dyna.actor.shape.rot.y);
        s16 posY = hakaHuta->dyna.actor.home.pos.y - 40.0f;
        s16 posZ = hakaHuta->dyna.actor.home.pos.z - (-25.0f) * Math_SinS(hakaHuta->dyna.actor.shape.rot.y) +
                   100.0f * Math_CosS(hakaHuta->dyna.actor.shape.rot.y);
        s16 rotX = 0;
        s16 rotY = hakaHuta->dyna.actor.shape.rot.y;
        s16 rotZ = 0;
        s16 params = 0xFD;

        if (!GetRandomizedEnemy(play, &actorId, &posX, &posY, &posZ, &rotX, &rotY, &rotZ, &params)) {
            assert(false);
        }

        Actor_Spawn(&play->actorCtx, play, actorId, posX, posY, posZ, rotX, rotY, rotZ, params);

        *should = false;
    });

    COND_VB_SHOULD(VB_PEEHAT_SPAWN_LARVAS, ENEMY_RANDOMIZER_ENABLED, {
        EnPeehat* peehat = va_arg(args, EnPeehat*);
        PlayState* play = va_arg(args, PlayState*);

        s16 actorId = ACTOR_EN_PEEHAT;
        s16 homePosX = peehat->actor.home.pos.x;
        s16 homePosY = peehat->actor.home.pos.y + 50.0f;
        s16 homePosZ = peehat->actor.home.pos.z;
        s16 rotX = 0;
        s16 rotY = 0;
        s16 rotZ = 0;
        s16 params = PEAHAT_TYPE_LARVA;

        // 3 is MAX_LARVA
        for (s32 i = 3 - peehat->unk_2FA; i > 0; i--) {
            if (!GetRandomizedEnemy(play, &actorId, &homePosX, &homePosY, &homePosZ, &rotX, &rotY, &rotZ, &params,
                                    i * 1000)) {
                assert(false);
            }

            Actor* enemy =
                Actor_Spawn(&play->actorCtx, play, actorId, homePosX, homePosY, homePosZ, rotX, rotY, rotZ, params);

            if (enemy == NULL) {
                assert(false);
            } else {
                peehat->unk_2FA++;
                ObjectExtension::GetInstance().Set<CustomPeehatLarvaData>(
                    enemy, CustomPeehatLarvaData{ .peehat = peehat, .originalDestroy = enemy->destroy });
                enemy->destroy = CustomPeehatLarvaDestroy;
            }
        }
    });
}

static const std::map<s32, const char*> enemyRandomizerModes = {
    { ENEMY_RANDOMIZER_OFF, "Disabled" },
    { ENEMY_RANDOMIZER_RANDOM, "Random" },
    { ENEMY_RANDOMIZER_RANDOM_SEEDED, "Random (Seeded)" },
};

void RegisterEnemyRandomizerWidgets() {
    WidgetPath path = { "Enhancements", "Extra Modes", SECTION_COLUMN_2 };

    SohGui::mSohMenu->AddWidget(path, "Enemy Randomizer", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("RandomizedEnemies"))
        .Callback([](WidgetInfo& info) { UpdateSelectedEnemies(); })
        .Options(
            UIWidgets::ComboboxOptions()
                .DefaultIndex(ENEMY_RANDOMIZER_OFF)
                .ComboMap(enemyRandomizerModes)
                .Tooltip("Replaces fixed enemies throughout the game with a random enemy. Bosses, Mini-Bosses and a "
                         "few specific regular enemies are excluded.\n"
                         "Enemies that need more than Deku Nuts & either Deku Sticks or a sword to kill are excluded "
                         "from spawning in \"clear enemy\" rooms.\n\n"
                         "- Random: Enemies are randomized every time you load a room.\n"
                         "- Random (Seeded): Enemies are randomized based on the current randomizer seed/file.\n"));

    SohGui::mSohMenu->AddWidget(path, "Randomized Enemy Sizes", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RandomizedEnemySizes"))
        .Options(UIWidgets::CheckboxOptions().Tooltip("Enemies and Bosses spawn with random sizes."));

    SohGui::mSohMenu->AddWidget(path, "Scale Health with Size", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("EnemySizeScalesHealth"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemySizes"), 0);
        })
        .Options(UIWidgets::CheckboxOptions().Tooltip("Scales normal enemies Health with their randomized size.\n"
                                                      "*This will NOT affect Bosses!*"));

    SohGui::mSohMenu->AddWidget(path, "Enemy List", WIDGET_SEPARATOR_TEXT).PreFunc([](WidgetInfo& info) {
        info.isHidden = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), 0);
    });

    SohGui::mSohMenu->AddWidget(path, "Select all Enemies", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RandomizedEnemyList.All"))
        .PreFunc([](WidgetInfo& info) { info.isHidden = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), 0); })
        .Callback([](WidgetInfo& info) { UpdateSelectedEnemies(); });

    SohGui::mSohMenu->AddWidget(path, "Enemy List", WIDGET_SEPARATOR).PreFunc([](WidgetInfo& info) {
        info.isHidden = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), 0);
    });

    for (int i = 0; i < ARRAY_COUNT(randomizedEnemySpawnTable); i++) {
        SohGui::mSohMenu->AddWidget(path, randomizedEnemySpawnTable[i].name, WIDGET_CVAR_CHECKBOX)
            .CVar(randomizedEnemySpawnTable[i].cvar)
            .Options(UIWidgets::CheckboxOptions().DefaultValue(true))
            .PreFunc([](WidgetInfo& info) {
                info.isHidden = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), 0);
                info.options->disabled = CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemyList.All"), 0);
                info.options->disabledTooltip = "These options are disabled because \"Select All Enemies\" is enabled.";
            })
            .Callback([](WidgetInfo& info) { UpdateSelectedEnemies(); });
    }
}

static RegisterShipInitFunc initFunc(RegisterEnemyRandomizer, { CVAR_ENEMY_RANDOMIZER_NAME });
static RegisterMenuInitFunc menuInitFunc(RegisterEnemyRandomizerWidgets);
