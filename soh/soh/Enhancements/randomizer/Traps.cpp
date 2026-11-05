#include "soh/Enhancements/Lang/TranslationCatalog.h"
#include "soh/Enhancements/custom-message/MessageCatalog.h"
#include "soh/Enhancements/Lang/GameLanguage.h"
#include "Traps.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/ShipUtils.h"

#include "soh/Enhancements/randomizer/rng.h"
#include "soh/Enhancements/randomizer/randomizerEnumStrings.h"

#define NOGDI // avoid various windows defines that conflict with things in z64.h
#include <spdlog/spdlog.h>

#include <vector>

// Table of trick names for ice traps
static std::array<std::vector<Rando::Traps::TrickName>, RG_MAX> trickNameTable;
bool initTrickNames = false; // Indicates if trick ice trap names have been initialized yet

/* Initialize the table of trick names with an easy, medium, and hard name for each language,
   each followed by the article to use when the name appears in a sentence.
   english, french, german // spanish */
static void InitTrickNames() {
    trickNameTable[RG_KOKIRI_SWORD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_SWORD.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada Korok"
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_SWORD.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada del héroe"
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_SWORD.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Espada de esmeril"
    };
    trickNameTable[RG_MASTER_SWORD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MASTER_SWORD.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada Divina"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MASTER_SWORD.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.2") }, // "Espada de los Sabios"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MASTER_SWORD.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Fay"
    };
    trickNameTable[RG_GIANTS_KNIFE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GIANTS_KNIFE.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.3") }, // "La espada de Medigoron"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GIANTS_KNIFE.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Espada de esmeril"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GIANTS_KNIFE.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Royal Claymore"
    };
    trickNameTable[RG_BIGGORON_SWORD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BIGGORON_SWORD.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada de poder",
        { CustomMessages::CreateText("randomizer.traps.names.RG_BIGGORON_SWORD.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada de la Fiera Deidad"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BIGGORON_SWORD.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada Maestra mejorada"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BIGGORON_SWORD.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Daga de Biggoron"
    };
    trickNameTable[RG_DEKU_SHIELD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_DEKU_SHIELD.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Escudo Boko"
        { CustomMessages::CreateText("randomizer.traps.names.RG_DEKU_SHIELD.1"),
          CustomMessages::CreateText("randomizer.article_forms.an.0") }, // "Escudo de Ordon"
        { CustomMessages::CreateText("randomizer.traps.names.RG_DEKU_SHIELD.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Escudo de madera"
    };
    trickNameTable[RG_HYLIAN_SHIELD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_HYLIAN_SHIELD.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Escudo Hylian"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HYLIAN_SHIELD.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Escudo Divino"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HYLIAN_SHIELD.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Escudo del héroe"
    };
    trickNameTable[RG_MIRROR_SHIELD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MIRROR_SHIELD.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Escudo mágico"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MIRROR_SHIELD.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Escudo arcano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MIRROR_SHIELD.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Espejo del Crepúsculo"
    };
    trickNameTable[RG_GORON_TUNIC] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_TUNIC.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Pechera gerudo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_TUNIC.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Armadura ignífuga"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_TUNIC.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.6") }, // "Ropas rojas"
    };
    trickNameTable[RG_ZORA_TUNIC] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_TUNIC.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Sayo rito"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_TUNIC.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Costume de sirène"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_TUNIC.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.3") }, // "Túnica Zora"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_TUNIC.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.6") }, // "Ropas azules"
    };
    trickNameTable[RG_IRON_BOOTS] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_IRON_BOOTS.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Botas férreas"
        { CustomMessages::CreateText("randomizer.traps.names.RG_IRON_BOOTS.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Botas de nieve"
        { CustomMessages::CreateText("randomizer.traps.names.RG_IRON_BOOTS.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Botas rojas"
        { CustomMessages::CreateText("randomizer.traps.names.RG_IRON_BOOTS.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.8") }, // "Zora Greaves"
        { CustomMessages::CreateText("randomizer.traps.names.RG_IRON_BOOTS.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Botas de plomo"
    };
    trickNameTable[RG_HOVER_BOOTS] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_HOVER_BOOTS.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Botas flotadoras"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HOVER_BOOTS.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Botas de Oro"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HOVER_BOOTS.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Botas de Pegaso"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HOVER_BOOTS.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Botas del desierto"
    };
    trickNameTable[RG_WEIRD_EGG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_WEIRD_EGG.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Huevo pasado"
        { CustomMessages::CreateText("randomizer.traps.names.RG_WEIRD_EGG.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Huevo Lon Lon"
        { CustomMessages::CreateText("randomizer.traps.names.RG_WEIRD_EGG.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Huevo Zora"
    };
    trickNameTable[RG_ZELDAS_LETTER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZELDAS_LETTER.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Carta de Ruto"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZELDAS_LETTER.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Carta para Kafei"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZELDAS_LETTER.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Carta"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZELDAS_LETTER.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Carta para Kafei"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZELDAS_LETTER.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Carta de la Cabra"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZELDAS_LETTER.5"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Carta de Dolores"
    };
    trickNameTable[RG_BOOMERANG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOOMERANG.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Plátano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOOMERANG.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Bumerang"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOOMERANG.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Bumerán tornado"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOOMERANG.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Bumerán mágico"
    };
    trickNameTable[RG_LENS_OF_TRUTH] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_LENS_OF_TRUTH.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Monóculo de la Verdad"
        { CustomMessages::CreateText("randomizer.traps.names.RG_LENS_OF_TRUTH.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Sensor Sheikah"
        { CustomMessages::CreateText("randomizer.traps.names.RG_LENS_OF_TRUTH.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Cristal de Visión"
        { CustomMessages::CreateText("randomizer.traps.names.RG_LENS_OF_TRUTH.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Lente Aumentadora"
    };
    trickNameTable[RG_MEGATON_HAMMER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MEGATON_HAMMER.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Mazo Goron"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MEGATON_HAMMER.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Martillo mágico"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MEGATON_HAMMER.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Martillo de hierro"
    };
    trickNameTable[RG_STONE_OF_AGONY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_STONE_OF_AGONY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Amuleto de la cueva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_STONE_OF_AGONY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Piedra de Agahnim"
        { CustomMessages::CreateText("randomizer.traps.names.RG_STONE_OF_AGONY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.11") }, // "Piedra de la Agonía"
        { CustomMessages::CreateText("randomizer.traps.names.RG_STONE_OF_AGONY.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") }, // "Amuleto Pirata"
    };
    trickNameTable[RG_DINS_FIRE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_DINS_FIRE.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Fuego de Eldin"
        { CustomMessages::CreateText("randomizer.traps.names.RG_DINS_FIRE.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Poder de Din"
        { CustomMessages::CreateText("randomizer.traps.names.RG_DINS_FIRE.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Linterna mágica"
        { CustomMessages::CreateText("randomizer.traps.names.RG_DINS_FIRE.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medallón de Tesoro"
        { CustomMessages::CreateText("randomizer.traps.names.RG_DINS_FIRE.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medallón del Temblor"
    };
    trickNameTable[RG_FARORES_WIND] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FARORES_WIND.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Viento de Farone"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FARORES_WIND.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Valor de Farore"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FARORES_WIND.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Tingle Air"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FARORES_WIND.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Medallón Maligno"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FARORES_WIND.4"),
          Text{ "", "", "" } }, // "El taxi de Airín"
    };
    trickNameTable[RG_NAYRUS_LOVE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_NAYRUS_LOVE.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.2") }, // "Amor de Lanayru"
        { CustomMessages::CreateText("randomizer.traps.names.RG_NAYRUS_LOVE.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Sabiduría de Nayru"
        { CustomMessages::CreateText("randomizer.traps.names.RG_NAYRUS_LOVE.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Escudo de hormigueo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_NAYRUS_LOVE.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Hechizo de Protección"
        { CustomMessages::CreateText("randomizer.traps.names.RG_NAYRUS_LOVE.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.13") }, // "Armadura mágica"
    };
    trickNameTable[RG_FIRE_ARROWS] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_ARROWS.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Cetro de fuego"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_ARROWS.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Flecha bomba"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_ARROWS.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Vela roja"
    };
    trickNameTable[RG_ICE_ARROWS] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ICE_ARROWS.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Cetro de Hielo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ICE_ARROWS.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Flecha ancestral"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ICE_ARROWS.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Cetro de hielo"
    };
    trickNameTable[RG_LIGHT_ARROWS] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_LIGHT_ARROWS.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Flecha del Viento"
        { CustomMessages::CreateText("randomizer.traps.names.RG_LIGHT_ARROWS.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Varita de Gamelón"
        { CustomMessages::CreateText("randomizer.traps.names.RG_LIGHT_ARROWS.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.3") }, // "Flecha eléctrica"
        { CustomMessages::CreateText("randomizer.traps.names.RG_LIGHT_ARROWS.3"),
          CustomMessages::CreateText("randomizer.article_forms.none.3") }, // "Flecha de plata"
    };
    trickNameTable[RG_GERUDO_MEMBERSHIP_CARD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_MEMBERSHIP_CARD.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.13") }, // "Escritura del desierto"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_MEMBERSHIP_CARD.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.11") }, // "Bandera de la Luna Creciente"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_MEMBERSHIP_CARD.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Cupón especial"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_MEMBERSHIP_CARD.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Tóken Gerudo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_MEMBERSHIP_CARD.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.13") }, // "Tarjeta Gerudo"
    };

    trickNameTable[RG_MAGIC_BEAN_PACK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MAGIC_BEAN_PACK.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.11") }, // "Lote de frijoles mágicos"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MAGIC_BEAN_PACK.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Bayas de garfio"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MAGIC_BEAN_PACK.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.11") }, // "Lote de alubias mágicas"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MAGIC_BEAN_PACK.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Paquete de semillas místicas"
    };
    trickNameTable[RG_DOUBLE_DEFENSE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_DOUBLE_DEFENSE.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Contenedor de diamante"
        { CustomMessages::CreateText("randomizer.traps.names.RG_DOUBLE_DEFENSE.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.4") }, // "Doble daño receptivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_DOUBLE_DEFENSE.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.4") }, // "Defensa cuádruple"
    };

    trickNameTable[RG_POCKET_EGG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_POCKET_EGG.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Huevo de Arpagos"
        { CustomMessages::CreateText("randomizer.traps.names.RG_POCKET_EGG.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Huevo Lon Lon"
        { CustomMessages::CreateText("randomizer.traps.names.RG_POCKET_EGG.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Huevo del Pez Viento"
    };
    trickNameTable[RG_POCKET_EGG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_POCKET_EGG.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Alarma emplumada portátil"
        { CustomMessages::CreateText("randomizer.traps.names.RG_POCKET_EGG.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Cuco de Kakariko"
        { CustomMessages::CreateText("randomizer.traps.names.RG_POCKET_EGG.5"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Pollo de bolsillo"
    };
    trickNameTable[RG_COJIRO] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_COJIRO.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") },                         // "Cucazul"
        { CustomMessages::CreateText("randomizer.traps.names.RG_COJIRO.1"), Text{ "", "", "" } }, // "Piyoko"
        { CustomMessages::CreateText("randomizer.traps.names.RG_COJIRO.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Cucco oscuro"
        { CustomMessages::CreateText("randomizer.traps.names.RG_COJIRO.3"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Cuco de Grog"
    };
    trickNameTable[RG_ODD_MUSHROOM] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_MUSHROOM.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Champiñón mágico"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_MUSHROOM.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Champiñón del bosque"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_MUSHROOM.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.11") }, // "Seta durmiente"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_MUSHROOM.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Seta"
    };
    trickNameTable[RG_ODD_POTION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_POTION.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Poción rara"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_POTION.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Medicina de la abuela"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_POTION.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Medicina de champiñones"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_POTION.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.11") }, // "Pócima secreta"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_POTION.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") }, // "Esporas de hongos"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ODD_POTION.5"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Espora Hanyu"
    };
    trickNameTable[RG_POACHERS_SAW] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_POACHERS_SAW.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Sierra del carpintero"
        { CustomMessages::CreateText("randomizer.traps.names.RG_POACHERS_SAW.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Espada del capataz"
        { CustomMessages::CreateText("randomizer.traps.names.RG_POACHERS_SAW.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Mandoble ancestral"
        { CustomMessages::CreateText("randomizer.traps.names.RG_POACHERS_SAW.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Hacha de leñador"
        { CustomMessages::CreateText("randomizer.traps.names.RG_POACHERS_SAW.4"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Sierra del Cazador Furtivo"
    };
    trickNameTable[RG_BROKEN_SWORD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BROKEN_SWORD.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada de Biggoron rota"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BROKEN_SWORD.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Daga gigante rota"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BROKEN_SWORD.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada noble rota"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BROKEN_SWORD.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada minish rota"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BROKEN_SWORD.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Espada decadente de leyenda"
    };
    trickNameTable[RG_PRESCRIPTION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PRESCRIPTION.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.2") }, // "Receta de Biggoron"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PRESCRIPTION.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Receta ocular"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PRESCRIPTION.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Prescripción"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PRESCRIPTION.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.11") }, // "Esgrimidorium"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PRESCRIPTION.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Retrato de Oren"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PRESCRIPTION.5"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Carta al Rey Zora"
    };
    trickNameTable[RG_EYEBALL_FROG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_EYEBALL_FROG.0"), Text{ "", "", "" } }, // "Don Gero"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EYEBALL_FROG.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Rana de patas calientes"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EYEBALL_FROG.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Espadachín perdido"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EYEBALL_FROG.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Globo Ocular de Rana"
    };
    trickNameTable[RG_EYEDROPS] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_EYEDROPS.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.3") }, // "Gotas de Biggoron"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EYEDROPS.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.2") }, // "Gotas oculares"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EYEDROPS.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Lágrima de Luna"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EYEDROPS.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Grasa del motor"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EYEDROPS.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Perfume Zora"
    };
    trickNameTable[RG_CLAIM_CHECK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_CLAIM_CHECK.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.11") }, // "Comprobant e de Reclamación"
        { CustomMessages::CreateText("randomizer.traps.names.RG_CLAIM_CHECK.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") }, // "Litografía arcana"
        { CustomMessages::CreateText("randomizer.traps.names.RG_CLAIM_CHECK.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Piedra Sheikah"
        { CustomMessages::CreateText("randomizer.traps.names.RG_CLAIM_CHECK.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.14") }, // "Pizarra de los Torbellinos"
    };

    trickNameTable[RG_GOLD_SKULLTULA_TOKEN] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GOLD_SKULLTULA_TOKEN.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Símbolo de Skulltula"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GOLD_SKULLTULA_TOKEN.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Tóken de Skulltula Dorada"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GOLD_SKULLTULA_TOKEN.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Skulltula dorada"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GOLD_SKULLTULA_TOKEN.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Maimai"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GOLD_SKULLTULA_TOKEN.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Gema de gratitud"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GOLD_SKULLTULA_TOKEN.5"),
          CustomMessages::CreateText("randomizer.article_forms.a.3") }, // "Semilla de kolog"
    };

    trickNameTable[RG_PROGRESSIVE_HOOKSHOT] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_HOOKSHOT.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Garra progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_HOOKSHOT.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Zarpa progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_HOOKSHOT.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Enganchador progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_HOOKSHOT.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.3") }, // "Cuerda progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_STRENGTH] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_STRENGTH.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Guanteletes progresivos"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_STRENGTH.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Brasaletes progresivos"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_STRENGTH.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Manoplas progresivas"
    };
    trickNameTable[RG_PROGRESSIVE_BOMB_BAG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMB_BAG.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Mayor capacidad de bombas"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMB_BAG.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Zurrón de bombas progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMB_BAG.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Bolsa de bombas progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMB_BAG.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Máscara explosiva progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMB_BAG.4"),
          CustomMessages::CreateText("randomizer.article_forms.none.5") }, // "Barril de polvo progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMB_BAG.5"),
          CustomMessages::CreateText("randomizer.article_forms.none.6") }, // "Bombas remotas progresivas"
    };
    trickNameTable[RG_PROGRESSIVE_BOW] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOW.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Mayor capacidad de flechas"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOW.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Arco del héroe progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOW.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.3") }, // "Ballesta progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOW.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Ballesta progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOW.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Arco Sagrado Progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOW.5"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Arco de centaleón Progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_SLINGSHOT] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SLINGSHOT.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Mayor capacidad de semillas"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SLINGSHOT.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Catapulta progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SLINGSHOT.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Resortera múltiple progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SLINGSHOT.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Lanzador de semillas progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SLINGSHOT.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Bolsa de semillas progresiva"
    };
    trickNameTable[RG_PROGRESSIVE_WALLET] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_WALLET.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Mayor capacidad de rupias"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_WALLET.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Cartera de rupias progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_WALLET.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Zurrón de rupias progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_WALLET.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Capacidad progresiva Rupobre"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_WALLET.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Bolsa de trofeos progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_WALLET.5"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Bolso Ruby progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_SCALE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SCALE.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.6") }, // "Aletas de zora progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SCALE.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Escama dragón acuático progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SCALE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Buceo progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SCALE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Perla progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_SCALE.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Fragmento Zora progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_NUT_UPGRADE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_NUT_UPGRADE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Mayor capacidad de semillas"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_NUT_UPGRADE.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Bolsa de cebo progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_NUT_UPGRADE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Capacidad progresiva de pera"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_NUT_UPGRADE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Bolsa de nueces progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_NUT_UPGRADE.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Mayor capacidad de castañas"
    };
    trickNameTable[RG_PROGRESSIVE_STICK_UPGRADE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_STICK_UPGRADE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Mayor capacidad de ramas deku"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_STICK_UPGRADE.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Mayor capacidad de bastones"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_STICK_UPGRADE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Capacidad progresiva de la sucursal"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_STICK_UPGRADE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Mayor capacidad de cetros deku"
    };
    trickNameTable[RG_PROGRESSIVE_BOMBCHU_BAG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMBCHU_BAG.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.6") }, // "Bombinsectos progresivos"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMBCHU_BAG.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.7") }, // "Bomba de helicobot progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMBCHU_BAG.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.8") }, // "Soldado bomba progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_BOMBCHU_BAG.3"),
          CustomMessages::CreateText("randomizer.article_forms.none.6") }, // "Misiles progresivos"
    };
    trickNameTable[RG_PROGRESSIVE_MAGIC_METER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_MAGIC_METER.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Medidor de vigor progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_MAGIC_METER.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Medidor de energía progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_MAGIC_METER.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Medidor de carga progresivo"
    };
    trickNameTable[RG_PROGRESSIVE_OCARINA] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_OCARINA.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Silbato progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_OCARINA.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Silbido progresivo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_OCARINA.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Flauta progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_OCARINA.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Armónica progresiva"
    };
    trickNameTable[RG_PROGRESSIVE_GORONSWORD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_GORONSWORD.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Hoja del Titán progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_GORONSWORD.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Daga Goron progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_GORONSWORD.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Espada gigante progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_GORONSWORD.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Espada Darknut progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_GORONSWORD.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Espada de poder progresiva"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PROGRESSIVE_GORONSWORD.5"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Gran puñalada progresiva"
    };

    trickNameTable[RG_EMPTY_BOTTLE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_EMPTY_BOTTLE.0"),
          CustomMessages::CreateText("randomizer.article_forms.an.1") }, // "cantimplora vacía"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EMPTY_BOTTLE.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.2") }, // "Vial de Vientos"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EMPTY_BOTTLE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de Tingle"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EMPTY_BOTTLE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Frasco feérico"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EMPTY_BOTTLE.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de cristal"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EMPTY_BOTTLE.5"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella Tingle"
    };
    trickNameTable[RG_BOTTLE_WITH_MILK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_MILK.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de Reserva Romani"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_MILK.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Biberón con leche Premium"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_MILK.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de leche extra"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_MILK.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de leche fresca"
    };
    trickNameTable[RG_BOTTLE_WITH_RED_POTION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_RED_POTION.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Jugo de Chuchu Rojo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_RED_POTION.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de poción de Hibisco"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_RED_POTION.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de medicina de la vida"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_RED_POTION.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de poción de salud"
    };
    trickNameTable[RG_BOTTLE_WITH_GREEN_POTION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_GREEN_POTION.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Jugo de Chuchu Verde"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_GREEN_POTION.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de Aceite de candil"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_GREEN_POTION.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de medicina mágica"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_GREEN_POTION.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de elixir vigorizante"
    };
    trickNameTable[RG_BOTTLE_WITH_BLUE_POTION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BLUE_POTION.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Jugo de Chuchu Azul"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BLUE_POTION.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de agua de la vida"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BLUE_POTION.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de oxígeno"
    };
    trickNameTable[RG_BOTTLE_WITH_FAIRY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_FAIRY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Luciérnaga del bosque"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_FAIRY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella con Deku Princess"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_FAIRY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Hada perdida en una botella"
    };
    trickNameTable[RG_BOTTLE_WITH_FISH] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_FISH.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Lord Chapu-Chapu embotellado"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_FISH.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Reekfish embotellada"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_FISH.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Locha de Hyrule embotellada"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_FISH.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Perca de Términa embotellada"
    };
    trickNameTable[RG_BOTTLE_WITH_BLUE_FIRE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BLUE_FIRE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de llama azul"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BLUE_FIRE.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de fuego ancestral"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BLUE_FIRE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella con una vela azul"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BLUE_FIRE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de Hielo rojo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BLUE_FIRE.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella de llamas de Nayru"
    };
    trickNameTable[RG_BOTTLE_WITH_BUGS] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BUGS.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Tektites en una botella"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BUGS.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella con un escarabajo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BUGS.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Celestarabajo embotellado"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BUGS.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Saltabosques embotellados"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BUGS.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Botella con una abeja dorada"
    };
    trickNameTable[RG_BOTTLE_WITH_POE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_POE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Ghini en una botella"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_POE.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Reapling en una botella"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_POE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Espectro en una botella"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_POE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Whisp en una botella"
    };

    trickNameTable[RG_RUTOS_LETTER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_RUTOS_LETTER.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Carta de Dolores"
        { CustomMessages::CreateText("randomizer.traps.names.RG_RUTOS_LETTER.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Carta para Kafei"
        { CustomMessages::CreateText("randomizer.traps.names.RG_RUTOS_LETTER.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Carta náutica"
    };
    trickNameTable[RG_BOTTLE_WITH_BIG_POE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BIG_POE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Hermana Poe embotellada"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BIG_POE.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Yaihalla embotellado"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTLE_WITH_BIG_POE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Bubble en una botella"
    };

    trickNameTable[RG_ZELDAS_LULLABY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZELDAS_LULLABY.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Cántico de la Diosa"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZELDAS_LULLABY.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Canción de curación"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZELDAS_LULLABY.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Canción del héroe"
    };
    trickNameTable[RG_EPONAS_SONG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_EPONAS_SONG.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Cantar del ave"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EPONAS_SONG.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Canción del viento"
        { CustomMessages::CreateText("randomizer.traps.names.RG_EPONAS_SONG.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Chant du cheval"
    };
    trickNameTable[RG_SARIAS_SONG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SARIAS_SONG.0"),
          Text{ "", "", "" } }, // "La canción de Mido"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SARIAS_SONG.1"),
          Text{ "", "", "" } }, // "El tema de Kass"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SARIAS_SONG.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Melodía del Eco"
    };
    trickNameTable[RG_SUNS_SONG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SUNS_SONG.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Melodía del transcurrir"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SUNS_SONG.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.2") }, // "Cara al Sol"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SUNS_SONG.2"),
          Text{ "", "", "" } }, // "La canción de la luna"
    };
    trickNameTable[RG_SONG_OF_TIME] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SONG_OF_TIME.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Canción del doble tiempo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SONG_OF_TIME.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Canción del tiempo invertida"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SONG_OF_TIME.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Melodía del Tiempo"
    };
    trickNameTable[RG_SONG_OF_STORMS] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SONG_OF_STORMS.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Melodía del Tornado"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SONG_OF_STORMS.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Canción del alma de la rana"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SONG_OF_STORMS.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Melodía del Viento"
    };
    trickNameTable[RG_MINUET_OF_FOREST] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MINUET_OF_FOREST.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Dueto del bosque"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MINUET_OF_FOREST.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Sonata del despertar"
        { CustomMessages::CreateText("randomizer.traps.names.RG_MINUET_OF_FOREST.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.2") }, // "Melodía del Espíritu del Viento"
    };
    trickNameTable[RG_BOLERO_OF_FIRE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOLERO_OF_FIRE.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Coro del fuego"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOLERO_OF_FIRE.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.3") }, // "Melodía de las Corrientes"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOLERO_OF_FIRE.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Nana goron"
    };
    trickNameTable[RG_SERENADE_OF_WATER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SERENADE_OF_WATER.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Sonata del agua"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SERENADE_OF_WATER.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Bossanova de las olas"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SERENADE_OF_WATER.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Mambo de Manbo"
    };
    trickNameTable[RG_REQUIEM_OF_SPIRIT] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_REQUIEM_OF_SPIRIT.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Reggae del espíritu"
        { CustomMessages::CreateText("randomizer.traps.names.RG_REQUIEM_OF_SPIRIT.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.2") }, // "Elegía al vacío"
        { CustomMessages::CreateText("randomizer.traps.names.RG_REQUIEM_OF_SPIRIT.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.2") }, // "Melodía del Espíritu de la Tierra"
    };
    trickNameTable[RG_NOCTURNE_OF_SHADOW] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_NOCTURNE_OF_SHADOW.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Diurno de la sombra"
        { CustomMessages::CreateText("randomizer.traps.names.RG_NOCTURNE_OF_SHADOW.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.2") }, // "Oda al orden"
        { CustomMessages::CreateText("randomizer.traps.names.RG_NOCTURNE_OF_SHADOW.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Canto revelador"
    };
    trickNameTable[RG_PRELUDE_OF_LIGHT] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PRELUDE_OF_LIGHT.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Predulio de luz"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PRELUDE_OF_LIGHT.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Balada del Piez Viento"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PRELUDE_OF_LIGHT.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Sonidos de la luz"
    };

    trickNameTable[RG_KOKIRI_EMERALD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_EMERALD.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Colgante del valor"
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_EMERALD.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Orbe de Farore"
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_EMERALD.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Yerbánida"
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_EMERALD.3"),
          CustomMessages::CreateText("randomizer.article_forms.none.9") }, // "Esmeralda de Farore"
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_EMERALD.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Ágata de los Kokiri"
    };
    trickNameTable[RG_GORON_RUBY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_RUBY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Colgante del poder"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_RUBY.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Orbe de Din"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_RUBY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.14") }, // "Bermellina"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_RUBY.3"),
          CustomMessages::CreateText("randomizer.article_forms.none.10") }, // "Rubí de Din"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_RUBY.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Topacio de los Goron"
    };
    trickNameTable[RG_ZORA_SAPPHIRE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_SAPPHIRE.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Colgante de la sabiduría"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_SAPPHIRE.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "Orbe de Nayru"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_SAPPHIRE.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.14") }, // "Azurina"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_SAPPHIRE.3"),
          CustomMessages::CreateText("randomizer.article_forms.none.10") }, // "Zafiro de Nayru"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_SAPPHIRE.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.14") }, // "Lapislázuli de los Zora"
    };
    trickNameTable[RG_FOREST_MEDALLION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_MEDALLION.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medallón del Viento"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_MEDALLION.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Elemento de aire"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_MEDALLION.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Medallón de Saria"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_MEDALLION.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Glifo de aire"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_MEDALLION.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medalla del Bosque"
    };
    trickNameTable[RG_FIRE_MEDALLION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_MEDALLION.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Elemento de fuego"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_MEDALLION.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Medallón de Darunia"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_MEDALLION.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Glifo de fuego"
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_MEDALLION.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medalla del Fuego"
    };
    trickNameTable[RG_WATER_MEDALLION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_MEDALLION.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Elemento de agua"
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_MEDALLION.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medallón Helado"
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_MEDALLION.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Medallón de Ruto"
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_MEDALLION.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Glifo de agua"
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_MEDALLION.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medalla del Agua"
    };
    trickNameTable[RG_SPIRIT_MEDALLION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_MEDALLION.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.0") }, // "Elemento de tierra"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_MEDALLION.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Medallón de Nabooru"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_MEDALLION.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Glifo de la tierra"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_MEDALLION.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medalla del Espíritu"
    };
    trickNameTable[RG_SHADOW_MEDALLION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_MEDALLION.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") }, // "Sombra Fundida"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_MEDALLION.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Medallón de Impa"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_MEDALLION.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Glifo de ilusión"
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_MEDALLION.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medalla de la Sombra"
    };
    trickNameTable[RG_LIGHT_MEDALLION] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_LIGHT_MEDALLION.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") }, // "Brújula de Luz"
        { CustomMessages::CreateText("randomizer.traps.names.RG_LIGHT_MEDALLION.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") }, // "Medallón de Rauru"
        { CustomMessages::CreateText("randomizer.traps.names.RG_LIGHT_MEDALLION.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.1") }, // "Glifo del destino"
        { CustomMessages::CreateText("randomizer.traps.names.RG_LIGHT_MEDALLION.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") }, // "Medalla de la Luz"
    };

    trickNameTable[RG_RECOVERY_HEART] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_RECOVERY_HEART.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.11") }, // "Te amo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_RECOVERY_HEART.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") }, // "vida"
        { CustomMessages::CreateText("randomizer.traps.names.RG_RECOVERY_HEART.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.6") }, // "VP"
    };
    trickNameTable[RG_GREEN_RUPEE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREEN_RUPEE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") },                                // "Peso hyliano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREEN_RUPEE.1"), Text{ "", "", "" } }, // "Un rubí"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREEN_RUPEE.2"),
          Text{ "", "", "" } }, // "Guaraní hyliano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREEN_RUPEE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Falso Greg"
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREEN_RUPEE.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Rupobre (1)"
    };
    trickNameTable[RG_BLUE_RUPEE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BLUE_RUPEE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") },                               // "Azupia"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BLUE_RUPEE.1"), Text{ "", "", "" } }, // "Cinco rubíes"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BLUE_RUPEE.2"),
          Text{ "", "", "" } }, // "Bolívar hyliano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BLUE_RUPEE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Peso hyliano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_BLUE_RUPEE.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Rupobre (5)"
    };
    trickNameTable[RG_RED_RUPEE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_RED_RUPEE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") },                              // "Los 20 grandes"
        { CustomMessages::CreateText("randomizer.traps.names.RG_RED_RUPEE.1"), Text{ "", "", "" } }, // "Veinte rubíes"
        { CustomMessages::CreateText("randomizer.traps.names.RG_RED_RUPEE.2"), Text{ "", "", "" } }, // "Colon hyliano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_RED_RUPEE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Peso hyliano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_RED_RUPEE.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Rupobre (20)"
    };
    trickNameTable[RG_PURPLE_RUPEE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PURPLE_RUPEE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "morupiua"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PURPLE_RUPEE.1"),
          Text{ "", "", "" } }, // "Cincuenta rubíes"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PURPLE_RUPEE.2"),
          Text{ "", "", "" } }, // "Balboa hyliano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PURPLE_RUPEE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Peso hyliano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PURPLE_RUPEE.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") }, // "Rupobre (50)"
    };
    trickNameTable[RG_HUGE_RUPEE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_HUGE_RUPEE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.6") }, // "Oro Rubi"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HUGE_RUPEE.1"),
          Text{ "", "", "" } }, // "Doscientos rubíes"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HUGE_RUPEE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Diamante"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HUGE_RUPEE.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") },                               // "Rubi gigante"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HUGE_RUPEE.4"), Text{ "", "", "" } }, // "Euro hyliano"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HUGE_RUPEE.5"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Dólar hyliano"
    };
    trickNameTable[RG_PIECE_OF_HEART] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PIECE_OF_HEART.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Pieza de Chorizo"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PIECE_OF_HEART.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.7") }, // "Un poco de amor"
        { CustomMessages::CreateText("randomizer.traps.names.RG_PIECE_OF_HEART.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") }, // "Pierre de pêche rare"
    };
    trickNameTable[RG_HEART_CONTAINER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_HEART_CONTAINER.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Corazón de cristal"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HEART_CONTAINER.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Vida Corazón"
        { CustomMessages::CreateText("randomizer.traps.names.RG_HEART_CONTAINER.2"),
          Text{ "", "", "" } }, // "Mucho amor"
    };
    trickNameTable[RG_TRIFORCE_PIECE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_TRIFORCE_PIECE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Piece of Cheese"
        { CustomMessages::CreateText("randomizer.traps.names.RG_TRIFORCE_PIECE.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.4") }, // "Triforce Shard"
        { CustomMessages::CreateText("randomizer.traps.names.RG_TRIFORCE_PIECE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") }, // "Shiny Rock"
    };
    trickNameTable[RG_TRIFORCE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_TRIFORCE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TRIFORCE.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.15") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TRIFORCE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.10") },
    };
    trickNameTable[RG_ROCS_FEATHER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ROCS_FEATHER.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.1") }, // "Chicken Wing"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ROCS_FEATHER.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.12") }, // "Roc's Leg"
        { CustomMessages::CreateText("randomizer.traps.names.RG_ROCS_FEATHER.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.13") }, // "Roc's Flapper"
    };
    trickNameTable[RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_DESERT_COLOSSUS_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_DESERT_COLOSSUS_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_DESERT_COLOSSUS_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_GERUDO_VALLEY_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_VALLEY_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_VALLEY_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_GRAVEYARD_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GRAVEYARD_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GRAVEYARD_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_KOKIRI_FOREST_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_FOREST_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KOKIRI_FOREST_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_LAKE_HYLIA_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_LAKE_HYLIA_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_LAKE_HYLIA_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_LOST_WOODS_BRIDGE_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_LOST_WOODS_BRIDGE_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_LOST_WOODS_BRIDGE_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_LOST_WOODS_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_LOST_WOODS_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_LOST_WOODS_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_ZORAS_RIVER_BEAN_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORAS_RIVER_BEAN_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORAS_RIVER_BEAN_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_GOHMA_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GOHMA_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GOHMA_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GOHMA_SOUL.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };
    trickNameTable[RG_KING_DODONGO_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_KING_DODONGO_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.13") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KING_DODONGO_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KING_DODONGO_SOUL.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") },
    };
    trickNameTable[RG_BARINADE_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BARINADE_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BARINADE_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.18") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BARINADE_SOUL.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_PHANTOM_GANON_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_PHANTOM_GANON_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_PHANTOM_GANON_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_PHANTOM_GANON_SOUL.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") },
    };
    trickNameTable[RG_VOLVAGIA_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_VOLVAGIA_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_VOLVAGIA_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.none.4") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_VOLVAGIA_SOUL.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") },
    };
    trickNameTable[RG_MORPHA_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MORPHA_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.14") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MORPHA_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MORPHA_SOUL.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.14") },
    };
    trickNameTable[RG_BONGO_BONGO_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BONGO_BONGO_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.13") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BONGO_BONGO_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.13") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BONGO_BONGO_SOUL.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") },
    };
    trickNameTable[RG_TWINROVA_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_TWINROVA_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.7") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TWINROVA_SOUL.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.14") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TWINROVA_SOUL.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };
    trickNameTable[RG_GANON_SOUL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANON_SOUL.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.1") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANON_SOUL.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANON_SOUL.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.14") },
    };
    trickNameTable[RG_FISHING_POLE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FISHING_POLE.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FISHING_POLE.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.0") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FISHING_POLE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.5") },
    };
    trickNameTable[RG_SCARECROWS_SONG] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SCARECROWS_SONG.0"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SCARECROWS_SONG.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SCARECROWS_SONG.2"),
          CustomMessages::CreateText("randomizer.article_forms.none.0") },
    };
    trickNameTable[RG_SKELETON_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKELETON_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKELETON_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKELETON_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKELETON_KEY.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKELETON_KEY.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_CLIMB] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_CLIMB.0"), Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_CRAWL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_CRAWL.0"), Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_OPEN_CHEST] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_OPEN_CHEST.0"), Text{ "", TODO_TRANSLATE, "" } },
    };

    trickNameTable[RG_OCARINA_A_BUTTON] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_A_BUTTON.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_A_BUTTON.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_A_BUTTON.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.5") },
    };
    trickNameTable[RG_OCARINA_C_UP_BUTTON] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_UP_BUTTON.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_UP_BUTTON.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_UP_BUTTON.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") },
    };
    trickNameTable[RG_OCARINA_C_DOWN_BUTTON] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_DOWN_BUTTON.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_DOWN_BUTTON.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_DOWN_BUTTON.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_DOWN_BUTTON.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") },
    };
    trickNameTable[RG_OCARINA_C_LEFT_BUTTON] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_LEFT_BUTTON.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_LEFT_BUTTON.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_LEFT_BUTTON.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_LEFT_BUTTON.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") },
    };
    trickNameTable[RG_OCARINA_C_RIGHT_BUTTON] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_RIGHT_BUTTON.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.10") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_RIGHT_BUTTON.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.4") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_OCARINA_C_RIGHT_BUTTON.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.11") },
    };

    trickNameTable[RG_GREG_RUPEE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREG_RUPEE.0"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREG_RUPEE.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREG_RUPEE.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.6") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREG_RUPEE.3"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GREG_RUPEE.4"), Text{ "", "", "" } },
    };

    trickNameTable[RG_FOREST_TEMPLE_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
    };
    trickNameTable[RG_FIRE_TEMPLE_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.6") },
    };
    trickNameTable[RG_WATER_TEMPLE_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
    };
    trickNameTable[RG_SPIRIT_TEMPLE_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_SMALL_KEY.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
    };
    trickNameTable[RG_SHADOW_TEMPLE_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.6") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
    };
    trickNameTable[RG_BOTTOM_OF_THE_WELL_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTOM_OF_THE_WELL_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTOM_OF_THE_WELL_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTOM_OF_THE_WELL_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
    };
    trickNameTable[RG_GERUDO_TRAINING_GROUND_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_TRAINING_GROUND_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_TRAINING_GROUND_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_TRAINING_GROUND_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
    };
    trickNameTable[RG_GERUDO_FORTRESS_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_SMALL_KEY.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_SMALL_KEY.4"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
    };
    trickNameTable[RG_GANONS_CASTLE_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.6") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
    };

    trickNameTable[RG_TREASURE_GAME_SMALL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_GAME_SMALL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.6") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_GAME_SMALL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_GAME_SMALL_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.8") },
    };

    trickNameTable[RG_FOREST_TEMPLE_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };
    trickNameTable[RG_FIRE_TEMPLE_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_WATER_TEMPLE_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };
    trickNameTable[RG_SPIRIT_TEMPLE_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_KEY_RING.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };
    trickNameTable[RG_SHADOW_TEMPLE_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };
    trickNameTable[RG_BOTTOM_OF_THE_WELL_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTOM_OF_THE_WELL_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTOM_OF_THE_WELL_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTTOM_OF_THE_WELL_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };
    trickNameTable[RG_GERUDO_TRAINING_GROUND_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_TRAINING_GROUND_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_TRAINING_GROUND_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_TRAINING_GROUND_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };
    trickNameTable[RG_GERUDO_FORTRESS_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_KEY_RING.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_FORTRESS_KEY_RING.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };
    trickNameTable[RG_GANONS_CASTLE_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };

    trickNameTable[RG_TREASURE_GAME_KEY_RING] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_GAME_KEY_RING.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_GAME_KEY_RING.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_GAME_KEY_RING.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
    };

    trickNameTable[RG_FOREST_TEMPLE_BOSS_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_BOSS_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_BOSS_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_BOSS_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_BOSS_KEY.3"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FOREST_TEMPLE_BOSS_KEY.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_FIRE_TEMPLE_BOSS_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_BOSS_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_BOSS_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_BOSS_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_BOSS_KEY.3"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FIRE_TEMPLE_BOSS_KEY.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_WATER_TEMPLE_BOSS_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_BOSS_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_BOSS_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_BOSS_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_BOSS_KEY.3"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_WATER_TEMPLE_BOSS_KEY.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_SPIRIT_TEMPLE_BOSS_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_BOSS_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_BOSS_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_BOSS_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_BOSS_KEY.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_TEMPLE_BOSS_KEY.4"), Text{ "", "", "" } },
    };
    trickNameTable[RG_SHADOW_TEMPLE_BOSS_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_BOSS_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_BOSS_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_BOSS_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_TEMPLE_BOSS_KEY.3"), Text{ "", "", "" } },
    };
    trickNameTable[RG_GANONS_CASTLE_BOSS_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_BOSS_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_BOSS_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_BOSS_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_BOSS_KEY.3"), Text{ "", "", "" } },
    };

    trickNameTable[RG_KEATON_MASK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_KEATON_MASK.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KEATON_MASK.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KEATON_MASK.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KEATON_MASK.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_SKULL_MASK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKULL_MASK.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKULL_MASK.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKULL_MASK.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_SPOOKY_MASK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPOOKY_MASK.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPOOKY_MASK.1"), Text{ "", TODO_TRANSLATE, "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPOOKY_MASK.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_BUNNY_HOOD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BUNNY_HOOD.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BUNNY_HOOD.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BUNNY_HOOD.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_MASK_OF_TRUTH] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MASK_OF_TRUTH.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MASK_OF_TRUTH.1"), Text{ "", TODO_TRANSLATE, "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MASK_OF_TRUTH.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_GORON_MASK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_MASK.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_MASK.1"), Text{ "", TODO_TRANSLATE, "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GORON_MASK.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };
    trickNameTable[RG_ZORA_MASK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_MASK.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_MASK.1"), Text{ "", TODO_TRANSLATE, "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_ZORA_MASK.2"), Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GERUDO_MASK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_MASK.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_MASK.1"), Text{ "", TODO_TRANSLATE, "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GERUDO_MASK.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.16") },
    };

    trickNameTable[RG_SPEAK_DEKU] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_DEKU.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_DEKU.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_DEKU.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
    };
    trickNameTable[RG_SPEAK_GERUDO] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_GERUDO.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_GERUDO.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_GERUDO.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
    };
    trickNameTable[RG_SPEAK_GORON] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_GORON.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_GORON.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_GORON.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
    };
    trickNameTable[RG_SPEAK_HYLIAN] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_HYLIAN.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_HYLIAN.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_HYLIAN.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
    };
    trickNameTable[RG_SPEAK_KOKIRI] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_KOKIRI.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_KOKIRI.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_KOKIRI.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
    };
    trickNameTable[RG_SPEAK_ZORA] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_ZORA.0"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_ZORA.1"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_ZORA.2"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPEAK_ZORA.3"),
          CustomMessages::CreateText("randomizer.article_forms.a.9") },
    };

    trickNameTable[RG_GUARD_HOUSE_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GUARD_HOUSE_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GUARD_HOUSE_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GUARD_HOUSE_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_MARKET_BAZAAR_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_BAZAAR_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_BAZAAR_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_BAZAAR_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_BAZAAR_KEY.3"), Text{ "", "", "" } },
    };
    trickNameTable[RG_MARKET_POTION_SHOP_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_POTION_SHOP_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_POTION_SHOP_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_POTION_SHOP_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_MASK_SHOP_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MASK_SHOP_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MASK_SHOP_KEY.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MASK_SHOP_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_MARKET_SHOOTING_GALLERY_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_SHOOTING_GALLERY_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_SHOOTING_GALLERY_KEY.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_MARKET_SHOOTING_GALLERY_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_BOMBCHU_BOWLING_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOMBCHU_BOWLING_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOMBCHU_BOWLING_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOMBCHU_BOWLING_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOMBCHU_BOWLING_KEY.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_TREASURE_CHEST_GAME_BUILDING_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_CHEST_GAME_BUILDING_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_CHEST_GAME_BUILDING_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_CHEST_GAME_BUILDING_KEY.2"),
          Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_CHEST_GAME_BUILDING_KEY.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TREASURE_CHEST_GAME_BUILDING_KEY.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_BOMBCHU_SHOP_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOMBCHU_SHOP_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOMBCHU_SHOP_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOMBCHU_SHOP_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_RICHARDS_HOUSE_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_RICHARDS_HOUSE_KEY.0"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_RICHARDS_HOUSE_KEY.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_RICHARDS_HOUSE_KEY.2"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_RICHARDS_HOUSE_KEY.3"), Text{ "", "", "" } },
    };
    trickNameTable[RG_ALLEY_HOUSE_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ALLEY_HOUSE_KEY.0"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_ALLEY_HOUSE_KEY.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_ALLEY_HOUSE_KEY.2"), Text{ "", "", "" } },
    };
    trickNameTable[RG_KAK_BAZAAR_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_BAZAAR_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_BAZAAR_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_BAZAAR_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_BAZAAR_KEY.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_KAK_POTION_SHOP_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_POTION_SHOP_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.17") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_POTION_SHOP_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_POTION_SHOP_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_BOSS_HOUSE_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOSS_HOUSE_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOSS_HOUSE_KEY.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOSS_HOUSE_KEY.2"), Text{ "", "", "" } },
    };
    trickNameTable[RG_GRANNYS_POTION_SHOP_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GRANNYS_POTION_SHOP_KEY.0"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GRANNYS_POTION_SHOP_KEY.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GRANNYS_POTION_SHOP_KEY.2"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_GRANNYS_POTION_SHOP_KEY.3"), Text{ "", "", "" } },
    };
    trickNameTable[RG_SKULLTULA_HOUSE_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKULLTULA_HOUSE_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKULLTULA_HOUSE_KEY.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_SKULLTULA_HOUSE_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_IMPAS_HOUSE_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_IMPAS_HOUSE_KEY.0"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_IMPAS_HOUSE_KEY.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_IMPAS_HOUSE_KEY.2"), Text{ "", "", "" } },
    };
    trickNameTable[RG_WINDMILL_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_WINDMILL_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_WINDMILL_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_KAK_SHOOTING_GALLERY_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_SHOOTING_GALLERY_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_SHOOTING_GALLERY_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_KAK_SHOOTING_GALLERY_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_DAMPES_HUT_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_DAMPES_HUT_KEY.0"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_DAMPES_HUT_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_DAMPES_HUT_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_TALONS_HOUSE_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_TALONS_HOUSE_KEY.0"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TALONS_HOUSE_KEY.1"), Text{ "", "", "" } },
        { CustomMessages::CreateText("randomizer.traps.names.RG_TALONS_HOUSE_KEY.2"), Text{ "", "", "" } },
    };
    trickNameTable[RG_STABLES_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_STABLES_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_STABLES_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_STABLES_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_BACK_TOWER_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BACK_TOWER_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BACK_TOWER_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BACK_TOWER_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BACK_TOWER_KEY.3"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_BACK_TOWER_KEY.4"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_HYLIA_LAB_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_HYLIA_LAB_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_HYLIA_LAB_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_HYLIA_LAB_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };
    trickNameTable[RG_FISHING_HOLE_KEY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_FISHING_HOLE_KEY.0"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FISHING_HOLE_KEY.1"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
        { CustomMessages::CreateText("randomizer.traps.names.RG_FISHING_HOLE_KEY.2"),
          CustomMessages::CreateText("randomizer.article_forms.the.12") },
    };

    trickNameTable[RG_SHADOW_SILVER_BLADES] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_SILVER_BLADES.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_SHADOW_SILVER_PIT] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_SILVER_PIT.0"), Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_SHADOW_SILVER_SPIKES] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_SILVER_SPIKES.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_SPIRIT_SILVER_CHILD] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_SILVER_CHILD.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_SPIRIT_SILVER_SUN] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_SILVER_SUN.0"), Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_SPIRIT_SILVER_BOULDERS] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_SILVER_BOULDERS.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_BOTW_SILVER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_BOTW_SILVER.0"), Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_ICE_CAVERN_SILVER_BLADES] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ICE_CAVERN_SILVER_BLADES.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_ICE_CAVERN_SILVER_BLOCK] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_ICE_CAVERN_SILVER_BLOCK.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GTG_SILVER_SLOPE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GTG_SILVER_SLOPE.0"), Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GTG_SILVER_LAVA] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GTG_SILVER_LAVA.0"), Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GTG_SILVER_WATER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GTG_SILVER_WATER.0"), Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GANONS_CASTLE_SILVER_LIGHT] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_SILVER_LIGHT.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GANONS_CASTLE_SILVER_FOREST] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_SILVER_FOREST.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GANONS_CASTLE_SILVER_FIRE] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_SILVER_FIRE.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GANONS_CASTLE_SILVER_SPIRIT] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_SILVER_SPIRIT.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_DODONGOS_CAVERN_MQ_SILVER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_DODONGOS_CAVERN_MQ_SILVER.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_SHADOW_MQ_SILVER_INVISIBLE_BLADES] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SHADOW_MQ_SILVER_INVISIBLE_BLADES.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_SPIRIT_MQ_SILVER_LOBBY] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_MQ_SILVER_LOBBY.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_SPIRIT_MQ_SILVER_BIG_WALL] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_SPIRIT_MQ_SILVER_BIG_WALL.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GANONS_CASTLE_MQ_SILVER_WATER] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_MQ_SILVER_WATER.0"),
          Text{ "", TODO_TRANSLATE, "" } },
    };
    trickNameTable[RG_GANONS_CASTLE_MQ_SILVER_SHADOW] = {
        { CustomMessages::CreateText("randomizer.traps.names.RG_GANONS_CASTLE_MQ_SILVER_SHADOW.0"),
          Text{ "", TODO_TRANSLATE, "" } },
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

/// @brief Gets the "trick name" for an Ice Trap
/// @param id The RandomizerGet of the item the Ice Trap is disguised as
/// @param iceTrapNamesOption The current value of the RSK_ICE_TRAP_NAMES setting
/// @param state The rng state
/// @return A Text object with the selected trick name
Rando::Traps::TrickName Rando::Traps::GetTrapName(RandomizerGet id, RandoIceTrapNames iceTrapNamesOption,
                                                  uint64_t* state) {
    if (iceTrapNamesOption == RO_ICE_TRAP_NAMES_SIMILAR) {
        // If the trick names table has not been initialized, do so
        if (!initTrickNames) {
            InitTrickNames();
            initTrickNames = true;
        }

        if (!trickNameTable[id].empty()) {
            // Randomly get the easy, medium, or hard name for the given item id
            return ShipUtils::RandomElement(trickNameTable[id], state);
        }

        // No trick name for this item, so fall through to its real name
        SPDLOG_ERROR("[Rando::Traps::GetTrapName] Couldn't find entry for RG {} in trickNameTable", id);
    }

    const Rando::Item& item =
        Rando::StaticData::RetrieveItem(iceTrapNamesOption == RO_ICE_TRAP_NAMES_REVEALED ? RG_ICE_TRAP : id);
    Text name = item.GetName();
    Text article = item.GetArticle();
    name.FillMissingTranslations();
    article.FillMissingTranslations();

    if (iceTrapNamesOption == RO_ICE_TRAP_NAMES_MISSPELLED_CHANGED_VOWEL) {
        name.ReplaceRandomVowel(state);
    } else if (iceTrapNamesOption == RO_ICE_TRAP_NAMES_MISSPELLED_DUPLICATED_LETTER) {
        name.DuplicateRandomLetter(state);
    }

    return { name, article };
}

static bool MatchesSavedName(const Text& saved, const Text& candidate) {
    if (saved.GetEnglish().empty())
        return false;
    if (saved.GetEnglish() == candidate.GetEnglish() || saved.GetEnglish() == candidate.GetFrench() ||
        saved.GetEnglish() == candidate.GetGerman())
        return true;
    for (const auto& [locale, value] : candidate.GetLocales())
        if (saved.GetEnglish() == value)
            return true;
    return false;
}

static void RestoreTextTranslations(Text& saved, const Text& candidate, bool localizedString) {
    // Old seed files may store one localized string in every legacy slot.
    // Existing independent French/German translations and explicit locales win.
    const bool collapsed = saved.GetFrench() == saved.GetEnglish() && saved.GetGerman() == saved.GetEnglish();
    if (localizedString && collapsed)
        saved.SetForLocale("en_US", candidate.GetEnglish());
    if (collapsed) {
        saved.SetForLocale("fr_FR", candidate.GetFrench());
        saved.SetForLocale("de_DE", candidate.GetGerman());
    }
    for (const auto& [locale, value] : candidate.GetLocales())
        if (!saved.GetLocales().contains(locale))
            saved.SetForLocale(locale, value);
}

bool Rando::Traps::RestoreTrapNameTranslations(RandomizerGet id, Text& name, Text& article) {
    if (id <= RG_NONE || id >= RG_MAX)
        return false;
    if (!initTrickNames) {
        InitTrickNames();
        initTrickNames = true;
    }
    const auto restore = [&](const TrickName& candidate) {
        if (!MatchesSavedName(name, candidate.name))
            return false;
        const bool localizedString = name.GetEnglish() != candidate.name.GetEnglish();
        RestoreTextTranslations(name, candidate.name, localizedString);
        RestoreTextTranslations(article, candidate.article, localizedString);
        return true;
    };
    for (const auto& candidate : trickNameTable[id])
        if (restore(candidate))
            return true;
    // Unchanged/revealed trap names use the real item rather than the joke pool.
    for (const auto itemId : { id, RG_ICE_TRAP }) {
        const auto& item = Rando::StaticData::RetrieveItem(itemId);
        if (restore({ item.GetName(), item.GetArticle() }))
            return true;
    }
    return false;
}

RandomizerGet Rando::Traps::GetTrapTrickModel(uint64_t* state) {
    auto ctx = Rando::Context::GetInstance();
    RandomizerGet trickModel = ShipUtils::RandomElementFromSet(ctx->possibleIceTrapModels, state);

    if (trickModel == RG_EMPTY_BOTTLE) {
        trickModel = ShipUtils::RandomElement(Rando::StaticData::normalBottles, state);
    } else if (trickModel == RG_GUARD_HOUSE_KEY) {
        trickModel = ShipUtils::RandomElement(Rando::StaticData::overworldKeys, state);
    } else if (trickModel == RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL) {
        trickModel = ShipUtils::RandomElement(Rando::StaticData::beanSouls, state);
    } else if (trickModel == RG_SHADOW_SILVER_BLADES) {
        trickModel = ShipUtils::RandomElement(Rando::StaticData::silverRupees);
    }

    return trickModel;
}

bool Rando::Traps::ShouldJunkItemBeTrap() {
    auto ctx = Rando::Context::GetInstance();
    return ctx->GetOption(RSK_ICE_TRAP_PERCENT).IsNot(0) &&
           (ctx->GetOption(RSK_ICE_TRAP_PERCENT).Is(100) ||
            Random(0, 100) < ctx->GetOption(RSK_ICE_TRAP_PERCENT).Get());
}

static const std::vector<std::string>& GetEnglishIceTrapMessages() {
    static const auto names = CustomMessages::GetGameTextPool("en_US", "randomizer.traps.messages");
    return names;
}

static const std::vector<std::string>& GetGermanIceTrapMessages() {
    static const auto names = CustomMessages::GetGameTextPool("de_DE", "randomizer.traps.messages");
    return names;
}

static const std::vector<std::string>& GetFrenchIceTrapMessages() {
    static const auto names = CustomMessages::GetGameTextPool("fr_FR", "randomizer.traps.messages");
    return names;
}

extern "C" SaveContext gSaveContext;

static std::string ReplaceItemName(const char* c_str, GetItemEntry getItemEntry, uint8_t language) {
    const static std::string placeholder = "${itemName}";

    std::string str = std::string(c_str);

    std::string name = CustomMessages::GetEnglishText("randomizer.traps.unknown_item");

    if (getItemEntry.drawModIndex == MOD_NONE) {
        name = SohUtils::GetGameItemName(getItemEntry.drawItemId, language);
    } else if (getItemEntry.drawModIndex == MOD_RANDOMIZER) {
        name =
            Rando::StaticData::RetrieveItem((RandomizerGet)getItemEntry.drawItemId).GetName().GetForLanguage(language);
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
        msg = CustomMessages::CreateMessage("randomizer.traps.christmas", { QM_BLUE });
    } else {
        msg = CustomMessage(
            ReplaceItemName(ShipUtils::RandomElement(GetEnglishIceTrapMessages()).c_str(), getItemEntry, LANGUAGE_ENG),
            ReplaceItemName(ShipUtils::RandomElement(GetGermanIceTrapMessages()).c_str(), getItemEntry, LANGUAGE_GER),
            ReplaceItemName(ShipUtils::RandomElement(GetFrenchIceTrapMessages()).c_str(), getItemEntry, LANGUAGE_FRA),
            { QM_BLUE, QM_BLUE, QM_BLUE });
        for (const auto& locale : CustomMessages::GetGameTextCatalog().GetLanguages()) {
            if (locale == "en_US" || locale == "de_DE" || locale == "fr_FR")
                continue;
            auto text = ShipUtils::RandomElement(CustomMessages::GetGameTextPool(locale, "randomizer.traps.messages"));
            std::string name = CustomMessages::GetGameText(locale, "randomizer.traps.unknown_item");
            if (getItemEntry.drawModIndex == MOD_NONE) {
                if (const auto* value = CustomMessages::GetGameTextCatalog().Find(
                        locale, "messages.vanilla.item_names." + std::to_string(getItemEntry.drawItemId)))
                    name = *value;
            } else if (getItemEntry.drawModIndex == MOD_RANDOMIZER) {
                name = Rando::StaticData::RetrieveItem((RandomizerGet)getItemEntry.drawItemId)
                           .GetName()
                           .GetForLocale(locale);
            }
            size_t cursor = 0;
            while ((cursor = text.find("${itemName}", cursor)) != std::string::npos) {
                text.replace(cursor, 11, name);
                cursor += name.size();
            }
            msg.SetForLocale(locale, std::move(text));
        }
    }

    msg.AutoFormat();
}
