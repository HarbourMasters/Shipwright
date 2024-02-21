#pragma once

#include <libultraship/libultraship.h>

#ifdef __cplusplus
class TimeSplitWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif


static const char* itemImage[156] {
    "ITEM_STICK",               "ITEM_NUT",             "ITEM_BOMB",                "ITEM_BOW",                 "ITEM_ARROW_FIRE",          "ITEM_DINS_FIRE",
    "ITEM_SLINGSHOT",           "ITEM_OCARINA_FAIRY",   "ITEM_OCARINA_TIME",        "ITEM_BOMBCHU",             "ITEM_HOOKSHOT",            "ITEM_LONGSHOT",
    "ITEM_ARROW_ICE",           "ITEM_FARORES_WIND",    "ITEM_BOOMERANG",           "ITEM_LENS",                "ITEM_BEAN",                "ITEM_HAMMER",
    "ITEM_ARROW_LIGHT",         "ITEM_NAYRUS_LOVE",     "ITEM_BOTTLE",              "ITEM_POTION_RED",          "ITEM_POTION_GREEN",        "ITEM_POTION_BLUE",
    "ITEM_FAIRY",               "ITEM_FISH",            "ITEM_MILK_BOTTLE",         "ITEM_LETTER_RUTO",         "ITEM_BLUE_FIRE",           "ITEM_BUG",
    "ITEM_BIG_POE",             "ITEM_MILK_HALF",       "ITEM_POE",                 "ITEM_WEIRD_EGG",           "ITEM_CHICKEN",             "ITEM_LETTER_ZELDA",
    "ITEM_MASK_KEATON",         "ITEM_MASK_SKULL",      "ITEM_MASK_SPOOKY",         "ITEM_MASK_BUNNY",          "ITEM_MASK_GORON",          "ITEM_MASK_ZORA",
    "ITEM_MASK_GERUDO",         "ITEM_MASK_TRUTH",      "ITEM_SOLD_OUT",            "ITEM_POCKET_EGG",          "ITEM_POCKET_CUCCO",        "ITEM_COJIRO",
    "ITEM_ODD_MUSHROOM",        "ITEM_ODD_POTION",      "ITEM_SAW",                 "ITEM_SWORD_BROKEN",        "ITEM_PRESCRIPTION",        "ITEM_FROG",
    "ITEM_EYEDROPS",            "ITEM_CLAIM_CHECK",     "ITEM_BOW_ARROW_FIRE",      "ITEM_BOW_ARROW_ICE",       "ITEM_BOW_ARROW_LIGHT",     "ITEM_SWORD_KOKIRI",
    "ITEM_SWORD_MASTER",        "ITEM_SWORD_BGS",       "ITEM_SHIELD_DEKU",         "ITEM_SHIELD_HYLIAN",       "ITEM_SHIELD_MIRROR",       "ITEM_TUNIC_KOKIRI",
    "ITEM_TUNIC_GORON",         "ITEM_TUNIC_ZORA",      "ITEM_BOOTS_KOKIRI",        "ITEM_BOOTS_IRON",          "ITEM_BOOTS_HOVER",         "ITEM_BULLET_BAG_30",
    "ITEM_BULLET_BAG_40",       "ITEM_BULLET_BAG_50",   "ITEM_QUIVER_30",           "ITEM_QUIVER_40",           "ITEM_QUIVER_50",           "ITEM_BOMB_BAG_20",
    "ITEM_BOMB_BAG_30",         "ITEM_BOMB_BAG_40",     "ITEM_BRACELET",            "ITEM_GAUNTLETS_SILVER",    "ITEM_GAUNTLETS_GOLD",      "ITEM_SCALE_SILVER",
    "ITEM_SCALE_GOLDEN",        "ITEM_SWORD_KNIFE",     "ITEM_WALLET_ADULT",        "ITEM_WALLET_GIANT",        "ITEM_SEEDS",               "ITEM_FISHING_POLE",
    "QUEST_SONG_MINUET",        "QUEST_SONG_BOLERO",    "QUEST_SONG_SERENADE",      "QUEST_SONG_REQUIEM",       "QUEST_SONG_NOCTURNE",      "QUEST_SONG_PRELUDE",
    "QUEST_SONG_LULLABY",       "QUEST_SONG_EPONA",     "QUEST_SONG_SARIA",         "QUEST_SONG_SUN",           "QUEST_SONG_TIME",          "QUEST_SONG_STORMS",
    "QUEST_MEDALLION_FOREST",   "QUEST_MEDALLION_FIRE", "QUEST_MEDALLION_WATER",    "QUEST_MEDALLION_SPIRIT",   "QUEST_MEDALLION_SHADOW",   "QUEST_MEDALLION_LIGHT",
    "QUEST_KOKIRI_EMERALD",     "QUEST_GORON_RUBY",     "QUEST_ZORA_SAPPHIRE",      "QUEST_STONE_OF_AGONY",     "QUEST_GERUDO_CARD",        "QUEST_SKULL_TOKEN",
    "ITEM_HEART_CONTAINER",     "ITEM_HEART_PIECE",     "ITEM_KEY_BOSS",            "ITEM_COMPASS",             "ITEM_DUNGEON_MAP",         "ITEM_KEY_SMALL",
    "ITEM_MAGIC_SMALL",         "ITEM_MAGIC_LARGE",     "ITEM_HEART_PIECE_2",       "ITEM_SINGLE_MAGIC",        "ITEM_DOUBLE_MAGIC",        "ITEM_DOUBLE_DEFENSE",
    "ITEM_INVALID_4",           "ITEM_INVALID_5",       "ITEM_INVALID_6",           "ITEM_INVALID_7",           "ITEM_MILK",                "ITEM_HEART",    
    "ITEM_RUPEE_GREEN",         "ITEM_RUPEE_BLUE",      "ITEM_RUPEE_RED",           "ITEM_RUPEE_PURPLE",        "ITEM_RUPEE_GOLD",          "ITEM_INVALID_8",           
    "ITEM_STICKS_5",            "ITEM_STICKS_10",       "ITEM_NUTS_5",              "ITEM_NUTS_10",             "ITEM_BOMBS_5",             "ITEM_BOMBS_10",            
    "ITEM_BOMBS_20",            "ITEM_BOMBS_30",        "ITEM_ARROWS_SMALL",        "ITEM_ARROWS_MEDIUM",       "ITEM_ARROWS_LARGE",        "ITEM_SEEDS_30",    
    "ITEM_BOMBCHUS_5",          "ITEM_BOMBCHUS_20",     "ITEM_STICK_UPGRADE_20",    "ITEM_STICK_UPGRADE_30",    "ITEM_NUT_UPGRADE_30",      "ITEM_NUT_UPGRADE_40"
};

typedef struct {
    uint32_t itemID;
    const char* itemName;
    const char* itemImage;
} TimeSplitObject;