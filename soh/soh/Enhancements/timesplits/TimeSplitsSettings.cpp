
#include "TimesplitsSettings.h"
#include "Timesplits.h"
#include <libultraship/libultraship.h>
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"

#include "soh/ShipUtils.h"
#include "soh_assets.h"
#include "soh/SohGui/ImGuiUtils.h"
#include "assets/textures/parameter_static/parameter_static.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "assets/textures/icon_item_static/icon_item_static.h"

extern "C" {
#include "variables.h"
}

IndexRangeObject sceneRange = { 0, 77 };
uint32_t sceneFilterIndex = 0;

// clang-format off
std::vector<TimesplitObject> sceneObjectList = {
    // Dungeon Entrances
    { SCENE_DEKU_TREE, "Inside the Deku Tree" },
    { SCENE_DODONGOS_CAVERN, "Dodongo's Cavern" },
    { SCENE_JABU_JABU, "Inside Jabu-Jabu's Belly" },
    { SCENE_FOREST_TEMPLE, "Forest Temple" },
    { SCENE_FIRE_TEMPLE, "Fire Temple" },
    { SCENE_WATER_TEMPLE, "Water Temple" },
    { SCENE_SPIRIT_TEMPLE, "Spirit Temple" },
    { SCENE_SHADOW_TEMPLE, "Shadow Temple" },
    { SCENE_BOTTOM_OF_THE_WELL, "Bottom of the Well" },
    { SCENE_ICE_CAVERN, "Ice Cavern" },
    { SCENE_GERUDO_TRAINING_GROUND, "Gerudo Training Ground" },
    { SCENE_INSIDE_GANONS_CASTLE, "Inside Ganon's Castle" },
    { SCENE_GANONS_TOWER, "Ganon's Tower" },
    { SCENE_GANONS_TOWER_COLLAPSE_INTERIOR, "Tower Collapse" },
    { SCENE_INSIDE_GANONS_CASTLE_COLLAPSE, "Castle Collapse" },

    // Kokiri Forest Region
    { SCENE_KOKIRI_FOREST, "Kokiri Forest" },
    { SCENE_LOST_WOODS, "Lost Woods" },
    { SCENE_SACRED_FOREST_MEADOW, "Sacred Forest Meadow" },
    { SCENE_KOKIRI_SHOP, "Kokiri Shop" },
    { SCENE_LINKS_HOUSE, "Link's House" },
    { SCENE_KNOW_IT_ALL_BROS_HOUSE, "Know-It-All Bros' House" },
    { SCENE_TWINS_HOUSE, "Twins' House" },
    { SCENE_MIDOS_HOUSE, "Mido's House" },
    { SCENE_SARIAS_HOUSE, "Saria's House" },

    // Hyrule Field & Lon Lon Ranch Region
    { SCENE_HYRULE_FIELD, "Hyrule Field" },
    { SCENE_MARKET_DAY, "Market (Day)" },
    { SCENE_MARKET_NIGHT, "Market (Night)" },
    { SCENE_MARKET_RUINS, "Market (Adult)" },
    { SCENE_HYRULE_CASTLE, "Hyrule Castle" },
    { SCENE_OUTSIDE_GANONS_CASTLE, "Outside Ganon's Castle" },
    { SCENE_TEMPLE_OF_TIME, "Temple of Time" },
    { SCENE_LON_LON_RANCH, "Lon Lon Ranch" },
    { SCENE_LON_LON_BUILDINGS, "Ranch House" },
    { SCENE_STABLE, "Stable" },
    { SCENE_CASTLE_COURTYARD_ZELDA, "Castle Courtyard" },
    { SCENE_SHOOTING_GALLERY, "Shooting Gallery" },
    { SCENE_BAZAAR, "Bazaar" },
    { SCENE_BOMBCHU_SHOP, "Bombchu Shop" },
    { SCENE_HAPPY_MASK_SHOP, "Happy Mask Shop" },
    { SCENE_TREASURE_BOX_SHOP, "Treasure Box Shop" },
    { SCENE_POTION_SHOP_MARKET, "Market Potion Shop" },
    { SCENE_BOMBCHU_BOWLING_ALLEY, "Bombchu Bowling Alley" },
    { SCENE_MARKET_GUARD_HOUSE, "Guard House" },
    { SCENE_DOG_LADY_HOUSE, "Richard's House" },
    { SCENE_BACK_ALLEY_HOUSE, "Man in Green's House" },

    // Kakariko & Death Mountain Region
    { SCENE_KAKARIKO_VILLAGE, "Kakariko Village" },
    { SCENE_GRAVEYARD, "Graveyard" },
    { SCENE_HOUSE_OF_SKULLTULA, "House of Skulltula" },
    { SCENE_DEATH_MOUNTAIN_TRAIL, "Death Mountain Trail" },
    { SCENE_DEATH_MOUNTAIN_CRATER, "Death Mountain Crater" },
    { SCENE_GORON_CITY, "Goron City" },
    { SCENE_KAKARIKO_CENTER_GUEST_HOUSE, "Carpenter Boss's House" },
    { SCENE_POTION_SHOP_KAKARIKO, "Kakariko Potion Shop" },
    { SCENE_POTION_SHOP_GRANNY, "Granny's Potion Shop" },
    { SCENE_IMPAS_HOUSE, "Impa's House" },
    { SCENE_GRAVEKEEPERS_HUT, "Gravekeeper's Hut" },
    { SCENE_GRAVE_WITH_FAIRYS_FOUNTAIN, "Fairy Fountain Grave" },
    { SCENE_ROYAL_FAMILYS_TOMB, "Royal Family's Tomb" },
    { SCENE_WINDMILL_AND_DAMPES_GRAVE, "Dampe's Grave" },
    { SCENE_REDEAD_GRAVE, "Redead Grave" },
    { SCENE_GORON_SHOP, "Goron Shop" },

    // Lake Hylia & Gerudo Valley Region
    { SCENE_LAKE_HYLIA, "Lake Hylia" },
    { SCENE_GERUDO_VALLEY, "Gerudo Valley" },
    { SCENE_DESERT_COLOSSUS, "Desert Colossus" },
    { SCENE_GERUDOS_FORTRESS, "Gerudo's Fortress" },
    { SCENE_THIEVES_HIDEOUT, "Thieves' Hideout" },
    { SCENE_HAUNTED_WASTELAND, "Haunted Wasteland" },
    { SCENE_FISHING_POND, "Fishing Pond" },
    { SCENE_LAKESIDE_LABORATORY, "Lakeside Lab" },
    { SCENE_CARPENTERS_TENT, "Carpenters' Tent" },

    // Zora Region
    { SCENE_ZORAS_RIVER, "Zora's River" },
    { SCENE_ZORAS_DOMAIN, "Zora's Domain" },
    { SCENE_ZORAS_FOUNTAIN, "Zora's Fountain" },
    { SCENE_ZORA_SHOP, "Zora Shop" },

    // Miscellaneous Areas
    { SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, "Great Fairy" },
    { SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS, "Great Fairy" },
    { SCENE_FAIRYS_FOUNTAIN, "Fairy Fountain" },
    { SCENE_GROTTOS, "Grotto" },
};

std::vector<TimesplitObject> splitObjectList = {
    // Equipment
    { ITEM_SWORD_KOKIRI,               "Kokiri Sword" },
    { ITEM_SWORD_MASTER,               "Master Sword" },
    { ITEM_SWORD_BGS,                  "Biggoron's Sword" },
    { ITEM_SHIELD_DEKU,                "Deku Shield" },
    { ITEM_SHIELD_HYLIAN,              "Hylian Shield" },
    { ITEM_SHIELD_MIRROR,              "Mirror Shield" },
    { ITEM_TUNIC_GORON,                "Goron Tunic" },
    { ITEM_TUNIC_ZORA,                 "Zora Tunic" },
    { ITEM_BOOTS_IRON,                 "Iron Boots" },
    { ITEM_BOOTS_HOVER,                "Hover Boots" },
    { ITEM_BRACELET,                   "Goron's Bracelet" },
    { SPLIT_SINGLE_MAGIC,    		   "Magic" },
    { ITEM_WALLET_ADULT,               "Adult's Wallet" },
    { SPLIT_DOUBLE_DEFENSE,            "Double Defense" },
    
    // Inventory
    { ITEM_STICK,                      "Deku Stick" },
    { ITEM_NUT,                        "Deku Nut" },
    { ITEM_BOMB,                       "Bomb" },
    { ITEM_BOW,                        "Fairy Bow" },
    { ITEM_ARROW_FIRE,                 "Fire Arrow" },
    { ITEM_DINS_FIRE,                  "Din's Fire" },
    { ITEM_SLINGSHOT,                  "Slingshot" },
    { ITEM_OCARINA_FAIRY,              "Fairy Ocarina" },
    { ITEM_BOMBCHU,                    "Bombchu" },
    { ITEM_HOOKSHOT,                   "Hookshot" },
    { ITEM_ARROW_ICE,                  "Ice Arrow" },
    { ITEM_FARORES_WIND,               "Farore's Wind" },
    { ITEM_BOOMERANG,                  "Boomerang" },
    { ITEM_LENS,                       "Lens of Truth" },
    { ITEM_BEAN,                       "Magic Bean" },
    { ITEM_HAMMER,                     "Megaton Hammer" },
    { ITEM_ARROW_LIGHT,                "Light Arrow" },
    { ITEM_NAYRUS_LOVE,                "Nayru's Love" },
    { ITEM_BOTTLE,                     "Empty Bottle" },
    { ITEM_POCKET_EGG,                 "Pocket Egg" },
    { ITEM_WEIRD_EGG,                  "Weird Egg" },
    
    // Quest
    { ITEM_SONG_LULLABY,               "Zelda's Lullaby" },
    { ITEM_SONG_EPONA,                 "Epona's Song" },
    { ITEM_SONG_SARIA,                 "Saria's Song" },
    { ITEM_SONG_SUN,                   "Sun's Song" },
    { ITEM_SONG_TIME,                  "Song of Time" },
    { ITEM_SONG_STORMS,                "Song of Storms" },
    { ITEM_SONG_MINUET,                "Minuet of Forest" },
    { ITEM_SONG_BOLERO,                "Bolero of Fire" },
    { ITEM_SONG_SERENADE,              "Serenade of Water" },
    { ITEM_SONG_REQUIEM,               "Requiem of Spirit" },
    { ITEM_SONG_NOCTURNE,              "Nocturne of Shadow" },
    { ITEM_SONG_PRELUDE,               "Prelude of Light" },
    { ITEM_MEDALLION_FOREST, 		   "Forest Medallion" },
    { ITEM_MEDALLION_FIRE,   		   "Fire Medallion" },
    { ITEM_MEDALLION_WATER,  		   "Water Medallion" },
    { ITEM_MEDALLION_SPIRIT, 		   "Spirit Medallion" },
    { ITEM_MEDALLION_SHADOW, 		   "Shadow Medallion" },
    { ITEM_MEDALLION_LIGHT,  		   "Light Medallion" },
    { ITEM_KOKIRI_EMERALD,   		   "Kokiri's Emerald" },
    { ITEM_GORON_RUBY,       		   "Goron's Ruby" },
    { ITEM_ZORA_SAPPHIRE,    		   "Zora's Sapphire" },

    // Bosses
    { SPLIT_KILLED_GOHMA,              "Queen Gohma" },
    { SPLIT_KILLED_KING_DODONGO,       "King Dodongo" },
    { SPLIT_KILLED_BARINADE,           "Barinade" },
    { SPLIT_KILLED_PHANTOM_GANON,      "Phantom Ganon" },
    { SPLIT_KILLED_MORPHA,             "Morpha" },
    { SPLIT_KILLED_VOLVAGIA,           "Volvagia" },
    { SPLIT_KILLED_BONGO_BONGO,        "Bongo Bongo" },
    { SPLIT_KILLED_TWINROVA,           "Twinrova" },
    { SPLIT_KILLED_GANONDORF,          "Ganondorf" },
    { SPLIT_KILLED_GANON,              "Ganon" },
    
    // Upgrade Items
    { ITEM_OCARINA_TIME,               "Ocarina of Time" },
    { ITEM_LONGSHOT,                   "Longshot" },
    { ITEM_BULLET_BAG_40,              "Big Bullet Bag" },
    { ITEM_BULLET_BAG_50,              "Biggest Bullet Bag" },
    { ITEM_QUIVER_40,                  "Big Quiver" },
    { ITEM_QUIVER_50,                  "Biggest Quiver" },
    { ITEM_BOMB_BAG_20,                "Bomb Bag" },
    { ITEM_BOMB_BAG_30,                "Big Bomb Bag" },
    { ITEM_BOMB_BAG_40,                "Biggest Bomb Bag" },
    { ITEM_WALLET_GIANT,               "Giant's Wallet" },
    { ITEM_GAUNTLETS_SILVER,           "Silver Gauntlets" },
    { ITEM_GAUNTLETS_GOLD,             "Gold Gauntlets" },
    { ITEM_SCALE_SILVER,               "Silver Scale" },
    { ITEM_SCALE_GOLDEN,               "Gold Scale" },
    { SPLIT_DOUBLE_MAGIC,    		   "Double Magic" },
    
    // Trade Items
    { ITEM_CHICKEN,                    "Chicken" },
    { ITEM_LETTER_ZELDA,               "Zelda's Letter" },
    { ITEM_MASK_KEATON,                "Keaton Mask" },
    { ITEM_MASK_SKULL,                 "Skull Mask" },
    { ITEM_MASK_SPOOKY,                "Spooky Mask" },
    { ITEM_MASK_BUNNY,                 "Bunny Hood" },
    { ITEM_MASK_GORON,                 "Goron Mask" },
    { ITEM_MASK_ZORA,                  "Zora Mask" },
    { ITEM_MASK_GERUDO,                "Gerudo Mask" },
    { ITEM_MASK_TRUTH,                 "Mask of Truth" },
    { ITEM_POCKET_CUCCO,               "Pocket Cucco" },
    { ITEM_COJIRO,                     "Cojiro" },
    { ITEM_ODD_MUSHROOM,               "Odd Mushroom" },
    { ITEM_ODD_POTION,                 "Odd Potion" },
    { ITEM_SAW,                        "Poacher's Saw" },
    { ITEM_SWORD_BROKEN,               "Broke Goron Sword" },
    { ITEM_PRESCRIPTION,               "Prescription" },
    { ITEM_FROG,                       "Eyeball Frog" },
    { ITEM_EYEDROPS,                   "Eye Drops" },
    { ITEM_CLAIM_CHECK,                "Claim Check" },
    
    
    // Bottled Items
    { ITEM_POTION_RED,                 "Red Potion" },
    { ITEM_POTION_GREEN,               "Green Potion" },
    { ITEM_POTION_BLUE,                "Blue Potion" },
    { ITEM_FAIRY,                      "Fairy" },
    { ITEM_FISH,                       "Fish" },
    { ITEM_MILK_BOTTLE,                "Milk" },
    { ITEM_LETTER_RUTO,                "Ruto's Letter" },
    { ITEM_BLUE_FIRE,                  "Blue Fire" },
    { ITEM_BUG,                        "Bugs" },
    { ITEM_BIG_POE,                    "Big Poe" },
    { ITEM_MILK_HALF,                  "Milk (Half)" },
    { ITEM_POE,                        "Poe" },
};

std::map<uint32_t, std::vector<uint32_t>> itemSubMenuList = {
    { ITEM_BOW,             { ITEM_BOW, ITEM_QUIVER_40, ITEM_QUIVER_50 } },
    { ITEM_BOMB,            { ITEM_BOMB_BAG_20, ITEM_BOMB_BAG_30, ITEM_BOMB_BAG_40 } },
    { ITEM_WEIRD_EGG,       { ITEM_WEIRD_EGG, ITEM_CHICKEN, ITEM_LETTER_ZELDA, ITEM_MASK_KEATON, ITEM_MASK_SKULL, ITEM_MASK_SPOOKY,
                              ITEM_MASK_BUNNY, ITEM_MASK_GORON, ITEM_MASK_ZORA, ITEM_MASK_GERUDO, ITEM_MASK_TRUTH } },
    { ITEM_POCKET_EGG,      { ITEM_POCKET_EGG, ITEM_POCKET_CUCCO, ITEM_COJIRO, ITEM_ODD_MUSHROOM, ITEM_ODD_POTION,
                              ITEM_SAW, ITEM_SWORD_BROKEN, ITEM_PRESCRIPTION, ITEM_FROG, ITEM_EYEDROPS, ITEM_CLAIM_CHECK } },
    { ITEM_BOTTLE,          { ITEM_POTION_RED, ITEM_POTION_GREEN, ITEM_POTION_BLUE, ITEM_FAIRY,
                              ITEM_FISH, ITEM_MILK_BOTTLE, ITEM_LETTER_RUTO, ITEM_BLUE_FIRE, 
                              ITEM_BUG, ITEM_BIG_POE, ITEM_MILK_HALF, ITEM_POE } },
    { ITEM_SCALE_SILVER,    { ITEM_SCALE_SILVER, ITEM_SCALE_GOLDEN } },
    { ITEM_BRACELET,        { ITEM_BRACELET, ITEM_GAUNTLETS_SILVER, ITEM_GAUNTLETS_GOLD } },
    { ITEM_WALLET_ADULT,    { ITEM_WALLET_ADULT, ITEM_WALLET_GIANT } },
    { SPLIT_SINGLE_MAGIC,   { SPLIT_SINGLE_MAGIC, SPLIT_DOUBLE_MAGIC } },
};

std::map<int16_t, ImVec4> itemColorMap = {
    { ITEM_SONG_LULLABY,           { 0.878f, 0.420f, 1.0f, 1.0f } },
    { ITEM_SONG_EPONA,             { 1.0f, 0.765f, 0.235f, 1.0f } },
    { ITEM_SONG_SARIA,             { 0.498f, 1.0f, 0.537f, 1.0f } },
    { ITEM_SONG_SUN,               { 1.0f, 1.0f, 0.235f, 1.0f } },
    { ITEM_SONG_TIME,              { 0.467f, 0.925f, 1.0f, 1.0f } },
    { ITEM_SONG_STORMS,            { 0.647f, 0.647f, 0.647f, 1.0f } },
    { ITEM_SONG_MINUET,            { 0.588f, 1.0f, 0.392f, 1.0f } },
    { ITEM_SONG_BOLERO,            { 1.0f, 0.314f, 0.157f, 1.0f } },
    { ITEM_SONG_SERENADE,          { 0.392f, 0.588f, 1.0f, 1.0f } },
    { ITEM_SONG_REQUIEM,           { 1.0f, 0.627f, 0.0f, 1.0f } },
    { ITEM_SONG_NOCTURNE,          { 1.0f, 0.392f, 1.0f, 1.0f } },
    { ITEM_SONG_PRELUDE,           { 1.0f, 0.941f, 0.392f, 1.0f } },
    { SPLIT_KILLED_GOHMA, 			UIWidgets::ColorValues.at(UIWidgets::Colors::Green) },
    { SPLIT_KILLED_KING_DODONGO, 	UIWidgets::ColorValues.at(UIWidgets::Colors::Red) },
    { SPLIT_KILLED_BARINADE, 		UIWidgets::ColorValues.at(UIWidgets::Colors::Blue) },
    { SPLIT_KILLED_PHANTOM_GANON, 	UIWidgets::ColorValues.at(UIWidgets::Colors::DarkGreen) },
    { SPLIT_KILLED_VOLVAGIA, 		UIWidgets::ColorValues.at(UIWidgets::Colors::DarkRed) },
    { SPLIT_KILLED_MORPHA, 			UIWidgets::ColorValues.at(UIWidgets::Colors::DarkBlue) },
    { SPLIT_KILLED_BONGO_BONGO, 	UIWidgets::ColorValues.at(UIWidgets::Colors::Purple) },
    { SPLIT_KILLED_TWINROVA, 		UIWidgets::ColorValues.at(UIWidgets::Colors::Orange) },
    { SPLIT_KILLED_GANONDORF, 		UIWidgets::ColorValues.at(UIWidgets::Colors::DarkGray) },
    { SPLIT_KILLED_GANON, 			UIWidgets::ColorValues.at(UIWidgets::Colors::Yellow) },
};

static std::vector<const char*> sceneAreaNameMap = {
    "All Scenes",
    "Dungeons",
    "Kokiri Forest",
    "Hyrule Field & Lon Lon Ranch",
    "Kakariko & Death Mountain",
    "Lake Hylia & Gerudo Valley",
    "Zora's River",
    "Miscellaneous",
};

static std::unordered_map<const char*, IndexRangeObject> sceneAreaRangeMap = {
    { "All Scenes",                     { SCENE_DEKU_TREE, SCENE_GROTTOS } },
    { "Dungeons",                       { SCENE_DEKU_TREE, SCENE_INSIDE_GANONS_CASTLE_COLLAPSE } },
    { "Kokiri Forest",                  { SCENE_KOKIRI_FOREST, SCENE_SARIAS_HOUSE } },
    { "Hyrule Field & Lon Lon Ranch",   { SCENE_HYRULE_FIELD, SCENE_BACK_ALLEY_HOUSE } },
    { "Kakariko & Death Mountain",      { SCENE_KAKARIKO_VILLAGE, SCENE_GORON_SHOP } },
    { "Lake Hylia & Gerudo Valley",     { SCENE_LAKE_HYLIA, SCENE_CARPENTERS_TENT } },
    { "Zora's River",                   { SCENE_ZORAS_RIVER, SCENE_ZORA_SHOP } },
    { "Miscellaneous",                  { SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC, SCENE_GROTTOS } },
};
// clang-format on

IndexRangeObject GetSceneIndexRange(uint32_t start, uint32_t end) {
    IndexRangeObject setRange = { 0, 0 };

    for (size_t i = 0; i < sceneObjectList.size(); i++) {
        if (sceneObjectList[i].splitId == start) {
            setRange.startIndex = static_cast<int>(i);
        }
        if (sceneObjectList[i].splitId == end) {
            setRange.endIndex = static_cast<int>(i);
        }
    }

    return setRange;
}

IndexRangeObject GetIndexRange(uint32_t start, uint32_t end) {
    IndexRangeObject setRange = { 0, 0 };

    for (size_t i = 0; i < splitObjectList.size(); i++) {
        if (splitObjectList[i].splitId == start) {
            setRange.startIndex = static_cast<int>(i);
        }
        if (splitObjectList[i].splitId == end) {
            setRange.endIndex = static_cast<int>(i);
        }
    }

    return setRange;
}

bool shouldPopUpOpen = false;
uint32_t popupItem = 0;
const char* popupTooltip = "";
IndexRangeObject range = GetIndexRange((uint32_t)ITEM_SWORD_KOKIRI, (uint32_t)SPLIT_DOUBLE_DEFENSE);
const char* listName = "Equipment";
uint32_t listColumns = 3;
const char* itemImage;
std::string listInputName;
std::vector<std::string> savedLists;
uint32_t selectedIndex = 0;
uint32_t comparedIndex = 0;

const char* GetItemImageById(uint32_t itemId) {
    if (itemId >= SPLIT_KILLED_GOHMA && itemId <= SPLIT_KILLED_GANON) {
        return "gWTriforcePieceTex";
    }
    if (itemId >= ITEM_SONG_MINUET && itemId <= ITEM_SONG_STORMS) {
        return "gSongNoteTex";
    }
    switch (itemId) {
        case SPLIT_SINGLE_MAGIC:
            return (const char*)gItemIcons[ITEM_MAGIC_SMALL];
        case SPLIT_DOUBLE_MAGIC:
            return (const char*)gItemIcons[ITEM_MAGIC_LARGE];
        case SPLIT_DOUBLE_DEFENSE:
            return (const char*)gItemIcons[ITEM_HEART_CONTAINER];
        default:
            if (itemId <= ITEM_NONE) {
                return itemMapping.find(itemId)->second.name.c_str();
            }
            break;
    }
}

ImVec4 GetItemColor(int16_t itemId) {
    ImVec4 itemColor = { 1, 1, 1, 1 };
    if ((itemId >= SPLIT_KILLED_GOHMA && itemId <= SPLIT_KILLED_GANON) ||
        (itemId >= ITEM_SONG_MINUET && itemId <= ITEM_SONG_STORMS)) {
        return itemColorMap.at(itemId);
    } else {
        return itemColor;
    }
}

ImVec2 GetItemImageSizeById(uint32_t itemId) {
    float defaultImageSize = 32.0f;
    if (itemId >= ITEM_SONG_MINUET && itemId <= ITEM_SONG_STORMS) {
        return ImVec2(defaultImageSize / 1.5f, defaultImageSize);
    } else {
        return ImVec2(defaultImageSize, defaultImageSize);
    }
}

void DrawOptions() {
    if (ImGui::BeginTable("Options", 3)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Enable Time Splits", "gSettings.TimeSplits.Enable",
                                {
                                    .color = THEME_COLOR,
                                });
        UIWidgets::Tooltip("Enables the Time Split system, splits will not occur with this unchecked.");

        ImGui::TableNextColumn();
        if (UIWidgets::CVarCheckbox("Show Headers", "gSettings.TimeSplits.ShowHeaders",
                                    {
                                        .color = THEME_COLOR,
                                    })) {
            UpdateSplitSettings(SPLIT_HEADERS);
        };
        UIWidgets::Tooltip("Shows the column names in the Split List.");

        ImGui::TableNextColumn();
        if (UIWidgets::CVarCheckbox("Hide Background", "gSettings.TimeSplits.Opacity",
                                    {
                                        .color = THEME_COLOR,
                                    })) {
            UpdateSplitSettings(SPLIT_OPACITY);
        };
        UIWidgets::Tooltip("Hides the background of the Splits List.\n"
                           "Note: The background will display if the window extrudes from the main game window.");

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Highlight Active Split", "gSettings.TimeSplits.Highlight",
                                {
                                    .color = THEME_COLOR,
                                });
        UIWidgets::Tooltip("Highlights the row with the current Active Split.");

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Follow Active Split", "gSettings.TimeSplits.Follow",
                                {
                                    .color = THEME_COLOR,
                                });
        UIWidgets::Tooltip("Forces the Split List window to keep the Active Split visible.\n"
                           "Note: This prevents user scrolling, disable to restore control.");

        ImGui::EndTable();
    }

    UIWidgets::CVarCheckbox("Compare Splits", "gSettings.TimeSplits.Compare",
                            {
                                .color = THEME_COLOR,
                            });
    UIWidgets::Tooltip("Enables Split Comparisons between lists, this will integrate within the Split List.");

    if (CVarGetInteger("gSettings.TimeSplits.Compare", 0)) {
        ImGui::SameLine();
        UIWidgets::PushStyleCombobox(THEME_COLOR);
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginCombo("##compareSplits", savedLists[comparedIndex].c_str())) {
            for (int i = 0; i < savedLists.size(); i++) {
                if (ImGui::Selectable(savedLists[i].c_str())) {
                    comparedIndex = i;
                    SplitLoadComparisonList();
                    break;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        UIWidgets::PopStyleCombobox();
    }
}

void DrawActionButtons() {
    if (ImGui::BeginTable("Action Buttons", 2)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextColumn();
        UIWidgets::InputString("New List", &listInputName,
                               {
                                   .labelPosition = UIWidgets::LabelPositions::None,
                                   .color = THEME_COLOR,
                                   .placeholder = "Enter new list name",
                               });

        ImGui::TableNextColumn();
        if (UIWidgets::Button("Create List", {
                                                 .color = THEME_COLOR,
                                             })) {
            SplitSaveFileAction(SPLIT_SAVE, listInputName);
            SplitSaveFileAction(SPLIT_RETRIEVE, "");
        }

        ImGui::TableNextColumn();
        UIWidgets::PushStyleCombobox(THEME_COLOR);
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::BeginCombo("##savedSplits", savedLists[selectedIndex].c_str())) {
            for (int i = 0; i < savedLists.size(); i++) {
                if (ImGui::Selectable(savedLists[i].c_str())) {
                    selectedIndex = i;
                    break;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        UIWidgets::PopStyleCombobox();

        ImGui::TableNextColumn();
        if (UIWidgets::Button("Save Splits", {
                                                 .size = { (ImGui::GetContentRegionAvail().x * 0.5f), 0 },
                                                 .color = THEME_COLOR,
                                             })) {
            if (savedLists[0] != "Create a List First") {
                SplitSaveFileAction(SPLIT_SAVE, savedLists[selectedIndex]);
            }
        }
        ImGui::SameLine();
        if (UIWidgets::Button("Load Splits", {
                                                 .size = { (ImGui::GetContentRegionAvail().x), 0 },
                                                 .color = THEME_COLOR,
                                             })) {
            if (savedLists[0] != "Create a List First") {
                SplitSaveFileAction(SPLIT_LOAD, savedLists[selectedIndex]);
            }
        }

        ImGui::TableNextColumn();
        if (UIWidgets::Button("New Attempt", {
                                                 .color = THEME_COLOR,
                                             })) {
            if (splitList.size() == 0) {
                return;
            }

            for (auto& splits : splitList) {
                splits.splitStatus = SPLIT_INACTIVE;
            }
            splitList[0].splitStatus = SPLIT_ACTIVE;
        }

        ImGui::TableNextColumn();
        if (UIWidgets::Button("Update Splits", {
                                                   .color = THEME_COLOR,
                                               })) {
            UpdateSplitBests();
        }

        ImGui::EndTable();
    }
}

void DrawEntranceList() {
    UIWidgets::PushStyleCombobox(THEME_COLOR);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::BeginCombo("##SceneFilter", sceneAreaNameMap[sceneFilterIndex])) {
        for (int i = 0; i < sceneAreaNameMap.size(); i++) {
            if (ImGui::Selectable(sceneAreaNameMap[i])) {
                sceneFilterIndex = i;
                sceneRange = GetSceneIndexRange(sceneAreaRangeMap.at(sceneAreaNameMap[i]).startIndex,
                                                sceneAreaRangeMap.at(sceneAreaNameMap[i]).endIndex);
                break;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    UIWidgets::PopStyleCombobox();

    if (ImGui::BeginChild("Entrance List")) {
        if (ImGui::BeginTable("Entrances", 2)) {
            for (int i = sceneRange.startIndex; i <= sceneRange.endIndex; i++) {
                ImGui::TableNextColumn();
                ImGui::PushID(sceneObjectList[i].splitId);
                SplitsPushImageButtonStyle();

                if (ImGui::ImageButton(
                        std::to_string(sceneObjectList[i].splitId).c_str(),
                        Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("gPauseUnusedCursorTex"),
                        ImVec2(32.0f, 32.0f))) {
                    AddSplitEntryBySceneId(sceneObjectList[i].splitId);
                };
                ImGui::SameLine();
                TableCellCenteredText(UIWidgets::ColorValues.at(UIWidgets::Colors::White),
                                      sceneObjectList[i].splitName.c_str());

                SplitsPopImageButtonStyle();
                ImGui::PopID();
            }

            ImGui::EndTable();
        }
        ImGui::EndChild();
    }
}

void DrawItemList(const char* tableName, IndexRangeObject range, uint32_t tableSize) {
    if (ImGui::BeginTable(tableName, tableSize)) {
        for (int i = range.startIndex; i <= range.endIndex; i++) {
            ImGui::TableNextColumn();
            SplitsPushImageButtonStyle();
            if (ImGui::ImageButton(std::to_string(splitObjectList[i].splitId).c_str(),
                                   Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                                       GetItemImageById(splitObjectList[i].splitId)),
                                   GetItemImageSizeById(splitObjectList[i].splitId) * 1.5f, ImVec2(0, 0), ImVec2(1, 1),
                                   ImVec4(0, 0, 0, 0), GetItemColor(splitObjectList[i].splitId))) {
                if (itemSubMenuList.contains(splitObjectList[i].splitId)) {
                    shouldPopUpOpen = true;
                    popupItem = splitObjectList[i].splitId;
                    ImGui::OpenPopup("ItemSubMenu");
                } else {
                    AddSplitEntryById(splitObjectList[i].splitId);
                }
            }
            UIWidgets::Tooltip(splitObjectList[i].splitName.c_str());
            if (listName == "Bosses") {
                ImGui::SameLine();
                TableCellCenteredText(UIWidgets::ColorValues.at(UIWidgets::Colors::White),
                                      splitObjectList[i].splitName.c_str());
            }

            SplitsPopImageButtonStyle();
        }
        HandlePopUpContext(popupItem);
        ImGui::EndTable();
    }
}

void TimesplitsSettingsWindow::DrawElement() {
    bool shouldRemoveEntry = false;
    uint32_t entryId = 0, entryIndex = 0;

    UIWidgets::PushStyleTabs(THEME_COLOR);
    if (ImGui::BeginTabBar("Timesplit Settings Tabs")) {
        if (ImGui::BeginTabItem("List Options")) {
            DrawOptions();
            DrawActionButtons();
            ImGui::SeparatorText("Current Splits");
            DrawSplitsList(false);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Split Entries")) {
            if (ImGui::BeginTable("Split Settings", 3)) {
                ImGui::TableSetupColumn("Preview",
                                        ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 88.0f);
                ImGui::TableSetupColumn("Item Categories",
                                        ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 120.0f);
                ImGui::TableSetupColumn("Item Grids",
                                        ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHeaderLabel);

                ImGui::TableNextColumn();
                ImGui::BeginDisabled();
                UIWidgets::Button("Preview", {
                                                 .color = THEME_COLOR,
                                             });
                ImGui::EndDisabled();
                ImGui::BeginChild("Preview List");
                for (size_t i = 0; i < splitList.size(); i++) {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetContentRegionAvail().x - 50.0f) * 0.5f));
                    ImGui::PushID(i);
                    SplitsPushImageButtonStyle();
                    if (ImGui::ImageButton(
                            std::to_string(i).c_str(),
                            Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                                splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemImageById(splitList[i].splitId)
                                                                            : "gPauseUnusedCursorTex"),
                            splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemImageSizeById(splitList[i].splitId)
                                                                        : ImVec2(32.0f, 32.0f),
                            ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                            splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemColor(splitList[i].splitId)
                                                                        : ImVec4(1, 1, 1, 1))) {
                        shouldRemoveEntry = true;
                        entryId = splitList[i].splitId;
                        entryIndex = i;
                    };
                    UIWidgets::Tooltip(splitList[i].splitName.c_str());

                    HandleDragAndDrop(i);
                    SplitsPopImageButtonStyle();
                    ImGui::PopID();
                }
                ImGui::EndChild();

                ImGui::TableNextColumn();
                if (UIWidgets::Button("Equipment", {
                                                       .color = THEME_COLOR,
                                                   })) {
                    range = GetIndexRange((uint32_t)ITEM_SWORD_KOKIRI, (uint32_t)SPLIT_DOUBLE_DEFENSE);
                    listName = "Equipment";
                    listColumns = 3;
                }
                if (UIWidgets::Button("Inventory", {
                                                       .color = THEME_COLOR,
                                                   })) {
                    range = GetIndexRange((uint32_t)ITEM_STICK, (uint32_t)ITEM_WEIRD_EGG);
                    listName = "Inventory";
                    listColumns = 6;
                }
                if (UIWidgets::Button("Quest", {
                                                   .color = THEME_COLOR,
                                               })) {
                    range = GetIndexRange((uint32_t)ITEM_SONG_LULLABY, (uint32_t)ITEM_ZORA_SAPPHIRE);
                    listName = "Quest";
                    listColumns = 6;
                }
                if (UIWidgets::Button("Bosses", {
                                                    .color = THEME_COLOR,
                                                })) {
                    range = GetIndexRange((uint32_t)SPLIT_KILLED_GOHMA, (uint32_t)SPLIT_KILLED_GANON);
                    listName = "Bosses";
                    listColumns = 2;
                }
                if (UIWidgets::Button("Entrances", {
                                                       .color = THEME_COLOR,
                                                   })) {
                    listName = "Entrances";
                }
                ImGui::TableNextColumn();
                if (listName != "Entrances") {
                    DrawItemList(listName, range, listColumns);
                } else {
                    DrawEntranceList();
                }

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    UIWidgets::PopStyleTabs();

    if (shouldRemoveEntry) {
        RemoveSplitEntry(entryId, entryIndex);
        shouldRemoveEntry = false;
    }
}

void TimesplitsSettingsWindow::InitElement() {
}