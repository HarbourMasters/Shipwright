#include "randomizer.h"
#include "json.hpp"
#include <fstream>
#include <variables.h>
#include <macros.h>
#include <objects/gameplay_keep/gameplay_keep.h>
#include <functions.h>
#include <Cvar.h>
#include <textures/icon_item_static/icon_item_static.h>
#include <textures/icon_item_24_static/icon_item_24_static.h>
#include <GameSettings.h>

using json = nlohmann::json;

std::unordered_map<std::string, Sprite> gSeedTextures;

Randomizer::Randomizer() {
    // this isn't as clean as i'd like it to be but it's working
    Sprite dekuStickSprite = { gDekuStickIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Deku Stick"] = dekuStickSprite;
    Sprite dekuNutSprite = { gDekuNutIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Deku Nut"] = dekuNutSprite;
    Sprite bowSprite = { gFairyBowIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Bow"] = bowSprite; 
    Sprite slingshotSprite = { gFairySlingshotIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Slingshot"] = slingshotSprite; 
    Sprite fairyOcarinaSprite = { gFairyOcarinaIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Fairy Ocarina"] = fairyOcarinaSprite;
    Sprite bombchuSprite = { gBombchuIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Bombchu"] = bombchuSprite;
    Sprite longshotSprite = { gLongshotIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Longshot"] = longshotSprite;
    Sprite boomerangSprite = { gBoomerangIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Boomerang"] = boomerangSprite;
    Sprite lensOfTruthSprite = { gLensofTruthIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Lens of Truth"] = lensOfTruthSprite;
    Sprite magicBeansSprite = { gMagicBeansIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Beans"] = magicBeansSprite; 
    Sprite megatonHammerSprite = { gMegatonHammerIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Megaton Hammer"] = megatonHammerSprite;
    Sprite fishSprite = { gFishIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Bottled Fish"] = fishSprite;
    Sprite milkSprite = { gMilkFullIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Bottled Milk"] = milkSprite;
    Sprite maskOfTruthSprite = { gMaskofTruthIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Mask of Truth"] = maskOfTruthSprite;
    Sprite soldOutSprite = { gSoldOutIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["SOLD OUT"] = soldOutSprite;
    Sprite cuccoSprite = { gCuccoIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Cucco"] = cuccoSprite;
    Sprite oddMushroomSprite = { gOddMushroomIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Mushroom"] = oddMushroomSprite;
    Sprite sawSprite = { gPoachersSawIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Saw"] = sawSprite;
    Sprite frogSprite = { gEyeBallFrogIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Frog"] = frogSprite;
    Sprite masterSwordSprite = { gMasterSwordIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Master Sword"] = masterSwordSprite;
    Sprite mirrorShieldSprite = { gMirrorShieldIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Mirror Shield"] = mirrorShieldSprite;
    Sprite kokiriTunicSprite = { gKokiriTunicIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Kokiri Tunic"] = kokiriTunicSprite;
    Sprite hoverBootsSprite = { gHoverBootsIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Hover Boots"] = hoverBootsSprite;
    Sprite silverGauntletsSprite = { gSilverGauntletsIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Silver Gauntlets"] = silverGauntletsSprite;
    Sprite goldenScaleSprite = { gGoldenScaleIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Gold Scale"] = goldenScaleSprite;
    Sprite stoneOfAgonySprite = { gStoneOfAgonyIconTex, 24, 24, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Stone of Agony"] = stoneOfAgonySprite;
    Sprite skullTokenSprite = { gGoldSkulltulaIconTex, 24, 24, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Skull Token"] = skullTokenSprite;
    Sprite heartContainerSprite = { gHeartContainerIconTex, 24, 24, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Heart Container"] = heartContainerSprite;
    Sprite bossKeySprite = { gBossKeyIconTex, 24, 24, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Boss Key"] = bossKeySprite;
    Sprite compassSprite = { gCompassIconTex, 24, 24, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Compass"] = compassSprite;
    Sprite mapSprite = { gDungeonMapIconTex, 24, 24, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Map"] = mapSprite;
    Sprite bigMagicSprite = { gBigMagicJarIconTex, 24, 24, G_IM_FMT_RGBA, G_IM_SIZ_32b };
    gSeedTextures["Big Magic"] = bigMagicSprite;
}

Randomizer::~Randomizer() { 
    this->itemLocations.clear();
}

std::unordered_map<std::string, RandomizerCheck> SpoilerfileCheckNameToEnum = {
    {"Links Pocket", RC_LINKS_POCKET},
    {"Link's Pocket", RC_LINKS_POCKET },
    {"Queen Gohma", RC_QUEEN_GOHMA},
    {"King Dodongo", RC_KING_DODONGO},
    {"Barinade", RC_BARINADE},
    {"Phantom Ganon", RC_PHANTOM_GANON},
    {"Volvagia", RC_VOLVAGIA},
    {"Morpha", RC_MORPHA},
    {"Bongo Bongo", RC_BONGO_BONGO},
    {"Twinrova", RC_TWINROVA},
    {"Ganon", RC_GANON},
    {"Gift from Sages", RC_GIFT_FROM_SAGES},
    {"Song from Impa", RC_SONG_FROM_IMPA},
    {"Song from Malon", RC_SONG_FROM_MALON},
    {"Song from Saria", RC_SONG_FROM_SARIA},
    {"Song from Royal Familys Tomb", RC_SONG_FROM_ROYAL_FAMILYS_TOMB},
    {"Song from Ocarina of Time", RC_SONG_FROM_OCARINA_OF_TIME},
    {"Song from Windmill", RC_SONG_FROM_WINDMILL},
    {"Sheik in Forest", RC_SHEIK_IN_FOREST},
    {"Sheik in Crater", RC_SHEIK_IN_CRATER},
    {"Sheik in Ice Cavern", RC_SHEIK_IN_ICE_CAVERN},
    {"Sheik at Colossus", RC_SHEIK_AT_COLOSSUS},
    {"Sheik in Kakariko", RC_SHEIK_IN_KAKARIKO},
    {"Sheik at Temple", RC_SHEIK_AT_TEMPLE},
    {"KF Midos Top Left Chest", RC_KF_MIDOS_TOP_LEFT_CHEST},
    {"KF Midos Top Right Chest", RC_KF_MIDOS_TOP_RIGHT_CHEST},
    {"KF Midos Bottom Left Chest", RC_KF_MIDOS_BOTTOM_LEFT_CHEST},
    {"KF Midos Bottom Right Chest", RC_KF_MIDOS_BOTTOM_RIGHT_CHEST},
    {"KF Kokiri Sword Chest", RC_KF_KOKIRI_SWORD_CHEST},
    {"KF Storms Grotto Chest", RC_KF_STORMS_GROTTO_CHEST},
    {"KF Links House Cow", RC_KF_LINKS_HOUSE_COW},
    {"KF GS Know It All House", RC_KF_GS_KNOW_IT_ALL_HOUSE},
    {"KF GS Bean Patch", RC_KF_GS_BEAN_PATCH},
    {"KF GS House of Twins", RC_KF_GS_HOUSE_OF_TWINS},
    {"KF Shop Item 1", RC_KF_SHOP_ITEM_1},
    {"KF Shop Item 2", RC_KF_SHOP_ITEM_2},
    {"KF Shop Item 3", RC_KF_SHOP_ITEM_3},
    {"KF Shop Item 4", RC_KF_SHOP_ITEM_4},
    {"KF Shop Item 5", RC_KF_SHOP_ITEM_5},
    {"KF Shop Item 6", RC_KF_SHOP_ITEM_6},
    {"KF Shop Item 7", RC_KF_SHOP_ITEM_7},
    {"KF Shop Item 8", RC_KF_SHOP_ITEM_8},
    {"LW Gift from Saria", RC_LW_GIFT_FROM_SARIA},
    {"LW Ocarina Memory Game", RC_LW_OCARINA_MEMORY_GAME},
    {"LW Target in Woods", RC_LW_TARGET_IN_WOODS},
    {"LW Near Shortcuts Grotto Chest", RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST},
    {"Deku Theater Skull Mask", RC_DEKU_THEATER_SKULL_MASK},
    {"Deku Theater Mask of Truth", RC_DEKU_THEATER_MASK_OF_TRUTH},
    {"LW Skull Kid", RC_LW_SKULL_KID},
    {"LW Deku Scrub Near Bridge", RC_LW_DEKU_SCRUB_NEAR_BRIDGE},
    {"LW Deku Scrub Near Deku Theater Left", RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT},
    {"LW Deku Scrub Near Deku Theater Right", RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT},
    {"LW Deku Scrub Grotto Front", RC_LW_DEKU_SCRUB_GROTTO_FRONT},
    {"LW Deku Scrub Grotto Rear", RC_LW_DEKU_SCRUB_GROTTO_REAR},
    {"LW GS Bean Patch Near Bridge", RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE},
    {"LW GS Bean Patch Near Theater", RC_LW_GS_BEAN_PATCH_NEAR_THEATER},
    {"LW GS Above Theater", RC_LW_GS_ABOVE_THEATER},
    {"SFM Wolfos Grotto Chest", RC_SFM_WOLFOS_GROTTO_CHEST},
    {"SFM Deku Scrub Grotto Front", RC_SFM_DEKU_SCRUB_GROTTO_FRONT},
    {"SFM Deku Scrub Grotto Rear", RC_SFM_DEKU_SCRUB_GROTTO_REAR},
    {"SFM GS", RC_SFM_GS},
    {"HF Ocarina of Time Item", RC_HF_OCARINA_OF_TIME_ITEM},
    {"HF Near Market Grotto Chest", RC_HF_NEAR_MARKET_GROTTO_CHEST},
    {"HF Tektite Grotto Freestanding PoH", RC_HF_TEKTITE_GROTTO_FREESTANDING_POH},
    {"HF Southeast Grotto Chest", RC_HF_SOUTHEAST_GROTTO_CHEST},
    {"HF Open Grotto Chest", RC_HF_OPEN_GROTTO_CHEST},
    {"HF Deku Scrub Grotto", RC_HF_DEKU_SCRUB_GROTTO},
    {"HF Cow Grotto Cow", RC_HF_COW_GROTTO_COW},
    {"HF GS Cow Grotto", RC_HF_GS_COW_GROTTO},
    {"HF GS Near Kak Grotto", RC_HF_GS_NEAR_KAK_GROTTO},
    {"Market Shooting Gallery Reward", RC_MARKET_SHOOTING_GALLERY_REWARD},
    {"Market Bombchu Bowling First Prize", RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE},
    {"Market Bombchu Bowling Second Prize", RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE},
    {"Market Bombchu Bowling Bombchus", RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS},
    {"Market Lost Dog", RC_MARKET_LOST_DOG},
    {"Market Treasure Chest Game Reward", RC_MARKET_TREASURE_CHEST_GAME_REWARD},
    {"Market 10 Big Poes", RC_MARKET_10_BIG_POES},
    {"Market GS Guard House", RC_MARKET_GS_GUARD_HOUSE},
    {"Market Bazaar Item 1", RC_MARKET_BAZAAR_ITEM_1},
    {"Market Bazaar Item 2", RC_MARKET_BAZAAR_ITEM_2},
    {"Market Bazaar Item 3", RC_MARKET_BAZAAR_ITEM_3},
    {"Market Bazaar Item 4", RC_MARKET_BAZAAR_ITEM_4},
    {"Market Bazaar Item 5", RC_MARKET_BAZAAR_ITEM_5},
    {"Market Bazaar Item 6", RC_MARKET_BAZAAR_ITEM_6},
    {"Market Bazaar Item 7", RC_MARKET_BAZAAR_ITEM_7},
    {"Market Bazaar Item 8", RC_MARKET_BAZAAR_ITEM_8},
    {"Market Potion Shop Item 1", RC_MARKET_POTION_SHOP_ITEM_1},
    {"Market Potion Shop Item 2", RC_MARKET_POTION_SHOP_ITEM_2},
    {"Market Potion Shop Item 3", RC_MARKET_POTION_SHOP_ITEM_3},
    {"Market Potion Shop Item 4", RC_MARKET_POTION_SHOP_ITEM_4},
    {"Market Potion Shop Item 5", RC_MARKET_POTION_SHOP_ITEM_5},
    {"Market Potion Shop Item 6", RC_MARKET_POTION_SHOP_ITEM_6},
    {"Market Potion Shop Item 7", RC_MARKET_POTION_SHOP_ITEM_7},
    {"Market Potion Shop Item 8", RC_MARKET_POTION_SHOP_ITEM_8},
    {"Market Bombchu Shop Item 1", RC_MARKET_BOMBCHU_SHOP_ITEM_1},
    {"Market Bombchu Shop Item 2", RC_MARKET_BOMBCHU_SHOP_ITEM_2},
    {"Market Bombchu Shop Item 3", RC_MARKET_BOMBCHU_SHOP_ITEM_3},
    {"Market Bombchu Shop Item 4", RC_MARKET_BOMBCHU_SHOP_ITEM_4},
    {"Market Bombchu Shop Item 5", RC_MARKET_BOMBCHU_SHOP_ITEM_5},
    {"Market Bombchu Shop Item 6", RC_MARKET_BOMBCHU_SHOP_ITEM_6},
    {"Market Bombchu Shop Item 7", RC_MARKET_BOMBCHU_SHOP_ITEM_7},
    {"Market Bombchu Shop Item 8", RC_MARKET_BOMBCHU_SHOP_ITEM_8},
    {"ToT Light Arrows Cutscene", RC_TOT_LIGHT_ARROWS_CUTSCENE},
    {"HC Malon Egg", RC_HC_MALON_EGG},
    {"HC Zeldas Letter", RC_HC_ZELDAS_LETTER},
    {"HC Great Fairy Reward", RC_HC_GREAT_FAIRY_REWARD},
    {"HC GS Tree", RC_HC_GS_TREE},
    {"HC GS Storms Grotto", RC_HC_GS_STORMS_GROTTO},
    {"LLR Talons Chickens", RC_LLR_TALONS_CHICKENS},
    {"LLR Freestanding PoH", RC_LLR_FREESTANDING_POH},
    {"LLR Deku Scrub Grotto Left", RC_LLR_DEKU_SCRUB_GROTTO_LEFT},
    {"LLR Deku Scrub Grotto Center", RC_LLR_DEKU_SCRUB_GROTTO_CENTER},
    {"LLR Deku Scrub Grotto Right", RC_LLR_DEKU_SCRUB_GROTTO_RIGHT},
    {"LLR Stables Left Cow", RC_LLR_STABLES_LEFT_COW},
    {"LLR Stables Right Cow", RC_LLR_STABLES_RIGHT_COW},
    {"LLR Tower Left Cow", RC_LLR_TOWER_LEFT_COW},
    {"LLR Tower Right Cow", RC_LLR_TOWER_RIGHT_COW},
    {"LLR GS House Window", RC_LLR_GS_HOUSE_WINDOW},
    {"LLR GS Tree", RC_LLR_GS_TREE},
    {"LLR GS Rain Shed", RC_LLR_GS_RAIN_SHED},
    {"LLR GS Back Wall", RC_LLR_GS_BACK_WALL},
    {"Kak Anju as Child", RC_KAK_ANJU_AS_CHILD},
    {"Kak Anju as Adult", RC_KAK_ANJU_AS_ADULT},
    {"Kak Impas House Freestanding PoH", RC_KAK_IMPAS_HOUSE_FREESTANDING_POH},
    {"Kak Windmill Freestanding PoH", RC_KAK_WINDMILL_FREESTANDING_POH},
    {"Kak Man on Roof", RC_KAK_MAN_ON_ROOF},
    {"Kak Open Grotto Chest", RC_KAK_OPEN_GROTTO_CHEST},
    {"Kak Redead Grotto Chest", RC_KAK_REDEAD_GROTTO_CHEST},
    {"Kak Shooting Gallery Reward", RC_KAK_SHOOTING_GALLERY_REWARD},
    {"Kak 10 Gold Skulltula Reward", RC_KAK_10_GOLD_SKULLTULA_REWARD},
    {"Kak 20 Gold Skulltula Reward", RC_KAK_20_GOLD_SKULLTULA_REWARD},
    {"Kak 30 Gold Skulltula Reward", RC_KAK_30_GOLD_SKULLTULA_REWARD},
    {"Kak 40 Gold Skulltula Reward", RC_KAK_40_GOLD_SKULLTULA_REWARD},
    {"Kak 50 Gold Skulltula Reward", RC_KAK_50_GOLD_SKULLTULA_REWARD},
    {"Kak Impas House Cow", RC_KAK_IMPAS_HOUSE_COW},
    {"Kak GS Tree", RC_KAK_GS_TREE},
    {"Kak GS Guards House", RC_KAK_GS_GUARDS_HOUSE},
    {"Kak GS Watchtower", RC_KAK_GS_WATCHTOWER},
    {"Kak GS Skulltula House", RC_KAK_GS_SKULLTULA_HOUSE},
    {"Kak GS House Under Construction", RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION},
    {"Kak GS Above Impas House", RC_KAK_GS_ABOVE_IMPAS_HOUSE},
    {"Kak Bazaar Item 1", RC_KAK_BAZAAR_ITEM_1},
    {"Kak Bazaar Item 2", RC_KAK_BAZAAR_ITEM_2},
    {"Kak Bazaar Item 3", RC_KAK_BAZAAR_ITEM_3},
    {"Kak Bazaar Item 4", RC_KAK_BAZAAR_ITEM_4},
    {"Kak Bazaar Item 5", RC_KAK_BAZAAR_ITEM_5},
    {"Kak Bazaar Item 6", RC_KAK_BAZAAR_ITEM_6},
    {"Kak Bazaar Item 7", RC_KAK_BAZAAR_ITEM_7},
    {"Kak Bazaar Item 8", RC_KAK_BAZAAR_ITEM_8},
    {"Kak Potion Shop Item 1", RC_KAK_POTION_SHOP_ITEM_1},
    {"Kak Potion Shop Item 2", RC_KAK_POTION_SHOP_ITEM_2},
    {"Kak Potion Shop Item 3", RC_KAK_POTION_SHOP_ITEM_3},
    {"Kak Potion Shop Item 4", RC_KAK_POTION_SHOP_ITEM_4},
    {"Kak Potion Shop Item 5", RC_KAK_POTION_SHOP_ITEM_5},
    {"Kak Potion Shop Item 6", RC_KAK_POTION_SHOP_ITEM_6},
    {"Kak Potion Shop Item 7", RC_KAK_POTION_SHOP_ITEM_7},
    {"Kak Potion Shop Item 8", RC_KAK_POTION_SHOP_ITEM_8},
    {"Graveyard Shield Grave Chest", RC_GRAVEYARD_SHIELD_GRAVE_CHEST},
    {"Graveyard Heart Piece Grave Chest", RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST},
    {"Graveyard Royal Familys Tomb Chest", RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST},
    {"Graveyard Freestanding PoH", RC_GRAVEYARD_FREESTANDING_POH},
    {"Graveyard Dampe Gravedigging Tour", RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR},
    {"Graveyard Hookshot Chest", RC_GRAVEYARD_HOOKSHOT_CHEST},
    {"Graveyard Dampe Race Freestanding PoH", RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH},
    {"Graveyard GS Bean Patch", RC_GRAVEYARD_GS_BEAN_PATCH},
    {"Graveyard GS Wall", RC_GRAVEYARD_GS_WALL},
    {"DMT Freestanding PoH", RC_DMT_FREESTANDING_POH},
    {"DMT Chest", RC_DMT_CHEST},
    {"DMT Storms Grotto Chest", RC_DMT_STORMS_GROTTO_CHEST},
    {"DMT Great Fairy Reward", RC_DMT_GREAT_FAIRY_REWARD},
    {"DMT Biggoron", RC_DMT_BIGGORON},
    {"DMT Cow Grotto Cow", RC_DMT_COW_GROTTO_COW},
    {"DMT GS Near Kak", RC_DMT_GS_NEAR_KAK},
    {"DMT GS Bean Patch", RC_DMT_GS_BEAN_PATCH},
    {"DMT GS Above Dodongos Cavern", RC_DMT_GS_ABOVE_DODONGOS_CAVERN},
    {"DMT GS Falling Rocks Path", RC_DMT_GS_FALLING_ROCKS_PATH},
    {"GC Darunias Joy", RC_GC_DARUNIAS_JOY},
    {"GC Pot Freestanding PoH", RC_GC_POT_FREESTANDING_POH},
    {"GC Rolling Goron as Child", RC_GC_ROLLING_GORON_AS_CHILD},
    {"GC Rolling Goron as Adult", RC_GC_ROLLING_GORON_AS_ADULT},
    {"GC Medigoron", RC_GC_MEDIGORON},
    {"GC Maze Left Chest", RC_GC_MAZE_LEFT_CHEST},
    {"GC Maze Right Chest", RC_GC_MAZE_RIGHT_CHEST},
    {"GC Maze Center Chest", RC_GC_MAZE_CENTER_CHEST},
    {"GC Deku Scrub Grotto Left", RC_GC_DEKU_SCRUB_GROTTO_LEFT},
    {"GC Deku Scrub Grotto Center", RC_GC_DEKU_SCRUB_GROTTO_CENTER},
    {"GC Deku Scrub Grotto Right", RC_GC_DEKU_SCRUB_GROTTO_RIGHT},
    {"GC GS Center Platform", RC_GC_GS_CENTER_PLATFORM},
    {"GC GS Boulder Maze", RC_GC_GS_BOULDER_MAZE},
    {"GC Shop Item 1", RC_GC_SHOP_ITEM_1},
    {"GC Shop Item 2", RC_GC_SHOP_ITEM_2},
    {"GC Shop Item 3", RC_GC_SHOP_ITEM_3},
    {"GC Shop Item 4", RC_GC_SHOP_ITEM_4},
    {"GC Shop Item 5", RC_GC_SHOP_ITEM_5},
    {"GC Shop Item 6", RC_GC_SHOP_ITEM_6},
    {"GC Shop Item 7", RC_GC_SHOP_ITEM_7},
    {"GC Shop Item 8", RC_GC_SHOP_ITEM_8},
    {"DMC Volcano Freestanding PoH", RC_DMC_VOLCANO_FREESTANDING_POH},
    {"DMC Wall Freestanding PoH", RC_DMC_WALL_FREESTANDING_POH},
    {"DMC Upper Grotto Chest", RC_DMC_UPPER_GROTTO_CHEST},
    {"DMC Great Fairy Reward", RC_DMC_GREAT_FAIRY_REWARD},
    {"DMC Deku Scrub", RC_DMC_DEKU_SCRUB},
    {"DMC Deku Scrub Grotto Left", RC_DMC_DEKU_SCRUB_GROTTO_LEFT},
    {"DMC Deku Scrub Grotto Center", RC_DMC_DEKU_SCRUB_GROTTO_CENTER},
    {"DMC Deku Scrub Grotto Right", RC_DMC_DEKU_SCRUB_GROTTO_RIGHT},
    {"DMC GS Crate", RC_DMC_GS_CRATE},
    {"DMC GS Bean Patch", RC_DMC_GS_BEAN_PATCH},
    {"ZR Magic Bean Salesman", RC_ZR_MAGIC_BEAN_SALESMAN},
    {"ZR Open Grotto Chest", RC_ZR_OPEN_GROTTO_CHEST},
    {"ZR Frogs Zeldas Lullaby", RC_ZR_FROGS_ZELDAS_LULLABY},
    {"ZR Frogs Eponas Song", RC_ZR_FROGS_EPONAS_SONG},
    {"ZR Frogs Sarias Song", RC_ZR_FROGS_SARIAS_SONG},
    {"ZR Frogs Suns Song", RC_ZR_FROGS_SUNS_SONG},
    {"ZR Frogs Song of Time", RC_ZR_FROGS_SONG_OF_TIME},
    {"ZR Frogs in the Rain", RC_ZR_FROGS_IN_THE_RAIN},
    {"ZR Frogs Ocarina Game", RC_ZR_FROGS_OCARINA_GAME},
    {"ZR Near Open Grotto Freestanding PoH", RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH},
    {"ZR Near Domain Freestanding PoH", RC_ZR_NEAR_DOMAIN_FREESTANDING_POH},
    {"ZR Deku Scrub Grotto Front", RC_ZR_DEKU_SCRUB_GROTTO_FRONT},
    {"ZR Deku Scrub Grotto Rear", RC_ZR_DEKU_SCRUB_GROTTO_REAR},
    {"ZR GS Tree", RC_ZR_GS_TREE},
    {"ZR GS Ladder", RC_ZR_GS_LADDER},
    {"ZR GS Near Raised Grottos", RC_ZR_GS_NEAR_RAISED_GROTTOS},
    {"ZR GS Above Bridge", RC_ZR_GS_ABOVE_BRIDGE},
    {"ZD Diving Minigame", RC_ZD_DIVING_MINIGAME},
    {"ZD Chest", RC_ZD_CHEST},
    {"ZD King Zora Thawed", RC_ZD_KING_ZORA_THAWED},
    {"ZD GS Frozen Waterfall", RC_ZD_GS_FROZEN_WATERFALL},
    {"ZD Shop Item 1", RC_ZD_SHOP_ITEM_1},
    {"ZD Shop Item 2", RC_ZD_SHOP_ITEM_2},
    {"ZD Shop Item 3", RC_ZD_SHOP_ITEM_3},
    {"ZD Shop Item 4", RC_ZD_SHOP_ITEM_4},
    {"ZD Shop Item 5", RC_ZD_SHOP_ITEM_5},
    {"ZD Shop Item 6", RC_ZD_SHOP_ITEM_6},
    {"ZD Shop Item 7", RC_ZD_SHOP_ITEM_7},
    {"ZD Shop Item 8", RC_ZD_SHOP_ITEM_8},
    {"ZF Great Fairy Reward", RC_ZF_GREAT_FAIRY_REWARD},
    {"ZF Iceberg Freestanding PoH", RC_ZF_ICEBERC_FREESTANDING_POH},
    {"ZF Bottom Freestanding PoH", RC_ZF_BOTTOM_FREESTANDING_POH},
    {"ZF GS Above the Log", RC_ZF_GS_ABOVE_THE_LOG},
    {"ZF GS Tree", RC_ZF_GS_TREE},
    {"ZF GS Hidden Cave", RC_ZF_GS_HIDDEN_CAVE},
    {"LH Underwater Item", RC_LH_UNDERWATER_ITEM},
    {"LH Child Fishing", RC_LH_CHILD_FISHING},
    {"LH Adult Fishing", RC_LH_ADULT_FISHING},
    {"LH Lab Dive", RC_LH_LAB_DIVE},
    {"LH Freestanding PoH", RC_LH_FREESTANDING_POH},
    {"LH Sun", RC_LH_SUN},
    {"LH Deku Scrub Grotto Left", RC_LH_DEKU_SCRUB_GROTTO_LEFT},
    {"LH Deku Scrub Grotto Center", RC_LH_DEKU_SCRUB_GROTTO_CENTER},
    {"LH Deku Scrub Grotto Right", RC_LH_DEKU_SCRUB_GROTTO_RIGHT},
    {"LH GS Bean Patch", RC_LH_GS_BEAN_PATCH},
    {"LH GS Lab Wall", RC_LH_GS_LAB_WALL},
    {"LH GS Small Island", RC_LH_GS_SMALL_ISLAND},
    {"LH GS Lab Crate", RC_LH_GS_LAB_CRATE},
    {"LH GS Tree", RC_LH_GS_TREE},
    {"GV Crate Freestanding PoH", RC_GV_CRATE_FREESTANDING_POH},
    {"GV Waterfall Freestanding PoH", RC_GV_WATERFALL_FREESTANDING_POH},
    {"GV Chest", RC_GV_CHEST},
    {"GV Deku Scrub Grotto Front", RC_GV_DEKU_SCRUB_GROTTO_FRONT},
    {"GV Deku Scrub Grotto Rear", RC_GV_DEKU_SCRUB_GROTTO_REAR},
    {"GV Cow", RC_GV_COW},
    {"GV GS Small Bridge", RC_GV_GS_SMALL_BRIDGE},
    {"GV GS Bean Patch", RC_GV_GS_BEAN_PATCH},
    {"GV GS Behind Tent", RC_GV_GS_BEHIND_TENT},
    {"GV GS Pillar", RC_GV_GS_PILLAR},
    {"GF Chest", RC_GF_CHEST},
    {"GF HBA 1000 Points", RC_GF_HBA_1000_POINTS},
    {"GF HBA 1500 Points", RC_GF_HBA_1500_POINTS},
    {"GF GS Top Floor", RC_GF_GS_TOP_FLOOR},
    {"GF GS Archery Range", RC_GF_GS_ARCHERY_RANGE},
    {"Hideout Jail Guard (1 Torch)", RC_HIDEOUT_JAIL_GUARD_1_TORCH},
    {"Hideout Jail Guard (2 Torches)", RC_HIDEOUT_JAIL_GUARD_2_TORCHES},
    {"Hideout Jail Guard (3 Torches)", RC_HIDEOUT_JAIL_GUARD_3_TORCHES},
    {"Hideout Jail Guard (4 Torches)", RC_HIDEOUT_JAIL_GUARD_4_TORCHES},
    {"Hideout Gerudo Membership Card", RC_HIDEOUT_GERUDO_MEMBERSHIP_CARD},
    {"Wasteland Bombchu Salesman", RC_WASTELAND_BOMBCHU_SALESMAN},
    {"Wasteland Chest", RC_WASTELAND_CHEST},
    {"Wasteland GS", RC_WASTELAND_GS},
    {"Colossus Great Fairy Reward", RC_COLOSSUS_GREAT_FAIRY_REWARD},
    {"Colossus Freestanding PoH", RC_COLOSSUS_FREESTANDING_POH},
    {"Colossus Deku Scrub Grotto Front", RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT},
    {"Colossus Deku Scrub Grotto Rear", RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR},
    {"Colossus GS Bean Patch", RC_COLOSSUS_GS_BEAN_PATCH},
    {"Colossus GS Tree", RC_COLOSSUS_GS_TREE},
    {"Colossus GS Hill", RC_COLOSSUS_GS_HILL},
    {"OGC Great Fairy Reward", RC_OGC_GREAT_FAIRY_REWARD},
    {"OGC GS", RC_OGC_GS},
    {"Deku Tree Map Chest", RC_DEKU_TREE_MAP_CHEST},
    {"Deku Tree Slingshot Room Side Chest", RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST},
    {"Deku Tree Slingshot Chest", RC_DEKU_TREE_SLINGSHOT_CHEST},
    {"Deku Tree Compass Chest", RC_DEKU_TREE_COMPASS_CHEST},
    {"Deku Tree Compass Room Side Chest", RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST},
    {"Deku Tree Basement Chest", RC_DEKU_TREE_BASEMENT_CHEST},
    {"Deku Tree GS Compass Room", RC_DEKU_TREE_GS_COMPASS_ROOM},
    {"Deku Tree GS Basement Vines", RC_DEKU_TREE_GS_BASEMENT_VINES},
    {"Deku Tree GS Basement Gate", RC_DEKU_TREE_GS_BASEMENT_GATE},
    {"Deku Tree GS Basement Back Room", RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM},
    {"Deku Tree MQ Map Chest", RC_DEKU_TREE_MQ_MAP_CHEST},
    {"Deku Tree MQ Slingshot Chest", RC_DEKU_TREE_MQ_SLINGSHOT_CHEST},
    {"Deku Tree MQ Slingshot Room Back Chest", RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST},
    {"Deku Tree MQ Compass Chest", RC_DEKU_TREE_MQ_COMPASS_CHEST},
    {"Deku Tree MQ Basement Chest", RC_DEKU_TREE_MQ_BASEMENT_CHEST},
    {"Deku Tree MQ Before Spinning Log Chest", RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST},
    {"Deku Tree MQ After Spinning Log Chest", RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST},
    {"Deku Tree MQ Deku Scrub", RC_DEKU_TREE_MQ_DEKU_SCRUB},
    {"Deku Tree MQ GS Lobby", RC_DEKU_TREE_MQ_GS_LOBBY},
    {"Deku Tree MQ GS Compass Room", RC_DEKU_TREE_MQ_GS_COMPASS_ROOM},
    {"Deku Tree MQ GS Basement Graves Room", RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM},
    {"Deku Tree MQ GS Basement Back Room", RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM},
    {"Deku Tree Queen Gohma Heart", RC_DEKU_TREE_QUEEN_GOHMA_HEART},
    {"Dodongos Cavern Map Chest", RC_DODONGOS_CAVERN_MAP_CHEST},
    {"Dodongos Cavern Compass Chest", RC_DODONGOS_CAVERN_COMPASS_CHEST},
    {"Dodongos Cavern Bomb Flower Platform Chest", RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST},
    {"Dodongos Cavern Bomb Bag Chest", RC_DODONGOS_CAVERN_BOMB_BAG_CHEST},
    {"Dodongos Cavern End of Bridge Chest", RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST},
    {"Dodongos Cavern Deku Scrub Side Room Near Dodongos", RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS},
    {"Dodongos Cavern Deku Scrub Lobby", RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY},
    {"Dodongos Cavern Deku Scrub Near Bomb Bag Left", RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT},
    {"Dodongos Cavern Deku Scrub Near Bomb Bag Right", RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT},
    {"Dodongos Cavern GS Side Room Near Lower Lizalfos", RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS},
    {"Dodongos Cavern GS Scarecrow", RC_DODONGOS_CAVERN_GS_SCARECROW},
    {"Dodongos Cavern GS Alcove Above Stairs", RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS},
    {"Dodongos Cavern GS Vines Above Stairs", RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS},
    {"Dodongos Cavern GS Back Room", RC_DODONGOS_CAVERN_GS_BACK_ROOM},
    {"Dodongos Cavern MQ Map Chest", RC_DODONGOS_CAVERN_MQ_MAP_CHEST},
    {"Dodongos Cavern MQ Bomb Bag Chest", RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST},
    {"Dodongos Cavern MQ Torch Puzzle Room Chest", RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST},
    {"Dodongos Cavern MQ Larvae Room Chest", RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST},
    {"Dodongos Cavern MQ Compass Chest", RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST},
    {"Dodongos Cavern MQ Under Grave Chest", RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST},
    {"Dodongos Cavern MQ Deku Scrub Lobby Front", RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT},
    {"Dodongos Cavern MQ Deku Scrub Lobby Rear", RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR},
    {"Dodongos Cavern MQ Deku Scrub Side Room Near Lower Lizalfos", RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS},
    {"Dodongos Cavern MQ Deku Scrub Staircase", RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE},
    {"Dodongos Cavern MQ GS Scrub Room", RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM},
    {"Dodongos Cavern MQ GS Larvae Room", RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM},
    {"Dodongos Cavern MQ GS Lizalfos Room", RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM},
    {"Dodongos Cavern MQ GS Song of Time Block Room", RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM},
    {"Dodongos Cavern MQ GS Back Area", RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA},
    {"Dodongos Cavern Boss Room Chest", RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST},
    {"Dodongos Cavern King Dodongo Heart", RC_DODONGOS_CAVERN_KING_DODONGO_HEART},
    {"Jabu Jabus Belly Boomerang Chest", RC_JABU_JABUS_BELLY_BOOMERANG_CHEST},
    {"Jabu Jabus Belly Map Chest", RC_JABU_JABUS_BELLY_MAP_CHEST},
    {"Jabu Jabus Belly Compass Chest", RC_JABU_JABUS_BELLY_COMPASS_CHEST},
    {"Jabu Jabus Belly Deku Scrub", RC_JABU_JABUS_BELLY_DEKU_SCRUB},
    {"Jabu Jabus Belly GS Water Switch Room", RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM},
    {"Jabu Jabus Belly GS Lobby Basement Lower", RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER},
    {"Jabu Jabus Belly GS Lobby Basement Upper", RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER},
    {"Jabu Jabus Belly GS Near Boss", RC_JABU_JABUS_BELLY_GS_NEAR_BOSS},
    {"Jabu Jabus Belly MQ Map Chest", RC_JABU_JABUS_BELLY_MQ_MAP_CHEST},
    {"Jabu Jabus Belly MQ First Room Side Chest", RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST},
    {"Jabu Jabus Belly MQ Second Room Lower Chest", RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST},
    {"Jabu Jabus Belly MQ Compass Chest", RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST},
    {"Jabu Jabus Belly MQ Basement Near Switches Chest", RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST},
    {"Jabu Jabus Belly MQ Basement Near Vines Chest", RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST},
    {"Jabu Jabus Belly MQ Boomerang Room Small Chest", RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST},
    {"Jabu Jabus Belly MQ Boomerang Chest", RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST},
    {"Jabu Jabus Belly MQ Falling Like Like Room Chest", RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST},
    {"Jabu Jabus Belly MQ Second Room Upper Chest", RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST},
    {"Jabu Jabus Belly MQ Near Boss Chest", RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST},
    {"Jabu Jabus Belly MQ Cow", RC_JABU_JABUS_BELLY_MQ_COW},
    {"Jabu Jabus Belly MQ GS Boomerang Chest Room", RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM},
    {"Jabu Jabus Belly MQ GS Tailpasaran Room", RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM},
    {"Jabu Jabus Belly MQ GS Invisible Enemies Room", RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM},
    {"Jabu Jabus Belly MQ GS Near Boss", RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS},
    {"Jabu Jabus Belly Barinade Heart", RC_JABU_JABUS_BELLY_BARINADE_HEART},
    {"Bottom of the Well Front Left Fake Wall Chest", RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST},
    {"Bottom of the Well Front Center Bombable Chest", RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST},
    {"Bottom of the Well Back Left Bombable Chest", RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST},
    {"Bottom of the Well Underwater Left Chest", RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST},
    {"Bottom of the Well Freestanding Key", RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY},
    {"Bottom of the Well Compass Chest", RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST},
    {"Bottom of the Well Center Skulltula Chest", RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST},
    {"Bottom of the Well Right Bottom Fake Wall Chest", RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST},
    {"Bottom of the Well Fire Keese Chest", RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST},
    {"Bottom of the Well Like Like Chest", RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST},
    {"Bottom of the Well Map Chest", RC_BOTTOM_OF_THE_WELL_MAP_CHEST},
    {"Bottom of the Well Underwater Front Chest", RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST},
    {"Bottom of the Well Invisible Chest", RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST},
    {"Bottom of the Well Lens of Truth Chest", RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST},
    {"Bottom of the Well GS West Inner Room", RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM},
    {"Bottom of the Well GS East Inner Room", RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM},
    {"Bottom of the Well GS Like Like Cage", RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE},
    {"Bottom of the Well MQ Map Chest", RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST},
    {"Bottom of the Well MQ East Inner Room Freestanding Key", RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY},
    {"Bottom of the Well MQ Compass Chest", RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST},
    {"Bottom of the Well MQ Dead Hand Freestanding Key", RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY},
    {"Bottom of the Well MQ Lens of Truth Chest", RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST},
    {"Bottom of the Well MQ GS Coffin Room", RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM},
    {"Bottom of the Well MQ GS West Inner Room", RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM},
    {"Bottom of the Well MQ GS Basement", RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT},
    {"Forest Temple First Room Chest", RC_FOREST_TEMPLE_FIRST_ROOM_CHEST},
    {"Forest Temple First Stalfos Chest", RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST},
    {"Forest Temple Raised Island Courtyard Chest", RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST},
    {"Forest Temple Map Chest", RC_FOREST_TEMPLE_MAP_CHEST},
    {"Forest Temple Well Chest", RC_FOREST_TEMPLE_WELL_CHEST},
    {"Forest Temple Eye Switch Chest", RC_FOREST_TEMPLE_EYE_SWITCH_CHEST},
    {"Forest Temple Boss Key Chest", RC_FOREST_TEMPLE_BOSS_KEY_CHEST},
    {"Forest Temple Floormaster Chest", RC_FOREST_TEMPLE_FLOORMASTER_CHEST},
    {"Forest Temple Red Poe Chest", RC_FOREST_TEMPLE_RED_POE_CHEST},
    {"Forest Temple Bow Chest", RC_FOREST_TEMPLE_BOW_CHEST},
    {"Forest Temple Blue Poe Chest", RC_FOREST_TEMPLE_BLUE_POE_CHEST},
    {"Forest Temple Falling Ceiling Room Chest", RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST},
    {"Forest Temple Basement Chest", RC_FOREST_TEMPLE_BASEMENT_CHEST},
    {"Forest Temple GS First Room", RC_FOREST_TEMPLE_GS_FIRST_ROOM},
    {"Forest Temple GS Lobby", RC_FOREST_TEMPLE_GS_LOBBY},
    {"Forest Temple GS Raised Island Courtyard", RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD},
    {"Forest Temple GS Level Island Courtyard", RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD},
    {"Forest Temple GS Basement", RC_FOREST_TEMPLE_GS_BASEMENT},
    {"Forest Temple MQ First Room Chest", RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST},
    {"Forest Temple MQ Wolfos Chest", RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST},
    {"Forest Temple MQ Well Chest", RC_FOREST_TEMPLE_MQ_WELL_CHEST},
    {"Forest Temple MQ Raised Island Courtyard Lower Chest", RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST},
    {"Forest Temple MQ Raised Island Courtyard Upper Chest", RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST},
    {"Forest Temple MQ Boss Key Chest", RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Forest Temple MQ Redead Chest", RC_FOREST_TEMPLE_MQ_REDEAD_CHEST},
    {"Forest Temple MQ Map Chest", RC_FOREST_TEMPLE_MQ_MAP_CHEST},
    {"Forest Temple MQ Bow Chest", RC_FOREST_TEMPLE_MQ_BOW_CHEST},
    {"Forest Temple MQ Compass Chest", RC_FOREST_TEMPLE_MQ_COMPASS_CHEST},
    {"Forest Temple MQ Falling Ceiling Room Chest", RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST},
    {"Forest Temple MQ Basement Chest", RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST},
    {"Forest Temple MQ GS First Hallway", RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY},
    {"Forest Temple MQ GS Raised Island Courtyard", RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD},
    {"Forest Temple MQ GS Level Island Courtyard", RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD},
    {"Forest Temple MQ GS Well", RC_FOREST_TEMPLE_MQ_GS_WELL},
    {"Forest Temple MQ GS Block Push Room", RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM},
    {"Forest Temple Phantom Ganon Heart", RC_FOREST_TEMPLE_PHANTOM_GANON_HEART},
    {"Fire Temple Near Boss Chest", RC_FIRE_TEMPLE_NEAR_BOSS_CHEST},
    {"Fire Temple Flare Dancer Chest", RC_FIRE_TEMPLE_FLARE_DANCER_CHEST},
    {"Fire Temple Boss Key Chest", RC_FIRE_TEMPLE_BOSS_KEY_CHEST},
    {"Fire Temple Big Lava Room Lower Open Door Chest", RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST},
    {"Fire Temple Big Lava Room Blocked Door Chest", RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST},
    {"Fire Temple Boulder Maze Lower Chest", RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST},
    {"Fire Temple Boulder Maze Side Room Chest", RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST},
    {"Fire Temple Map Chest", RC_FIRE_TEMPLE_MAP_CHEST},
    {"Fire Temple Boulder Maze Shortcut Chest", RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST},
    {"Fire Temple Boulder Maze Upper Chest", RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST},
    {"Fire Temple Scarecrow Chest", RC_FIRE_TEMPLE_SCARECROW_CHEST},
    {"Fire Temple Compass Chest", RC_FIRE_TEMPLE_COMPASS_CHEST},
    {"Fire Temple Megaton Hammer Chest", RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST},
    {"Fire Temple Highest Goron Chest", RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST},
    {"Fire Temple GS Boss Key Loop", RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP},
    {"Fire Temple GS Song of Time Room", RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM},
    {"Fire Temple GS Boulder Maze", RC_FIRE_TEMPLE_GS_BOULDER_MAZE},
    {"Fire Temple GS Scarecrow Climb", RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB},
    {"Fire Temple GS Scarecrow Top", RC_FIRE_TEMPLE_GS_SCARECROW_TOP},
    {"Fire Temple MQ Map Room Side Chest", RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST},
    {"Fire Temple MQ Megaton Hammer Chest", RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST},
    {"Fire Temple MQ Map Chest", RC_FIRE_TEMPLE_MQ_MAP_CHEST},
    {"Fire Temple MQ Near Boss Chest", RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST},
    {"Fire Temple MQ Big Lava Room Blocked Door Chest", RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST},
    {"Fire Temple MQ Boss Key Chest", RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Fire Temple MQ Lizalfos Maze Side Room Chest", RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST},
    {"Fire Temple MQ Compass Chest", RC_FIRE_TEMPLE_MQ_COMPASS_CHEST},
    {"Fire Temple MQ Lizalfos Maze Upper Chest", RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST},
    {"Fire Temple MQ Lizalfos Maze Lower Chest", RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST},
    {"Fire Temple MQ Freestanding Key", RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY},
    {"Fire Temple MQ Chest On Fire", RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE},
    {"Fire Temple MQ GS Big Lava Room Open Door", RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR},
    {"Fire Temple MQ GS Skull On Fire", RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE},
    {"Fire Temple MQ GS Fire Wall Maze Center", RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER},
    {"Fire Temple MQ GS Fire Wall Maze Side Room", RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM},
    {"Fire Temple MQ GS Above Fire Wall Maze", RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE},
    {"Fire Temple Volvagia Heart", RC_FIRE_TEMPLE_VOLVAGIA_HEART},
    {"Water Temple Compass Chest", RC_WATER_TEMPLE_COMPASS_CHEST},
    {"Water Temple Map Chest", RC_WATER_TEMPLE_MAP_CHEST},
    {"Water Temple Cracked Wall Chest", RC_WATER_TEMPLE_CRACKED_WALL_CHEST},
    {"Water Temple Torches Chest", RC_WATER_TEMPLE_TORCHES_CHEST},
    {"Water Temple Boss Key Chest", RC_WATER_TEMPLE_BOSS_KEY_CHEST},
    {"Water Temple Central Pillar Chest", RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST},
    {"Water Temple Central Bow Target Chest", RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST},
    {"Water Temple Longshot Chest", RC_WATER_TEMPLE_LONGSHOT_CHEST},
    {"Water Temple River Chest", RC_WATER_TEMPLE_RIVER_CHEST},
    {"Water Temple Dragon Chest", RC_WATER_TEMPLE_DRAGON_CHEST},
    {"Water Temple GS Behind Gate", RC_WATER_TEMPLE_GS_BEHIND_GATE},
    {"Water Temple GS Near Boss Key Chest", RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST},
    {"Water Temple GS Central Pillar", RC_WATER_TEMPLE_GS_CENTRAL_PILLAR},
    {"Water Temple GS Falling Platform Room", RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM},
    {"Water Temple GS River", RC_WATER_TEMPLE_GS_RIVER},
    {"Water Temple MQ Longshot Chest", RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST},
    {"Water Temple MQ Map Chest", RC_WATER_TEMPLE_MQ_MAP_CHEST},
    {"Water Temple MQ Compass Chest", RC_WATER_TEMPLE_MQ_COMPASS_CHEST},
    {"Water Temple MQ Central Pillar Chest", RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST},
    {"Water Temple MQ Boss Key Chest", RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Water Temple MQ Freestanding Key", RC_WATER_TEMPLE_MQ_FREESTANDING_KEY},
    {"Water Temple MQ GS Lizalfos Hallway", RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY},
    {"Water Temple MQ GS Before Upper Water Switch", RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH},
    {"Water Temple MQ GS River", RC_WATER_TEMPLE_MQ_GS_RIVER},
    {"Water Temple MQ GS Freestanding Key Area", RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA},
    {"Water Temple MQ GS Triple Wall Torch", RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH},
    {"Water Temple Morpha Heart", RC_WATER_TEMPLE_MORPHA_HEART},
    {"Shadow Temple Map Chest", RC_SHADOW_TEMPLE_MAP_CHEST},
    {"Shadow Temple Hover Boots Chest", RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST},
    {"Shadow Temple Compass Chest", RC_SHADOW_TEMPLE_COMPASS_CHEST},
    {"Shadow Temple Early Silver Rupee Chest", RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST},
    {"Shadow Temple Invisible Blades Visible Chest", RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST},
    {"Shadow Temple Invisible Blades Invisible Chest", RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST},
    {"Shadow Temple Falling Spikes Lower Chest", RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST},
    {"Shadow Temple Falling Spikes Upper Chest", RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST},
    {"Shadow Temple Falling Spikes Switch Chest", RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST},
    {"Shadow Temple Invisible Spikes Chest", RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST},
    {"Shadow Temple Freestanding Key", RC_SHADOW_TEMPLE_FREESTANDING_KEY},
    {"Shadow Temple Wind Hint Chest", RC_SHADOW_TEMPLE_WIND_HINT_CHEST},
    {"Shadow Temple After Wind Enemy Chest", RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST},
    {"Shadow Temple After Wind Hidden Chest", RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST},
    {"Shadow Temple Spike Walls Left Chest", RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST},
    {"Shadow Temple Boss Key Chest", RC_SHADOW_TEMPLE_BOSS_KEY_CHEST},
    {"Shadow Temple Invisible Floormaster Chest", RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST},
    {"Shadow Temple GS Like Like Room", RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM},
    {"Shadow Temple GS Falling Spikes Room", RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM},
    {"Shadow Temple GS Single Giant Pot", RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT},
    {"Shadow Temple GS Near Ship", RC_SHADOW_TEMPLE_GS_NEAR_SHIP},
    {"Shadow Temple GS Triple Giant Pot", RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT},
    {"Shadow Temple MQ Early Gibdos Chest", RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST},
    {"Shadow Temple MQ Map Chest", RC_SHADOW_TEMPLE_MQ_MAP_CHEST},
    {"Shadow Temple MQ Near Ship Invisible Chest", RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST},
    {"Shadow Temple MQ Compass Chest", RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST},
    {"Shadow Temple MQ Hover Boots Chest", RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST},
    {"Shadow Temple MQ Invisible Blades Invisible Chest", RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST},
    {"Shadow Temple MQ Invisible Blades Visible Chest", RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST},
    {"Shadow Temple MQ Beamos Silver Rupees Chest", RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST},
    {"Shadow Temple MQ Falling Spikes Lower Chest", RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST},
    {"Shadow Temple MQ Falling Spikes Upper Chest", RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST},
    {"Shadow Temple MQ Falling Spikes Switch Chest", RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST},
    {"Shadow Temple MQ Invisible Spikes Chest", RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST},
    {"Shadow Temple MQ Stalfos Room Chest", RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST},
    {"Shadow Temple MQ Wind Hint Chest", RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST},
    {"Shadow Temple MQ After Wind Hidden Chest", RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST},
    {"Shadow Temple MQ After Wind Enemy Chest", RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST},
    {"Shadow Temple MQ Boss Key Chest", RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Shadow Temple MQ Spike Walls Left Chest", RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST},
    {"Shadow Temple MQ Freestanding Key", RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY},
    {"Shadow Temple MQ Bomb Flower Chest", RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST},
    {"Shadow Temple MQ GS Falling Spikes Room", RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM},
    {"Shadow Temple MQ GS Wind Hint Room", RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM},
    {"Shadow Temple MQ GS After Wind", RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND},
    {"Shadow Temple MQ GS After Ship", RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP},
    {"Shadow Temple MQ GS Near Boss", RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS},
    {"Shadow Temple Bongo Bongo Heart", RC_SHADOW_TEMPLE_BONGO_BONGO_HEART},
    {"Spirit Temple Child Bridge Chest", RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST},
    {"Spirit Temple Child Early Torches Chest", RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST},
    {"Spirit Temple Child Climb North Chest", RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST},
    {"Spirit Temple Child Climb East Chest", RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST},
    {"Spirit Temple Map Chest", RC_SPIRIT_TEMPLE_MAP_CHEST},
    {"Spirit Temple Sun Block Room Chest", RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST},
    {"Spirit Temple MQ Entrance Front Left Chest", RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST},
    {"Spirit Temple MQ Entrance Back Right Chest", RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST},
    {"Spirit Temple MQ Entrance Front Right Chest", RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST},
    {"Spirit Temple MQ Entrance Back Left Chest", RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST},
    {"Spirit Temple MQ Map Chest", RC_SPIRIT_TEMPLE_MQ_MAP_CHEST},
    {"Spirit Temple MQ Map Room Enemy Chest", RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST},
    {"Spirit Temple MQ Child Climb North Chest", RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST},
    {"Spirit Temple MQ Child Climb South Chest", RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST},
    {"Spirit Temple MQ Compass Chest", RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST},
    {"Spirit Temple MQ Silver Block Hallway Chest", RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST},
    {"Spirit Temple MQ Sun Block Room Chest", RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST},
    {"Spirit Temple Silver Gauntlets Chest", RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST},
    {"Spirit Temple Compass Chest", RC_SPIRIT_TEMPLE_COMPASS_CHEST},
    {"Spirit Temple Early Adult Right Chest", RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST},
    {"Spirit Temple First Mirror Left Chest", RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST},
    {"Spirit Temple First Mirror Right Chest", RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST},
    {"Spirit Temple Statue Room Northeast Chest", RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST},
    {"Spirit Temple Statue Room Hand Chest", RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST},
    {"Spirit Temple Near Four Armos Chest", RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST},
    {"Spirit Temple Hallway Right Invisible Chest", RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST},
    {"Spirit Temple Hallway Left Invisible Chest", RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST},
    {"Spirit Temple MQ Child Hammer Switch Chest", RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST},
    {"Spirit Temple MQ Statue Room Lullaby Chest", RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST},
    {"Spirit Temple MQ Statue Room Invisible Chest", RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST},
    {"Spirit Temple MQ Leever Room Chest", RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST},
    {"Spirit Temple MQ Symphony Room Chest", RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST},
    {"Spirit Temple MQ Beamos Room Chest", RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST},
    {"Spirit Temple MQ Chest Switch Chest", RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST},
    {"Spirit Temple MQ Boss Key Chest", RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Spirit Temple Mirror Shield Chest", RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST},
    {"Spirit Temple Boss Key Chest", RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST},
    {"Spirit Temple Topmost Chest", RC_SPIRIT_TEMPLE_TOPMOST_CHEST},
    {"Spirit Temple MQ Mirror Puzzle Invisible Chest", RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST},
    {"Spirit Temple GS Metal Fence", RC_SPIRIT_TEMPLE_GS_METAL_FENCE},
    {"Spirit Temple GS Sun on Floor Room", RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM},
    {"Spirit Temple GS Hall After Sun Block Room", RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM},
    {"Spirit Temple GS Lobby", RC_SPIRIT_TEMPLE_GS_LOBBY},
    {"Spirit Temple GS Boulder Room", RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM},
    {"Spirit Temple MQ GS Sun Block Room", RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM},
    {"Spirit Temple MQ GS Leever Room", RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM},
    {"Spirit Temple MQ GS Symphony Room", RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM},
    {"Spirit Temple MQ GS Nine Thrones Room West", RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST},
    {"Spirit Temple MQ GS Nine Thrones Room North", RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH},
    {"Spirit Temple Twinrova Heart", RC_SPIRIT_TEMPLE_TWINROVA_HEART},
    {"Ice Cavern Map Chest", RC_ICE_CAVERN_MAP_CHEST},
    {"Ice Cavern Compass Chest", RC_ICE_CAVERN_COMPASS_CHEST},
    {"Ice Cavern Freestanding PoH", RC_ICE_CAVERN_FREESTANDING_POH},
    {"Ice Cavern Iron Boots Chest", RC_ICE_CAVERN_IRON_BOOTS_CHEST},
    {"Ice Cavern GS Spinning Scythe Room", RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM},
    {"Ice Cavern GS Heart Piece Room", RC_ICE_CAVERN_GS_HEART_PIECE_ROOM},
    {"Ice Cavern GS Push Block Room", RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM},
    {"Ice Cavern MQ Map Chest", RC_ICE_CAVERN_MQ_MAP_CHEST},
    {"Ice Cavern MQ Compass Chest", RC_ICE_CAVERN_MQ_COMPASS_CHEST},
    {"Ice Cavern MQ Freestanding PoH", RC_ICE_CAVERN_MQ_FREESTANDING_POH},
    {"Ice Cavern MQ Iron Boots Chest", RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST},
    {"Ice Cavern MQ GS Red Ice", RC_ICE_CAVERN_MQ_GS_RED_ICE},
    {"Ice Cavern MQ GS Ice Block", RC_ICE_CAVERN_MQ_GS_ICE_BLOCK},
    {"Ice Cavern MQ GS Scarecrow", RC_ICE_CAVERN_MQ_GS_SCARECROW},
    {"Gerudo Training Ground Lobby Left Chest", RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST},
    {"Gerudo Training Ground Lobby Right Chest", RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST},
    {"Gerudo Training Ground Stalfos Chest", RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST},
    {"Gerudo Training Ground Before Heavy Block Chest", RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST},
    {"Gerudo Training Ground Heavy Block First Chest", RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST},
    {"Gerudo Training Ground Heavy Block Second Chest", RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST},
    {"Gerudo Training Ground Heavy Block Third Chest", RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST},
    {"Gerudo Training Ground Heavy Block Fourth Chest", RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST},
    {"Gerudo Training Ground Eye Statue Chest", RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST},
    {"Gerudo Training Ground Near Scarecrow Chest", RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST},
    {"Gerudo Training Ground Hammer Room Clear Chest", RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST},
    {"Gerudo Training Ground Hammer Room Switch Chest", RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST},
    {"Gerudo Training Ground Freestanding Key", RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY},
    {"Gerudo Training Ground Maze Right Central Chest", RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST},
    {"Gerudo Training Ground Maze Right Side Chest", RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST},
    {"Gerudo Training Ground Underwater Silver Rupee Chest", RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST},
    {"Gerudo Training Ground Beamos Chest", RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST},
    {"Gerudo Training Ground Hidden Ceiling Chest", RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST},
    {"Gerudo Training Ground Maze Path First Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST},
    {"Gerudo Training Ground Maze Path Second Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST},
    {"Gerudo Training Ground Maze Path Third Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST},
    {"Gerudo Training Ground Maze Path Final Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST},
    {"Gerudo Training Ground MQ Lobby Left Chest", RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST},
    {"Gerudo Training Ground MQ Lobby Right Chest", RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST},
    {"Gerudo Training Ground MQ First Iron Knuckle Chest", RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST},
    {"Gerudo Training Ground MQ Before Heavy Block Chest", RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST},
    {"Gerudo Training Ground MQ Heavy Block Chest", RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST},
    {"Gerudo Training Ground MQ Eye Statue Chest", RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST},
    {"Gerudo Training Ground MQ Ice Arrows Chest", RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST},
    {"Gerudo Training Ground MQ Second Iron Knuckle Chest", RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST},
    {"Gerudo Training Ground MQ Flame Circle Chest", RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST},
    {"Gerudo Training Ground MQ Maze Right Central Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST},
    {"Gerudo Training Ground MQ Maze Right Side Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST},
    {"Gerudo Training Ground MQ Underwater Silver Rupee Chest", RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST},
    {"Gerudo Training Ground MQ Dinolfos Chest", RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST},
    {"Gerudo Training Ground MQ Hidden Ceiling Chest", RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST},
    {"Gerudo Training Ground MQ Maze Path First Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST},
    {"Gerudo Training Ground MQ Maze Path Third Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST},
    {"Gerudo Training Ground MQ Maze Path Second Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST},
    {"Ganons Castle Forest Trial Chest", RC_GANONS_CASTLE_FOREST_TRIAL_CHEST},
    {"Ganons Castle Water Trial Left Chest", RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST},
    {"Ganons Castle Water Trial Right Chest", RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST},
    {"Ganons Castle Shadow Trial Front Chest", RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST},
    {"Ganons Castle Shadow Trial Golden Gauntlets Chest", RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST},
    {"Ganons Castle Light Trial First Left Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST},
    {"Ganons Castle Light Trial Second Left Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST},
    {"Ganons Castle Light Trial Third Left Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST},
    {"Ganons Castle Light Trial First Right Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST},
    {"Ganons Castle Light Trial Second Right Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST},
    {"Ganons Castle Light Trial Third Right Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST},
    {"Ganons Castle Light Trial Invisible Enemies Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST},
    {"Ganons Castle Light Trial Lullaby Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST},
    {"Ganons Castle Spirit Trial Crystal Switch Chest", RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST},
    {"Ganons Castle Spirit Trial Invisible Chest", RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST},
    {"Ganons Castle Deku Scrub Left", RC_GANONS_CASTLE_DEKU_SCRUB_LEFT},
    {"Ganons Castle Deku Scrub Center-Left", RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT},
    {"Ganons Castle Deku Scrub Center-Right", RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT},
    {"Ganons Castle Deku Scrub Right", RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT},
    {"Ganons Castle MQ Forest Trial Freestanding Key", RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY},
    {"Ganons Castle MQ Forest Trial Eye Switch Chest", RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST},
    {"Ganons Castle MQ Forest Trial Frozen Eye Switch Chest", RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST},
    {"Ganons Castle MQ Water Trial Chest", RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST},
    {"Ganons Castle MQ Shadow Trial Bomb Flower Chest", RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST},
    {"Ganons Castle MQ Shadow Trial Eye Switch Chest", RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST},
    {"Ganons Castle MQ Light Trial Lullaby Chest", RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST},
    {"Ganons Castle MQ Spirit Trial First Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST},
    {"Ganons Castle MQ Spirit Trial Invisible Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST},
    {"Ganons Castle MQ Spirit Trial Sun Front Left Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST},
    {"Ganons Castle MQ Spirit Trial Sun Back Left Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST},
    {"Ganons Castle MQ Spirit Trial Sun Back Right Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST},
    {"Ganons Castle MQ Spirit Trial Golden Gauntlets Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST},
    {"Ganons Castle MQ Deku Scrub Left", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT},
    {"Ganons Castle MQ Deku Scrub Center-Left", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT},
    {"Ganons Castle MQ Deku Scrub Center", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER},
    {"Ganons Castle MQ Deku Scrub Center-Right", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT},
    {"Ganons Castle MQ Deku Scrub Right", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT},
    {"Ganons Tower Boss Key Chest", RC_GANONS_TOWER_BOSS_KEY_CHEST},
    {"Pierre", RC_PIERRE},
    {"Deliver Rutos Letter", RC_DELIVER_RUTOS_LETTER},
    {"Master Sword Pedestal", RC_MASTER_SWORD_PEDESTAL}
};

std::unordered_map<s16, s16> itemIdToModel = { { GI_NONE, GID_MAX },
                                               { GI_BOMBS_5, GID_BOMB },
                                               { GI_NUTS_5, GID_NUTS },
                                               { GI_BOMBCHUS_10, GID_BOMBCHU },
                                               { GI_BOW, GID_BOW },
                                               { GI_SLINGSHOT, GID_SLINGSHOT },
                                               { GI_BOOMERANG, GID_BOOMERANG },
                                               { GI_STICKS_1, GID_STICK },
                                               { GI_HOOKSHOT, GID_HOOKSHOT },
                                               { GI_LONGSHOT, GID_LONGSHOT },
                                               { GI_LENS, GID_LENS },
                                               { GI_LETTER_ZELDA, GID_LETTER_ZELDA },
                                               { GI_OCARINA_OOT, GID_OCARINA_TIME },
                                               { GI_HAMMER, GID_HAMMER },
                                               { GI_COJIRO, GID_COJIRO },
                                               { GI_LETTER_RUTO, GID_LETTER_RUTO },
                                               { GI_LETTER_RUTO, GID_LETTER_RUTO },
                                               { GI_BOTTLE, GID_BOTTLE },
                                               { GI_POTION_RED, GID_POTION_RED },
                                               { GI_POTION_GREEN, GID_POTION_GREEN },
                                               { GI_POTION_BLUE, GID_POTION_BLUE },
                                               { GI_FAIRY, GID_FAIRY },
                                               { GI_MILK_BOTTLE, GID_MILK },
                                               { GI_LETTER_RUTO, GID_LETTER_RUTO },
                                               { GI_BEAN, GID_BEAN },
                                               { GI_MASK_SKULL, GID_MASK_SKULL },
                                               { GI_MASK_SPOOKY, GID_MASK_SPOOKY },
                                               { GI_CHICKEN, GID_CHICKEN },
                                               { GI_MASK_KEATON, GID_MASK_KEATON },
                                               { GI_MASK_BUNNY, GID_MASK_BUNNY },
                                               { GI_MASK_TRUTH, GID_MASK_TRUTH },
                                               { GI_POCKET_EGG, GID_EGG },
                                               { GI_POCKET_CUCCO, GID_CHICKEN },
                                               { GI_ODD_MUSHROOM, GID_ODD_MUSHROOM },
                                               { GI_ODD_POTION, GID_ODD_POTION },
                                               { GI_SAW, GID_SAW },
                                               { GI_SWORD_BROKEN, GID_SWORD_BROKEN },
                                               { GI_PRESCRIPTION, GID_PRESCRIPTION },
                                               { GI_FROG, GID_FROG },
                                               { GI_EYEDROPS, GID_EYEDROPS },
                                               { GI_CLAIM_CHECK, GID_CLAIM_CHECK },
                                               { GI_SWORD_KOKIRI, GID_SWORD_KOKIRI },
                                               { GI_SWORD_KNIFE, GID_SWORD_BGS },
                                               { GI_SHIELD_DEKU, GID_SHIELD_DEKU },
                                               { GI_SHIELD_HYLIAN, GID_SHIELD_HYLIAN },
                                               { GI_SHIELD_MIRROR, GID_SHIELD_MIRROR },
                                               { GI_TUNIC_GORON, GID_TUNIC_GORON },
                                               { GI_TUNIC_ZORA, GID_TUNIC_ZORA },
                                               { GI_BOOTS_IRON, GID_BOOTS_IRON },
                                               { GI_BOOTS_HOVER, GID_BOOTS_HOVER },
                                               { GI_QUIVER_40, GID_QUIVER_40 },
                                               { GI_QUIVER_50, GID_QUIVER_50 },
                                               { GI_BOMB_BAG_20, GID_BOMB_BAG_20 },
                                               { GI_BOMB_BAG_30, GID_BOMB_BAG_30 },
                                               { GI_BOMB_BAG_40, GID_BOMB_BAG_40 },
                                               { GI_GAUNTLETS_SILVER, GID_GAUNTLETS_SILVER },
                                               { GI_GAUNTLETS_GOLD, GID_GAUNTLETS_GOLD },
                                               { GI_SCALE_SILVER, GID_SCALE_SILVER },
                                               { GI_SCALE_GOLD, GID_SCALE_GOLDEN },
                                               { GI_STONE_OF_AGONY, GID_STONE_OF_AGONY },
                                               { GI_GERUDO_CARD, GID_GERUDO_CARD },
                                               { GI_OCARINA_FAIRY, GID_OCARINA_FAIRY },
                                               { GI_SEEDS_5, GID_SEEDS },
                                               { GI_HEART_CONTAINER, GID_HEART_CONTAINER },
                                               { GI_HEART_PIECE, GID_HEART_PIECE },
                                               { GI_KEY_BOSS, GID_KEY_BOSS },
                                               { GI_COMPASS, GID_COMPASS },
                                               { GI_MAP, GID_DUNGEON_MAP },
                                               { GI_KEY_SMALL, GID_KEY_SMALL },
                                               { GI_MAGIC_SMALL, GID_MAGIC_SMALL },
                                               { GI_MAGIC_LARGE, GID_MAGIC_LARGE },
                                               { GI_WALLET_ADULT, GID_WALLET_ADULT },
                                               { GI_WALLET_GIANT, GID_WALLET_GIANT },
                                               { GI_WEIRD_EGG, GID_EGG },
                                               { GI_HEART, GID_HEART },
                                               { GI_ARROWS_SMALL, GID_ARROWS_SMALL },
                                               { GI_ARROWS_MEDIUM, GID_ARROWS_MEDIUM },
                                               { GI_ARROWS_LARGE, GID_ARROWS_LARGE },
                                               { GI_RUPEE_GREEN, GID_RUPEE_GREEN },
                                               { GI_RUPEE_BLUE, GID_RUPEE_BLUE },
                                               { GI_RUPEE_RED, GID_RUPEE_RED },
                                               { GI_HEART_CONTAINER_2, GI_HEART_CONTAINER_2 },
                                               { GI_MILK, GID_MILK },
                                               { GI_MASK_GORON, GID_MASK_GORON },
                                               { GI_MASK_ZORA, GID_MASK_ZORA },
                                               { GI_MASK_GERUDO, GID_MASK_GERUDO },
                                               { GI_BRACELET, GID_BRACELET },
                                               { GI_RUPEE_PURPLE, GID_RUPEE_PURPLE },
                                               { GI_RUPEE_GOLD, GID_RUPEE_GOLD },
                                               { GI_SWORD_BGS, GID_SWORD_BGS },
                                               { GI_ARROW_FIRE, GID_ARROW_FIRE },
                                               { GI_ARROW_ICE, GID_ARROW_ICE },
                                               { GI_ARROW_LIGHT, GID_ARROW_LIGHT },
                                               { GI_SKULL_TOKEN, GID_SKULL_TOKEN },
                                               { GI_DINS_FIRE, GID_DINS_FIRE },
                                               { GI_FARORES_WIND, GID_FARORES_WIND },
                                               { GI_NAYRUS_LOVE, GID_NAYRUS_LOVE },
                                               { GI_BULLET_BAG_30, GID_BULLET_BAG },
                                               { GI_BULLET_BAG_40, GID_BULLET_BAG },
                                               { GI_STICKS_5, GID_STICK },
                                               { GI_STICKS_10, GID_STICK },
                                               { GI_NUTS_5_2, GID_NUTS },
                                               { GI_NUTS_10, GID_NUTS },
                                               { GI_BOMBS_1, GID_BOMB },
                                               { GI_BOMBS_10, GID_BOMB },
                                               { GI_BOMBS_20, GID_BOMB },
                                               { GI_BOMBS_30, GID_BOMB },
                                               { GI_SEEDS_30, GID_SEEDS },
                                               { GI_BOMBCHUS_5, GID_BOMBCHU },
                                               { GI_BOMBCHUS_20, GID_BOMBCHU },
                                               { GI_FISH, GID_FISH },
                                               { GI_BUGS, GID_BUG },
                                               { GI_BLUE_FIRE, GID_BLUE_FIRE },
                                               { GI_POE, GID_POE },
                                               { GI_BIG_POE, GID_BIG_POE },
                                               { GI_DOOR_KEY, GID_KEY_SMALL },
                                               { GI_RUPEE_GREEN_LOSE, GID_RUPEE_GREEN },
                                               { GI_RUPEE_BLUE_LOSE, GID_RUPEE_BLUE },
                                               { GI_RUPEE_RED_LOSE, GID_RUPEE_RED },
                                               { GI_RUPEE_PURPLE_LOSE, GID_RUPEE_PURPLE },
                                               { GI_HEART_PIECE_WIN, GID_HEART_PIECE },
                                               { GI_STICK_UPGRADE_20, GID_STICK },
                                               { GI_STICK_UPGRADE_30, GID_STICK },
                                               { GI_NUT_UPGRADE_30, GID_NUTS },
                                               { GI_NUT_UPGRADE_40, GID_NUTS },
                                               { GI_BULLET_BAG_50, GID_BULLET_BAG_50 },
                                               { GI_ICE_TRAP, GID_MAX },
                                               { GI_TEXT_0, GID_MAX } };

std::unordered_map<std::string, RandomizerGet> SpoilerfileGetNameToEnum = {
    { "Bombs (5)", RG_BOMBS_5 },
    { "Deku Nuts (5)", RG_DEKU_NUTS_5 },
    { "Bombchus (10)", RG_BOMBCHUS_10 },
    { "Boomerang", RG_BOOMERANG },
    { "Deku Stick (1)", RG_DEKU_STICK_1 },
    { "Lens of Truth", RG_LENS_OF_TRUTH },
    { "Megaton Hammer", RG_MEGATON_HAMMER },
    { "Cojiro", RG_COJIRO },
    { "Bottle", RG_BOTTLE },
    { "Bottle with Milk", RG_BOTTLE_WITH_MILK },
    { "Rutos Letter", RG_RUTOS_LETTER },
    { "Deliver Letter", RG_DELIVER_LETTER },
    { "Sell Big Poe", RG_BOTTLE_WITH_BIG_POE },
    { "Magic Bean", RG_MAGIC_BEAN },
    { "Skull Mask", RG_SKULL_MASK },
    { "Spooky Mask", RG_SPOOKY_MASK },
    { "Keaton Mask", RG_KEATON_MASK },
    { "Bunny Hood", RG_BUNNY_HOOD },
    { "Mask of Truth", RG_MASK_OF_TRUTH },
    { "Pocket Egg", RG_POCKET_EGG },
    { "Pocket Cucco", RG_POCKET_CUCCO },
    { "Odd Mushroom", RG_ODD_MUSHROOM },
    { "Odd Potion", RG_ODD_POTION },
    { "Poachers Saw", RG_POACHERS_SAW },
    { "Broken Sword", RG_BROKEN_SWORD },
    { "Prescription", RG_PRESCRIPTION },
    { "Eyeball Frog", RG_EYEBALL_FROG },
    { "Eyedrops", RG_EYEDROPS },
    { "Claim Check", RG_CLAIM_CHECK },
    { "Kokiri Sword", RG_KOKIRI_SWORD },
    { "Giants Knife", RG_GIANTS_KNIFE },
    { "Deku Shield", RG_DEKU_SHIELD },
    { "Hylian Shield", RG_HYLIAN_SHIELD },
    { "Mirror Shield", RG_MIRROR_SHIELD },
    { "Goron Tunic", RG_GORON_TUNIC },
    { "Zora Tunic", RG_ZORA_TUNIC },
    { "Iron Boots", RG_IRON_BOOTS },
    { "Hover Boots", RG_HOVER_BOOTS },
    { "Stone of Agony", RG_STONE_OF_AGONY },
    { "Gerudo Membership Card", RG_GERUDO_MEMBERSHIP_CARD },
    { "Heart Container", RG_HEART_CONTAINER },
    { "Piece of Heart", RG_PIECE_OF_HEART },
    { "Boss Key", RG_BOSS_KEY },
    { "Compass", RG_COMPASS },
    { "Map", RG_MAP },
    { "Small Key", RG_SMALL_KEY },
    { "Weird Egg", RG_WEIRD_EGG },
    { "Recovery Heart", RG_RECOVERY_HEART },
    { "Arrows (5)", RG_ARROWS_5 },
    { "Arrows (10)", RG_ARROWS_10 },
    { "Arrows (30)", RG_ARROWS_30 },
    { "Rupee (1)", RG_RUPEE_1 },
    { "Rupees (5)", RG_RUPEES_5 },
    { "Rupees (20)", RG_RUPEES_20 },
    { "Milk", RG_MILK },
    { "Goron Mask", RG_GORON_MASK },
    { "Zora Mask", RG_ZORA_MASK },
    { "Gerudo Mask", RG_GERUDO_MASK },
    { "Rupees (50)", RG_RUPEES_50 },
    { "Rupees (200)", RG_RUPEES_200 },
    { "Biggoron Sword", RG_BIGGORON_SWORD },
    { "Fire Arrows", RG_FIRE_ARROWS },
    { "Ice Arrows", RG_ICE_ARROWS },
    { "Light Arrows", RG_LIGHT_ARROWS },
    { "Gold Skulltula Token", RG_GOLD_SKULLTULA_TOKEN },
    { "Dins Fire", RG_DINS_FIRE },
    { "Nayrus Love", RG_NAYRUS_LOVE },
    { "Farores Wind", RG_FARORES_WIND },
    { "Deku Nuts (10)", RG_DEKU_NUTS_10 },
    { "Bombs (10)", RG_BOMBS_10 },
    { "Bombs (20)", RG_BOMBS_20 },
    { "Deku Seeds (30)", RG_DEKU_SEEDS_30 },
    { "Bombchus (5)", RG_BOMBCHUS_5 },
    { "Bombchus (20)", RG_BOMBCHUS_20 },
    { "Rupee (Treasure Chest Game)", RG_RUPEE_TREASURE_CHEST_GAME },
    { "Piece of Heart (Treasure Chest Game)", RG_PIECE_OF_HEART_TREASURE_CHEST_GAME },
    { "Ice Trap", RG_ICE_TRAP },
    { "Progressive Hookshot", RG_PROGRESSIVE_HOOKSHOT },
    { "Progressive Strength Upgrade", RG_PROGRESSIVE_STRENGTH_UPGRADE },
    { "Bomb Bag", RG_BOMB_BAG },
    { "Bow", RG_BOW },
    { "Slingshot", RG_SLINGSHOT },
    { "Progressive Wallet", RG_PROGRESSIVE_WALLET },
    { "Progressive Scale", RG_PROGRESSIVE_SCALE },
    { "Deku Nut Capacity", RG_DEKU_NUT_CAPACITY },
    { "Deku Stick Capacity", RG_DEKU_STICK_CAPACITY },
    { "Bombchus", RG_BOMBCHUS },
    { "Magic Meter", RG_MAGIC_METER },
    { "Ocarina", RG_OCARINA },
    { "Bottle with Red Potion", RG_BOTTLE_WITH_RED_POTION },
    { "Bottle with Green Potion", RG_BOTTLE_WITH_GREEN_POTION },
    { "Bottle with Blue Potion", RG_BOTTLE_WITH_BLUE_POTION },
    { "Bottle with Fairy", RG_BOTTLE_WITH_FAIRY },
    { "Bottle with Fish", RG_BOTTLE_WITH_FISH },
    { "Bottle with Blue Fire", RG_BOTTLE_WITH_BLUE_FIRE },
    { "Bottle with Bugs", RG_BOTTLE_WITH_BUGS },
    { "Bottle with Big Poe", RG_BOTTLE_WITH_BIG_POE },
    { "Bottle with Poe", RG_BOTTLE_WITH_POE },
    { "Boss Key (Forest Temple)", RG_BOSS_KEY_FOREST_TEMPLE },
    { "Boss Key (Fire Temple)", RG_BOSS_KEY_FIRE_TEMPLE },
    { "Boss Key (Water Temple)", RG_BOSS_KEY_WATER_TEMPLE },
    { "Boss Key (Spirit Temple)", RG_BOSS_KEY_SPIRIT_TEMPLE },
    { "Boss Key (Shadow Temple)", RG_BOSS_KEY_SHADOW_TEMPLE },
    { "Boss Key (Ganons Castle)", RG_BOSS_KEY_GANONS_CASTLE },
    { "Compass (Deku Tree)", RG_COMPASS_DEKU_TREE },
    { "Compass (Dodongos Cavern)", RG_COMPASS_DODONGOS_CAVERN },
    { "Compass (Jabu Jabus Belly)", RG_COMPASS_JABU_JABUS_BELLY },
    { "Compass (Forest Temple)", RG_COMPASS_FOREST_TEMPLE },
    { "Compass (Fire Temple)", RG_COMPASS_FIRE_TEMPLE },
    { "Compass (Water Temple)", RG_COMPASS_WATER_TEMPLE },
    { "Compass (Spirit Temple)", RG_COMPASS_SPIRIT_TEMPLE },
    { "Compass (Shadow Temple)", RG_COMPASS_SHADOW_TEMPLE },
    { "Compass (Bottom of the Well)", RG_COMPASS_BOTTOM_OF_THE_WELL },
    { "Compass (Ice Cavern)", RG_COMPASS_ICE_CAVERN },
    { "Map (Deku Tree)", RG_MAP_DEKU_TREE },
    { "Map (Dodongos Cavern)", RG_MAP_DODONGOS_CAVERN },
    { "Map (Jabu Jabus Belly)", RG_MAP_JABU_JABUS_BELLY },
    { "Map (Forest Temple)", RG_MAP_FOREST_TEMPLE },
    { "Map (Fire Temple)", RG_MAP_FIRE_TEMPLE },
    { "Map (Water Temple)", RG_MAP_WATER_TEMPLE },
    { "Map (Spirit Temple)", RG_MAP_SPIRIT_TEMPLE },
    { "Map (Shadow Temple)", RG_MAP_SHADOW_TEMPLE },
    { "Map (Bottom of the Well)", RG_MAP_BOTTOM_OF_THE_WELL },
    { "Map (Ice Cavern)", RG_MAP_ICE_CAVERN },
    { "Small Key (Forest Temple)", RG_SMALL_KEY_FOREST_TEMPLE },
    { "Small Key (Fire Temple)", RG_SMALL_KEY_FIRE_TEMPLE },
    { "Small Key (Water Temple)", RG_SMALL_KEY_WATER_TEMPLE },
    { "Small Key (Spirit Temple)", RG_SMALL_KEY_SPIRIT_TEMPLE },
    { "Small Key (Shadow Temple)", RG_SMALL_KEY_SHADOW_TEMPLE },
    { "Small Key (Bottom of the Well)", RG_SMALL_KEY_BOTTOM_OF_THE_WELL },
    { "Small Key (Gerudo Training Ground)", RG_SMALL_KEY_GERUDO_TRAINING_GROUND },
    { "Small Key (Thieves Hideout)", RG_SMALL_KEY_THIEVES_HIDEOUT },
    { "Small Key (Ganons Castle)", RG_SMALL_KEY_GANONS_CASTLE },
    { "Double Defense", RG_DOUBLE_DEFENSE },
    { "Magic Bean Pack", RG_MAGIC_BEAN_PACK },
    { "Triforce Piece", RG_TRIFORCE_PIECE },
    { "Zeldas Letter", RG_ZELDAS_LETTER },
    { "Time Travel", RG_TIME_TRAVEL },
    { "Scarecrow Song", RG_SCARECROW_SONG },
    { "Triforce", RG_TRIFORCE },
    { "Small Key Ring (Forest Temple)", RG_SMALL_KEY_RING_FOREST_TEMPLE },
    { "Small Key Ring (Fire Temple)", RG_SMALL_KEY_RING_FIRE_TEMPLE },
    { "Small Key Ring (Water Temple)", RG_SMALL_KEY_RING_WATER_TEMPLE },
    { "Small Key Ring (Spirit Temple)", RG_SMALL_KEY_RING_SPIRIT_TEMPLE },
    { "Small Key Ring (Shadow Temple)", RG_SMALL_KEY_RING_SHADOW_TEMPLE },
    { "Small Key Ring (Bottom of the Well)", RG_SMALL_KEY_RING_BOTTOM_OF_THE_WELL },
    { "Small Key Ring (Gerudo Training Ground)", RG_SMALL_KEY_RING_GERUDO_TRAINING_GROUND },
    { "Small Key Ring (Thieves Hideout)", RG_SMALL_KEY_RING_THIEVES_HIDEOUT },
    { "Small Key Ring (Ganons Castle)", RG_SMALL_KEY_RING_GANONS_CASTLE },
    { "Water Temple Clear", RG_WATER_TEMPLE_CLEAR },
    { "Forest Trial Clear", RG_FOREST_TRIAL_CLEAR },
    { "Fire Trial Clear", RG_FIRE_TRIAL_CLEAR },
    { "Water Trial Clear", RG_WATER_TRIAL_CLEAR },
    { "Shadow Trial Clear", RG_SHADOW_TRIAL_CLEAR },
    { "Spirit Trial Clear", RG_SPIRIT_TRIAL_CLEAR },
    { "Light Trial Clear", RG_LIGHT_TRIAL_CLEAR },
    { "Deku Stick Drop", RG_DEKU_STICK_DROP },
    { "Deku Nut Drop", RG_DEKU_NUT_DROP },
    { "Blue Fire", RG_BLUE_FIRE },
    { "Fairy", RG_FAIRY },
    { "Fish", RG_FISH },
    { "Bugs", RG_BUGS },
    { "Big Poe", RG_BIG_POE },
    { "Bombchu Drop", RG_BOMBCHU_DROP },
    { "Arrows", RG_ARROWS_CONSUMABLE_REFILL },
    { "Bombs", RG_BOMBS_CONSUMABLE_REFILL },
    { "Deku Seeds", RG_DEKU_SEEDS_CONSUMABLE_REFILL },
    { "Deku Sticks", RG_DEKU_STICKS_CONSUMABLE_REFILL },
    { "Deku Nuts", RG_DEKU_NUTS_CONSUMABLE_REFILL },
    { "Rupees", RG_RUPEES_CONSUMABLE_REFILL },
    { "Minuet of Forest", RG_MINUET_OF_FOREST },
    { "Bolero of Fire", RG_BOLERO_OF_FIRE },
    { "Serenade of Water", RG_SERENADE_OF_WATER },
    { "Requiem of Spirit", RG_REQUIEM_OF_SPIRIT },
    { "Nocturne of Shadow", RG_NOCTURNE_OF_SHADOW },
    { "Prelude of Light", RG_PRELUDE_OF_LIGHT },
    { "Zeldas Lullaby", RG_ZELDAS_LULLABY },
    { "Eponas Song", RG_EPONAS_SONG },
    { "Sarias Song", RG_SARIAS_SONG },
    { "Suns Song", RG_SUNS_SONG },
    { "Song of Time", RG_SONG_OF_TIME },
    { "Song of Storms", RG_SONG_OF_STORMS },
    { "Buy Deku Nut (5)", RG_BUY_DEKU_NUT_5 },
    { "Buy Arrows (30)", RG_BUY_ARROWS_30 },
    { "Buy Arrows (50)", RG_BUY_ARROWS_50 },
    { "Buy Bombs (5) for 25 Rupees", RG_BUY_BOMBS_5_FOR_25_RUPEES },
    { "Buy Deku Nut (10)", RG_BUY_DEKU_NUT_10 },
    { "Buy Deku Stick (1)", RG_BUY_DEKU_STICK_1 },
    { "Buy Bombs (10)", RG_BUY_BOMBS_10 },
    { "Buy Fish", RG_BUY_FISH },
    { "Buy Red Potion for 30 Rupees", RG_BUY_RED_POTION_FOR_30_RUPEES },
    { "Buy Green Potion", RG_BUY_GREEN_POTION },
    { "Buy Blue Potion", RG_BUY_BLUE_POTION },
    { "Buy Hylian Shield", RG_BUY_HYLIAN_SHIELD },
    { "Buy Deku Shield", RG_BUY_DEKU_SHIELD },
    { "Buy Goron Tunic", RG_BUY_GORON_TUNIC },
    { "Buy Zora Tunic", RG_BUY_ZORA_TUNIC },
    { "Buy Heart", RG_BUY_HEART },
    { "Buy Bombchu (10)", RG_BUY_BOMBCHU_10 },
    { "Buy Bombchu (20)", RG_BUY_BOMBCHU_20 },
    { "Buy Bombchu (5)", RG_BUY_BOMBCHU_5 },
    { "Buy Deku Seeds (30)", RG_BUY_DEKU_SEEDS_30 },
    { "Sold Out", RG_SOLD_OUT },
    { "Buy Blue Fire", RG_BUY_BLUE_FIRE },
    { "Buy Bottle Bug", RG_BUY_BOTTLE_BUG },
    { "Buy Poe", RG_BUY_POE },
    { "Buy Fairy\'s Spirit", RG_BUY_FAIRYS_SPIRIT },
    { "Buy Arrows (10)", RG_BUY_ARROWS_10 },
    { "Buy Bombs (20)", RG_BUY_BOMBS_20 },
    { "Buy Bombs (30)", RG_BUY_BOMBS_30 },
    { "Buy Bombs (5) for 35 Rupees", RG_BUY_BOMBS_5_FOR_35_RUPEES },
    { "Buy Red Potion for 40 Rupees", RG_BUY_RED_POTION_FOR_40_RUPEES },
    { "Buy Red Potion for 50 Rupees", RG_BUY_RED_POTION_FOR_50_RUPEES },
    { "Kokiri Emerald", RG_KOKIRI_EMERALD },
    { "Goron Ruby", RG_GORON_RUBY },
    { "Zora Sapphire", RG_ZORA_SAPPHIRE },
    { "Forest Medallion", RG_FOREST_MEDALLION },
    { "Fire Medallion", RG_FIRE_MEDALLION },
    { "Water Medallion", RG_WATER_MEDALLION },
    { "Spirit Medallion", RG_SPIRIT_MEDALLION },
    { "Shadow Medallion", RG_SHADOW_MEDALLION },
    { "Light Medallion", RG_LIGHT_MEDALLION }
};

s16 Randomizer::GetItemModelFromId(s16 itemId) {
    return itemIdToModel[itemId];
}

void Randomizer::LoadItemLocations(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        // bandaid until new save stuff happens
        ParseItemLocationsFile(spoilerFileName);

        for (auto itemLocation : gSaveContext.itemLocations) {
            this->itemLocations[itemLocation.check] = itemLocation.get;
        }
    } else {
        u16 index = 0;
        for (auto itemLocation : this->itemLocations) {
            gSaveContext.itemLocations[index].check = itemLocation.first;
            gSaveContext.itemLocations[index].get = itemLocation.second;
            index++;
        }
    }
}

std::string sanitize(std::string stringValue) {
    // Add backslashes.
    for (auto i = stringValue.begin();;) {
        auto const pos =
            std::find_if(i, stringValue.end(), [](char const c) { return '\\' == c || '\'' == c || '"' == c; });
        if (pos == stringValue.end()) {
            break;
        }
        i = std::next(stringValue.insert(pos, '\\'), 2);
    }

    // Removes others.
    stringValue.erase(std::remove_if(stringValue.begin(), stringValue.end(),
                                     [](char const c) { return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c; }),
                      stringValue.end());

    return stringValue;
}

void Randomizer::ParseItemLocationsFile(const char* spoilerFileName) {
    // todo pull this in from cvar or something

    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json locationsJson = spoilerFileJson["locations"];
        json hashJson = spoilerFileJson["file_hash"];

        int index = 0;
        for (auto it = hashJson.begin(); it != hashJson.end(); ++it) {
            gSaveContext.seedIcons[index] = gSeedTextures[it.value()];
            index++;
        }

        index = 0;
        for (auto it = locationsJson.begin(); it != locationsJson.end(); ++it) {
            if (it->is_structured()) {
                json itemJson = *it;
                for (auto itemit = itemJson.begin(); itemit != itemJson.end(); ++itemit) {
                    // todo handle prices
                    if (itemit.key() == "item") {

                        gSaveContext.itemLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];
                        gSaveContext.itemLocations[index].get = SpoilerfileGetNameToEnum[itemit.value()];
                    }
                }
            } else {
                gSaveContext.itemLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];
                gSaveContext.itemLocations[index].get = SpoilerfileGetNameToEnum[it.value()];
            }

            index++;
        }

        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        success = true;
    } catch (const std::exception& e) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        return;
    }

    if (success) {
        CVar_SetS32("gRandomizer", 1);
        CVar_SetS32("gDroppedNewSpoilerFile", 0);
        Game::SaveSettings();
    }
}

GetItemID Randomizer::GetRandomizedItemId(GetItemID ogId, s16 actorId, s16 actorParams, s16 sceneNum) {
    GetItemID itemId = GetItemFromActor(actorId, actorParams, sceneNum, ogId);
    return itemId;
}

GetItemID Randomizer::GetItemFromActor(s16 actorId, s16 actorParams, s16 sceneNum, GetItemID ogItemId) {
    return GetItemFromGet(this->itemLocations[GetCheckFromActor(sceneNum, actorId, actorParams)], ogItemId);
}

GetItemID Randomizer::GetItemFromGet(RandomizerGet randoGet, GetItemID ogItemId) {
    switch (randoGet) {
        case RG_UNKNOWN_GET:
            return ogItemId;
        case RG_KOKIRI_SWORD:
            return GI_SWORD_KOKIRI;
        case RG_DEKU_SHIELD:
            return GI_SHIELD_DEKU;
        case RG_BOMBCHUS_20:
            return GI_BOMBCHUS_20;
        case RG_ICE_TRAP:
            return GI_ICE_TRAP;
        case RG_PIECE_OF_HEART:
            return GI_HEART_PIECE;
        case RG_BOMBS_5:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? GI_BOMBS_5 : GI_RUPEE_BLUE;
        case RG_BOMBS_10:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? GI_BOMBS_10 : GI_RUPEE_BLUE;
        case RG_BOMBS_20:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? GI_BOMBS_20 : GI_RUPEE_BLUE;
        case RG_DEKU_NUTS_5:
            return GI_NUTS_5;
        case RG_BOMBCHUS_10:
            return GI_BOMBCHUS_10;
        case RG_BOW:
            switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                case 0:
                    return GI_BOW;
                case 1:
                    return GI_QUIVER_40;
                case 2:
                    return GI_QUIVER_50;
            }
        case RG_SLINGSHOT:
            switch (CUR_UPG_VALUE(UPG_BULLET_BAG)) {
                case 0:
                    return GI_SLINGSHOT;
                case 1:
                    return GI_BULLET_BAG_40;
                case 2:
                    return GI_BULLET_BAG_50;
            }
        case RG_BOOMERANG:
            return GI_BOOMERANG;
        case RG_PROGRESSIVE_HOOKSHOT:
            switch (gSaveContext.inventory.items[SLOT_HOOKSHOT]) {
                case ITEM_NONE:
                    return GI_HOOKSHOT;
                case ITEM_HOOKSHOT:
                    return GI_LONGSHOT;
            }
        case RG_LENS_OF_TRUTH:
            return GI_LENS;
        case RG_ZELDAS_LETTER:
            return GI_LETTER_ZELDA;
        case RG_MEGATON_HAMMER:
            return GI_HAMMER;
        case RG_PRESCRIPTION:
            return GI_PRESCRIPTION;
        case RG_HYLIAN_SHIELD:
            return GI_SHIELD_HYLIAN;
        case RG_MIRROR_SHIELD:
            return GI_SHIELD_MIRROR;
        case RG_GORON_TUNIC:
            return GI_TUNIC_GORON;
        case RG_ZORA_TUNIC:
            return GI_TUNIC_ZORA;
        case RG_IRON_BOOTS:
            return GI_BOOTS_IRON;
        case RG_HOVER_BOOTS:
            return GI_BOOTS_HOVER;
        case RG_BOMB_BAG:
            switch (CUR_UPG_VALUE(UPG_BOMB_BAG)) {
                case ITEM_NONE:
                    return GI_BOMB_BAG_20;
                case ITEM_BOMB_BAG_20:
                    return GI_BOMB_BAG_30;
                case ITEM_BOMB_BAG_30:
                    return GI_BOMB_BAG_40;
                case 3:
                    return GI_RUPEE_BLUE;
            }
        case RG_PROGRESSIVE_STRENGTH_UPGRADE:
            switch (CUR_UPG_VALUE(UPG_STRENGTH)) {
                case 0:
                    return GI_BRACELET;
                case 1:
                    return GI_GAUNTLETS_SILVER;
                case 2:
                    return GI_GAUNTLETS_GOLD;
            }
        case RG_PROGRESSIVE_SCALE:
            switch (CUR_UPG_VALUE(UPG_SCALE)) {
                case 0:
                    return GI_SCALE_SILVER;
                case 1:
                    return GI_SCALE_GOLD;
            }
        case RG_STONE_OF_AGONY:
            return GI_STONE_OF_AGONY;
        case RG_GERUDO_MEMBERSHIP_CARD:
            return GI_GERUDO_CARD;
        case RG_HEART_CONTAINER:
            // todo figure out what GI_HEART_CONTAINER_2 is
            return GI_HEART_CONTAINER;
        // todo implement dungeon-specific keys/keyrings
        case RG_BOSS_KEY:
        case RG_BOSS_KEY_FIRE_TEMPLE:
        case RG_BOSS_KEY_FOREST_TEMPLE:
        case RG_BOSS_KEY_GANONS_CASTLE:
        case RG_BOSS_KEY_SHADOW_TEMPLE:
        case RG_BOSS_KEY_SPIRIT_TEMPLE:
        case RG_BOSS_KEY_WATER_TEMPLE:
            return GI_KEY_BOSS;
        case RG_SMALL_KEY:
        case RG_SMALL_KEY_BOTTOM_OF_THE_WELL:
        case RG_SMALL_KEY_FIRE_TEMPLE:
        case RG_SMALL_KEY_FOREST_TEMPLE:
        case RG_SMALL_KEY_GANONS_CASTLE:
        case RG_SMALL_KEY_GERUDO_TRAINING_GROUND:
        case RG_SMALL_KEY_SHADOW_TEMPLE:
        case RG_SMALL_KEY_SPIRIT_TEMPLE:
        case RG_SMALL_KEY_THIEVES_HIDEOUT:
        case RG_SMALL_KEY_WATER_TEMPLE:
            return GI_KEY_SMALL;
        // todo implement dungeon-specific maps/compasses
        case RG_MAP_DEKU_TREE:
        case RG_MAP_DODONGOS_CAVERN:
        case RG_MAP_JABU_JABUS_BELLY:
        case RG_MAP_FOREST_TEMPLE:
        case RG_MAP_FIRE_TEMPLE:
        case RG_MAP_WATER_TEMPLE:
        case RG_MAP_SPIRIT_TEMPLE:
        case RG_MAP_SHADOW_TEMPLE:
        case RG_MAP_BOTTOM_OF_THE_WELL:
        case RG_MAP_ICE_CAVERN:
            return GI_MAP;
        case RG_COMPASS_DEKU_TREE:
        case RG_COMPASS_DODONGOS_CAVERN:
        case RG_COMPASS_JABU_JABUS_BELLY:
        case RG_COMPASS_FOREST_TEMPLE:
        case RG_COMPASS_FIRE_TEMPLE:
        case RG_COMPASS_WATER_TEMPLE:
        case RG_COMPASS_SPIRIT_TEMPLE:
        case RG_COMPASS_SHADOW_TEMPLE:
        case RG_COMPASS_BOTTOM_OF_THE_WELL:
        case RG_COMPASS_ICE_CAVERN:
            return GI_COMPASS;
        case RG_MAGIC_METER:
            switch (gSaveContext.magicLevel) {
                case 0:
                    return GI_MAGIC_SMALL;
                case 1:
                    return GI_MAGIC_LARGE;
            }
        case RG_PROGRESSIVE_WALLET:
            switch (CUR_UPG_VALUE(UPG_WALLET)) {
                case 0:
                    return GI_WALLET_ADULT;
                case 1:
                    return GI_WALLET_GIANT;
            }
        case RG_RECOVERY_HEART:
            return GI_HEART;
        case RG_RUPEE_1:
            return GI_RUPEE_GREEN;
        case RG_RUPEES_5:
            return GI_RUPEE_BLUE;
        case RG_RUPEES_20:
            return GI_RUPEE_RED;
        case RG_RUPEES_50:
            return GI_RUPEE_PURPLE;
        case RG_RUPEES_200:
            return GI_RUPEE_GOLD;
        case RG_FIRE_ARROWS:
            return GI_ARROW_FIRE;
        case RG_ICE_ARROWS:
            return GI_ARROW_ICE;
        case RG_LIGHT_ARROWS:
            return GI_ARROW_LIGHT;
        case RG_DINS_FIRE:
            return GI_DINS_FIRE;
        case RG_FARORES_WIND:
            return GI_FARORES_WIND;
        case RG_NAYRUS_LOVE:
            return GI_NAYRUS_LOVE;
        case RG_DEKU_NUTS_10:
            return GI_NUTS_10;
        case RG_DEKU_SEEDS_30:
            return GI_SEEDS_30;
        case RG_BOTTLE_WITH_BIG_POE:
            return GI_BIG_POE;
        case RG_BOTTLE:
            return GI_BOTTLE;
        case RG_BOTTLE_WITH_BLUE_FIRE:
            return GI_BLUE_FIRE;
        case RG_BOTTLE_WITH_BLUE_POTION:
            return GI_POTION_BLUE;
        case RG_BOTTLE_WITH_BUGS:
            return GI_BUGS;
        case RG_BOTTLE_WITH_FAIRY:
            return GI_FAIRY;
        case RG_BOTTLE_WITH_FISH:
            return GI_FISH;
        case RG_BOTTLE_WITH_GREEN_POTION:
            return GI_POTION_GREEN;
        case RG_BOTTLE_WITH_MILK:
            return GI_MILK_BOTTLE;
        case RG_BOTTLE_WITH_POE:
            return GI_POE;
        case RG_BOTTLE_WITH_RED_POTION:
            return GI_POTION_RED;
        case RG_PIECE_OF_HEART_TREASURE_CHEST_GAME:
            return GI_HEART_PIECE_WIN;
        case RG_DEKU_STICK_CAPACITY:
            switch (CUR_UPG_VALUE(UPG_STICKS)) {
                case 0:
                    return GI_STICK_UPGRADE_20;
                case 1:
                    return GI_STICK_UPGRADE_30;
            }
        case RG_DEKU_NUT_CAPACITY:
            switch (CUR_UPG_VALUE(UPG_NUTS)) {
                case 0:
                    return GI_NUT_UPGRADE_30;
                case 1:
                    return GI_NUT_UPGRADE_40;
            }
        case RG_RUTOS_LETTER:
            return GI_LETTER_RUTO;
        case RG_ARROWS_5:
            return GI_ARROWS_SMALL;
        case RG_ARROWS_10:
            return GI_ARROWS_MEDIUM;
        case RG_ARROWS_30:
            return GI_ARROWS_LARGE;
        case RG_DEKU_STICK_1:
            return GI_STICKS_1;

        case RG_LIGHT_MEDALLION:
            return GI_MEDALLION_LIGHT;
        case RG_FOREST_MEDALLION:
            return GI_MEDALLION_FOREST;
        case RG_FIRE_MEDALLION:
            return GI_MEDALLION_FIRE;
        case RG_WATER_MEDALLION:
            return GI_MEDALLION_WATER;
        case RG_SHADOW_MEDALLION:
            return GI_MEDALLION_SHADOW;
        case RG_SPIRIT_MEDALLION:
            return GI_MEDALLION_SPIRIT;

        case RG_KOKIRI_EMERALD:
            return GI_STONE_KOKIRI;
        case RG_GORON_RUBY:
            return GI_STONE_GORON;
        case RG_ZORA_SAPPHIRE:
            return GI_STONE_ZORA;

        case RG_ZELDAS_LULLABY:
            return GI_ZELDAS_LULLABY;
        case RG_SUNS_SONG:
            return GI_SUNS_SONG;
        case RG_EPONAS_SONG:
            return GI_EPONAS_SONG;
        case RG_SONG_OF_STORMS:
            return GI_SONG_OF_STORMS;
        case RG_SONG_OF_TIME:
            return GI_SONG_OF_TIME;
        case RG_SARIAS_SONG:
            return GI_SARIAS_SONG;

        case RG_MINUET_OF_FOREST:
            return GI_MINUET_OF_FOREST;
        case RG_BOLERO_OF_FIRE:
            return GI_BOLERO_OF_FIRE;
        case RG_SERENADE_OF_WATER:
            return GI_SERENADE_OF_WATER;
        case RG_NOCTURNE_OF_SHADOW:
            return GI_NOCTURNE_OF_SHADOW;
        case RG_REQUIEM_OF_SPIRIT:
            return GI_REQUIEM_OF_SPIRIT;
        case RG_PRELUDE_OF_LIGHT:
            return GI_PRELUDE_OF_LIGHT;

        default:
            return ogItemId;
    }
}

GetItemID Randomizer::GetRandomizedItemIdFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId) {
    return GetItemFromGet(this->itemLocations[randomizerCheck], ogId);
}

RandomizerCheck Randomizer::GetCheckFromActor(s16 sceneNum, s16 actorId, s16 actorParams) {
    if (!gSaveContext.n64ddFlag) {
        return RC_UNKNOWN_CHECK;
    }

    switch(sceneNum) {
        case 82:
            switch (actorId) {
                case 316:
                    if (LINK_IS_ADULT) {
                        return RC_KAK_ANJU_AS_ADULT;
                    } else {
                        return RC_KAK_ANJU_AS_CHILD;
                    }
            }
        case 17:
            switch (actorId) {
                case 95:
                    return RC_DEKU_TREE_QUEEN_GOHMA_HEART;
                case 93:
                    return RC_QUEEN_GOHMA;
            }
        case 18:
            switch (actorId) {
                case 95:
                    return RC_DODONGOS_CAVERN_KING_DODONGO_HEART;
                case 93:
                    return RC_KING_DODONGO;
            }
        case 19:
            switch (actorId) {
                case 95:
                    return RC_JABU_JABUS_BELLY_BARINADE_HEART;
                case 93:
                    return RC_BARINADE;
            }
        case 20:
            switch (actorId) {
                case 95:
                    return RC_FOREST_TEMPLE_PHANTOM_GANON_HEART;
                case 93:
                    return RC_PHANTOM_GANON;
            }
        case 21:
            switch (actorId) {
                case 95:
                    return RC_FIRE_TEMPLE_VOLVAGIA_HEART;
                case 93:
                    return RC_VOLVAGIA;
            }
        case 22:
            switch (actorId) {
                case 95:
                    return RC_WATER_TEMPLE_MORPHA_HEART;
                case 93:
                    return RC_MORPHA;
            }
        case 23:
            switch (actorId) {
                case 95:
                    return RC_SPIRIT_TEMPLE_TWINROVA_HEART;
                case 93:
                    return RC_TWINROVA;
            }
        case 24:
            switch (actorId) {
                case 95:
                    return RC_SHADOW_TEMPLE_BONGO_BONGO_HEART;
                case 93:
                    return RC_BONGO_BONGO;
            }
        case 40:
            switch (actorParams) {
                case 22944:
                    return RC_KF_MIDOS_TOP_LEFT_CHEST;
                case 22945:
                    return RC_KF_MIDOS_TOP_RIGHT_CHEST;
                case 22914:
                    return RC_KF_MIDOS_BOTTOM_LEFT_CHEST;
                case 22787:
                    return RC_KF_MIDOS_BOTTOM_RIGHT_CHEST;
            }
        case 84:
            switch (actorParams) {
                case 1030:
                    return RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH;
                case 2822:
                    return RC_ZR_NEAR_DOMAIN_FREESTANDING_POH;
            }
        case 85:
            switch (actorParams) {
                case 1248:
                    return RC_KF_KOKIRI_SWORD_CHEST;
            }
        case 62:
            switch (actorParams) {
                case 22988:
                    return RC_KF_STORMS_GROTTO_CHEST;
                case 22964:
                    return RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST;
                case 31409:
                    return RC_SFM_WOLFOS_GROTTO_CHEST;
                case 22944:
                    return RC_HF_NEAR_MARKET_GROTTO_CHEST;
                case 22978:
                    return RC_HF_SOUTHEAST_GROTTO_CHEST;
                case 22947:
                    return RC_HF_OPEN_GROTTO_CHEST;
                case 22984:
                    return RC_KAK_OPEN_GROTTO_CHEST;
                case 31434:
                    return RC_KAK_REDEAD_GROTTO_CHEST;
                case 23255:
                    return RC_DMT_STORMS_GROTTO_CHEST;
                case 23802:
                    return RC_DMC_UPPER_GROTTO_CHEST;
                case 22985:
                    return RC_ZR_OPEN_GROTTO_CHEST;
                case 262:
                    return RC_HF_TEKTITE_GROTTO_FREESTANDING_POH;
            }
        case 64:
            switch (actorParams) {
                case 21824:
                    return RC_GRAVEYARD_SHIELD_GRAVE_CHEST;
            }
        case 63:
            switch (actorParams) {
                case -22592:
                    return RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST;
            }
        case 65:
            switch (actorParams) {
                case -32736:
                    return RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST;
            }
        case 72:
            // todo make sure dampe gives the PoH when the chest is opened
            // default logic seems to be give PoH when hookshot in inventory
            switch (actorParams) {
                case 4352:
                    return RC_GRAVEYARD_HOOKSHOT_CHEST;
                case 262:
                    return RC_KAK_WINDMILL_FREESTANDING_POH;
                case 1798:
                    return RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH;
            }
        case 96:
            switch (actorParams) {
                case 7686:
                    return RC_DMT_FREESTANDING_POH;
                case 23201:
                    return RC_DMT_CHEST;
            }
        case 98:
            switch (actorParams) {
                case 23232:
                    return RC_GC_MAZE_LEFT_CHEST;
                case 23201:
                    return RC_GC_MAZE_RIGHT_CHEST;
                case 23202:
                    return RC_GC_MAZE_CENTER_CHEST;
                case 7942:
                    return RC_GC_POT_FREESTANDING_POH;
            }
        case 88:
            switch (actorParams) {
                case -18496:
                    return RC_ZD_CHEST;
            }
        case 90:
            switch (actorParams) {
                case 23200:
                    return RC_GV_CHEST;
                case 262:
                    return RC_GV_WATERFALL_FREESTANDING_POH;
                case 518:
                    return RC_GV_CRATE_FREESTANDING_POH;
            }
        case 93:
            switch (actorParams) {
                case 1984:
                    return RC_GF_CHEST;
            }
        case 94:
            switch (actorParams) {
                case -30048:
                    return RC_WASTELAND_CHEST;
            }
        case 0:
            switch (actorParams) {
                case 2083:
                    return RC_DEKU_TREE_MAP_CHEST;
                case 22789:
                    return RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST;
                case 161:
                    return RC_DEKU_TREE_SLINGSHOT_CHEST;
                case 2050:
                    return RC_DEKU_TREE_COMPASS_CHEST;
                case 22790:
                    return RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST;
                case 22788:
                    return RC_DEKU_TREE_BASEMENT_CHEST;
            }
        case 1:
            switch (actorParams) {
                case 2088:
                    return RC_DODONGOS_CAVERN_MAP_CHEST;
                case 2053:
                    return RC_DODONGOS_CAVERN_COMPASS_CHEST;
                case 22982:
                    return RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST;
                case 1604:
                    return RC_DODONGOS_CAVERN_BOMB_BAG_CHEST;
                case 21802:
                    return RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST;
                case 20512:
                    return RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST;
            }
        case 2:
            switch (actorParams) {
                case 4289:
                    return RC_JABU_JABUS_BELLY_BOOMERANG_CHEST;
                case 6178:
                    return RC_JABU_JABUS_BELLY_MAP_CHEST;
                case -18428:
                    return RC_JABU_JABUS_BELLY_COMPASS_CHEST;
            }
        case 8:
            switch (actorParams) {
                case 22600:
                    return RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST;
                case 20578:
                    return RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST;
                case 23684:
                    return RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST;
                case 22793:
                    return RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST;
                case 2049:
                    return RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST;
                case 20558:
                    return RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST;
                case 22597:
                    return RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST;
                case 21802:
                    return RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST;
                case 21836:
                    return RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST;
                case 2087:
                    return RC_BOTTOM_OF_THE_WELL_MAP_CHEST;
                case 23760:
                    return RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST;
                case 4419:
                    return RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST;
                case 27348:
                    return RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST;
            }
        case 3:
            switch (actorParams) {
                case 22595:
                    return RC_FOREST_TEMPLE_FIRST_ROOM_CHEST;
                case 30784:
                    return RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST;
                case 22789:
                    return RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST;
                case 6177:
                    return RC_FOREST_TEMPLE_MAP_CHEST;
                case 22601:
                    return RC_FOREST_TEMPLE_WELL_CHEST;
                case -30364:
                    return RC_FOREST_TEMPLE_EYE_SWITCH_CHEST;
                case 10222:
                    return RC_FOREST_TEMPLE_BOSS_KEY_CHEST;
                case 30786:
                    return RC_FOREST_TEMPLE_FLOORMASTER_CHEST;
                case 30797:
                    return RC_FOREST_TEMPLE_RED_POE_CHEST;
                case -20340:
                    return RC_FOREST_TEMPLE_BOW_CHEST;
                case 6159:
                    return RC_FOREST_TEMPLE_BLUE_POE_CHEST;
                case 22855:
                    return RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST;
                case 22827:
                    return RC_FOREST_TEMPLE_BASEMENT_CHEST;
            }
        case 4:
            switch (actorParams) {
                case 22593:
                    return RC_FIRE_TEMPLE_NEAR_BOSS_CHEST;
                case 31936:
                    return RC_FIRE_TEMPLE_FLARE_DANCER_CHEST;
                case 10220:
                    return RC_FIRE_TEMPLE_BOSS_KEY_CHEST;
                case 22596:
                    return RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST;
                case 22594:
                    return RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST;
                case 22595:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST;
                case 22600:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST;
                case 2090:
                    return RC_FIRE_TEMPLE_MAP_CHEST;
                case 22603:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST;
                case 22598:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST;
                case 23245:
                    return RC_FIRE_TEMPLE_SCARECROW_CHEST;
                case 2055:
                    return RC_FIRE_TEMPLE_COMPASS_CHEST;
                case 421:
                    return RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST;
                case 22601:
                    return RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST;
            }
        case 5:
            switch (actorParams) {
                case 2057:
                    return RC_WATER_TEMPLE_COMPASS_CHEST;
                case 6178:
                    return RC_WATER_TEMPLE_MAP_CHEST;
                case 22592:
                    return RC_WATER_TEMPLE_CRACKED_WALL_CHEST;
                case 30785:
                    return RC_WATER_TEMPLE_TORCHES_CHEST;
                case 10213:
                    return RC_WATER_TEMPLE_BOSS_KEY_CHEST;
                case 22598:
                    return RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST;
                case 22600:
                    return RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST;
                case 295:
                    return RC_WATER_TEMPLE_LONGSHOT_CHEST;
                case 22595:
                    return RC_WATER_TEMPLE_RIVER_CHEST;
                case 22602:
                    return RC_WATER_TEMPLE_DRAGON_CHEST;
            }
        case 7:
            switch (actorParams) {
                case 6177:
                    return RC_SHADOW_TEMPLE_MAP_CHEST;
                case 5607:
                    return RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST;
                case 6147:
                    return RC_SHADOW_TEMPLE_COMPASS_CHEST;
                case 22594:
                    return RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST;
                case 22668:
                    return RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST;
                case 26998:
                    return RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST;
                case 22853:
                    return RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST;
                case 22662:
                    return RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST;
                case -30652:
                    return RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST;
                case 30857:
                    return RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST;
                case 26965:
                    return RC_SHADOW_TEMPLE_WIND_HINT_CHEST;
                case 30856:
                    return RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST;
                case 26708:
                    return RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST;
                case 22666:
                    return RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST;
                case 10219:
                    return RC_SHADOW_TEMPLE_BOSS_KEY_CHEST;
                case 30797:
                    return RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST;
            }
        case 6:
            switch (actorParams) {
                case 21800:
                    return RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST;
                case -30656:
                    return RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST;
                case -32666:
                    return RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST;
                case -31444:
                    return RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST;
                case -18397:
                    return RC_SPIRIT_TEMPLE_MAP_CHEST;
                case -30655:
                    return RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST;
                case 14340:
                    return RC_SPIRIT_TEMPLE_COMPASS_CHEST;
                case 22599:
                    return RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST;
                case -30451:
                    return RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST;
                case -28786:
                    return RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST;
                case -30577:
                    return RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST;
                case -30654:
                    return RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST;
                case 22597:
                    return RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST;
                case 26900:
                    return RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST;
                case 26901:
                    return RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST;
                case 10218:
                    return RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST;
                case -29454:
                    return RC_SPIRIT_TEMPLE_TOPMOST_CHEST;
            }
        case 92:
            switch (actorParams) {
                case 1707:
                    return RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST;
                case 13673:
                    return RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST;
                case 3334:
                    return RC_COLOSSUS_FREESTANDING_POH;
            }
        case 9:
            switch (actorParams) {
                case 2080:
                    return RC_ICE_CAVERN_MAP_CHEST;
                case 2049:
                    return RC_ICE_CAVERN_COMPASS_CHEST;
                case 5570:
                    return RC_ICE_CAVERN_IRON_BOOTS_CHEST;
                case 262:
                    return RC_ICE_CAVERN_FREESTANDING_POH;
            }
        case 11:
            switch (actorParams) {
                case -30573:
                    return RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST;
                case -30393:
                    return RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST;
                case -30656:
                    return RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST;
                case 31089:
                    return RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST;
                case 31439:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST;
                case 30862:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST;
                case 26708:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST;
                case 24450:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST;
                case -30653:
                    return RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST;
                case 22596:
                    return RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST;
                case 31058:
                    return RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST;
                case 22608:
                    return RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST;
                case 23877:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST;
                case 22888:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST;
                case -30643:
                    return RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST;
                case -30655:
                    return RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST;
                case 22603:
                    return RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST;
                case 23206:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST;
                case 22986:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST;
                case 22889:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST;
                case 2860:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST;
            }
        case 13:
            switch (actorParams) {
                case 30857:
                    return RC_GANONS_CASTLE_FOREST_TRIAL_CHEST;
                case 24455:
                    return RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST;
                case 22790:
                    return RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST;
                case 22664:
                    return RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST;
                case 14021:
                    return RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST;
                case 22668:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST;
                case 24459:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST;
                case 22797:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST;
                case 24462:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST;
                case 22890:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST;
                case 24463:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST;
                case 30800:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST;
                case -30639:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST;
                case -29326:
                    return RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST;
                case 26964:
                    return RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST;
            }
        case 10:
            switch (actorParams) {
                case 10219:
                    return RC_GANONS_TOWER_BOSS_KEY_CHEST;
            }
        case 87:
            switch (actorId) {
                case 271:
                    if (LINK_IS_ADULT) {
                        return RC_LH_SUN;
                    } else {
                        return RC_LH_UNDERWATER_ITEM;
                    }
            }
            switch(actorParams) {
                case 7686:
                    return RC_LH_FREESTANDING_POH;
            }
        case 76:
            switch (actorParams) {
                case 262:
                    return RC_LLR_FREESTANDING_POH;
            }
        case 55:
            switch (actorParams) {
                case 262:
                    return RC_KAK_IMPAS_HOUSE_FREESTANDING_POH;
            }
        case 83:
            switch (actorParams) {
                case 1030:
                    return RC_GRAVEYARD_FREESTANDING_POH;
            }
        case 97:
            switch (actorParams) {
                case 2054:
                    return RC_DMC_VOLCANO_FREESTANDING_POH;
                case 518:
                    return RC_DMC_WALL_FREESTANDING_POH;
            }
        case 89:
            switch (actorParams) {
                case 5126:
                    return RC_ZF_BOTTOM_FREESTANDING_POH;
                case 262:
                    return RC_ZF_ICEBERC_FREESTANDING_POH;
            }
        case 59:
            // using fountainType as actorParams
            switch (actorParams) {
                case 1:
                    return RC_DMT_GREAT_FAIRY_REWARD;
                case 2:
                    return RC_DMC_GREAT_FAIRY_REWARD;
                case 3:
                    return RC_OGC_GREAT_FAIRY_REWARD;
            }
        case 61:
            // using fountainType as actorParams
            switch (actorParams) {
                case 1:
                    return RC_ZF_GREAT_FAIRY_REWARD;
                case 2:
                    return RC_HC_GREAT_FAIRY_REWARD;
                case 3:
                    return RC_COLOSSUS_GREAT_FAIRY_REWARD;
            }
    }

    return RC_UNKNOWN_CHECK;
}
