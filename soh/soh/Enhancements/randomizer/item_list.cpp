#include "static_data.h"
#include "3drando/logic.hpp"
#include "z64object.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "draw.h"

using namespace Rando;

std::array<Item, RG_MAX> StaticData::itemTable;

void StaticData::InitItemTable() {
    itemTable[RG_NONE] =
        Item(RG_NONE, Text{ "No Item", "Rien", "Kein Artikel" }, ITEMTYPE_EVENT, GI_RUPEE_GREEN, false,
                  &Logic::noVariable, RHT_NONE, ITEM_NONE, 0, 0, 0, 0, 0, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_KOKIRI_SWORD] =
        Item(RG_KOKIRI_SWORD, Text{ "Kokiri Sword", "Épée Kokiri", "Kokiri-Schwert" }, ITEMTYPE_ITEM,
                  GI_SWORD_KOKIRI, true, &Logic::KokiriSword, RHT_KOKIRI_SWORD, ITEM_SWORD_KOKIRI, OBJECT_GI_SWORD_1,
                  GID_SWORD_KOKIRI, 0xA4, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE, GI_SWORD_KNIFE);
    itemTable[RG_GIANTS_KNIFE] =
        Item(RG_GIANTS_KNIFE, Text{ "Giant's Knife", "Lame des Géants", "Messer des Riesen" }, ITEMTYPE_ITEM,
                  GI_SWORD_KNIFE, true, &Logic::KokiriSword, RHT_GIANTS_KNIFE, ITEM_SWORD_KNIFE, OBJECT_GI_LONGSWORD,
                  GID_SWORD_BGS, 0x4B, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BIGGORON_SWORD] =
        Item(RG_BIGGORON_SWORD, Text{ "Biggoron's Sword", "Épée de Biggoron", "Biggoron-Schwert" }, ITEMTYPE_ITEM,
                  GI_SWORD_BGS, true, &Logic::BiggoronSword, RHT_BIGGORON_SWORD, ITEM_SWORD_BGS, OBJECT_GI_LONGSWORD,
                  GID_SWORD_BGS, 0x0C, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_DEKU_SHIELD] =
        Item(RG_DEKU_SHIELD, Text{ "Deku Shield", "Bouclier Mojo", "Deku-Schild" }, ITEMTYPE_ITEM, GI_SHIELD_DEKU,
                  false, &Logic::noVariable, RHT_DEKU_SHIELD, ITEM_SHIELD_DEKU, OBJECT_GI_SHIELD_1, GID_SHIELD_DEKU,
                  0x4C, 0xA0, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_HYLIAN_SHIELD] =
        Item(RG_HYLIAN_SHIELD, Text{ "Hylian Shield", "Bouclier Hylien", "Hylia-Schild" }, ITEMTYPE_ITEM,
                  GI_SHIELD_HYLIAN, false, &Logic::noVariable, RHT_HYLIAN_SHIELD, ITEM_SHIELD_HYLIAN, OBJECT_GI_SHIELD_2,
                  GID_SHIELD_HYLIAN, 0x4D, 0xA0, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_MIRROR_SHIELD] =
        Item(RG_MIRROR_SHIELD, Text{ "Mirror Shield", "Bouclier Miroir", "Spiegel-Schild" }, ITEMTYPE_ITEM,
                  GI_SHIELD_MIRROR, true, &Logic::MirrorShield, RHT_MIRROR_SHIELD, ITEM_SHIELD_MIRROR,
                  OBJECT_GI_SHIELD_3, GID_SHIELD_MIRROR, 0x4E, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_GORON_TUNIC] =
        Item(RG_GORON_TUNIC, Text{ "Goron Tunic", "Tunique Goron", "Goronen-Tunika" }, ITEMTYPE_ITEM,
                  GI_TUNIC_GORON, true, &Logic::GoronTunic, RHT_GORON_TUNIC, ITEM_TUNIC_GORON, OBJECT_GI_CLOTHES,
                  GID_TUNIC_GORON, 0x50, 0xA0, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_ZORA_TUNIC] =
        Item(RG_ZORA_TUNIC, Text{ "Zora Tunic", "Tunique Zora", "Zora-Tunika" }, ITEMTYPE_ITEM, GI_TUNIC_ZORA,
                  true, &Logic::ZoraTunic, RHT_ZORA_TUNIC, ITEM_TUNIC_ZORA, OBJECT_GI_CLOTHES, GID_TUNIC_ZORA, 0x51,
                  0xA0, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_IRON_BOOTS] =
        Item(RG_IRON_BOOTS, Text{ "Iron Boots", "Bottes de plomb", "Eisenstiefel" }, ITEMTYPE_ITEM, GI_BOOTS_IRON,
                  true, &Logic::IronBoots, RHT_IRON_BOOTS, ITEM_BOOTS_IRON, OBJECT_GI_BOOTS_2, GID_BOOTS_IRON, 0x53,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_HOVER_BOOTS] =
        Item(RG_HOVER_BOOTS, Text{ "Hover Boots", "Bottes de airs", "Gleitstiefel" }, ITEMTYPE_ITEM,
                  GI_BOOTS_HOVER, true, &Logic::HoverBoots, RHT_HOVER_BOOTS, ITEM_BOOTS_HOVER, OBJECT_GI_HOVERBOOTS,
                  GID_BOOTS_HOVER, 0x54, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BOOMERANG] =
        Item(RG_BOOMERANG, Text{ "Boomerang", "Boomerang", "Bumerang" }, ITEMTYPE_ITEM, GI_BOOMERANG, true,
                  &Logic::Boomerang, RHT_BOOMERANG, ITEM_BOOMERANG, OBJECT_GI_BOOMERANG, GID_BOOMERANG, 0x35, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_LENS_OF_TRUTH] =
        Item(RG_LENS_OF_TRUTH, Text{ "Lens of Truth", "Monocle de Vérité", "Auge der Wahrheit" }, ITEMTYPE_ITEM,
                  GI_LENS, true, &Logic::LensOfTruth, RHT_LENS_OF_TRUTH, ITEM_LENS, OBJECT_GI_GLASSES, GID_LENS, 0x39,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_MEGATON_HAMMER] =
        Item(RG_MEGATON_HAMMER, Text{ "Megaton Hammer", "Masse des Titans", "Stahlhammer" }, ITEMTYPE_ITEM,
                  GI_HAMMER, true, &Logic::Hammer, RHT_MEGATON_HAMMER, ITEM_HAMMER, OBJECT_GI_HAMMER, GID_HAMMER, 0x38,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_STONE_OF_AGONY] =
        Item(RG_STONE_OF_AGONY, Text{ "Stone of Agony", "Pierre de Souffrance", "Stein der Qualen" },
                  ITEMTYPE_ITEM, GI_STONE_OF_AGONY, true, &Logic::ShardOfAgony, RHT_STONE_OF_AGONY, ITEM_STONE_OF_AGONY,
                  OBJECT_GI_MAP, GID_STONE_OF_AGONY, 0x68, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_DINS_FIRE] =
        Item(RG_DINS_FIRE, Text{ "Din's Fire", "Feu de Din", "Dins Feuerinferno" }, ITEMTYPE_ITEM, GI_DINS_FIRE,
                  true, &Logic::DinsFire, RHT_DINS_FIRE, ITEM_DINS_FIRE, OBJECT_GI_GODDESS, GID_DINS_FIRE, 0xAD, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_FARORES_WIND] =
        Item(RG_FARORES_WIND, Text{ "Farore's Wind", "Vent de Farore", "Farores Donnersturm" }, ITEMTYPE_ITEM,
                  GI_FARORES_WIND, true, &Logic::FaroresWind, RHT_FARORES_WIND, ITEM_FARORES_WIND, OBJECT_GI_GODDESS,
                  GID_FARORES_WIND, 0xAE, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_NAYRUS_LOVE] =
        Item(RG_NAYRUS_LOVE, Text{ "Nayru's Love", "Amour de Nayru", "Nayrus Umarmung" }, ITEMTYPE_ITEM,
                  GI_NAYRUS_LOVE, true, &Logic::NayrusLove, RHT_NAYRUS_LOVE, ITEM_NAYRUS_LOVE, OBJECT_GI_GODDESS,
                  GID_NAYRUS_LOVE, 0xAF, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_FIRE_ARROWS] =
        Item(RG_FIRE_ARROWS, Text{ "Fire Arrow", "Flèche de Feu", "Feuerpfeile" }, ITEMTYPE_ITEM, GI_ARROW_FIRE,
                  true, &Logic::FireArrows, RHT_FIRE_ARROWS, ITEM_ARROW_FIRE, OBJECT_GI_M_ARROW, GID_ARROW_FIRE, 0x70,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ICE_ARROWS] =
        Item(RG_ICE_ARROWS, Text{ "Ice Arrow", "Flèche de Glace", "Eispfeil" }, ITEMTYPE_ITEM, GI_ARROW_ICE, true,
                  &Logic::IceArrows, RHT_ICE_ARROWS, ITEM_ARROW_ICE, OBJECT_GI_M_ARROW, GID_ARROW_ICE, 0x71, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_LIGHT_ARROWS] =
        Item(RG_LIGHT_ARROWS, Text{ "Light Arrow", "Flèche de Lumière", "Lichtpfeil" }, ITEMTYPE_ITEM,
                  GI_ARROW_LIGHT, true, &Logic::LightArrows, RHT_LIGHT_ARROWS, ITEM_ARROW_LIGHT, OBJECT_GI_M_ARROW,
                  GID_ARROW_LIGHT, 0x72, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_GERUDO_MEMBERSHIP_CARD] =
        Item(RG_GERUDO_MEMBERSHIP_CARD, Text{ "Gerudo Membership Card", "Carte Gerudo", "Gerudo Karte" },
                  ITEMTYPE_ITEM, GI_GERUDO_CARD, true, &Logic::GerudoToken, RHT_GERUDO_MEMBERSHIP_CARD, ITEM_GERUDO_CARD,
                  OBJECT_GI_GERUDO, GID_GERUDO_CARD, 0x7B, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_MAGIC_BEAN] =
        Item(RG_MAGIC_BEAN, Text{ "Magic Bean", "Haricots Magiques", "Wundererbse" }, ITEMTYPE_ITEM, GI_BEAN, true,
                  &Logic::MagicBean, RHT_MAGIC_BEAN, ITEM_BEAN, OBJECT_GI_BEAN, GID_BEAN, 0x48, 0x80, CHEST_ANIM_SHORT,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_MAGIC_BEAN_PACK] =
        Item(RG_MAGIC_BEAN_PACK, Text{ "Magic Bean Pack", "Paquet de Haricots Magiques", "Wundererbsen-Packung" },
                  ITEMTYPE_ITEM, RG_MAGIC_BEAN_PACK, true, &Logic::MagicBeanPack, RHT_MAGIC_BEAN_PACK,
                  RG_MAGIC_BEAN_PACK, OBJECT_GI_BEAN, GID_BEAN, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_DOUBLE_DEFENSE] = 
        Item(RG_DOUBLE_DEFENSE, Text{ "Double Defense", "Double Défence", "Doppelte Verteidigung" }, ITEMTYPE_ITEM,
                  RG_DOUBLE_DEFENSE, true, &Logic::DoubleDefense, RHT_DOUBLE_DEFENSE, RG_DOUBLE_DEFENSE, 
                  OBJECT_GI_HEARTS,GID_HEART_CONTAINER, 0xE9, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, 
                  MOD_RANDOMIZER);
    itemTable[RG_DOUBLE_DEFENSE].SetCustomDrawFunc(Randomizer_DrawDoubleDefense);
    // Trade Quest Items
    itemTable[RG_WEIRD_EGG] =
        Item(RG_WEIRD_EGG, Text{ "Weird Egg", "Oeuf Curieux", "Seltsames Ei" }, ITEMTYPE_ITEM, GI_WEIRD_EGG, true,
                  &Logic::WeirdEgg, RHT_WEIRD_EGG, ITEM_WEIRD_EGG, OBJECT_GI_EGG, GID_EGG, 0x9A, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ZELDAS_LETTER] =
        Item(RG_ZELDAS_LETTER, Text{ "Zelda's Letter", "Lettre de Zelda", "Zeldas Brief" }, ITEMTYPE_ITEM,
                  GI_LETTER_ZELDA, true, &Logic::ZeldasLetter, RHT_ZELDAS_LETTER, ITEM_LETTER_ZELDA, OBJECT_GI_LETTER,
                  GID_LETTER_ZELDA, 0x69, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_POCKET_EGG] =
        Item(RG_POCKET_EGG, Text{ "Pocket Egg", "Oeuf de poche", "Taschenei" }, ITEMTYPE_ITEM, GI_POCKET_EGG, true,
                  &Logic::PocketEgg, RHT_POCKET_EGG, ITEM_POCKET_EGG, OBJECT_GI_EGG, GID_EGG, 0x01, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_COJIRO] = 
        Item(RG_COJIRO, Text{ "Cojiro", "P'tit Poulet", "Cojiro" }, ITEMTYPE_ITEM, GI_COJIRO, true, &Logic::Cojiro,       
                  RHT_COJIRO, ITEM_COJIRO, OBJECT_GI_NIWATORI, GID_COJIRO, 0x02, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ODD_MUSHROOM] =
        Item(RG_ODD_MUSHROOM, Text{ "Odd Mushroom", "Champigon Suspect", "Seltsamer Pilz" }, ITEMTYPE_ITEM,
                  GI_ODD_MUSHROOM, true, &Logic::OddMushroom, RHT_ODD_MUSHROOM, ITEM_ODD_MUSHROOM, OBJECT_GI_MUSHROOM,
                  GID_ODD_MUSHROOM, 0x03, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ODD_POTION] =
        Item(RG_ODD_POTION, Text{ "Odd Potion", "Mixture Suspecte", "Seltsamer Trank" }, ITEMTYPE_ITEM,
                  GI_ODD_POTION, true, &Logic::OddPoultice, RHT_ODD_POTION, ITEM_ODD_POTION, OBJECT_GI_POWDER,
                  GID_ODD_POTION, 0x04, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_POACHERS_SAW] =
        Item(RG_POACHERS_SAW, Text{ "Poacher's Saw", "Scie du Chasseur", "Wilderer-Säge" }, ITEMTYPE_ITEM, GI_SAW,
                  true, &Logic::PoachersSaw, RHT_POACHERS_SAW, ITEM_SAW, OBJECT_GI_SAW, GID_SAW, 0x05, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BROKEN_SWORD] = Item(
        RG_BROKEN_SWORD, Text{ "Broken Goron's Sword", "Épée Brisée de Goron", "Schwert des gebrochenen Goronen" },
        ITEMTYPE_ITEM, GI_SWORD_BROKEN, true, &Logic::BrokenSword, RHT_BROKEN_SWORD, ITEM_SWORD_BROKEN,
        OBJECT_GI_BROKENSWORD, GID_SWORD_BROKEN, 0x08, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_PRESCRIPTION] =
        Item(RG_PRESCRIPTION, Text{ "Prescription", "Ordonnance", "Verschreibung" }, ITEMTYPE_ITEM,
                  GI_PRESCRIPTION, true, &Logic::Prescription, RHT_PRESCRIPTION, ITEM_PRESCRIPTION,
                  OBJECT_GI_PRESCRIPTION, GID_PRESCRIPTION, 0x09, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_EYEBALL_FROG] =
        Item(RG_EYEBALL_FROG, Text{ "Eyeball Frog", "Crapaud-qui-louche", "Augapfel-Frosch" }, ITEMTYPE_ITEM,
                  GI_FROG, true, &Logic::EyeballFrog, RHT_EYEBALL_FROG, ITEM_FROG, OBJECT_GI_FROG, GID_FROG, 0x0D, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_EYEDROPS] =
        Item(RG_EYEDROPS, Text{ "World's Finest Eyedrops", "Super Gouttes", "Supertropfen" }, ITEMTYPE_ITEM,
                  GI_EYEDROPS, true, &Logic::Eyedrops, RHT_EYEDROPS, ITEM_EYEDROPS, OBJECT_GI_EYE_LOTION, GID_EYEDROPS,
                  0x0E, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_CLAIM_CHECK] =
        Item(RG_CLAIM_CHECK, Text{ "Claim Check", "Certificat", "Quittung" }, ITEMTYPE_ITEM, GI_CLAIM_CHECK, true,
                  &Logic::ClaimCheck, RHT_CLAIM_CHECK, ITEM_CLAIM_CHECK, OBJECT_GI_TICKETSTONE, GID_CLAIM_CHECK, 0x0A,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    // Skulltula Token
    itemTable[RG_GOLD_SKULLTULA_TOKEN] = 
        Item(RG_GOLD_SKULLTULA_TOKEN,
                  Text{ "Gold Skulltula Token", "Symbole de Skulltula d'Or", "Goldenes Skulltula-Symbol" },
                  ITEMTYPE_TOKEN, GI_SKULL_TOKEN, true, &Logic::GoldSkulltulaTokens, RHT_GOLD_SKULLTULA_TOKEN,
                  ITEM_SKULL_TOKEN, OBJECT_GI_SUTARU, GID_SKULL_TOKEN, 0xB4, 0x80, CHEST_ANIM_SHORT,
                  ITEM_CATEGORY_SKULLTULA_TOKEN, MOD_NONE);
    // Progressive Items
    itemTable[RG_PROGRESSIVE_HOOKSHOT] =
        Item(RG_PROGRESSIVE_HOOKSHOT, Text{ "Progressive Hookshot", "Grappin (prog.)", "Progressiver Fanghaken" },
                  ITEMTYPE_ITEM, 0x80, true, &Logic::ProgressiveHookshot, RHT_PROGRESSIVE_HOOKSHOT,
                  true);
    itemTable[RG_PROGRESSIVE_STRENGTH] = Item(
        RG_PROGRESSIVE_STRENGTH,
        Text{ "Strength Upgrade", "Amélioration de Force (prog.)", "Progressives Kraft-Upgrade" },
        ITEMTYPE_ITEM, 0x81, true, &Logic::ProgressiveStrength, RHT_PROGRESSIVE_STRENGTH, true);
    itemTable[RG_PROGRESSIVE_BOMB_BAG] = Item(
        RG_PROGRESSIVE_BOMB_BAG, Text{ "Progressive Bomb Bag", "Sac de Bombes (prog.)", "Progressive Bombentasche" },
        ITEMTYPE_ITEM, 0x82, true, &Logic::ProgressiveBombBag, RHT_PROGRESSIVE_BOMB_BAG, true);
    itemTable[RG_PROGRESSIVE_BOW] =
        Item(RG_PROGRESSIVE_BOW, Text{ "Progressive Bow", "Arc (prog.)", "Progressiver Bogen" }, ITEMTYPE_ITEM,
                  0x83, true, &Logic::ProgressiveBow, RHT_PROGRESSIVE_BOW, true);
    itemTable[RG_PROGRESSIVE_SLINGSHOT] = Item(
        RG_PROGRESSIVE_SLINGSHOT, Text{ "Progressive Slingshot", "Lance-Pierre (prog.)", "Progressive Steinschleuder" },
        ITEMTYPE_ITEM, 0x84, true, &Logic::ProgressiveBulletBag, RHT_PROGRESSIVE_SLINGSHOT, true);
    itemTable[RG_PROGRESSIVE_WALLET] =
        Item(RG_PROGRESSIVE_WALLET, Text{ "Progressive Wallet", "Bourse (prog.)", "Progressive Brieftasche" },
                  ITEMTYPE_ITEM, 0x85, true, &Logic::ProgressiveWallet, RHT_PROGRESSIVE_WALLET, true);
    itemTable[RG_PROGRESSIVE_SCALE] =
        Item(RG_PROGRESSIVE_SCALE, Text{ "Progressive Scale", "Écaille (prog.)", "Progressive Skala" },
                  ITEMTYPE_ITEM, 0x86, true, &Logic::ProgressiveScale, RHT_PROGRESSIVE_SCALE, true);
    itemTable[RG_PROGRESSIVE_NUT_UPGRADE] = Item(
        RG_PROGRESSIVE_NUT_UPGRADE,
        Text{ "Progressive Nut Capacity", "Capacité de Noix (prog.)", "Progressive Nusskapazität" }, ITEMTYPE_ITEM,
        0x87, false, &Logic::noVariable, RHT_PROGRESSIVE_NUT_UPGRADE, true);
    itemTable[RG_PROGRESSIVE_STICK_UPGRADE] = Item(
        RG_PROGRESSIVE_STICK_UPGRADE,
        Text{ "Progressive Stick Capacity", "Capacité de Bâtons (prog.)", "Progressive Stick-Kapazität" },
        ITEMTYPE_ITEM, 0x88, false, &Logic::noVariable, RHT_PROGRESSIVE_STICK_UPGRADE, true);
    itemTable[RG_PROGRESSIVE_BOMBCHUS] =
        Item(RG_PROGRESSIVE_BOMBCHUS, Text{ "Progressive Bombchu", "Missiles (prog.)", "Progressive Kriechgrube" },
                  ITEMTYPE_ITEM, 0x89, true, &Logic::Bombchus, RHT_PROGRESSIVE_BOMBCHUS, true);
    itemTable[RG_PROGRESSIVE_MAGIC_METER] = Item(
        RG_PROGRESSIVE_MAGIC_METER,
        Text{ "Progressive Magic Meter", "Jauge de Magie (prog.)", "Progressives magisches Messgerät" }, ITEMTYPE_ITEM,
        0x8A, true, &Logic::ProgressiveMagic, RHT_PROGRESSIVE_MAGIC_METER, true);
    itemTable[RG_PROGRESSIVE_OCARINA] = Item(
        RG_PROGRESSIVE_OCARINA, Text{ "Progressive Ocarina", "Ocarina (prog.)", "Progressive Okarina" }, ITEMTYPE_ITEM,
        0x8B, true, &Logic::ProgressiveOcarina, RHT_PROGRESSIVE_OCARINA, true);
    itemTable[RG_PROGRESSIVE_GORONSWORD] =
        Item(RG_PROGRESSIVE_GORONSWORD,
                  Text{ "Progressive Goron Sword", "Épée Goron (prog.)", "Progressives Goronenschwert" }, ITEMTYPE_ITEM,
                  0xD4, true, &Logic::ProgressiveGiantKnife, RHT_PROGRESSIVE_GORONSWORD, true);
    // Bottles
    itemTable[RG_EMPTY_BOTTLE] =
        Item(RG_EMPTY_BOTTLE, Text{ "Empty Bottle", "Bouteille Vide", "Leere Flasche" }, ITEMTYPE_ITEM,
                  GI_BOTTLE, true, &Logic::Bottles, RHT_BOTTLE_WITH_MILK, ITEM_BOTTLE, OBJECT_GI_BOTTLE, GID_BOTTLE,
                  0x42, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BOTTLE_WITH_MILK] =
        Item(RG_BOTTLE_WITH_MILK, Text{ "Bottle with Milk", "Bouteille avec du Lait", "Flasche mit Milch" },
                  ITEMTYPE_ITEM, GI_MILK_BOTTLE, true, &Logic::Bottles, RHT_BOTTLE_WITH_MILK, ITEM_MILK_BOTTLE,
                  OBJECT_GI_MILK, GID_MILK, 0x98, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BOTTLE_WITH_RED_POTION] =
        Item(RG_BOTTLE_WITH_RED_POTION,
                  Text{ "Bottle with Red Potion", "Bouteille avec une Potion Rouge", "Flasche mit rotem Elixier" },
                  ITEMTYPE_ITEM, 0x8C, true, &Logic::Bottles, RHT_BOTTLE_WITH_RED_POTION,
                  RG_BOTTLE_WITH_RED_POTION, OBJECT_GI_LIQUID, GID_POTION_RED, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_BOTTLE_WITH_GREEN_POTION] =
        Item(RG_BOTTLE_WITH_GREEN_POTION,
                  Text{ "Bottle with Green Potion", "Bouteille avec une Potion Verte", "Flasche mit grünem Elixier" },
                  ITEMTYPE_ITEM, 0x8D, true, &Logic::Bottles, RHT_BOTTLE_WITH_GREEN_POTION,
                  RG_BOTTLE_WITH_GREEN_POTION, OBJECT_GI_LIQUID, GID_POTION_GREEN, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_BOTTLE_WITH_BLUE_POTION] =
        Item(RG_BOTTLE_WITH_BLUE_POTION,
                  Text{ "Bottle with Blue Potion", "Bouteille avec une Potion Bleue", "Flasche mit blauem Elixier" },
                  ITEMTYPE_ITEM, 0x8E, true, &Logic::Bottles, RHT_BOTTLE_WITH_BLUE_POTION,
                  RG_BOTTLE_WITH_BLUE_POTION, OBJECT_GI_LIQUID, GID_POTION_BLUE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_BOTTLE_WITH_FAIRY] =
        Item(RG_BOTTLE_WITH_FAIRY, Text{ "Bottle with Fairy", "Bouteille avec une Fée", "Flasche mit Fee"},
                  ITEMTYPE_ITEM, 0x8F, true, &Logic::Bottles, RHT_BOTTLE_WITH_FAIRY,
                  RG_BOTTLE_WITH_FAIRY, OBJECT_GI_BOTTLE, GID_BOTTLE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_BOTTLE_WITH_FISH] =
        Item(RG_BOTTLE_WITH_FISH, Text{ "Bottle with Fish", "Bouteille avec un Poisson", "Flasche mit Fisch" },
                  ITEMTYPE_ITEM, 0x90, true, &Logic::Bottles, RHT_BOTTLE_WITH_FISH, RG_BOTTLE_WITH_FISH,
                  OBJECT_GI_FISH, GID_FISH, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR,
                  MOD_RANDOMIZER);
    itemTable[RG_BOTTLE_WITH_BLUE_FIRE] =
        Item(RG_BOTTLE_WITH_BLUE_FIRE,
                  Text{ "Bottle with Blue Fire", "Bouteille avec une Flamme Bleue", "Flasche mit blauem Feuer" },
                  ITEMTYPE_ITEM, 0x91, true, &Logic::Bottles, RHT_BOTTLE_WITH_BLUE_FIRE,
                  RG_BOTTLE_WITH_BLUE_FIRE, OBJECT_GI_FIRE, GID_BLUE_FIRE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_BOTTLE_WITH_BUGS] =
        Item(RG_BOTTLE_WITH_BUGS, Text{ "Bottle with Bugs", "Bouteille avec des Insectes", "Flasche mit Wanzen" },
                  ITEMTYPE_ITEM, 0x92, true, &Logic::Bottles, RHT_BOTTLE_WITH_BUGS, RG_BOTTLE_WITH_BUGS,
                  OBJECT_GI_INSECT, GID_BUG, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR,
                  MOD_RANDOMIZER);
    itemTable[RG_BOTTLE_WITH_POE] =
        Item(RG_BOTTLE_WITH_POE, Text{ "Bottle with Poe", "Bouteille avec un Esprit", "Flasche mit einem Geist" },
                  ITEMTYPE_ITEM, 0x94, true, &Logic::Bottles, RHT_BOTTLE_WITH_POE, RG_BOTTLE_WITH_POE,
                  OBJECT_GI_GHOST, GID_POE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR,
                  MOD_RANDOMIZER);
    itemTable[RG_RUTOS_LETTER] =
        Item(RG_RUTOS_LETTER,
                  Text{ "Bottle with Ruto's Letter", "Bouteille avec la Lettre de Ruto", "Flasche mit Rutos Brief" },
                  ITEMTYPE_ITEM, GI_LETTER_RUTO, true, &Logic::RutosLetter, RHT_RUTOS_LETTER, ITEM_LETTER_RUTO,
                  OBJECT_GI_BOTTLE_LETTER, GID_LETTER_RUTO, 0x99, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BOTTLE_WITH_BIG_POE] =
        Item(RG_BOTTLE_WITH_BIG_POE, Text{ "Bottle with Big Poe", "Bouteille avec une Âme", "Flasche mit Seele" },
                  ITEMTYPE_ITEM, 0x93, true, &Logic::BottleWithBigPoe, RHT_BOTTLE_WITH_BIG_POE,
                  RG_BOTTLE_WITH_BIG_POE, OBJECT_GI_GHOST, GID_BIG_POE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    // Songs
    itemTable[RG_ZELDAS_LULLABY] =
        Item(RG_ZELDAS_LULLABY, Text{ "Zelda's Lullaby", "Berceuse de Zelda", "Zeldas Wiegenlied" }, ITEMTYPE_SONG,
                  0xC1, true, &Logic::ZeldasLullaby, RHT_ZELDAS_LULLABY, ITEM_SONG_LULLABY,
                  OBJECT_GI_MELODY, GID_SONG_ZELDA, 0xD4, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_EPONAS_SONG] =
        Item(RG_EPONAS_SONG, Text{ "Epona's Song", "Chant d'Epona", "Eponas Lied" }, ITEMTYPE_SONG, 0xC2,
                  true, &Logic::EponasSong, RHT_EPONAS_SONG, ITEM_SONG_EPONA, OBJECT_GI_MELODY, GID_SONG_EPONA, 0xD2,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_SARIAS_SONG] =
        Item(RG_SARIAS_SONG, Text{ "Saria's Song", "Chant de Saria", "Sarias Lied" }, ITEMTYPE_SONG,
                  0xC3, true, &Logic::SariasSong, RHT_SARIAS_SONG, ITEM_SONG_SARIA, OBJECT_GI_MELODY,
                  GID_SONG_SARIA, 0xD1, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_SUNS_SONG] =
        Item(RG_SUNS_SONG, Text{ "Sun's Song", "Chant du Soleil", "Das Lied der Sonne" }, ITEMTYPE_SONG,
                  0xC4, true, &Logic::SunsSong, RHT_SUNS_SONG, ITEM_SONG_SUN, OBJECT_GI_MELODY, GID_SONG_SUN,
                  0xD3, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_SONG_OF_TIME] =
        Item(RG_SONG_OF_TIME, Text{ "Song of Time", "Chant du Temps", "Das Lied der Zeit" }, ITEMTYPE_SONG,
                  0xC5, true, &Logic::SongOfTime, RHT_SONG_OF_TIME, ITEM_SONG_TIME, OBJECT_GI_MELODY,
                  GID_SONG_TIME, 0xD5, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_SONG_OF_STORMS] =
        Item(RG_SONG_OF_STORMS, Text{ "Song of Storms", "Chant des Tempêtes", "Lied der Stürme" }, ITEMTYPE_SONG,
                  0xC6, true, &Logic::SongOfStorms, RHT_SONG_OF_STORMS, ITEM_SONG_STORMS, OBJECT_GI_MELODY,
                  GID_SONG_STORM, 0xD6, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_MINUET_OF_FOREST] =
        Item(RG_MINUET_OF_FOREST, Text{ "Minuet of Forest", "Menuet des Bois", "Wald-Minuett", }, ITEMTYPE_SONG,
                  0xBB, true, &Logic::MinuetOfForest, RHT_MINUET_OF_FOREST, ITEM_SONG_MINUET,                    
                  OBJECT_GI_MELODY, GID_SONG_MINUET, 0x73, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BOLERO_OF_FIRE] =
        Item(RG_BOLERO_OF_FIRE, Text{ "Bolero of Fire", "Boléro du Feu", "Bolero des Feuers" }, ITEMTYPE_SONG,
                  0xBC, true, &Logic::BoleroOfFire, RHT_BOLERO_OF_FIRE, ITEM_SONG_BOLERO, OBJECT_GI_MELODY,
                  GID_SONG_BOLERO, 0x74, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_SERENADE_OF_WATER] =
        Item(RG_SERENADE_OF_WATER, Text{ "Serenade of Water", "Sérénade de l'Eau", "Serenade des Wassers" },
                  ITEMTYPE_SONG, 0xBD, true, &Logic::SerenadeOfWater, RHT_SERENADE_OF_WATER,
                  ITEM_SONG_SERENADE, OBJECT_GI_MELODY, GID_SONG_SERENADE, 0x75, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_NOCTURNE_OF_SHADOW] =
        Item(RG_NOCTURNE_OF_SHADOW, Text{ "Nocturne of Shadow", "Nocturne de l'Ombre", "Nocturne der Schatten "},
                  ITEMTYPE_SONG, 0xBF, true, &Logic::NocturneOfShadow, RHT_NOCTURNE_OF_SHADOW,
                  ITEM_SONG_NOCTURNE, OBJECT_GI_MELODY, GID_SONG_NOCTURNE, 0x77, 0x80, CHEST_ANIM_LONG, 
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_REQUIEM_OF_SPIRIT] =
        Item(RG_REQUIEM_OF_SPIRIT, Text{ "Requiem of Spirit", "Requiem des Esprits", "Requiem des Geistes" },
                  ITEMTYPE_SONG, 0xBE, true, &Logic::RequiemOfSpirit, RHT_REQUIEM_OF_SPIRIT,
                  ITEM_SONG_REQUIEM, OBJECT_GI_MELODY, GID_SONG_REQUIEM, 0x76, 0x80, CHEST_ANIM_LONG, 
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_PRELUDE_OF_LIGHT] =
        Item(RG_PRELUDE_OF_LIGHT, Text{ "Prelude of Light", "Prélude de la Lumière", "Präludium des Lichts" },
                  ITEMTYPE_SONG, 0xC0, true, &Logic::PreludeOfLight, RHT_PRELUDE_OF_LIGHT,
                  ITEM_SONG_PRELUDE, OBJECT_GI_MELODY, GID_SONG_PRELUDE, 0x78, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    // Maps and Compasses
    itemTable[RG_DEKU_TREE_MAP] =
        Item(RG_DEKU_TREE_MAP, Text{ "Great Deku Tree Map", "Carte de l'Arbre Mojo", "Karte des Deku-Baums" },
                  ITEMTYPE_MAP, 0xA5, false, &Logic::noVariable, RHT_DEKU_TREE_MAP, RG_DEKU_TREE_MAP,                    
                  OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_DODONGOS_CAVERN_MAP] =
        Item(RG_DODONGOS_CAVERN_MAP,
                  Text{ "Dodongo's Cavern Map", "Carte de la Caverne Dodongo", "Karte der Dodongo-Höhle" },
                  ITEMTYPE_MAP, 0xA6, false, &Logic::noVariable, RHT_DODONGOS_CAVERN_MAP,
                  RG_DODONGOS_CAVERN_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_JABU_JABUS_BELLY_MAP] =
        Item(RG_JABU_JABUS_BELLY_MAP,
                  Text{ "Jabu-Jabu's Belly Map", "Carte du Ventre de Jabu-Jabu", "Karte des Jabu-Jabu-Bauchs" },
                  ITEMTYPE_MAP, 0xA7, false, &Logic::noVariable, RHT_JABU_JABUS_BELLY_MAP,
                  RG_JABU_JABUS_BELLY_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_FOREST_TEMPLE_MAP] =
        Item(RG_FOREST_TEMPLE_MAP, Text{ "Forest Temple Map", "Carte du Temple de la Forêt", "Waldtempel Karte" },
                  ITEMTYPE_MAP, 0xA8, false, &Logic::noVariable, RHT_FOREST_TEMPLE_MAP,
                  RG_FOREST_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_FIRE_TEMPLE_MAP] =
        Item(RG_FIRE_TEMPLE_MAP, Text{ "Fire Temple Map", "Carte due Temple de Feu", "Feuertempel Karte" },
                  ITEMTYPE_MAP, 0xA9, false, &Logic::noVariable, RHT_FIRE_TEMPLE_MAP, RG_FIRE_TEMPLE_MAP,                  
                  OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_WATER_TEMPLE_MAP] =
        Item(RG_WATER_TEMPLE_MAP, Text{ "Water Temple Map", "Carte du Temple de l'Eau", "Wassertempel-Karte" },
                  ITEMTYPE_MAP, 0xAA, false, &Logic::noVariable, RHT_WATER_TEMPLE_MAP, RG_WATER_TEMPLE_MAP,                 
                  OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, 
                  ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_SPIRIT_TEMPLE_MAP] =
        Item(RG_SPIRIT_TEMPLE_MAP,
                  Text{ "Spirit Temple Map", "Carte due Temple de l'Esprit", "Geistertempel Karte"}, ITEMTYPE_MAP,
                  0xAB, false, &Logic::noVariable, RHT_SPIRIT_TEMPLE_MAP, RG_SPIRIT_TEMPLE_MAP,
                  OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_SHADOW_TEMPLE_MAP] =
        Item(RG_SHADOW_TEMPLE_MAP,
                  Text{ "Shadow Temple Map", "Carte du Temple de l'Ombre", "Karte des Schattentempels"}, ITEMTYPE_MAP,
                  0xAC, false, &Logic::noVariable, RHT_SHADOW_TEMPLE_MAP, RG_SHADOW_TEMPLE_MAP,
                  OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_BOTTOM_OF_THE_WELL_MAP] =
        Item(RG_BOTTOM_OF_THE_WELL_MAP,
                  Text{ "Bottom of the Well Map", "Carte du Puits", "Boden des Brunnens Karte"}, ITEMTYPE_MAP,
                  0xAD, false, &Logic::noVariable, RHT_BOTTOM_OF_THE_WELL_MAP,
                  RG_BOTTOM_OF_THE_WELL_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_ICE_CAVERN_MAP] =
        Item(RG_ICE_CAVERN_MAP, Text{ "Ice Cavern Map", "Carte de la Caverne Polaire", "Eishöhle Karte" },
                  ITEMTYPE_MAP, 0xAE, false, &Logic::noVariable, RHT_ICE_CAVERN_MAP, RG_ICE_CAVERN_MAP,                    
                  OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_DEKU_TREE_COMPASS] =
        Item(RG_DEKU_TREE_COMPASS,
                  Text{ "Great Deku Tree Compass", "Boussole de l'Arbre Mojo", "Kompass des Deku-Baums"},
                  ITEMTYPE_COMPASS, 0x9B, false, &Logic::noVariable, RHT_DEKU_TREE_COMPASS,
                  RG_DEKU_TREE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_DODONGOS_CAVERN_COMPASS] =
        Item(RG_DODONGOS_CAVERN_COMPASS,
                  Text{ "Dodongo's Cavern Compass", "Boussole de la Caverne Dodongo", "Kompass der Dodongo-Höhle" },
                  ITEMTYPE_COMPASS, 0x9C, false, &Logic::noVariable, RHT_DODONGOS_CAVERN_COMPASS,
                  RG_DODONGOS_CAVERN_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_JABU_JABUS_BELLY_COMPASS] =
        Item(RG_JABU_JABUS_BELLY_COMPASS,
                  Text{ "Jabu-Jabu's Belly Compass", "Boussole du Ventre de Jabu-Jabu", "Kompass des Jabu-Jabu-Bauchs" },
                  ITEMTYPE_COMPASS, 0x9D, false, &Logic::noVariable, RHT_JABU_JABUS_BELLY_COMPASS,
                  RG_JABU_JABUS_BELLY_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_FOREST_TEMPLE_COMPASS] =
        Item(RG_FOREST_TEMPLE_COMPASS,
                  Text{ "Forest Temple Compass", "Boussole du Temple de la Forêt", "Waldtempel-Kompass" },
                  ITEMTYPE_COMPASS, 0x9E, false, &Logic::noVariable, RHT_FOREST_TEMPLE_COMPASS,
                  RG_FOREST_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_FIRE_TEMPLE_COMPASS] =
        Item(RG_FIRE_TEMPLE_COMPASS,
                  Text{ "Fire Temple Compass", "Boussole du Temple du Feu", "Feuertempel-Kompass" }, ITEMTYPE_COMPASS,
                  0x9F, false, &Logic::noVariable, RHT_FIRE_TEMPLE_COMPASS, RG_FIRE_TEMPLE_COMPASS,              
                  OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_WATER_TEMPLE_COMPASS] =
        Item(RG_WATER_TEMPLE_COMPASS,
                  Text{ "Water Temple Compass", "Boussole du Temple de l'Eau", "Wassertempel-Kompass" },
                  ITEMTYPE_COMPASS, 0xA0, false, &Logic::noVariable, RHT_WATER_TEMPLE_COMPASS, 
                  RG_WATER_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_SPIRIT_TEMPLE_COMPASS] =
        Item(RG_SPIRIT_TEMPLE_COMPASS,
                  Text{ "Spirit Temple Compass", "Boussole due Temple de l'Esprit", "Geistiger Tempelkompass" },
                  ITEMTYPE_COMPASS, 0xA1, false, &Logic::noVariable, RHT_SPIRIT_TEMPLE_COMPASS,
                  RG_SPIRIT_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_SHADOW_TEMPLE_COMPASS] =
        Item(RG_SHADOW_TEMPLE_COMPASS,
                  Text{ "Shadow Temple Compass", "Boussole du Temple de l'Ombre", "Kompass des Schattentempels" },
                  ITEMTYPE_COMPASS, 0xA2, false, &Logic::noVariable, RHT_SHADOW_TEMPLE_COMPASS,
                  RG_SHADOW_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_BOTTOM_OF_THE_WELL_COMPASS] =
        Item(RG_BOTTOM_OF_THE_WELL_COMPASS,
                  Text{ "Bottom of the Well Compass", "Boussole du Puits", "Boden des Brunnenkompasses" },
                  ITEMTYPE_COMPASS, 0xA3, false, &Logic::noVariable,
                  RHT_BOTTOM_OF_THE_WELL_COMPASS, RG_BOTTOM_OF_THE_WELL_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_ICE_CAVERN_COMPASS] =
        Item(RG_ICE_CAVERN_COMPASS,
                  Text{ "Ice Cavern Compass", "Boussole de la Caverne Polaire", "Eishöhlenkompass"}, ITEMTYPE_COMPASS,
                  0xA4, false, &Logic::noVariable, RHT_ICE_CAVERN_COMPASS, RG_ICE_CAVERN_COMPASS,               
                  OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    // Boss Keys
    itemTable[RG_FOREST_TEMPLE_BOSS_KEY] =
        Item(RG_FOREST_TEMPLE_BOSS_KEY,
                  Text{ "Forest Temple Boss Key", "Clé d'Or du Temple de la Forêt", "Waldtempel-Boss-Schlüssel" },
                  ITEMTYPE_BOSSKEY, 0x95, true, &Logic::BossKeyForestTemple,
                  RHT_FOREST_TEMPLE_BOSS_KEY, RG_FOREST_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_BOSS_KEY, MOD_RANDOMIZER);
    itemTable[RG_FOREST_TEMPLE_BOSS_KEY].SetCustomDrawFunc(Randomizer_DrawBossKey);
    itemTable[RG_FIRE_TEMPLE_BOSS_KEY] =
        Item(RG_FIRE_TEMPLE_BOSS_KEY,
                  Text{ "Fire Temple Boss Key", "Clé d'Or du Temple du Feu", "Feuertempel-Boss-Schlüssel" },
                  ITEMTYPE_BOSSKEY, 0x96, true, &Logic::BossKeyFireTemple, RHT_FIRE_TEMPLE_BOSS_KEY,
                  RG_FIRE_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_BOSS_KEY, MOD_RANDOMIZER);
    itemTable[RG_FIRE_TEMPLE_BOSS_KEY].SetCustomDrawFunc(Randomizer_DrawBossKey);
    itemTable[RG_WATER_TEMPLE_BOSS_KEY] =
        Item(RG_WATER_TEMPLE_BOSS_KEY,
                  Text{ "Water Temple Boss Key", "Clé d'Or du Temple de l'Eau", "Wassertempel-Boss-Schlüssel" },
                  ITEMTYPE_BOSSKEY, 0x97, true, &Logic::BossKeyWaterTemple,
                  RHT_WATER_TEMPLE_BOSS_KEY, RG_WATER_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_BOSS_KEY, MOD_RANDOMIZER);
    itemTable[RG_WATER_TEMPLE_BOSS_KEY].SetCustomDrawFunc(Randomizer_DrawBossKey);
    itemTable[RG_SPIRIT_TEMPLE_BOSS_KEY] =
        Item(RG_SPIRIT_TEMPLE_BOSS_KEY,
                  Text{ "Spirit Temple Boss Key", "Clé d'Or du Temple de l'Esprit", "Geistertempel-Boss-Schlüssel" },
                  ITEMTYPE_BOSSKEY, 0x98, true, &Logic::BossKeySpiritTemple,
                  RHT_SPIRIT_TEMPLE_BOSS_KEY, RG_SPIRIT_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_BOSS_KEY, MOD_RANDOMIZER);
    itemTable[RG_SPIRIT_TEMPLE_BOSS_KEY].SetCustomDrawFunc(Randomizer_DrawBossKey);
    itemTable[RG_SHADOW_TEMPLE_BOSS_KEY] =
        Item(RG_SHADOW_TEMPLE_BOSS_KEY,
                  Text{ "Shadow Temple Boss Key", "Clé d'Or du Temple de l'Ombre", 
                  "Schlüssel für den Boss des Schattentempels" }, ITEMTYPE_BOSSKEY, 0x99, true,
                  &Logic::BossKeyShadowTemple, RHT_SHADOW_TEMPLE_BOSS_KEY, RG_SHADOW_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY,
                  GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_BOSS_KEY,
                  MOD_RANDOMIZER);
    itemTable[RG_SHADOW_TEMPLE_BOSS_KEY].SetCustomDrawFunc(Randomizer_DrawBossKey);
    itemTable[RG_GANONS_CASTLE_BOSS_KEY] =
        Item(RG_GANONS_CASTLE_BOSS_KEY,
                  Text{ "Ganon's Castle Boss Key", "Clé d'Or du Château de Ganon", "Ganons Schloss-Boss-Schlüssel" },
                  ITEMTYPE_BOSSKEY, 0x9A, true, &Logic::BossKeyGanonsCastle,
                  RHT_GANONS_CASTLE_BOSS_KEY, RG_GANONS_CASTLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_BOSS_KEY, MOD_RANDOMIZER);
    itemTable[RG_GANONS_CASTLE_BOSS_KEY].SetCustomDrawFunc(Randomizer_DrawBossKey);
    itemTable[RG_FOREST_TEMPLE_SMALL_KEY] =
        Item(RG_FOREST_TEMPLE_SMALL_KEY,
                  Text{ "Forest Temple Small Key", "Petite Clé du Temple de la Forêt", "Waldtempel Kleiner Schlüssel" },
                  ITEMTYPE_SMALLKEY, 0xAF, true, &Logic::ForestTempleKeys,
                  RHT_FOREST_TEMPLE_SMALL_KEY, RG_FOREST_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_FOREST_TEMPLE_SMALL_KEY].SetCustomDrawFunc(Randomizer_DrawSmallKey);
    itemTable[RG_FIRE_TEMPLE_SMALL_KEY] =
        Item(RG_FIRE_TEMPLE_SMALL_KEY,
                  Text{ "Fire Temple Small Key", "Petite Clé du Temple du Feu", "Feuertempel Kleiner Schlüssel" },
                  ITEMTYPE_SMALLKEY, 0xB0, true, &Logic::FireTempleKeys, RHT_FIRE_TEMPLE_SMALL_KEY,
                  RG_FIRE_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_FIRE_TEMPLE_SMALL_KEY].SetCustomDrawFunc(Randomizer_DrawSmallKey);
    itemTable[RG_WATER_TEMPLE_SMALL_KEY] =
        Item(RG_WATER_TEMPLE_SMALL_KEY,
                  Text{ "Water Temple Small Key", "Petite Clé du Temple de l'Eau", "Wassertempel Kleiner Schlüssel" },
                  ITEMTYPE_SMALLKEY, 0xB1, true, &Logic::WaterTempleKeys,
                  RHT_WATER_TEMPLE_SMALL_KEY, RG_WATER_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_WATER_TEMPLE_SMALL_KEY].SetCustomDrawFunc(Randomizer_DrawSmallKey);
    itemTable[RG_SPIRIT_TEMPLE_SMALL_KEY] =
        Item(RG_SPIRIT_TEMPLE_SMALL_KEY, 
                  Text{ "Spirit Temple Small Key", "Petite Clé du Temple de l'Esprit", "Geisttempel Kleiner Schlüssel" },
                  ITEMTYPE_SMALLKEY, 0xB2, true, &Logic::SpiritTempleKeys,
                  RHT_SPIRIT_TEMPLE_SMALL_KEY, RG_SPIRIT_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_SPIRIT_TEMPLE_SMALL_KEY].SetCustomDrawFunc(Randomizer_DrawSmallKey);
    itemTable[RG_SHADOW_TEMPLE_SMALL_KEY] =
        Item(RG_SHADOW_TEMPLE_SMALL_KEY,
                  Text{ "Shadow Temple Small Key", "Petite Clé du Temple de l'Ombre",
                  "Schattentempel Kleiner Schlüssel" }, ITEMTYPE_SMALLKEY, 0xB3, true,
                  &Logic::ShadowTempleKeys, RHT_SHADOW_TEMPLE_SMALL_KEY, RG_SHADOW_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, 
                  GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY,
                  MOD_RANDOMIZER);
    itemTable[RG_SHADOW_TEMPLE_SMALL_KEY].SetCustomDrawFunc(Randomizer_DrawSmallKey);
    itemTable[RG_BOTTOM_OF_THE_WELL_SMALL_KEY] =
        Item(RG_BOTTOM_OF_THE_WELL_SMALL_KEY,
                  Text{ "Bottom of the Well Small Key", "Petite Clé du Puits", "Boden des Brunnens Kleiner Schlüssel" },
                  ITEMTYPE_SMALLKEY, 0xB4, true, &Logic::BottomOfTheWellKeys,
                  RHT_BOTTOM_OF_THE_WELL_SMALL_KEY, RG_BOTTOM_OF_THE_WELL_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_BOTTOM_OF_THE_WELL_SMALL_KEY].SetCustomDrawFunc(Randomizer_DrawSmallKey);
    itemTable[RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY] = Item(
        RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY,
        Text{ "Training Grounds Small Key", "Petite Clé du Gymnase Gerudo", "Trainingsgelände Kleiner Schlüssel" },
        ITEMTYPE_SMALLKEY, 0xB5, true, &Logic::GerudoTrainingGroundsKeys,
        RHT_GERUDO_TRAINING_GROUNDS_SMALL_KEY, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL,
        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY].SetCustomDrawFunc(Randomizer_DrawSmallKey);
    itemTable[RG_GERUDO_FORTRESS_SMALL_KEY] = Item(
        RG_GERUDO_FORTRESS_SMALL_KEY,
        Text{ "Gerudo Fortress Small Key", "Petite Clé du Repaire des Voleurs", "Gerudo-Festung Kleiner Schlüssel" },
        ITEMTYPE_FORTRESS_SMALLKEY, 0xB6, true, &Logic::GerudoFortressKeys,
        RHT_GERUDO_FORTRESS_SMALL_KEY, RG_GERUDO_FORTRESS_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL,
        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_GERUDO_FORTRESS_SMALL_KEY].SetCustomDrawFunc(Randomizer_DrawSmallKey);
    itemTable[RG_GANONS_CASTLE_SMALL_KEY] = Item(
        RG_GANONS_CASTLE_SMALL_KEY,
        Text{ "Ganon's Castle Small Key", "Petite Clé du Château de Ganon", "Ganons Schloss Kleiner Schlüssel" },
        ITEMTYPE_SMALLKEY, 0xB7, true, &Logic::GanonsCastleKeys, RHT_GANONS_CASTLE_SMALL_KEY,
        RG_GANONS_CASTLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT,
        ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_GANONS_CASTLE_SMALL_KEY].SetCustomDrawFunc(Randomizer_DrawSmallKey);
    itemTable[RG_TREASURE_GAME_SMALL_KEY] = Item(
        RG_TREASURE_GAME_SMALL_KEY,
        Text{ "Chest Game Small Key", "Petite Clé du jeu la Chasse-aux-Trésors", "Truhenspiel Kleiner Schlüssel" },
        ITEMTYPE_SMALLKEY, 0xDE, true, &Logic::TreasureGameKeys, RHT_TREASURE_GAME_SMALL_KEY, 
        RG_TREASURE_GAME_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT,
        ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    // Key Rings
    itemTable[RG_FOREST_TEMPLE_KEY_RING] =
        Item(RG_FOREST_TEMPLE_KEY_RING,
                  Text{ "Forest Temple Key Ring", "Trousseau du Temple de la Forêt", "Waldtempel Schlüsselanhänger" },
                  ITEMTYPE_SMALLKEY, 0xD5, true, &Logic::ForestTempleKeys,
                  RHT_FOREST_TEMPLE_KEY_RING, RG_FOREST_TEMPLE_KEY_RING, OBJECT_GI_KEY, GID_KEY_SMALL,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_FOREST_TEMPLE_KEY_RING].SetCustomDrawFunc(Randomizer_DrawKeyRing);
    itemTable[RG_FIRE_TEMPLE_KEY_RING] =
        Item(RG_FIRE_TEMPLE_KEY_RING,
                  Text{ "Fire Temple Key Ring", "Trousseau du Temple du Feu", "Feuertempel Schlüsselanhänger" },
                  ITEMTYPE_SMALLKEY, 0xD6, true, &Logic::FireTempleKeys, RHT_FIRE_TEMPLE_KEY_RING,
                  RG_FIRE_TEMPLE_KEY_RING, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_FIRE_TEMPLE_KEY_RING].SetCustomDrawFunc(Randomizer_DrawKeyRing);
    itemTable[RG_WATER_TEMPLE_KEY_RING] =
        Item(RG_WATER_TEMPLE_KEY_RING,
                  Text{ "Water Temple Key Ring", "Trousseau du Temple de l'Eau", "Wassertempel Schlüsselanhänger" },
                  ITEMTYPE_SMALLKEY, 0xD7, true, &Logic::WaterTempleKeys, RHT_WATER_TEMPLE_KEY_RING,
                  RG_WATER_TEMPLE_KEY_RING, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80,
                  CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_WATER_TEMPLE_KEY_RING].SetCustomDrawFunc(Randomizer_DrawKeyRing);
    itemTable[RG_SPIRIT_TEMPLE_KEY_RING] =
        Item(RG_SPIRIT_TEMPLE_KEY_RING,
                  Text{ "Spirit Temple Key Ring", "Trousseau du Temple de l'Esprit", "Geisttempel Schlüsselanhänger" },
                  ITEMTYPE_SMALLKEY, 0xD8, true, &Logic::SpiritTempleKeys,
                  RHT_SPIRIT_TEMPLE_KEY_RING, RG_SPIRIT_TEMPLE_KEY_RING, OBJECT_GI_KEY, GID_KEY_SMALL,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_SPIRIT_TEMPLE_KEY_RING].SetCustomDrawFunc(Randomizer_DrawKeyRing);
    itemTable[RG_SHADOW_TEMPLE_KEY_RING] = Item(
        RG_SHADOW_TEMPLE_KEY_RING,
        Text{ "Shadow Temple Key Ring", "Trousseau du Temple de l'Ombre", "Schattentempel Schlüsselanhänger" },
        ITEMTYPE_SMALLKEY, 0xD9, true, &Logic::ShadowTempleKeys, RHT_SHADOW_TEMPLE_KEY_RING,
        RG_SHADOW_TEMPLE_KEY_RING, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT,
        ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_SHADOW_TEMPLE_KEY_RING].SetCustomDrawFunc(Randomizer_DrawKeyRing);
    itemTable[RG_BOTTOM_OF_THE_WELL_KEY_RING] =
        Item(RG_BOTTOM_OF_THE_WELL_KEY_RING,
                  Text{ "Bottom of the Well Key Ring", "Trousseau du Puits", "Boden des Brunnens Schlüsselanhänger" },
                  ITEMTYPE_SMALLKEY, 0xDA, true, &Logic::BottomOfTheWellKeys,
                  RHT_BOTTOM_OF_THE_WELL_KEY_RING, RG_BOTTOM_OF_THE_WELL_KEY_RING, OBJECT_GI_KEY, GID_KEY_SMALL,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_BOTTOM_OF_THE_WELL_KEY_RING].SetCustomDrawFunc(Randomizer_DrawKeyRing);
    itemTable[RG_GERUDO_TRAINING_GROUNDS_KEY_RING] = Item(
        RG_GERUDO_TRAINING_GROUNDS_KEY_RING,
        Text{ "Training Grounds Key Ring", "Trousseau du Gymnase Gerudo", "Trainingsgelände Schlüsselanhänger" },
        ITEMTYPE_SMALLKEY, 0xDB, true, &Logic::GerudoTrainingGroundsKeys,
        RHT_GERUDO_TRAINING_GROUNDS_KEY_RING, RG_GERUDO_TRAINING_GROUNDS_KEY_RING, OBJECT_GI_KEY, GID_KEY_SMALL,
        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_GERUDO_TRAINING_GROUNDS_KEY_RING].SetCustomDrawFunc(Randomizer_DrawKeyRing);
    itemTable[RG_GERUDO_FORTRESS_KEY_RING] = Item(
        RG_GERUDO_FORTRESS_KEY_RING,
        Text{ "Gerudo Fortress Key Ring", "Trousseau du Repaire des Voleurs", "Gerudo Festung Schlüsselanhänger" },
        ITEMTYPE_FORTRESS_SMALLKEY, 0xDC, true, &Logic::GerudoFortressKeys,
        RHT_GERUDO_FORTRESS_KEY_RING, RG_GERUDO_FORTRESS_KEY_RING, OBJECT_GI_KEY, GID_KEY_SMALL,
        TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_GERUDO_FORTRESS_KEY_RING].SetCustomDrawFunc(Randomizer_DrawKeyRing);
    itemTable[RG_GANONS_CASTLE_KEY_RING] = Item(
        RG_GANONS_CASTLE_KEY_RING,
        Text{ "Ganon's Castle Key Ring", "Trousseau du Château de Ganon", "Ganons Schloss Schlüsselanhänger" },
        ITEMTYPE_SMALLKEY, 0xDD, true, &Logic::GanonsCastleKeys, RHT_GANONS_CASTLE_KEY_RING,
        RG_GANONS_CASTLE_KEY_RING, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT,
        ITEM_CATEGORY_SMALL_KEY, MOD_RANDOMIZER);
    itemTable[RG_GANONS_CASTLE_KEY_RING].SetCustomDrawFunc(Randomizer_DrawKeyRing);
    // Dungeon Rewards
    itemTable[RG_KOKIRI_EMERALD] = Item(
        RG_KOKIRI_EMERALD, Text{ "Kokiri's Emerald", "Émeraude Kokiri", "Kokiri-Smaragd" }, ITEMTYPE_DUNGEONREWARD,
        0xCB, true, &Logic::KokiriEmerald, RHT_KOKIRI_EMERALD, ITEM_KOKIRI_EMERALD, OBJECT_GI_JEWEL,
        GID_KOKIRI_EMERALD, 0x80, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_GORON_RUBY] =
        Item(RG_GORON_RUBY, Text{ "Goron's Ruby", "Rubis Goron", "Goronen-Rubin" }, ITEMTYPE_DUNGEONREWARD,
                  0xCC, true, &Logic::GoronRuby, RHT_GORON_RUBY, ITEM_GORON_RUBY, OBJECT_GI_JEWEL,
                  GID_GORON_RUBY, 0x81, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ZORA_SAPPHIRE] =
        Item(RG_ZORA_SAPPHIRE, Text{ "Zora's Sapphire", "Saphir Zora", "Zora-Saphir" }, ITEMTYPE_DUNGEONREWARD,
                  0xCD, true, &Logic::ZoraSapphire, RHT_ZORA_SAPPHIRE, ITEM_ZORA_SAPPHIRE, OBJECT_GI_JEWEL,
                  GID_ZORA_SAPPHIRE, 0x82, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_FOREST_MEDALLION] =
        Item(RG_FOREST_MEDALLION, Text{ "Forest Medallion", "Médaillon de la Forêt", "Wald Medaillon" },
                  ITEMTYPE_DUNGEONREWARD, 0xCE, true, &Logic::ForestMedallion, RHT_FOREST_MEDALLION,
                  ITEM_MEDALLION_FOREST, OBJECT_GI_MEDAL, GID_MEDALLION_FOREST, 0x3E, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_FIRE_MEDALLION] = Item(
        RG_FIRE_MEDALLION, Text{ "Fire Medallion", "Médaillon du Feu", "Feuer Medaillon" }, ITEMTYPE_DUNGEONREWARD,
        0xCF, true, &Logic::FireMedallion, RHT_FIRE_MEDALLION, ITEM_MEDALLION_FIRE, OBJECT_GI_MEDAL,
        GID_MEDALLION_FIRE, 0x3C, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_WATER_MEDALLION] = Item(
        RG_WATER_MEDALLION, Text{ "Water Medallion", "Médaillon de l'Eau", "Wasser Medaillon" }, ITEMTYPE_DUNGEONREWARD,
        0xD0, true, &Logic::WaterMedallion, RHT_WATER_MEDALLION, ITEM_MEDALLION_WATER, OBJECT_GI_MEDAL,
        GID_MEDALLION_WATER, 0x3D, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_SPIRIT_MEDALLION] =
        Item(RG_SPIRIT_MEDALLION, Text{ "Spirit Medallion", "Médaillon de l'Esprit", "Geist Medaillon" },
                  ITEMTYPE_DUNGEONREWARD, 0xD1, true, &Logic::SpiritMedallion, RHT_SPIRIT_MEDALLION,
                  ITEM_MEDALLION_SPIRIT, OBJECT_GI_MEDAL, GID_MEDALLION_SPIRIT, 0x3F, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_SHADOW_MEDALLION] =
        Item(RG_SHADOW_MEDALLION, Text{ "Shadow Medallion", "Médaillon de l'Ombre", "Schatten Medaillon" },
                  ITEMTYPE_DUNGEONREWARD, 0xD2, true, &Logic::ShadowMedallion, RHT_SHADOW_MEDALLION,
                  ITEM_MEDALLION_SHADOW, OBJECT_GI_MEDAL, GID_MEDALLION_SHADOW, 0x41, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_LIGHT_MEDALLION] =
        Item(RG_LIGHT_MEDALLION, Text{ "Light Medallion", "Médaillon de la Lumière", "Licht Medaillon" },
                  ITEMTYPE_DUNGEONREWARD, 0xD3, true, &Logic::LightMedallion, RHT_LIGHT_MEDALLION,
                  ITEM_MEDALLION_LIGHT, OBJECT_GI_MEDAL, GID_MEDALLION_LIGHT, 0x40, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    // Generic Items
    itemTable[RG_RECOVERY_HEART] =
        Item(RG_RECOVERY_HEART, Text{ "Recovery Heart", "Coeur de Vie", "Herz" }, ITEMTYPE_ITEM, GI_HEART, false,
                  &Logic::noVariable, RHT_RECOVERY_HEART, ITEM_HEART, OBJECT_GI_HEART, GID_HEART, 0x55, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_GREEN_RUPEE] =
        Item(RG_GREEN_RUPEE, Text{ "Green Rupee", "Rubis Vert", "Grüne Rupie" }, ITEMTYPE_ITEM, GI_RUPEE_GREEN,
                  false, &Logic::noVariable, RHT_GREEN_RUPEE, ITEM_RUPEE_GREEN, OBJECT_GI_RUPY, GID_RUPEE_GREEN, 0x6F,
                  0x00, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_GREG_RUPEE] =
        Item(RG_GREG_RUPEE, Text{ "Greg the Green Rupee", "Rubis Greg", "Greg Rupie" }, ITEMTYPE_ITEM,
                  GI_RUPEE_GREEN, true, &Logic::Greg, RHT_GREG_RUPEE, RG_GREG_RUPEE, OBJECT_GI_RUPY, GID_RUPEE_GREEN,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_BLUE_RUPEE] =
        Item(RG_BLUE_RUPEE, Text{ "Blue Rupee", "Rubis Bleu", "Blaue Rupie" }, ITEMTYPE_ITEM, GI_RUPEE_BLUE, false,
                  &Logic::noVariable, RHT_BLUE_RUPEE, ITEM_RUPEE_BLUE, OBJECT_GI_RUPY, GID_RUPEE_BLUE, 0xCC, 0x01,
                  CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_RED_RUPEE] =
        Item(RG_RED_RUPEE, Text{ "Red Rupee", "Rubis Rouge", "Rote Rupie" }, ITEMTYPE_ITEM, GI_RUPEE_RED, false,
                  &Logic::noVariable, RHT_RED_RUPEE, ITEM_RUPEE_RED, OBJECT_GI_RUPY, GID_RUPEE_RED, 0xF0, 0x02,
                  CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_PURPLE_RUPEE] =
        Item(RG_PURPLE_RUPEE, Text{ "Purple Rupee", "Rubis Pourpre", "Lila Rupie" }, ITEMTYPE_ITEM,
                  GI_RUPEE_PURPLE, false, &Logic::noVariable, RHT_PURPLE_RUPEE, ITEM_RUPEE_PURPLE, OBJECT_GI_RUPY,
                  GID_RUPEE_PURPLE, 0xF1, 0x14, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_HUGE_RUPEE] =
        Item(RG_HUGE_RUPEE, Text{ "Huge Rupee", "Énorme Rubis", "Riesige Rupie" }, ITEMTYPE_ITEM, GI_RUPEE_GOLD,
                  false, &Logic::noVariable, RHT_HUGE_RUPEE, ITEM_RUPEE_GOLD, OBJECT_GI_RUPY, GID_RUPEE_GOLD, 0xF2, 0x13,
                  CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_PIECE_OF_HEART] =
        Item(RG_PIECE_OF_HEART, Text{ "Piece of Heart", "Quart de Coeur", "Herzstück" }, ITEMTYPE_ITEM,
                  GI_HEART_PIECE, true, &Logic::PieceOfHeart, RHT_PIECE_OF_HEART, ITEM_HEART_PIECE_2, OBJECT_GI_HEARTS,
                  GID_HEART_PIECE, 0xC2, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_HEART_CONTAINER] =
        Item(RG_HEART_CONTAINER, Text{ "Heart Container", "Réceptacle de Coeur", "Herz-Container" }, ITEMTYPE_ITEM,
                  GI_HEART_CONTAINER_2, true, &Logic::HeartContainer, RHT_HEART_CONTAINER, ITEM_HEART_CONTAINER,
                  OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xC6, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_ICE_TRAP] =
        Item(RG_ICE_TRAP, Text{ "Ice Trap", "Piège de Glace", "Eisfalle" }, ITEMTYPE_ITEM, RG_ICE_TRAP, false,
                  &Logic::noVariable, RHT_ICE_TRAP, RG_ICE_TRAP, OBJECT_GI_RUPY, GID_RUPEE_GOLD,
                  TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_MILK] =
        Item(RG_MILK, Text{ "Milk", "Lait", "Milch" }, ITEMTYPE_ITEM, GI_MILK, false, &Logic::noVariable, RHT_NONE,
                  ITEM_MILK, OBJECT_GI_MILK, GID_MILK, 0x98, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_JUNK, MOD_NONE);
    // Refills
    itemTable[RG_BOMBS_5] = Item(RG_BOMBS_5, Text{ "Bombs (5)", "Bombes (5)", "Bomben (5)" }, ITEMTYPE_REFILL,
                                      GI_BOMBS_5, false, &Logic::noVariable, RHT_BOMBS_5, ITEM_BOMBS_5, OBJECT_GI_BOMB_1,
                                      GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_BOMBS_10] =
        Item(RG_BOMBS_10, Text{ "Bombs (10)", "Bombes (10)", "Bomben (10)" }, ITEMTYPE_REFILL, GI_BOMBS_10, false,
                  &Logic::noVariable, RHT_BOMBS_10, ITEM_BOMBS_10, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59,
                  CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_BOMBS_20] =
        Item(RG_BOMBS_20, Text{ "Bombs (20)", "Bombes (20)", "Bomben (20)" }, ITEMTYPE_REFILL, GI_BOMBS_20, false,
                  &Logic::noVariable, RHT_BOMBS_20, ITEM_BOMBS_20, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT,
                  ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_BOMBCHU_5] =
        Item(RG_BOMBCHU_5, Text{ "Bombchus (5)", "Missiles (5)", "Bombchus (5)" }, ITEMTYPE_REFILL, GI_BOMBCHUS_5,
                  true, &Logic::Bombchus5, RHT_BOMBCHU_5, ITEM_BOMBCHUS_5, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80,
                  CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_BOMBCHU_10] =
        Item(RG_BOMBCHU_10, Text{ "Bombchus (10)", "Missiles (10)", "Bombchus (10)" }, ITEMTYPE_REFILL,
                  GI_BOMBCHUS_10, true, &Logic::Bombchus10, RHT_BOMBCHU_10, ITEM_BOMBCHU, OBJECT_GI_BOMB_2, GID_BOMBCHU,
                  0x33, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_BOMBCHU_20] =
        Item(RG_BOMBCHU_20, Text{ "Bombchus (20)", "Missiles (20)", "Bombchus (20)" }, ITEMTYPE_REFILL,
                  GI_BOMBCHUS_20, true, &Logic::Bombchus20, RHT_BOMBCHU_20, ITEM_BOMBCHUS_20, OBJECT_GI_BOMB_2,
                  GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_BOMBCHU_DROP] =
        Item(RG_BOMBCHU_DROP, Text{ "Bombchu Drop", "Drop Missiles", "Bombchus" }, ITEMTYPE_DROP, GI_BOMBCHUS_10,
                  true, &Logic::BombchuDrop, RHT_NONE, ITEM_BOMBCHU, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80,
                  CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_ARROWS_5] =
        Item(RG_ARROWS_5, Text{ "Arrows (5)", "Flèches (5)", "Pfeile (5)" }, ITEMTYPE_REFILL, GI_ARROWS_SMALL,
                  false, &Logic::noVariable, RHT_ARROWS_5, ITEM_ARROWS_SMALL, OBJECT_GI_ARROW, GID_ARROWS_SMALL, 0xE6,
                  0x48, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_ARROWS_10] =
        Item(RG_ARROWS_10, Text{ "Arrows (10)", "Flèches (10)", "Pfeile (10)" }, ITEMTYPE_REFILL, GI_ARROWS_MEDIUM,
                  false, &Logic::noVariable, RHT_ARROWS_10, ITEM_ARROWS_MEDIUM, OBJECT_GI_ARROW, GID_ARROWS_MEDIUM, 0xE6,
                  0x49, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_ARROWS_30] =
        Item(RG_ARROWS_30, Text{ "Arrows (30)", "Flèches (30)", "Pfeile (30)" }, ITEMTYPE_REFILL, GI_ARROWS_LARGE,
                  false, &Logic::noVariable, RHT_ARROWS_30, ITEM_ARROWS_LARGE, OBJECT_GI_ARROW, GID_ARROWS_LARGE, 0xE6,
                  0x4A, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_DEKU_NUTS_5] =
        Item(RG_DEKU_NUTS_5, Text{ "Deku Nuts (5)", "Noix Mojo (5)", "Deku-Nüsse (5)" }, ITEMTYPE_REFILL,
                  GI_NUTS_5, false, &Logic::noVariable, RHT_DEKU_NUTS_5, ITEM_NUTS_5, OBJECT_GI_NUTS, GID_NUTS, 0x34,
                  0x0C, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_DEKU_NUTS_10] =
        Item(RG_DEKU_NUTS_10, Text{ "Deku Nuts (10)", "Noix Mojo (10)", "Deku-Nüsse (10)" }, ITEMTYPE_REFILL,
                  GI_NUTS_10, false, &Logic::noVariable, RHT_DEKU_NUTS_10, ITEM_NUTS_10, OBJECT_GI_NUTS, GID_NUTS, 0x34,
                  0x0C, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_DEKU_SEEDS_30] =
        Item(RG_DEKU_SEEDS_30, Text{ "Deku Seeds (30)", "Graines Mojo (30)", "Deku-Samen (30)" }, ITEMTYPE_REFILL,
                  GI_SEEDS_30, false, &Logic::noVariable, RHT_DEKU_SEEDS_30, ITEM_SEEDS_30, OBJECT_GI_SEED, GID_SEEDS,
                  0xDC, 0x50, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_DEKU_STICK_1] =
        Item(RG_DEKU_STICK_1, Text{ "Deku Stick (1)", "Bâton Mojo (1)", "Deku-Stick (1)" }, ITEMTYPE_REFILL,
                  GI_STICKS_1, false, &Logic::noVariable, RHT_DEKU_STICK_1, ITEM_STICK, OBJECT_GI_STICK, GID_STICK, 0x37,
                  0x0D, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_RED_POTION_REFILL] =
        Item(RG_RED_POTION_REFILL,
                  Text{ "Red Potion Refill", "Recharge de Potion Rouge", "Rotes Elixier Nachfüllpackung" },
                  ITEMTYPE_REFILL, GI_POTION_RED, false, &Logic::noVariable, RHT_NONE, ITEM_POTION_RED, OBJECT_GI_LIQUID,
                  GID_POTION_RED, 0x43, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_GREEN_POTION_REFILL] =
        Item(RG_GREEN_POTION_REFILL,
                  Text{ "Green Potion Refill", "Recharge de Potion Verte", "Grünes Elixier Nachfüllpackung" },
                  ITEMTYPE_REFILL, GI_POTION_GREEN, false, &Logic::noVariable, RHT_NONE, ITEM_POTION_GREEN,
                  OBJECT_GI_LIQUID, GID_POTION_GREEN, 0x44, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_BLUE_POTION_REFILL] =
        Item(RG_BLUE_POTION_REFILL,
                  Text{ "Blue Potion Refill", "Recharge de Potion Bleue", "Blaues Elixier Nachfüllpackung" },
                  ITEMTYPE_REFILL, GI_POTION_BLUE, false, &Logic::noVariable, RHT_NONE, ITEM_POTION_BLUE,
                  OBJECT_GI_LIQUID, GID_POTION_BLUE, 0x45, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_JUNK, MOD_NONE);
    // Treasure Game
    itemTable[RG_TREASURE_GAME_HEART] = Item(
        RG_TREASURE_GAME_HEART,
        Text{ "Piece of Heart (WINNER)", "Quart de Coeur (Chasse-aux-Trésors)",
              "Herzstück (Schatztruhen-Minispiel)" },
        ITEMTYPE_ITEM, GI_HEART_PIECE_WIN, true, &Logic::PieceOfHeart, RHT_TREASURE_GAME_HEART, ITEM_HEART_PIECE_2,
        OBJECT_GI_HEARTS, GID_HEART_PIECE, 0xFA, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_TREASURE_GAME_GREEN_RUPEE] = Item(
        RG_TREASURE_GAME_GREEN_RUPEE,
        Text{ "Green Rupee (LOSER)", "Rubis Vert (Chasse-aux-Trésors)",
              "Grüne Rupie (Schatztruhe-Minispiel)" },
        ITEMTYPE_ITEM, GI_RUPEE_GREEN_LOSE, false, &Logic::noVariable, RHT_TREASURE_GAME_GREEN_RUPEE, ITEM_RUPEE_GREEN,
        OBJECT_GI_RUPY, GID_RUPEE_GREEN, 0xF4, 0x00, CHEST_ANIM_SHORT, ITEM_CATEGORY_MAJOR, MOD_NONE);
    // Shop
    itemTable[RG_BUY_DEKU_NUT_5] =
        Item(RG_BUY_DEKU_NUT_5, Text{ "Buy Deku Nut (5)", "Acheter: Noix Mojo (5)", "Deku-Nuss kaufen (5)" },
                  ITEMTYPE_SHOP, RG_BUY_DEKU_NUT_5, true, &Logic::Nuts, RHT_DEKU_NUTS_5, false, 15);
    itemTable[RG_BUY_ARROWS_30] =
        Item(RG_BUY_ARROWS_30, Text{ "Buy Arrows (30)", "Acheter: Flèches (30)", "Pfeile kaufen (30)" },
                  ITEMTYPE_SHOP, RG_BUY_ARROWS_30, true, &Logic::BuyArrow, RHT_ARROWS_30, false, 60);
    itemTable[RG_BUY_ARROWS_50] =
        Item(RG_BUY_ARROWS_50, Text{ "Buy Arrows (50)", "Acheter: Flèches (50)", "Pfeile kaufen (50)" },
                  ITEMTYPE_SHOP, RG_BUY_ARROWS_50, true, &Logic::BuyArrow, RHT_ARROWS_30, false, 90);
    itemTable[RG_BUY_BOMBS_525] =
        Item(RG_BUY_BOMBS_525, Text{ "Buy Bombs (5) [25]", "Acheter: Bombes (5) [25]", "Bomben kaufen (5) [25]" },
                  ITEMTYPE_SHOP, RG_BUY_BOMBS_525, true, &Logic::BuyBomb, RHT_BOMBS_5, false, 25);
    itemTable[RG_BUY_DEKU_NUT_10] =
        Item(RG_BUY_DEKU_NUT_10, Text{ "Buy Deku Nut (10)", "Acheter: Noix Mojo (10)", "Deku-Nuss kaufen (10)" },
                  ITEMTYPE_SHOP, RG_BUY_DEKU_NUT_10, true, &Logic::Nuts, RHT_DEKU_NUTS_10, false, 30);
    itemTable[RG_BUY_DEKU_STICK_1] =
        Item(RG_BUY_DEKU_STICK_1, Text{ "Buy Deku Stick (1)", "Acheter: Bâton Mojo (1)", "Deku-Stick kaufen (1)" },
                  ITEMTYPE_SHOP, RG_BUY_DEKU_STICK_1, true, &Logic::Sticks, RHT_DEKU_STICK_1, false, 10);
    itemTable[RG_BUY_BOMBS_10] =
        Item(RG_BUY_BOMBS_10, Text{ "Buy Bombs (10)", "Acheter: Bombes (10)", "Bomben kaufen (10)" },
                  ITEMTYPE_SHOP, RG_BUY_BOMBS_10, true, &Logic::BuyBomb, RHT_BOMBS_10, false, 50);
    itemTable[RG_BUY_FISH] =
        Item(RG_BUY_FISH, Text{ "Buy Fish", "Acheter: Poisson", "Fisch kaufen" }, ITEMTYPE_SHOP, RG_BUY_FISH, true,
                  &Logic::FishAccess, RHT_BOTTLE_WITH_FISH, false, 200);
    itemTable[RG_BUY_RED_POTION_30] = Item(
        RG_BUY_RED_POTION_30, Text{ "Buy Red Potion [30]", "Acheter: Potion Rouge [30]", "Rotes Elixier kaufen [30]" },
        ITEMTYPE_SHOP, RG_BUY_RED_POTION_30, false, &Logic::noVariable, RHT_BOTTLE_WITH_RED_POTION, false, 30);
    itemTable[RG_BUY_GREEN_POTION] =
        Item(RG_BUY_GREEN_POTION, Text{ "Buy Green Potion", "Acheter: Potion Verte", "Grünes Elixier kaufen" },
                  ITEMTYPE_SHOP, RG_BUY_GREEN_POTION, true, &Logic::BuyGPotion, RHT_BOTTLE_WITH_GREEN_POTION, false, 30);
    itemTable[RG_BUY_BLUE_POTION] =
        Item(RG_BUY_BLUE_POTION, Text{ "Buy Blue Potion", "Acheter: Potion Bleue", "Blaues Elixier kaufen" },
                  ITEMTYPE_SHOP, RG_BUY_BLUE_POTION, true, &Logic::BuyBPotion, RHT_BOTTLE_WITH_BLUE_POTION, false, 100);
    itemTable[RG_BUY_HYLIAN_SHIELD] = Item(
        RG_BUY_HYLIAN_SHIELD, Text{ "Buy Hylian Shield", "Acheter: Bouclier Hylien", "Hylianischer Schild kaufen" },
        ITEMTYPE_SHOP, RG_BUY_HYLIAN_SHIELD, true, &Logic::HylianShield, RHT_HYLIAN_SHIELD, false, 80);
    itemTable[RG_BUY_DEKU_SHIELD] =
        Item(RG_BUY_DEKU_SHIELD, Text{ "Buy Deku Shield", "Acheter: Bouclier Mojo", "Deku-Schild kaufen" },
                  ITEMTYPE_SHOP, RG_BUY_DEKU_SHIELD, true, &Logic::DekuShield, RHT_DEKU_SHIELD, false, 40);
    itemTable[RG_BUY_GORON_TUNIC] =
        Item(RG_BUY_GORON_TUNIC, Text{ "Buy Goron Tunic", "Acheter: Tunique Goron", "Goronen-Tunika kaufen" },
                  ITEMTYPE_SHOP, RG_BUY_GORON_TUNIC, true, &Logic::GoronTunic, RHT_GORON_TUNIC, false, 200);
    itemTable[RG_BUY_ZORA_TUNIC] =
        Item(RG_BUY_ZORA_TUNIC, Text{ "Buy Zora Tunic", "Acheter: Tunique Zora", "Zora-Tunika kaufen" },
                  ITEMTYPE_SHOP, RG_BUY_ZORA_TUNIC, true, &Logic::ZoraTunic, RHT_ZORA_TUNIC, false, 300);
    itemTable[RG_BUY_HEART] =
        Item(RG_BUY_HEART, Text{ "Buy Heart", "Acheter: Coeur de Vie", "Herz kaufen" }, ITEMTYPE_SHOP,
                  RG_BUY_HEART, false, &Logic::noVariable, RHT_RECOVERY_HEART, false, 10);
    itemTable[RG_BUY_BOMBCHU_10] =
        Item(RG_BUY_BOMBCHU_10, Text{ "Buy Bombchu (10)", "Acheter: Missiles (10)", "Bomchu kaufen (10)" },
                  ITEMTYPE_SHOP, RG_BUY_BOMBCHU_10, true, &Logic::BuyBombchus10, RHT_BOMBCHU_10, false, 99);
    itemTable[RG_BUY_BOMBCHU_20] =
        Item(RG_BUY_BOMBCHU_20, Text{ "Buy Bombchu (20)", "Acheter: Missiles (20)", "Bomchu kaufen (20)" },
                  ITEMTYPE_SHOP, RG_BUY_BOMBCHU_20, true, &Logic::BuyBombchus20, RHT_BOMBCHU_20, false, 180);
    itemTable[RG_BUY_DEKU_SEEDS_30] = Item(
        RG_BUY_DEKU_SEEDS_30, Text{ "Buy Deku Seeds (30)", "Acheter: Graines Mojo (30)", "Deku-Samen kaufen (30)" },
        ITEMTYPE_SHOP, RG_BUY_DEKU_SEEDS_30, true, &Logic::BuySeed, RHT_DEKU_SEEDS_30, false, 30);
    itemTable[RG_SOLD_OUT] = Item(RG_SOLD_OUT, Text{ "Sold Out", "Rupture de stock", "Ausverkauft" },
                                       ITEMTYPE_SHOP, RG_SOLD_OUT, false, &Logic::noVariable, RHT_NONE, false, 0);
    itemTable[RG_BUY_BLUE_FIRE] =
        Item(RG_BUY_BLUE_FIRE, Text{ "Buy Blue Fire", "Acheter: Flamme Bleue", "Blaues Feuer kaufen" },
                  ITEMTYPE_SHOP, RG_BUY_BLUE_FIRE, true, &Logic::BlueFireAccess, RHT_BOTTLE_WITH_BLUE_FIRE, false, 300);
    itemTable[RG_BUY_BOTTLE_BUG] =
        Item(RG_BUY_BOTTLE_BUG, Text{ "Buy Bottle Bug", "Acheter: Insecte en bouteille", "Flaschenkäfer kaufen" },
                  ITEMTYPE_SHOP, RG_BUY_BOTTLE_BUG, true, &Logic::BugsAccess, RHT_BOTTLE_WITH_BUGS, false, 50);
    itemTable[RG_BUY_POE] = Item(RG_BUY_POE, Text{ "Buy Poe", "Acheter: Esprit", "Geist kaufen" }, ITEMTYPE_SHOP,
                                      RG_BUY_POE, false, &Logic::noVariable, RHT_BOTTLE_WITH_BIG_POE, false, 30);
    itemTable[RG_BUY_FAIRYS_SPIRIT] =
        Item(RG_BUY_FAIRYS_SPIRIT, Text{ "Buy Fairy's Spirit", "Acheter: Esprit de Fée", "Feengeist kaufen" },
                  ITEMTYPE_SHOP, RG_BUY_FAIRYS_SPIRIT, true, &Logic::FairyAccess, RHT_BOTTLE_WITH_FAIRY, false, 50);
    itemTable[RG_BUY_ARROWS_10] =
        Item(RG_BUY_ARROWS_10, Text{ "Buy Arrows (10)", "Acheter: Flèches (10)", "Pfeile kaufen (10)" },
                  ITEMTYPE_SHOP, RG_BUY_ARROWS_10, true, &Logic::BuyArrow, RHT_ARROWS_10, false, 20);
    itemTable[RG_BUY_BOMBS_20] =
        Item(RG_BUY_BOMBS_20, Text{ "Buy Bombs (20)", "Acheter: Bombes (20)", "Bomben kaufen (20)" },
                  ITEMTYPE_SHOP, RG_BUY_BOMBS_20, true, &Logic::BuyBomb, RHT_BOMBS_20, false, 80);
    itemTable[RG_BUY_BOMBS_30] =
        Item(RG_BUY_BOMBS_30, Text{ "Buy Bombs (30)", "Acheter: Bombes (30)", "Bomben kaufen (30)" },
                  ITEMTYPE_SHOP, RG_BUY_BOMBS_30, true, &Logic::BuyBomb, RHT_BOMBS_20, false, 120);
    itemTable[RG_BUY_BOMBS_535] =
        Item(RG_BUY_BOMBS_535, Text{ "Buy Bombs (5) [35]", "Acheter: Bombes (5) [35]", "Bomben kaufen (5) [35]" },
                  ITEMTYPE_SHOP, RG_BUY_BOMBS_535, true, &Logic::BuyBomb, RHT_BOMBS_5, false, 35);
    itemTable[RG_BUY_RED_POTION_40] = Item(
        RG_BUY_RED_POTION_40, Text{ "Buy Red Potion [40]", "Acheter: Potion Rouge [40]", "Rotes Elixier kaufen [40]" },
        ITEMTYPE_SHOP, RG_BUY_RED_POTION_40, false, &Logic::noVariable, RHT_BOTTLE_WITH_RED_POTION, false, 40);
    itemTable[RG_BUY_RED_POTION_50] = Item(
        RG_BUY_RED_POTION_50, Text{ "Buy Red Potion [50]", "Acheter: Potion Rouge [50]", "Rotes Elixier kaufen [50]" },
        ITEMTYPE_SHOP, RG_BUY_RED_POTION_50, false, &Logic::noVariable, RHT_BOTTLE_WITH_RED_POTION, false, 50);
    // Misc.
    itemTable[RG_TRIFORCE] = Item(RG_TRIFORCE, Text{ "Triforce", "Triforce", "Triforce" }, ITEMTYPE_EVENT,
                                       RG_TRIFORCE, false, &Logic::noVariable, RHT_NONE);
    itemTable[RG_HINT] = Item(RG_HINT, Text{ "Hint", "Indice", "Hinweis" }, ITEMTYPE_EVENT, RG_HINT, false,
                                   &Logic::noVariable, RHT_NONE);
    // Individual stages of progressive items (only here for GetItemEntry purposes, not for use in seed gen)
    itemTable[RG_HOOKSHOT] =
        Item(RG_HOOKSHOT, Text{ "Hookshot", "Grappin", "Fanghaken" }, ITEMTYPE_ITEM, GI_HOOKSHOT, true,
                  &Logic::ProgressiveHookshot, RHT_HOOKSHOT, ITEM_HOOKSHOT, OBJECT_GI_HOOKSHOT, GID_HOOKSHOT, 0x36, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_LONGSHOT] =
        Item(RG_LONGSHOT, Text{ "Longshot", "Super-Grappin", "Enterhaken" }, ITEMTYPE_ITEM, GI_LONGSHOT, true,
                  &Logic::ProgressiveHookshot, RHT_LONGSHOT, ITEM_LONGSHOT, OBJECT_GI_HOOKSHOT, GID_LONGSHOT, 0x4F, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_FAIRY_OCARINA] =
        Item(RG_FAIRY_OCARINA, Text{ "Fairy Ocarina", "Ocarina des fées", "Feen-Okarina" }, ITEMTYPE_ITEM,
                  GI_OCARINA_FAIRY, true, &Logic::ProgressiveOcarina, RHT_FAIRY_OCARINA, ITEM_OCARINA_FAIRY,
                  OBJECT_GI_OCARINA_0, GID_OCARINA_FAIRY, 0x4A, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_OCARINA_OF_TIME] =
        Item(RG_OCARINA_OF_TIME, Text{ "Ocarina of Time", "Ocarina du Temps", "Okarina der Zeit" }, ITEMTYPE_ITEM,
                  GI_OCARINA_OOT, true, &Logic::ProgressiveOcarina, RHT_OCARINA_OF_TIME, ITEM_OCARINA_TIME,
                  OBJECT_GI_OCARINA, GID_OCARINA_TIME, 0x3A, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BOMB_BAG] =
        Item(RG_BOMB_BAG, Text{ "Bomb Bag", "Sac de Bombes", "Bombentasche" }, ITEMTYPE_ITEM, GI_BOMB_BAG_20, true,
                  &Logic::ProgressiveBombBag, RHT_BOMB_BAG, ITEM_BOMB_BAG_20, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_20, 0x58,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BIG_BOMB_BAG] =
        Item(RG_BIG_BOMB_BAG, Text{ "Big Bomb Bag", "Grand Sac de Bombes", "Große Bombentasche" }, ITEMTYPE_ITEM,
                  GI_BOMB_BAG_30, true, &Logic::ProgressiveBombBag, RHT_BIG_BOMB_BAG, ITEM_BOMB_BAG_30,
                  OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_30, 0x59, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_BIGGEST_BOMB_BAG] = Item(
        RG_BIGGEST_BOMB_BAG, Text{ "Biggest Bomb Bag", "Énorme Sac de Bombes", "Größte Bombentasche" }, ITEMTYPE_ITEM,
        GI_BOMB_BAG_40, true, &Logic::ProgressiveBombBag, RHT_BIGGEST_BOMB_BAG, ITEM_BOMB_BAG_40, OBJECT_GI_BOMBPOUCH,
        GID_BOMB_BAG_40, 0x5A, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_FAIRY_BOW] = Item(RG_FAIRY_BOW, Text{ "Fairy Bow", "Arc des Fées", "Feen-Bogen" }, ITEMTYPE_ITEM,
                                        GI_BOW, true, &Logic::ProgressiveBow, RHT_FAIRY_BOW, ITEM_BOW, OBJECT_GI_BOW,
                                        GID_BOW, 0x31, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BIG_QUIVER] =
        Item(RG_BIG_QUIVER, Text{ "Big Quiver", "Grand carquois", "Großer Köcher" }, ITEMTYPE_ITEM, GI_QUIVER_40,
                  true, &Logic::ProgressiveBow, RHT_BIG_QUIVER, ITEM_QUIVER_40, OBJECT_GI_ARROWCASE, GID_QUIVER_40, 0x56,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_BIGGEST_QUIVER] =
        Item(RG_BIGGEST_QUIVER, Text{ "Biggest Quiver", "Énorme carquois", "Größter Köcher" }, ITEMTYPE_ITEM,
                  GI_QUIVER_50, true, &Logic::ProgressiveBow, RHT_BIGGEST_QUIVER, ITEM_QUIVER_50, OBJECT_GI_ARROWCASE,
                  GID_QUIVER_50, 0x57, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_FAIRY_SLINGSHOT] =
        Item(RG_FAIRY_SLINGSHOT, Text{ "Fairy Slingshot", "Lance-Pierre des Fées", "Feen-Schleuder" },
                  ITEMTYPE_ITEM, GI_SLINGSHOT, true, &Logic::ProgressiveBulletBag, RHT_FAIRY_SLINGSHOT, ITEM_SLINGSHOT,
                  OBJECT_GI_PACHINKO, GID_SLINGSHOT, 0x30, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BIG_BULLET_BAG] = Item(
        RG_BIG_BULLET_BAG,
        Text{ "Big Deku Seed Bullet Bag", "Grand sac de graines mojo", "Großer Deku-Samenkugelsack" }, ITEMTYPE_ITEM,
        GI_BULLET_BAG_30, true, &Logic::ProgressiveBulletBag, RHT_BIG_BULLET_BAG, ITEM_BULLET_BAG_30,
        OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG, 0x07, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_BIGGEST_BULLET_BAG] = Item(
        RG_BIGGEST_BULLET_BAG,
        Text{ "Biggest Deku Seed Bullet Bag", "Énorme sac de graines mojo", "Größte Deku-Samenkugelsack" },
        ITEMTYPE_ITEM, GI_BULLET_BAG_40, true, &Logic::ProgressiveBulletBag, RHT_BIGGEST_BULLET_BAG, ITEM_BULLET_BAG_40,
        OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG, 0x07, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_GORONS_BRACELET] =
        Item(RG_GORONS_BRACELET, Text{ "Goron's Bracelet", "Bracelet Goron", "Goronen-Armband" }, ITEMTYPE_ITEM,
                  GI_BRACELET, true, &Logic::ProgressiveStrength, RHT_GORONS_BRACELET, ITEM_BRACELET, OBJECT_GI_BRACELET,
                  GID_BRACELET, 0x79, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_SILVER_GAUNTLETS] =
        Item(RG_SILVER_GAUNTLETS, Text{ "Silver Gauntlets", "Gantelets d'argent", "Silberne Fehdehandschuhe" },
                  ITEMTYPE_ITEM, GI_GAUNTLETS_SILVER, true, &Logic::ProgressiveStrength, RHT_SILVER_GAUNTLETS,
                  ITEM_GAUNTLETS_SILVER, OBJECT_GI_GLOVES, GID_GAUNTLETS_SILVER, 0x5B, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_GOLDEN_GAUNTLETS] = Item(
        RG_GOLDEN_GAUNTLETS, Text{ "Golden Gauntlets", "Gantelets d'or", "Goldene Fehdehandschuhe" }, ITEMTYPE_ITEM,
        GI_GAUNTLETS_GOLD, true, &Logic::ProgressiveStrength, RHT_GOLDEN_GAUNTLETS, ITEM_GAUNTLETS_GOLD,
        OBJECT_GI_GLOVES, GID_GAUNTLETS_GOLD, 0x5C, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_SILVER_SCALE] =
        Item(RG_SILVER_SCALE, Text{ "Silver Scale", "Écaille d'argent", "Silberne Skala" }, ITEMTYPE_ITEM,
                  GI_SCALE_SILVER, true, &Logic::ProgressiveScale, RHT_SILVER_SCALE, ITEM_SCALE_SILVER, OBJECT_GI_SCALE,
                  GID_SCALE_SILVER, 0xCD, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_GOLDEN_SCALE] =
        Item(RG_GOLDEN_SCALE, Text{ "Golden Scale", "Écaille d'or", "Goldene Skala" }, ITEMTYPE_ITEM,
                  GI_SCALE_GOLD, true, &Logic::ProgressiveScale, RHT_GOLDEN_SCALE, ITEM_SCALE_GOLDEN, OBJECT_GI_SCALE,
                  GID_SCALE_GOLDEN, 0xCE, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ADULT_WALLET] =
        Item(RG_ADULT_WALLET, Text{ "Adult Wallet", "Grande Bourse", "Erwachsene Geldbörse" }, ITEMTYPE_ITEM,
                  GI_WALLET_ADULT, true, &Logic::ProgressiveWallet, RHT_ADULT_WALLET, ITEM_WALLET_ADULT, OBJECT_GI_PURSE,
                  GID_WALLET_ADULT, 0x5E, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_GIANT_WALLET] =
        Item(RG_GIANT_WALLET, Text{ "Giant Wallet", "Bourse de Géant", "Riesige Geldbörse" }, ITEMTYPE_ITEM,
                  GI_WALLET_GIANT, true, &Logic::ProgressiveWallet, RHT_GIANT_WALLET, ITEM_WALLET_GIANT, OBJECT_GI_PURSE,
                  GID_WALLET_GIANT, 0x5F, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_TYCOON_WALLET] = Item(
        RG_TYCOON_WALLET, Text{ "Tycoon Wallet", "Bourse de Magnat", "Reiche Geldbörse" }, ITEMTYPE_ITEM,
        RG_TYCOON_WALLET, true, &Logic::ProgressiveWallet, RHT_TYCOON_WALLET, RG_TYCOON_WALLET, OBJECT_GI_PURSE,
        GID_WALLET_GIANT, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
    itemTable[RG_DEKU_NUT_CAPACITY_30] = Item(
        RG_DEKU_NUT_CAPACITY_30,
        Text{ "Deku Nut Capacity (30)", "Capacité de noix Mojo (30)", "Deku Nuss Kapazität (30)" }, ITEMTYPE_ITEM,
        GI_NUT_UPGRADE_30, false, &Logic::noVariable, RHT_DEKU_NUT_CAPACITY_30, ITEM_NUT_UPGRADE_30, OBJECT_GI_NUTS,
        GID_NUTS, 0xA7, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_DEKU_NUT_CAPACITY_40] = Item(
        RG_DEKU_NUT_CAPACITY_40,
        Text{ "Deku Nut Capacity (40)", "Capacité de noix Mojo (40)", "Deku Nuss Kapazität (40)" }, ITEMTYPE_ITEM,
        GI_NUT_UPGRADE_40, false, &Logic::noVariable, RHT_DEKU_NUT_CAPACITY_40, ITEM_NUT_UPGRADE_40, OBJECT_GI_NUTS,
        GID_NUTS, 0xA8, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_DEKU_STICK_CAPACITY_20] = Item(
        RG_DEKU_STICK_CAPACITY_20,
        Text{ "Deku Stick Capacity (20)", "Capacité de Bâtons Mojo (20)", "Deku Stick Kapazität (20)" }, ITEMTYPE_ITEM,
        GI_STICK_UPGRADE_20, false, &Logic::noVariable, RHT_DEKU_STICK_CAPACITY_20, ITEM_STICK_UPGRADE_20,
        OBJECT_GI_STICK, GID_STICK, 0x90, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_DEKU_STICK_CAPACITY_30] = Item(
        RG_DEKU_STICK_CAPACITY_30,
        Text{ "Deku Stick Capacity (30)", "Capacité de Bâtons Mojo (30)", "Deku Stick Kapazität (30)" }, ITEMTYPE_ITEM,
        GI_STICK_UPGRADE_30, false, &Logic::noVariable, RHT_DEKU_STICK_CAPACITY_30, ITEM_STICK_UPGRADE_30,
        OBJECT_GI_STICK, GID_STICK, 0x91, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_MAGIC_SINGLE] =
        Item(RG_MAGIC_SINGLE, Text{ "Magic Meter", "Jauge de Magie", "Magisches Messgerät" }, ITEMTYPE_ITEM, 0x8A,
                  true, &Logic::ProgressiveMagic, RHT_MAGIC_SINGLE, RG_MAGIC_SINGLE, OBJECT_GI_MAGICPOT, GID_MAGIC_SMALL,
                  0xE4, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_MAGIC_DOUBLE] = Item(
        RG_MAGIC_DOUBLE, Text{ "Enhanced Magic Meter", "Jauge de Magie améliorée", "Verbesserte Magieanzeige" },
        ITEMTYPE_ITEM, 0x8A, true, &Logic::ProgressiveMagic, RHT_MAGIC_DOUBLE, RG_MAGIC_DOUBLE, OBJECT_GI_MAGICPOT,
        GID_MAGIC_LARGE, 0xE8, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_RANDOMIZER);
}

Item& StaticData::RetrieveItem(const RandomizerGet rgid) {
    return itemTable[rgid];
}

Item& StaticData::ItemFromGIID(const int giid) {
    uint32_t index = 0;
    while (index < RG_MAX) {
        if (itemTable[index].GetItemID() == giid) {
            return itemTable[index];
        }
        index++;
    }

    // there are vanilla items that don't exist in the item table we're reading from here
    // if we made it this far, it means we didn't find an item in the table
    // if we don't return anything, the game will crash, so, as a workaround, return greg
    return itemTable[RG_GREEN_RUPEE];
}

std::array<Item, RG_MAX>& StaticData::GetItemTable() {
    return itemTable;
}