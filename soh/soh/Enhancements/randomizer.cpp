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
    {"Links Pocket", LINKS_POCKET},
    {"Queen Gohma", QUEEN_GOHMA},
    {"King Dodongo", KING_DODONGO},
    {"Barinade", BARINADE},
    {"Phantom Ganon", PHANTOM_GANON},
    {"Volvagia", VOLVAGIA},
    {"Morpha", MORPHA},
    {"Bongo Bongo", BONGO_BONGO},
    {"Twinrova", TWINROVA},
    {"Ganon", GANON},
    {"Gift from Sages", GIFT_FROM_SAGES},
    {"Song from Impa", SONG_FROM_IMPA},
    {"Song from Malon", SONG_FROM_MALON},
    {"Song from Saria", SONG_FROM_SARIA},
    {"Song from Royal Familys Tomb", SONG_FROM_ROYAL_FAMILYS_TOMB},
    {"Song from Ocarina of Time", SONG_FROM_OCARINA_OF_TIME},
    {"Song from Windmill", SONG_FROM_WINDMILL},
    {"Sheik in Forest", SHEIK_IN_FOREST},
    {"Sheik in Crater", SHEIK_IN_CRATER},
    {"Sheik in Ice Cavern", SHEIK_IN_ICE_CAVERN},
    {"Sheik at Colossus", SHEIK_AT_COLOSSUS},
    {"Sheik in Kakariko", SHEIK_IN_KAKARIKO},
    {"Sheik at Temple", SHEIK_AT_TEMPLE},
    {"KF Midos Top Left Chest", KF_MIDOS_TOP_LEFT_CHEST},
    {"KF Midos Top Right Chest", KF_MIDOS_TOP_RIGHT_CHEST},
    {"KF Midos Bottom Left Chest", KF_MIDOS_BOTTOM_LEFT_CHEST},
    {"KF Midos Bottom Right Chest", KF_MIDOS_BOTTOM_RIGHT_CHEST},
    {"KF Kokiri Sword Chest", KF_KOKIRI_SWORD_CHEST},
    {"KF Storms Grotto Chest", KF_STORMS_GROTTO_CHEST},
    {"KF Links House Cow", KF_LINKS_HOUSE_COW},
    {"KF GS Know It All House", KF_GS_KNOW_IT_ALL_HOUSE},
    {"KF GS Bean Patch", KF_GS_BEAN_PATCH},
    {"KF GS House of Twins", KF_GS_HOUSE_OF_TWINS},
    {"KF Shop Item 1", KF_SHOP_ITEM_1},
    {"KF Shop Item 2", KF_SHOP_ITEM_2},
    {"KF Shop Item 3", KF_SHOP_ITEM_3},
    {"KF Shop Item 4", KF_SHOP_ITEM_4},
    {"KF Shop Item 5", KF_SHOP_ITEM_5},
    {"KF Shop Item 6", KF_SHOP_ITEM_6},
    {"KF Shop Item 7", KF_SHOP_ITEM_7},
    {"KF Shop Item 8", KF_SHOP_ITEM_8},
    {"LW Gift from Saria", LW_GIFT_FROM_SARIA},
    {"LW Ocarina Memory Game", LW_OCARINA_MEMORY_GAME},
    {"LW Target in Woods", LW_TARGET_IN_WOODS},
    {"LW Near Shortcuts Grotto Chest", LW_NEAR_SHORTCUTS_GROTTO_CHEST},
    {"Deku Theater Skull Mask", DEKU_THEATER_SKULL_MASK},
    {"Deku Theater Mask of Truth", DEKU_THEATER_MASK_OF_TRUTH},
    {"LW Skull Kid", LW_SKULL_KID},
    {"LW Deku Scrub Near Bridge", LW_DEKU_SCRUB_NEAR_BRIDGE},
    {"LW Deku Scrub Near Deku Theater Left", LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT},
    {"LW Deku Scrub Near Deku Theater Right", LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT},
    {"LW Deku Scrub Grotto Front", LW_DEKU_SCRUB_GROTTO_FRONT},
    {"LW Deku Scrub Grotto Rear", LW_DEKU_SCRUB_GROTTO_REAR},
    {"LW GS Bean Patch Near Bridge", LW_GS_BEAN_PATCH_NEAR_BRIDGE},
    {"LW GS Bean Patch Near Theater", LW_GS_BEAN_PATCH_NEAR_THEATER},
    {"LW GS Above Theater", LW_GS_ABOVE_THEATER},
    {"SFM Wolfos Grotto Chest", SFM_WOLFOS_GROTTO_CHEST},
    {"SFM Deku Scrub Grotto Front", SFM_DEKU_SCRUB_GROTTO_FRONT},
    {"SFM Deku Scrub Grotto Rear", SFM_DEKU_SCRUB_GROTTO_REAR},
    {"SFM GS", SFM_GS},
    {"HF Ocarina of Time Item", HF_OCARINA_OF_TIME_ITEM},
    {"HF Near Market Grotto Chest", HF_NEAR_MARKET_GROTTO_CHEST},
    {"HF Tektite Grotto Freestanding PoH", HF_TEKTITE_GROTTO_FREESTANDING_POH},
    {"HF Southeast Grotto Chest", HF_SOUTHEAST_GROTTO_CHEST},
    {"HF Open Grotto Chest", HF_OPEN_GROTTO_CHEST},
    {"HF Deku Scrub Grotto", HF_DEKU_SCRUB_GROTTO},
    {"HF Cow Grotto Cow", HF_COW_GROTTO_COW},
    {"HF GS Cow Grotto", HF_GS_COW_GROTTO},
    {"HF GS Near Kak Grotto", HF_GS_NEAR_KAK_GROTTO},
    {"Market Shooting Gallery Reward", MARKET_SHOOTING_GALLERY_REWARD},
    {"Market Bombchu Bowling First Prize", MARKET_BOMBCHU_BOWLING_FIRST_PRIZE},
    {"Market Bombchu Bowling Second Prize", MARKET_BOMBCHU_BOWLING_SECOND_PRIZE},
    {"Market Bombchu Bowling Bombchus", MARKET_BOMBCHU_BOWLING_BOMBCHUS},
    {"Market Lost Dog", MARKET_LOST_DOG},
    {"Market Treasure Chest Game Reward", MARKET_TREASURE_CHEST_GAME_REWARD},
    {"Market 10 Big Poes", MARKET_10_BIG_POES},
    {"Market GS Guard House", MARKET_GS_GUARD_HOUSE},
    {"Market Bazaar Item 1", MARKET_BAZAAR_ITEM_1},
    {"Market Bazaar Item 2", MARKET_BAZAAR_ITEM_2},
    {"Market Bazaar Item 3", MARKET_BAZAAR_ITEM_3},
    {"Market Bazaar Item 4", MARKET_BAZAAR_ITEM_4},
    {"Market Bazaar Item 5", MARKET_BAZAAR_ITEM_5},
    {"Market Bazaar Item 6", MARKET_BAZAAR_ITEM_6},
    {"Market Bazaar Item 7", MARKET_BAZAAR_ITEM_7},
    {"Market Bazaar Item 8", MARKET_BAZAAR_ITEM_8},
    {"Market Potion Shop Item 1", MARKET_POTION_SHOP_ITEM_1},
    {"Market Potion Shop Item 2", MARKET_POTION_SHOP_ITEM_2},
    {"Market Potion Shop Item 3", MARKET_POTION_SHOP_ITEM_3},
    {"Market Potion Shop Item 4", MARKET_POTION_SHOP_ITEM_4},
    {"Market Potion Shop Item 5", MARKET_POTION_SHOP_ITEM_5},
    {"Market Potion Shop Item 6", MARKET_POTION_SHOP_ITEM_6},
    {"Market Potion Shop Item 7", MARKET_POTION_SHOP_ITEM_7},
    {"Market Potion Shop Item 8", MARKET_POTION_SHOP_ITEM_8},
    {"Market Bombchu Shop Item 1", MARKET_BOMBCHU_SHOP_ITEM_1},
    {"Market Bombchu Shop Item 2", MARKET_BOMBCHU_SHOP_ITEM_2},
    {"Market Bombchu Shop Item 3", MARKET_BOMBCHU_SHOP_ITEM_3},
    {"Market Bombchu Shop Item 4", MARKET_BOMBCHU_SHOP_ITEM_4},
    {"Market Bombchu Shop Item 5", MARKET_BOMBCHU_SHOP_ITEM_5},
    {"Market Bombchu Shop Item 6", MARKET_BOMBCHU_SHOP_ITEM_6},
    {"Market Bombchu Shop Item 7", MARKET_BOMBCHU_SHOP_ITEM_7},
    {"Market Bombchu Shop Item 8", MARKET_BOMBCHU_SHOP_ITEM_8},
    {"ToT Light Arrows Cutscene", TOT_LIGHT_ARROWS_CUTSCENE},
    {"HC Malon Egg", HC_MALON_EGG},
    {"HC Zeldas Letter", HC_ZELDAS_LETTER},
    {"HC Great Fairy Reward", HC_GREAT_FAIRY_REWARD},
    {"HC GS Tree", HC_GS_TREE},
    {"HC GS Storms Grotto", HC_GS_STORMS_GROTTO},
    {"LLR Talons Chickens", LLR_TALONS_CHICKENS},
    {"LLR Freestanding PoH", LLR_FREESTANDING_POH},
    {"LLR Deku Scrub Grotto Left", LLR_DEKU_SCRUB_GROTTO_LEFT},
    {"LLR Deku Scrub Grotto Center", LLR_DEKU_SCRUB_GROTTO_CENTER},
    {"LLR Deku Scrub Grotto Right", LLR_DEKU_SCRUB_GROTTO_RIGHT},
    {"LLR Stables Left Cow", LLR_STABLES_LEFT_COW},
    {"LLR Stables Right Cow", LLR_STABLES_RIGHT_COW},
    {"LLR Tower Left Cow", LLR_TOWER_LEFT_COW},
    {"LLR Tower Right Cow", LLR_TOWER_RIGHT_COW},
    {"LLR GS House Window", LLR_GS_HOUSE_WINDOW},
    {"LLR GS Tree", LLR_GS_TREE},
    {"LLR GS Rain Shed", LLR_GS_RAIN_SHED},
    {"LLR GS Back Wall", LLR_GS_BACK_WALL},
    {"Kak Anju as Child", KAK_ANJU_AS_CHILD},
    {"Kak Anju as Adult", KAK_ANJU_AS_ADULT},
    {"Kak Impas House Freestanding PoH", KAK_IMPAS_HOUSE_FREESTANDING_POH},
    {"Kak Windmill Freestanding PoH", KAK_WINDMILL_FREESTANDING_POH},
    {"Kak Man on Roof", KAK_MAN_ON_ROOF},
    {"Kak Open Grotto Chest", KAK_OPEN_GROTTO_CHEST},
    {"Kak Redead Grotto Chest", KAK_REDEAD_GROTTO_CHEST},
    {"Kak Shooting Gallery Reward", KAK_SHOOTING_GALLERY_REWARD},
    {"Kak 10 Gold Skulltula Reward", KAK_10_GOLD_SKULLTULA_REWARD},
    {"Kak 20 Gold Skulltula Reward", KAK_20_GOLD_SKULLTULA_REWARD},
    {"Kak 30 Gold Skulltula Reward", KAK_30_GOLD_SKULLTULA_REWARD},
    {"Kak 40 Gold Skulltula Reward", KAK_40_GOLD_SKULLTULA_REWARD},
    {"Kak 50 Gold Skulltula Reward", KAK_50_GOLD_SKULLTULA_REWARD},
    {"Kak Impas House Cow", KAK_IMPAS_HOUSE_COW},
    {"Kak GS Tree", KAK_GS_TREE},
    {"Kak GS Guards House", KAK_GS_GUARDS_HOUSE},
    {"Kak GS Watchtower", KAK_GS_WATCHTOWER},
    {"Kak GS Skulltula House", KAK_GS_SKULLTULA_HOUSE},
    {"Kak GS House Under Construction", KAK_GS_HOUSE_UNDER_CONSTRUCTION},
    {"Kak GS Above Impas House", KAK_GS_ABOVE_IMPAS_HOUSE},
    {"Kak Bazaar Item 1", KAK_BAZAAR_ITEM_1},
    {"Kak Bazaar Item 2", KAK_BAZAAR_ITEM_2},
    {"Kak Bazaar Item 3", KAK_BAZAAR_ITEM_3},
    {"Kak Bazaar Item 4", KAK_BAZAAR_ITEM_4},
    {"Kak Bazaar Item 5", KAK_BAZAAR_ITEM_5},
    {"Kak Bazaar Item 6", KAK_BAZAAR_ITEM_6},
    {"Kak Bazaar Item 7", KAK_BAZAAR_ITEM_7},
    {"Kak Bazaar Item 8", KAK_BAZAAR_ITEM_8},
    {"Kak Potion Shop Item 1", KAK_POTION_SHOP_ITEM_1},
    {"Kak Potion Shop Item 2", KAK_POTION_SHOP_ITEM_2},
    {"Kak Potion Shop Item 3", KAK_POTION_SHOP_ITEM_3},
    {"Kak Potion Shop Item 4", KAK_POTION_SHOP_ITEM_4},
    {"Kak Potion Shop Item 5", KAK_POTION_SHOP_ITEM_5},
    {"Kak Potion Shop Item 6", KAK_POTION_SHOP_ITEM_6},
    {"Kak Potion Shop Item 7", KAK_POTION_SHOP_ITEM_7},
    {"Kak Potion Shop Item 8", KAK_POTION_SHOP_ITEM_8},
    {"Graveyard Shield Grave Chest", GRAVEYARD_SHIELD_GRAVE_CHEST},
    {"Graveyard Heart Piece Grave Chest", GRAVEYARD_HEART_PIECE_GRAVE_CHEST},
    {"Graveyard Royal Familys Tomb Chest", GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST},
    {"Graveyard Freestanding PoH", GRAVEYARD_FREESTANDING_POH},
    {"Graveyard Dampe Gravedigging Tour", GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR},
    {"Graveyard Hookshot Chest", GRAVEYARD_HOOKSHOT_CHEST},
    {"Graveyard Dampe Race Freestanding PoH", GRAVEYARD_DAMPE_RACE_FREESTANDING_POH},
    {"Graveyard GS Bean Patch", GRAVEYARD_GS_BEAN_PATCH},
    {"Graveyard GS Wall", GRAVEYARD_GS_WALL},
    {"DMT Freestanding PoH", DMT_FREESTANDING_POH},
    {"DMT Chest", DMT_CHEST},
    {"DMT Storms Grotto Chest", DMT_STORMS_GROTTO_CHEST},
    {"DMT Great Fairy Reward", DMT_GREAT_FAIRY_REWARD},
    {"DMT Biggoron", DMT_BIGGORON},
    {"DMT Cow Grotto Cow", DMT_COW_GROTTO_COW},
    {"DMT GS Near Kak", DMT_GS_NEAR_KAK},
    {"DMT GS Bean Patch", DMT_GS_BEAN_PATCH},
    {"DMT GS Above Dodongos Cavern", DMT_GS_ABOVE_DODONGOS_CAVERN},
    {"DMT GS Falling Rocks Path", DMT_GS_FALLING_ROCKS_PATH},
    {"GC Darunias Joy", GC_DARUNIAS_JOY},
    {"GC Pot Freestanding PoH", GC_POT_FREESTANDING_POH},
    {"GC Rolling Goron as Child", GC_ROLLING_GORON_AS_CHILD},
    {"GC Rolling Goron as Adult", GC_ROLLING_GORON_AS_ADULT},
    {"GC Medigoron", GC_MEDIGORON},
    {"GC Maze Left Chest", GC_MAZE_LEFT_CHEST},
    {"GC Maze Right Chest", GC_MAZE_RIGHT_CHEST},
    {"GC Maze Center Chest", GC_MAZE_CENTER_CHEST},
    {"GC Deku Scrub Grotto Left", GC_DEKU_SCRUB_GROTTO_LEFT},
    {"GC Deku Scrub Grotto Center", GC_DEKU_SCRUB_GROTTO_CENTER},
    {"GC Deku Scrub Grotto Right", GC_DEKU_SCRUB_GROTTO_RIGHT},
    {"GC GS Center Platform", GC_GS_CENTER_PLATFORM},
    {"GC GS Boulder Maze", GC_GS_BOULDER_MAZE},
    {"GC Shop Item 1", GC_SHOP_ITEM_1},
    {"GC Shop Item 2", GC_SHOP_ITEM_2},
    {"GC Shop Item 3", GC_SHOP_ITEM_3},
    {"GC Shop Item 4", GC_SHOP_ITEM_4},
    {"GC Shop Item 5", GC_SHOP_ITEM_5},
    {"GC Shop Item 6", GC_SHOP_ITEM_6},
    {"GC Shop Item 7", GC_SHOP_ITEM_7},
    {"GC Shop Item 8", GC_SHOP_ITEM_8},
    {"DMC Volcano Freestanding PoH", DMC_VOLCANO_FREESTANDING_POH},
    {"DMC Wall Freestanding PoH", DMC_WALL_FREESTANDING_POH},
    {"DMC Upper Grotto Chest", DMC_UPPER_GROTTO_CHEST},
    {"DMC Great Fairy Reward", DMC_GREAT_FAIRY_REWARD},
    {"DMC Deku Scrub", DMC_DEKU_SCRUB},
    {"DMC Deku Scrub Grotto Left", DMC_DEKU_SCRUB_GROTTO_LEFT},
    {"DMC Deku Scrub Grotto Center", DMC_DEKU_SCRUB_GROTTO_CENTER},
    {"DMC Deku Scrub Grotto Right", DMC_DEKU_SCRUB_GROTTO_RIGHT},
    {"DMC GS Crate", DMC_GS_CRATE},
    {"DMC GS Bean Patch", DMC_GS_BEAN_PATCH},
    {"ZR Magic Bean Salesman", ZR_MAGIC_BEAN_SALESMAN},
    {"ZR Open Grotto Chest", ZR_OPEN_GROTTO_CHEST},
    {"ZR Frogs Zeldas Lullaby", ZR_FROGS_ZELDAS_LULLABY},
    {"ZR Frogs Eponas Song", ZR_FROGS_EPONAS_SONG},
    {"ZR Frogs Sarias Song", ZR_FROGS_SARIAS_SONG},
    {"ZR Frogs Suns Song", ZR_FROGS_SUNS_SONG},
    {"ZR Frogs Song of Time", ZR_FROGS_SONG_OF_TIME},
    {"ZR Frogs in the Rain", ZR_FROGS_IN_THE_RAIN},
    {"ZR Frogs Ocarina Game", ZR_FROGS_OCARINA_GAME},
    {"ZR Near Open Grotto Freestanding PoH", ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH},
    {"ZR Near Domain Freestanding PoH", ZR_NEAR_DOMAIN_FREESTANDING_POH},
    {"ZR Deku Scrub Grotto Front", ZR_DEKU_SCRUB_GROTTO_FRONT},
    {"ZR Deku Scrub Grotto Rear", ZR_DEKU_SCRUB_GROTTO_REAR},
    {"ZR GS Tree", ZR_GS_TREE},
    {"ZR GS Ladder", ZR_GS_LADDER},
    {"ZR GS Near Raised Grottos", ZR_GS_NEAR_RAISED_GROTTOS},
    {"ZR GS Above Bridge", ZR_GS_ABOVE_BRIDGE},
    {"ZD Diving Minigame", ZD_DIVING_MINIGAME},
    {"ZD Chest", ZD_CHEST},
    {"ZD King Zora Thawed", ZD_KING_ZORA_THAWED},
    {"ZD GS Frozen Waterfall", ZD_GS_FROZEN_WATERFALL},
    {"ZD Shop Item 1", ZD_SHOP_ITEM_1},
    {"ZD Shop Item 2", ZD_SHOP_ITEM_2},
    {"ZD Shop Item 3", ZD_SHOP_ITEM_3},
    {"ZD Shop Item 4", ZD_SHOP_ITEM_4},
    {"ZD Shop Item 5", ZD_SHOP_ITEM_5},
    {"ZD Shop Item 6", ZD_SHOP_ITEM_6},
    {"ZD Shop Item 7", ZD_SHOP_ITEM_7},
    {"ZD Shop Item 8", ZD_SHOP_ITEM_8},
    {"ZF Great Fairy Reward", ZF_GREAT_FAIRY_REWARD},
    {"ZF Iceberg Freestanding PoH", ZF_ICEBERG_FREESTANDING_POH},
    {"ZF Bottom Freestanding PoH", ZF_BOTTOM_FREESTANDING_POH},
    {"ZF GS Above the Log", ZF_GS_ABOVE_THE_LOG},
    {"ZF GS Tree", ZF_GS_TREE},
    {"ZF GS Hidden Cave", ZF_GS_HIDDEN_CAVE},
    {"LH Underwater Item", LH_UNDERWATER_ITEM},
    {"LH Child Fishing", LH_CHILD_FISHING},
    {"LH Adult Fishing", LH_ADULT_FISHING},
    {"LH Lab Dive", LH_LAB_DIVE},
    {"LH Freestanding PoH", LH_FREESTANDING_POH},
    {"LH Sun", LH_SUN},
    {"LH Deku Scrub Grotto Left", LH_DEKU_SCRUB_GROTTO_LEFT},
    {"LH Deku Scrub Grotto Center", LH_DEKU_SCRUB_GROTTO_CENTER},
    {"LH Deku Scrub Grotto Right", LH_DEKU_SCRUB_GROTTO_RIGHT},
    {"LH GS Bean Patch", LH_GS_BEAN_PATCH},
    {"LH GS Lab Wall", LH_GS_LAB_WALL},
    {"LH GS Small Island", LH_GS_SMALL_ISLAND},
    {"LH GS Lab Crate", LH_GS_LAB_CRATE},
    {"LH GS Tree", LH_GS_TREE},
    {"GV Crate Freestanding PoH", GV_CRATE_FREESTANDING_POH},
    {"GV Waterfall Freestanding PoH", GV_WATERFALL_FREESTANDING_POH},
    {"GV Chest", GV_CHEST},
    {"GV Deku Scrub Grotto Front", GV_DEKU_SCRUB_GROTTO_FRONT},
    {"GV Deku Scrub Grotto Rear", GV_DEKU_SCRUB_GROTTO_REAR},
    {"GV Cow", GV_COW},
    {"GV GS Small Bridge", GV_GS_SMALL_BRIDGE},
    {"GV GS Bean Patch", GV_GS_BEAN_PATCH},
    {"GV GS Behind Tent", GV_GS_BEHIND_TENT},
    {"GV GS Pillar", GV_GS_PILLAR},
    {"GF Chest", GF_CHEST},
    {"GF HBA 1000 Points", GF_HBA_1000_POINTS},
    {"GF HBA 1500 Points", GF_HBA_1500_POINTS},
    {"GF GS Top Floor", GF_GS_TOP_FLOOR},
    {"GF GS Archery Range", GF_GS_ARCHERY_RANGE},
    {"Hideout Jail Guard (1 Torch)", HIDEOUT_JAIL_GUARD_1_TORCH},
    {"Hideout Jail Guard (2 Torches)", HIDEOUT_JAIL_GUARD_2_TORCHES},
    {"Hideout Jail Guard (3 Torches)", HIDEOUT_JAIL_GUARD_3_TORCHES},
    {"Hideout Jail Guard (4 Torches)", HIDEOUT_JAIL_GUARD_4_TORCHES},
    {"Hideout Gerudo Membership Card", HIDEOUT_GERUDO_MEMBERSHIP_CARD},
    {"Wasteland Bombchu Salesman", WASTELAND_BOMBCHU_SALESMAN},
    {"Wasteland Chest", WASTELAND_CHEST},
    {"Wasteland GS", WASTELAND_GS},
    {"Colossus Great Fairy Reward", COLOSSUS_GREAT_FAIRY_REWARD},
    {"Colossus Freestanding PoH", COLOSSUS_FREESTANDING_POH},
    {"Colossus Deku Scrub Grotto Front", COLOSSUS_DEKU_SCRUB_GROTTO_FRONT},
    {"Colossus Deku Scrub Grotto Rear", COLOSSUS_DEKU_SCRUB_GROTTO_REAR},
    {"Colossus GS Bean Patch", COLOSSUS_GS_BEAN_PATCH},
    {"Colossus GS Tree", COLOSSUS_GS_TREE},
    {"Colossus GS Hill", COLOSSUS_GS_HILL},
    {"OGC Great Fairy Reward", OGC_GREAT_FAIRY_REWARD},
    {"OGC GS", OGC_GS},
    {"Deku Tree Map Chest", DEKU_TREE_MAP_CHEST},
    {"Deku Tree Slingshot Room Side Chest", DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST},
    {"Deku Tree Slingshot Chest", DEKU_TREE_SLINGSHOT_CHEST},
    {"Deku Tree Compass Chest", DEKU_TREE_COMPASS_CHEST},
    {"Deku Tree Compass Room Side Chest", DEKU_TREE_COMPASS_ROOM_SIDE_CHEST},
    {"Deku Tree Basement Chest", DEKU_TREE_BASEMENT_CHEST},
    {"Deku Tree GS Compass Room", DEKU_TREE_GS_COMPASS_ROOM},
    {"Deku Tree GS Basement Vines", DEKU_TREE_GS_BASEMENT_VINES},
    {"Deku Tree GS Basement Gate", DEKU_TREE_GS_BASEMENT_GATE},
    {"Deku Tree GS Basement Back Room", DEKU_TREE_GS_BASEMENT_BACK_ROOM},
    {"Deku Tree MQ Map Chest", DEKU_TREE_MQ_MAP_CHEST},
    {"Deku Tree MQ Slingshot Chest", DEKU_TREE_MQ_SLINGSHOT_CHEST},
    {"Deku Tree MQ Slingshot Room Back Chest", DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST},
    {"Deku Tree MQ Compass Chest", DEKU_TREE_MQ_COMPASS_CHEST},
    {"Deku Tree MQ Basement Chest", DEKU_TREE_MQ_BASEMENT_CHEST},
    {"Deku Tree MQ Before Spinning Log Chest", DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST},
    {"Deku Tree MQ After Spinning Log Chest", DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST},
    {"Deku Tree MQ Deku Scrub", DEKU_TREE_MQ_DEKU_SCRUB},
    {"Deku Tree MQ GS Lobby", DEKU_TREE_MQ_GS_LOBBY},
    {"Deku Tree MQ GS Compass Room", DEKU_TREE_MQ_GS_COMPASS_ROOM},
    {"Deku Tree MQ GS Basement Graves Room", DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM},
    {"Deku Tree MQ GS Basement Back Room", DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM},
    {"Deku Tree Queen Gohma Heart", DEKU_TREE_QUEEN_GOHMA_HEART},
    {"Dodongos Cavern Map Chest", DODONGOS_CAVERN_MAP_CHEST},
    {"Dodongos Cavern Compass Chest", DODONGOS_CAVERN_COMPASS_CHEST},
    {"Dodongos Cavern Bomb Flower Platform Chest", DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST},
    {"Dodongos Cavern Bomb Bag Chest", DODONGOS_CAVERN_BOMB_BAG_CHEST},
    {"Dodongos Cavern End of Bridge Chest", DODONGOS_CAVERN_END_OF_BRIDGE_CHEST},
    {"Dodongos Cavern Deku Scrub Side Room Near Dodongos", DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS},
    {"Dodongos Cavern Deku Scrub Lobby", DODONGOS_CAVERN_DEKU_SCRUB_LOBBY},
    {"Dodongos Cavern Deku Scrub Near Bomb Bag Left", DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT},
    {"Dodongos Cavern Deku Scrub Near Bomb Bag Right", DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT},
    {"Dodongos Cavern GS Side Room Near Lower Lizalfos", DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS},
    {"Dodongos Cavern GS Scarecrow", DODONGOS_CAVERN_GS_SCARECROW},
    {"Dodongos Cavern GS Alcove Above Stairs", DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS},
    {"Dodongos Cavern GS Vines Above Stairs", DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS},
    {"Dodongos Cavern GS Back Room", DODONGOS_CAVERN_GS_BACK_ROOM},
    {"Dodongos Cavern MQ Map Chest", DODONGOS_CAVERN_MQ_MAP_CHEST},
    {"Dodongos Cavern MQ Bomb Bag Chest", DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST},
    {"Dodongos Cavern MQ Torch Puzzle Room Chest", DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST},
    {"Dodongos Cavern MQ Larvae Room Chest", DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST},
    {"Dodongos Cavern MQ Compass Chest", DODONGOS_CAVERN_MQ_COMPASS_CHEST},
    {"Dodongos Cavern MQ Under Grave Chest", DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST},
    {"Dodongos Cavern MQ Deku Scrub Lobby Front", DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT},
    {"Dodongos Cavern MQ Deku Scrub Lobby Rear", DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR},
    {"Dodongos Cavern MQ Deku Scrub Side Room Near Lower Lizalfos", DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS},
    {"Dodongos Cavern MQ Deku Scrub Staircase", DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE},
    {"Dodongos Cavern MQ GS Scrub Room", DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM},
    {"Dodongos Cavern MQ GS Larvae Room", DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM},
    {"Dodongos Cavern MQ GS Lizalfos Room", DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM},
    {"Dodongos Cavern MQ GS Song of Time Block Room", DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM},
    {"Dodongos Cavern MQ GS Back Area", DODONGOS_CAVERN_MQ_GS_BACK_AREA},
    {"Dodongos Cavern Boss Room Chest", DODONGOS_CAVERN_BOSS_ROOM_CHEST},
    {"Dodongos Cavern King Dodongo Heart", DODONGOS_CAVERN_KING_DODONGO_HEART},
    {"Jabu Jabus Belly Boomerang Chest", JABU_JABUS_BELLY_BOOMERANG_CHEST},
    {"Jabu Jabus Belly Map Chest", JABU_JABUS_BELLY_MAP_CHEST},
    {"Jabu Jabus Belly Compass Chest", JABU_JABUS_BELLY_COMPASS_CHEST},
    {"Jabu Jabus Belly Deku Scrub", JABU_JABUS_BELLY_DEKU_SCRUB},
    {"Jabu Jabus Belly GS Water Switch Room", JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM},
    {"Jabu Jabus Belly GS Lobby Basement Lower", JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER},
    {"Jabu Jabus Belly GS Lobby Basement Upper", JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER},
    {"Jabu Jabus Belly GS Near Boss", JABU_JABUS_BELLY_GS_NEAR_BOSS},
    {"Jabu Jabus Belly MQ Map Chest", JABU_JABUS_BELLY_MQ_MAP_CHEST},
    {"Jabu Jabus Belly MQ First Room Side Chest", JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST},
    {"Jabu Jabus Belly MQ Second Room Lower Chest", JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST},
    {"Jabu Jabus Belly MQ Compass Chest", JABU_JABUS_BELLY_MQ_COMPASS_CHEST},
    {"Jabu Jabus Belly MQ Basement Near Switches Chest", JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST},
    {"Jabu Jabus Belly MQ Basement Near Vines Chest", JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST},
    {"Jabu Jabus Belly MQ Boomerang Room Small Chest", JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST},
    {"Jabu Jabus Belly MQ Boomerang Chest", JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST},
    {"Jabu Jabus Belly MQ Falling Like Like Room Chest", JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST},
    {"Jabu Jabus Belly MQ Second Room Upper Chest", JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST},
    {"Jabu Jabus Belly MQ Near Boss Chest", JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST},
    {"Jabu Jabus Belly MQ Cow", JABU_JABUS_BELLY_MQ_COW},
    {"Jabu Jabus Belly MQ GS Boomerang Chest Room", JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM},
    {"Jabu Jabus Belly MQ GS Tailpasaran Room", JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM},
    {"Jabu Jabus Belly MQ GS Invisible Enemies Room", JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM},
    {"Jabu Jabus Belly MQ GS Near Boss", JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS},
    {"Jabu Jabus Belly Barinade Heart", JABU_JABUS_BELLY_BARINADE_HEART},
    {"Bottom of the Well Front Left Fake Wall Chest", BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST},
    {"Bottom of the Well Front Center Bombable Chest", BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST},
    {"Bottom of the Well Back Left Bombable Chest", BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST},
    {"Bottom of the Well Underwater Left Chest", BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST},
    {"Bottom of the Well Freestanding Key", BOTTOM_OF_THE_WELL_FREESTANDING_KEY},
    {"Bottom of the Well Compass Chest", BOTTOM_OF_THE_WELL_COMPASS_CHEST},
    {"Bottom of the Well Center Skulltula Chest", BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST},
    {"Bottom of the Well Right Bottom Fake Wall Chest", BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST},
    {"Bottom of the Well Fire Keese Chest", BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST},
    {"Bottom of the Well Like Like Chest", BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST},
    {"Bottom of the Well Map Chest", BOTTOM_OF_THE_WELL_MAP_CHEST},
    {"Bottom of the Well Underwater Front Chest", BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST},
    {"Bottom of the Well Invisible Chest", BOTTOM_OF_THE_WELL_INVISIBLE_CHEST},
    {"Bottom of the Well Lens of Truth Chest", BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST},
    {"Bottom of the Well GS West Inner Room", BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM},
    {"Bottom of the Well GS East Inner Room", BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM},
    {"Bottom of the Well GS Like Like Cage", BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE},
    {"Bottom of the Well MQ Map Chest", BOTTOM_OF_THE_WELL_MQ_MAP_CHEST},
    {"Bottom of the Well MQ East Inner Room Freestanding Key", BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY},
    {"Bottom of the Well MQ Compass Chest", BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST},
    {"Bottom of the Well MQ Dead Hand Freestanding Key", BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY},
    {"Bottom of the Well MQ Lens of Truth Chest", BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST},
    {"Bottom of the Well MQ GS Coffin Room", BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM},
    {"Bottom of the Well MQ GS West Inner Room", BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM},
    {"Bottom of the Well MQ GS Basement", BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT},
    {"Forest Temple First Room Chest", FOREST_TEMPLE_FIRST_ROOM_CHEST},
    {"Forest Temple First Stalfos Chest", FOREST_TEMPLE_FIRST_STALFOS_CHEST},
    {"Forest Temple Raised Island Courtyard Chest", FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST},
    {"Forest Temple Map Chest", FOREST_TEMPLE_MAP_CHEST},
    {"Forest Temple Well Chest", FOREST_TEMPLE_WELL_CHEST},
    {"Forest Temple Eye Switch Chest", FOREST_TEMPLE_EYE_SWITCH_CHEST},
    {"Forest Temple Boss Key Chest", FOREST_TEMPLE_BOSS_KEY_CHEST},
    {"Forest Temple Floormaster Chest", FOREST_TEMPLE_FLOORMASTER_CHEST},
    {"Forest Temple Red Poe Chest", FOREST_TEMPLE_RED_POE_CHEST},
    {"Forest Temple Bow Chest", FOREST_TEMPLE_BOW_CHEST},
    {"Forest Temple Blue Poe Chest", FOREST_TEMPLE_BLUE_POE_CHEST},
    {"Forest Temple Falling Ceiling Room Chest", FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST},
    {"Forest Temple Basement Chest", FOREST_TEMPLE_BASEMENT_CHEST},
    {"Forest Temple GS First Room", FOREST_TEMPLE_GS_FIRST_ROOM},
    {"Forest Temple GS Lobby", FOREST_TEMPLE_GS_LOBBY},
    {"Forest Temple GS Raised Island Courtyard", FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD},
    {"Forest Temple GS Level Island Courtyard", FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD},
    {"Forest Temple GS Basement", FOREST_TEMPLE_GS_BASEMENT},
    {"Forest Temple MQ First Room Chest", FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST},
    {"Forest Temple MQ Wolfos Chest", FOREST_TEMPLE_MQ_WOLFOS_CHEST},
    {"Forest Temple MQ Well Chest", FOREST_TEMPLE_MQ_WELL_CHEST},
    {"Forest Temple MQ Raised Island Courtyard Lower Chest", FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST},
    {"Forest Temple MQ Raised Island Courtyard Upper Chest", FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST},
    {"Forest Temple MQ Boss Key Chest", FOREST_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Forest Temple MQ Redead Chest", FOREST_TEMPLE_MQ_REDEAD_CHEST},
    {"Forest Temple MQ Map Chest", FOREST_TEMPLE_MQ_MAP_CHEST},
    {"Forest Temple MQ Bow Chest", FOREST_TEMPLE_MQ_BOW_CHEST},
    {"Forest Temple MQ Compass Chest", FOREST_TEMPLE_MQ_COMPASS_CHEST},
    {"Forest Temple MQ Falling Ceiling Room Chest", FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST},
    {"Forest Temple MQ Basement Chest", FOREST_TEMPLE_MQ_BASEMENT_CHEST},
    {"Forest Temple MQ GS First Hallway", FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY},
    {"Forest Temple MQ GS Raised Island Courtyard", FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD},
    {"Forest Temple MQ GS Level Island Courtyard", FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD},
    {"Forest Temple MQ GS Well", FOREST_TEMPLE_MQ_GS_WELL},
    {"Forest Temple MQ GS Block Push Room", FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM},
    {"Forest Temple Phantom Ganon Heart", FOREST_TEMPLE_PHANTOM_GANON_HEART},
    {"Fire Temple Near Boss Chest", FIRE_TEMPLE_NEAR_BOSS_CHEST},
    {"Fire Temple Flare Dancer Chest", FIRE_TEMPLE_FLARE_DANCER_CHEST},
    {"Fire Temple Boss Key Chest", FIRE_TEMPLE_BOSS_KEY_CHEST},
    {"Fire Temple Big Lava Room Lower Open Door Chest", FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST},
    {"Fire Temple Big Lava Room Blocked Door Chest", FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST},
    {"Fire Temple Boulder Maze Lower Chest", FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST},
    {"Fire Temple Boulder Maze Side Room Chest", FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST},
    {"Fire Temple Map Chest", FIRE_TEMPLE_MAP_CHEST},
    {"Fire Temple Boulder Maze Shortcut Chest", FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST},
    {"Fire Temple Boulder Maze Upper Chest", FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST},
    {"Fire Temple Scarecrow Chest", FIRE_TEMPLE_SCARECROW_CHEST},
    {"Fire Temple Compass Chest", FIRE_TEMPLE_COMPASS_CHEST},
    {"Fire Temple Megaton Hammer Chest", FIRE_TEMPLE_MEGATON_HAMMER_CHEST},
    {"Fire Temple Highest Goron Chest", FIRE_TEMPLE_HIGHEST_GORON_CHEST},
    {"Fire Temple GS Boss Key Loop", FIRE_TEMPLE_GS_BOSS_KEY_LOOP},
    {"Fire Temple GS Song of Time Room", FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM},
    {"Fire Temple GS Boulder Maze", FIRE_TEMPLE_GS_BOULDER_MAZE},
    {"Fire Temple GS Scarecrow Climb", FIRE_TEMPLE_GS_SCARECROW_CLIMB},
    {"Fire Temple GS Scarecrow Top", FIRE_TEMPLE_GS_SCARECROW_TOP},
    {"Fire Temple MQ Map Room Side Chest", FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST},
    {"Fire Temple MQ Megaton Hammer Chest", FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST},
    {"Fire Temple MQ Map Chest", FIRE_TEMPLE_MQ_MAP_CHEST},
    {"Fire Temple MQ Near Boss Chest", FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST},
    {"Fire Temple MQ Big Lava Room Blocked Door Chest", FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST},
    {"Fire Temple MQ Boss Key Chest", FIRE_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Fire Temple MQ Lizalfos Maze Side Room Chest", FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST},
    {"Fire Temple MQ Compass Chest", FIRE_TEMPLE_MQ_COMPASS_CHEST},
    {"Fire Temple MQ Lizalfos Maze Upper Chest", FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST},
    {"Fire Temple MQ Lizalfos Maze Lower Chest", FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST},
    {"Fire Temple MQ Freestanding Key", FIRE_TEMPLE_MQ_FREESTANDING_KEY},
    {"Fire Temple MQ Chest On Fire", FIRE_TEMPLE_MQ_CHEST_ON_FIRE},
    {"Fire Temple MQ GS Big Lava Room Open Door", FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR},
    {"Fire Temple MQ GS Skull On Fire", FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE},
    {"Fire Temple MQ GS Fire Wall Maze Center", FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER},
    {"Fire Temple MQ GS Fire Wall Maze Side Room", FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM},
    {"Fire Temple MQ GS Above Fire Wall Maze", FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE},
    {"Fire Temple Volvagia Heart", FIRE_TEMPLE_VOLVAGIA_HEART},
    {"Water Temple Compass Chest", WATER_TEMPLE_COMPASS_CHEST},
    {"Water Temple Map Chest", WATER_TEMPLE_MAP_CHEST},
    {"Water Temple Cracked Wall Chest", WATER_TEMPLE_CRACKED_WALL_CHEST},
    {"Water Temple Torches Chest", WATER_TEMPLE_TORCHES_CHEST},
    {"Water Temple Boss Key Chest", WATER_TEMPLE_BOSS_KEY_CHEST},
    {"Water Temple Central Pillar Chest", WATER_TEMPLE_CENTRAL_PILLAR_CHEST},
    {"Water Temple Central Bow Target Chest", WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST},
    {"Water Temple Longshot Chest", WATER_TEMPLE_LONGSHOT_CHEST},
    {"Water Temple River Chest", WATER_TEMPLE_RIVER_CHEST},
    {"Water Temple Dragon Chest", WATER_TEMPLE_DRAGON_CHEST},
    {"Water Temple GS Behind Gate", WATER_TEMPLE_GS_BEHIND_GATE},
    {"Water Temple GS Near Boss Key Chest", WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST},
    {"Water Temple GS Central Pillar", WATER_TEMPLE_GS_CENTRAL_PILLAR},
    {"Water Temple GS Falling Platform Room", WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM},
    {"Water Temple GS River", WATER_TEMPLE_GS_RIVER},
    {"Water Temple MQ Longshot Chest", WATER_TEMPLE_MQ_LONGSHOT_CHEST},
    {"Water Temple MQ Map Chest", WATER_TEMPLE_MQ_MAP_CHEST},
    {"Water Temple MQ Compass Chest", WATER_TEMPLE_MQ_COMPASS_CHEST},
    {"Water Temple MQ Central Pillar Chest", WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST},
    {"Water Temple MQ Boss Key Chest", WATER_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Water Temple MQ Freestanding Key", WATER_TEMPLE_MQ_FREESTANDING_KEY},
    {"Water Temple MQ GS Lizalfos Hallway", WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY},
    {"Water Temple MQ GS Before Upper Water Switch", WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH},
    {"Water Temple MQ GS River", WATER_TEMPLE_MQ_GS_RIVER},
    {"Water Temple MQ GS Freestanding Key Area", WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA},
    {"Water Temple MQ GS Triple Wall Torch", WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH},
    {"Water Temple Morpha Heart", WATER_TEMPLE_MORPHA_HEART},
    {"Shadow Temple Map Chest", SHADOW_TEMPLE_MAP_CHEST},
    {"Shadow Temple Hover Boots Chest", SHADOW_TEMPLE_HOVER_BOOTS_CHEST},
    {"Shadow Temple Compass Chest", SHADOW_TEMPLE_COMPASS_CHEST},
    {"Shadow Temple Early Silver Rupee Chest", SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST},
    {"Shadow Temple Invisible Blades Visible Chest", SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST},
    {"Shadow Temple Invisible Blades Invisible Chest", SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST},
    {"Shadow Temple Falling Spikes Lower Chest", SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST},
    {"Shadow Temple Falling Spikes Upper Chest", SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST},
    {"Shadow Temple Falling Spikes Switch Chest", SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST},
    {"Shadow Temple Invisible Spikes Chest", SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST},
    {"Shadow Temple Freestanding Key", SHADOW_TEMPLE_FREESTANDING_KEY},
    {"Shadow Temple Wind Hint Chest", SHADOW_TEMPLE_WIND_HINT_CHEST},
    {"Shadow Temple After Wind Enemy Chest", SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST},
    {"Shadow Temple After Wind Hidden Chest", SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST},
    {"Shadow Temple Spike Walls Left Chest", SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST},
    {"Shadow Temple Boss Key Chest", SHADOW_TEMPLE_BOSS_KEY_CHEST},
    {"Shadow Temple Invisible Floormaster Chest", SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST},
    {"Shadow Temple GS Like Like Room", SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM},
    {"Shadow Temple GS Falling Spikes Room", SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM},
    {"Shadow Temple GS Single Giant Pot", SHADOW_TEMPLE_GS_SINGLE_GIANT_POT},
    {"Shadow Temple GS Near Ship", SHADOW_TEMPLE_GS_NEAR_SHIP},
    {"Shadow Temple GS Triple Giant Pot", SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT},
    {"Shadow Temple MQ Early Gibdos Chest", SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST},
    {"Shadow Temple MQ Map Chest", SHADOW_TEMPLE_MQ_MAP_CHEST},
    {"Shadow Temple MQ Near Ship Invisible Chest", SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST},
    {"Shadow Temple MQ Compass Chest", SHADOW_TEMPLE_MQ_COMPASS_CHEST},
    {"Shadow Temple MQ Hover Boots Chest", SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST},
    {"Shadow Temple MQ Invisible Blades Invisible Chest", SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST},
    {"Shadow Temple MQ Invisible Blades Visible Chest", SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST},
    {"Shadow Temple MQ Beamos Silver Rupees Chest", SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST},
    {"Shadow Temple MQ Falling Spikes Lower Chest", SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST},
    {"Shadow Temple MQ Falling Spikes Upper Chest", SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST},
    {"Shadow Temple MQ Falling Spikes Switch Chest", SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST},
    {"Shadow Temple MQ Invisible Spikes Chest", SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST},
    {"Shadow Temple MQ Stalfos Room Chest", SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST},
    {"Shadow Temple MQ Wind Hint Chest", SHADOW_TEMPLE_MQ_WIND_HINT_CHEST},
    {"Shadow Temple MQ After Wind Hidden Chest", SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST},
    {"Shadow Temple MQ After Wind Enemy Chest", SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST},
    {"Shadow Temple MQ Boss Key Chest", SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Shadow Temple MQ Spike Walls Left Chest", SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST},
    {"Shadow Temple MQ Freestanding Key", SHADOW_TEMPLE_MQ_FREESTANDING_KEY},
    {"Shadow Temple MQ Bomb Flower Chest", SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST},
    {"Shadow Temple MQ GS Falling Spikes Room", SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM},
    {"Shadow Temple MQ GS Wind Hint Room", SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM},
    {"Shadow Temple MQ GS After Wind", SHADOW_TEMPLE_MQ_GS_AFTER_WIND},
    {"Shadow Temple MQ GS After Ship", SHADOW_TEMPLE_MQ_GS_AFTER_SHIP},
    {"Shadow Temple MQ GS Near Boss", SHADOW_TEMPLE_MQ_GS_NEAR_BOSS},
    {"Shadow Temple Bongo Bongo Heart", SHADOW_TEMPLE_BONGO_BONGO_HEART},
    {"Spirit Temple Child Bridge Chest", SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST},
    {"Spirit Temple Child Early Torches Chest", SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST},
    {"Spirit Temple Child Climb North Chest", SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST},
    {"Spirit Temple Child Climb East Chest", SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST},
    {"Spirit Temple Map Chest", SPIRIT_TEMPLE_MAP_CHEST},
    {"Spirit Temple Sun Block Room Chest", SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST},
    {"Spirit Temple MQ Entrance Front Left Chest", SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST},
    {"Spirit Temple MQ Entrance Back Right Chest", SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST},
    {"Spirit Temple MQ Entrance Front Right Chest", SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST},
    {"Spirit Temple MQ Entrance Back Left Chest", SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST},
    {"Spirit Temple MQ Map Chest", SPIRIT_TEMPLE_MQ_MAP_CHEST},
    {"Spirit Temple MQ Map Room Enemy Chest", SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST},
    {"Spirit Temple MQ Child Climb North Chest", SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST},
    {"Spirit Temple MQ Child Climb South Chest", SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST},
    {"Spirit Temple MQ Compass Chest", SPIRIT_TEMPLE_MQ_COMPASS_CHEST},
    {"Spirit Temple MQ Silver Block Hallway Chest", SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST},
    {"Spirit Temple MQ Sun Block Room Chest", SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST},
    {"Spirit Temple Silver Gauntlets Chest", SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST},
    {"Spirit Temple Compass Chest", SPIRIT_TEMPLE_COMPASS_CHEST},
    {"Spirit Temple Early Adult Right Chest", SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST},
    {"Spirit Temple First Mirror Left Chest", SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST},
    {"Spirit Temple First Mirror Right Chest", SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST},
    {"Spirit Temple Statue Room Northeast Chest", SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST},
    {"Spirit Temple Statue Room Hand Chest", SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST},
    {"Spirit Temple Near Four Armos Chest", SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST},
    {"Spirit Temple Hallway Right Invisible Chest", SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST},
    {"Spirit Temple Hallway Left Invisible Chest", SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST},
    {"Spirit Temple MQ Child Hammer Switch Chest", SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST},
    {"Spirit Temple MQ Statue Room Lullaby Chest", SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST},
    {"Spirit Temple MQ Statue Room Invisible Chest", SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST},
    {"Spirit Temple MQ Leever Room Chest", SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST},
    {"Spirit Temple MQ Symphony Room Chest", SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST},
    {"Spirit Temple MQ Beamos Room Chest", SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST},
    {"Spirit Temple MQ Chest Switch Chest", SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST},
    {"Spirit Temple MQ Boss Key Chest", SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST},
    {"Spirit Temple Mirror Shield Chest", SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST},
    {"Spirit Temple Boss Key Chest", SPIRIT_TEMPLE_BOSS_KEY_CHEST},
    {"Spirit Temple Topmost Chest", SPIRIT_TEMPLE_TOPMOST_CHEST},
    {"Spirit Temple MQ Mirror Puzzle Invisible Chest", SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST},
    {"Spirit Temple GS Metal Fence", SPIRIT_TEMPLE_GS_METAL_FENCE},
    {"Spirit Temple GS Sun on Floor Room", SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM},
    {"Spirit Temple GS Hall After Sun Block Room", SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM},
    {"Spirit Temple GS Lobby", SPIRIT_TEMPLE_GS_LOBBY},
    {"Spirit Temple GS Boulder Room", SPIRIT_TEMPLE_GS_BOULDER_ROOM},
    {"Spirit Temple MQ GS Sun Block Room", SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM},
    {"Spirit Temple MQ GS Leever Room", SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM},
    {"Spirit Temple MQ GS Symphony Room", SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM},
    {"Spirit Temple MQ GS Nine Thrones Room West", SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST},
    {"Spirit Temple MQ GS Nine Thrones Room North", SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH},
    {"Spirit Temple Twinrova Heart", SPIRIT_TEMPLE_TWINROVA_HEART},
    {"Ice Cavern Map Chest", ICE_CAVERN_MAP_CHEST},
    {"Ice Cavern Compass Chest", ICE_CAVERN_COMPASS_CHEST},
    {"Ice Cavern Freestanding PoH", ICE_CAVERN_FREESTANDING_POH},
    {"Ice Cavern Iron Boots Chest", ICE_CAVERN_IRON_BOOTS_CHEST},
    {"Ice Cavern GS Spinning Scythe Room", ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM},
    {"Ice Cavern GS Heart Piece Room", ICE_CAVERN_GS_HEART_PIECE_ROOM},
    {"Ice Cavern GS Push Block Room", ICE_CAVERN_GS_PUSH_BLOCK_ROOM},
    {"Ice Cavern MQ Map Chest", ICE_CAVERN_MQ_MAP_CHEST},
    {"Ice Cavern MQ Compass Chest", ICE_CAVERN_MQ_COMPASS_CHEST},
    {"Ice Cavern MQ Freestanding PoH", ICE_CAVERN_MQ_FREESTANDING_POH},
    {"Ice Cavern MQ Iron Boots Chest", ICE_CAVERN_MQ_IRON_BOOTS_CHEST},
    {"Ice Cavern MQ GS Red Ice", ICE_CAVERN_MQ_GS_RED_ICE},
    {"Ice Cavern MQ GS Ice Block", ICE_CAVERN_MQ_GS_ICE_BLOCK},
    {"Ice Cavern MQ GS Scarecrow", ICE_CAVERN_MQ_GS_SCARECROW},
    {"Gerudo Training Ground Lobby Left Chest", GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST},
    {"Gerudo Training Ground Lobby Right Chest", GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST},
    {"Gerudo Training Ground Stalfos Chest", GERUDO_TRAINING_GROUND_STALFOS_CHEST},
    {"Gerudo Training Ground Before Heavy Block Chest", GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST},
    {"Gerudo Training Ground Heavy Block First Chest", GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST},
    {"Gerudo Training Ground Heavy Block Second Chest", GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST},
    {"Gerudo Training Ground Heavy Block Third Chest", GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST},
    {"Gerudo Training Ground Heavy Block Fourth Chest", GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST},
    {"Gerudo Training Ground Eye Statue Chest", GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST},
    {"Gerudo Training Ground Near Scarecrow Chest", GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST},
    {"Gerudo Training Ground Hammer Room Clear Chest", GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST},
    {"Gerudo Training Ground Hammer Room Switch Chest", GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST},
    {"Gerudo Training Ground Freestanding Key", GERUDO_TRAINING_GROUND_FREESTANDING_KEY},
    {"Gerudo Training Ground Maze Right Central Chest", GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST},
    {"Gerudo Training Ground Maze Right Side Chest", GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST},
    {"Gerudo Training Ground Underwater Silver Rupee Chest", GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST},
    {"Gerudo Training Ground Beamos Chest", GERUDO_TRAINING_GROUND_BEAMOS_CHEST},
    {"Gerudo Training Ground Hidden Ceiling Chest", GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST},
    {"Gerudo Training Ground Maze Path First Chest", GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST},
    {"Gerudo Training Ground Maze Path Second Chest", GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST},
    {"Gerudo Training Ground Maze Path Third Chest", GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST},
    {"Gerudo Training Ground Maze Path Final Chest", GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST},
    {"Gerudo Training Ground MQ Lobby Left Chest", GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST},
    {"Gerudo Training Ground MQ Lobby Right Chest", GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST},
    {"Gerudo Training Ground MQ First Iron Knuckle Chest", GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST},
    {"Gerudo Training Ground MQ Before Heavy Block Chest", GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST},
    {"Gerudo Training Ground MQ Heavy Block Chest", GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST},
    {"Gerudo Training Ground MQ Eye Statue Chest", GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST},
    {"Gerudo Training Ground MQ Ice Arrows Chest", GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST},
    {"Gerudo Training Ground MQ Second Iron Knuckle Chest", GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST},
    {"Gerudo Training Ground MQ Flame Circle Chest", GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST},
    {"Gerudo Training Ground MQ Maze Right Central Chest", GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST},
    {"Gerudo Training Ground MQ Maze Right Side Chest", GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST},
    {"Gerudo Training Ground MQ Underwater Silver Rupee Chest", GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST},
    {"Gerudo Training Ground MQ Dinolfos Chest", GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST},
    {"Gerudo Training Ground MQ Hidden Ceiling Chest", GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST},
    {"Gerudo Training Ground MQ Maze Path First Chest", GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST},
    {"Gerudo Training Ground MQ Maze Path Third Chest", GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST},
    {"Gerudo Training Ground MQ Maze Path Second Chest", GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST},
    {"Ganons Castle Forest Trial Chest", GANONS_CASTLE_FOREST_TRIAL_CHEST},
    {"Ganons Castle Water Trial Left Chest", GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST},
    {"Ganons Castle Water Trial Right Chest", GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST},
    {"Ganons Castle Shadow Trial Front Chest", GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST},
    {"Ganons Castle Shadow Trial Golden Gauntlets Chest", GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST},
    {"Ganons Castle Light Trial First Left Chest", GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST},
    {"Ganons Castle Light Trial Second Left Chest", GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST},
    {"Ganons Castle Light Trial Third Left Chest", GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST},
    {"Ganons Castle Light Trial First Right Chest", GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST},
    {"Ganons Castle Light Trial Second Right Chest", GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST},
    {"Ganons Castle Light Trial Third Right Chest", GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST},
    {"Ganons Castle Light Trial Invisible Enemies Chest", GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST},
    {"Ganons Castle Light Trial Lullaby Chest", GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST},
    {"Ganons Castle Spirit Trial Crystal Switch Chest", GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST},
    {"Ganons Castle Spirit Trial Invisible Chest", GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST},
    {"Ganons Castle Deku Scrub Left", GANONS_CASTLE_DEKU_SCRUB_LEFT},
    {"Ganons Castle Deku Scrub Center-Left", GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT},
    {"Ganons Castle Deku Scrub Center-Right", GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT},
    {"Ganons Castle Deku Scrub Right", GANONS_CASTLE_DEKU_SCRUB_RIGHT},
    {"Ganons Castle MQ Forest Trial Freestanding Key", GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY},
    {"Ganons Castle MQ Forest Trial Eye Switch Chest", GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST},
    {"Ganons Castle MQ Forest Trial Frozen Eye Switch Chest", GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST},
    {"Ganons Castle MQ Water Trial Chest", GANONS_CASTLE_MQ_WATER_TRIAL_CHEST},
    {"Ganons Castle MQ Shadow Trial Bomb Flower Chest", GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST},
    {"Ganons Castle MQ Shadow Trial Eye Switch Chest", GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST},
    {"Ganons Castle MQ Light Trial Lullaby Chest", GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST},
    {"Ganons Castle MQ Spirit Trial First Chest", GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST},
    {"Ganons Castle MQ Spirit Trial Invisible Chest", GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST},
    {"Ganons Castle MQ Spirit Trial Sun Front Left Chest", GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST},
    {"Ganons Castle MQ Spirit Trial Sun Back Left Chest", GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST},
    {"Ganons Castle MQ Spirit Trial Sun Back Right Chest", GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST},
    {"Ganons Castle MQ Spirit Trial Golden Gauntlets Chest", GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST},
    {"Ganons Castle MQ Deku Scrub Left", GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT},
    {"Ganons Castle MQ Deku Scrub Center-Left", GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT},
    {"Ganons Castle MQ Deku Scrub Center", GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER},
    {"Ganons Castle MQ Deku Scrub Center-Right", GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT},
    {"Ganons Castle MQ Deku Scrub Right", GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT},
    {"Ganons Tower Boss Key Chest", GANONS_TOWER_BOSS_KEY_CHEST},
    {"Pierre", PIERRE},
    {"Deliver Rutos Letter", DELIVER_RUTOS_LETTER},
    {"Master Sword Pedestal", MASTER_SWORD_PEDESTAL}
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
    {"Bombs (5)", BOMBS_5},
    {"Deku Nuts (5)", DEKU_NUTS_5},
    {"Bombchus (10)", BOMBCHUS_10},
    {"Boomerang", BOOMERANG},
    {"Deku Stick (1)", DEKU_STICK_1},
    {"Lens of Truth", LENS_OF_TRUTH},
    {"Megaton Hammer", MEGATON_HAMMER},
    {"Cojiro", COJIRO},
    {"Bottle", BOTTLE},
    {"Bottle with Milk", BOTTLE_WITH_MILK},
    {"Rutos Letter", RUTOS_LETTER},
    {"Deliver Letter", DELIVER_LETTER},
    {"Sell Big Poe", SELL_BIG_POE},
    {"Magic Bean", MAGIC_BEAN},
    {"Skull Mask", SKULL_MASK},
    {"Spooky Mask", SPOOKY_MASK},
    {"Keaton Mask", KEATON_MASK},
    {"Bunny Hood", BUNNY_HOOD},
    {"Mask of Truth", MASK_OF_TRUTH},
    {"Pocket Egg", POCKET_EGG},
    {"Pocket Cucco", POCKET_CUCCO},
    {"Odd Mushroom", ODD_MUSHROOM},
    {"Odd Potion", ODD_POTION},
    {"Poachers Saw", POACHERS_SAW},
    {"Broken Sword", BROKEN_SWORD},
    {"Prescription", PRESCRIPTION},
    {"Eyeball Frog", EYEBALL_FROG},
    {"Eyedrops", EYEDROPS},
    {"Claim Check", CLAIM_CHECK},
    {"Kokiri Sword", KOKIRI_SWORD},
    {"Giants Knife", GIANTS_KNIFE},
    {"Deku Shield", DEKU_SHIELD},
    {"Hylian Shield", HYLIAN_SHIELD},
    {"Mirror Shield", MIRROR_SHIELD},
    {"Goron Tunic", GORON_TUNIC},
    {"Zora Tunic", ZORA_TUNIC},
    {"Iron Boots", IRON_BOOTS},
    {"Hover Boots", HOVER_BOOTS},
    {"Stone of Agony", STONE_OF_AGONY},
    {"Gerudo Membership Card", GERUDO_MEMBERSHIP_CARD},
    {"Heart Container", HEART_CONTAINER},
    {"Piece of Heart", PIECE_OF_HEART},
    {"Boss Key", BOSS_KEY},
    {"Compass", COMPASS},
    {"Map", MAP},
    {"Small Key", SMALL_KEY},
    {"Weird Egg", WEIRD_EGG},
    {"Recovery Heart", RECOVERY_HEART},
    {"Arrows (5)", ARROWS_5},
    {"Arrows (10)", ARROWS_10},
    {"Arrows (30)", ARROWS_30},
    {"Rupee (1)", RUPEE_1},
    {"Rupees (5)", RUPEES_5},
    {"Rupees (20)", RUPEES_20},
    {"Milk", MILK},
    {"Goron Mask", GORON_MASK},
    {"Zora Mask", ZORA_MASK},
    {"Gerudo Mask", GERUDO_MASK},
    {"Rupees (50)", RUPEES_50},
    {"Rupees (200)", RUPEES_200},
    {"Biggoron Sword", BIGGORON_SWORD},
    {"Fire Arrows", FIRE_ARROWS},
    {"Ice Arrows", ICE_ARROWS},
    {"Light Arrows", LIGHT_ARROWS},
    {"Gold Skulltula Token", GOLD_SKULLTULA_TOKEN},
    {"Dins Fire", DINS_FIRE},
    {"Nayrus Love", NAYRUS_LOVE},
    {"Farores Wind", FARORES_WIND},
    {"Deku Nuts (10)", DEKU_NUTS_10},
    {"Bombs (10)", BOMBS_10},
    {"Bombs (20)", BOMBS_20},
    {"Deku Seeds (30)", DEKU_SEEDS_30},
    {"Bombchus (5)", BOMBCHUS_5},
    {"Bombchus (20)", BOMBCHUS_20},
    {"Rupee (Treasure Chest Game)", RUPEE_TREASURE_CHEST_GAME},
    {"Piece of Heart (Treasure Chest Game)", PIECE_OF_HEART_TREASURE_CHEST_GAME},
    {"Ice Trap", ICE_TRAP},
    {"Progressive Hookshot", PROGRESSIVE_HOOKSHOT},
    {"Progressive Strength Upgrade", PROGRESSIVE_STRENGTH_UPGRADE},
    {"Bomb Bag", BOMB_BAG},
    {"Bow", BOW},
    {"Slingshot", SLINGSHOT},
    {"Progressive Wallet", PROGRESSIVE_WALLET},
    {"Progressive Scale", PROGRESSIVE_SCALE},
    {"Deku Nut Capacity", DEKU_NUT_CAPACITY},
    {"Deku Stick Capacity", DEKU_STICK_CAPACITY},
    {"Bombchus", BOMBCHUS},
    {"Magic Meter", MAGIC_METER},
    {"Ocarina", OCARINA},
    {"Bottle with Red Potion", BOTTLE_WITH_RED_POTION},
    {"Bottle with Green Potion", BOTTLE_WITH_GREEN_POTION},
    {"Bottle with Blue Potion", BOTTLE_WITH_BLUE_POTION},
    {"Bottle with Fairy", BOTTLE_WITH_FAIRY},
    {"Bottle with Fish", BOTTLE_WITH_FISH},
    {"Bottle with Blue Fire", BOTTLE_WITH_BLUE_FIRE},
    {"Bottle with Bugs", BOTTLE_WITH_BUGS},
    {"Bottle with Big Poe", BOTTLE_WITH_BIG_POE},
    {"Bottle with Poe", BOTTLE_WITH_POE},
    {"Boss Key (Forest Temple)", BOSS_KEY_FOREST_TEMPLE},
    {"Boss Key (Fire Temple)", BOSS_KEY_FIRE_TEMPLE},
    {"Boss Key (Water Temple)", BOSS_KEY_WATER_TEMPLE},
    {"Boss Key (Spirit Temple)", BOSS_KEY_SPIRIT_TEMPLE},
    {"Boss Key (Shadow Temple)", BOSS_KEY_SHADOW_TEMPLE},
    {"Boss Key (Ganons Castle)", BOSS_KEY_GANONS_CASTLE},
    {"Compass (Deku Tree)", COMPASS_DEKU_TREE},
    {"Compass (Dodongos Cavern)", COMPASS_DODONGOS_CAVERN},
    {"Compass (Jabu Jabus Belly)", COMPASS_JABU_JABUS_BELLY},
    {"Compass (Forest Temple)", COMPASS_FOREST_TEMPLE},
    {"Compass (Fire Temple)", COMPASS_FIRE_TEMPLE},
    {"Compass (Water Temple)", COMPASS_WATER_TEMPLE},
    {"Compass (Spirit Temple)", COMPASS_SPIRIT_TEMPLE},
    {"Compass (Shadow Temple)", COMPASS_SHADOW_TEMPLE},
    {"Compass (Bottom of the Well)", COMPASS_BOTTOM_OF_THE_WELL},
    {"Compass (Ice Cavern)", COMPASS_ICE_CAVERN},
    {"Map (Deku Tree)", MAP_DEKU_TREE},
    {"Map (Dodongos Cavern)", MAP_DODONGOS_CAVERN},
    {"Map (Jabu Jabus Belly)", MAP_JABU_JABUS_BELLY},
    {"Map (Forest Temple)", MAP_FOREST_TEMPLE},
    {"Map (Fire Temple)", MAP_FIRE_TEMPLE},
    {"Map (Water Temple)", MAP_WATER_TEMPLE},
    {"Map (Spirit Temple)", MAP_SPIRIT_TEMPLE},
    {"Map (Shadow Temple)", MAP_SHADOW_TEMPLE},
    {"Map (Bottom of the Well)", MAP_BOTTOM_OF_THE_WELL},
    {"Map (Ice Cavern)", MAP_ICE_CAVERN},
    {"Small Key (Forest Temple)", SMALL_KEY_FOREST_TEMPLE},
    {"Small Key (Fire Temple)", SMALL_KEY_FIRE_TEMPLE},
    {"Small Key (Water Temple)", SMALL_KEY_WATER_TEMPLE},
    {"Small Key (Spirit Temple)", SMALL_KEY_SPIRIT_TEMPLE},
    {"Small Key (Shadow Temple)", SMALL_KEY_SHADOW_TEMPLE},
    {"Small Key (Bottom of the Well)", SMALL_KEY_BOTTOM_OF_THE_WELL},
    {"Small Key (Gerudo Training Ground)", SMALL_KEY_GERUDO_TRAINING_GROUND},
    {"Small Key (Thieves Hideout)", SMALL_KEY_THIEVES_HIDEOUT},
    {"Small Key (Ganons Castle)", SMALL_KEY_GANONS_CASTLE},
    {"Double Defense", DOUBLE_DEFENSE},
    {"Magic Bean Pack", MAGIC_BEAN_PACK},
    {"Triforce Piece", TRIFORCE_PIECE},
    {"Zeldas Letter", ZELDAS_LETTER},
    {"Time Travel", TIME_TRAVEL},
    {"Scarecrow Song", SCARECROW_SONG},
    {"Triforce", TRIFORCE},
    {"Small Key Ring (Forest Temple)", SMALL_KEY_RING_FOREST_TEMPLE},
    {"Small Key Ring (Fire Temple)", SMALL_KEY_RING_FIRE_TEMPLE},
    {"Small Key Ring (Water Temple)", SMALL_KEY_RING_WATER_TEMPLE},
    {"Small Key Ring (Spirit Temple)", SMALL_KEY_RING_SPIRIT_TEMPLE},
    {"Small Key Ring (Shadow Temple)", SMALL_KEY_RING_SHADOW_TEMPLE},
    {"Small Key Ring (Bottom of the Well)", SMALL_KEY_RING_BOTTOM_OF_THE_WELL},
    {"Small Key Ring (Gerudo Training Ground)", SMALL_KEY_RING_GERUDO_TRAINING_GROUND},
    {"Small Key Ring (Thieves Hideout)", SMALL_KEY_RING_THIEVES_HIDEOUT},
    {"Small Key Ring (Ganons Castle)", SMALL_KEY_RING_GANONS_CASTLE},
    {"Water Temple Clear", WATER_TEMPLE_CLEAR},
    {"Forest Trial Clear", FOREST_TRIAL_CLEAR},
    {"Fire Trial Clear", FIRE_TRIAL_CLEAR},
    {"Water Trial Clear", WATER_TRIAL_CLEAR},
    {"Shadow Trial Clear", SHADOW_TRIAL_CLEAR},
    {"Spirit Trial Clear", SPIRIT_TRIAL_CLEAR},
    {"Light Trial Clear", LIGHT_TRIAL_CLEAR},
    {"Deku Stick Drop", DEKU_STICK_DROP},
    {"Deku Nut Drop", DEKU_NUT_DROP},
    {"Blue Fire", BLUE_FIRE},
    {"Fairy", FAIRY},
    {"Fish", FISH},
    {"Bugs", BUGS},
    {"Big Poe", BIG_POE},
    {"Bombchu Drop", BOMBCHU_DROP},
    {"Arrows", ARROWS_CONSUMABLE_REFILL},
    {"Bombs", BOMBS_CONSUMABLE_REFILL},
    {"Deku Seeds", DEKU_SEEDS_CONSUMABLE_REFILL},
    {"Deku Sticks", DEKU_STICKS_CONSUMABLE_REFILL},
    {"Deku Nuts", DEKU_NUTS_CONSUMABLE_REFILL},
    {"Rupees", RUPEES_CONSUMABLE_REFILL},
    {"Minuet of Forest", MINUET_OF_FOREST},
    {"Bolero of Fire", BOLERO_OF_FIRE},
    {"Serenade of Water", SERENADE_OF_WATER},
    {"Requiem of Spirit", REQUIEM_OF_SPIRIT},
    {"Nocturne of Shadow", NOCTURNE_OF_SHADOW},
    {"Prelude of Light", PRELUDE_OF_LIGHT},
    {"Zeldas Lullaby", ZELDAS_LULLABY},
    {"Eponas Song", EPONAS_SONG},
    {"Sarias Song", SARIAS_SONG},
    {"Suns Song", SUNS_SONG},
    {"Song of Time", SONG_OF_TIME},
    {"Song of Storms", SONG_OF_STORMS},
    {"Buy Deku Nut (5)", BUY_DEKU_NUT_5},
    {"Buy Arrows (30)", BUY_ARROWS_30},
    {"Buy Arrows (50)", BUY_ARROWS_50},
    {"Buy Bombs (5) for 25 Rupees", BUY_BOMBS_5_FOR_25_RUPEES},
    {"Buy Deku Nut (10)", BUY_DEKU_NUT_10},
    {"Buy Deku Stick (1)", BUY_DEKU_STICK_1},
    {"Buy Bombs (10)", BUY_BOMBS_10},
    {"Buy Fish", BUY_FISH},
    {"Buy Red Potion for 30 Rupees", BUY_RED_POTION_FOR_30_RUPEES},
    {"Buy Green Potion", BUY_GREEN_POTION},
    {"Buy Blue Potion", BUY_BLUE_POTION},
    {"Buy Hylian Shield", BUY_HYLIAN_SHIELD},
    {"Buy Deku Shield", BUY_DEKU_SHIELD},
    {"Buy Goron Tunic", BUY_GORON_TUNIC},
    {"Buy Zora Tunic", BUY_ZORA_TUNIC},
    {"Buy Heart", BUY_HEART},
    {"Buy Bombchu (10)", BUY_BOMBCHU_10},
    {"Buy Bombchu (20)", BUY_BOMBCHU_20},
    {"Buy Bombchu (5)", BUY_BOMBCHU_5},
    {"Buy Deku Seeds (30)", BUY_DEKU_SEEDS_30},
    {"Sold Out", SOLD_OUT},
    {"Buy Blue Fire", BUY_BLUE_FIRE},
    {"Buy Bottle Bug", BUY_BOTTLE_BUG},
    {"Buy Poe", BUY_POE},
    {"Buy Fairy\'s Spirit", BUY_FAIRYS_SPIRIT},
    {"Buy Arrows (10)", BUY_ARROWS_10},
    {"Buy Bombs (20)", BUY_BOMBS_20},
    {"Buy Bombs (30)", BUY_BOMBS_30},
    {"Buy Bombs (5) for 35 Rupees", BUY_BOMBS_5_FOR_35_RUPEES},
    {"Buy Red Potion for 40 Rupees", BUY_RED_POTION_FOR_40_RUPEES},
    {"Buy Red Potion for 50 Rupees", BUY_RED_POTION_FOR_50_RUPEES},
    {"Kokiri Emerald", KOKIRI_EMERALD},
    {"Goron Ruby", GORON_RUBY},
    {"Zora Sapphire", ZORA_SAPPHIRE},
    {"Forest Medallion", FOREST_MEDALLION},
    {"Fire Medallion", FIRE_MEDALLION},
    {"Water Medallion", WATER_MEDALLION},
    {"Spirit Medallion", SPIRIT_MEDALLION},
    {"Shadow Medallion", SHADOW_MEDALLION},
    {"Light Medallion", LIGHT_MEDALLION}
};

s16 Randomizer::GetItemModelFromId(s16 itemId) {
    return itemIdToModel[itemId];
}

void Randomizer::LoadItemLocations(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        // bandaid until new save stuff happens
        ParseItemLocations(spoilerFileName);

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

void Randomizer::ParseItemLocations(const char* spoilerFileName) {
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
    switch(randoGet) {
        case UNKNOWN_GET:
            return ogItemId;
        case KOKIRI_SWORD:
            return GI_SWORD_KOKIRI;
        case DEKU_SHIELD:
            return GI_SHIELD_DEKU;
        case BOMBCHUS_20:
            return GI_BOMBCHUS_20;
        case ICE_TRAP:
            return GI_ICE_TRAP;
        case PIECE_OF_HEART:
            return GI_HEART_PIECE;
        case BOMBS_5:
            return GI_BOMBS_5;
        case DEKU_NUTS_5:
            return GI_NUTS_5;
        case BOMBCHUS_10:
            return GI_BOMBCHUS_10;
        case BOW:
            switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                case 0:
                    return GI_BOW;
                case 1:
                    return GI_QUIVER_40;
                case 2:
                    return GI_QUIVER_50;
            }
        case SLINGSHOT:
            switch (CUR_UPG_VALUE(UPG_BULLET_BAG)) {
                case 0:
                    return GI_SLINGSHOT;
                case 1:
                    return GI_BULLET_BAG_40;
                case 2:
                    return GI_BULLET_BAG_50;
            }
        case BOOMERANG:
            return GI_BOOMERANG;
        case PROGRESSIVE_HOOKSHOT:
            switch (gSaveContext.inventory.items[SLOT_HOOKSHOT]) {
            case ITEM_NONE:
                return GI_HOOKSHOT;
            case ITEM_HOOKSHOT:
                return GI_LONGSHOT;
            }
        case LENS_OF_TRUTH:
            return GI_LENS;
        case ZELDAS_LETTER:
            return GI_LETTER_ZELDA;
        case MEGATON_HAMMER:
            return GI_HAMMER;
        case PRESCRIPTION:
            return GI_PRESCRIPTION;
        case HYLIAN_SHIELD:
            return GI_SHIELD_HYLIAN;
        case MIRROR_SHIELD:
            return GI_SHIELD_MIRROR;
        case GORON_TUNIC:
            return GI_TUNIC_GORON;
        case ZORA_TUNIC:
            return GI_TUNIC_ZORA;
        case IRON_BOOTS:
            return GI_BOOTS_IRON;
        case HOVER_BOOTS:
            return GI_BOOTS_HOVER;
        case BOMB_BAG:
            switch (CUR_UPG_VALUE(UPG_BOMB_BAG)) {
                case ITEM_NONE:
                    return GI_BOMB_BAG_20;
                case ITEM_BOMB_BAG_20:
                    return GI_BOMB_BAG_30;
                case ITEM_BOMB_BAG_30:
                    return GI_BOMB_BAG_40;
            }
        case PROGRESSIVE_STRENGTH_UPGRADE:
            switch (CUR_UPG_VALUE(UPG_STRENGTH)) {
                case 0:
                    return GI_BRACELET;
                case 1:
                    return GI_GAUNTLETS_SILVER;
                case 2:
                    return GI_GAUNTLETS_GOLD;
            }
        case PROGRESSIVE_SCALE:
            switch (CUR_UPG_VALUE(UPG_SCALE)) {
                case 0:
                    return GI_SCALE_SILVER;
                case 1:
                    return GI_SCALE_GOLD;
            }
        case STONE_OF_AGONY:
            return GI_STONE_OF_AGONY;
        case GERUDO_MEMBERSHIP_CARD:
            return GI_GERUDO_CARD;
        case HEART_CONTAINER:
            // todo figure out what GI_HEART_CONTAINER_2 is
            return GI_HEART_CONTAINER;
        // todo implement dungeon-specific keys/keyrings
        case BOSS_KEY:
        case BOSS_KEY_FIRE_TEMPLE:
        case BOSS_KEY_FOREST_TEMPLE:
        case BOSS_KEY_GANONS_CASTLE:
        case BOSS_KEY_SHADOW_TEMPLE:
        case BOSS_KEY_SPIRIT_TEMPLE:
        case BOSS_KEY_WATER_TEMPLE:
            return GI_KEY_BOSS;
        case SMALL_KEY:
        case SMALL_KEY_BOTTOM_OF_THE_WELL:
        case SMALL_KEY_FIRE_TEMPLE:
        case SMALL_KEY_FOREST_TEMPLE:
        case SMALL_KEY_GANONS_CASTLE:
        case SMALL_KEY_GERUDO_TRAINING_GROUND:
        case SMALL_KEY_SHADOW_TEMPLE:
        case SMALL_KEY_SPIRIT_TEMPLE:
        case SMALL_KEY_THIEVES_HIDEOUT:
        case SMALL_KEY_WATER_TEMPLE:
            return GI_KEY_SMALL;
        // todo implement dungeon-specific maps/compasses
        case MAP_DEKU_TREE:
        case MAP_DODONGOS_CAVERN:
        case MAP_JABU_JABUS_BELLY:
        case MAP_FOREST_TEMPLE:
        case MAP_FIRE_TEMPLE:
        case MAP_WATER_TEMPLE:
        case MAP_SPIRIT_TEMPLE:
        case MAP_SHADOW_TEMPLE:
        case MAP_BOTTOM_OF_THE_WELL:
        case MAP_ICE_CAVERN:
            return GI_MAP;
        case COMPASS_DEKU_TREE:
        case COMPASS_DODONGOS_CAVERN:
        case COMPASS_JABU_JABUS_BELLY:
        case COMPASS_FOREST_TEMPLE:
        case COMPASS_FIRE_TEMPLE:
        case COMPASS_WATER_TEMPLE:
        case COMPASS_SPIRIT_TEMPLE:
        case COMPASS_SHADOW_TEMPLE:
        case COMPASS_BOTTOM_OF_THE_WELL:
        case COMPASS_ICE_CAVERN:
            return GI_COMPASS;
        case MAGIC_METER:
            switch (gSaveContext.magicLevel) {
                case 0:
                    return GI_MAGIC_SMALL;
                case 1:
                    return GI_MAGIC_LARGE;
            }
        case PROGRESSIVE_WALLET:
            switch (CUR_UPG_VALUE(UPG_WALLET)) {
                case 0:
                    return GI_WALLET_ADULT;
                case 1:
                    return GI_WALLET_GIANT;
            }
        case RECOVERY_HEART:
            return GI_HEART;
        case RUPEE_1:
            return GI_RUPEE_GREEN;
        case RUPEES_5:
            return GI_RUPEE_BLUE;
        case RUPEES_20:
            return GI_RUPEE_RED;
        case RUPEES_50:
            return GI_RUPEE_PURPLE;
        case RUPEES_200:
            return GI_RUPEE_GOLD;
        case FIRE_ARROWS:
            return GI_ARROW_FIRE;
        case ICE_ARROWS:
            return GI_ARROW_ICE;
        case LIGHT_ARROWS:
            return GI_ARROW_LIGHT;
        case DINS_FIRE:
            return GI_DINS_FIRE;
        case FARORES_WIND:
            return GI_FARORES_WIND;
        case NAYRUS_LOVE:
            return GI_NAYRUS_LOVE;
        case DEKU_NUTS_10:
            return GI_NUTS_10;
        case BOMBS_10:
            return GI_BOMBS_10;
        case BOMBS_20:
            return GI_BOMBS_20;
        case DEKU_SEEDS_30:
            return GI_SEEDS_30;
        case BOTTLE_WITH_BIG_POE:
            return GI_BIG_POE;
        case BOTTLE:
            return GI_BOTTLE;
        case BOTTLE_WITH_BLUE_FIRE:
            return GI_BLUE_FIRE;
        case BOTTLE_WITH_BLUE_POTION:
            return GI_POTION_BLUE;
        case BOTTLE_WITH_BUGS:
            return GI_BUGS;
        case BOTTLE_WITH_FAIRY:
            return GI_FAIRY;
        case BOTTLE_WITH_FISH:
            return GI_FISH;
        case BOTTLE_WITH_GREEN_POTION:
            return GI_POTION_GREEN;
        case BOTTLE_WITH_MILK:
            return GI_MILK_BOTTLE;
        case BOTTLE_WITH_POE:
            return GI_POE;
        case BOTTLE_WITH_RED_POTION:
            return GI_POTION_RED;
        case PIECE_OF_HEART_TREASURE_CHEST_GAME:
            return GI_HEART_PIECE_WIN;
        case DEKU_STICK_CAPACITY:
            switch (CUR_UPG_VALUE(UPG_STICKS)) {
                case 0:
                    return GI_STICK_UPGRADE_20;
                case 1:
                    return GI_STICK_UPGRADE_30;
            }
        case DEKU_NUT_CAPACITY:
            switch (CUR_UPG_VALUE(UPG_NUTS)) {
                case 0:
                    return GI_NUT_UPGRADE_30;
                case 1:
                    return GI_NUT_UPGRADE_40;
            }
        case RUTOS_LETTER:
            return GI_LETTER_RUTO;
        case ARROWS_5:
            return GI_ARROWS_SMALL;
        case ARROWS_10:
            return GI_ARROWS_MEDIUM;
        case ARROWS_30:
            return GI_ARROWS_LARGE;
        case DEKU_STICK_1:
            return GI_STICKS_1;

        case LIGHT_MEDALLION:
            return GI_MEDALLION_LIGHT;
        case FOREST_MEDALLION:
            return GI_MEDALLION_FOREST;
        case FIRE_MEDALLION:
            return GI_MEDALLION_FIRE;
        case WATER_MEDALLION:
            return GI_MEDALLION_WATER;
        case SHADOW_MEDALLION:
            return GI_MEDALLION_SHADOW;
        case SPIRIT_MEDALLION:
            return GI_MEDALLION_SPIRIT;

        case KOKIRI_EMERALD:
            return GI_STONE_KOKIRI;
        case GORON_RUBY:
            return GI_STONE_GORON;
        case ZORA_SAPPHIRE:
            return GI_STONE_ZORA;

        case ZELDAS_LULLABY:
            return GI_ZELDAS_LULLABY;
        case SUNS_SONG:
            return GI_SUNS_SONG;
        case EPONAS_SONG:
            return GI_EPONAS_SONG;
        case SONG_OF_STORMS:
            return GI_SONG_OF_STORMS;
        case SONG_OF_TIME:
            return GI_SONG_OF_TIME;
        case SARIAS_SONG:
            return GI_SARIAS_SONG;

        case MINUET_OF_FOREST:
            return GI_MINUET_OF_FOREST;
        case BOLERO_OF_FIRE:
            return GI_BOLERO_OF_FIRE;
        case SERENADE_OF_WATER:
            return GI_SERENADE_OF_WATER;
        case NOCTURNE_OF_SHADOW:
            return GI_NOCTURNE_OF_SHADOW;
        case REQUIEM_OF_SPIRIT:
            return GI_REQUIEM_OF_SPIRIT;
        case PRELUDE_OF_LIGHT:
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
        return UNKNOWN_CHECK;
    }

    switch(sceneNum) {
        case 82:
            switch (actorId) {
                case 316:
                    if (LINK_IS_ADULT) {
                        return KAK_ANJU_AS_ADULT;
                    } else {
                        return KAK_ANJU_AS_CHILD;
                    }
            }
        case 17:
            switch (actorId) {
                case 95:
                    return DEKU_TREE_QUEEN_GOHMA_HEART;
                case 93:
                    return QUEEN_GOHMA;
            }
        case 18:
            switch (actorId) {
                case 95:
                    return DODONGOS_CAVERN_KING_DODONGO_HEART;
                case 93:
                    return KING_DODONGO;
            }
        case 19:
            switch (actorId) {
                case 95:
                    return JABU_JABUS_BELLY_BARINADE_HEART;
                case 93:
                    return BARINADE;
            }
        case 20:
            switch (actorId) {
                case 95:
                    return FOREST_TEMPLE_PHANTOM_GANON_HEART;
                case 93:
                    return PHANTOM_GANON;
            }
        case 21:
            switch (actorId) {
                case 95:
                    return FIRE_TEMPLE_VOLVAGIA_HEART;
                case 93:
                    return VOLVAGIA;
            }
        case 22:
            switch (actorId) {
                case 95:
                    return WATER_TEMPLE_MORPHA_HEART;
                case 93:
                    return MORPHA;
            }
        case 23:
            switch (actorId) {
                case 95:
                    return SPIRIT_TEMPLE_TWINROVA_HEART;
                case 93:
                    return TWINROVA;
            }
        case 24:
            switch (actorId) {
                case 95:
                    return SHADOW_TEMPLE_BONGO_BONGO_HEART;
                case 93:
                    return BONGO_BONGO;
            }
        case 40:
            switch(actorParams) {
                case 22944:
                    return KF_MIDOS_TOP_LEFT_CHEST;
                case 22945:
                    return KF_MIDOS_TOP_RIGHT_CHEST;
                case 22914:
                    return KF_MIDOS_BOTTOM_LEFT_CHEST;
                case 22787:
                    return KF_MIDOS_BOTTOM_RIGHT_CHEST;
            }
        case 84:
            switch (actorParams) {
                case 1030:
                    return ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH;
                case 2822:
                    return ZR_NEAR_DOMAIN_FREESTANDING_POH;
            }
        case 85:
            switch(actorParams) {
                case 1248:
                    return KF_KOKIRI_SWORD_CHEST;
            }
        case 62:
            switch(actorParams) {
                case 22988:
                    return KF_STORMS_GROTTO_CHEST;
                case 22964:
                    return LW_NEAR_SHORTCUTS_GROTTO_CHEST;
                case 31409:
                    return SFM_WOLFOS_GROTTO_CHEST;
                case 22944:
                    return HF_NEAR_MARKET_GROTTO_CHEST;
                case 22978:
                    return HF_SOUTHEAST_GROTTO_CHEST;
                case 22947:
                    return HF_OPEN_GROTTO_CHEST;
                case 22984:
                    return KAK_OPEN_GROTTO_CHEST;
                case 31434:
                    return KAK_REDEAD_GROTTO_CHEST;
                case 23255:
                    return DMT_STORMS_GROTTO_CHEST;
                case 23802:
                    return DMC_UPPER_GROTTO_CHEST;
                case 22985:
                    return ZR_OPEN_GROTTO_CHEST;
                case 262:
                    return HF_TEKTITE_GROTTO_FREESTANDING_POH;
            }
        case 64:
            switch(actorParams) {
                case 21824:
                    return GRAVEYARD_SHIELD_GRAVE_CHEST;
            }
        case 63:
            switch(actorParams) {
                case -22592:
                    return GRAVEYARD_HEART_PIECE_GRAVE_CHEST;
            }
        case 65:
            switch(actorParams) {
                case -32736:
                    return GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST;
            }
        case 72:
            // todo make sure dampe gives the PoH when the chest is opened
            // default logic seems to be give PoH when hookshot in inventory
            switch(actorParams) {
                case 4352:
                    return GRAVEYARD_HOOKSHOT_CHEST;
                case 262:
                    return KAK_WINDMILL_FREESTANDING_POH;
                case 1798:
                    return GRAVEYARD_DAMPE_RACE_FREESTANDING_POH;
            }
        case 96:
            switch(actorParams) {
                case 7686:
                    return DMT_FREESTANDING_POH;
                case 23201:
                    return DMT_CHEST;
            }
        case 98:
            switch(actorParams) {
                case 23232:
                    return GC_MAZE_LEFT_CHEST;
                case 23201:
                    return GC_MAZE_RIGHT_CHEST;
                case 23202:
                    return GC_MAZE_CENTER_CHEST;
                case 7942:
                    return GC_POT_FREESTANDING_POH;
            }
        case 88:
            switch(actorParams) {
                case -18496:
                    return ZD_CHEST;
            }
        case 90:
            switch(actorParams) {
                case 23200:
                    return GV_CHEST;
                case 262:
                    return GV_WATERFALL_FREESTANDING_POH;
                case 518:
                    return GV_CRATE_FREESTANDING_POH;
            }
        case 93:
            switch(actorParams) {
                case 1984:
                    return GF_CHEST;
            }
        case 94:
            switch(actorParams) {
                case -30048:
                    return WASTELAND_CHEST;
            }
        case 0:
            switch(actorParams) {
                case 2083:
                    return DEKU_TREE_MAP_CHEST;
                case 22789:
                    return DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST;
                case 161:
                    return DEKU_TREE_SLINGSHOT_CHEST;
                case 2050:
                    return DEKU_TREE_COMPASS_CHEST;
                case 22790:
                    return DEKU_TREE_COMPASS_ROOM_SIDE_CHEST;
                case 22788:
                    return DEKU_TREE_BASEMENT_CHEST;
            }
        case 1:
            switch(actorParams) {
                case 2088:
                    return DODONGOS_CAVERN_MAP_CHEST;
                case 2053:
                    return DODONGOS_CAVERN_COMPASS_CHEST;
                case 22982:
                    return DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST;
                case 1604:
                    return DODONGOS_CAVERN_BOMB_BAG_CHEST;
                case 21802:
                    return DODONGOS_CAVERN_END_OF_BRIDGE_CHEST;
                case 20512:
                    return DODONGOS_CAVERN_BOSS_ROOM_CHEST;
            }
        case 2:
            switch(actorParams) {
                case 4289:
                    return JABU_JABUS_BELLY_BOOMERANG_CHEST;
                case 6178:
                    return JABU_JABUS_BELLY_MAP_CHEST;
                case -18428:
                    return JABU_JABUS_BELLY_COMPASS_CHEST;
            }
        case 8:
            switch(actorParams) {
                case 22600:
                    return BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST;
                case 20578:
                    return BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST;
                case 23684:
                    return BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST;
                case 22793:
                    return BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST;
                case 2049:
                    return BOTTOM_OF_THE_WELL_COMPASS_CHEST;
                case 20558:
                    return BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST;
                case 22597:
                    return BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST;
                case 21802:
                    return BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST;
                case 21836:
                    return BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST;
                case 2087:
                    return BOTTOM_OF_THE_WELL_MAP_CHEST;
                case 23760:
                    return BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST;
                case 4419:
                    return BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST;
                case 27348:
                    return BOTTOM_OF_THE_WELL_INVISIBLE_CHEST;
            }
        case 3:
            switch(actorParams) {
                case 22595:
                    return FOREST_TEMPLE_FIRST_ROOM_CHEST;
                case 30784:
                    return FOREST_TEMPLE_FIRST_STALFOS_CHEST;
                case 22789:
                    return FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST;
                case 6177:
                    return FOREST_TEMPLE_MAP_CHEST;
                case 22601:
                    return FOREST_TEMPLE_WELL_CHEST;
                case -30364:
                    return FOREST_TEMPLE_EYE_SWITCH_CHEST;
                case 10222:
                    return FOREST_TEMPLE_BOSS_KEY_CHEST;
                case 30786:
                    return FOREST_TEMPLE_FLOORMASTER_CHEST;
                case 30797:
                    return FOREST_TEMPLE_RED_POE_CHEST;
                case -20340:
                    return FOREST_TEMPLE_BOW_CHEST;
                case 6159:
                    return FOREST_TEMPLE_BLUE_POE_CHEST;
                case 22855:
                    return FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST;
                case 22827:
                    return FOREST_TEMPLE_BASEMENT_CHEST;
            }
        case 4:
            switch(actorParams) {
                case 22593:
                    return FIRE_TEMPLE_NEAR_BOSS_CHEST;
                case 31936:
                    return FIRE_TEMPLE_FLARE_DANCER_CHEST;
                case 10220:
                    return FIRE_TEMPLE_BOSS_KEY_CHEST;
                case 22596:
                    return FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST;
                case 22594:
                    return FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST;
                case 22595:
                    return FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST;
                case 22600:
                    return FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST;
                case 2090:
                    return FIRE_TEMPLE_MAP_CHEST;
                case 22603:
                    return FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST;
                case 22598:
                    return FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST;
                case 23245:
                    return FIRE_TEMPLE_SCARECROW_CHEST;
                case 2055:
                    return FIRE_TEMPLE_COMPASS_CHEST;
                case 421:
                    return FIRE_TEMPLE_MEGATON_HAMMER_CHEST;
                case 22601:
                    return FIRE_TEMPLE_HIGHEST_GORON_CHEST;
            }
        case 5:
            switch(actorParams) {
                case 2057:
                    return WATER_TEMPLE_COMPASS_CHEST;
                case 6178:
                    return WATER_TEMPLE_MAP_CHEST;
                case 22592:
                    return WATER_TEMPLE_CRACKED_WALL_CHEST;
                case 30785:
                    return WATER_TEMPLE_TORCHES_CHEST;
                case 10213:
                    return WATER_TEMPLE_BOSS_KEY_CHEST;
                case 22598:
                    return WATER_TEMPLE_CENTRAL_PILLAR_CHEST;
                case 22600:
                    return WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST;
                case 295:
                    return WATER_TEMPLE_LONGSHOT_CHEST;
                case 22595:
                    return WATER_TEMPLE_RIVER_CHEST;
                case 22602:
                    return WATER_TEMPLE_DRAGON_CHEST;
            }
        case 7:
            switch(actorParams) {
                case 6177:
                    return SHADOW_TEMPLE_MAP_CHEST;
                case 5607:
                    return SHADOW_TEMPLE_HOVER_BOOTS_CHEST;
                case 6147:
                    return SHADOW_TEMPLE_COMPASS_CHEST;
                case 22594:
                    return SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST;
                case 22668:
                    return SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST;
                case 26998:
                    return SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST;
                case 22853:
                    return SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST;
                case 22662:
                    return SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST;
                case -30652:
                    return SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST;
                case 30857:
                    return SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST;
                case 26965:
                    return SHADOW_TEMPLE_WIND_HINT_CHEST;
                case 30856:
                    return SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST;
                case 26708:
                    return SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST;
                case 22666:
                    return SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST;
                case 10219:
                    return SHADOW_TEMPLE_BOSS_KEY_CHEST;
                case 30797:
                    return SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST;
            }
        case 6:
            switch(actorParams) {
                case 21800:
                    return SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST;
                case -30656:
                    return SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST;
                case -32666:
                    return SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST;
                case -31444:
                    return SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST;
                case -18397:
                    return SPIRIT_TEMPLE_MAP_CHEST;
                case -30655:
                    return SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST;
                case 14340:
                    return SPIRIT_TEMPLE_COMPASS_CHEST;
                case 22599:
                    return SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST;
                case -30451:
                    return SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST;
                case -28786:
                    return SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST;
                case -30577:
                    return SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST;
                case -30654:
                    return SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST;
                case 22597:
                    return SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST;
                case 26900:
                    return SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST;
                case 26901:
                    return SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST;
                case 10218:
                    return SPIRIT_TEMPLE_BOSS_KEY_CHEST;
                case -29454:
                    return SPIRIT_TEMPLE_TOPMOST_CHEST;
            }
        case 92:
            switch(actorParams) {
                case 1707:
                    return SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST;
                case 13673:
                    return SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST;
                case 3334:
                    return COLOSSUS_FREESTANDING_POH;
            }
        case 9:
            switch(actorParams) {
                case 2080:
                    return ICE_CAVERN_MAP_CHEST;
                case 2049:
                    return ICE_CAVERN_COMPASS_CHEST;
                case 5570:
                    return ICE_CAVERN_IRON_BOOTS_CHEST;
                case 262:
                    return ICE_CAVERN_FREESTANDING_POH;
            }
        case 11:
            switch(actorParams) {
                case -30573:
                    return GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST;
                case -30393:
                    return GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST;
                case -30656:
                    return GERUDO_TRAINING_GROUND_STALFOS_CHEST;
                case 31089:
                    return GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST;
                case 31439:
                    return GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST;
                case 30862:
                    return GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST;
                case 26708:
                    return GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST;
                case 24450:
                    return GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST;
                case -30653:
                    return GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST;
                case 22596:
                    return GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST;
                case 31058:
                    return GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST;
                case 22608:
                    return GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST;
                case 23877:
                    return GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST;
                case 22888:
                    return GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST;
                case -30643:
                    return GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST;
                case -30655:
                    return GERUDO_TRAINING_GROUND_BEAMOS_CHEST;
                case 22603:
                    return GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST;
                case 23206:
                    return GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST;
                case 22986:
                    return GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST;
                case 22889:
                    return GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST;
                case 2860:
                    return GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST;
            }
        case 13:
            switch(actorParams) {
                case 30857:
                    return GANONS_CASTLE_FOREST_TRIAL_CHEST;
                case 24455:
                    return GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST;
                case 22790:
                    return GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST;
                case 22664:
                    return GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST;
                case 14021:
                    return GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST;
                case 22668:
                    return GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST;
                case 24459:
                    return GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST;
                case 22797:
                    return GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST;
                case 24462:
                    return GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST;
                case 22890:
                    return GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST;
                case 24463:
                    return GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST;
                case 30800:
                    return GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST;
                case -30639:
                    return GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST;
                case -29326:
                    return GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST;
                case 26964:
                    return GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST;
            }
        case 10:
            switch(actorParams) {
                case 10219:
                    return GANONS_TOWER_BOSS_KEY_CHEST;
            }
        case 87:
            switch(actorParams) {
                case 7686:
                    return LH_FREESTANDING_POH;
            }
        case 76:
            switch(actorParams) {
                case 262:
                    return LLR_FREESTANDING_POH;
            }
        case 55:
            switch(actorParams) {
                case 262:
                    return KAK_IMPAS_HOUSE_FREESTANDING_POH;
            }
        case 83:
            switch(actorParams) {
                case 1030:
                    return GRAVEYARD_FREESTANDING_POH;
            }
        case 97:
            switch(actorParams) {
                case 2054:
                    return DMC_VOLCANO_FREESTANDING_POH;
                case 518:
                    return DMC_WALL_FREESTANDING_POH;
            }
        case 89:
            switch(actorParams) {
                case 5126:
                    return ZF_BOTTOM_FREESTANDING_POH;
                case 262:
                    return ZF_ICEBERG_FREESTANDING_POH;
            }
        case 59:
            // using fountainType as actorParams 
            switch(actorParams) {
                case 1:
                    return DMT_GREAT_FAIRY_REWARD;
                case 2:
                    return DMC_GREAT_FAIRY_REWARD;
                case 3:
                    return OGC_GREAT_FAIRY_REWARD;
            }
        case 61:
            // using fountainType as actorParams
            switch(actorParams) {
                case 1:
                    return ZF_GREAT_FAIRY_REWARD;
                case 2:
                    return HC_GREAT_FAIRY_REWARD;
                case 3:
                    return COLOSSUS_GREAT_FAIRY_REWARD;
            }
    }

    return UNKNOWN_CHECK;
}
