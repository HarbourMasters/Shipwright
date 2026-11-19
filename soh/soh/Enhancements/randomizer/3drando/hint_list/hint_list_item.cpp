#include "soh/Enhancements/custom-message/MessageCatalog.h"
#include "../../static_data.h"
#include "../hints.hpp"
#include "../../../custom-message/CustomMessageManager.h"

namespace Rando {

void StaticData::HintTable_Init_Item() {
    // clang-format off

    hintTextTable[RHT_KOKIRI_SWORD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_SWORD.clear"),
                                               // /*spanish*/la Espada Kokiri
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_SWORD.ambiguous.0")
                                                // /*spanish*/una espada
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_SWORD.obscure.0"),
                                                // /*spanish*/un ágil puñal
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_SWORD.obscure.1"),
                                                // /*spanish*/una hoja de principiantes
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_SWORD.obscure.2")});
                                                // /*spanish*/una navaja

    hintTextTable[RHT_MASTER_SWORD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASTER_SWORD.clear"),
                                               // /*spanish*/la Espada Maestra
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_MASTER_SWORD.ambiguous.0")
                                                // /*spanish*/una espada
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_MASTER_SWORD.obscure.0"),
                                                // /*spanish*/la destructora del mal
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_MASTER_SWORD.obscure.1")});
                                                // /*spanish*/unos siete años de espera

    hintTextTable[RHT_GIANTS_KNIFE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GIANTS_KNIFE.clear"),
                                               // /*spanish*/la daga gigante
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_GIANTS_KNIFE.ambiguous.0")
                                                // /*spanish*/una espada
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_GIANTS_KNIFE.obscure.0"),
                                                // /*spanish*/una frágil hoja
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_GIANTS_KNIFE.obscure.1")});
                                                // /*spanish*/un rompible acero

    hintTextTable[RHT_BIGGORON_SWORD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BIGGORON_SWORD.clear"),
                                                 // /*spanish*/la Espada de Biggoron
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_BIGGORON_SWORD.ambiguous.0")
                                                  // /*spanish*/una espada
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_BIGGORON_SWORD.obscure.0"),
                                                  // /*spanish*/el mayor mandoble
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_BIGGORON_SWORD.obscure.1")});
                                                  // /*spanish*/un estoque colosal

    hintTextTable[RHT_DEKU_SHIELD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_SHIELD.clear"),
                                              // /*spanish*/un escudo deku
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_SHIELD.ambiguous.0")
                                               // /*spanish*/un escudo
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_SHIELD.obscure.0"),
                                               // /*spanish*/una protección del bosque
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_SHIELD.obscure.1")});
                                               // /*spanish*/una barrera quemable

    hintTextTable[RHT_HYLIAN_SHIELD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HYLIAN_SHIELD.clear"),
                                                // /*spanish*/un escudo hyliano
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_HYLIAN_SHIELD.ambiguous.0")
                                                 // /*spanish*/un escudo
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_HYLIAN_SHIELD.obscure.0"),
                                                 // /*spanish*/una protección de acero
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_HYLIAN_SHIELD.obscure.1")});
                                                 // /*spanish*/un alimento de Like Like

    hintTextTable[RHT_MIRROR_SHIELD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MIRROR_SHIELD.clear"),
                                                // /*spanish*/el escudo espejo
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_MIRROR_SHIELD.ambiguous.0")
                                                 // /*spanish*/un escudo
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_MIRROR_SHIELD.obscure.0"),
                                                 // /*spanish*/una muralla reflectora
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_MIRROR_SHIELD.obscure.1"),
                                                 // /*spanish*/la debilidad de Medusa
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_MIRROR_SHIELD.obscure.2")});
                                                 // /*spanish*/una superficie plateada

    hintTextTable[RHT_GORON_TUNIC] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_TUNIC.clear"),
                                              // /*spanish*/un sayo goron
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_TUNIC.ambiguous.0")
                                               // /*spanish*/un sayo
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_TUNIC.obscure.0"),
                                               // /*spanish*/una vestimenta rubí
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_TUNIC.obscure.1"),
                                               // /*spanish*/una ignífuga prenda
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_TUNIC.obscure.2")});
                                               // /*spanish*/unos abrasantes ropajes

    hintTextTable[RHT_ZORA_TUNIC] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_TUNIC.clear"),
                                             // /*spanish*/un sayo zora
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_TUNIC.ambiguous.0"),
                                              // /*spanish*/un sayo
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_TUNIC.ambiguous.1")
                                              // /*spanish*/algo caro
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_TUNIC.obscure.0"),
                                              // /*spanish*/una vestidura zafiro
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_TUNIC.obscure.1"),
                                              // /*spanish*/un traje impermeable
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_TUNIC.obscure.2")});
                                              // /*spanish*/unos ropajes sumergibles

    hintTextTable[RHT_IRON_BOOTS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_IRON_BOOTS.clear"),
                                             // /*spanish*/las botas de hierro
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_IRON_BOOTS.ambiguous.0"),
                                              // /*spanish*/un par de botas
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_IRON_BOOTS.ambiguous.1"),
                                              // /*spanish*/algo particular del Templo del Agua
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_IRON_BOOTS.ambiguous.2")
                                              // /*spanish*/algo de lo más pesado
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_IRON_BOOTS.obscure.0"),
                                              // /*spanish*/un calzado de las profundidades
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_IRON_BOOTS.obscure.1")});
                                              // /*spanish*/unas suelas férreas

    hintTextTable[RHT_HOVER_BOOTS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HOVER_BOOTS.clear"),
                                              // /*spanish*/las botas voladoras
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_HOVER_BOOTS.ambiguous.0")
                                               // /*spanish*/un par de botas
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_HOVER_BOOTS.obscure.0"),
                                               // /*spanish*/unas suelas resvaladizas
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_HOVER_BOOTS.obscure.1"),
                                               // /*spanish*/unos escurridizos botines
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_HOVER_BOOTS.obscure.2")});
                                               // /*spanish*/un calzado antigravitatorio

    hintTextTable[RHT_ZELDAS_LETTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZELDAS_LETTER.clear"),
                                                // /*spanish*/la carta de Zelda
                                                {}, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_ZELDAS_LETTER.obscure.0"),
                                                 // /*spanish*/un autógrafo
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_ZELDAS_LETTER.obscure.1"),
                                                 // /*spanish*/un escrito real
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_ZELDAS_LETTER.obscure.2")});
                                                 // /*spanish*/correo de la realeza

    hintTextTable[RHT_WEIRD_EGG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WEIRD_EGG.clear"),
                                            // /*spanish*/el huevo extraño
                                            {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_WEIRD_EGG.ambiguous.0")
                                             // /*spanish*/un huevo
                                            }, {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_WEIRD_EGG.obscure.0")});
                                             // /*spanish*/el dilema de la gallina

    hintTextTable[RHT_BOOMERANG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOOMERANG.clear"),
                                            // /*spanish*/el bumerán
                                            {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOOMERANG.ambiguous.0"),
                                             // /*spanish*/algo que pueda agarrar cosas
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOOMERANG.ambiguous.1")
                                             // /*spanish*/algo que pueda paralizar
                                            }, {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOOMERANG.obscure.0"),
                                             // /*spanish*/un plátano
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOOMERANG.obscure.1"),
                                             // /*spanish*/un palo aturdidor
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOOMERANG.obscure.2")});
                                             // /*spanish*/un ángulo amarillo

    hintTextTable[RHT_LENS_OF_TRUTH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LENS_OF_TRUTH.clear"),
                                                // /*spanish*/la Lupa de la Verdad
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_LENS_OF_TRUTH.ambiguous.0")
                                                 // /*spanish*/un instrumento para hallar objetos
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_LENS_OF_TRUTH.obscure.0"),
                                                 // /*spanish*/el detector de ilusiones
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_LENS_OF_TRUTH.obscure.1"),
                                                 // /*spanish*/el rastreador paranormal
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_LENS_OF_TRUTH.obscure.2"),
                                                 // /*spanish*/el ojo que todo ve
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_LENS_OF_TRUTH.obscure.3")});
                                                 // /*spanish*/la revelación verdadera

    hintTextTable[RHT_MEGATON_HAMMER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MEGATON_HAMMER.clear"),
                                                 // /*spanish*/el martillo Megatón
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_MEGATON_HAMMER.ambiguous.0")
                                                  // /*spanish*/algo que pueda quitar rocas
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_MEGATON_HAMMER.obscure.0"),
                                                  // /*spanish*/un destructor de dragones
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_MEGATON_HAMMER.obscure.1"),
                                                  // /*spanish*/un mazo de metal
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_MEGATON_HAMMER.obscure.2")});
                                                  // /*spanish*/un machacador

    hintTextTable[RHT_STONE_OF_AGONY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_STONE_OF_AGONY.clear"),
                                                 // /*spanish*/la Piedra de la Agonía
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_STONE_OF_AGONY.ambiguous.0"),
                                                  // /*spanish*/un obsequio de la Casa Skulltula
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_STONE_OF_AGONY.ambiguous.1")
                                                  // /*spanish*/un instrumento para hallar objetos
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_STONE_OF_AGONY.obscure.0"),
                                                  // /*spanish*/el fragmento tintineante
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_STONE_OF_AGONY.obscure.1")});
                                                  // /*spanish*/una azul alarma

    hintTextTable[RHT_DINS_FIRE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DINS_FIRE.clear"),
                                            // /*spanish*/el Fuego de Din
                                            {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_DINS_FIRE.ambiguous.0")
                                             // /*spanish*/el poder de una Gran Hada
                                            }, {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_DINS_FIRE.obscure.0"),
                                             // /*spanish*/un incendio
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_DINS_FIRE.obscure.1"),
                                             // /*spanish*/una onda de calor
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_DINS_FIRE.obscure.2")});
                                             // /*spanish*/una roja esfera

    hintTextTable[RHT_FARORES_WIND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FARORES_WIND.clear"),
                                               // /*spanish*/el Viento de Farore
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_FARORES_WIND.ambiguous.0")
                                                // /*spanish*/el poder de una Gran Hada
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_FARORES_WIND.obscure.0"),
                                                // /*spanish*/un teletransportador
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_FARORES_WIND.obscure.1"),
                                                // /*spanish*/una runa de transporte
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_FARORES_WIND.obscure.2")});
                                                // /*spanish*/una verde esfera

    hintTextTable[RHT_NAYRUS_LOVE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_NAYRUS_LOVE.clear"),
                                              // /*spanish*/el Amor de Nayru
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_NAYRUS_LOVE.ambiguous.0")
                                               // /*spanish*/el poder de una Gran Hada
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_NAYRUS_LOVE.obscure.0"),
                                               // /*spanish*/una seguridad temporal
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_NAYRUS_LOVE.obscure.1"),
                                               // /*spanish*/un aura impenetrable
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_NAYRUS_LOVE.obscure.2")});
                                               // /*spanish*/una barrera azul

    hintTextTable[RHT_FIRE_ARROWS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_ARROWS.clear"),
                                              // /*spanish*/la flecha de fuego
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_ARROWS.ambiguous.0")
                                               // /*spanish*/una flecha mágica
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_ARROWS.obscure.0"),
                                               // /*spanish*/el ardiente aguijón
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_ARROWS.obscure.1"),
                                               // /*spanish*/las puntas ígneas
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_ARROWS.obscure.2")});
                                               // /*spanish*/el misil abrasador

    hintTextTable[RHT_ICE_ARROWS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_ARROWS.clear"),
                                             // /*spanish*/la flecha de hielo
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_ARROWS.ambiguous.0"),
                                              // /*spanish*/una flecha mágica
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_ARROWS.ambiguous.1")
                                              // /*spanish*/algo que pueda paralizar
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_ARROWS.obscure.0"),
                                              // /*spanish*/el misil congelador
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_ARROWS.obscure.1"),
                                              // /*spanish*/las puntas gélidas
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_ARROWS.obscure.2")});
                                              // /*spanish*/el control de escarcha

    hintTextTable[RHT_LIGHT_ARROWS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_ARROWS.clear"),
                                               // /*spanish*/la flecha de luz
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_ARROWS.ambiguous.0")
                                                // /*spanish*/una flecha mágica
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_ARROWS.obscure.0"),
                                                // /*spanish*/el haz de luz
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_ARROWS.obscure.1"),
                                                // /*spanish*/el disparo luminoso
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_ARROWS.obscure.2"),
                                                // /*spanish*/la perdición de Ganondorf
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_ARROWS.obscure.3")});
                                                // /*spanish*/las puntas resplandecientes

    hintTextTable[RHT_GERUDO_MEMBERSHIP_CARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_MEMBERSHIP_CARD.clear"),
                                                         // /*spanish*/el pase de socio gerudo
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_MEMBERSHIP_CARD.ambiguous.0")
                                                          // /*spanish*/una prueba de reconocimiento
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_MEMBERSHIP_CARD.obscure.0"),
                                                          // /*spanish*/una fémina membresía
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_MEMBERSHIP_CARD.obscure.1")});
                                                          // /*spanish*/el vale del desierto

    hintTextTable[RHT_MAGIC_BEAN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MAGIC_BEAN.clear"),
                                             // /*spanish*/una judía mágica
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_MAGIC_BEAN.ambiguous.0")
                                              // /*spanish*/algo a veces enterrado
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_MAGIC_BEAN.obscure.0")});
                                              // /*spanish*/una legumbre hechizada

    hintTextTable[RHT_MAGIC_BEAN_PACK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MAGIC_BEAN_PACK.clear"),
                                                  // /*spanish*/unas judías mágicas
                                                  {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_MAGIC_BEAN_PACK.ambiguous.0")
                                                   // /*spanish*/algo a veces enterrado
                                                  }, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_MAGIC_BEAN_PACK.obscure.0")});
                                                   // /*spanish*/unas legumbres hechizadas

    hintTextTable[RHT_DOUBLE_DEFENSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DOUBLE_DEFENSE.clear"),
                                                 // /*spanish*/la doble defensa
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_DOUBLE_DEFENSE.ambiguous.0"),
                                                  // /*spanish*/el poder de una Gran Hada
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_DOUBLE_DEFENSE.ambiguous.1")
                                                  // /*spanish*/algo con forma de corazón
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_DOUBLE_DEFENSE.obscure.0"),
                                                  // /*spanish*/un contorno blanco
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_DOUBLE_DEFENSE.obscure.1"),
                                                  // /*spanish*/una reducción de daño
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_DOUBLE_DEFENSE.obscure.2")});
                                                  // /*spanish*/un amor fortalecido

    hintTextTable[RHT_GOLD_SKULLTULA_TOKEN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GOLD_SKULLTULA_TOKEN.clear"),
                                                       // /*spanish*/un símbolo de skulltula dorada
                                                       {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_GOLD_SKULLTULA_TOKEN.ambiguous.0"),
                                                        // /*spanish*/una prueba de reconocimiento
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_GOLD_SKULLTULA_TOKEN.ambiguous.1")
                                                        // /*spanish*/algo a veces enterrado
                                                       }, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_GOLD_SKULLTULA_TOKEN.obscure.0"),
                                                        // /*spanish*/una prueba de la destrucción
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_GOLD_SKULLTULA_TOKEN.obscure.1"),
                                                        // /*spanish*/una figura arácnida
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_GOLD_SKULLTULA_TOKEN.obscure.2"),
                                                        // /*spanish*/unos restos dorados
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_GOLD_SKULLTULA_TOKEN.obscure.3")});
                                                        // /*spanish*/una centésima de una maldición

    hintTextTable[RHT_POCKET_EGG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POCKET_EGG.clear"),
                                             // /*spanish*/el huevo de bolsillo
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_POCKET_EGG.ambiguous.0"),
                                              // /*spanish*/un objeto de una misión secundaria
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_POCKET_EGG.ambiguous.1")
                                              // /*spanish*/un huevo
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_POCKET_EGG.obscure.0"),
                                              // /*spanish*/cuco contenido
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_POCKET_EGG.obscure.1"),
                                              // /*spanish*/un futuro cuco
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_POCKET_EGG.obscure.2")});
                                              // /*spanish*/una dulce juventud

    hintTextTable[RHT_POCKET_CUCCO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POCKET_CUCCO.clear"),
                                               // /*spanish*/el cuco de bolsillo
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_POCKET_CUCCO.ambiguous.0")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_POCKET_CUCCO.obscure.0")});
                                                // /*spanish*/un pollito chiquito

    hintTextTable[RHT_COJIRO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COJIRO.clear"),
                                         // /*spanish*/a Cojiro
                                         {
                                         CustomMessages::CreateMessage("randomizer.hints.RHT_COJIRO.ambiguous.0")
                                          // /*spanish*/un objeto de una misión secundaria
                                         }, {
                                         CustomMessages::CreateMessage("randomizer.hints.RHT_COJIRO.obscure.0")});
                                          // /*spanish*/un cerúleo capón

    hintTextTable[RHT_ODD_MUSHROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ODD_MUSHROOM.clear"),
                                               // /*spanish*/un champiñón extraño
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_ODD_MUSHROOM.ambiguous.0")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_ODD_MUSHROOM.obscure.0")});
                                                // /*spanish*/un oloroso ingrediente

    hintTextTable[RHT_ODD_POTION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ODD_POTION.clear"),
                                             // /*spanish*/una medicina rara
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ODD_POTION.ambiguous.0"),
                                              // /*spanish*/algo que contenga medicina
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ODD_POTION.ambiguous.1"),
                                              // /*spanish*/algo con un olor extraño
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ODD_POTION.ambiguous.2")
                                              // /*spanish*/un objeto de una misión secundaria
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_ODD_POTION.obscure.0")});
                                              // /*spanish*/la especialidad de la abuela

    hintTextTable[RHT_POACHERS_SAW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POACHERS_SAW.clear"),
                                               // /*spanish*/la sierra del furtivo
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_POACHERS_SAW.ambiguous.0")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_POACHERS_SAW.obscure.0")});
                                                // /*spanish*/un destructor de árboles

    hintTextTable[RHT_BROKEN_SWORD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BROKEN_SWORD.clear"),
                                               // /*spanish*/la espada goron rota
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_BROKEN_SWORD.ambiguous.0"),
                                                // /*spanish*/un objeto de una misión secundaria
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_BROKEN_SWORD.ambiguous.1")
                                                // /*spanish*/una espada
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_BROKEN_SWORD.obscure.0")});
                                                // /*spanish*/una rebanadora rota

    hintTextTable[RHT_PRESCRIPTION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PRESCRIPTION.clear"),
                                               // /*spanish*/la receta
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_PRESCRIPTION.ambiguous.0")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_PRESCRIPTION.obscure.0"),
                                                // /*spanish*/un instructivo medicinal
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_PRESCRIPTION.obscure.1")});
                                                // /*spanish*/unas notas del doctor

    hintTextTable[RHT_EYEBALL_FROG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_EYEBALL_FROG.clear"),
                                               // /*spanish*/la rana de ojos saltones
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_EYEBALL_FROG.ambiguous.0")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_EYEBALL_FROG.obscure.0")});
                                                // /*spanish*/un variopinto batracio

    hintTextTable[RHT_EYEDROPS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_EYEDROPS.clear"),
                                           // /*spanish*/las supergotas oculares
                                           {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_EYEDROPS.ambiguous.0")
                                            // /*spanish*/un objeto de una misión secundaria
                                           }, {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_EYEDROPS.obscure.0")});
                                            // /*spanish*/un remedio para la vista

    hintTextTable[RHT_CLAIM_CHECK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CLAIM_CHECK.clear"),
                                              // /*spanish*/el recibo
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_CLAIM_CHECK.ambiguous.0")
                                               // /*spanish*/un objeto de una misión secundaria
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_CLAIM_CHECK.obscure.0")});
                                               // /*spanish*/unos tres días de espera

    hintTextTable[RHT_PROGRESSIVE_HOOKSHOT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_HOOKSHOT.clear"),
                                                       // /*spanish*/un gancho
                                                       {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_HOOKSHOT.ambiguous.0"),
                                                        // /*spanish*/algo que pueda agarrar cosas
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_HOOKSHOT.ambiguous.1")
                                                        // /*spanish*/algo que pueda paralizar
                                                       }, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_HOOKSHOT.obscure.0"),
                                                        // /*spanish*/un recuerdo de Dampé
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_HOOKSHOT.obscure.1"),
                                                        // /*spanish*/una garra metálica
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_HOOKSHOT.obscure.2")});
                                                        // /*spanish*/una cadena retráctil

    hintTextTable[RHT_PROGRESSIVE_STRENGTH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_STRENGTH.clear"),
                                                       // /*spanish*/un aumento de fuerza
                                                       {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_STRENGTH.ambiguous.0")
                                                        // /*spanish*/algo que pueda quitar rocas
                                                       }, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_STRENGTH.obscure.0"),
                                                        // /*spanish*/unos poderosos guanteletes
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_STRENGTH.obscure.1"),
                                                        // /*spanish*/unas manoplas metálicas
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_STRENGTH.obscure.2")});
                                                        // /*spanish*/un levantamiento pesado

    hintTextTable[RHT_PROGRESSIVE_BOMB_BAG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMB_BAG.clear"),
                                                       // /*spanish*/un saco de bombas
                                                       {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMB_BAG.ambiguous.0"),
                                                        // /*spanish*/un montón de explosivos
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMB_BAG.ambiguous.1")
                                                        // /*spanish*/algo que pueda quitar rocas
                                                       }, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMB_BAG.obscure.0"),
                                                        // /*spanish*/un recipiente explosivo
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMB_BAG.obscure.1")});
                                                        // /*spanish*/un zurrón de estallidos

    hintTextTable[RHT_PROGRESSIVE_BOW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOW.clear"),
                                                  // /*spanish*/un arco de las hadas
                                                  {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOW.ambiguous.0")
                                                   // /*spanish*/un arma de proyectil
                                                  }, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOW.obscure.0"),
                                                   // /*spanish*/un tiro al blanco
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOW.obscure.1")});
                                                   // /*spanish*/un peligroso lanzadardos

    hintTextTable[RHT_PROGRESSIVE_SLINGSHOT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_SLINGSHOT.clear"),
                                                        // /*spanish*/una resortera de las hadas
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_SLINGSHOT.ambiguous.0")
                                                         // /*spanish*/un arma de proyectil
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_SLINGSHOT.obscure.0"),
                                                         // /*spanish*/un lanzasemillas
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_SLINGSHOT.obscure.1"),
                                                         // /*spanish*/un tirachinas
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_SLINGSHOT.obscure.2")});
                                                         // /*spanish*/una catapulta infantil

    hintTextTable[RHT_PROGRESSIVE_WALLET] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_WALLET.clear"),
                                                     // /*spanish*/una bolsa de rupias
                                                     {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_WALLET.ambiguous.0")
                                                      // /*spanish*/un obsequio de la Casa Skulltula
                                                     }, {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_WALLET.obscure.0"),
                                                      // /*spanish*/una cartera de dinero
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_WALLET.obscure.1"),
                                                      // /*spanish*/un zurrón de gemas
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_WALLET.obscure.2")});
                                                      // /*spanish*/un banco portable

    hintTextTable[RHT_PROGRESSIVE_SCALE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_SCALE.clear"),
                                                    // /*spanish*/una escama Zora
                                                    {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_SCALE.ambiguous.0")
                                                     // /*spanish*/un instrumento de buceo
                                                    }, {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_SCALE.obscure.0"),
                                                     // /*spanish*/un profundo buceo
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_SCALE.obscure.1")});
                                                     // /*spanish*/un fragmento de Zora

    hintTextTable[RHT_PROGRESSIVE_NUT_UPGRADE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_NUT_UPGRADE.clear"),
                                                          // /*spanish*/un aumento de nueces deku
                                                          {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_NUT_UPGRADE.ambiguous.0"),
                                                           // /*spanish*/un montón de municiones Deku
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_NUT_UPGRADE.ambiguous.1")
                                                           // /*spanish*/algo que pueda paralizar
                                                          }, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_NUT_UPGRADE.obscure.0"),
                                                           // /*spanish*/más semillas de nogal
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_NUT_UPGRADE.obscure.1")});
                                                           // /*spanish*/más frutos aturdidores

    hintTextTable[RHT_PROGRESSIVE_STICK_UPGRADE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_STICK_UPGRADE.clear"),
                                                            // /*spanish*/un aumento de palos deku
                                                            {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_STICK_UPGRADE.ambiguous.0")
                                                             // /*spanish*/un montón de municiones Deku
                                                            }, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_STICK_UPGRADE.obscure.0"),
                                                             // /*spanish*/más bastones
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_STICK_UPGRADE.obscure.1")});
                                                             // /*spanish*/más varas

    hintTextTable[RHT_PROGRESSIVE_MAGIC_METER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_MAGIC_METER.clear"),
                                                          // /*spanish*/un aumento de poder mágico
                                                          {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_MAGIC_METER.ambiguous.0")
                                                           // /*spanish*/el poder de una Gran Hada
                                                          }, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_MAGIC_METER.obscure.0"),
                                                           // /*spanish*/una maestría mística
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_MAGIC_METER.obscure.1"),
                                                           // /*spanish*/un polvo de hada
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_MAGIC_METER.obscure.2")});
                                                           // /*spanish*/una verduzca barra

    hintTextTable[RHT_PROGRESSIVE_OCARINA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_OCARINA.clear"),
                                                      // /*spanish*/una ocarina
                                                      {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_OCARINA.ambiguous.0"),
                                                       // /*spanish*/un obsequio de Saria
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_OCARINA.ambiguous.1")
                                                       // /*spanish*/algo guardado por la familia real
                                                      }, {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_OCARINA.obscure.0"),
                                                       // /*spanish*/un utensilio musical
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_OCARINA.obscure.1")});
                                                       // /*spanish*/un instrumento

    hintTextTable[RHT_PROGRESSIVE_BOMBCHUS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMBCHUS.clear"),
                                                       // /*spanish*/unos bombchus
                                                       {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMBCHUS.ambiguous.0"),
                                                        // /*spanish*/un obsequio de la Casa Skulltula
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMBCHUS.ambiguous.1")
                                                        // /*spanish*/un montón de explosivos
                                                       }, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMBCHUS.obscure.0"),
                                                        // /*spanish*/unas bombas roedoras
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMBCHUS.obscure.1"),
                                                        // /*spanish*/unos explosivos ratoncitos
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMBCHUS.obscure.2"),
                                                        // /*spanish*/unos trepaparedes
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_BOMBCHUS.obscure.3")});
                                                        // /*spanish*/unas ratas propulsadas

    hintTextTable[RHT_PROGRESSIVE_GORONSWORD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_GORONSWORD.clear"),
                                                         // /*spanish*/una espada goron
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_GORONSWORD.ambiguous.0")
                                                          // /*spanish*/una espada
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_GORONSWORD.obscure.0"),
                                                          // /*spanish*/una gran hoja
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_PROGRESSIVE_GORONSWORD.obscure.1")});
                                                          // /*spanish*/un arma goron

    hintTextTable[RHT_EMPTY_BOTTLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_EMPTY_BOTTLE.clear"),
                                               // /*spanish*/una botella
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_EMPTY_BOTTLE.ambiguous.0")
                                                // /*spanish*/una botella
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_EMPTY_BOTTLE.obscure.0"),
                                                // /*spanish*/un recipiente de cristal
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_EMPTY_BOTTLE.obscure.1"),
                                                // /*spanish*/un frasco vacío
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_EMPTY_BOTTLE.obscure.2")});
                                                // /*spanish*/aire a presión

    hintTextTable[RHT_BOTTLE_WITH_MILK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_MILK.clear"),
                                                   // /*spanish*/una botella de leche
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_MILK.ambiguous.0")
                                                    // /*spanish*/una botella
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_MILK.obscure.0"),
                                                    // /*spanish*/una fuente de calcio
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_MILK.obscure.1"),
                                                    // /*spanish*/una bebida nutritiva
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_MILK.obscure.2")});
                                                    // /*spanish*/un trago para bebés

    hintTextTable[RHT_BOTTLE_WITH_RED_POTION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_RED_POTION.clear"),
                                                         // /*spanish*/una botella de poción roja
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_RED_POTION.ambiguous.0")
                                                          // /*spanish*/una botella
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_RED_POTION.obscure.0"),
                                                          // /*spanish*/una pócima vitalicia
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_RED_POTION.obscure.1")});
                                                          // /*spanish*/un remedio rojizo

    hintTextTable[RHT_BOTTLE_WITH_GREEN_POTION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_GREEN_POTION.clear"),
                                                           // /*spanish*/una botella de poción verde
                                                           {
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_GREEN_POTION.ambiguous.0")
                                                            // /*spanish*/una botella
                                                           }, {
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_GREEN_POTION.obscure.0"),
                                                            // /*spanish*/un potingue mágico
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_GREEN_POTION.obscure.1")});
                                                            // /*spanish*/un remedio verduzco

    hintTextTable[RHT_BOTTLE_WITH_BLUE_POTION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BLUE_POTION.clear"),
                                                          // /*spanish*/una botella de poción azul
                                                          {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BLUE_POTION.ambiguous.0")
                                                           // /*spanish*/una botella
                                                          }, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BLUE_POTION.obscure.0"),
                                                           // /*spanish*/un antídoto para el dolor
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BLUE_POTION.obscure.1")});
                                                           // /*spanish*/un remedio índigo

    hintTextTable[RHT_BOTTLE_WITH_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_FAIRY.clear"),
                                                    // /*spanish*/un hada en una botella
                                                    {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_FAIRY.ambiguous.0")
                                                     // /*spanish*/una botella
                                                    }, {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_FAIRY.obscure.0"),
                                                     // /*spanish*/un hada atrapada
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_FAIRY.obscure.1"),
                                                     // /*spanish*/una oportunidad más
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_FAIRY.obscure.2")});
                                                     // /*spanish*/una prima de Navi

    hintTextTable[RHT_BOTTLE_WITH_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_FISH.clear"),
                                                   // /*spanish*/un pez en una botella
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_FISH.ambiguous.0")
                                                    // /*spanish*/una botella
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_FISH.obscure.0"),
                                                    // /*spanish*/un escamado ser
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_FISH.obscure.1")});
                                                    // /*spanish*/un tentempié de cierta deidad

    hintTextTable[RHT_BOTTLE_WITH_BLUE_FIRE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BLUE_FIRE.clear"),
                                                        // /*spanish*/una botella de fuego azul
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BLUE_FIRE.ambiguous.0")
                                                         // /*spanish*/una botella
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BLUE_FIRE.obscure.0"),
                                                         // /*spanish*/un incendio retenido
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BLUE_FIRE.obscure.1")});
                                                         // /*spanish*/unas brasas enfrascadas

    hintTextTable[RHT_BOTTLE_WITH_BUGS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BUGS.clear"),
                                                   // /*spanish*/unos insectos en una botella
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BUGS.ambiguous.0")
                                                    // /*spanish*/una botella
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BUGS.obscure.0"),
                                                    // /*spanish*/unos invertebrados seres
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BUGS.obscure.1")});
                                                    // /*spanish*/unos rastreadores de skulltulas

    hintTextTable[RHT_BOTTLE_WITH_POE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_POE.clear"),
                                                  // /*spanish*/un Poe en una botella
                                                  {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_POE.ambiguous.0")
                                                   // /*spanish*/una botella
                                                  }, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_POE.obscure.0"),
                                                   // /*spanish*/un espantoso espectro
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_POE.obscure.1")});
                                                   // /*spanish*/una expresión enfrascada

    hintTextTable[RHT_BOTTLE_WITH_BIG_POE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BIG_POE.clear"),
                                                      // /*spanish*/un Gran Poe en una botella
                                                      {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BIG_POE.ambiguous.0")
                                                       // /*spanish*/una botella
                                                      }, {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BIG_POE.obscure.0"),
                                                       // /*spanish*/el espectro más espeluznante
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTLE_WITH_BIG_POE.obscure.1")});
                                                       // /*spanish*/un buen valorado espíritu

    hintTextTable[RHT_RUTOS_LETTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_RUTOS_LETTER.clear"),
                                               // /*spanish*/la carta de Ruto
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_RUTOS_LETTER.ambiguous.0")
                                                // /*spanish*/una botella
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_RUTOS_LETTER.obscure.0"),
                                                // /*spanish*/una llamada de auxilio
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_RUTOS_LETTER.obscure.1"),
                                                // /*spanish*/un escrito mweep
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_RUTOS_LETTER.obscure.2"),
                                                // /*spanish*/una nota de socorro
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_RUTOS_LETTER.obscure.3")});
                                                // /*spanish*/un mensaje de ayuda

    hintTextTable[RHT_ZELDAS_LULLABY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZELDAS_LULLABY.clear"),
                                                 // /*spanish*/la Nana de Zelda
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ZELDAS_LULLABY.ambiguous.0"),
                                                  // /*spanish*/una cancion normal
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ZELDAS_LULLABY.ambiguous.1")
                                                  // /*spanish*/algo guardado por la familia real
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ZELDAS_LULLABY.obscure.0"),
                                                  // /*spanish*/la canción real
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ZELDAS_LULLABY.obscure.1")});
                                                  // /*spanish*/la melodía de la trifuerza

    hintTextTable[RHT_EPONAS_SONG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_EPONAS_SONG.clear"),
                                              // /*spanish*/la Canción de Epona
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_EPONAS_SONG.ambiguous.0")
                                               // /*spanish*/una cancion normal
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_EPONAS_SONG.obscure.0"),
                                               // /*spanish*/una copla ecuestre
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_EPONAS_SONG.obscure.1"),
                                               // /*spanish*/la sonata de Malon
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_EPONAS_SONG.obscure.2")});
                                               // /*spanish*/un canto rupestre

    hintTextTable[RHT_SARIAS_SONG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SARIAS_SONG.clear"),
                                              // /*spanish*/la Canción de Saria
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_SARIAS_SONG.ambiguous.0"),
                                               // /*spanish*/una cancion normal
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_SARIAS_SONG.ambiguous.1")
                                               // /*spanish*/un obsequio de Saria
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_SARIAS_SONG.obscure.0"),
                                               // /*spanish*/un pegadizo tono goron
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_SARIAS_SONG.obscure.1")});
                                               // /*spanish*/una consulta de asistencia

    hintTextTable[RHT_SUNS_SONG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SUNS_SONG.clear"),
                                            // /*spanish*/la Canción del Sol
                                            {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_SUNS_SONG.ambiguous.0"),
                                             // /*spanish*/una cancion normal
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_SUNS_SONG.ambiguous.1")
                                             // /*spanish*/algo que pueda paralizar
                                            }, {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_SUNS_SONG.obscure.0"),
                                             // /*spanish*/un día soleado
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_SUNS_SONG.obscure.1"),
                                             // /*spanish*/la destructora de Redeads
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_SUNS_SONG.obscure.2")});
                                             // /*spanish*/la destructora de Gibdos

    hintTextTable[RHT_SONG_OF_TIME] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_OF_TIME.clear"),
                                               // /*spanish*/la Canción del tiempo
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_OF_TIME.ambiguous.0")
                                                // /*spanish*/una cancion normal
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_OF_TIME.obscure.0"),
                                                // /*spanish*/la setenada canción
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_OF_TIME.obscure.1")});
                                                // /*spanish*/la melodía eónica

    hintTextTable[RHT_SONG_OF_STORMS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_OF_STORMS.clear"),
                                                 // /*spanish*/la Canción de la Tormenta
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_OF_STORMS.ambiguous.0")
                                                  // /*spanish*/una cancion normal
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_OF_STORMS.obscure.0"),
                                                  // /*spanish*/la danza de la lluvia
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_OF_STORMS.obscure.1"),
                                                  // /*spanish*/una sonata tormentosa
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_OF_STORMS.obscure.2")});
                                                  // /*spanish*/el arranque de molinos

    hintTextTable[RHT_MINUET_OF_FOREST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MINUET_OF_FOREST.clear"),
                                                   // /*spanish*/el Minueto del bosque
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_MINUET_OF_FOREST.ambiguous.0")
                                                    // /*spanish*/una canción de teletransportación
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_MINUET_OF_FOREST.obscure.0"),
                                                    // /*spanish*/la canción de las copas
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_MINUET_OF_FOREST.obscure.1"),
                                                    // /*spanish*/el himno forestal
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_MINUET_OF_FOREST.obscure.2")});
                                                    // /*spanish*/el sendero esmeralda

    hintTextTable[RHT_BOLERO_OF_FIRE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOLERO_OF_FIRE.clear"),
                                                 // /*spanish*/el Bolero del fuego
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_BOLERO_OF_FIRE.ambiguous.0")
                                                  // /*spanish*/una canción de teletransportación
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_BOLERO_OF_FIRE.obscure.0"),
                                                  // /*spanish*/la canción de la lava
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_BOLERO_OF_FIRE.obscure.1"),
                                                  // /*spanish*/el sendero rubí
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_BOLERO_OF_FIRE.obscure.2")});
                                                  // /*spanish*/el verso volcánico

    hintTextTable[RHT_SERENADE_OF_WATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SERENADE_OF_WATER.clear"),
                                                    // /*spanish*/la Serenata del agua
                                                    {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SERENADE_OF_WATER.ambiguous.0")
                                                     // /*spanish*/una canción de teletransportación
                                                    }, {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SERENADE_OF_WATER.obscure.0"),
                                                     // /*spanish*/la canción del estanque
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SERENADE_OF_WATER.obscure.1"),
                                                     // /*spanish*/el sendero zafiro
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SERENADE_OF_WATER.obscure.2")});
                                                     // /*spanish*/la letra del lago

    hintTextTable[RHT_REQUIEM_OF_SPIRIT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_REQUIEM_OF_SPIRIT.clear"),
                                                    // /*spanish*/el Réquiem del espíritu
                                                    {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_REQUIEM_OF_SPIRIT.ambiguous.0")
                                                     // /*spanish*/una canción de teletransportación
                                                    }, {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_REQUIEM_OF_SPIRIT.obscure.0"),
                                                     // /*spanish*/la canción de la gran estatua
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_REQUIEM_OF_SPIRIT.obscure.1"),
                                                     // /*spanish*/el sendero ámbar
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_REQUIEM_OF_SPIRIT.obscure.2")});
                                                     // /*spanish*/la estrofa del desierto

    hintTextTable[RHT_NOCTURNE_OF_SHADOW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_NOCTURNE_OF_SHADOW.clear"),
                                                     // /*spanish*/el Nocturno de la sombra
                                                     {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_NOCTURNE_OF_SHADOW.ambiguous.0")
                                                      // /*spanish*/una canción de teletransportación
                                                     }, {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_NOCTURNE_OF_SHADOW.obscure.0"),
                                                      // /*spanish*/la canción de los espectros
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_NOCTURNE_OF_SHADOW.obscure.1"),
                                                      // /*spanish*/una honra fúnebre
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_NOCTURNE_OF_SHADOW.obscure.2"),
                                                      // /*spanish*/una estrofa encantada
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_NOCTURNE_OF_SHADOW.obscure.3")});
                                                      // /*spanish*/el sendero malva

    hintTextTable[RHT_PRELUDE_OF_LIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PRELUDE_OF_LIGHT.clear"),
                                                   // /*spanish*/el Preludio de la luz
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_PRELUDE_OF_LIGHT.ambiguous.0")
                                                    // /*spanish*/una canción de teletransportación
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_PRELUDE_OF_LIGHT.obscure.0"),
                                                    // /*spanish*/la melodía refulgente
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_PRELUDE_OF_LIGHT.obscure.1"),
                                                    // /*spanish*/el sendero resplandeciente
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_PRELUDE_OF_LIGHT.obscure.2")});
                                                    // /*spanish*/la ruta del templo

    hintTextTable[RHT_DEKU_TREE_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MAP.clear"),
                                                // /*spanish*/el mapa del Gran Árbol Deku
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MAP.ambiguous.0")
                                                 // /*spanish*/un mapa
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MAP.obscure.0"),
                                                 // /*spanish*/un atlas musgoso
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MAP.obscure.1")});
                                                 // /*spanish*/unos planos musgosos

    hintTextTable[RHT_DODONGOS_CAVERN_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MAP.clear"),
                                                      // /*spanish*/el mapa de la Cueva de los Dodongos
                                                      {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MAP.ambiguous.0")
                                                       // /*spanish*/un mapa
                                                      }, {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MAP.obscure.0"),
                                                       // /*spanish*/un atlas rocoso
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MAP.obscure.1")});
                                                       // /*spanish*/unos planos rocosos

    hintTextTable[RHT_JABU_JABUS_BELLY_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MAP.clear"),
                                                       // /*spanish*/el mapa de la Tripa de Jabu-Jabu
                                                       {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MAP.ambiguous.0")
                                                        // /*spanish*/un mapa
                                                       }, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MAP.obscure.0"),
                                                        // /*spanish*/un atlas digesto
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MAP.obscure.1")});
                                                        // /*spanish*/unos planos digestos

    hintTextTable[RHT_FOREST_TEMPLE_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MAP.clear"),
                                                    // /*spanish*/el mapa del Templo del Bosque
                                                    {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MAP.ambiguous.0")
                                                     // /*spanish*/un mapa
                                                    }, {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MAP.obscure.0"),
                                                     // /*spanish*/un atlas enselvado
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MAP.obscure.1")});
                                                     // /*spanish*/unos planos enselvados

    hintTextTable[RHT_FIRE_TEMPLE_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MAP.clear"),
                                                  // /*spanish*/el mapa del Templo del Fuego
                                                  {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MAP.ambiguous.0")
                                                   // /*spanish*/un mapa
                                                  }, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MAP.obscure.0"),
                                                   // /*spanish*/un atlas fundido
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MAP.obscure.1")});
                                                   // /*spanish*/unos planos fundidos

    hintTextTable[RHT_WATER_TEMPLE_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MAP.clear"),
                                                   // /*spanish*/el mapa del Templo del Agua
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MAP.ambiguous.0")
                                                    // /*spanish*/un mapa
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MAP.obscure.0"),
                                                    // /*spanish*/un atlas mojado
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MAP.obscure.1")});
                                                    // /*spanish*/unos planos mojados

    hintTextTable[RHT_SPIRIT_TEMPLE_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MAP.clear"),
                                                    // /*spanish*/el mapa del Templo del Espíritu
                                                    {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MAP.ambiguous.0")
                                                     // /*spanish*/un mapa
                                                    }, {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MAP.obscure.0"),
                                                     // /*spanish*/un atlas arenoso
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MAP.obscure.1")});
                                                     // /*spanish*/unos planos arenosos

    hintTextTable[RHT_SHADOW_TEMPLE_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MAP.clear"),
                                                    // /*spanish*/el mapa del Templo de las Sombras
                                                    {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MAP.ambiguous.0")
                                                     // /*spanish*/un mapa
                                                    }, {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MAP.obscure.0"),
                                                     // /*spanish*/un atlas siniestra
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MAP.obscure.1")});
                                                     // /*spanish*/unos planos siniestras

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MAP.clear"),
                                                         // /*spanish*/el mapa del Fondo del pozo
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MAP.ambiguous.0")
                                                          // /*spanish*/un mapa
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MAP.obscure.0"),
                                                          // /*spanish*/un atlas mohoso
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MAP.obscure.1")});
                                                          // /*spanish*/unos planos mohosos

    hintTextTable[RHT_ICE_CAVERN_MAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MAP.clear"),
                                                 // /*spanish*/el mapa de la Caverna de hielo
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MAP.ambiguous.0")
                                                  // /*spanish*/un mapa
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MAP.obscure.0"),
                                                  // /*spanish*/un atlas polar
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MAP.obscure.1")});
                                                  // /*spanish*/unos planos polars

    hintTextTable[RHT_DEKU_TREE_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_COMPASS.clear"),
                                                    // /*spanish*/la brújula del Gran Árbol Deku
                                                    {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_COMPASS.ambiguous.0")
                                                     // /*spanish*/una brújula
                                                    }, {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_COMPASS.obscure.0"),
                                                     // /*spanish*/un zahorí musgoso
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_COMPASS.obscure.1")});
                                                     // /*spanish*/un imán musgoso

    hintTextTable[RHT_DODONGOS_CAVERN_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_COMPASS.clear"),
                                                          // /*spanish*/la brújula de la Cueva de los Dodongos
                                                          {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_COMPASS.ambiguous.0")
                                                           // /*spanish*/una brújula
                                                          }, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_COMPASS.obscure.0"),
                                                           // /*spanish*/un zahorí rocoso
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_COMPASS.obscure.1")});
                                                           // /*spanish*/un imán rocoso

    hintTextTable[RHT_JABU_JABUS_BELLY_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_COMPASS.clear"),
                                                           // /*spanish*/la brújula de la Tripa de Jabu-Jabu
                                                           {
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_COMPASS.ambiguous.0")
                                                            // /*spanish*/una brújula
                                                           }, {
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_COMPASS.obscure.0"),
                                                            // /*spanish*/un zahorí digesto
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_COMPASS.obscure.1")});
                                                            // /*spanish*/un imán digesto

    hintTextTable[RHT_FOREST_TEMPLE_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_COMPASS.clear"),
                                                        // /*spanish*/la brújula del Templo del Bosque
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_COMPASS.ambiguous.0")
                                                         // /*spanish*/una brújula
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_COMPASS.obscure.0"),
                                                         // /*spanish*/un zahorí enselvado
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_COMPASS.obscure.1")});
                                                         // /*spanish*/un imán enselvado

    hintTextTable[RHT_FIRE_TEMPLE_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_COMPASS.clear"),
                                                      // /*spanish*/la brújula del Templo del Fuego
                                                      {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_COMPASS.ambiguous.0")
                                                       // /*spanish*/una brújula
                                                      }, {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_COMPASS.obscure.0"),
                                                       // /*spanish*/un zahorí fundido
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_COMPASS.obscure.1")});
                                                       // /*spanish*/un imán fundido

    hintTextTable[RHT_WATER_TEMPLE_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_COMPASS.clear"),
                                                       // /*spanish*/la brújula del Templo del Agua
                                                       {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_COMPASS.ambiguous.0")
                                                        // /*spanish*/una brújula
                                                       }, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_COMPASS.obscure.0"),
                                                        // /*spanish*/un zahorí mojado
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_COMPASS.obscure.1")});
                                                        // /*spanish*/un imán mojado

    hintTextTable[RHT_SPIRIT_TEMPLE_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_COMPASS.clear"),
                                                        // /*spanish*/la brújula del Templo del Espíritu
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_COMPASS.ambiguous.0")
                                                         // /*spanish*/una brújula
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_COMPASS.obscure.0"),
                                                         // /*spanish*/un zahorí arenoso
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_COMPASS.obscure.1")});
                                                         // /*spanish*/un imán arenoso

    hintTextTable[RHT_SHADOW_TEMPLE_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_COMPASS.clear"),
                                                        // /*spanish*/la brújula del Templo de las Sombras
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_COMPASS.ambiguous.0")
                                                         // /*spanish*/una brújula
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_COMPASS.obscure.0"),
                                                         // /*spanish*/un zahorí siniestra
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_COMPASS.obscure.1")});
                                                         // /*spanish*/un imán siniestra

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_COMPASS.clear"),
                                                             // /*spanish*/la brújula del Fondo del pozo
                                                             {
                                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_COMPASS.ambiguous.0")
                                                              // /*spanish*/una brújula
                                                             }, {
                                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_COMPASS.obscure.0"),
                                                              // /*spanish*/un zahorí mohoso
                                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_COMPASS.obscure.1")});
                                                              // /*spanish*/un imán mohoso

    hintTextTable[RHT_ICE_CAVERN_COMPASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_COMPASS.clear"),
                                                     // /*spanish*/la brújula de la Caverna de hielo
                                                     {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_COMPASS.ambiguous.0")
                                                      // /*spanish*/una brújula
                                                     }, {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_COMPASS.obscure.0"),
                                                      // /*spanish*/un zahorí polar
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_COMPASS.obscure.1")});
                                                      // /*spanish*/un imán polar

    hintTextTable[RHT_FOREST_TEMPLE_BOSS_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BOSS_KEY.clear"),
                                                         // /*spanish*/la gran llave del Templo del Bosque
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BOSS_KEY.ambiguous.0")
                                                          // /*spanish*/una gran llave
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BOSS_KEY.obscure.0"),
                                                          // /*spanish*/la clave enselvada de un jefe
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BOSS_KEY.obscure.1")});
                                                          // /*spanish*/el pase maestro enselvado

    hintTextTable[RHT_FIRE_TEMPLE_BOSS_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOSS_KEY.clear"),
                                                       // /*spanish*/la gran llave del Templo del Fuego
                                                       {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOSS_KEY.ambiguous.0")
                                                        // /*spanish*/una gran llave
                                                       }, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOSS_KEY.obscure.0"),
                                                        // /*spanish*/la clave fundido de un jefe
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOSS_KEY.obscure.1")});
                                                        // /*spanish*/el pase maestro fundido

    hintTextTable[RHT_WATER_TEMPLE_BOSS_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_BOSS_KEY.clear"),
                                                        // /*spanish*/la gran llave del Templo del Agua
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_BOSS_KEY.ambiguous.0")
                                                         // /*spanish*/una gran llave
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_BOSS_KEY.obscure.0"),
                                                         // /*spanish*/la clave mojado de un jefe
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_BOSS_KEY.obscure.1")});
                                                         // /*spanish*/el pase maestro mojado

    hintTextTable[RHT_SPIRIT_TEMPLE_BOSS_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_BOSS_KEY.clear"),
                                                         // /*spanish*/la gran llave del Templo del Espíritu
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_BOSS_KEY.ambiguous.0")
                                                          // /*spanish*/una gran llave
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_BOSS_KEY.obscure.0"),
                                                          // /*spanish*/la clave arenoso de un jefe
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_BOSS_KEY.obscure.1")});
                                                          // /*spanish*/el pase maestro arenoso

    hintTextTable[RHT_SHADOW_TEMPLE_BOSS_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_BOSS_KEY.clear"),
                                                         // /*spanish*/la gran llave del Templo de las Sombras
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_BOSS_KEY.ambiguous.0")
                                                          // /*spanish*/una gran llave
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_BOSS_KEY.obscure.0"),
                                                          // /*spanish*/la clave siniestra de un jefe
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_BOSS_KEY.obscure.1")});
                                                          // /*spanish*/el pase maestro siniestra

    hintTextTable[RHT_GANONS_CASTLE_BOSS_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_BOSS_KEY.clear"),
                                                         // /*spanish*/la gran llave del Castillo de Ganon
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_BOSS_KEY.ambiguous.0")
                                                          // /*spanish*/una gran llave
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_BOSS_KEY.obscure.0"),
                                                          // /*spanish*/la clave final de un jefe
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_BOSS_KEY.obscure.1")});
                                                          // /*spanish*/el pase maestro final

    hintTextTable[RHT_FOREST_TEMPLE_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_SMALL_KEY.clear"),
                                                          // /*spanish*/una llave pequeña del Templo del Bosque
                                                          {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_SMALL_KEY.ambiguous.0")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_SMALL_KEY.obscure.0"),
                                                           // /*spanish*/una clave de una entrada enselvada
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_SMALL_KEY.obscure.1"),
                                                           // /*spanish*/un pase de una mazmorra enselvada
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_SMALL_KEY.obscure.2"),
                                                           // /*spanish*/un destructor de cerraduras enselvada
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_SMALL_KEY.obscure.3")});
                                                           // /*spanish*/una apertura portentosa enselvada

    hintTextTable[RHT_FIRE_TEMPLE_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_SMALL_KEY.clear"),
                                                        // /*spanish*/una llave pequeña del Templo del Fuego
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_SMALL_KEY.ambiguous.0")
                                                         // /*spanish*/una llave pequeña
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_SMALL_KEY.obscure.0"),
                                                         // /*spanish*/una clave de una entrada fundida
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_SMALL_KEY.obscure.1"),
                                                         // /*spanish*/un pase de una mazmorra fundida
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_SMALL_KEY.obscure.2"),
                                                         // /*spanish*/un destructor de cerraduras fundida
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_SMALL_KEY.obscure.3")});
                                                         // /*spanish*/una apertura portentosa fundida

    hintTextTable[RHT_WATER_TEMPLE_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_SMALL_KEY.clear"),
                                                         // /*spanish*/una llave pequeña del Templo del Agua
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_SMALL_KEY.ambiguous.0")
                                                          // /*spanish*/una llave pequeña
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_SMALL_KEY.obscure.0"),
                                                          // /*spanish*/una clave de una entrada mojada
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_SMALL_KEY.obscure.1"),
                                                          // /*spanish*/un pase de una mazmorra mojada
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_SMALL_KEY.obscure.2"),
                                                          // /*spanish*/un destructor de cerraduras mojada
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_SMALL_KEY.obscure.3")});
                                                          // /*spanish*/una apertura portentosa mojada

    hintTextTable[RHT_SPIRIT_TEMPLE_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SMALL_KEY.clear"),
                                                          // /*spanish*/una llave pequeña del Templo del Espíritu
                                                          {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SMALL_KEY.ambiguous.0")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SMALL_KEY.obscure.0"),
                                                           // /*spanish*/una clave de una entrada arenosa
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SMALL_KEY.obscure.1"),
                                                           // /*spanish*/un pase de una mazmorra arenosa
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SMALL_KEY.obscure.2"),
                                                           // /*spanish*/un destructor de cerraduras arenosa
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SMALL_KEY.obscure.3")});
                                                           // /*spanish*/una apertura portentosa arenosa

    hintTextTable[RHT_SHADOW_TEMPLE_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_SMALL_KEY.clear"),
                                                          // /*spanish*/una llave pequeña del Templo de las Sombras
                                                          {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_SMALL_KEY.ambiguous.0")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_SMALL_KEY.obscure.0"),
                                                           // /*spanish*/una clave de una entrada siniestra:a
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_SMALL_KEY.obscure.1"),
                                                           // /*spanish*/un pase de una mazmorra siniestra:a
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_SMALL_KEY.obscure.2"),
                                                           // /*spanish*/un destructor de cerraduras siniestra:a
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_SMALL_KEY.obscure.3")});
                                                           // /*spanish*/una apertura portentosa siniestra:a

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_SMALL_KEY.clear"),
                                                                    // /*spanish*/una llave pequeña del Centro de Instrucción Gerudo
                                                                    {
                                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_SMALL_KEY.ambiguous.0")
                                                                     // /*spanish*/una llave pequeña
                                                                    }, {
                                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_SMALL_KEY.obscure.0"),
                                                                     // /*spanish*/una clave de una entrada laberíntica
                                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_SMALL_KEY.obscure.1"),
                                                                     // /*spanish*/un pase de una mazmorra laberíntica
                                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_SMALL_KEY.obscure.2"),
                                                                     // /*spanish*/un destructor de cerraduras laberíntica
                                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_SMALL_KEY.obscure.3")});
                                                                     // /*spanish*/una apertura portentosa laberíntica

    hintTextTable[RHT_GERUDO_FORTRESS_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_SMALL_KEY.clear"),
                                                            // /*spanish*/una llave pequeña de la Fortaleza Gerudo
                                                            {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_SMALL_KEY.ambiguous.0")
                                                             // /*spanish*/una llave pequeña
                                                            }, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_SMALL_KEY.obscure.0"),
                                                             // /*spanish*/una clave de una entrada encarcelada
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_SMALL_KEY.obscure.1"),
                                                             // /*spanish*/un pase de una mazmorra encarcelada
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_SMALL_KEY.obscure.2"),
                                                             // /*spanish*/un destructor de cerraduras encarcelada
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_SMALL_KEY.obscure.3")});
                                                             // /*spanish*/una apertura portentosa encarcelada

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_SMALL_KEY.clear"),
                                                               // /*spanish*/una llave pequeña del Fondo del pozo
                                                               {
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_SMALL_KEY.ambiguous.0")
                                                                // /*spanish*/una llave pequeña
                                                               }, {
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_SMALL_KEY.obscure.0"),
                                                                // /*spanish*/una clave de una entrada mohosa
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_SMALL_KEY.obscure.1"),
                                                                // /*spanish*/un pase de una mazmorra mohosa
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_SMALL_KEY.obscure.2"),
                                                                // /*spanish*/un destructor de cerraduras mohosa
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_SMALL_KEY.obscure.3")});
                                                                // /*spanish*/una apertura portentosa mohosa

    hintTextTable[RHT_GANONS_CASTLE_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SMALL_KEY.clear"),
                                                          // /*spanish*/una llave pequeña del Castillo de Ganon
                                                          {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SMALL_KEY.ambiguous.0")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SMALL_KEY.obscure.0"),
                                                           // /*spanish*/una clave de una entrada final
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SMALL_KEY.obscure.1"),
                                                           // /*spanish*/un pase de una mazmorra final
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SMALL_KEY.obscure.2"),
                                                           // /*spanish*/un destructor de cerraduras final
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SMALL_KEY.obscure.3")});
                                                           // /*spanish*/una apertura portentosa final

    hintTextTable[RHT_OVERWORLD_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OVERWORLD_KEY.clear"),
                                                // /*spanish*/una llave del mundo exterior
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_OVERWORLD_KEY.ambiguous.0")
                                                 // /*spanish*/una llave
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_OVERWORLD_KEY.obscure.0"),
                                                 // /*spanish*/una llave al mundo
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_OVERWORLD_KEY.obscure.1"),
                                                 // /*spanish*/una llave al reino
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_OVERWORLD_KEY.obscure.2")});
                                                 // /*spanish*/una llave al universo

    hintTextTable[RHT_FOREST_TEMPLE_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_KEY_RING.clear"),
                                                         // /*spanish*/un llavero del Templo del Bosque
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_KEY_RING.ambiguous.0")
                                                          // /*spanish*/un llavero
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_KEY_RING.obscure.0"),
                                                          // /*spanish*/un conjunto silvestre de cerrajero
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_KEY_RING.obscure.1"),
                                                          // /*spanish*/un pase vip de mazmorras silvestre
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_KEY_RING.obscure.2"),
                                                          // /*spanish*/una cadena multiusos silvestre
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_KEY_RING.obscure.3")});
                                                          // /*spanish*/un anillo silvestre contra cerrojos

    hintTextTable[RHT_FIRE_TEMPLE_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_KEY_RING.clear"),
                                                       // /*spanish*/un llavero del Templo del Fuego
                                                       {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_KEY_RING.ambiguous.0")
                                                        // /*spanish*/un llavero
                                                       }, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_KEY_RING.obscure.0"),
                                                        // /*spanish*/un conjunto fundido de cerrajero
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_KEY_RING.obscure.1"),
                                                        // /*spanish*/un pase vip de mazmorras fundido
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_KEY_RING.obscure.2"),
                                                        // /*spanish*/una cadena multiusos fundida
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_KEY_RING.obscure.3")});
                                                        // /*spanish*/un anillo fundido contra cerrojos

    hintTextTable[RHT_WATER_TEMPLE_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_KEY_RING.clear"),
                                                        // /*spanish*/un llavero del Templo del Agua
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_KEY_RING.ambiguous.0")
                                                         // /*spanish*/un llavero
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_KEY_RING.obscure.0"),
                                                         // /*spanish*/un conjunto abisal de cerrajero
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_KEY_RING.obscure.1"),
                                                         // /*spanish*/un pase vip de mazmorras abisal
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_KEY_RING.obscure.2"),
                                                         // /*spanish*/una cadena multiusos abisal
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_KEY_RING.obscure.3")});
                                                         // /*spanish*/un anillo abisal contra cerrojos

    hintTextTable[RHT_SPIRIT_TEMPLE_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_KEY_RING.clear"),
                                                         // /*spanish*/un llavero del Templo del Espíritu
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_KEY_RING.ambiguous.0")
                                                          // /*spanish*/un llavero
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_KEY_RING.obscure.0"),
                                                          // /*spanish*/un conjunto arenoso de cerrajero
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_KEY_RING.obscure.1"),
                                                          // /*spanish*/un pase vip de mazmorras arenoso
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_KEY_RING.obscure.2"),
                                                          // /*spanish*/una cadena multiusos arenosa
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_KEY_RING.obscure.3")});
                                                          // /*spanish*/un anillo arenoso contra cerrojos

    hintTextTable[RHT_SHADOW_TEMPLE_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_KEY_RING.clear"),
                                                         // /*spanish*/un llavero del Templo de las Sombras
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_KEY_RING.ambiguous.0")
                                                          // /*spanish*/un llavero
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_KEY_RING.obscure.0"),
                                                          // /*spanish*/un conjunto tenebroso de cerrajero
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_KEY_RING.obscure.1"),
                                                          // /*spanish*/un pase vip de mazmorras tenebroso
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_KEY_RING.obscure.2"),
                                                          // /*spanish*/una cadena multiusos tenebrosa
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_KEY_RING.obscure.3")});
                                                          // /*spanish*/un anillo tenebroso contra cerrojos

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_KEY_RING.clear"),
                                                                   // /*spanish*/un llavero del Centro de Instrucción Gerudo
                                                                   {
                                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_KEY_RING.ambiguous.0")
                                                                    // /*spanish*/un llavero
                                                                   }, {
                                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_KEY_RING.obscure.0"),
                                                                    // /*spanish*/un conjunto laberíntico de cerrajero
                                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_KEY_RING.obscure.1"),
                                                                    // /*spanish*/un pase vip de mazmorras laberíntico
                                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_KEY_RING.obscure.2"),
                                                                    // /*spanish*/una cadena multiusos laberíntica
                                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_KEY_RING.obscure.3")});
                                                                    // /*spanish*/un anillo laberíntico contra cerrojos

    hintTextTable[RHT_GERUDO_FORTRESS_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_KEY_RING.clear"),
                                                           // /*spanish*/un llavero de la Fortaleza Gerudo
                                                           {
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_KEY_RING.ambiguous.0")
                                                            // /*spanish*/un llavero
                                                           }, {
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_KEY_RING.obscure.0"),
                                                            // /*spanish*/un conjunto enjaulado de cerrajero
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_KEY_RING.obscure.1"),
                                                            // /*spanish*/un pase vip de una mazmorra enjaulado
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_KEY_RING.obscure.2"),
                                                            // /*spanish*/una cadena multiusos enjaulada
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_KEY_RING.obscure.3")});
                                                            // /*spanish*/un anillo enjaulado contra cerrojos

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_KEY_RING.clear"),
                                                              // /*spanish*/un llavero del Fondo del pozo
                                                              {
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_KEY_RING.ambiguous.0")
                                                               // /*spanish*/un llavero
                                                              }, {
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_KEY_RING.obscure.0"),
                                                               // /*spanish*/un conjunto subterráneo de cerrajero
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_KEY_RING.obscure.1"),
                                                               // /*spanish*/un pase vip de una mazmorra subterráneo
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_KEY_RING.obscure.2"),
                                                               // /*spanish*/una cadena multiusos subterránea
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_KEY_RING.obscure.3")});
                                                               // /*spanish*/un anillo subterráneo contra cerrojos

    hintTextTable[RHT_GANONS_CASTLE_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_KEY_RING.clear"),
                                                         // /*spanish*/un llavero del Castillo de Ganon
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_KEY_RING.ambiguous.0")
                                                          // /*spanish*/un llavero
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_KEY_RING.obscure.0"),
                                                          // /*spanish*/un conjunto decisivo de cerrajero
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_KEY_RING.obscure.1"),
                                                          // /*spanish*/un pase vip de una mazmorra decisivo
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_KEY_RING.obscure.2"),
                                                          // /*spanish*/una cadena multiusos decisiva
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_KEY_RING.obscure.3")});
                                                          // /*spanish*/un anillo decisivo multiusos

    hintTextTable[RHT_TREASURE_GAME_SMALL_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_SMALL_KEY.clear"),
                                                          // /*spanish*/una llave pequeña del Cofre del Tesoro
                                                          {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_SMALL_KEY.ambiguous.0")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_SMALL_KEY.obscure.0"),
                                                           // /*spanish*/una clave de un juego de azar
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_SMALL_KEY.obscure.1"),
                                                           // /*spanish*/un pase de un juego de azar
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_SMALL_KEY.obscure.2"),
                                                           // /*spanish*/un destructor de cerraduras del juego de azar
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_SMALL_KEY.obscure.3")});
                                                           // /*spanish*/una apertura portentosa del juego de azar

     hintTextTable[RHT_TREASURE_GAME_KEY_RING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_KEY_RING.clear"),
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_KEY_RING.ambiguous.0")
                                                          // /*spanish*/un llavero
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_KEY_RING.obscure.0"),
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_KEY_RING.obscure.1"),
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_KEY_RING.obscure.2"),
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_KEY_RING.obscure.3")});

    hintTextTable[RHT_KOKIRI_EMERALD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_EMERALD.clear"),
                                                 // /*spanish*/la Esmeralda de los Kokiri
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_EMERALD.ambiguous.0")
                                                  // /*spanish*/una piedra espiritual
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_EMERALD.obscure.0"),
                                                  // /*spanish*/una piedra verde
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_EMERALD.obscure.1")});
                                                  // /*spanish*/un obsequio testamentario

    hintTextTable[RHT_GORON_RUBY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_RUBY.clear"),
                                             // /*spanish*/el Rubí de los Goron
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_RUBY.ambiguous.0")
                                              // /*spanish*/una piedra espiritual
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_RUBY.obscure.0"),
                                              // /*spanish*/una piedra carmín
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_RUBY.obscure.1")});
                                              // /*spanish*/el juramento de hermanos de sangre

    hintTextTable[RHT_ZORA_SAPPHIRE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_SAPPHIRE.clear"),
                                                // /*spanish*/el Zafiro de los Zora
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_SAPPHIRE.ambiguous.0")
                                                 // /*spanish*/una piedra espiritual
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_SAPPHIRE.obscure.0"),
                                                 // /*spanish*/una piedra celeste
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_ZORA_SAPPHIRE.obscure.1")});
                                                 // /*spanish*/un regalo de compromiso

    hintTextTable[RHT_FOREST_MEDALLION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_MEDALLION.clear"),
                                                   // /*spanish*/el Medallón del Bosque
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_MEDALLION.ambiguous.0")
                                                    // /*spanish*/un medallón
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_MEDALLION.obscure.0"),
                                                    // /*spanish*/una moneda esmeralda
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_MEDALLION.obscure.1")});
                                                    // /*spanish*/la amistad de Saria

    hintTextTable[RHT_FIRE_MEDALLION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_MEDALLION.clear"),
                                                 // /*spanish*/el Medallón del Fuego
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_MEDALLION.ambiguous.0")
                                                  // /*spanish*/un medallón
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_MEDALLION.obscure.0"),
                                                  // /*spanish*/una moneda rubí
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_MEDALLION.obscure.1")});
                                                  // /*spanish*/la fraternidad de Darunia

    hintTextTable[RHT_WATER_MEDALLION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_MEDALLION.clear"),
                                                  // /*spanish*/el Medallón del Agua
                                                  {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_MEDALLION.ambiguous.0")
                                                   // /*spanish*/un medallón
                                                  }, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_MEDALLION.obscure.0"),
                                                   // /*spanish*/una moneda zafiro
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_MEDALLION.obscure.1")});
                                                   // /*spanish*/el amor de Ruto

    hintTextTable[RHT_SPIRIT_MEDALLION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_MEDALLION.clear"),
                                                   // /*spanish*/el Medallón del Espíritu
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_MEDALLION.ambiguous.0")
                                                    // /*spanish*/un medallón
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_MEDALLION.obscure.0"),
                                                    // /*spanish*/una moneda ámbar
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_MEDALLION.obscure.1")});
                                                    // /*spanish*/el respeto de Nabooru

    hintTextTable[RHT_SHADOW_MEDALLION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_MEDALLION.clear"),
                                                   // /*spanish*/el Medallón de la Sombra
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_MEDALLION.ambiguous.0")
                                                    // /*spanish*/un medallón
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_MEDALLION.obscure.0"),
                                                    // /*spanish*/una moneda malva
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_MEDALLION.obscure.1")});
                                                    // /*spanish*/la confianza de Impa

    hintTextTable[RHT_LIGHT_MEDALLION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_MEDALLION.clear"),
                                                  // /*spanish*/el Medallón de la Luz
                                                  {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_MEDALLION.ambiguous.0")
                                                   // /*spanish*/un medallón
                                                  }, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_MEDALLION.obscure.0"),
                                                   // /*spanish*/una moneda resplandeciente
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_MEDALLION.obscure.1")});
                                                   // /*spanish*/la fe de Rauru

    hintTextTable[RHT_RECOVERY_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_RECOVERY_HEART.clear"),
                                                 // /*spanish*/un corazón
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_RECOVERY_HEART.ambiguous.0")
                                                  // /*spanish*/algo con forma de corazón
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_RECOVERY_HEART.obscure.0"),
                                                  // /*spanish*/una cura de regalo
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_RECOVERY_HEART.obscure.1"),
                                                  // /*spanish*/una sanación romántica
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_RECOVERY_HEART.obscure.2")});
                                                  // /*spanish*/un corazoncito sanador

    hintTextTable[RHT_GREEN_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GREEN_RUPEE.clear"),
                                              // /*spanish*/una rupia verde
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_GREEN_RUPEE.ambiguous.0")
                                               // /*spanish*/una cantidad de rupias
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_GREEN_RUPEE.obscure.0"),
                                               // /*spanish*/una singular moneda
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_GREEN_RUPEE.obscure.1"),
                                               // /*spanish*/un peso hyliano
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_GREEN_RUPEE.obscure.2")});
                                               // /*spanish*/una gema verde

    hintTextTable[RHT_GREG_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GREG_RUPEE.clear"),
                                             // /*spanish*/Greg
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GREG_RUPEE.ambiguous.0")
                                              // /*spanish*/una rupia verde
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GREG_RUPEE.obscure.0"),
                                              // /*spanish*/Greg
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GREG_RUPEE.obscure.1")});
                                              // /*spanish*/Greg

    hintTextTable[RHT_BLUE_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BLUE_RUPEE.clear"),
                                             // /*spanish*/una rupia azul
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BLUE_RUPEE.ambiguous.0")
                                              // /*spanish*/una cantidad de rupias
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BLUE_RUPEE.obscure.0"),
                                              // /*spanish*/una moneda usual
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BLUE_RUPEE.obscure.1")});
                                              // /*spanish*/una gema azul

    hintTextTable[RHT_RED_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_RED_RUPEE.clear"),
                                            // /*spanish*/una rupia roja
                                            {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_RED_RUPEE.ambiguous.0")
                                             // /*spanish*/una cantidad de rupias
                                            }, {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_RED_RUPEE.obscure.0"),
                                             // /*spanish*/una buena moneda
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_RED_RUPEE.obscure.1")});
                                             // /*spanish*/una gema roja

    hintTextTable[RHT_PURPLE_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PURPLE_RUPEE.clear"),
                                               // /*spanish*/una rupia morada
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_PURPLE_RUPEE.ambiguous.0")
                                                // /*spanish*/una cantidad de rupias
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_PURPLE_RUPEE.obscure.0"),
                                                // /*spanish*/plata de calidad
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_PURPLE_RUPEE.obscure.1"),
                                                // /*spanish*/una gema morada
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_PURPLE_RUPEE.obscure.2")});
                                                // /*spanish*/una buena riqueza

    hintTextTable[RHT_HUGE_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HUGE_RUPEE.clear"),
                                             // /*spanish*/una rupia gigante
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_HUGE_RUPEE.ambiguous.0")
                                              // /*spanish*/una cantidad de rupias
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_HUGE_RUPEE.obscure.0"),
                                              // /*spanish*/el premio gordo
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_HUGE_RUPEE.obscure.1"),
                                              // /*spanish*/una gema amarilla
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_HUGE_RUPEE.obscure.2"),
                                              // /*spanish*/una gema descomunal
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_HUGE_RUPEE.obscure.3")});
                                              // /*spanish*/dinero a caudales

    hintTextTable[RHT_PIECE_OF_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PIECE_OF_HEART.clear"),
                                                 // /*spanish*/una pieza de corazón
                                                 {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_PIECE_OF_HEART.ambiguous.0")
                                                  // /*spanish*/algo con forma de corazón
                                                 }, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_PIECE_OF_HEART.obscure.0"),
                                                  // /*spanish*/un cuarto de amor
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_PIECE_OF_HEART.obscure.1")});
                                                  // /*spanish*/un corazón roto

    hintTextTable[RHT_HEART_CONTAINER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HEART_CONTAINER.clear"),
                                                  // /*spanish*/un contenedor de corazón
                                                  {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_HEART_CONTAINER.ambiguous.0")
                                                   // /*spanish*/algo con forma de corazón
                                                  }, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_HEART_CONTAINER.obscure.0"),
                                                   // /*spanish*/amor por doquier
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_HEART_CONTAINER.obscure.1"),
                                                   // /*spanish*/un contenedor de afección
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_HEART_CONTAINER.obscure.2")});
                                                   // /*spanish*/los órganos de un jefe

    hintTextTable[RHT_ICE_TRAP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_TRAP.clear"),
                                           // /*spanish*/una trampa de hielo
                                           {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_TRAP.ambiguous.0"),
                                            // /*spanish*/el poder de una Gran Hada
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_TRAP.ambiguous.1"),
                                            // /*spanish*/una flecha mágica
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_TRAP.ambiguous.2"),
                                            // /*spanish*/un medallón
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_TRAP.ambiguous.3"),
                                            // /*spanish*/una piedra espiritual
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_TRAP.ambiguous.4")
                                            // /*spanish*/algo que pueda paralizar
                                           }, {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_TRAP.obscure.0"),
                                            // /*spanish*/un regalo de Ganon
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_TRAP.obscure.1"),
                                            // /*spanish*/un escalofriante hallazgo
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_TRAP.obscure.2")});
                                            // /*spanish*/una gélida diversión

    hintTextTable[RHT_BOMBS_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_5.clear"),
                                          // /*spanish*/unas (5) bombas
                                          {
                                          CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_5.ambiguous.0")
                                           // /*spanish*/un montón de explosivos
                                          }, {
                                          CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_5.obscure.0"),
                                           // /*spanish*/un par de explosivos
                                          CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_5.obscure.1")});
                                           // /*spanish*/un par de estallidos

    hintTextTable[RHT_BOMBS_10] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_10.clear"),
                                           // /*spanish*/unas (10) bombas
                                           {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_10.ambiguous.0")
                                            // /*spanish*/un montón de explosivos
                                           }, {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_10.obscure.0"),
                                            // /*spanish*/unos cuantos explosivos
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_10.obscure.1")});
                                            // /*spanish*/unos cuantos estallidos

    hintTextTable[RHT_BOMBS_20] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_20.clear"),
                                           // /*spanish*/unas (20) bombas
                                           {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_20.ambiguous.0")
                                            // /*spanish*/un montón de explosivos
                                           }, {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_20.obscure.0"),
                                            // /*spanish*/un puñado de explosivos
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBS_20.obscure.1")});
                                            // /*spanish*/bastantes estallidos

    hintTextTable[RHT_BOMBCHUS_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_5.clear"),
                                            // /*spanish*/unos (5) bombchus
                                            {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_5.ambiguous.0"),
                                             // /*spanish*/un obsequio de la Casa Skulltula
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_5.ambiguous.1")
                                             // /*spanish*/un montón de explosivos
                                            }, {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_5.obscure.0"),
                                             // /*spanish*/un par de bombas roedoras
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_5.obscure.1"),
                                             // /*spanish*/un par de explosivos ratoncitos
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_5.obscure.2"),
                                             // /*spanish*/un par de trepaparedes
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_5.obscure.3")});
                                             // /*spanish*/un par de ratas propulsadas

    hintTextTable[RHT_BOMBCHUS_10] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_10.clear"),
                                             // /*spanish*/unos (10) bombchus
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_10.ambiguous.0"),
                                              // /*spanish*/un obsequio de la Casa Skulltula
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_10.ambiguous.1")
                                              // /*spanish*/un montón de explosivos
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_10.obscure.0"),
                                              // /*spanish*/unas cuantas bombas roedoras
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_10.obscure.1"),
                                              // /*spanish*/unos cuantos explosivos ratoncitos
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_10.obscure.2"),
                                              // /*spanish*/unos cuantos trepaparedes
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_10.obscure.3")});
                                              // /*spanish*/unas cuantas ratas propulsadas

    hintTextTable[RHT_BOMBCHUS_20] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_20.clear"),
                                             // /*spanish*/unos (20) bombchus
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_20.ambiguous.0"),
                                              // /*spanish*/un obsequio de la Casa Skulltula
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_20.ambiguous.1")
                                              // /*spanish*/un montón de explosivos
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_20.obscure.0"),
                                              // /*spanish*/bastantes bombas roedoras
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_20.obscure.1"),
                                              // /*spanish*/bastantes explosivos ratoncitos
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_20.obscure.2"),
                                              // /*spanish*/bastantes trepaparedes
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHUS_20.obscure.3")});
                                              // /*spanish*/bastantes ratas propulsadas

    hintTextTable[RHT_ARROWS_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_5.clear"),
                                           // /*spanish*/unas (5) flechas
                                           {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_5.ambiguous.0")
                                            // /*spanish*/un proyectil
                                           }, {
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_5.obscure.0"),
                                            // /*spanish*/un par de peligrosos dardos
                                           CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_5.obscure.1")});
                                            // /*spanish*/un par de puntas afiladas

    hintTextTable[RHT_ARROWS_10] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_10.clear"),
                                            // /*spanish*/unas (10) flechas
                                            {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_10.ambiguous.0")
                                             // /*spanish*/un proyectil
                                            }, {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_10.obscure.0"),
                                             // /*spanish*/unos cuantos peligrosos dardos
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_10.obscure.1")});
                                             // /*spanish*/unas cuantas puntas afiladas

    hintTextTable[RHT_ARROWS_30] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_30.clear"),
                                            // /*spanish*/unas (30) flechas
                                            {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_30.ambiguous.0")
                                             // /*spanish*/un proyectil
                                            }, {
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_30.obscure.0"),
                                             // /*spanish*/bastantes peligrosos dardos
                                            CustomMessages::CreateMessage("randomizer.hints.RHT_ARROWS_30.obscure.1")});
                                             // /*spanish*/bastantes puntas afiladas

    hintTextTable[RHT_DEKU_NUTS_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_5.clear"),
                                              // /*spanish*/unas (5) nueces deku
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_5.ambiguous.0"),
                                               // /*spanish*/un montón de municiones Deku
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_5.ambiguous.1")
                                               // /*spanish*/algo que pueda paralizar
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_5.obscure.0"),
                                               // /*spanish*/un par de nueces
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_5.obscure.1"),
                                               // /*spanish*/un par de semillas aturdidoras
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_5.obscure.2")});
                                               // /*spanish*/un par de escupitajos deku

    hintTextTable[RHT_DEKU_NUTS_10] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_10.clear"),
                                               // /*spanish*/unas (10) nueces deku
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_10.ambiguous.0"),
                                                // /*spanish*/un montón de municiones Deku
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_10.ambiguous.1")
                                                // /*spanish*/algo que pueda paralizar
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_10.obscure.0"),
                                                // /*spanish*/un puñado de nueces
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_10.obscure.1"),
                                                // /*spanish*/unas cuantas semillas aturdidoras
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_NUTS_10.obscure.2")});
                                                // /*spanish*/unos cuantos escupitajos deku

    hintTextTable[RHT_DEKU_SEEDS_30] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_SEEDS_30.clear"),
                                                // /*spanish*/unas (30) semillas deku
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_SEEDS_30.ambiguous.0"),
                                                 // /*spanish*/un proyectil
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_SEEDS_30.ambiguous.1")
                                                 // /*spanish*/un montón de municiones Deku
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_SEEDS_30.obscure.0"),
                                                 // /*spanish*/un par de munición infantil
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_SEEDS_30.obscure.1")});
                                                 // /*spanish*/un puñado de semillas

    hintTextTable[RHT_DEKU_STICK_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_STICK_1.clear"),
                                               // /*spanish*/un palo deku
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_STICK_1.ambiguous.0")
                                                // /*spanish*/un montón de municiones Deku
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_STICK_1.obscure.0")});
                                                // /*spanish*/un pequeño báculo

    hintTextTable[RHT_TREASURE_GAME_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_HEART.clear"),
                                                      // /*spanish*/el amor de la victoria
                                                      {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_HEART.ambiguous.0")
                                                       // /*spanish*/algo con forma de corazón
                                                      }, {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_HEART.obscure.0")});
                                                       // /*spanish*/el amor victorioso

    hintTextTable[RHT_TREASURE_GAME_GREEN_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_GREEN_RUPEE.clear"),
                                                            // /*spanish*/una rupia verde
                                                            {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_GREEN_RUPEE.ambiguous.0")
                                                             // /*spanish*/una cantidad de rupias
                                                            }, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_TREASURE_GAME_GREEN_RUPEE.obscure.0")});
                                                             // /*spanish*/el peso de la derrota

    hintTextTable[RHT_TRIFORCE_PIECE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TRIFORCE_PIECE.clear"),
                                                 // /*spanish*/un fragmento de la Trifuerza
                                                 {}, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_TRIFORCE_PIECE.obscure.0"),
                                                  // /*spanish*/un trígono del triunfo
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_TRIFORCE_PIECE.obscure.1"),
                                                  // /*spanish*/un porción de queso
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_TRIFORCE_PIECE.obscure.2")});
                                                  // /*spanish*/un fragmento dorado

    hintTextTable[RHT_ROCS_FEATHER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ROCS_FEATHER.clear"),
                                                 {}, {
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ROCS_FEATHER.obscure.0"),
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ROCS_FEATHER.obscure.1"),
                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_ROCS_FEATHER.obscure.2")});

    hintTextTable[RHT_BEAN_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEAN_SOUL.clear"));

    hintTextTable[RHT_GOHMA_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GOHMA_SOUL.clear"),
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GOHMA_SOUL.ambiguous.0")
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GOHMA_SOUL.obscure.0"),
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GOHMA_SOUL.obscure.1")});

    hintTextTable[RHT_KING_DODONGO_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KING_DODONGO_SOUL.clear"),
                                                    {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_KING_DODONGO_SOUL.ambiguous.0")
                                                    }, {
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_KING_DODONGO_SOUL.obscure.0"),
                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_KING_DODONGO_SOUL.obscure.1")});

    hintTextTable[RHT_BARINADE_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BARINADE_SOUL.clear"),
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_BARINADE_SOUL.ambiguous.0")
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_BARINADE_SOUL.obscure.0"),
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_BARINADE_SOUL.obscure.1")});

    hintTextTable[RHT_PHANTOM_GANON_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PHANTOM_GANON_SOUL.clear"),
                                                     {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_PHANTOM_GANON_SOUL.ambiguous.0")
                                                     }, {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_PHANTOM_GANON_SOUL.obscure.0"),

                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_PHANTOM_GANON_SOUL.obscure.1")});

    hintTextTable[RHT_VOLVAGIA_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_VOLVAGIA_SOUL.clear"),
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_VOLVAGIA_SOUL.ambiguous.0")
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_VOLVAGIA_SOUL.obscure.0"),
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_VOLVAGIA_SOUL.obscure.1")});

    hintTextTable[RHT_MORPHA_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MORPHA_SOUL.clear"),
                                              {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_MORPHA_SOUL.ambiguous.0")
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_MORPHA_SOUL.obscure.0"),
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_MORPHA_SOUL.obscure.1")});

    hintTextTable[RHT_BONGO_BONGO_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BONGO_BONGO_SOUL.clear"),
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BONGO_BONGO_SOUL.ambiguous.0")
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BONGO_BONGO_SOUL.obscure.0"),
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_BONGO_BONGO_SOUL.obscure.1")});

    hintTextTable[RHT_TWINROVA_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TWINROVA_SOUL.clear"),
                                                {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_TWINROVA_SOUL.ambiguous.0")
                                                }, {
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_TWINROVA_SOUL.obscure.0"),
                                                CustomMessages::CreateMessage("randomizer.hints.RHT_TWINROVA_SOUL.obscure.1")});

    hintTextTable[RHT_GANON_SOUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_SOUL.clear"),
                                             {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_SOUL.ambiguous.0")
                                             }, {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_SOUL.obscure.0"),
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_SOUL.obscure.1")});

    hintTextTable[RHT_OCARINA_A_BUTTON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_A_BUTTON.clear"),
                                                   // /*spanish*/un botón A de Ocarina
                                                   {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_A_BUTTON.ambiguous.0")
                                                    // /*spanish*/algo melódico
                                                   }, {
                                                   CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_A_BUTTON.obscure.0")});
                                                    // /*spanish*/una letra musical

    hintTextTable[RHT_OCARINA_C_UP_BUTTON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_UP_BUTTON.clear"),
                                                      // /*spanish*/un botón C superior de Ocarina
                                                      {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_UP_BUTTON.ambiguous.0")
                                                       // /*spanish*/algo melódico
                                                      }, {
                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_UP_BUTTON.obscure.0")});
                                                       // /*spanish*/un tono alto

    hintTextTable[RHT_OCARINA_C_DOWN_BUTTON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_DOWN_BUTTON.clear"),
                                                        // /*spanish*/un botón C inferior de Ocarina
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_DOWN_BUTTON.ambiguous.0")
                                                         // /*spanish*/algo melódico
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_DOWN_BUTTON.obscure.0")});
                                                         // /*spanish*/un tono bajo

    hintTextTable[RHT_OCARINA_C_LEFT_BUTTON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_LEFT_BUTTON.clear"),
                                                        // /*spanish*/un botón C izquierdo de Ocarina
                                                        {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_LEFT_BUTTON.ambiguous.0")
                                                         // /*spanish*/algo melódico
                                                        }, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_LEFT_BUTTON.obscure.0")});
                                                         // /*spanish*/un tono hacia la izquierda

    hintTextTable[RHT_OCARINA_C_RIGHT_BUTTON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_RIGHT_BUTTON.clear"),
                                                         // /*spanish*/un botón C derecho de Ocarina
                                                         {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_RIGHT_BUTTON.ambiguous.0")
                                                          // /*spanish*/algo melódico
                                                         }, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_OCARINA_C_RIGHT_BUTTON.obscure.0")});
                                                          // /*spanish*/un tono hacia la derecha

    hintTextTable[RHT_MASK_KEATON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_KEATON.clear"), {CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_KEATON.ambiguous.0")});
    hintTextTable[RHT_MASK_SKULL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_SKULL.clear"), {CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_SKULL.ambiguous.0")});
    hintTextTable[RHT_MASK_SPOOKY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_SPOOKY.clear"), {CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_SPOOKY.ambiguous.0")});
    hintTextTable[RHT_MASK_BUNNY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_BUNNY.clear"), {CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_BUNNY.ambiguous.0")});
    hintTextTable[RHT_MASK_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_GORON.clear"), {CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_GORON.ambiguous.0")});
    hintTextTable[RHT_MASK_ZORA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_ZORA.clear"), {CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_ZORA.ambiguous.0")});
    hintTextTable[RHT_MASK_GERUDO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_GERUDO.clear"), {CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_GERUDO.ambiguous.0")});
    hintTextTable[RHT_MASK_TRUTH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_TRUTH.clear"), {CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_TRUTH.ambiguous.0")});

    hintTextTable[RHT_SCARECROWS_SONG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SCARECROWS_SONG.clear"),
                                                  {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_SCARECROWS_SONG.ambiguous.0")
                                                  }, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_SCARECROWS_SONG.obscure.0")});

    hintTextTable[RHT_FISHING_POLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FISHING_POLE.clear"),
                                               // /*spanish*/caña de pescar
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_FISHING_POLE.ambiguous.0")
                                                // /*spanish*/(caña de pescar)
                                               }, {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_FISHING_POLE.obscure.0")});
                                                // /*spanish*/(caña de pescar)

    hintTextTable[RHT_BOMBCHU_BAG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHU_BAG.clear"),
                                              {
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHU_BAG.ambiguous.0"),
                                                  // /*spanish*/un montón de explosivos
                                             CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHU_BAG.ambiguous.1")
                                                  // /*spanish*/algo que pueda quitar rocas
                                              }, {
                                              CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHU_BAG.obscure.0")});

    hintTextTable[RHT_SILVER_RUPEE_DODONGOS_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SILVER_RUPEE_DODONGOS_CAVERN.clear"));
    hintTextTable[RHT_SILVER_RUPEE_SHADOW_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SILVER_RUPEE_SHADOW_TEMPLE.clear"));
    hintTextTable[RHT_SILVER_RUPEE_SPIRIT_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SILVER_RUPEE_SPIRIT_TEMPLE.clear"));
    hintTextTable[RHT_SILVER_RUPEE_BOTW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SILVER_RUPEE_BOTW.clear"));
    hintTextTable[RHT_SILVER_RUPEE_ICE_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SILVER_RUPEE_ICE_CAVERN.clear"));
    hintTextTable[RHT_SILVER_RUPEE_GTG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SILVER_RUPEE_GTG.clear"));
    hintTextTable[RHT_SILVER_RUPEE_GANONS_CASTLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SILVER_RUPEE_GANONS_CASTLE.clear"));

    hintTextTable[RHT_SKELETON_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SKELETON_KEY.clear"),
                                               // /*spanish*/una Llave Maestra
                                               {
                                               CustomMessages::CreateMessage("randomizer.hints.RHT_SKELETON_KEY.ambiguous.0")
                                                // /*spanish*/una llave
                                               },
                                               { CustomMessages::CreateMessage("randomizer.hints.RHT_SKELETON_KEY.obscure.0") });
                                                // /*spanish*/un desbloqueador maestro

    //RANDOTODO if these are ever used for anything other than name, they want abscure and ambiguous hints
    hintTextTable[RHT_CLIMB] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CLIMB.clear"));
    hintTextTable[RHT_CRAWL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRAWL.clear"));
    hintTextTable[RHT_OPEN_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OPEN_CHEST.clear"));
    hintTextTable[RHT_SPEAK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPEAK.clear"));

    hintTextTable[RHT_QUIVER_INF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_QUIVER_INF.clear"));

    hintTextTable[RHT_BOMB_BAG_INF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOMB_BAG_INF.clear"));

    hintTextTable[RHT_BULLET_BAG_INF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BULLET_BAG_INF.clear"));

    hintTextTable[RHT_STICK_UPGRADE_INF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_STICK_UPGRADE_INF.clear"));

    hintTextTable[RHT_NUT_UPGRADE_INF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_NUT_UPGRADE_INF.clear"));

    hintTextTable[RHT_MAGIC_INF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MAGIC_INF.clear"));

    hintTextTable[RHT_BOMBCHU_INF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOMBCHU_INF.clear"));

    hintTextTable[RHT_WALLET_INF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WALLET_INF.clear"));

    hintTextTable[RHT_EPONA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_EPONA.clear"),
                                    // /*spanish*/a Epona
                                    {
                                    CustomMessages::CreateMessage("randomizer.hints.RHT_EPONA.ambiguous.0"),
                                         // /*spanish*/un obsequio de Malon
                                    CustomMessages::CreateMessage("randomizer.hints.RHT_EPONA.ambiguous.1"),
                                         // /*spanish*/una melodía de ranas
                                    CustomMessages::CreateMessage("randomizer.hints.RHT_EPONA.ambiguous.2")
                                         // /*spanish*/algo para cruzar un puente roto
                                    }, {
                                    CustomMessages::CreateMessage("randomizer.hints.RHT_EPONA.obscure.0"),
                                         // /*spanish*/una yegua
                                    CustomMessages::CreateMessage("randomizer.hints.RHT_EPONA.obscure.1")});
                                         // /*spanish*/una amiga cuadrúpeda

    //What is this used for?
    hintTextTable[RHT_HINT_MYSTERIOUS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HINT_MYSTERIOUS.clear"));
                                                  // /*spanish*/algo misterioso

    hintTextTable[RHT_MYSTERIOUS_ITEM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MYSTERIOUS_ITEM.clear"));
                                                  // /*spanish*/algo misterioso

     hintTextTable[RHT_MYSTERIOUS_ITEM_CAPITAL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MYSTERIOUS_ITEM_CAPITAL.clear"));
                                                  // /*spanish*/Algo Misterioso

    // clang-format on
}
} // namespace Rando
