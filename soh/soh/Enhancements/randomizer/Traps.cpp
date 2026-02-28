#include "Traps.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"

#include <vector>

static std::array<std::vector<Text>, RG_MAX> trickNameTable; // Table of trick names for ice traps
bool initTrickNames = false; // Indicates if trick ice trap names have been initialized yet

/* Initialize the table of trick names with an easy, medium, and hard name for each language.
   english, french, german // spanish */
static void InitTrickNames() {
    trickNameTable[RG_KOKIRI_SWORD] = {
        Text{ "Korok Sword", "Épée Korok", "Korok-Schwert" },     // "Espada Korok"
        Text{ "Hero's Sword", "Épée du Héros", "Heroenschwert" }, // "Espada del héroe"
        Text{ "Razor Sword", "Lame Rasoir", "Elfenschwert" },     // "Espada de esmeril"
    };
    trickNameTable[RG_MASTER_SWORD] = {
        Text{ "Goddess Sword", "Épée de la déesse", "Schwert der Göttin" }, // "Espada Divina"
        Text{ "Gilded Sword", "Excalibur", "Schmirgelklinge" },             // "Espada de los Sabios"
        Text{ "Magical Sword", "Lame dorée", "Magisches Schwert" },         // "Fay"
    };
    trickNameTable[RG_GIANTS_KNIFE] = {
        Text{ "Medigoron's Sword", "l'Épée de Medigoron", "Medigoron-Schwert" }, // "La espada de Medigoron"
        Text{ "Razor Sword", "Lame Rasoir", "Elfenschwert" },                    // "Espada de esmeril"
        Text{ "Royal Claymore", "Claymore Royale", "Königs-Zweihänder" },        // "Royal Claymore"
    };
    trickNameTable[RG_BIGGORON_SWORD] = {
        Text{ "Power Sword", "Épée de Puissance", "Schwert der Macht" }, // "Espada de poder",
        Text{ "Fierce Deity Sword", "Épée du dieu démon",
              "Schwert der grimmischen Gottheit" },                              // "Espada de la Fiera Deidad"
        Text{ "Tempered Sword", "Épée de Légende Nv.2", "Master-Schwert Lv.2" }, // "Espada Maestra mejorada"
        Text{ "Biggoron's Knife", "Lame de Grogoron", "Biggoron-Messer" },       // "Daga de Biggoron"
    };
    trickNameTable[RG_DEKU_SHIELD] = {
        Text{ "Boko Shield", "Bouclier Boko", "Bokschild" },        // "Escudo Boko"
        Text{ "Ordon Shield", "Bouclier de Toal", "Ordon-Schild" }, // "Escudo de Ordon"
        Text{ "Wooden Shield", "Bouclier de Bois", "Holzschild" },  // "Escudo de madera"
    };
    trickNameTable[RG_HYLIAN_SHIELD] = {
        Text{ "Hyrule Shield", "Bouclier d'Hyrule", "Hyrule-Schild" },   // "Escudo Hylian"
        Text{ "Goddess Shield", "Bouclier Sacré", "Göttlicher Schild" }, // "Escudo Divino"
        Text{ "Hero's Shield", "Bouclier du Héros", "Heroenschild" },    // "Escudo del héroe"
    };
    trickNameTable[RG_MIRROR_SHIELD] = {
        Text{ "Magic Mirror", "Miroir Magique", "Magiespiegel" },             // "Escudo mágico"
        Text{ "Magical Shield", "Bouclier Magique", "Magischer Schild" },     // "Escudo arcano"
        Text{ "Mirror of Twilight", "Miroir des Ombres", "Schattenspiegel" }, // "Espejo del Crepúsculo"
    };
    trickNameTable[RG_GORON_TUNIC] = {
        Text{ "Gerudo Top", "Tunique Gerudo", "Damengewand" },                  // "Pechera gerudo"
        Text{ "Flamebreaker Armor", "Armure de Pierre", "Anti-Feuer-Rüstung" }, // "Armadura ignífuga"
        Text{ "Red Mail", "Habits Rouges", "Rotes Gewand" },                    // "Ropas rojas"
    };
    trickNameTable[RG_ZORA_TUNIC] = {
        Text{ "Rito Tunic", "Tunique Rito", "Orni-Gewand" },       // "Sayo rito"
        Text{ "Mermaid Suit", "Costume de sirène", "Nixenanzug" }, // "Costume de sirène"
        Text{ "Zora Armor", "Armure Zora", "Zora-Panzer" },        // "Túnica Zora"
        Text{ "Blue Mail", "Habits Bleus", "Blaues Gewand" },      // "Ropas azules"
    };
    trickNameTable[RG_IRON_BOOTS] = {
        Text{ "Iron Hoofs", "Patins de Plomb", "Eisenhufe€" },             // "Botas férreas"
        Text{ "Snow Boots", "Bottes de Neige", "Schneestiefel€" },         // "Botas de nieve"
        Text{ "Red Boots", "Bottes rouges", "Rote Stiefel€" },             // "Botas rojas"
        Text{ "Zora Greaves", "Bottes Zora", "Zora-Beinschutz" },          // "Zora Greaves"
        Text{ "Boots of Power", "Bottes de Puissance", "Stärkestiefel€" }, // "Botas de plomo"
    };
    trickNameTable[RG_HOVER_BOOTS] = {
        Text{ "Hover Hoofs", "Patins des airs", "Gleithufe€" },         // "Botas flotadoras"
        Text{ "Golden Boots", "Bottes dorées", "Goldstiefel€" },        // "Botas de Oro"
        Text{ "Pegasus Boots", "Bottes pégase", "Pegasus-Stiefel€" },   // "Botas de Pegaso"
        Text{ "Boots of Speed", "Bottes de vitesse", "Tempostiefel€" }, // "Botas del desierto"
    };
    trickNameTable[RG_WEIRD_EGG] = {
        Text{ "Poached Egg", "Oeuf à la coque", "Spiegelei" }, // "Huevo pasado"
        Text{ "Lon Lon Egg", "Oeuf Lon Lon", "Lon Lon-Ei" },   // "Huevo Lon Lon"
        Text{ "Zora Egg", "Oeuf Zora", "Zora-Ei" },            // "Huevo Zora"
    };
    trickNameTable[RG_ZELDAS_LETTER] = {
        Text{ "Ruto's Letter", "Lettre de Ruto", "Rutos Brief" },                            // "Carta de Ruto"
        Text{ "Royal Letter", "Lettre Eoyale", "Royaler Brief" },                            // "Carta para Kafei"
        Text{ "Zelda's Business Card", "Carte d'affaires de Zelda", "Zeldas Visitenkarte" }, // "Carta"
        Text{ "Letter to Kafei", "Lettre pour Kafei", "Brief an Kafei" },                    // "Carta para Kafei"
        Text{ "Goat's Letter", "Lettre de la Chèvre", "Brief der Ziege" },                   // "Carta de la Cabra"
        Text{ "Maggie's Letter", "Lettre de Maggy", "Dolores' Brief" },                      // "Carta de Dolores"
    };
    trickNameTable[RG_BOOMERANG] = {
        Text{ "Banana", "Banane", "Banane" },                                 // "Plátano"
        Text{ "Prank Fetch Toy", "Inséparable Bâtonnet", "Boomerang" },       // "Bumerang"
        Text{ "Gale Boomerang", "Boomerang Tornade", "Sturmbumerang" },       // "Bumerán tornado"
        Text{ "Magic Boomerang", "Boomerang Magique", "Magischer Bumerang" }, // "Bumerán mágico"
    };
    trickNameTable[RG_LENS_OF_TRUTH] = {
        Text{ "Sheikah-leidoscope", "Sheikah-léidoscope", "Shiekah-leidoskop" }, // "Monóculo de la Verdad"
        Text{ "Sheikah Sensor", "Sonar Sheikah", "Shiekah-Sensor" },             // "Sensor Sheikah"
        Text{ "Crystal of Vision", "Cristal de Vision", "Kristall der Sicht" },  // "Cristal de Visión"
        Text{ "Magnifying Lens", "Loupe", "Magische Lupe" },                     // "Lente Aumentadora"
    };
    trickNameTable[RG_MEGATON_HAMMER] = {
        Text{ "Goron Gavel", "Masse Perforatrice", "Goronen-Hammer" }, // "Mazo Goron"
        Text{ "Magic Hammer", "Marteau Magique", "Magischer Hammer" }, // "Martillo mágico"
        Text{ "Skull Hammer", "Maillet Ressort", "Schädel-Hammer" },   // "Martillo de hierro"
    };
    trickNameTable[RG_STONE_OF_AGONY] = {
        Text{ "Cave Charm", "Charme de grotte", "Höhlenamulett" },                 // "Amuleto de la cueva"
        Text{ "Stone of Agahnim", "Fragment d'Agahnim", "Stein des Agahnim" },     // "Piedra de Agahnim"
        Text{ "Shard of Agony", "Fragment de Souffrance", "Scherbe des Wissens" }, // "Piedra de la Agonía"
        Text{ "Pirate's Charm", "Pierre de Pirate", "Links Talisman" },            // "Amuleto Pirata"
    };
    trickNameTable[RG_DINS_FIRE] = {
        Text{ "Eldin's Fire", "Feu d'Eldin", "Eldins Feuerinferno" },           // "Fuego de Eldin"
        Text{ "Din's Blaze", "Flamme de Din", "Dins Flammen" },                 // "Poder de Din"
        Text{ "Magic Lantern", "Lanterne Magique", "Magische Laterne" },        // "Linterna mágica"
        Text{ "Ether Medallion", "Médaillon d'Éther", "Luft-Medaillon" },       // "Medallón de Tesoro"
        Text{ "Bombos Medallion", "Médaillon des Flammes", "Feuer-Medaillon" }, // "Medallón del Temblor"
    };
    trickNameTable[RG_FARORES_WIND] = {
        Text{ "Faron's Wind", "Vent de Firone", "Phirones Donnersturm" },              // "Viento de Farone"
        Text{ "Farore's Windfall", "Zéphyr de Farore", "Farores Wind" },               // "Valor de Farore"
        Text{ "Tingle Air", "Tingle de aire", "Tingle Luft" },                         // "Tingle Air"
        Text{ "Travel Medallion", "Amulette de téléportation", "Teleport-Medaillon" }, // "Medallón Maligno"
        Text{ "Irene's Taxi", "Le taxi d'Aëline", "Irenes Taxi" },                     // "El taxi de Airín"
    };
    trickNameTable[RG_NAYRUS_LOVE] = {
        Text{ "Lanayru's Love", "Amour de Lanelle", "Ranelles Umarmung" }, // "Amor de Lanayru"
        Text{ "Nayru's Passion", "Passion de Nayru", "Nayrus Hingabe" },   // "Sabiduría de Nayru"
        Text{ "Tingle Shield", "Bouclier Tingle", "Tingleschild" },        // "Escudo de hormigueo"
        Text{ "Shield Spell", "Bouclier Magique", "Schildzauber" },        // "Hechizo de Protección"
        Text{ "Magic Armor", "Armure Magique", "Magie-Rüstung" },          // "Armadura mágica"
    };
    trickNameTable[RG_FIRE_ARROWS] = {
        Text{ "Fire Rod", "Baguette de feu", "Feuerstab" },     // "Cetro de fuego"
        Text{ "Bomb Arrow", "Flèche-Bombe", "Bomben-Pfeile€" }, // "Flecha bomba"
        Text{ "Red Candle", "Bougie Rouge", "Rote Kerze" },     // "Vela roja"
    };
    trickNameTable[RG_ICE_ARROWS] = {
        Text{ "Ice Rod", "Baguette des Glaces", "Eisstab" },                       // "Cetro de Hielo"
        Text{ "Ancient Arrow", "Flèche Archéonique", "Antike Pfeile€" },           // "Flecha ancestral"
        Text{ "Ice Trap Arrow", "Flèche de Piège de Glace", "Eisfallen-Pfeile€" }, // "Cetro de hielo"
    };
    trickNameTable[RG_LIGHT_ARROWS] = {
        Text{ "Wind Arrow", "Flèche de Vent", "Wind-Pfeile€" },                     // "Flecha del Viento"
        Text{ "Wand of Gamelon", "Baguette de Gamelon", "Zauberstab von Gamelon" }, // "Varita de Gamelón"
        Text{ "Shock Arrow", "Flèches Électriques", "Elektro-Pfeile€" },            // "Flecha eléctrica"
        Text{ "Silver Arrow", "Flèches d'Argent", "Silber-Pfeile€" },               // "Flecha de plata"
    };
    trickNameTable[RG_GERUDO_MEMBERSHIP_CARD] = {
        Text{ "Desert Title Deed", "Abonnement Gerudo", "Wüsten-Urkunde" }, // "Escritura del desierto"
        Text{ "Sickle Moon Flag", "Drapeau du croissant de lune",
              "Mondsichel-Fahne" },                                           // "Bandera de la Luna Creciente"
        Text{ "Complimentary ID", "Bon de félicitation", "Laudatio-Schein" }, // "Cupón especial"
        Text{ "Gerudo's Card", "Carte Goron", "Gerudo-Symbol" },              // "Tóken Gerudo"
        Text{ "Gerudo's Membership Card", "Autographe de Nabooru", "Gerudo-Mitgliedskarte" }, // "Tarjeta Gerudo"
    };

    trickNameTable[RG_MAGIC_BEAN_PACK] = {
        Text{ "Funky Bean Pack", "Paquet de Fèves Magiques", "Wunderbohnen-Packung" },    // "Lote de frijoles mágicos"
        Text{ "Grapple Berries", "Baies de grappin", "Grapple-Beeren€" },                 // "Bayas de garfio"
        Text{ "Crenel Bean Pack", "Paquet de Haricots Gonggle", "Gongolerbsen-Packung" }, // "Lote de alubias mágicas"
        Text{ "Mystical Seed Pack", "Pack de graines mystiques", "Saatbeutel" }, // "Paquete de semillas místicas"
    };
    trickNameTable[RG_DOUBLE_DEFENSE] = {
        Text{ "Diamond Hearts", "Coeurs de Diamant", "Diamantherzen€" },            // "Contenedor de diamante"
        Text{ "Double Damage", "Double Souffrance", "Doppelte Angriffskraft" },     // "Doble daño receptivo"
        Text{ "Quadruple Defence", "Quadruple Défence", "Vierfache Verteidigung" }, // "Defensa cuádruple"
    };

    trickNameTable[RG_POCKET_EGG] = {
        Text{ "Arpagos Egg", "Oeuf d'Arpagos", "Arpagos-Ei" }, // "Huevo de Arpagos"
        Text{ "Lon Lon Egg", "oeuf Lon Lon", "Lon Lon-Ei" },   // "Huevo Lon Lon"
        Text{ "Zora Egg", "oeuf Zora", "Zora-Ei" },            // "Huevo del Pez Viento"
    };
    trickNameTable[RG_POCKET_EGG] = {
        Text{ "D.I.Y. Alarm Clock", "Réveille-matin improvisé",
              "Improvisierter Wecker" },                                 // "Alarma emplumada portátil"
        Text{ "Kakariko Cucco", "Cocotte Cocorico", "Kakariko-Huhn" },   // "Cuco de Kakariko"
        Text{ "Hatched Cucco", "Cocotte éclose", "Geschlüpftes Küken" }, // "Pollo de bolsillo"
    };
    trickNameTable[RG_COJIRO] = {
        Text{ "Blucco", "Chair-Qui-Poule", "Blenni" },           // "Cucazul"
        Text{ "Piyoko", "Piyoko", "Piyoko" },                    // "Piyoko"
        Text{ "Dark Cucco", "Cocotte Sombre", "Dunkles Huhn" },  // "Cucco oscuro"
        Text{ "Grog's Cucco", "Cocotte de Grog", "Grogs Huhn" }, // "Cuco de Grog"
    };
    trickNameTable[RG_ODD_MUSHROOM] = {
        Text{ "Magic Mushroom", "Champignon magique", "Magischer Pilz" },  // "Champiñón mágico"
        Text{ "Endura Shroom", "Champi Vigueur", "Fittling" },             // "Champiñón del bosque"
        Text{ "Sleepy Toadstool", "Crapaud Fatigué", "Schlummermorchel" }, // "Seta durmiente"
        Text{ "Mushroom", "Champignon", "Pilz" },                          // "Seta"
    };
    trickNameTable[RG_ODD_POTION] = {
        Text{ "Odd Medicine", "Élixir suspect", "Moderpaket" },               // "Poción rara"
        Text{ "Granny's Poultice", "Mixture de Granny", "Omas Mixtur" },      // "Medicina de la abuela"
        Text{ "Mushroom Poultice", "Mixture de champignon", "Pilzumschlag" }, // "Medicina de champiñones"
        Text{ "Secret Medicine", "Médicament", "Geheime Medizin" },           // "Pócima secreta"
        Text{ "Mushroom Spores", "Spores de Champignons", "Pilzsporen" },     // "Esporas de hongos"
        Text{ "Hanyu Spore", "Hanyu Spore", "Hanyu-Sporen" },                 // "Espora Hanyu"
    };
    trickNameTable[RG_POACHERS_SAW] = {
        Text{ "Carpenter's Saw", "Scie du charpentier", "Zimmermannssäge" },      // "Sierra del carpintero"
        Text{ "Poacher's Sword", "Hache du chasseur", "Wildererschwert" },        // "Espada del capataz"
        Text{ "Ancient Bladesaw", "Longue Épée Archéonique", "Antike Großsäge" }, // "Mandoble ancestral"
        Text{ "Woodcutter's Axe", "Hache du Bûcheron", "Holzfälleraxt" },         // "Hacha de leñador"
        Text{ "Grog's Saw", "Scie de Grog", "Grogs Säge" },                       // "Sierra del Cazador Furtivo"
    };
    trickNameTable[RG_BROKEN_SWORD] = {
        Text{ "Broken Biggoron's Sword", "Épée brisée de Grogoron",
              "Zerbr. Biggoron-Schwert" },                                                // "Espada de Biggoron rota"
        Text{ "Broken Giant's Knife", "Lame des Géants brisée", "Zerbr. Langschwert" },   // "Daga gigante rota"
        Text{ "Broken Noble Sword", "Épée noble brisée", "Zerbr. Edelschwert" },          // "Espada noble rota"
        Text{ "Broken Picori Blade", "Épée Minish brisée", "Zerbr. Schwert der Minish" }, // "Espada minish rota"
        Text{ "Decayed Master Sword", "Épée de légende pourrie",
              "Zerf. Master-Schwert" }, // "Espada decadente de leyenda"
    };
    trickNameTable[RG_PRESCRIPTION] = {
        Text{ "Biggoron's Prescription", "Ordonnance de Grogoron", "Biggorons Rezept" }, // "Receta de Biggoron"
        Text{ "Eyedrop Prescription", "Ordonnance de gouttes", "Augentropfen-Rezept" },  // "Receta ocular"
        Text{ "Urgent Prescription", "Ordonnance urgente", "Dringendes Rezept" },        // "Prescripción"
        Text{ "Swordsman's Scroll", "Précis d'escrime", "Schwert-Rolle" },               // "Esgrimidorium"
        Text{ "Portrait of Oren", "Portrait d'Orlène", "Portrait von Oreen" },           // "Retrato de Oren"
        Text{ "Letter to King Zora", "Lettre au roi Zora", "Brief an König Zora" },      // "Carta al Rey Zora"
    };
    trickNameTable[RG_EYEBALL_FROG] = {
        Text{ "Don Gero", "Don Gero", "Don Gero" },                             // "Don Gero"
        Text{ "Hot-Footed Frog", "Grenouille à pieds chauds", "Spurtkröte" },   // "Rana de patas calientes"
        Text{ "Lost Swordsmith", "Forgeron perdu", "Vermisster Schmied" },      // "Espadachín perdido"
        Text{ "Eyedrop Frog", "Grenouille-qui-louche", "Augentropfen-Frosch" }, // "Globo Ocular de Rana"
    };
    trickNameTable[RG_EYEDROPS] = {
        Text{ "Biggoron's Eyedrops", "Gouttes de Grogoron", "Biggorons Augentropfen" },       // "Gotas de Biggoron"
        Text{ "Hyrule's Finest Eyedrops", "Eau du Lac Hylia", "Hyrules beste Augentropfen" }, // "Gotas oculares"
        Text{ "Moon's Tear", "Larme de Lune", "Mondträne" },                                  // "Lágrima de Luna"
        Text{ "Engine Grease", "Graisse moteur", "Schmierfett" },                             // "Grasa del motor"
        Text{ "Zora Perfume", "Parfum Zora", "Zora-Parfüm" },                                 // "Perfume Zora"
    };
    trickNameTable[RG_CLAIM_CHECK] = {
        Text{ "Clay Check", "Certificat Grogoron", "Berg-Urkunde" },      // "Comprobant e de Reclamación"
        Text{ "Ancient Tablet", "Stèle ancienne", "Alte Steintafel" },    // "Litografía arcana"
        Text{ "Sheikah Slate", "Tablette Sheikah", "Shiekah-Stein" },     // "Piedra Sheikah"
        Text{ "Cyclone Slate", "Ardoise des tornades", "Froschkompass" }, // "Pizarra de los Torbellinos"
    };

    trickNameTable[RG_GOLD_SKULLTULA_TOKEN] = {
        Text{ "Skulltula Token", "Bon de Skulltula dorée", "Skulltula-Beweis" },          // "Símbolo de Skulltula"
        Text{ "Golden Skulltula Spirit", "Pièce de Skulltula dorée", "Skulltula-Seele" }, // "Tóken de Skulltula Dorada"
        Text{ "Gold Walltula Token", "Jeton de Walltula dorée", "Walltula-Symbol" },      // "Skulltula dorada"
        Text{ "Maiamai", "Ti'gorneau", "Maimai" },                                        // "Maimai"
        Text{ "Gratitude Crystal", "Cristal de gratitude", "Juwel der Güte" },            // "Gema de gratitud"
        Text{ "Korok Seed", "Noix korogu", "Krog-Samen" },                                // "Semilla de kolog"
    };

    trickNameTable[RG_PROGRESSIVE_HOOKSHOT] = {
        Text{ "Progressive Grappling Hook", "Lance-chaîne (prog.)", "Enterhaken (prog.)" }, // "Garra progresiva"
        Text{ "Progressive Clawshot", "Grappin-griffe (prog.)", "Greifhaken (prog.)" },     // "Zarpa progresiva"
        Text{ "Progressive Gripshot", "Grappince (prog.)", "Tauschhaken (prog.)" },         // "Enganchador progresivo"
        Text{ "Progressive Rope", "Corde (prog.)", "Giftwurm (prog.)" },                    // "Cuerda progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_STRENGTH] = {
        Text{ "Power Glove", "Gant de Puissance (prog.)", "Krafthandschuh (prog.)" },  // "Guanteletes progresivos"
        Text{ "Power Bracelet", "Bracelet de Force (prog.)", "Kraftarmband (prog.)" }, // "Brasaletes progresivos"
        Text{ "Magic Bracelet", "Bracelet Magique (prog.)", "Magiearmband (prog.)" },  // "Manoplas progresivas"
    };
    trickNameTable[RG_PROGRESSIVE_BOMB_BAG] = {
        Text{ "Progressive Bomb Capacity", "Capacité de bombes (prog.)",
              "Bomben-Kapazität (prog.)" }, // "Mayor capacidad de bombas"
        Text{ "Progressive Bomb Pack", "Paquet de bombes (prog.)",
              "Bombenrucksack (prog.)" }, // "Zurrón de bombas progresivo"
        Text{ "Progressive Bomb Box", "Boîte à bombes (prog.)",
              "Bombenschachtel (prog.)" }, // "Bolsa de bombas progresiva"
        Text{ "Progressive Blast Mask", "Masque d'Explosion (prog.)",
              "Ka-Bumm-Maske (prog.)" }, // "Máscara explosiva progresiva"
        Text{ "Progressive Powder Kegs", "Baril de Poudre (prog.)",
              "Pulverfass (prog.)" }, // "Barril de polvo progresivo"
        Text{ "Progressive Remote Bombs", "Bombes à distance (prog.)",
              "Fernzünderbomben (prog.)€" }, // "Bombas remotas progresivas"
    };
    trickNameTable[RG_PROGRESSIVE_BOW] = {
        Text{ "Progressive Arrow Capacity", "Capacité de flèches (prog.)",
              "Pfeil-Kapazität (prog.)" },                                               // "Mayor capacidad de flechas"
        Text{ "Progressive Hero's Bow", "Arc du héros (prog.)", "Heroenbogen (prog.)" }, // "Arco del héroe progresivo"
        Text{ "Progressive Arrow Holder", "Arbalète (prog.)", "Köcher (prog.)" },        // "Ballesta progresiva"
        Text{ "Progressive Crossbow", "Arbalète (prog.)", "Armbrust (prog.)" },          // "Ballesta progresiva"
        Text{ "Progressive Sacred Bow", "Arc sacré (prog)", "Heiliger Bogen (prog.)" },  // "Arco Sagrado Progresivo"
        Text{ "Progressive Lynel Bow", "Arc de Lynel (prog.)",
              "Leunenbogen (prog.)" }, // "Arco de centaleón Progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_SLINGSHOT] = {
        Text{ "Progressive Seed Capacity", "Capacité de graines (prog.)",
              "Kern-Kapazität (prog.)" },                                        // "Mayor capacidad de semillas"
        Text{ "Progressive Catapult", "Catapulte (prog.)", "Katapult (prog.)" }, // "Catapulta progresiva"
        Text{ "Progressive Scattershot", "Lance-Pierre rafale (prog.)",
              "Streuschleuder (prog.)" }, // "Resortera múltiple progresiva"
        Text{ "Progressive Seed Launcher", "Lanceur de semences (prog.)",
              "Blasrohr (prog.)" }, // "Lanzador de semillas progresivo"
        Text{ "Progressive Seed Satchel", "Sac de graines (prog.)",
              "Saatbeutel (prog.)" }, // "Bolsa de semillas progresiva"
    };
    trickNameTable[RG_PROGRESSIVE_WALLET] = {
        Text{ "Progressive Rupee Capacity", "Capacité de rubis (prog.)",
              "Rubin-Kapazität (prog.)" },                                             // "Mayor capacidad de rupias"
        Text{ "Progressive Purse", "Sacoche (prog.)", "Portemonnaie (prog.)" },        // "Cartera de rupias progresiva"
        Text{ "Progressive Rupee Bag", "Sac à rubis (prog.)", "Rubintasche (prog.)" }, // "Zurrón de rupias progresivo"
        Text{ "Progressive Rupoor Capacity", "Capacité de Roupir (prog.)",
              "Rubinfalle (prog.)" }, // "Capacidad progresiva Rupobre"
        Text{ "Progressive Spoils Bag", "Sac à Butin (prog.)", "Beutetasche (prog.)" }, // "Bolsa de trofeos progresiva"
        Text{ "Progressive Ruby Bag", "Capacité du sac Ruby (prog.)", "Rubytasche (prog.)" }, // "Bolso Ruby progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_SCALE] = {
        Text{ "Progressive Flippers", "Palmes de Zora (prog.)",
              "Schwimmflossen (prog.)€" }, // "Aletas de zora progresiva"
        Text{ "Progressive Dragon's Scale", "Écaille du dragon d'eau (prog.)",
              "Drachen-Schuppe (prog.)" }, // "Escama dragón acuático progresiva"
        Text{ "Progressive Diving Ability", "Plongée (prog.)", "Tauchfähigkeit (prog.)" }, // "Buceo progresivo"
        Text{ "Progressive Pearl", "Perle (prog.)", "Perle (prog.)" },                     // "Perla progresiva"
        Text{ "Progressive Scute", "Bulle (prog.)", "Schuppe (prog.)" }, // "Fragmento Zora progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_NUT_UPGRADE] = {
        Text{ "Progressive Nut Pack", "Paquet de noix (prog.)",
              "Nußrucksack (prog.)" },                                                  // "Mayor capacidad de semillas"
        Text{ "Progressive Bait Bag", "Sac à Appâts (prog.)", "Futtertasche (prog.)" }, // "Bolsa de cebo progresiva"
        Text{ "Progressive Pear Capacity", "Capacité de poire (prog.)",
              "Putput-Kapazität (prog.)" },                                        // "Capacidad progresiva de pera"
        Text{ "Progressive Nut Bag", "Sac de noix (prog.)", "Nußbeutel (prog.)" }, // "Bolsa de nueces progresiva"
        Text{ "Progressive Husk Capacity", "Capacité de noisettes (prog.)",
              "Schalen-Kapazität (prog.)€" }, // "Mayor capacidad de castañas"
    };
    trickNameTable[RG_PROGRESSIVE_STICK_UPGRADE] = {
        Text{ "Progressive Stick Bag", "Sac de bâtons (prog.)",
              "Stabbeutel (prog.)" }, // "Mayor capacidad de ramas deku"
        Text{ "Progressive Stick Pack", "Paquet de bâtons Mojo (prog.)",
              "Stabrucksack (prog.)" }, // "Mayor capacidad de bastones"
        Text{ "Progressive Branch Capacity", "Capacité de la succursale (prog.)",
              "Ast-Kapazität (prog.)" }, // "Capacidad progresiva de la sucursal"
        Text{ "Progressive Rod Capacity", "Capacité de tiges (prog.)",
              "Stock-Kapazität (prog.)" }, // "Mayor capacidad de cetros deku"
    };
    trickNameTable[RG_PROGRESSIVE_BOMBCHU_BAG] = {
        Text{ "Progressive Bomblings", "Bombinsectes (prog.)", "Bombenmäuse (prog.)€" }, // "Bombinsectos progresivos"
        Text{ "Progressive Sentrobe Bombs", "Bombe de Sphérodrone (prog.)",
              "Rokopterbomben (prog.)€" },                                          // "Bomba de helicobot progresivo"
        Text{ "Progressive Bomb-ombs", "Bombe Soldat (prog.)", "Bob-omb (prog.)" }, // "Soldado bomba progresivo"
        Text{ "Progressive Missiles", "Missiles (prog.)", "Missiles (prog.)€" },    // "Misiles progresivos"
    };
    trickNameTable[RG_PROGRESSIVE_MAGIC_METER] = {
        Text{ "Progressive Stamina Meter", "Jauge d'endurance (prog.)",
              "Ausdauer (prog.)" }, // "Medidor de vigor progresivo"
        Text{ "Progressive Energy Gauge", "Jauge d'énergie (prog.)",
              "Energie (prog.)" }, // "Medidor de energía progresivo"
        Text{ "Progressive Magic Powder", "Poudre magique (prog.)",
              "Zauberpulver (prog.)" }, // "Medidor de carga progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_OCARINA] = {
        Text{ "Progressive Memento", "Souvenir (prog.)", "Souvenir (prog.)" },   // "Silbato progresivo"
        Text{ "Progressive Whistle", "Siffler (prog.)", "Pfeife (prog.)" },      // "Silbido progresivo"
        Text{ "Progressive Flute", "Flûte (prog.)", "Flöte (prog.)" },           // "Flauta progresiva"
        Text{ "Progressive Recorder", "Harmonica (prog.)", "Rekorder (prog.)" }, // "Armónica progresiva"
    };
    trickNameTable[RG_PROGRESSIVE_GORONSWORD] = {
        Text{ "Progressive Titan Blade", "Lame des Titans (prog.)",
              "Titanenklinge (prog.)" }, // "Hoja del Titán progresiva"
        Text{ "Progressive Goron Knife", "Lame Goron (prog.)", "Goronenmesser (prog.)" }, // "Daga Goron progresiva"
        Text{ "Progressive Giant Sword", "Épée géante (prog.)",
              "Riesenschwert (prog.)" }, // "Espada gigante progresiva"
        Text{ "Progressive Darknut Sword", "Épée de Darknut (prog.)",
              "Gardeschwert (prog.)" }, // "Espada Darknut progresiva"
        Text{ "Progressive Power Sword", "Épée de Puissance (prog.)",
              "Schwert der Macht (prog.)" }, // "Espada de poder progresiva"
        Text{ "Progressive Big Stabby", "Gros coup de poignard (prog.)",
              "Großschwert (prog.)" }, // "Gran puñalada progresiva"
    };

    trickNameTable[RG_EMPTY_BOTTLE] = {
        Text{ "Empty Canteen", "Cantine vide", "Leere Feldflasche" },    // "cantimplora vacía"
        Text{ "Vial of Winds", "Fiole de vents", "Phiole der Winde" },   // "Vial de Vientos"
        Text{ "Tingle Bottle", "Flacon de Tingle", "Tingle-Flasche" },   // "Botella de Tingle"
        Text{ "Magic Bottle", "Flacon magique", "Magische Flasche" },    // "Frasco feérico"
        Text{ "Glass Bottle", "Flacon de verre", "Glasflasche" },        // "Botella de cristal"
        Text{ "Bottle with Water", "Flacon d'eau", "Flasche (Wasser)" }, // "Botella Tingle"
    };
    trickNameTable[RG_BOTTLE_WITH_MILK] = {
        Text{ "Bottle with Chateau Romani", "Flacon de cuvée Romani",
              "Flasche (Chateau Romani)" }, // "Botella de Reserva Romani"
        Text{ "Bottle with Premium Milk", "Flacon avec lait de qualité supérieure",
              "Flasche (Vollmilch)" }, // "Biberón con leche Premium"
        Text{ "Bottle with Mystery Milk", "Flacon de lait grand cru",
              "Flasche (Verdächtige Milch)" }, // "Botella de leche extra"
        Text{ "Bottle with Fresh Milk", "Flacon de lait frais",
              "Flasche (Frische Milch)" }, // "Botella de leche fresca"
    };
    trickNameTable[RG_BOTTLE_WITH_RED_POTION] = {
        Text{ "Bottle with Red Chu Jelly", "Flacon de gelée Chuchu rouge",
              "Flasche (Rotes Schleim-Gelee)" }, // "Jugo de Chuchu Rojo"
        Text{ "Bottle with Hibiscus Potion", "Flacon de potion de Hibiscus",
              "Flasche (Hibiskus-Trank)" }, // "Botella de poción de Hibisco"
        Text{ "Bottle with Medicine of Life", "Flacon d'élixir rouge",
              "Flasche (Roter Trank)" }, // "Botella de medicina de la vida"
        Text{ "Bottle with Heart Potion", "Flacon de potion de soin",
              "Flasche (Herztrank)" }, // "Botella de poción de salud"
    };
    trickNameTable[RG_BOTTLE_WITH_GREEN_POTION] = {
        Text{ "Bottle with Green Chu Jelly", "Flacon de gelée Chuchu verte",
              "Flasche (Grünes Schleim-Gelee)" }, // "Jugo de Chuchu Verde"
        Text{ "Bottle with Lamp Oil", "Flacon de Huile à lanterne",
              "Flasche (Lampenöl)" }, // "Botella de Aceite de candil"
        Text{ "Bottle with Medicine of Magic", "Flacon d'élixir vert",
              "Flasche (Grüner Trank)" }, // "Botella de medicina mágica"
        Text{ "Bottle with Stamina Potion", "Flacon d'Endurol",
              "Flasche (Ausdauertrank)" }, // "Botella de elixir vigorizante"
    };
    trickNameTable[RG_BOTTLE_WITH_BLUE_POTION] = {
        Text{ "Bottle with Blue Chu Jelly", "Flacon de gelée Chuchu bleue",
              "Flasche (Blaues Schleim-Gelee)" }, // "Jugo de Chuchu Azul"
        Text{ "Bottle with Water of Life", "Flacon d'élixir bleu",
              "Flasche (Blauer Trank)" }, // "Botella de agua de la vida"
        Text{ "Bottle with Air Potion", "Flacon de potion d'oxygène", "Flasche (Lufttrank)" }, // "Botella de oxígeno"
    };
    trickNameTable[RG_BOTTLE_WITH_FAIRY] = {
        Text{ "Bottle with Forest Firefly", "Flacon avec une luciole",
              "Flasche (Glühwürmchen)" }, // "Luciérnaga del bosque"
        Text{ "Bottle with Deku Princess", "Flacon avec Deku Princess",
              "Flasche (Deku-Prinzessin)" }, // "Botella con Deku Princess"
        Text{ "Bottle with Stray Fairy", "Flacon avec une fée perdue",
              "Flasche (Verirrte Fee)" }, // "Hada perdida en una botella"
    };
    trickNameTable[RG_BOTTLE_WITH_FISH] = {
        Text{ "Bottle with Small Jabu-Jabu", "Flacon avec mini Jabu-Jabu",
              "Flasche (Baby Jabu)" }, // "Lord Chapu-Chapu embotellado"
        Text{ "Bottle with Reekfish", "Flacon avec Reekfish", "Flasche (Dunstforelle)" }, // "Reekfish embotellada"
        Text{ "Bottle with Hyrule Bass", "Flacon avec perche d'Hyrule",
              "Flasche (Hyrulebarsch)" }, // "Locha de Hyrule embotellada"
        Text{ "Bottle with Hyrule Loach", "Flacon avec loche d'Hyrule",
              "Flasche (Hylianische Forelle)" }, // "Perca de Términa embotellada"
    };
    trickNameTable[RG_BOTTLE_WITH_BLUE_FIRE] = {
        Text{ "Bottle with Will-O-Wisp", "Flacon avec feu follet", "Flasche (Rotes Feuer)" }, // "Botella de llama azul"
        Text{ "Bottle with Ancient Flame", "Flacon de flamme ancienne",
              "Flasche (Antike Flamme)" }, // "Botella de fuego ancestral"
        Text{ "Bottle with a Blue Candle", "Flacon avec une bougie bleue",
              "Flasche (Blaue Kerze)" },                                               // "Botella con una vela azul"
        Text{ "Bottle with Red Ice", "Flacon de Glace Rouge", "Flasche (Rotes Eis)" }, // "Botella de Hielo rojo"
        Text{ "Bottle with Nayru's Flame", "Flacon de flamme de Nayru",
              "Flasche (Nayrus Flamme)" }, // "Botella de llamas de Nayru"
    };
    trickNameTable[RG_BOTTLE_WITH_BUGS] = {
        Text{ "Bottle with Baby Tektites", "Flacon de bébé Araknon",
              "Flasche (Baby Arachno)" },                                             // "Tektites en una botella"
        Text{ "Bottle with A Beetle", "Flacon avec un scarabée", "Flasche (Wanze)" }, // "Botella con un escarabajo"
        Text{ "Bottle with Lanayru Ants", "Flacon de fourmis de Lanelle",
              "Flasche (Ranelle-Ameise)" },                                      // "Celestarabajo embotellado"
        Text{ "Bottle with Insects", "Flacon de bibittes", "Flasche (Insekt)" }, // "Saltabosques embotellados"
        Text{ "Bottle with a Golden Bee", "Flacon avec une abeille dorée",
              "Flasche (Goldene Biene)" }, // "Botella con una abeja dorada"
    };
    trickNameTable[RG_BOTTLE_WITH_POE] = {
        Text{ "Bottle with Ghini", "Flacon avec Ghini", "Flasche (Ghini)" },                // "Ghini en una botella"
        Text{ "Bottle with Reapling", "Flacon avec Âme Damnée", "Flasche (Zombie)" },       // "Reapling en una botella"
        Text{ "Bottle with Imp Poe", "Flacon avec Spectre", "Flasche (Geist)" },            // "Espectro en una botella"
        Text{ "Bottle with Anti-Fairy", "Flacon avec Tetdoss", "Flasche (Knochenfratze)" }, // "Whisp en una botella"
    };

    trickNameTable[RG_RUTOS_LETTER] = {
        Text{ "Bottle with Maggie's Letter", "Flacon avec lettre de Maggy",
              "Flasche (Dolores' Brief)" }, // "Carta de Dolores"
        Text{ "Bottle with Letter to Kafei", "Flacon avec lettre pour Kafei",
              "Flasche (Brief an Kafei)" }, // "Carta para Kafei"
        Text{ "Bottle with Zelda's Letter", "Flacon avec Lettre de Zelda",
              "Flasche (Zeldas Brief)" }, // "Carta náutica"
    };
    trickNameTable[RG_BOTTLE_WITH_BIG_POE] = {
        Text{ "Bottle with Composer Brother", "Flacon avec un compositeur",
              "Flasche (Ludwig Brahmstein)" },                                        // "Hermana Poe embotellada"
        Text{ "Bottle with Jalhalla", "Flacon avec Jalhalla", "Flasche (Jalhalla)" }, // "Yaihalla embotellado"
        Text{ "Bottle with Grim Repoe", "Flacon avec le Faucheur",
              "Flasche (Tagschwärmer)" }, // "Bubble en una botella"
    };

    trickNameTable[RG_ZELDAS_LULLABY] = {
        Text{ "Ballad of the Goddess", "Chant de la déesse", "Hymne der Göttin" }, // "Cántico de la Diosa"
        Text{ "Song of Healing", "Chant de l'apaisement", "Lied der Befreiung" },  // "Canción de curación"
        Text{ "Song of the Hero", "Chant du héros", "Heldenlied" },                // "Canción del héroe"
    };
    trickNameTable[RG_EPONAS_SONG] = {
        Text{ "Song of Birds", "Chant des oiseaux", "Vogelruf" },            // "Cantar del ave"
        Text{ "Song of Soaring", "Chant de l'envol", "Lied der Schwingen" }, // "Canción del viento"
        Text{ "Song of Horse", "Chant du cheval", "Lied des Pferdes" },      // "Chant du cheval"
    };
    trickNameTable[RG_SARIAS_SONG] = {
        Text{ "Mido's Song", "La chanson de Mido", "Midos Lied" },         // "La canción de Mido"
        Text{ "Kass' Theme", "Le thème de Kass", "Kashiwas Thema" },       // "El tema de Kass"
        Text{ "Tune of Echoes", "Chant des Échos ", "Melodie des Echos" }, // "Melodía del Eco"
    };
    trickNameTable[RG_SUNS_SONG] = {
        Text{ "Song of Passing", "Mambo de Manbo", "Kleine Tag- und Nachtmusik" },      // "Melodía del transcurrir"
        Text{ "Command Melody", "Air du marionnettiste", "Sonate des Puppenspielers" }, // "Cara al Sol"
        Text{ "Moon's Song", "La chanson de Moon", "Lied des Mondes" },                 // "La canción de la luna"
    };
    trickNameTable[RG_SONG_OF_TIME] = {
        Text{ "Song of Double Time", "Chant accéléré", "Thema der Zeit im Wind" }, // "Canción del doble tiempo"
        Text{ "Inverted Song of Time", "Chant du temps inversé",
              "Ballade des Kronos" },                                 // "Canción del tiempo invertida"
        Text{ "Tune of Ages", "Chant du Temps", "Melodie der Zeit" }, // "Melodía del Tiempo"
    };
    trickNameTable[RG_SONG_OF_STORMS] = {
        Text{ "Ballad of Gales", "Requiem de la tornade", "Kanon des Sturmes" }, // "Melodía del Tornado"
        Text{ "Frog's Song of Soul", "Rap des grenouilles", "Krötenrap" },       // "Canción del alma de la rana"
        Text{ "Wind's Requiem", "Mélodie du vent", "Lied des Windes" },          // "Melodía del Viento"
    };
    trickNameTable[RG_MINUET_OF_FOREST] = {
        Text{ "Saria's Karaoke", "Karaoké de Saria", "Salias Karaoke" },            // "Dueto del bosque"
        Text{ "Sonata of Awakening", "Sonate de l'éveil", "Sonate des Erwachens" }, // "Sonata del despertar"
        Text{ "Wind God's Aria", "Hymne du dieu du vent",
              "Hymne des Zephirgottes" }, // "Melodía del Espíritu del Viento"
    };
    trickNameTable[RG_BOLERO_OF_FIRE] = {
        Text{ "Darunia's Tango", "Tango de Darunia", "Darunias Tango" },             // "Coro del fuego"
        Text{ "Tune of Currents", "Chants des Flux", "Lied des Zeitstroms" },        // "Melodía de las Corrientes"
        Text{ "Goron Lullaby", "Berceuse des Gorons", "Goronisches Schlummerlied" }, // "Nana goron"
    };
    trickNameTable[RG_SERENADE_OF_WATER] = {
        Text{ "Ruto's Blues", "Blues de Ruto", "Rutos Blues" },                           // "Sonata del agua"
        Text{ "New Wave Bossa Nova", "Bossa-nova des flots", "Bossa Nova der Kaskaden" }, // "Bossanova de las olas"
        Text{ "Manbo's Mambo", "Mambo de Manbo", "Manbos Mambo" },                        // "Mambo de Manbo"
    };
    trickNameTable[RG_REQUIEM_OF_SPIRIT] = {
        Text{ "Nabooru's Reggae", "Reggae de Nabooru", "Naborus Reggae" },          // "Reggae del espíritu"
        Text{ "Elegy of Emptiness", "Hymne du vide", "Elegie des leeren Herzens" }, // "Elegía al vacío"
        Text{ "Earth God's Lyric", "Hymne du dieu de la terre",
              "Hymne des Terragottes" }, // "Melodía del Espíritu de la Tierra"
    };
    trickNameTable[RG_NOCTURNE_OF_SHADOW] = {
        Text{ "Impa's Death Metal", "Death métal d'Impa", "Impas Death Metal" }, // "Diurno de la sombra"
        Text{ "Oath to Order", "Ode de l'appel", "Gesang des Himmels" },         // "Oda al orden"
        Text{ "Song of Discovery", "Chant des secrets", "Schatzsucherlied" },    // "Canto revelador"
    };
    trickNameTable[RG_PRELUDE_OF_LIGHT] = {
        Text{ "Rauru's Sing-Along", "Chansonnette de Rauru", "Raurus Singalong" }, // "Predulio de luz"
        Text{ "Ballad of the Wind Fish", "Ballade sur Poisson-Rêve",
              "Ballade vom Windfisch" },                                   // "Balada del Piez Viento"
        Text{ "Song of Light", "Chant de la lumière", "Lied des Lichts" }, // "Sonidos de la luz"
    };

    trickNameTable[RG_KOKIRI_EMERALD] = {
        Text{ "Pendant of Courage", "Pendentif du courage", "Amulett des Mutes" }, // "Colgante del valor"
        Text{ "Farore's Pearl", "Perle de Farore", "Farores Deamont" },            // "Orbe de Farore"
        Text{ "Aquanine", "Smaragdine", "Smaragdstahl" },                          // "Yerbánida"
        Text{ "Farore's Emerald", "Émeraude de Farore", "Farore-Smaragd" },        // "Esmeralda de Farore"
        Text{ "Kokiri's Peridot", "Péridot Kokiri", "Kokiri-Peridot" },            // "Ágata de los Kokiri"
    };
    trickNameTable[RG_GORON_RUBY] = {
        Text{ "Pendant of Power", "Pendentif de la force", "Amulett der Stärke" }, // "Colgante del poder"
        Text{ "Din's Pearl", "Perle de Din", "Dins Deamont" },                     // "Orbe de Din"
        Text{ "Crimsonine", "Alzanine", "Scharlachstahl" },                        // "Bermellina"
        Text{ "Din's Ruby", "Rubis de Din", "Din-Rubin" },                         // "Rubí de Din"
        Text{ "Goron's Garnet", "Grenat Goron", "Goronen-Granat" },                // "Topacio de los Goron"
    };
    trickNameTable[RG_ZORA_SAPPHIRE] = {
        Text{ "Pendant of Wisdom", "Pendentif de la sagesse", "Amulett der Weisheit" }, // "Colgante de la sabiduría"
        Text{ "Nayru's Pearl", "Perle de Nayru", "Nayrus Deamont" },                    // "Orbe de Nayru"
        Text{ "Azurine", "Aquanine", "Azurstahl" },                                     // "Azurina"
        Text{ "Nayru's Sapphire", "Saphir de Nayru", "Nayru-Saphir" },                  // "Zafiro de Nayru"
        Text{ "Zora's Aquamarine", "Aquamarine Zora", "Zora-Aquamarin" },               // "Lapislázuli de los Zora"
    };
    trickNameTable[RG_FOREST_MEDALLION] = {
        Text{ "Wind Medallion", "Médaillon du vent", "Amulett des Windes" }, // "Medallón del Viento"
        Text{ "Wind Element", "Elément Vent", "Wind-Element" },              // "Elemento de aire"
        Text{ "Saria's Medallion", "Médaillon de Saria", "Salias Amulett" }, // "Medallón de Saria"
        Text{ "Sign of Air", "Glyphe de l'air", "Zeichen der Luft" },        // "Glifo de aire"
        Text{ "Medallion of Forest", "Médaillon du Temple de la Forêt",
              "Medaillon des Waldes" }, // "Medalla del Bosque"
    };
    trickNameTable[RG_FIRE_MEDALLION] = {
        Text{ "Fire Element", "Elément Feu", "Flammen-Element" },                          // "Elemento de fuego"
        Text{ "Darunia's Medallion", "Médaillon de Darunia", "Darunias Amulett" },         // "Medallón de Darunia"
        Text{ "Sign of Fire", "Glyphe de feu", "Zeichen des Feuers" },                     // "Glifo de fuego"
        Text{ "Medallion of Fire", "Médaillon du Temple du Feu", "Medaillon des Feuers" }, // "Medalla del Fuego"
    };
    trickNameTable[RG_WATER_MEDALLION] = {
        Text{ "Water Element", "Elément Eau", "Tropfen-Element" },                             // "Elemento de agua"
        Text{ "Ice Medallion", "Médaillon de glace", "Amulett des Eises" },                    // "Medallón Helado"
        Text{ "Ruto's Medallion", "Médaillon de Ruto", "Rutos Amulett" },                      // "Medallón de Ruto"
        Text{ "Sign of Water", "Glyphe de l'eau", "Zeichen des Wassers" },                     // "Glifo de agua"
        Text{ "Medallion of Water", "Médaillon du Temple de l'Eau", "Medaillon des Wassers" }, // "Medalla del Agua"
    };
    trickNameTable[RG_SPIRIT_MEDALLION] = {
        Text{ "Earth Element", "Elément Terre", "Erd-Element" },                  // "Elemento de tierra"
        Text{ "Nabooru's Medallion", "Médaillon de Nabooru", "Naborus Amulett" }, // "Medallón de Nabooru"
        Text{ "Sign of Earth", "Glyphe de la Terre", "Zeichen der Erde" },        // "Glifo de la tierra"
        Text{ "Medallion of Spirit", "Médaillon du Temple de l'Esprit",
              "Medaillon der Geister" }, // "Medalla del Espíritu"
    };
    trickNameTable[RG_SHADOW_MEDALLION] = {
        Text{ "Fused Shadow", "Cristal d'ombre", "Schattenkristall" },                // "Sombra Fundida"
        Text{ "Impa's Medallion", "Médaillon d'Impa", "Impas Amulett" },              // "Medallón de Impa"
        Text{ "Sign of Illusion", "Glyphe de l'illusion", "Zeichen der Illusionen" }, // "Glifo de ilusión"
        Text{ "Medallion of Shadow", "Médaillon du Temple de l'Ombre",
              "Medaillon des Schattens" }, // "Medalla de la Sombra"
    };
    trickNameTable[RG_LIGHT_MEDALLION] = {
        Text{ "Compass of Light", "Boussole de lumière", "Licht-Kompaß" },                      // "Brújula de Luz"
        Text{ "Rauru's Medallion", "Médaillon de Rauru", "Raurus Amulett" },                    // "Medallón de Rauru"
        Text{ "Sign of Destiny", "Glyphe du destin", "Zeichen des Schicksals" },                // "Glifo del destino"
        Text{ "Medallion of Light", "Médaillon du temple de lumière", "Medaillon des Lichts" }, // "Medalla de la Luz"
    };

    trickNameTable[RG_RECOVERY_HEART] = {
        Text{ "Love", "Bisou", "Liebe" }, // "Te amo"
        Text{ "Life", "Vie", "Leben" },   // "vida"
        Text{ "HP", "PV", "KP" },         // "VP"
    };
    trickNameTable[RG_GREEN_RUPEE] = {
        Text{ "Rupee (1)", "Rubis (1)", "Rubin (1)" },        // "Peso hyliano"
        Text{ "One Ruby", "Un rubis", "ein Ruby" },           // "Un rubí"
        Text{ "One Rupee", "Un rubis", "ein Rubin" },         // "Guaraní hyliano"
        Text{ "False Greg", "Faux Greg", "falscher Greg" },   // "Falso Greg"
        Text{ "Rupoor (1)", "Roupir (1)", "Rubinfalle (1)" }, // "Rupobre (1)"
    };
    trickNameTable[RG_BLUE_RUPEE] = {
        Text{ "Blupee", "Bleubi", "Fünfer" },                 // "Azupia"
        Text{ "Five Rubies", "Cinq Rubys", "fünf Rubies€" },  // "Cinco rubíes"
        Text{ "Five Rupees", "Cinq rubis", "fünf Rubine€" },  // "Bolívar hyliano"
        Text{ "Rupee (5)", "Rubis (5)", "Rubin (5)" },        // "Peso hyliano"
        Text{ "Rupoor (5)", "Roupir (5)", "Rubinfalle (5)" }, // "Rupobre (5)"
    };
    trickNameTable[RG_RED_RUPEE] = {
        Text{ "Big 20", "Grand 20", "Zwanni" },                    // "Los 20 grandes"
        Text{ "Twenty Rubies", "vingt rubis", "zwanzig Rubies€" }, // "Veinte rubíes"
        Text{ "Twenty Rupees", "Vingt rubis", "zwanzig Rubine€" }, // "Colon hyliano"
        Text{ "Rupee (20)", "Rubis (20)", "Rubin (20)" },          // "Peso hyliano"
        Text{ "Rupoor (20)", "Roupir (20)", "Rubinfalle (20)" },   // "Rupobre (20)"
    };
    trickNameTable[RG_PURPLE_RUPEE] = {
        Text{ "Purpee", "Pourbi", "Fuffi" },                          // "morupiua"
        Text{ "Fifty Rubies", "Cinquante rubis", "fünfzig Rubies€" }, // "Cincuenta rubíes"
        Text{ "Fifty Rupees", "Cinquante rubis", "fünfzig Rubine€" }, // "Balboa hyliano"
        Text{ "Rupee (50)", "Rubis (50)", "Rubin (50)" },             // "Peso hyliano"
        Text{ "Rupoor (50)", "Roupir (50)", "Rubinfalle (50)" },      // "Rupobre (50)"
    };
    trickNameTable[RG_HUGE_RUPEE] = {
        Text{ "Hugo", "Or Rubi", "zwei Hunnis€" },                               // "Oro Rubi"
        Text{ "Two Hundred Rubies", "Deux cents rubis", "zweihundert Rubies€" }, // "Doscientos rubíes"
        Text{ "Diamond", "Diamant", "Diamant" },                                 // "Diamante"
        Text{ "Huge Ruby", "Énorme rubis", "großer Ruby" },                      // "Rubi gigante"
        Text{ "Two Hundred Rupees", "Deux cent rubis", "zweihundert Rubine€" },  // "Euro hyliano"
        Text{ "Rupee (200)", "Rubis (200)", "Rubin (200)" },                     // "Dólar hyliano"
    };
    trickNameTable[RG_PIECE_OF_HEART] = {
        Text{ "Pizza Heart", "Fromage de cœur", "Harzteil" },                             // "Pieza de Chorizo"
        Text{ "Little Bit Of Love", "Un peu d'amour", "etwas Liebe" },                    // "Un poco de amor"
        Text{ "Rare Peach Stone", "Pierre de pêche rare", "ein seltener Pfirsichstein" }, // "Pierre de pêche rare"
    };
    trickNameTable[RG_HEART_CONTAINER] = {
        Text{ "Crystal Heart", "Cœur de cristal", "Kristallherz" }, // "Corazón de cristal"
        Text{ "Life Heart", "Cœur de vie", "Lebensherz" },          // "Vida Corazón"
        Text{ "Lots of Love", "Beaucoup d'amour", "viel Liebe" },   // "Mucho amor"
    };
    trickNameTable[RG_TRIFORCE_PIECE] = {
        Text{ "Piece of Cheese", "Morceau de Fromage", "Käseteil" },        // "Piece of Cheese"
        Text{ "Triforce Shard", "Éclat de Triforce", "Triforce-Fragment" }, // "Triforce Shard"
        Text{ "Shiny Rock", "Caillou Brillant", "glänzender Stein" },       // "Shiny Rock"
    };
    trickNameTable[RG_ROCS_FEATHER] = {
        Text{ "Chicken Wing", "Chicken Wing", "Chicken Wing" }, // "Chicken Wing"
        Text{ "Roc's Leg", "Roc's Leg", "Roc's Leg" },          // "Roc's Leg"
        Text{ "Roc's Fapper", "Roc's Fapper", "Roc's Fapper" }, // "Roc's Fapper"
    };
    trickNameTable[RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "Volcano Seed Spirit" },
        Text{ "Bolero Sprout Platform" },
    };
    trickNameTable[RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "Dodongo's Seed Spirit" },
        Text{ "Boulder Sprout Platform" },
    };
    trickNameTable[RG_DESERT_COLOSSUS_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "Spirit Temple Seed Spirit" },
        Text{ "Colossus Arch Sprout Platform" },
    };
    trickNameTable[RG_GERUDO_VALLEY_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "Waterfall Seed Spirit" },
        Text{ "Gerudo Cow Sprout Platform" },
    };
    trickNameTable[RG_GRAVEYARD_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "GY Crate Seed Spirit" },
        Text{ "Dampe's Sprout Platform" },
    };
    trickNameTable[RG_KOKIRI_FOREST_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "Rupee Ledge Seed Spirit" },
        Text{ "KF Shop Sprout Platform" },
    };
    trickNameTable[RG_LAKE_HYLIA_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "Hylia Lab Seed Spirit" },
        Text{ "Fishing Sprout Platform" },
    };
    trickNameTable[RG_LOST_WOODS_BRIDGE_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "LW Bridge Seed Spirit" },
        Text{ "Skull Kid Sprout Platform" },
    };
    trickNameTable[RG_LOST_WOODS_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "Deku Theatre Seed Spirit" },
        Text{ "Deku Scrubs Sprout Platform" },
    };
    trickNameTable[RG_ZORAS_RIVER_BEAN_SOUL] = {
        // TODO_TRANSLATE
        Text{ "River Ride Seed Spirit" },
        Text{ "Bean Salesman Sprout Platform" },
    };
    trickNameTable[RG_GOHMA_SOUL] = {
        Text{ "Spider Sense", "Sens de l'Araignée", "Spinnensinn" },
        Text{ "Deku Spirit", "Parasite Mojo", "Deku Geist" },
        Text{ "Ghost of Ghoma", "Fantôme de Gohma", "Gohmas Geist" },
    };
    trickNameTable[RG_KING_DODONGO_SOUL] = {
        Text{ "Lizard Soul", "Âme d'un Lézard", "Reptilienseele" },
        Text{ "Regal Remains", "Restes Délicieux", "royale Überreste€" },
        Text{ "Dodongo's Core", "Coeur de Dodongo", "Dodongos Kern" },
    };
    trickNameTable[RG_BARINADE_SOUL] = {
        Text{ "Parasitic Poltergeist", "Poltergeist Parasite", "infektiöser Poltergeist" },
        Text{ "Jabu Insides", "Entrailles de Jabu-Jabu", "Jabus Innereien" },
        Text{ "Barinade Bacteria", "Bactérie de Barinade", "Barinades Bakterien" },
    };
    trickNameTable[RG_PHANTOM_GANON_SOUL] = {
        Text{ "Bigger Poe", "Âme Gigantesque", "ein großer Nachtschwärmer" },
        Text{ "Sacred Forest Pine Tree", "Grande Perche du Bosquet Sacré", "Kiefer der Heiligen Lichtung" },
        Text{ "Ganon's Phantom", "Fantôme de Ganon", "Ganons Phantom" },
    };
    trickNameTable[RG_VOLVAGIA_SOUL] = {
        Text{ "Dragon Roast", "Friture du Dragon", "Drachenbraten" },
        Text{ "Hot n' Ready", "Sauce Barbecue", "Barbecue Sauce" },
        Text{ "Volvagia's Vitality", "Vitalité de Volcania", "Volvagias Vitalität" },
    };
    trickNameTable[RG_MORPHA_SOUL] = {
        Text{ "Dihydrogen Monoxide", "Monoxyde de Dihydrogène", "Dihydrogenmonoxid" },
        Text{ "Morpha Molecules", "Molécule de Morpha", "Morphas Moleküle" },
        Text{ "Wet Stuff", "Truc Mouillé", "nasses Zeug" },
    };
    trickNameTable[RG_BONGO_BONGO_SOUL] = {
        Text{ "Shadow Soul", "Âme de l'Ombre", "Schattenseele" },
        Text{ "Dark Essence", "Essence Sombre", "Dunkle Essenz" },
        Text{ "Bongo Bongo's Bongo", "Bongo de Bongo Bongo", "Bongo Bongos Bongo" },
    };
    trickNameTable[RG_TWINROVA_SOUL] = {
        Text{ "Sandy Ashes", "Cendres des Vieilles", "Sandige Asche" },
        Text{ "Spiritual Spirit", "Esprit Spirituel", "Geisterhafter Geist" },
        Text{ "Twin Rovers", "Duo Angélique", "Twinrovas Seile" },
    };
    trickNameTable[RG_GANON_SOUL] = {
        Text{ "Pure Evil", "Mal Incarné", "Das pure Böse" },
        Text{ "Ganon's Ghost", "Le Malin", "Ganons Geist" },
        Text{ "Pork", "Porc", "Schweinefleisch" },
    };
    trickNameTable[RG_FISHING_POLE] = {
        Text{ "Fish Tickler", "Fish Tickler", "Fischkitzler" },
        Text{ "Floating Lure", "Floating Lure", "Schwimmer" },
        Text{ "Fishing Reel", "Fishing Reel", "Angelschnur" },
    };
    trickNameTable[RG_SKELETON_KEY] = {
        // TODO_TRANSLATE
        Text{ "Stalfos Key" }, Text{ "Nightmare Key" }, Text{ "Graveyard Key" },
        Text{ "King's Key" },  Text{ "Hero's Key" },
    };
    trickNameTable[RG_CLIMB] = {
        // TODO_TRANSLATE
        Text{ "Cling" },
    };
    trickNameTable[RG_CRAWL] = {
        // TODO_TRANSLATE
        Text{ "Crouch" },
    };
    trickNameTable[RG_OPEN_CHEST] = {
        // TODO_TRANSLATE
        Text{ "Open Cheats" },
    };

    trickNameTable[RG_OCARINA_A_BUTTON] = {
        Text{ "Ocarina J Button", "Touche Ha de l'Ocarina", "J-Taste der Okarina" },
        Text{ "Ocarina Ayy Button", "Touche Ah de l'Ocarina", "A-Taste der Flöte" },
        Text{ "Ocarina A Trigger", "Bumper A de l'Ocarina", "A-Trigger der Okarina" },
    };
    trickNameTable[RG_OCARINA_C_UP_BUTTON] = {
        Text{ "Ocarina C North Button", "Touche C Nord de l'Ocarina", "C-Nord-Taste der Okarina" },
        Text{ "Ocarina C App Button", "Touche C'est Haut de l'Ocarina", "C-Oben-Taste des Rekorders" },
        Text{ "Ocarina Sup Button", "Touche O de l'Ocarina", "C-Oben-Trigger der Okarina" },
    };
    trickNameTable[RG_OCARINA_C_DOWN_BUTTON] = {
        Text{ "Ocarina C South Button", "Touche C Sud de l'Ocarina", "C-Süd-Taste der Okarina" },
        Text{ "Ocarina Z Down Button", "Touche Z Bas de l'Ocarina", "Z-Unten-Taste der Okarina" },
        Text{ "Ocarina See Down Button", "Touche C'est Bas de l'Ocarina", "C-Unten-Taste der Harfe" },
        Text{ "Ocarina C Dawn Button", "Touche Séba de l'Ocarina", "C-Unten-Trigger der Okarina" },
    };
    trickNameTable[RG_OCARINA_C_LEFT_BUTTON] = {
        Text{ "Ocarina C West Button", "Touche C Ouest de l'Ocarina", "C-West-Taste der Okarina" },
        Text{ "Ocarina Sea Left Button", "Touche Cégoche de L'Ocarina", "C-Links-Taste der Nixenglocke" },
        Text{ "Ocarina C Lift Button", "Touche C'est Gauche de l'Ocarina", "C-Zelda-Taste der Okarina" },
        Text{ "Ocarina Rewind Button", "Touche Rembobiner de l'Ocarina", "C-Links-Trigger der Okarina" },
    };
    trickNameTable[RG_OCARINA_C_RIGHT_BUTTON] = {
        Text{ "Ocarina C East Button", "Touche C Est de l'Ocarina", "C-Ost-Taste der Okarina" },
        Text{ "Ocarina C Wright Button", "Touche C'est Droite de l'Ocarina", "C-Rechts-Trigger der Okarina" },
        Text{ "Overworld C Right Button", "Trou Droit de l'Ocarina", "C-Rechts-Taste der E-Gitarre" },
    };

    trickNameTable[RG_GREG_RUPEE] = {
        // TODO_TRANSALTE
        Text{ "Morshu the Green Ruby", "Morshu the Green Ruby", "Morshu the Green Ruby" },
        Text{ "Geoffrey the Gray Rupoor", "Geoffrey the Gray Rupoor", "Geoffrey the Gray Rupoor" },
        Text{ "Validation Rupee", "Validation Rupee", "Validation Rupee" },
        Text{ "Gary, just Gary", "Gary, just Gary", "Gary, just Gary" },
        Text{ "Ike the Indigo Ice Trap", "Ike the Indigo Ice Trap", "Ike the Indigo Ice Trap" },
    };

    trickNameTable[RG_FOREST_TEMPLE_SMALL_KEY] = {
        // TODO_TRANSALTE
        Text{ "Wind Temple Smol Key", "Wind Temple Smol Key", "Wind Temple Smol Key" },
        Text{ "Woodfall Temple Small Key", "Woodfall Temple Small Key", "Woodfall Temple Small Key" },
        Text{ "Skull Woods Small Key", "Skull Woods Small Key", "Skull Woods Small Key" },
    };
    trickNameTable[RG_FIRE_TEMPLE_SMALL_KEY] = {
        // TODO_TRANSALTE
        Text{ "Ice Cavern Small Keese", "Ice Cavern Small Keese", "Ice Cavern Small Keese" },
        Text{ "Goron Temple Small Key", "Goron Temple Small Key", "Goron Temple Small Key" },
        Text{ "Eldin Temple Salmon Koi", "Eldin Temple Salmon Koi", "Eldin Temple Salmon Koi" },
    };
    trickNameTable[RG_WATER_TEMPLE_SMALL_KEY] = {
        // TODO_TRANSALTE
        Text{ "Swamp Palace Small Keese", "Swamp Palace Small Keese", "Swamp Palace Small Keese" },
        Text{ "Great Bay Temple Small Key", "Great Bay Temple Small Key", "Great Bay Temple Small Key" },
        Text{ "Lakebed Temple Small Key", "Lakebed Temple Small Key", "Lakebed Temple Small Key" },
    };
    trickNameTable[RG_SPIRIT_TEMPLE_SMALL_KEY] = {
        // TODO_TRANSALTE
        Text{ "Light Temple Small Key", "Light Temple Small Key", "Light Temple Small Key" },
        Text{ "Lightning Temple Smol Key", "Lightning Temple Smol Key", "Lightning Temple Smol Key" },
        Text{ "Desert Palace Small Key", "Desert Palace Small Key", "Desert Palace Small Key" },
        Text{ "Stone Tower Small Keese", "Stone Tower Small Keese", "Stone Tower Small Keese" },
    };
    trickNameTable[RG_SHADOW_TEMPLE_SMALL_KEY] = {
        // TODO_TRANSALTE
        Text{ "Palace of Darkness Small Key", "Palace of Darkness Small Key", "Palace of Darkness Small Key" },
        Text{ "Shrine of Illusion Salmon Koi", "Shrine of Illusion Salmon Koi", "Shrine of Illusion Salmon Koi" },
        Text{ "Palace of Twilight Small Key", "Palace of Twilight Small Key", "Palace of Twilight Small Key" },
    };
    trickNameTable[RG_BOTTOM_OF_THE_WELL_SMALL_KEY] = {
        // TODO_TRANSALTE
        Text{ "Top of the Wall Small Key", "Top of the Wall Small Key", "Top of the Wall Small Key" },
        Text{ "Breath of the Wild Small Key", "Breath of the Wild Small Key", "Breath of the Wild Small Key" },
        Text{ "Beneath the Well Small Key", "Beneath the Well Small Key", "Beneath the Well Small Key" },
    };
    trickNameTable[RG_GERUDO_TRAINING_GROUND_SMALL_KEY] = {
        // TODO_TRANSALTE
        Text{ "Gerudo Sanctum Small Key", "Gerudo Sanctum Small Key", "Gerudo Sanctum Small Key" },
        Text{ "Lady's Lair Small Keese", "Lady's Lair Small Keese", "Lady's Lair Small Keese" },
        Text{ "Knight Acadamy Small Key", "Knight Acadamy Small Key", "Knight Acadamy Small Key" },
    };
    trickNameTable[RG_GERUDO_FORTRESS_SMALL_KEY] = {
        // TODO_TRANSALTE
        Text{ "Fortress of Winds Small Key", "Fortress of Winds Small Key", "Fortress of Winds Small Key" },
        Text{ "Thieve's Town Small Key", "Thieve's Town Small Key", "Thieve's Town Small Key" },
        Text{ "Fortress Centrum Small Key", "Fortress Centrum Small Key", "Fortress Centrum Small Key" },
        Text{ "Forsaken Fortress Smol Key", "Forsaken Fortress Smol Key", "Forsaken Fortress Smol Key" },
        Text{ "Pirate's Fortress Small Key", "Pirate's Fortress Small Key", "Pirate's Fortress Small Key" },
    };
    trickNameTable[RG_GANONS_CASTLE_SMALL_KEY] = {
        // TODO_TRANSALTE
        Text{ "Hyrule Castle Salmon Koi", "Hyrule Castle Salmon Koi", "Hyrule Castle Salmon Koi" },
        Text{ "Onox's Castle Small Key", "Onox's Castle Small Key", "Onox's Castle Small Key" },
        Text{ "Vaati's Palace Small Key", "Vaati's Palace Small Key", "Vaati's Palace Small Key" },
    };

    trickNameTable[RG_FOREST_TEMPLE_KEY_RING] = {
        // TODO_TRANSALTE
        Text{ "Wind Temple Key Ring", "Wind Temple Key Ring", "Wind Temple Key Ring" },
        Text{ "Woodfall Temple Key Ring", "Woodfall Temple Key Ring", "Woodfall Temple Key Ring" },
        Text{ "Skull Woods Key Ring", "Skull Woods Key Ring", "Skull Woods Key Ring" },
    };
    trickNameTable[RG_FIRE_TEMPLE_KEY_RING] = {
        // TODO_TRANSALTE
        Text{ "Ice Cavern Keese Ring", "Ice Cavern Keese Ring", "Ice Cavern Keese Ring" },
        Text{ "Goron Temple Key Ring", "Goron Temple Key Ring", "Goron Temple Key Ring" },
        Text{ "Eldin Temple Koi Ray", "Eldin Temple Koi Ray", "Eldin Temple Koi Ray" },
    };
    trickNameTable[RG_WATER_TEMPLE_KEY_RING] = {
        // TODO_TRANSALTE
        Text{ "Swamp Palace Keese Ring", "Swamp Palace Keese Ring", "Swamp Palace Keese Ring" },
        Text{ "Great Bay Temple Key Ring", "Great Bay Temple Key Ring", "Great Bay Temple Key Ring" },
        Text{ "Lakebed Temple Key Ring", "Lakebed Temple Key Ring", "Lakebed Temple Key Ring" },
    };
    trickNameTable[RG_SPIRIT_TEMPLE_KEY_RING] = {
        // TODO_TRANSALTE
        Text{ "Light Temple Key Ring", "Light Temple Key Ring", "Light Temple Key Ring" },
        Text{ "Lightning Temple Key Ring", "Lightning Temple Key Ring", "Lightning Temple Key Ring" },
        Text{ "Desert Palace Key Ring", "Desert Palace Key Ring", "Desert Palace Key Ring" },
        Text{ "Stone Tower Keese Ring", "Stone Tower Keese Ring", "Stone Tower Keese Ring" },
    };
    trickNameTable[RG_SHADOW_TEMPLE_KEY_RING] = {
        // TODO_TRANSALTE
        Text{ "Palace of Darkness Key Ring", "Palace of Darkness Key Ring", "Palace of Darkness Key Ring" },
        Text{ "Shrine of Illusion Koi Ray", "Shrine of Illusion Koi Ray", "Shrine of Illusion Koi Ray" },
        Text{ "Palace of Twilight Key Ring", "Palace of Twilight Key Ring", "Palace of Twilight Key Ring" },
    };
    trickNameTable[RG_BOTTOM_OF_THE_WELL_KEY_RING] = {
        // TODO_TRANSALTE
        Text{ "Top of the Wall Key Ring", "Top of the Wall Key Ring", "Top of the Wall Key Ring" },
        Text{ "Breath of the Wild Key Ring", "Breath of the Wild Key Ring", "Breath of the Wild Key Ring" },
        Text{ "Beneath the Well Key Ring", "Beneath the Well Key Ring", "Beneath the Well Key Ring" },
    };
    trickNameTable[RG_GERUDO_TRAINING_GROUND_KEY_RING] = {
        // TODO_TRANSALTE
        Text{ "Gerudo Sanctum Key Ring", "Gerudo Sanctum Key Ring", "Gerudo Sanctum Key Ring" },
        Text{ "Lady's Lair Keese Ring", "Lady's Lair Keese Ring", "Lady's Lair Keese Ring" },
        Text{ "Knight Acadamy Key Ring", "Knight Acadamy Key Ring", "Knight Acadamy Key Ring" },
    };
    trickNameTable[RG_GERUDO_FORTRESS_KEY_RING] = {
        // TODO_TRANSALTE
        Text{ "Fortress of Winds Key Ring", "Fortress of Winds Key Ring", "Fortress of Winds Key Ring" },
        Text{ "Thieve's Town Key Ring", "Thieve's Town Key Ring", "Thieve's Town Key Ring" },
        Text{ "Fortress Centrum Key Ring", "Fortress Centrum Key Ring", "Fortress Centrum Key Ring" },
        Text{ "Forsaken Fortress Key Ring", "Forsaken Fortress Key Ring", "Forsaken Fortresse Key Ring" },
        Text{ "Pirate's Fortress Key Ring", "Pirate's Fortress Key Ring", "Pirate's Fortress Key Ring" },
    };
    trickNameTable[RG_GANONS_CASTLE_KEY_RING] = {
        // TODO_TRANSALTE
        Text{ "Hyrule Castle Koi Ray", "Hyrule Castle Koi Ray", "Hyrule Castle Koi Ray" },
        Text{ "Onox's Castle Key Ring", "Onox's Castle Key Ring", "Onox's Castle Key Ring" },
        Text{ "Vaati's Palace Key Ring", "Vaati's Palace Key Ring", "Vaati's Palace Key Ring" },
    };

    trickNameTable[RG_FOREST_TEMPLE_BOSS_KEY] = {
        // TODO_TRANSALTE
        Text{ "Wind Temple Boss Key", "Wind Temple Boss Key", "Wind Temple Boss Key" },
        Text{ "Woodfall Temple Boss Key", "Woodfall Temple Boss Key", "Woodfall Temple Boss Key" },
        Text{ "Skull Woods Boss Key", "Skull Woods Boss Key", "Skull Woods Boss Key" },
        Text{ "Phantom Ganon's Key", "Phantom Ganon's Key", "Phantom Ganon's Key" },
        Text{ "Deku Tree's Boss Key", "Deku Tree's Boss Key", "Deku Tree's Boss Key" },
    };
    trickNameTable[RG_FIRE_TEMPLE_BOSS_KEY] = {
        // TODO_TRANSALTE
        Text{ "Ice Cavern Boss Keese", "Ice Cavern Boss Keese", "Ice Cavern Boss Keese" },
        Text{ "Goron Temple Boss Key", "Goron Temple Boss Key", "Goron Temple Boss Key" },
        Text{ "Eldin Temple Boss Koi", "Eldin Temple Boss Koi", "Eldin Temple Boss Koi" },
        Text{ "Volvagia's Key", "Volvagia's Key", "Volvagia's Key" },
        Text{ "Dodongo's Cavern Boss Key", "Dodongo's Cavern Boss Key", "Dodongo's Cavern Boss Key" },
    };
    trickNameTable[RG_WATER_TEMPLE_BOSS_KEY] = {
        // TODO_TRANSALTE
        Text{ "Swamp Palace Boss Keese", "Swamp Palace Boss Keese", "Swamp Palace Boss Keese" },
        Text{ "Great Bay Temple Boss Key", "Great Bay Temple Boss Key", "Great Bay Temple Boss Key" },
        Text{ "Lakebed Temple Boss Key", "Lakebed Temple Boss Key", "Lakebed Temple Boss Key" },
        Text{ "Morpha's Key", "Morpha's Key", "Morpha's Key" },
        Text{ "Jabu Jabu's Belly Boss Key", "Jabu Jabu's Belly Boss Key", "Jabu Jabu's Belly Boss Key" },
    };
    trickNameTable[RG_SPIRIT_TEMPLE_BOSS_KEY] = {
        // TODO_TRANSALTE
        Text{ "Light Temple Boss Key", "Light Temple Boss Key", "Light Temple Boss Key" },
        Text{ "Lightning Temple Boss Key", "Lightning Temple Boss Key", "Lightning Temple Boss Key" },
        Text{ "Desert Palace Boss Key", "Desert Palace Boss Key", "Desert Palace Boss Key" },
        Text{ "Stone Tower Boss Keese", "Stone Tower Boss Keese", "Stone Tower Boss Keese" },
        Text{ "Twinrova's Key", "Twinrova's Key", "Twinrova's Key" },
    };
    trickNameTable[RG_SHADOW_TEMPLE_BOSS_KEY] = {
        // TODO_TRANSALTE
        Text{ "Palace of Darkness Boss Key", "Palace of Darkness Boss Key", "Palace of Darkness Boss Key" },
        Text{ "Shrine of Illusion Bass Koi", "Shrine of Illusion Bass Koi", "Shrine of Illusion Bass Koi" },
        Text{ "Palace of Twilight Boss Key", "Palace of Twilight Boss Key", "Palace of Twilight Boss Key" },
        Text{ "Bongo Bongo's Key", "Bongo Bongo's Key", "Bongo Bongo's Key" },
    };
    trickNameTable[RG_GANONS_CASTLE_BOSS_KEY] = {
        // TODO_TRANSALTE
        Text{ "Hyrule Castle Bass Koi", "Hyrule Castle Bass Koi", "Hyrule Castle Bass Koi" },
        Text{ "Onox's Castle Boss Key", "Onox's Castle Boss Key", "Onox's Castle Boss Key" },
        Text{ "Vaati's Palace Boss Key", "Vaati's Palace Boss Key", "Vaati's Palace Boss Key" },
        Text{ "Ganondorf's Key", "Ganondorf's Key", "Ganondorf's Key" },
    };

    trickNameTable[RG_KEATON_MASK] = {
        // TODO_TRANSLATE
        Text{ "Korok Mask" },
        Text{ "Lynel Mask" },
        Text{ "Cucco Mask" },
        Text{ "Remlit Mask" },
    };
    trickNameTable[RG_SKULL_MASK] = {
        // TODO_TRANSLATE
        Text{ "Darknut Mask" },
        Text{ "Stalfos Mask" },
        Text{ "Captain's Hat" },
    };
    trickNameTable[RG_SPOOKY_MASK] = {
        // TODO_TRANSLATE
        Text{ "Gibdo Mask" },
        Text{ "Garo's Mask" },
        Text{ "Redead mask" },
    };
    trickNameTable[RG_BUNNY_HOOD] = {
        // TODO_TRANSLATE
        Text{ "Bunny Mask" },
        Text{ "Bremen Mask" },
        Text{ "Rabbit Hood" },
    };
    trickNameTable[RG_MASK_OF_TRUTH] = {
        // TODO_TRANSLATE
        Text{ "Feirce Diety Mask" },
        Text{ "Majora's Mask" },
        Text{ "Hero's Charm" },
    };
    trickNameTable[RG_GORON_MASK] = {
        // TODO_TRANSLATE
        Text{ "Stone Mask" },
        Text{ "Darmani's Mask" },
        Text{ "Goron Garb" },
    };
    trickNameTable[RG_ZORA_MASK] = {
        // TODO_TRANSLATE
        Text{ "Zora Costume" },
        Text{ "Don Gero's Mask" },
        Text{ "Mikau's Mask" },
    };
    trickNameTable[RG_GERUDO_MASK] = {
        // TODO_TRANSLATE
        Text{ "Great Fairy Mask" },
        Text{ "Romani's Mask" },
        Text{ "Gerudo Veil" },
    };

    trickNameTable[RG_SPEAK_DEKU] = {
        // TODO_TRANSLATE
        Text{ "Picori Jabber Nut" },
        Text{ "Kikwi Blabber Nut" },
        Text{ "Talking Deku Nut" },
    };
    trickNameTable[RG_SPEAK_GERUDO] = {
        // TODO_TRANSLATE
        Text{ "Gerudo Tuni Nut" },
        Text{ "Twili Jabber Nut" },
        Text{ "Zuna Blabber Nut" },
    };
    trickNameTable[RG_SPEAK_GORON] = {
        // TODO_TRANSLATE
        Text{ "Cobble Jabber Nut" },
        Text{ "Mogma Blabber Nut" },
        Text{ "Goron Speak Stone" },
    };
    trickNameTable[RG_SPEAK_HYLIAN] = {
        // TODO_TRANSLATE
        Text{ "Human Jingle Nut" },
        Text{ "Sheikah Jabber nut" },
        Text{ "Lorulean Blabber Nut" },
    };
    trickNameTable[RG_SPEAK_KOKIRI] = {
        // TODO_TRANSLATE
        Text{ "Korok Blabber Nut" },
        Text{ "Minish Jabber Nut" },
        Text{ "Fairy Jingle Nut" },
    };
    trickNameTable[RG_SPEAK_ZORA] = {
        // TODO_TRANSLATE
        Text{ "Parella Jabber Nut" },
        Text{ "Fishman Jabber Nut" },
        Text{ "Zora Jabu Nut" },
        Text{ "Rito Jingle Nut" },
    };

    trickNameTable[RG_GUARD_HOUSE_KEY] = {
        // TODO_TRANSLATE
        Text{ "Pot Room Key", "Pot Room Key", "Pot Room Key" },
        Text{ "Poe Shop Keese", "Poe Shop Keese", "Poe Shop Keese" },
        Text{ "Pot Collectors Club Key", "Pot Collectors Club Key", "Pot Collectors Club Key" },
    };
    trickNameTable[RG_MARKET_BAZAAR_KEY] = {
        // TODO_TRANSLATE
        Text{ "Malo Mart Key", "Malo Mart Key", "Malo Mart Key" },
        Text{ "Zora Shop Key", "Zora Shop Key", "Zora Shop Key" },
        Text{ "Goronu General Store Key", "Goronu General Store Key", "Goronu General Store Key" },
        Text{ "Chudly's Fine Goods Key", "Chudly's Fine Goods Key", "Chudly's Fine Goods Key" },
    };
    trickNameTable[RG_MARKET_POTION_SHOP_KEY] = {
        // TODO_TRANSLATE
        Text{ "Market Medicine Shop Koi", "Market Medicine Shop Koi", "Market Medicine Shop Koi" },
        Text{ "Market Pharmacy Key", "Market Pharmacy Key", "Market Pharmacy Key" },
        Text{ "Market Drug Store Keese", "Market Drug Store Keese", "Market Drug Store Keese" },
    };
    trickNameTable[RG_MASK_SHOP_KEY] = {
        // TODO_TRANSLATE
        Text{ "Masked Ship Koi", "Masked Ship Koi", "Masked Ship Koi" },
        Text{ "Madame Couture's Key", "Madame Couture's Key", "Madame Couture's Key" },
        Text{ "South Clock Town Key", "South Clock Town Key", "South Clock Town Key" },
    };
    trickNameTable[RG_MARKET_SHOOTING_GALLERY_KEY] = {
        // TODO_TRANSLATE
        Text{ "Swamp Shooting Gallery Key", "Swamp Shooting Gallery Key", "Swamp Shooting Gallery Key" },
        Text{ "Koume's Target Shooting Key", "Koume's Target Shooting Key", "Koume's Target Shooting Key" },
        Text{ "Pumpkin Pull Key", "Pumpkin Pull Key", "Pumpkin Pull Key" },
    };
    trickNameTable[RG_BOMBCHU_BOWLING_KEY] = {
        // TODO_TRANSLATE
        Text{ "Bombchu Gallery Key", "Bombchu Gallery Key", "Bombchu Gallery Key" },
        Text{ "Cucco Bowling Ally Key", "Cucco Bowling Ally Key", "Cucco Bowling Ally Key" },
        Text{ "Snowball Bowling Key", "Snowball Bowling Key", "Snowball Bowling Key" },
        Text{ "Bombsketball Key", "Bombsketball Key", "Bombsketball Key" },
    };
    trickNameTable[RG_TREASURE_CHEST_GAME_BUILDING_KEY] = {
        // TODO_TRANSLATE
        Text{ "Lucky Treasure Game Koi", "Lucky Treasure Game Koi", "Lucky Treasure Game Koi" },
        Text{ "1 in 32 Key", "1 in 32 Key", "1 in 32 Key" },
        Text{ "Fortune's Coice Key", "Fortune's Coice Key", "Fortune's Coice Key" },
        Text{ "Money Making Game Key", "Money Making Game Key", "Money Making Game Key" },
        Text{ "Trading Card Game Key", "Trading Card Game Key", "Trading Card Game Key" },
    };
    trickNameTable[RG_BOMBCHU_SHOP_KEY] = {
        // TODO_TRANSLATE
        Text{ "Curiosity Shop Key", "Curiosity Shop Key", "Curiosity Shop Key" },
        Text{ "Barnes Bomb Shop Key", "Barnes Bomb Shop Key", "Barnes Bomb Shop Key" },
        Text{ "Bomb Flower Shop Key", "Bomb Flower Shop Key", "Bomb Flower Shop Key" },
    };
    trickNameTable[RG_RICHARDS_HOUSE_KEY] = {
        // TODO_TRANSLATE
        Text{ "Stockwell's House Key", "Stockwell's House Key", "Stockwell's House Key" },
        Text{ "Blue Dog's House Key", "Blue Dog's House Key", "Blue Dog's House Key" },
        Text{ "Barkle's House Key", "Barkle's House Key", "Barkle's House Key" },
        Text{ "Chimimi's House Key", "Chimimi's House Key", "Chimimi's House Key" },
    };
    trickNameTable[RG_ALLEY_HOUSE_KEY] = {
        // TODO_TRANSLATE
        Text{ "Mido's House Key", "Mido's House Key", "Mido's House Key" },
        Text{ "Saria's House Key", "Saria's House Key", "Saria's House Key" },
        Text{ "@'s House Key", "@'s House Key", "@'s House Key" },
    };
    trickNameTable[RG_KAK_BAZAAR_KEY] = {
        // TODO_TRANSLATE
        Text{ "Skyloft Bazaar Key", "Skyloft Bazaar Key", "Skyloft Bazaar Key" },
        Text{ "Harlequin Bazaar Key", "Harlequin Bazaar Key", "Harlequin Bazaar Key" },
        Text{ "Kokiri Shop Key", "Kokiri Shop Key", "Kokiri Shop Key" },
        Text{ "Goron Shop Key", "Goron Shop Key", "Goron Shop Key" },
    };
    trickNameTable[RG_KAK_POTION_SHOP_KEY] = {
        // TODO_TRANSLATE
        Text{ "Kak Medicine Shop Keep", "Kak Medicine Shop Keep", "Kak Medicine Shop Keep" },
        Text{ "Kak Pharmacy Key", "Kak Pharmacy Key", "Kak Pharmacy Key" },
        Text{ "Kak Drug Store Keese", "Kak Drug Store Keese", "Kak Drug Store Keese" },
    };
    trickNameTable[RG_BOSS_HOUSE_KEY] = {
        // TODO_TRANSLATE
        Text{ "Kakariko Village Boss Key", "Kakariko Village Boss Key", "Kakariko Village Boss Key" },
        Text{ "Lord Kohga's House Key", "Lord Kohga's House Key", "Lord Kohga's House Key" },
        Text{ "Twinrova's House Key", "Twinrova's House Key", "Twinrova's House Key" },
    };
    trickNameTable[RG_GRANNYS_POTION_SHOP_KEY] = {
        // TODO_TRANSLATE
        Text{ "Grandpa's Potion Shop Key", "Grandpa's Potion Shop Key", "Grandpa's Potion Shop Key" },
        Text{ "Witch's Hut Key", "Witch's Hut Key", "Witch's Hut Key" },
        Text{ "Hags's Potion Shop Key", "Hags's Potion Shop Key", "Hags's Potion Shop Key" },
        Text{ "Syrup's Potion Shop Key", "Syrup's Potion Shop Key", "Syrup's Potion Shop Key" },
    };
    trickNameTable[RG_SKULLTULA_HOUSE_KEY] = {
        // TODO_TRANSLATE
        Text{ "Town Spider House Key", "Town Spider House Key", "Town Spider House Key" },
        Text{ "Jovani's House Key", "Jovani's House Key", "Jovani's House Key" },
        Text{ "Maiamai House Key", "Maiamai House Key", "Maiamai House Key" },
    };
    trickNameTable[RG_IMPAS_HOUSE_KEY] = {
        // TODO_TRANSLATE
        Text{ "Zelda's House Key", "Zelda's House Key", "Zelda's House Key" },
        Text{ "Sheik's House Key", "Sheik's House Key", "Sheik's House Key" },
        Text{ "Purah's House Key", "Purah's House Key", "Purah's House Key" },
    };
    trickNameTable[RG_WINDMILL_KEY] = {
        // TODO_TRANSLATE
        Text{ "Wind Switch Key", "Wind Switch Key", "Wind Switch Key" },
        Text{ "Weather Vane Key", "Weather Vane Key", "Weather Vane Key" },
    };
    trickNameTable[RG_KAK_SHOOTING_GALLERY_KEY] = {
        // TODO_TRANSLATE
        Text{ "Firing Range Key", "Firing Range Key", "Firing Range Key" },
        Text{ "Crossbow Training Key", "Crossbow Training Key", "Crossbow Training Key" },
        Text{ "Goron Target Range Key", "Goron Target Range Key", "Goron Target Range Key" },
    };
    trickNameTable[RG_DAMPES_HUT_KEY] = {
        // TODO_TRANSLATE
        Text{ "Dampe's Grave Key", "Dampe's Grave Key", "Dampe's Grave Key" },
        Text{ "Dampe Studio Key", "Dampe Studio Key", "Dampe Studio Key" },
        Text{ "Old Man's Cabin Key", "Old Man's Cabin Key", "Old Man's Cabin Key" },
    };
    trickNameTable[RG_TALONS_HOUSE_KEY] = {
        // TODO_TRANSLATE
        Text{ "Malon's House Koi", "Malon's House Koi", "Malon's House Koi" },
        Text{ "Ingo's House Keese", "Ingo's House Keese", "Ingo's House Keese" },
        Text{ "Mario's House Key", "Mario's House Key", "Mario's House Key" },
    };
    trickNameTable[RG_STABLES_KEY] = {
        // TODO_TRANSLATE
        Text{ "Corral Key", "Corral Key", "Corral Key" },
        Text{ "Foothill Stable Key", "Foothill Stable Key", "Foothill Stable Key" },
        Text{ "Goat Barn Key", "Goat Barn Key", "Goat Barn Key" },
    };
    trickNameTable[RG_BACK_TOWER_KEY] = {
        // TODO_TRANSLATE
        Text{ "Tower of Hera Key", "Tower of Hera Key", "Tower of Hera Key" },
        Text{ "Clock Tower Key", "Clock Tower Key", "Clock Tower Key" },
        Text{ "Tingle Tower Key", "Tingle Tower Key", "Tingle Tower Key" },
        Text{ "Skyview Tower Key", "Skyview Tower Key", "Skyview Tower Key" },
        Text{ "Sheikah Tower Key", "Sheikah Tower Key", "Sheikah Tower Key" },
    };
    trickNameTable[RG_HYLIA_LAB_KEY] = {
        // TODO_TRANSLATE
        Text{ "Marine Research Lab Key", "Marine Research Lab Key", "Marine Research Lab Key" },
        Text{ "Hateno Tech Lab Key", "Hateno Tech Lab Key", "Hateno Tech Lab Key" },
        Text{ "Tough Mango Lab Key", "Tough Mango Lab Key", "Tough Mango Lab Key" },
    };
    trickNameTable[RG_FISHING_HOLE_KEY] = {
        // TODO_TRANSLATE
        Text{ "Swamp Fishing Hole Key", "Swamp Fishing Hole Key", "Swamp Fishing Hole Key" },
        Text{ "Beaver Race Key", "Beaver Race Key", "Beaver Race Key" },
        Text{ "Squid-Hunt Key", "Squid-Hunt Key", "Squid-Hunt Key" },
    };

    /*
    //Names for individual upgrades, in case progressive names are replaced
    trickNameTable[GI_HOOKSHOT] = {
        Text{"Grappling Hook", "Grappin-griffe", "Enterhaken" }, // "Gancho lanzable"
        Text{"Clawshot", "Lance-chaîne", "Greifhaken" }, // "Zarpa"
        Text{"Gripshot", "Grappince", "Tauschhaken" }, // "Enganchador"
    };
    trickNameTable[GI_LONGSHOT] = {
        Text{"Longshot, no strings attached", "Grappin sans attrape", "Enterhaken, Umtausch ausgeschloßen"}, // "Gancho
    lanzable más largo" Text{"Double Clawshot", "Double-grappin", "Doppelhaken" }, // "Superzarpa" Text{"Switch Hook",
    "Great grappin", "Tauschhaken" }, // "Gancho chulo"
    };
    trickNameTable[GI_BOMB_BAG_1] = {
        Text{"Bomb Capacity (20)", "Capacité de bombes (20)", "Bomben-Kapazität (20)" }, // "Bolsa de bombas (contiene
    20)" Text{"Bronze Bomb Bag", "Sac de Bombes de bronze", "Bronzene Bombentasche" }, // "Saco de bronce de bombas"
        Text{"Small Bomb Bag", "Petit Sac de Bombes", "Kleine Bombentasche" }, // "Zurrón de bombas pequeño"
    };
    trickNameTable[GI_BOMB_BAG_2] = {
        Text{"Bomb Capacity (30)", "Capacité de bombes (30)", "Bomben-Kapazität (30)" }, // "Bolsa de bombas (contiene
    30)" Text{"Silver Bomb Bag", "Sac de Bombes d'argent", "Silberne Bombentasche" }, // "Saco plateado de bombas"
        Text{"Medium Bomb Bag", "Sac de Bombes moyen", "Mittlere Bombentasche" }, // "Zurrón de bombas mediano"
    };
    trickNameTable[GI_BOMB_BAG_3] = {
        Text{"Bomb Capacity (40)", "Capacité de bombes (40)", "Bomben-Kapazität (40)" }, // "Bolsa de bombas (contiene
    40)" Text{"Golden Bomb Bag", "Sac de Bombes d'or", "Goldene Bombentasche" }, // "Saco dorado de bombas" Text{"Large
    Bomb Bag", "Gros Sac de Bombes", "Gigantische Bombentasche" }, // "Zurrón de bombas grande"
    };
    trickNameTable[GI_BOW_1] = {
        Text{"Bow", "Arc", "Bogen" }, // "Arco del Hada"
        Text{"Hero's Bow", "Arc du héros", "Heroenbogen" }, // "Arco del héroe"
        Text{"Small Quiver", "Petit carquois", "Kleiner Köcher" }, // "Saco de flechas pequeño"
    };
    trickNameTable[GI_BOW_2] = {
        Text{"Arrow Capacity (40)", "Capacité de flèches (40)", "Pfeil-Kapazität (40)" }, // "Capacidad de flechas (40)"
        Text{"Silver Quiver", "Carquois d'argent", "Silberner Köcher" }, // "Carcaj plateado"
        Text{"Medium Quiver", "Carquois moyen", "Mittlerer Köcher" }, // "Saco de flechas mediano"
    };
    trickNameTable[GI_BOW_3] = {
        Text{"Arrow Capacity (50)", "Capacité de flèches (50)", "Pfeil-Kapazität (50)" }, // "Capacidad de flechas (50)"
        Text{"Golden Quiver", "Carquois d'or", "Goldener Köcher" }, // "Carcaj dorado"
        Text{"Large Quiver", "Gros carquois", "Gigantischer Köcher" }, // "Saco de flechas grande"
    };
    trickNameTable[GI_SLINGSHOT_1] = {
        Text{"Slingshot", "Lance-Pierre", "Schleuder" }, // "Tirachinas del Hada"
        Text{"Scattershot", "Lance-Pierre rafale", "Streuschleuder" }, // "Tirachinas múltiple"
        Text{"Small Seed Satchel", "Petit sac de graines", "Kleine Munitionstasche" }, // "Bolsa de semillas pequeña"
    };
    trickNameTable[GI_SLINGSHOT_2] = {
        Text{"Deku Seed Capacity (40)", "Capacité de graines (40)", "Deku-Kern-Kapazität (40)" }, // "Capacidad de
    semillas (40)" Text{"Silver Deku Seed Bullet Bag", "Sac de graines d'argent", "Silberne Munitionstasche" }, //
    "Bolsa de balas (contiene 40)" Text{"Medium Seed Satchel", "Sac de graines moyen", "Mittlere Munitionstasche" }, //
    "Bolsa de semillas mediana"
    };
    trickNameTable[GI_SLINGSHOT_3] = {
        Text{"Deku Seed Capacity (50)", "Capacité de graines (50)", "Deku-Kern-Kapazität (50)" }, // "Capacidad de
    semillas (50)" Text{"Golden Deku Seed Bullet Bag", "Sac de graines d'or", "Goldene Munitionstasche" }, // "Bolsa de
    balas (contiene 50)" Text{"Large Seed Satchel", "Gros sac de graines", "Gigantische Munitionstasche" }, // "Bolsa de
    semillas grande"
    };
    trickNameTable[GI_STRENGTH_1] = {
        Text{"Goron's Gauntlet", "Gantelet Goron", "Goronen-Handschuhe€" }, // "Brazalete amarillo"
        Text{"Power Bracelet", "Bracelet de force", "Kraftarmband" }, // "Brazalete de fuerza"
        Text{"Magic Bracelet", "Bracelet de Lavio", "Magiearmband" }, // "Brazalete de Ravio"
    };
    trickNameTable[GI_STRENGTH_2] = {
        Text{"Silver Bracelets", "Bracelets d'argent", "Silberarmband" }, // "Guantes Moguma"
        Text{"Power Gloves", "Gant de puissance", "Silberhandschuhe€" }, // "Guante del Poder"
        Text{"Magic Gauntlets", "Gantelet magique", "Magiehandschuhe€" }, // "Guante mágico"
    };
    trickNameTable[GI_STRENGTH_3] = {
        Text{"Golden Bracelets", "Bracelets d'or", "Goldarmband" }, // "Guantelete de Thanos"
        Text{"Titan's Mitts", "Moufle de titan", "Goldhandschuhe€" }, // "Guantes de Titán"
        Text{"Magnetic Gloves", "Magnéto-gants", "Magnethandschuhe€" }, // "Guantes de fuego"
    };
    trickNameTable[GI_SCALE_1] = {
        Text{"Silver Pearl", "Perle d'argent", "Silberne Perle" }, // "Perla de Plata progresiva"
        Text{"Adult Scale", "Écaille d'adulte", "Große Schuppe" }, // "Bola de bolos zora"
        Text{"Zora Scale", "Écaille Zora", "Zora-Schuppe" }, // "Escama de Zora"
    };
    trickNameTable[GI_SCALE_2] = {
        Text{"Golden Pearl", "Perle d'or", "Goldene Perle" }, // "Perla de Oro progresiva"
        Text{"Giant Scale", "Écaille de géant", "Riesenschuppe" }, // "Escama de Faren"
        Text{"Water Dragon Scale", "Écaille du dragon de l'eau", "Wasserdrachenschuppe" }, // "Escama de dragón
    acuático"
    };
    trickNameTable[GI_WALLET_1] = {
        Text{"Rupee Capacity (200)", "Capacité de rubis (200)", "Rubinkapazität (200)" }, // "Capacidad de rupias (200)"
        Text{"Silver Wallet", "Bourse d'argent", "Silberne Geldbörse" }, // "Cartera de rupias de adulto"
        Text{"Medium Wallet", "Bourse moyenne", "Mittlere Geldbörse" }, // "Zurrón de rupias mediano"
    };
    trickNameTable[GI_WALLET_2] = {
        Text{"Rupee Capacity (500)", "Capacité de rubis (500)", "Rubin-Kapazität (500)" }, // "Capacidad de rupias
    (500)" Text{"Golden Wallet", "Bourse d'or", "Riesenportemonnaie" }, // "Cartera de rupias gigante" Text{"Large
    Wallet", "Grosse Bourse", "Gigantische Geldbörse" }, // "Zurrón de rupias grande"
    };
    trickNameTable[GI_WALLET_3] = {
        Text{"Rupee Capacity (999)", "Capacité de rubis (999)", "Rubin-Kapazität (999) }, // "Capacidad de rupias (999)"
        Text{"Golden Wallet", "Bourse d'or", "Goldenes Portemonnaie" }, // "Cartera de ricachón"
        Text{"Large Wallet", "Grosse Bourse", "Gigantische Geldbörse" }, // "Zurrón de rupias gigante"
    };
    trickNameTable[GI_DEKU_NUT_UPGRADE_1] = {
        Text{"Deku Bomb Capacity (30)", "Capacité de bombes Mojo (30)", "Deku-Bomben-Kapazität (30)" }, // "Capacidad de
    semillas deku (40)" Text{"Baba Nut Capacity (30)", "Capacité de noix Baba (30)", "Ranha-Nuß-Kapazität (30)" }, //
    "Capacidad de nueces baba (40)" Text{"Deku Nut Pack (30)", "Paquet de noix Mojo (30)", "Deku-Nußrucksack (30)" }, //
    "Capacidad de nueces mojo (40)"
    };
    trickNameTable[GI_DEKU_NUT_UPGRADE_2] = {
        Text{"Deku Bomb Capacity (40)", "Capacité de bombes Mojo (40)", "Deku-Bomben-Kapazität (40)" }, // "Capacidad de
    semillas deku (50)" Text{"Baba Nut Capacity (40)", "Capacité de noix Baba (40)", "Ranha-Nuß-Kapazität (40)" }, //
    "Capacidad de nueces baba (50)" Text{"Deku Nut Pack (40)", "Paquet de noix Mojo (40)", "Deku-Nußrucksack (40)" }, //
    "Capacidad de nueces mojo (50)"
    };
    trickNameTable[GI_DEKU_STICK_UPGRADE_1] = {
        Text{"Deku Rod Capacity (20)", "Capacité de tiges Mojo (20)", "Deku-Stock-Kapazität (20)" }, // "Capacidad de
    palos mojo (20)" Text{"Boko Stick Capacity (20)", "Capacité de Bâtons Boko (20)", "Bokstock-Kapazität (20)" }, //
    "Capacidad de palos boko (20)" Text{"Deku Stick Pack (20)", "Paquet de bâtons Mojo (20)", "Deku-Stabrucksack (20)"
    }, // "Capacidad de bastones deku (20)"
    };
    trickNameTable[GI_DEKU_STICK_UPGRADE_2] = {
        Text{"Deku Rod Capacity (30)", "Capacité de tiges Mojo (30)", "Deku-Stock-Kapazität (30)" }, // "Capacidad de
    palos mojo (30)" Text{"Boko Stick Capacity (30)", "Capacité de Bâtons Boko (30)", "Bokstock-Kapazität (30)" }, //
    "Capacidad de palos boko (30)" Text{"Deku Stick Pack (30)", "Paquet de bâtons Mojo (30)", "Deku-Stabrucksack (30)"
    }, // "Capacidad de bastones deku (30)"
    };
    trickNameTable[GI_MAGIC_1] = {
        Text{"Stamina Meter", "Jauge d'endurance", "Ausdauerleiste" }, // "Medidor de vigor"
        Text{"Energy Meter", "Jauge d'énergie", "Energieleiste" }, // "Medidor de energía"
        Text{"Magic Powder", "Poudre magique", "Zauberpulver" }, // "Medidor de carga"
    };
    trickNameTable[GI_MAGIC_2] = {
        Text{"Enhanced Stamina Meter", "Jauge d'endurance améliorée", "Verb. Ausdauerleiste" }, // "Medidor de vigor
    mejorado" Text{"Enhanced Energy Meter", "Jauge d'énergie améliorée", "Verb. Energieleiste" }, // "Medidor de energía
    mejorado" Text{"Enhanced Magic Powder", "Poudre magique améliorée", "Verb. Zauberpulver" }, // "Medidor de carga
    mejorado"
    };
    trickNameTable[GI_OCARINA_1] = {
        Text{"Ocarina", "Ocarina", "Okarina" }, // "Ocarina"
        Text{"Saria's Ocarina", "Ocarina de Saria", "Salias Okarina" }, // "Ocarina de Saria"
        Text{"Wood Ocarina", "Ocarina de bois", "Holzokarina" }, // "Ocarina del Hada"
    };
    trickNameTable[GI_OCARINA_2] = {
        Text{"Flute", "Flûte", "Flöte" }, // "Flauta"
        Text{"Zelda's Ocarina", "Ocarina de Zelda", "Zeldas Okarina" }, // "Ocarina de Zelda"
        Text{"Ocarina of Winds", "Ocarina des vents", "Okarina des Windes" }, // "Ocarina del Viento"
    };
    trickNameTable[GI_CUCCO] = {
        Text{"D.I.Y. Alarm Clock", "Réveille-matin improvisé", "Improvisierter Wecker"}, // "Alarma emplumada"
        Text{"Kakariko Cucco", "Cocotte Cocorico", "Kakariko-Huhn" }, // "Cuco de Kakariko"
        Text{"Hatched Cucco", "Cocotte éclose", "Geschlüpftes Küken" }, // "Pollo"
    };
    trickNameTable[GI_MASK_KEATON] = {
        Text{"Kee... Something Mask", "Masque de Quiche", "Keaton-Maske" }, // "Máscara Kealgo"
        Text{"Kitsune Mask", "Masque de Kitsune", "Kitsune-Maske" }, // "Máscara Kitsune"
        Text{"Kafei's Mask", "Masque de Kafei", "Kafeis Maske" }, // "Máscara de Kafei"
    };
    trickNameTable[GI_MASK_SKULL] = {
        Text{"Skull Kid's Mask", "Masque de Skull Kid", "Horror-Kids-Maske" }, // "Máscara de Skull Kid"
        Text{"Stalfos Mask", "Masque de squelette", "Stalfos-Maske" }, // "Máscara de Stalfos"
        Text{"Captain's Hat", "Heaume du capitaine", "Helm des Hauptmanns" }, // "Casco del capitán"
    };
    trickNameTable[GI_MASK_SPOOKY] = {
        Text{"Skrik Mask", "Masque Skrik", "Grusel-Maske" }, // "Máscara Escalofriante"
        Text{"ReDead Mask", "Masque de Remort", "Remort-Maske" }, // "Máscara de ReDead"
        Text{"Gibdo Mask", "Masque de Gibdo", "Gibdo-Maske" }, // "Careta de Gibdo"
    };
    trickNameTable[GI_MASK_BUNNY] = {
        Text{"Peppy Mask", "Masque de Peppy", "Peppy-Maske" }, // "Capucha de Pascua"
        Text{"Bunny Ears", "Oreilles de lapin", "Ohrlöffel" }, // "Orejas de conejo"
        Text{"Postman's Hat", "Casquette du facteur", "Mütze des Postboten" }, // "Gorra de cartero"
    };
    trickNameTable[GI_MASK_GORON] = {
        Text{"Goro Mask", "Masque Goro", "Garos Maske" }, // "Máscara Goro"
        Text{"Mask of Goron", "Masque des Gorons", "Goronen-Haut" }, // "Máscara de los Goron"
        Text{"Darunia Mask", "Masque de Darunia", }, "Darunias Maske" // "Máscara de Darmani"
    };
    trickNameTable[GI_MASK_ZORA] = {
        Text{"Zola Mask", "Masque Zola", "Zola-Maske" }, // "Máscara Zola"
        Text{"Mask of Zora", "Masque des Zoras", "Zora-Schuppen€" }, // "Máscara de los Zora"
        Text{"Ruto Mask", "Masque de Ruto", "Rutos Maske" }, // "Máscara de Mikau"
    };
    trickNameTable[GI_MASK_GERUDO] = {
        Text{"Ganguro Mask", "Masque de Ganguro", "Ganguro-Maske" }, // "Máscara Canguro"
        Text{"Mask of Gerudo", "Masque des Gerudos", "Gerudo-Perücke" }, // "Máscara de las Gerudo"
        Text{"Nabooru Mask", "Masque de Nabooru", "Naborus Maske" }, // "Máscara de Nabooru"
    };
    trickNameTable[GI_MASK_TRUTH] = {
        Text{"Sheikah Mask", "Masque Sheikah", "Shiekah-Maske" }, // "Máscara Sheikah"
        Text{"Mask of Gossip", "Masque de potins", "Maske der Mythen" }, // "Máscara chismosa"
        Text{"Eye of Truth", "oeil de vérité", "Auge der Wahrheit" }, // "Ojo de la Verdad"
    };
    */
}

// Generate a fake name for the ice trap based on the item it's displayed as
Text Rando::Traps::GetTrapName(uint16_t id) {
    // If the trick names table has not been initialized, do so
    if (!initTrickNames) {
        InitTrickNames();
        initTrickNames = true;
    }

    if (trickNameTable[id].empty()) {
        assert(false);
        return Text{ "not an Ice Trap" };
    }

    // Randomly get the easy, medium, or hard name for the given item id
    return RandomElement(trickNameTable[id]);
}

RandomizerGet Rando::Traps::GetTrapTrickModel() {
    auto ctx = Rando::Context::GetInstance();
    RandomizerGet trickModel = RandomElementFromSet(ctx->possibleIceTrapModels);

    if (trickModel == RG_EMPTY_BOTTLE) {
        trickModel = RandomElement(Rando::StaticData::normalBottles);
    } else if (trickModel == RG_GUARD_HOUSE_KEY) {
        trickModel = RandomElement(Rando::StaticData::overworldKeys);
    } else if (trickModel == RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) {
        trickModel = RandomElement(Rando::StaticData::beanSouls);
    }

    return trickModel;
}

bool Rando::Traps::ShouldJunkItemBeTrap() {
    auto ctx = Rando::Context::GetInstance();

    if (ctx->GetOption(RSK_ICE_TRAP_PERCENT).Is(0)) {
        return false;
    }

    if (ctx->GetOption(RSK_ICE_TRAP_PERCENT).Is(100) || Random(0, 100) < ctx->GetOption(RSK_ICE_TRAP_PERCENT).Get()) {
        return true;
    }

    return false;
}

static const char* const englishIceTrapMessages[] = {
    "You are a #FOOL#!",
    "You are a #FOWL#!",
    "#FOOL#!",
    "You just got #PUNKED#!",
    "Stay #frosty#, @.",
    "Take a #chill pill#, @.",
    "#Winter# is coming.",
    "#ICE# to see you, @.",
    "Feeling a little %rhot%w under the collar? #Let's fix that#.",
    "It's a #cold day# in the Evil Realm.",
    "Getting #cold feet#?",
    "Say hello to the #Zoras# for me!",
    "Can you keep a #cool head#?",
    "Ganondorf used #Ice Trap#!&It's super effective!",
    "Allow me to break the #ice#!",
    "#Cold pun#.",
    "The #Titanic# would be scared of you, @.",
    "Oh no!",
    "Uh oh!",
    "What killed the dinosaurs?&The #ICE# age!",
    "Knock knock. Who's there? Ice. Ice who? Ice see that you're a #FOOL#.",
    "Never gonna #give you up#. Never gonna #let you down#. Never gonna run around and #desert you#.",
    "Thank you #@#! But your item is in another castle!",
    "#FREEZE#! Don't move!",
    "Wouldn't it be #ice# if we were colder?",
    "Greetings from #Snowhead#! Wish you were here.",
    "Too #cool# for you?",
    "#Ice#, #ice#, baby...",
    "Time to break the #ice#.",
    "We wish that you would read this... We wish that you would read this... But we set our bar low.",
    "#Freeze# and put your hands in the air!",
    "#Ice# to meet you!",
    "Do you want to #freeze# a snowman?",
    "Isn't there a #mansion# around here?",
    "Now you know how #King Zora# feels.",
    "May the #Frost# be with you.",
    "Carpe diem. #Freeze# the day.",
    "There #snow# place like home.",
    "That'll do, #ice#. That'll do.",
    "All that is #cold# does not glitter. Not all those who wander are #frost#.",
    "I Used To Be An Adventurer Like You. Then I Took An #Icetrap# To The Knee.",
    "Would you like #ice# with that?",
    "You have obtained the #Ice# Medallion!",
    "Quick, do a #Zora# impression!",
    "One %r${itemName}%w #on the rocks#!",
    "How much does a polar bear weigh?&Enough to break the #ice#.",
    "You got Din's #Ice#!",
    "You got Nayru's #Cold#!",
    "You got Farore's #Freeze#!",
    "KEKW",
    "You just got #ICE TRAPPED#! Tag your friends to totally #ICE TRAP# them!",
    "Are you okay, @? You're being #cold# today.",
    "In a moment, your game might experience some #freezing#.",
    "Breeze? Trees? Squeeze? No, it's a #freeze#!",
    "After collecting this item, @ was assaulted in #cold# blood.",
    "Only #chill# vibes around here!",
    "Here's a #cool# gift for you!",
    "Aha! You THOUGHT.",
    "Stay hydrated and brush your teeth!",
    "Isn't it too hot here? Let's turn the #AC# on.",
    "One serving of #cold# @, coming right up!",
    "Is it #cold# in here or is that just me?",
    "Yahaha! You found me!",
    "You'd make a great #ice#-tronaut!",
    "That's just the tip of the #iceberg#!",
    "It's the triforce!&No, just kidding, it's an #ice trap#.",
    "WINNER!",
    "LOSER!",
    "Greetings from #Cold Miser#!",
    "Pardon me while I turn up the #AC#.",
    "If you can't stand the #cold#, get out of the #freezer#.",
    "Oh, goodie! #Frozen @# for the main course!",
    "You have #freeze# power!",
    "You obtained the #Ice Beam#! No wait, wrong game.",
    "Here's to another lousy millenium!",
    "You've activated my #trap card#!",
    "I love #refrigerators#!",
    "You expected an item,&BUT IT WAS I, AN #ICE TRAP#!",
    "It's dangerous to go alone! Take #this#!",
    "soh.exe has #stopped responding#.",
    "Enough! My #Ice Trap# thaws in the morning!",
    "Nobody expects the span-#ice# inquisition!",
    "This is one #cool# item!",
    "Say hello to my #little friend#!",
    "We made you an offer you #can't refuse#.",
    "Hyrule? More like #Hycool#!",
    "Ice puns are #snow# problem!",
    "This #ice# is #snow# joke!",
    "There's no business like #snow# business!",
    "No, dude.",
    "N#ice# trap ya got here!",
    "Quick do your best impression of #Zoras Domain#!",
    "Ganon used #ice beam#, it's super effective!",
    "I was #frozen# today.",
    "You're not in a #hurry#, right?",
    "It's a #trap#!",
    "At least it's not a VC crash and only Link is #frozen#!",
    "Oh no! #BRAIN FREEZE#!",
    "Looks like your game #froze#! Nope just you!",
    "PK #FREEZE#!",
    "May I interest you in some #iced# Tea?",
    "Time for some Netflix and #chill#.",
    "I know, I know... #FREEZE#!",
    "#Ice# of you to drop by!",
    "STOP!&You violated the #Thaw#!",
    "I wanted to give you a treasure, but it looks like you got #cold feet#.",
    "You told me you wanted to deliver #just ice# to Ganondorf!",
    "You got the triforce!&This ancient artifact of divine power can grant any- wait, no, sorry, it's just an ice "
    "trap. My bad.",
    "Time to #cool off#!",
    "The #Ice Cavern# sends its regards.",
    "Loading item, please #wait#...",
    "Mash A+B to not #die#.",
    "Sorry, your %r${itemName}%w is in another location.",
    "You only wish this was %gGreg%w.",
    "Do you want to drink a hot chocolate?",
    "The #cold# never bothered me anyway.",
    "Hope you're too school for #cool#!",
    "Be thankful this isn't #absolute zero#.",
    "Did you know the F in ZFG stands for #Freeze#?",
    "You got #Ice Age (2002)#!",
    "Now you can cast a #spell# you don't know.",
    "How's about a hero #on the rocks#?",
    "Ain't no tunic for #this#!",
    "I knew you were #part metroid#!",
    "That's just the #icing on the cake#!",
    "You're so #cool#, @!",
    "You found #disappointment#!",
    "You got #FOOLED#!",
    "Start Mashing.",
    "This item will #self-destruct# in 5 seconds...",
    "Remember, there may be some momentary #discomfort#.",
    "In a perfect world #ice traps# like me would not exist, but this is not a perfect world.",
    "Gee, it sure is #cold# around here.",
    "You tested the %r${itemName}%w with your #ice detector#, it beeped.",
    "You have found the way of the zero. The #sub-zero#.",
    "Mweep... mweep... mweep...",
    "Scum, #freezebag#! I mean #freeze#, scumbag!",
    "Is it #chilly# in here or is it just #you#?",
    "#Proceed#.",
    "WHAT'S SHE GONNA DO, MAKE ME AN #[Ice Cream]#!?",
    "You've met with a #terrible fate#, haven't you?",
    "So I heard you like the Shining, here's how it #ends#.",
    "Minor routing mistake. #I win#.",
    "Hold this #L#, @.",
    "#SKILL ISSUE#.",
    "All your heat are belong to us.",
    "Wait a second, don't you already have #this item#?",
    "#Freeze#! We have you surrounded!",
    "Error 404 - Item not #found#.",
    "Hydration break! Hey, who #froze# my water?",
    "Oops, wrong #item model#.",
    "Whoops! You have to put the item #in your inventory#.",
    "You dropped the %r${itemName}%w, shattering it into #shards of ice#!",
    "Is this... golden age Simpsons?&BECAUSE I'M ABOUT TO #CHOKE A CHILD#.",
    "You are the weakest @, #goodbye#!",
    "Ugh... Why did we even randomize #this item#?",
    "The #Frost Moon# is rising...",
    "According to all known laws of physics and biology, there is no way that @ should be able to survive #getting "
    "fully encased in ice#. The cells in @'s body would all die by the time they #unthaw#. Of course, this is a video "
    "game, so @ survives anyway... #Probably#.",
    "Okay, so stop me if you've heard this one - a gamer and a bottle of #liquid nitrogen# walk into a milk bar...",
    "Lástima, es una #trampa de hielo#...&&Nobody expects the Spanish #ice trap#!",
    "Gee, it sure is #BURR#ing around here.",
    "Navi? Oh! I thought she was called #Névé#!",
    "It's fine, @ knew this was a #trap#, they're just using it to take damage intentionally to manipulate RNG.",
    "Unfortunately, the %r${itemName}%w has #stopped#.",
    "This %r${itemName}%w is #not available# in your country.",
    "#Ice# try. #;)#",
    "D'oh, I #missed#!",
    "Where is my #super suit#?",
    "#Titanic's revenge#.",
};

static const char* const germanIceTrapMessages[] = {
    "Du bist ein #DUMMKOPF#!",
    "Du bist eine #Frostbeule#!",
    "#DUMMKOPF#!",
    "Du wurdest #eiskalt# erwischt!",
    "Es läuft Dir #eiskalt# den Rücken hinunter, @.",
    "Bleib #cool#, @.",
    "Der #Winter# naht.",
    "Alles #cool# im Pool?",
    "#Abkühlung gefällig#?",
    "Es ist ein %kalter%w Tag im Herzen von Hyrule.",
    "Bekommst Du etwa #kalte# Füße?",
    "Sag den #Zoras# viele Grüße von mir!",
    "Bewahre einen #kühlen#! Kopf.",
    "Ganondorf setzt #Eisstrahl# ein. Das ist sehr effektiv!",
    "Ein Lächeln ist der beste Weg, um das #Eis# zu brechen!",
    "#Eiskalt# lässt du meine Seele erfrier'n.",
    "Die #Titanic# hätte Angst vor Dir, @.",
    "Oh nein!",
    "Was die Dinosaurier getötet hat?&Die #Eiszeit#!",
    "Nachts ist es #kälter# als draußen.",
    "Never gonna #give you up#. Never gonna #let you down#. Never gonna run around and #desert you#.",
    "Danke #@#! Aber der Gegenstand ist in einem anderem Schloß!",
    "Kalt. Kalt. Kälter. #EISKALT#!",
};

static const char* const frenchIceTrapMessages[] = {
    "#Pauvre fou#...",
    "Tu es un #glaçon#, Harry!",
    "#Sot# que tu es.",
    "Ça me #glace# le sang!",
    "#Reste au frais#, @.",
    "Et c'est la douche #froide#!",
    "L'#hiver# vient.",
    "#Glacier#!",
    "Ça en jette un #froid#.",
    "Est-ce que tu as déjà eu des sueurs #froides#?",
    "La vengeance est un plat qui se mange #froid#!",
    "Dit bonjour aux #Zoras# pour moi!",
    "Il faut parfois savoir garder la tête #froide#!",
    "Ganondorf utilise #Piège de Glace#! C'est super efficace!",
    "Laisse moi briser la #glace#!",
    "Balance man..., Cadence man..., Trace la #glace#..., c'est le Cooooolllll Rasta!",
    "Le #Titanic# aurait peur de toi, @.",
    "Oh non!",
    "Qu'est-ce qui a tué les dinosaures?&L'ère #glacière#!",
    "L'imbécile réfléchit uniquement quand il s'observe dans la #glace#.",
    "Never gonna #give you up#. Never gonna #let you down#. Never gonna run around and #desert you#.",
    "Merci #@#! Mais ton objet est dans un autre château!",
    "J'espère que ça ne te fait ni chaud, ni #froid#.",
    "Je voulais t'offrir un trésor, mais il semble que tu aies eu #froid aux pieds#",
    "Tu m'as dit que tu voulais livrer #de la glace# à Ganondorf!",
    "Tu as obtenu la Triforce!&Cet ancien artefact divin peut exaucer n'importe quel... ah non, désolé, c'est juste un "
    "piège de glace.",
    "Il est temps de #te rafraîchir#!",
    "La #Caverne Polaire# te passe le bonjour.",
    "Chargement de l'objet, veuillez #patienter#...",
    "Martèle A+B pour ne pas #mourir#.",
    "Désolé, ton objet est à un autre endroit.", // would be better if it could have the name of the item
    "Tu espérais que ce soit %gGreg%w.",
    "Tu veux boire un chocolat chaud?",
    "Le #froid# ne m'a jamais dérangé, de toute façon.",
    "J'espère que tu es trop cool pour être #cool#!",
    "Sois reconnaissant que ce n'est pas le #zéro absolu#.",
    "Tu savais que le G de ZFG signifie #Glace#?",
    "Tu as obtenu #L'Âge de Glace (2002)#!",
    "Maintenant, tu peux lancer un #sort# que tu ne connais pas.",
    "Que dirais-tu d'un héros #sur glace# ?",
    "Pas de tunique pour #ça#!",
    "Je savais que tu étais #partiellement Metroid#!",
    "Voilà juste la #cerise sur le gâteau#!",
    "Tu es tellement #cool#, @!",
    "Tu as trouvé de la #déception#!",
    "Tu t'es fait #BERNER#!",
    "Commence à marteler.",
    "Cet objet va #s'autodétruire# dans 5 secondes...",
    "Souviens-toi, il pourrait y avoir un léger #inconfort#.",
    "Dans un monde parfait, les #pièges de glace# comme moi n'existeraient pas, mais ce n'est pas un monde parfait.",
    "Mon dieu qu'il fait #froid# ici.",
    "Tu as testé l'objet avec ton #détecteur de glace#, il a bipé.", // would be better if it could have the name of the
                                                                     // item
    "Tu as découvert le chemin du zéro. Le #sub-zéro#.",
    "Mweep... mweep... mweep...",
    "Gelé, #sac à glace#! Je veux dire #gèle-toi#, racaille!",
    "Est-ce qu'il fait #frais# ici ou est-ce juste #toi#?",
    "#Continue#",
    "QU'EST-CE QU'ELLE VA FAIRE, ME FAIRE UNE #[Glace]#!?",
    "Tu as rencontré un #terrible destin#, n'est-ce pas?",
    "Alors comme ça, tu aimes Shining ? Voici comment ça #finit#.",
    "Petite erreur de trajectoire. #Je gagne#.",
    "Prends ce #L#, @.",
    "#Problème de compétence#",
    "Tout ton chauffage nous appartient.",
    "Attends une seconde, tu as déjà #cet objet#, non?",
    "#Gèle#! Tu es entouré !",
    "Erreur 404 - Objet non #trouvé#.",
    "Pause hydratation ! Hé, qui a #gelé# mon eau?",
    "Oups, mauvais #modèle d'objet#.",
    "Oups! Tu dois mettre l'objet #dans ton inventaire#.",
    "Tu as fait tomber l'objet, le brisant en #éclats de glace#!", // would be better if it could have the name of the
                                                                   // item
    "Tu es le maillon faible @, #au revoir#!",
    "Ugh... Pourquoi avons-nous même randomisé #cet objet#?",
    "La #Lune de Givre# se lève...",
    "Selon toutes les lois connues de la physique et de la biologie, @ ne devrait pas survivre à #être complètement "
    "enfermé dans la glace#. Les cellules de @ mourraient avant qu'elles ne #dégèlent#. Mais c'est un jeu vidéo, alors "
    "@ survit... #Probablement#.",
    "OK, arrête-moi si tu l'as déjà entendue - un joueur et une bouteille de #nitrogène liquide# entrent dans un bar à "
    "lait...",
    "Lástima, c'est un #piège de glace#...&&Personne ne s'attend à un #piège de glace espagnol#!",
    "Mon dieu qu'il fait #GLAGLA# ici.",
    "C'est bon, @ savait que c'était un #piège#, il l'utilise juste pour prendre des dégâts intentionnellement et "
    "manipuler la RNG.",
    "Cet objet n'est #pas disponible# dans votre pays.", // would be better if it could have the name of the item
    "#Bonne# tentative. #;)#",
    "Où est mon #Super Costume#?",
    "#La revanche du Titanic#.",
};

extern "C" SaveContext gSaveContext;

static std::string ReplaceItemName(const char* c_str, GetItemEntry getItemEntry) {
    const static std::string placeholder = "${itemName}";

    std::string str = std::string(c_str);

    std::string name = "item";

    if (getItemEntry.drawModIndex == MOD_NONE) {
        name = SohUtils::GetItemName(getItemEntry.drawItemId);
    } else if (getItemEntry.drawModIndex == MOD_RANDOMIZER) {
        name = Rando::StaticData::RetrieveItem((RandomizerGet)getItemEntry.drawItemId)
                   .GetName()
                   .GetForLanguage(gSaveContext.language);
    } else {
        assert(false);
    }

    size_t index = 0;
    while (true) {
        index = str.find(placeholder, index);
        if (index == std::string::npos) {
            break;
        }

        str.replace(index, placeholder.length(), name);

        index += name.length();
    }

    return str;
}

void Rando::Traps::BuildIceTrapMessage(CustomMessage& msg, GetItemEntry getItemEntry) {
    if (CVarGetInteger(CVAR_GENERAL("LetItSnow"), 0)) {
        msg = CustomMessage(
            /*english*/ "This year for Christmas, all you get is #COAL#!",
            /*german*/ TODO_TRANSLATE,
            /*french*/ "Pour Noël, cette année, tu n'auras que du #CHARBON#! %rJoyeux Noël%w!", { QM_BLUE });
    } else {
        msg = CustomMessage(ReplaceItemName(ShipUtils::RandomElement(englishIceTrapMessages), getItemEntry),
                            ReplaceItemName(ShipUtils::RandomElement(germanIceTrapMessages), getItemEntry),
                            ReplaceItemName(ShipUtils::RandomElement(frenchIceTrapMessages), getItemEntry),
                            { QM_BLUE, QM_BLUE, QM_BLUE });
    }

    msg.AutoFormat();
}