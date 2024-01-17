#include "hint_list.hpp"
#include "custom_messages.hpp"

#include "../randomizerTypes.h"
#include "../context.h"

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

std::array<HintText, RHT_MAX> hintTable;

void HintTable_Init() {
    /*--------------------------
    |       GENERAL TEXT       |
    ---------------------------*/
    hintTable[RHT_NONE] = HintText::Exclude({ Text{ "No Hint", "Pas d'Indice", "" } });
    hintTable[RHT_PREFIX] =
        HintText::Exclude({ Text{ "They say that ", /*french*/ "Selon moi, ", /*spanish*/ "Según dicen, " } });
    hintTable[RHT_WAY_OF_THE_HERO] =
        HintText::Exclude({ Text{ " is on #the way of the hero#.", /*french*/ " est sur #la voie du héros#.",
                                  /*spanish*/ " conduce a la senda del héroe." } });
    hintTable[RHT_PLUNDERING] =
        HintText::Exclude({ Text{ "plundering ", /*french*/ "explorer ", /*spanish*/ "inspeccionar " } });
    hintTable[RHT_FOOLISH] = HintText::Exclude(
        { Text{ " is #a foolish choice#.", /*french*/ " est #futile#.", /*spanish*/ " #no es una sabia decisión#." } });
    hintTable[RHT_CAN_BE_FOUND_AT] =
        HintText::Exclude({ Text{ "can be found at", /*french*/ "se trouve dans", /*spanish*/ "aguarda en" } });
    hintTable[RHT_HOARDS] = HintText::Exclude({ Text{ "hoards", /*french*/ "recèle", /*spanish*/ "acapara" } });

    HintTable_Init_Item();
    HintTable_Init_Exclude_Overworld();
    HintTable_Init_Exclude_Dungeon();


    /*--------------------------
    |    SOMETIMES HINT TEXT   |
    ---------------------------*/

    hintTable[RHT_KF_LINKS_HOUSE_COW] = HintText::Sometimes(
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

    hintTable[RHT_KAK_100_GOLD_SKULLTULA_REWARD] = HintText::Sometimes(
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

    hintTable[RHT_SONG_FROM_OCARINA_OF_TIME] = HintText::Sometimes({
        // obscure text
        Text{ "the #Ocarina of Time# teaches", /*french*/ "l'#Ocarina du Temps# est accompagné par",
              /*spanish*/ "la #Ocarina del Tiempo# enseña" },
    });

    hintTable[RHT_SONG_FROM_ROYAL_FAMILYS_TOMB] = HintText::Sometimes({
        // obscure text
        Text{ "#ReDead in the Composers' Grave# guard",
              /*french*/ "les #Éffrois du tombeau des compositeurs# protègent",
              /*spanish*/ "los #ReDeads del Panteón Real# guardan" },
        Text{ "the #Composer Brothers wrote#", /*french*/ "le #trésor des compositeurs# est",
              /*spanish*/ "los #hermanos compositores escribieron#" },
    });

    hintTable[RHT_SHEIK_IN_FOREST] = HintText::Sometimes({
        // obscure text
        Text{ "#in a meadow# Sheik teaches", /*french*/ "Sheik confiera, #dans un bosquet#,",
              /*spanish*/ "#en la pradera sagrada# Sheik enseña" },
    });

    hintTable[RHT_SHEIK_AT_TEMPLE] = HintText::Sometimes({
        // obscure text
        Text{ "Sheik waits at a #monument to time# to teach",
              /*french*/ "Sheik confiera, #au pied de l'épée légendaire#,",
              /*spanish*/ "Sheik espera en el #momumento del tiempo# para enseñar" },
    });

    hintTable[RHT_SHEIK_IN_CRATER] = HintText::Sometimes({
        // obscure text
        Text{ "the #crater's melody# is", /*french*/ "Sheik confiera, #entouré de lave#,",
              /*spanish*/ "la #melodía del cráter# otorga" },
    });

    hintTable[RHT_SHEIK_IN_ICE_CAVERN] = HintText::Sometimes({
        // obscure text
        Text{ "the #frozen cavern# echoes with", /*french*/ "Sheik confiera, #dans une caverne enneigée#,",
              /*spanish*/ "en la #caverna de hielo# retumban los ecos de" },
    });

    hintTable[RHT_SHEIK_IN_KAKARIKO] = HintText::Sometimes({
        // obscure text
        Text{ "a #ravaged village# mourns with", /*french*/ "Sheik confirera, #au coeur d'un village ravagé#,",
              /*spanish*/ "un #arrasado pueblo# llora" },
    });

    hintTable[RHT_SHEIK_AT_COLOSSUS] = HintText::Sometimes({
        // obscure text
        Text{ "a hero ventures #beyond the wasteland# to learn",
              /*french*/ "Sheik confiera, #au bout d'un chemin sableux#,",
              /*spanish*/ "el héroe que se adentre #más allá del desierto# aprenderá" },
    });

    hintTable[RHT_MARKET_10_BIG_POES] = HintText::Sometimes(
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

    hintTable[RHT_DEKU_THEATER_SKULL_MASK] = HintText::Sometimes({
        // obscure text
        Text{ "the #Skull Mask# yields", /*french*/ "le #Masque de Mort# donne",
              /*spanish*/ "la #máscara de calavera# otorga" },
    });

    hintTable[RHT_DEKU_THEATER_MASK_OF_TRUTH] = HintText::Sometimes(
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

    hintTable[RHT_HF_OCARINA_OF_TIME_ITEM] = HintText::Sometimes({
        // obscure text
        Text{ "the #treasure thrown by Princess Zelda# is", /*french*/ "le trésor #laissé par la princesse# est",
              /*spanish*/ "el #tesoro arrojado por la Princesa Zelda# se trata de" },
    });

    hintTable[RHT_DMT_TRADE_BROKEN_SWORD] = HintText::Sometimes({
        // obscure text
        Text{ "a #blinded Biggoron# entrusts", /*french*/ "un #Grogoron aveuglé# confie",
              /*spanish*/ "un #miope Biggoron# otorga" },
    });

    hintTable[RHT_DMT_TRADE_EYEDROPS] = HintText::Sometimes({
        // obscure text
        Text{ "while you wait, #Biggoron# gives", /*french*/ "pendant que tu attends, #Biggoron# donne",
              /*spanish*/ "#Biggoron# está a la espera de otorgar" },
    });

    hintTable[RHT_DMT_TRADE_CLAIM_CHECK] = HintText::Sometimes({
        // obscure text
        Text{ "#Biggoron# crafts", /*french*/ "#Biggoron# fabrique", /*spanish*/ "#Biggoron# forja" },
    });

    hintTable[RHT_KAK_50_GOLD_SKULLTULA_REWARD] = HintText::Sometimes(
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

    hintTable[RHT_KAK_40_GOLD_SKULLTULA_REWARD] = HintText::Sometimes(
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

    hintTable[RHT_KAK_30_GOLD_SKULLTULA_REWARD] = HintText::Sometimes(
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

    hintTable[RHT_KAK_20_GOLD_SKULLTULA_REWARD] = HintText::Sometimes(
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

    hintTable[RHT_KAK_ANJU_AS_CHILD] = HintText::Sometimes(
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

    hintTable[RHT_KAK_TRADE_POCKET_CUCCO] = HintText::Sometimes({
        // obscure text
        Text{ "an adult's #happy Cucco# awards", /*french*/ "un adulte avec une #poulette joyeuse# obtient",
              /*spanish*/ "un #alegre cuco# en la madurez otorga" },
    });

    hintTable[RHT_KAK_TRADE_ODD_MUSHROOM] = HintText::Sometimes({
        // obscure text
        Text{ "the #potion shop lady# entrusts", /*french*/ "la #gribiche du magasin de potion# confie",
              /*spanish*/ "la #señora de la tienda de pociones# otorga" },
    });

    hintTable[RHT_GC_DARUNIAS_JOY] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #groovin' goron# gifts", /*french*/ "#le Goron joyeux# donne",
                  /*spanish*/ "#un goron marchoso# otorga" },
        },
        {},
        // clear text
        Text{ "#Darunia's dance# leads to", /*french*/ "#la dance de Darunia# donne",
              /*spanish*/ "#el baile de Darunia# conduce a" });

    hintTable[RHT_LW_SKULL_KID] = HintText::Sometimes({
        // obscure text
        Text{ "the #Skull Kid# grants", /*french*/ "le #Skull Kid# donne", /*spanish*/ "#Skull Kid# otorga" },
    });

    hintTable[RHT_LW_TRADE_COJIRO] = HintText::Sometimes({
        // obscure text
        Text{ "returning a #special Cucco# awards", /*french*/ "ramener une #poulette précieuse# donne",
              /*spanish*/ "quien devuelva un #cuco especial# encontrará" },
    });

    hintTable[RHT_LW_TRADE_ODD_POTION] = HintText::Sometimes({
        // obscure text
        Text{ "a #Kokiri girl in the woods# leaves", /*french*/ "la #fillette Kokiri dans les bois# laisse",
              /*spanish*/ "una #chica kokiri del bosque# otorga" },
    });

    hintTable[RHT_LH_SUN] = HintText::Sometimes(
        {
            // obscure text
            Text{ "staring into #the sun# grants", /*french*/ "regarder #le soleil# donne",
                  /*spanish*/ "#mirar al sol# revela" },
        },
        {},
        // clear text
        Text{ "shooting #the sun# grants", /*french*/ "tirer une flèche dans #sur le soleil# donne",
              /*spanish*/ "#disparar al sol# revela" });

    hintTable[RHT_LH_TRADE_FROG] = HintText::Sometimes({
        // obscure text
        Text{ "#Lake Hylia's scientist# hurriedly entrusts", /*french*/ "le #scientifique du lac# confie rapidement",
              /*spanish*/ "el #científico del Lago Hylia# otorga con prisa" },
    });

    hintTable[RHT_MARKET_TREASURE_CHEST_GAME_REWARD] = HintText::Sometimes(
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

    hintTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_1] = HintText::Sometimes(
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

    hintTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_2] = HintText::Sometimes(
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

    hintTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_3] = HintText::Sometimes(
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

    hintTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_4] = HintText::Sometimes(
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

    hintTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_5] = HintText::Sometimes(
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

    hintTable[RHT_GF_HBA_1500_POINTS] = HintText::Sometimes(
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

    hintTable[RHT_GRAVEYARD_HEART_PIECE_GRAVE_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "playing #Sun's Song# in a grave spawns", /*french*/ "jouer le #chant du soleil# dans un tombeau donne",
              /*spanish*/ "#tocar la Canción del Sol# en una cripta conduce a" },
    });

    hintTable[RHT_GC_MAZE_LEFT_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "in #Goron City# the hammer unlocks", /*french*/ "dans le #village Goron#, le marteau donne accès à",
              /*spanish*/ "en la #Ciudad Goron# el martillo desbloquea" },
    });

    hintTable[RHT_GV_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "in #Gerudo Valley# the hammer unlocks", /*french*/ "dans la #Vallée Gerudo#, le marteau donne accès à",
              /*spanish*/ "en el #Valle Gerudo# el martillo desbloquea" },
    });

    hintTable[RHT_GV_TRADE_SAW] = HintText::Sometimes({
        // obscure text
        Text{ "the #boss of the carpenters# leaves", /*french*/ "le #patron des ouvriers# laisse",
              /*spanish*/ "el #capataz de los carpinteros# otorga" },
    });

    hintTable[RHT_GV_COW] = HintText::Sometimes({
        // obscure text
        Text{ "a #cow in Gerudo Valley# gifts", /*french*/ "la #vache de la Vallée Gerudo# donne",
              /*spanish*/ "una #vaca del Valle Gerudo# brinda" },
    });

    hintTable[RHT_HC_GS_STORMS_GROTTO] = HintText::Sometimes({
        // obscure text
        Text{ "a #spider behind a muddy wall# in a grotto holds",
              /*french*/ "l'#araignée derrière un mur de boue# dans une grotte donne",
              /*spanish*/ "una #Skulltula tras la agrietada pared# de una cueva otorga" },
    });

    hintTable[RHT_HF_GS_COW_GROTTO] = HintText::Sometimes({
        // obscure text
        Text{ "a #spider behind webs# in a grotto holds",
              /*french*/ "l'#araignée derrière une toile# dans une grotte donne",
              /*spanish*/ "una #Skulltula tras la telaraña# de una cueva otorga" },
    });

    hintTable[RHT_HF_COW_GROTTO_COW] = HintText::Sometimes(
        {
            // obscure text
            Text{ "the #cobwebbed cow# gifts", /*french*/ "la #vache prisonnière d'araignées# donne",
                  /*spanish*/ "una #vaca tras una telaraña# brinda" },
        },
        {},
        // clear text
        Text{ "a #cow behind webs# in a grotto gifts", /*french*/ "la #vache derrière les toiles# d'une grotte donne",
              /*spanish*/ "una #vaca tras la telaraña# de una cueva brinda" });

    hintTable[RHT_ZR_FROGS_OCARINA_GAME] = HintText::Sometimes(
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

    hintTable[RHT_ZF_GS_HIDDEN_CAVE] = HintText::Sometimes({
        // obscure text
        Text{ "a spider high #above the icy waters# holds", /*french*/ "l'araignée #en haut des eaux glacées# donne",
              /*spanish*/ "una Skulltula en lo #alto de las congeladas aguas# otorga" },
    });

    hintTable[RHT_WASTELAND_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "#deep in the wasteland# is", /*french*/ "#loin dans le désert# gît",
              /*spanish*/ "en lo #profundo del desierto encantado# yace" },
        Text{ "beneath #the sands#, flames reveal", /*french*/ "#sous le désert#, les flammes font apparaître",
              /*spanish*/ "tras las #arenas# unas llamas revelan" },
    });

    hintTable[RHT_WASTELAND_GS] = HintText::Sometimes({
        // obscure text
        Text{ "a #spider in the wasteland# holds", /*french*/ "#l'araignée dans le désert# donne",
              /*spanish*/ "una #Skulltula del desierto encantado# otorga" },
    });

    hintTable[RHT_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "#flames in the Composers' Grave# reveal",
              /*french*/ "#les flammes dans le tombeau des compositeurs# cachent",
              /*spanish*/ "#las llamas del Panteón Real# revelan" },
        Text{ "the #Composer Brothers hid#", /*french*/ "#les Frères compositeurs on caché#",
              /*spanish*/ "los #hermanos compositores esconden#" },
    });

    hintTable[RHT_ZF_BOTTOM_FREESTANDING_POH] = HintText::Sometimes({
        // obscure text
        Text{ "#under the icy waters# lies", /*french*/ "#sous les eaux glacées# se cache",
              /*spanish*/ "#bajo las congeladas aguas# yace" },
    });

    hintTable[RHT_GC_POT_FREESTANDING_POH] = HintText::Sometimes({
        // obscure text
        Text{ "spinning #Goron pottery# contains", /*french*/ "la #potterie Goron# contient",
              /*spanish*/ "una #cerámica goron# contiene" },
    });

    hintTable[RHT_ZD_KING_ZORA_THAWED] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #defrosted dignitary# gifts", /*french*/ "le #monarque libéré# donne",
                  /*spanish*/ "una #liberación monárquica# brinda" },
        },
        {},
        // clear text
        Text{ "unfreezing #King Zora# grants", /*french*/ "dégeler # le Roi Zora# donne",
              /*spanish*/ "#descongelar al Rey Zora# conduce a" });

    hintTable[RHT_ZD_TRADE_PRESCRIPTION] = HintText::Sometimes({
        // obscure text
        Text{ "#King Zora# hurriedly entrusts", /*french*/ "le #roi Zora# confie rapidement",
              /*spanish*/ "el #Rey Zora# otorga con prisa" },
    });

    hintTable[RHT_DMC_DEKU_SCRUB] = HintText::Sometimes({
        // obscure text
        Text{ "a single #scrub in the crater# sells", /*french*/ "la #peste Mojo dans le cratère# vend",
              /*spanish*/ "un solitario #deku del cráter# vende" },
    });

    hintTable[RHT_DMC_GS_CRATE] = HintText::Sometimes({
        // obscure text
        Text{ "a spider under a #crate in the crater# holds", /*french*/ "la Skulltula dans une #boîte volcanique# a",
              /*spanish*/ "una Skulltula bajo una #caja del cráter# otorga" },
    });

    hintTable[RHT_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST] = HintText::Sometimes(
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

    hintTable[RHT_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM] = HintText::Sometimes(
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

    hintTable[RHT_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM] = HintText::Sometimes(
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

    hintTable[RHT_JABU_JABUS_BELLY_BOOMERANG_CHEST] = HintText::Sometimes(
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

    hintTable[RHT_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM] = HintText::Sometimes(
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

    hintTable[RHT_JABU_JABUS_BELLY_MQ_COW] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #cow swallowed by a deity# gifts", /*french*/ "la #vache dans le gardien# donne",
                  /*spanish*/ "una #vaca engullida por cierta deidad# brinda" },
        },
        {},
        // clear text
        Text{ "a #cow swallowed by Jabu-Jabu# gifts", /*french*/ "la #vache avallée par Jabu-Jabu# donne",
              /*spanish*/ "una #vaca engullida por Jabu-Jabu# brinda" });

    hintTable[RHT_FIRE_TEMPLE_SCARECROW_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "a #scarecrow atop the volcano# hides", /*french*/ "l'#épouvantail au sommet d'un volcan# donne",
                  /*spanish*/ "un #espantapájaros en lo alto del volcán# esconde" },
        },
        {},
        // clear text
        Text{ "#Pierre atop the Fire Temple# hides", /*french*/ "#Pierre au sommet du Temple du Feu# donne",
              /*spanish*/ "#Pierre en lo alto del Templo del Fuego# esconde" });

    hintTable[RHT_FIRE_TEMPLE_MEGATON_HAMMER_CHEST] = HintText::Sometimes(
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

    hintTable[RHT_FIRE_TEMPLE_MQ_CHEST_ON_FIRE] = HintText::Sometimes(
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

    hintTable[RHT_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE] = HintText::Sometimes(
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

    hintTable[RHT_WATER_TEMPLE_RIVER_CHEST] = HintText::Sometimes(
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

    hintTable[RHT_WATER_TEMPLE_BOSS_KEY_CHEST] = HintText::Sometimes(
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

    hintTable[RHT_WATER_TEMPLE_GS_BEHIND_GATE] = HintText::Sometimes(
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

    hintTable[RHT_WATER_TEMPLE_MQ_FREESTANDING_KEY] = HintText::Sometimes(
        {
            // obscure text
            Text{ "hidden in a #box under the lake# lies", /*french*/ "dans une #boîte sous le lac# gît",
                  /*spanish*/ "en una #caja bajo el lago# yace" },
        },
        {},
        // clear text
        Text{ "hidden in a #box in the Water Temple# lies", /*french*/ "dans une #boîte dans le Temple de l'Eau# gît",
              /*spanish*/ "en una #caja del Templo del Agua# yace" });

    hintTable[RHT_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA] = HintText::Sometimes(
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

    hintTable[RHT_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH] = HintText::Sometimes(
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

    hintTable[RHT_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "those who seek #sunken silver rupees# will find",
              /*french*/ "ceux qui pêchent les #joyaux argentés# trouveront",
              /*spanish*/ "aquellos que busquen las #rupias plateadas sumergidas# encontrarán" },
        Text{ "the #thieves' underwater training# rewards", /*french*/ "l'#épreuve de plongée des voleurs# recèle",
              /*spanish*/ "la #instrucción submarina de las bandidas# premia" },
    });

    hintTable[RHT_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "those who seek #sunken silver rupees# will find",
              /*french*/ "ceux qui pêchent les #joyaux argentés# trouveront",
              /*spanish*/ "aquellos que busquen las #rupias plateadas sumergidas# encontrarán" },
        Text{ "the #thieves' underwater training# rewards", /*french*/ "l'#épreuve de plongée des voleurs# recèle",
              /*spanish*/ "la #instrucción submarina de las bandidas# premia" },
    });

    hintTable[RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "the final prize of #the thieves' training# is",
              /*french*/ "la récompense ultime de #l'épreuve des voleurs# est",
              /*spanish*/ "la recompensa final de la #instrucción de las bandida# brinda" },
    });

    hintTable[RHT_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "the final prize of #the thieves' training# is",
              /*french*/ "la récompense ultime de #l'épreuve des voleurs# est",
              /*spanish*/ "el premio final de la #instrucción de las bandidas# brinda" },
    });

    hintTable[RHT_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST] = HintText::Sometimes(
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

    hintTable[RHT_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST] = HintText::Sometimes(
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

    hintTable[RHT_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST] = HintText::Sometimes(
        {
            // obscure text
            Text{ "the treasure #sought by Nabooru# is", /*french*/ "le trésor que #recherche Nabooru# est",
                  /*spanish*/ "el #ansiado tesoro de Nabooru# brinda" },
        },
        {},
        // clear text
        Text{ "upon the #Colossus's right hand# is", /*french*/ "sur la #main droite du colosse# repose",
              /*spanish*/ "en la #mano derecha del Coloso# yace" });

    hintTable[RHT_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "upon the #Colossus's left hand# is", /*french*/ "sur la #main gauche du colosse# repose",
              /*spanish*/ "en la #mano izquierda del Coloso# yace" },
    });

    hintTable[RHT_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST] = HintText::Sometimes(
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

    hintTable[RHT_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST] = HintText::Sometimes(
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

    hintTable[RHT_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM] = HintText::Sometimes(
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

    hintTable[RHT_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "shadows in an #invisible maze# guard", /*french*/ "les ombres dans le #labyrinthe invisible# protègent",
              /*spanish*/ "las sombras del #laberinto misterioso# esconden" },
    });

    hintTable[RHT_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST] = HintText::Sometimes({
        // obscure text
        Text{ "shadows in an #invisible maze# guard", /*french*/ "les ombres dans le #labyrinthe invisible# protègent",
              /*spanish*/ "las sombras del #laberinto invisible# esconden" },
    });

    /*--------------------------
    |    ENTRANCE HINT TEXT    |
    ---------------------------*/

    hintTable[RHT_DESERT_COLOSSUS_TO_COLOSSUS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "lifting a #rock in the desert# reveals", /*french*/ "soulever une #roche dans le désert# révèle",
              /*spanish*/ "levantar una #roca del desierto# revela" },
    });

    hintTable[RHT_GV_GROTTO_LEDGE_TO_GV_OCTOROK_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a rock on #a ledge in the valley# hides", /*french*/ "soulever une #roche dans la vallée# révèle",
              /*spanish*/ "levantar una #roca al borde del valle# esconde" },
    });

    hintTable[RHT_GC_GROTTO_PLATFORM_TO_GC_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #pool of lava# in Goron City blocks the way to",
              /*french*/ "l'#étang de lave# dans le village Goron renferme",
              /*spanish*/ "un #estanque de lava# en la Ciudad Goron bloquea el paso a" },
    });

    hintTable[RHT_GERUDO_FORTRESS_TO_GF_STORMS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #storm within Gerudo's Fortress# reveals", /*french*/ "la #tempête dans la forteresse# révèle",
              /*spanish*/ "una #tormenta en la Fortaleza Gerudo# revela" },
    });

    hintTable[RHT_ZORAS_DOMAIN_TO_ZD_STORMS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #storm within Zora's Domain# reveals", /*french*/ "la #tempête dans le Domaine Zora# révèle",
              /*spanish*/ "una #tormenta en la Región de los Zora# revela" },
    });

    hintTable[RHT_HYRULE_CASTLE_GROUNDS_TO_HC_STORMS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #storm near the castle# reveals", /*french*/ "la #tempête près du château# révèle",
              /*spanish*/ "una #tormenta junto al castillo# revela" },
    });

    hintTable[RHT_GV_FORTRESS_SIDE_TO_GV_STORMS_GROTTO] = HintText::Entrance({
        // obscure text
        Text{ "a #storm in the valley# reveals", /*french*/ "la #tempête dans la vallée# révèle",
              /*spanish*/ "una #tormenta en el valle# revela" },
    });

    hintTable[RHT_DESERT_COLOSSUS_TO_COLOSSUS_GREAT_FAIRY_FOUNTAIN] = HintText::Entrance({
        // obscure text
        Text{ "a #fractured desert wall# hides", /*french*/ "le #mur fragile du désert# cache",
              /*spanish*/ "una #agrietada pared del desierto# esconde" },
    });

    hintTable[RHT_GANONS_CASTLE_GROUNDS_TO_OGC_GREAT_FAIRY_FOUNTAIN] = HintText::Entrance({
        // obscure text
        Text{ "a #heavy pillar# outside the castle obstructs", /*french*/ "le #rocher fragile près du château# cache",
              /*spanish*/ "una #pesada columna# fuera del castillo obstruye" },
    });

    hintTable[RHT_ZORAS_FOUNTAIN_TO_ZF_GREAT_FAIRY_FOUNTAIN] = HintText::Entrance({
        // obscure text
        Text{ "a #fountain wall# hides", /*french*/ "le #mur fragile du réservoir# cache",
              /*spanish*/ "una #pared de la fuente# esconde" },
    });

    hintTable[RHT_GV_FORTRESS_SIDE_TO_GV_CARPENTER_TENT] = HintText::Entrance({
        // obscure text
        Text{ "a #tent in the valley# covers", /*french*/ "la #tente dans la vallée# recouvre",
              /*spanish*/ "una #tienda de campaña del valle# cubre" },
    });

    hintTable[RHT_GRAVEYARD_WARP_PAD_REGION_TO_SHADOW_TEMPLE_ENTRYWAY] = HintText::Entrance({
        // obscure text
        Text{ "at the #back of the Graveyard#, there is", /*french*/ "#derrière le Cimetière# gît",
              /*spanish*/ "en la #parte trasera del cementerio# se halla" },
    });

    hintTable[RHT_LAKE_HYLIA_TO_WATER_TEMPLE_LOBBY] = HintText::Entrance({
        // obscure text
        Text{ "deep #under a vast lake#, one can find", /*french*/ "#sous le lac# gît",
              /*spanish*/ "en las #profundidades de un lago inmenso# se halla" },
    });

    hintTable[RHT_GERUDO_FORTRESS_TO_GERUDO_TRAINING_GROUNDS_LOBBY] = HintText::Entrance({
        // obscure text
        Text{ "paying a #fee to the Gerudos# grants access to",
              /*french*/ "l'#entrée payante des Gerudo# donne accès à",
              /*spanish*/ "pagarle una #tasa a las gerudo# da acceso a" },
    });

    hintTable[RHT_ZORAS_FOUNTAIN_TO_JABU_JABUS_BELLY_BEGINNING] = HintText::Entrance({
        // obscure text
        Text{ "inside #Jabu-Jabu#, one can find", /*french*/ "#dans Jabu-Jabu# se trouve",
              /*spanish*/ "dentro de #Jabu-Jabu# se halla" },
    });

    hintTable[RHT_KAKARIKO_VILLAGE_TO_BOTTOM_OF_THE_WELL] = HintText::Entrance({
        // obscure text
        Text{ "a #village well# leads to", /*french*/ "dans le fond du #Puits du village# gît",
              /*spanish*/ "el #pozo de un pueblo# conduce a" },
    });

    /*--------------------------
    |      EXIT HINT TEXT      |
    ---------------------------*/
    // maybe make a new type for this? I'm not sure if it really matters

    hintTable[RHT_LINKS_POCKET] = HintText::Exclude({
        // obscure text
        Text{ "Link's Pocket", /*french*/ "les Poches de @", /*spanish*/ "el bolsillo de @" },
    });

    hintTable[RHT_KOKIRI_FOREST] = HintText::Exclude({
        // obscure text
        Text{ "Kokiri Forest", /*french*/ "la Forêt Kokiri", /*spanish*/ "el Bosque Kokiri" },
    });

    hintTable[RHT_THE_LOST_WOODS] = HintText::Exclude({
        // obscure text
        Text{ "the Lost Woods", /*french*/ "les Bois Perdus", /*spanish*/ "el Bosque Perdido" },
    });

    hintTable[RHT_SACRED_FOREST_MEADOW] = HintText::Exclude({
        // obscure text
        Text{ "Sacred Forest Meadow", /*french*/ "le Bosquet Sacré", /*spanish*/ "la pradera sagrada del bosque" },
    });

    hintTable[RHT_HYRULE_FIELD] = HintText::Exclude({
        // obscure text
        Text{ "Hyrule Field", /*french*/ "la Plaine d'Hyrule", /*spanish*/ "la Llanura de Hyrule" },
    });

    hintTable[RHT_LAKE_HYLIA] = HintText::Exclude({
        // obscure text
        Text{ "Lake Hylia", /*french*/ "le Lac Hylia", /*spanish*/ "el Lago Hylia" },
    });

    hintTable[RHT_GERUDO_VALLEY] = HintText::Exclude({
        // obscure text
        Text{ "Gerudo Valley", /*french*/ "la Vallée Gerudo", /*spanish*/ "el Valle Gerudo" },
    });

    hintTable[RHT_GERUDO_FORTRESS] = HintText::Exclude({
        // obscure text
        Text{ "Gerudo's Fortress", /*french*/ "le Repaire des Voleurs", /*spanish*/ "la Fortaleza Gerudo" },
    });

    hintTable[RHT_HAUNTED_WASTELAND] = HintText::Exclude({
        // obscure text
        Text{ "Haunted Wasteland", /*french*/ "le Désert Hanté", /*spanish*/ "el desierto encantado" },
    });

    hintTable[RHT_DESERT_COLOSSUS] = HintText::Exclude({
        // obscure text
        Text{ "Desert Colossus", /*french*/ "le Colosse du Désert", /*spanish*/ "el Coloso del Desierto" },
    });

    hintTable[RHT_THE_MARKET] = HintText::Exclude({
        // obscure text
        Text{ "the Market", /*french*/ "la Place du Marché", /*spanish*/ "la plaza del mercado" },
    });

    hintTable[RHT_TEMPLE_OF_TIME] = HintText::Exclude({
        // obscure text
        Text{ "Temple of Time", /*french*/ "le Temple du Temps", /*spanish*/ "el Templo del Tiempo" },
    });

    hintTable[RHT_CASTLE_GROUNDS] = HintText::Exclude({
        // obscure text
        Text{ "the Castle Grounds", /*french*/ "le Château d'Hyrule", /*spanish*/ "" },
    });

    hintTable[RHT_HYRULE_CASTLE] = HintText::Exclude({
        // obscure text
        Text{ "Hyrule Castle", /*french*/ "le Château d'Hyrule", /*spanish*/ "el Castillo de Hyrule" },
    });

    hintTable[RHT_OUTSIDE_GANONS_CASTLE] = HintText::Exclude({
        // obscure text
        Text{ "outside Ganon's Castle", /*french*/ "les alentours du Château de Ganon",
              /*spanish*/ "el exterior del Castillo de Ganon" },
    });

    hintTable[RHT_CASTLE_GROUNDS] = HintText::Exclude({
        // obscure text
        Text{ "the Castle Grounds", /*french*/ "le Château d'Hyrule", /*spanish*/ "" },
    });

    hintTable[RHT_KAKARIKO_VILLAGE] = HintText::Exclude({
        // obscure text
        Text{ "Kakariko Village", /*french*/ "le Village Cocorico", /*spanish*/ "Kakariko" },
    });

    hintTable[RHT_THE_GRAVEYARD] = HintText::Exclude({
        // obscure text
        Text{ "the Graveyard", /*french*/ "le Cimetière", /*spanish*/ "el cementerio" },
    });

    hintTable[RHT_DEATH_MOUNTAIN_TRAIL] = HintText::Exclude({
        // obscure text
        Text{ "Death Mountain Trail", /*french*/ "le Chemin du Péril",
              /*spanish*/ "el sendero de la Montaña de la Muerte" },
    });

    hintTable[RHT_GORON_CITY] = HintText::Exclude({
        // obscure text
        Text{ "Goron City", /*french*/ "le Village Goron", /*spanish*/ "la Ciudad Goron" },
    });

    hintTable[RHT_DEATH_MOUNTAIN_CRATER] = HintText::Exclude({
        // obscure text
        Text{ "Death Mountain Crater", /*french*/ "le Cratère du Péril",
              /*spanish*/ "el cráter de la Montaña de la Muerte" },
    });

    hintTable[RHT_ZORAS_RIVER] = HintText::Exclude({
        // obscure text
        Text{ "Zora's River", /*french*/ "la Rivière Zora", /*spanish*/ "el Río Zora" },
    });

    hintTable[RHT_ZORAS_DOMAIN] = HintText::Exclude({
        // obscure text
        Text{ "Zora's Domain", /*french*/ "le Domaine Zora", /*spanish*/ "la Región de los Zora" },
    });

    hintTable[RHT_ZORAS_FOUNTAIN] = HintText::Exclude({
        // obscure text
        Text{ "Zora's Fountain", /*french*/ "la Fontaine Zora", /*spanish*/ "la Fuente Zora" },
    });

    hintTable[RHT_LON_LON_RANCH] = HintText::Exclude({
        // obscure text
        Text{ "Lon Lon Ranch", /*french*/ "le Ranch Lon Lon", /*spanish*/ "el Rancho Lon Lon" },
    });

    /*--------------------------
    |     REGION HINT TEXT     |
    ---------------------------*/

    hintTable[RHT_KF_LINKS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Link's House", /*french*/ "la #maison de @#", /*spanish*/ "la casa de @" },
    });

    //   hintTable[RHT_TOT_MAIN] = HintText::Region({
    //                    //obscure text
    //                    Text{"the #Temple of Time#", /*french*/"le #Temple du Temps#", /*spanish*/"el Templo del
    //                    Tiempo"},
    // });

    hintTable[RHT_KF_MIDOS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Mido's house", /*french*/ "la Cabane du Grand Mido", /*spanish*/ "la casa de Mido" },
    });

    hintTable[RHT_KF_SARIAS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Saria's House", /*french*/ "la Cabane de Saria", /*spanish*/ "la casa de Saria" },
    });

    hintTable[RHT_KF_HOUSE_OF_TWINS] = HintText::Region({
        // obscure text
        Text{ "the #House of Twins#", /*french*/ "la Cabane des Jumelles", /*spanish*/ "la casa de las gemelas" },
    });

    hintTable[RHT_KF_KNOW_IT_ALL_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Know-It-All Brothers' House", /*french*/ "la Cabane des frères Je-Sais-Tout",
              /*spanish*/ "la casa de los hermanos Sabelotodo" },
    });

    hintTable[RHT_KF_KOKIRI_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Kokiri Shop#", /*french*/ "le #Magasin Kokiri#", /*spanish*/ "la tienda kokiri" },
    });

    hintTable[RHT_LH_LAB] = HintText::Region({
        // obscure text
        Text{ "the #Lakeside Laboratory#", /*french*/ "le #Laboratoire du Lac#",
              /*spanish*/ "el laboratorio del lago" },
    });

    hintTable[RHT_LH_FISHING_HOLE] = HintText::Region({
        // obscure text
        Text{ "the #Fishing Pond#", /*french*/ "l'#Étang#", /*spanish*/ "el estanque" },
    });

    hintTable[RHT_GV_CARPENTER_TENT] = HintText::Region({
        // obscure text
        Text{ "the #Carpenters' tent#", /*french*/ "la #Tente des charpentiers#",
              /*spanish*/ "la #tienda de campaña de los carpinteros#" },
    });

    hintTable[RHT_MARKET_GUARD_HOUSE] = HintText::Region({
        // obscure text
        Text{ "the #Guard House#", /*french*/ "le #poste de garde#", /*spanish*/ "la caseta de guardia" },
    });

    hintTable[RHT_MARKET_MASK_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Happy Mask Shop#", /*french*/ "la #Foire Aux Masques#",
              /*spanish*/ "la tienda de La Máscara Feliz" },
    });

    hintTable[RHT_MARKET_BOMBCHU_BOWLING] = HintText::Region({
        // obscure text
        Text{ "the #Bombchu Bowling Alley#", /*french*/ "le #Bowling Teigneux#", /*spanish*/ "la Bolera Bombchu" },
    });

    hintTable[RHT_MARKET_POTION_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Market Potion Shop#", /*french*/ "l'#apothicaire de la Place du Marché#",
              /*spanish*/ "la tienda de pociones de la plaza del mercado" },
    });

    hintTable[RHT_MARKET_TREASURE_CHEST_GAME] = HintText::Region({
        // obscure text
        Text{ "the #Treasure Chest Shop#", /*french*/ "la #Chasse-aux-Trésors#", /*spanish*/ "el Cofre del Tesoro" },
    });

    hintTable[RHT_MARKET_BOMBCHU_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Bombchu Shop#", /*french*/ "le #Magasin de Missiles#", /*spanish*/ "la Tienda Bombchu" },
    });

    hintTable[RHT_MARKET_MAN_IN_GREEN_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Man in Green's House", /*french*/ "la #Maison de l'Homme en Vert#",
              /*spanish*/ "la casa del hombre de verde" },
    });

    hintTable[RHT_KAK_WINDMILL] = HintText::Region({
        // obscure text
        Text{ "the #Windmill#", /*french*/ "le #Moulin#", /*spanish*/ "el #molino#" },
    });

    hintTable[RHT_KAK_CARPENTER_BOSS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "the #Carpenters' Boss House#", /*french*/ "la #Maison du Chef des ouvriers#",
              /*spanish*/ "la casa del capataz de los carpinteros" },
    });

    hintTable[RHT_KAK_HOUSE_OF_SKULLTULA] = HintText::Region({
        // obscure text
        Text{ "the #House of Skulltula#", /*french*/ "la #Maison des Skulltulas#",
              /*spanish*/ "la casa de las Skulltulas" },
    });

    hintTable[RHT_KAK_IMPAS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Impa's House", /*french*/ "la #Maison d'Impa#", /*spanish*/ "la casa de Impa" },
    });

    hintTable[RHT_KAK_IMPAS_HOUSE_BACK] = HintText::Region({
        // obscure text
        Text{ "Impa's cow cage", /*french*/ "la #cage à vache d'Impa#", /*spanish*/ "la jaula de la vaca de Impa" },
    });

    hintTable[RHT_KAK_ODD_POTION_BUILDING] = HintText::Region({
        // obscure text
        Text{ "Granny's Potion Shop", /*french*/ "la #maison bleue de Cocorico#",
              /*spanish*/ "la tienda de pociones de la abuela" },
    });

    hintTable[RHT_GRAVEYARD_DAMPES_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Dampé's Hut", /*french*/ "la #Cabane du Fossoyeur#", /*spanish*/ "la cabaña de Dampé" },
    });

    hintTable[RHT_GC_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Goron Shop#", /*french*/ "la #Boutique Goron#", /*spanish*/ "la #tienda goron#" },
    });

    hintTable[RHT_ZD_SHOP] = HintText::Region({
        // obscure text
        Text{ "the #Zora Shop#", /*french*/ "la #Boutique Zora#", /*spanish*/ "la #tienda zora#" },
    });

    hintTable[RHT_LLR_TALONS_HOUSE] = HintText::Region({
        // obscure text
        Text{ "Talon's House", /*french*/ "la #Maison de Talon#", /*spanish*/ "la casa de Talon" },
    });

    hintTable[RHT_LLR_STABLES] = HintText::Region({
        // obscure text
        Text{ "a #stable#", /*french*/ "l'#Étable#", /*spanish*/ "el establo" },
    });

    hintTable[RHT_LLR_TOWER] = HintText::Region({
        // obscure text
        Text{ "the #Lon Lon Tower#", /*french*/ "le #silo du Ranch Lon Lon#", /*spanish*/ "la torre Lon Lon" },
    });

    hintTable[RHT_MARKET_BAZAAR] = HintText::Region({
        // obscure text
        Text{ "the #Market Bazaar#", /*french*/ "le #Bazar de la Place du Marché#",
              /*spanish*/ "el bazar de la plaza del mercado" },
    });

    hintTable[RHT_MARKET_SHOOTING_GALLERY] = HintText::Region({
        // obscure text
        Text{ "a #Slingshot Shooting Gallery#", /*french*/ "le #Jeu d'Adresse de la Place du Marché#",
              /*spanish*/ "el Tiro al Blanco con tirachinas" },
    });

    hintTable[RHT_KAK_BAZAAR] = HintText::Region({
        // obscure text
        Text{ "the #Kakariko Bazaar#", /*french*/ "le #Bazar de Cocorico#", /*spanish*/ "el bazar de Kakariko" },
    });

    hintTable[RHT_KAK_POTION_SHOP_FRONT] = HintText::Region({
        // obscure text
        Text{ "the #Kakariko Potion Shop#", /*french*/ "l'#apothicaire de Cocorico#",
              /*spanish*/ "la tienda de pociones de Kakariko" },
    });

    hintTable[RHT_KAK_POTION_SHOP_BACK] = HintText::Region({
        // obscure text
        Text{ "the #Kakariko Potion Shop#", /*french*/ "l'#apothicaire de Cocorico#",
              /*spanish*/ "la tienda de pociones de Kakariko" },
    });

    hintTable[RHT_KAK_SHOOTING_GALLERY] = HintText::Region({
        // obscure text
        Text{ "a #Bow Shooting Gallery#", /*french*/ "le #jeu d'adresse de Cocorico#",
              /*spanish*/ "el Tiro al Blanco con arco" },
    });

    hintTable[RHT_COLOSSUS_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[RHT_HC_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[RHT_OGC_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[RHT_DMC_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[RHT_DMT_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[RHT_ZF_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
        // obscure text
        Text{ "a #Great Fairy Fountain#", /*french*/ "une #Fontaine Royale des Fées#",
              /*spanish*/ "una fuente de la Gran Hada" },
    });

    hintTable[RHT_GRAVEYARD_SHIELD_GRAVE] = HintText::Region({
        // obscure text
        Text{ "a #grave with a free chest#", /*french*/ "le #tombeau avec un trésor#",
              /*spanish*/ "la #tumba con un cofre#" },
    });

    hintTable[RHT_GRAVEYARD_HEART_PIECE_GRAVE] = HintText::Region({
        // obscure text
        Text{ "a chest spawned by #Sun's Song#", /*french*/ "un #coffre apparaît avec le Chant du Soleil#",
              /*spanish*/ "la #tumba de la Canción del Sol#" },
    });

    hintTable[RHT_GRAVEYARD_COMPOSERS_GRAVE] = HintText::Region({
        // obscure text
        Text{ "the #Composers' Grave#", /*french*/ "la #Tombe royale#", /*spanish*/ "el #Panteón Real#" },
    });

    hintTable[RHT_GRAVEYARD_DAMPES_GRAVE] = HintText::Region({
        // obscure text
        Text{ "Dampé's Grave", /*french*/ "la #Tombe d'Igor#", /*spanish*/ "la #tumba de Dampé#" },
    });

    hintTable[RHT_DMT_COW_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a solitary #Cow#", /*french*/ "la #grotte avec une vache#", /*spanish*/ "una #vaca# solitaria" },
    });

    hintTable[RHT_HC_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a sandy grotto with #fragile walls#", /*french*/ "la #grotte avec des murs fragiles#",
              /*spanish*/ "la arenosa gruta de #frágiles paredes#" },
    });

    hintTable[RHT_HF_TEKTITE_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a pool guarded by a #Tektite#", /*french*/ "l'#étang sous-terrain avec un Araknon#",
              /*spanish*/ "un charco custodiado por un #Tektite#" },
    });

    hintTable[RHT_HF_NEAR_KAK_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #Big Skulltula# guarding a Gold one", /*french*/ "la #grotte d'araignées#",
              /*spanish*/ "una #gran Skulltula# custodiando una dorada" },
    });

    hintTable[RHT_HF_COW_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a grotto full of #spider webs#", /*french*/ "la #grotte couverte de toiles d'araignées#",
              /*spanish*/ "una gruta llena de #telarañas#" },
    });

    hintTable[RHT_KAK_REDEAD_GROTTO] = HintText::Region({
        // obscure text
        Text{ "#ReDeads# guarding a chest", /*french*/ "le tombeau de #deux morts#",
              /*spanish*/ "los #ReDeads# que custodian un cofre" },
    });

    hintTable[RHT_SFM_WOLFOS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "#Wolfos# guarding a chest", /*french*/ "la #grotte iridescente#",
              /*spanish*/ "los #Wolfos# que custodian un cofre" },
    });

    hintTable[RHT_GV_OCTOROK_GROTTO] = HintText::Region({
        // obscure text
        Text{ "an #Octorok# guarding a rich pool", /*french*/ "un #étang sous-terrain avec un Octorok#",
              /*spanish*/ "un #Octorok# que custodia un lujoso charco" },
    });

    hintTable[RHT_DEKU_THEATER] = HintText::Region({
        // obscure text
        Text{ "the #Lost Woods Stage#", /*french*/ "le #théâtre sylvestre#",
              /*spanish*/ "el #escenario del Bosque Perdido#" },
    });

    hintTable[RHT_ZR_OPEN_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[RHT_DMC_UPPER_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[RHT_DMT_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[RHT_KAK_OPEN_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[RHT_HF_NEAR_MARKET_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[RHT_HF_OPEN_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[RHT_HF_SOUTHEAST_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[RHT_KF_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[RHT_LW_NEAR_SHORTCUTS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #generic grotto#", /*french*/ "une #grotte avec un trésor#", /*spanish*/ "una #cueva genérica#" },
    });

    hintTable[RHT_HF_INSIDE_FENCE_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a #single Upgrade Deku Scrub#", /*french*/ "une #grotte avec une peste Mojo#",
              /*spanish*/ "una cueva con un #solitario mercader deku#" },
    });

    hintTable[RHT_LW_SCRUBS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "#2 Deku Scrubs# including an Upgrade one", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[RHT_COLOSSUS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "2 Deku Scrubs", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[RHT_ZR_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "2 Deku Scrubs", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[RHT_SFM_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "2 Deku Scrubs", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[RHT_GV_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "2 Deku Scrubs", /*french*/ "une #grotte avec deux pestes Mojo#",
              /*spanish*/ "una cueva con #dos mercaderes deku#" },
    });

    hintTable[RHT_LH_GROTTO] = HintText::Region({
        // obscure text
        Text{ "3 Deku Scrubs", /*french*/ "une #grotte avec trois pestes Mojo#",
              /*spanish*/ "una cueva con #tres mercaderes deku#" },
    });

    hintTable[RHT_DMC_HAMMER_GROTTO] = HintText::Region({
        // obscure text
        Text{ "3 Deku Scrubs", /*french*/ "une #grotte avec trois pestes Mojo#",
              /*spanish*/ "una cueva con #tres mercaderes deku#" },
    });

    hintTable[RHT_GC_GROTTO] = HintText::Region({
        // obscure text
        Text{ "3 Deku Scrubs", /*french*/ "une #grotte avec trois pestes Mojo#",
              /*spanish*/ "una cueva con #tres mercaderes deku#" },
    });

    hintTable[RHT_LLR_GROTTO] = HintText::Region({
        // obscure text
        Text{ "3 Deku Scrubs", /*french*/ "une #grotte avec trois pestes Mojo#",
              /*spanish*/ "una cueva con #tres mercaderes deku#" },
    });

    hintTable[RHT_ZR_FAIRY_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    hintTable[RHT_HF_FAIRY_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    hintTable[RHT_SFM_FAIRY_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    hintTable[RHT_ZD_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    hintTable[RHT_GF_STORMS_GROTTO] = HintText::Region({
        // obscure text
        Text{ "a small #Fairy Fountain#", /*french*/ "une #fontaine de fées#",
              /*spanish*/ "una pequeña #fuente de hadas#" },
    });

    /*--------------------------
    |      JUNK HINT TEXT      |
    ---------------------------*/

    hintTable[RHT_JUNK02] = HintText::Junk({
        // obscure text
        Text{ "They say you must read the names of \"Special Deal\" shop items carefully.",
              /*french*/ "Selon moi, les \"Offres spéciales\" sont parfois trompeuses... Lisez les attentivement!",
              /*spanish*/ "Según dicen, se debería prestar atención a los nombres de las ofertas especiales." },
    });

    hintTable[RHT_JUNK03] = HintText::Junk({
        // obscure text
        Text{ "They say that Zelda is a poor leader.", /*french*/ "Selon moi, Zelda ne ferait pas un bon monarque.",
              /*spanish*/ "Según dicen, Zelda es mala líder." },
    });

    hintTable[RHT_JUNK04] = HintText::Junk({
        // obscure text
        Text{ "These hints can be quite useful. This is an exception.",
              /*french*/ "Ces indices sont très utiles, à l'exception de celui-ci.",
              /*spanish*/ "Las pistas suelen servir de ayuda. En cambio, esta no." },
    });

    hintTable[RHT_JUNK05] = HintText::Junk({
        // obscure text
        Text{ "They say that the Lizalfos in Dodongo's Cavern like to play in lava.",
              /*french*/ "Selon moi, les Lézalfos de la Caverne Dodongo aiment patauger dans la lave.",
              /*spanish*/ "Según dicen, a los Lizalfos de la Cueva de los Dodongos les gusta jugar en la lava." },
    });

    hintTable[RHT_JUNK06] = HintText::Junk({
        // obscure text
        Text{ "They say that all the Zora drowned in Wind Waker.",
              /*french*/ "Selon moi, les Zoras se sont noyés dans Wind Waker.",
              /*spanish*/ "Según dicen, en Wind Waker todos los zora se ahogaron." },
    });

    hintTable[RHT_JUNK07] = HintText::Junk({
        // obscure text
        Text{ "If Gorons eat rocks, does that mean I'm in danger?",
              /*french*/ "Ne dis pas au Gorons que je suis ici. Ils mangent des roches, tu sais!",
              /*spanish*/ "Si los Goron se tragan las piedras, ¿no me hace ser una especia vulnarable o algo así" },
    });

    hintTable[RHT_JUNK08] = HintText::Junk({
        // obscure text
        Text{
            "'Member when Ganon was a blue pig?^I 'member.",
            /*french*/ "Dans mon temps, Ganon était un cochon bleu...^Pff! Les jeunes de nos jours, et leur Ganondorf!",
            /*spanish*/ "¿T'acuerdas cuando Ganon era un cerdo azul?^Qué tiempos, chico." },
    });

    hintTable[RHT_JUNK09] = HintText::Junk({
        // obscure text
        Text{ "One who does not have Triforce can't go in.", /*french*/ "Ceux sans Triforce doivent rebrousser chemin.",
              /*spanish*/ "Aquel que no porte la Trifuerza no podrá pasar." },
    });

    hintTable[RHT_JUNK10] = HintText::Junk({
        // obscure text
        Text{ "Save your future, end the Happy Mask Salesman.",
              /*french*/ "Selon moi, tu t'éviteras des jours de malheur si tu vaincs le vendeur de masques...",
              /*spanish*/ "Salva tu futuro, acaba con el dueño de La Máscara Feliz." },
    });

    hintTable[RHT_JUNK11] = HintText::Junk({
        // obscure text
        Text{ "Glitches are a pathway to many abilities some consider to be... Unnatural.", /*french*/
              "Les glitchs sont un moyen d'acquérir de nombreuses facultés considérées par certains comme... contraire "
              "à la nature.",
              /*spanish*/ "Los glitches son el camino a muchas habilidades que varios consideran... nada natural." },
    });

    hintTable[RHT_JUNK12] = HintText::Junk({
        // obscure text
        Text{ "I'm stoned. Get it?", /*french*/ "Allez, roche, papier, ciseau...&Roche.",
              /*spanish*/ "Me he quedado de piedra. ¿Lo pillas?" },
    });

    hintTable[RHT_JUNK13] = HintText::Junk({
        // obscure text
        Text{ "Hoot! Hoot! Would you like me to repeat that?", /*french*/ "Hou hou! Veux-tu que je répète tout ça?",
              /*spanish*/ "¡Buuu, buuu! ¿Te lo vuelvo a repetir?" },
    });

    hintTable[RHT_JUNK14] = HintText::Junk({
        // obscure text
        Text{ "Gorons are stupid. They eat rocks.", /*french*/ "Les Gorons sont des vraies têtes dures.",
              /*spanish*/ "Los Goron son tontos. Se comen las piedras." },
    });

    hintTable[RHT_JUNK15] = HintText::Junk({
        // obscure text
        Text{ "They say that Lon Lon Ranch prospered under Ingo.",
              /*french*/ "Selon moi, le Ranch Lon Lon était plus prospère sous Ingo.",
              /*spanish*/ "Según dicen, el Rancho Lon Lon prosperó gracias a Ingo." },
    });

    hintTable[RHT_JUNK16] = HintText::Junk({
        // obscure text
        Text{ "The single rupee is a unique item.", /*french*/ "Nul objet n'est plus unique que le rubis vert.",
              /*spanish*/ "La rupia de uno es un objeto singular." },
    });

    hintTable[RHT_JUNK17] = HintText::Junk({
        // obscure text
        Text{ "Without the Lens of Truth, the Treasure Chest Mini-Game is a 1 out of 32 chance.^Good luck!",
              /*french*/ "Gagner la Chasse-aux-Trésors est 1 chance sur 32.^Bonne chance!", /*spanish*/
              "Sin la Lupa de la Verdad, ganarías 1/32 veces en el Cofre del Tesoro.^¡Buena suerte con ello!" },
    });

    hintTable[RHT_JUNK18] = HintText::Junk({
        // obscure text
        Text{ "Use bombs wisely.", /*french*/ "Utilise les bombes avec précaution.",
              /*spanish*/ "No desperdicies las bombas." },
    });

    hintTable[RHT_JUNK19] = HintText::Junk({
        // obscure text
        Text{ "They say that Volvagia hates splinters", /*french*/ "Selon moi, Volvagia déteste les échardes.",
              /*spanish*/ "Según dicen, Volvagia le teme a las astillas." },
    });

    hintTable[RHT_JUNK20] = HintText::Junk({
        // obscure text
        Text{ "They say that funky monkeys can be spotted on Friday.",
              /*french*/ "Selon moi, des capucins coquins sortent le vendredi.",
              /*spanish*/ "Según dicen, en los viernes puedes hallar monos marchosos." },
    });

    hintTable[RHT_JUNK21] = HintText::Junk({
        // obscure text
        Text{ "I found you, faker!", /*french*/ "Ah-ha! Je t'ai trouvé!", /*spanish*/ "¡Ahí estás, impostor!" },
    });

    hintTable[RHT_JUNK22] = HintText::Junk({
        // obscure text
        Text{ "They say the Groose is loose.", /*french*/ "Selon moi, Hergo est le vrai héros.",
              /*spanish*/ "Según dicen, Malton es un espanto." },
    });

    hintTable[RHT_JUNK23] = HintText::Junk({
        // obscure text
        Text{
            "They say that players who select the \"ON\" option for \"MOTION CONTROL\" are the real \"Zelda players!\"",
            /*french*/ "Selon moi, ceux qui utilisent les contrôles gyroscopiques sont les VRAIS joueurs.", /*spanish*/
            "Según dicen, aquellos que juegan usando el control por movimiento son los verdaderos jugadores de "
            "Zelda." },
    });

    hintTable[RHT_JUNK24] = HintText::Junk({
        // obscure text
        Text{ "What happened to Sheik?", /*french*/ "Donc... Qu'est-ce qui arrive avec Sheik?",
              /*spanish*/ "¿Qué la habrá pasado a Sheik?" },
    });

    hintTable[RHT_JUNK25] = HintText::Junk({
        // obscure text
        Text{ "L2P @.", /*french*/ "Arrête de lire les indices et joue comme un grand, @.",
              /*spanish*/ "Mira que eres novato, @." },
    });

    hintTable[RHT_JUNK26] = HintText::Junk({
        // obscure text
        Text{ "I've heard you can cheat at Sploosh Kaboom.",
              /*french*/ "Selon moi, il y a une carte aux trésors à Mercantîle... Duh!",
              /*spanish*/ "He oído por ahí que puedes hacer trampa en el Sploosh Kaboom." },
    });

    hintTable[RHT_JUNK27] = HintText::Junk({
        // obscure text
        Text{ "I'm Lonk from Pennsylvania.", /*french*/ "Je suis Lonk, le héros de Pennsylvanie!",
              /*spanish*/ "Soy Lonk, de Pensilvania." },
    });

    hintTable[RHT_JUNK28] = HintText::Junk({
        // obscure text
        Text{ "I bet you'd like to have more bombs.", /*french*/ "Je parie que tu veux plus de bombes.",
              /*spanish*/ "Me apuesto a que quisieras tener más bombas." },
    });

    hintTable[RHT_JUNK29] = HintText::Junk({
        // obscure text
        Text{ "When all else fails, use Fire.", /*french*/ "Quand rien ne marche, utilise le feu.",
              /*spanish*/ "Cuando nada funcione, usa el fuego." },
    });

    hintTable[RHT_JUNK30] = HintText::Junk({
        // obscure text
        Text{ "Here's a hint, @. Don't be bad.", /*french*/ "Selon moi, la #Triforce# n'est pas dans le jeu... Duh!",
              /*spanish*/ "Aquí tienes una pista, @: deja de ser manco." },
    });

    hintTable[RHT_JUNK31] = HintText::Junk({
        // obscure text
        Text{ "Game Over. Return of Ganon.", /*french*/ "Partie terminée. RETour de Ganon.",
              /*spanish*/ "Fin de la partida. El regreso de Ganon." },
    });

    hintTable[RHT_JUNK32] = HintText::Junk({
        // obscure text
        Text{ "May the way of the Hero lead to the Triforce.",
              /*french*/ "Que le chemin du héros te mène à la Triforce.",
              /*spanish*/ "Puede que la senda del héroe te lleve hacia la Trifuerza." },
    });

    hintTable[RHT_JUNK33] = HintText::Junk({
        // obscure text
        Text{ "Can't find an item? Scan an Amiibo.", /*french*/ "Tu cherches de quoi? Utilise un Amiibo!",
              /*spanish*/ "¿No encuentras algo? Escanea un amiibo." },
    });

    hintTable[RHT_JUNK34] = HintText::Junk({
        // obscure text
        Text{ "They say this game has just a few glitches.",
              /*french*/ "Selon moi, ce jeu est complètement exempt de glitchs.",
              /*spanish*/ "Dicen que este juego apenas tiene glitches." },
    });

    hintTable[RHT_JUNK35] = HintText::Junk({
        // obscure text
        Text{ "BRRING BRRING This is Ulrira. Wrong number?",
              /*french*/ "DRING DRING!! Pépé le Ramollo à l'appareil... Quoi? Faux numéro?",
              /*spanish*/ "¡Ring! ¡Ring! Al habla Ulrira. ¿Me he equivocado de número?" },
    });

    hintTable[RHT_JUNK36] = HintText::Junk({
        // obscure text
        Text{ "Tingle Tingle Kooloo Limpah!", /*french*/ "Tingle! Tingle! Kooloolin... Pah!",
              /*spanish*/ "Tingle, Tingle, Kurulín... ¡PA!" },
    });

    hintTable[RHT_JUNK37] = HintText::Junk({
        // obscure text
        Text{ "L is real 2401", /*french*/ "L is real 2401", /*spanish*/ "L es real 2401." },
    });

    hintTable[RHT_JUNK38] = HintText::Junk({
        // obscure text
        Text{ "They say that Ganondorf will appear in the next Mario Tennis.",
              /*french*/ "Selon moi, Ganondorf sera la nouvelle recrue dans Mario Tennis.",
              /*spanish*/ "Según dicen, Ganondorf estará en el próximo Mario Tennis." },
    });

    hintTable[RHT_JUNK39] = HintText::Junk({
        // obscure text
        Text{ "Medigoron sells the earliest Breath of the Wild demo.",
              /*french*/ "Selon moi, Medigoron vend une démo de #Breath of the Wild#.",
              /*spanish*/ "Medigoron vende la primera demo del Breath of the Wild." },
    });

    hintTable[RHT_JUNK40] = HintText::Junk({
        // obscure text
        Text{ "Can you move me? I don't get great service here.",
              /*french*/ "Peux-tu me déplacer? J'ai pas une bonne réception ici.",
              /*spanish*/ "¿Puedes llevarme a otro lado? Aquí nadie me presta atención." },
    });

    hintTable[RHT_JUNK41] = HintText::Junk({
        // obscure text
        Text{ "They say if you use Strength on the truck, you can find Mew.",
              /*french*/ "Selon moi, #Mew# se trouve dessous le camion... Duh!",
              /*spanish*/ "Según dicen, puedes hallar un Mew usando Fuerza contra el camión de Ciudad Carmín." },
    });

    hintTable[RHT_JUNK42] = HintText::Junk({
        // obscure text
        Text{ "I'm a helpful hint Gossip Stone!^See, I'm helping.",
              /*french*/ "Salut! Je suis une pierre de bons conseils!^Tiens, tu vois? J'aide bien, hein?",
              /*spanish*/ "Soy una Piedra Sheikah muy útil.^¡Mira cómo te ayudo!" },
    });

    hintTable[RHT_JUNK43] = HintText::Junk({
        // obscure text
        Text{
            "Dear @, please come to the castle. I've baked a cake for you.&Yours truly, Princess Zelda.", /*french*/
            "Mon très cher @:&Viens vite au château, je t'ai préparé&un délicieux gâteau...^À bientôt, Princesse Zelda",
            /*spanish*/
            "Querido @: Por favor, ven al castillo. He hecho una tarta para ti.&Sinceramente tuya: Princesa Zelda." },
    });

    hintTable[RHT_JUNK44] = HintText::Junk({
        // obscure text
        Text{ "They say all toasters toast toast.", /*french*/ "Selon moi, les grille-pains grillent du pain.",
              /*spanish*/ "Según dicen, todas las tostadoras tostan tostadas tostadas." },
    });

    hintTable[RHT_JUNK45] = HintText::Junk({
        // obscure text
        Text{ "You thought it would be a useful hint, but it was me, junk hint!",
              /*french*/ "Tu t'attendais à un bon indice... Mais c'était moi, un mauvais indice!", /*spanish*/
              "Je... Creeías que iba a ser una piedra de utilidad, ¡pero no, era yo, la piedra de la agonía!" },
    });

    hintTable[RHT_JUNK46] = HintText::Junk({
        // obscure text
        Text{ "They say that quest guidance can be found at a talking rock.",
              /*french*/ "Selon moi, des #indices# se trouvent auprès d'une pierre parlante... Duh!",
              /*spanish*/ "Según dicen, puedes consultarle ayuda a rocas parlanchinas." },
    });

    hintTable[RHT_JUNK47] = HintText::Junk({
        // obscure text
        Text{ "They say that the final item you're looking for can be found somewhere in Hyrule.",
              /*french*/ "Selon moi, le #dernier objet# se trouve quelque part dans Hyrule... Duh!",
              /*spanish*/ "Según dicen, el último objeto que te falte puede estar en cualquier rincón de Hyrule." },
    });

    hintTable[RHT_JUNK48] = HintText::Junk({
        // obscure text
        Text{ "Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.",
              /*french*/ "Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.",
              /*spanish*/ "Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep." },
    });

    hintTable[RHT_JUNK49] = HintText::Junk({
        // obscure text
        Text{ "They say that Barinade fears Deku Nuts.", /*french*/ "Selon moi, Barinade a la frousse des noix Mojo.",
              /*spanish*/ "Según dicen, lo que más teme a Barinade son las nueces deku." },
    });

    hintTable[RHT_JUNK50] = HintText::Junk({
        // obscure text
        Text{ "They say that Flare Dancers do not fear Goron-crafted blades.",
              /*french*/ "Selon moi, le danse-flamme n'a pas peur des armes de Goron.",
              /*spanish*/ "Según dicen, los Bailafuegos no le temen a las armas forjadas por Gorons." },
    });

    hintTable[RHT_JUNK51] = HintText::Junk({
        // obscure text
        Text{ "They say that Morpha is easily trapped in a corner.",
              /*french*/ "Selon moi, Morpha est facilement coincé.",
              /*spanish*/ "Según dicen, puedes atrapar a Morpha con facilidad en una esquina." },
    });

    hintTable[RHT_JUNK52] = HintText::Junk({
        // obscure text
        Text{ "They say that Bongo Bongo really hates the cold.",
              /*french*/ "Selon moi, Bongo Bongo a facilement froid aux doigts.",
              /*spanish*/ "Según dicen, Bongo Bongo odia a muerte el frío." },
    });

    hintTable[RHT_JUNK53] = HintText::Junk({
        // obscure text
        Text{ "They say that your sword is most powerful when you put it away.",
              /*french*/ "Selon moi, ton épée est à pleine puissance quand tu la rengaines.",
              /*spanish*/ "Según dicen, tu espada se vuelve más poderosa si la guardas." },
    });

    hintTable[RHT_JUNK54] = HintText::Junk({
        // obscure text
        Text{ "They say that bombing the hole Volvagia last flew into can be rewarding.",
              /*french*/ "Selon moi, le trou où se creuse Volvagia est vulnérable aux bombes.", /*spanish*/
              "Según dicen, trae buena suerte colocar una bomba en el último agujero de donde salió Volvagia." },
    });

    hintTable[RHT_JUNK55] = HintText::Junk({
        // obscure text
        Text{ "They say that invisible ghosts can be exposed with Deku Nuts.",
              /*french*/ "Selon moi, des fantômes invisibles apparaissent avec des noix Mojo.",
              /*spanish*/ "Según dicen, puedes exponer a los espectros invisibles con nueces deku." },
    });

    hintTable[RHT_JUNK56] = HintText::Junk({
        // obscure text
        Text{ "They say that the real Phantom Ganon is bright and loud.",
              /*french*/ "Selon moi, le vrai spectre de Ganon est clair et bruyant.",
              /*spanish*/ "Según dicen, el verdadero Ganon Fantasma es brillante y ruidoso." },
    });

    hintTable[RHT_JUNK57] = HintText::Junk({
        // obscure text
        Text{ "They say that walking backwards is very fast.",
              /*french*/ "Selon moi, tu fais marche arrière très rapidement pour un héros.",
              /*spanish*/ "Según dicen, es más rápido caminar hacia atrás." },
    });

    hintTable[RHT_JUNK58] = HintText::Junk({
        // obscure text
        Text{
            "They say that leaping above the Market entrance enriches most children.",
            /*french*/ "Selon moi, les enfants riches se pavanent en haut du pont-levis.",
            /*spanish*/ "Según dicen, saltar por las cadenas a la entrada de la plaza enriquece a muchos chiquillos." },
    });

    hintTable[RHT_JUNK59] = HintText::Junk({
        // obscure text
        Text{ "They say Ingo is not very good at planning ahead.",
              /*french*/ "Selon moi, Ingo ne fait pas un très bon geôlier.",
              /*spanish*/ "Según dicen, a Ingo no se le da especialmente bien planificar con antelación." },
    });

    hintTable[RHT_JUNK60] = HintText::Junk({
        // obscure text
        Text{ "You found a spiritual Stone! By which I mean, I worship Nayru.",
              /*french*/ "Vous avez trouvé une Pierre Ancestrale! En effet, je vénère la déesse Hylia.",
              /*spanish*/ "¡Has encontrado una piedra espiritual! Es que le rindo culto a Nayru..." },
    });

    hintTable[RHT_JUNK61] = HintText::Junk({
        // obscure text
        Text{ "They say that a flying strike with a Deku Stick is no stronger than a grounded one.",
              /*french*/ "Selon moi, un coup de bâton sauté n'est pas meilleur qu'au sol.",
              /*spanish*/ "Según dicen, los golpes aéreos con palos deku son tan fuertes como los normales." },
    });

    hintTable[RHT_JUNK62] = HintText::Junk({
        // obscure text
        Text{ "Open your eyes.^Open your eyes.^Wake up, @.",
              /*french*/ "Réveille-toi...^Réveille-toi.^Ouvre les yeux, @.",
              /*spanish*/ "Abre los ojos...^Abre los ojos...^Despierta, @..." },
    });

    hintTable[RHT_JUNK63] = HintText::Junk({
        // obscure text
        Text{ "They say that the Nocturne of Shadow can bring you very close to Ganon.",
              /*french*/ "Selon moi, le nocturne de l'ombre peut t'amener très près de Ganon.",
              /*spanish*/ "Según dicen, el Nocturno de la sombra te puede acercar mucho a Ganon." },
    });

    hintTable[RHT_JUNK64] = HintText::Junk({
        // obscure text
        Text{ "They say that Twinrova always casts the same spell the first three times.",
              /*french*/ "Selon moi, Twinrova lance toujours les mêmes trois premiers sorts.",
              /*spanish*/ "Según dicen, Birova siempre lanza el mismo hechizo las tres primeras veces." },
    });

    hintTable[RHT_JUNK65] = HintText::Junk({
        // obscure text
        Text{ "They say that the nightly builds may be unstable.",
              /*french*/ "Selon moi, les \"nightly builds\" peuvent être instables.",
              /*spanish*/ "Según dicen, las últimas nightlies pueden llegar a ser algo inestables." },
    });

    hintTable[RHT_JUNK66] = HintText::Junk({
        // obscure text
        Text{ "You're playing a Randomizer. I'm randomized!^Here's a random number:  #4#.&Enjoy your Randomizer!",
              /*french*/ "Tu joues à un randomizer. Je suis aléatoire!^Voici un nombre aléatoire: #4#.&Bonne partie!",
              /*spanish*/
              "¡Estás jugando un Randomizer! ¡Yo también estoy aleatorizada!^Aquí tienes un número aleatorio:  "
              "#8#.&¡Diviértete!" },
    });

    hintTable[RHT_JUNK67] = HintText::Junk({
        // obscure text
        Text{ "They say Ganondorf's bolts can be reflected with glass or steel.",
              /*french*/ "Selon moi, les éclairs de Ganon se reflètent sur l'acier et le verre.",
              /*spanish*/ "Según dicen, puedes reflejar las esferas de energía de Ganondorf con cristal y acero." },
    });

    hintTable[RHT_JUNK68] = HintText::Junk({
        // obscure text
        Text{ "They say Ganon's tail is vulnerable to nuts, arrows, swords, explosives, hammers...^...sticks, seeds, "
              "boomerangs...^...rods, shovels, iron balls, angry bees...",
              /*french*/
              "Selon moi, la queue de Ganon est vulnérable aux noix, flèches, épées, bombes, marteaux...^...bâtons, "
              "graines, boomerangs...^...baguettes, pelles, boulets de fer, abeilles enragées...",
              /*spanish*/
              "Según dicen, la cola de Ganon es vulnerable a nueces, flechas, espadas, explosivos, "
              "martillos...^...palos, semillas, bumeráns...^...cetros, palas, bolas de hierro, abejas..." },
    });

    hintTable[RHT_JUNK69] = HintText::Junk({
        // obscure text
        Text{
            "They say that you're wasting time reading this hint, but I disagree. Talk to me again!",
            /*french*/ "Selon moi... tu sais quoi? Parle-moi encore, et je te le dirai!", /*spanish*/
            "Según dicen, pierdes el tiempo en leer esta pista, pero no pienso igual. ¡Vuelve a hablarme, ya verás!" },
    });

    hintTable[RHT_JUNK70] = HintText::Junk({
        // obscure text
        Text{ "They say Ganondorf knows where to find the instrument of his doom.",
              /*french*/ "Selon moi, Ganondorf sait où il a caché son point faible.",
              /*spanish*/ "Según dicen, Ganondorf sabe dónde hallar el instrumento de su perdición." },
    });

    hintTable[RHT_JUNK71] = HintText::Junk({
        // obscure text
        Text{ "I heard @ is pretty good at Zelda.", /*french*/ "Apparemment, @ est super bon à Zelda.",
              /*spanish*/ "He oído que a @ se le dan muy bien los Zelda." },
    });

    hintTable[RHT_JUNK72] = HintText::Junk({
        // obscure text
        Text{ "Hi @, we've been trying to reach you about your car's extended warranty. ",
              /*french*/ "Bonjour, @. Vous avez une voiture? Vous savez, nous offrons des assurances abordables...",
              /*spanish*/
              "Buenas, @. Le llamamos para ofrecerle un nuevo seguro de hogar que puede pagar en cómodos plazos, sin "
              "intereses ni comisiones." },
    });

    hintTable[RHT_JUNK73] = HintText::Junk({
        // obscure text
        Text{ "They say that the best weapon against Iron Knuckles is item 176.",
              /*french*/ "Selon moi, les hache-viandes sont vulnérables contre l'objet 176.",
              /*spanish*/ "Según dicen, la mejor arma para enfrentarse a los Nudillos de hierro es el objeto 176." },
    });

    hintTable[RHT_JUNK74] = HintText::Junk({
        // obscure text
        Text{ "They say that it's actually possible to beat the running man.",
              /*french*/ "Selon moi, il est possible de battre le coureur.&Donc, tu prends ton arc, et...",
              /*spanish*/ "Según dicen, con mucha perseverancia puedes ganarle al corredor con la capucha de conejo." },
    });

    hintTable[RHT_JUNK75] = HintText::Junk({
        // obscure text
        Text{ "They say that the stone-cold guardian of the Well is only present during work hours.",
              /*french*/ "Selon moi, le gardien de pierre du Puits quitte le soir pour aller se coucher.",
              /*spanish*/ "Según dicen, la inmensa roca que bloquea el pozo solo trabaja en horas laborales." },
    });

    hintTable[RHT_JUNK76] = HintText::Junk({
        // obscure text
        Text{
            "They say this hint makes more sense in other languages.",
            /*french*/ "Selon moi, ces indices auraient pu être mieux traduits... Duh!",
            /*spanish*/ "Según dicen, esta pista revela algo de vital importancia si cambias el idioma del juego..." },
    });

    hintTable[RHT_JUNK77] = HintText::Junk({
        // obscure text
        Text{ "BOK? No way.", /*french*/ "BD'accord? Hors de question.", /*spanish*/ "¿BVale? Ni hablar." },
    });

    // ^ junk hints above are from 3drando
    // v junk hints below are new to soh rando

#define HINT_TEXT_NEEDS_TRANSLATION_FR \
    "Erreur 0x69a504:&Traduction manquante^C'est de la faute à Purple Hato!&J'vous jure!"

    hintTable[RHT_JUNK78] = HintText::Junk({
        //obscure text
        Text{"They say blarg...^...or at least briaguya does.", /*french*/"Tout ce que j'ai à dire, c'est blarg...^... 'fin c'est plutôt ce que briaguya dirait.", /*spanish*/"blarg"},
    });

    hintTable[RHT_JUNK79] = HintText::Junk({
        //obscure text
        Text{"They say this peace is what all true warriors strive for.", /*french*/"Selon moi, cette paix est ce pour quoi luttent tous les vrais guerriers.", /*spanish*/"blarg"},
    });

    hintTable[RHT_JUNK80] = HintText::Junk({
        //obscure text
        Text{"They say this ship is what all true gamers strive for.", /*french*/"Selon moi, cette version du port est ce pour quoi luttent tous les vrais gamers.", /*spanish*/"blarg"},
    });

    hintTable[RHT_JUNK81] = HintText::Junk({
        //obscure text
        Text{"They say that Glowsticks can be found in the Raveyard.", /*french*/"On peut trouver des Bâtons Lumineux sur le dancefloor du cimetière.", /*spanish*/"blarg"},
    });

    hintTable[RHT_JUNK_WTC_1] = HintText::Junk({
        Text{ "They say %rthere are no more than 18&people on this island.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_2] = HintText::Junk({
        Text{ "They say I am one yet many", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_3] = HintText::Junk({
        Text{ "They say its all in the name of guiding&humanity down the right path.", HINT_TEXT_NEEDS_TRANSLATION_FR,
              "blarg" },
    });

    hintTable[RHT_JUNK_WTC_4] = HintText::Junk({
        Text{ "They say \"Repetition requested\"", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_5] = HintText::Junk({
        Text{ "They say %rThe red tells only the truth!", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_6] = HintText::Junk({
        Text{ "They say good tidings to you^my traitorous @", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_7] = HintText::Junk({
        Text{ "They say when the seagulls cried,&none were left alive.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_7] = HintText::Junk({
        Text{ "They say when the seagulls cried,&none were left alive.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_8] = HintText::Junk({
        Text{ "They say she is lying with the red letters!", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_9] = HintText::Junk({
        Text{ "They say we'll meet again,&when something else cries.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_10] = HintText::Junk({
        Text{ "They say \"Forgive me, but-^Your script will not be used.&....After all...^The one writing the rest of "
              "the script...&will be me.\"",
              HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_11] = HintText::Junk({
        Text{ "They say tea is best enjoyed...^\"\"With your fellow monsters.\"\"", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_WTC_12] = HintText::Junk({
        Text{ "They say I shall make you some black tea. With my own hands, not magic.", HINT_TEXT_NEEDS_TRANSLATION_FR,
              "blarg" },
    });

    hintTable[RHT_JUNK_SEI_1] = HintText::Junk({
        Text{ "They say you know I've kiboshed before...^and I will kibosh again.", HINT_TEXT_NEEDS_TRANSLATION_FR,
              "blarg" },
    });

    hintTable[RHT_JUNK_SEI_2] = HintText::Junk({
        Text{ "They say if relationship @ walks through that door,^they will KILL independent @.",
              HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_3] = HintText::Junk({
        Text{ "They say you gotta have the BIG Salad.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_4] = HintText::Junk({
        Text{ "They say it's a festivus miracle", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_5] = HintText::Junk({
        Text{ "They say there are no houses in Tuscany to rent.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_6] = HintText::Junk({
        Text{ "They say my last boyfriend had a real&Kroner comprehension problem.", HINT_TEXT_NEEDS_TRANSLATION_FR,
              "blarg" },
    });

    hintTable[RHT_JUNK_SEI_7] = HintText::Junk({
        Text{ "They say it's a festivus miracle.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_8] = HintText::Junk({
        Text{ "They say Louis quit the importing&to focus on the exporting.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_9] = HintText::Junk({
        Text{ "They say no thanks, I can't drink coffee&late at night, it keeps me up.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    
    hintTable[RHT_JUNK_SEI_10] = HintText::Junk({
        Text{ "They say it's not a lie if you believe it.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_11] = HintText::Junk({
        Text{ "They say there was a second spitter.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_12] = HintText::Junk({
        Text{ "They say there was a second spitter.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_13] = HintText::Junk({
        Text{ "They say the jerk store called,^they're running out of YOU.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    
    hintTable[RHT_JUNK_SEI_14] = HintText::Junk({
        Text{ "They say when you look annoyed all the time,&people thing you are busy.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_15] = HintText::Junk({
        Text{ "They say when you look annoyed all the time,&people think you are busy.", HINT_TEXT_NEEDS_TRANSLATION_FR,
              "blarg" },
    });

    hintTable[RHT_JUNK_SEI_16] = HintText::Junk({
        Text{ "They say he fires people like its a bodily function.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_17] = HintText::Junk({
        Text{ "They say he threatened to move the ship to New Jersey&just to upset people.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_18] = HintText::Junk({
        Text{ "They say there was significant shrinkage.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_19] = HintText::Junk({
        Text{ "They say if it wasn't for the toilet there'd be no books.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_20] = HintText::Junk({
        Text{ "They say if it wasn't for the toilet there'd be no books.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_21] = HintText::Junk({
        Text{ "They say don't trust men in capes.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    
    hintTable[RHT_JUNK_SEI_22] = HintText::Junk({
        Text{ "They say @'s uncle works for Nintendo.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_23] = HintText::Junk({
        Text{ "They say @'s stole the marble rye.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_24] = HintText::Junk({
        Text{ "They say there is no better harmony&than the black and white cookie.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_25] = HintText::Junk({
        Text{ "They say @ hasn't vomited since 1983.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_26] = HintText::Junk({
        Text{ "They say you gotta have the early bird special.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_27] = HintText::Junk({
        Text{ "They say a donation has been made in your name&to the human fund.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_28] = HintText::Junk({
        Text{ "They say you want to be my latex salesman.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SEI_29] = HintText::Junk({
        Text{ "They say if every instinct you have is wrong...^... then the opposite would have to be right.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_OTR_MEANS_1] = HintText::Junk({
        Text{ "They say OTR stands for&Over the Rainbow", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_2] = HintText::Junk({
        Text{ "They say that OTR stands for&Onions, Tomatoes, and Radishes", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_3] = HintText::Junk({
        Text{ "They say that OTR stands for&Ocarina of Time Resources", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_4] = HintText::Junk({
        Text{ "They say that OTR stands for&Over the Road", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_5] = HintText::Junk({
        Text{ "They say that OTR stands for&Off the Record", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_6] = HintText::Junk({
        Text{ "They say that OTR stands for&Office of Tax and Revenue", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_7] = HintText::Junk({
        Text{ "They say OTR stands for&Over the Rainbow", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_8] = HintText::Junk({
        Text{ "They say that OTR stands for&Office of Trade Relations", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_9] = HintText::Junk({
        Text{ "They say that OTR stands for&Original Theatrical Release", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_10] = HintText::Junk({
        Text{ "They say that OTR stands for&Operational Test Requirement", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_11] = HintText::Junk({
        Text{ "They say that OTR stands for&Operational Trouble Report", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_12] = HintText::Junk({
        Text{ "They say that OTR stands for&Oxygen Transmission Rate", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_13] = HintText::Junk({
        Text{ "They say that OTR stands for&One Touch Recording", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_14] = HintText::Junk({
        Text{ "They say that OTR stands for&Olympic Torch Relay", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_15] = HintText::Junk({
        Text{ "They say that OTR stands for&Off the Rack", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_16] = HintText::Junk({
        Text{ "They say that OTR stands for&Overhead Transfer Rate", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_17] = HintText::Junk({
        Text{ "They say that OTR stands for&Operational TurnaRound", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_18] = HintText::Junk({
        Text{ "They say that OTR stands for&Opportunity to Recall", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_19] = HintText::Junk({
        Text{ "They say that OTR stands for&Operability Test Report", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_20] = HintText::Junk({
        Text{ "They say that OTR stands for&Overall Tuning Range", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_21] = HintText::Junk({
        Text{ "They say that OTR stands for&One Time Requisition", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_22] = HintText::Junk({
        Text{ "They say that OTR stands for&Oblivious to Reality", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_23] = HintText::Junk({
        Text{ "They say that OTR stands for&On the Run", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_24] = HintText::Junk({
        Text{ "They say that OTR stands for&On Time Reporting", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_25] = HintText::Junk({
        Text{ "They say that OTR stands for&Order to Receipt", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_26] = HintText::Junk({
        Text{ "They say that OTR stands for&Other Terrestrial Radio", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_27] = HintText::Junk({
        Text{ "They say that OTR stands for&On Target Reports", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_28] = HintText::Junk({
        Text{ "They say that OTR stands for&One Time Repair", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_29] = HintText::Junk({
        Text{ "They say that OTR stands for&Own the Room", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });
    hintTable[RHT_JUNK_OTR_MEANS_30] = HintText::Junk({
        Text{ "They say that OTR stands for&Online Text Repository", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_1] = HintText::Junk({
        Text{ "They say %gKenix%w isn't a developer...^...Just a PR guy", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_2] = HintText::Junk({
        Text {"They say... No", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg"},
     });

    hintTable[RHT_JUNK_MISC_3] = HintText::Junk({ 
        Text{ "They say BIG RIGS: OTR", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" } 
    });

    hintTable[RHT_JUNK_MISC_4] = HintText::Junk({
        Text{ "They say you wanted to see me %pMr. Kenix%w?", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_5] = HintText::Junk({
        Text{ "They say Louis once saw an&equals not get set equals", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_6] = HintText::Junk({
        Text{ "They say only you can find your rom.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_7] = HintText::Junk({
        Text{ "They say ZAPD is good software.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_8] = HintText::Junk({
        Text{ "They say you can encounter&a parascode in tall grass.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_9] = HintText::Junk({
        Text{ "They say the ship sails on March 32nd.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_10] = HintText::Junk({
        Text{ "They say bombing dodongos is fun.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_11] = HintText::Junk({
        Text{ "They say shopkeepers don't give credits.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_12] = HintText::Junk({
        Text{ "They say shopkeepers don't give credits.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_13] = HintText::Junk({
        Text{ "They say Malon is glitched.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_14] = HintText::Junk({
        Text{ "They say do I look like I know&what a DList is?", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_15] = HintText::Junk({
        Text{ "They say do I look like I know&what an AList is?", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_16] = HintText::Junk({
        Text{ "They say the king drinks enthusiastically", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_MISC_17] = HintText::Junk({
        Text{ "They say Rubies are on the path to&Lamp Oil, Rope, and Bombs", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SG_1] = HintText::Junk({
        Text{ "They say %rError. Human is dead, mismatch.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SG_2] = HintText::Junk({
        Text{ "They say this is the choice of the&steins gate.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SG_3] = HintText::Junk({
        Text{ "They say el psy kongroo.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SG_4] = HintText::Junk({
        Text{ "They say tutturu~.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });


    hintTable[RHT_JUNK_SG_5] = HintText::Junk({
        Text{ "They say im not Christina!.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SG_6] = HintText::Junk({
        Text{ "They say you know where to find an IBN5100.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SG_7] = HintText::Junk({
        Text{ "They say when you're on a chicken bender&grab a box of chicken tenders.", HINT_TEXT_NEEDS_TRANSLATION_FR, "blarg" },
    });

    hintTable[RHT_JUNK_SG_8] = HintText::Junk({
        Text{ "Juicy Chicken #1! Wow!.", HINT_TEXT_NEEDS_TRANSLATION_FR,
              "blarg" },
    });

    /*--------------------------
    |     DUNGEON HINT TEXT    |
    ---------------------------*/

    hintTable[RHT_DEKU_TREE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "an ancient tree", /*french*/ "le vieil arbre", /*spanish*/ "un ancestral árbol" },
        },
        {},
        // clear text
        Text{ "Deku Tree", /*french*/ "l'Arbre Mojo", /*spanish*/ "el Gran Árbol Deku" });

    hintTable[RHT_DODONGOS_CAVERN] = HintText::DungeonName(
        {
            // obscure text
            Text{ "an immense cavern", /*french*/ "l'immense caverne", /*spanish*/ "una descomunal cueva" },
        },
        {},
        // clear text
        Text{ "Dodongo's Cavern", /*french*/ "la Caverne Dodongo", /*spanish*/ "la Cueva de los Dodongos" });

    hintTable[RHT_JABU_JABUS_BELLY] = HintText::DungeonName(
        {
            // obscure text
            Text{ "the belly of a deity", /*french*/ "le ventre d'un gardien",
                  /*spanish*/ "la tripa de cierta deidad" },
        },
        {},
        // clear text
        Text{ "Jabu-Jabu's Belly", /*french*/ "le Ventre de Jabu-Jabu", /*spanish*/ "tripa de Jabu-Jabu" });

    hintTable[RHT_FOREST_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a deep forest", /*french*/ "la profonde forêt", /*spanish*/ "las profundidades del bosque" },
        },
        {},
        // clear text
        Text{ "Forest Temple", /*french*/ "le Temple de la Forêt", /*spanish*/ "el Templo del Bosque" });

    hintTable[RHT_FIRE_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a high mountain", /*french*/ "la grande montagne", /*spanish*/ "una alta montaña" },
        },
        {},
        // clear text
        Text{ "Fire Temple", /*french*/ "le Temple du Feu", /*spanish*/ "el Templo del Fuego" });

    hintTable[RHT_WATER_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a vast lake", /*french*/ "le vaste lac", /*spanish*/ "un lago inmenso" },
        },
        {},
        // clear text
        Text{ "Water Temple", /*french*/ "le Temple de l'Eau", /*spanish*/ "el Templo del Agua" });

    hintTable[RHT_SPIRIT_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "the goddess of the sand", /*french*/ "la déesse des sables", /*spanish*/ "la diosa de las arenas" },
        },
        {},
        // clear text
        Text{ "Spirit Temple", /*french*/ "le Temple de l'Esprit", /*spanish*/ "el Templo del Espíritu" }

    );

    hintTable[RHT_SHADOW_TEMPLE] = HintText::DungeonName(
        {
            // obscure text
            Text{ "the house of the dead", /*french*/ "la maison des morts", /*spanish*/ "la casa de la muerte" },
        },
        {},
        // clear text
        Text{ "Shadow Temple", /*french*/ "le Temple de l'Ombre", /*spanish*/ "el Templo de las Sombras" });

    hintTable[RHT_ICE_CAVERN] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a frozen maze", /*french*/ "le dédale glacé", /*spanish*/ "un gélido laberinto" },
        },
        {},
        // clear text
        Text{ "Ice Cavern", /*french*/ "la caverne de glace", /*spanish*/ "la caverna de hielo" });

    hintTable[RHT_BOTTOM_OF_THE_WELL] = HintText::DungeonName(
        {
            // obscure text
            Text{ "a shadow\'s prison", /*french*/ "la prison d'une ombre", /*spanish*/ "la prisión de las sombras" },
        },
        {},
        // clear text
        Text{ "Bottom of the Well", /*french*/ "le fonds du Puits", /*spanish*/ "el fondo del pozo" });

    hintTable[RHT_GERUDO_TRAINING_GROUND] = HintText::DungeonName(
        {
            // obscure text
            Text{ "the test of thieves", /*french*/ "l'épreuve des voleurs", /*spanish*/ "la prueba de las bandidas" },
        },
        {},
        // clear text
        Text{ "Gerudo Training Grounds", /*french*/ "le Gymnase Gerudo",
              /*spanish*/ "el Centro de Instrucción Gerudo" });

    hintTable[RHT_GANONS_CASTLE] = HintText::DungeonName(
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

    hintTable[RHT_QUEEN_GOHMA] = HintText::Boss({
                       // obscure text
                       Text{"the #Parasitic Armored Arachnid# holds", /*french*/"le #monstre insectoïde géant# possède", /*spanish*/"el #arácnido parasitario acorazado# porta"},
                     }, {},
                       //clear text
                       Text{"#Queen Gohma# holds", /*french*/"la #Reine Gohma# possède", /*spanish*/"la #Reina Goma# porta"});

    hintTable[RHT_KING_DODONGO] = HintText::Boss({
                       //obscure text
                       Text{"the #Infernal Dinosaur# holds", /*french*/"le #dinosaure infernal# possède", /*spanish*/"el #dinosaurio infernal# porta"},
                     }, {},
                       //clear text
                       Text{"#King Dodongo# holds", /*french*/"le #Roi Dodongo# possède", /*spanish*/"el #Rey Dodongo# porta"});

    hintTable[RHT_BARINADE] = HintText::Boss({
                       //obscure text
                       Text{"the #Bio-Electric Anemone# holds", /*french*/"l'#anémone bioélectrique# possède", /*spanish*/"la #anémona bioeléctrica# porta"},
                     }, {},
                       //clear text
                       Text{"#Barinade# holds", /*french*/"#Barinade# possède", /*spanish*/"#Barinade# porta"});

    hintTable[RHT_PHANTOM_GANON] = HintText::Boss({
                       //obscure text
                       Text{"the #Evil Spirit from Beyond# holds", /*french*/"l'#esprit maléfique de l'au-delà# possède", /*spanish*/"el #espíritu maligno de ultratumba# porta"},
                     }, {},
                       //clear text
                       Text{"#Phantom Ganon# holds", /*french*/"#Ganon Spectral# possède", /*spanish*/"#Ganon Fantasma# porta"});

    hintTable[RHT_VOLVAGIA] = HintText::Boss({
                       //obscure text
                       Text{"the #Subterranean Lava Dragon# holds", /*french*/"le #dragon des profondeurs# possède", /*spanish*/"el #dragón de lava subterráneo# porta"},
                     }, {},
                       //clear text
                       Text{"#Volvagia# holds", /*french*/"#Volvagia# possède", /*spanish*/"#Volvagia# porta"});

    hintTable[RHT_MORPHA] = HintText::Boss({
                       //obscure text
                       Text{"the #Giant Aquatic Amoeba# holds", /*french*/"l'#amibe aquatique géante# possède", /*spanish*/"la #ameba acuática gigante# porta"},
                     }, {},
                       //clear text
                       Text{"#Morpha# holds", /*french*/"#Morpha# possède", /*spanish*/"#Morpha# porta"});

    hintTable[RHT_BONGO_BONGO] = HintText::Boss({
                       //obscure text
                       Text{"the #Phantom Shadow Beast# holds", /*french*/"le #monstre de l'ombre# possède", /*spanish*/"la #alimaña oscura espectral# porta"},
                     }, {},
                       //clear text
                       Text{"#Bongo Bongo# holds", /*french*/"#Bongo Bongo# possède", /*spanish*/"#Bongo Bongo# porta"});

    hintTable[RHT_TWINROVA] = HintText::Boss({
                       //obscure text
                       Text{"the #Sorceress Sisters# hold", /*french*/"#les sorcières jumelles# possède", /*spanish*/"las #hermanas hechiceras# portan"},
                     }, {},
                       //clear text
                       Text{"#Twinrova# holds", /*french*/"#Twinrova# possède", /*spanish*/"#Birova# porta"});
    //
    // [RHT_LINKS_POCKET_BOSS] = HintText::Boss({
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

    hintTable[RHT_BRIDGE_OPEN_HINT] = HintText::Bridge({
        // obscure text
        Text{ "The awakened ones have #already created a bridge# to the castle where the evil dwells.",
              /*french*/ "Les êtres de sagesse ont#déjà créé un pont# vers le repaire du mal.",
            /*spanish*/ "Los sabios #ya habrán creado un puente#&al castillo, de donde emana el mal.",
              /*german*/"Die Weisen haben #bereits&eine Brücke zum Portal von&Ganons Schloss gelegt#..." },
    });

    hintTable[RHT_BRIDGE_VANILLA_HINT] = HintText::Bridge({
        // obscure text
        Text{ "The awakened ones require the #Shadow and Spirit Medallions# as well as the #Light Arrows#.",
              /*french*/
              "Les êtres de sagesse attendront le héros muni des #Médaillons de l'Ombre et l'Esprit# et des #Flèches de Lumière#.",
              /*spanish*/
              "Los sabios aguardarán a que el héroe obtenga tanto el #Medallón de las Sombras y el del Espíritu# junto "
            "a la #flecha de luz#.",
              /*german*/"Die Weisen werden darauf warten, dass der Held das #Amulett des Schattens, Amulett der Geister# und die #Licht-Pfeile# sammelt." },
    });

    hintTable[RHT_BRIDGE_STONES_HINT] = HintText::Bridge({
        // obscure text                                                  singular        plural
        Text{
            "The awakened ones will await for the Hero to collect #%d |Spiritual Stone|Spiritual Stones|#.",
            /*french*/ "Les êtres de sagesse attendront le héros muni de #%d |Pierre Ancestrale|Pierres Ancestrales|#.",
            /*spanish*/ "Los sabios aguardarán a que el héroe&obtenga #%d |piedra espiritual|piedras espirituales|#.",
            /*german*/"Die Weisen werden darauf warten, dass der Held #%d |Heiligen Stein|Heilige Steine|# sammelt." },
    });

    hintTable[RHT_BRIDGE_MEDALLIONS_HINT] = HintText::Bridge({
        // obscure text                                                  singular  plural
        Text{ "The awakened ones will await for the Hero to collect #%d |Medallion|Medallions|#.",
              /*french*/ "Les êtres de sagesse attendront le héros muni de #%d |Médaillon|Médaillons|#.",
              /*spanish*/ "Los sabios aguardarán a que el héroe&obtenga #%d |medallón|medallones|#.",
              /*german*/"Die Weisen werden darauf warten, dass der Held #%d |Amulett|Amulette|# sammelt." },
    });

    hintTable[RHT_BRIDGE_REWARDS_HINT] = HintText::Bridge({
        // obscure text                                                  singular                     plural
        Text{ "The awakened ones will await for the Hero to collect #%d |Spiritual Stone or Medallion|Spiritual Stones "
              "and Medallions|#.",
              /*french*/
              "Les êtres de sagesse attendront le héros muni de #%d |Pierre Ancestrale ou Médaillon|Pierres Ancestrales ou Médaillons|#.",
              /*spanish*/
              "Los sabios aguardarán a que el héroe obtenga #%d |piedra espiritual o medallón|piedras espirtuales y "
            "medallones|#.",
              /*german*/"Die Weisen werden darauf warten, dass der Held #%d |Heiligen Stein oder Amulett|Heilige Steine oder Amulette|# sammelt." },
    });

    hintTable[RHT_BRIDGE_DUNGEONS_HINT] = HintText::Bridge({
        // obscure text                                                  singular plural
        Text{ "The awakened ones will await for the Hero to conquer #%d |Dungeon|Dungeons|#.",
              /*french*/ "Les êtres de sagesse attendront la conquête de #%d |Donjon|Donjons|#.",
              /*spanish*/ "Los sabios aguardarán a que el héroe complete #%d |mazmorra|mazmorras|#.",
              /*german*/"Die Weisen werden darauf warten, dass der Held #%d |Labyrinth|Labyrinthe|# abschließt." },
    });

    hintTable[RHT_BRIDGE_TOKENS_HINT] = HintText::Bridge({
        // obscure text
        Text{ "The awakened ones will await for the Hero to collect #%d |Gold Skulltula Token|Gold Skulltula Tokens|#.",
              /*french*/ "Les êtres de sagesse attendront le héros muni de #%d |Symbole|Symboles| de Skulltula d'or#.",
              /*spanish*/ "Los sabios aguardarán a que el héroe obtenga #%d |símbolo|símbolos| de skulltula dorada#.",
              /*german*/"Die Weisen werden darauf warten, dass der Held #%d |Skulltula-Symbol|Skulltula-Symbole|# sammelt." },
    });

    hintTable[RHT_BRIDGE_GREG_HINT] = HintText::Bridge({
        // obscure text
        Text{ "The awakened ones will await for the Hero to find %gGreg%w.",
              /*french*/ "The awakened ones will await for the Hero to find %gGreg%w.",
              /*spanish*/ "The awakened ones will await for the Hero to find %gGreg%w." },
    });

    /*--------------------------
    | GANON BOSS KEY HINT TEXT |
    ---------------------------*/

    hintTable[RHT_GANON_BK_START_WITH_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the #evil one#'s key will be #given from the start#.",
              /*french*/ "Aussi, la #clé du Malin# sera #possession même du héros#.",
              /*spanish*/ "Y obtendrás la llave del #señor del mal# desde el #inicio#.",
              /*german*/"Und der #Schlüssel des Bösen#, wird #von Anfang an, im Besitz des Helden# sein." },
    });

    hintTable[RHT_GANON_BK_VANILLA_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the #evil one#'s key will be kept in a big chest #inside its tower#.",
              /*french*/ "Aussi, la #clé du Malin# sera encoffrée #dans sa tour#.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará en un gran cofre de #su torre#.",
              /*german*/"Und der #Schlüssel des Bösen#, wird in der großen Truhe #im Teufelsturm# zu finden sein." },
    });

    hintTable[RHT_GANON_BK_OWN_DUNGEON_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the #evil one#'s key will be hidden somewhere #inside its castle#.",
              /*french*/ "Aussi, la #clé du Malin# sera cachée #dans son vaste château#.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará en #algún lugar de su castillo#.",
              /*german*/"Und der #Schlüssel des Bösen#, wird irgendwo #in Ganons Schloss# zu finden sein." },
    });

    hintTable[RHT_GANON_BK_OVERWORLD_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the #evil one#'s key will be hidden #outside of dungeons# in Hyrule.",
              /*french*/ "Aussi, la #clé du Malin# se trouve #hors des donjons# d'Hyrule.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará #fuera de las mazmorras# de Hyrule.",
              /*german*/"Und der #Schlüssel des Bösen#, wird irgendwo #in Hyrule, außerhalb von Labyrinthen# zu finden sein." },
    });

    hintTable[RHT_GANON_BK_ANY_DUNGEON_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the #evil one#'s key will be hidden #inside a dungeon# in Hyrule.",
              /*french*/ "Aussi, la #clé du Malin# se trouve #dans un donjon# d'Hyrule.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará #en una mazmorra# de Hyrule.",
              /*german*/"Und der #Schlüssel des Bösen#, wird irgendwo #in Hyrule, innerhalb eines Labyrinths# zu finden sein." },
    });

    hintTable[RHT_GANON_BK_ANYWHERE_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the #evil one#'s key will be hidden somewhere&#in Hyrule#.",
              /*french*/ "Aussi, la #clé du Malin# se trouve quelque part #dans Hyrule#.",
              /*spanish*/ "Y la llave del #señor del mal# aguardará en #cualquier lugar de Hyrule#.",
              /*german*/"Und der #Schlüssel des Bösen#, wird irgendwo #in Hyrule# zu finden sein." },
    });

    hintTable[RHT_GANON_BK_TRIFORCE_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text{ "And the #evil one#'s key will be given to the Hero once the #Triforce## is completed.",
              /*french*/ "Aussi, la #clé du Malin# se&révèlera une fois la #Triforce#&assemblée.",
              /*spanish*/ "Y el héroe recibirá la llave del #señor del mal# cuando haya completado la #Trifuerza#.",
              /*german*/"Und der #Schlüssel des Bösen# wird verliehen, sobald das #Triforce# vervollständigt wurde." },
    });

    hintTable[RHT_GANON_BK_SKULLTULA_HINT] = HintText::GanonsBossKey({
        // obscure text
        Text { "And the #evil one#'s key will be provided by the cursed rich man once #100 Gold Skulltula Tokens# are retrieved.",
              /*french*/ "Aussi, la #clé du Malin# sera&donnée par l'homme maudit une fois que #100 Symboles de Skulltula d'or# auront été trouvés.",
              /*spanish*/ "Y el rico maldito entregará la llave&del #señor de mal# tras obtener&100 símbolos de skulltula dorada#."},
    });

    /*--------------------------
    |      LACS HINT TEXT      |
    ---------------------------*/

    hintTable[RHT_LACS_VANILLA_HINT] = HintText::LACS({
        // obscure text
        Text{
            "And the #evil one#'s key will be provided by Zelda once the #Shadow and Spirit Medallions# are retrieved.",
            /*french*/ "Aussi, la #clé du Malin# sera fournie par Zelda une fois que les #Médaillons de l'Ombre et de l'Esprit# seront récupérés.",
            /*spanish*/
            "Y Zelda entregará la llave del #señor del mal# tras obtener #el medallón de las sombras y del "
            "espíritu#.",
            /*german*/"Und der #Schlüssel des Bösen# wird von Zelda verliehen, sobald #die Amulette des Schattens und der Geister# geborgen wurden." },
    });

    hintTable[RHT_LACS_MEDALLIONS_HINT] = HintText::LACS({
        // obscure text                                                     singular      plural
        Text{ "And the #evil one#'s key will be provided by Zelda once #%d |Medallion# is|Medallions# are| retrieved.",
              /*french*/ "Aussi, la #clé du Malin# sera fournie par Zelda une fois |qu' #%d Médaillon# aura été récupéré|que #%d Médaillons# auront été récupérés|.",
              /*spanish*/ "Y Zelda entregará la llave&del #señor del mal# tras obtener #%d |medallón|medallones|#.",
              /*german*/"Und der #Schlüssel des Bösen# wird von Zelda verliehen, sobald #%d |Amulett# geborgen wurde|Amulette# geborgen wurden|." },
    });

    hintTable[RHT_LACS_STONES_HINT] = HintText::LACS({
        // obscure text                                                     singular            plural
        Text{
            "And the #evil one#'s key will be provided by Zelda once #%d |Spiritual Stone# is|Spiritual Stones# are| "
            "retrieved.",
            /*french*/ "Aussi, la #clé du Malin# sera fournie par Zelda une fois |qu' #%d Pierre Ancestrale# aura été&récupérée|que #%d Pierres Ancestrales# auront été récupérées|.",
            /*spanish*/
            "Y Zelda entregará la llave del #señor del mal# tras obtener #%d |piedra espiritual|piedras "
            "espirituales|#.",
            /*german*/"Und der #Schlüssel des Bösen# wird von Zelda verliehen, sobald #%d |Heiliger Stein# geborgen wurde|Heilige Steine# geborgen wurden|." },
    });

    hintTable[RHT_LACS_REWARDS_HINT] = HintText::LACS({
        // obscure text                                                     singular                         plural
        Text{ "And the #evil one#'s key will be provided by Zelda once #%d |Spiritual Stone or Medallion# "
              "is|Spiritual Stones and Medallions# are| retrieved.",
              /*french*/
              "Aussi, la #clé du Malin# sera fournie par Zelda une fois |qu' #%d Pierre Ancestrale ou %d Médaillon# sera récupéré|que&#%d Pierres Ancestrales et&Médaillons# seront récupérés|.",
              /*spanish*/
              "Y Zelda entregará la llave del #señor del mal# tras obtener #%d |piedra espiritual o medallón|piedras "
            "espirituales o medallones|#.",
              /*german*/"Und der #Schlüssel des Bösen# wird von Zelda verliehen, sobald #%d |Heiliger Stein oder Amulett#&geborgen wurde|Heilige Steine oder Amulette#&geborgen wurden|." },
    });

    hintTable[RHT_LACS_DUNGEONS_HINT] = HintText::LACS({
        // obscure text                                                     singular    plural
        Text{ "And the #evil one#'s key will be provided by Zelda once #%d |Dungeon# is|Dungeons# are| conquered.",
              /*french*/ "Aussi, la #clé du Malin# sera fournie par Zelda une fois |qu' #%d donjon #sera conquis|que #%d donjons# seront conquis|.",
            /*spanish*/ "Y Zelda entregará la llave del #señor del mal# tras completar #%d |mazmorra|mazmorras|#.",
              /*german*/"Und der #Schlüssel des Bösen# wird von Zelda verliehen, sobald #%d |Labyrinth# abgeschlossen wurde|Labyrinthe# abgeschlossen wurden|." },
    });

    hintTable[RHT_LACS_TOKENS_HINT] = HintText::LACS({
        // obscure text                                                     singular                 plural
        Text{ "And the #evil one#'s key will be provided by Zelda once #%d |Gold&Skulltula Token# is|Gold Skulltula "
              "Tokens# are| retrieved.",
              /*french*/ "Aussi, la #clé du Malin# sera fournie par Zelda une fois |qu' #%d symbole de Skulltula d'or #sera récupuéré|que &#%d symboles de Skulltula d'or&#seront recupérés|.",
              /*spanish*/
              "Y Zelda entregará la llave del #señor del mal# tras obtener #%d |símbolo|símbolos| de skulltula "
            "dorada#.",
              /*german*/"Und der #Schlüssel des Bösen# wird von Zelda verliehen, sobald #%d |Skulltula-Symbol# gesammelt wurde|Skulltula-Symbole# gesammelt wurden|." },
    });
    /*--------------------------
    |     TRIAL HINT TEXT      |
    ---------------------------*/

    hintTable[RHT_SIX_TRIALS] = HintText::Exclude({
        // obscure text
        Text{ "#Ganon's Tower# is protected by a powerful barrier.",
              /*french*/ "#la Tour de Ganon# est protégée par une puissante barrière",
              /*spanish*/ "la #torre de Ganon# está protegida por una poderosa barrera",
              /*german*/"der #Teufelsturm# sei von einer mächtigen Barriere geschützt." },
    });

    hintTable[RHT_ZERO_TRIALS] = HintText::Exclude({
        // obscure text
        Text{ "Sheik dispelled the barrier around #Ganon's Tower#.",
              /*french*/ "Sheik a dissipé la barrière autour de #la Tour de Ganon#",
              /*spanish*/ "Sheik disipó la barrera alrededor de la #torre de Ganon#.",
              /*german*/"Shiek habe die Barriere um den #Teufelsturm# aufgelöst." },
    });

    hintTable[RHT_FOUR_TO_FIVE_TRIALS] = HintText::Exclude({
        // obscure text
        Text{ " was dispelled by Sheik.", /*french*/ " a été dissipée par Sheik.",
            /*spanish*/ " se disipó gracias a Sheik.",
              /*german*/" sei von Shiek aufgelöst worden." },
    });

    hintTable[RHT_ONE_TO_THREE_TRIALS] = HintText::Exclude({
        // obscure text
        Text{ " protects Ganons Tower.", /*french*/ " protège la Tour de Ganon.",
            /*spanish*/ " protege la torre de Ganon",
                /*german*/" schütze den Teufelsturm." },
    });

    /*--------------------------
    |        ALTAR TEXT        |
    ---------------------------*/

    hintTable[RHT_SPIRITUAL_STONE_TEXT_START] = HintText::Altar({
        // obscure text
        Text{ "3 Spiritual Stones found in Hyrule...",
              /*french*/ "Les trois Pierres Ancestrales cachées&dans Hyrule...",
              /*spanish*/ "Tres piedras espirituales halladas por Hyrule...",
              /*german*/"Drei Heilige Steine, zu finden in Hyrule..." },
    });

    hintTable[RHT_CHILD_ALTAR_TEXT_END_DOTOPEN] = HintText::Altar({
        // obscure text
        Text{ "Ye who may become a Hero...&The path to the future is open...",
              /*french*/ "À celui qui a quête de devenir&héros...&Le futur vous accueille béant...",
              /*spanish*/ "Para aquel que se convierta en el héroe...&La puerta al futuro está a su disposición...",
              /*german*/"Jener auf dem Weg des Helden...&Der Pfad zur Zukunft sei geöffnet..." },
    });

    hintTable[RHT_CHILD_ALTAR_TEXT_END_DOTSONGONLY] = HintText::Altar({
        // obscure text
        Text{
            "Ye who may become a Hero...&Stand with the Ocarina and&play the Song of Time.",
            /*french*/ "À celui qui a quête de devenir&héros...&Portez l'Ocarina et jouez&le chant du temps.",
            /*spanish*/ "Para aquel que se convierta en el héroe...&Tome la ocarina y&entone la Canción del Tiempo.",
            /*german*/"Jener auf dem Weg des Helden...&Nehme er seine Okarina zur Hand und&spiele hier die Hymne der Zeit." },
    });

    hintTable[RHT_CHILD_ALTAR_TEXT_END_DOTCLOSED] = HintText::Altar({
        // obscure text
        Text{ "Ye who may become a Hero...&Offer the spiritual stones and&play the Song of Time.",
              /*french*/
              "À celui qui a quête de devenir&héros... Présentez les Pierres&Ancestrales et jouez&le chant du temps.",
              /*spanish*/
              "Para aquel que se convierta en el héroe...&Tome las piedras espirituales y&entone la Canción del "
            "Tiempo.",
              /*german*/"Jener mit den drei Heiligen Steinen&nehme seine Okarina zur Hand und&spiele hier die Hymne der Zeit." },
    });

    hintTable[RHT_ADULT_ALTAR_TEXT_START] = HintText::Altar({
        // obscure text
        Text{ "An awakening voice from the Sacred Realm will call those destined to be Sages, who dwell in the #five temples#.",
              /*french*/
              "Quand le mal aura triomphé, une voix du Saint Royaume appellera ceux cachés dans les #cinq temples#, "
              "destinés^à être Sages.",
              /*spanish*/
              "Cuando el mal lo impregne todo, desde el Reino Sagrado surgirá una voz que hará despertar a los sabios "
            "que moran en los #cinco templos#.",
              /*german*/"Beherrscht das Böse die Welt, weilen&jene Weisen, die von der Stimme des Heiligen Reiches erweckt werden, noch&in den #fünf Tempeln#." },
    });

    hintTable[RHT_ADULT_ALTAR_TEXT_END] = HintText::Altar({
        // obscure text
        Text{
            "$kTogether with the Hero of Time, the awakened ones will return the light of peace to the world...",
            /*french*/
            "$kEnsemble avec le Héros du Temps, ces Sages emprisonneront le mal et réinstaureront la lumière de paix "
            "dans le monde...",
            /*spanish*/
            "Con el Héroe del Tiempo, aquellos&que despierten detendrán el mal y&volverán al mundo de luz la paz...",
            /*german*/"$kZusammen mit dem Auserwählten wer-den diese ihre Kräfte einsetzen, um der Welt den Frieden wiederzugeben." },
    });

    /*--------------------------
    |   VALIDATION LINE TEXT   |
    ---------------------------*/

    hintTable[RHT_VALIDATION_LINE] = HintText::Validation({
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

    hintTable[RHT_LIGHT_ARROW_LOCATION_HINT] = HintText::OtherHint({
        // obscure text
        Text{
            "Ha ha ha... You'll never beat me by reflecting my lightning bolts and unleashing the arrows from ",
            /*french*/
            "Ha ha ha... Pauvre fou! Tu ne pourras jamais me vaincre sans les flèches que j'ai cachées dans ",
            /*spanish*/
            "Ja, ja, ja... Nunca me derrotarás reflejando mis esferas de energía y desplegando la flecha de luz de " },
    });

    hintTable[RHT_SHEIK_LIGHT_ARROW_HINT] = HintText::SheikLine({
        // obscure text
        Text{
            "I overheard Ganondorf say that he misplaced the %yLight Arrows%w in %r",
            /*french*/
            "J'ai entendu dire que Ganondorf aurait caché les %yFlèches de Lumière%w dans %r",
            /*spanish*/
            ""},
    });

    /*--------------------------
    |MASTER SWORD LOCATION TEXT|
    ---------------------------*/

    hintTable[RHT_MASTER_SWORD_LOCATION_HINT] = HintText::MasterSword({
        // obscure text
        Text{"And even if you do, you'll never find the legendary blade hidden in ",
            /*french*/
            "Et même si tu les trouves, tu ne touveras jamais l'épée de légende cachée dans ",
            /*spanish*/
            "E incluso si lo haces, nunca encontrarás la espada legendaria escondida en " },
    });

    hintTable[RHT_SHEIK_MASTER_SWORD_LOCATION_HINT] = HintText::SheikLine({
        // obscure text
        Text{"He also stole %gthe Master Sword%w and hid it in %r",
            /*french*/
            "Il aurait aussi volé %gl'épée de légende%w, qu'il aurait caché dans %r",
            /*spanish*/
            "Él también robó %gla Espada Maestra%w y la escondió en %r" },
    });

    hintTable[RHT_YOUR_POCKET] = HintText::Exclude({
        // obscure text
        Text{ "your pocket", /*french*/ "tes poches", /*spanish*/ "tu bolsillo" },
    });

    /*--------------------------
    |      OTHER HINT TEXT     |
    ---------------------------*/

    hintTable[RHT_DAMPE_DIARY01] = HintText::OtherHint({
        // obscure text
        Text{
            "Whoever reads this, please enter %r", 
            /*french*/
            "Toi qui lit ce journal, rends-toi dans %r",
            /*german?*/
            "Wer immer dies liest, der möge folgenden Ort aufsuchen: %r"},
    });

    hintTable[RHT_DAMPE_DIARY02] = HintText::OtherHint({
        // obscure text
        Text{
            "%w. I will let you have my stretching, shrinking keepsake.^I'm waiting for you.&--Dampé",
            /*french*/
            "%w. Et peut-être auras-tu droit à mon précieux %rtrésor%w.^Je t'attends...&--Igor",
            /*german?*/
            "%w. Ihm gebe ich meinen langen, kurzen Schatz.^Ich warte!&Boris"},
    });

    hintTable[RHT_GREG_HINT01] = HintText::OtherHint({
        // obscure text
        Text{
            "By the way, if you're interested, I saw the shiniest %gGreen Rupee%w somewhere in%g ", 
            /*french*/
            "Au fait, si ça t'intéresse, j'ai aperçu le plus éclatant des %gRubis Verts%w quelque part à %g",
            /*spanish*/
            ""},
    });

    hintTable[RHT_GREG_HINT02] = HintText::OtherHint({
        // obscure text
        Text{
            "%w.^It's said to have %rmysterious powers%w...^But then, it could just be another regular rupee.&Oh well.",
            /*french*/
            "%w. On dit qu'il possède des pouvoirs mystérieux... Mais bon, ça pourrait juste être un autre rubis ordinaire.",
            /*spanish*/
            ""},
    });

    hintTable[RHT_SARIA_TEXT01] = HintText::OtherHint({
        // obscure text
        Text{
            "Did you feel the %gsurge of magic%w recently? A mysterious bird told me it came from %g",
            /*french*/
            "As-tu récemment ressenti une vague de %gpuissance magique%w? Un mystérieux hibou m'a dit  qu'elle provenait du %g",
            /*spanish*/
            ""},
    });

    hintTable[RHT_SARIA_TEXT02] = HintText::OtherHint({
        // obscure text
        Text{
            "%w.^You should check that place out, @!$C",
            /*french*/
            "%w. Tu devrais aller y jeter un coup d'oeil, @!$C",
            /*spanish*/
            "%w.$C"},
    });

    hintTable[RHT_WARP_TO] = HintText::OtherHint({
        Text{
            "Warp to&#",
            "Se téléporter vers&#",
            "",
            "Zu&#"},
        });
    hintTable[RHT_WARP_CHOICE] = HintText::OtherHint({
        Text{
            "#?&" + TWO_WAY_CHOICE() + "#OK&No#",
            "#?&" + TWO_WAY_CHOICE() + "#OK!&Non#",
            "###",
            "#?&" + TWO_WAY_CHOICE() + "#OK&No#"
        }
    });

    hintTable[RHT_FROGS_HINT01] = HintText::OtherHint({Text{
        "Some frogs holding #",
        "Des grenouilles se trouvant sous l'eau vous fixent attentivement, tenant fermement #",
        "",
        "Unter Wasser gibt es Frösche, die #"
    }});
    hintTable[RHT_FROGS_HINT02] = HintText::OtherHint({Text{
        "# are looking at you from underwater...",
        "#.",
        "",
        "# bei sich haben und Dich neugierig beobachten..."
    }});

    hintTable[RHT_FISHING_POLE_HINT01] = HintText::OtherHint({
        // obscure text
        Text{"^If I remember correctly, I lost it somewhere in %r",
        /*french*/
        "",
        /*spanish*/
        "" }
    });

    hintTable[RHT_FISHING_POLE_HINT02] = HintText::OtherHint({
        // obscure text
        Text{"%w...&Let me know if you find it!",
        /*french*/
        "",
        /*spanish*/
        "" }
    });

    /*--------------------------
    |      GANON LINE TEXT     |
    ---------------------------*/

    hintTable[RHT_GANON_LINE01] = HintText::GanonLine({
        // obscure text
        Text{ "Oh! It's @.&I was expecting someone called Sheik.&Do you know what happened to them?",
              /*french*/ "Ah, c'est @.&J'attendais un certain Sheik.&Tu sais ce qui lui est arrivé?",
              /*spanish*/
              "¡Oh! Pero si es @.&Estaba esperando a alguien llamado Sheik. ¿Sabes qué puede haberle pasado?" },
    });

    hintTable[RHT_GANON_LINE02] = HintText::GanonLine({
        // obscure text
        Text{ "I knew I shouldn't have put the key on the other side of my door.",
              /*french*/ "J'aurais dû garder la clé ici. Hélas...",
              /*spanish*/ "Sabía que no tendría que haber dejado la llave al otro lado de la puerta." },
    });

    hintTable[RHT_GANON_LINE03] = HintText::GanonLine({
        // obscure text
        Text{ "Looks like it's time for a round of tennis.",
              /*french*/ "C'est l'heure de jouer au tennis.",
              /*spanish*/ "Parece que es hora de una pachanga de tenis." },
    });

    hintTable[RHT_GANON_LINE04] = HintText::GanonLine({
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

    hintTable[RHT_GANON_LINE05] = HintText::GanonLine({
        // obscure text
        Text{ "Why did I leave my trident back in the desert?",
              /*french*/ "Sale bêtise... Et j'ai oublié mon trident dans le désert!",
              /*spanish*/ "Santa Hylia... ¿Por qué me habré dejado el tridente en el desierto?" },
    });

    hintTable[RHT_GANON_LINE06] = HintText::GanonLine({
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

    hintTable[RHT_GANON_LINE07] = HintText::GanonLine({
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

    hintTable[RHT_GANON_LINE08] = HintText::GanonLine({
        // obscure text
        Text{ "I've learned this spell,&it's really neat,&I'll keep it later&for your treat!",
              /*french*/
              "Gamin, ton destin achève,&sous mon sort tu périras!&Cette partie ne fut pas brève,&et cette mort, tu "
              "subiras!",
              /*spanish*/
              "Veamos ahora que harás,&la batalla ha de comenzar,&te enviaré de una vez al más allá,&¿listo para "
              "afrontar la verdad?" },
    });

    hintTable[RHT_GANON_LINE09] = HintText::GanonLine({
        // obscure text
        Text{ "Many tricks are up my sleeve,&to save yourself&you'd better leave!",
              /*french*/ "Sale petit garnement,&tu fais erreur!&C'est maintenant que marque&ta dernière heure!",
              /*spanish*/ "¿No osarás a mí enfrentarte?&Rimas aparte,&¡voy a matarte!" },
    });

    hintTable[RHT_GANON_LINE10] = HintText::GanonLine({
        // obscure text
        Text{ "After what you did to Koholint Island, how can you call me the bad guy?",
              /*french*/ "J'admire ce que tu as fait à l'Île Koholint... Toi et moi, nous devrions faire équipe!",
              /*spanish*/ "Después de lo que le hiciste a la Isla Koholint, ¿cómo te atreves a llamarme malvado?" },
    });

    hintTable[RHT_GANON_LINE11] = HintText::GanonLine({
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

    hintTable[RHT_BEAN_SALESMAN_FIRST] = HintText::MerchantsDialogs({
        Text{
            "I tried to be a #magic bean#&salesman, but it turns out my&marketing skills weren't worth&"
            "beans!^Anyway, want to buy #",
            /*french*/ "J'ai essayé d'être un vendeur&de #haricots magiques#, mais&j'étais mauvais au niveau du&marketing et ça "
            "me courait sur&le haricot...^Enfin bref, ça te dirait de m'acheter #",
            /*spanish*/ "",
            /*german*/ "Möchten Sie #"
        }
    });
    hintTable[RHT_BEAN_SALESMAN_SECOND] = HintText::MerchantsDialogs({
        Text{
            "# for #60 Rupees#?&" + TWO_WAY_CHOICE() + "#Yes&No#",
            /*french*/ "# pour #60 Rubis#?&" + TWO_WAY_CHOICE() + "#Oui&Non#",
            /*spanish*/ "",
            /*german*/ "# für #60 Rubine#?&" + TWO_WAY_CHOICE() + "#Ja&Nein#"
        }
    });

    hintTable[RHT_MEDIGORON_DIALOG_FIRST] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "How about buying #",
              /*french*/ "Veux-tu acheter #",
              /*spanish*/ "¿Me compras #",
              /*german*/ "Möchtest du #" },
    });

    hintTable[RHT_MEDIGORON_DIALOG_SECOND] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "# for #200 Rupees#?&" + TWO_WAY_CHOICE() + "#Buy&Don't buy#",
              /*french*/ "# pour #200 rubis#?&" + TWO_WAY_CHOICE() + "#Acheter&Ne pas acheter#",
              /*spanish*/ "# por #200 rupias#?&" + TWO_WAY_CHOICE() + "#Comprar&No comprar#",
              /*german*/ "# für #200 Rubine# kaufen?&" + TWO_WAY_CHOICE() + "#Klar!&Nie im Leben!#" },
    });

    hintTable[RHT_CARPET_SALESMAN_DIALOG_FIRST] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "Welcome!^I am selling stuff, strange and rare, from all over the world to everybody. Today's special "
              "is...^#",
              /*french*/
              "Bienvenue!^Je vends des objets rares et merveilleux du&monde entier. En spécial aujourd'hui...^#",
              /*spanish*/
              "¡Acércate!^Vendo productos extraños y difíciles de&encontrar... De todo el mundo a todo el&mundo. La "
              "oferta de hoy es...^#¡",
              /* german */
              "Sei gegrüßt!^Ich verkaufe allerlei Kuriorisäten.&Stets sonderliche und seltene Ware&aus "
              "aller Welt für jedermann.&Das heutige Angebot bleibt...^#" },
    });

    hintTable[RHT_CARPET_SALESMAN_DIALOG_MYSTERIOUS] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "!# Terrifying! I won't tell you what it is until I see the #money#...^How about #200 Rupees#?&&" +
                  TWO_WAY_CHOICE() + "#Buy&Don't buy#",
              /*french*/
              "!# Un concentré de puissance! Mais montre tes #rubis# avant que je te dise ce que c'est...^Disons #200 "
              "rubis#?&&" +
                  TWO_WAY_CHOICE() + "#Acheter&Ne pas acheter#",
              /*spanish*/
              "!# ¡Terrorífico! No te revelaré su nombre hasta que vea el #dinero#...^#200 rupias#, ¿qué te parece?&&" +
                  TWO_WAY_CHOICE() + "#Comprar&No comprar#",
              /*german*/
              "!# Furchterregend oder? Ich erzähle Euch mehr, wenn ich #Geld# sehe...^Wie wär's mit #200 Rubinen#?&&" +
                  TWO_WAY_CHOICE() + "#Aber sicher!&Ich bin weg!#" },
    });

    hintTable[RHT_CARPET_SALESMAN_DIALOG_HINTED] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "!# It's real, I promise! A lonely man such as myself wouldn't #lie# to you, hmm?^"
              "How about #200 Rupees#?&&" + TWO_WAY_CHOICE() + "#Buy&Don't buy#",
              /*french*/
              "!# C'est vrai! J'te jure! Un gars comme moi ne te #mentirai# pas tu ne crois pas?^Disons #200 "
              "rubis#?&&" + TWO_WAY_CHOICE() + "#Acheter&Ne pas acheter#",
              /*spanish*/ // TODO: Flesh this out with the flavor text if we ever support spanish translations
              "#!^###200 rupias#, ¿qué te parece?&&" + TWO_WAY_CHOICE() + "#Comprar&No comprar#",
              /*german*/
              "#! Ich kann versichern es ist ein aufrichtiges Angebot!^Ein einsamer Mann wie ich würde dich doch "
              "nicht #anlügen#, oder?^Wie wär's mit #200 Rubinen#?&&" + TWO_WAY_CHOICE() + 
              "#Aber sicher!&Ich bin weg!#" },
    });

    hintTable[RHT_CARPET_SALESMAN_DIALOG_FINAL] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "Finally! Now I can go back to being &an %rarms dealer%w!",
              /*french*/ "Squalala! Je vais enfin pouvoir &%rprendre des vacances%w!",
              /*spanish*/ "", // TODO: Flesh this out with the flavor text if we ever support spanish translations
              /*german*/ "Endlich! Schon bald kann ich wieder &%rKrabbelminen-Händler%w sein!" },
    });

    hintTable[RHT_GRANNY_DIALOG] = HintText::MerchantsDialogs({
        // obscure text
        Text{ "! How about #100 Rupees#?&" + TWO_WAY_CHOICE() + "#Buy&Don't buy#",
              /*french*/ "! Que dis-tu de #100 rubis#?&" + TWO_WAY_CHOICE() + "#Acheter&Ne pas acheter#",
              /*spanish*/ ". Vendo por #100 rupias#.&" + TWO_WAY_CHOICE() + "#Comprar&No comprar#",
              /*german*/ "! Sagen wir #100 Rubine#!&" + TWO_WAY_CHOICE() + "#Gerne!&Auf keinen Fall!#" },
    });
}

int32_t StonesRequiredBySettings() {
    auto ctx = Rando::Context::GetInstance();
    int32_t stones = 0;
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_STONES)) {
        stones = std::max<int32_t>({ stones, (int32_t)ctx->GetOption(RSK_RAINBOW_BRIDGE_STONE_COUNT).Value<uint8_t>() });
    }
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)) {
        stones = std::max<int32_t>({ stones, (int32_t)ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Value<uint8_t>() - 6 });
    }
    if ((ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS)) && (ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON))) {
        stones = std::max<int32_t>({ stones, (int32_t)ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Value<uint8_t>() - 6 });
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_STONES)) {
        stones = std::max<int32_t>({ stones, (int32_t)ctx->GetOption(RSK_LACS_STONE_COUNT).Value<uint8_t>() });
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_STONES)) {
        stones = std::max<int32_t>({ stones, (int32_t)ctx->GetOption(RSK_LACS_REWARD_COUNT).Value<uint8_t>() - 6 });
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS)) {
        stones = std::max<int32_t>({ stones, (int32_t)ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Value<uint8_t>() - 6 });
    }
    return stones;
}

int32_t MedallionsRequiredBySettings() {
    auto ctx = Rando::Context::GetInstance();
    int32_t medallions = 0;
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_MEDALLIONS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)ctx->GetOption(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT).Value<uint8_t>() });
    }
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Value<uint8_t>() - 3 });
    }
    if ((ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS)) && (ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON))) {
        medallions = std::max<int32_t>({ medallions, (int32_t)ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Value<uint8_t>() - 3 });
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_MEDALLIONS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)ctx->GetOption(RSK_LACS_MEDALLION_COUNT).Value<uint8_t>() });
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_REWARDS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)ctx->GetOption(RSK_LACS_REWARD_COUNT).Value<uint8_t>() - 3 });
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_DUNGEONS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Value<uint8_t>() - 3 });
    }
    return medallions;
}

int32_t TokensRequiredBySettings() {
    auto ctx = Rando::Context::GetInstance();
    int32_t tokens = 0;
    if (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS)) {
        tokens = std::max<int32_t>({ tokens, (int32_t)ctx->GetOption(RSK_RAINBOW_BRIDGE_TOKEN_COUNT).Value<uint8_t>() });
    }
    if (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_LACS_TOKENS)) {
        tokens = std::max<int32_t>({ tokens, (int32_t)ctx->GetOption(RSK_LACS_TOKEN_COUNT).Value<uint8_t>() });
    }
    return tokens;
}

std::array<RandomizerHintTextKey, RA_MAX> AreaHintTextKeys = {
    RHT_NONE,
    RHT_LINKS_POCKET,
    RHT_KOKIRI_FOREST,
    RHT_THE_LOST_WOODS,
    RHT_SACRED_FOREST_MEADOW,
    RHT_HYRULE_FIELD,
    RHT_LAKE_HYLIA,
    RHT_GERUDO_VALLEY,
    RHT_GERUDO_FORTRESS,
    RHT_HAUNTED_WASTELAND,
    RHT_DESERT_COLOSSUS,
    RHT_THE_MARKET,
    RHT_TEMPLE_OF_TIME,
    RHT_HYRULE_CASTLE,
    RHT_OUTSIDE_GANONS_CASTLE,
    RHT_CASTLE_GROUNDS,
    RHT_KAKARIKO_VILLAGE,
    RHT_THE_GRAVEYARD,
    RHT_DEATH_MOUNTAIN_TRAIL,
    RHT_GORON_CITY,
    RHT_DEATH_MOUNTAIN_CRATER,
    RHT_ZORAS_RIVER,
    RHT_ZORAS_DOMAIN,
    RHT_ZORAS_FOUNTAIN,
    RHT_LON_LON_RANCH,
    RHT_DEKU_TREE,
    RHT_DODONGOS_CAVERN,
    RHT_JABU_JABUS_BELLY,
    RHT_FOREST_TEMPLE,
    RHT_FIRE_TEMPLE,
    RHT_WATER_TEMPLE,
    RHT_SPIRIT_TEMPLE,
    RHT_SHADOW_TEMPLE,
    RHT_BOTTOM_OF_THE_WELL,
    RHT_ICE_CAVERN,
    RHT_GERUDO_TRAINING_GROUND,
    RHT_GANONS_CASTLE
};

std::array<ConditionalAlwaysHint, 12> conditionalAlwaysHints = {
    std::make_pair(RC_MARKET_10_BIG_POES,
                   []() {
                       auto ctx = Rando::Context::GetInstance();
                       return ctx->GetOption(RSK_BIG_POE_COUNT).Value<uint8_t>() >= 3 && !ctx->GetOption(RSK_BIG_POES_HINT);
                   }), // Remember, the option's value being 3 means 4 are required
    std::make_pair(RC_DEKU_THEATER_MASK_OF_TRUTH,
                   []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_COMPLETE_MASK_QUEST);
                   }),
    std::make_pair(RC_SONG_FROM_OCARINA_OF_TIME, []() { return StonesRequiredBySettings() < 2; }),
    std::make_pair(RC_HF_OCARINA_OF_TIME_ITEM, []() { return StonesRequiredBySettings() < 2; }),
    std::make_pair(RC_SHEIK_IN_KAKARIKO, []() { return MedallionsRequiredBySettings() < 5; }),
    std::make_pair(RC_DMT_TRADE_CLAIM_CHECK,
                   []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_BIGGORON_HINT);
                   }),
    std::make_pair(RC_KAK_30_GOLD_SKULLTULA_REWARD,
                   []() {
                       auto ctx = Rando::Context::GetInstance();
                       return TokensRequiredBySettings() < 30 && !ctx->GetOption(RSK_KAK_30_SKULLS_HINT);
                   }),
    std::make_pair(RC_KAK_40_GOLD_SKULLTULA_REWARD,
                   []() {
                       auto ctx = Rando::Context::GetInstance();
                       return TokensRequiredBySettings() < 40 && !ctx->GetOption(RSK_KAK_40_SKULLS_HINT);
                   }),
    std::make_pair(RC_KAK_50_GOLD_SKULLTULA_REWARD,
                   []() {
                       auto ctx = Rando::Context::GetInstance();
                       return TokensRequiredBySettings() < 50 && !ctx->GetOption(RSK_KAK_50_SKULLS_HINT);
                   }),
    std::make_pair(RC_ZR_FROGS_OCARINA_GAME,
                   []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_FROGS_HINT);
                   }),
    std::make_pair(RC_KF_LINKS_HOUSE_COW,
                   []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_MALON_HINT);
                   }),
    std::make_pair(RC_KAK_100_GOLD_SKULLTULA_REWARD,
                   []() {
                       auto ctx = Rando::Context::GetInstance();
                       return !ctx->GetOption(RSK_KAK_100_SKULLS_HINT);
                   }),
};

const HintText& Hint(const RandomizerHintTextKey hintKey) {
    return hintTable[hintKey];
}

const HintText& Hint(const RandomizerArea area) {
    return hintTable[AreaHintTextKeys[area]];
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
