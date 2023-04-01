#include "static_data.h"
#include "3drando/logic.hpp"
#include "z64object.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"

void StaticData::InitItemTable() {
    itemTable[RG_NONE] =
        RandoItem(RG_NONE, Text{ "No Item", "Rien", "Kein Artikel" }, ITEMTYPE_EVENT, GI_RUPEE_GREEN, false,
                  &Logic::noVariable, RG_NONE, ITEM_NONE, 0, 0, 0, 0, 0, ITEM_CATEGORY_JUNK, MOD_NONE);
    itemTable[RG_KOKIRI_SWORD] =
        RandoItem(RG_KOKIRI_SWORD, Text{ "Kokiri Sword", "Épée Kokiri", "Kokiri-Schwert" }, ITEMTYPE_ITEM,
                  GI_SWORD_KOKIRI, true, &Logic::KokiriSword, RG_KOKIRI_SWORD, ITEM_SWORD_KOKIRI, OBJECT_GI_SWORD_1,
                  GID_SWORD_KOKIRI, 0xA4, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE, GI_SWORD_KNIFE);
    itemTable[RG_GIANTS_KNIFE] =
        RandoItem(RG_GIANTS_KNIFE, Text{ "Giant's Knife", "Lame des Géants", "Messer des Riesen" }, ITEMTYPE_ITEM,
                  GI_SWORD_KNIFE, true, &Logic::KokiriSword, RG_GIANTS_KNIFE, ITEM_SWORD_KNIFE, OBJECT_GI_LONGSWORD,
                  GID_SWORD_BGS, 0x4B, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BIGGORON_SWORD] =
        RandoItem(RG_BIGGORON_SWORD, Text{ "Biggoron's Sword", "Épée de Biggoron", "Biggoron-Schwert" }, ITEMTYPE_ITEM,
                  GI_SWORD_BGS, true, &Logic::BiggoronSword, RG_BIGGORON_SWORD, ITEM_SWORD_BGS, OBJECT_GI_LONGSWORD,
                  GID_SWORD_BGS, 0x0C, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_DEKU_SHIELD] =
        RandoItem(RG_DEKU_SHIELD, Text{ "Deku Shield", "Bouclier Mojo", "Deku-Schild" }, ITEMTYPE_ITEM, GI_SHIELD_DEKU,
                  false, &Logic::noVariable, RG_DEKU_SHIELD, ITEM_SHIELD_DEKU, OBJECT_GI_SHIELD_1, GID_SHIELD_DEKU,
                  0x4C, 0xA0, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_HYLIAN_SHIELD] =
        RandoItem(RG_HYLIAN_SHIELD, Text{ "Hylian Shield", "Bouclier Hylien", "Hylia-Schild" }, ITEMTYPE_ITEM,
                  GI_SHIELD_HYLIAN, false, &Logic::noVariable, RG_HYLIAN_SHIELD, ITEM_SHIELD_HYLIAN, OBJECT_GI_SHIELD_2,
                  GID_SHIELD_HYLIAN, 0x4D, 0xA0, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_MIRROR_SHIELD] =
        RandoItem(RG_MIRROR_SHIELD, Text{ "Mirror Shield", "Bouclier Miroir", "Spiegel-Schild" }, ITEMTYPE_ITEM,
                  GI_SHIELD_MIRROR, true, &Logic::MirrorShield, RG_MIRROR_SHIELD, ITEM_SHIELD_MIRROR,
                  OBJECT_GI_SHIELD_3, GID_SHIELD_MIRROR, 0x4E, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_GORON_TUNIC] =
        RandoItem(RG_GORON_TUNIC, Text{ "Goron Tunic", "Tunique Goron", "Goronen-Tunika" }, ITEMTYPE_ITEM,
                  GI_TUNIC_GORON, true, &Logic::GoronTunic, RG_GORON_TUNIC, ITEM_TUNIC_GORON, OBJECT_GI_CLOTHES,
                  GID_TUNIC_GORON, 0x50, 0xA0, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_ZORA_TUNIC] =
        RandoItem(RG_ZORA_TUNIC, Text{ "Zora Tunic", "Tunique Zora", "Zora-Tunika" }, ITEMTYPE_ITEM, GI_TUNIC_ZORA,
                  true, &Logic::ZoraTunic, RG_ZORA_TUNIC, ITEM_TUNIC_ZORA, OBJECT_GI_CLOTHES, GID_TUNIC_ZORA, 0x51,
                  0xA0, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, MOD_NONE);
    itemTable[RG_IRON_BOOTS] =
        RandoItem(RG_IRON_BOOTS, Text{ "Iron Boots", "Bottes de plomb", "Eisenstiefel" }, ITEMTYPE_ITEM, GI_BOOTS_IRON,
                  true, &Logic::IronBoots, RG_IRON_BOOTS, ITEM_BOOTS_IRON, OBJECT_GI_BOOTS_2, GID_BOOTS_IRON, 0x53,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_HOVER_BOOTS] =
        RandoItem(RG_HOVER_BOOTS, Text{ "Hover Boots", "Bottes de airs", "Gleitstiefel" }, ITEMTYPE_ITEM,
                  GI_BOOTS_HOVER, true, &Logic::HoverBoots, RG_HOVER_BOOTS, ITEM_BOOTS_HOVER, OBJECT_GI_HOVERBOOTS,
                  GID_BOOTS_HOVER, 0x54, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BOOMERANG] =
        RandoItem(RG_BOOMERANG, Text{ "Boomerang", "Boomerang", "Bumerang" }, ITEMTYPE_ITEM, GI_BOOMERANG, true,
                  &Logic::Boomerang, RG_BOOMERANG, ITEM_BOOMERANG, OBJECT_GI_BOOMERANG, GID_BOOMERANG, 0x35, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_LENS_OF_TRUTH] =
        RandoItem(RG_LENS_OF_TRUTH, Text{ "Lens of Truth", "Monocle de Vérité", "Auge der Wahrheit" }, ITEMTYPE_ITEM,
                  GI_LENS, true, &Logic::LensOfTruth, RG_LENS_OF_TRUTH, ITEM_LENS, OBJECT_GI_GLASSES, GID_LENS, 0x39,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_MEGATON_HAMMER] =
        RandoItem(RG_MEGATON_HAMMER, Text{ "Megaton Hammer", "Masse des Titans", "Stahlhammer" }, ITEMTYPE_ITEM,
                  GI_HAMMER, true, &Logic::Hammer, RG_MEGATON_HAMMER, ITEM_HAMMER, OBJECT_GI_HAMMER, GID_HAMMER, 0x38,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_STONE_OF_AGONY] =
        RandoItem(RG_STONE_OF_AGONY, Text{ "Stone of Agony", "Pierre de Souffrance", "Stein der Qualen" },
                  ITEMTYPE_ITEM, GI_STONE_OF_AGONY, true, &Logic::ShardOfAgony, RG_STONE_OF_AGONY, ITEM_STONE_OF_AGONY,
                  OBJECT_GI_MAP, GID_STONE_OF_AGONY, 0x68, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_DINS_FIRE] =
        RandoItem(RG_DINS_FIRE, Text{ "Din's Fire", "Feu de Din", "Dins Feuerinferno" }, ITEMTYPE_ITEM, GI_DINS_FIRE,
                  true, &Logic::DinsFire, RG_DINS_FIRE, ITEM_DINS_FIRE, OBJECT_GI_GODDESS, GID_DINS_FIRE, 0xAD, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_FARORES_WIND] =
        RandoItem(RG_FARORES_WIND, Text{ "Farore's Wind", "Vent de Farore", "Farores Donnersturm" }, ITEMTYPE_ITEM,
                  GI_FARORES_WIND, true, &Logic::FaroresWind, RG_FARORES_WIND, ITEM_FARORES_WIND, OBJECT_GI_GODDESS,
                  GID_FARORES_WIND, 0xAE, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_NAYRUS_LOVE] =
        RandoItem(RG_NAYRUS_LOVE, Text{ "Nayru's Love", "Amour de Nayru", "Nayrus Umarmung" }, ITEMTYPE_ITEM,
                  GI_NAYRUS_LOVE, true, &Logic::NayrusLove, RG_NAYRUS_LOVE, ITEM_NAYRUS_LOVE, OBJECT_GI_GODDESS,
                  GID_NAYRUS_LOVE, 0xAF, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_FIRE_ARROWS] =
        RandoItem(RG_FIRE_ARROWS, Text{ "Fire Arrow", "Flèche de Feu", "Feuerpfeile" }, ITEMTYPE_ITEM, GI_ARROW_FIRE,
                  true, &Logic::FireArrows, RG_FIRE_ARROWS, ITEM_ARROW_FIRE, OBJECT_GI_M_ARROW, GID_ARROW_FIRE, 0x70,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ICE_ARROWS] =
        RandoItem(RG_ICE_ARROWS, Text{ "Ice Arrow", "Flèche de Glace", "Eispfeil" }, ITEMTYPE_ITEM, GI_ARROW_ICE, true,
                  &Logic::IceArrows, RG_ICE_ARROWS, ITEM_ARROW_ICE, OBJECT_GI_M_ARROW, GID_ARROW_ICE, 0x71, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_LIGHT_ARROWS] =
        RandoItem(RG_LIGHT_ARROWS, Text{ "Light Arrow", "Flèche de Lumière", "Lichtpfeil" }, ITEMTYPE_ITEM,
                  GI_ARROW_LIGHT, true, &Logic::LightArrows, RG_LIGHT_ARROWS, ITEM_ARROW_LIGHT, OBJECT_GI_M_ARROW,
                  GID_ARROW_LIGHT, 0x72, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_GERUDO_MEMBERSHIP_CARD] =
        RandoItem(RG_GERUDO_MEMBERSHIP_CARD, Text{ "Gerudo Membership Card", "Carte Gerudo", "Gerudo Karte" },
                  ITEMTYPE_ITEM, GI_GERUDO_CARD, true, &Logic::GerudoToken, RG_GERUDO_MEMBERSHIP_CARD, ITEM_GERUDO_CARD,
                  OBJECT_GI_GERUDO, GID_GERUDO_CARD, 0x7B, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_MAGIC_BEAN] =
        RandoItem(RG_MAGIC_BEAN, Text{ "Magic Bean", "Haricots Magiques", "Wundererbse" }, ITEMTYPE_ITEM, GI_BEAN, true,
                  &Logic::MagicBean, RG_MAGIC_BEAN, ITEM_BEAN, OBJECT_GI_BEAN, GID_BEAN, 0x48, 0x80, CHEST_ANIM_SHORT,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_MAGIC_BEAN_PACK] =
        RandoItem(RG_MAGIC_BEAN_PACK, Text{ "Magic Bean Pack", "Paquet de Haricots Magiques", "Wundererbsen-Packung" },
                  ITEMTYPE_ITEM, RG_MAGIC_BEAN_PACK, true, &Logic::MagicBeanPack, RG_MAGIC_BEAN_PACK,
                  RG_MAGIC_BEAN_PACK, OBJECT_GI_BEAN, GID_BEAN, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_RANDOMIZER);
    itemTable[RG_DOUBLE_DEFENSE] = 
        RandoItem(RG_DOUBLE_DEFENSE, Text{ "Double Defense", "Double Défence", "Doppelte Verteidigung" }, ITEMTYPE_ITEM,
                  RG_DOUBLE_DEFENSE, true, &Logic::DoubleDefense, RG_DOUBLE_DEFENSE, RG_DOUBLE_DEFENSE, 
                  OBJECT_GI_HEARTS,GID_HEART_CONTAINER, 0xE9, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_LESSER, 
                  MOD_RANDOMIZER);
    itemTable[RG_WEIRD_EGG] =
        RandoItem(RG_WEIRD_EGG, Text{ "Weird Egg", "Oeuf Curieux", "Seltsames Ei" }, ITEMTYPE_ITEM, GI_WEIRD_EGG, true,
                  &Logic::WeirdEgg, RG_WEIRD_EGG, ITEM_WEIRD_EGG, OBJECT_GI_EGG, GID_EGG, 0x9A, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ZELDAS_LETTER] =
        RandoItem(RG_ZELDAS_LETTER, Text{ "Zelda's Letter", "Lettre de Zelda", "Zeldas Brief" }, ITEMTYPE_ITEM,
                  GI_LETTER_ZELDA, true, &Logic::ZeldasLetter, RG_ZELDAS_LETTER, ITEM_LETTER_ZELDA, OBJECT_GI_LETTER,
                  GID_LETTER_ZELDA, 0x69, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_POCKET_EGG] =
        RandoItem(RG_POCKET_EGG, Text{ "Pocket Egg", "Oeuf de poche", "Taschenei" }, ITEMTYPE_ITEM, GI_POCKET_EGG, true,
                  &Logic::PocketEgg, RG_POCKET_EGG, ITEM_POCKET_EGG, OBJECT_GI_EGG, GID_EGG, 0x01, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_COJIRO] = 
        RandoItem(RG_COJIRO, Text{ "Cojiro", "P'tit Poulet", "Cojiro" }, ITEMTYPE_ITEM, GI_COJIRO, true, &Logic::Cojiro,       
                  RG_COJIRO, ITEM_COJIRO, OBJECT_GI_NIWATORI, GID_COJIRO, 0x02, 0x80, CHEST_ANIM_LONG,
                  ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ODD_MUSHROOM] =
        RandoItem(RG_ODD_MUSHROOM, Text{ "Odd Mushroom", "Champigon Suspect", "Seltsamer Pilz" }, ITEMTYPE_ITEM,
                  GI_ODD_MUSHROOM, true, &Logic::OddMushroom, RG_ODD_MUSHROOM, ITEM_ODD_MUSHROOM, OBJECT_GI_MUSHROOM,
                  GID_ODD_MUSHROOM, 0x03, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_ODD_POTION] =
        RandoItem(RG_ODD_POTION, Text{ "Odd Potion", "Mixture Suspecte", "Seltsamer Trank" }, ITEMTYPE_ITEM,
                  GI_ODD_POTION, true, &Logic::OddPoultice, RG_ODD_POTION, ITEM_ODD_POTION, OBJECT_GI_POWDER,
                  GID_ODD_POTION, 0x04, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_POACHERS_SAW] =
        RandoItem(RG_POACHERS_SAW, Text{ "Poacher's Saw", "Scie du Chasseur", "Wilderer-Säge" }, ITEMTYPE_ITEM, GI_SAW,
                  true, &Logic::PoachersSaw, RG_POACHERS_SAW, ITEM_SAW, OBJECT_GI_SAW, GID_SAW, 0x05, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_BROKEN_SWORD] = RandoItem(
        RG_BROKEN_SWORD, Text{ "Broken Goron's Sword", "Épée Brisée de Goron", "Schwert des gebrochenen Goronen" },
        ITEMTYPE_ITEM, GI_SWORD_BROKEN, true, &Logic::BrokenSword, RG_BROKEN_SWORD, ITEM_SWORD_BROKEN,
        OBJECT_GI_BROKENSWORD, GID_SWORD_BROKEN, 0x08, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_PRESCRIPTION] =
        RandoItem(RG_PRESCRIPTION, Text{ "Prescription", "Ordonnance", "Verschreibung" }, ITEMTYPE_ITEM,
                  GI_PRESCRIPTION, true, &Logic::Prescription, RG_PRESCRIPTION, ITEM_PRESCRIPTION,
                  OBJECT_GI_PRESCRIPTION, GID_PRESCRIPTION, 0x09, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_EYEBALL_FROG] =
        RandoItem(RG_EYEBALL_FROG, Text{ "Eyeball Frog", "Crapaud-qui-louche", "Augapfel-Frosch" }, ITEMTYPE_ITEM,
                  GI_FROG, true, &Logic::EyeballFrog, RG_EYEBALL_FROG, ITEM_FROG, OBJECT_GI_FROG, GID_FROG, 0x0D, 0x80,
                  CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_EYEDROPS] =
        RandoItem(RG_EYEDROPS, Text{ "World's Finest Eyedrops", "Super Gouttes", "Supertropfen" }, ITEMTYPE_ITEM,
                  GI_EYEDROPS, true, &Logic::Eyedrops, RG_EYEDROPS, ITEM_EYEDROPS, OBJECT_GI_EYE_LOTION, GID_EYEDROPS,
                  0x0E, 0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_CLAIM_CHECK] =
        RandoItem(RG_CLAIM_CHECK, Text{ "Claim Check", "Certificat", "Quittung" }, ITEMTYPE_ITEM, GI_CLAIM_CHECK, true,
                  &Logic::ClaimCheck, RG_CLAIM_CHECK, ITEM_CLAIM_CHECK, OBJECT_GI_TICKETSTONE, GID_CLAIM_CHECK, 0x0A,
                  0x80, CHEST_ANIM_LONG, ITEM_CATEGORY_MAJOR, MOD_NONE);
    itemTable[RG_GOLD_SKULLTULA_TOKEN] = 
        RandoItem(RG_GOLD_SKULLTULA_TOKEN,
                  Text{ "Gold Skulltula Token", "Symbole de Skulltula d'Or", "Goldenes Skulltula-Symbol" },
                  ITEMTYPE_TOKEN, GI_SKULL_TOKEN, true, &Logic::GoldSkulltulaTokens, RG_GOLD_SKULLTULA_TOKEN,
                  ITEM_SKULL_TOKEN, OBJECT_GI_SUTARU, GID_SKULL_TOKEN, 0xB4, 0x80, CHEST_ANIM_SHORT,
                  ITEM_CATEGORY_SKULLTULA_TOKEN, MOD_NONE);
    
}