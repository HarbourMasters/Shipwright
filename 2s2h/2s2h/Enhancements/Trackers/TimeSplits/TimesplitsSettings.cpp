
#include "TimesplitsSettings.h"
#include "Timesplits.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
#include "2s2h/BenGui/UIWidgets.hpp"

extern "C" {
#include "variables.h"
}

#include "interface/parameter_static/parameter_static.h"
#include "assets/archives/icon_item_24_static/icon_item_24_static_yar.h"
#include "assets/archives/icon_item_static/icon_item_static_yar.h"
#include "assets/interface/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "assets/interface/icon_item_field_static/icon_item_field_static.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "GameInteractor/GameInteractor.h"

#include <fast/Fast3dGui.h>

IndexRangeObject sceneRange = { 0, 98 };
uint32_t sceneFilterIndex = 0;

std::vector<TimesplitObject> sceneObjectList = {
    // Misc. Areas
    { SCENE_LOST_WOODS, "Lost Woods (Intro)" },
    { SCENE_OPENINGDAN, "Before Clock Town" },
    { SCENE_KAKUSIANA, "Lone Peak Shrine & Grottos" },
    { SCENE_YOUSEI_IZUMI, "Fairy's Fountain" },
    { SCENE_KYOJINNOMA, "Giants' Chamber" },
    // Clock Town Areas
    { SCENE_TOWN, "East Clock Town" },
    { SCENE_ICHIBA, "West Clock Town" },
    { SCENE_BACKTOWN, "North Clock Town" },
    { SCENE_CLOCKTOWER, "South Clock Town" },
    { SCENE_INSIDETOWER, "Clock Tower Interior" },
    { SCENE_OKUJOU, "Clock Tower Rooftop" },
    { SCENE_ALLEY, "Laundry Pool" },
    { SCENE_YADOYA, "Stock Pot Inn" },
    { SCENE_DOUJOU, "Swordsman's School" },
    { SCENE_AYASHIISHOP, "Curiosity Shop" },
    { SCENE_8ITEMSHOP, "Trading Post" },
    { SCENE_BOMYA, "Bomb Shop" },
    { SCENE_BOWLING, "Honey & Darling's Shop" },
    { SCENE_SONCHONOIE, "The Mayor's Residence" },
    { SCENE_DEKUTES, "Deku Scrub Playground" },
    { SCENE_TAKARAKUJI, "Lottery Shop" },
    { SCENE_TAKARAYA, "Treasure Chest Shop" },
    { SCENE_MILK_BAR, "Milk Bar" },
    { SCENE_SYATEKI_MIZU, "Town Shooting Gallery" },
    { SCENE_POSTHOUSE, "Post Office" },
    // Termina Field Region
    { SCENE_00KEIKOKU, "Termina Field" },
    { SCENE_F01, "Romani Ranch" },
    { SCENE_TENMON_DAI, "Astral Observatory" },
    { SCENE_ROMANYMAE, "Milk Road" },
    { SCENE_KOEPONARACE, "Gorman Track" },
    { SCENE_OMOYA, "Mama's House & Barn" },
    { SCENE_F01_B, "Doggy Racetrack" },
    { SCENE_F01C, "Cucco Shack" },
    // Woodfall Region
    { SCENE_24KEMONOMITI, "Road to Southern Swamp" },
    { SCENE_20SICHITAI, "Southern Swamp (poison)" },
    { SCENE_20SICHITAI2, "Southern Swamp (Clear)" },
    { SCENE_22DEKUCITY, "Deku Palace" },
    { SCENE_21MITURINMAE, "Woodfall" },
    { SCENE_MITURIN, "Woodfall Temple" },
    { SCENE_MITURIN_BS, "Odolwa's Lair" },
    { SCENE_26SARUNOMORI, "Woods of Mystery" },
    { SCENE_WITCH_SHOP, "Magic Hags' Potion Shop" },
    { SCENE_MAP_SHOP, "Tourist Information" },
    { SCENE_DANPEI, "Deku Shrine" },
    { SCENE_DEKU_KING, "Deku King's Chamber" },
    { SCENE_SYATEKI_MORI, "Swamp Shooting Gallery" },
    { SCENE_KINSTA1, "Swamp Spider House" },
    // Snowhead Region
    { SCENE_13HUBUKINOMITI, "Path to Mountain Village" },
    { SCENE_10YUKIYAMANOMURA, "Mountain Village (winter)" },
    { SCENE_10YUKIYAMANOMURA2, "Mountain Village (spring)" },
    { SCENE_17SETUGEN, "Path to Goron Village (winter)" },
    { SCENE_17SETUGEN2, "Path to Goron Village (spring)" },
    { SCENE_11GORONNOSATO, "Goron Village (winter)" },
    { SCENE_11GORONNOSATO2, "Goron Village (spring)" },
    { SCENE_GORONRACE, "Goron Racetrack" },
    { SCENE_14YUKIDAMANOMITI, "Path to Snowhead" },
    { SCENE_12HAKUGINMAE, "Snowhead" },
    { SCENE_HAKUGIN, "Snowhead Temple" },
    { SCENE_HAKUGIN_BS, "Goht's Lair" },
    { SCENE_KAJIYA, "Mountain Smithy" },
    { SCENE_GORON_HAKA, "Goron Graveyard" },
    { SCENE_16GORON_HOUSE, "Goron Shrine" },
    { SCENE_GORONSHOP, "Goron Shop" },
    // Great Bay Region
    { SCENE_30GYOSON, "Great Bay Coast" },
    { SCENE_31MISAKI, "Zora Cape" },
    { SCENE_SINKAI, "Pinnacle Rock" },
    { SCENE_KAIZOKU, "Pirates' Fortress" },
    { SCENE_TORIDE, "Pirates' Fortress Moat" },
    { SCENE_PIRATE, "Pirates' Fortress Interior" },
    { SCENE_SEA, "Great Bay Temple" },
    { SCENE_SEA_BS, "Gyorg's Lair" },
    { SCENE_35TAKI, "Waterfall Rapids" },
    { SCENE_FISHERMAN, "Fisherman's Hut" },
    { SCENE_LABO, "Marine Research Lab" },
    { SCENE_KINDAN2, "Oceanside Spider House" },
    { SCENE_33ZORACITY, "Zora Hall" },
    // Ikana Region
    { SCENE_IKANAMAE, "Road to Ikana" },
    { SCENE_IKANA, "Ikana Canyon" },
    { SCENE_BOTI, "Ikana Graveyard" },
    { SCENE_HAKASHITA, "Beneath the Graveyard" },
    { SCENE_F40, "Stone Tower" },
    { SCENE_F41, "Inverted Stone Tower" },
    { SCENE_INISIE_N, "Stone Tower Temple" },
    { SCENE_INISIE_BS, "Twinmold's Lair" },
    { SCENE_INISIE_R, "Inverted Stone Tower Temple" },
    { SCENE_CASTLE, "Ancient Castle of Ikana" },
    { SCENE_IKNINSIDE, "Igos du Ikana's Lair" },
    { SCENE_DANPEI2TEST, "Beneath Graveyard and Dampe's House" },
    { SCENE_REDEAD, "Beneath the Well" },
    { SCENE_SECOM, "Sakon's Hideout" },
    { SCENE_TOUGITES, "Ghost Hut" },
    { SCENE_MUSICHOUSE, "Music Box House" },
    { SCENE_RANDOM, "Secret Shrine" },
    // The Moon
    { SCENE_SOUGEN, "The Moon" },
    { SCENE_LAST_DEKU, "Moon Deku Trial" },
    { SCENE_LAST_GORON, "Moon Goron Trial" },
    { SCENE_LAST_ZORA, "Moon Zora Trial" },
    { SCENE_LAST_LINK, "Moon Link Trial" },
    { SCENE_LAST_BS, "Majora's Lair" },
};

std::vector<TimesplitObject> splitObjectList = {
    // clang-format off
    { ITEM_HEART_PIECE, "Piece of Heart" },

    // Inventory
    { ITEM_OCARINA_OF_TIME, 	"Ocarina of Time" },
    { ITEM_BOW, 				"Bow" },
    { ITEM_ARROW_FIRE, 			"Fire Arrow" },
    { ITEM_ARROW_ICE, 			"Ice Arrow" },
    { ITEM_ARROW_LIGHT, 		"Light Arrow" },
    { ITEM_MOONS_TEAR, 			"Moon's Tear" },
    { ITEM_BOMB, 				"Bomb" },
    { ITEM_BOMBCHU, 			"Bombchu" },
    { ITEM_DEKU_STICK, 			"Deku Stick" },
    { ITEM_DEKU_NUT, 			"Deku Nut" },
    { ITEM_MAGIC_BEANS, 		"Magic Bean" },
    { ITEM_ROOM_KEY, 			"Room Key" },
    { ITEM_POWDER_KEG, 			"Powder Keg" },
    { ITEM_PICTOGRAPH_BOX, 		"Pictograph" },
    { ITEM_LENS_OF_TRUTH, 		"Lens of Truth" },
    { ITEM_HOOKSHOT, 			"Hookshot" },
    { ITEM_SWORD_GREAT_FAIRY, 	"Great Fairy Sword" },
    { ITEM_LETTER_TO_KAFEI,     "Letter to Kafei" },
    { ITEM_BOTTLE,              "Empty Bottle" },

    // Masks
    { ITEM_MASK_POSTMAN,        "Postman's Hat" },
    { ITEM_MASK_ALL_NIGHT,      "All-Night Mask" },
    { ITEM_MASK_BLAST,          "Blast Mask" },
    { ITEM_MASK_STONE,          "Stone Mask" },
    { ITEM_MASK_GREAT_FAIRY,    "Great Fairy's Mask" },
    { ITEM_MASK_DEKU,           "Deku Mask" },
    { ITEM_MASK_KEATON,         "Keaton Mask" },
    { ITEM_MASK_BREMEN,         "Bremen Mask" },
    { ITEM_MASK_BUNNY,          "Bunny Hood" },
    { ITEM_MASK_DON_GERO,       "Don Gero's Mask" },
    { ITEM_MASK_SCENTS,         "Mask of Scents" },
    { ITEM_MASK_GORON,          "Goron Mask" },
    { ITEM_MASK_ROMANI,         "Romani's Mask" },
    { ITEM_MASK_CIRCUS_LEADER,  "Circus Leader's Mask" },
    { ITEM_MASK_KAFEIS_MASK,    "Kafei's Mask" },
    { ITEM_MASK_COUPLE,         "Couple's Mask" },
    { ITEM_MASK_TRUTH,          "Mask of Truth" },
    { ITEM_MASK_ZORA,           "Zora Mask" },
    { ITEM_MASK_KAMARO,         "Kamaro's Mask" },
    { ITEM_MASK_GIBDO,          "Gibdo Mask" },
    { ITEM_MASK_GARO,           "Garo's Mask" },
    { ITEM_MASK_CAPTAIN,        "Captain's Hat" },
    { ITEM_MASK_GIANT,          "Giant's Mask" },
    { ITEM_MASK_FIERCE_DEITY,   "Fierce Deity's Mask" },

    // Songs
    { ITEM_SONG_TIME, 		    "Song of Time" },
    { ITEM_SONG_HEALING,	    "Song of Healing" },
    { ITEM_SONG_EPONA, 		    "Epona's Song" },
    { ITEM_SONG_SOARING, 	    "Song of Soaring" },
    { ITEM_SONG_STORMS, 	    "Song of Storms" },
    { ITEM_SONG_SONATA, 	    "Sonata of Awakening" },
    { ITEM_SONG_LULLABY, 	    "Goron Lullaby" },
    { ITEM_SONG_NOVA, 		    "New Wave Bossa Nova" },
    { ITEM_SONG_ELEGY, 		    "Elegy of Emptiness" },
    { ITEM_SONG_OATH, 		    "Oath to Order" },

    // Quest
    { ITEM_REMAINS_ODOLWA, 	 	"Odolwa's Remains" },
    { ITEM_REMAINS_GOHT, 	 	"Goht's Remains" },
    { ITEM_REMAINS_GYORG, 	 	"Gyorg's Remains" },
    { ITEM_REMAINS_TWINMOLD, 	"Twinmold's Remains" },
    { ITEM_SWORD_KOKIRI, 	 	"Kokiri Sword" },
    { ITEM_SHIELD_HERO, 	 	"Hero's Shield" },
    { ITEM_WALLET_ADULT, 	 	"Adult Wallet" },
    { ITEM_BOMBERS_NOTEBOOK, 	"Bombers' Notebook" },

    // Quest II
    { SPLIT_SINGLE_MAGIC,       "Single Magic" },
    { SPLIT_DOUBLE_DEFENSE,     "Double Defense" },

    // Dungeon Bosses
    { SPLIT_KILLED_ODOLWA,      "Odolwa" },
    { SPLIT_KILLED_GOHT,        "Goht" },
    { SPLIT_KILLED_GYORG,       "Gyorg" },
    { SPLIT_KILLED_TWINMOLD,    "Twinmold" },
    { SPLIT_KILLED_MAJORA,      "Majora" },


    // Upgrade Items
    { ITEM_QUIVER_30, 			"Quiver" },
    { ITEM_QUIVER_40, 			"Large Quiver" },
    { ITEM_QUIVER_50, 			"Largest Quiver" },
    { ITEM_BOMB_BAG_20, 		"Bomb Bag" },
    { ITEM_BOMB_BAG_30, 		"Big Bomb Bag" },
    { ITEM_BOMB_BAG_40, 		"Biggest Bomb Bag" },
    { ITEM_SONG_LULLABY_INTRO, 	"Goron Lullaby Intro" },
    { ITEM_SWORD_RAZOR, 	    "Razor Sword" },
    { ITEM_SWORD_GILDED, 	    "Gilded Sword" },
    { ITEM_SHIELD_MIRROR, 	    "Mirror Shield" },
    { ITEM_WALLET_GIANT, 	    "Giant Wallet" },
    { SPLIT_DOUBLE_MAGIC,       "Double Magic" },

    // Trade Items
    { ITEM_DEED_LAND, 			"Land Title Deed" },
    { ITEM_DEED_SWAMP, 			"Swamp Title Deed" },
    { ITEM_DEED_MOUNTAIN, 		"Mountain Title Deed" },
    { ITEM_DEED_OCEAN, 			"Ocean Title Deed" },
    { ITEM_LETTER_MAMA, 		"Letter to Mama" },
    { ITEM_PENDANT_OF_MEMORIES, "Pendant of Memories" },

    // Bottled Items
    { ITEM_POTION_RED,      	"Red Potion" },
    { ITEM_POTION_GREEN,        "Green Potion" },
    { ITEM_POTION_BLUE,         "Blue Potion" },
    { ITEM_FAIRY,    			"Bottled Fairy" },
    { ITEM_DEKU_PRINCESS,		"Bottled Deku Princess" },
    { ITEM_MILK_BOTTLE,     	"Bottled Full Milk" },
    { ITEM_MILK_HALF,      		"Bottled Half Milk" },
    { ITEM_FISH,          		"Bottled Fish" },
    { ITEM_BUG,       			"Bottled Bug" },
    { ITEM_BLUE_FIRE,      		"Bottled Blue Fire" },
    { ITEM_POE,          		"Bottled Poe" },
    { ITEM_BIG_POE,        		"Bottled Big Poe" },
    { ITEM_SPRING_WATER,  		"Spring Water" },
    { ITEM_HOT_SPRING_WATER,	"Hot Spring Water" },
    { ITEM_ZORA_EGG,       		"Bottled Zora Egg" },
    { ITEM_GOLD_DUST,      		"Bottled Gold Dust" },
    { ITEM_MUSHROOM,      		"Bottled Mushroom" },
    { ITEM_SEAHORSE,      		"Bottled Seahorse" },
    { ITEM_CHATEAU,        		"Chateau Romani" },

    // clang-format on
};

std::map<uint32_t, std::vector<uint32_t>> itemSubMenuList = {
    // clang-format off
    { ITEM_BOW,             { ITEM_BOW, ITEM_QUIVER_40, ITEM_QUIVER_50 } },
    { ITEM_BOMB,            { ITEM_BOMB_BAG_20, ITEM_BOMB_BAG_30, ITEM_BOMB_BAG_40 } },
    { ITEM_MOONS_TEAR,      { ITEM_MOONS_TEAR, ITEM_DEED_LAND, ITEM_DEED_SWAMP, ITEM_DEED_MOUNTAIN, ITEM_DEED_OCEAN } },
    { ITEM_ROOM_KEY,        { ITEM_ROOM_KEY, ITEM_LETTER_MAMA } },
    { ITEM_LETTER_TO_KAFEI, { ITEM_LETTER_TO_KAFEI, ITEM_PENDANT_OF_MEMORIES } },
    { ITEM_BOTTLE,          { ITEM_POTION_RED, ITEM_POTION_GREEN, ITEM_POTION_BLUE, ITEM_FAIRY, ITEM_DEKU_PRINCESS, 
                              ITEM_MILK_BOTTLE, ITEM_MILK_HALF, ITEM_FISH, ITEM_BUG, ITEM_BLUE_FIRE, ITEM_POE, 
                              ITEM_BIG_POE, ITEM_SPRING_WATER, ITEM_HOT_SPRING_WATER, ITEM_ZORA_EGG, ITEM_GOLD_DUST, 
                              ITEM_MUSHROOM, ITEM_SEAHORSE, ITEM_CHATEAU } },
    { ITEM_SONG_LULLABY,    { ITEM_SONG_LULLABY_INTRO, ITEM_SONG_LULLABY } },
    { ITEM_SWORD_KOKIRI,    { ITEM_SWORD_KOKIRI, ITEM_SWORD_RAZOR, ITEM_SWORD_GILDED } },
    { ITEM_SHIELD_HERO,     { ITEM_SHIELD_HERO, ITEM_SHIELD_MIRROR } },
    { ITEM_WALLET_ADULT,    { ITEM_WALLET_ADULT, ITEM_WALLET_GIANT } },
    { SPLIT_SINGLE_MAGIC,   { SPLIT_SINGLE_MAGIC, SPLIT_DOUBLE_MAGIC } },
};

static std::vector<const char*> sceneAreaNameMap = {
    "All Scenes",
    "Miscellaneous",
    "Clock Town",
    "Termina Field & Romani's Ranch",
    "Woodfall Region",
    "Snowhead Region",
    "Great Bay Region",
    "Ikana Region",
    "The Moon",
};

static std::unordered_map<const char*, IndexRangeObject> sceneAreaRangeMap = {
    { "All Scenes",                     { SCENE_LOST_WOODS, SCENE_LAST_BS } },
    { "Miscellaneous",                  { SCENE_LOST_WOODS, SCENE_KYOJINNOMA } },
    { "Clock Town",                     { SCENE_TOWN, SCENE_POSTHOUSE } },
    { "Termina Field & Romani's Ranch", { SCENE_00KEIKOKU, SCENE_F01C } },
    { "Woodfall Region",                { SCENE_24KEMONOMITI, SCENE_KINSTA1 } },
    { "Snowhead Region",                { SCENE_13HUBUKINOMITI, SCENE_GORONSHOP } },
    { "Great Bay Region",               { SCENE_30GYOSON, SCENE_33ZORACITY } },
    { "Ikana Region",                   { SCENE_IKANAMAE, SCENE_RANDOM } },
    { "The Moon",                       { SCENE_SOUGEN, SCENE_LAST_BS } },
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
IndexRangeObject range = GetIndexRange((uint32_t)ITEM_OCARINA_OF_TIME, (uint32_t)ITEM_BOTTLE);
const char* listName = "Inventory";
uint32_t listColumns = 6;
TexturePtr itemImage;
std::string listInputName;
std::vector<std::string> savedLists;
uint32_t selectedIndex = 0;
uint32_t comparedIndex = 0;

const char* GetItemImageById(uint32_t itemId) {
    switch (itemId) {
        case SPLIT_KILLED_ODOLWA:
        case SPLIT_KILLED_GOHT:
        case SPLIT_KILLED_GYORG:
        case SPLIT_KILLED_TWINMOLD:
        case SPLIT_KILLED_MAJORA:
            return gDungeonMapSkullTex;
        case SPLIT_SINGLE_MAGIC:
            return (const char*)gItemIcons[ITEM_MAGIC_JAR_SMALL];
        case SPLIT_DOUBLE_MAGIC:
            return (const char*)gItemIcons[ITEM_MAGIC_JAR_BIG];
        case SPLIT_DOUBLE_DEFENSE:
            return (const char*)gItemIcons[ITEM_HEART_CONTAINER];
        default:
            if (itemId <= ITEM_NONE) {
                return (const char*)gItemIcons[itemId];
            }
            break;
    }
}

ImVec2 GetItemImageSizeById(uint32_t itemId) {
    float defaultImageSize = 32.0f;
    if ((itemId >= ITEM_SONG_SONATA && itemId <= ITEM_SONG_SUN) || itemId == ITEM_SONG_LULLABY_INTRO) {
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
                                    .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                });
        UIWidgets::Tooltip("Enables the Time Split system, splits will not occur with this unchecked.");

        ImGui::TableNextColumn();
        if (UIWidgets::CVarCheckbox("Show Headers", "gSettings.TimeSplits.ShowHeaders",
                                    {
                                        .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                    })) {
            UpdateSplitSettings(SPLIT_HEADERS);
        };
        UIWidgets::Tooltip("Shows the column names in the Split List.");

        ImGui::TableNextColumn();
        if (UIWidgets::CVarCheckbox("Hide Background", "gSettings.TimeSplits.Opacity",
                                    {
                                        .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                    })) {
            UpdateSplitSettings(SPLIT_OPACITY);
        };
        UIWidgets::Tooltip("Hides the background of the Splits List.\n"
                           "Note: The background will display if the window extrudes from the main game window.");

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Highlight Active Split", "gSettings.TimeSplits.Highlight",
                                {
                                    .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                });
        UIWidgets::Tooltip("Highlights the row with the current Active Split.");

        ImGui::TableNextColumn();
        UIWidgets::CVarCheckbox("Follow Active Split", "gSettings.TimeSplits.Follow",
                                {
                                    .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                });
        UIWidgets::Tooltip("Forces the Split List window to keep the Active Split visible.\n"
                           "Note: This prevents user scrolling, disable to restore control.");

        ImGui::EndTable();
    }

    UIWidgets::CVarCheckbox("Compare Splits", "gSettings.TimeSplits.Compare",
                            {
                                .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                            });
    UIWidgets::Tooltip("Enables Split Comparisons between lists, this will integrate within the Split List.");

    if (CVarGetInteger("gSettings.TimeSplits.Compare", 0)) {
        ImGui::SameLine();
        UIWidgets::PushStyleCombobox(UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)));
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
                                   .labelPosition = UIWidgets::LabelPosition::None,
                                   .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                   .placeholder = "Enter new list name",
                               });

        ImGui::TableNextColumn();
        if (UIWidgets::Button("Create List", {
                                                 .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                             })) {
            SplitSaveFileAction(SPLIT_SAVE, listInputName);
            SplitSaveFileAction(SPLIT_RETRIEVE, "");
        }

        ImGui::TableNextColumn();
        UIWidgets::PushStyleCombobox(UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)));
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
                                                 .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                             })) {
            if (savedLists[0] != "Create a List First") {
                SplitSaveFileAction(SPLIT_SAVE, savedLists[selectedIndex]);
            }
        }
        ImGui::SameLine();
        if (UIWidgets::Button("Load Splits", {
                                                 .size = { (ImGui::GetContentRegionAvail().x), 0 },
                                                 .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                             })) {
            if (savedLists[0] != "Create a List First") {
                SplitSaveFileAction(SPLIT_LOAD, savedLists[selectedIndex]);
            }
        }

        ImGui::TableNextColumn();
        if (UIWidgets::Button("New Attempt", {
                                                 .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
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
        if (UIWidgets::Button("Update Splits",
                              {
                                  .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                              })) {
            UpdateSplitBests();
        }

        ImGui::EndTable();
    }
}

void DrawEntranceList() {
    UIWidgets::PushStyleCombobox(UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)));
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

                if (ImGui::ImageButton(std::to_string(sceneObjectList[i].splitId).c_str(),
                                       std::dynamic_pointer_cast<Fast::Fast3dGui>(
                                           Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                                           ->GetTextureByName(gPauseUnusedCursorTex),
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
            if (ImGui::ImageButton(
                    std::to_string(splitObjectList[i].splitId).c_str(),
                    std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                        ->GetTextureByName(GetItemImageById(splitObjectList[i].splitId)),
                    GetItemImageSizeById(splitObjectList[i].splitId) * 1.5f, ImVec2(0, 0), ImVec2(1, 1),
                    ImVec4(0, 0, 0, 0), Ship_GetItemColorTint(splitObjectList[i].splitId))) {
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

    UIWidgets::PushStyleTabs(UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)));
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
                                                 .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                             });
                ImGui::EndDisabled();
                ImGui::BeginChild("Preview List");
                for (size_t i = 0; i < splitList.size(); i++) {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetContentRegionAvail().x - 50.0f) * 0.5f));
                    ImGui::PushID(i);
                    SplitsPushImageButtonStyle();
                    if (ImGui::ImageButton(
                            std::to_string(i).c_str(),
                            std::dynamic_pointer_cast<Fast::Fast3dGui>(
                                Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                                ->GetTextureByName(splitList[i].splitType == SPLIT_TYPE_NORMAL
                                                       ? GetItemImageById(splitList[i].splitId)
                                                       : gPauseUnusedCursorTex),
                            splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemImageSizeById(splitList[i].splitId)
                                                                        : ImVec2(32.0f, 32.0f),
                            ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                            splitList[i].splitType == SPLIT_TYPE_NORMAL ? Ship_GetItemColorTint(splitList[i].splitId)
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
                if (UIWidgets::Button("Inventory",
                                      {
                                          .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                      })) {
                    range = GetIndexRange((uint32_t)ITEM_OCARINA_OF_TIME, (uint32_t)ITEM_BOTTLE);
                    listName = "Inventory";
                    listColumns = 6;
                }
                if (UIWidgets::Button("Masks",
                                      {
                                          .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                      })) {
                    range = GetIndexRange((uint32_t)ITEM_MASK_POSTMAN, (uint32_t)ITEM_MASK_FIERCE_DEITY);
                    listName = "Masks";
                    listColumns = 6;
                }
                if (UIWidgets::Button("Songs",
                                      {
                                          .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                      })) {
                    range = GetIndexRange((uint32_t)ITEM_SONG_TIME, (uint32_t)ITEM_SONG_OATH);
                    listName = "Songs";
                    listColumns = 5;
                }
                if (UIWidgets::Button("Quest",
                                      {
                                          .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                      })) {
                    range = GetIndexRange((uint32_t)ITEM_REMAINS_ODOLWA, (uint32_t)ITEM_BOMBERS_NOTEBOOK);
                    listName = "Quest";
                    listColumns = 4;
                }
                if (UIWidgets::Button("Bosses",
                                      {
                                          .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                      })) {
                    range = GetIndexRange((uint32_t)SPLIT_KILLED_ODOLWA, (uint32_t)SPLIT_KILLED_MAJORA);
                    listName = "Bosses";
                    listColumns = 1;
                }
                if (UIWidgets::Button("Entrances",
                                      {
                                          .color = UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5)),
                                      })) {
                    listName = "Entrances";
                }
                ImGui::TableNextColumn();
                if (listName != "Entrances") {
                    DrawItemList(listName, range, listColumns);
                    if (listName == "Quest") {
                        DrawItemList("Quest II",
                                     GetIndexRange((uint32_t)SPLIT_SINGLE_MAGIC, (uint32_t)SPLIT_DOUBLE_DEFENSE), 3);
                    }
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
