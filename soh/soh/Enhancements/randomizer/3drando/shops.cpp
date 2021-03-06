#include "item_location.hpp"
#include "item_pool.hpp"
#include "location_access.hpp"
#include "random.hpp"
#include "item.hpp"
#include "shops.hpp"
#include "debug.hpp"

#include <math.h>
#include <map>
#include "z64item.h"

using namespace Settings;

std::vector<ItemAndPrice> NonShopItems = {};

static std::array<std::array<Text, 3>, 0xD5> trickNameTable; //Table of trick names for ice traps
bool initTrickNames = false; //Indicates if trick ice trap names have been initialized yet

//Set vanilla shop item locations before potentially shuffling
void PlaceVanillaShopItems() {
  //Loop to place vanilla items in each location
  for (size_t i = 0; i < ShopLocationLists.size(); i++) {
    for (size_t j = 0; j < ShopLocationLists[i].size(); j++) {
      Location(ShopLocationLists[i][j])->PlaceVanillaItem();
    }
  }
}

//These are the vanilla shop items, but in a priority order of importance
//However many shop item slots were cleared, this will return 64 minus that number of vanilla shop items to be placed with assumed fill
//The first 32 items here will always be present in shops
//Shopsanity 4 will only have the first 32, shopsanity 1 will have the first 56, etc.
//Shopsanity random will have anywhere from the first 32 to the first 56, so the order of items after 32 is relevant
std::vector<uint32_t> GetMinVanillaShopItems(int total_replaced) {
    std::vector<uint32_t> minShopItems = {
    BUY_DEKU_SHIELD,
    BUY_HYLIAN_SHIELD,
    BUY_GORON_TUNIC,
    BUY_ZORA_TUNIC,
    BUY_DEKU_NUT_5,
    BUY_DEKU_NUT_5,
    BUY_DEKU_NUT_10,
    BUY_DEKU_STICK_1,
    BUY_DEKU_STICK_1,
    BUY_DEKU_SEEDS_30,
    BUY_ARROWS_10,
    BUY_ARROWS_10,
    BUY_ARROWS_30,
    BUY_ARROWS_50,
    BUY_BOMBCHU_5,
    BUY_BOMBCHU_10,
    BUY_BOMBCHU_10,
    BUY_BOMBCHU_20,
    BUY_BOMBS_525,
    BUY_BOMBS_535,
    BUY_BOMBS_10,
    BUY_BOMBS_20,
    BUY_GREEN_POTION,
    BUY_RED_POTION_30,
    BUY_BLUE_FIRE,
    BUY_FAIRYS_SPIRIT,
    BUY_BOTTLE_BUG,
    BUY_FISH,
    //^First 28 items from OoTR
    BUY_HYLIAN_SHIELD,
    BUY_BOTTLE_BUG,
    BUY_DEKU_STICK_1,
    BUY_FAIRYS_SPIRIT,
    //^First 32 items: Always guaranteed
    BUY_BLUE_FIRE,
    BUY_FISH,
    BUY_BOMBCHU_10,
    BUY_DEKU_NUT_5,
    BUY_ARROWS_10,
    BUY_BOMBCHU_20,
    BUY_BOMBS_535,
    BUY_RED_POTION_30,
    //^First 40 items: Exist on shopsanity 3 or less
    BUY_BOMBS_30,
    BUY_BOMBCHU_20,
    BUY_DEKU_NUT_5,
    BUY_ARROWS_10,
    BUY_DEKU_NUT_5,
    BUY_ARROWS_30,
    BUY_RED_POTION_40,
    BUY_FISH,
    //^First 48 items: Exist on shopsanity 2 or less
    BUY_BOMBCHU_20,
    BUY_ARROWS_30,
    BUY_RED_POTION_50,
    BUY_ARROWS_30,
    BUY_DEKU_NUT_5,
    BUY_ARROWS_50,
    BUY_ARROWS_50,
    BUY_GREEN_POTION,
    //^First 56 items: Exist on shopsanity 1 or less
    BUY_POE,
    BUY_POE,
    BUY_HEART,
    BUY_HEART,
    BUY_HEART,
    BUY_HEART,
    BUY_HEART,
    BUY_HEART,
    //^All 64 items: Only exist with shopsanity 0
  };
  //Now delete however many items there are to replace
  for (int i = 0; i < total_replaced; i++) {
    minShopItems.pop_back();
  }
  return minShopItems;
}

//This table contains a cumulative probability for each possible shop price based on
// a beta distribution with alpha = 1.5, beta = 2, and the result of the distribution, a float in [0.0, 1.0),
// being mutliplied by 60, casted to an integer, then multiplied by 5 to give a value in range [0, 295] in increments of 5.
// Meaning the first value is the probability of 0, the next value is the probability of 0 plus the probability of 5, etc.
//Probabilities generated using a python script with 1 billion trials, so should hopefully be pretty good
//Average price ~126
//~38% chance of needing no wallet, ~45% chance of needing 1, ~17% chance of needing 2
static constexpr std::array<double, 60> ShopPriceProbability= {
  0.005326994, 0.014908518, 0.027114719, 0.041315285, 0.057136304, 0.074325887, 0.092667151, 0.112002061, 0.132198214, 0.153125390,
  0.174696150, 0.196810540, 0.219388148, 0.242361379, 0.265657012, 0.289205134, 0.312970402, 0.336877590, 0.360881110, 0.384932772,
  0.408976198, 0.432982176, 0.456902494, 0.480686053, 0.504313389, 0.527746488, 0.550938554, 0.573856910, 0.596465330, 0.618736235,
  0.640646600, 0.662162782, 0.683240432, 0.703859801, 0.724001242, 0.743631336, 0.762722631, 0.781259986, 0.799198449, 0.816521905,
  0.833208595, 0.849243398, 0.864579161, 0.879211177, 0.893112051, 0.906263928, 0.918639420, 0.930222611, 0.940985829, 0.950914731,
  0.959992180, 0.968187000, 0.975495390, 0.981884488, 0.987344345, 0.991851853, 0.995389113, 0.997937921, 0.999481947, 1.000000000,
};
int GetRandomShopPrice() {
  double random = RandomDouble(); //Randomly generated probability value
  for (size_t i = 0; i < ShopPriceProbability.size(); i++) {
    if (random < ShopPriceProbability[i]) {
      //The randomly generated value has surpassed the total probability up to this point, so this is the generated price
      return i * 5; //i in range [0, 59], output in range [0, 295] in increments of 5
    }
  }
  return -1; //Shouldn't happen
}

//Similar to above, beta distribution with alpha = 1, beta = 2,
// multiplied by 20 instead of 60 to give values in rage [0, 95] in increments of 5
//Average price ~31
static constexpr std::array<double, 20> ScrubPriceProbability = {
  0.097500187, 0.190002748, 0.277509301, 0.360018376, 0.437522571, 0.510021715, 0.577520272, 0.640029304, 0.697527584, 0.750024535,
  0.797518749, 0.840011707, 0.877508776, 0.910010904, 0.937504342, 0.960004661, 0.977502132, 0.989998967, 0.997500116, 1.000000000,
};

int16_t GetRandomScrubPrice() {
    double random = RandomDouble();
    for (size_t i = 0; i < ScrubPriceProbability.size(); i++) {
        if (random < ScrubPriceProbability[i]) {
            return i * 5; // i in range [0, 19], output in range [0, 95] in increments of 5
        }
    }
    return -1;
}

//Get 1 to 4, or a random number from 1-4 depending on shopsanity setting
int GetShopsanityReplaceAmount() {
  if (Settings::Shopsanity.Is(SHOPSANITY_ONE)) {
    return 1;
  } else if (Settings::Shopsanity.Is(SHOPSANITY_TWO)) {
    return 2;
  } else if (Settings::Shopsanity.Is(SHOPSANITY_THREE)) {
    return 3;
  } else if (Settings::Shopsanity.Is(SHOPSANITY_FOUR)) {
    return 4;
  } else { //Random, get number in [1, 4]
    return Random(1, 5);
  }
}

//Initialize the table of trick names with an easy, medium, and hard name for each language
void InitTrickNames() {
  trickNameTable[GI_SWORD_KOKIRI] = {
     Text{"Korok Sword", "??p??e Korok", "Espada Korok"},
     Text{"Hero's Sword", "??p??e du H??ros", "Espada del h??roe"},
     Text{"Razor Sword", "Lame Rasoir", "Espada de esmeril"}};
/*  trickNameTable[GI_SWORD_MASTER] = {
     Text{"Goddess Sword", "??p??e de la d??esse", "Espada Divina"},
     Text{"Gilded Sword", "Excalibur", "Espada de los Sabios"},
     Text{"Magical Sword", "Lame dor??e", "Fay"}};*/
  trickNameTable[GI_SWORD_KNIFE] = {
     Text{"Big Goron's Sword", "??p??e de Gros Goron", "Espada de Big Goron"},
     Text{"Fierce Deity's Sword", "??p??e du Dieu D??mon", "Espada de la Fiera Deidad"},
     Text{"Biggoron's Knife", "Lame de Grogoron", "Daga de Biggoron"}};
  trickNameTable[GI_SWORD_BGS] = {
     Text{"Big Goron's Sword", "??p??e de Biggoron", "Espada de Big Goron"},
     Text{"Fierce Deity's Sword", "??p??e du dieu d??mon", "Espada de la Fiera Deidad"},
     Text{"Biggoron's Knife", "Lame de Grogoron", "Daga de Biggoron"}};
  trickNameTable[GI_SHIELD_DEKU] = {
     Text{"Boko Shield", "Bouclier Boko", "Escudo Boko"},
     Text{"Ordon Shield", "Bouclier de Toal", "Escudo de Ordon"},
     Text{"Wooden Shield", "Bouclier de Bois", "Escudo de madera"}};
  trickNameTable[GI_SHIELD_HYLIAN] = {
     Text{"Hyrule Shield", "Bouclier d'Hyrule", "Escudo Hylian"},
     Text{"Goddess Shield", "Bouclier Sacr??", "Escudo Divino"},
     Text{"Hero's Shield", "Bouclier du H??ros", "Escudo del h??roe"}};
  trickNameTable[GI_SHIELD_MIRROR] = {
     Text{"Magic Mirror", "Miroir Magique", "Escudo m??gico"},
     Text{"Magical Shield", "Bouclier Magique", "Escudo arcano"},
     Text{"Mirror of Twilight", "Miroir des Ombres", "Espejo del Crep??sculo"}};
  trickNameTable[GI_TUNIC_GORON] = {
     Text{"Gerudo Tunic", "Tunique Gerudo", "Sayo gerudo"},
     Text{"Magic Armor", "Armure Magique", "T??nica Goron"},
     Text{"Red Mail", "Habits Rouges", "Ropas rojas"}};
  trickNameTable[GI_TUNIC_ZORA] = {
     Text{"Rito Tunic", "Tunique Rito", "Sayo rito"},
     Text{"Zora Armor", "Armure Zora", "T??nica Zora"},
     Text{"Blue Mail", "Habits Bleus", "Ropas azules"}};
  trickNameTable[GI_BOOTS_IRON] = {
     Text{"Iron Hoofs", "Patins de Plomb", "Botas f??rreas"},
     Text{"Snow Boots", "Bottes de Neige", "Botas de nieve"},
     Text{"Boots of Power", "Bottes de Puissance", "Botas de plomo"}};
  trickNameTable[GI_BOOTS_HOVER] = {
     Text{"Hover Hoofs", "Patins des airs", "Botas flotadoras"},
     Text{"Pegasus Boots", "Bottes p??gase", "Botas de Pegaso"},
     Text{"Boots of Speed", "Bottes de vitesse", "Botas del desierto"}};
  trickNameTable[GI_WEIRD_EGG] = {
     Text{"Poached Egg", "Oeuf ?? la coque", "Huevo pasado"},
     Text{"Lon Lon Egg", "Oeuf Lon Lon", "Huevo Lon Lon"},
     Text{"Zora Egg", "Oeuf Zora", "Huevo Zora"}};
  trickNameTable[GI_LETTER_ZELDA] = {
     Text{"Ruto's Letter", "Lettre de Ruto", "Carta de Ruto"},
     Text{"Royal Letter", "Lettre Eoyale", "Carta para Kafei"},
     Text{"Zelda's Business Card", "Carte d'affaires de Zelda", "Carta"}};
  trickNameTable[GI_BOOMERANG] = {
     Text{"Prank Fetch Toy", "Ins??parable B??tonnet", "Bumerang"},
     Text{"Gale Boomerang", "Boomerang Tornade", "Bumer??n tornado"},
     Text{"Magic Boomerang", "Boomerang Magique", "Bumer??n m??gico"}};
  trickNameTable[GI_LENS] = {
     Text{"Sheikah-leidoscope", "Sheikah-l??idoscope", "Mon??culo de la Verdad"},
     Text{"Sheikah Sensor", "Sonar Sheikah", "Sensor Sheikah"},
     Text{"Magnifying Lens", "Loupe", "Lente Aumentadora"}};
  trickNameTable[GI_HAMMER] = {
     Text{"Goron Gavel", "Masse Perforatrice", "Mazo Goron"},
     Text{"Magic Hammer", "Marteau Magique", "Martillo m??gico"},
     Text{"Skull Hammer", "Maillet Ressort", "Martillo de hierro"}};
  trickNameTable[GI_STONE_OF_AGONY] = {
     Text{"Shard of Agahnim", "Fragment d'Agahnim", "Piedra de Agahnim"},
     Text{"Stone of Agony", "Pierre de Souffrance", "Fragmento de la Agon??a"},
     Text{"Pirate's Charm", "Pierre de Pirate", "Amuleto Pirata"}};
  trickNameTable[GI_DINS_FIRE] = {
     Text{"Eldin's Fire", "Feu d'Eldin", "Fuego de Eldin"},
     Text{"Din's Blaze", "Flamme de Din", "Poder de Din"},
     Text{"Din's Pearl", "Perle de Din", "Orbe de Din"}};
  trickNameTable[GI_FARORES_WIND] = {
     Text{"Faron's Wind", "Vent de Firone", "Viento de Farone"},
     Text{"Farore's Windfall", "Z??phyr de Farore", "Valor de Farore"},
     Text{"Farore's Pearl", "Perle de Farore", "Orbe de Farore"}};
  trickNameTable[GI_NAYRUS_LOVE] = {
     Text{"Lanayru's Love", "Amour de Lanelle", "Amor de Lanayru"},
     Text{"Nayru's Passion", "Passion de Nayru", "Sabidur??a de Nayru"},
     Text{"Nayru's Pearl", "Perle de Nayru", "Orbe de Nayru"}};
  trickNameTable[GI_ARROW_FIRE] = {
     Text{"Soul Arrow", "Fl??che des Esprits", "Flecha del Esp??ritu"},
     Text{"Bomb Arrow", "Fl??che-Bombe", "Flecha bomba"},
     Text{"Fire Candy", "Bonbon deFfeu", "Cetro de fuego"}};
  trickNameTable[GI_ARROW_ICE] = {
     Text{"Shadow Arrow", "Fl??che d'Ombre", "Flecha de las Sombras"},
     Text{"Ancient Arrow", "Fl??che Arch??onique", "Flecha ancestral"},
     Text{"Ice Trap Arrow", "Fl??che de Pi??ge de Glace", "Cetro de hielo"}};
  trickNameTable[GI_ARROW_LIGHT] = {
     Text{"Wind Arrow", "Fl??che de Vent", "Flecha del Viento"},
     Text{"Shock Arrow", "Fl??ches ??lectriques", "Flecha el??ctrica"},
     Text{"Silver Arrow", "Fl??ches d'Argent", "Flecha de plata"}};
  trickNameTable[GI_GERUDO_CARD] = {
     Text{"Desert Title Deed", "Abonnement Gerudo", "Escritura del desierto"},
     Text{"Gerudo's Card", "Carte Goron", "T??ken Gerudo"},
     Text{"Gerudo's Membership Card", "Autographe de Nabooru", "Tarjeta Gerudo"}};

  trickNameTable[0xC9] = {
     Text{"Funky Bean Pack", "Paquet de F??ves Magiques", "Lote de frijoles m??gicos"},
     Text{"Crenel Bean Pack", "Paquet de Haricots Gonggle", "Lote de alubias m??gicas"},
     Text{"Mystic Bean Pack", "Paquet de Haricots Mystiques", "Lote de porotos m??gicos"}};
  trickNameTable[0xB8] = {
     Text{"Diamond Hearts", "Coeurs de Diamant", "Contenedor de diamante"},
     Text{"Double Damage", "Double Souffrance", "Doble da??o receptivo"},
     Text{"Quadruple Defence", "Quadruple D??fence", "Defensa cu??druple"}};

  trickNameTable[GI_POCKET_EGG] = {
     Text{"Poached Egg", "oeuf ?? la coque", "Huevo pasado"},
     Text{"Lon Lon Egg", "oeuf Lon Lon", "Huevo Lon Lon"},
     Text{"Zora Egg", "oeuf Zora", "Huevo del Pez Viento"}};
  trickNameTable[GI_POCKET_CUCCO] = {
     Text{"D.I.Y. Alarm Clock", "R??veille-matin improvis??", "Alarma emplumada port??til"},
     Text{"Kakariko Cucco", "Cocotte Cocorico", "Cuco de Kakariko"},
     Text{"Hatched Cucco", "Cocotte ??close", "Pollo de bolsillo"}};
  trickNameTable[GI_COJIRO] = {
     Text{"Blucco", "Chair-Qui-Poule", "Cucazul"},
     Text{"Grog's Cucco", "Cocotte de Grog", "Cuco de Grog"},
     Text{"Corijo", "Cojiro", "Corijo"}};
  trickNameTable[GI_ODD_MUSHROOM] = {
     Text{"Magic Mushroom", "Champignon magique", "Champi????n m??gico"},
     Text{"Endura Shroom", "Champi Vigueur", "Champi????n del bosque"},
     Text{"Mushroom", "Champignon", "Seta"}};
  trickNameTable[GI_ODD_POTION] = {
     Text{"Odd Medicine", "??lixir suspect", "Poci??n rara"},
     Text{"Granny's Poultice", "Mixture de Granny", "Medicina de la abuela"},
     Text{"Mushroom Poultice", "Mixture de champignon", "Medicina de champi??ones"}};
  trickNameTable[GI_SAW] = {
     Text{"Carpenter's Saw", "Scie du charpentier", "Sierra del carpintero"},
     Text{"Poacher's Sword", "Hache du chasseur", "Espada del capataz"},
     Text{"Grog's Saw", "Scie de Grog", "Sierra del Cazador Furtivo"}};
  trickNameTable[GI_SWORD_BROKEN] = {
     Text{"Broken Biggoron's Sword", "??p??e bris??e de Grogoron", "Espada de Biggoron rota"},
     Text{"Broken Giant's Knife", "Lame des G??ants bris??e", "Daga gigante rota"},
     Text{"Biggoron's Sword", "??p??e de Biggoron", "Espada de Biggoron"}};
  trickNameTable[GI_PRESCRIPTION] = {
     Text{"Biggoron's Prescription", "Ordonnance de Grogoron", "Receta de Biggoron"},
     Text{"Eyedrop Prescription", "Ordonnance de gouttes", "Receta ocular"},
     Text{"Urgent Prescription", "Ordonnance urgente", "Prescripci??n"}};
  trickNameTable[GI_FROG] = {
     Text{"Don Gero", "Don Gero", "Don Gero"},
     Text{"Eyedrop Frog", "Grenouille-qui-louche", "Globo Ocular de Rana"},
     Text{"Frog", "Crapaud", "Rana"}};
  trickNameTable[GI_EYEDROPS] = {
     Text{"Biggoron's Eyedrops", "Gouttes de Grogoron", "Gotas de Biggoron"},
     Text{"Hyrule's Finest Eyedrops", "Eau du Lac Hylia", "Gotas oculares"},
     Text{"Zora Perfume", "Parfum Zora", "Perfume Zora"}};
  trickNameTable[GI_CLAIM_CHECK] = {
     Text{"Clay Check", "Certificat Grogoron", "Comprobante de Reclamaci??n"},
     Text{"Sheikah Slate", "Tablette Sheikah", "Piedra Sheikah"},
     Text{"Cyclone Slate", "Ardoise des tornades", "Pizarra de los Torbellinos"}};

  trickNameTable[GI_SKULL_TOKEN] = {
     Text{"Skulltula Token", "Bon de Skulltula dor??e", "S??mbolo de Skulltula"},
     Text{"Golden Skulltula Spirit", "Pi??ce de Skulltula dor??e", "T??ken de Skulltula Dorada"},
     Text{"Gold Walltula Token", "Jeton de Walltula dor??e", "Skulltula dorada"}};

  trickNameTable[0x80] = {
     Text{"Progressive Grappling Hook", "Lance-cha??ne (prog.)", "Garra progresiva"},
     Text{"Progressive Clawshot", "Grappin-griffe (prog.)", "Zarpa progresiva"},
     Text{"Progressive Gripshot", "Grappince (prog.)", "Enganchador progresivo"}};
  trickNameTable[0x81] = {
     Text{"Progressive Glove", "Gant de puissance (prog.)", "Guanteletes progresivos"},
     Text{"Progressive Power Bracelet", "Bracelet de force (prog.)", "Brasaletes progresivos"},
     Text{"Progressive Magic Bracelet", "Bracelet magique (prog.)", "Manoplas progresivas"}};
  trickNameTable[0x82] = {
     Text{"Progressive Bomb Capacity", "Capacit?? de bombes (prog.)", "Mayor capacidad de bombas"},
     Text{"Progressive Bomb Pack", "Paquet de bombes (prog.)", "Zurr??n de bombas progresivo"},
     Text{"Progressive Bomb Box", "Bo??te ?? bombes (prog.)", "Bolsa de bombas progresiva"}};
  trickNameTable[0x83] = {
     Text{"Progressive Arrow Capacity", "Capacit?? de fl??ches (prog.)", "Mayor capacidad de flechas"},
     Text{"Progressive Hero's Bow", "Arc du h??ros (prog.)", "Arco del h??roe progresivo"},
     Text{"Progressive Arrow Holder", "Arbal??te (prog.)", "Ballesta progresiva"}};
  trickNameTable[0x84] = {
     Text{"Progressive Seed Capacity", "Capacit?? de graines (prog.)", "Mayor capacidad de semillas"},
     Text{"Progressive Scattershot", "Lance-Pierre rafale (prog.)", "Resortera m??ltiple progresiva"},
     Text{"Progressive Seed Satchel", "Sac de graines (prog.)", "Bolsa de semillas progresiva"}};
  trickNameTable[0x85] = {
     Text{"Progressive Rupee Capacity", "Capacit?? de rubis (prog.)", "Mayor capacidad de rupias"},
     Text{"Progressive Purse", "Sacoche (prog.)", "Cartera de rupias progresiva"},
     Text{"Progressive Rupee Bag", "Sac ?? rubis (prog.)", "Zurr??n de rupias progresivo"}};
  trickNameTable[0x86] = {
     Text{"Progressive Diving Ability", "Plong??e (prog.)", "Buceo progresivo"},
     Text{"Progressive Pearl", "Perle (prog.)", "Perla progresiva"},
     Text{"Progressive Scute", "Bulle (prog.)", "Fragmento Zora progresivo"}};
  trickNameTable[0x87] = {
     Text{"Progressive Nut Pack", "Paquet de noix (prog.)", "Mayor capacidad de semillas"},
     Text{"Progressive Nut Bag", "Sac de noix (prog.)", "Bolsa de nueces progresiva"},
     Text{"Progressive Husk Capacity", "Capacit?? de noisettes (prog.)", "Mayor capacidad de casta??as"}};
  trickNameTable[0x88] = {
     Text{"Progressive Stick Pack", "Paquet de b??tons Mojo (prog.)", "Mayor capacidad de bastones"},
     Text{"Progressive Stick Bag", "Sac de b??tons (prog.)", "Mayor capacidad de ramas deku"},
     Text{"Progressive Rod Capacity", "Capacit?? de tiges (prog.)", "Mayor capacidad de cetros deku"}};
  trickNameTable[0x89] = {
     Text{"Progressive Bomblings", "Bombinsectes (prog.)", "Bombinsectos progresivos"},
     Text{"Progressive Missiles", "Missiles (prog.)", "Misiles progresivos"},
     Text{"Progressive Bombchu Bag", "Sac ?? Bombchu (prog.)", "Bombachus progresivos"}};
  trickNameTable[0x8A] = {
     Text{"Progressive Stamina Meter", "Jauge d'endurance (prog.)", "Medidor de vigor progresivo"},
     Text{"Progressive Energy Meter", "Jauge d'??nergie (prog.)", "Medidor de energ??a progresivo"},
     Text{"Progressive Magic Powder", "Poudre magique (prog.)", "Medidor de carga progresivo"}};
  trickNameTable[0x8B] = {
     Text{"Progressive Memento", "Souvenir (prog.)", "Silbato progresivo"},
     Text{"Progressive Flute", "Fl??te (prog.)", "Flauta progresiva"},
     Text{"Progressive Recorder", "Harmonica (prog.)", "Arm??nica progresiva"}};
  trickNameTable[0xD4] = {
     Text{"Progressive Titan Blade", "Lame des Titans (prog.)", "Hoja del Tit??n progresiva"},
     Text{"Progressive Goron Knife", "Lame Goron (prog.)", "Daga Goron progresiva"},
     Text{"Progressive Giant Sword", "??p??e g??ante (prog.)", "Espada gigante progresiva"}};

  trickNameTable[0x0F] = {
     Text{"Magic Bottle", "Flacon magique", "Frasco fe??rico"},
     Text{"Glass Bottle", "Flacon de verre", "Botella de cristal"},
     Text{"Bottle with Water", "Flacon d'eau", "Botella Tingle"}};
  trickNameTable[0x14] = {
     Text{"Bottle with Chateau Romani", "Flacon de cuv??e Romani", "Botella de Reserva Romani"},
     Text{"Bottle with Fresh Milk", "Flacon de lait frais", "Botella de leche fresca"},
     Text{"Bottle with Mystery Milk", "Flacon de lait grand cru", "Botella de leche extra"}};
  trickNameTable[0x8C] = {
     Text{"Bottle with Red Chu Jelly", "Flacon de gel??e Chuchu rouge", "Jugo de Chuchu Rojo"},
     Text{"Bottle with Medicine of Life", "Flacon d'??lixir rouge", "Botella de medicina de la vida"},
     Text{"Bottle with Heart Potion", "Flacon de potion de soin", "Botella de poci??n de salud"}};
  trickNameTable[0x8D] = {
     Text{"Bottle with Green Chu Jelly", "Flacon de gel??e Chuchu verte", "Jugo de Chuchu Verde"},
     Text{"Bottle with Medicine of Magic", "Flacon d'??lixir vert", "Botella de medicina m??gica"},
     Text{"Bottle with Stamina Potion", "Flacon d'Endurol", "Botella de elixir vigorizante"}};
  trickNameTable[0x8E] = {
     Text{"Bottle with Blue Chu Jelly", "Flacon de gel??e Chuchu bleue", "Jugo de Chuchu Azul"},
     Text{"Bottle with Water of Life", "Flacon d'??lixir bleu", "Botella de agua de la vida"},
     Text{"Bottle with Air Potion", "Flacon de potion d'oxyg??ne", "Botella de ox??geno"}};
  trickNameTable[0x8F] = {
     Text{"Bottle with Forest Firefly", "Flacon avec une luciole", "Luci??rnaga del bosque"},
     Text{"Bottle with Faerie", "Flacon de poudre f????rique", "Gran Hada embotellada"},
     Text{"Bottle with Stray Fairy", "Flacon avec une f??e perdue", "Hada perdida en una botella"}};
  trickNameTable[0x90] = {
     Text{"Bottle with Small Jabu-Jabu", "Flacon avec mini Jabu-Jabu", "Lord Chapu-Chapu embotellado"},
     Text{"Bottle with Hyrule Bass", "Flacon avec perche d'Hyrule", "Locha de Hyrule embotellada"},
     Text{"Bottle with Hyrule Loach", "Flacon avec loche d'Hyrule", "Perca de T??rmina embotellada"}};
  trickNameTable[0x91] = {
     Text{"Bottle with Will-O-Wisp", "Flacon avec feu follet", "Botella de llama azul"},
     Text{"Bottle with Ancient Flame", "Flacon de flamme ancienne", "Botella de fuego ancestral"},
     Text{"Bottle with Nayru's Flame", "Flacon de flamme de Nayru", "Botella de llamas de Nayru"}};
  trickNameTable[0x92] = {
     Text{"Bottle with Baby Tektites", "Flacon de b??b?? Araknon", "Tektites en una botella"},
     Text{"Bottle with Lanayru Ants", "Flacon de fourmis de Lanelle", "Celestarabajo embotellado"},
     Text{"Bottle with Insects", "Flacon de bibittes", "Saltabosques embotellados"}};
  trickNameTable[0x94] = {
     Text{"Bottle with Ghini", "Flacon avec Ghini", "Ghini en una botella"},
     Text{"Bottle with Imp Poe", "Flacon avec Spectre", "Espectro en una botella"},
     Text{"Bottle with Anti-Fairy", "Flacon avec Tetdoss", "Whisp en una botella"}};

  trickNameTable[0x15] = {
     Text{"Bottle with Maggie's Letter", "Flacon avec lettre de Maggy", "Carta de Dolores"},
     Text{"Bottle with Letter to Kafei", "Flacon avec lettre pour Kafei", "Carta para Kafei"},
     Text{"Bottle with Zelda's Letter", "Flacon avec Lettre de Zelda", "Carta n??utica"}};
  trickNameTable[0x93] = {
     Text{"Bottle with Composer Brother", "Flacon avec un compositeur", "Hermana Poe embotellada"},
     Text{"Bottle with Jalhalla", "Flacon avec Jalhalla", "Yaihalla embotellado"},
     Text{"Bottle with Grim Repoe", "Flacon avec le Faucheur", "Bubble en una botella"}};

  trickNameTable[0xC1] = {
     Text{"Ballad of the Goddess", "Chant de la d??esse", "C??ntico de la Diosa"},
     Text{"Song of Healing", "Chant de l'apaisement", "Canci??n de curaci??n"},
     Text{"Bolero of Fire", "Bol??ro du feu", "Bolero del fuego"}};
  trickNameTable[0xC2] = {
     Text{"Earth God's Lyric", "Hymne du dieu de la terre", "Melod??a del Esp??ritu de la Tierra"},
     Text{"Song of Soaring", "Chant de l'envol", "Canci??n del viento"},
     Text{"Requiem of Spirit", "Requiem des esprits", "R??quiem del esp??ritu"}};
  trickNameTable[0xC3] = {
     Text{"Wind God's Aria", "Hymne du dieu du vent", "Melod??a del Esp??ritu del Viento"},
     Text{"Wind's Requiem", "M??lodie du vent", "Melod??a del Viento"},
     Text{"Minuet of Forest", "Menuet de la for??t", "Minueto del bosque"}};
  trickNameTable[0xC4] = {
     Text{"Song of Passing", "Mambo de Manbo", "Melod??a del transcurrir"},
     Text{"Command Melody", "Air du marionnettiste", "Cara al Sol"},
     Text{"Prelude of Light", "Pr??lude de la lumi??re", "Preludio de la luz"}};
  trickNameTable[0xC5] = {
     Text{"Song of Double Time", "Chant acc??l??r??", "Canci??n del doble tiempo"},
     Text{"Inverted Song of Time", "Chant du temps invers??", "Canci??n del tiempo invertida"},
     Text{"Serenade of Water", "S??r??nade de l'eau", "Serenata del agua"}};
  trickNameTable[0xC6] = {
     Text{"Ballad of Gales", "Requiem de la tornade", "Melod??a del Tornado"},
     Text{"Frog's Song of Soul", "Rap des grenouilles", "Canci??n del alma de la rana"},
     Text{"Nocturne of Shadow", "Nocturne de l'ombre", "Nocturno de la sombra"}};
  trickNameTable[0xBB] = {
     Text{"Saria's Karaoke", "Karaok?? de Saria", "Dueto del bosque"},
     Text{"Sonata of Awakening", "Sonate de l'??veil", "Sonata del despertar"},
     Text{"Saria's Song", "Chant de Saria", "Canci??n de Saria"}};
  trickNameTable[0xBC] = {
     Text{"Darunia's Tango", "Tango de Darunia", "Coro del fuego"},
     Text{"Goron Lullaby", "Berceuse des Gorons", "Nana goron"},
     Text{"Zelda's Lullaby", "Berceuse de Zelda", "Nana de Zelda"}};
  trickNameTable[0xBD] = {
     Text{"Ruto's Blues", "Blues de Ruto", "Sonata del agua"},
     Text{"New Wave Bossa Nova", "Bossa-nova des flots", "Bossanova de las olas"},
     Text{"Song of Time", "Chant du temps", "Canci??n del tiempo"}};
  trickNameTable[0xBE] = {
     Text{"Nabooru's Reggae", "Reggae de Nabooru", "Reggae del esp??ritu"},
     Text{"Elegy of Emptiness", "Hymne du vide", "Eleg??a al vac??o"},
     Text{"Epona's Song", "Chant d'??pona", "Canci??n de Epona"}};
  trickNameTable[0xBF] = {
     Text{"Impa's Death Metal", "Death m??tal d'Impa", "Diurno de la sombra"},
     Text{"Oath to Order", "Ode de l'appel", "Oda al orden"},
     Text{"Song of Storms", "Chant des temp??tes", "Canci??n de la tormenta"}};
  trickNameTable[0xC0] = {
     Text{"Rauru's Sing-Along", "Chansonnette de Rauru", "Predulio de luz"},
     Text{"Ballad of the Wind Fish", "Ballade sur Poisson-R??ve", "Balada del Piez Viento"},
     Text{"Sun's Song", "Chant du soleil", "Canci??n del Sol"}};

  trickNameTable[0xCB] = {
     Text{"Pendant of Courage", "Pendentif du courage", "Colgante del valor"},
     Text{"Farore's Emerald", "??meraude de Farore", "Esmeralda de Farore"},
     Text{"Kokiri's Peridot", "P??ridot Kokiri", "??gata de los Kokiri"}};
  trickNameTable[0xCC] = {
     Text{"Pendant of Power", "Pendentif de la force", "Colgante del poder"},
     Text{"Din's Ruby", "Rubis de Din", "Rub?? de Din"},
     Text{"Goron's Garnet", "Grenat Goron", "Topacio de los Goron"}};
  trickNameTable[0xCD] = {
     Text{"Pendant of Wisdom", "Pendentif de la sagesse", "Colgante de la sabidur??a"},
     Text{"Nayru's Sapphire", "Saphir de Nayru", "Zafiro de Nayru"},
     Text{"Zora's Aquamarine", "Aquamarine Zora", "Lapisl??zuli de los Zora"}};
  trickNameTable[0xCE] = {
     Text{"Wind Medallion", "M??daillon du vent", "Medall??n del Viento"},
     Text{"Saria's Medallion", "M??daillon de Saria", "Medall??n de Saria"},
     Text{"Medallion of Forest", "M??daillon du Temple de la For??t", "Medalla del Bosque"}};
  trickNameTable[0xCF] = {
     Text{"Bombos Medallion", "M??daillon des flammes", "Medall??n del Temblor"},
     Text{"Darunia's Medallion", "M??daillon de Darunia", "Medall??n de Darunia"},
     Text{"Medallion of Fire", "M??daillon du Temple du Feu", "Medalla del Fuego"}};
  trickNameTable[0xD0] = {
     Text{"Ice Medallion", "M??daillon de glace", "Medall??n Helado"},
     Text{"Ruto's Medallion", "M??daillon de Ruto", "Medall??n de Ruto"},
     Text{"Medallion of Water", "M??daillon du Temple de l'Eau", "Medalla del Agua"}};
  trickNameTable[0xD1] = {
     Text{"Quake Medallion", "M??daillon des secousses", "Medall??n Llamarada"},
     Text{"Nabooru's Medallion", "M??daillon de Nabooru", "Medall??n de Nabooru"},
     Text{"Medallion of Spirit", "M??daillon du Temple de l'Esprit", "Medalla del Esp??ritu"}};
  trickNameTable[0xD2] = {
     Text{"Travel Medallion", "Amulette de t??l??portation", "Medall??n Maligno"},
     Text{"Impa's Medallion", "M??daillon d'Impa", "Medall??n de Impa"},
     Text{"Medallion of Shadow", "M??daillon du Temple de l'Ombre", "Medalla de la Sombra"}};
  trickNameTable[0xD3] = {
     Text{"Ether Medallion", "M??daillon d'??ther", "Medall??n de Tesoro"},
     Text{"Rauru's Medallion", "M??daillon de Rauru", "Medall??n de Rauru"},
     Text{"Medallion of Light", "M??daillon du temple de lumi??re", "Medalla de la Luz"}};

  trickNameTable[GI_HEART] = {
     Text{"Love", "Bisou", "Te amo"},
     Text{"Heart Container", "R??ceptacle de coeur", "Contenedor de coraz??n"},
     Text{"Piece of Heart", "Quart de coeur", "Pieza de coraz??n"}};
  trickNameTable[GI_RUPEE_GREEN] = {
     Text{"Green Rupy", "Rupee vert", "Rubia verde"},
     Text{"One Rupee", "Un rubis", "Guaran?? hyliano"},
     Text{"Rupee (1)", "Rubis (1)", "Peso hyliano"}};
  trickNameTable[GI_RUPEE_BLUE] = {
     Text{"Blue Rupy", "Rupee bleu", "Rubia azul"},
     Text{"Five Rupees", "Cinq rubis", "Bol??var hyliano"},
     Text{"Rupee (5)", "Rubis (5)", "Peso hyliano"}};
  trickNameTable[GI_RUPEE_RED] = {
     Text{"Red Rupy", "Rupee rouge", "Rubia roja"},
     Text{"Twenty Rupees", "Vingt rubis", "Colon hyliano"},
     Text{"Rupee (20)", "Rubis (20)", "Peso hyliano"}};
  trickNameTable[GI_RUPEE_PURPLE] = {
     Text{"Purple Rupy", "Rupee pourpre", "Rubia morada"},
     Text{"Fifty Rupees", "Cinquante rubis", "Balboa hyliano"},
     Text{"Rupee (50)", "Rubis (50)", "Peso hyliano"}};
  trickNameTable[GI_RUPEE_GOLD] = {
     Text{"Huge Rupy", "??norme Rupee", "Rubia gigante"},
     Text{"Two Hundred Rupees", "Deux cent rubis", "Euro hyliano"},
     Text{"Rupee (200)", "Rubis (200)", "D??lar hyliano"}};
  trickNameTable[GI_HEART_PIECE] = {
     Text{"Piece of Health", "Quart d'??nergie", "Pieza de amor"},
     Text{"Recovery Heart", "Coeur d'??nergie", "Coraz??n"},
     Text{"Heart Container", "R??ceptacle de coeur", "Contenedor de coraz??n"}};
  trickNameTable[GI_HEART_CONTAINER_2] = {
     Text{"Health Container", "R??ceptacle d'??nergie", "Contenedor de amor"},
     Text{"Recovery Heart", "Quart de coeur", "Coraz??n"},
     Text{"Piece of Heart", "Coeur d'??nergie", "Pieza de coraz??n"}};

/*
  //Names for individual upgrades, in case progressive names are replaced
  trickNameTable[GI_HOOKSHOT] = {
     Text{"Grappling Hook", "Grappin-griffe", "Gancho lanzable"},
     Text{"Clawshot", "Lance-cha??ne", "Zarpa"},
     Text{"Gripshot", "Grappince", "Enganchador"}};
  trickNameTable[GI_LONGSHOT] = {
	 Text{"Longshot, no strings attached", "Grappin sans attrape", "Gancho lanzable m??s largo"},
     Text{"Double Clawshot", "Double-grappin", "Superzarpa"},
     Text{"Switch Hook", "Great grappin", "Gancho chulo"}};
  trickNameTable[GI_BOMB_BAG_1] = {
	 Text{"Bomb Capacity (20)", "Capacit?? de bombes (20)", "Bolsa de bombas (contiene 20)"},
     Text{"Bronze Bomb Bag", "Sac de Bombes de bronze", "Saco de bronce de bombas"},
     Text{"Small Bomb Bag", "Petit Sac de Bombes", "Zurr??n de bombas peque??o"}};
  trickNameTable[GI_BOMB_BAG_2] = {
	 Text{"Bomb Capacity (30)", "Capacit?? de bombes (30)", "Bolsa de bombas (contiene 30)"},
     Text{"Silver Bomb Bag", "Sac de Bombes d'argent", "Saco plateado de bombas"},
     Text{"Medium Bomb Bag", "Sac de Bombes moyen", "Zurr??n de bombas mediano"}};
  trickNameTable[GI_BOMB_BAG_3] = {
	 Text{"Bomb Capacity (40)", "Capacit?? de bombes (40)", "Bolsa de bombas (contiene 40)"},
     Text{"Golden Bomb Bag", "Sac de Bombes d'or", "Saco dorado de bombas"},
     Text{"Large Bomb Bag", "Gros Sac de Bombes", "Zurr??n de bombas grande"}};
  trickNameTable[GI_BOW_1] = {
	 Text{"Bow", "Arc", "Arco del Hada"},
     Text{"Hero's Bow", "Arc du h??ros", "Arco del h??roe"},
     Text{"Small Quiver", "Petit carquois", "Saco de flechas peque??o"}};
  trickNameTable[GI_BOW_2] = {
	 Text{"Arrow Capacity (40)", "Capacit?? de fl??ches (40)", "Capacidad de flechas (40)"},
     Text{"Silver Quiver", "Carquois d'argent", "Carcaj plateado"},
     Text{"Medium Quiver", "Carquois moyen", "Saco de flechas mediano"}};
  trickNameTable[GI_BOW_3] = {
	 Text{"Arrow Capacity (50)", "Capacit?? de fl??ches (50)", "Capacidad de flechas (50)"},
     Text{"Golden Quiver", "Carquois d'or", "Carcaj dorado"},
     Text{"Large Quiver", "Gros carquois", "Saco de flechas grande"}};
  trickNameTable[GI_SLINGSHOT_1] = {
	 Text{"Slingshot", "Lance-Pierre", "Tirachinas del Hada"},
     Text{"Scattershot", "Lance-Pierre rafale", "Tirachinas m??ltiple"},
     Text{"Small Seed Satchel", "Petit sac de graines", "Bolsa de semillas peque??a"}};
  trickNameTable[GI_SLINGSHOT_2] = {
	 Text{"Deku Seed Capacity (40)", "Capacit?? de graines (40)", "Capacidad de semillas (40)"},
     Text{"Silver Deku Seed Bullet Bag", "Sac de graines d'argent", "Bolsa de balas (contiene 40)"},
     Text{"Medium Seed Satchel", "Sac de graines moyen", "Bolsa de semillas mediana"}};
  trickNameTable[GI_SLINGSHOT_3] = {
	 Text{"Deku Seed Capacity (50)", "Capacit?? de graines (50)", "Capacidad de semillas (50)"},
     Text{"Golden Deku Seed Bullet Bag", "Sac de graines d'or", "Bolsa de balas (contiene 50)"},
     Text{"Large Seed Satchel", "Gros sac de graines", "Bolsa de semillas grande"}};
  trickNameTable[GI_STRENGTH_1] = {
	 Text{"Goron's Gauntlet", "Gantelet Goron", "Brazalete amarillo"},
     Text{"Power Bracelet", "Bracelet de force", "Brazalete de fuerza"},
     Text{"Magic Bracelet", "Bracelet de Lavio", "Brazalete de Ravio"}};
  trickNameTable[GI_STRENGTH_2] = {
	 Text{"Silver Bracelets", "Bracelets d'argent", "Guantes Moguma"},
     Text{"Power Gloves", "Gant de puissance", "Guante del Poder"},
     Text{"Magic Gauntlets", "Gantelet magique", "Guante m??gico"}};
  trickNameTable[GI_STRENGTH_3] = {
	 Text{"Golden Bracelets", "Bracelets d'or", "Guantelete de Thanos"},
     Text{"Titan's Mitts", "Moufle de titan", "Guantes de Tit??n"},
     Text{"Magnetic Gloves", "Magn??to-gants", "Guantes de fuego"}};
  trickNameTable[GI_SCALE_1] = {
	 Text{"Silver Pearl", "Perle d'argent", "Perla de Plata progresiva"},
     Text{"Adult Scale", "??caille d'adulte", "Bola de bolos zora"},
     Text{"Zora Scale", "??caille Zora", "Escama de Zora"}};
  trickNameTable[GI_SCALE_2] = {
	 Text{"Golden Pearl", "Perle d'or", "Perla de Oro progresiva"},
     Text{"Giant Scale", "??caille de g??ant", "Escama de Faren"},
     Text{"Water Dragon Scale", "??caille du dragon de l'eau", "Escama de drag??n acu??tico"}};
  trickNameTable[GI_WALLET_1] = {
	 Text{"Rupee Capacity (200)", "Capacit?? de rubis (200)", "Capacidad de rupias (200)"},
     Text{"Silver Wallet", "Bourse d'argent", "Cartera de rupias de adulto"},
     Text{"Medium Wallet", "Bourse moyenne", "Zurr??n de rupias mediano"}};
  trickNameTable[GI_WALLET_2] = {
	 Text{"Rupee Capacity (500)", "Capacit?? de rubis (500)", "Capacidad de rupias (500)"},
     Text{"Golden Wallet", "Bourse d'or", "Cartera de rupias gigante"},
     Text{"Large Wallet", "Grosse Bourse", "Zurr??n de rupias grande"}};
  trickNameTable[GI_WALLET_3] = {
	 Text{"Rupee Capacity (999)", "Capacit?? de rubis (999)", "Capacidad de rupias (999)"},
     Text{"Golden Wallet", "Bourse d'or", "Cartera de ricach??n"},
     Text{"Large Wallet", "Grosse Bourse", "Zurr??n de rupias gigante"}};
  trickNameTable[GI_DEKU_NUT_UPGRADE_1] = {
	 Text{"Deku Bomb Capacity (30)", "Capacit?? de bombes Mojo (30)", "Capacidad de semillas deku (40)"},
     Text{"Baba Nut Capacity (30)", "Capacit?? de noix Baba (30)", "Capacidad de nueces baba (40)"},
     Text{"Deku Nut Pack (30)", "Paquet de noix Mojo (30)", "Capacidad de nueces mojo (40)"}};
  trickNameTable[GI_DEKU_NUT_UPGRADE_2] = {
	 Text{"Deku Bomb Capacity (40)", "Capacit?? de bombes Mojo (40)", "Capacidad de semillas deku (50)"},
     Text{"Baba Nut Capacity (40)", "Capacit?? de noix Baba (40)", "Capacidad de nueces baba (50)"},
     Text{"Deku Nut Pack (40)", "Paquet de noix Mojo (40)", "Capacidad de nueces mojo (50)"}};
  trickNameTable[GI_DEKU_STICK_UPGRADE_1] = {
	 Text{"Deku Rod Capacity (20)", "Capacit?? de tiges Mojo (20)", "Capacidad de palos mojo (20)"},
     Text{"Boko Stick Capacity (20)", "Capacit?? de B??tons Boko (20)", "Capacidad de palos boko (20)"},
     Text{"Deku Stick Pack (20)", "Paquet de b??tons Mojo (20)", "Capacidad de bastones deku (20)"}};
  trickNameTable[GI_DEKU_STICK_UPGRADE_2] = {
	 Text{"Deku Rod Capacity (30)", "Capacit?? de tiges Mojo (30)", "Capacidad de palos mojo (30)"},
     Text{"Boko Stick Capacity (30)", "Capacit?? de B??tons Boko (30)", "Capacidad de palos boko (30)"},
     Text{"Deku Stick Pack (30)", "Paquet de b??tons Mojo (30)", "Capacidad de bastones deku (30)"}};
  trickNameTable[GI_MAGIC_1] = {
	 Text{"Stamina Meter", "Jauge d'endurance", "Medidor de vigor"},
     Text{"Energy Meter", "Jauge d'??nergie", "Medidor de energ??a"},
     Text{"Magic Powder", "Poudre magique", "Medidor de carga"}};
  trickNameTable[GI_MAGIC_2] = {
	 Text{"Enhanced Stamina Meter", "Jauge d'endurance am??lior??e", "Medidor de vigor mejorado"},
     Text{"Enhanced Energy Meter", "Jauge d'??nergie am??lior??e", "Medidor de energ??a mejorado"},
     Text{"Enhanced Magic Powder", "Poudre magique am??lior??e", "Medidor de carga mejorado"}};
  trickNameTable[GI_OCARINA_1] = {
	 Text{"Ocarina", "Ocarina", "Ocarina"},
     Text{"Saria's Ocarina", "Ocarina de Saria", "Ocarina de Saria"},
     Text{"Wood Ocarina", "Ocarina de bois", "Ocarina del Hada"}};
  trickNameTable[GI_OCARINA_2] = {
	 Text{"Flute", "Fl??te", "Flauta"},
     Text{"Zelda's Ocarina", "Ocarina de Zelda", "Ocarina de Zelda"},
     Text{"Ocarina of Winds", "Ocarina des vents", "Ocarina del Viento"}};
  trickNameTable[GI_CUCCO] = {
	 Text{"D.I.Y. Alarm Clock", "R??veille-matin improvis??", "Alarma emplumada"},
     Text{"Kakariko Cucco", "Cocotte Cocorico", "Cuco de Kakariko"},
     Text{"Hatched Cucco", "Cocotte ??close", "Pollo"}};
  trickNameTable[GI_MASK_KEATON] = {
	 Text{"Kee... Something Mask", "Masque de Quiche", "M??scara Kealgo"},
     Text{"Kitsune Mask", "Masque de Kitsune", "M??scara Kitsune"},
     Text{"Kafei's Mask", "Masque de Kafei", "M??scara de Kafei"}};
  trickNameTable[GI_MASK_SKULL] = {
	 Text{"Skull Kid's Mask", "Masque de Skull Kid", "M??scara de Skull Kid"},
     Text{"Stalfos Mask", "Masque de squelette", "M??scara de Stalfos"},
     Text{"Captain's Hat", "Heaume du capitaine", "Casco del capit??n"}};
  trickNameTable[GI_MASK_SPOOKY] = {
	 Text{"Skrik Mask", "Masque Skrik", "M??scara Escalofriante"},
     Text{"ReDead Mask", "Masque de Remort", "M??scara de ReDead"},
     Text{"Gibdo Mask", "Masque de Gibdo", "Careta de Gibdo"}};
  trickNameTable[GI_MASK_BUNNY] = {
	 Text{"Peppy Mask", "Masque de Peppy", "Capucha de Pascua"},
     Text{"Bunny Ears", "Oreilles de lapin", "Orejas de conejo"},
     Text{"Postman's Hat", "Casquette du facteur", "Gorra de cartero"}};
  trickNameTable[GI_MASK_GORON] = {
	 Text{"Goro Mask", "Masque Goro", "M??scara Goro"},
     Text{"Mask of Goron", "Masque des Gorons", "M??scara de los Goron"},
     Text{"Darunia Mask", "Masque de Darunia", "M??scara de Darmani"}};
  trickNameTable[GI_MASK_ZORA] = {
	 Text{"Zola Mask", "Masque Zola", "M??scara Zola"},
     Text{"Mask of Zora", "Masque des Zoras", "M??scara de los Zora"},
     Text{"Ruto Mask", "Masque de Ruto", "M??scara de Mikau"}};
  trickNameTable[GI_MASK_GERUDO] = {
	 Text{"Ganguro Mask", "Masque de Ganguro", "M??scara Canguro"},
     Text{"Mask of Gerudo", "Masque des Gerudos", "M??scara de las Gerudo"},
     Text{"Nabooru Mask", "Masque de Nabooru", "M??scara de Nabooru"}};
  trickNameTable[GI_MASK_TRUTH] = {
	 Text{"Sheikah Mask", "Masque Sheikah", "M??scara Sheikah"},
     Text{"Mask of Gossip", "Masque de potins", "M??scara chismosa"},
     Text{"Eye of Truth", "oeil de v??rit??", "Ojo de la Verdad"}};*/
}

//Generate a fake name for the ice trap based on the item it's displayed as
Text GetIceTrapName(uint8_t id) {
  //If the trick names table has not been initialized, do so
  if (!initTrickNames) {
    InitTrickNames();
    initTrickNames = true;
  }
  //Randomly get the easy, medium, or hard name for the given item id
  return RandomElement(trickNameTable[id]);
}

//Get shop index based on a given location
static std::map<std::string_view, int> ShopNameToNum = {{"KF Shop", 0},   {"Kak Potion Shop", 1}, {"MK Bombchu Shop", 2}, {"MK Potion Shop", 3},
                                                        {"MK Bazaar", 4}, {"Kak Bazaar", 5},      {"ZD Shop", 6},         {"GC Shop", 7}};
int GetShopIndex(uint32_t loc) {
  //Kind of hacky, but extract the shop and item position from the name
  const std::string& name(Location(loc)->GetName());
  int split = name.find(" Item ");
  std::string_view shop(name.c_str(), split);
  int pos = std::stoi(name.substr(split+6, 1)) - 1;
  int shopnum = ShopNameToNum[shop];
  return shopnum*8 + pos;
}

//Without this transformed index, shop-related tables and arrays would need 64 entries- But only half of that is needed for shopsanity
//So we use this transformation to map only important indices to an array with 32 entries in the following manner:
//Shop index:  4  5  6  7 12 13 14 15 20 21 22 23...
//Transformed: 0  1  2  3  4  5  6  7  8  9 10 11...
//So we first divide the shop index by 4, then by 2 which basically tells us the index of the shop it's in,
//then multiply by 4 since there are 4 items per shop
//And finally we use a modulo by 4 to get the index within the "shop" of 4 items, and add
int TransformShopIndex(int index) {
  return 4*((index / 4) / 2) + index % 4;
}
