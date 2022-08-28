#include "randomizer.h"
#include "Lib/nlohmann/json.hpp"
#include "3drando/settings.hpp"
#include <fstream>
#include <variables.h>
#include <macros.h>
#include <objects/gameplay_keep/gameplay_keep.h>
#include <functions.h>
#include <Cvar.h>
#include <textures/icon_item_static/icon_item_static.h>
#include <textures/icon_item_24_static/icon_item_24_static.h>
#include "../libultraship/ImGuiImpl.h"
#include <thread>
#include "3drando/rando_main.hpp"
#include <soh/Enhancements/debugger/ImGuiHelpers.h>
#include "Lib/ImGui/imgui_internal.h"
#include <soh/Enhancements/custom-message/CustomMessageTypes.h>
#include <soh/Enhancements/item-tables/ItemTableManager.h>
#include <stdexcept>
#include "randomizer_check_objects.h"
#include <sstream>
#include "draw.h"

using json = nlohmann::json;
using namespace std::literals::string_literals;

std::unordered_map<uint8_t, Sprite> gSeedTextures;
std::unordered_map<std::string, RandomizerCheck> SpoilerfileCheckNameToEnum;
std::set<RandomizerCheck> excludedLocations;

u8 generated;

const std::string Randomizer::getItemMessageTableID = "Randomizer";
const std::string Randomizer::hintMessageTableID = "RandomizerHints";
const std::string Randomizer::merchantMessageTableID = "RandomizerMerchants";
const std::string Randomizer::rupeeMessageTableID = "RandomizerRupees";
const std::string Randomizer::NaviRandoMessageTableID = "RandomizerNavi";

static const char* englishRupeeNames[44] = {
    "Rupees",    "Bitcoin",   "Bananas",     "Cornflakes", "Gummybears", "Floopies", "Dollars",    "Lemmings",
    "Emeralds",  "Bucks",     "Rubles",      "Diamonds",   "Moons",      "Stars",    "Mana",       "Doll Hairs",
    "Dogecoin",  "Mushrooms", "Experience",  "Friends",    "Coins",      "Rings",    "Gil",        "Pokédollars",
    "Bells",     "Orbs",      "Bottle Caps", "Simoleons",  "Pokémon",    "Toys",     "Smackaroos", "Zorkmids",
    "Zenny",     "Bones",     "Souls",       "Studs",      "Munny",      "Rubies",   "Gald",       "Gold",
    "Shillings", "Pounds",    "Glimmer",     "Potch"
};

static const char* germanRupeeNames[1] = { 
    "Rubine"
};

static const char* frenchRupeeNames[36] = {
    "Rubis",       "Bitcoin", "Bananes",   "Euros",     "Dollars", "Émeraudes",  "Joyaux",   "Diamants",
    "Balles",      "Pokémon", "Pièces",    "Lunes",     "Étoiles", "Dogecoin",   "Anneaux",  "Radis",
    "Pokédollars", "Zennies", "Pépètes",   "Mailles",   "Éthers",  "Clochettes", "Capsules", "Gils",
    "Champignons", "Blés",    "Halos",     "Munnies",   "Orens",   "Florens",    "Crédits",  "Galds",
    "Bling",       "Orbes",   "Baguettes", "Croissants"
};

Randomizer::Randomizer() {
    Sprite bowSprite = { dgFairyBowIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 0 };
    gSeedTextures[0] = bowSprite;

    Sprite bombchuSprite = { dgBombchuIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 1 };
    gSeedTextures[1] = bombchuSprite;

    Sprite beansSprite = { dgMagicBeansIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 2 };
    gSeedTextures[2] = beansSprite;

    Sprite milkSprite = { dgMilkFullIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 3 };
    gSeedTextures[3] = milkSprite;

    Sprite frogSprite = { dgEyeBallFrogIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 4 };
    gSeedTextures[4] = frogSprite;

    Sprite mirrorShieldSprite = { dgMirrorShieldIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 5 };
    gSeedTextures[5] = mirrorShieldSprite;

    Sprite hoverBootsSprite = { dgHoverBootsIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 6 };
    gSeedTextures[6] = hoverBootsSprite;

    Sprite megatonHammerSprite = { dgMegatonHammerIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 7 };
    gSeedTextures[7] = megatonHammerSprite;

    Sprite silverGauntletsSprite = { dgSilverGauntletsIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 8 };
    gSeedTextures[8] = silverGauntletsSprite;

    Sprite ootOcarinaSprite = { dgOcarinaofTimeIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 9 };
    gSeedTextures[9] = ootOcarinaSprite;

    for (auto [randomizerCheck, rcObject] : RandomizerCheckObjects::GetAllRCObjects()) {
        SpoilerfileCheckNameToEnum[rcObject.rcSpoilerName] = rcObject.rc;
    }
    SpoilerfileCheckNameToEnum["Invalid Location"] = RC_UNKNOWN_CHECK;
    SpoilerfileCheckNameToEnum["Link's Pocket"] = RC_LINKS_POCKET;
}

Sprite* Randomizer::GetSeedTexture(uint8_t index) {
    return &gSeedTextures[index];
}

Randomizer::~Randomizer() { 
    this->randoSettings.clear();
    this->itemLocations.clear();
    this->randomizerMerchantPrices.clear();
}

std::unordered_map<std::string, RandomizerInf> spoilerFileTrialToEnum = {
    { "the Forest Trial", RAND_INF_TRIALS_DONE_FOREST_TRIAL },
    { "l'épreuve de la Forêt", RAND_INF_TRIALS_DONE_FOREST_TRIAL },
    { "the Fire Trial", RAND_INF_TRIALS_DONE_FIRE_TRIAL },
    { "l'épreuve du Feu", RAND_INF_TRIALS_DONE_FIRE_TRIAL },
    { "the Water Trial", RAND_INF_TRIALS_DONE_WATER_TRIAL },
    { "l'épreuve de l'Eau", RAND_INF_TRIALS_DONE_WATER_TRIAL },
    { "the Spirit Trial", RAND_INF_TRIALS_DONE_SPIRIT_TRIAL },
    { "l'épreuve de l'Esprit", RAND_INF_TRIALS_DONE_SPIRIT_TRIAL },
    { "the Shadow Trial", RAND_INF_TRIALS_DONE_SHADOW_TRIAL },
    { "l'épreuve de l'Ombre", RAND_INF_TRIALS_DONE_SHADOW_TRIAL },
    { "the Light Trial", RAND_INF_TRIALS_DONE_LIGHT_TRIAL },
    { "l'épreuve de la Lumière", RAND_INF_TRIALS_DONE_LIGHT_TRIAL }
};

std::unordered_map<s16, s16> getItemIdToItemId = {
    { GI_BOW, ITEM_BOW },
    { GI_ARROW_FIRE, ITEM_ARROW_FIRE },
    { GI_DINS_FIRE, ITEM_DINS_FIRE },
    { GI_SLINGSHOT, ITEM_SLINGSHOT },
    { GI_OCARINA_FAIRY, ITEM_OCARINA_FAIRY },
    { GI_OCARINA_OOT, ITEM_OCARINA_TIME },
    { GI_HOOKSHOT, ITEM_HOOKSHOT },
    { GI_LONGSHOT, ITEM_LONGSHOT },
    { GI_ARROW_ICE, ITEM_ARROW_ICE },
    { GI_FARORES_WIND, ITEM_FARORES_WIND },
    { GI_BOOMERANG, ITEM_BOOMERANG },
    { GI_LENS, ITEM_LENS },
    { GI_HAMMER, ITEM_HAMMER },
    { GI_ARROW_LIGHT, ITEM_ARROW_LIGHT },
    { GI_NAYRUS_LOVE, ITEM_NAYRUS_LOVE },
    { GI_BOTTLE, ITEM_BOTTLE },
    { GI_POTION_RED, ITEM_POTION_RED },
    { GI_POTION_GREEN, ITEM_POTION_GREEN },
    { GI_POTION_BLUE, ITEM_POTION_BLUE },
    { GI_FAIRY, ITEM_FAIRY },
    { GI_FISH, ITEM_FISH },
    { GI_MILK_BOTTLE, ITEM_MILK_BOTTLE },
    { GI_LETTER_RUTO, ITEM_LETTER_RUTO },
    { GI_BLUE_FIRE, ITEM_BLUE_FIRE },
    { GI_BUGS, ITEM_BUG },
    { GI_BIG_POE, ITEM_BIG_POE },
    { GI_POE, ITEM_POE },
    { GI_WEIRD_EGG, ITEM_WEIRD_EGG },
    { GI_LETTER_ZELDA, ITEM_LETTER_ZELDA },
    { GI_POCKET_EGG, ITEM_POCKET_EGG },
    { GI_COJIRO, ITEM_COJIRO },
    { GI_ODD_MUSHROOM, ITEM_ODD_MUSHROOM },
    { GI_ODD_POTION, ITEM_ODD_POTION },
    { GI_SAW, ITEM_SAW },
    { GI_SWORD_BROKEN, ITEM_SWORD_BROKEN },
    { GI_PRESCRIPTION, ITEM_PRESCRIPTION },
    { GI_FROG, ITEM_FROG },
    { GI_EYEDROPS, ITEM_EYEDROPS },
    { GI_CLAIM_CHECK, ITEM_CLAIM_CHECK }
};

std::unordered_map<std::string, RandomizerGet> SpoilerfileGetNameToEnum = {
    { "No Item", RG_NONE },
    { "Rien", RG_NONE },
    { "Kokiri Sword", RG_KOKIRI_SWORD },
    { "Épée Kokiri", RG_KOKIRI_SWORD },
    { "Giant's Knife", RG_GIANTS_KNIFE },
    { "Lame des Géants", RG_GIANTS_KNIFE },
    { "Biggoron's Sword", RG_BIGGORON_SWORD },
    { "Épée de Biggoron", RG_BIGGORON_SWORD },
    { "Deku Shield", RG_DEKU_SHIELD },
    { "Bouclier Mojo", RG_DEKU_SHIELD },
    { "Hylian Shield", RG_HYLIAN_SHIELD },
    { "Bouclier Hylien", RG_HYLIAN_SHIELD },
    { "Mirror Shield", RG_MIRROR_SHIELD },
    { "Bouclier Miroir", RG_MIRROR_SHIELD },
    { "Goron Tunic", RG_GORON_TUNIC },
    { "Tunique Goron", RG_GORON_TUNIC },
    { "Zora Tunic", RG_ZORA_TUNIC },
    { "Tunique Zora", RG_ZORA_TUNIC },
    { "Iron Boots", RG_IRON_BOOTS },
    { "Bottes de plomb", RG_IRON_BOOTS },
    { "Hover Boots", RG_HOVER_BOOTS },
    { "Bottes des airs", RG_HOVER_BOOTS },
    { "Boomerang", RG_BOOMERANG },
    { "Lens of Truth", RG_LENS_OF_TRUTH },
    { "Monocle de Vérité", RG_LENS_OF_TRUTH },
    { "Megaton Hammer", RG_MEGATON_HAMMER },
    { "Masse des Titans", RG_MEGATON_HAMMER },
    { "Stone of Agony", RG_STONE_OF_AGONY },
    { "Pierre de Souffrance", RG_STONE_OF_AGONY },
    { "Din's Fire", RG_DINS_FIRE },
    { "Feu de Din", RG_DINS_FIRE },
    { "Farore's Wind", RG_FARORES_WIND },
    { "Vent de Farore", RG_FARORES_WIND },
    { "Nayru's Love", RG_NAYRUS_LOVE },
    { "Amour de Nayru", RG_NAYRUS_LOVE },
    { "Fire Arrow", RG_FIRE_ARROWS },
    { "Flèche de Feu", RG_FIRE_ARROWS },
    { "Ice Arrow", RG_ICE_ARROWS },
    { "Flèche de Glace", RG_ICE_ARROWS },
    { "Light Arrow", RG_LIGHT_ARROWS },
    { "Flèche de Lumière", RG_LIGHT_ARROWS },
    { "Gerudo Membership Card", RG_GERUDO_MEMBERSHIP_CARD },
    { "Carte Gerudo", RG_GERUDO_MEMBERSHIP_CARD },
    { "Magic Bean", RG_MAGIC_BEAN },
    { "Haricots Magiques", RG_MAGIC_BEAN },
    { "Magic Bean Pack", RG_MAGIC_BEAN_PACK },
    { "Paquet de Haricots Magiques", RG_MAGIC_BEAN_PACK },
    { "Double Defense", RG_DOUBLE_DEFENSE },
    { "Double Défence", RG_DOUBLE_DEFENSE },
    { "Weird Egg", RG_WEIRD_EGG },
    { "Oeuf Curieux", RG_WEIRD_EGG },
    { "Zelda's Letter", RG_ZELDAS_LETTER },
    { "Lettre de Zelda", RG_ZELDAS_LETTER },
    { "Pocket Egg", RG_POCKET_EGG },
    { "Oeuf de poche", RG_POCKET_EGG },
    { "Cojiro", RG_COJIRO },
    { "P'tit Poulet", RG_COJIRO },
    { "Odd Mushroom", RG_ODD_MUSHROOM },
    { "Champignon Suspect", RG_ODD_MUSHROOM },
    { "Odd Potion", RG_ODD_POTION },
    { "Mixture Suspecte ", RG_ODD_POTION },
    { "Poacher's Saw", RG_POACHERS_SAW },
    { "Scie du Chasseur", RG_POACHERS_SAW },
    { "Broken Goron's Sword", RG_BROKEN_SWORD },
    { "Épée Brisée de Goron", RG_BROKEN_SWORD },
    { "Prescription", RG_PRESCRIPTION },
    { "Ordonnance", RG_PRESCRIPTION },
    { "Eyeball Frog", RG_EYEBALL_FROG },
    { "Crapaud-qui-louche", RG_EYEBALL_FROG },
    { "World's Finest Eyedrops", RG_EYEDROPS },
    { "Super Gouttes", RG_EYEDROPS },
    { "Claim Check", RG_CLAIM_CHECK },
    { "Certificat", RG_CLAIM_CHECK },
    { "Gold Skulltula Token", RG_GOLD_SKULLTULA_TOKEN },
    { "Symbole de Skulltula d'Or", RG_GOLD_SKULLTULA_TOKEN },
    { "Progressive Hookshot", RG_PROGRESSIVE_HOOKSHOT },
    { "Grappin (prog.)", RG_PROGRESSIVE_HOOKSHOT },
    { "Progressive Strength Upgrade", RG_PROGRESSIVE_STRENGTH },
    { "Amélioration de Force (prog.)", RG_PROGRESSIVE_STRENGTH },
    { "Progressive Bomb Bag", RG_PROGRESSIVE_BOMB_BAG },
    { "Sac de Bombes (prog.)", RG_PROGRESSIVE_BOMB_BAG },
    { "Progressive Bow", RG_PROGRESSIVE_BOW },
    { "Arc (prog.)", RG_PROGRESSIVE_BOW },
    { "Progressive Slingshot", RG_PROGRESSIVE_SLINGSHOT },
    { "Lance-Pierre (prog.)", RG_PROGRESSIVE_SLINGSHOT },
    { "Progressive Wallet", RG_PROGRESSIVE_WALLET },
    { "Bourse (prog.)", RG_PROGRESSIVE_WALLET },
    { "Progressive Scale", RG_PROGRESSIVE_SCALE },
    { "Écaille (prog.)", RG_PROGRESSIVE_SCALE },
    { "Progressive Nut Capacity", RG_PROGRESSIVE_NUT_UPGRADE },
    { "Capacité de Noix (prog.)", RG_PROGRESSIVE_NUT_UPGRADE },
    { "Progressive Stick Capacity", RG_PROGRESSIVE_STICK_UPGRADE },
    { "Capacité de Bâtons (prog.)", RG_PROGRESSIVE_STICK_UPGRADE },
    { "Progressive Bombchu", RG_PROGRESSIVE_BOMBCHUS },
    { "Missiles (prog.)", RG_PROGRESSIVE_BOMBCHUS },
    { "Progressive Magic Meter", RG_PROGRESSIVE_MAGIC_METER },
    { "Jauge de Magie (prog.)", RG_PROGRESSIVE_MAGIC_METER },
    { "Progressive Ocarina", RG_PROGRESSIVE_OCARINA },
    { "Ocarina (prog.)", RG_PROGRESSIVE_OCARINA },
    { "Progressive Goron Sword", RG_PROGRESSIVE_GORONSWORD },
    { "Épée Goron (prog.)", RG_PROGRESSIVE_GORONSWORD },
    { "Empty Bottle", RG_EMPTY_BOTTLE },
    { "Bouteille Vide", RG_EMPTY_BOTTLE },
    { "Bottle with Milk", RG_BOTTLE_WITH_MILK },
    { "Bouteille avec du Lait", RG_BOTTLE_WITH_MILK },
    { "Bottle with Red Potion", RG_BOTTLE_WITH_RED_POTION },
    { "Bouteille avec une Potion Rouge", RG_BOTTLE_WITH_RED_POTION },
    { "Bottle with Green Potion", RG_BOTTLE_WITH_GREEN_POTION },
    { "Bouteille avec une Potion Verte", RG_BOTTLE_WITH_GREEN_POTION },
    { "Bottle with Blue Potion", RG_BOTTLE_WITH_BLUE_POTION },
    { "Bouteille avec une Potion Bleue", RG_BOTTLE_WITH_BLUE_POTION },
    { "Bottle with Fairy", RG_BOTTLE_WITH_FAIRY },
    { "Bouteille avec une Fée", RG_BOTTLE_WITH_FAIRY },
    { "Bottle with Fish", RG_BOTTLE_WITH_FISH },
    { "Bouteille avec un Poisson", RG_BOTTLE_WITH_FISH },
    { "Bottle with Blue Fire", RG_BOTTLE_WITH_BLUE_FIRE },
    { "Bouteille avec une Flamme Bleue", RG_BOTTLE_WITH_BLUE_FIRE },
    { "Bottle with Bugs", RG_BOTTLE_WITH_BUGS },
    { "Bouteille avec des Insectes", RG_BOTTLE_WITH_BUGS },
    { "Bottle with Poe", RG_BOTTLE_WITH_POE },
    { "Bouteille avec un Esprit", RG_BOTTLE_WITH_POE },
    { "Bottle with Ruto's Letter", RG_RUTOS_LETTER },
    { "Bouteille avec la Lettre de Ruto", RG_RUTOS_LETTER },
    { "Bottle with Big Poe", RG_BOTTLE_WITH_BIG_POE },
    { "Bouteille avec une Âme", RG_BOTTLE_WITH_BIG_POE },
    { "Zelda's Lullaby", RG_ZELDAS_LULLABY },
    { "Berceuse de Zelda", RG_ZELDAS_LULLABY },
    { "Epona's Song", RG_EPONAS_SONG },
    { "Chant d'Épona", RG_EPONAS_SONG },
    { "Saria's Song", RG_SARIAS_SONG },
    { "Chant de Saria", RG_SARIAS_SONG },
    { "Sun's Song", RG_SUNS_SONG },
    { "Chant du Soleil", RG_SUNS_SONG },
    { "Song of Time", RG_SONG_OF_TIME },
    { "Chant du Temps", RG_SONG_OF_TIME },
    { "Song of Storms", RG_SONG_OF_STORMS },
    { "Chant des Tempêtes", RG_SONG_OF_STORMS },
    { "Minuet of Forest", RG_MINUET_OF_FOREST },
    { "Menuet des Bois", RG_MINUET_OF_FOREST },
    { "Bolero of Fire", RG_BOLERO_OF_FIRE },
    { "Boléro du Feu", RG_BOLERO_OF_FIRE },
    { "Serenade of Water", RG_SERENADE_OF_WATER },
    { "Sérénade de l'Eau", RG_SERENADE_OF_WATER },
    { "Requiem of Spirit", RG_REQUIEM_OF_SPIRIT },
    { "Requiem des Esprits", RG_REQUIEM_OF_SPIRIT },
    { "Nocturne of Shadow", RG_NOCTURNE_OF_SHADOW },
    { "Nocturne de l'Ombre", RG_NOCTURNE_OF_SHADOW },
    { "Prelude of Light", RG_PRELUDE_OF_LIGHT },
    { "Prélude de la Lumière", RG_PRELUDE_OF_LIGHT },
    { "Great Deku Tree Map", RG_DEKU_TREE_MAP },
    { "Carte de l'Arbre Mojo", RG_DEKU_TREE_MAP },
    { "Dodongo's Cavern Map", RG_DODONGOS_CAVERN_MAP },
    { "Carte de la Caverne Dodongo", RG_DODONGOS_CAVERN_MAP },
    { "Jabu-Jabu's Belly Map", RG_JABU_JABUS_BELLY_MAP },
    { "Carte du Ventre de Jabu-Jabu", RG_JABU_JABUS_BELLY_MAP },
    { "Forest Temple Map", RG_FOREST_TEMPLE_MAP },
    { "Carte du Temple de la Forêt", RG_FOREST_TEMPLE_MAP },
    { "Fire Temple Map", RG_FIRE_TEMPLE_MAP },
    { "Carte du Temple du Feu", RG_FIRE_TEMPLE_MAP },
    { "Water Temple Map", RG_WATER_TEMPLE_MAP },
    { "Carte du Temple de l'Eau", RG_WATER_TEMPLE_MAP },
    { "Spirit Temple Map", RG_SPIRIT_TEMPLE_MAP },
    { "Carte du Temple de l'Esprit", RG_SPIRIT_TEMPLE_MAP },
    { "Shadow Temple Map", RG_SHADOW_TEMPLE_MAP },
    { "Carte du Temple de l'Ombre", RG_SHADOW_TEMPLE_MAP },
    { "Bottom of the Well Map", RG_BOTTOM_OF_THE_WELL_MAP },
    { "Carte du Puits", RG_BOTTOM_OF_THE_WELL_MAP },
    { "Ice Cavern Map", RG_ICE_CAVERN_MAP },
    { "Carte de la Caverne Polaire", RG_ICE_CAVERN_MAP },
    { "Great Deku Tree Compass", RG_DEKU_TREE_COMPASS },
    { "Boussole de l'Arbre Mojo", RG_DEKU_TREE_COMPASS },
    { "Dodongo's Cavern Compass", RG_DODONGOS_CAVERN_COMPASS },
    { "Boussole de la Caverne Dodongo", RG_DODONGOS_CAVERN_COMPASS },
    { "Jabu-Jabu's Belly Compass", RG_JABU_JABUS_BELLY_COMPASS },
    { "Boussole du Ventre de Jabu-Jabu", RG_JABU_JABUS_BELLY_COMPASS },
    { "Forest Temple Compass", RG_FOREST_TEMPLE_COMPASS },
    { "Boussole du Temple de la Forêt", RG_FOREST_TEMPLE_COMPASS },
    { "Fire Temple Compass", RG_FIRE_TEMPLE_COMPASS },
    { "Boussole du Temple du Feu", RG_FIRE_TEMPLE_COMPASS },
    { "Water Temple Compass", RG_WATER_TEMPLE_COMPASS },
    { "Boussole du Temple de l'Eau", RG_WATER_TEMPLE_COMPASS },
    { "Spirit Temple Compass", RG_SPIRIT_TEMPLE_COMPASS },
    { "Boussole du Temple des Esprits", RG_SPIRIT_TEMPLE_COMPASS },
    { "Shadow Temple Compass", RG_SHADOW_TEMPLE_COMPASS },
    { "Boussole du Temple de l'Ombre", RG_SHADOW_TEMPLE_COMPASS },
    { "Bottom of the Well Compass", RG_BOTTOM_OF_THE_WELL_COMPASS },
    { "Boussole du Puits", RG_BOTTOM_OF_THE_WELL_COMPASS },
    { "Ice Cavern Compass", RG_ICE_CAVERN_COMPASS },
    { "Boussole de la Caverne Polaire", RG_ICE_CAVERN_COMPASS },
    { "Forest Temple Big Key", RG_FOREST_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple de la Forêt", RG_FOREST_TEMPLE_BOSS_KEY },
    { "Fire Temple Big Key", RG_FIRE_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple du Feu", RG_FIRE_TEMPLE_BOSS_KEY },
    { "Water Temple Big Key", RG_WATER_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple de l'Eau", RG_WATER_TEMPLE_BOSS_KEY },
    { "Spirit Temple Big Key", RG_SPIRIT_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple de l'Esprit", RG_SPIRIT_TEMPLE_BOSS_KEY },
    { "Shadow Temple Big Key", RG_SHADOW_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple de l'Ombre", RG_SHADOW_TEMPLE_BOSS_KEY },
    { "Ganon's Castle Big Key", RG_GANONS_CASTLE_BOSS_KEY },
    { "Clé d'Or du Château de Ganon", RG_GANONS_CASTLE_BOSS_KEY },
    { "Forest Temple Small Key", RG_FOREST_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple de la Forêt", RG_FOREST_TEMPLE_SMALL_KEY },
    { "Fire Temple Small Key", RG_FIRE_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple du Feu", RG_FIRE_TEMPLE_SMALL_KEY },
    { "Water Temple Small Key", RG_WATER_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple de l'Eau", RG_WATER_TEMPLE_SMALL_KEY },
    { "Spirit Temple Small Key", RG_SPIRIT_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple de l'Esprit", RG_SPIRIT_TEMPLE_SMALL_KEY },
    { "Shadow Temple Small Key", RG_SHADOW_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple de l'Ombre", RG_SHADOW_TEMPLE_SMALL_KEY },
    { "Bottom of the Well Small Key", RG_BOTTOM_OF_THE_WELL_SMALL_KEY },
    { "Petite Clé du Puits", RG_BOTTOM_OF_THE_WELL_SMALL_KEY },
    { "Training Grounds Small Key", RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY },
    { "Petite Clé du Gymnase Gerudo", RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY },
    { "Gerudo Fortress Small Key", RG_GERUDO_FORTRESS_SMALL_KEY },
    { "Petite Clé du Repaire des Voleurs", RG_GERUDO_FORTRESS_SMALL_KEY },
    { "Ganon's Castle Small Key", RG_GANONS_CASTLE_SMALL_KEY },
    { "Petite Clé du Château de Ganon", RG_GANONS_CASTLE_SMALL_KEY },
    { "Chest Game Small Key", RG_TREASURE_GAME_SMALL_KEY },
    { "Petite Clé du jeu la Chasse-aux-Trésors", RG_TREASURE_GAME_SMALL_KEY },
    { "Forest Temple Key Ring", RG_FOREST_TEMPLE_KEY_RING },
    { "Trousseau du Temple de la Forêt", RG_FOREST_TEMPLE_KEY_RING },
    { "Fire Temple Key Ring", RG_FIRE_TEMPLE_KEY_RING },
    { "Trousseau du Temple du Feu", RG_FIRE_TEMPLE_KEY_RING },
    { "Water Temple Key Ring", RG_WATER_TEMPLE_KEY_RING },
    { "Trousseau du Temple de l'Eau", RG_WATER_TEMPLE_KEY_RING },
    { "Spirit Temple Key Ring", RG_SPIRIT_TEMPLE_KEY_RING },
    { "Trousseau du Temple de l'Esprit", RG_SPIRIT_TEMPLE_KEY_RING },
    { "Shadow Temple Key Ring", RG_SHADOW_TEMPLE_KEY_RING },
    { "Trousseau du Temple de l'Ombre", RG_SHADOW_TEMPLE_KEY_RING },
    { "Bottom of the Well Key Ring", RG_BOTTOM_OF_THE_WELL_KEY_RING },
    { "Trousseau du Puits", RG_BOTTOM_OF_THE_WELL_KEY_RING },
    { "Training Grounds Key Ring", RG_GERUDO_TRAINING_GROUNDS_KEY_RING },
    { "Trousseau du Gymnase Gerudo", RG_GERUDO_TRAINING_GROUNDS_KEY_RING },
    { "Gerudo Fortress Key Ring", RG_GERUDO_FORTRESS_KEY_RING },
    { "Trousseau du Repaire des Voleurs", RG_GERUDO_FORTRESS_KEY_RING },
    { "Ganon's Castle Key Ring", RG_GANONS_CASTLE_KEY_RING },
    { "Trousseau du Château de Ganon", RG_GANONS_CASTLE_KEY_RING },
    { "Kokiri's Emerald", RG_KOKIRI_EMERALD },
    { "Émeraude Kokiri", RG_KOKIRI_EMERALD },
    { "Goron's Ruby", RG_GORON_RUBY },
    { "Rubis Goron", RG_GORON_RUBY },
    { "Zora's Sapphire", RG_ZORA_SAPPHIRE },
    { "Saphir Zora", RG_ZORA_SAPPHIRE },
    { "Forest Medallion", RG_FOREST_MEDALLION },
    { "Médaillon de la Forêt", RG_FOREST_MEDALLION },
    { "Fire Medallion", RG_FIRE_MEDALLION },
    { "Médaillon du Feu", RG_FIRE_MEDALLION },
    { "Water Medallion", RG_WATER_MEDALLION },
    { "Médaillon de l'Eau", RG_WATER_MEDALLION },
    { "Spirit Medallion", RG_SPIRIT_MEDALLION },
    { "Médaillon de l'Esprit", RG_SPIRIT_MEDALLION },
    { "Shadow Medallion", RG_SHADOW_MEDALLION },
    { "Médaillon de l'Ombre", RG_SHADOW_MEDALLION },
    { "Light Medallion", RG_LIGHT_MEDALLION },
    { "Médaillon de la Lumière", RG_LIGHT_MEDALLION },
    { "Recovery Heart", RG_RECOVERY_HEART },
    { "Coeur de Vie", RG_RECOVERY_HEART },
    { "Green Rupee", RG_GREEN_RUPEE },
    { "Rubis Vert", RG_GREEN_RUPEE },
    { "Blue Rupee", RG_BLUE_RUPEE },
    { "Rubis Bleu", RG_BLUE_RUPEE },
    { "Red Rupee", RG_RED_RUPEE },
    { "Rubis Rouge", RG_RED_RUPEE },
    { "Purple Rupee", RG_PURPLE_RUPEE },
    { "Rubis Pourpre", RG_PURPLE_RUPEE },
    { "Huge Rupee", RG_HUGE_RUPEE },
    { "Énorme Rubis", RG_HUGE_RUPEE },
    { "Piece of Heart", RG_PIECE_OF_HEART },
    { "Quart de Coeur", RG_PIECE_OF_HEART },
    { "Heart Container", RG_HEART_CONTAINER },
    { "Réceptacle de Coeur", RG_HEART_CONTAINER },
    { "Ice Trap", RG_ICE_TRAP },
    { "Piège de Glace", RG_ICE_TRAP },
    { "Milk", RG_MILK },
    { "Lait", RG_MILK },
    { "Bombs (5)", RG_BOMBS_5 },
    { "Bombes (5)", RG_BOMBS_5 },
    { "Bombs (10)", RG_BOMBS_10 },
    { "Bombes (10)", RG_BOMBS_10 },
    { "Bombs (20)", RG_BOMBS_20 },
    { "Bombes (20)", RG_BOMBS_20 },
    { "Bombchu (5)", RG_BOMBCHU_5 },
    { "Missiles (5)", RG_BOMBCHU_5 },
    { "Bombchu (10)", RG_BOMBCHU_10 },
    { "Missiles (10)", RG_BOMBCHU_10 },
    { "Bombchu (20)", RG_BOMBCHU_20 },
    { "Missiles (20)", RG_BOMBCHU_20 },
    { "Bombchu Drop", RG_BOMBCHU_DROP },
    { "Drop Missile", RG_BOMBCHU_DROP },
    { "Arrows (5)", RG_ARROWS_5 },
    { "Flèches (5)", RG_ARROWS_5 },
    { "Arrows (10)", RG_ARROWS_10 },
    { "Flèches (10)", RG_ARROWS_10 },
    { "Arrows (30)", RG_ARROWS_30 },
    { "Flèches (30)", RG_ARROWS_30 },
    { "Deku Nuts (5)", RG_DEKU_NUTS_5 },
    { "Noix Mojo (5)", RG_DEKU_NUTS_5 },
    { "Deku Nuts (10)", RG_DEKU_NUTS_10 },
    { "Noix Mojo (10)", RG_DEKU_NUTS_10 },
    { "Deku Seeds (30)", RG_DEKU_SEEDS_30 },
    { "Graines Mojo (30)", RG_DEKU_SEEDS_30 },
    { "Deku Stick (1)", RG_DEKU_STICK_1 },
    { "Bâton Mojo (1)", RG_DEKU_STICK_1 },
    { "Red Potion Refill", RG_RED_POTION_REFILL },
    { "Recharge de Potion Rouge", RG_RED_POTION_REFILL },
    { "Green Potion Refill", RG_GREEN_POTION_REFILL },
    { "Recharge de Potion Verte", RG_GREEN_POTION_REFILL },
    { "Blue Potion Refill", RG_BLUE_POTION_REFILL },
    { "Recharge de Potion Bleue", RG_BLUE_POTION_REFILL },
    { "Piece of Heart (Treasure Chest Minigame)", RG_TREASURE_GAME_HEART },
    { "Quart de Coeur (Chasse-aux-Trésors)", RG_TREASURE_GAME_HEART },
    { "Green Rupee (Treasure Chest Minigame)", RG_TREASURE_GAME_GREEN_RUPEE },
    { "Rubis Vert (Chasse-aux-Trésors)", RG_TREASURE_GAME_GREEN_RUPEE },
    { "Buy Deku Nut (5)", RG_BUY_DEKU_NUT_5 },
    { "Acheter: Noix Mojo (5)", RG_BUY_DEKU_NUT_5 },
    { "Buy Arrows (30)", RG_BUY_ARROWS_30 },
    { "Acheter: Flèches (30)", RG_BUY_ARROWS_30 },
    { "Buy Arrows (50)", RG_BUY_ARROWS_50 },
    { "Acheter: Flèches (50)", RG_BUY_ARROWS_50 },
    { "Buy Bombs (5) [25]", RG_BUY_BOMBS_525 },
    { "Acheter: Bombes (5) [25]", RG_BUY_BOMBS_525 },
    { "Buy Deku Nut (10)", RG_BUY_DEKU_NUT_10 },
    { "Acheter: Noix Mojo (10)", RG_BUY_DEKU_NUT_10 },
    { "Buy Deku Stick (1)", RG_BUY_DEKU_STICK_1 },
    { "Acheter: Bâton Mojo (1)", RG_BUY_DEKU_STICK_1 },
    { "Buy Bombs (10)", RG_BUY_BOMBS_10 },
    { "Acheter: Bombes ", RG_BUY_BOMBS_10 },
    { "Buy Fish", RG_BUY_FISH },
    { "Acheter: Poisson", RG_BUY_FISH },
    { "Buy Red Potion [30]", RG_BUY_RED_POTION_30 },
    { "Acheter: Potion Rouge [30]", RG_BUY_RED_POTION_30 },
    { "Buy Green Potion", RG_BUY_GREEN_POTION },
    { "Acheter: Potion Verte", RG_BUY_GREEN_POTION },
    { "Buy Blue Potion", RG_BUY_BLUE_POTION },
    { "Acheter: Potion Bleue", RG_BUY_BLUE_POTION },
    { "Buy Hylian Shield", RG_BUY_HYLIAN_SHIELD },
    { "Acheter: Bouclier Hylien", RG_BUY_HYLIAN_SHIELD },
    { "Buy Deku Shield", RG_BUY_DEKU_SHIELD },
    { "Acheter: Bouclier Mojo", RG_BUY_DEKU_SHIELD },
    { "Buy Goron Tunic", RG_BUY_GORON_TUNIC },
    { "Acheter: Tunique Goron", RG_BUY_GORON_TUNIC },
    { "Buy Zora Tunic", RG_BUY_ZORA_TUNIC },
    { "Acheter: Tunique Zora", RG_BUY_ZORA_TUNIC },
    { "Buy Heart", RG_BUY_HEART },
    { "Acheter: Coeur de Vie", RG_BUY_HEART },
    { "Buy Bombchu (10)", RG_BUY_BOMBCHU_10 },
    { "Acheter: Missiles (10)", RG_BUY_BOMBCHU_10 },
    { "Buy Bombchu (20)", RG_BUY_BOMBCHU_20 },
    { "Acheter: Missiles (20)", RG_BUY_BOMBCHU_20 },
    { "Buy Bombchu (5)", RG_BUY_BOMBCHU_5 },
    { "Acheter: Missiles (5)", RG_BUY_BOMBCHU_5 },
    { "Buy Deku Seeds (30)", RG_BUY_DEKU_SEEDS_30 },
    { "Acheter: Graines Mojo (30)", RG_BUY_DEKU_SEEDS_30 },
    { "Sold Out", RG_SOLD_OUT },
    { "Rupture de stock", RG_SOLD_OUT },
    { "Buy Blue Fire", RG_BUY_BLUE_FIRE },
    { "Acheter: Flamme Bleue", RG_BUY_BLUE_FIRE },
    { "Buy Bottle Bug", RG_BUY_BOTTLE_BUG },
    { "Acheter: Insecte en bouteille", RG_BUY_BOTTLE_BUG },
    { "Buy Poe", RG_BUY_POE },
    { "Acheter: Esprit", RG_BUY_POE },
    { "Buy Fairy's Spirit", RG_BUY_FAIRYS_SPIRIT },
    { "Acheter: Esprit de Fée", RG_BUY_FAIRYS_SPIRIT },
    { "Buy Arrows (10)", RG_BUY_ARROWS_10 },
    { "Acheter: Flèches (10)", RG_BUY_ARROWS_10 },
    { "Buy Bombs (20)", RG_BUY_BOMBS_20 },
    { "Acheter: Bombes (20)", RG_BUY_BOMBS_20 },
    { "Buy Bombs (30)", RG_BUY_BOMBS_30 },
    { "Acheter: Bombes (30)", RG_BUY_BOMBS_30 },
    { "Buy Bombs (5) [35]", RG_BUY_BOMBS_535 },
    { "Acheter: Bombes (5) [35]", RG_BUY_BOMBS_535 },
    { "Buy Red Potion [40]", RG_BUY_RED_POTION_40 },
    { "Acheter: Potion Rouge [40]", RG_BUY_RED_POTION_40 },
    { "Buy Red Potion [50]", RG_BUY_RED_POTION_50 },
    { "Acheter: Potion Rouge [50]", RG_BUY_RED_POTION_50 },
    { "Triforce", RG_TRIFORCE },
    { "Hint", RG_HINT },
    { "Indice", RG_HINT }
};

std::unordered_map<std::string, RandomizerSettingKey> SpoilerfileSettingNameToEnum = {
    { "Open Settings:Forest", RSK_FOREST },
    { "Open Settings:Kakariko Gate", RSK_KAK_GATE },
    { "Open Settings:Door of Time", RSK_DOOR_OF_TIME },
    { "Open Settings:Zora's Fountain", RSK_ZORAS_FOUNTAIN },
    { "Open Settings:Gerudo Fortress", RSK_GERUDO_FORTRESS },
    { "Open Settings:Rainbow Bridge", RSK_RAINBOW_BRIDGE },
    { "Open Settings:Stone Count", RSK_RAINBOW_BRIDGE_STONE_COUNT },
    { "Open Settings:Medallion Count", RSK_RAINBOW_BRIDGE_MEDALLION_COUNT },
    { "Open Settings:Reward Count", RSK_RAINBOW_BRIDGE_REWARD_COUNT },
    { "Open Settings:Dungeon Count", RSK_RAINBOW_BRIDGE_DUNGEON_COUNT },
    { "Open Settings:Token Count", RSK_RAINBOW_BRIDGE_TOKEN_COUNT },
    { "Open Settings:Random Ganon's Trials", RSK_RANDOM_TRIALS },
    { "Open Settings:Trial Count", RSK_TRIAL_COUNT },
    { "Shuffle Settings:Shuffle Gerudo Card", RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD },
    { "Shuffle Settings:Scrub Shuffle", RSK_SHUFFLE_SCRUBS },
    { "Shuffle Settings:Shuffle Cows", RSK_SHUFFLE_COWS },
    { "Shuffle Settings:Tokensanity", RSK_SHUFFLE_TOKENS },
    { "Shuffle Settings:Shuffle Adult Trade", RSK_SHUFFLE_ADULT_TRADE },
    { "Shuffle Settings:Shuffle Magic Beans", RSK_SHUFFLE_MAGIC_BEANS},
    { "Start with Deku Shield", RSK_STARTING_DEKU_SHIELD },
    { "Start with Kokiri Sword", RSK_STARTING_KOKIRI_SWORD },
    { "Start with Fairy Ocarina", RSK_STARTING_OCARINA },
    { "Shuffle Dungeon Items:Maps/Compasses", RSK_STARTING_MAPS_COMPASSES },
    { "Shuffle Dungeon Items:Small Keys", RSK_KEYSANITY },
    { "Shuffle Dungeon Items:Gerudo Fortress Keys", RSK_GERUDO_KEYS },
    { "Shuffle Dungeon Items:Boss Keys", RSK_BOSS_KEYSANITY },
    { "Shuffle Dungeon Items:Ganon's Boss Key", RSK_GANONS_BOSS_KEY },
    { "Misc Settings:Gossip Stone Hints", RSK_GOSSIP_STONE_HINTS },
    { "Misc Settings:Hint Clarity", RSK_HINT_CLARITY },
    { "Misc Settings:Hint Distribution", RSK_HINT_DISTRIBUTION },
    { "Skip Child Zelda", RSK_SKIP_CHILD_ZELDA },
    { "Start with Consumables", RSK_STARTING_CONSUMABLES },
    { "Start with Max Rupees", RSK_FULL_WALLETS },
    { "Timesaver Settings:Cuccos to return", RSK_CUCCO_COUNT },
    { "Timesaver Settings:Big Poe Target Count", RSK_BIG_POE_COUNT },
    { "Timesaver Settings:Skip Child Stealth", RSK_SKIP_CHILD_STEALTH },
    { "Timesaver Settings:Skip Epona Race", RSK_SKIP_EPONA_RACE },
    { "Timesaver Settings:Skip Tower Escape", RSK_SKIP_TOWER_ESCAPE },
    { "Timesaver Settings:Complete Mask Quest", RSK_COMPLETE_MASK_QUEST },
    { "Timesaver Settings:Enable Glitch-Useful Cutscenes", RSK_ENABLE_GLITCH_CUTSCENES },
};

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

#pragma optimize("", off)
#pragma GCC push_options
#pragma GCC optimize ("O0")
bool Randomizer::SpoilerFileExists(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        std::ifstream spoilerFileStream(sanitize(spoilerFileName));
        if (!spoilerFileStream) {
            return false;
        } else {
            return true;
        }
    }

    return false;
}
#pragma GCC pop_options
#pragma optimize("", on)

void Randomizer::LoadRandomizerSettings(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseRandomizerSettingsFile(spoilerFileName);
    }

    for(auto randoSetting : gSaveContext.randoSettings) {
        if(randoSetting.key == RSK_NONE) break;
        this->randoSettings[randoSetting.key] = randoSetting.value;
    }
}

void Randomizer::LoadHintLocations(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseHintLocationsFile(spoilerFileName);
    }

    CustomMessageManager::Instance->ClearMessageTable(Randomizer::hintMessageTableID);
    CustomMessageManager::Instance->AddCustomMessageTable(Randomizer::hintMessageTableID);

    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_ALTAR_CHILD,
                                           { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, gSaveContext.childAltarText,
                                             gSaveContext.childAltarText, gSaveContext.childAltarText });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_ALTAR_ADULT,
                                           { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, gSaveContext.adultAltarText,
                                             gSaveContext.adultAltarText, gSaveContext.adultAltarText });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_GANONDORF,
                                           { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, gSaveContext.ganonHintText,
                                             gSaveContext.ganonHintText, gSaveContext.ganonHintText });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_GANONDORF_NOHINT,
                                           { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, gSaveContext.ganonText,
                                             gSaveContext.ganonText, gSaveContext.ganonText });

    this->childAltarText = gSaveContext.childAltarText;
    this->adultAltarText = gSaveContext.adultAltarText;
    this->ganonHintText = gSaveContext.ganonHintText;
    this->ganonText = gSaveContext.ganonText;

    for (auto hintLocation : gSaveContext.hintLocations) {
        if(hintLocation.check == RC_LINKS_POCKET) break;
        this->hintLocations[hintLocation.check] = hintLocation.hintText;
        CustomMessageManager::Instance->CreateMessage(
            Randomizer::hintMessageTableID, hintLocation.check, { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, hintLocation.hintText, hintLocation.hintText, hintLocation.hintText });
    }
}

void Randomizer::LoadItemLocations(const char* spoilerFileName, bool silent) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseItemLocationsFile(spoilerFileName, silent);
    }

    for (auto itemLocation : gSaveContext.itemLocations) {
        this->itemLocations[itemLocation.check] = itemLocation.get;
    }

    itemLocations[RC_UNKNOWN_CHECK] = RG_NONE;
}

void Randomizer::LoadRequiredTrials(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseRequiredTrialsFile(spoilerFileName);
    }
}

void Randomizer::ParseRandomizerSettingsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        // clear out existing settings
        // RANDOTODO don't use magic number for settings array size
        for(size_t i = 0; i < 300; i++) {
            gSaveContext.randoSettings[i].key = RSK_NONE;
            gSaveContext.randoSettings[i].value = 0;
        }

        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json settingsJson = spoilerFileJson["settings"];

        int index = 0;

        for (auto it = settingsJson.begin(); it != settingsJson.end(); ++it) {
            // todo load into cvars for UI
            
            std::string numericValueString;
            if(SpoilerfileSettingNameToEnum.count(it.key())) {
                gSaveContext.randoSettings[index].key = SpoilerfileSettingNameToEnum[it.key()];
                // this is annoying but the same strings are used in different orders
                // and i don't want the spoilerfile to just have numbers instead of
                // human readable settings values so it'll have to do for now
                switch(gSaveContext.randoSettings[index].key) {
                    case RSK_FOREST:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Closed Deku") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_KAK_GATE:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_DOOR_OF_TIME:
                        if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Song only") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_ZORAS_FOUNTAIN:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Closed as child") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_GERUDO_FORTRESS:
                        if(it.value() == "Normal") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Fast") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_RAINBOW_BRIDGE:
                        if(it.value() == "Always open") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Stones") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Medallions") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Dungeon rewards") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Dungeons") {
                            gSaveContext.randoSettings[index].value = 5;
                        } else if(it.value() == "Tokens") {
                            gSaveContext.randoSettings[index].value = 6;
                        }
                        break;
                    case RSK_RAINBOW_BRIDGE_STONE_COUNT:
                    case RSK_RAINBOW_BRIDGE_MEDALLION_COUNT:
                    case RSK_RAINBOW_BRIDGE_REWARD_COUNT:
                    case RSK_RAINBOW_BRIDGE_DUNGEON_COUNT:
                    case RSK_RAINBOW_BRIDGE_TOKEN_COUNT:
                    case RSK_TRIAL_COUNT:
                    case RSK_BIG_POE_COUNT:
                    case RSK_CUCCO_COUNT:
                        numericValueString = it.value();
                        gSaveContext.randoSettings[index].value = std::stoi(numericValueString);
                        break;
                    case RSK_SHUFFLE_SCRUBS:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Affordable") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Expensive") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Random Prices") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD:
                    case RSK_SHUFFLE_COWS:
                    case RSK_SHUFFLE_ADULT_TRADE:
                    case RSK_SHUFFLE_MAGIC_BEANS:
                    case RSK_RANDOM_TRIALS:
                    case RSK_STARTING_DEKU_SHIELD:
                    case RSK_STARTING_KOKIRI_SWORD:
                    case RSK_COMPLETE_MASK_QUEST:
                    case RSK_ENABLE_GLITCH_CUTSCENES:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "On") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_STARTING_MAPS_COMPASSES:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_STARTING_OCARINA:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Fairy Ocarina") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_GOSSIP_STONE_HINTS:
                        if(it.value() == "No Hints") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Need Nothing") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Mask of Truth") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Stone of Agony") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_HINT_CLARITY:
                        if(it.value() == "Obscure") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Ambiguous") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Clear") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_HINT_DISTRIBUTION:
                        if(it.value() == "Useless") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Balanced") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Strong") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Very Strong") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_KEYSANITY:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_BOSS_KEYSANITY:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_GANONS_BOSS_KEY:
                        if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if(it.value() == "Own dungeon") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Start with") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_SKIP_CHILD_ZELDA:
                        gSaveContext.randoSettings[index].value = it.value();
                        break;
                    case RSK_STARTING_CONSUMABLES:
                    case RSK_FULL_WALLETS:
                        if(it.value() == "No") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Yes") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_SKIP_CHILD_STEALTH:
                    case RSK_SKIP_EPONA_RACE:
                    case RSK_SKIP_TOWER_ESCAPE:
                        if(it.value() == "Don't Skip") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if (it.value() == "Skip") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_SHUFFLE_TOKENS:
                        if (it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if (it.value() == "Dungeons") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if (it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if (it.value() == "All Tokens") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                }
                index++;
            }
        }

        success = true;
    } catch (const std::exception& e) {
        return;
    }
}

std::string AltarIconString(char iconChar) {
    std::string iconString = "";
    switch (iconChar) {
        case '0':
            // Kokiri Emerald
            iconString += 0x13;
            iconString += 0x6C;
            break;
        case '1':
            // Goron Ruby
            iconString += 0x13;
            iconString += 0x6D;
            break;
        case '2':
            // Zora Sapphire
            iconString += 0x13;
            iconString += 0x6E;
            break;
        case '3':
            // Forest Medallion
            iconString += 0x13;
            iconString += 0x66;
            break;
        case '4':
            // Fire Medallion
            iconString += 0x13;
            iconString += 0x67;
            break;
        case '5':
            // Water Medallion
            iconString += 0x13;
            iconString += 0x68;
            break;
        case '6':
            // Spirit Medallion
            iconString += 0x13;
            iconString += 0x69;
            break;
        case '7':
            // Shadow Medallion
            iconString += 0x13;
            iconString += 0x6A;
            break;
        case '8':
            // Light Medallion
            iconString += 0x13;
            iconString += 0x6B;
            break;
        case 'o':
            // Open DOT (master sword)
            iconString += 0x13;
            iconString += 0x3C;
            break;
        case 'c':
            // Closed DOT (fairy ocarina)
            iconString += 0x13;
            iconString += 0x07;
            break;
        case 'i':
            // Intended DOT (oot)
            iconString += 0x13;
            iconString += 0x08;
            break;
        case 'l':
            // Light Arrow (for bridge reqs)
            iconString += 0x13;
            iconString += 0x12;
            break;
        case 'b':
            // Boss Key (ganon boss key location)
            iconString += 0x13;
            iconString += 0x74;
            break;
        case 'L':
            // Bow with Light Arrow
            iconString += 0x13;
            iconString += 0x3A;
            break;
        case 'k':
            // Kokiri Tunic
            iconString += 0x13;
            iconString += 0x41;
            break;
    }
    return iconString;
}

std::string FormatJsonHintText(std::string jsonHint) {
    std::string formattedHintMessage = jsonHint;

    // add icons to altar text
    for (char iconChar : {'0', '1', '2', '3', '4', '5', '6', '7', '8', 'o', 'c', 'i', 'l', 'b', 'L', 'k'}) {
        std::string textToReplace = "$";
        textToReplace += iconChar;
        size_t start_pos = formattedHintMessage.find(textToReplace);
        if(!(start_pos == std::string::npos)) {
            std::string iconString = AltarIconString(iconChar);
            formattedHintMessage.replace(start_pos, textToReplace.length(), iconString);
        }
    }

    formattedHintMessage += 0x02;

    return formattedHintMessage;
}

void Randomizer::ParseHintLocationsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;

        std::string childAltarJsonText = spoilerFileJson["childAltarText"].get<std::string>();
        std::string formattedChildAltarText = FormatJsonHintText(childAltarJsonText);
        memcpy(gSaveContext.childAltarText, formattedChildAltarText.c_str(), formattedChildAltarText.length());

        std::string adultAltarJsonText = spoilerFileJson["adultAltarText"].get<std::string>();
        std::string formattedAdultAltarText = FormatJsonHintText(adultAltarJsonText);
        memcpy(gSaveContext.adultAltarText, formattedAdultAltarText.c_str(), formattedAdultAltarText.length());

        std::string ganonHintJsonText = spoilerFileJson["ganonHintText"].get<std::string>();
        std::string formattedGanonHintJsonText = FormatJsonHintText(ganonHintJsonText);
        memcpy(gSaveContext.ganonHintText, formattedGanonHintJsonText.c_str(), formattedGanonHintJsonText.length());

        std::string ganonJsonText = spoilerFileJson["ganonText"].get<std::string>();
        std::string formattedGanonJsonText = FormatJsonHintText(ganonJsonText);
        memcpy(gSaveContext.ganonText, formattedGanonJsonText.c_str(), formattedGanonJsonText.length());

        json hintsJson = spoilerFileJson["hints"];
        int index = 0;
        for (auto it = hintsJson.begin(); it != hintsJson.end(); ++it) {
            gSaveContext.hintLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];

            std::string hintMessage = FormatJsonHintText(it.value());
            memcpy(gSaveContext.hintLocations[index].hintText, hintMessage.c_str(), hintMessage.length());

            index++;
        }

        success = true;
    } catch (const std::exception& e) {
        return;
    }
}

void Randomizer::ParseRequiredTrialsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream) {
        return;
    }

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json trialsJson = spoilerFileJson["requiredTrials"];

        for (auto it = trialsJson.begin(); it != trialsJson.end(); it++) {
            this->trialsRequired[spoilerFileTrialToEnum[it.value()]] = true;
        }
    } catch (const std::exception& e) {
        return;
    }
}

void Randomizer::ParseItemLocationsFile(const char* spoilerFileName, bool silent) {
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
            gSaveContext.seedIcons[index] = gSeedTextures[it.value()].id;
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
                    } else if (itemit.key() == "price") {
                        randomizerMerchantPrices[gSaveContext.itemLocations[index].check] = itemit.value();
                    }
                }
            } else {
                gSaveContext.itemLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];
                gSaveContext.itemLocations[index].get = SpoilerfileGetNameToEnum[it.value()];
            }

            index++;
        }

        if(!silent) {
            Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
        success = true;
    } catch (const std::exception& e) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        return;
    }
}

bool Randomizer::IsTrialRequired(RandomizerInf trial) {
    return this->trialsRequired.contains(trial);
}

s16 Randomizer::GetRandomizedItemIdFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogId) {
    RandomizerCheck randomizerCheck = GetCheckFromActor(actorId, sceneNum, actorParams);

    return GetRandomizedItemIdFromKnownCheck(randomizerCheck, ogId);
}

s16 Randomizer::GetItemFromGet(RandomizerGet randoGet, GetItemID ogItemId) {
    switch (randoGet) {
        case RG_NONE:
            return ogItemId;

        case RG_KOKIRI_SWORD:
            return !CHECK_OWNED_EQUIP(EQUIP_SWORD, 0) ? GI_SWORD_KOKIRI : GI_RUPEE_BLUE;
        case RG_GIANTS_KNIFE:
            return GI_SWORD_KNIFE;
        case RG_BIGGORON_SWORD:
            return !gSaveContext.bgsFlag ? GI_SWORD_BGS : GI_RUPEE_BLUE;

        case RG_DEKU_SHIELD:
            return GI_SHIELD_DEKU;
        case RG_HYLIAN_SHIELD:
            return GI_SHIELD_HYLIAN;
        case RG_MIRROR_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_SHIELD, 2) ? GI_SHIELD_MIRROR : GI_RUPEE_BLUE;

        case RG_GORON_TUNIC:
            return GI_TUNIC_GORON;
        case RG_ZORA_TUNIC:
            return GI_TUNIC_ZORA;

        case RG_IRON_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_BOOTS, 1) ? GI_BOOTS_IRON : GI_RUPEE_BLUE;
        case RG_HOVER_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_BOOTS, 2) ? GI_BOOTS_HOVER : GI_RUPEE_BLUE;

        case RG_BOOMERANG:
            return INV_CONTENT(ITEM_BOOMERANG) == ITEM_NONE ? GI_BOOMERANG : GI_RUPEE_BLUE;

        case RG_LENS_OF_TRUTH:
            return INV_CONTENT(ITEM_LENS) == ITEM_NONE ? GI_LENS : GI_RUPEE_BLUE;

        case RG_MEGATON_HAMMER:
            return INV_CONTENT(ITEM_HAMMER) == ITEM_NONE ? GI_HAMMER : GI_RUPEE_BLUE;

        case RG_STONE_OF_AGONY:
            return GI_STONE_OF_AGONY;

        case RG_DINS_FIRE:
            return INV_CONTENT(ITEM_DINS_FIRE) == ITEM_NONE ? GI_DINS_FIRE : GI_RUPEE_BLUE;
        case RG_FARORES_WIND:
            return INV_CONTENT(ITEM_FARORES_WIND) == ITEM_NONE ? GI_FARORES_WIND : GI_RUPEE_BLUE;
        case RG_NAYRUS_LOVE:
            return INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NONE ? GI_NAYRUS_LOVE : GI_RUPEE_BLUE;

        case RG_FIRE_ARROWS:
            return INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_NONE ? GI_ARROW_FIRE : GI_RUPEE_BLUE;
        case RG_ICE_ARROWS:
            return INV_CONTENT(ITEM_ARROW_ICE) == ITEM_NONE ? GI_ARROW_ICE : GI_RUPEE_BLUE;
        case RG_LIGHT_ARROWS:
            return INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_NONE ? GI_ARROW_LIGHT : GI_RUPEE_BLUE;
        
        case RG_DOUBLE_DEFENSE:
            return !gSaveContext.doubleDefense ? (GetItemID)RG_DOUBLE_DEFENSE : GI_RUPEE_BLUE;

        case RG_GERUDO_MEMBERSHIP_CARD:
            return GI_GERUDO_CARD;

        case RG_MAGIC_BEAN:
            return GI_BEAN;

        case RG_WEIRD_EGG:
            return GI_WEIRD_EGG;

        case RG_ZELDAS_LETTER:
            return GI_LETTER_ZELDA;
        case RG_RUTOS_LETTER:
            return GI_LETTER_RUTO;

        case RG_POCKET_EGG:
            return GI_POCKET_EGG;
        case RG_COJIRO:
            return GI_COJIRO;
        case RG_ODD_MUSHROOM:
            return GI_ODD_MUSHROOM;
        case RG_ODD_POTION:
            return GI_ODD_POTION;
        case RG_POACHERS_SAW:
            return GI_SAW;
        case RG_BROKEN_SWORD:
            return GI_SWORD_BROKEN;
        case RG_PRESCRIPTION:
            return GI_PRESCRIPTION;
        case RG_EYEBALL_FROG:
            return GI_FROG;
        case RG_EYEDROPS:
            return GI_EYEDROPS;
        case RG_CLAIM_CHECK:
            return GI_CLAIM_CHECK;
        
        case RG_GOLD_SKULLTULA_TOKEN:
            return GI_SKULL_TOKEN;

        case RG_PROGRESSIVE_HOOKSHOT:
            switch (gSaveContext.inventory.items[SLOT_HOOKSHOT]) {
                case ITEM_NONE:
                    return GI_HOOKSHOT;
                case ITEM_HOOKSHOT:
                    return GI_LONGSHOT;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_STRENGTH:
            switch (CUR_UPG_VALUE(UPG_STRENGTH)) {
                case 0:
                    return GI_BRACELET;
                case 1:
                    return GI_GAUNTLETS_SILVER;
                case 2:
                    return GI_GAUNTLETS_GOLD;
            }
            return GI_RUPEE_BLUE;
        
        case RG_PROGRESSIVE_BOMB_BAG:
            switch (CUR_UPG_VALUE(UPG_BOMB_BAG)) {
                case 0:
                    return GI_BOMB_BAG_20;
                case 1:
                    return GI_BOMB_BAG_30;
                case 2:
                    return GI_BOMB_BAG_40;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_BOW:
            switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                case 0:
                    return GI_BOW;
                case 1:
                    return GI_QUIVER_40;
                case 2:
                    return GI_QUIVER_50;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_SLINGSHOT:
            switch (CUR_UPG_VALUE(UPG_BULLET_BAG)) {
                case 0:
                    return GI_SLINGSHOT;
                case 1:
                    return GI_BULLET_BAG_40;
                case 2:
                    return GI_BULLET_BAG_50;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_WALLET:
            switch (CUR_UPG_VALUE(UPG_WALLET)) {
                case 0:
                    return GI_WALLET_ADULT;
                case 1:
                    return GI_WALLET_GIANT;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_SCALE:
            switch (CUR_UPG_VALUE(UPG_SCALE)) {
                case 0:
                    return GI_SCALE_SILVER;
                case 1:
                    return GI_SCALE_GOLD;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_NUT_UPGRADE:
            switch (CUR_UPG_VALUE(UPG_NUTS)) {
                case 0:
                case 1:
                    return GI_NUT_UPGRADE_30;
                case 2:
                    return GI_NUT_UPGRADE_40;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_STICK_UPGRADE:
            switch (CUR_UPG_VALUE(UPG_STICKS)) {
                case 0:
                case 1:
                    return GI_STICK_UPGRADE_20;
                case 2:
                    return GI_STICK_UPGRADE_30;
            }
            return GI_RUPEE_BLUE;
        
        case RG_PROGRESSIVE_BOMBCHUS:
            return GI_BOMBCHUS_20; //todo progressive?
        
        case RG_PROGRESSIVE_MAGIC_METER:
            switch (gSaveContext.magicLevel) {
                case 0:
                    return (GetItemID)RG_MAGIC_SINGLE;
                case 1:
                    return (GetItemID)RG_MAGIC_DOUBLE;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_OCARINA:
            switch (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_NONE) {
                case 1:
                    return GI_OCARINA_FAIRY;
                case 0:
                    return GI_OCARINA_OOT;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_GORONSWORD:
            return GI_SWORD_BGS; //todo progressive?

        case RG_EMPTY_BOTTLE:
            return GI_BOTTLE;
        case RG_BOTTLE_WITH_MILK:
            return GI_MILK_BOTTLE;
            
        // todo test this with keys in own dungeon
        case RG_TREASURE_GAME_SMALL_KEY:
            return GI_DOOR_KEY;

        // todo keyrings
        case RG_FOREST_TEMPLE_KEY_RING:
        case RG_FIRE_TEMPLE_KEY_RING:
        case RG_WATER_TEMPLE_KEY_RING:
        case RG_SPIRIT_TEMPLE_KEY_RING:
        case RG_SHADOW_TEMPLE_KEY_RING:
        case RG_BOTTOM_OF_THE_WELL_KEY_RING:
        case RG_GERUDO_TRAINING_GROUNDS_KEY_RING:
        case RG_GERUDO_FORTRESS_KEY_RING:
        case RG_GANONS_CASTLE_KEY_RING:
            return GI_RUPEE_BLUE;

        case RG_RECOVERY_HEART:
            return GI_HEART;

        case RG_GREEN_RUPEE:
            return GI_RUPEE_GREEN;
        case RG_BLUE_RUPEE:
            return GI_RUPEE_BLUE;
        case RG_RED_RUPEE:
            return GI_RUPEE_RED;
        case RG_PURPLE_RUPEE:
            return GI_RUPEE_PURPLE;
        case RG_HUGE_RUPEE:
            return GI_RUPEE_GOLD;

        case RG_PIECE_OF_HEART:
            return GI_HEART_PIECE;
        case RG_HEART_CONTAINER:
            // todo figure out what GI_HEART_CONTAINER_2 is
            return GI_HEART_CONTAINER;
        case RG_MILK:
            return GI_MILK; //todo logic around needing a bottle?

        case RG_BOMBS_5:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? GI_BOMBS_5 : GI_RUPEE_BLUE;
        case RG_BOMBS_10:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? GI_BOMBS_10 : GI_RUPEE_BLUE;
        case RG_BOMBS_20:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? GI_BOMBS_20 : GI_RUPEE_BLUE;

        case RG_BOMBCHU_5:
            return GI_BOMBCHUS_5;
        case RG_BOMBCHU_10:
            return GI_BOMBCHUS_10;
        case RG_BOMBCHU_20:
            return GI_BOMBCHUS_20;
        case RG_BOMBCHU_DROP:
            return GI_BOMBCHUS_5; //todo figure out what we want to do for chu drops

        case RG_ARROWS_5:
            return CUR_UPG_VALUE(UPG_QUIVER) ? GI_ARROWS_SMALL : GI_RUPEE_BLUE;
        case RG_ARROWS_10:
            return CUR_UPG_VALUE(UPG_QUIVER) ? GI_ARROWS_MEDIUM : GI_RUPEE_BLUE;
        case RG_ARROWS_30:
            return CUR_UPG_VALUE(UPG_QUIVER) ? GI_ARROWS_LARGE : GI_RUPEE_BLUE;

        case RG_DEKU_NUTS_5:
            return GI_NUTS_5;
        case RG_DEKU_NUTS_10:
            return GI_NUTS_10;

        case RG_DEKU_SEEDS_30:
            return CUR_UPG_VALUE(UPG_BULLET_BAG) ? GI_SEEDS_30 : GI_RUPEE_BLUE;

        case RG_DEKU_STICK_1:
            return GI_STICKS_1;

        // RANDOTODO these won't be used until we implement shopsanity/scrub shuffle
        case RG_RED_POTION_REFILL:
        case RG_GREEN_POTION_REFILL:
        case RG_BLUE_POTION_REFILL:
            return GI_NONE;

        case RG_TREASURE_GAME_HEART:
            return GI_HEART_PIECE_WIN;
        case RG_TREASURE_GAME_GREEN_RUPEE:
            return GI_RUPEE_GREEN_LOSE; //todo figure out how this works outside of the game

        case RG_TRIFORCE:
            return GI_RUPEE_BLUE; //todo figure out what this is/does

        case RG_HINT:
            return GI_RUPEE_BLUE; //todo

        default:
            if (!IsItemVanilla(randoGet)) {
                return randoGet;
            }
            return ogItemId;
        }
    }

bool Randomizer::IsItemVanilla(RandomizerGet randoGet) {
    switch (randoGet) {
        case RG_NONE:
        case RG_KOKIRI_SWORD: 
        case RG_GIANTS_KNIFE: 
        case RG_BIGGORON_SWORD:
        case RG_DEKU_SHIELD:
        case RG_HYLIAN_SHIELD:
        case RG_MIRROR_SHIELD:
        case RG_GORON_TUNIC:
        case RG_ZORA_TUNIC:
        case RG_IRON_BOOTS:
        case RG_HOVER_BOOTS:
        case RG_BOOMERANG:
        case RG_LENS_OF_TRUTH:
        case RG_MEGATON_HAMMER:
        case RG_STONE_OF_AGONY:
        case RG_DINS_FIRE:
        case RG_FARORES_WIND:
        case RG_NAYRUS_LOVE:
        case RG_FIRE_ARROWS:
        case RG_ICE_ARROWS:
        case RG_LIGHT_ARROWS:
        case RG_GERUDO_MEMBERSHIP_CARD:
        case RG_MAGIC_BEAN:
        case RG_WEIRD_EGG: 
        case RG_ZELDAS_LETTER:
        case RG_RUTOS_LETTER:
        case RG_POCKET_EGG:
        case RG_COJIRO:
        case RG_ODD_MUSHROOM:
        case RG_ODD_POTION:
        case RG_POACHERS_SAW:
        case RG_BROKEN_SWORD:
        case RG_PRESCRIPTION:
        case RG_EYEBALL_FROG:
        case RG_EYEDROPS:
        case RG_CLAIM_CHECK:
        case RG_GOLD_SKULLTULA_TOKEN:
        case RG_PROGRESSIVE_HOOKSHOT:
        case RG_PROGRESSIVE_STRENGTH:
        case RG_PROGRESSIVE_BOMB_BAG:
        case RG_PROGRESSIVE_BOW:
        case RG_PROGRESSIVE_SLINGSHOT:
        case RG_PROGRESSIVE_WALLET:
        case RG_PROGRESSIVE_SCALE:
        case RG_PROGRESSIVE_NUT_UPGRADE:
        case RG_PROGRESSIVE_STICK_UPGRADE:
        case RG_PROGRESSIVE_BOMBCHUS:
        case RG_PROGRESSIVE_OCARINA:
        case RG_PROGRESSIVE_GORONSWORD:
        case RG_EMPTY_BOTTLE:
        case RG_BOTTLE_WITH_MILK:
        case RG_RECOVERY_HEART:
        case RG_GREEN_RUPEE:
        case RG_BLUE_RUPEE:
        case RG_RED_RUPEE:
        case RG_PURPLE_RUPEE:
        case RG_HUGE_RUPEE:
        case RG_PIECE_OF_HEART:
        case RG_HEART_CONTAINER:
        case RG_MILK:
        case RG_BOMBS_5:
        case RG_BOMBS_10:
        case RG_BOMBS_20:
        case RG_BOMBCHU_5:
        case RG_BOMBCHU_10:
        case RG_BOMBCHU_20:
        case RG_BOMBCHU_DROP:
        case RG_ARROWS_5:
        case RG_ARROWS_10:
        case RG_ARROWS_30:
        case RG_DEKU_NUTS_5:
        case RG_DEKU_NUTS_10:
        case RG_DEKU_SEEDS_30:
        case RG_DEKU_STICK_1:
        case RG_RED_POTION_REFILL:
        case RG_GREEN_POTION_REFILL:
        case RG_BLUE_POTION_REFILL:
        case RG_TREASURE_GAME_HEART:
        case RG_TREASURE_GAME_GREEN_RUPEE:
        case RG_BUY_DEKU_NUT_5:
        case RG_BUY_ARROWS_30:
        case RG_BUY_ARROWS_50:
        case RG_BUY_BOMBS_525:
        case RG_BUY_DEKU_NUT_10:
        case RG_BUY_DEKU_STICK_1:
        case RG_BUY_BOMBS_10:
        case RG_BUY_FISH:
        case RG_BUY_RED_POTION_30:
        case RG_BUY_GREEN_POTION:
        case RG_BUY_BLUE_POTION:
        case RG_BUY_HYLIAN_SHIELD:
        case RG_BUY_DEKU_SHIELD:
        case RG_BUY_GORON_TUNIC:
        case RG_BUY_ZORA_TUNIC:
        case RG_BUY_HEART:
        case RG_BUY_BOMBCHU_10:
        case RG_BUY_BOMBCHU_20:
        case RG_BUY_BOMBCHU_5:
        case RG_BUY_DEKU_SEEDS_30:
        case RG_SOLD_OUT:
        case RG_BUY_BLUE_FIRE:
        case RG_BUY_BOTTLE_BUG:
        case RG_BUY_POE:
        case RG_BUY_FAIRYS_SPIRIT:
        case RG_BUY_ARROWS_10:
        case RG_BUY_BOMBS_20:
        case RG_BUY_BOMBS_30:
        case RG_BUY_BOMBS_535:
        case RG_BUY_RED_POTION_40:
        case RG_BUY_RED_POTION_50:
            return true;
        default:
            return false;
    }
}

bool Randomizer::CheckContainsVanillaItem(RandomizerCheck randoCheck) {
    RandomizerGet randoGet = this->itemLocations[randoCheck];
    return IsItemVanilla(randoGet);
}

std::string Randomizer::GetAdultAltarText() const {
    return this->adultAltarText;
}

std::string Randomizer::GetChildAltarText() const {
    return this->childAltarText;
}

std::string Randomizer::GetGanonText() const {
    return ganonText;
}

std::string Randomizer::GetGanonHintText() const {
    return ganonHintText;
}

u8 Randomizer::GetRandoSettingValue(RandomizerSettingKey randoSettingKey) {
    return this->randoSettings[randoSettingKey];
}

s16 Randomizer::GetRandomizedItemIdFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId) {
    return GetItemFromGet(this->itemLocations[randomizerCheck], ogId);
}

std::thread randoThread;

void GenerateRandomizerImgui() {
    CVar_SetS32("gRandoGenerating", 1);
    CVar_Save();

    std::unordered_map<RandomizerSettingKey, u8> cvarSettings;
    cvarSettings[RSK_LOGIC_RULES] = CVar_GetS32("gRandomizeLogicRules", 0);
    cvarSettings[RSK_FOREST] = CVar_GetS32("gRandomizeForest", 0);
    cvarSettings[RSK_KAK_GATE] = CVar_GetS32("gRandomizeKakarikoGate", 0);
    cvarSettings[RSK_DOOR_OF_TIME] = CVar_GetS32("gRandomizeDoorOfTime", 0);
    cvarSettings[RSK_ZORAS_FOUNTAIN] = CVar_GetS32("gRandomizeZorasFountain", 0);
    cvarSettings[RSK_GERUDO_FORTRESS] = CVar_GetS32("gRandomizeGerudoFortress", 0);
    cvarSettings[RSK_RAINBOW_BRIDGE] = CVar_GetS32("gRandomizeRainbowBridge", 0);
    cvarSettings[RSK_RAINBOW_BRIDGE_STONE_COUNT] = CVar_GetS32("gRandomizeStoneCount", 3);
    cvarSettings[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT] = CVar_GetS32("gRandomizeMedallionCount", 6);
    cvarSettings[RSK_RAINBOW_BRIDGE_REWARD_COUNT] = CVar_GetS32("gRandomizeRewardCount", 9);
    cvarSettings[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT] = CVar_GetS32("gRandomizeDungeonCount", 8);
    cvarSettings[RSK_RAINBOW_BRIDGE_TOKEN_COUNT] = CVar_GetS32("gRandomizeTokenCount", 100);
    cvarSettings[RSK_RANDOM_TRIALS] = CVar_GetS32("gRandomizeGanonTrial", 0);
    cvarSettings[RSK_TRIAL_COUNT] = CVar_GetS32("gRandomizeGanonTrialCount", 0);
    cvarSettings[RSK_STARTING_OCARINA] = CVar_GetS32("gRandomizeStartingOcarina", 0);
    cvarSettings[RSK_SHUFFLE_OCARINA] = CVar_GetS32("gRandomizeShuffleOcarinas", 0) ||
                                        CVar_GetS32("gRandomizeStartingOcarina", 0);
    cvarSettings[RSK_STARTING_KOKIRI_SWORD] = CVar_GetS32("gRandomizeStartingKokiriSword", 0);
    cvarSettings[RSK_SHUFFLE_KOKIRI_SWORD] = CVar_GetS32("gRandomizeShuffleKokiriSword", 0) ||
                                             CVar_GetS32("gRandomizeStartingKokiriSword", 0);
    cvarSettings[RSK_STARTING_DEKU_SHIELD] = CVar_GetS32("gRandomizeStartingDekuShield", 0);
    cvarSettings[RSK_STARTING_MAPS_COMPASSES] = CVar_GetS32("gRandomizeStartingMapsCompasses", 2);
    cvarSettings[RSK_SHUFFLE_DUNGEON_REWARDS] = CVar_GetS32("gRandomizeShuffleDungeonReward", 0);
    cvarSettings[RSK_SHUFFLE_SONGS] = CVar_GetS32("gRandomizeShuffleSongs", 0);
    cvarSettings[RSK_SHUFFLE_TOKENS] = CVar_GetS32("gRandomizeShuffleTokens", 0);
    cvarSettings[RSK_SHUFFLE_SCRUBS] = CVar_GetS32("gRandomizeShuffleScrubs", 0);
    cvarSettings[RSK_SHUFFLE_COWS] = CVar_GetS32("gRandomizeShuffleCows", 0);
    cvarSettings[RSK_SHUFFLE_ADULT_TRADE] = CVar_GetS32("gRandomizeShuffleAdultTrade", 0);
    cvarSettings[RSK_SHUFFLE_MAGIC_BEANS] = CVar_GetS32("gRandomizeShuffleBeans", 0);
    cvarSettings[RSK_SKIP_CHILD_ZELDA] = CVar_GetS32("gRandomizeSkipChildZelda", 0);

    // if we skip child zelda, we start with zelda's letter, and malon starts
    // at the ranch, so we should *not* shuffle the weird egg
    cvarSettings[RSK_SHUFFLE_WEIRD_EGG] = ((CVar_GetS32("gRandomizeSkipChildZelda", 0) == 0) &&
                                            CVar_GetS32("gRandomizeShuffleWeirdEgg", 0));
    
    cvarSettings[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD] = CVar_GetS32("gRandomizeShuffleGerudoToken", 0);
    cvarSettings[RSK_SHUFFLE_FROG_SONG_RUPEES] = CVar_GetS32("gRandomizeShuffleFrogSongRupees", 0);
    cvarSettings[RSK_ITEM_POOL] = CVar_GetS32("gRandomizeItemPool", 1);
    cvarSettings[RSK_ICE_TRAPS] = CVar_GetS32("gRandomizeIceTraps", 1);
    cvarSettings[RSK_GOSSIP_STONE_HINTS] = CVar_GetS32("gRandomizeGossipStoneHints", 1);
    cvarSettings[RSK_HINT_CLARITY] = CVar_GetS32("gRandomizeHintClarity", 2);
    cvarSettings[RSK_HINT_DISTRIBUTION] = CVar_GetS32("gRandomizeHintDistribution", 1);
    cvarSettings[RSK_KEYSANITY] = CVar_GetS32("gRandomizeKeysanity", 2);
    cvarSettings[RSK_GERUDO_KEYS] = CVar_GetS32("gRandomizeGerudoKeys", 0);
    cvarSettings[RSK_BOSS_KEYSANITY] = CVar_GetS32("gRandomizeBossKeysanity", 2);
    cvarSettings[RSK_GANONS_BOSS_KEY] = CVar_GetS32("gRandomizeShuffleGanonBossKey", 1);
    cvarSettings[RSK_STARTING_CONSUMABLES] = CVar_GetS32("gRandomizeStartingConsumables", 0);
    cvarSettings[RSK_FULL_WALLETS] = CVar_GetS32("gRandomizeFullWallets", 0);
    
    // RANDOTODO implement chest minigame shuffle with keysanity
    cvarSettings[RSK_SHUFFLE_CHEST_MINIGAME] = false;

    cvarSettings[RSK_LANGUAGE] = CVar_GetS32("gLanguages", 0);

    cvarSettings[RSK_CUCCO_COUNT] = CVar_GetS32("gRandomizeCuccosToReturn", 7);
    cvarSettings[RSK_BIG_POE_COUNT] = CVar_GetS32("gRandomizeBigPoeTargetCount", 10);

    // If we skip child zelda, skip child stealth is pointless, so this needs to be reflected in the spoiler log
    cvarSettings[RSK_SKIP_CHILD_STEALTH] =
        !CVar_GetS32("gRandomizeSkipChildZelda", 0) && CVar_GetS32("gRandomizeSkipChildStealth", 0);

    cvarSettings[RSK_SKIP_EPONA_RACE] = CVar_GetS32("gRandomizeSkipEponaRace", 0);
    cvarSettings[RSK_SKIP_TOWER_ESCAPE] = CVar_GetS32("gRandomizeSkipTowerEscape", 0);
    cvarSettings[RSK_COMPLETE_MASK_QUEST] = CVar_GetS32("gRandomizeCompleteMaskQuest", 0);
    cvarSettings[RSK_ENABLE_GLITCH_CUTSCENES] = CVar_GetS32("gRandomizeEnableGlitchCutscenes", 0);

    cvarSettings[RSK_SKULLS_SUNS_SONG] = CVar_GetS32("gRandomizeGsExpectSunsSong", 0);

    // todo: this efficently when we build out cvar array support
    std::set<RandomizerCheck> excludedLocations;
    std::stringstream excludedLocationStringStream(CVar_GetString("gRandomizeExcludedLocations", ""));
    std::string excludedLocationString;
    while(getline(excludedLocationStringStream, excludedLocationString, ',')) {
        excludedLocations.insert((RandomizerCheck)std::stoi(excludedLocationString));
    }

    RandoMain::GenerateRando(cvarSettings, excludedLocations);

    CVar_SetS32("gRandoGenerating", 0);
    CVar_Save();
    CVar_Load();

    generated = 1;
}

void DrawRandoEditor(bool& open) {
    if (generated) {
        generated = 0;
        randoThread.join();
    }

    if (!open) {
        CVar_SetS32("gRandomizerSettingsEnabled", 0);
        return;
    }

    // Randomizer settings
    // Logic Settings
    const char* randoLogicRules[2] = { "Glitchless", "No logic" };

    // Open Settings
    const char* randoForest[3] = { "Closed", "Closed Deku", "Open" };
    const char* randoKakarikoGate[2] = { "Closed", "Open" };
    const char* randoDoorOfTime[3] = { "Closed", "Song only", "Open" };
    const char* randoZorasFountain[3] = { "Closed", "Closed as child", "Open" };
    const char* randoGerudoFortress[3] = { "Normal", "Fast", "Open" };
    const char* randoRainbowBridge[7] = { "Vanilla",         "Always open", "Stones", "Medallions",
                                          "Dungeon rewards", "Dungeons",    "Tokens" };
    const char* randoGanonsTrial[3] = { "Skip", "Set Number", "Random Number" };

    // World Settings
    const char* randoStartingAge[3] = { "Child", "Adult", "Random" };
    const char* randoShuffleEntrances[2] = { "Off", "On" };
    const char* randoShuffleDungeonsEntrances[2] = { "Off", "On" };
    const char* randoShuffleOverworldEntrances[2] = { "Off", "On" };
    const char* randoShuffleInteriorsEntrances[2] = { "Off", "On" };
    const char* randoShuffleGrottosEntrances[2] = { "Off", "On" };
    const char* randoBombchusInLogic[2] = { "Off", "On" };
    const char* randoAmmoDrops[3] = { "On + Bombchu", "Off", "On" };
    const char* randoHeartDropsAndRefills[4] = { "On", "No Drop", "No Refill", "Off" };

    // Shuffle Settings
    const char* randoShuffleDungeonRewards[4] = { "End of dungeons", "Any dungeon", "Overworld", "Anywhere" };
    const char* randoLinksPocket[4] = { "Dungeon Reward", "Advancement", "Anything", "Nothing" };
    const char* randoShuffleSongs[3] = { "Song Locations", "Dungeon Rewards", "Anywhere" };
    const char* randoShuffleTokens[4] = { "Off", "Dungeons", "Overworld", "All Tokens" };
    const char* randoShopsanity[7] = { "Off", "0", "1", "2", "3", "4", "Random" };
    const char* randoTokensanity[4] = { "Off", "Dungeons", "Overworld", "All Tokens" };
    const char* randoShuffleScrubs[4] = { "Off", "Affordable", "Expensive", "Random Prices" };
    const char* randoShuffleCows[2] = { "Off", "On" };
    const char* randoShuffleKokiriSword[2] = { "Off", "On" };
    const char* randoShuffleOcarinas[2] = { "Off", "On" };
    const char* randoShuffleWeirdEgg[2] = { "Off", "On" };
    const char* randoShuffleGerudoToken[2] = { "Off", "On" };
    const char* randoShuffleMagicBeans[2] = { "Off", "On" };
    const char* randoShuffleMerchants[3] = { "Off", "On (no hints)", "On (with hints)" };
    const char* randoShuffleFrogSongRupees[2] = { "Off", "On" };
    const char* randoShuffleAdultTrade[2] = { "Off", "On" };

    // Shuffle Dungeon Items Settings
    const char* randoShuffleMapsAndCompasses[6] = { "Start With",  "Vanilla",   "Own Dungeon",
                                                    "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleSmallKeys[6] = { "Start With",  "Vanilla",   "Own Dungeon",
                                             "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleGerudoFortressKeys[4] = { "Vanilla", "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleBossKeys[6] = { "Start With",  "Vanilla",   "Own Dungeon",
                                            "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleGanonsBossKey[6] = { "Vanilla",     "Own dungeon", "Start with",
                                                 "Any Dungeon", "Overworld",   "Anywhere" };

    // Timesaver Settings
    const char* randoSkipSongReplays[3] = { "Don't skip", "Skip (no SFX)", "Skip (Keep SFX)" };

    // Misc Settings
    const char* randoGossipStoneHints[4] = { "No Hints", "Need Nothing", "Mask of Truth", "Stone of Agony" };
    const char* randoHintClarity[3] = { "Obscure", "Ambiguous", "Clear" };
    const char* randoHintDistribution[4] = { "Useless", "Balanced", "Strong", "Very Strong" };
    const char* randoDamageMultiplier[7] = { "x1", "x2", "x4", "x8", "x16", "OHKO", "x1/2" };
    const char* randoStartingTime[2] = { "Day", "Night" };
    const char* randoChestAnimations[2] = { "Always Fast", "Match contents" };
    const char* randoChestSizeAndColor[2] = { "Vanilla", "Match contents" };
    const char* randoOpenInfoMenuWith[6] = { "Select", "Start", "D-Pad Up", "D-Pad Down", "D-Pad Right", "D-Pad Left" };
    const char* randoRandomTrapDamage[3] = { "Basic", "Advanced", "Off" };

    // Item Pool Settings
    const char* randoItemPool[4] = { "Plentiful", "Balanced", "Scarce", "Minimal" };
    const char* randoIceTraps[5] = { "Off", "Normal", "Extra", "Mayhem", "Onslaught" };

    // SFX Settings
    const char* randoSFXBackgroundMusic[3] = { "Normal", "No Music", "Random" };
    const char* randoSFXFanfares[3] = { "Normal", "No Fanfares", "Random" };
    const char* randoSFXLowHP[29] = { "Default",
                                      "Completely Random",
                                      "Random Ear-Safe",
                                      "None",
                                      "Bark",
                                      "Bomb Bounce",
                                      "Bongo Bongo Low",
                                      "Business Scrub",
                                      "Carrot Refill",
                                      "Cluck",
                                      "Drawbridge Set",
                                      "Guay",
                                      "HP Recover",
                                      "Horse Trot",
                                      "Iront Boots",
                                      "Moo",
                                      "Mweep!",
                                      "Navi \"Hey!\"",
                                      "Navi Random",
                                      "Notification",
                                      "Pot Shattering",
                                      "Ribbit",
                                      "Rupee (Silver)",
                                      "Soft Beep",
                                      "Switch",
                                      "Sword Bonk",
                                      "Tambourine",
                                      "Timer",
                                      "Zelda Gasp (Adult)" };
    const char* randoSFXHorse[13] = {
        "Default",       "Completely Random", "Random Ear-Safe", "Random Choice", "None",
        "Armos",         "Child Scream",      "Great Fairy",     "Moo",           "Mweep!",
        "Redead Scream", "Ruto Wiggle",       "Stalchild Attack"
    };
    const char* randoSFXNightfall[13] = {
        "Default",        "Completely Random",    "Random Ear-Safe", "Random Choice", "None",
        "Cockadoodiedoo", "Gold Skulltula Token", "Great Fairy",     "Moo",           "Mweep!",
        "Redead Moan",    "Talon Snore",          "Thunder"
    };
    const char* randoSFXHoverBoots[11] = {
        "Default",      "Completely Random",  "Random Ear-Safe", "Random Choice", "None",      "Bark",
        "Cartoon Fall", "Flare Dancer Laugh", "Mweep!",          "Shabom Pop",    "Tambourine"
    };
    const char* randoSFXOcarina[7] = { "Dafault", "Random Choice", "Flute", "Harp", "Whistle", "Malon", "Grind Organ" };
    const char* randoSFXMenu[65] = { "Default",
                                     "Completely Random",
                                     "Random Ear-Safe",
                                     "Random Choise",
                                     "None",
                                     "Bark",
                                     "Bomb Bounce",
                                     "Bongo Bongo High",
                                     "Bongo Bongo Low",
                                     "Bottle Cork",
                                     "Bow Twang",
                                     "Bubble Laugh",
                                     "Carrot Refill",
                                     "Change Item",
                                     "Child Pant",
                                     "Cluck",
                                     "Deku Baba",
                                     "Drawbridge Set",
                                     "Dusk Howl",
                                     "Fanfare (Light)",
                                     "Fanfare (Mediaum)",
                                     "Field Shrub",
                                     "Flare Dancer Startled",
                                     "Ganondorf \"Teh!\"",
                                     "Gohma Larva Croak",
                                     "Gold Skulltula Token",
                                     "Goron Wake",
                                     "Guay",
                                     "Gunshot",
                                     "HP Low",
                                     "HP Recover",
                                     "Hammer Bonk",
                                     "Horse Trot",
                                     "Iron Boots",
                                     "Iron Knuckle",
                                     "Moo",
                                     "Mweep!",
                                     "Notification",
                                     "Phantom Ganon Laugh",
                                     "Plant Explode",
                                     "Pot Shattering",
                                     "Redead Moan",
                                     "Ribbit",
                                     "Rupee",
                                     "Rupee (Silver)",
                                     "Ruto Crash",
                                     "Ruto Lift",
                                     "Ruto Thrown",
                                     "Scrub Emerge",
                                     "Shabom Bounce",
                                     "Shabom Pop",
                                     "Shellblade",
                                     "Skulltula",
                                     "Soft Beep",
                                     "Spit Nut",
                                     "Switch",
                                     "Sword Bonk",
                                     "Talon \"Hmmm\"",
                                     "Talon Snore",
                                     "Talon Surprised",
                                     "Tambourine",
                                     "Target Ennemy",
                                     "Target Neutral",
                                     "Timer",
                                     "Zelda Gasp (Adult)" };
    const char* randoSFXNavi[32] = { "Default",
                                     "Completely Random",
                                     "Random Ear-Safe",
                                     "Random  Choice",
                                     "None",
                                     "Bark",
                                     "Business Scrub",
                                     "Carrot Refill",
                                     "Click",
                                     "Dusk Howl",
                                     "Exploding Crate",
                                     "Explosion",
                                     "Great Fairy",
                                     "Guay",
                                     "HP Low",
                                     "HP Recover",
                                     "Horse Neigh",
                                     "Ice Shattering",
                                     "Moo",
                                     "Mweep!",
                                     "Navi \"Hello!\"",
                                     "Notification",
                                     "Poe",
                                     "Pot Shattering",
                                     "Redead Scream",
                                     "Ribit",
                                     "Ruto Giggle",
                                     "Skulltula",
                                     "Soft Beep",
                                     "Tambourine",
                                     "Timer",
                                     "Zelda Gasp (Adult)" };

    ImGui::SetNextWindowSize(ImVec2(830, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Randomizer Editor", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    bool disableEditingRandoSettings = CVar_GetS32("gRandoGenerating", 0) || CVar_GetS32("gOnFileSelectNameEntry", 0);
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disableEditingRandoSettings);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (disableEditingRandoSettings ? 0.5f : 1.0f));
    SohImGui::EnhancementCheckbox("Enable Randomizer", "gRandomizer");

    if (CVar_GetS32("gRandomizer", 0) == 1) {
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        if (ImGui::Button("Generate Seed")) {
            if (CVar_GetS32("gRandoGenerating", 0) == 0) {
                randoThread = std::thread(&GenerateRandomizerImgui);
            }
        }
        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        std::string spoilerfilepath = CVar_GetString("gSpoilerLog", "");
        ImGui::Text("Spoiler File: %s", spoilerfilepath.c_str());

        // RANDOTODO settings presets
        // std::string presetfilepath = CVar_GetString("gLoadedPreset", "");
        // ImGui::Text("Settings File: %s", presetfilepath.c_str());
    }
    PaddedSeparator();

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    static ImVec2 cellPadding(8.0f, 8.0f);

    if (CVar_GetS32("gRandomizer", 0) == 1 &&
        ImGui::BeginTabBar("Randomizer Settings", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Main Rules")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoMainRules", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Open Settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Shuffle Settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Shuffle Dungeon Items", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - OPEN SETTINGS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::PushItemWidth(-FLT_MIN);

                if (CVar_GetS32("gRandomizeAllOpenSettings", 0) != 1) {
                    // Forest
                    ImGui::Text(Settings::OpenForest.GetName().c_str());
                    InsertHelpHoverText("Closed - Kokiri sword & shield are required to access "
                                        "the Deku Tree, and completing the Deku Tree is required to "
                                        "access the Hyrule Field exit.\n"
                                        "\n"
                                        "Closed Deku - Kokiri boy no longer blocks the path to Hyrule "
                                        "Field but Mido still requires the Kokiri sword and Deku shield "
                                        "to access the tree.\n"
                                        "\n"
                                        "Open - Mido no longer blocks the path to the Deku Tree. Kokiri "
                                        "boy no longer blocks the path out of the forest.");
                    SohImGui::EnhancementCombobox("gRandomizeForest", randoForest, 3, 0);
                    PaddedSeparator();
                    // Kakariko Gate
                    ImGui::Text(Settings::OpenKakariko.GetName().c_str());
                    InsertHelpHoverText("Closed - The gate will remain closed until Zelda's letter "
                                        "is shown to the guard.\n"
                                        "\n"
                                        "Open - The gate is always open. The happy mask shop "
                                        "will open immediately after obtaining Zelda's letter.");
                    SohImGui::EnhancementCombobox("gRandomizeKakarikoGate", randoKakarikoGate, 2, 0);
                    PaddedSeparator();

                    // Door of Time
                    ImGui::Text(Settings::OpenDoorOfTime.GetName().c_str());
                    InsertHelpHoverText("Closed - The Ocarina of Time, the Song of Time and all "
                                        "three spiritual stones are required to open the Door of Time.\n"
                                        "\n"
                                        "Song only - Play the Song of Time in front of the Door of "
                                        "Time to open it.\n"
                                        "\n"
                                        "Open - The Door of Time is permanently open with no requirements.");
                    SohImGui::EnhancementCombobox("gRandomizeDoorOfTime", randoDoorOfTime, 3, 0);
                    PaddedSeparator();

                    // Zora's Fountain
                    ImGui::Text(Settings::ZorasFountain.GetName().c_str());
                    InsertHelpHoverText("Closed - King Zora obstructs the way to Zora's Fountain. "
                                        "Ruto's letter must be shown as child Link in order to move "
                                        "him in both time periods.\n"
                                        "\n"
                                        "Closed as child - Ruto's Letter is only required to move King Zora "
                                        "as child Link. Zora's Fountain starts open as adult.\n"
                                        "\n"
                                        "Open - King Zora has already mweeped out of the way in both "
                                        "time periods. Ruto's Letter is removed from the item pool.");
                    SohImGui::EnhancementCombobox("gRandomizeZorasFountain", randoZorasFountain, 3, 0);
                    PaddedSeparator();

                    // Gerudo Fortress
                    ImGui::Text(Settings::GerudoFortress.GetName().c_str());
                    InsertHelpHoverText("Sets the amount of carpenters required to repair the bridge "
                                        "in Gerudo Valley.\n"
                                        "\n"
                                        "Normal - All 4 carpenters are required to be saved.\n"
                                        "\n"
                                        "Fast - Only the bottom left carpenter requires rescuing.\n"
                                        "\n"
                                        "Open - The bridge is repaired from the start.");
                    SohImGui::EnhancementCombobox("gRandomizeGerudoFortress", randoGerudoFortress, 3, 0);
                    PaddedSeparator();

                    // Rainbow Bridge
                    ImGui::Text(Settings::Bridge.GetName().c_str());
                    InsertHelpHoverText("Alters the requirements to open the bridge to Ganon's Castle.\n"
                                        "\n"
                                        "Vanilla - Obtain the Shadow Medallion, Spirit Medallion and Light Arrows.\n"
                                        "\n"
                                        "Always open - No requirements.\n"
                                        "\n"
                                        "Stones - Obtain the specified amount of spiritual stones.\n"
                                        "\n"
                                        "Medallions - Obtain the specified amount of medallions.\n"
                                        "\n"
                                        "Dungeon rewards - Obtain the specified total sum of spiritual "
                                        "stones or medallions.\n"
                                        "\n"
                                        "Dungeons - Complete the specified amount of dungeons. Dungeons "
                                        "are considered complete after stepping in to the blue warp after "
                                        "the boss.\n"
                                        "\n"
                                        "Tokens - Obtain the specified amount of Skulltula tokens.");
                    SohImGui::EnhancementCombobox("gRandomizeRainbowBridge", randoRainbowBridge, 7, 0);
                    ImGui::PopItemWidth();
                    switch (CVar_GetS32("gRandomizeRainbowBridge", 0)) {
                        case 0:
                            break;
                        case 1:
                            break;
                        case 2:
                            ImGui::Dummy(ImVec2(0.0f, 0.0f));
                            SohImGui::EnhancementSliderInt("Stone Count: %d", "##RandoStoneCount",
                                                           "gRandomizeStoneCount", 1, 3, "", 3, true);
                            break;
                        case 3:
                            ImGui::Dummy(ImVec2(0.0f, 0.0f));
                            SohImGui::EnhancementSliderInt("Medallion Count: %d", "##RandoMedallionCount",
                                                           "gRandomizeMedallionCount", 1, 6, "", 6, true);
                            break;
                        case 4:
                            ImGui::Dummy(ImVec2(0.0f, 0.0f));
                            SohImGui::EnhancementSliderInt("Reward Count: %d", "##RandoRewardCount",
                                                           "gRandomizeRewardCount", 1, 9, "", 9, true);
                            break;
                        case 5:
                            ImGui::Dummy(ImVec2(0.0f, 0.0f));
                            SohImGui::EnhancementSliderInt("Dungeon Count: %d", "##RandoDungeonCount",
                                                           "gRandomizeDungeonCount", 1, 8, "", 8, true);
                            break;
                        case 6:
                            ImGui::Dummy(ImVec2(0.0f, 0.0f));
                            SohImGui::EnhancementSliderInt("Token Count: %d", "##RandoTokenCount",
                                                           "gRandomizeTokenCount", 1, 100, "", 100, true);
                            break;
                    }
                    PaddedSeparator();

                    // Random Ganon's Trials
                    ImGui::Text("Ganon's Trials");
                    InsertHelpHoverText("Sets the number of Ganon's Trials required to dispel the barrier\n\n"
                                        "Skip - No Trials are required and the barrier is already dispelled.\n\n"
                                        "Set Number - Select a number of trials that will be required from the"
                                        "slider below. Which specific trials you need to complete will be random.\n\n"
                                        "Random Number - A Random number and set of trials will be required.");
                    SohImGui::EnhancementCombobox("gRandomizeGanonTrial", randoGanonsTrial, 3, 0);
                    if (CVar_GetS32("gRandomizeGanonTrial", 0) == 1) {
                        SohImGui::EnhancementSliderInt("Ganon's Trial Count: %d", "##RandoTrialCount",
                                                       "gRandomizeGanonTrialCount", 1, 6, "", 6);
                        InsertHelpHoverText("Set the number of trials required to enter Ganon's Tower.");
                    }
                }

                // COLUMN 2 - Shuffle Settings
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::PushItemWidth(-FLT_MIN);
                if (CVar_GetS32("gRandomizeAllShuffleSettings", 0) != 1) {

                    // Shuffle Songs
                    ImGui::Text(Settings::ShuffleSongs.GetName().c_str());
                    InsertHelpHoverText(
                        "Song locations - Songs will only appear at locations that normally teach songs.\n"
                        "\n"
                        "Dungeon rewards - Songs appear after beating a major dungeon boss.\n"
                        "The 4 remaining songs are located at:\n"
                        "  - Zelda's lullaby location\n"
                        "  - Ice Cavern's Serenade of Water location\n"
                        "  - Bottom of the Well Lens of Truth location\n"
                        "  - Gerudo Training Ground's Ice Arrows location\n"
                        "\n"
                        "Anywhere - Songs can appear at any location.");

                    SohImGui::EnhancementCombobox("gRandomizeShuffleSongs", randoShuffleSongs, 3, 0);
                    PaddedSeparator();

                    // Shuffle Scrubs
                    ImGui::Text(Settings::Scrubsanity.GetName().c_str());
                    InsertHelpHoverText(
                        "Off - Scrubs will not be shuffled. The 3 Scrubs that give one-time items in the vanilla game (PoH, Deku Nut capacity, and Deku Stick capacity) will have random items.\n"
                        "\n"
                        "Affordable - Scrubs will be shuffled and their item will cost 10 rupees.\n"
                        "\n"
                        "Expensive - Scrubs will be shuffled and their item will cost the vanilla price.\n"
                        "\n"
                        "Random - Scrubs will be shuffled and their item will cost will be between 0-95 rupees.\n"
                    );
                    SohImGui::EnhancementCombobox("gRandomizeShuffleScrubs", randoShuffleScrubs, 4, 0);
                    PaddedSeparator();

                    // Shuffle Tokens
                    ImGui::Text(Settings::Tokensanity.GetName().c_str());
                    InsertHelpHoverText("Shuffles Golden Skulltula Tokens into the item pool. This means "
                                        "Golden Skulltulas can contain other items as well.\n"
                                        "\n"
                                        "Off - GS tokens will not be shuffled.\n"
                                        "\n"
                                        "Dungeons - Only shuffle GS tokens that are within dungeons.\n"
                                        "\n"
                                        "Overworld - Only shuffle GS tokens that are outside of dungeons.\n"
                                        "\n"
                                        "All Tokens - Shuffle all 100 GS tokens.");
                    SohImGui::EnhancementCombobox("gRandomizeShuffleTokens", randoShuffleTokens, 4, 0);
                    PaddedSeparator();

                    SohImGui::EnhancementCheckbox("Nighttime GS expect Sun's Song", "gRandomizeGsExpectSunsSong");
                    InsertHelpHoverText("All Golden Skulltulas that require nighttime to appear will only be "
                                        "expected to be collected after getting Sun's Song.");
                    PaddedSeparator();


                    // Shuffle Cows
                    SohImGui::EnhancementCheckbox(Settings::ShuffleCows.GetName().c_str(), "gRandomizeShuffleCows");
                    InsertHelpHoverText("Cows give a randomized item from the pool upon performing Epona's Song in front of them.");
                    PaddedSeparator();

                    // Shuffle Adult Trade Quest
                    SohImGui::EnhancementCheckbox(Settings::ShuffleAdultTradeQuest.GetName().c_str(),
                                                  "gRandomizeShuffleAdultTrade");
                    InsertHelpHoverText("Adds all of the adult trade quest items into the pool, each of which "
                                        "can be traded for a unique reward.\n"
                                        "\n"
                                        "You will be able to choose which of your owned adult trade items is visible "
                                        "in the inventory by selecting the item with A and using the control stick or "
                                        "D-pad.\n"
                                        "\n"
                                        "If disabled, only the Claim Check will be found in the pool.");
                    PaddedSeparator();

                    SohImGui::EnhancementCheckbox(Settings::ShuffleMagicBeans.GetName().c_str(), "gRandomizeShuffleBeans");
                    InsertHelpHoverText("Enabling this adds a pack of 10 beans to the item pool and changes the Magic Bean Salesman to sell a"
                                        "random item at a price of 60 rupees.");
                    PaddedSeparator();

                    if (CVar_GetS32("gRandomizeStartingKokiriSword", 0) == 0) {
                        // Shuffle Kokiri Sword
                        SohImGui::EnhancementCheckbox(Settings::ShuffleKokiriSword.GetName().c_str(),
                                                      "gRandomizeShuffleKokiriSword");
                        InsertHelpHoverText("Shuffles the Kokiri Sword into the item pool.\n"
                                            "\n"
                                            "This will require the use of sticks until the Kokiri Sword is found.");
                        PaddedSeparator();
                    }

                    if (CVar_GetS32("gRandomizeStartingOcarina", 0) == 0) {
                        // Shuffle Ocarinas
                        SohImGui::EnhancementCheckbox(Settings::ShuffleOcarinas.GetName().c_str(),
                                                      "gRandomizeShuffleOcarinas");
                        InsertHelpHoverText(
                            "Enabling this shuffles the Fairy Ocarina and the Ocarina of Time into the item pool.\n"
                            "\n"
                            "This will require finding an Ocarina before being able to play songs.");
                        PaddedSeparator();
                    }

                    // Shuffle Weird Egg
                    // Disabled when Skip Child Zelda is active
                    if (!disableEditingRandoSettings) {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, CVar_GetS32("gRandomizeSkipChildZelda", 0));
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha,
                                            ImGui::GetStyle().Alpha *
                                                (CVar_GetS32("gRandomizeSkipChildZelda", 0) ? 0.5f : 1.0f));
                    }
                    SohImGui::EnhancementCheckbox(Settings::ShuffleWeirdEgg.GetName().c_str(),
                                                  "gRandomizeShuffleWeirdEgg");
                    if (!disableEditingRandoSettings) {
                        ImGui::PopStyleVar();
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
                            CVar_GetS32("gRandomizeSkipChildZelda", 0)) {
                            ImGui::SetTooltip("%s", "This option is disabled because \"Skip Child Zelda\" is enabled");
                        }
                        ImGui::PopItemFlag();
                    }
                    InsertHelpHoverText("Shuffles the Weird Egg from Malon in to the item pool. Enabling "
                                        "\"Skip Child Zelda\" disables this feature.\n"
                                        "\n"
                                        "The Weird Egg is required to unlock several events:\n"
                                        "  - Zelda's Lullaby from Impa\n"
                                        "  - Saria's song in Sacred Forest Meadow\n"
                                        "  - Epona's song and chicken minigame at Lon Lon Ranch\n"
                                        "  - Zelda's letter for Kakariko gate (if set to closed)\n"
                                        "  - Happy Mask Shop sidequest\n");
                    PaddedSeparator();

                    // Shuffle Gerudo Membership Card
                    SohImGui::EnhancementCheckbox(Settings::ShuffleGerudoToken.GetName().c_str(),
                                                  "gRandomizeShuffleGerudoToken");
                    InsertHelpHoverText("Shuffles the Gerudo Membership Card into the item pool.\n"
                                        "\n"
                                        "The Gerudo Card is required to enter the Gerudo Training Grounds, opening "
                                        "the gate to Haunted Wasteland and the Horseback Archery minigame.");
                    PaddedSeparator();

                    // Shuffle Frog Song Rupees
                    SohImGui::EnhancementCheckbox(Settings::ShuffleFrogSongRupees.GetName().c_str(),
                                                  "gRandomizeShuffleFrogSongRupees");
                    InsertHelpHoverText("Shuffles 5 Purple Rupees into to the item pool, and allows\n"
                                        "you to earn items by playing songs at the Frog Choir.\n"
                                        "\n"
                                        "This setting does not effect the item earned from playing\n"
                                        "the Song of Storms and the frog song minigame.");
                }
                ImGui::PopItemWidth();

                // COLUMN 3 - Shuffle Dungeon Items
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::PushItemWidth(-FLT_MIN);

                // Shuffle Dungeon Rewards
                ImGui::Text(Settings::ShuffleRewards.GetName().c_str());
                InsertHelpHoverText(
                    "Shuffles the location of spiritual stones and medallions.\n"
                    "\n"
                    "End of dungeons - Spiritual stones and medallions will be given as rewards "
                    "for beating major dungeons. Link will always start with one stone or medallion.\n"
                    "\n"
                    "Any dungeon - Spiritual stones and medallions can be found inside any dungeon.\n"
                    "\n"
                    "Overworld - Spiritual stones and medallions can only be found outside of dungeons.\n"
                    "\n"
                    "Anywhere - Spiritual stones and medallions can appear anywhere.");
                SohImGui::EnhancementCombobox("gRandomizeShuffleDungeonReward", randoShuffleDungeonRewards, 4, 0);
                PaddedSeparator();

                // Maps & Compasses
                ImGui::Text(Settings::MapsAndCompasses.GetName().c_str());
                InsertHelpHoverText("Start with - You will start with Maps & Compasses from all dungeons.\n"
                                    "\n"
                                    "Vanilla - Maps & Compasses will appear in their vanilla locations.\n"
                                    "\n"
                                    "Own dungeon - Maps & Compasses can only appear in their respective dungeon.\n"
                                    "\n"
                                    "Any dungeon - Maps & Compasses can only appear inside of any dungon.\n"
                                    "\n"
                                    "Overworld - Maps & Compasses can only appear outside of dungeons.\n"
                                    "\n"
                                    "Anywhere - Maps & Compasses can appear anywhere in the world.");
                SohImGui::EnhancementCombobox("gRandomizeStartingMapsCompasses", randoShuffleMapsAndCompasses, 6, 2);
                PaddedSeparator();

                // Keysanity
                ImGui::Text(Settings::Keysanity.GetName().c_str());
                InsertHelpHoverText("Start with - You will start with all Small Keys from all dungeons.\n"
                                    "\n"
                                    "Vanilla - Small Keys will appear in their vanilla locations.\n"
                                    "\n"
                                    "Own dungeon - Small Keys can only appear in their respective dungeon.\n"
                                    "\n"
                                    "Any dungeon - Small Keys can only appear inside of any dungon.\n"
                                    "\n"
                                    "Overworld - Small Keys can only appear outside of dungeons.\n"
                                    "\n"
                                    "Anywhere - Small Keys can appear anywhere in the world.");
                SohImGui::EnhancementCombobox("gRandomizeKeysanity", randoShuffleSmallKeys, 6, 2);
                PaddedSeparator();

                // Gerudo Keys
                ImGui::Text(Settings::GerudoKeys.GetName().c_str());
                InsertHelpHoverText("Vanilla - Thieve's Hideout Keys will appear in their vanilla locations.\n"
                                    "\n"
                                    "Any dungeon - Thieve's Hideout Keys can only appear inside of any dungon.\n"
                                    "\n"
                                    "Overworld - Thieve's Hideout Keys can only appear outside of dungeons.\n"
                                    "\n"
                                    "Anywhere - Thieve's Hideout Keys can appear anywhere in the world.");
                SohImGui::EnhancementCombobox("gRandomizeGerudoKeys", randoShuffleGerudoFortressKeys, 4, 0);
                PaddedSeparator();

                // Boss Keysanity
                ImGui::Text(Settings::BossKeysanity.GetName().c_str());
                InsertHelpHoverText("Start with - You will start with Boss keys from all dungeons.\n"
                                    "\n"
                                    "Vanilla - Boss Keys will appear in their vanilla locations.\n"
                                    "\n"
                                    "Own dungeon - Boss Keys can only appear in their respective dungeon.\n"
                                    "\n"
                                    "Any dungeon - Boss Keys can only appear inside of any dungon.\n"
                                    "\n"
                                    "Overworld - Boss Keys can only appear outside of dungeons.\n"
                                    "\n"
                                    "Anywhere - Boss Keys can appear anywhere in the world.");
                SohImGui::EnhancementCombobox("gRandomizeBossKeysanity", randoShuffleBossKeys, 6, 2);
                PaddedSeparator();

                // Ganon's Boss Key
                ImGui::Text(Settings::GanonsBossKey.GetName().c_str());
                InsertHelpHoverText("Vanilla - Ganon's Boss Key will appear in the vanilla location.\n"
                                    "\n"
                                    "Own dungeon - Ganon's Boss Key can appear anywhere inside Ganon's Castle.\n"
                                    "\n"
                                    "Start with - Places Ganon's Boss Key in your starting inventory."
                                    "\n"
                                    "Any dungeon - Ganon's Boss Key Key can only appear inside of any dungon.\n"
                                    "\n"
                                    "Overworld - Ganon's Boss Key Key can only appear outside of dungeons.\n"
                                    "\n"
                                    "Anywhere - Ganon's Boss Key Key can appear anywhere in the world.");
                SohImGui::EnhancementCombobox("gRandomizeShuffleGanonBossKey", randoShuffleGanonsBossKey, 6, 1);

                ImGui::PopItemWidth();
                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Other")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoOther", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Timesavers", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("World Settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Item Pool & Hint Settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - TIME SAVERS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;

                // Cuccos to return
                SohImGui::EnhancementSliderInt("Cuccos to return: %d", "##RandoCuccosToReturn",
                                               "gRandomizeCuccosToReturn", 0, 7, "", 7, true);
                InsertHelpHoverText("The amount of cuccos needed to claim the reward from Anju the cucco lady");
                PaddedSeparator();

                // Big Poe Target Count
                SohImGui::EnhancementSliderInt("Big Poe Target Count: %d", "##RandoBigPoeTargetCount",
                                               "gRandomizeBigPoeTargetCount", 1, 10, "", 10, true);
                InsertHelpHoverText("The Poe collector will give a reward for turning in this many Big Poes.");
                PaddedSeparator();

                // Skip child stealth
                // Disabled when Skip Child Zelda is active
                if (!disableEditingRandoSettings) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, CVar_GetS32("gRandomizeSkipChildZelda", 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha,
                                        ImGui::GetStyle().Alpha *
                                            (CVar_GetS32("gRandomizeSkipChildZelda", 0) ? 0.5f : 1.0f));
                }
                SohImGui::EnhancementCheckbox(Settings::SkipChildStealth.GetName().c_str(),
                                              "gRandomizeSkipChildStealth");
                if (!disableEditingRandoSettings) {
                    ImGui::PopStyleVar();
                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
                        CVar_GetS32("gRandomizeSkipChildZelda", 0)) {
                        ImGui::SetTooltip("%s", "This option is disabled because \"Skip Child Zelda\" is enabled");
                    }
                    ImGui::PopItemFlag();
                }
                InsertHelpHoverText("The crawlspace into Hyrule Castle goes straight to Zelda, skipping the guards.");
                PaddedSeparator();

                // Skip child zelda
                SohImGui::EnhancementCheckbox("Skip Child Zelda", "gRandomizeSkipChildZelda");
                InsertHelpHoverText("Start with Zelda's Letter in your inventory and skip the sequence up "
                                    "until after meeting Zelda. Disables the ability to shuffle Weird Egg.");
                PaddedSeparator();

                // Skip Epona race
                SohImGui::EnhancementCheckbox(Settings::SkipEponaRace.GetName().c_str(), "gRandomizeSkipEponaRace");
                InsertHelpHoverText("Epona can be summoned with Epona's Song without needing to race Ingo.");
                PaddedSeparator();

                // Skip tower escape
                SohImGui::EnhancementCheckbox(Settings::SkipTowerEscape.GetName().c_str(), "gRandomizeSkipTowerEscape");
                InsertHelpHoverText("The tower escape sequence between Ganondorf and Ganon will be skipped.");
                PaddedSeparator();

                // Complete mask quest
                SohImGui::EnhancementCheckbox(Settings::CompleteMaskQuest.GetName().c_str(),
                                              "gRandomizeCompleteMaskQuest");
                InsertHelpHoverText("Once the happy mask shop is opened, all masks will be available to be borrowed.");
                PaddedSeparator();

                // Enable Glitch-Useful Cutscenes
                SohImGui::EnhancementCheckbox(Settings::EnableGlitchCutscenes.GetName().c_str(),
                                              "gRandomizeEnableGlitchCutscenes");
                InsertHelpHoverText(
                    "The cutscenes of the Poes in Forest Temple and Darunia in Fire Temple will not be skipped. "
                    "These cutscenes are only useful for glitched gameplay and can be safely skipped otherwise.");

                // COLUMN 2 - WORLD SETTINGS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::Text("Coming soon");
                
                ImGui::PopItemWidth();

                // COLUMN 3 - ITEM POOL & HINT SETTINGS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::PushItemWidth(-FLT_MIN);

                ImGui::Text(Settings::ItemPoolValue.GetName().c_str());
                InsertHelpHoverText("Sets how many major items appear in the item pool.\n"
                                    "\n"
                                    "Plentiful - Extra major items are added to the pool.\n"
                                    "\n"
                                    "Balanced - Original item pool.\n"
                                    "\n"
                                    "Scarce - Some excess items are removed, including health upgrades.\n"
                                    "\n"
                                    "Minimal - Most excess items are removed.");
                SohImGui::EnhancementCombobox("gRandomizeItemPool", randoItemPool, 4, 1);
                PaddedSeparator();

                // Ice Traps
                ImGui::Text(Settings::IceTrapValue.GetName().c_str());
                InsertHelpHoverText("Sets how many items are replaced by ice traps.\n"
                                    "\n"
                                    "Off - No ice traps.\n"
                                    "\n"
                                    "Normal - Only Ice Traps from the base item pool are shuffled in.\n"
                                    "\n"
                                    "Extra - Chance to replace added junk items with additional ice traps.\n"
                                    "\n"
                                    "Mayhem - All added junk items will be Ice Traps.\n"
                                    "\n"
                                    "Onslaught - All junk items will be replaced by Ice Traps, even those "
                                    "in the base pool.");
                SohImGui::EnhancementCombobox("gRandomizeIceTraps", randoIceTraps, 5, 1);

                PaddedSeparator();

                // Gossip Stone Hints
                ImGui::Text(Settings::GossipStoneHints.GetName().c_str());
                InsertHelpHoverText(
                    "Allows Gossip Stones to provide hints on item locations. Hints mentioning "
                    "\"Way of the Hero\" indicate a location that holds an item required to beat "
                    "the seed.\n"
                    "\n"
                    "No hints - No hints will be given at all.\n"
                    "\n"
                    "Need Nothing - Hints are always available from Gossip Stones.\n"
                    "\n"
                    "Need Stone of Agony - Hints are only available after obtaining the Stone of Agony.\n"
                    "\n"
                    "Need Mask of Truth - Hints are only available whilst wearing the Mask of Truth.\n");

                SohImGui::EnhancementCombobox("gRandomizeGossipStoneHints", randoGossipStoneHints, 4, 1);
                if (CVar_GetS32("gRandomizeGossipStoneHints", 1) != 0) {
                    // Hint Clarity
                    ImGui::Dummy(ImVec2(0.0f, 0.0f));
                    ImGui::Indent();
                    ImGui::Text(Settings::ClearerHints.GetName().c_str());
                    InsertHelpHoverText("Sets the difficulty of hints.\n"
                                        "\n"
                                        "Obscure - Hints are unique for each item, but the writing may be cryptic.\n"
                                        "Ex: Kokiri Sword > a butter knife\n"
                                        "\n"
                                        "Ambiguous - Hints are clearly written, but may refer to more than one item.\n"
                                        "Ex: Kokiri Sword > a sword\n"
                                        "\n"
                                        "Clear - Hints are clearly written and are unique for each item.\n"
                                        "Ex: Kokiri Sword > the Kokiri Sword");
                    SohImGui::EnhancementCombobox("gRandomizeHintClarity", randoHintClarity, 3, 2);

                    // Hint Distribution
                    ImGui::Dummy(ImVec2(0.0f, 0.0f));
                    ImGui::Text(Settings::HintDistribution.GetName().c_str());
                    InsertHelpHoverText("Sets how many hints will be useful.\n"
                                        "\n"
                                        "Useless - Only junk hints.\n"
                                        "\n"
                                        "Balanced - Recommended hint spread.\n"
                                        "\n"
                                        "Strong - More useful hints.\n"
                                        "\n"
                                        "Very Strong - Many powerful hints.");
                    SohImGui::EnhancementCombobox("gRandomizeHintDistribution", randoHintDistribution, 4, 1);
                    ImGui::Unindent();
                }

                ImGui::PopItemWidth();
                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        static bool locationsTabOpen = false;
        if (ImGui::BeginTabItem("Locations")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (!locationsTabOpen) {
                locationsTabOpen = true;
                RandomizerCheckObjects::UpdateImGuiVisibility();
                // todo: this efficently when we build out cvar array support
                std::stringstream excludedLocationStringStream(CVar_GetString("gRandomizeExcludedLocations", ""));
                std::string excludedLocationString;
                excludedLocations.clear();
                while (getline(excludedLocationStringStream, excludedLocationString, ',')) {
                    excludedLocations.insert((RandomizerCheck)std::stoi(excludedLocationString));
                }
            }

            if (ImGui::BeginTable("tableRandoLocations", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Included", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Excluded", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - INCLUDED LOCATIONS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;

                static ImGuiTextFilter locationSearch;
                locationSearch.Draw();

                ImGui::BeginChild("ChildIncludedLocations", ImVec2(0, -8));
                for (auto [rcArea, rcObjects] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
                    bool hasItems = false;
                    for (auto [randomizerCheck, rcObject] : rcObjects) {
                        if (rcObject.visibleInImgui && !excludedLocations.count(rcObject.rc) &&
                            locationSearch.PassFilter(rcObject.rcSpoilerName.c_str())) {

                            hasItems = true;
                            break;
                        }
                    }

                    if (hasItems) {
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        if (ImGui::TreeNode(RandomizerCheckObjects::GetRCAreaName(rcArea).c_str())) {
                            for (auto [randomizerCheck, rcObject] : rcObjects) {
                                if (rcObject.visibleInImgui && !excludedLocations.count(rcObject.rc) &&
                                    locationSearch.PassFilter(rcObject.rcSpoilerName.c_str())) {

                                    if (ImGui::ArrowButton(std::to_string(rcObject.rc).c_str(), ImGuiDir_Right)) {
                                        excludedLocations.insert(rcObject.rc);
                                        // todo: this efficently when we build out cvar array support
                                        std::string excludedLocationString = "";
                                        for (auto excludedLocationIt : excludedLocations) {
                                            excludedLocationString += std::to_string(excludedLocationIt);
                                            excludedLocationString += ",";
                                        }
                                        CVar_SetString("gRandomizeExcludedLocations", excludedLocationString.c_str());
                                        SohImGui::needs_save = true;
                                    }
                                    ImGui::SameLine();
                                    ImGui::Text(rcObject.rcShortName.c_str());
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                }
                ImGui::EndChild();

                // COLUMN 2 - EXCLUDED LOCATIONS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;

                ImGui::BeginChild("ChildExcludedLocations", ImVec2(0, -8));
                for (auto [rcArea, rcObjects] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
                    bool hasItems = false;
                    for (auto [randomizerCheck, rcObject] : rcObjects) {
                        if (rcObject.visibleInImgui && excludedLocations.count(rcObject.rc)) {
                            hasItems = true;
                            break;
                        }
                    }

                    if (hasItems) {
                        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                        if (ImGui::TreeNode(RandomizerCheckObjects::GetRCAreaName(rcArea).c_str())) {
                            for (auto [randomizerCheck, rcObject] : rcObjects) {
                                auto elfound = excludedLocations.find(rcObject.rc);
                                if (rcObject.visibleInImgui && elfound != excludedLocations.end()) {
                                    if (ImGui::ArrowButton(std::to_string(rcObject.rc).c_str(), ImGuiDir_Left)) {
                                        excludedLocations.erase(elfound);
                                        // todo: this efficently when we build out cvar array support
                                        std::string excludedLocationString = "";
                                        for (auto excludedLocationIt : excludedLocations) {
                                            excludedLocationString += std::to_string(excludedLocationIt);
                                            excludedLocationString += ",";
                                        }
                                        CVar_SetString("gRandomizeExcludedLocations", excludedLocationString.c_str());
                                        SohImGui::needs_save = true;
                                    }
                                    ImGui::SameLine();
                                    ImGui::Text(rcObject.rcShortName.c_str());
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                }
                ImGui::EndChild();

                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        } else {
            locationsTabOpen = false;
        }

        if (ImGui::BeginTabItem("Tricks/Glitches")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoLogic", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(170.0);
                ImGui::Text("Logic Rules");
                InsertHelpHoverText("Glitchless - No glitches are required, but may require some minor tricks.\n"
                                    "\n"
                                    "No logic - Item placement is completely random. MAY BE IMPOSSIBLE TO BEAT.");
                SohImGui::EnhancementCombobox("gRandomizeLogicRules", randoLogicRules, 2, 0);
                ImGui::PopItemWidth();
                ImGui::EndTable();
            }
            if (ImGui::BeginTable("tableRandoTricksGlitches", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Enable Tricks", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Enable Glitches", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - ENABLE TRICKS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::Text("Coming soon");

                // COLUMN 2 - ENABLE GLITCHES
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::Text("Coming soon");

                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Starting Inventory")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (ImGui::BeginTable("tableRandoStartingInventory", 3,
                                  ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Starting Equipment", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Starting Items", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Starting Songs", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::TableHeadersRow();
                ImGui::PopItemFlag();
                ImGui::TableNextRow();

                // COLUMN 1 - STARTING EQUIPMENT
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                SohImGui::EnhancementCheckbox(Settings::StartingKokiriSword.GetName().c_str(),
                                              "gRandomizeStartingKokiriSword");
                PaddedSeparator();
                SohImGui::EnhancementCheckbox(Settings::StartingDekuShield.GetName().c_str(),
                                              "gRandomizeStartingDekuShield");

                // COLUMN 2 - STARTING ITEMS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                SohImGui::EnhancementCheckbox(Settings::StartingOcarina.GetName().c_str(), "gRandomizeStartingOcarina");
                PaddedSeparator();
                SohImGui::EnhancementCheckbox(Settings::StartingConsumables.GetName().c_str(),
                                              "gRandomizeStartingConsumables");
                PaddedSeparator();
                SohImGui::EnhancementCheckbox("Full Wallets", "gRandomizeFullWallets");
                InsertHelpHoverText("Start with a full wallet. All wallet upgrades come filled with rupees.");

                // COLUMN 3 - STARTING SONGS
                ImGui::TableNextColumn();
                window->DC.CurrLineTextBaseOffset = 0.0f;
                ImGui::Text("Coming soon");

                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
    ImGui::End();
}

void CreateGetItemMessages(std::vector<GetItemMessage> messageEntries) {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::getItemMessageTableID);
    for (GetItemMessage messageEntry : messageEntries) {
        if (messageEntry.giid == RG_ICE_TRAP) {
            customMessageManager->CreateMessage(Randomizer::getItemMessageTableID, messageEntry.giid,
                                                { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, messageEntry.english,
                                                  messageEntry.german, messageEntry.french });
        } else {
            customMessageManager->CreateGetItemMessage(Randomizer::getItemMessageTableID, messageEntry.giid,
                                                       messageEntry.iid,
                                                       { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, messageEntry.english,
                                                         messageEntry.german, messageEntry.french });
        }
    }
}

// Currently these are generated at runtime, one for each price between 0-95. We're soon going to migrate this
// to being generated at save load, with only messages specific to each scrub.
void CreateMerchantMessages() {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::merchantMessageTableID);
    customMessageManager->CreateMessage(Randomizer::merchantMessageTableID, 0,
        { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
            "\x12\x38\x82\All right! You win! In return for&sparing me, I will give you a&%gmysterious item%w!&Please, take it!\x07\x10\xA3",
            "\x12\x38\x82\In Ordnung! Du gewinnst! Im Austausch&dafür, dass du mich verschont hast,&werde ich dir einen %gmysteriösen&Gegenstand%w geben! Bitte nimm ihn!\x07\x10\xA3",
            "\x12\x38\x82\D'accord! Vous avez gagné! En échange&de m'épargner, je vous donnerai un &%gobjet mystérieux%w! S'il vous plaît,&prenez-le!\x07\x10\xA3",
        });

    for (u32 price = 5; price <= 95; price += 5) {
        customMessageManager->CreateMessage(Randomizer::merchantMessageTableID, price,
            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
              "\x12\x38\x82\All right! You win! In return for&sparing me, I will sell you a&%gmysterious item%w!&%r" +
                  std::to_string(price) + " Rupees%w it is!\x07\x10\xA3",
            // RANDTODO: Translate the below string to German.
              "\x12\x38\x82\All right! You win! In return for&sparing me, I will sell you a&%gmysterious item%w!&%r" +
                  std::to_string(price) + " Rupees%w it is!\x07\x10\xA3",
              "\x12\x38\x82J'abandonne! Tu veux bien m'acheter&un %gobjet mystérieux%w?&Ça fera %r" +
                  std::to_string(price) + " Rubis%w!\x07\x10\xA3"
            });
    }
    customMessageManager->CreateMessage(
        Randomizer::merchantMessageTableID, TEXT_BEAN_SALESMAN,
        {
            TEXTBOX_TYPE_BLACK,
            TEXTBOX_POS_BOTTOM,
            "I tried to be a %rmagic bean%w salesman,&but it turns out my marketing skills&weren't worth "
            "beans!^Anyway, want to buy my&%gmysterious item%w for 60 Rupees?\x1B&%gYes&No%w",
            "Möchten Sie einen geheimnisvollen&Gegenstand für 60 Rubine?\x1B&%gJa&Nein%w",
            "J'ai essayé d'être un vendeur de&%rharicots magiques%w, mais j'étais&mauvais au niveau du marketing et&ça "
            "me courait sur le haricot...^Enfin bref, ça te dirait de m'acheter un&"
            "%gobjet mystérieux%w pour 60 Rubis?\x1B&%gOui&Non%w",
        });
}

void CreateRupeeMessages() {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::rupeeMessageTableID);
    const std::vector<u8> rupees = { TEXT_BLUE_RUPEE, TEXT_RED_RUPEE, TEXT_PURPLE_RUPEE, TEXT_HUGE_RUPEE };
    std::string rupeeText = " ";
    for (u8 rupee : rupees) {
        switch (rupee) {
            case TEXT_BLUE_RUPEE:
                rupeeText = "\x05\x03 5 #RUPEE#\x05\x00";
                break;
            case TEXT_RED_RUPEE:
                rupeeText = "\x05\x01 20 #RUPEE#\x05\x00";
                break;
            case TEXT_PURPLE_RUPEE:
                rupeeText = "\x05\x05 50 #RUPEE#\x05\x00";
                break;
            case TEXT_HUGE_RUPEE:
                rupeeText = "\x05\x06 200 #RUPEE#\x05\x00";
                break;
        }
        customMessageManager->CreateMessage(Randomizer::rupeeMessageTableID, rupee,
            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
                "You found" + rupeeText + " !",
                "Du hast" + rupeeText + "  gefunden!",
                "Vous obtenez" + rupeeText + " !"
        });
    }
}

std::string Randomizer::RandomizeRupeeName(std::string message, int language) {
    int randomIndex;
    std::string replaceWith;
    switch (language) {
        case LANGUAGE_ENG:
            randomIndex = rand() % (sizeof(englishRupeeNames) / sizeof(englishRupeeNames[0]));
            replaceWith = englishRupeeNames[randomIndex];
            break;
        case LANGUAGE_GER:
            randomIndex = rand() % (sizeof(germanRupeeNames) / sizeof(germanRupeeNames[0]));
            replaceWith = germanRupeeNames[randomIndex];
            break;
        case LANGUAGE_FRA:
            randomIndex = rand() % (sizeof(frenchRupeeNames) / sizeof(frenchRupeeNames[0]));
            replaceWith = frenchRupeeNames[randomIndex];
            break;
    }
    std::string replaceString = "#RUPEE#";
    size_t pos = message.find(replaceString);
    size_t len = replaceString.length();
    message.replace(pos, len, replaceWith);
    CustomMessageManager::Instance->FormatCustomMessage(message);
    return message;
}

CustomMessageEntry Randomizer::GetRupeeMessage(u16 rupeeTextId) {
    CustomMessageEntry messageEntry =
        CustomMessageManager::Instance->RetrieveMessage(Randomizer::rupeeMessageTableID, rupeeTextId);
    messageEntry.english = Randomizer::RandomizeRupeeName(messageEntry.english, LANGUAGE_ENG);
    messageEntry.german = Randomizer::RandomizeRupeeName(messageEntry.german, LANGUAGE_GER);
    messageEntry.french = Randomizer::RandomizeRupeeName(messageEntry.french, LANGUAGE_FRA);
    return messageEntry;
}

CustomMessageMinimal NaviMessages[NUM_NAVI_MESSAGES] = { 
    
    { "%cMissing a small key in a dungeon?&Maybe the %rboss %chas it!", 
      "%cFehlt dir ein kleiner Schlüssel in &einem Labyrinth? Vielleicht hat ihn&ja der %rEndgegner%c!", 
      "%cIl te manque une %wPetite Clé %cdans&un donjon? C'est peut-être le %rboss&%cqui l'a!" }, 

    { "%cSometimes you can use the %rMegaton&Hammer %cinstead of bombs!", 
      "%cManchmal kannst du den %rStahlhammer&%cstatt Bomben verwenden!",
      "%cParfois, tu peux utiliser la %rMasse&des Titans %cau lieu de tes bombes!" }, 

    { "%cThere are three %gbusiness scrubs %cin &Hyrule who sell %wmysterious items%c. Do&you know where they are?",
      "%cEs gibt drei %gDeku-Händler %cin Hyrule&die mysteriöse Gegenstände&verkaufen. Weißt du wo Sie sind?",
      "%cIl y a trois %gPestes Marchandes%c en&Hyrule qui vendent des %wobjets&mystérieux%c. Tu sais où elles sont?" },

    { "%cStuck on this seed? You could &throw in the towel and check the&%wspoiler log%c...",
      "%cHängst du bei diesem Seed fest?&Du könntest die Flinte ins Korn&werfen und ins %wSpoiler Log %cschauen...",
      "%cSi tu es coincé sur cette seed,&tu peux toujours jeter l'éponge&et regader le %wSpoiler log%c..." },

    { "%cDid you know that the %yHover&Boots %ccan be used to cross&%wquicksand%c?", 
      "%cWusstest du, dass du mit den&%yGleitstiefeln %cTreibsand %wüberqueren&kannst%c?",
      "%cEst-ce que tu savais que les %rBottes&des airs %cpouvaient être utilisées&pour traverser les %wsables mouvants%c?" },

    { "%cYou can reach the back of %wImpa's&House %cby jumping from the&unfinished house with a %rcucco%c!", 
      "%cDu kannst den Balkon von %wImpas&Haus %cerreichen indem du von&der Baustelle aus mit einem %rHuhn&%cspringst!",
      "%cTu peux atteindre l'arrière de la&%wMaison d'Impa %cen sautant depuis la&maison en construction avec une&%rcocotte%c!" },

    { "%cThe %ySpirit Trial %cin %pGanon's Castle&%chas a %whidden chest %csomewhere.&Did you already know that?", 
      "%cDie %yGeister-Prüfung %cin %pGanons&Schloss %chat irgendwo eine&%wversteckte Kiste%c. Weißt du schon&wo?",
      "%cL'%yÉpreuve de l'Esprit%c dans le %pChâteau&de Ganon %ca un coffre caché quelque&part. Je suppose que tu le savais&déjà?" },

    { "%cYou know the %wBombchu Bowling&Alley%c? I heard %wonly two of the &prizes %care worthwhile. The rest &is junk!", 
      "%cKennst du die %wMinenbowlingbahn%c?&Ich habe gehört dass sich nur &%wzwei der Preise%c lohnen. Der Rest&ist Krimskrams!",
      "%cEst-ce que tu savais qu'au %wBowling&Teigneux%c, il n'y a que les %wdeux&premiers prix %cqui sont intéréssant?" },

    { "%cHave you been using %wDeku Nuts&%cenough? I've seen them blow up&a %rBombchu%c!",
      "%cBenutzt du auch genügend %wDeku&Nüsse%c? Ich habe mal gesehen dass&man damit %rKrabbelminen %cdetonieren&kann!",
      "%cTu es sûr d'utiliser tes %wNoix Mojo &%ccorrectement? J'en ai déjà vu&exploser des %rChoux-Péteurs%c!" },

    { "%cYou might end up with an %wextra&key %cfor the %bWater Temple %cor the&%rFire Temple%c. It's for your safety!", 
      "%cVielleicht verbleibt dir im&%bWassertempel %coder %rFeuertempel %cein&%wzusätzlicher Schlüssel%c. Dies&ist zu deiner Sicherheit!",
      "%cIl se peut que tu aies une %wPetite&Clé %cen trop dans le %bTemple de l'Eau&%cou le %rTemple du Feu%c. C'est pour ta&propre sécurité!" },

    { "%cIf you can't pick up a %rbomb&flower %cwith your hands, you can&still detonate it with %rfire %cor&with %warrows%c!", 
      "%cNur weil du eine %rDonnerblume&%cnicht hochheben kannst, so kannst&du sie immernoch mit %rFeuer %coder&%wPfeilen %cdetonieren!",
      "%cSi tu ne peux pas ramasser&un %rChoux-Péteur %cavec tes mains, tu&peux toujours le faire exploser&avec du %rFeu %cou avec des %wflèches%c!" },

    { "%cEven an adult can't push large&blocks without some %wextra&strength%c!", 
      "%cSelbst ein Erwachsener kann ohne&etwas %wzusätzliche Kraft %ckeine&großen Blöcke verschieben!",
      "%cMême un adulte ne peut pas pousser&de grands blocs sans un petit %wgain&de force%c!" },

    { "%cI've heard that %rFlare Dancer&%cis weak to the %wMaster Sword%c!&Have you tried it?", 
      "%cIch habe gehört dass der&%rFlammenderwisch %ceine Schwäche für&das %wMasterschwert %caufweist. Hast du&es schonmal versucht einzusetzen?",
      "%cJ'ai entendu dire que les %rDanse-&Flammes %csont faîbles face à l'%wÉpée de&Légende%c! Est-ce que tu as essayé?" },

    { "%cDon't have a weapon to kill a&%rspider%c? Try throwing a %wpot&%cat it!", 
      "%cFehlt dir die Waffe um gegen&eine %rSkulltula %czu kämpfen? Versuch&Sie mit einem %wKrug %cabzuwerfen!",
      "%cSi tu n'as pas d'arme pour tuer&une %raraignée%c, pourquoi n'essayerais&-tu pas de lui jetter une %wjarre&%cà la figure?" },

    { "%cI hear the patch of %wsoft soil&%cin %bZora's River %cis the only one&that isn't home to a %rspider%c!", 
      "%cIch habe gehört dass die Stelle&%wfeuchten Bodens %cim %bZora-Fluss %cals&einzige keine %rSkulltula %cbeherbergt.",
      "%cJ'ai entendu dire que la %wterre meuble&%cqui se trouve à la %bRivière Zora %cest&la seule qui ne contienne pas&d'%raraignée%c." },
};

void CreateNaviRandoMessages() {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::NaviRandoMessageTableID);
    for (u8 i = 0; i <= (NUM_NAVI_MESSAGES - 1); i++) {
        customMessageManager->CreateMessage(Randomizer::NaviRandoMessageTableID, i,
                                            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, NaviMessages[i].english,
                                              NaviMessages[i].german, NaviMessages[i].french });
    }
}

void Randomizer::CreateCustomMessages() {
    // RANDTODO: Translate into french and german and replace GIMESSAGE_UNTRANSLATED
    // with GIMESSAGE(getItemID, itemID, english, german, french).
    const std::vector<GetItemMessage> getItemMessages = {
        GIMESSAGE(RG_ICE_TRAP, ITEM_NONE, "\x08\x06\x30You are a %bFOWL%w!\x0E\x20",
                  "\x08\x06\x15 Du bist ein %bDUMMKOPF%w!\x0E\x20", "\x08\x06\x50%bIDIOT%w\x0E\x20"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_BLUE_FIRE, ITEM_BLUE_FIRE, "You got a %rBottle with Blue &Fire%w! Use it to melt Red Ice!",
            "Vous obtenez une %rBouteille avec&une Flamme Bleue%w! Utilisez-la&pour faire fondre la %rGlace&Rouge%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_BIG_POE, ITEM_BIG_POE,
                            "You got a %rBig Poe in a Bottle%w!&Sell it to the Ghost Shop!",
                            "Vous obtenez une %rBouteille avec&une Âme%w! Vendez-la au Marchand&d'Âme"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_BLUE_POTION, ITEM_POTION_BLUE,
                            "You got a %rBottle of Blue Potion%w!&Drink it to replenish your&%ghealth%w and %bmagic%w!",
                            "Vous obtenez une %rBouteille avec&une Potion Bleue%w! Buvez-la pour&restaurer votre "
                            "%rénergie vitale%w&ainsi que votre %gmagie%w!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_FISH, ITEM_FISH,
            "You got a %rFish in a Bottle%w!&It looks fresh and delicious!&They say Jabu-Jabu loves them!",
            "Vous obtenez une %rBouteille avec&un Poisson%w! Il a l'air délicieux!&Il paraîtrait que %bJabu-Jabu "
            "%wen&serait friand!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_BUGS, ITEM_BUG, "You got a %rBug in a Bottle%w!&They love to burrow in&dirt holes!",
            "Vous obtenez une %rBouteille avec&des Insectes%w! Ils adorent creuser&dans la terre meuble!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_FAIRY, ITEM_FAIRY, "You got a %rFairy in a Bottle%w!&Use it wisely!",
                            "Vous obtenez une %rBouteille avec&une Fée%w! Faites-en bon usage!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_RED_POTION, ITEM_POTION_RED,
            "You got a %rBottle of Red Potion%w!&Drink it to replenish your&%ghealth%w!",
            "Vous obtenez une %rBouteille avec&une Potion Rouge%w! Buvez-la pour&restaurer votre %rénergie vitale%w!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_GREEN_POTION, ITEM_POTION_GREEN,
            "You got a %rBottle of Green Potion%w!&Drink it to replenish your&%bmagic%w!",
            "Vous obtenez une %rBouteille avec&une Potion Verte%w! Buvez-la pour&restaurer votre %gmagie%w!"),
        GIMESSAGE_NO_GERMAN(
            RG_BOTTLE_WITH_POE, ITEM_POE,
            "You got a %rPoe in a Bottle%w!&That creepy Ghost Shop might&be interested in this...",
            "Vous obtenez une %rBouteille avec&un Esprit%w! Ça intéresserait&peut-être le vendeur d'Âme "),

        GIMESSAGE_NO_GERMAN(RG_GERUDO_FORTRESS_SMALL_KEY, ITEM_KEY_SMALL, "You found a %yThieves Hideout &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %yRepaire des Voleurs%w!"),
        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %gForest Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %rFire Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %bWater Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %ySpirit Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %pShadow Temple &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTOM_OF_THE_WELL_SMALL_KEY, ITEM_KEY_SMALL,
                            "You found a %pBottom of the &Well %wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %Puits%w!"),
        GIMESSAGE_NO_GERMAN(RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, ITEM_KEY_SMALL,
                            "You found a %yGerudo Training &Grounds %wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %yGymnase Gerudo%w!"),
        GIMESSAGE_NO_GERMAN(RG_GANONS_CASTLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %rGanon's Castle &%wSmall Key!",
                            "Vous obtenez une %rPetite Clé %w&du %Château de Ganon%w!"),

        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %gForest Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %rFire Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %bWater Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %ySpirit Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %pShadow Temple &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_GANONS_CASTLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %rGanon's Castle &%wBoss Key!",
                            "Vous obtenez la %rClé d'or %wdu&%rChâteau de Ganon%w!"),

        GIMESSAGE_NO_GERMAN(RG_DEKU_TREE_MAP, ITEM_DUNGEON_MAP, "You found the %gDeku Tree &%wMap!",
                            "Vous obtenez la %rCarte %wde&l'%gArbre Mojo%w!"),
        GIMESSAGE_NO_GERMAN(RG_DODONGOS_CAVERN_MAP, ITEM_DUNGEON_MAP, "You found the %rDodongo's Cavern &%wMap!",
                            "Vous obtenez la %rCarte %wde la&%rCaverne Dodongo%w!"),
        GIMESSAGE_NO_GERMAN(RG_JABU_JABUS_BELLY_MAP, ITEM_DUNGEON_MAP, "You found the %bJabu Jabu's Belly &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%bVentre de Jabu-Jabu%w!"),
        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %gForest Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %rFire Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %bWater Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %ySpirit Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %pShadow Temple &%wMap!",
                            "Vous obtenez la %rCarte %wdu &%pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTOM_OF_THE_WELL_MAP, ITEM_DUNGEON_MAP, "You found the %pBottom of the &Well %wMap!",
                            "Vous obtenez la %rCarte %wdu &%pPuits%w!"),
        GIMESSAGE_NO_GERMAN(RG_ICE_CAVERN_MAP, ITEM_DUNGEON_MAP, "You found the %cIce Cavern &%wMap!",
                            "Vous obtenez la %rCarte %wde &la %cCaverne Polaire%w!"),

        GIMESSAGE_NO_GERMAN(RG_DEKU_TREE_COMPASS, ITEM_COMPASS, "You found the %gDeku Tree &%wCompass!",
                            "Vous obtenez la %rBoussole %wde&l'%gArbre Mojo%w!"),
        GIMESSAGE_NO_GERMAN(RG_DODONGOS_CAVERN_COMPASS, ITEM_COMPASS, "You found the %rDodongo's Cavern &%wCompass!",
                            "Vous obtenez la %rBoussole %wde la&%rCaverne Dodongo%w!"),
        GIMESSAGE_NO_GERMAN(RG_JABU_JABUS_BELLY_COMPASS, ITEM_COMPASS, "You found the %bJabu Jabu's Belly &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%bVentre de Jabu-Jabu%w!"),
        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %gForest Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %rFire Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %bWater Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %ySpirit Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %pShadow Temple &%wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTOM_OF_THE_WELL_COMPASS, ITEM_COMPASS,
                            "You found the %pBottom of the &Well %wCompass!",
                            "Vous obtenez la %rBoussole %wdu &%pPuits%w!"),
        GIMESSAGE_NO_GERMAN(RG_ICE_CAVERN_COMPASS, ITEM_COMPASS, "You found the %cIce Cavern &%wCompass!",
                            "Vous obtenez la %rBoussole %wde &la %cCaverne Polaire%w!"),
        GIMESSAGE(RG_MAGIC_BEAN_PACK, ITEM_BEAN,
                  "You got a %rPack of Magic Beans%w!&Find a suitable spot for a garden&and plant them. Then, wait for&something fun to happen!",
                  "Du hast eine %rPackung&Magic Beans%w! Finde&einen geeigneten Platz fur einen&Garten und pflanze sie. Dann^warte auf etwas Lustiges passiert!",
                  "Vous avez un %rPack de&haricots magiques%w ! Trouvez&un endroit convenable pour un&jardin et plantez-les.^Ensuite, attendez quelque&chose d'amusant doit arriver !")
    };
    CreateGetItemMessages(getItemMessages);
    CreateMerchantMessages();
    CreateRupeeMessages();
    CreateNaviRandoMessages();
}

class ExtendedVanillaTableInvalidItemIdException: public std::exception {
    private:
    s16 itemID;

    public:
      ExtendedVanillaTableInvalidItemIdException(s16 itemID): itemID(itemID) {}
      std::string what() {
        return itemID + " is not a valid ItemID for the extendedVanillaGetItemTable. If you are adding a new"
        "item, try adding it to randoGetItemTable instead.";
      }
};

void InitRandoItemTable() {
    // These entries have ItemIDs from vanilla, but not GetItemIDs or entries in the old sGetItemTable
    GetItemEntry extendedVanillaGetItemTable[] = {
        GET_ITEM(ITEM_MEDALLION_LIGHT, OBJECT_GI_MEDAL, GID_MEDALLION_LIGHT, 0x40, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_LIGHT_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_FOREST, OBJECT_GI_MEDAL, GID_MEDALLION_FOREST, 0x3E, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_FOREST_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_FIRE, OBJECT_GI_MEDAL, GID_MEDALLION_FIRE, 0x3C, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_FIRE_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_WATER, OBJECT_GI_MEDAL, GID_MEDALLION_WATER, 0x3D, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_WATER_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_SHADOW, OBJECT_GI_MEDAL, GID_MEDALLION_SHADOW, 0x41, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SHADOW_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_SPIRIT, OBJECT_GI_MEDAL, GID_MEDALLION_SPIRIT, 0x3F, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SPIRIT_MEDALLION),

        GET_ITEM(ITEM_KOKIRI_EMERALD, OBJECT_GI_JEWEL, GID_KOKIRI_EMERALD, 0x80, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_KOKIRI_EMERALD),
        GET_ITEM(ITEM_GORON_RUBY, OBJECT_GI_JEWEL, GID_GORON_RUBY, 0x81, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_GORON_RUBY),
        GET_ITEM(ITEM_ZORA_SAPPHIRE, OBJECT_GI_JEWEL, GID_ZORA_SAPPHIRE, 0x82, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_ZORA_SAPPHIRE),

        GET_ITEM(ITEM_SONG_LULLABY, OBJECT_GI_MELODY, GID_SONG_ZELDA, 0xD4, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_ZELDAS_LULLABY),
        GET_ITEM(ITEM_SONG_SUN, OBJECT_GI_MELODY, GID_SONG_SUN, 0xD3, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SUNS_SONG),
        GET_ITEM(ITEM_SONG_EPONA, OBJECT_GI_MELODY, GID_SONG_EPONA, 0xD2, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_EPONAS_SONG),
        GET_ITEM(ITEM_SONG_STORMS, OBJECT_GI_MELODY, GID_SONG_STORM, 0xD6, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SONG_OF_STORMS),
        GET_ITEM(ITEM_SONG_TIME, OBJECT_GI_MELODY, GID_SONG_TIME, 0xD5, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SONG_OF_TIME),
        GET_ITEM(ITEM_SONG_SARIA, OBJECT_GI_MELODY, GID_SONG_SARIA, 0xD1, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SARIAS_SONG),

        GET_ITEM(ITEM_SONG_MINUET, OBJECT_GI_MELODY, GID_SONG_MINUET, 0x73, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_MINUET_OF_FOREST),
        GET_ITEM(ITEM_SONG_BOLERO, OBJECT_GI_MELODY, GID_SONG_BOLERO, 0x74, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_BOLERO_OF_FIRE),
        GET_ITEM(ITEM_SONG_SERENADE, OBJECT_GI_MELODY, GID_SONG_SERENADE, 0x75, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SERENADE_OF_WATER),
        GET_ITEM(ITEM_SONG_NOCTURNE, OBJECT_GI_MELODY, GID_SONG_NOCTURNE, 0x77, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_NOCTURNE_OF_SHADOW),
        GET_ITEM(ITEM_SONG_REQUIEM, OBJECT_GI_MELODY, GID_SONG_REQUIEM, 0x76, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_REQUIEM_OF_SPIRIT),
        GET_ITEM(ITEM_SONG_PRELUDE, OBJECT_GI_MELODY, GID_SONG_PRELUDE, 0x78, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_PRELUDE_OF_LIGHT),
    };

    // These do not have ItemIDs or GetItemIDs from vanilla, so I'm using their
    // RandomizerGet enum values for both.
    GetItemEntry randoGetItemTable[] = {
        GET_ITEM(RG_ICE_TRAP, OBJECT_GI_RUPY, GID_RUPEE_GOLD, 0, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_ICE_TRAP),
        GET_ITEM(RG_MAGIC_SINGLE, OBJECT_GI_MAGICPOT, GID_MAGIC_SMALL, 0xE4, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_MAGIC_SINGLE),
        GET_ITEM(RG_MAGIC_DOUBLE, OBJECT_GI_MAGICPOT, GID_MAGIC_LARGE, 0xE8, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_MAGIC_DOUBLE),
        GET_ITEM(RG_DOUBLE_DEFENSE, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xE9, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DOUBLE_DEFENSE),
        GET_ITEM(RG_BOTTLE_WITH_RED_POTION, OBJECT_GI_LIQUID, GID_POTION_RED, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_RED_POTION),
        GET_ITEM(RG_BOTTLE_WITH_GREEN_POTION, OBJECT_GI_LIQUID, GID_POTION_GREEN, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_GREEN_POTION),
        GET_ITEM(RG_BOTTLE_WITH_BLUE_POTION, OBJECT_GI_LIQUID, GID_POTION_BLUE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BLUE_POTION),
        GET_ITEM(RG_BOTTLE_WITH_FAIRY, OBJECT_GI_BOTTLE, GID_BOTTLE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_FAIRY),
        GET_ITEM(RG_BOTTLE_WITH_FISH, OBJECT_GI_FISH, GID_FISH, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_FISH),
        GET_ITEM(RG_BOTTLE_WITH_BLUE_FIRE, OBJECT_GI_FIRE, GID_BLUE_FIRE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BLUE_FIRE),
        GET_ITEM(RG_BOTTLE_WITH_BUGS, OBJECT_GI_INSECT, GID_BUG, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BUGS),
        GET_ITEM(RG_BOTTLE_WITH_POE, OBJECT_GI_GHOST, GID_POE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_POE),
        GET_ITEM(RG_BOTTLE_WITH_BIG_POE, OBJECT_GI_GHOST, GID_BIG_POE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BIG_POE),
        GET_ITEM(RG_GERUDO_FORTRESS_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_GERUDO_FORTRESS_SMALL_KEY),
        GET_ITEM(RG_FOREST_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_FOREST_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_FIRE_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_FIRE_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_WATER_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_WATER_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_SPIRIT_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_SHADOW_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_SMALL_KEY),
        GET_ITEM(RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY),
        GET_ITEM(RG_GANONS_CASTLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_GANONS_CASTLE_SMALL_KEY),
        GET_ITEM(RG_FOREST_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FOREST_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_FIRE_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FIRE_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_WATER_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_WATER_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_SPIRIT_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_SHADOW_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_GANONS_CASTLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_GANONS_CASTLE_BOSS_KEY),
        GET_ITEM(RG_DEKU_TREE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DEKU_TREE_MAP),
        GET_ITEM(RG_DODONGOS_CAVERN_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DODONGOS_CAVERN_MAP),
        GET_ITEM(RG_JABU_JABUS_BELLY_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_JABU_JABUS_BELLY_MAP),
        GET_ITEM(RG_FOREST_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FOREST_TEMPLE_MAP),
        GET_ITEM(RG_FIRE_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FIRE_TEMPLE_MAP),
        GET_ITEM(RG_WATER_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_WATER_TEMPLE_MAP),
        GET_ITEM(RG_SPIRIT_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_MAP),
        GET_ITEM(RG_SHADOW_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_MAP),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_MAP),
        GET_ITEM(RG_ICE_CAVERN_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_ICE_CAVERN_MAP),
        GET_ITEM(RG_DEKU_TREE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DEKU_TREE_COMPASS),
        GET_ITEM(RG_DODONGOS_CAVERN_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DODONGOS_CAVERN_COMPASS),
        GET_ITEM(RG_JABU_JABUS_BELLY_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_JABU_JABUS_BELLY_COMPASS),
        GET_ITEM(RG_FOREST_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FOREST_TEMPLE_COMPASS),
        GET_ITEM(RG_FIRE_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FIRE_TEMPLE_COMPASS),
        GET_ITEM(RG_WATER_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_WATER_TEMPLE_COMPASS),
        GET_ITEM(RG_SPIRIT_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_COMPASS),
        GET_ITEM(RG_SHADOW_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_COMPASS),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_COMPASS),
        GET_ITEM(RG_ICE_CAVERN_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_ICE_CAVERN_COMPASS),
        GET_ITEM(RG_MAGIC_BEAN_PACK, OBJECT_GI_BEAN, GID_BEAN, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_MAGIC_BEAN_PACK)
    };
    ItemTableManager::Instance->AddItemTable(MOD_RANDOMIZER);
    for (int i = 0; i < ARRAY_COUNT(extendedVanillaGetItemTable); i++) {
        ItemTableManager::Instance->AddItemEntry(MOD_RANDOMIZER, extendedVanillaGetItemTable[i].getItemId, extendedVanillaGetItemTable[i]);
    }
    for (int i = 0; i < ARRAY_COUNT(randoGetItemTable); i++) {
        if (randoGetItemTable[i].itemId >= RG_FOREST_TEMPLE_SMALL_KEY && randoGetItemTable[i].itemId <= RG_GANONS_CASTLE_SMALL_KEY
            && randoGetItemTable[i].itemId != RG_GERUDO_FORTRESS_SMALL_KEY) {
            randoGetItemTable[i].drawFunc = (CustomDrawFunc)Randomizer_DrawSmallKey;
        } else if (randoGetItemTable[i].itemId >= RG_FOREST_TEMPLE_BOSS_KEY && randoGetItemTable[i].itemId <= RG_GANONS_CASTLE_BOSS_KEY) {
            randoGetItemTable[i].drawFunc = (CustomDrawFunc)Randomizer_DrawBossKey;
        } else if (randoGetItemTable[i].itemId == RG_DOUBLE_DEFENSE) {
            randoGetItemTable[i].drawFunc = (CustomDrawFunc)Randomizer_DrawDoubleDefense;
        }
        ItemTableManager::Instance->AddItemEntry(MOD_RANDOMIZER, randoGetItemTable[i].itemId, randoGetItemTable[i]);
    }
}


void InitRando() {
    SohImGui::AddWindow("Randomizer", "Randomizer Settings", DrawRandoEditor);
    Randomizer::CreateCustomMessages();
    InitRandoItemTable();
}

extern "C" {

void Rando_Init(void) {
    InitRando();
}

}
