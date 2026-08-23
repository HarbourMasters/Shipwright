#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Notification/Notification.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
GetItemEntry ItemTable_RetrieveEntry(s16 modIndex, s16 getItemID);
}

#define CVAR_EXTRA_TRAPS_NAME CVAR_ENHANCEMENT("ExtraTraps.Enabled")
#define CVAR_EXTRA_TRAPS_DEFAULT 0
#define CVAR_EXTRA_TRAPS_VALUE CVarGetInteger(CVAR_EXTRA_TRAPS_NAME, CVAR_EXTRA_TRAPS_DEFAULT)

typedef enum {
    ADD_ICE_TRAP,
    ADD_BURN_TRAP,
    ADD_SHOCK_TRAP,
    ADD_KNOCK_TRAP,
    ADD_SPEED_TRAP,
    ADD_BOMB_TRAP,
    ADD_VOID_TRAP,
    ADD_AMMO_TRAP,
    ADD_KILL_TRAP,
    ADD_TELEPORT_TRAP,
    ADD_TRAP_MAX
} AltTrapType;

static AltTrapType roll = ADD_TRAP_MAX;
static int statusTimer = -1;
static int eventTimer = -1;
static EntranceIndex teleportRoll = ENTR_MAX;

const char* altTrapTypeCvars[] = {
    CVAR_ENHANCEMENT("ExtraTraps.Ice"),   CVAR_ENHANCEMENT("ExtraTraps.Burn"),
    CVAR_ENHANCEMENT("ExtraTraps.Shock"), CVAR_ENHANCEMENT("ExtraTraps.Knockback"),
    CVAR_ENHANCEMENT("ExtraTraps.Speed"), CVAR_ENHANCEMENT("ExtraTraps.Bomb"),
    CVAR_ENHANCEMENT("ExtraTraps.Void"),  CVAR_ENHANCEMENT("ExtraTraps.Ammo"),
    CVAR_ENHANCEMENT("ExtraTraps.Kill"),  CVAR_ENHANCEMENT("ExtraTraps.Teleport"),
};

const std::vector<EntranceIndex> simpleTeleportDestinations = {
    ENTR_LINKS_HOUSE_CHILD_SPAWN, ENTR_SACRED_FOREST_MEADOW_WARP_PAD, ENTR_DEATH_MOUNTAIN_CRATER_WARP_PAD,
    ENTR_LAKE_HYLIA_WARP_PAD,     ENTR_DESERT_COLOSSUS_WARP_PAD,      ENTR_GRAVEYARD_WARP_PAD,
    ENTR_TEMPLE_OF_TIME_WARP_PAD,
};

// Mirrors the entrances available in rando
const std::vector<EntranceIndex> advancedTeleportDestinations = {
    ENTR_DEKU_TREE_ENTRANCE,
    ENTR_KOKIRI_FOREST_OUTSIDE_DEKU_TREE,
    ENTR_DODONGOS_CAVERN_ENTRANCE,
    ENTR_DEATH_MOUNTAIN_TRAIL_OUTSIDE_DODONGOS_CAVERN,
    ENTR_JABU_JABU_ENTRANCE,
    ENTR_ZORAS_FOUNTAIN_OUTSIDE_JABU_JABU,
    ENTR_FOREST_TEMPLE_ENTRANCE,
    ENTR_SACRED_FOREST_MEADOW_OUTSIDE_TEMPLE,
    ENTR_FIRE_TEMPLE_ENTRANCE,
    ENTR_DEATH_MOUNTAIN_CRATER_OUTSIDE_TEMPLE,
    ENTR_WATER_TEMPLE_ENTRANCE,
    ENTR_LAKE_HYLIA_OUTSIDE_TEMPLE,
    ENTR_SPIRIT_TEMPLE_ENTRANCE,
    ENTR_DESERT_COLOSSUS_OUTSIDE_TEMPLE,
    ENTR_SHADOW_TEMPLE_ENTRANCE,
    ENTR_GRAVEYARD_OUTSIDE_TEMPLE,
    ENTR_BOTTOM_OF_THE_WELL_ENTRANCE,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_BOTTOM_OF_THE_WELL,
    ENTR_ICE_CAVERN_ENTRANCE,
    ENTR_ZORAS_FOUNTAIN_OUTSIDE_ICE_CAVERN,
    ENTR_GERUDO_TRAINING_GROUND_ENTRANCE,
    ENTR_GERUDOS_FORTRESS_OUTSIDE_GERUDO_TRAINING_GROUND,
    ENTR_INSIDE_GANONS_CASTLE_ENTRANCE,
    ENTR_CASTLE_GROUNDS_RAINBOW_BRIDGE_EXIT,
    ENTR_MIDOS_HOUSE_0,
    ENTR_KOKIRI_FOREST_OUTSIDE_MIDOS_HOUSE,
    ENTR_SARIAS_HOUSE_0,
    ENTR_KOKIRI_FOREST_OUTSIDE_SARIAS_HOUSE,
    ENTR_TWINS_HOUSE_0,
    ENTR_KOKIRI_FOREST_OUTSIDE_TWINS_HOUSE,
    ENTR_KNOW_IT_ALL_BROS_HOUSE_0,
    ENTR_KOKIRI_FOREST_OUTSIDE_KNOW_IT_ALL_HOUSE,
    ENTR_KOKIRI_SHOP_0,
    ENTR_KOKIRI_FOREST_OUTSIDE_SHOP,
    ENTR_LAKESIDE_LABORATORY_0,
    ENTR_LAKE_HYLIA_OUTSIDE_LAB,
    ENTR_FISHING_POND_0,
    ENTR_LAKE_HYLIA_OUTSIDE_FISHING_POND,
    ENTR_CARPENTERS_TENT_0,
    ENTR_GERUDO_VALLEY_OUTSIDE_TENT,
    ENTR_MARKET_GUARD_HOUSE_0,
    ENTR_MARKET_ENTRANCE_OUTSIDE_GUARD_HOUSE,
    ENTR_HAPPY_MASK_SHOP_0,
    ENTR_MARKET_DAY_OUTSIDE_HAPPY_MASK_SHOP,
    ENTR_BOMBCHU_BOWLING_ALLEY_0,
    ENTR_MARKET_DAY_OUTSIDE_BOMBCHU_BOWLING,
    ENTR_POTION_SHOP_MARKET_0,
    ENTR_MARKET_DAY_OUTSIDE_POTION_SHOP,
    ENTR_TREASURE_BOX_SHOP_0,
    ENTR_MARKET_DAY_OUTSIDE_TREASURE_BOX_SHOP,
    ENTR_BOMBCHU_SHOP_1,
    ENTR_BACK_ALLEY_DAY_OUTSIDE_BOMBCHU_SHOP,
    ENTR_BACK_ALLEY_MAN_IN_GREEN_HOUSE,
    ENTR_BACK_ALLEY_DAY_OUTSIDE_MAN_IN_GREEN_HOUSE,
    ENTR_KAKARIKO_CENTER_GUEST_HOUSE_0,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_CENTER_GUEST_HOUSE,
    ENTR_HOUSE_OF_SKULLTULA_0,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_SKULKLTULA_HOUSE,
    ENTR_IMPAS_HOUSE_FRONT,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_FRONT,
    ENTR_IMPAS_HOUSE_BACK,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_BACK,
    ENTR_POTION_SHOP_GRANNY_0,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOP_GRANNY,
    ENTR_GRAVEKEEPERS_HUT_0,
    ENTR_GRAVEYARD_OUTSIDE_DAMPES_HUT,
    ENTR_GORON_SHOP_0,
    ENTR_GORON_CITY_OUTSIDE_SHOP,
    ENTR_ZORA_SHOP_0,
    ENTR_ZORAS_DOMAIN_OUTSIDE_SHOP,
    ENTR_LON_LON_BUILDINGS_TALONS_HOUSE,
    ENTR_LON_LON_RANCH_OUTSIDE_TALONS_HOUSE,
    ENTR_STABLE_0,
    ENTR_LON_LON_RANCH_OUTSIDE_STABLES,
    ENTR_LON_LON_BUILDINGS_TOWER,
    ENTR_LON_LON_RANCH_OUTSIDE_TOWER,
    ENTR_BAZAAR_1,
    ENTR_MARKET_DAY_OUTSIDE_BAZAAR,
    ENTR_SHOOTING_GALLERY_1,
    ENTR_MARKET_DAY_OUTSIDE_SHOOTING_GALLERY,
    ENTR_BAZAAR_0,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_BAZAAR,
    ENTR_SHOOTING_GALLERY_0,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOOTING_GALLERY,
    ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_NAYRUS_COLOSSUS,
    ENTR_DESERT_COLOSSUS_GREAT_FAIRY_EXIT,
    ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_DINS_HC,
    ENTR_CASTLE_GROUNDS_GREAT_FAIRY_EXIT,
    ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_OGC_DD,
    ENTR_POTION_SHOP_KAKARIKO_1,
    ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMC,
    ENTR_DEATH_MOUNTAIN_CRATER_GREAT_FAIRY_EXIT,
    ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMT,
    ENTR_DEATH_MOUNTAIN_TRAIL_GREAT_FAIRY_EXIT,
    ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_FARORES_ZF,
    ENTR_ZORAS_FOUNTAIN_OUTSIDE_GREAT_FAIRY,
    ENTR_LINKS_HOUSE_1,
    ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE,
    ENTR_TEMPLE_OF_TIME_ENTRANCE,
    ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_OUTSIDE_TEMPLE,
    ENTR_WINDMILL_AND_DAMPES_GRAVE_WINDMILL,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_WINDMILL,
    ENTR_POTION_SHOP_KAKARIKO_FRONT,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_FRONT,
    ENTR_POTION_SHOP_KAKARIKO_BACK,
    ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_BACK,
    ENTR_GRAVE_WITH_FAIRYS_FOUNTAIN_0,
    ENTR_GRAVEYARD_SHIELD_GRAVE_EXIT,
    ENTR_REDEAD_GRAVE_0,
    ENTR_GRAVEYARD_HEART_PIECE_GRAVE_EXIT,
    ENTR_ROYAL_FAMILYS_TOMB_0,
    ENTR_GRAVEYARD_ROYAL_TOMB_EXIT,
    ENTR_WINDMILL_AND_DAMPES_GRAVE_GRAVE,
    ENTR_GRAVEYARD_DAMPES_GRAVE_EXIT,
    ENTR_LOST_WOODS_BRIDGE_EAST_EXIT,
    ENTR_KOKIRI_FOREST_LOWER_EXIT,
    ENTR_LOST_WOODS_SOUTH_EXIT,
    ENTR_KOKIRI_FOREST_UPPER_EXIT,
    ENTR_GORON_CITY_TUNNEL_SHORTCUT,
    ENTR_LOST_WOODS_TUNNEL_SHORTCUT,
    ENTR_ZORAS_RIVER_UNDERWATER_SHORTCUT,
    ENTR_LOST_WOODS_UNDERWATER_SHORTCUT,
    ENTR_SACRED_FOREST_MEADOW_SOUTH_EXIT,
    ENTR_LOST_WOODS_NORTH_EXIT,
    ENTR_HYRULE_FIELD_WOODED_EXIT,
    ENTR_LOST_WOODS_BRIDGE_WEST_EXIT,
    ENTR_LAKE_HYLIA_NORTH_EXIT,
    ENTR_HYRULE_FIELD_FENCE_EXIT,
    ENTR_GERUDO_VALLEY_EAST_EXIT,
    ENTR_HYRULE_FIELD_ROCKY_PATH,
    ENTR_MARKET_ENTRANCE_NEAR_GUARD_EXIT,
    ENTR_HYRULE_FIELD_ON_BRIDGE_SPAWN,
    ENTR_KAKARIKO_VILLAGE_FRONT_GATE,
    ENTR_HYRULE_FIELD_STAIRS_EXIT,
    ENTR_ZORAS_RIVER_WEST_EXIT,
    ENTR_HYRULE_FIELD_RIVER_EXIT,
    ENTR_LON_LON_RANCH_ENTRANCE,
    ENTR_HYRULE_FIELD_CENTER_EXIT,
    ENTR_ZORAS_DOMAIN_UNDERWATER_SHORTCUT,
    ENTR_LAKE_HYLIA_UNDERWATER_SHORTCUT,
    ENTR_GERUDOS_FORTRESS_EAST_EXIT,
    ENTR_GERUDO_VALLEY_WEST_EXIT,
    ENTR_HAUNTED_WASTELAND_EAST_EXIT,
    ENTR_GERUDOS_FORTRESS_GATE_EXIT,
    ENTR_DESERT_COLOSSUS_EAST_EXIT,
    ENTR_HAUNTED_WASTELAND_WEST_EXIT,
    ENTR_MARKET_SOUTH_EXIT,
    ENTR_MARKET_ENTRANCE_NORTH_EXIT,
    ENTR_CASTLE_GROUNDS_SOUTH_EXIT,
    ENTR_MARKET_DAY_CASTLE_EXIT,
    ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_GOSSIP_STONE_EXIT,
    ENTR_MARKET_DAY_TEMPLE_EXIT,
    ENTR_GRAVEYARD_ENTRANCE,
    ENTR_KAKARIKO_VILLAGE_SOUTHEAST_EXIT,
    ENTR_DEATH_MOUNTAIN_TRAIL_BOTTOM_EXIT,
    ENTR_KAKARIKO_VILLAGE_GUARD_GATE,
    ENTR_GORON_CITY_UPPER_EXIT,
    ENTR_DEATH_MOUNTAIN_TRAIL_GC_EXIT,
    ENTR_DEATH_MOUNTAIN_CRATER_GC_EXIT,
    ENTR_GORON_CITY_DARUNIA_ROOM_EXIT,
    ENTR_DEATH_MOUNTAIN_CRATER_UPPER_EXIT,
    ENTR_DEATH_MOUNTAIN_TRAIL_SUMMIT_EXIT,
    ENTR_ZORAS_DOMAIN_ENTRANCE,
    ENTR_ZORAS_RIVER_WATERFALL_EXIT,
    ENTR_ZORAS_FOUNTAIN_TUNNEL_EXIT,
    ENTR_ZORAS_DOMAIN_KING_ZORA_EXIT,
    ENTR_LAKE_HYLIA_RIVER_EXIT,
    ENTR_HYRULE_FIELD_OWL_DROP,
    ENTR_KAKARIKO_VILLAGE_OWL_DROP,
    ENTR_LINKS_HOUSE_CHILD_SPAWN,
    ENTR_HYRULE_FIELD_10,
    ENTR_SACRED_FOREST_MEADOW_WARP_PAD,
    ENTR_DEATH_MOUNTAIN_CRATER_WARP_PAD,
    ENTR_LAKE_HYLIA_WARP_PAD,
    ENTR_DESERT_COLOSSUS_WARP_PAD,
    ENTR_GRAVEYARD_WARP_PAD,
    ENTR_TEMPLE_OF_TIME_WARP_PAD,
    ENTR_DEKU_TREE_BOSS_ENTRANCE,
    ENTR_DEKU_TREE_BOSS_DOOR,
    ENTR_DODONGOS_CAVERN_BOSS_ENTRANCE,
    ENTR_DODONGOS_CAVERN_BOSS_DOOR,
    ENTR_JABU_JABU_BOSS_ENTRANCE,
    ENTR_JABU_JABU_BOSS_DOOR,
    ENTR_FOREST_TEMPLE_BOSS_ENTRANCE,
    ENTR_FOREST_TEMPLE_BOSS_DOOR,
    ENTR_FIRE_TEMPLE_BOSS_ENTRANCE,
    ENTR_FIRE_TEMPLE_BOSS_DOOR,
    ENTR_WATER_TEMPLE_BOSS_ENTRANCE,
    ENTR_WATER_TEMPLE_BOSS_DOOR,
    ENTR_SPIRIT_TEMPLE_BOSS_ENTRANCE,
    ENTR_SPIRIT_TEMPLE_BOSS_DOOR,
    ENTR_SHADOW_TEMPLE_BOSS_ENTRANCE,
    ENTR_SHADOW_TEMPLE_BOSS_DOOR,
};

std::vector<AltTrapType> getEnabledAddTraps() {
    std::vector<AltTrapType> enabledAddTraps;
    for (int i = 0; i < ADD_TRAP_MAX; i++) {
        if (CVarGetInteger(altTrapTypeCvars[i], 0)) {
            if (gSaveContext.equips.buttonItems[0] == ITEM_FISHING_POLE &&
                (i == ADD_VOID_TRAP || i == ADD_TELEPORT_TRAP)) {
                continue; // don't add void or teleport if you're holding the fishing pole, as this causes issues
            }
            enabledAddTraps.push_back(static_cast<AltTrapType>(i));
        }
    }
    if (enabledAddTraps.size() == 0) {
        enabledAddTraps.push_back(ADD_ICE_TRAP);
    }
    return enabledAddTraps;
};

static void RollRandomTrap(uint64_t seed) {
    uint64_t finalSeed = seed + (IS_RANDO ? static_cast<uint64_t>(Rando::Context::GetInstance()->GetSeed())
                                          : gSaveContext.ship.stats.fileCreatedAt);
    uint64_t state;
    ShipUtils::RandInit(finalSeed, &state);

    roll = ShipUtils::RandomElement(getEnabledAddTraps(), &state);
    switch (roll) {
        case ADD_ICE_TRAP:
            GameInteractor::RawAction::FreezePlayer();
            break;
        case ADD_BURN_TRAP:
            GameInteractor::RawAction::BurnPlayer();
            break;
        case ADD_SHOCK_TRAP:
            GameInteractor::RawAction::ElectrocutePlayer();
            break;
        case ADD_KNOCK_TRAP:
            eventTimer = 3;
            break;
        case ADD_SPEED_TRAP:
            Audio_PlaySfxGeneral(NA_SE_VO_KZ_MOVE, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            GameInteractor::State::MovementSpeedMultiplier = 0.5f;
            statusTimer = 200;
            Notification::Emit({ .message = "Speed Decreased!" });
            break;
        case ADD_BOMB_TRAP:
            eventTimer = 3;
            break;
        case ADD_VOID_TRAP:
            Audio_PlaySfxGeneral(NA_SE_EN_GANON_LAUGH, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            eventTimer = 3;
            break;
        case ADD_AMMO_TRAP:
            eventTimer = 3;
            Notification::Emit({ .message = "Ammo Halved!" });
            break;
        case ADD_KILL_TRAP:
            GameInteractor::RawAction::SetPlayerHealth(0);
            break;
        case ADD_TELEPORT_TRAP:
            eventTimer = 3;
            teleportRoll =
                CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Teleport"), TELEPORT_TRAP_OFF) == TELEPORT_TRAP_ADVANCED
                    ? ShipUtils::RandomElement(advancedTeleportDestinations, &state)
                    : ShipUtils::RandomElement(simpleTeleportDestinations, &state);
            break;
        default:
            break;
    }
}

static void OnPlayerUpdate() {
    Player* player = GET_PLAYER(gPlayState);
    if (statusTimer == 0) {
        GameInteractor::State::MovementSpeedMultiplier = 1.0f;
    }
    if (eventTimer == 0) {
        switch (roll) {
            case ADD_KNOCK_TRAP:
                GameInteractor::RawAction::KnockbackPlayer(1);
                break;
            case ADD_BOMB_TRAP:
                GameInteractor::RawAction::SpawnActor(ACTOR_EN_BOM, 1);
                break;
            case ADD_VOID_TRAP:
                Play_TriggerRespawn(gPlayState);
                break;
            case ADD_AMMO_TRAP:
                AMMO(ITEM_STICK) = static_cast<int8_t>(floor(AMMO(ITEM_STICK) * 0.5f));
                AMMO(ITEM_NUT) = static_cast<int8_t>(floor(AMMO(ITEM_NUT) * 0.5f));
                AMMO(ITEM_SLINGSHOT) = static_cast<int8_t>(floor(AMMO(ITEM_SLINGSHOT) * 0.5f));
                AMMO(ITEM_BOW) = static_cast<int8_t>(floor(AMMO(ITEM_BOW) * 0.5f));
                AMMO(ITEM_BOMB) = static_cast<int8_t>(floor(AMMO(ITEM_BOMB) * 0.5f));
                AMMO(ITEM_BOMBCHU) = static_cast<int8_t>(floor(AMMO(ITEM_BOMBCHU) * 0.5f));
                Audio_PlaySfxGeneral(NA_SE_VO_FR_SMILE_0, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                break;
            case ADD_TELEPORT_TRAP: {
                GameInteractor::RawAction::TeleportPlayer(teleportRoll);
                break;
            }
            default:
                break;
        }
    }
    if (statusTimer >= 0) {
        statusTimer--;
    }
    if (eventTimer >= 0) {
        eventTimer--;
    }
}

void RegisterExtraTraps() {
    COND_HOOK(OnPlayerUpdate, CVAR_EXTRA_TRAPS_VALUE, OnPlayerUpdate);

    COND_VB_SHOULD(VB_SHORT_CIRCUIT_GIVE_ITEM_PROCESS, true, {
        if (!gSaveContext.ship.pendingIceTrapCount) {
            return;
        }

        Player* player = GET_PLAYER(gPlayState);

        *should = true;
        gSaveContext.ship.pendingIceTrapCount--;
        gSaveContext.ship.stats.count[COUNT_ICE_TRAPS]++;
        GameInteractor_ExecuteOnItemReceiveHooks(ItemTable_RetrieveEntry(MOD_RANDOMIZER, RG_ICE_TRAP));
        if (CVAR_EXTRA_TRAPS_VALUE) {
            RollRandomTrap(gPlayState->sceneNum + player->getItemEntry.drawItemId);
        } else {
            GameInteractor::RawAction::FreezePlayer();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterExtraTraps, { CVAR_EXTRA_TRAPS_NAME });
