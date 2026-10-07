#pragma once

#include <array>
#include <cstdint>
#include <optional>

extern "C" {
#include "z64.h"
}

namespace HeartPieceViewer {
enum class HeartFlagType { Collectible, Chest, ItemGet, Event, Info, Fishing };
struct HeartFlag {
    const char* name;
    HeartFlagType type;
    uint16_t scene;
    uint16_t flag;
};

// Vanilla reward flags, including NPCs and minigames. Ice Cavern uses the same
// collectible bit in original and Master Quest. Boss containers are not pieces.
inline constexpr std::array<HeartFlag, 36> heartFlags = { {
    { "Lost Woods - Skull Kid", HeartFlagType::ItemGet, SCENE_LOST_WOODS, 22 },
    { "Lost Woods - Ocarina Memory Game", HeartFlagType::ItemGet, SCENE_LOST_WOODS, 23 },
    { "Hyrule Field - Tektite Grotto Freestanding PoH", HeartFlagType::Collectible, SCENE_GROTTOS, 0x01 },
    { "Hyrule Field - Deku Scrub Grotto", HeartFlagType::ItemGet, SCENE_GROTTOS, ITEMGETINF_DEKU_SCRUB_HEART_PIECE },
    { "Lake Hylia - Child Fishing", HeartFlagType::Fishing, SCENE_FISHING_POND, HS_FISH_PRIZE_CHILD },
    { "Lake Hylia - Lab Dive", HeartFlagType::ItemGet, SCENE_LAKESIDE_LABORATORY, 16 },
    { "Lake Hylia - Freestanding PoH", HeartFlagType::Collectible, SCENE_LAKE_HYLIA, 0x1E },
    { "Gerudo Valley - Waterfall Freestanding PoH", HeartFlagType::Collectible, SCENE_GERUDO_VALLEY, 0x01 },
    { "Gerudo Valley - Crate Freestanding PoH", HeartFlagType::Collectible, SCENE_GERUDO_VALLEY, 0x02 },
    { "Gerudo Fortress - Chest", HeartFlagType::Chest, SCENE_GERUDOS_FORTRESS, 0x00 },
    { "Gerudo Fortress - HBA 1000 Points", HeartFlagType::Info, SCENE_GERUDOS_FORTRESS, INFTABLE_190 },
    { "Desert Colossus - Freestanding PoH", HeartFlagType::Collectible, SCENE_DESERT_COLOSSUS, 0x0D },
    { "Market - Treasure Chest Game Reward", HeartFlagType::ItemGet, SCENE_TREASURE_BOX_SHOP, 27 },
    { "Market - Bombchu Bowling Second Prize", HeartFlagType::ItemGet, SCENE_BOMBCHU_BOWLING_ALLEY, 18 },
    { "Market - Lost Dog", HeartFlagType::Info, SCENE_DOG_LADY_HOUSE, INFTABLE_191 },
    { "Kakariko Village - 50 Gold Skulltula Reward", HeartFlagType::Event, SCENE_HOUSE_OF_SKULLTULA,
      ((EVENTCHKINF_SKULLTULA_REWARD_INDEX << 4) | EVENTCHKINF_SKULLTULA_REWARD_50_SHIFT) },
    { "Kakariko Village - Man on Roof", HeartFlagType::ItemGet, SCENE_KAKARIKO_VILLAGE, 21 },
    { "Kakariko Village - Impas House Freestanding PoH", HeartFlagType::Collectible, SCENE_IMPAS_HOUSE, 0x01 },
    { "Kakariko Village - Windmill Freestanding PoH", HeartFlagType::Collectible, SCENE_WINDMILL_AND_DAMPES_GRAVE,
      0x01 },
    { "Graveyard - Heart Piece Grave Chest", HeartFlagType::Chest, SCENE_REDEAD_GRAVE, 0x00 },
    { "Graveyard - Freestanding PoH", HeartFlagType::Collectible, SCENE_GRAVEYARD, 0x04 },
    { "Graveyard - Dampe Race Freestanding PoH", HeartFlagType::Collectible, SCENE_WINDMILL_AND_DAMPES_GRAVE, 0x07 },
    { "Graveyard - Dampe Gravedigging Tour", HeartFlagType::Collectible, SCENE_GRAVEYARD, 0x19 },
    { "Death Mountain Trail - Freestanding PoH", HeartFlagType::Collectible, SCENE_DEATH_MOUNTAIN_TRAIL, 0x1E },
    { "Goron City - Pot Freestanding PoH", HeartFlagType::Collectible, SCENE_GORON_CITY, 0x1F },
    { "Death Mountain Crater - Wall Freestanding PoH", HeartFlagType::Collectible, SCENE_DEATH_MOUNTAIN_CRATER, 0x02 },
    { "Death Mountain Crater - Volcano Freestanding PoH", HeartFlagType::Collectible, SCENE_DEATH_MOUNTAIN_CRATER,
      0x08 },
    { "Zora's River - Frogs in the Rain", HeartFlagType::Event, SCENE_ZORAS_RIVER, EVENTCHKINF_SONGS_FOR_FROGS_STORMS },
    { "Zora's River - Frogs Ocarina Game", HeartFlagType::Event, SCENE_ZORAS_RIVER, EVENTCHKINF_SONGS_FOR_FROGS_CHOIR },
    { "Zora's River - Near Open Grotto Freestanding PoH", HeartFlagType::Collectible, SCENE_ZORAS_RIVER, 0x04 },
    { "Zora's River - Near Domain Freestanding PoH", HeartFlagType::Collectible, SCENE_ZORAS_RIVER, 0x0B },
    { "Zora's Domain - Chest", HeartFlagType::Chest, SCENE_ZORAS_DOMAIN, 0x00 },
    { "Zora's Fountain - Iceberg Freestanding PoH", HeartFlagType::Collectible, SCENE_ZORAS_FOUNTAIN, 0x01 },
    { "Zora's Fountain - Bottom Freestanding PoH", HeartFlagType::Collectible, SCENE_ZORAS_FOUNTAIN, 0x14 },
    { "Lon Lon Ranch - Freestanding PoH", HeartFlagType::Collectible, SCENE_LON_LON_BUILDINGS, 0x01 },
    { "Ice Cavern - Freestanding PoH", HeartFlagType::Collectible, SCENE_ICE_CAVERN, 0x01 },
} };

// Boss drops all use collectible 0x1F in their respective boss room.
// Keep them separate from the 36-piece total.
inline constexpr std::array<HeartFlag, 8> bossHeartFlags = { {
    { "Queen Gohma Heart Container", HeartFlagType::Collectible, SCENE_DEKU_TREE_BOSS, 0x1F },
    { "King Dodongo Heart Container", HeartFlagType::Collectible, SCENE_DODONGOS_CAVERN_BOSS, 0x1F },
    { "Barinade Heart Container", HeartFlagType::Collectible, SCENE_JABU_JABU_BOSS, 0x1F },
    { "Phantom Ganon Heart Container", HeartFlagType::Collectible, SCENE_FOREST_TEMPLE_BOSS, 0x1F },
    { "Volvagia Heart Container", HeartFlagType::Collectible, SCENE_FIRE_TEMPLE_BOSS, 0x1F },
    { "Morpha Heart Container", HeartFlagType::Collectible, SCENE_WATER_TEMPLE_BOSS, 0x1F },
    { "Twinrova Heart Container", HeartFlagType::Collectible, SCENE_SPIRIT_TEMPLE_BOSS, 0x1F },
    { "Bongo Bongo Heart Container", HeartFlagType::Collectible, SCENE_SHADOW_TEMPLE_BOSS, 0x1F },
} };

// Read the active scene first: its newly acquired flags may not yet be copied
// to the save context. This never changes the save or relies on tracker history.
inline std::optional<bool> ReadFlag(const HeartFlag& entry, const SaveContext& save, const PlayState* play) {
    const bool current = play != nullptr && play->sceneNum == entry.scene;
    switch (entry.type) {
        case HeartFlagType::Collectible:
            if (entry.scene >= std::size(save.sceneFlags) || entry.flag >= 32)
                return std::nullopt;
            return ((current ? play->actorCtx.flags.collect : save.sceneFlags[entry.scene].collect) &
                    (uint32_t{ 1 } << entry.flag)) != 0;
        case HeartFlagType::Chest:
            if (entry.scene >= std::size(save.sceneFlags) || entry.flag >= 32)
                return std::nullopt;
            return ((current ? play->actorCtx.flags.chest : save.sceneFlags[entry.scene].chest) &
                    (uint32_t{ 1 } << entry.flag)) != 0;
        case HeartFlagType::ItemGet:
            if ((entry.flag >> 4) >= std::size(save.itemGetInf))
                return std::nullopt;
            return (save.itemGetInf[entry.flag >> 4] & (1u << (entry.flag & 15))) != 0;
        case HeartFlagType::Event:
            if ((entry.flag >> 4) >= std::size(save.eventChkInf))
                return std::nullopt;
            return (save.eventChkInf[entry.flag >> 4] & (1u << (entry.flag & 15))) != 0;
        case HeartFlagType::Info:
            if ((entry.flag >> 4) >= std::size(save.infTable))
                return std::nullopt;
            return (save.infTable[entry.flag >> 4] & (1u << (entry.flag & 15))) != 0;
        case HeartFlagType::Fishing:
            return (save.highScores[HS_FISHING] & entry.flag) != 0;
    }
    return std::nullopt;
}

inline const char* FlagName(HeartFlagType type) {
    switch (type) {
        case HeartFlagType::Collectible:
            return "collectible";
        case HeartFlagType::Chest:
            return "chest";
        case HeartFlagType::ItemGet:
            return "itemGetInf";
        case HeartFlagType::Event:
            return "eventChkInf";
        case HeartFlagType::Info:
            return "infTable";
        case HeartFlagType::Fishing:
            return "fishing prize mask";
    }
    return "unknown";
}
} // namespace HeartPieceViewer
