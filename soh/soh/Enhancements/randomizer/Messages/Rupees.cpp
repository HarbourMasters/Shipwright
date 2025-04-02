/**
 * This file is for handling the Randomize Rupee Names
 * enhancement
 */
#include <soh/OTRGlobals.h>

extern "C" {
#include "variables.h"
}

static const char* englishRupeeNames[175] = { "[P]",
                                              "Bad RNG Rolls",
                                              "Bananas",
                                              "Beanbean Coins",
                                              "Beans",
                                              "Beli",
                                              "Bells",
                                              "Berries",
                                              "Bison Dollars",
                                              "Bitcoin",
                                              "Blue Essence",
                                              "Bolts",
                                              "Bones",
                                              "Boondollars",
                                              "Bottle Caps",
                                              "Bratwürste",
                                              "Bucks",
                                              "BugFrags",
                                              "Canadian Dollars",
                                              "Cards",
                                              "Chaos Orbs",
                                              "Clams",
                                              "Coal",
                                              "Cocoa Beans",
                                              "Coins",
                                              "Cookies",
                                              "Copper",
                                              "Cor",
                                              "Cornflakes",
                                              "Credits",
                                              "Crimebucks",
                                              "Crystal Shards",
                                              "Cubits",
                                              "Cucumbers",
                                              "Dalmations",
                                              "Dampécoin",
                                              "Dark Elixir",
                                              "Darseks",
                                              "Dead Memes",
                                              "Diamonds",
                                              "DNA",
                                              "Doge",
                                              "Dogecoin",
                                              "Doll Hairs",
                                              "Dollars",
                                              "Dollarydoos",
                                              "Dosh",
                                              "Doubloons",
                                              "Dwarfbucks",
                                              "Elexit",
                                              "Emeralds",
                                              "Energon",
                                              "Eris",
                                              "Ether",
                                              "Euro",
                                              "Experience",
                                              "Extinction Points",
                                              "Floopies",
                                              "Flurbos",
                                              "FPS",
                                              "Friends",
                                              "Frog Coins",
                                              "Gald",
                                              "Gekz",
                                              "Gems",
                                              "Geo",
                                              "Gil",
                                              "Glimmer",
                                              "Glitches",
                                              "Gold",
                                              "Gold Dragons",
                                              "Goober Dollars",
                                              "Green Herbs",
                                              "Greg Siblings",
                                              "Grouses",
                                              "Gummybears",
                                              "Hell",
                                              "Hyrule Loaches",
                                              "Ice Traps",
                                              "ISK",
                                              "Jiggies",
                                              "KF7 Ammo",
                                              "Kinstones",
                                              "Kremcoins",
                                              "Kroner",
                                              "Leaves",
                                              "Lemmings",
                                              "Lien",
                                              "Lira",
                                              "Lumber",
                                              "Lungmen Dollars",
                                              "Macca",
                                              "Mana",
                                              "Mann Co. Keys",
                                              "Meat",
                                              "Meat Stacks",
                                              "Medaparts",
                                              "Meseta",
                                              "Mesetas",
                                              "Minerals",
                                              "Monopoly Money",
                                              "Moons",
                                              "Mora",
                                              "Mumbo Tokens",
                                              "Munny",
                                              "Mushrooms",
                                              "Mysteries",
                                              "Neopoints",
                                              "Notes",
                                              "Nuyen",
                                              "Orbs",
                                              "Ore",
                                              "Pix",
                                              "Pixels",
                                              "Plastyks",
                                              "Platinum",
                                              "Pokédollars",
                                              "Pokémon",
                                              "Poko",
                                              "Pokos",
                                              "Potch",
                                              "Pounds",
                                              "Power Pellets",
                                              "Primogems",
                                              "Réals",
                                              "Refined Metal",
                                              "Remote Mines",
                                              "Retweets",
                                              "Rhinu",
                                              "Rings",
                                              "Riot Points",
                                              "Robux",
                                              "Rubies",
                                              "Rubles",
                                              "Runite Ore",
                                              "Rupees",
                                              "Saint Quartz",
                                              "Septims",
                                              "Shekels",
                                              "Shillings",
                                              "Silver",
                                              "Simoleons",
                                              "Smackaroos",
                                              "Social Credit",
                                              "Souls",
                                              "Spent Casings",
                                              "Spice",
                                              "Spondulicks",
                                              "Spoons",
                                              "Star Bits",
                                              "Star Chips",
                                              "Stars",
                                              "Stones of Jordan",
                                              "Store Credit",
                                              "Strawbs",
                                              "Studs",
                                              "Super Sea Snails",
                                              "Talent",
                                              "Teef",
                                              "Telecrystals",
                                              "Tiberium",
                                              "TokKul",
                                              "Toys",
                                              "Turnips",
                                              "Upvotes",
                                              "V-Bucks",
                                              "Vespene Gas",
                                              "Watts",
                                              "Widgets",
                                              "Woolongs",
                                              "World Dollars",
                                              "Wumpa Fruit",
                                              "Yen",
                                              "Zenny",
                                              "Zorkmids" };

static const char* germanRupeeNames[65] = {
    "Bananen",    "Bitcoin",  "Bonbons",    "Bratwürste", "Brause UFOs", "Brötchen",      "Cent",    "Diamanten",
    "Diridari",   "Dogecoin", "ECU",        "Elexit",     "Erz",         "Erzbrocken",    "Euro",    "EXP",
    "Forint",     "Franken",  "Freunde",    "Gil",        "Gold",        "Groschen",      "Gulden",  "Gummibären",
    "Heller",     "Juwelen",  "Karolin",    "Kartoffeln", "Kies",        "Knete",         "Knochen", "Kohle",
    "Kraniche",   "Kreuzer",  "Kronen",     "Kronkorken", "Kröten",      "Mark",          "Mäuse",   "Monde",
    "Moorhühner", "Moos",     "Münzen",     "Penunze",    "Pesa",        "Pfandflaschen", "Pfennig", "Pfund",
    "Pilze",      "Plastiks", "Pokédollar", "Radieschen", "Rappen",      "Rubine",        "Saphire", "Schilling",
    "Seelen",     "Smaragde", "Steine",     "Sterne",     "Sternis",     "Tael",          "Taler",   "Wagenchips",
    "Zenny"
};

static const char* frenchRupeeNames[40] = {
    "Anneaux",   "Baguettes",   "Balles",     "Bananes",     "Bitcoin",    "Blés",     "Bling",    "Capsules",
    "Centimes",  "Champignons", "Clochettes", "Crédits",     "Croissants", "Diamants", "Dogecoin", "Dollars",
    "Émeraudes", "Éthers",      "Étoiles",    "Euros",       "Florens",    "Francs",   "Galds",    "Gils",
    "Grouses",   "Halos",       "Joyaux",     "Lunes",       "Mailles",    "Munnies",  "Orbes",    "Orens",
    "Pépètes",   "Pièces",      "Plastyks",   "Pokédollars", "Pokémon",    "Radis",    "Rubis",    "Zennies"
};

void BuildRupeeMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage(
        "You found [[color]][[amount]] [[rupee]]\x05\x00!", "Du hast [[color]][[amount]] [[rupee]]\x05\x00 gefunden!",
        "Vous obtenez [[color]][[amount]] [[rupee]]\x05\x00!", TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM);
    std::string color;
    std::string amount;
    CustomMessage rupee = CustomMessage(RandomElement(englishRupeeNames), RandomElement(germanRupeeNames),
                                        RandomElement(frenchRupeeNames));
    switch (*textId) {
        case TEXT_BLUE_RUPEE:
            color = "\x05\x03";
            amount = "5";
            break;
        case TEXT_RED_RUPEE:
            color = "\x05\x01";
            amount = "20";
            break;
        case TEXT_PURPLE_RUPEE:
            color = "\x05\x05";
            amount = "50";
            break;
        case TEXT_HUGE_RUPEE:
            color = "\x05\x06";
            amount = "200";
            break;
        default:
            assert(!"This should not be reachable");
            return;
    }
    msg.Replace("[[color]]", color);
    msg.Replace("[[amount]]", amount);
    msg.Replace("[[rupee]]", rupee);
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterRandomRupeeNames() {
    COND_ID_HOOK(OnOpenText, TEXT_BLUE_RUPEE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), 1), BuildRupeeMessage);
    COND_ID_HOOK(OnOpenText, TEXT_RED_RUPEE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), 1), BuildRupeeMessage);
    COND_ID_HOOK(OnOpenText, TEXT_PURPLE_RUPEE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), 1), BuildRupeeMessage);
    COND_ID_HOOK(OnOpenText, TEXT_HUGE_RUPEE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), 1), BuildRupeeMessage);
}

static RegisterShipInitFunc initFunc(RegisterRandomRupeeNames, { CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames") });