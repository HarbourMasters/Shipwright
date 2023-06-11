#include "hint_list.hpp"
#include "custom_messages.hpp"

#include <array>

using namespace CustomMessages;

// Big thanks to Lioncache, Gabyelnuevo, Danius88, and Charade for their translations!

// Text is {english, french, spanish}

// there are special characters that are read for certain in game commands:
// ^ is a box break
// & is a new line
// @ will print the player name
// surrounding text with '#' will make it a different color
// - OoT Randomizer

// '%d' indicates a number will be placed there.

std::array<HintText, KEY_ENUM_MAX> hintTable;

void HintTable_Init() {
    /*--------------------------
    |       GENERAL TEXT       |
    ---------------------------*/
    hintTable[NONE] = HintText::Exclude({ Text{ "No Hint", "Pas d'Indice", "" } });
    hintTable[PREFIX] =
        HintText::Exclude({ Text{ "They say that ", /*french*/ "Selon moi, ", /*spanish*/ "Según dicen, " } });
    hintTable[WAY_OF_THE_HERO] =
        HintText::Exclude({ Text{ " is on the way of the hero.", /*french*/ " est sur la voie du héros.",
                                  /*spanish*/ " conduce a la senda del héroe." } });
    hintTable[PLUNDERING] =
        HintText::Exclude({ Text{ "plundering ", /*french*/ "explorer ", /*spanish*/ "inspeccionar " } });
    hintTable[FOOLISH] = HintText::Exclude(
        { Text{ " is a foolish choice.", /*french*/ " est futile.", /*spanish*/ " no es una sabia decisión." } });
    hintTable[CAN_BE_FOUND_AT] =
        HintText::Exclude({ Text{ "can be found at", /*french*/ "se trouve dans", /*spanish*/ "aguarda en" } });
    hintTable[HOARDS] = HintText::Exclude({ Text{ "hoards", /*french*/ "recèle", /*spanish*/ "acapara" } });

    HintTable_Init_Item();
    HintTable_Init_Exclude_Overworld();
    HintTable_Init_Exclude_Dungeon();

    /*--------------------------
    |     ALWAYS HINT TEXT     |
    ---------------------------*/

    hintTable[ZR_FROGS_OCARINA_GAME] = HintText::Always(
        {
            // obscure text
            Text{ "an #amphibian feast# yields", /*french*/ "un #festin d'amphibiens# donne",
                  /*spanish*/ "una #fiesta anfibia# brinda" },
            Text{ "the #croaking choir's magnum opus# awards", /*french*/ "la #chorale coassante# donne",
                  /*spanish*/ "un #coro maestro de ancas# premia" },
            Text{ "the #froggy finale# yields", /*french*/ "la #finale amphibienne# donne",
                  /*spanish*/ "el #gran final batracio# brinda" },
        },
        {},
        // clear text
        Text{ "the final reward from the #Frogs of Zora's River# is",
              /*french*/ "la dernière récompense des #grenouilles de la Rivière Zora# est",
              /*spanish*/ "la recompensa final de las #ranas del Río Zora# premia" });

    hintTable[KF_LINKS_HOUSE_COW] = HintText::Always(
        {
            // obscure text
            Text{ "the #bovine bounty of a horseback hustle# gifts",
                  /*french*/ "le cadeau #qui découle d'une réussite équestre# est",
                  /*spanish*/ "la #recompensa bovina de un paseo a caballo# brinda" },
        },
        {},
        // clear text
        Text{ "#Malon's obstacle course# leads to", /*french*/ "la #course à obstacle de Malon# amène à",
              /*spanish*/ "la #carrera de obstáculos de Malon# brinda" });

    hintTable[KAK_100_GOLD_SKULLTULA_REWARD] = HintText::Always(
        {
            // obscure text
            Text{ "#100 bug badges# rewards",
                  /*french*/ "#100 écussons# donnent",
                  /*spanish*/ "#100 medallas de insectos# otorgan" },
            Text{ "#100 spider souls# yields",
                  /*french*/ "#100 âmes d'arachnide# donnent",
                  /*spanish*/ "#100 almas de araña# otorgan" },
            Text{ "#100 auriferous arachnids# lead to",
                  /*french*/ "#100 arachnides aurifères# donnent",
                  /*spanish*/ "#100 arácnidos auríferos# otorgan" },
        },
        {},
        // clear text
        Text{ "slaying #100 Gold Skulltulas# reveals",
              /*french*/ "détruire #100 Skulltulas d'or# donne",
              /*spanish*/ "exterminar #100 skulltulas doradas# revela" });

    /*--------------------------
    |    SOMETIMES HINT TEXT   |
    ---------------------------*/

    hintTable[SONG_FROM_OCARINA_OF_TIME] = HintText::Sometimes({
        // obscure text
        Text{ "the #Ocarina of Time# teaches", /*french*/ "l'#Ocarina du Temps# est accompagné par",
              /*spanish*/ "la #Ocarina del Tiempo# enseña" },
    });

    hintTable[SONG_FROM_COMPOSERS_GRAVE] = HintText::Sometimes({
        // obscure text
        Text{ "#ReDead in the Composers' Grave# guard",
              /*french*/ "les #Éffrois du tombeau des compositeurs# protègent",
              /*spanish*/ "los #ReDeads del Panteón Real# guardan" },
        Text{ "the #Composer Brothers wrote#", /*french*/ "le #trésor des compositeurs# est",
              /*spanish*/ "los #hermanos compositores escribieron#" },
    });

    hintTable[SHEIK_IN_FOREST] = HintText::Sometimes({
        // obscure text
        Text{ "#in a meadow# Sheik teaches", /*french*/ "Sheik confiera, #dans un bosquet#,",
              /*spanish*/ "#en la pradera sagrada# Sheik enseña" },
    });

    hintTable[SHEIK_AT_TEMPLE] = HintText::Sometimes({
        // obscure text
        Text{ "Sheik waits at a #monument to time# to teach",
              /*french*/ "Sheik confiera, #au pied de l'épée légendaire#,",
              /*spanish*/ "Sheik espera en el #momumento del tiempo# para enseñar" },
    });

    hintTable[SHEIK_IN_CRATER] = HintText::Sometimes({
        // obscure text
        Text{ "the #crater's melody# is", /*french*/ "Sheik confiera, #entouré de lave#,",
              /*spanish*/ "la #melodía del cráter# otorga" },
    });

    hintTable[SHEIK_IN_ICE_CAVERN] = HintText::Sometimes({
        // obscure text
        Text{ "the #frozen cavern# echoes with", /*french*/ "Sheik confiera, #dans une caverne enneigée#,",
              /*spanish*/ "en la #caverna de hielo# retumban los ecos de" },
    });

    hintTable[SHEIK_IN_KAKARIKO] = HintText::Sometimes({
        // obscure text
        Text{ "a #ravaged village# mourns with", /*french*/ "Sheik confirera, #au coeur d'un village ravagé#,",
              /*spanish*/ "un #arrasado pueblo# llora" },
    });

    hintTable[SHEIK_AT_COLOSSUS] = HintText::Sometimes({
        // obscure text
        Text{ "a hero ventures #beyond the wasteland# to learn",
              /*french*/ "Sheik confiera, #au bout d'un chemin sableux#,",
              /*spanish*/ "el héroe que se adentre #más allá del desierto# aprenderá" },
    });

    hintTable[MARKET_10_BIG_POES] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#ghost hunters# will be rewarded with",
                  /*french*/ "#les chasseurs de fantômes# sont récompensés avec",
                  /*spanish*/ "los #cazafantasmas# son premiados con" },
        },
        {},
        // clear text
        Text{ "catching #Big Poes# leads to", /*french*/ "#d'attraper des Àmes# donne",
              /*spanish*/ "hacerte con #Grandes Poes# conduce a" });

    hintTable[DEKU_THEATER_SKULL_MASK] = HintText::Sometimes({
        // obscure text
        Text{ "the #Skull Mask# yields", /*french*/ "le #Masque de Mort# donne",
              /*spanish*/ "la #máscara de calavera# otorga" },
    });

    hintTable[DEKU_THEATER_MASK_OF_TRUTH] = HintText::Sometimes(
        {
            // obscure text
            Text{ "showing a #truthful eye to the crowd# rewards",
                  /*french*/ "montrer #l'oeil de vérité à la foule# donne",
                  /*spanish*/ "#mostrarle el ojo verdadero# a una multitud brinda" },
        },
        {},
        // clear text
        Text{ "the #Mask of Truth# yields", /*french*/ "le #Masque de Vérité# donne",
              /*spanish*/ "la #máscara de la verdad# premia" });

    hintTable[HF_OCARINA_OF_TIME_ITEM] = HintText::Sometimes({
        // obscure text
        Text{ "the #treasure thrown by Princess Zelda# is", /*french*/ "le trésor #laissé par la princesse# est",
              /*spanish*/ "el #tesoro arrojado por la Princesa Zelda# se trata de" },
    });

    hintTable[DMT_TRADE_BROKEN_SWORD] = HintText::Sometimes({
        // obscure text
        Text{ "a #blinded Biggoron# entrusts", /*french*/ "un #Grogoron aveuglé# confie",
              /*spanish*/ "un #miope Biggoron# otorga" },
    });

    hintTable[DMT_TRADE_EYEDROPS] = HintText::Sometimes({
        // obscure text
        Text{ "while you wait, #Biggoron# gives", /*french*/ "pendant que tu attends, #Biggoron# donne",
              /*spanish*/ "#Biggoron# está a la espera de otorgar" },
    });

    hintTable[DMT_TRADE_CLAIM_CHECK] = HintText::Sometimes({
        // obscure text
        Text{ "#Biggoron# crafts", /*french*/ "#Biggoron# fabrique", /*spanish*/ "#Biggoron# forja" },
    });

    hintTable[KAK_50_GOLD_SKULLTULA_REWARD] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#50 bug badges# rewards", /*french*/ "#50 écussons# donnent",
                  /*spanish*/ "#50 medallas de insectos# otorgan" },
            Text{ "#50 spider souls# yields", /*french*/ "#50 âmes d'arachnide# donnent",
                  /*spanish*/ "#50 almas de araña# otorgan" },
            Text{ "#50 auriferous arachnids# lead to", /*french*/ "#50 arachnides aurifères# donnent",
                  /*spanish*/ "#50 arácnidos auríferos# otorgan" },
        },
        {},
        // clear text
        Text{ "slaying #50 Gold Skulltulas# reveals", /*french*/ "détruire #50 Skulltulas d'or# donne",
              /*spanish*/ "exterminar #50 skulltulas doradas# revela" });

    hintTable[KAK_40_GOLD_SKULLTULA_REWARD] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#40 bug badges# rewards", /*french*/ "#40 écussons# donnent",
                  /*spanish*/ "#40 medallas de insectos# otorgan" },
            Text{ "#40 spider souls# yields", /*french*/ "#40 âmes d'arachnide# donnent",
                  /*spanish*/ "#40 almas de araña# otorgan" },
            Text{ "#40 auriferous arachnids# lead to", /*french*/ "#40 arachnides aurifères# donnent",
                  /*spanish*/ "#40 arácnidos auríferos# otorgan" },
        },
        {},
        // clear text
        Text{ "slaying #40 Gold Skulltulas# reveals", /*french*/ "détruire #40 Skulltulas d'or# donne",
              /*spanish*/ "exterminar #40 skulltulas doradas# revela" });

    hintTable[KAK_30_GOLD_SKULLTULA_REWARD] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#30 bug badges# rewards", /*french*/ "#30 écussons# donnent",
                  /*spanish*/ "#30 medallas de insectos# otorgan" },
            Text{ "#30 spider souls# yields", /*french*/ "#30 âmes d'arachnide# donnent",
                  /*spanish*/ "#30 almas de araña# otorgan" },
            Text{ "#30 auriferous arachnids# lead to", /*french*/ "#30 arachnides aurifères# donnent",
                  /*spanish*/ "#30 arácnidos auríferos# otorgan" },
        },
        {},
        // clear text
        Text{ "slaying #30 Gold Skulltulas# reveals", /*french*/ "détruire #30 Skulltulas d'or# donne",
              /*spanish*/ "exterminar #30 skulltulas doradas# revela" });

    hintTable[KAK_20_GOLD_SKULLTULA_REWARD] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#20 bug badges# rewards", /*french*/ "#20 écussons# donnent",
                  /*spanish*/ "#20 medallas de insectos# otorgan" },
            Text{ "#20 spider souls# yields", /*french*/ "#20 âmes d'arachnide# donnent",
                  /*spanish*/ "#20 almas de araña# otorgan" },
            Text{ "#20 auriferous arachnids# lead to", /*french*/ "#20 arachnides aurifères# donnent",
                  /*spanish*/ "#20 arácnidos auríferos# otorgan" },
        },
        {},
        // clear text
        Text{ "slaying #20 Gold Skulltulas# reveals", /*french*/ "détruire #20 Skulltulas d'or# donne",
              /*spanish*/ "exterminar #20 skulltulas doradas# revela" });

    hintTable[KAK_ANJU_AS_CHILD] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#wrangling roosters# rewards", /*french*/ "#plumer des poulets# donne",
                  /*spanish*/ "#atrapar a las gallinas# premia" },
            Text{ "#chucking chickens# gifts", /*french*/ "#lancer des poulets# donne",
                  /*spanish*/ "#reunir a unos emplumados# premia" },
        },
        {},
        // clear text
        Text{ "#collecting cuccos# rewards", /*french*/ "#rapporter les Cocottes# donne",
              /*spanish*/ "#hacerte con todos los cucos# premia" });

    hintTable[KAK_TRADE_POCKET_CUCCO] = HintText::Sometimes({
        // obscure text
        Text{ "an adult's #happy Cucco# awards", /*french*/ "un adulte avec une #poulette joyeuse# obtient",
              /*spanish*/ "un #alegre cuco# en la madurez otorga" },
    });

    hintTable[KAK_TRADE_ODD_MUSHROOM] = HintText::Sometimes({
        // obscure text
        Text{ "the #potion shop lady# entrusts", /*french*/ "la #gribiche du magasin de potion# confie",
              /*spanish*/ "la #señora de la tienda de pociones# otorga" },
    });

    hintTable[GC_DARUNIAS_JOY] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #groovin' goron# gifts", /*french*/ "#le Goron joyeux# donne",
                  /*spanish*/ "#un goron marchoso# otorga" },
        },
        {},
        // clear text
        Text{ "#Darunia's dance# leads to", /*french*/ "#la dance de Darunia# donne",
              /*spanish*/ "#el baile de Darunia# conduce a" });

    hintTable[LW_SKULL_KID] = HintText::Sometimes({
        // obscure text
        Text{ "the #Skull Kid# grants", /*french*/ "le #Skull Kid# donne", /*spanish*/ "#Skull Kid# otorga" },
    });

    hintTable[LW_TRADE_COJIRO] = HintText::Sometimes({
        // obscure text
        Text{ "returning a #special Cucco# awards", /*french*/ "ramener une #poulette précieuse# donne",
              /*spanish*/ "quien devuelva un #cuco especial# encontrará" },
    });

    hintTable[LW_TRADE_ODD_POTION] = HintText::Sometimes({
        // obscure text
        Text{ "a #Kokiri girl in the woods# leaves", /*french*/ "la #fillette Kokiri dans les bois# laisse",
              /*spanish*/ "una #chica kokiri del bosque# otorga" },
    });

    hintTable[LH_SUN] = HintText::Sometimes(
        {
            // obscure text
            Text{ "staring into #the sun# grants", /*french*/ "regarder #le soleil# donne",
                  /*spanish*/ "#mirar al sol# revela" },
        },
        {},
        // clear text
        Text{ "shooting #the sun# grants", /*french*/ "tirer une flèche dans #sur le soleil# donne",
              /*spanish*/ "#disparar al sol# revela" });

    hintTable[LH_TRADE_FROG] = HintText::Sometimes({
        // obscure text
        Text{ "#Lake Hylia's scientist# hurriedly entrusts", /*french*/ "le #scientifique du lac# confie rapidement",
              /*spanish*/ "el #científico del Lago Hylia# otorga con prisa" },
    });

    hintTable[MARKET_TREASURE_CHEST_GAME_REWARD] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#gambling# grants", /*french*/ "#parier# donne", /*spanish*/ "#los juegos de azar# revelan" },
            Text{ "there is a #1/32 chance# to win", /*french*/ "être #le gagnant parmi 32# donne",
                  /*spanish*/ "hay una #probabilidad de 1/32# de ganar" },
        },
        {},
        // clear text
        Text{ "the #treasure chest game# grants", /*french*/ "la #Chasse-aux-Trésors# donne",
              /*spanish*/ "#el Cofre del Tesoro# premia" });

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_1] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#gambling once# grants", /*french*/ "#parier une fois# donne",
                  /*spanish*/ "#apostar solo una vez# revelará" },
            Text{ "the #first or second game chest# contains",
                  /*french*/ "le #premier ou deuxième coffre à jeu# contient",
                  /*spanish*/ "#el primer o segundo cofre del azar# revela" },
        },
        {},
        // clear text
        Text{ "the #first locked room# in the chest game contains",
              /*french*/ "la #première salle# de la Chasse-aux-Trésors contient",
              /*spanish*/ "#en la primera sala del Cofre del Tesoro# aguarda" });

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_2] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#gambling twice# grants", /*french*/ "#parier deux fois# donne",
                  /*spanish*/ "#apostar dos veces# revelará" },
            Text{ "the #third or fourth game chest# contains",
                  /*french*/ "le #troisième ou quatrième coffre à jeu# contient",
                  /*spanish*/ "#el tercer o cuarto cofre del azar# revela" },
        },
        {},
        // clear text
        Text{ "the #second locked room# in the chest game contains",
              /*french*/ "la #deuxième salle# de la Chasse-aux-Trésors contient",
              /*spanish*/ "#en la segunda sala del Cofre del Tesoro# aguarda" });

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_3] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#gambling 3 times# grants", /*french*/ "#parier trois fois# donne",
                  /*spanish*/ "#apostar tres veces# revelará" },
            Text{ "the #fifth or sixth game chest# contains",
                  /*french*/ "le #cinquième ou sixième coffre à jeu# contient",
                  /*spanish*/ "#el quinto o sexto cofre del azar# revela" },
        },
        {},
        // clear text
        Text{ "the #third locked room# in the chest game contains",
              /*french*/ "la #troisième salle# de la Chasse-aux-Trésors contient",
              /*spanish*/ "#en la tercera sala del Cofre del Tesoro# aguarda" });

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_4] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#gambling 4 times# grants", /*french*/ "#parier quatre fois# donne",
                  /*spanish*/ "#apostar cuatro veces# revelará" },
            Text{ "the #seventh or eighth game chest# contains",
                  /*french*/ "le #septième ou huitième coffre à jeu# contient",
                  /*spanish*/ "#el séptimo u octavo cofre del azar# revela" },
        },
        {},
        // clear text
        Text{ "the #fourth locked room# in the chest game contains",
              /*french*/ "la #quatrième salle# de la Chasse-aux-Trésors contient",
              /*spanish*/ "#en la cuarta sala del Cofre del Tesoro# aguarda" });

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_5] = HintText::Sometimes(
        {
            // obscure text
            Text{ "#gambling 5 times# grants", /*french*/ "#parier cinq fois# donne",
                  /*spanish*/ "#apostar cinco veces# revelará" },
            Text{ "the #ninth or tenth game chest# contains",
                  /*french*/ "le #neuvième ou dixième coffre à jeu# contient",
                  /*spanish*/ "#el noveno o décimo cofre del azar# revela" },
        },
        {},
        // clear text
        Text{ "the #fifth locked room# in the chest game contains",
              /*french*/ "la #cinquième salle# de la Chasse-aux-Trésors contient",
              /*spanish*/ "#en la quinta sala del Cofre del Tesoro# aguarda" });

    hintTable[GF_HBA_1500_POINTS] = HintText::Sometimes(
        {
            // obscure text
            Text{ "mastery of #horseback archery# grants", /*french*/ "maîtriser l'#archerie équestre# donne",
                  /*spanish*/ "dominar el #tiro con arco a caballo# premia con" },
        },
        {},
        // clear text
        Text{ "scoring 1500 in #horseback archery# grants",
              /*french*/ "obtenir 1500 points dans l'#archerie équestre# donne",
              /*spanish*/ "conseguir 1500 puntos en el #tiro con arco a caballo# premia" });

    hintTable[GRAVEYARD_HEART_PIECE_GRAVE_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "playing #Sun's Song# in a grave spawns", /*french*/ "jouer le #chant du soleil# dans un tombeau donne",
              /*spanish*/ "#tocar la Canción del Sol# en una cripta conduce a" },
    });

    hintTable[GC_MAZE_LEFT_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "in #Goron City# the hammer unlocks", /*french*/ "dans le #village Goron#, le marteau donne accès à",
              /*spanish*/ "en la #Ciudad Goron# el martillo desbloquea" },
    });

    hintTable[GV_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "in #Gerudo Valley# the hammer unlocks", /*french*/ "dans la #Vallée Gerudo#, le marteau donne accès à",
              /*spanish*/ "en el #Valle Gerudo# el martillo desbloquea" },
    });

    hintTable[GV_TRADE_SAW] = HintText::Sometimes({
        // obscure text
        Text{ "the #boss of the carpenters# leaves", /*french*/ "le #patron des ouvriers# laisse",
              /*spanish*/ "el #capataz de los carpinteros# otorga" },
    });

    hintTable[GV_COW] = HintText::Sometimes({
        // obscure text
        Text{ "a #cow in Gerudo Valley# gifts", /*french*/ "la #vache de la Vallée Gerudo# donne",
              /*spanish*/ "una #vaca del Valle Gerudo# brinda" },
    });

    hintTable[HC_GS_STORMS_GROTTO] = HintText::Sometimes({
        // obscure text
        Text{ "a #spider behind a muddy wall# in a grotto holds",
              /*french*/ "l'#araignée derrière un mur de boue# dans une grotte donne",
              /*spanish*/ "una #Skulltula tras la agrietada pared# de una cueva otorga" },
    });

    hintTable[HF_GS_COW_GROTTO] = HintText::Sometimes({
        // obscure text
        Text{ "a #spider behind webs# in a grotto holds",
              /*french*/ "l'#araignée derrière une toile# dans une grotte donne",
              /*spanish*/ "una #Skulltula tras la telaraña# de una cueva otorga" },
    });

    hintTable[HF_COW_GROTTO_COW] = HintText::Sometimes(
        {
            // obscure text
            Text{ "the #cobwebbed cow# gifts", /*french*/ "la #vache prisonnière d'araignées# donne",
                  /*spanish*/ "una #vaca tras una telaraña# brinda" },
        },
        {},
        // clear text
        Text{ "a #cow behind webs# in a grotto gifts", /*french*/ "la #vache derrière les toiles# d'une grotte donne",
              /*spanish*/ "una #vaca tras la telaraña# de una cueva brinda" });

    hintTable[ZF_GS_HIDDEN_CAVE] = HintText::Sometimes({
        // obscure text
        Text{ "a spider high #above the icy waters# holds", /*french*/ "l'araignée #en haut des eaux glacées# donne",
              /*spanish*/ "una Skulltula en lo #alto de las congeladas aguas# otorga" },
    });

    hintTable[WASTELAND_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "#deep in the wasteland# is", /*french*/ "#loin dans le désert# gît",
              /*spanish*/ "en lo #profundo del desierto encantado# yace" },
        Text{ "beneath #the sands#, flames reveal", /*french*/ "#sous le désert#, les flammes font apparaître",
              /*spanish*/ "tras las #arenas# unas llamas revelan" },
    });

    hintTable[WASTELAND_GS] = HintText::Sometimes({
        // obscure text
        Text{ "a #spider in the wasteland# holds", /*french*/ "#l'araignée dans le désert# donne",
              /*spanish*/ "una #Skulltula del desierto encantado# otorga" },
    });

    hintTable[GRAVEYARD_COMPOSERS_GRAVE_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "#flames in the Composers' Grave# reveal",
              /*french*/ "#les flammes dans le tombeau des compositeurs# cachent",
              /*spanish*/ "#las llamas del Panteón Real# revelan" },
        Text{ "the #Composer Brothers hid#", /*french*/ "#les Frères compositeurs on caché#",
              /*spanish*/ "los #hermanos compositores esconden#" },
    });

    hintTable[ZF_BOTTOM_FREESTANDING_POH] = HintText::Sometimes({
        // obscure text
        Text{ "#under the icy waters# lies", /*french*/ "#sous les eaux glacées# se cache",
              /*spanish*/ "#bajo las congeladas aguas# yace" },
    });

    hintTable[GC_POT_FREESTANDING_POH] = HintText::Sometimes({
        // obscure text
        Text{ "spinning #Goron pottery# contains", /*french*/ "la #potterie Goron# contient",
              /*spanish*/ "una #cerámica goron# contiene" },
    });

    hintTable[ZD_KING_ZORA_THAWED] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #defrosted dignitary# gifts", /*french*/ "le #monarque libéré# donne",
                  /*spanish*/ "una #liberación monárquica# brinda" },
        },
        {},
        // clear text
        Text{ "unfreezing #King Zora# grants", /*french*/ "dégeler # le Roi Zora# donne",
              /*spanish*/ "#descongelar al Rey Zora# conduce a" });

    hintTable[ZD_TRADE_PRESCRIPTION] = HintText::Sometimes({
        // obscure text
        Text{ "#King Zora# hurriedly entrusts", /*french*/ "le #roi Zora# confie rapidement",
              /*spanish*/ "el #Rey Zora# otorga con prisa" },
    });

    hintTable[DMC_DEKU_SCRUB] = HintText::Sometimes({
        // obscure text
        Text{ "a single #scrub in the crater# sells", /*french*/ "la #peste Mojo dans le cratère# vend",
              /*spanish*/ "un solitario #deku del cráter# vende" },
    });

    hintTable[DMC_GS_CRATE] = HintText::Sometimes({
        // obscure text
        Text{ "a spider under a #crate in the crater# holds", /*french*/ "la Skulltula dans une #boîte volcanique# a",
              /*spanish*/ "una Skulltula bajo una #caja del cráter# otorga" },
    });

    hintTable[DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #temporal stone within a tree# contains", /*french*/ "la #pierre bleue dans un arbre# mène à",
                  /*spanish*/ "un #bloque temporal de un árbol# contiene" },
        },
        {},
        // clear text
        Text{ "a #temporal stone within the Deku Tree# contains",
              /*french*/ "la #pierre temporelle dans l'Arbre Mojo# cache",
              /*spanish*/ "un #bloque temporal del Gran Árbol Deku# contiene" });

    hintTable[DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #spider on a ceiling in a tree# holds", /*french*/ "l'#araignée haut-perchée dans un arbre# a",
                  /*spanish*/ "una #Skulltula en el techo de un árbol# otorga" },
        },
        {},
        // clear text
        Text{ "a #spider on a ceiling in the Deku Tree# holds",
              /*french*/ "la#Skulltula dans le Cimetière de l'Arbre Mojo# a",
              /*spanish*/ "una #Skulltula en el techo del Gran Árbol Deku# otorga" });

    hintTable[DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a spider under #temporal stones in a cavern# holds",
                  /*french*/ "l'araignée sous #une pierre bleue dans une caverne# a",
                  /*spanish*/ "una Skulltula bajo #bloques temporales de una cueva# otorga" },
        },
        {},
        // clear text
        Text{ "a spider under #temporal stones in Dodongo's Cavern# holds",
              /*french*/ "la Skulltula sous #la pierre temporelle dans la Caverne Dodongo# a",
              /*spanish*/ "una Skulltula bajo #bloques temporales de la Cueva de los Dodongos# otorga" });

    hintTable[JABU_JABUS_BELLY_BOOMERANG_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a school of #stingers swallowed by a deity# guard",
                  /*french*/ "les #raies dans un gardien# protègent",
                  /*spanish*/ "unos de #stingers engullidos por cierta deidad# guardan" },
        },
        {},
        // clear text
        Text{ "a school of #stingers swallowed by Jabu-Jabu# guard", /*french*/ "les #raies dans Jabu-Jabu# protègent",
              /*spanish*/ "unos #stingers engullidos por Jabu-Jabu# guardan" });

    hintTable[JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a spider surrounded by #shadows in the belly of a deity# holds",
                  /*french*/ "l'araignée entourée d'#ombres dans le ventre du gardien# possède",
                  /*spanish*/ "una Skulltula rodeada de #sombras en la tripa de cierta diedad# otorga" },
        },
        {},
        // clear text
        Text{ "a spider surrounded by #shadows in Jabu-Jabu's Belly# holds",
              /*french*/ "la Skulltula entourée d'#ombres dans Jabu-Jabu# possède",
              /*spanish*/ "una Skulltula rodeada de #sombras en la Tripa de Jabu-Jabu# otorga" });

    hintTable[JABU_JABUS_BELLY_MQ_COW] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #cow swallowed by a deity# gifts", /*french*/ "la #vache dans le gardien# donne",
                  /*spanish*/ "una #vaca engullida por cierta deidad# brinda" },
        },
        {},
        // clear text
        Text{ "a #cow swallowed by Jabu-Jabu# gifts", /*french*/ "la #vache avallée par Jabu-Jabu# donne",
              /*spanish*/ "una #vaca engullida por Jabu-Jabu# brinda" });

    hintTable[FIRE_TEMPLE_SCARECROW_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #scarecrow atop the volcano# hides", /*french*/ "l'#épouvantail au sommet d'un volcan# donne",
                  /*spanish*/ "un #espantapájaros en lo alto del volcán# esconde" },
        },
        {},
        // clear text
        Text{ "#Pierre atop the Fire Temple# hides", /*french*/ "#Pierre au sommet du Temple du Feu# donne",
              /*spanish*/ "#Pierre en lo alto del Templo del Fuego# esconde" });

    hintTable[FIRE_TEMPLE_MEGATON_HAMMER_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "the #Flare Dancer atop the volcano# guards a chest containing",
                  /*french*/ "le #danseur au sommet du volcan# protège",
                  /*spanish*/ "el #Bailafuego en lo alto del volcán# otorga" },
        },
        {},
        // clear text
        Text{ "the #Flare Dancer atop the Fire Temple# guards a chest containing",
              /*french*/ "le #Danse-Flamme au sommet du Temple du Feu# protège",
              /*spanish*/ "el #Bailaguego en lo alto del Templo del Fuego# otorga" });

    hintTable[FIRE_TEMPLE_MQ_CHEST_ON_FIRE] = HintText::Sometimes(
        {
            // obscure text
            Text{ "the #Flare Dancer atop the volcano# guards a chest containing",
                  /*french*/ "le #danseur au sommet du volcan# protège",
                  /*spanish*/ "el #Bailafuego en lo alto del volcán# otorga" },
        },
        {},
        // clear text
        Text{ "the #Flare Dancer atop the Fire Temple# guards a chest containing",
              /*french*/ "le #Danse-Flamme au sommet du Temple du Feu# protège",
              /*spanish*/ "el #Bailafuego en lo alto del Templo del Fuego# otorga" });

    hintTable[FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #spider under a block in the volcano# holds",
                  /*french*/ "l'#araignée sous un bloc dans le volcan# a",
                  /*spanish*/ "una #Skulltula bajo el bloque de un volcán# otorga" },
        },
        {},
        // clear text
        Text{ "a #spider under a block in the Fire Temple# holds",
              /*french*/ "une #Skulltula sous un bloc dans le Temple du Feu# a",
              /*spanish*/ "una #Skulltula bajo un bloque del Templo del Fuego# otorga" });

    hintTable[WATER_TEMPLE_RIVER_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "beyond the #river under the lake# waits", /*french*/ "au delà de #la rivière sous le lac# se cache",
                  /*spanish*/ "tras el #río bajo el lago# yace" },
        },
        {},
        // clear text
        Text{ "beyond the #river in the Water Temple# waits",
              /*french*/ "au delà de #la rivière dans le Temple de l'Eau# se cache",
              /*spanish*/ "tras el #río del Templo del Agua# yace" });

    hintTable[WATER_TEMPLE_BOSS_KEY_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "dodging #rolling boulders under the lake# leads to",
                  /*french*/ "éviter des #rochers roulants sous le lac# mène à",
                  /*spanish*/ "esquivar #rocas rodantes bajo el lago# conduce a" },
        },
        {},
        // clear text
        Text{ "dodging #rolling boulders in the Water Temple# leads to",
              /*french*/ "éviter des #rochers roulants dans le Temple de l'Eau# mène à",
              /*spanish*/ "esquivar #rocas rondantes del Templo del Agua# conduce a" });

    hintTable[WATER_TEMPLE_GS_BEHIND_GATE] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a spider behind a #gate under the lake# holds",
                  /*french*/ "l'araignée derrière une #barrière sous le lac# a",
                  /*spanish*/ "una Skulltula tras #una valla bajo el lago# otorga" },
        },
        {},
        // clear text
        Text{ "a spider behind a #gate in the Water Temple# holds",
              /*french*/ "la Skulltula derrière une #barrière dans le Temple de l'Eau# a",
              /*spanish*/ "una Skulltula tras #una valla del Templo del Agua# otorga" });

    hintTable[WATER_TEMPLE_MQ_FREESTANDING_KEY] = HintText::Sometimes(
        {
            // obscure text
            Text{ "hidden in a #box under the lake# lies", /*french*/ "dans une #boîte sous le lac# gît",
                  /*spanish*/ "en una #caja bajo el lago# yace" },
        },
        {},
        // clear text
        Text{ "hidden in a #box in the Water Temple# lies", /*french*/ "dans une #boîte dans le Temple de l'Eau# gît",
              /*spanish*/ "en una #caja del Templo del Agua# yace" });

    hintTable[WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA] = HintText::Sometimes(
        {
            // obscure text
            Text{ "the #locked spider under the lake# holds", /*french*/ "l'#araignée emprisonnée sous le lac# a",
                  /*spanish*/ "la #Skulltula enjaulada bajo el lago# otorga" },
        },
        {},
        // clear text
        Text{ "the #locked spider in the Water Temple# holds",
              /*french*/ "une #Skulltula emprisonnée dans le Temple de l'Eau# a",
              /*spanish*/ "la #Skulltula enjaulada del Templo del Agua# otorga" });

    hintTable[WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a spider behind a #gate under the lake# holds",
                  /*french*/ "l'#araignée derrière une barrière sous le lac# a",
                  /*spanish*/ "una Skulltula tras una #valla bajo el lago# otorga" },
        },
        {},
        // clear text
        Text{ "a spider behind a #gate in the Water Temple# holds",
              /*french*/ "une #Skulltula derrière une barrière dans le Temple de l'Eau# a",
              /*spanish*/ "una Skulltula tras una #valla del Templo del Agua#" });

    hintTable[GERUDO_TRAINING_GROUNDS_UNDERWATER_SILVER_RUPEE_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "those who seek #sunken silver rupees# will find",
              /*french*/ "ceux qui pêchent les #joyaux argentés# trouveront",
              /*spanish*/ "aquellos que busquen las #rupias plateadas sumergidas# encontrarán" },
        Text{ "the #thieves' underwater training# rewards", /*french*/ "l'#épreuve de plongée des voleurs# recèle",
              /*spanish*/ "la #instrucción submarina de las bandidas# premia" },
    });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER_SILVER_RUPEE_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "those who seek #sunken silver rupees# will find",
              /*french*/ "ceux qui pêchent les #joyaux argentés# trouveront",
              /*spanish*/ "aquellos que busquen las #rupias plateadas sumergidas# encontrarán" },
        Text{ "the #thieves' underwater training# rewards", /*french*/ "l'#épreuve de plongée des voleurs# recèle",
              /*spanish*/ "la #instrucción submarina de las bandidas# premia" },
    });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_PATH_FINAL_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "the final prize of #the thieves' training# is",
              /*french*/ "la récompense ultime de #l'épreuve des voleurs# est",
              /*spanish*/ "la recompensa final de la #instrucción de las bandida# brinda" },
    });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_ICE_ARROWS_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "the final prize of #the thieves' training# is",
              /*french*/ "la récompense ultime de #l'épreuve des voleurs# est",
              /*spanish*/ "el premio final de la #instrucción de las bandidas# brinda" },
    });

    hintTable[BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "the well's #grasping ghoul# hides", /*french*/ "la #terreur du Puits# cache",
                  /*spanish*/ "en las #profundidades del pozo# se esconde" },
            Text{ "a #nether dweller in the well# holds", /*french*/ "le #spectre qui réside dans le Puits# a",
                  /*spanish*/ "el #temido morador del pozo# concede" },
        },
        {},
        // clear text
        Text{ "#Dead Hand in the well# holds", /*french*/ "le #Poigneur dans le Puits# cache",
              /*spanish*/ "la #Mano Muerta del pozo# concede" });

    hintTable[BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "the well's #grasping ghoul# hides", /*french*/ "la #terreur du Puits# cache",
                  /*spanish*/ "en las #profundidades del pozo# se esconde" },
            Text{ "a #nether dweller in the well# holds", /*french*/ "le #spectre qui réside dans le Puits# a",
                  /*spanish*/ "el #temido morador del pozo# concede" },
        },
        {},
        // clear text
        Text{ "#Dead Hand in the well# holds", /*french*/ "le #Poigneur dans le Puits# cache",
              /*spanish*/ "la #Mano Muerta del pozo# concede" });

    hintTable[SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "the treasure #sought by Nabooru# is", /*french*/ "le trésor que #recherche Nabooru# est",
                  /*spanish*/ "el #ansiado tesoro de Nabooru# brinda" },
        },
        {},
        // clear text
        Text{ "upon the #Colossus's right hand# is", /*french*/ "sur la #main droite du colosse# repose",
              /*spanish*/ "en la #mano derecha del Coloso# yace" });

    hintTable[SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "upon the #Colossus's left hand# is", /*french*/ "sur la #main gauche du colosse# repose",
              /*spanish*/ "en la #mano izquierda del Coloso# yace" },
    });

    hintTable[SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #temporal paradox in the Colossus# yields",
                  /*french*/ "un #paradoxe temporel dans le colosse# révèle",
                  /*spanish*/ "una #paradoja temporal del Coloso# conduce a" },
        },
        {},
        // clear text
        Text{ "a #temporal paradox in the Spirit Temple# yields",
              /*french*/ "le #paradoxe temporel dans le Temple de l'Esprit# révèle",
              /*spanish*/ "una #paradoja temporal del Coloso# conduce a" });

    hintTable[SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #symphony in the Colossus# yields", /*french*/ "la #symphonie du colosse# révèle",
                  /*spanish*/ "una #sinfonía del Coloso# conduce a" },
        },
        {},
        // clear text
        Text{ "a #symphony in the Spirit Temple# yields",
              /*french*/ "les #cinq chansons du Temple de l'Esprit# révèlent",
              /*spanish*/ "una #sinfonía del Coloso# conduce a" });

    hintTable[SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #spider's symphony in the Colossus# yields",
                  /*french*/ "la #mélodie de l'araignée du colosse# révèle",
                  /*spanish*/ "la #Skulltula de la sinfonía del Coloso# otorga" },
        },
        {},
        // clear text
        Text{ "a #spider's symphony in the Spirit Temple# yields",
              /*french*/ "la #mélodie de la Skulltula du Temple de l'Esprit# révèle",
              /*spanish*/ "la #Skulltula de la sinfonía del Coloso# otorga" });

    hintTable[SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "shadows in an #invisible maze# guard", /*french*/ "les ombres dans le #labyrinthe invisible# protègent",
              /*spanish*/ "las sombras del #laberinto misterioso# esconden" },
    });

    hintTable[SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "shadows in an #invisible maze# guard", /*french*/ "les ombres dans le #labyrinthe invisible# protègent",
              /*spanish*/ "las sombras del #laberinto invisible# esconden" },
    });

    /*--------------------------
    |    ENTRANCE HINT TEXT    |
    ---------------------------*/

    hintTable[DESERT_COLOSSUS_TO_COLOSSUS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "lifting a #rock in the desert# reveals", /*french*/ "soulever une #roche dans le désert# révèle",
              /*spanish*/ "levantar una #roca del desierto# revela" },
    });

    hintTable[GV_GROTTO_LEDGE_TO_GV_OCTOROK_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a rock on #a ledge in the valley# hides", /*french*/ "soulever une #roche dans la vallée# révèle",
              /*spanish*/ "levantar una #roca al borde del valle# esconde" },
    });

    hintTable[GC_GROTTO_PLATFORM_TO_GC_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #pool of lava# in Goron City blocks the way to",
              /*french*/ "l'#étang de lave# dans le village Goron renferme",
              /*spanish*/ "un #estanque de lava# en la Ciudad Goron bloquea el paso a" },
    });

    hintTable[GERUDO_FORTRESS_TO_GF_STORMS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #storm within Gerudo's Fortress# reveals", /*french*/ "la #tempête dans la forteresse# révèle",
              /*spanish*/ "una #tormenta en la Fortaleza Gerudo# revela" },
    });

    hintTable[ZORAS_DOMAIN_TO_ZD_STORMS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #storm within Zora's Domain# reveals", /*french*/ "la #tempête dans le Domaine Zora# révèle",
              /*spanish*/ "una #tormenta en la Región de los Zora# revela" },
    });

    hintTable[HYRULE_CASTLE_GROUNDS_TO_HC_STORMS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #storm near the castle# reveals", /*french*/ "la #tempête près du château# révèle",
              /*spanish*/ "una #tormenta junto al castillo# revela" },
    });

    hintTable[GV_FORTRESS_SIDE_TO_GV_STORMS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #storm in the valley# reveals", /*french*/ "la #tempête dans la vallée# révèle",
              /*spanish*/ "una #tormenta en el valle# revela" },
    });

    hintTable[DESERT_COLOSSUS_TO_COLOSSUS_GREAT_FAIRY_FOUNTAIN] = HintText::Entrance({
        // obscure text
        Text{ "a #fractured desert wall# hides", /*french*/ "le #mur fragile du désert# cache",
              /*spanish*/ "una #agrietada pared del desierto# esconde" },
    });

    hintTable[GANONS_CASTLE_GROUNDS_TO_OGC_GREAT_FAIRY_FOUNTAIN] = HintText::Entrance({
        // obscure text
        Text{ "a #heavy pillar# outside the castle obstructs", /*french*/ "le #rocher fragile près du château# cache",
              /*spanish*/ "una #pesada columna# fuera del castillo obstruye" },
    });

    hintTable[ZORAS_FOUNTAIN_TO_ZF_GREAT_FAIRY_FOUNTAIN] = HintText::Entrance({
        // obscure text
        Text{ "a #fountain wall# hides", /*french*/ "le #mur fragile du réservoir# cache",
              /*spanish*/ "una #pared de la fuente# esconde" },
    });

    hintTable[GV_FORTRESS_SIDE_TO_GV_CARPENTER_TENT] = HintText::Entrance({
        // obscure text
        Text{ "a #tent in the valley# covers", /*french*/ "la #tente dans la vallée# recouvre",
              /*spanish*/ "una #tienda de campaña del valle# cubre" },
    });

    hintTable[GRAVEYARD_WARP_PAD_REGION_TO_SHADOW_TEMPLE_ENTRYWAY] = HintText::Entrance({
        // obscure text
        Text{ "at the #back of the Graveyard#, there is", /*french*/ "#derrière le Cimetière# gît",
              /*spanish*/ "en la #parte trasera del cementerio# se halla" },
    });

    hintTable[LAKE_HYLIA_TO_WATER_TEMPLE_LOBBY] = HintText::Entrance({
        // obscure text
        Text{ "deep #under a vast lake#, one can find", /*french*/ "#sous le lac# gît",
              /*spanish*/ "en las #profundidades de un lago inmenso# se halla" },
    });

    hintTable[GERUDO_FORTRESS_TO_GERUDO_TRAINING_GROUNDS_LOBBY] = HintText::Entrance({
        // obscure text
        Text{ "paying a #fee to the Gerudos# grants access to",
              /*french*/ "l'#entrée payante des Gerudo# donne accès à",
              /*spanish*/ "pagarle una #tasa a las gerudo# da acceso a" },
    });

    hintTable[ZORAS_FOUNTAIN_TO_JABU_JABUS_BELLY_BEGINNING] = HintText::Entrance({
        // obscure text
        Text{ "inside #Jabu-Jabu#, one can find", /*french*/ "#dans Jabu-Jabu# se trouve",
              /*spanish*/ "dentro de #Jabu-Jabu# se halla" },
    });

    hintTable[KAKARIKO_VILLAGE_TO_BOTTOM_OF_THE_WELL] = HintText::Entrance({
        // obscure text
        Text{ "a #village well# leads to", /*french*/ "dans le fond du #Puits du village# gît",
              /*spanish*/ "el #pozo de un pueblo# conduce a" },
    });

    /*--------------------------
    |      EXIT HINT TEXT      |
    ---------------------------*/
    // maybe make a new type for this? I'm not sure if it really matters

    hintTable[LINKS_POCKET] = HintText::Exclude({
        // obscure text
        Text{ "Link's Pocket", /*french*/ "les Poches de @", /*spanish*/ "el bolsillo de @" },
    });

    hintTable[KOKIRI_FOREST] = HintText::Exclude({
        // obscure text
        Text{ "Kokiri Forest", /*french*/ "la Forêt Kokiri", /*spanish*/ "el Bosque Kokiri" },
    });

    hintTable[THE_LOST_WOODS] = HintText::Exclude({
        // obscure text
        Text{ "the Lost Woods", /*french*/ "les Bois Perdus", /*spanish*/ "el Bosque Perdido" },
    });

    hintTable[SACRED_FOREST_MEADOW] = HintText::Exclude({
        // obscure text
        Text{ "Sacred Forest Meadow", /*french*/ "le Bosquet Sacré", /*spanish*/ "la pradera sagrada del bosque" },
    });

    hintTable[HYRULE_FIELD] = HintText::Exclude({
        // obscure text
        Text{ "Hyrule Field", /*french*/ "la Plaine d'Hyrule", /*spanish*/ "la Llanura de Hyrule" },
    });

    hintTable[LAKE_HYLIA] = HintText::Exclude({
        // obscure text
        Text{ "Lake Hylia", /*french*/ "le Lac Hylia", /*spanish*/ "el Lago Hylia" },
    });

    hintTable[GERUDO_VALLEY] = HintText::Exclude({
        // obscure text
        Text{ "Gerudo Valley", /*french*/ "la Vallée Gerudo", /*spanish*/ "el Valle Gerudo" },
    });

    hintTable[GERUDO_FORTRESS] = HintText::Exclude({
        // obscure text
        Text{ "Gerudo's Fortress", /*french*/ "le Repaire des Voleurs", /*spanish*/ "la Fortaleza Gerudo" },
    });

    hintTable[HAUNTED_WASTELAND] = HintText::Exclude({
        // obscure text
        Text{ "Haunted Wasteland", /*french*/ "le Désert Hanté", /*spanish*/ "el desierto encantado" },
    });

    hintTable[DESERT_COLOSSUS] = HintText::Exclude({
        // obscure text
        Text{ "Desert Colossus", /*french*/ "le Colosse du Désert", /*spanish*/ "el Coloso del Desierto" },
    });

    hintTable[THE_MARKET] = HintText::Exclude({
        // obscure text
        Text{ "the Market", /*french*/ "la Place du Marché", /*spanish*/ "la plaza del mercado" },
    });

    hintTable[TEMPLE_OF_TIME] = HintText::Exclude({
        // obscure text
        Text{ "Temple of Time", /*french*/ "le Temple du Temps", /*spanish*/ "el Templo del Tiempo" },
    });

    hintTable[HYRULE_CASTLE] = HintText::Exclude({
        // obscure text
        Text{ "Hyrule Castle", /*french*/ "le Château d'Hyrule", /*spanish*/ "el Castillo de Hyrule" },
    });

    hintTable[OUTSIDE_GANONS_CASTLE] = HintText::Exclude({
        // obscure text
        Text{ "outside Ganon's Castle", /*french*/ "les alentours du Château&de Ganon",
              /*spanish*/ "el exterior del Castillo de Ganon" },
    });

    hintTable[KAKARIKO_VILLAGE] = HintText::Exclude({
        // obscure text
        Text{ "Kakariko Village", /*french*/ "le Village Cocorico", /*spanish*/ "Kakariko" },
    });

    hintTable[THE_GRAVEYARD] = HintText::Exclude({
        // obscure text
        Text{ "the Graveyard", /*french*/ "le Cimetière", /*spanish*/ "el cementerio" },
    });

    hintTable[DEATH_MOUNTAIN_TRAIL] = HintText::Exclude({
        // obscure text
        Text{ "Death Mountain Trail", /*french*/ "le Chemin du Péril",
              /*spanish*/ "el sendero de la Montaña de la Muerte" },
    });

    hintTable[GORON_CITY] = HintText::Exclude({
        // obscure text
        Text{ "Goron City", /*french*/ "le Village Goron", /*spanish*/ "la Ciudad Goron" },
    });

    hintTable[DEATH_MOUNTAIN_CRATER] = HintText::Exclude({
        // obscure text
        Text{ "Death Mountain Crater", /*french*/ "le Cratère du Péril",
              /*spanish*/ "el cráter de la Montaña de la Muerte" },
    });

    hintTable[ZORAS_RIVER] = HintText::Exclude({
        // obscure text
        Text{ "Zora's River", /*french*/ "la Rivière Zora", /*spanish*/ "el Río Zora" },
    });

    hintTable[ZORAS_DOMAIN] = HintText::Exclude({
        // obscure text
        Text{ "Zora's Domain", /*french*/ "le Domaine Zora", /*spanish*/ "la Región de los Zora" },
    });

    hintTable[ZORAS_FOUNTAIN] = HintText::Exclude({
        // obscure text
        Text{ "Zora's Fountain", /*french*/ "la Fontaine Zora", /*spanish*/ "la Fuente Zora" },
    });

    hintTable[LON_LON_RANCH] = HintText::Exclude({
        // obscure text
        Text{ "Lon Lon Ranch", /*french*/ "le Ranch Lon Lon", /*spanish*/ "el Rancho Lon Lon" },
    });

    /*--------------------------
    |     REGION HINT TEXT     |
    ---------------------------*/

    hintTable[KF_LINKS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Link's House", /*french*/ "la #maison de @#", /*spanish*/ "la casa de @" },
    });

    //   hintTable[TOT_MAIN] = HintText::Region({
    //                    //obscure text
    //                    Text{"the #Temple of Time#", /*french*/"le #Temple du Temps#", /*spanish*/"el Templo del
    //                    Tiempo"},
    // });

    hintTable[KF_MIDOS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Mido's house", /*french*/ "la Cabane du Grand Mido", /*spanish*/ "la casa de Mido" },
    });

    hintTable[KF_SARIAS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Saria's House", /*french*/ "la Cabane de Saria", /*spanish*/ "la casa de Saria" },
    });

    hintTable[KF_HOUSE_OF_TWINS] = HintText::Region({
        // obscure text
        Text{ "the #House of Twins#", /*french*/ "la Cabane des Jumelles", /*spanish*/ "la casa de las gemelas" },
    });

    hintTable[KF_KNOW_IT_ALL_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Know-It-All Brothers' House", /*french*/ "la Cabane des frères Je-Sais-Tout",
              /*spanish*/ "la casa de los hermanos Sabelotodo" },
    });

    hintTable[KF_KOKIRI_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Kokiri Shop#", /*french*/ "le #Magasin Kokiri#", /*spanish*/ "la tienda kokiri" },
    });

    hintTable[LH_LAB] = HintText::Region({
        // obscure text
        Text{ "the #Lakeside Laboratory#", /*french*/ "le #Laboratoire du Lac#",
              /*spanish*/ "el laboratorio del lago" },
    });

    hintTable[LH_FISHING_HOLE] = HintText::Region({
        // obscure text
        Text{ "the #Fishing Pond#", /*french*/ "l'#Étang#", /*spanish*/ "el estanque" },
    });

    hintTable[GV_CARPENTER_TENT] = HintText::Region({
        // obscure text
        Text{ "the #Carpenters' tent#", /*french*/ "la #Tente des charpentiers#",
              /*spanish*/ "la #tienda de campaña de los carpinteros#" },
    });

    hintTable[MARKET_GUARD_HOUSE] = HintText::Region({
        // obscure text
        Text{ "the #Guard House#", /*french*/ "le #poste de garde#", /*spanish*/ "la caseta de guardia" },
    });

    hintTable[MARKET_MASK_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Happy Mask Shop#", /*french*/ "la #Foire Aux Masques#",
              /*spanish*/ "la tienda de La Máscara Feliz" },
    });

    hintTable[MARKET_BOMBCHU_BOWLING] = HintText::Region({
        // obscure text
        Text{ "the #Bombchu Bowling Alley#", /*french*/ "le #Bowling Teigneux#", /*spanish*/ "la Bolera Bombchu" },
    });

    hintTable[MARKET_POTION_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Market Potion Shop#", /*french*/ "l'#apothicaire de la Place du Marché#",
              /*spanish*/ "la tienda de pociones de la plaza del mercado" },
    });

    hintTable[MARKET_TREASURE_CHEST_GAME] = HintText::Region({
        // obscure text
        Text{ "the #Treasure Chest Shop#", /*french*/ "la #Chasse-aux-Trésors#", /*spanish*/ "el Cofre del Tesoro" },
    });

    hintTable[MARKET_BOMBCHU_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Bombchu Shop#", /*french*/ "le #Magasin de Missiles#", /*spanish*/ "la Tienda Bombchu" },
    });

    hintTable[MARKET_MAN_IN_GREEN_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Man in Green's House", /*french*/ "la #Maison de l'Homme en Vert#",
              /*spanish*/ "la casa del hombre de verde" },
    });

    hintTable[KAK_WINDMILL] = HintText::Region({
        // obscure text
        Text{ "the #Windmill#", /*french*/ "le #Moulin#", /*spanish*/ "el #molino#" },
    });

    hintTable[KAK_CARPENTER_BOSS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "the #Carpenters' Boss House#", /*french*/ "la #Maison du Chef des ouvriers#",
              /*spanish*/ "la casa del capataz de los carpinteros" },
    });

    hintTable[KAK_HOUSE_OF_SKULLTULA] = HintText::Region({
        // obscure text
        Text{ "the #House of Skulltula#", /*french*/ "la #Maison des Skulltulas#",
              /*spanish*/ "la casa de las Skulltulas" },
    });

    hintTable[KAK_IMPAS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Impa's House", /*french*/ "la #Maison d'Impa#", /*spanish*/ "la casa de Impa" },
    });

    hintTable[KAK_IMPAS_HOUSE_BACK] = HintText::Region({
        // obscure text
        Text{ "Impa's cow cage", /*french*/ "la #cage à vache d'Impa#", /*spanish*/ "la jaula de la vaca de Impa" },
    });

    hintTable[KAK_ODD_POTION_BUILDING] = HintText::Region({
        // obscure text
        Text{ "Granny's Potion Shop", /*french*/ "la #maison bleue de Cocorico#",
              /*spanish*/ "la tienda de pociones de la abuela" },
    });

    hintTable[GRAVEYARD_DAMPES_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Dampé's Hut", /*french*/ "la #Cabane du Fossoyeur#", /*spanish*/ "la cabaña de Dampé" },
    });

    hintTable[GC_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Goron Shop#", /*french*/ "la #Boutique Goron#", /*spanish*/ "la #tienda goron#" },
    });

    hintTable[ZD_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Zora Shop#", /*french*/ "la #Boutique Zora#", /*spanish*/ "la #tienda zora#" },
    });

    hintTable[LLR_TALONS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Talon's House", /*french*/ "la #Maison de Talon#", /*spanish*/ "la casa de Talon" },
    });

    hintTable[LLR_STABLES] = HintText::Region({
        // obscure text
        Text{ "a #stable#", /*french*/ "l'#Étable#", /*spanish*/ "el establo" },
    });

    hintTable[LLR_TOWER] = HintText::Region({
        // obscure text
        Text{ "the #Lon Lon Tower#", /*french*/ "le #silo du Ranch Lon Lon#", /*spanish*/ "la torre Lon Lon" },
    });

    hintTable[MARKET_BAZAAR] = HintText::Region({
        // obscure text
        Text{ "the #Market Bazaar#", /*french*/ "le #Bazar de la Place du Marché#",
              /*spanish*/ "el bazar de la plaza del mercado" },
    });

    hintTable[MARKET_SHOOTING_GALLERY] = HintText::Region({
        // obscure text
        Text{ "a #Slingshot Shooting Gallery#", /*french*/ "le #Jeu d'Adresse de la Place du Marché#",
              /*spanish*/ "el Tiro al Blanco con tirachinas" },
    });

    hintTable[KAK_BAZAAR] = HintText::Region({
        // obscure text
        Text{ "the #Kakariko Bazaar#", /*french*/ "le #Bazar de Cocorico#", /*spanish*/ "el bazar de Kakariko" },
    });

    hintTable[KAK_POTION_SHOP_FRONT] = HintText::Region({
        // obscure text
        Text{ "the #Kakariko Potion Shop#", /*french*/ "l'#apothicaire de Cocorico#",
              /*spanish*/ "la tienda de pociones de Kakariko" },
    });

    hintTable[KAK_POTION_SHOP_BACK] = HintText::Region({
        // obscure text
        Text{ "the #Kakariko Potion Shop#", /*french*/ "l'#apothicaire de Cocorico#",
              /*spanish*/ "la tienda de pociones de Kakariko" },
    });

    hintTable[KAK_SHOOTING_GALLERY] = HintText::Region({
        // obscure text
        Text{ "a #Bow Shooting Gallery#", /*french*/ "le #jeu d'adresse de Cocorico#",
              /*spanish*/ "el Tiro al Blanco con arco" },
    });

    hintTable[COLOSSUS_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[HC_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[OGC_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[DMC_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[DMT_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[ZF_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[GRAVEYARD_SHIELD_GRAVE] = HintText::Region({
        // obscure text
        Text{ "a #grave with a free chest#", /*french*/ "le #tombeau avec un trésor#",
              /*spanish*/ "la #tumba con un cofre#" },
    });

    hintTable[GRAVEYARD_HEART_PIECE_GRAVE] = HintText::Region({
        // obscure text
        Text{ "a chest spawned by #Sun's Song#", /*french*/ "un #coffre apparaît avec le Chant du Soleil#",
              /*spanish*/ "la #tumba de la Canción del Sol#" },
    });

    hintTable[GRAVEYARD_COMPOSERS_GRAVE] = HintText::Region({
        // obscure text
        Text{ "the #Composers' Grave#", /*french*/ "la #Tombe royale#", /*spanish*/ "el #Panteón Real#" },
    });

    hintTable[GRAVEYARD_DAMPES_GRAVE] = HintText::Region({
        // obscure text
        Text{ "Dampé's Grave", /*french*/ "la #Tombe d'Igor#", /*spanish*/ "la #tumba de Dampé#" },
    });

    hintTable[DMT_COW_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a solitary #Cow#", /*french*/ "la #grotte avec une vache#", /*spanish*/ "una #vaca# solitaria" },
    });

    hintTable[HC_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a sandy grotto with #fragile walls#", /*french*/ "la #grotte avec des murs fragiles#",
              /*spanish*/ "la arenosa gruta de #frágiles paredes#" },
    });

    hintTable[HF_TEKTITE_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a pool guarded by a #Tektite#", /*french*/ "l'#étang sous-terrain avec un Araknon#",
              /*spanish*/ "un charco custodiado por un #Tektite#" },
    });

    hintTable[HF_NEAR_KAK_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #Big Skulltula# guarding a Gold one", /*french*/ "la #grotte d'araignées#",
              /*spanish*/ "una #gran Skulltula# custodiando una dorada" },
    });

    hintTable[HF_COW_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a grotto full of #spider webs#", /*french*/ "la #grotte couverte de toiles d'araignées#",
              /*spanish*/ "una gruta llena de #telarañas#" },
    });

    hintTable[KAK_REDEAD_GROTTO] = HintText::Region({
        // obscure text
        Text{ "#ReDeads# guarding a chest", /*french*/ "le tombeau de #deux morts#",
              /*spanish*/ "los #ReDeads# que custodian un cofre" },
    });

    hintTable[SFM_WOLFOS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "#Wolfos# guarding a chest", /*french*/ "la #grotte iridescente#",
              /*spanish*/ "los #Wolfos# que custodian un cofre" },
    });

    hintTable[GV_OCTOROK_GROTTO] = HintText::Region({
        // obscure text
        Text{ "an #Octorok# guarding a rich pool", /*french*/ "un #étang sous-terrain avec un Octorok#",
              /*spanish*/ "un #Octorok# que custodia un lujoso charco" },
    });

    hintTable[DEKU_THEATER] = HintText::Region({
        // obscure text
        Text{ "the #Lost Woods Stage#", /*french*/ "le #théâtre sylvestre#",
              /*spanish*/ "el #escenario del Bosque Perdido#" },
    });

    hintTable[ZR_OPEN_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[DMC_UPPER_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[DMT_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[KAK_OPEN_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[HF_NEAR_MARKET_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[HF_OPEN_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[HF_SOUTHEAST_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[KF_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[LW_NEAR_SHORTCUTS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[HF_INSIDE_FENCE_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #single Upgrade Deku Scrub#", /*french*/ "une #grotte avec une peste Mojo#",
              /*spanish*/ "una cueva con un #solitario mercader deku#" },
    });

    hintTable[LW_SCRUBS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "#2 Deku Scrubs# including an Upgrade one", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[COLOSSUS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "2 Deku Scrubs", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[ZR_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "2 Deku Scrubs", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[SFM_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "2 Deku Scrubs", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[GV_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "2 Deku Scrubs", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[LH_GROTTO] = HintText::Region({
        // obscure text
        Text{ "3 Deku Scrubs", /*french*/ "une #grotte avec trois pestes Mojo#",
              /*spanish*/ "una cueva con #tres mercaderes deku#" },
    });

    hintTable[DMC_HAMMER_GROTTO] = HintText::Region({
        // obscure text
        Text{ "3 Deku Scrubs", /*french*/ "une #grotte avec trois pestes Mojo#",
              /*spanish*/ "una cueva con #tres mercaderes deku#" },
    });

    hintTable[GC_GROTTO] = HintText::Region({
        // obscure text
        Text{ "3 Deku Scrubs", /*french*/ "une #grotte avec trois pestes Mojo#",
              /*spanish*/ "una cueva con #tres mercaderes deku#" },
    });

    hintTable[LLR_GROTTO] = HintText::Region({
        // obscure text
        Text{ "3 Deku Scrubs", /*french*/ "une #grotte avec trois pestes Mojo#",
              /*spanish*/ "una cueva con #tres mercaderes deku#" },
    });

    hintTable[ZR_FAIRY_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    hintTable[HF_FAIRY_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    hintTable[SFM_FAIRY_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    hintTable[ZD_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    hintTable[GF_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    /*--------------------------
    |     DUNGEON HINT TEXT    |
    ---------------------------*/

    hintTable[DEKU_TREE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "an ancient tree", /*french*/ "le vieil arbre", /*spanish*/ "un ancestral árbol" },
        },
        {},
        // clear text
        Text{ "Deku Tree", /*french*/ "l'Arbre Mojo", /*spanish*/ "el Gran Árbol Deku" });

    hintTable[DODONGOS_CAVERN] = HintText::DungeonName(
        {
            // obscure text
            Text{ "an immense cavern", /*french*/ "l'immense caverne", /*spanish*/ "una descomunal cueva" },
        },
        {},
        // clear text
        Text{ "Dodongo's Cavern", /*french*/ "la Caverne Dodongo", /*spanish*/ "la Cueva de los Dodongos" });

    hintTable[JABU_JABUS_BELLY] = HintText::DungeonName(
        {
            // obscure text
            Text{ "the belly of a deity", /*french*/ "le ventre d'un gardien",
                  /*spanish*/ "la tripa de cierta deidad" },
        },
        {},
        // clear text
        Text{ "Jabu-Jabu's Belly", /*french*/ "le Ventre de Jabu-Jabu", /*spanish*/ "tripa de Jabu-Jabu" });

    hintTable[FOREST_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a deep forest", /*french*/ "la profonde forêt", /*spanish*/ "las profundidades del bosque" },
        },
        {},
        // clear text
        Text{ "Forest Temple", /*french*/ "le Temple de la Forêt", /*spanish*/ "el Templo del Bosque" });

    hintTable[FIRE_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a high mountain", /*french*/ "la grande montagne", /*spanish*/ "una alta montaña" },
        },
        {},
        // clear text
        Text{ "Fire Temple", /*french*/ "le Temple du Feu", /*spanish*/ "el Templo del Fuego" });

    hintTable[WATER_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a vast lake", /*french*/ "le vaste lac", /*spanish*/ "un lago inmenso" },
        },
        {},
        // clear text
        Text{ "Water Temple", /*french*/ "le Temple de l'Eau", /*spanish*/ "el Templo del Agua" });

    hintTable[SPIRIT_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "the goddess of the sand", /*french*/ "la déesse des sables", /*spanish*/ "la diosa de las arenas" },
        },
        {},
        // clear text
        Text{ "Spirit Temple", /*french*/ "le Temple de l'Esprit", /*spanish*/ "el Templo del Espíritu" }

    );

    hintTable[SHADOW_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "the house of the dead", /*french*/ "la maison des morts", /*spanish*/ "la casa de la muerte" },
        },
        {},
        // clear text
        Text{ "Shadow Temple", /*french*/ "le Temple de l'Ombre", /*spanish*/ "el Templo de las Sombras" });

    hintTable[ICE_CAVERN] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a frozen maze", /*french*/ "le dédale glacé", /*spanish*/ "un gélido laberinto" },
        },
        {},
        // clear text
        Text{ "Ice Cavern", /*french*/ "la caverne de glace", /*spanish*/ "la caverna de hielo" });

    hintTable[BOTTOM_OF_THE_WELL] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a shadow\'s prison", /*french*/ "la prison d'une ombre", /*spanish*/ "la prisión de las sombras" },
        },
        {},
        // clear text
        Text{ "Bottom of the Well", /*french*/ "le fonds du Puits", /*spanish*/ "el fondo del pozo" });

    hintTable[GERUDO_TRAINING_GROUNDS] = HintText::DungeonName(
        {
            // obscure text
            Text{ "the test of thieves", /*french*/ "l'épreuve des voleurs", /*spanish*/ "la prueba de las bandidas" },
        },
        {},
        // clear text
        Text{ "Gerudo Training Grounds", /*french*/ "le Gymnase Gerudo",
              /*spanish*/ "el Centro de Instrucción Gerudo" });

    hintTable[GANONS_CASTLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a conquered citadel", /*french*/ "la citadelle assiégée", /*spanish*/ "una conquistada ciudadela" },
        },
        {},
        // clear text
        Text{ "Inside Ganon's Castle", /*french*/ "l'intérieur du Château de Ganon",
              /*spanish*/ "el interior del Castillo de Ganon" });

    /*--------------------------
    |      BOSS HINT TEXT      |
    ---------------------------*/

    hintTable[QUEEN_GOHMA] = HintText::Boss({
                       // obscure text
                       Text{"the #Parasitic Armored Arachnid# holds", /*french*/"le #monstre insectoïde géant# possède", /*spanish*/"el #arácnido parasitario acorazado# porta"},
                     }, {},
                       //clear text
                       Text{"#Queen Gohma# holds", /*french*/"la #Reine Gohma# possède", /*spanish*/"la #Reina Goma# porta"});

    hintTable[KING_DODONGO] = HintText::Boss({
                       //obscure text
                       Text{"the #Infernal Dinosaur# holds", /*french*/"le #dinosaure infernal# possède", /*spanish*/"el #dinosaurio infernal# porta"},
                     }, {},
                       //clear text
                       Text{"#King Dodongo# holds", /*french*/"le #Roi Dodongo# possède", /*spanish*/"el #Rey Dodongo# porta"});

    hintTable[BARINADE] = HintText::Boss({
                       //obscure text
                       Text{"the #Bio-Electric Anemone# holds", /*french*/"l'#anémone bioélectrique# possède", /*spanish*/"la #anémona bioeléctrica# porta"},
                     }, {},
                       //clear text
                       Text{"#Barinade# holds", /*french*/"#Barinade# possède", /*spanish*/"#Barinade# porta"});

    hintTable[PHANTOM_GANON] = HintText::Boss({
                       //obscure text
                       Text{"the #Evil Spirit from Beyond# holds", /*french*/"l'#esprit maléfique de l'au-delà# possède", /*spanish*/"el #espíritu maligno de ultratumba# porta"},
                     }, {},
                       //clear text
                       Text{"#Phantom Ganon# holds", /*french*/"#Ganon Spectral# possède", /*spanish*/"#Ganon Fantasma# porta"});

    hintTable[VOLVAGIA] = HintText::Boss({
                       //obscure text
                       Text{"the #Subterranean Lava Dragon# holds", /*french*/"le #dragon des profondeurs# possède", /*spanish*/"el #dragón de lava subterráneo# porta"},
                     }, {},
                       //clear text
                       Text{"#Volvagia# holds", /*french*/"#Volvagia# possède", /*spanish*/"#Volvagia# porta"});

    hintTable[MORPHA] = HintText::Boss({
                       //obscure text
                       Text{"the #Giant Aquatic Amoeba# holds", /*french*/"l'#amibe aquatique géante# possède", /*spanish*/"la #ameba acuática gigante# porta"},
                     }, {},
                       //clear text
                       Text{"#Morpha# holds", /*french*/"#Morpha# possède", /*spanish*/"#Morpha# porta"});

    hintTable[BONGO_BONGO] = HintText::Boss({
                       //obscure text
                       Text{"the #Phantom Shadow Beast# holds", /*french*/"le #monstre de l'ombre# possède", /*spanish*/"la #alimaña oscura espectral# porta"},
                     }, {},
                       //clear text
                       Text{"#Bongo Bongo# holds", /*french*/"#Bongo Bongo# possède", /*spanish*/"#Bongo Bongo# porta"});

    hintTable[TWINROVA] = HintText::Boss({
                       //obscure text
                       Text{"the #Sorceress Sisters# hold", /*french*/"#les sorcières jumelles# possède", /*spanish*/"las #hermanas hechiceras# portan"},
                     }, {},
                       //clear text
                       Text{"#Twinrova# holds", /*french*/"#Twinrova# possède", /*spanish*/"#Birova# porta"});
    //
    // [LINKS_POCKET_BOSS] = HintText::Boss({
    //                    //obscure text
    //                    Text{"#@'s pocket# rewards", /*french*/"#@ débute avec#", /*spanish*/"el #bolsillo de @#
    //                    premia con"},
    //                  },
    //                    //clear text
    //                           Text{"#@ already has#", /*french*/"#@ a déjà#", /*spanish*/"el #bolsillo de @ ya
    //                           tiene#"}
    // );

    /*--------------------------
    |     BRIDGE HINT TEXT     |
    ---------------------------*/

    hintTable[BRIDGE_OPEN_HINT] = HintText::Bridge({
        // obscure text
        Text{ "The awakened ones have already&created a bridge to the castle&where the evil dwells.",
              /*french*/ "Les êtres de sagesse ont&déjà créé un pont vers&le repaire du mal.",
              /*spanish*/ "Los sabios #ya habrán creado un puente#&al castillo, de donde emana el mal." },
    });

    hintTable[BRIDGE_VANILLA_HINT] = HintText::Bridge({
        // obscure text
        Text{ "The awakened ones require&the Shadow and Spirit Medallions&as well as the Light Arrows.",
              /*french*/
              "Les êtres de sagesse attendront&le héros muni des %rMédaillons de&l'Ombre et l'Esprit%w et des&%yFlèches de Lumière%w.",
              /*spanish*/
              "Los sabios aguardarán a que el héroe&obtenga tanto el #Medallón de las&Sombras y el del Espíritu# junto "
              "a la #flecha de luz#." },
    });

    hintTable[BRIDGE_STONES_HINT] = HintText::Bridge({
        // obscure text                                                  singular        plural
        Text{
            "The awakened ones will&await for the Hero to collect&%d |Spiritual Stone|Spiritual Stones|.",
            /*french*/ "Les êtres de sagesse attendront&le héros muni de %r%d |Pierre&Ancestrale|Pierres&Ancestrales|%w.",
            /*spanish*/ "Los sabios aguardarán a que el héroe&obtenga #%d |piedra espiritual|piedras espirituales|#." },
    });

    hintTable[BRIDGE_MEDALLIONS_HINT] = HintText::Bridge({
        // obscure text                                                  singular  plural
        Text{ "The awakened ones will await&for the Hero to collect&%d |Medallion|Medallions|.",
              /*french*/ "Les êtres de sagesse attendront&le héros muni de %r#%d |Médaillon|Médaillons|%w.",
              /*spanish*/ "Los sabios aguardarán a que el héroe&obtenga #%d |medallón|medallones|#." },
    });

    hintTable[BRIDGE_REWARDS_HINT] = HintText::Bridge({
        // obscure text                                                  singular                     plural
        Text{ "The awakened ones will await&for the Hero to collect&%d |Spiritual Stone or Medallion|Spiritual Stones "
              "and Medallions|.",
              /*french*/
              "Les êtres de sagesse attendront&le héros muni de %r%d |Pierre&Ancestrale ou Médaillon|Pierres&Ancestrales ou Médaillons|%w.",
              /*spanish*/
              "Los sabios aguardarán a que el héroe&obtenga #%d |piedra espiritual o medallón|piedras espirtuales y "
              "medallones|#." },
    });

    hintTable[BRIDGE_DUNGEONS_HINT] = HintText::Bridge({
        // obscure text                                                  singular plural
        Text{ "The awakened ones will await&for the Hero to conquer&%d |Dungeon|Dungeons|.",
              /*french*/ "Les êtres de sagesse attendront&la conquête de %r%d |Donjon|Donjons|%w.",
              /*spanish*/ "Los sabios aguardarán a que el héroe& complete #%d |mazmorra|mazmorras|#." },
    });

    hintTable[BRIDGE_TOKENS_HINT] = HintText::Bridge({
        // obscure text
        Text{ "The awakened ones will await&for the Hero to collect&%d |Gold Skulltula Token|Gold Skulltula Tokens|.",
              /*french*/ "Les êtres de sagesse attendront&le héros muni de %r%d |Symbole|Symboles| &de Skulltula d'or%w.",
              /*spanish*/ "Los sabios aguardarán a que el héroe&obtenga #%d |símbolo|símbolos| de&skulltula dorada#." },
    });

    hintTable[BRIDGE_GREG_HINT] = HintText::Bridge({
        // obscure text
        Text{ "The awakened ones will await&for the Hero to find %gGreg%w.",
              /*french*/ "The awakened ones will await&for the Hero to find %gGreg%w.",
              /*spanish*/ "The awakened ones will await&for the Hero to find %gGreg%w." },
    });

    /*--------------------------
    | GANON BOSS KEY HINT TEXT |
    ---------------------------*/

    hintTable[GANON_BK_START_WITH_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the %revil one%w's key will&be given %rfrom the start%w.",
              /*french*/ "Aussi, la %rclé du Malin%w sera&possession %rmême du héros%w.",
              /*spanish*/ "Y obtendrás la llave del #señor del mal# desde el #inicio#." },
    });

    hintTable[GANON_BK_VANILLA_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the %revil one%w's key will&be kept in a big chest&%rinside its tower%w.",
              /*french*/ "Aussi, la %rclé du Malin%w sera&encoffrée %rdans sa tour%w.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará en un gran cofre de #su torre#." },
    });

    hintTable[GANON_BK_OWN_DUNGEON_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the %revil one%w's key will&be hidden somewhere %rinside&its castle%w.",
              /*french*/ "Aussi, la %rclé du Malin%w sera&cachée %rdans son vaste château%w.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará en #algún lugar de su castillo#." },
    });

    hintTable[GANON_BK_OVERWORLD_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the %revil one%w's key will&be hidden %routside of&dungeons%w in Hyrule.",
              /*french*/ "Aussi, la %rclé du Malin%w se&trouve %rhors des donjons%w d'Hyrule.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará #fuera de las mazmorras# de Hyrule." },
    });

    hintTable[GANON_BK_ANY_DUNGEON_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the %revil one%w's key will&be hidden %rinside a&dungeon%w in Hyrule.",
              /*french*/ "Aussi, la %rclé du Malin%w se&trouve %rdans un donjon%w d'Hyrule.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará #en una mazmorra# de Hyrule." },
    });

    hintTable[GANON_BK_ANYWHERE_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the %revil one%w's key will&be hidden somewhere&%rin Hyrule%w.",
              /*french*/ "Aussi, la %rclé du Malin%w se&trouve quelque part %rdans Hyrule%w.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará en #cualquier lugar de Hyrule#." },
    });

    hintTable[GANON_BK_TRIFORCE_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the %revil one%w's key will&be given to the Hero once&the %rTriforce%w is completed.",
              /*french*/ "Aussi, la %rclé du Malin%w se&révèlera une fois la %rTriforce%w&assemblée.",
              /*spanish*/ "Y el héroe recibirá la llave del #señor del mal# cuando haya completado la #Trifuerza#." },
    });

    hintTable[GANON_BK_SKULLTULA_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text { "And the %revil one%w's key will be&provided by the cursed rich man&once %r100 Gold Skulltula Tokens%w&are retrieved.",
              /*french*/ "Aussi, la %rclé du Malin%w sera&donnée par l'homme maudit une&fois que %r100 Symboles de&Skulltula d'or%w auront été trouvés.",
              /*spanish*/ "Y el rico maldito entregará la llave&del #señor de mal# tras obtener&100 símbolos de skulltula dorada#."},
    });

    /*--------------------------
    |      LACS HINT TEXT      |
    ---------------------------*/

    hintTable[LACS_VANILLA_HINT] = HintText::LACS({
        // obscure text
        Text{
            "And the %revil one%w's key will be&provided by Zelda once the&%rShadow and Spirit Medallions%w&are retrieved.",
            /*french*/ "Aussi, la %rclé du Malin%w sera&fournie par Zelda une fois que &les %rMédaillons de l'Ombre et de&l'Esprit%w seront récupérés.",
            /*spanish*/
            "Y Zelda entregará la llave&del #señor del mal# tras obtener&#el medallón de las sombras y del "
            "espíritu#." },
    });

    hintTable[LACS_MEDALLIONS_HINT] = HintText::LACS({
        // obscure text                                                     singular      plural
        Text{ "And the %revil one%w's key will be&provided by Zelda once %r%d&|Medallion%w is|Medallions%w are| retrieved.",
              /*french*/ "Aussi, la %rclé du Malin%w sera&fournie par Zelda une fois |qu'&%r%d Médaillon%w aura été&récupéré|que&%r%d Médaillons%w auront été&récupérés|.",
              /*spanish*/ "Y Zelda entregará la llave&del #señor del mal# tras obtener&#%d |medallón|medallones|#." },
    });

    hintTable[LACS_STONES_HINT] = HintText::LACS({
        // obscure text                                                     singular            plural
        Text{
            "And the %revil one%w's key will be&provided by Zelda once %r%d&|Spiritual Stone%w is|Spiritual Stones%w are| "
            "retrieved.",
            /*french*/ "Aussi, la %rclé du Malin%w sera&fournie par Zelda une fois |qu'&%r%d Pierre Ancestrale%w aura été&récupérée|que&%r%d Pierres Ancestrales%w auront été&récupérées|.",
            /*spanish*/
            "Y Zelda entregará la llave&del #señor del mal# tras obtener&#%d |piedra espiritual|piedras "
            "espirituales|#." },
    });

    hintTable[LACS_REWARDS_HINT] = HintText::LACS({
        // obscure text                                                     singular                         plural
        Text{ "And the %revil one%w's key will be&provided by Zelda once %r%d&|Spiritual Stone or Medallion%w&"
              "is|Spiritual Stones and Medallions%w&are| retrieved.",
              /*french*/
              "Aussi, la %rclé du Malin%w sera&fournie par Zelda une fois |qu'&%r%d Pierre Ancestrale ou %d&Médaillon%w sera récupéré|que&%r%d Pierres Ancestrales et&Médaillons%w seront récupérés|.",
              /*spanish*/
              "Y Zelda entregará la llave&del #señor del mal# tras obtener&#%d |piedra espiritual o medallón|piedras "
              "espirituales o medallones|#." },
    });

    hintTable[LACS_DUNGEONS_HINT] = HintText::LACS({
        // obscure text                                                     singular    plural
        Text{ "And the %revil one%w's key will be&provided by Zelda once %r%d&|Dungeon%w is|Dungeons%w are| conquered.",
              /*french*/ "Aussi, la %rclé du Malin%w sera&fournie par Zelda une fois |qu'&%r%d donjon %wsera conquis|que&%r%d donjons%w seront conquis|.",
              /*spanish*/ "Y Zelda entregará la llave&del #señor del mal# tras completar&#%d |mazmorra|mazmorras|#." },
    });

    hintTable[LACS_TOKENS_HINT] = HintText::LACS({
        // obscure text                                                     singular                 plural
        Text{ "And the %revil one%w's key will be&provided by Zelda once %r%d |Gold&Skulltula Token%w is|Gold&Skulltula "
              "Tokens%w are| retrieved.",
              /*french*/ "Aussi, la %rclé du Malin%w sera&fournie par Zelda une fois |qu'&%r%d symbole de Skulltula d'or&%wsera récupuéré|que &%r%d symboles de Skulltula d'or&%wseront recupérés|.",
              /*spanish*/
              "Y Zelda entregará la llave&del #señor del mal# tras obtener&#%d |símbolo|símbolos| de&skulltula "
              "dorada#." },
    });
    /*--------------------------
    |     TRIAL HINT TEXT      |
    ---------------------------*/

    hintTable[SIX_TRIALS] = HintText::Exclude({
        // obscure text
        Text{ "#Ganon's Tower# is protected by a powerful barrier.",
              /*french*/ "#la Tour de Ganon# est protégée par une puissante barrière",
              /*spanish*/ "la #torre de Ganon# está protegida por una poderosa barrera" },
    });

    hintTable[ZERO_TRIALS] = HintText::Exclude({
        // obscure text
        Text{ "Sheik dispelled the barrier around #Ganon's Tower#.",
              /*french*/ "Sheik a dissipé la barrière autour de #la Tour de Ganon#",
              /*spanish*/ "Sheik disipó la barrera alrededor de la #torre de Ganon#." },
    });

    hintTable[FOUR_TO_FIVE_TRIALS] = HintText::Exclude({
        // obscure text
        Text{ " was dispelled by Sheik.", /*french*/ " a été dissipée par Sheik.",
              /*spanish*/ " se disipó gracias a Sheik." },
    });

    hintTable[ONE_TO_THREE_TRIALS] = HintText::Exclude({
        // obscure text
        Text{ " protects Ganons Tower.", /*french*/ " protège la Tour de Ganon.",
              /*spanish*/ " protege la torre de Ganon" },
    });

    /*--------------------------
    |        ALTAR TEXT        |
    ---------------------------*/

    hintTable[SPIRITUAL_STONE_TEXT_START] = HintText::Altar({
        // obscure text
        Text{ "3 Spiritual Stones found in Hyrule...",
              /*french*/ "Les trois Pierres Ancestrales cachées&dans Hyrule...",
              /*spanish*/ "Tres piedras espirituales halladas por Hyrule..." },
    });

    hintTable[CHILD_ALTAR_TEXT_END_DOTOPEN] = HintText::Altar({
        // obscure text
        Text{ "Ye who may become a Hero...&The path to the future is open...",
              /*french*/ "À celui qui a quête de devenir&héros...&Le futur vous accueille béant...",
              /*spanish*/ "Para aquel que se convierta en el héroe...&La puerta al futuro está a su disposición..." },
    });

    hintTable[CHILD_ALTAR_TEXT_END_DOTSONGONLY] = HintText::Altar({
        // obscure text
        Text{
            "Ye who may become a Hero...&Stand with the Ocarina and&play the Song of Time.",
            /*french*/ "À celui qui a quête de devenir&héros...&Portez l'Ocarina et jouez&le chant du temps.",
            /*spanish*/ "Para aquel que se convierta en el héroe...&Tome la ocarina y&entone la Canción del Tiempo." },
    });

    hintTable[CHILD_ALTAR_TEXT_END_DOTCLOSED] = HintText::Altar({
        // obscure text
        Text{ "Ye who may become a Hero...&Offer the spiritual stones and&play the Song of Time.",
              /*french*/
              "À celui qui a quête de devenir&héros... Présentez les Pierres&Ancestrales et jouez&le chant du temps.",
              /*spanish*/
              "Para aquel que se convierta en el héroe...&Tome las piedras espirituales y&entone la Canción del "
              "Tiempo." },
    });

    hintTable[ADULT_ALTAR_TEXT_START] = HintText::Altar({
        // obscure text
        Text{ "An awakening voice from the Sacred&Realm will call those destined to be&Sages, who dwell in the five "
              "temples.",
              /*french*/
              "Quand le mal aura triomphé, une voix&du Saint Royaume appellera ceux&cachés dans les cinq temples, "
              "destinés^à être Sages.",
              /*spanish*/
              "Cuando el mal lo impregne todo, desde el Reino Sagrado surgirá una voz que hará despertar a los sabios "
              "que moran en los #cinco templos#." },
    });

    hintTable[ADULT_ALTAR_TEXT_END] = HintText::Altar({
        // obscure text
        Text{
            "$kTogether with the Hero of Time,&the awakened ones will return&the light of peace to the world...",
            /*french*/
            "$kEnsemble avec le Héros du &Temps, ces Sages emprisonneront&le mal et réinstaureront la&lumière de paix "
            "dans le monde...",
            /*spanish*/
            "Con el Héroe del Tiempo, aquellos&que despierten detendrán el mal y&volverán al mundo de luz la paz..." },
    });

    /*--------------------------
    |   VALIDATION LINE TEXT   |
    ---------------------------*/

    hintTable[VALIDATION_LINE] = HintText::Validation({
        // obscure text
        Text{ "Hmph... Since you made it this far, I'll let you know what glorious prize of Ganon's you likely missed "
              "out on in my tower.^Behold...^",
              /*french*/
              "Pah! Puisque tu t'es rendu ici, je te dirai quel trésor tu as manqué dans ma tour.^Et c'est...^",
              /*spanish*/
              "Mmm... Ya que has llegado hasta aquí, te diré qué preciado objeto de mi propiedad puedes haberte dejado "
              "en mi torre.^He aquí...^" },
    });

    /*--------------------------
    | LIGHT ARROW LOCATION TEXT|
    ---------------------------*/

    hintTable[LIGHT_ARROW_LOCATION_HINT] = HintText::LightArrow({
        // obscure text
        Text{
            "Ha ha ha... You'll never beat me by reflecting my lightning bolts and unleashing the arrows from ",
            /*french*/
            "Ha ha ha... Pauvre fou! Tu ne pourras jamais me vaincre sans les flèches que j'ai cachées dans ",
            /*spanish*/
            "Ja, ja, ja... Nunca me derrotarás reflejando mis esferas de energía y desplegando la flecha de luz de " },
    });

    hintTable[YOUR_POCKET] = HintText::Exclude({
        // obscure text
        Text{ "your pocket", /*french*/ "tes poches", /*spanish*/ "tu bolsillo" },
    });

    /*--------------------------
    |      GANON LINE TEXT     |
    ---------------------------*/

    hintTable[GANON_LINE01] = HintText::GanonLine({
        // obscure text
        Text{ "Oh! It's @.&I was expecting someone called Sheik.&Do you know what happened to them?",
              /*french*/ "Ah, c'est @.&J'attendais un certain Sheik.&Tu sais ce qui lui est arrivé?",
              /*spanish*/
              "¡Oh! Pero si es @.&Estaba esperando a alguien llamado Sheik. ¿Sabes qué puede haberle pasado?" },
    });

    hintTable[GANON_LINE02] = HintText::GanonLine({
        // obscure text
        Text{ "I knew I shouldn't have put the key on the other side of my door.",
              /*french*/ "J'aurais dû garder la clé ici. Hélas...",
              /*spanish*/ "Sabía que no tendría que haber dejado la llave al otro lado de la puerta." },
    });

    hintTable[GANON_LINE03] = HintText::GanonLine({
        // obscure text
        Text{ "Looks like it's time for a round of tennis.",
              /*french*/ "C'est l'heure de jouer au tennis.",
              /*spanish*/ "Parece que es hora de una pachanga de tenis." },
    });

    hintTable[GANON_LINE04] = HintText::GanonLine({
        // obscure text
        Text{ "You'll never deflect my bolts of energy with your sword, then shoot me with those Light Arrows you "
              "happen to have.",
              /*french*/
              "Ne perds pas ton temps à frapper mes éclairs d'énergie avec ton épée et me tirer avec tes flèches de "
              "Lumière!",
              /*spanish*/
              "Nunca reflejarás mis esferas de energía con tu espada, para después dispararme con las flechas de luz "
              "que tendrás." },
    });

    hintTable[GANON_LINE05] = HintText::GanonLine({
        // obscure text
        Text{ "Why did I leave my trident back in the desert?",
              /*french*/ "Sale bêtise... Et j'ai oublié mon trident dans le désert!",
              /*spanish*/ "Santa Hylia... ¿Por qué me habré dejado el tridente en el desierto?" },
    });

    hintTable[GANON_LINE06] = HintText::GanonLine({
        // obscure text
        Text{ "Zelda is probably going to do something stupid, like send you back to your own timeline.^So this is "
              "quite meaningless. Do you really want to save this moron?",
              /*french*/
              "Même si je suis vaincu... Zelda te renverra dans ton ère, et je reviendrai conquérir!^Telle est la "
              "prophécie d'Hyrule Historia!",
              /*spanish*/
              "Seguro que Zelda trata de hacer alguna tontería, como enviarte de vuelta a tu línea temporal.^No tiene "
              "ningún sentido alguno. ¿De verdad quieres salvar a esa tonta?" },
    });

    hintTable[GANON_LINE07] = HintText::GanonLine({
        // obscure text
        Text{ "What about Zelda makes you think&she'd be a better ruler than I?^I saved Lon Lon Ranch,&fed the "
              "hungry,&and my castle floats.",
              /*french*/
              "Zelda ne sera jamais un meilleur monarque que moi!^J'ai un château volant, mes sujets sont des belles "
              "amazones... et mes Moblins sont clairement plus puissants que jamais!",
              /*spanish*/
              "¿Qué te hace pensar que Zelda gobierna mejor que yo?^Yo he salvado el Rancho Lon Lon,&he alimentado a "
              "los hambrientos&y hasta hago que mi castillo flote." },
    });

    hintTable[GANON_LINE08] = HintText::GanonLine({
        // obscure text
        Text{ "I've learned this spell,&it's really neat,&I'll keep it later&for your treat!",
              /*french*/
              "Gamin, ton destin achève,&sous mon sort tu périras!&Cette partie ne fut pas brève,&et cette mort, tu "
              "subiras!",
              /*spanish*/
              "Veamos ahora que harás,&la batalla ha de comenzar,&te enviaré de una vez al más allá,&¿listo para "
              "afrontar la verdad?" },
    });

    hintTable[GANON_LINE09] = HintText::GanonLine({
        // obscure text
        Text{ "Many tricks are up my sleeve,&to save yourself&you'd better leave!",
              /*french*/ "Sale petit garnement,&tu fais erreur!&C'est maintenant que marque&ta dernière heure!",
              /*spanish*/ "¿No osarás a mí enfrentarte?&Rimas aparte,&¡voy a matarte!" },
    });

    hintTable[GANON_LINE10] = HintText::GanonLine({
        // obscure text
        Text{ "After what you did to Koholint Island, how can you call me the bad guy?",
              /*french*/ "J'admire ce que tu as fait à l'Île Koholint... Toi et moi, nous devrions faire équipe!",
              /*spanish*/ "Después de lo que le hiciste a la Isla Koholint, ¿cómo te atreves a llamarme malvado?" },
    });

    hintTable[GANON_LINE11] = HintText::GanonLine({
        // obscure text
        Text{ "Today, let's begin down&'The Hero is Defeated' timeline.",
              /*french*/
              "Si tu me vaincs, Hyrule sera englouti... mais si tu meurs, on aura A Link to the Past, le meilleur opus "
              "de la série!",
              /*spanish*/
              "Hoy daremos lugar a la línea temporal del Héroe Derrotado.&¡Prepárate para el culmen de esta saga!" },
    });

    /*--------------------------
    |      MERCHANTS' ITEMS     |
    ---------------------------*/

    hintTable[MEDIGORON_DIALOG_FIRST] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "How about buying ",
              /*french*/ "Veux-tu acheter ",
              /*spanish*/ "¿Me compras " },
    });

    hintTable[MEDIGORON_DIALOG_SECOND] = HintText::MerchantsDialogs({
        // obscure text
        Text{ " for #200 Rupees#?&" + TWO_WAY_CHOICE() + "#Buy&Don't buy#",
              /*french*/ " pour #200 rubis#?&" + TWO_WAY_CHOICE() + "#Acheter&Ne pas acheter#",
              /*spanish*/ " por #200 rupias#?&" + TWO_WAY_CHOICE() + "#Comprar&No comprar#" },
    });

    hintTable[CARPET_SALESMAN_DIALOG_FIRST] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "Welcome!^I am selling stuff, strange and rare, from&all over the world to everybody. Today's&special "
              "is...^",
              /*french*/
              "Bienvenue!^Je vends des objets rares et merveilleux du&monde entier. En spécial aujourd'hui...^",
              /*spanish*/
              "¡Acércate!^Vendo productos extraños y difíciles de&encontrar... De todo el mundo a todo el&mundo. La "
              "oferta de hoy es...^¡" },
    });

    hintTable[CARPET_SALESMAN_DIALOG_SECOND] = HintText::MerchantsDialogs({
        // obscure text
        Text{
            "! Terrifying!&I won't tell you what it is until I see the&money...^How about #200 Rupees#?&&" +
                TWO_WAY_CHOICE() + "#Buy&Don't buy#",
            /*french*/
                "! Un&concentré de puissance! Mais montre tes&rubis avant que je te dise ce que c'est...^Disons #200 "
                "rubis#?&&" +
                TWO_WAY_CHOICE() + "#Acheter&Ne pas acheter#",
            /*spanish*/
                "! ¡Terrorífico!&No te revelaré su nombre hasta que&vea el dinero...^#200 rupias#, ¿qué te parece?&&" +
                TWO_WAY_CHOICE() + "#Comprar&No comprar#" },
    });

    hintTable[CARPET_SALESMAN_DIALOG_THIRD] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "Thank you very much!^What I'm selling is... #",
              /*french*/ "Merci beaucoup!^Cet objet extraordinaire est... #",
              /*spanish*/ "¡Muchas gracias!^Lo que vendo es... #¡" },
    });

    hintTable[CARPET_SALESMAN_DIALOG_FOURTH] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "!#^The mark that will lead you to the #Spirit&Temple# is the #flag on the " + IF_NOT_MQ() + "left" +
                  MQ_ELSE() + "right" + MQ_END() + "# outside the shop. Be seeing you!",
              /*french*/ "!#^La marque qui te mènera au #Temple de l'Esprit# est le #drapeau " + IF_NOT_MQ() +
                  "gauche" + MQ_ELSE() + "droite" + MQ_END() + "# en sortant d'ici. À la prochaine!",
              /*spanish*/ "!#^La marca que te guiará al #Templo del&Espíritu# es la #bandera que está a la&" +
                  IF_NOT_MQ() + "izquierda" + MQ_ELSE() + "derecha" + MQ_END() + "# al salir de aquí. ¡Nos vemos!" },
    });

    hintTable[GRANNY_DIALOG] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "! How about #100 Rupees#?&" + TWO_WAY_CHOICE() + "#Buy&Don't buy#",
              /*french*/ "! Que dis-tu de #100 rubis#?&" + TWO_WAY_CHOICE() + "#Acheter&Ne pas acheter#",
              /*spanish*/ ". Vendo por #100 rupias#.&" + TWO_WAY_CHOICE() + "#Comprar&No comprar#" },
    });
}

int32_t StonesRequiredBySettings() {
    int32_t stones = 0;
    if (Settings::Bridge.Is(RAINBOWBRIDGE_STONES)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::BridgeStoneCount.Value<uint8_t>() });
    }
    if (Settings::Bridge.Is(RAINBOWBRIDGE_REWARDS)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::BridgeRewardCount.Value<uint8_t>() - 6 });
    }
    if ((Settings::Bridge.Is(RAINBOWBRIDGE_DUNGEONS)) && (Settings::ShuffleRewards.Is(REWARDSHUFFLE_END_OF_DUNGEON))) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::BridgeDungeonCount.Value<uint8_t>() - 6 });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_STONES)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::LACSStoneCount.Value<uint8_t>() });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_REWARDS)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::LACSRewardCount.Value<uint8_t>() - 6 });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_DUNGEONS)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::LACSDungeonCount.Value<uint8_t>() - 6 });
    }
    return stones;
}

int32_t MedallionsRequiredBySettings() {
    int32_t medallions = 0;
    if (Settings::Bridge.Is(RAINBOWBRIDGE_MEDALLIONS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::BridgeMedallionCount.Value<uint8_t>() });
    }
    if (Settings::Bridge.Is(RAINBOWBRIDGE_REWARDS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::BridgeRewardCount.Value<uint8_t>() - 3 });
    }
    if ((Settings::Bridge.Is(RAINBOWBRIDGE_DUNGEONS)) && (Settings::ShuffleRewards.Is(REWARDSHUFFLE_END_OF_DUNGEON))) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::BridgeDungeonCount.Value<uint8_t>() - 3 });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_MEDALLIONS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::LACSMedallionCount.Value<uint8_t>() });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_REWARDS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::LACSRewardCount.Value<uint8_t>() - 3 });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_DUNGEONS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::LACSDungeonCount.Value<uint8_t>() - 3 });
    }
    return medallions;
}

int32_t TokensRequiredBySettings() {
    int32_t tokens = 0;
    if (Settings::Bridge.Is(RAINBOWBRIDGE_TOKENS)) {
        tokens = std::max<int32_t>({ tokens, (int32_t)Settings::BridgeTokenCount.Value<uint8_t>() });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_TOKENS)) {
        tokens = std::max<int32_t>({ tokens, (int32_t)Settings::LACSTokenCount.Value<uint8_t>() });
    }
    return tokens;
}

std::array<ConditionalAlwaysHint, 9> conditionalAlwaysHints = {
    std::make_pair(MARKET_10_BIG_POES,
                   []() {
                       return Settings::BigPoeTargetCount.Value<uint8_t>() >= 3;
                   }), // Remember, the option's value being 3 means 4 are required
    std::make_pair(DEKU_THEATER_MASK_OF_TRUTH, []() { return !Settings::CompleteMaskQuest; }),
    std::make_pair(SONG_FROM_OCARINA_OF_TIME, []() { return StonesRequiredBySettings() < 2; }),
    std::make_pair(HF_OCARINA_OF_TIME_ITEM, []() { return StonesRequiredBySettings() < 2; }),
    std::make_pair(SHEIK_IN_KAKARIKO, []() { return MedallionsRequiredBySettings() < 5; }),
    std::make_pair(DMT_TRADE_CLAIM_CHECK, []() { return false; }),
    std::make_pair(KAK_30_GOLD_SKULLTULA_REWARD, []() { return TokensRequiredBySettings() < 30; }),
    std::make_pair(KAK_40_GOLD_SKULLTULA_REWARD, []() { return TokensRequiredBySettings() < 40; }),
    std::make_pair(KAK_50_GOLD_SKULLTULA_REWARD, []() { return TokensRequiredBySettings() < 50; })
};

const HintText& Hint(const uint32_t hintKey) {
    return hintTable[hintKey];
}

std::vector<HintText> GetHintCategory(HintCategory category) {

    std::vector<HintText> hintsInCategory = {};

    for (const auto& hint : hintTable) {
        if (hint.GetType() == category) {
            hintsInCategory.push_back(hint);
        }
    }
    return hintsInCategory;
}
