#include "z64.h"
#include <libultraship/libultraship.h>

int64_t OoTAP_APItemToGameItem(int64_t item) {
    switch (item - 66000) {
        case 0x01: return ITEM_BOMBS_5;
        case 0x02: return ITEM_NUTS_5;
        case 0x03: return ITEM_BOMBCHUS_5;
        case 0x06: return ITEM_BOOMERANG;
        case 0x07: return ITEM_STICK;
        case 0x0A: return ITEM_LENS;
        case 0x0B: return ITEM_LETTER_ZELDA;
        case 0x0D: return ITEM_HAMMER;
        case 0x0E: return ITEM_COJIRO;
        case 0x0F: return ITEM_BOTTLE;
        case 0x14: return ITEM_MILK_BOTTLE;
        case 0x15: return ITEM_LETTER_RUTO;
        case 0x16: return ITEM_BEAN;
        case 0x17: return ITEM_MASK_SKULL;
        case 0x18: return ITEM_MASK_SPOOKY;
        case 0x1A: return ITEM_MASK_KEATON;
        case 0x1B: return ITEM_MASK_BUNNY;
        case 0x1C: return ITEM_MASK_TRUTH;
        case 0x1D: return ITEM_POCKET_EGG;
        case 0x1E: return ITEM_POCKET_CUCCO;
        case 0x1F: return ITEM_ODD_MUSHROOM;
        case 0x20: return ITEM_ODD_POTION;
        case 0x21: return ITEM_SAW;
        case 0x22: return ITEM_SWORD_BROKEN;
        case 0x23: return ITEM_PRESCRIPTION;
        case 0x24: return ITEM_FROG;
        case 0x25: return ITEM_EYEDROPS;
        case 0x26: return ITEM_CLAIM_CHECK;
        case 0x27: return ITEM_SWORD_KOKIRI;
        case 0x28: return ITEM_SWORD_KNIFE;
        case 0x29: return ITEM_SHIELD_DEKU;
        case 0x2A: return ITEM_SHIELD_HYLIAN;
        case 0x2B: return ITEM_SHIELD_MIRROR;
        case 0x2C: return ITEM_TUNIC_GORON;
        case 0x2D: return ITEM_TUNIC_ZORA;
        case 0x2E: return ITEM_BOOTS_IRON;
        case 0x2F: return ITEM_BOOTS_HOVER;
        case 0x39: return ITEM_STONE_OF_AGONY;
        case 0x3A: return ITEM_GERUDO_CARD;
        case 0x3D: return ITEM_HEART_CONTAINER;
        case 0x3E: return ITEM_HEART_PIECE;
        case 0x3F: return ITEM_KEY_BOSS;
        case 0x40: return ITEM_COMPASS;
        case 0x41: return ITEM_DUNGEON_MAP;
        case 0x42: return ITEM_KEY_SMALL;
        case 0x47: return ITEM_WEIRD_EGG;
        case 0x48: return ITEM_HEART;
        case 0x49: return ITEM_ARROWS_SMALL;
        case 0x4A: return ITEM_ARROWS_MEDIUM;
        case 0x4B: return ITEM_ARROWS_LARGE;
        case 0x4C: return ITEM_RUPEE_GREEN;
        case 0x4D: return ITEM_RUPEE_BLUE;
        case 0x4E: return ITEM_RUPEE_RED;
        case 0x50: return ITEM_MILK;
        case 0x51: return ITEM_MASK_GORON;
        case 0x52: return ITEM_MASK_ZORA;
        case 0x53: return ITEM_MASK_GERUDO;
        case 0x55: return ITEM_RUPEE_PURPLE;
        case 0x56: return ITEM_RUPEE_GOLD;
        case 0x57: return ITEM_SWORD_BGS;
        case 0x58: return ITEM_ARROW_FIRE;
        case 0x59: return ITEM_ARROW_ICE;
        case 0x5A: return ITEM_ARROW_LIGHT;
        case 0x5B: return ITEM_SKULL_TOKEN;
        case 0x5C: return ITEM_DINS_FIRE;
        case 0x5E: return ITEM_NAYRUS_LOVE;
        case 0x5D: return ITEM_FARORES_WIND;
        case 0x64: return ITEM_NUTS_10;
        case 0x66: return ITEM_BOMBS_10;
        case 0x67: return ITEM_BOMBS_20;
        case 0x69: return ITEM_SEEDS_30;
        case 0x6A: return ITEM_BOMBCHUS_5;
        case 0x6B: return ITEM_BOMBCHUS_20;
        case 0x6C: return ITEM_KOKIRI_EMERALD;
        case 0x6D: return ITEM_GORON_RUBY;
        case 0x6E: return ITEM_ZORA_SAPPHIRE;
        case 0x72: return ITEM_RUPEE_BLUE;
        case 0x76: return ITEM_HEART_PIECE_2;
        case 0x7C: return ITEM_ICE_TRAP;
        case 0x80: return ITEM_HOOKSHOT;
        case 0x81: return ITEM_BRACELET;
        case 0x82: return ITEM_BOMB_BAG_20;
        case 0x83: return ITEM_BOW;
        case 0x84: return ITEM_SLINGSHOT;
        case 0x85: return ITEM_WALLET_ADULT;
        case 0x86: return ITEM_SCALE_SILVER;
        case 0x87: return ITEM_NUT_UPGRADE_30;
        case 0x88: return ITEM_STICK_UPGRADE_20;
        case 0x89: return ITEM_BOMBCHU;
        case 0x8A: return ITEM_SINGLE_MAGIC;
        case 0x8B: return ITEM_OCARINA_FAIRY;
        case 0x8C: return ITEM_POTION_RED;
        case 0x8D: return ITEM_POTION_GREEN;
        case 0x8E: return ITEM_POTION_BLUE;
        case 0x8F: return ITEM_FAIRY;
        case 0x90: return ITEM_FISH;
        case 0x91: return ITEM_BLUE_FIRE;
        case 0x92: return ITEM_BUG;
        case 0x93: return ITEM_BIG_POE;
        case 0x94: return ITEM_POE;
        case 0x95: return ITEM_KEY_BOSS_FOREST;
        case 0x96: return ITEM_KEY_BOSS_FIRE;
        case 0x97: return ITEM_KEY_BOSS_WATER;
        case 0x98: return ITEM_KEY_BOSS_SPIRIT;
        case 0x99: return ITEM_KEY_BOSS_SHADOW;
        case 0x9A: return ITEM_KEY_BOSS_GANON;
        case 0x9B: return ITEM_COMPASS_DEKU;
        case 0x9C: return ITEM_COMPASS_DODONGO;
        case 0x9D: return ITEM_COMPASS_JABU;
        case 0x9E: return ITEM_COMPASS_FOREST;
        case 0x9F: return ITEM_COMPASS_FIRE;
        case 0xA0: return ITEM_COMPASS_WATER;
        case 0xA1: return ITEM_COMPASS_SPIRIT;
        case 0xA2: return ITEM_COMPASS_SHADOW;
        case 0xA3: return ITEM_COMPASS_WELL;
        case 0xA4: return ITEM_COMPASS_ICE_CAVERN;
        case 0xA5: return ITEM_MAP_DEKU;
        case 0xA6: return ITEM_MAP_DODONGO;
        case 0xA7: return ITEM_MAP_JABU;
        case 0xA8: return ITEM_MAP_FOREST;
        case 0xA9: return ITEM_MAP_FIRE;
        case 0xAA: return ITEM_MAP_WATER;
        case 0xAB: return ITEM_MAP_SPIRIT;
        case 0xAC: return ITEM_MAP_SHADOW;
        case 0xAD: return ITEM_MAP_WELL;
        case 0xAE: return ITEM_MAP_ICE_CAVERN;
        case 0xAF: return ITEM_KEY_SMALL_FOREST;
        case 0xB0: return ITEM_KEY_SMALL_FIRE;
        case 0xB1: return ITEM_KEY_SMALL_WATER;
        case 0xB2: return ITEM_KEY_SMALL_SPIRIT;
        case 0xB3: return ITEM_KEY_SMALL_SHADOW;
        case 0xB4: return ITEM_KEY_SMALL_WELL;
        case 0xB5: return ITEM_KEY_SMALL_GTG;
        case 0xB6: return ITEM_KEY_SMALL_HIDEOUT;
        case 0xB7: return ITEM_KEY_SMALL_GANON;
        case 0xB8: return ITEM_DOUBLE_DEFENSE;
        case 0xBB: return ITEM_SONG_MINUET;
        case 0xBC: return ITEM_SONG_BOLERO;
        case 0xBD: return ITEM_SONG_SERENADE;
        case 0xBE: return ITEM_SONG_REQUIEM;
        case 0xBF: return ITEM_SONG_NOCTURNE;
        case 0xC0: return ITEM_SONG_PRELUDE;
        case 0xC1: return ITEM_SONG_LULLABY;
        case 0xC2: return ITEM_SONG_EPONA;
        case 0xC3: return ITEM_SONG_SARIA;
        case 0xC4: return ITEM_SONG_SUN;
        case 0xC5: return ITEM_SONG_TIME;
        case 0xC6: return ITEM_SONG_STORMS;
        case 0xC9: return ITEM_BEAN;
        case 0xCA: return ITEM_TRIFORCE_PIECE;
        case 0xCB: return ITEM_KEY_RING_FOREST;
        case 0xCC: return ITEM_KEY_RING_FIRE;
        case 0xCD: return ITEM_KEY_RING_WATER;
        case 0xCE: return ITEM_KEY_RING_SPIRIT;
        case 0xCF: return ITEM_KEY_RING_SHADOW;
        case 0xD0: return ITEM_KEY_RING_WELL;
        case 0xD1: return ITEM_KEY_RING_GTG;
        case 0xD2: return ITEM_KEY_RING_HIDEOUT;
        case 0xD3: return ITEM_KEY_RING_GANON;
    }

    return -1;
}

std::string OoTAP_GetItemName(int64_t item) {
    switch (item) {
        case ITEM_STICK: return "Deku Stick";
        case ITEM_NUT: return "Deku Nut";
        case ITEM_BOMB: return "Bomb";
        case ITEM_BOW: return "Fairy Bow";
        case ITEM_ARROW_FIRE: return "Fire Arrows";
        case ITEM_DINS_FIRE: return "Din's Fire";
        case ITEM_SLINGSHOT: return "Slingshot";
        case ITEM_OCARINA_FAIRY: return "Fairy Ocarina";
        case ITEM_OCARINA_TIME: return "Ocarina of Time";
        case ITEM_BOMBCHU: return "Bombchu";
        case ITEM_HOOKSHOT: return "Hookshot";
        case ITEM_LONGSHOT: return "Longshot";
        case ITEM_ARROW_ICE: return "Ice Arrows";
        case ITEM_FARORES_WIND: return "Farore's Wind";
        case ITEM_BOOMERANG: return "Boomerang";
        case ITEM_LENS: return "Lens of Truth";
        case ITEM_BEAN: return "Magic Bean";
        case ITEM_HAMMER: return "Megaton Hammer";
        case ITEM_ARROW_LIGHT: return "Light Arrows";
        case ITEM_NAYRUS_LOVE: return "Nayru's Love";
        case ITEM_BOTTLE: return "Empty Bottle";
        case ITEM_POTION_RED: return "Bottle with Red Potion";
        case ITEM_POTION_GREEN: return "Bottle with Green Potion";
        case ITEM_POTION_BLUE: return "Bottle with Blue Potion";
        case ITEM_FAIRY: return "Bottle with Fairy";
        case ITEM_FISH: return "Bottle with Fish";
        case ITEM_MILK_BOTTLE: return "Bottle with Milk";
        case ITEM_LETTER_RUTO: return "Ruto's Letter";
        case ITEM_BLUE_FIRE: return "Bottle with Blue Fire";
        case ITEM_BUG: return "Bottle with Bug";
        case ITEM_BIG_POE: return "Bottle with Big Poe";
        case ITEM_POE: return "Bottle with Poe";
        case ITEM_WEIRD_EGG: return "Weird Egg";
        case ITEM_CHICKEN: return "Chicken";
        case ITEM_LETTER_ZELDA: return "Zelda's Letter";
        case ITEM_MASK_KEATON: return "Keaton Mask";
        case ITEM_MASK_SKULL: return "Skull Mask";
        case ITEM_MASK_SPOOKY: return "Spooky Mask";
        case ITEM_MASK_BUNNY: return "Bunny Hood";
        case ITEM_MASK_GORON: return "Goron Mask";
        case ITEM_MASK_ZORA: return "Zora Mask";
        case ITEM_MASK_GERUDO: return "Gerudo Mask";
        case ITEM_MASK_TRUTH: return "Mask of Truth";
        case ITEM_POCKET_EGG: return "Pocket Egg";
        case ITEM_POCKET_CUCCO: return "Pocket Cucco";
        case ITEM_COJIRO: return "Cojiro";
        case ITEM_ODD_MUSHROOM: return "Odd Mushroom";
        case ITEM_ODD_POTION: return "Odd Potion";
        case ITEM_SAW: return "Poacher's Saw";
        case ITEM_SWORD_BROKEN: return "Broken Goron Sword";
        case ITEM_PRESCRIPTION: return "Prescription";
        case ITEM_FROG: return "Eyeball Frog";
        case ITEM_EYEDROPS: return "Eyedrops";
        case ITEM_CLAIM_CHECK: return "Claim Check";
        case ITEM_SWORD_KOKIRI: return "Kokiri Sword";
        case ITEM_SWORD_BGS: return "Biggoron Sword";
        case ITEM_SHIELD_DEKU: return "Deku Shield";
        case ITEM_SHIELD_HYLIAN: return "Hylian Shield";
        case ITEM_SHIELD_MIRROR: return "Mirror Shield";
        case ITEM_TUNIC_GORON: return "Goron Tunic";
        case ITEM_TUNIC_ZORA: return "Zora Tunic";
        case ITEM_BOOTS_IRON: return "Iron Boots";
        case ITEM_BOOTS_HOVER: return "Hover Boots";
        case ITEM_BOMB_BAG_20: return "Bomb Bag";
        case ITEM_BOMB_BAG_30: return "Bigger Bomb Bag";
        case ITEM_BOMB_BAG_40: return "Biggest Bomb Bag";
        case ITEM_BRACELET: return "Goron Bracelet";
        case ITEM_GAUNTLETS_SILVER: return "Silver Gauntlets";
        case ITEM_GAUNTLETS_GOLD: return "Gold Gauntlets";
        case ITEM_SCALE_SILVER: return "Silver Scale";
        case ITEM_SCALE_GOLDEN: return "Golden Scale";
        case ITEM_SWORD_KNIFE: return "Giant's Knife";
        case ITEM_WALLET_ADULT: return "Adult's Wallet";
        case ITEM_WALLET_GIANT: return "Giant's Wallet";
        case ITEM_SONG_MINUET: return "Minuet of Forest";
        case ITEM_SONG_BOLERO: return "Bolero of Fire";
        case ITEM_SONG_SERENADE: return "Serenade of Water";
        case ITEM_SONG_REQUIEM: return "Requiem of Spirit";
        case ITEM_SONG_NOCTURNE: return "Nocturne of Shadow";
        case ITEM_SONG_PRELUDE: return "Prelude of Light";
        case ITEM_SONG_LULLABY: return "Zelda's Lullaby";
        case ITEM_SONG_EPONA: return "Epona's Song";
        case ITEM_SONG_SARIA: return "Saria's Song";
        case ITEM_SONG_SUN: return "Sun's Song";
        case ITEM_SONG_TIME: return "Song of Time";
        case ITEM_SONG_STORMS: return "Song of Storms";
        case ITEM_MEDALLION_FOREST: return "Forest Medallion";
        case ITEM_MEDALLION_FIRE: return "Fire Medallion";
        case ITEM_MEDALLION_WATER: return "Water Medallion";
        case ITEM_MEDALLION_SPIRIT: return "Spirit Medallion";
        case ITEM_MEDALLION_SHADOW: return "Shadow Medallion";
        case ITEM_MEDALLION_LIGHT: return "Light Medallion";
        case ITEM_KOKIRI_EMERALD: return "Kokiri Emerald";
        case ITEM_GORON_RUBY: return "Goron Ruby";
        case ITEM_ZORA_SAPPHIRE: return "Zora Sapphire";
        case ITEM_STONE_OF_AGONY: return "Stone of Agony";
        case ITEM_GERUDO_CARD: return "Gerudo Membership Card";
        case ITEM_SKULL_TOKEN: return "Gold Skulltula Token";
        case ITEM_HEART_CONTAINER: return "Heart Container";
        case ITEM_HEART_PIECE_2:
        case ITEM_HEART_PIECE: return "Heart Piece";
        case ITEM_KEY_BOSS: return "Boss Key";
        case ITEM_COMPASS: return "Compass";
        case ITEM_DUNGEON_MAP: return "Map";
        case ITEM_KEY_SMALL: return "Small Key";
        case ITEM_SINGLE_MAGIC: return "Magic Meter";
        case ITEM_DOUBLE_MAGIC: return "Double Magic Meter";
        case ITEM_DOUBLE_DEFENSE: return "Double Defense";
        case ITEM_MILK: return "Milk";
        case ITEM_HEART: return "Recovery Heart";
        case ITEM_RUPEE_GREEN: return "Rupee (1)";
        case ITEM_RUPEE_BLUE: return "Rupee (5)";
        case ITEM_RUPEE_RED: return "Rupee (20)";
        case ITEM_RUPEE_PURPLE: return "Rupee (50)";
        case ITEM_RUPEE_GOLD: return "Rupee (200)";
        case ITEM_STICKS_5: return "Deku Sticks (5)";
        case ITEM_STICKS_10: return "Deku Sticks (10)";
        case ITEM_NUTS_5: return "Deku Nuts (5)";
        case ITEM_NUTS_10: return "Deku Nuts (10)";
        case ITEM_BOMBS_5: return "Bombs (5)";
        case ITEM_BOMBS_10: return "Bombs (10)";
        case ITEM_BOMBS_20: return "Bombs (20)";
        case ITEM_BOMBS_30: return "Bombs (30)";
        case ITEM_ARROWS_SMALL: return "Small Arrow Pack";
        case ITEM_ARROWS_MEDIUM: return "Medium Arrow Pack";
        case ITEM_ARROWS_LARGE: return "Large Arrow Pack";
        case ITEM_SEEDS_30: return "Deku Seeds (30)";
        case ITEM_BOMBCHUS_5: return "Bombchus (5)";
        case ITEM_BOMBCHUS_20: return "Bombchus (20)";
        case ITEM_STICK_UPGRADE_20:
        case ITEM_STICK_UPGRADE_30: return "Deku Stick Upgrade";
        case ITEM_NUT_UPGRADE_30:
        case ITEM_NUT_UPGRADE_40: return "Deku Nut Upgrade";
        case ITEM_KEY_BOSS_FOREST: return "Boss Key (Forest Temple)";
        case ITEM_KEY_BOSS_FIRE: return "Boss Key (Fire Temple)";
        case ITEM_KEY_BOSS_WATER: return "Boss Key (Water Temple)";
        case ITEM_KEY_BOSS_SPIRIT: return "Boss Key (Spirit Temple)";
        case ITEM_KEY_BOSS_SHADOW: return "Boss Key (Shadow Temple)";
        case ITEM_KEY_BOSS_GANON: return "Ganon's Boss Key";
        case ITEM_COMPASS_DEKU: return "Compass (Deku Tree)";
        case ITEM_COMPASS_DODONGO: return "Compass (Dodongo's Cavern)";
        case ITEM_COMPASS_JABU: return "Compass (Jabu Jabu's Belly)";
        case ITEM_COMPASS_FOREST: return "Compass (Forest Temple)";
        case ITEM_COMPASS_FIRE: return "Compass (Fire Temple)";
        case ITEM_COMPASS_WATER: return "Compass (Water Temple)";
        case ITEM_COMPASS_SPIRIT: return "Compass (Spirit Temple)";
        case ITEM_COMPASS_SHADOW: return "Compass (Shadow Temple)";
        case ITEM_COMPASS_WELL: return "Compass (Bottom of the Well)";
        case ITEM_COMPASS_ICE_CAVERN: return "Compass (Ice Cavern)";
        case ITEM_KEY_SMALL_FOREST: return "Small Key (Forest Temple)";
        case ITEM_KEY_SMALL_FIRE: return "Small Key (Fire Temple)";
        case ITEM_KEY_SMALL_WATER: return "Small Key (Water Temple)";
        case ITEM_KEY_SMALL_SPIRIT: return "Small Key (Spirit Temple)";
        case ITEM_KEY_SMALL_SHADOW: return "Small Key (Shadow Temple)";
        case ITEM_KEY_SMALL_WELL: return "Small Key (Bottom of the Well)";
        case ITEM_KEY_SMALL_GTG: return "Small Key (Gerudo Training Ground)";
        case ITEM_KEY_SMALL_HIDEOUT: return "Small Key (Theives' Hideout)";
        case ITEM_KEY_SMALL_GANON: return "Small Key (Ganon's Castle)";
        case ITEM_KEY_RING_FOREST: return "Small Key Ring (Forest Temple)";
        case ITEM_KEY_RING_FIRE: return "Small Key Ring (Fire Temple)";
        case ITEM_KEY_RING_WATER: return "Small Key Ring (Water Temple)";
        case ITEM_KEY_RING_SPIRIT: return "Small Key Ring (Spirit Temple)";
        case ITEM_KEY_RING_SHADOW: return "Small Key Ring (Shadow Temple)";
        case ITEM_KEY_RING_WELL: return "Small Key Ring (Bottom of the Well)";
        case ITEM_KEY_RING_GTG: return "Small Key Ring (Gerudo Training Ground)";
        case ITEM_KEY_RING_HIDEOUT: return "Small Key Ring (Theives' Hideout)";
        case ITEM_KEY_RING_GANON: return "Small Key Ring (Ganon's Castle)";
        case ITEM_MAP_DEKU: return "Map (Deku Tree)";
        case ITEM_MAP_DODONGO: return "Map (Dodongo's Cavern)";
        case ITEM_MAP_JABU: return "Map (Jabu Jabu's Belly)";
        case ITEM_MAP_FOREST: return "Map (Forest Temple)";
        case ITEM_MAP_FIRE: return "Map (Fire Temple)";
        case ITEM_MAP_WATER: return "Map (Water Temple)";
        case ITEM_MAP_SPIRIT: return "Map (Spirit Temple)";
        case ITEM_MAP_SHADOW: return "Map (Shadow Temple)";
        case ITEM_MAP_WELL: return "Map (Bottom of the Well)";
        case ITEM_MAP_ICE_CAVERN: return "Map (Ice Cavern)";
        case ITEM_TRIFORCE_PIECE: return "Triforce Piece";
    }

    return "Unknown Item (" + std::to_string(item) + ")";
}

std::map<RandomizerCheck, int64_t> APCheckLookup = {
    { RC_SONG_FROM_IMPA, 67010 },
    { RC_SONG_FROM_MALON, 67011 },
    { RC_SONG_FROM_SARIA, 67012 },
    { RC_SONG_FROM_ROYAL_FAMILYS_TOMB, 67013 },
    { RC_SONG_FROM_OCARINA_OF_TIME, 67014 },
    { RC_SONG_FROM_WINDMILL, 67015 },

    { RC_SHEIK_IN_FOREST, 67016 },
    { RC_SHEIK_IN_CRATER, 67017 },
    { RC_SHEIK_IN_ICE_CAVERN, 67018 },
    { RC_SHEIK_AT_COLOSSUS, 67019 },
    { RC_SHEIK_IN_KAKARIKO, 67020 },
    { RC_SHEIK_AT_TEMPLE, 67021 },

    { RC_KF_MIDOS_TOP_LEFT_CHEST, 67022 },
    { RC_KF_MIDOS_TOP_RIGHT_CHEST, 67023 },
    { RC_KF_MIDOS_BOTTOM_LEFT_CHEST, 67024 },
    { RC_KF_MIDOS_BOTTOM_RIGHT_CHEST, 67025 },
    { RC_KF_KOKIRI_SWORD_CHEST, 67026 },
    { RC_KF_STORMS_GROTTO_CHEST, 67027 },
    { RC_KF_LINKS_HOUSE_COW, 67028 },
    { RC_KF_GS_KNOW_IT_ALL_HOUSE, 67029 },
    { RC_KF_GS_BEAN_PATCH, 67030 },
    { RC_KF_GS_HOUSE_OF_TWINS, 67031 },
    { RC_KF_SHOP_ITEM_1, 67032 },
    { RC_KF_SHOP_ITEM_2, 67033 },
    { RC_KF_SHOP_ITEM_3, 67034 },
    { RC_KF_SHOP_ITEM_4, 67035 },
    { RC_KF_SHOP_ITEM_5, 67036 },
    { RC_KF_SHOP_ITEM_6, 67037 },
    { RC_KF_SHOP_ITEM_7, 67038 },
    { RC_KF_SHOP_ITEM_8, 67039 },

    { RC_LW_GIFT_FROM_SARIA, 67040 },
    { RC_LW_OCARINA_MEMORY_GAME, 67041 },
    { RC_LW_TARGET_IN_WOODS, 67042 },
    { RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST, 67043 },
    { RC_DEKU_THEATER_SKULL_MASK, 67044 },
    { RC_DEKU_THEATER_MASK_OF_TRUTH, 67045 },
    { RC_LW_SKULL_KID, 67046 },
    { RC_LW_DEKU_SCRUB_NEAR_BRIDGE, 67047 },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT, 67048 },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, 67049 },
    { RC_LW_DEKU_SCRUB_GROTTO_FRONT, 67050 },
    { RC_LW_DEKU_SCRUB_GROTTO_REAR, 67051 },
    { RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE, 67052 },
    { RC_LW_GS_BEAN_PATCH_NEAR_THEATER, 67053 },
    { RC_LW_GS_ABOVE_THEATER, 67054 },

    { RC_SFM_WOLFOS_GROTTO_CHEST, 67055 },
    { RC_SFM_DEKU_SCRUB_GROTTO_FRONT, 67056 },
    { RC_SFM_DEKU_SCRUB_GROTTO_REAR, 67057 },
    { RC_SFM_GS, 67058 },

    { RC_HF_OCARINA_OF_TIME_ITEM, 67059 },
    { RC_HF_NEAR_MARKET_GROTTO_CHEST, 67060 },
    { RC_HF_TEKTITE_GROTTO_FREESTANDING_POH, 67061 },
    { RC_HF_SOUTHEAST_GROTTO_CHEST, 67062 },
    { RC_HF_OPEN_GROTTO_CHEST, 67063 },
    { RC_HF_DEKU_SCRUB_GROTTO, 67064 },
    { RC_HF_COW_GROTTO_COW, 67065 },
    { RC_HF_GS_COW_GROTTO, 67066 },
    { RC_HF_GS_NEAR_KAK_GROTTO, 67067 },

    { RC_MARKET_SHOOTING_GALLERY_REWARD, 67068 },
    { RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE, 67069 },
    { RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, 67070 },
    { RC_MARKET_LOST_DOG, 67072 },
    { RC_MARKET_TREASURE_CHEST_GAME_REWARD, 67073 },
    { RC_MARKET_10_BIG_POES, 67074 },
    { RC_MARKET_GS_GUARD_HOUSE, 67075 },
    { RC_MARKET_BAZAAR_ITEM_1, 67076 },
    { RC_MARKET_BAZAAR_ITEM_2, 67077 },
    { RC_MARKET_BAZAAR_ITEM_3, 67078 },
    { RC_MARKET_BAZAAR_ITEM_4, 67079 },
    { RC_MARKET_BAZAAR_ITEM_5, 67080 },
    { RC_MARKET_BAZAAR_ITEM_6, 67081 },
    { RC_MARKET_BAZAAR_ITEM_7, 67082 },
    { RC_MARKET_BAZAAR_ITEM_8, 67083 },
    { RC_MARKET_POTION_SHOP_ITEM_1, 67084 },
    { RC_MARKET_POTION_SHOP_ITEM_2, 67085 },
    { RC_MARKET_POTION_SHOP_ITEM_3, 67086 },
    { RC_MARKET_POTION_SHOP_ITEM_4, 67087 },
    { RC_MARKET_POTION_SHOP_ITEM_5, 67088 },
    { RC_MARKET_POTION_SHOP_ITEM_6, 67089 },
    { RC_MARKET_POTION_SHOP_ITEM_7, 67090 },
    { RC_MARKET_POTION_SHOP_ITEM_8, 67091 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_1, 67092 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_2, 67093 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_3, 67094 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_4, 67095 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_5, 67096 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_6, 67097 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_7, 67098 },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_8, 67099 },

    { RC_TOT_LIGHT_ARROWS_CUTSCENE, 67100 },

    { RC_HC_MALON_EGG, 67101 },
    { RC_HC_ZELDAS_LETTER, 67102 },
    { RC_HC_GREAT_FAIRY_REWARD, 67103 },
    { RC_HC_GS_TREE, 67104 },
    { RC_HC_GS_STORMS_GROTTO, 67105 },

    { RC_LLR_TALONS_CHICKENS, 67106 },
    { RC_LLR_FREESTANDING_POH, 67107 },
    { RC_LLR_DEKU_SCRUB_GROTTO_LEFT, 67108 },
    { RC_LLR_DEKU_SCRUB_GROTTO_CENTER, 67109 },
    { RC_LLR_DEKU_SCRUB_GROTTO_RIGHT, 67110 },
    { RC_LLR_STABLES_LEFT_COW, 67111 },
    { RC_LLR_STABLES_RIGHT_COW, 67112 },
    { RC_LLR_TOWER_LEFT_COW, 67113 },
    { RC_LLR_TOWER_RIGHT_COW, 67114 },
    { RC_LLR_GS_HOUSE_WINDOW, 67115 },
    { RC_LLR_GS_TREE, 67116 },
    { RC_LLR_GS_RAIN_SHED, 67117 },
    { RC_LLR_GS_BACK_WALL, 67118 },

    { RC_KAK_ANJU_AS_CHILD, 67119 },
    { RC_KAK_ANJU_AS_ADULT, 67120 },
    { RC_KAK_IMPAS_HOUSE_FREESTANDING_POH, 67121 },
    { RC_KAK_WINDMILL_FREESTANDING_POH, 67122 },
    { RC_KAK_MAN_ON_ROOF, 67123 },
    { RC_KAK_OPEN_GROTTO_CHEST, 67124 },
    { RC_KAK_REDEAD_GROTTO_CHEST, 67125 },
    { RC_KAK_SHOOTING_GALLERY_REWARD, 67126 },
    { RC_KAK_10_GOLD_SKULLTULA_REWARD, 67127 },
    { RC_KAK_20_GOLD_SKULLTULA_REWARD, 67128 },
    { RC_KAK_30_GOLD_SKULLTULA_REWARD, 67129 },
    { RC_KAK_40_GOLD_SKULLTULA_REWARD, 67130 },
    { RC_KAK_50_GOLD_SKULLTULA_REWARD, 67131 },
    { RC_KAK_IMPAS_HOUSE_COW, 67132 },
    { RC_KAK_GS_TREE, 67133 },
    { RC_KAK_GS_GUARDS_HOUSE, 67134 },
    { RC_KAK_GS_WATCHTOWER, 67135 },
    { RC_KAK_GS_SKULLTULA_HOUSE, 67136 },
    { RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION, 67137 },
    { RC_KAK_GS_ABOVE_IMPAS_HOUSE, 67138 },
    { RC_KAK_BAZAAR_ITEM_1, 67139 },
    { RC_KAK_BAZAAR_ITEM_2, 67140 },
    { RC_KAK_BAZAAR_ITEM_3, 67141 },
    { RC_KAK_BAZAAR_ITEM_4, 67142 },
    { RC_KAK_BAZAAR_ITEM_5, 67143 },
    { RC_KAK_BAZAAR_ITEM_6, 67144 },
    { RC_KAK_BAZAAR_ITEM_7, 67145 },
    { RC_KAK_BAZAAR_ITEM_8, 67146 },
    { RC_KAK_POTION_SHOP_ITEM_1, 67147 },
    { RC_KAK_POTION_SHOP_ITEM_2, 67148 },
    { RC_KAK_POTION_SHOP_ITEM_3, 67149 },
    { RC_KAK_POTION_SHOP_ITEM_4, 67150 },
    { RC_KAK_POTION_SHOP_ITEM_5, 67151 },
    { RC_KAK_POTION_SHOP_ITEM_6, 67152 },
    { RC_KAK_POTION_SHOP_ITEM_7, 67153 },
    { RC_KAK_POTION_SHOP_ITEM_8, 67154 },

    { RC_GRAVEYARD_SHIELD_GRAVE_CHEST, 67155 },
    { RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST, 67156 },
    { RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST, 67157 },
    { RC_GRAVEYARD_FREESTANDING_POH, 67158 },
    { RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, 67159 },
    { RC_GRAVEYARD_HOOKSHOT_CHEST, 67160 },
    { RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH, 67161 },
    { RC_GRAVEYARD_GS_BEAN_PATCH, 67162 },
    { RC_GRAVEYARD_GS_WALL, 67163 },

    { RC_DMT_FREESTANDING_POH, 67164 },
    { RC_DMT_CHEST, 67165 },
    { RC_DMT_STORMS_GROTTO_CHEST, 67166 },
    { RC_DMT_GREAT_FAIRY_REWARD, 67167 },
    { RC_DMT_TRADE_CLAIM_CHECK, 67168 },
    { RC_DMT_COW_GROTTO_COW, 67169 },
    { RC_DMT_GS_NEAR_KAK, 67170 },
    { RC_DMT_GS_BEAN_PATCH, 67171 },
    { RC_DMT_GS_ABOVE_DODONGOS_CAVERN, 67172 },
    { RC_DMT_GS_FALLING_ROCKS_PATH, 67173 },

    { RC_GC_DARUNIAS_JOY, 67174 },
    { RC_GC_POT_FREESTANDING_POH, 67175 },
    { RC_GC_ROLLING_GORON_AS_CHILD, 67176 },
    { RC_GC_ROLLING_GORON_AS_ADULT, 67177 },
    { RC_GC_MEDIGORON, 67178 },
    { RC_GC_MAZE_LEFT_CHEST, 67179 },
    { RC_GC_MAZE_RIGHT_CHEST, 67180 },
    { RC_GC_MAZE_CENTER_CHEST, 67181 },
    { RC_GC_DEKU_SCRUB_GROTTO_LEFT, 67182 },
    { RC_GC_DEKU_SCRUB_GROTTO_CENTER, 67183 },
    { RC_GC_DEKU_SCRUB_GROTTO_RIGHT, 67184 },
    { RC_GC_GS_CENTER_PLATFORM, 67185 },
    { RC_GC_GS_BOULDER_MAZE, 67186 },
    { RC_GC_SHOP_ITEM_1, 67187 },
    { RC_GC_SHOP_ITEM_2, 67188 },
    { RC_GC_SHOP_ITEM_3, 67189 },
    { RC_GC_SHOP_ITEM_4, 67190 },
    { RC_GC_SHOP_ITEM_5, 67191 },
    { RC_GC_SHOP_ITEM_6, 67192 },
    { RC_GC_SHOP_ITEM_7, 67193 },
    { RC_GC_SHOP_ITEM_8, 67194 },

    { RC_DMC_VOLCANO_FREESTANDING_POH, 67195 },
    { RC_DMC_WALL_FREESTANDING_POH, 67196 },
    { RC_DMC_UPPER_GROTTO_CHEST, 67197 },
    { RC_DMC_GREAT_FAIRY_REWARD, 67198 },
    { RC_DMC_DEKU_SCRUB, 67199 },
    { RC_DMC_DEKU_SCRUB_GROTTO_LEFT, 67200 },
    { RC_DMC_DEKU_SCRUB_GROTTO_CENTER, 67201 },
    { RC_DMC_DEKU_SCRUB_GROTTO_RIGHT, 67202 },
    { RC_DMC_GS_CRATE, 67203 },
    { RC_DMC_GS_BEAN_PATCH, 67204 },

    { RC_ZR_MAGIC_BEAN_SALESMAN, 67205 },
    { RC_ZR_OPEN_GROTTO_CHEST, 67206 },
    { RC_ZR_FROGS_IN_THE_RAIN, 67207 },
    { RC_ZR_FROGS_OCARINA_GAME, 67208 },
    { RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, 67209 },
    { RC_ZR_NEAR_DOMAIN_FREESTANDING_POH, 67210 },
    { RC_ZR_DEKU_SCRUB_GROTTO_FRONT, 67211 },
    { RC_ZR_DEKU_SCRUB_GROTTO_REAR, 67212 },
    { RC_ZR_GS_TREE, 67213 },
    { RC_ZR_GS_LADDER, 67214 },
    { RC_ZR_GS_NEAR_RAISED_GROTTOS, 67215 },
    { RC_ZR_GS_ABOVE_BRIDGE, 67216 },

    { RC_ZR_FROGS_ZELDAS_LULLABY, 67747 },
    { RC_ZR_FROGS_EPONAS_SONG, 67748 },
    { RC_ZR_FROGS_SARIAS_SONG, 67749 },
    { RC_ZR_FROGS_SUNS_SONG, 67750 },
    { RC_ZR_FROGS_SONG_OF_TIME, 67751 },

    { RC_ZD_DIVING_MINIGAME, 67217 },
    { RC_ZD_CHEST, 67218 },
    { RC_ZD_KING_ZORA_THAWED, 67219 },
    { RC_ZD_GS_FROZEN_WATERFALL, 67220 },
    { RC_ZD_SHOP_ITEM_1, 67221 },
    { RC_ZD_SHOP_ITEM_2, 67222 },
    { RC_ZD_SHOP_ITEM_3, 67223 },
    { RC_ZD_SHOP_ITEM_4, 67224 },
    { RC_ZD_SHOP_ITEM_5, 67225 },
    { RC_ZD_SHOP_ITEM_6, 67226 },
    { RC_ZD_SHOP_ITEM_7, 67227 },
    { RC_ZD_SHOP_ITEM_8, 67228 },

    { RC_ZF_GREAT_FAIRY_REWARD, 67229 },
    { RC_ZF_ICEBERC_FREESTANDING_POH, 67230 },
    { RC_ZF_BOTTOM_FREESTANDING_POH, 67231 },
    { RC_ZF_GS_ABOVE_THE_LOG, 67232 },
    { RC_ZF_GS_TREE, 67233 },
    { RC_ZF_GS_HIDDEN_CAVE, 67234 },

    { RC_LH_UNDERWATER_ITEM, 67235 },
    { RC_LH_CHILD_FISHING, 67236 },
    { RC_LH_ADULT_FISHING, 67237 },
    { RC_LH_LAB_DIVE, 67238 },
    { RC_LH_FREESTANDING_POH, 69239 },
    { RC_LH_SUN, 69240 },
    { RC_LH_DEKU_SCRUB_GROTTO_LEFT, 69241 },
    { RC_LH_DEKU_SCRUB_GROTTO_CENTER, 69242 },
    { RC_LH_DEKU_SCRUB_GROTTO_RIGHT, 69243 },
    { RC_LH_GS_BEAN_PATCH, 69244 },
    { RC_LH_GS_LAB_WALL, 67245 },
    { RC_LH_GS_SMALL_ISLAND, 67246 },
    { RC_LH_GS_LAB_CRATE, 67247 },
    { RC_LH_GS_TREE, 67248 },

    { RC_GV_CRATE_FREESTANDING_POH, 67249 },
    { RC_GV_WATERFALL_FREESTANDING_POH, 67250 },
    { RC_GV_CHEST, 67251 },
    { RC_GV_DEKU_SCRUB_GROTTO_FRONT, 67252 },
    { RC_GV_DEKU_SCRUB_GROTTO_REAR, 67253 },
    { RC_GV_COW, 67254 },
    { RC_GV_GS_SMALL_BRIDGE, 67255 },
    { RC_GV_GS_BEAN_PATCH, 67256 },
    { RC_GV_GS_BEHIND_TENT, 67257 },
    { RC_GV_GS_PILLAR, 67258 },

    { RC_GF_CHEST, 67259 },
    { RC_GF_HBA_1000_POINTS, 67260 },
    { RC_GF_HBA_1500_POINTS, 67261 },
    { RC_GF_GS_TOP_FLOOR, 67262 },
    { RC_GF_GS_ARCHERY_RANGE, 67263 },

    { RC_HIDEOUT_JAIL_GUARD_1_TORCH, 67264 },
    { RC_HIDEOUT_JAIL_GUARD_2_TORCHES, 67265 },
    { RC_HIDEOUT_JAIL_GUARD_3_TORCHES, 67266 },
    { RC_HIDEOUT_JAIL_GUARD_4_TORCHES, 67267 },
    { RC_HIDEOUT_GERUDO_MEMBERSHIP_CARD, 67268 },

    { RC_WASTELAND_BOMBCHU_SALESMAN, 67269 },
    { RC_WASTELAND_CHEST, 67270 },
    { RC_WASTELAND_GS, 67271 },

    { RC_COLOSSUS_GREAT_FAIRY_REWARD, 67272 },
    { RC_COLOSSUS_FREESTANDING_POH, 67273 },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, 67274 },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR, 67275 },
    { RC_COLOSSUS_GS_BEAN_PATCH, 67276 },
    { RC_COLOSSUS_GS_TREE, 67277 },
    { RC_COLOSSUS_GS_HILL, 67278 },

    { RC_OGC_GREAT_FAIRY_REWARD, 67279 },
    { RC_OGC_GS, 67280 },

    { RC_DEKU_TREE_MAP_CHEST, 67281 },
    { RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, 67282 },
    { RC_DEKU_TREE_SLINGSHOT_CHEST, 67283 },
    { RC_DEKU_TREE_COMPASS_CHEST, 67284 },
    { RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, 67285 },
    { RC_DEKU_TREE_BASEMENT_CHEST, 67286 },
    { RC_DEKU_TREE_GS_COMPASS_ROOM, 67287 },
    { RC_DEKU_TREE_GS_BASEMENT_VINES, 67288 },
    { RC_DEKU_TREE_GS_BASEMENT_GATE, 67289 },
    { RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM, 67290 },
    { RC_DEKU_TREE_MQ_MAP_CHEST, 67291 },
    { RC_DEKU_TREE_MQ_SLINGSHOT_CHEST, 67292 },
    { RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, 67293 },
    { RC_DEKU_TREE_MQ_COMPASS_CHEST, 67294 },
    { RC_DEKU_TREE_MQ_BASEMENT_CHEST, 67295 },
    { RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST, 69296 },
    { RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST, 69297 },
    { RC_DEKU_TREE_MQ_DEKU_SCRUB, 69298 },
    { RC_DEKU_TREE_MQ_GS_LOBBY, 69299 },
    { RC_DEKU_TREE_MQ_COMPASS_CHEST, 69300 },
    { RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM, 69301 },
    { RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM, 69302 },
    { RC_DEKU_TREE_QUEEN_GOHMA_HEART, 69303 },

    { RC_DODONGOS_CAVERN_MAP_CHEST, 67304 },
    { RC_DODONGOS_CAVERN_COMPASS_CHEST, 67305 },
    { RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, 67306 },
    { RC_DODONGOS_CAVERN_BOMB_BAG_CHEST, 67307 },
    { RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, 67308 },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, 67309 },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY, 67310 },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT, 67311 },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, 67312 },
    { RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS, 67313 },
    { RC_DODONGOS_CAVERN_GS_SCARECROW, 67314 },
    { RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS, 67315 },
    { RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS, 67316 },
    { RC_DODONGOS_CAVERN_GS_BACK_ROOM, 67317 },
    { RC_DODONGOS_CAVERN_MQ_MAP_CHEST, 67318 },
    { RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, 67319 },
    { RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST, 67320 },
    { RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST, 67321 },
    { RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST, 67322 },
    { RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, 67323 },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT, 67324 },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR, 67325 },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, 67326 },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE, 67327 },
    { RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM, 67328 },
    { RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM, 67329 },
    { RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM, 67330 },
    { RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM, 67331 },
    { RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA, 67332 },
    { RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST, 67333 },
    { RC_DODONGOS_CAVERN_KING_DODONGO_HEART, 67334 },

    { RC_JABU_JABUS_BELLY_BOOMERANG_CHEST, 67335 },
    { RC_JABU_JABUS_BELLY_MAP_CHEST, 67336 },
    { RC_JABU_JABUS_BELLY_COMPASS_CHEST, 67337 },
    { RC_JABU_JABUS_BELLY_DEKU_SCRUB, 67338 },
    { RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, 67339 },
    { RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER, 67340 },
    { RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, 67341 },
    { RC_JABU_JABUS_BELLY_GS_NEAR_BOSS, 67342 },
    { RC_JABU_JABUS_BELLY_MQ_MAP_CHEST, 67343 },
    { RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, 67344 },
    { RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST, 67345 },
    { RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST, 67346 },
    { RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, 67347 },
    { RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST, 67348 },
    { RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST, 67349 },
    { RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST, 67350 },
    { RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, 67351 },
    { RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST, 67352 },
    { RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST, 67353 },
    { RC_JABU_JABUS_BELLY_MQ_COW, 67354 },
    { RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM, 67355 },
    { RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM, 67356 },
    { RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM, 67357 },
    { RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS, 67358 },
    { RC_JABU_JABUS_BELLY_BARINADE_HEART, 67359 },

    { RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST, 67360 },
    { RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST, 67361 },
    { RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST, 67362 },
    { RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST, 67363 },
    { RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY, 67364 },
    { RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST, 67365 },
    { RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST, 67366 },
    { RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST, 67367 },
    { RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST, 67368 },
    { RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST, 67369 },
    { RC_BOTTOM_OF_THE_WELL_MAP_CHEST, 67370 },
    { RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST, 67371 },
    { RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST, 67372 },
    { RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST, 67373 },
    { RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM, 67374 },
    { RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM, 67375 },
    { RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE, 67376 },
    { RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST, 67377 },
    { RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, 67378 },
    { RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST, 67379 },
    { RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, 67380 },
    { RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST, 67381 },
    { RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM, 67382 },
    { RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM, 67383 },
    { RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT, 67384 },

    { RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, 67385 },
    { RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST, 67386 },
    { RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, 67387 },
    { RC_FOREST_TEMPLE_MAP_CHEST, 67388 },
    { RC_FOREST_TEMPLE_WELL_CHEST, 67389 },
    { RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, 67390 },
    { RC_FOREST_TEMPLE_BOSS_KEY_CHEST, 67391 },
    { RC_FOREST_TEMPLE_FLOORMASTER_CHEST, 67392 },
    { RC_FOREST_TEMPLE_RED_POE_CHEST, 67393 },
    { RC_FOREST_TEMPLE_BOW_CHEST, 67394 },
    { RC_FOREST_TEMPLE_BLUE_POE_CHEST, 67395 },
    { RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, 67396 },
    { RC_FOREST_TEMPLE_BASEMENT_CHEST, 67397 },
    { RC_FOREST_TEMPLE_GS_FIRST_ROOM, 67398 },
    { RC_FOREST_TEMPLE_GS_LOBBY, 67399 },
    { RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD, 67400 },
    { RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, 67401 },
    { RC_FOREST_TEMPLE_GS_BASEMENT, 67402 },
    { RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, 67403 },
    { RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST, 67404 },
    { RC_FOREST_TEMPLE_MQ_WELL_CHEST, 67405 },
    { RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, 67406 },
    { RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, 67407 },
    { RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, 67408 },
    { RC_FOREST_TEMPLE_MQ_REDEAD_CHEST, 67409 },
    { RC_FOREST_TEMPLE_MQ_MAP_CHEST, 67410 },
    { RC_FOREST_TEMPLE_MQ_BOW_CHEST, 67411 },
    { RC_FOREST_TEMPLE_MQ_COMPASS_CHEST, 67412 },
    { RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, 67413 },
    { RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST, 67414 },
    { RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, 67415 },
    { RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, 67416 },
    { RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, 67417 },
    { RC_FOREST_TEMPLE_MQ_GS_WELL, 67418 },
    { RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, 67419 },
    { RC_FOREST_TEMPLE_PHANTOM_GANON_HEART, 67420 },

    { RC_FIRE_TEMPLE_NEAR_BOSS_CHEST, 67421 },
    { RC_FIRE_TEMPLE_FLARE_DANCER_CHEST, 67422 },
    { RC_FIRE_TEMPLE_BOSS_KEY_CHEST, 67423 },
    { RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST, 67424 },
    { RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, 67425 },
    { RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, 67426 },
    { RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST, 67427 },
    { RC_FIRE_TEMPLE_MAP_CHEST, 67428 },
    { RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, 67429 },
    { RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST, 67430 },
    { RC_FIRE_TEMPLE_SCARECROW_CHEST, 67431 },
    { RC_FIRE_TEMPLE_COMPASS_CHEST, 67432 },
    { RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST, 67433 },
    { RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST, 67434 },
    { RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP, 67435 },
    { RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM, 67436 },
    { RC_FIRE_TEMPLE_GS_BOULDER_MAZE, 67437 },
    { RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB, 67438 },
    { RC_FIRE_TEMPLE_GS_SCARECROW_TOP, 67439 },
    { RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, 67440 },
    { RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, 67441 },
    { RC_FIRE_TEMPLE_MQ_MAP_CHEST, 67442 },
    { RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST, 67443 },
    { RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, 67444 },
    { RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST, 67445 },
    { RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, 67446 },
    { RC_FIRE_TEMPLE_MQ_COMPASS_CHEST, 67447 },
    { RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST, 67448 },
    { RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST, 67449 },
    { RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY, 67450 },
    { RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE, 67451 },
    { RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR, 67452 },
    { RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE, 67453 },
    { RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER, 67454 },
    { RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM, 67455 },
    { RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE, 67456 },
    { RC_FIRE_TEMPLE_VOLVAGIA_HEART, 67457 },

    { RC_WATER_TEMPLE_COMPASS_CHEST, 67458 },
    { RC_WATER_TEMPLE_MAP_CHEST, 67459 },
    { RC_WATER_TEMPLE_CRACKED_WALL_CHEST, 67460 },
    { RC_WATER_TEMPLE_TORCHES_CHEST, 67461 },
    { RC_WATER_TEMPLE_BOSS_KEY_CHEST, 67462 },
    { RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST, 67463 },
    { RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, 67464 },
    { RC_WATER_TEMPLE_LONGSHOT_CHEST, 67465 },
    { RC_WATER_TEMPLE_RIVER_CHEST, 67466 },
    { RC_WATER_TEMPLE_DRAGON_CHEST, 67467 },
    { RC_WATER_TEMPLE_GS_BEHIND_GATE, 67468 },
    { RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, 67469 },
    { RC_WATER_TEMPLE_GS_CENTRAL_PILLAR, 67470 },
    { RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM, 67471 },
    { RC_WATER_TEMPLE_GS_RIVER, 67472 },
    { RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST, 67473 },
    { RC_WATER_TEMPLE_MQ_MAP_CHEST, 67474 },
    { RC_WATER_TEMPLE_MQ_COMPASS_CHEST, 67475 },
    { RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST, 67476 },
    { RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST, 67477 },
    { RC_WATER_TEMPLE_MQ_FREESTANDING_KEY, 67478 },
    { RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY, 67479 },
    { RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH, 67480 },
    { RC_WATER_TEMPLE_MQ_GS_RIVER, 67481 },
    { RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA, 67482 },
    { RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH, 67483 },
    { RC_WATER_TEMPLE_MORPHA_HEART, 67484 },

    { RC_SHADOW_TEMPLE_MAP_CHEST, 67485 },
    { RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST, 67486 },
    { RC_SHADOW_TEMPLE_COMPASS_CHEST, 67487 },
    { RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST, 67488 },
    { RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST, 67489 },
    { RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, 67490 },
    { RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST, 67491 },
    { RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST, 67492 },
    { RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST, 67493 },
    { RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST, 67494 },
    { RC_SHADOW_TEMPLE_FREESTANDING_KEY, 67495 },
    { RC_SHADOW_TEMPLE_WIND_HINT_CHEST, 67496 },
    { RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST, 67497 },
    { RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST, 67498 },
    { RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST, 67499 },
    { RC_SHADOW_TEMPLE_BOSS_KEY_CHEST, 67500 },
    { RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST, 67501 },
    { RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM, 67502 },
    { RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM, 67503 },
    { RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT, 67504 },
    { RC_SHADOW_TEMPLE_GS_NEAR_SHIP, 67505 },
    { RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT, 67506 },
    { RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST, 67507 },
    { RC_SHADOW_TEMPLE_MQ_MAP_CHEST, 67508 },
    { RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, 67509 },
    { RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST, 67510 },
    { RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, 67511 },
    { RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, 67512 },
    { RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST, 67513 },
    { RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST, 67514 },
    { RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST, 67515 },
    { RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST, 67516 },
    { RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, 67517 },
    { RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST, 67518 },
    { RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST, 67519 },
    { RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST, 67520 },
    { RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, 67521 },
    { RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST, 67522 },
    { RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST, 67523 },
    { RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, 67524 },
    { RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY, 67525 },
    { RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST, 67526 },
    { RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM, 67527 },
    { RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM, 67528 },
    { RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND, 67529 },
    { RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP, 67530 },
    { RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS, 67531 },
    { RC_SHADOW_TEMPLE_BONGO_BONGO_HEART, 67532 },

    { RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST, 67533 },
    { RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST, 67534 },
    { RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, 67535 },
    { RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST, 67536 },
    { RC_SPIRIT_TEMPLE_MAP_CHEST, 67537 },
    { RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST, 67538 },
    { RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST, 67539 },
    { RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST, 67540 },
    { RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, 67541 },
    { RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST, 67542 },
    { RC_SPIRIT_TEMPLE_MQ_MAP_CHEST, 67543 },
    { RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST, 67544 },
    { RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, 67545 },
    { RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST, 67546 },
    { RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST, 67547 },
    { RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, 67548 },
    { RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST, 67549 },
    { RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, 67550 },
    { RC_SPIRIT_TEMPLE_COMPASS_CHEST, 67551 },
    { RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST, 67552 },
    { RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST, 67553 },
    { RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, 67554 },
    { RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST, 67555 },
    { RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST, 67556 },
    { RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST, 67557 },
    { RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, 67558 },
    { RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST, 67559 },
    { RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST, 67560 },
    { RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST, 67561 },
    { RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, 67562 },
    { RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST, 67563 },
    { RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST, 67564 },
    { RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST, 67565 },
    { RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST, 67566 },
    { RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST, 67567 },
    { RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, 67568 },
    { RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST, 67569 },
    { RC_SPIRIT_TEMPLE_TOPMOST_CHEST, 67570 },
    { RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, 67571 },
    { RC_SPIRIT_TEMPLE_GS_METAL_FENCE, 67572 },
    { RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM, 67573 },
    { RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM, 67574 },
    { RC_SPIRIT_TEMPLE_GS_LOBBY, 67575 },
    { RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM, 67576 },
    { RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM, 67577 },
    { RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM, 67578 },
    { RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM, 67579 },
    { RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST, 67580 },
    { RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH, 67581 },
    { RC_SPIRIT_TEMPLE_TWINROVA_HEART, 67582 },

    { RC_ICE_CAVERN_MAP_CHEST, 67583 },
    { RC_ICE_CAVERN_COMPASS_CHEST, 67584 },
    { RC_ICE_CAVERN_IRON_BOOTS_CHEST, 67585 },
    { RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, 67586 },
    { RC_ICE_CAVERN_GS_HEART_PIECE_ROOM, 67587 },
    { RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM, 67588 },
    { RC_ICE_CAVERN_FREESTANDING_POH, 67589 },
    { RC_ICE_CAVERN_MQ_MAP_CHEST, 67590 },
    { RC_ICE_CAVERN_MQ_COMPASS_CHEST, 67591 },
    { RC_ICE_CAVERN_MQ_FREESTANDING_POH, 67592 },
    { RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, 67593 },
    { RC_ICE_CAVERN_MQ_GS_RED_ICE, 67594 },
    { RC_ICE_CAVERN_MQ_GS_ICE_BLOCK, 67595 },
    { RC_ICE_CAVERN_MQ_GS_SCARECROW, 67596 },

    { RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST, 67597 },
    { RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST, 67598 },
    { RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST, 67599 },
    { RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST, 67600 },
    { RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST, 67601 },
    { RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST, 67602 },
    { RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST, 67603 },
    { RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST, 67604 },
    { RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST, 67605 },
    { RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST, 67606 },
    { RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST, 67607 },
    { RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST, 67608 },
    { RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY, 67609 },
    { RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST, 67610 },
    { RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST, 67611 },
    { RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST, 67612 },
    { RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST, 67613 },
    { RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST, 67614 },
    { RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST, 67615 },
    { RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST, 67616 },
    { RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST, 67617 },
    { RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST, 67618 },
    { RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST, 67619 },
    { RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST, 67620 },
    { RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST, 67621 },
    { RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST, 67622 },
    { RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST, 67623 },
    { RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST, 67624 },
    { RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST, 67625 },
    { RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST, 67626 },
    { RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST, 67627 },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST, 67628 },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST, 67629 },
    { RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST, 67630 },
    { RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST, 67631 },
    { RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST, 67632 },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST, 67633 },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST, 67634 },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST, 67635 },

    { RC_GANONS_CASTLE_FOREST_TRIAL_CHEST, 67636 },
    { RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST, 67637 },
    { RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, 67638 },
    { RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST, 67639 },
    { RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, 67640 },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST, 67641 },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST, 67642 },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST, 67643 },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST, 67644 },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST, 67645 },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST, 67646 },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST, 67647 },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST, 67648 },
    { RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, 67649 },
    { RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST, 67650 },
    { RC_GANONS_CASTLE_DEKU_SCRUB_LEFT, 67651 },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT, 67652 },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, 67653 },
    { RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT, 67654 },
    { RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY, 67655 },
    { RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST, 67656 },
    { RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, 67657 },
    { RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, 67658 },
    { RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, 67659 },
    { RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST, 67660 },
    { RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, 67661 },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST, 67662 },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST, 67663 },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST, 67664 },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST, 67665 },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST, 67666 },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST, 67667 },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT, 67668 },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT, 67669 },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER, 67670 },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, 67671 },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT, 67672 },

    { RC_GANONS_TOWER_BOSS_KEY_CHEST, 67673 },

    { RC_GIFT_FROM_SAGES, 67746 },
};
std::map<int64_t, RandomizerCheck> ReverseAPCheckLookup;

template<typename K, typename V>
static std::map<V, K> reverse_map(const std::map<K, V>& m) {
    std::map<V, K> r;
    for (const auto& kv : m)
        r[kv.second] = kv.first;
    return r;
}

void OoTAP_InitCheckLookup() {
    ReverseAPCheckLookup = reverse_map(APCheckLookup);
}

std::vector<int64_t> const& OoTAP_GetSupportedLocations() {
    std::vector<int64_t> checks;
    for (const auto& kv : APCheckLookup) {
        checks.push_back(kv.second);
    }

    return checks;
}

int64_t OoTAP_RandoCheckToAPCheck(RandomizerCheck check) {
    if (APCheckLookup.count(check)) {
        return APCheckLookup[check];
    }

    return -404;
}

RandomizerCheck OoTAP_APCheckToRandoCheck(int64_t id) {
    if (ReverseAPCheckLookup.count(id)) {
        return ReverseAPCheckLookup[id];
    }

    return RC_UNKNOWN_CHECK;
}