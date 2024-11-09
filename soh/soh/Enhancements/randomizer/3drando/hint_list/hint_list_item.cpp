#include "../../static_data.h"
#include "../hints.hpp"
#include "../../../custom-message/CustomMessageManager.h"

namespace Rando {

void StaticData::HintTable_Init_Item() {
    
    hintTextTable[RHT_KOKIRI_SWORD] = HintText(CustomMessage("the Kokiri Sword", /*german*/"das Kokiri-Schwert", /*french*/"l'Épée Kokiri"),
                                               // /*spanish*/la Espada Kokiri
                                               {
                                               CustomMessage("a sword", /*german*/"ein Schwert", /*french*/"une épée")
                                                // /*spanish*/una espada
                                               }, {
                                               CustomMessage("a butter knife", /*german*/"ein Buttermesser", /*french*/"un couteau à beurre"),
                                                // /*spanish*/un ágil puñal
                                               CustomMessage("a starter slasher", /*german*/"ein Anfängerschwert", /*french*/"une arme de débutant"),
                                                // /*spanish*/una hoja de principiantes
                                               CustomMessage("a switchblade", /*german*/"ein Stellmesser", /*french*/"un canif")});
                                                // /*spanish*/una navaja

    hintTextTable[RHT_MASTER_SWORD] = HintText(CustomMessage("the Master Sword", /*german*/"das Master-Schwert", /*french*/"l'Épée de Légende"),
                                               // /*spanish*/la Espada Maestra
                                               {
                                               CustomMessage("a sword", /*german*/"ein Schwert", /*french*/"une épée")
                                                // /*spanish*/una espada
                                               }, {
                                               CustomMessage("evil's bane", /*german*/"der böse Fluch", /*french*/"le fléau du mal"),
                                                // /*spanish*/la destructora del mal
                                               CustomMessage("a seven year limbo", /*german*/"eine siebenjährige Erwartung", /*french*/"une stase de sept ans")});
                                                // /*spanish*/unos siete años de espera

    hintTextTable[RHT_GIANTS_KNIFE] = HintText(CustomMessage("the Giant's Knife", /*german*/"das Langschwert", /*french*/"la Lame des Géants"),
                                               // /*spanish*/la daga gigante
                                               {
                                               CustomMessage("a sword", /*german*/"ein Schwert", /*french*/"une épée")
                                                // /*spanish*/una espada
                                               }, {
                                               CustomMessage("a fragile blade", /*german*/"ein fragiles Schwert", /*french*/"une lame fragile"),
                                                // /*spanish*/una frágil hoja
                                               CustomMessage("a breakable cleaver", /*german*/"ein brüchiger Spalter", /*french*/"un espadon de verre")});
                                                // /*spanish*/un rompible acero

    hintTextTable[RHT_BIGGORON_SWORD] = HintText(CustomMessage("the Biggoron Sword", /*german*/"das Biggoron-Schwert", /*french*/"l'Épée de Biggoron"),
                                                 // /*spanish*/la Espada de Biggoron
                                                 {
                                                 CustomMessage("a sword", /*german*/"ein Schwert", /*french*/"une épée")
                                                  // /*spanish*/una espada
                                                 }, {
                                                 CustomMessage("the biggest blade", /*german*/"das größte Schwert", /*french*/"une lame gigantesque"),
                                                  // /*spanish*/el mayor mandoble
                                                 CustomMessage("a colossal cleaver", /*german*/"ein kolossaler Spalter", /*french*/"un espadon colossal")});
                                                  // /*spanish*/un estoque colosal

    hintTextTable[RHT_DEKU_SHIELD] = HintText(CustomMessage("a Deku Shield", /*german*/"ein Deku-Schild", /*french*/"un Bouclier Mojo"),
                                              // /*spanish*/un escudo deku
                                              {
                                              CustomMessage("a shield", /*german*/"ein Schild", /*french*/"un bouclier")
                                               // /*spanish*/un escudo
                                              }, {
                                              CustomMessage("a wooden ward", /*german*/"eine hölzerne Abwehr", /*french*/"un écu d'écorce"),
                                               // /*spanish*/una protección del bosque
                                              CustomMessage("a burnable barrier", /*german*/"eine brennbare Barriere", /*french*/"une protection inflammable")});
                                               // /*spanish*/una barrera quemable

    hintTextTable[RHT_HYLIAN_SHIELD] = HintText(CustomMessage("a Hylian Shield", /*german*/"ein Hylia-Schild", /*french*/"un Bouclier Hylien"),
                                                // /*spanish*/un escudo hyliano
                                                {
                                                CustomMessage("a shield", /*german*/"ein Schild", /*french*/"un bouclier")
                                                 // /*spanish*/un escudo
                                                }, {
                                                CustomMessage("a steel safeguard", /*german*/"ein eiserner Schutz", /*french*/"une carapace d'acier"),
                                                 // /*spanish*/una protección de acero
                                                CustomMessage("Like Like's metal meal", /*german*/"Raubschleims Metallmahlzeit", /*french*/"un amuse-gueule de Pudding")});
                                                 // /*spanish*/un alimento de Like Like

    hintTextTable[RHT_MIRROR_SHIELD] = HintText(CustomMessage("the Mirror Shield", /*german*/"das Spiegelschild", /*french*/"le Bouclier Miroir"),
                                                // /*spanish*/el escudo espejo
                                                {
                                                CustomMessage("a shield", /*german*/"ein Schild", /*french*/"un bouclier")
                                                 // /*spanish*/un escudo
                                                }, {
                                                CustomMessage("a reflective rampart", /*german*/"ein reflektierender Schutzwall", /*french*/"un capteur de lumière"),
                                                 // /*spanish*/una muralla reflectora
                                                CustomMessage("Medusa's weakness", /*german*/"Medusas Schwäche", /*french*/"la faiblesse de Méduse"),
                                                 // /*spanish*/la debilidad de Medusa
                                                CustomMessage("a silvered surface", /*german*/"eine silberne Oberfläche", /*french*/"une surface argentée")});
                                                 // /*spanish*/una superficie plateada

    hintTextTable[RHT_GORON_TUNIC] = HintText(CustomMessage("a Goron Tunic", /*german*/"eine Goronen-Tunika", /*french*/"une Tunique Goron"),
                                              // /*spanish*/un sayo goron
                                              {
                                              CustomMessage("a tunic", /*german*/"eine Tunika", /*french*/"une tunique")
                                               // /*spanish*/un sayo
                                              }, {
                                              CustomMessage("ruby robes", /*german*/"Rubinroben", /*french*/"un pigment rouge"),
                                               // /*spanish*/una vestimenta rubí
                                              CustomMessage("fireproof fabric", /*german*/"feuerfestes Gewebe", /*french*/"un trésor anti-flamme"),
                                               // /*spanish*/una ignífuga prenda
                                              CustomMessage("cooking clothes", /*german*/"Kochschürze", /*french*/"une tenue de cuisine")});
                                               // /*spanish*/unos abrasantes ropajes

    hintTextTable[RHT_ZORA_TUNIC] = HintText(CustomMessage("a Zora Tunic", /*german*/"eine Zora-Tunika", /*french*/"une Tunique Zora"),
                                             // /*spanish*/un sayo zora
                                             {
                                             CustomMessage("a tunic", /*german*/"eine Tunika", /*french*/"une tunique"),
                                              // /*spanish*/un sayo
                                             CustomMessage("something expensive", /*german*/"etwas Teures", /*french*/"une chose dispendieuse")
                                              // /*spanish*/algo caro
                                             }, {
                                             CustomMessage("a sapphire suit", /*german*/"ein Saphiranzug", /*french*/"un pigment bleuté"),
                                              // /*spanish*/una vestidura zafiro
                                             CustomMessage("scuba gear", /*german*/"eine Taucherausrüstung", /*french*/"un habit de plongée"),
                                              // /*spanish*/un traje impermeable
                                             CustomMessage("a swimsuit", /*german*/"ein Badeanzug", /*french*/"un costume de baignade")});
                                              // /*spanish*/unos ropajes sumergibles

    hintTextTable[RHT_IRON_BOOTS] = HintText(CustomMessage("the Iron Boots", /*german*/"die Eisenstiefel", /*french*/"une paire de Bottes de plomb"),
                                             // /*spanish*/las botas de hierro
                                             {
                                             CustomMessage("some boots", /*german*/"ein paar Stiefel", /*french*/"une paire de bottes"),
                                              // /*spanish*/un par de botas
                                             CustomMessage("a feature of the Water Temple", /*german*/"ein Merkmal des Wassertempels", /*french*/"une particularité du Temple de l'Eau"),
                                              // /*spanish*/algo particular del Templo del Agua
                                             CustomMessage("something heavy", /*german*/"etwas Schweres", /*french*/"une chose pesante")
                                              // /*spanish*/algo de lo más pesado
                                             }, {
                                             CustomMessage("sink shoes", /*german*/"Sinkschuhe", /*french*/"un boulet de fer"),
                                              // /*spanish*/un calzado de las profundidades
                                             CustomMessage("clank cleats", /*german*/"klirrende Knacken", /*french*/"une paire de talons bruyants")});
                                              // /*spanish*/unas suelas férreas

    hintTextTable[RHT_HOVER_BOOTS] = HintText(CustomMessage("the Hover Boots", /*german*/"die Gleitstiefel", /*french*/"une paire de Bottes des airs"),
                                              // /*spanish*/las botas voladoras
                                              {
                                              CustomMessage("some boots", /*german*/"ein paar Stiefel", /*french*/"une paire de bottes")
                                               // /*spanish*/un par de botas
                                              }, {
                                              CustomMessage("butter boots", /*german*/"Butterstiefel", /*french*/"une paire de patins de beurre"),
                                               // /*spanish*/unas suelas resvaladizas
                                              CustomMessage("sacred slippers", /*german*/"heilige Pantoffeln", /*french*/"une paire de pantoufles sacrées"),
                                               // /*spanish*/unos escurridizos botines
                                              CustomMessage("spacewalkers", /*german*/"Weltraumstiefel", /*french*/"une paire de bottes spatiales")});
                                               // /*spanish*/un calzado antigravitatorio

    hintTextTable[RHT_ZELDAS_LETTER] = HintText(CustomMessage("Zelda's Letter", /*german*/"Zeldas Brief", /*french*/"la Lettre de Zelda"),
                                                // /*spanish*/la carta de Zelda
                                                {}, {
                                                CustomMessage("an autograph", /*german*/"ein Autograph", /*french*/"un autographe"),
                                                 // /*spanish*/un autógrafo
                                                CustomMessage("royal stationery", /*german*/"royales Briefpapier", /*french*/"du papier royal"),
                                                 // /*spanish*/un escrito real
                                                CustomMessage("royal snail mail", /*german*/"ein royaler Umschlag", /*french*/"une enveloppe royale")});
                                                 // /*spanish*/correo de la realeza

    hintTextTable[RHT_WEIRD_EGG] = HintText(CustomMessage("the Weird Egg", /*german*/"ein seltsames Ei", /*french*/"l'Oeuf Curieux"),
                                            // /*spanish*/el huevo extraño
                                            {
                                            CustomMessage("an egg", /*german*/"ein Ei", /*french*/"un oeuf")
                                             // /*spanish*/un huevo
                                            }, {
                                            CustomMessage("a chicken dilemma", /*german*/"ein Hühnerdilemma", /*french*/"un drôle d'ovale")});
                                             // /*spanish*/el dilema de la gallina

    hintTextTable[RHT_BOOMERANG] = HintText(CustomMessage("the Boomerang", /*german*/"ein Bumerang", /*french*/"le Boomerang"),
                                            // /*spanish*/el bumerán
                                            {
                                            CustomMessage("something that can grab things", /*german*/"etwas, das Dinge greifen kann", /*french*/"une chose qui peut attraper"),
                                             // /*spanish*/algo que pueda agarrar cosas
                                            CustomMessage("something that can stun", /*german*/"etwas, das paralysieren kann", /*french*/"une chose qui peut paralyser")
                                             // /*spanish*/algo que pueda paralizar
                                            }, {
                                            CustomMessage("a banana", /*german*/"eine Banane", /*french*/"une banane"),
                                             // /*spanish*/un plátano
                                            CustomMessage("a stun stick", /*german*/"ein paralysierender Stab", /*french*/"un bâton étourdissant"),
                                             // /*spanish*/un palo aturdidor
                                            CustomMessage("a yellow angle", /*german*/"ein gelber Winkel", /*french*/"un angle jaune")});
                                             // /*spanish*/un ángulo amarillo

    hintTextTable[RHT_LENS_OF_TRUTH] = HintText(CustomMessage("the Lens of Truth", /*german*/"das Auge der Wahrheit", /*french*/"le Monocle de Vérité"),
                                                // /*spanish*/la Lupa de la Verdad
                                                {
                                                CustomMessage("a secret-finding tool", /*german*/"ein geheimnisfindendes Werkzeug", /*french*/"un cherche-secrets")
                                                 // /*spanish*/un instrumento para hallar objetos
                                                }, {
                                                CustomMessage("a lie detector", /*german*/"ein Lügendetektor", /*french*/"un détecteur de mensonges"),
                                                 // /*spanish*/el detector de ilusiones
                                                CustomMessage("a ghost tracker", /*german*/"ein Geisterfinder", /*french*/"un trouve-fantôme"),
                                                 // /*spanish*/el rastreador paranormal
                                                CustomMessage("true sight", /*german*/"wahre Sicht", /*french*/"le troisième il"),
                                                 // /*spanish*/el ojo que todo ve
                                                CustomMessage("a detective's tool", /*german*/"ein Detektivwerkzeug", /*french*/"un trésor Sheikah")});
                                                 // /*spanish*/la revelación verdadera

    hintTextTable[RHT_MEGATON_HAMMER] = HintText(CustomMessage("the Megaton Hammer", /*german*/"der Stahlhammer", /*french*/"la Masse des Titans"),
                                                 // /*spanish*/el martillo Megatón
                                                 {
                                                 CustomMessage("something that can remove boulders", /*german*/"etwas, das Geröll entfernen kann", /*french*/"une chose qui enlève les rochers")
                                                  // /*spanish*/algo que pueda quitar rocas
                                                 }, {
                                                 CustomMessage("the dragon smasher", /*german*/"ein Drachenschläger", /*french*/"le tueur de dragons"),
                                                  // /*spanish*/un destructor de dragones
                                                 CustomMessage("the metal mallet", /*german*/"ein stählerner Schlägel", /*french*/"un outil de construction"),
                                                  // /*spanish*/un mazo de metal
                                                 CustomMessage("the heavy hitter", /*german*/"ein schwerer Schläger", /*french*/"un poids lourd")});
                                                  // /*spanish*/un machacador

    hintTextTable[RHT_STONE_OF_AGONY] = HintText(CustomMessage("the Stone of Agony", /*german*/"der Stein des Wissens", /*french*/"la Pierre de Souffrance"),
                                                 // /*spanish*/la Piedra de la Agonía
                                                 {
                                                 CustomMessage("a prize of the House of Skulltulas", /*german*/"ein Preis des Skulltula-Hauses", /*french*/"un prix de la maison des Skulltulas"),
                                                  // /*spanish*/un obsequio de la Casa Skulltula
                                                 CustomMessage("a secret-finding tool", /*german*/"ein geheimnisfindendes Werkzeug", /*french*/"un cherche-secrets")
                                                  // /*spanish*/un instrumento para hallar objetos
                                                 }, {
                                                 CustomMessage("the shake stone", /*german*/"der Schüttelstein", /*french*/"le fragment vibrant"),
                                                  // /*spanish*/el fragmento tintineante
                                                 CustomMessage("a gray alarm", /*german*/"der graue Alarm", /*french*/"une alerte bleue")});
                                                  // /*spanish*/una azul alarma

    hintTextTable[RHT_DINS_FIRE] = HintText(CustomMessage("Din's Fire", /*german*/"Dins Feuerinferno", /*french*/"le Feu de Din"),
                                            // /*spanish*/el Fuego de Din
                                            {
                                            CustomMessage("a Great Fairy's power", /*german*/"eine Kraft einer großen Fee", /*french*/"le pouvoir d'une grande fée")
                                             // /*spanish*/el poder de una Gran Hada
                                            }, {
                                            CustomMessage("an inferno", /*german*/"ein Inferno", /*french*/"un brasier"),
                                             // /*spanish*/un incendio
                                            CustomMessage("a heat wave", /*german*/"eine Hitzewelle", /*french*/"une vague de chaleur"),
                                             // /*spanish*/una onda de calor
                                            CustomMessage("a red ball", /*german*/"ein roter Ball", /*french*/"une explosion de flammes")});
                                             // /*spanish*/una roja esfera

    hintTextTable[RHT_FARORES_WIND] = HintText(CustomMessage("Farore's Wind", /*german*/"Farores Donnersturm", /*french*/"le Vent de Farore"),
                                               // /*spanish*/el Viento de Farore
                                               {
                                               CustomMessage("a Great Fairy's power", /*german*/"eine Kraft einer großen Fee", /*french*/"le pouvoir d'une grande fée")
                                                // /*spanish*/el poder de una Gran Hada
                                               }, {
                                               CustomMessage("teleportation", /*german*/"ein Teleportierer", /*french*/"la téléportation"),
                                                // /*spanish*/un teletransportador
                                               CustomMessage("a relocation rune", /*german*/"eine Umzugsrune", /*french*/"une rune de relocation"),
                                                // /*spanish*/una runa de transporte
                                               CustomMessage("a green ball", /*german*/"ein grüner Ball", /*french*/"une boule verte")});
                                                // /*spanish*/una verde esfera

    hintTextTable[RHT_NAYRUS_LOVE] = HintText(CustomMessage("Nayru's Love", /*german*/"Nayrus Umarmung", /*french*/"l'Amour de Nayru"),
                                              // /*spanish*/el Amor de Nayru
                                              {
                                              CustomMessage("a Great Fairy's power", /*german*/"eine Kraft einer großen Fee", /*french*/"le pouvoir d'une grande fée")
                                               // /*spanish*/el poder de una Gran Hada
                                              }, {
                                              CustomMessage("a safe space", /*german*/"ein sicherer Raum", /*french*/"une bulle de cristal"),
                                               // /*spanish*/una seguridad temporal
                                              CustomMessage("an impregnable aura", /*german*/"eine undurchdringliche Aura", /*french*/"un aura impénétrable"),
                                               // /*spanish*/un aura impenetrable
                                              CustomMessage("a blue barrier", /*german*/"eine blaue Barriere", /*french*/"une toison bleu")});
                                               // /*spanish*/una barrera azul

    hintTextTable[RHT_FIRE_ARROWS] = HintText(CustomMessage("the Fire Arrows", /*german*/"die Feuerpfeile", /*french*/"les Flèches de Feu"),
                                              // /*spanish*/la flecha de fuego
                                              {
                                              CustomMessage("a magic arrow", /*german*/"ein magischer Pfeil", /*french*/"une flèche magique")
                                               // /*spanish*/una flecha mágica
                                              }, {
                                              CustomMessage("the furnace firearm", /*german*/"die Ofenwaffe", /*french*/"une fusée solaire"),
                                               // /*spanish*/el ardiente aguijón
                                              CustomMessage("the burning bolts", /*german*/"die Brennstifte", /*french*/"un obus enflammé"),
                                               // /*spanish*/las puntas ígneas
                                              CustomMessage("a magma missile", /*german*/"eine vulkanische Rakete", /*french*/"un missile volcanique")});
                                               // /*spanish*/el misil abrasador

    hintTextTable[RHT_ICE_ARROWS] = HintText(CustomMessage("the Ice Arrows", /*german*/"die Eispfeile", /*french*/"les Flèches de Glace"),
                                             // /*spanish*/la flecha de hielo
                                             {
                                             CustomMessage("a magic arrow", /*german*/"ein magischer Pfeil", /*french*/"une flèche magique"),
                                              // /*spanish*/una flecha mágica
                                             CustomMessage("something that can stun", /*german*/"etwas, das paralysieren kann", /*french*/"une chose qui peut paralyser")
                                              // /*spanish*/algo que pueda paralizar
                                             }, {
                                             CustomMessage("the refrigerator rocket", /*german*/"die Kühlschrankrakete", /*french*/"un missile pétrifiant"),
                                              // /*spanish*/el misil congelador
                                             CustomMessage("the frostbite bolts", /*german*/"die Froststifte", /*french*/"un froid mordant"),
                                              // /*spanish*/las puntas gélidas
                                             CustomMessage("an iceberg maker", /*german*/"ein Eisbergmacher", /*french*/"une aiguille glaciale")});
                                              // /*spanish*/el control de escarcha

    hintTextTable[RHT_LIGHT_ARROWS] = HintText(CustomMessage("the Light Arrows", /*german*/"die Lichtpfeile", /*french*/"les Flèches de Lumière"),
                                               // /*spanish*/la flecha de luz
                                               {
                                               CustomMessage("a magic arrow", /*german*/"ein magischer Pfeil", /*french*/"une flèche magique")
                                                // /*spanish*/una flecha mágica
                                               }, {
                                               CustomMessage("the shining shot", /*german*/"der strahlende Schuss", /*french*/"l'arme brillante"),
                                                // /*spanish*/el haz de luz
                                               CustomMessage("the luminous launcher", /*german*/"der leuchtende Werfer", /*french*/"un jet de lumière"),
                                                // /*spanish*/el disparo luminoso
                                               CustomMessage("Ganondorf's bane", /*german*/"Ganondorfs Verderben", /*french*/"le fléau de Ganondorf"),
                                                // /*spanish*/la perdición de Ganondorf
                                               CustomMessage("the lighting bolts", /*german*/"die Lichtstifte", /*french*/"l'éclair sacré")});
                                                // /*spanish*/las puntas resplandecientes

    hintTextTable[RHT_GERUDO_MEMBERSHIP_CARD] = HintText(CustomMessage("the Gerudo Membership Card", /*german*/"der Gerudo-Pass", /*french*/"la Carte Gerudo"),
                                                         // /*spanish*/el pase de socio gerudo
                                                         {
                                                         CustomMessage("a token of recognition", /*german*/"ein Zeichen der Anerkennung", /*french*/"une preuve de reconnaissance")
                                                          // /*spanish*/una prueba de reconocimiento
                                                         }, {
                                                         CustomMessage("a girl club membership", /*german*/"eine Mitgliedskarte", /*french*/"une carte de membre"),
                                                          // /*spanish*/una fémina membresía
                                                         CustomMessage("a desert tribe's pass", /*german*/"ein Pass eines Wüstenstammes", /*french*/"un laissez-passer")});
                                                          // /*spanish*/el vale del desierto

    hintTextTable[RHT_MAGIC_BEAN] = HintText(CustomMessage("a Magic Bean", /*german*/"eine Wundererbse", /*french*/"un Haricot Magique"),
                                             // /*spanish*/una judía mágica
                                             {
                                             CustomMessage("something sometimes buried", /*german*/"etwas, das manchmal begraben ist", /*french*/"une chose parfois enterrée")
                                              // /*spanish*/algo a veces enterrado
                                             }, {
                                             CustomMessage("a wizardly legume", /*german*/"eine zauberhafte Hülse", /*french*/"un légume ensorcelé")});
                                              // /*spanish*/una legumbre hechizada

    hintTextTable[RHT_MAGIC_BEAN_PACK] = HintText(CustomMessage("Magic Beans", /*german*/"Wundererbsen", /*french*/"un Paquet de Haricots Magiques"),
                                                  // /*spanish*/unas judías mágicas
                                                  {
                                                  CustomMessage("something sometimes buried", /*german*/"etwas, das manchmal begraben ist", /*french*/"une chose parfois enterrée")
                                                   // /*spanish*/algo a veces enterrado
                                                  }, {
                                                  CustomMessage("wizardly legumes", /*german*/"zauberhafte Hülsen", /*french*/"un paquet de légumes ensorcelés")});
                                                   // /*spanish*/unas legumbres hechizadas

    hintTextTable[RHT_DOUBLE_DEFENSE] = HintText(CustomMessage("Double Defense", /*german*/"Doppelte Verteidigung", /*french*/"la Double Défence"),
                                                 // /*spanish*/la doble defensa
                                                 {
                                                 CustomMessage("a Great Fairy's power", /*german*/"eine Kraft einer großen Fee", /*french*/"le pouvoir d'une grande fée"),
                                                  // /*spanish*/el poder de una Gran Hada
                                                 CustomMessage("something heart-shaped", /*german*/"etwas Herzförmiges", /*french*/"une chose en forme de coeur")
                                                  // /*spanish*/algo con forma de corazón
                                                 }, {
                                                 CustomMessage("a white outline", /*german*/"ein weißes Profil", /*french*/"un rebord blanc"),
                                                  // /*spanish*/un contorno blanco
                                                 CustomMessage("damage decrease", /*german*/"Schadensverringerung", /*french*/"une protection supplémentaire"),
                                                  // /*spanish*/una reducción de daño
                                                 CustomMessage("strengthened love", /*german*/"gestärkte Liebe", /*french*/"un amour coriace")});
                                                  // /*spanish*/un amor fortalecido

    hintTextTable[RHT_GOLD_SKULLTULA_TOKEN] = HintText(CustomMessage("a Gold Skulltula Token", /*german*/"ein goldenes Skulltula-Symbol", /*french*/"un Symbole de Skulltula d'or"),
                                                       // /*spanish*/un símbolo de skulltula dorada
                                                       {
                                                       CustomMessage("a token of recognition", /*german*/"ein Zeichen der Anerkennung", /*french*/"une preuve de reconnaissance"),
                                                        // /*spanish*/una prueba de reconocimiento
                                                       CustomMessage("something sometimes buried", /*german*/"etwas, das manchmal begraben ist", /*french*/"une chose parfois enterrée")
                                                        // /*spanish*/algo a veces enterrado
                                                       }, {
                                                       CustomMessage("proof of destruction", /*german*/"Nachweis der Zerstörung", /*french*/"un certificat d'élimination"),
                                                        // /*spanish*/una prueba de la destrucción
                                                       CustomMessage("an arachnid chip", /*german*/"ein spinnenartiges Symbol", /*french*/"un symbole cranien"),
                                                        // /*spanish*/una figura arácnida
                                                       CustomMessage("spider remains", /*german*/"Spinnenüberreste", /*french*/"une dépouille dorée"),
                                                        // /*spanish*/unos restos dorados
                                                       CustomMessage("one percent of a curse", /*german*/"ein Prozent eines Fluchs", /*french*/"un centième de malédiction")});
                                                        // /*spanish*/una centésima de una maldición

    hintTextTable[RHT_POCKET_EGG] = HintText(CustomMessage("the Pocket Egg", /*german*/"das Ei", /*french*/"l'Oeuf de Poche"),
                                             // /*spanish*/el huevo de bolsillo
                                             {
                                             CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges"),
                                              // /*spanish*/un objeto de una misión secundaria
                                             CustomMessage("an egg", /*german*/"ein Ei", /*french*/"un oeuf")
                                              // /*spanish*/un huevo
                                             }, {
                                             CustomMessage("a Cucco container", /*german*/"ein Hühnerbehälter", /*french*/"un réservoir à Cocotte"),
                                              // /*spanish*/cuco contenido
                                             CustomMessage("a Cucco, eventually", /*german*/"schlussendlich ein Huhn", /*french*/"un poussin éventuel"),
                                              // /*spanish*/un futuro cuco
                                             CustomMessage("a fowl youth", /*german*/"ein junges Geflügel", /*french*/"une omelette crue")});
                                              // /*spanish*/una dulce juventud

    hintTextTable[RHT_POCKET_CUCCO] = HintText(CustomMessage("the Pocket Cucco", /*german*/"Kiki", /*french*/"la Cocotte de Poche"),
                                               // /*spanish*/el cuco de bolsillo
                                               {
                                               CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessage("a little clucker", /*german*/"ein kleiner Gackerer", /*french*/"un petit glousseur")});
                                                // /*spanish*/un pollito chiquito

    hintTextTable[RHT_COJIRO] = HintText(CustomMessage("Cojiro", /*german*/"Henni", /*french*/"le p'tit poulet"),
                                         // /*spanish*/a Cojiro
                                         {
                                         CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges")
                                          // /*spanish*/un objeto de una misión secundaria
                                         }, {
                                         CustomMessage("a cerulean capon", /*german*/"ein coelinblaues Kapaun", /*french*/"un paon azur")});
                                          // /*spanish*/un cerúleo capón

    hintTextTable[RHT_ODD_MUSHROOM] = HintText(CustomMessage("an Odd Mushroom", /*german*/"ein Schimmelpilz", /*french*/"un Champignon Suspect"),
                                               // /*spanish*/un champiñón extraño
                                               {
                                               CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessage("a powder ingredient", /*german*/"eine Puderingredienz", /*french*/"un ingrédient à poudre")});
                                                // /*spanish*/un oloroso ingrediente

    hintTextTable[RHT_ODD_POTION] = HintText(CustomMessage("an Odd Potion", /*german*/"ein Modertrank", /*french*/"une Mixture Suspecte"),
                                             // /*spanish*/una medicina rara
                                             {
                                             CustomMessage("something that contains medicine", /*german*/"etwas, das Medizin enthält", /*french*/"une chose médicamenteuse"),
                                              // /*spanish*/algo que contenga medicina
                                             CustomMessage("something with a strange smell", /*german*/"etwas, das streng riecht", /*french*/"une chose qui sent bizarre"),
                                              // /*spanish*/algo con un olor extraño
                                             CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges")
                                              // /*spanish*/un objeto de una misión secundaria
                                             }, {
                                             CustomMessage("Granny's goodies", /*german*/"Omas Zuckerwerk", /*french*/"la confiserie de mamie")});
                                              // /*spanish*/la especialidad de la abuela

    hintTextTable[RHT_POACHERS_SAW] = HintText(CustomMessage("the Poacher's Saw", /*german*/"eine Säge", /*french*/"la Scie du Chasseur"),
                                               // /*spanish*/la sierra del furtivo
                                               {
                                               CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessage("a tree killer", /*german*/"ein Baumtöter", /*french*/"un coupeur d'arbres")});
                                                // /*spanish*/un destructor de árboles

    hintTextTable[RHT_BROKEN_SWORD] = HintText(CustomMessage("the Broken Goron's Sword", /*german*/"das zerbrochene Goronen-Schwert", /*french*/"l'Épée Brisée de Goron"),
                                               // /*spanish*/la espada goron rota
                                               {
                                               CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges"),
                                                // /*spanish*/un objeto de una misión secundaria
                                               CustomMessage("a sword", /*german*/"ein Schwert", /*french*/"une épée")
                                                // /*spanish*/una espada
                                               }, {
                                               CustomMessage("a shattered slicer", /*german*/"ein zersplitterter Schneider", /*french*/"une arme cassée")});
                                                // /*spanish*/una rebanadora rota

    hintTextTable[RHT_PRESCRIPTION] = HintText(CustomMessage("the Prescription", /*german*/"ein Rezept", /*french*/"une Ordonnance"),
                                               // /*spanish*/la receta
                                               {
                                               CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessage("a pill pamphlet", /*german*/"ein Pillenpamphlet", /*french*/"un document urgent"),
                                                // /*spanish*/un instructivo medicinal
                                               CustomMessage("a doctor's note", /*german*/"eine Notiz eines Doktors", /*french*/"un papier médical")});
                                                // /*spanish*/unas notas del doctor

    hintTextTable[RHT_EYEBALL_FROG] = HintText(CustomMessage("the Eyeball Frog", /*german*/"der Glotzfrosch", /*french*/"le Crapaud-qui-louche"),
                                               // /*spanish*/la rana de ojos saltones
                                               {
                                               CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges")
                                                // /*spanish*/un objeto de una misión secundaria
                                               }, {
                                               CustomMessage("a perceiving polliwog", /*german*/"eine wahrnehmende Kaulquappe", /*french*/"un amphibien")});
                                                // /*spanish*/un variopinto batracio

    hintTextTable[RHT_EYEDROPS] = HintText(CustomMessage("the Eyedrops", /*german*/"die Augentropfen", /*french*/"une phiole de Super-Gouttes"),
                                           // /*spanish*/las supergotas oculares
                                           {
                                           CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges")
                                            // /*spanish*/un objeto de una misión secundaria
                                           }, {
                                           CustomMessage("a vision vial", /*german*/"eine Sichtphiole", /*french*/"une solution oculaire")});
                                            // /*spanish*/un remedio para la vista

    hintTextTable[RHT_CLAIM_CHECK] = HintText(CustomMessage("the Claim Check", /*german*/"ein Zertifikat", /*french*/"un Certificat"),
                                              // /*spanish*/el recibo
                                              {
                                              CustomMessage("a trade quest item", /*german*/"ein Gegenstand einer Handelsmission", /*french*/"un objet de quête d'échanges")
                                               // /*spanish*/un objeto de una misión secundaria
                                              }, {
                                              CustomMessage("a three day wait", /*german*/"eine dreitägige Erwartung", /*french*/"un rendez-vous dans trois jours")});
                                               // /*spanish*/unos tres días de espera

    hintTextTable[RHT_PROGRESSIVE_HOOKSHOT] = HintText(CustomMessage("a Hookshot", /*german*/"ein Enterhaken", /*french*/"un Grappin"),
                                                       // /*spanish*/un gancho
                                                       {
                                                       CustomMessage("something that can grab things", /*german*/"etwas, das Dinge greifen kann", /*french*/"une chose qui peut attraper"),
                                                        // /*spanish*/algo que pueda agarrar cosas
                                                       CustomMessage("something that can stun", /*german*/"etwas, das paralysieren kann", /*french*/"une chose qui peut paralyser")
                                                        // /*spanish*/algo que pueda paralizar
                                                       }, {
                                                       CustomMessage("Dampé's keepsake", /*german*/"Dampés Andenken", /*french*/"l'héritage d'Igor"),
                                                        // /*spanish*/un recuerdo de Dampé
                                                       CustomMessage("the Grapple Beam", /*german*/"der Greifstrahl", /*french*/"le rayon grippeur"),
                                                        // /*spanish*/una garra metálica
                                                       CustomMessage("the RHT_BOING! chain", /*german*/"die Kette des RHT_BOING!", /*french*/"la chaîne de RHT_BOING!")});
                                                        // /*spanish*/una cadena retráctil

    hintTextTable[RHT_PROGRESSIVE_STRENGTH] = HintText(CustomMessage("a Strength Upgrade", /*german*/"eine Stärkeverbesserung", /*french*/"une Amélioration de Force"),
                                                       // /*spanish*/un aumento de fuerza
                                                       {
                                                       CustomMessage("something that can remove boulders", /*german*/"etwas, das Geröll entfernen kann", /*french*/"une chose qui enlève les rochers")
                                                        // /*spanish*/algo que pueda quitar rocas
                                                       }, {
                                                       CustomMessage("power gloves", /*german*/"Krafthandschuhe", /*french*/"une paire de gants de travail"),
                                                        // /*spanish*/unos poderosos guanteletes
                                                       CustomMessage("metal mittens", /*german*/"Metallfäustlinge", /*french*/"une paire de mitaines"),
                                                        // /*spanish*/unas manoplas metálicas
                                                       CustomMessage("the heavy lifty", /*german*/"der Schwerlastheber", /*french*/"la puissance de dix hommes")});
                                                        // /*spanish*/un levantamiento pesado

    hintTextTable[RHT_PROGRESSIVE_BOMB_BAG] = HintText(CustomMessage("a Bomb Bag", /*german*/"ein Bombenbeutel", /*french*/"un Sac de Bombes"),
                                                       // /*spanish*/un saco de bombas
                                                       {
                                                       CustomMessage("explosives", /*german*/"ein Explosivpaket", /*french*/"un paquet d'explosifs"),
                                                        // /*spanish*/un montón de explosivos
                                                       CustomMessage("something that can remove boulders", /*german*/"etwas, das Geröll entfernen kann", /*french*/"une chose qui enlève les rochers")
                                                        // /*spanish*/algo que pueda quitar rocas
                                                       }, {
                                                       CustomMessage("an explosive container", /*german*/"ein Explosivbehälter", /*french*/"un porte-grenade"),
                                                        // /*spanish*/un recipiente explosivo
                                                       CustomMessage("a blast bag", /*german*/"ein Dodongomagen", /*french*/"un estomac de Dodongo")});
                                                        // /*spanish*/un zurrón de estallidos

    hintTextTable[RHT_PROGRESSIVE_BOW] = HintText(CustomMessage("a Fairy Bow", /*german*/"ein Feen-Bogen", /*french*/"l'Arc des Fées"),
                                                  // /*spanish*/un arco de las hadas
                                                  {
                                                  CustomMessage("a projectile shooter", /*german*/"ein Projektilwerfer", /*french*/"un tire-projectile")
                                                   // /*spanish*/un arma de proyectil
                                                  }, {
                                                  CustomMessage("an archery enabler", /*german*/"ein Bogenschussermöglicher", /*french*/"un facilitateur de tir"),
                                                   // /*spanish*/un tiro al blanco
                                                  CustomMessage("a danger dart launcher", /*german*/"ein Pfeilwerfer", /*french*/"un tire-fléchette")});
                                                   // /*spanish*/un peligroso lanzadardos

    hintTextTable[RHT_PROGRESSIVE_SLINGSHOT] = HintText(CustomMessage("a Slingshot", /*german*/"eine Schleuder", /*french*/"un Lance-Pierre"),
                                                        // /*spanish*/una resortera de las hadas
                                                        {
                                                        CustomMessage("a projectile shooter", /*german*/"ein Projektilwerfer", /*french*/"un tire-projectile")
                                                         // /*spanish*/un arma de proyectil
                                                        }, {
                                                        CustomMessage("a seed shooter", /*german*/"ein Nusswerfer", /*french*/"un lance-noix"),
                                                         // /*spanish*/un lanzasemillas
                                                        CustomMessage("a rubberband", /*german*/"ein Gummiband", /*french*/"un élastique"),
                                                         // /*spanish*/un tirachinas
                                                        CustomMessage("a child's catapult", /*german*/"ein Kinderkatapult", /*french*/"un jouet d'enfant")});
                                                         // /*spanish*/una catapulta infantil

    hintTextTable[RHT_PROGRESSIVE_WALLET] = HintText(CustomMessage("a Wallet", /*german*/"eine Börse", /*french*/"une Bourse"),
                                                     // /*spanish*/una bolsa de rupias
                                                     {
                                                     CustomMessage("a prize of the House of Skulltulas", /*german*/"ein Preis des Skulltula-Hauses", /*french*/"un prix de la maison des Skulltulas")
                                                      // /*spanish*/un obsequio de la Casa Skulltula
                                                     }, {
                                                     CustomMessage("a mo' money holder", /*german*/"ein Geldhalter", /*french*/"un sac à sous"),
                                                      // /*spanish*/una cartera de dinero
                                                     CustomMessage("a gem purse", /*german*/"ein Edelsteinportemonnaie", /*french*/"une sacoche"),
                                                      // /*spanish*/un zurrón de gemas
                                                     CustomMessage("a portable bank", /*german*/"eine tragbare Bank", /*french*/"une petite banque")});
                                                      // /*spanish*/un banco portable

    hintTextTable[RHT_PROGRESSIVE_SCALE] = HintText(CustomMessage("a Zora Scale", /*german*/"eine Zora-Schuppe", /*french*/"une Écaille Zora"),
                                                    // /*spanish*/una escama Zora
                                                    {
                                                    CustomMessage("a diving tool", /*german*/"ein Tauchwerkzeug", /*french*/"un outil de plongée")
                                                     // /*spanish*/un instrumento de buceo
                                                    }, {
                                                    CustomMessage("a deeper dive", /*german*/"eine Tauchblase", /*french*/"une bulle de plongée"),
                                                     // /*spanish*/un profundo buceo
                                                    CustomMessage("a piece of Zora", /*german*/"ein Zora-Fragment", /*french*/"un morceau de Zora")});
                                                     // /*spanish*/un fragmento de Zora

    hintTextTable[RHT_PROGRESSIVE_NUT_UPGRADE] = HintText(CustomMessage("Deku Nut Capacity", /*german*/"Deku-Nuß-Kapazität", /*french*/"une Augmentation de Noix Mojo"),
                                                          // /*spanish*/un aumento de nueces deku
                                                          {
                                                          CustomMessage("some Deku munitions", /*german*/"etwas Deku-Munition", /*french*/"un paquet de munitions Mojo"),
                                                           // /*spanish*/un montón de municiones Deku
                                                          CustomMessage("something that can stun", /*german*/"etwas, das paralysieren kann", /*french*/"une chose qui peut paralyser")
                                                           // /*spanish*/algo que pueda paralizar
                                                          }, {
                                                          CustomMessage("more nuts", /*german*/"mehr Nüsse", /*french*/"encore plus de noix"),
                                                           // /*spanish*/más semillas de nogal
                                                          CustomMessage("flashbang storage", /*german*/"Blendgranatenvorrat", /*french*/"un sac à noix")});
                                                           // /*spanish*/más frutos aturdidores

    hintTextTable[RHT_PROGRESSIVE_STICK_UPGRADE] = HintText(CustomMessage("Deku Stick Capacity", /*german*/"Deku-Stab-Kapazität", /*french*/"une augmentation de bâtons Mojo"),
                                                            // /*spanish*/un aumento de palos deku
                                                            {
                                                            CustomMessage("some Deku munitions", /*german*/"etwas Deku-Munition", /*french*/"un paquet de munitions Mojo")
                                                             // /*spanish*/un montón de municiones Deku
                                                            }, {
                                                            CustomMessage("lumber racks", /*german*/"Holzgestelle", /*french*/"un paquet de bois"),
                                                             // /*spanish*/más bastones
                                                            CustomMessage("more flammable twigs", /*german*/"mehr entflammbare Zweige", /*french*/"beaucoup de branches")});
                                                             // /*spanish*/más varas

    hintTextTable[RHT_PROGRESSIVE_MAGIC_METER] = HintText(CustomMessage("a Magic Meter", /*german*/"ein Magieabmaß", /*french*/"une Jauge de Magie"),
                                                          // /*spanish*/un aumento de poder mágico
                                                          {
                                                          CustomMessage("a Great Fairy's power", /*german*/"eine Kraft einer großen Fee", /*french*/"le pouvoir d'une grande fée")
                                                           // /*spanish*/el poder de una Gran Hada
                                                          }, {
                                                          CustomMessage("mystic training", /*german*/"mystisches Training", /*french*/"un potentiel magique"),
                                                           // /*spanish*/una maestría mística
                                                          CustomMessage("pixie dust", /*german*/"Feenstaub", /*french*/"de la poudre de fée"),
                                                           // /*spanish*/un polvo de hada
                                                          CustomMessage("a green rectangle", /*german*/"ein grünes Rechteck", /*french*/"un rectangle vert")});
                                                           // /*spanish*/una verduzca barra

    hintTextTable[RHT_PROGRESSIVE_OCARINA] = HintText(CustomMessage("an Ocarina", /*german*/"eine Okarina", /*french*/"un ocarina"),
                                                      // /*spanish*/una ocarina
                                                      {
                                                      CustomMessage("something given by Saria", /*german*/"ein Geschenk von Salia", /*french*/"un cadeau de Saria"),
                                                       // /*spanish*/un obsequio de Saria
                                                      CustomMessage("something kept by the royal family", /*german*/"etwas, das von der royalen Familie bewahrt wird", /*french*/"une chose qui paralyse")
                                                       // /*spanish*/algo guardado por la familia real
                                                      }, {
                                                      CustomMessage("a flute", /*german*/"eine Flöte", /*french*/"un bec musical"),
                                                       // /*spanish*/un utensilio musical
                                                      CustomMessage("a music maker", /*german*/"ein Musikmacher", /*french*/"un porteur de chansons")});
                                                       // /*spanish*/un instrumento

    hintTextTable[RHT_PROGRESSIVE_BOMBCHUS] = HintText(CustomMessage("Bombchus", /*german*/"Krabbelminen", /*french*/"un paquet de Missiles"),
                                                       // /*spanish*/unos bombchus
                                                       {
                                                       CustomMessage("a prize of the House of Skulltulas", /*german*/"ein Preis des Skulltula-Hauses", /*french*/"un prix de la maison des Skulltulas"),
                                                        // /*spanish*/un obsequio de la Casa Skulltula
                                                       CustomMessage("explosives", /*german*/"ein Explosivpaket", /*french*/"un paquet d'explosifs")
                                                        // /*spanish*/un montón de explosivos
                                                       }, {
                                                       CustomMessage("mice bombs", /*german*/"Mäusebomben", /*french*/"un adorable explosif"),
                                                        // /*spanish*/unas bombas roedoras
                                                       CustomMessage("proximity mice", /*german*/"Näherungsmäuse", /*french*/"une mine anti-rongeur"),
                                                        // /*spanish*/unos explosivos ratoncitos
                                                       CustomMessage("wall crawlers", /*german*/"Wandkrabbler", /*french*/"un rapide grimpeur"),
                                                        // /*spanish*/unos trepaparedes
                                                       CustomMessage("trail blazers", /*german*/"Vorreiter", /*french*/"un zigzag éclatant")});
                                                        // /*spanish*/unas ratas propulsadas

    hintTextTable[RHT_PROGRESSIVE_GORONSWORD] = HintText(CustomMessage("a Goron Sword", /*german*/"ein Goronen-Schwert", /*french*/"une épée Goron"),
                                                         // /*spanish*/una espada goron
                                                         {
                                                         CustomMessage("a sword", /*german*/"ein Schwert", /*french*/"une épée")
                                                          // /*spanish*/una espada
                                                         }, {
                                                         CustomMessage("a long blade", /*german*/"eine lange Klinge", /*french*/"une longue lame"),
                                                          // /*spanish*/una gran hoja
                                                         CustomMessage("a Goron weapon", /*german*/"eine Goronenwaffe", /*french*/"une arme Goron")});
                                                          // /*spanish*/un arma goron

    hintTextTable[RHT_EMPTY_BOTTLE] = HintText(CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon vide"),
                                               // /*spanish*/una botella
                                               {
                                               CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                // /*spanish*/una botella
                                               }, {
                                               CustomMessage("a glass container", /*german*/"ein Glasbehälter", /*french*/"un cylindre de cristal"),
                                                // /*spanish*/un recipiente de cristal
                                               CustomMessage("an empty jar", /*german*/"ein leerer Krug", /*french*/"une jarre incassable"),
                                                // /*spanish*/un frasco vacío
                                               CustomMessage("encased air", /*german*/"eingeschlossene Luft", /*french*/"un bocal d'air")});
                                                // /*spanish*/aire a presión

    hintTextTable[RHT_BOTTLE_WITH_MILK] = HintText(CustomMessage("a Milk Bottle", /*german*/"eine Milchflasche", /*french*/"un flacon de lait"),
                                                   // /*spanish*/una botella de leche
                                                   {
                                                   CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                    // /*spanish*/una botella
                                                   }, {
                                                   CustomMessage("cow juice", /*german*/"Kuhsaft", /*french*/"une source de calcium"),
                                                    // /*spanish*/una fuente de calcio
                                                   CustomMessage("a white liquid", /*german*/"eine weiße Flüssigkeit", /*french*/"un liquide blanc"),
                                                    // /*spanish*/una bebida nutritiva
                                                   CustomMessage("a baby's breakfast", /*german*/"ein Babyfrühstück", /*french*/"du jus pour bébé")});
                                                    // /*spanish*/un trago para bebés

    hintTextTable[RHT_BOTTLE_WITH_RED_POTION] = HintText(CustomMessage("a Red Potion Bottle", /*german*/"eine Flasche mit rotem Elixier", /*french*/"un flacon de potion rouge"),
                                                         // /*spanish*/una botella de poción roja
                                                         {
                                                         CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                          // /*spanish*/una botella
                                                         }, {
                                                         CustomMessage("a vitality vial", /*german*/"eine Vitalitätsphiole", /*french*/"un mélange de vitalité"),
                                                          // /*spanish*/una pócima vitalicia
                                                         CustomMessage("a red liquid", /*german*/"eine rote Flüssigkeit", /*french*/"un liquide rouge")});
                                                          // /*spanish*/un remedio rojizo

    hintTextTable[RHT_BOTTLE_WITH_GREEN_POTION] = HintText(CustomMessage("a Green Potion Bottle", /*german*/"eine Flasche mit grünem Elixier", /*french*/"un flacon de potion verte"),
                                                           // /*spanish*/una botella de poción verde
                                                           {
                                                           CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                            // /*spanish*/una botella
                                                           }, {
                                                           CustomMessage("a magic mixture", /*german*/"eine magische Mixtur", /*french*/"une réserve magique"),
                                                            // /*spanish*/un potingue mágico
                                                           CustomMessage("a green liquid", /*german*/"eine grüne Flüssigkeit", /*french*/"un liquide vert")});
                                                            // /*spanish*/un remedio verduzco

    hintTextTable[RHT_BOTTLE_WITH_BLUE_POTION] = HintText(CustomMessage("a Blue Potion Bottle", /*german*/"eine Flasche mit blauem Elixier", /*french*/"un flacon de potion bleue"),
                                                          // /*spanish*/una botella de poción azul
                                                          {
                                                          CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                           // /*spanish*/una botella
                                                          }, {
                                                          CustomMessage("an ailment antidote", /*german*/"ein Krankheitsantidot", /*french*/"l'élixir ultime"),
                                                           // /*spanish*/un antídoto para el dolor
                                                          CustomMessage("a blue liquid", /*german*/"eine blaue Flüssigkeit", /*french*/"un liquide bleu")});
                                                           // /*spanish*/un remedio índigo

    hintTextTable[RHT_BOTTLE_WITH_FAIRY] = HintText(CustomMessage("a Fairy Bottle", /*german*/"eine Feenflasche", /*french*/"une fée en flacon"),
                                                    // /*spanish*/un hada en una botella
                                                    {
                                                    CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                     // /*spanish*/una botella
                                                    }, {
                                                    CustomMessage("an imprisoned fairy", /*german*/"eine gefangene Fee", /*french*/"une fée emprisonnée"),
                                                     // /*spanish*/un hada atrapada
                                                    CustomMessage("an extra life", /*german*/"ein Extraleben", /*french*/"une vie de rechange"),
                                                     // /*spanish*/una oportunidad más
                                                    CustomMessage("Navi's cousin", /*german*/"Navis Vetter", /*french*/"le cousin de Navi")});
                                                     // /*spanish*/una prima de Navi

    hintTextTable[RHT_BOTTLE_WITH_FISH] = HintText(CustomMessage("a Fish Bottle", /*german*/"eine Fischflasche", /*french*/"un poisson en flacon"),
                                                   // /*spanish*/un pez en una botella
                                                   {
                                                   CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                    // /*spanish*/una botella
                                                   }, {
                                                   CustomMessage("an aquarium", /*german*/"ein Aquarium", /*french*/"un aquarium"),
                                                    // /*spanish*/un escamado ser
                                                   CustomMessage("a deity's snack", /*german*/"ein Gottheitssnack", /*french*/"le repas d'un dieu marin")});
                                                    // /*spanish*/un tentempié de cierta deidad

    hintTextTable[RHT_BOTTLE_WITH_BLUE_FIRE] = HintText(CustomMessage("a Blue Fire Bottle", /*german*/"eine Flasche mit blauem Feuer", /*french*/"une flamme bleue en flacon"),
                                                        // /*spanish*/una botella de fuego azul
                                                        {
                                                        CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                         // /*spanish*/una botella
                                                        }, {
                                                        CustomMessage("a conflagration canteen", /*german*/"eine Brandfeldflasche", /*french*/"une mystérieuse flamme"),
                                                         // /*spanish*/un incendio retenido
                                                        CustomMessage("an icemelt jar", /*german*/"ein eisschmelzender Krug", /*french*/"un brasier glacial")});
                                                         // /*spanish*/unas brasas enfrascadas

    hintTextTable[RHT_BOTTLE_WITH_BUGS] = HintText(CustomMessage("a Bug Bottle", /*german*/"eine Wanzenflasche", /*french*/"un insecte en flacon"),
                                                   // /*spanish*/unos insectos en una botella
                                                   {
                                                   CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                    // /*spanish*/una botella
                                                   }, {
                                                   CustomMessage("an insectarium", /*german*/"ein Insektarium", /*french*/"un insectarium"),
                                                    // /*spanish*/unos invertebrados seres
                                                   CustomMessage("Skulltula finders", /*german*/"Skulltula-Finder", /*french*/"une poignée de trouve-Skulltula")});
                                                    // /*spanish*/unos rastreadores de skulltulas

    hintTextTable[RHT_BOTTLE_WITH_POE] = HintText(CustomMessage("a Poe Bottle", /*german*/"eine Irrlichtflasche", /*french*/"un Esprit en flacon"),
                                                  // /*spanish*/un Poe en una botella
                                                  {
                                                  CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                   // /*spanish*/una botella
                                                  }, {
                                                  CustomMessage("a spooky ghost", /*german*/"ein spukhafter Geist", /*french*/"un effroyable fantôme"),
                                                   // /*spanish*/un espantoso espectro
                                                  CustomMessage("a face in the jar", /*german*/"ein Gesicht im Krug", /*french*/"un visage dans un bocal")});
                                                   // /*spanish*/una expresión enfrascada

    hintTextTable[RHT_BOTTLE_WITH_BIG_POE] = HintText(CustomMessage("a Big Poe Bottle", /*german*/"eine Nachtschwärmerflasche", /*french*/"une Ame en flacon"),
                                                      // /*spanish*/un Gran Poe en una botella
                                                      {
                                                      CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                       // /*spanish*/una botella
                                                      }, {
                                                      CustomMessage("the spookiest ghost", /*german*/"der spukhafteste Geist", /*french*/"un épouvantable spectre"),
                                                       // /*spanish*/el espectro más espeluznante
                                                      CustomMessage("a sidequest spirit", /*german*/"ein Nebenmissionsgeist", /*french*/"un précieux esprit")});
                                                       // /*spanish*/un buen valorado espíritu

    hintTextTable[RHT_RUTOS_LETTER] = HintText(CustomMessage("Ruto's Letter", /*german*/"Rutos Brief", /*french*/"la lettre de Ruto"),
                                               // /*spanish*/la carta de Ruto
                                               {
                                               CustomMessage("a bottle", /*german*/"eine Flasche", /*french*/"un flacon")
                                                // /*spanish*/una botella
                                               }, {
                                               CustomMessage("a call for help", /*german*/"ein Hilferuf", /*french*/"un appel au secours"),
                                                // /*spanish*/una llamada de auxilio
                                               CustomMessage("the note that Mweeps", /*german*/"eine Notiz, die Mweeps", /*french*/"un message qui fait mwip"),
                                                // /*spanish*/un escrito mweep
                                               CustomMessage("an RHT_SOS call", /*german*/"ein RHT_SOS Signal", /*french*/"un signal RHT_SOS"),
                                                // /*spanish*/una nota de socorro
                                               CustomMessage("a fishy stationery", /*german*/"ein nasses Papier", /*french*/"un papier mouillé")});
                                                // /*spanish*/un mensaje de ayuda

    hintTextTable[RHT_ZELDAS_LULLABY] = HintText(CustomMessage("Zelda's Lullaby", /*german*/"Zeldas Wiegenlied", /*french*/"la berceuse de Zelda"),
                                                 // /*spanish*/la Nana de Zelda
                                                 {
                                                 CustomMessage("a regular song", /*german*/"ein normales Lied", /*french*/"une chanson normale"),
                                                  // /*spanish*/una cancion normal
                                                 CustomMessage("something kept by the royal family", /*german*/"etwas, das von der royalen Familie bewahrt wird", /*french*/"une chose qui paralyse")
                                                  // /*spanish*/algo guardado por la familia real
                                                 }, {
                                                 CustomMessage("a song of royal slumber", /*german*/"ein royales Lied", /*french*/"une chanson royale"),
                                                  // /*spanish*/la canción real
                                                 CustomMessage("a triforce tune", /*german*/"eine heilige Musik", /*french*/"la musique sacrée")});
                                                  // /*spanish*/la melodía de la trifuerza

    hintTextTable[RHT_EPONAS_SONG] = HintText(CustomMessage("Epona's Song", /*german*/"Eponas Lied", /*french*/"le chant d'Epona"),
                                              // /*spanish*/la Canción de Epona
                                              {
                                              CustomMessage("a regular song", /*german*/"ein normales Lied", /*french*/"une chanson normale")
                                               // /*spanish*/una cancion normal
                                              }, {
                                              CustomMessage("an equestrian etude", /*german*/"eine Reiterhymne", /*french*/"une hymne équestre"),
                                               // /*spanish*/una copla ecuestre
                                              CustomMessage("Malon's melody", /*german*/"Malons Melodie", /*french*/"la mélodie des vaches"),
                                               // /*spanish*/la sonata de Malon
                                              CustomMessage("a ranch song", /*german*/"ein Lied des Ackers", /*french*/"le chant des champs")});
                                               // /*spanish*/un canto rupestre

    hintTextTable[RHT_SARIAS_SONG] = HintText(CustomMessage("Saria's Song", /*german*/"Salias Lied", /*french*/"le chant de Saria"),
                                              // /*spanish*/la Canción de Saria
                                              {
                                              CustomMessage("a regular song", /*german*/"ein normales Lied", /*french*/"une chanson normale"),
                                               // /*spanish*/una cancion normal
                                              CustomMessage("something given by Saria", /*german*/"ein Geschenk von Salia", /*french*/"un cadeau de Saria")
                                               // /*spanish*/un obsequio de Saria
                                              }, {
                                              CustomMessage("a song of dancing Gorons", /*german*/"ein Lied der tanzenden Goronen", /*french*/"une chanson danceuse"),
                                               // /*spanish*/un pegadizo tono goron
                                              CustomMessage("Saria's phone number", /*german*/"Salias Telefonnummer", /*french*/"le téléphone d'une amie")});
                                               // /*spanish*/una consulta de asistencia

    hintTextTable[RHT_SUNS_SONG] = HintText(CustomMessage("the Sun's Song", /*german*/"Hymne der Sonne", /*french*/"le chant du soleil"),
                                            // /*spanish*/la Canción del Sol
                                            {
                                            CustomMessage("a regular song", /*german*/"ein normales Lied", /*french*/"une chanson normale"),
                                             // /*spanish*/una cancion normal
                                            CustomMessage("something that can stun", /*german*/"etwas, das paralysieren kann", /*french*/"une chose qui peut paralyser")
                                             // /*spanish*/algo que pueda paralizar
                                            }, {
                                            CustomMessage("Sunny Day", /*german*/"sonniger Tag", /*french*/"Zénith"),
                                             // /*spanish*/un día soleado
                                            CustomMessage("the ReDead's bane", /*german*/"das Verderben der Remorts", /*french*/"le fléau des Éffrois"),
                                             // /*spanish*/la destructora de Redeads
                                            CustomMessage("the Gibdo's bane", /*german*/"das Verderben der Gibdos", /*french*/"le fléau des Gibdo")});
                                             // /*spanish*/la destructora de Gibdos

    hintTextTable[RHT_SONG_OF_TIME] = HintText(CustomMessage("the Song of Time", /*german*/"Hymne der Zeit", /*french*/"le chant du temps"),
                                               // /*spanish*/la Canción del tiempo
                                               {
                                               CustomMessage("a regular song", /*german*/"ein normales Lied", /*french*/"une chanson normale")
                                                // /*spanish*/una cancion normal
                                               }, {
                                               CustomMessage("a song 7 years long", /*german*/"ein sieben Jahre langes Lied", /*french*/"le flot du temps"),
                                                // /*spanish*/la setenada canción
                                               CustomMessage("the tune of ages", /*german*/"Hymne des Äons", /*french*/"le Chant des Âges")});
                                                // /*spanish*/la melodía eónica

    hintTextTable[RHT_SONG_OF_STORMS] = HintText(CustomMessage("the Song of Storms", /*german*/"Hymne des Sturms", /*french*/"le chant des tempêtes"),
                                                 // /*spanish*/la Canción de la Tormenta
                                                 {
                                                 CustomMessage("a regular song", /*german*/"ein normales Lied", /*french*/"une chanson normale")
                                                  // /*spanish*/una cancion normal
                                                 }, {
                                                 CustomMessage("Rain Dance", /*german*/"Regentanz", /*french*/"Danse Pluie"),
                                                  // /*spanish*/la danza de la lluvia
                                                 CustomMessage("a thunderstorm tune", /*german*/"eine Gewitterhymne", /*french*/"une hymne foudroyante"),
                                                  // /*spanish*/una sonata tormentosa
                                                 CustomMessage("windmill acceleration", /*german*/"Windmühlenbeschleunigung", /*french*/"l'accélérateur de moulins")});
                                                  // /*spanish*/el arranque de molinos

    hintTextTable[RHT_MINUET_OF_FOREST] = HintText(CustomMessage("the Minuet of Forest", /*german*/"Menuett des Waldes", /*french*/"le menuet de la forêt"),
                                                   // /*spanish*/el Minueto del bosque
                                                   {
                                                   CustomMessage("a warp song", /*german*/"ein Teleportationslied", /*french*/"une chanson de téléportation")
                                                    // /*spanish*/una canción de teletransportación
                                                   }, {
                                                   CustomMessage("the song of tall trees", /*german*/"Lied der großen Bäume", /*french*/"le bruit des arbres"),
                                                    // /*spanish*/la canción de las copas
                                                   CustomMessage("an arboreal anthem", /*german*/"eine baumartige Hymne", /*french*/"l'hymne sylvestre"),
                                                    // /*spanish*/el himno forestal
                                                   CustomMessage("a green spark trail", /*german*/"ein grüner Komet", /*french*/"une comète verte")});
                                                    // /*spanish*/el sendero esmeralda

    hintTextTable[RHT_BOLERO_OF_FIRE] = HintText(CustomMessage("the Bolero of Fire", /*german*/"Bolero des Feuers", /*french*/"le boléro du feu"),
                                                 // /*spanish*/el Bolero del fuego
                                                 {
                                                 CustomMessage("a warp song", /*german*/"ein Teleportationslied", /*french*/"une chanson de téléportation")
                                                  // /*spanish*/una canción de teletransportación
                                                 }, {
                                                 CustomMessage("a song of lethal lava", /*german*/"Lied der tödlichen Lava", /*french*/"une musique enflammée"),
                                                  // /*spanish*/la canción de la lava
                                                 CustomMessage("a red spark trail", /*german*/"ein roter Komet", /*french*/"une comète rouge"),
                                                  // /*spanish*/el sendero rubí
                                                 CustomMessage("a volcanic verse", /*german*/"ein vulkanischer Vers", /*french*/"le souffle du volcan")});
                                                  // /*spanish*/el verso volcánico

    hintTextTable[RHT_SERENADE_OF_WATER] = HintText(CustomMessage("the Serenade of Water", /*german*/"Serenade des Wassers", /*french*/"la sérénade de l'eau"),
                                                    // /*spanish*/la Serenata del agua
                                                    {
                                                    CustomMessage("a warp song", /*german*/"ein Teleportationslied", /*french*/"une chanson de téléportation")
                                                     // /*spanish*/una canción de teletransportación
                                                    }, {
                                                    CustomMessage("a song of a damp ditch", /*german*/"die Stille des Wassers", /*french*/"le calme de l'eau"),
                                                     // /*spanish*/la canción del estanque
                                                    CustomMessage("a blue spark trail", /*german*/"ein blauer Komet", /*french*/"une comète bleue"),
                                                     // /*spanish*/el sendero zafiro
                                                    CustomMessage("the lake's lyric", /*german*/"die Lyrik des Sees", /*french*/"la voix du lac")});
                                                     // /*spanish*/la letra del lago

    hintTextTable[RHT_REQUIEM_OF_SPIRIT] = HintText(CustomMessage("the Requiem of Spirit", /*german*/"Requiem der Geister", /*french*/"le requiem des esprits"),
                                                    // /*spanish*/el Réquiem del espíritu
                                                    {
                                                    CustomMessage("a warp song", /*german*/"ein Teleportationslied", /*french*/"une chanson de téléportation")
                                                     // /*spanish*/una canción de teletransportación
                                                    }, {
                                                    CustomMessage("a song of sandy statues", /*german*/"Lied der sandigen Statuen", /*french*/"la mélodie d'une grande statue"),
                                                     // /*spanish*/la canción de la gran estatua
                                                    CustomMessage("an orange spark trail", /*german*/"ein oranger Komet", /*french*/"une comète orange"),
                                                     // /*spanish*/el sendero ámbar
                                                    CustomMessage("the desert ditty", /*german*/"der Wind der Wüste", /*french*/"le vent du désert")});
                                                     // /*spanish*/la estrofa del desierto

    hintTextTable[RHT_NOCTURNE_OF_SHADOW] = HintText(CustomMessage("the Nocturne of Shadow", /*german*/"Nocturne des Schattens", /*french*/"le nocturne de l'ombre"),
                                                     // /*spanish*/el Nocturno de la sombra
                                                     {
                                                     CustomMessage("a warp song", /*german*/"ein Teleportationslied", /*french*/"une chanson de téléportation")
                                                      // /*spanish*/una canción de teletransportación
                                                     }, {
                                                     CustomMessage("a song of spooky spirits", /*german*/"Lied der spukhaften Geister", /*french*/"une hymne de chair de poule"),
                                                      // /*spanish*/la canción de los espectros
                                                     CustomMessage("a graveyard boogie", /*german*/"ein Friedhofsboogie", /*french*/"un boogie de fantômes"),
                                                      // /*spanish*/una honra fúnebre
                                                     CustomMessage("a haunted hymn", /*german*/"eine Spukhymne", /*french*/"une chanson lugubre"),
                                                      // /*spanish*/una estrofa encantada
                                                     CustomMessage("a purple spark trail", /*german*/"ein violetter Komet", /*french*/"une comète mauve")});
                                                      // /*spanish*/el sendero malva

    hintTextTable[RHT_PRELUDE_OF_LIGHT] = HintText(CustomMessage("the Prelude of Light", /*german*/"Kantate des Lichts", /*french*/"le prélude de la lumière"),
                                                   // /*spanish*/el Preludio de la luz
                                                   {
                                                   CustomMessage("a warp song", /*german*/"ein Teleportationslied", /*french*/"une chanson de téléportation")
                                                    // /*spanish*/una canción de teletransportación
                                                   }, {
                                                   CustomMessage("a luminous prologue melody", /*german*/"eine leuchtende Melodie", /*french*/"une matine illuminée"),
                                                    // /*spanish*/la melodía refulgente
                                                   CustomMessage("a yellow spark trail", /*german*/"ein gelber Komet", /*french*/"une comète jaune"),
                                                    // /*spanish*/el sendero resplandeciente
                                                   CustomMessage("the temple traveler", /*german*/"ein Tempelreisender", /*french*/"un chant de sanctuaire")});
                                                    // /*spanish*/la ruta del templo

    hintTextTable[RHT_DEKU_TREE_MAP] = HintText(CustomMessage("the Deku Tree Map", /*german*/"die Karte des Deku-Baums", /*french*/"la carte de l'Arbre Mojo"),
                                                // /*spanish*/el mapa del Gran Árbol Deku
                                                {
                                                CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                 // /*spanish*/un mapa
                                                }, {
                                                CustomMessage("a mossy atlas", /*german*/"ein moosiger Atlas", /*french*/"un atlas boisé"),
                                                 // /*spanish*/un atlas musgoso
                                                CustomMessage("some mossy blueprints", /*german*/"einige moosige Blaupausen", /*french*/"un plan boisé")});
                                                 // /*spanish*/unos planos musgosos

    hintTextTable[RHT_DODONGOS_CAVERN_MAP] = HintText(CustomMessage("the Dodongo's Cavern Map", /*german*/"die Karte der Dodongo-Höhle", /*french*/"la carte de la Caverne Dodongo"),
                                                      // /*spanish*/el mapa de la Cueva de los Dodongos
                                                      {
                                                      CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                       // /*spanish*/un mapa
                                                      }, {
                                                      CustomMessage("a rocky atlas", /*german*/"eine felsiger Atlas", /*french*/"un atlas rocheux"),
                                                       // /*spanish*/un atlas rocoso
                                                      CustomMessage("some rocky blueprints", /*german*/"einige felsige Blaupausen", /*french*/"un plan rocheux")});
                                                       // /*spanish*/unos planos rocosos

    hintTextTable[RHT_JABU_JABUS_BELLY_MAP] = HintText(CustomMessage("the Jabu-Jabu's Belly Map", /*german*/"die Karte des Jabu-Jabu-Bauchs", /*french*/"la carte de Jabu-Jabu"),
                                                       // /*spanish*/el mapa de la Tripa de Jabu-Jabu
                                                       {
                                                       CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                        // /*spanish*/un mapa
                                                       }, {
                                                       CustomMessage("a fishy atlas", /*german*/"ein fischiger Atlas", /*french*/"un atlas digéré"),
                                                        // /*spanish*/un atlas digesto
                                                       CustomMessage("some fishy blueprints", /*german*/"einige fischige Blaupausen", /*french*/"un plan digéré")});
                                                        // /*spanish*/unos planos digestos

    hintTextTable[RHT_FOREST_TEMPLE_MAP] = HintText(CustomMessage("the Forest Temple Map", /*german*/"die Karte des Waldtempels", /*french*/"la carte du Temple de la Forêt"),
                                                    // /*spanish*/el mapa del Templo del Bosque
                                                    {
                                                    CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                     // /*spanish*/un mapa
                                                    }, {
                                                    CustomMessage("a sylvan atlas", /*german*/"ein waldiger Atlas", /*french*/"un atlas sylvestre"),
                                                     // /*spanish*/un atlas enselvado
                                                    CustomMessage("some sylvan blueprints", /*german*/"einige waldige Blaupausen", /*french*/"un plan sylvestre")});
                                                     // /*spanish*/unos planos enselvados

    hintTextTable[RHT_FIRE_TEMPLE_MAP] = HintText(CustomMessage("the Fire Temple Map", /*german*/"die Karte des Feuertempels", /*french*/"la carte du Temple du Feu"),
                                                  // /*spanish*/el mapa del Templo del Fuego
                                                  {
                                                  CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                   // /*spanish*/un mapa
                                                  }, {
                                                  CustomMessage("a molten atlas", /*german*/"ein geschmolzener Atlas", /*french*/"un atlas fondu"),
                                                   // /*spanish*/un atlas fundido
                                                  CustomMessage("some molten blueprints", /*german*/"einige geschmolzene Blaupausen", /*french*/"un plan fondu")});
                                                   // /*spanish*/unos planos fundidos

    hintTextTable[RHT_WATER_TEMPLE_MAP] = HintText(CustomMessage("the Water Temple Map", /*german*/"die Karte des Wassertempels", /*french*/"la carte du Temple de l'Eau"),
                                                   // /*spanish*/el mapa del Templo del Agua
                                                   {
                                                   CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                    // /*spanish*/un mapa
                                                   }, {
                                                   CustomMessage("a wet atlas", /*german*/"ein nasser Atlas", /*french*/"un atlas humide"),
                                                    // /*spanish*/un atlas mojado
                                                   CustomMessage("some wet blueprints", /*german*/"einige nasse Blaupausen", /*french*/"un plan humide")});
                                                    // /*spanish*/unos planos mojados

    hintTextTable[RHT_SPIRIT_TEMPLE_MAP] = HintText(CustomMessage("the Spirit Temple Map", /*german*/"die Karte des Geistertempels", /*french*/"la carte du Temple de l'Esprit"),
                                                    // /*spanish*/el mapa del Templo del Espíritu
                                                    {
                                                    CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                     // /*spanish*/un mapa
                                                    }, {
                                                    CustomMessage("a sandy atlas", /*german*/"ein sandiger Atlas", /*french*/"un atlas sableux"),
                                                     // /*spanish*/un atlas arenoso
                                                    CustomMessage("some sandy blueprints", /*german*/"einige sandige Blaupausen", /*french*/"un plan sableux")});
                                                     // /*spanish*/unos planos arenosos

    hintTextTable[RHT_SHADOW_TEMPLE_MAP] = HintText(CustomMessage("the Shadow Temple Map", /*german*/"die Karte des Schattentempels", /*french*/"la carte du Temple de l'Ombre"),
                                                    // /*spanish*/el mapa del Templo de las Sombras
                                                    {
                                                    CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                     // /*spanish*/un mapa
                                                    }, {
                                                    CustomMessage("a creepy atlas", /*german*/"eine gruseliger Atlas", /*french*/"un atlas sinistre"),
                                                     // /*spanish*/un atlas siniestra
                                                    CustomMessage("some creepy blueprints", /*german*/"einige gruselige Blaupausen", /*french*/"un plan sinistre")});
                                                     // /*spanish*/unos planos siniestras

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MAP] = HintText(CustomMessage("the Bottom of the Well Map", /*german*/"die Karte des Grund des Brunnens", /*french*/"la carte du fond du Puits"),
                                                         // /*spanish*/el mapa del Fondo del pozo
                                                         {
                                                         CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                          // /*spanish*/un mapa
                                                         }, {
                                                         CustomMessage("a moldy atlas", /*german*/"ein schimmeliger Atlas", /*french*/"un atlas moisi"),
                                                          // /*spanish*/un atlas mohoso
                                                         CustomMessage("some moldy blueprints", /*german*/"einige schimmelige Blaupausen", /*french*/"un plan moisi")});
                                                          // /*spanish*/unos planos mohosos

    hintTextTable[RHT_ICE_CAVERN_MAP] = HintText(CustomMessage("the Ice Cavern Map", /*german*/"die Karte der Eishöhle", /*french*/"la carte de la Caverne Polaire"),
                                                 // /*spanish*/el mapa de la Caverna de hielo
                                                 {
                                                 CustomMessage("a dungeon map", /*german*/"eine Karte", /*french*/"une carte")
                                                  // /*spanish*/un mapa
                                                 }, {
                                                 CustomMessage("a polar atlas", /*german*/"ein polarer Atlas", /*french*/"un atlas polaire"),
                                                  // /*spanish*/un atlas polar
                                                 CustomMessage("some polar blueprints", /*german*/"einige polare Blaupausen", /*french*/"un plan polaire")});
                                                  // /*spanish*/unos planos polars

    hintTextTable[RHT_DEKU_TREE_COMPASS] = HintText(CustomMessage("the Deku Tree Compass", /*german*/"der Kompaß des Deku-Baums", /*french*/"la boussole de l'Arbre Mojo"),
                                                    // /*spanish*/la brújula del Gran Árbol Deku
                                                    {
                                                    CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                     // /*spanish*/una brújula
                                                    }, {
                                                    CustomMessage("a mossy treasure tracker", /*german*/"ein moosiger Schatzfinder", /*french*/"un cherche-trésor boisé"),
                                                     // /*spanish*/un zahorí musgoso
                                                    CustomMessage("a mossy magnetic needle", /*german*/"eine moosige Magnetnadel", /*french*/"une aimant boisée")});
                                                     // /*spanish*/un imán musgoso

    hintTextTable[RHT_DODONGOS_CAVERN_COMPASS] = HintText(CustomMessage("the Dodongo's Cavern Compass", /*german*/"der Kompaß der Dodongo-Höhle", /*french*/"la boussole de la Caverne Dodongo"),
                                                          // /*spanish*/la brújula de la Cueva de los Dodongos
                                                          {
                                                          CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                           // /*spanish*/una brújula
                                                          }, {
                                                          CustomMessage("a rocky treasure tracker", /*german*/"ein felsiger Schatzfinder", /*french*/"un cherche-trésor rocheux"),
                                                           // /*spanish*/un zahorí rocoso
                                                          CustomMessage("a rocky magnetic needle", /*german*/"eine felsige Magnetnadel", /*french*/"une aimant rocheux")});
                                                           // /*spanish*/un imán rocoso

    hintTextTable[RHT_JABU_JABUS_BELLY_COMPASS] = HintText(CustomMessage("the Jabu-Jabu's Belly Compass", /*german*/"der Kompaß des Jabu-Jabu-Bauchs", /*french*/"la boussole de Jabu-Jabu"),
                                                           // /*spanish*/la brújula de la Tripa de Jabu-Jabu
                                                           {
                                                           CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                            // /*spanish*/una brújula
                                                           }, {
                                                           CustomMessage("a fishy treasure tracker", /*german*/"ein fischiger Schatzfinder", /*french*/"un cherche-trésor digéré"),
                                                            // /*spanish*/un zahorí digesto
                                                           CustomMessage("a fishy magnetic needle", /*german*/"eine fischige Magnetnadel", /*french*/"une aimant digéré")});
                                                            // /*spanish*/un imán digesto

    hintTextTable[RHT_FOREST_TEMPLE_COMPASS] = HintText(CustomMessage("the Forest Temple Compass", /*german*/"der Kompaß des Waldtempels", /*french*/"la boussole du Temple de la Forêt"),
                                                        // /*spanish*/la brújula del Templo del Bosque
                                                        {
                                                        CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                         // /*spanish*/una brújula
                                                        }, {
                                                        CustomMessage("a sylvan treasure tracker", /*german*/"ein waldiger Schatzfinder", /*french*/"un cherche-trésor sylvestre"),
                                                         // /*spanish*/un zahorí enselvado
                                                        CustomMessage("a sylvan magnetic needle", /*german*/"eine waldige Magnetnadel", /*french*/"une aimant sylvestre")});
                                                         // /*spanish*/un imán enselvado

    hintTextTable[RHT_FIRE_TEMPLE_COMPASS] = HintText(CustomMessage("the Fire Temple Compass", /*german*/"der Kompaß des Feuertempels", /*french*/"la boussole du Temple du Feu"),
                                                      // /*spanish*/la brújula del Templo del Fuego
                                                      {
                                                      CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                       // /*spanish*/una brújula
                                                      }, {
                                                      CustomMessage("a molten treasure tracker", /*german*/"ein geschmolzener Schatzfinder", /*french*/"un cherche-trésor fondu"),
                                                       // /*spanish*/un zahorí fundido
                                                      CustomMessage("a molten magnetic needle", /*german*/"eine geschmolzene Magnetnadel", /*french*/"une aimant fondu")});
                                                       // /*spanish*/un imán fundido

    hintTextTable[RHT_WATER_TEMPLE_COMPASS] = HintText(CustomMessage("the Water Temple Compass", /*german*/"der Kompaß des Wassertempels", /*french*/"la boussole du Temple de l'Eau"),
                                                       // /*spanish*/la brújula del Templo del Agua
                                                       {
                                                       CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                        // /*spanish*/una brújula
                                                       }, {
                                                       CustomMessage("a wet treasure tracker", /*german*/"ein nasser Schatzfinder", /*french*/"un cherche-trésor humide"),
                                                        // /*spanish*/un zahorí mojado
                                                       CustomMessage("a wet magnetic needle", /*german*/"eine nasse Magnetnadel", /*french*/"une aimant humide")});
                                                        // /*spanish*/un imán mojado

    hintTextTable[RHT_SPIRIT_TEMPLE_COMPASS] = HintText(CustomMessage("the Spirit Temple Compass", /*german*/"der Kompaß des Geistertempels", /*french*/"la boussole du Temple de l'Esprit"),
                                                        // /*spanish*/la brújula del Templo del Espíritu
                                                        {
                                                        CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                         // /*spanish*/una brújula
                                                        }, {
                                                        CustomMessage("a sandy treasure tracker", /*german*/"ein sandiger Schatzfinder", /*french*/"un cherche-trésor sableux"),
                                                         // /*spanish*/un zahorí arenoso
                                                        CustomMessage("a sandy magnetic needle", /*german*/"eine sandige Magnetnadel", /*french*/"une aimant sableux")});
                                                         // /*spanish*/un imán arenoso

    hintTextTable[RHT_SHADOW_TEMPLE_COMPASS] = HintText(CustomMessage("the Shadow Temple Compass", /*german*/"der Kompaß des Schattentempels", /*french*/"la boussole du Temple de l'Ombre"),
                                                        // /*spanish*/la brújula del Templo de las Sombras
                                                        {
                                                        CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                         // /*spanish*/una brújula
                                                        }, {
                                                        CustomMessage("a creepy treasure tracker", /*german*/"ein gruseliger Schatzfinder", /*french*/"un cherche-trésor sinistre"),
                                                         // /*spanish*/un zahorí siniestra
                                                        CustomMessage("a creepy magnetic needle", /*german*/"eine gruselige Magnetnadel", /*french*/"une aimant sinistre")});
                                                         // /*spanish*/un imán siniestra

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_COMPASS] = HintText(CustomMessage("the Bottom of the Well Compass", /*german*/"der Kompaß des Grund des Brunnens", /*french*/"la boussole du fond du Puits"),
                                                             // /*spanish*/la brújula del Fondo del pozo
                                                             {
                                                             CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                              // /*spanish*/una brújula
                                                             }, {
                                                             CustomMessage("a dank treasure tracker", /*german*/"ein feuchter Schatzfinder", /*french*/"un cherche-trésor moisi"),
                                                              // /*spanish*/un zahorí mohoso
                                                             CustomMessage("a dank magnetic needle", /*german*/"eine feuchte Magnetnadel", /*french*/"une aimant moisi")});
                                                              // /*spanish*/un imán mohoso

    hintTextTable[RHT_ICE_CAVERN_COMPASS] = HintText(CustomMessage("the Ice Cavern Compass", /*german*/"der Kompaß der Eishöhle", /*french*/"la Boussole de la Caverne Polaire"),
                                                     // /*spanish*/la brújula de la Caverna de hielo
                                                     {
                                                     CustomMessage("a compass", /*german*/"ein Kompaß", /*french*/"une boussole")
                                                      // /*spanish*/una brújula
                                                     }, {
                                                     CustomMessage("a polar treasure tracker", /*german*/"ein polarer Schatzfinder", /*french*/"un cherche-trésor polaire"),
                                                      // /*spanish*/un zahorí polar
                                                     CustomMessage("a polar magnetic needle", /*german*/"eine polare Magnetnadel", /*french*/"une aimant polaire")});
                                                      // /*spanish*/un imán polar

    hintTextTable[RHT_FOREST_TEMPLE_BOSS_KEY] = HintText(CustomMessage("the Forest Temple Boss Key", /*german*/"der Waldtempel-Master-Schlüssel", /*french*/"la Clé d'Or du Temple de la Forêt"),
                                                         // /*spanish*/la gran llave del Templo del Bosque
                                                         {
                                                         CustomMessage("a boss key", /*german*/"ein Master-Schlüssel", /*french*/"une Clé d'Or")
                                                          // /*spanish*/una gran llave
                                                         }, {
                                                         CustomMessage("a sylvan master of unlocking", /*german*/"ein waldiger Meister des Entschlüsselns", /*french*/"un anti-grosse porte sylvestre"),
                                                          // /*spanish*/la clave enselvada de un jefe
                                                         CustomMessage("a sylvan dungeon's master pass", /*german*/"ein waldiger Dungeon-Meisterpass", /*french*/"une clé maléfique sylvestre")});
                                                          // /*spanish*/el pase maestro enselvado

    hintTextTable[RHT_FIRE_TEMPLE_BOSS_KEY] = HintText(CustomMessage("the Fire Temple Boss Key", /*german*/"der Feuertempel-Master-Schlüssel", /*french*/"la Clé d'Or du Temple du Feu"),
                                                       // /*spanish*/la gran llave del Templo del Fuego
                                                       {
                                                       CustomMessage("a boss key", /*german*/"ein Master-Schlüssel", /*french*/"une Clé d'Or")
                                                        // /*spanish*/una gran llave
                                                       }, {
                                                       CustomMessage("a molten master of unlocking", /*german*/"ein geschmolzener Meister des Entschlüsselns", /*french*/"un anti-grosse porte fondu"),
                                                        // /*spanish*/la clave fundido de un jefe
                                                       CustomMessage("a molten dungeon's master pass", /*german*/"ein geschmolzener Dungeon-Meisterpass", /*french*/"une clé maléfique fondu")});
                                                        // /*spanish*/el pase maestro fundido

    hintTextTable[RHT_WATER_TEMPLE_BOSS_KEY] = HintText(CustomMessage("the Water Temple Boss Key", /*german*/"der Wassertempel-Master-Schlüssel", /*french*/"la Clé d'Or du Temple de l'Eau"),
                                                        // /*spanish*/la gran llave del Templo del Agua
                                                        {
                                                        CustomMessage("a boss key", /*german*/"ein Master-Schlüssel", /*french*/"une Clé d'Or")
                                                         // /*spanish*/una gran llave
                                                        }, {
                                                        CustomMessage("a wet master of unlocking", /*german*/"ein nasser Meister des Entschlüsselns", /*french*/"un anti-grosse porte humide"),
                                                         // /*spanish*/la clave mojado de un jefe
                                                        CustomMessage("a wet dungeon's master pass", /*german*/"ein nasser Dungeon-Meisterpass", /*french*/"une clé maléfique humide")});
                                                         // /*spanish*/el pase maestro mojado

    hintTextTable[RHT_SPIRIT_TEMPLE_BOSS_KEY] = HintText(CustomMessage("the Spirit Temple Boss Key", /*german*/"der Geistertempel-Master-Schlüssel", /*french*/"la Clé d'Or du Temple de l'Esprit"),
                                                         // /*spanish*/la gran llave del Templo del Espíritu
                                                         {
                                                         CustomMessage("a boss key", /*german*/"ein Master-Schlüssel", /*french*/"une Clé d'Or")
                                                          // /*spanish*/una gran llave
                                                         }, {
                                                         CustomMessage("a sandy master of unlocking", /*german*/"ein sandiger Meister des Entschlüsselns", /*french*/"un anti-grosse porte sableux"),
                                                          // /*spanish*/la clave arenoso de un jefe
                                                         CustomMessage("a sandy dungeon's master pass", /*german*/"ein sandiger Dungeon-Meisterpass", /*french*/"une clé maléfique sableux")});
                                                          // /*spanish*/el pase maestro arenoso

    hintTextTable[RHT_SHADOW_TEMPLE_BOSS_KEY] = HintText(CustomMessage("the Shadow Temple Boss Key", /*german*/"der Schattentempel-Master-Schlüssel", /*french*/"la Clé d'Or du Temple de l'Ombre"),
                                                         // /*spanish*/la gran llave del Templo de las Sombras
                                                         {
                                                         CustomMessage("a boss key", /*german*/"ein Master-Schlüssel", /*french*/"une Clé d'Or")
                                                          // /*spanish*/una gran llave
                                                         }, {
                                                         CustomMessage("a creepy master of unlocking", /*german*/"ein gruseliger Meister des Entschlüsselns", /*french*/"un anti-grosse porte sinistre"),
                                                          // /*spanish*/la clave siniestra de un jefe
                                                         CustomMessage("a creepy dungeon's master pass", /*german*/"ein gruseliger Dungeon-Meisterpass", /*french*/"une clé maléfique sinistre")});
                                                          // /*spanish*/el pase maestro siniestra

    hintTextTable[RHT_GANONS_CASTLE_BOSS_KEY] = HintText(CustomMessage("the Ganon's Castle Boss Key", /*german*/"der Master-Schlüssel für Ganons Schloß", /*french*/"la Clé d'Or du Château de Ganon"),
                                                         // /*spanish*/la gran llave del Castillo de Ganon
                                                         {
                                                         CustomMessage("a boss key", /*german*/"ein Master-Schlüssel", /*french*/"une Clé d'Or")
                                                          // /*spanish*/una gran llave
                                                         }, {
                                                         CustomMessage("a final master of unlocking", /*german*/"ein finaler Meister des Entschlüsselns", /*french*/"un anti-grosse porte final"),
                                                          // /*spanish*/la clave final de un jefe
                                                         CustomMessage("a final dungeon's master pass", /*german*/"ein finaler Dungeon-Meisterpass", /*french*/"une clé maléfique final")});
                                                          // /*spanish*/el pase maestro final

    hintTextTable[RHT_FOREST_TEMPLE_SMALL_KEY] = HintText(CustomMessage("a Forest Temple Small Key", /*german*/"ein kleiner Waldtempel-Schlüssel", /*french*/"une petite clé du Temple de la Forêt"),
                                                          // /*spanish*/una llave pequeña del Templo del Bosque
                                                          {
                                                          CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessage("a sylvan tool for unlocking", /*german*/"ein waldiges Werkzeug zur Entschlüsselung", /*french*/"un anti-porte sylvestre"),
                                                           // /*spanish*/una clave de una entrada enselvada
                                                          CustomMessage("a sylvan dungeon pass", /*german*/"ein waldiger Dungeon-Pass", /*french*/"le rêve sylvestre d'un prisonnier"),
                                                           // /*spanish*/un pase de una mazmorra enselvada
                                                          CustomMessage("a sylvan lock remover", /*german*/"ein waldiger Schlossentferner", /*french*/"un efface-serrure sylvestre"),
                                                           // /*spanish*/un destructor de cerraduras enselvada
                                                          CustomMessage("a sylvan lockpick", /*german*/"ein waldiger Dietrich", /*french*/"un crochet à porte sylvestre")});
                                                           // /*spanish*/una apertura portentosa enselvada

    hintTextTable[RHT_FIRE_TEMPLE_SMALL_KEY] = HintText(CustomMessage("a Fire Temple Small Key", /*german*/"ein kleiner Feuertempel-Schlüssel", /*french*/"une petite clé du Temple du Feu"),
                                                        // /*spanish*/una llave pequeña del Templo del Fuego
                                                        {
                                                        CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                         // /*spanish*/una llave pequeña
                                                        }, {
                                                        CustomMessage("a molten tool for unlocking", /*german*/"ein geschmolzenes Werkzeug zur Entschlüsselung", /*french*/"un anti-porte fondu"),
                                                         // /*spanish*/una clave de una entrada fundida
                                                        CustomMessage("a molten dungeon pass", /*german*/"ein geschmolzener Dungeon-Pass", /*french*/"le rêve fondu d'un prisonnier"),
                                                         // /*spanish*/un pase de una mazmorra fundida
                                                        CustomMessage("a molten lock remover", /*german*/"ein geschmolzener Schlossentferner", /*french*/"un efface-serrure fondu"),
                                                         // /*spanish*/un destructor de cerraduras fundida
                                                        CustomMessage("a molten lockpick", /*german*/"ein geschmolzener Dietrich", /*french*/"un crochet à porte fondu")});
                                                         // /*spanish*/una apertura portentosa fundida

    hintTextTable[RHT_WATER_TEMPLE_SMALL_KEY] = HintText(CustomMessage("a Water Temple Small Key", /*german*/"ein kleiner Wassertempel-Schlüssel", /*french*/"une petite clé du Temple de l'Eau"),
                                                         // /*spanish*/una llave pequeña del Templo del Agua
                                                         {
                                                         CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                          // /*spanish*/una llave pequeña
                                                         }, {
                                                         CustomMessage("a wet tool for unlocking", /*german*/"ein nasses Werkzeug zur Entschlüsselung", /*french*/"un anti-porte humide"),
                                                          // /*spanish*/una clave de una entrada mojada
                                                         CustomMessage("a wet dungeon pass", /*german*/"ein nasser Dungeon-Pass", /*french*/"le rêve humide d'un prisonnier"),
                                                          // /*spanish*/un pase de una mazmorra mojada
                                                         CustomMessage("a wet lock remover", /*german*/"ein nasser Schlossentferner", /*french*/"un efface-serrure humide"),
                                                          // /*spanish*/un destructor de cerraduras mojada
                                                         CustomMessage("a wet lockpick", /*german*/"ein nasser Dietrich", /*french*/"un crochet à porte humide")});
                                                          // /*spanish*/una apertura portentosa mojada

    hintTextTable[RHT_SPIRIT_TEMPLE_SMALL_KEY] = HintText(CustomMessage("a Spirit Temple Small Key", /*german*/"ein kleiner Geistertempel-Schlüssel", /*french*/"une petite clé du Temple de l'Esprit"),
                                                          // /*spanish*/una llave pequeña del Templo del Espíritu
                                                          {
                                                          CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessage("a sandy tool for unlocking", /*german*/"ein sandiges Werkzeug zur Entschlüsselung", /*french*/"un anti-porte sableux"),
                                                           // /*spanish*/una clave de una entrada arenosa
                                                          CustomMessage("a sandy dungeon pass", /*german*/"ein sandiger Dungeon-Pass", /*french*/"le rêve sableux d'un prisonnier"),
                                                           // /*spanish*/un pase de una mazmorra arenosa
                                                          CustomMessage("a sandy lock remover", /*german*/"ein sandiger Schlossentferner", /*french*/"un efface-serrure sableux"),
                                                           // /*spanish*/un destructor de cerraduras arenosa
                                                          CustomMessage("a sandy lockpick", /*german*/"ein sandiger Dietrich", /*french*/"un crochet à porte sableux")});
                                                           // /*spanish*/una apertura portentosa arenosa

    hintTextTable[RHT_SHADOW_TEMPLE_SMALL_KEY] = HintText(CustomMessage("a Shadow Temple Small Key", /*german*/"ein kleiner Schattentempel-Schlüssel", /*french*/"une petite clé du Temple de l'Ombre"),
                                                          // /*spanish*/una llave pequeña del Templo de las Sombras
                                                          {
                                                          CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessage("a creepy tool for unlocking", /*german*/"ein gruseliges Werkzeug zur Entschlüsselung", /*french*/"un anti-porte sinistre"),
                                                           // /*spanish*/una clave de una entrada siniestra:a
                                                          CustomMessage("a creepy dungeon pass", /*german*/"ein gruseliger Dungeon-Pass", /*french*/"le rêve sinistre d'un prisonnier"),
                                                           // /*spanish*/un pase de una mazmorra siniestra:a
                                                          CustomMessage("a creepy lock remover", /*german*/"ein gruseliger Schlossentferner", /*french*/"un efface-serrure sinistre"),
                                                           // /*spanish*/un destructor de cerraduras siniestra:a
                                                          CustomMessage("a creepy lockpick", /*german*/"ein gruseliger Dietrich", /*french*/"un crochet à porte sinistre")});
                                                           // /*spanish*/una apertura portentosa siniestra:a

    hintTextTable[RHT_GERUDO_TRAINING_GROUNDS_SMALL_KEY] = HintText(CustomMessage("a Gerudo Training Ground Small Key", /*german*/"ein kleiner Schlüssel des Gerudo-Trainingsgeländes", /*french*/"une petite clé du Gymnase Gerudo"),
                                                                    // /*spanish*/una llave pequeña del Centro de Instrucción Gerudo
                                                                    {
                                                                    CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                                     // /*spanish*/una llave pequeña
                                                                    }, {
                                                                    CustomMessage("a labyrinthian tool for unlocking", /*german*/"ein labyrinthisches Werkzeug zur Entschlüsselung", /*french*/"un anti-porte labyrinthique"),
                                                                     // /*spanish*/una clave de una entrada laberíntica
                                                                    CustomMessage("a labyrinthian dungeon pass", /*german*/"ein labyrinthischer Dungeon-Pass", /*french*/"le rêve labyrinthique d'un prisonnier"),
                                                                     // /*spanish*/un pase de una mazmorra laberíntica
                                                                    CustomMessage("a labyrinthian lock remover", /*german*/"ein labyrinthischer Schlossentferner", /*french*/"un efface-serrure labyrinthique"),
                                                                     // /*spanish*/un destructor de cerraduras laberíntica
                                                                    CustomMessage("a labyrinthian lockpick", /*german*/"ein labyrinthischer Dietrich", /*french*/"un crochet à porte labyrinthique")});
                                                                     // /*spanish*/una apertura portentosa laberíntica

    hintTextTable[RHT_GERUDO_FORTRESS_SMALL_KEY] = HintText(CustomMessage("a Gerudo Fortress Small Key", /*german*/"ein kleiner Schlüssel für die Gerudo-Festung", /*french*/"une petite clé de la Repaire des Voleurs"),
                                                            // /*spanish*/una llave pequeña de la Fortaleza Gerudo
                                                            {
                                                            CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                             // /*spanish*/una llave pequeña
                                                            }, {
                                                            CustomMessage("an imprisoned tool for unlocking", /*german*/"ein gefangenes Werkzeug zur Entschlüsselung", /*french*/"un anti-porte emprisonné"),
                                                             // /*spanish*/una clave de una entrada encarcelada
                                                            CustomMessage("an imprisoned dungeon pass", /*german*/"ein gefangener Dungeon-Pass", /*french*/"le rêve emprisonné d'un prisonnier"),
                                                             // /*spanish*/un pase de una mazmorra encarcelada
                                                            CustomMessage("an imprisoned lock remover", /*german*/"ein gefangener Schlossentferner", /*french*/"un efface-serrure emprisonné"),
                                                             // /*spanish*/un destructor de cerraduras encarcelada
                                                            CustomMessage("an imprisoned lockpick", /*german*/"ein gefangener Dietrich", /*french*/"un crochet à porte emprisonné")});
                                                             // /*spanish*/una apertura portentosa encarcelada

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_SMALL_KEY] = HintText(CustomMessage("a Bottom of the Well Small Key", /*german*/"ein kleiner Schlüssel des Grund des Brunnens", /*french*/"une petite clé du fond du Puits"),
                                                               // /*spanish*/una llave pequeña del Fondo del pozo
                                                               {
                                                               CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                                // /*spanish*/una llave pequeña
                                                               }, {
                                                               CustomMessage("a moldy tool for unlocking", /*german*/"ein schimmeliges Werkzeug zur Entschlüsselung", /*french*/"un anti-porte moisi"),
                                                                // /*spanish*/una clave de una entrada mohosa
                                                               CustomMessage("a moldy dungeon pass", /*german*/"ein schimmeliger Dungeon-Pass", /*french*/"le rêve moisi d'un prisonnier"),
                                                                // /*spanish*/un pase de una mazmorra mohosa
                                                               CustomMessage("a moldy lock remover", /*german*/"ein schimmeliger Schlossentferner", /*french*/"un efface-serrure moisi"),
                                                                // /*spanish*/un destructor de cerraduras mohosa
                                                               CustomMessage("a moldy lockpick", /*german*/"ein schimmeliger Dietrich", /*french*/"un crochet à porte moisi")});
                                                                // /*spanish*/una apertura portentosa mohosa

    hintTextTable[RHT_GANONS_CASTLE_SMALL_KEY] = HintText(CustomMessage("a Ganon's Castle Small Key", /*german*/"ein kleiner Schlüssel für Ganons Schloß", /*french*/"une petite clé du Château de Ganon"),
                                                          // /*spanish*/una llave pequeña del Castillo de Ganon
                                                          {
                                                          CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessage("a final tool for unlocking", /*german*/"ein finales Werkzeug zur Entschlüsselung", /*french*/"un anti-porte final"),
                                                           // /*spanish*/una clave de una entrada final
                                                          CustomMessage("a final dungeon pass", /*german*/"ein finaler Dungeon-Pass", /*french*/"le rêve final d'un prisonnier"),
                                                           // /*spanish*/un pase de una mazmorra final
                                                          CustomMessage("a final lock remover", /*german*/"ein finaler Schlossentferner", /*french*/"un efface-serrure final"),
                                                           // /*spanish*/un destructor de cerraduras final
                                                          CustomMessage("a final lockpick", /*german*/"ein finaler Dietrich", /*french*/"un crochet à porte final")});
                                                           // /*spanish*/una apertura portentosa final

    hintTextTable[RHT_FOREST_TEMPLE_KEY_RING] = HintText(CustomMessage("a Forest Temple Key Ring", /*german*/"ein Schlüsselbund des Waldtempels", /*french*/"un trousseau de clés du Temple de la Forêt"),
                                                         // /*spanish*/un llavero del Templo del Bosque
                                                         {
                                                         CustomMessage("a key ring", /*german*/"ein Schlüsselbund", /*french*/"un trousseau de clés")
                                                          // /*spanish*/un llavero
                                                         }, {
                                                         CustomMessage("a sylvan toolbox for unlocking", /*german*/"eine waldige Werkzeugkiste zur Entschlüsselung", /*french*/"des anti-portes sylvestres"),
                                                          // /*spanish*/un conjunto silvestre de cerrajero
                                                         CustomMessage("a sylvan dungeon season pass", /*german*/"ein waldiger Dungeon-Season-Pass", /*french*/"les rêves sylvestres d'un prisonnier"),
                                                          // /*spanish*/un pase vip de mazmorras silvestre
                                                         CustomMessage("a sylvan jingling ring", /*german*/"ein waldiger Multifunktionsschlüssel", /*french*/"des efface-serrures sylvestres"),
                                                          // /*spanish*/una cadena multiusos silvestre
                                                         CustomMessage("a sylvan skeleton key", /*german*/"ein waldiger Skelettschlüssel", /*french*/"des crochets à porte sylvestres")});
                                                          // /*spanish*/un anillo silvestre contra cerrojos

    hintTextTable[RHT_FIRE_TEMPLE_KEY_RING] = HintText(CustomMessage("a Fire Temple Key Ring", /*german*/"ein Schlüsselbund des Feuertempels", /*french*/"un trousseau de clés du Temple du Feu"),
                                                       // /*spanish*/un llavero del Templo del Fuego
                                                       {
                                                       CustomMessage("a key ring", /*german*/"ein Schlüsselbund", /*french*/"un trousseau de clés")
                                                        // /*spanish*/un llavero
                                                       }, {
                                                       CustomMessage("a molten toolbox for unlocking", /*german*/"eine geschmolzene Werkzeugkiste zur Entschlüsselung", /*french*/"des anti-portes fondus"),
                                                        // /*spanish*/un conjunto fundido de cerrajero
                                                       CustomMessage("a molten dungeon season pass", /*german*/"ein geschmolzener Dungeon-Season-Pass", /*french*/"les rêves fondus d'un prisonnier"),
                                                        // /*spanish*/un pase vip de mazmorras fundido
                                                       CustomMessage("a molten jingling ring", /*german*/"ein geschmolzener Multifunktionsschlüssel", /*french*/"des efface-serrures fondus"),
                                                        // /*spanish*/una cadena multiusos fundida
                                                       CustomMessage("a molten skeleton key", /*german*/"ein geschmolzener Skelettschlüssel", /*french*/"des crochets à porte fondus")});
                                                        // /*spanish*/un anillo fundido contra cerrojos

    hintTextTable[RHT_WATER_TEMPLE_KEY_RING] = HintText(CustomMessage("a Water Temple Key Ring", /*german*/"ein Schlüsselbund des Wassertempels", /*french*/"un trousseau de clés du Temple de l'Eau"),
                                                        // /*spanish*/un llavero del Templo del Agua
                                                        {
                                                        CustomMessage("a key ring", /*german*/"ein Schlüsselbund", /*french*/"un trousseau de clés")
                                                         // /*spanish*/un llavero
                                                        }, {
                                                        CustomMessage("a wet toolbox for unlocking", /*german*/"eine nasse Werkzeugkiste zur Entschlüsselung", /*french*/"des anti-portes humides"),
                                                         // /*spanish*/un conjunto abisal de cerrajero
                                                        CustomMessage("a wet dungeon season pass", /*german*/"ein nasser Dungeon-Season-Pass", /*french*/"les rêves humides d'un prisonnier"),
                                                         // /*spanish*/un pase vip de mazmorras abisal
                                                        CustomMessage("a wet jingling ring", /*german*/"ein nasser Multifunktionsschlüssel", /*french*/"des efface-serrures humides"),
                                                         // /*spanish*/una cadena multiusos abisal
                                                        CustomMessage("a wet skeleton key", /*german*/"ein nasser Skelettschlüssel", /*french*/"des crochets à porte humides")});
                                                         // /*spanish*/un anillo abisal contra cerrojos

    hintTextTable[RHT_SPIRIT_TEMPLE_KEY_RING] = HintText(CustomMessage("a Spirit Temple Key Ring", /*german*/"ein Schlüsselbund des Geistertempels", /*french*/"un trousseau de clés du Temple de l'Esprit"),
                                                         // /*spanish*/un llavero del Templo del Espíritu
                                                         {
                                                         CustomMessage("a key ring", /*german*/"ein Schlüsselbund", /*french*/"un trousseau de clés")
                                                          // /*spanish*/un llavero
                                                         }, {
                                                         CustomMessage("a sandy toolbox for unlocking", /*german*/"eine sandige Werkzeugkiste zur Entschlüsselung", /*french*/"des anti-portes sableux"),
                                                          // /*spanish*/un conjunto arenoso de cerrajero
                                                         CustomMessage("a sandy dungeon season pass", /*german*/"ein sandiger Dungeon-Season-Pass", /*french*/"les rêves sableux d'un prisonnier"),
                                                          // /*spanish*/un pase vip de mazmorras arenoso
                                                         CustomMessage("a sandy jingling ring", /*german*/"ein sandiger Multifunktionsschlüssel", /*french*/"des efface-serrures sableux"),
                                                          // /*spanish*/una cadena multiusos arenosa
                                                         CustomMessage("a sandy skeleton key", /*german*/"ein sandiger Skelettschlüssel", /*french*/"des crochets à porte sableux")});
                                                          // /*spanish*/un anillo arenoso contra cerrojos

    hintTextTable[RHT_SHADOW_TEMPLE_KEY_RING] = HintText(CustomMessage("a Shadow Temple Key Ring", /*german*/"ein Schlüsselbund des Schattentempels", /*french*/"un trousseau de clés du Temple de l'Ombre"),
                                                         // /*spanish*/un llavero del Templo de las Sombras
                                                         {
                                                         CustomMessage("a key ring", /*german*/"ein Schlüsselbund", /*french*/"un trousseau de clés")
                                                          // /*spanish*/un llavero
                                                         }, {
                                                         CustomMessage("a creepy toolbox for unlocking", /*german*/"eine gruselige Werkzeugkiste zur Entschlüsselung", /*french*/"des anti-portes sinistres"),
                                                          // /*spanish*/un conjunto tenebroso de cerrajero
                                                         CustomMessage("a creepy dungeon season pass", /*german*/"ein gruseliger Dungeon-Season-Pass", /*french*/"les rêves sinistres d'un prisonnier"),
                                                          // /*spanish*/un pase vip de mazmorras tenebroso
                                                         CustomMessage("a creepy jingling ring", /*german*/"ein gruseliger Multifunktionsschlüssel", /*french*/"des efface-serrures sinistres"),
                                                          // /*spanish*/una cadena multiusos tenebrosa
                                                         CustomMessage("a creepy skeleton key", /*german*/"ein gruseliger Skelettschlüssel", /*french*/"des crochets à porte sinistres")});
                                                          // /*spanish*/un anillo tenebroso contra cerrojos

    hintTextTable[RHT_GERUDO_TRAINING_GROUNDS_KEY_RING] = HintText(CustomMessage("a Gerudo Training Ground Key Ring", /*german*/"ein Schlüsselbund des Gerudo-Trainingsgeländes", /*french*/"un trousseau de clés du Gymnase Gerudo"),
                                                                   // /*spanish*/un llavero del Centro de Instrucción Gerudo
                                                                   {
                                                                   CustomMessage("a key ring", /*german*/"ein Schlüsselbund", /*french*/"un trousseau de clés")
                                                                    // /*spanish*/un llavero
                                                                   }, {
                                                                   CustomMessage("a labyrinthian toolbox for unlocking", /*german*/"eine labyrinthische Werkzeugkiste zur Entschlüsselung", /*french*/"des anti-portes labyrinthiques"),
                                                                    // /*spanish*/un conjunto laberíntico de cerrajero
                                                                   CustomMessage("a labyrinthian dungeon season pass", /*german*/"ein labyrinthischer Dungeon-Season-Pass", /*french*/"les rêves labyrinthiques d'un prisonnier"),
                                                                    // /*spanish*/un pase vip de mazmorras laberíntico
                                                                   CustomMessage("a labyrinthian jingling ring", /*german*/"ein labyrinthischer Multifunktionsschlüssel", /*french*/"des efface-serrures labyrinthiques"),
                                                                    // /*spanish*/una cadena multiusos laberíntica
                                                                   CustomMessage("a labyrinthian skeleton key", /*german*/"ein labyrinthischer Skelettschlüssel", /*french*/"des crochets à porte labyrinthiques")});
                                                                    // /*spanish*/un anillo laberíntico contra cerrojos

    hintTextTable[RHT_GERUDO_FORTRESS_KEY_RING] = HintText(CustomMessage("a Gerudo Fortress Key Ring", /*german*/"ein Schlüsselbund der Gerudo-Festung", /*french*/"un trousseau de clés de la Repaire des Voleurs"),
                                                           // /*spanish*/un llavero de la Fortaleza Gerudo
                                                           {
                                                           CustomMessage("a key ring", /*german*/"ein Schlüsselbund", /*french*/"un trousseau de clés")
                                                            // /*spanish*/un llavero
                                                           }, {
                                                           CustomMessage("an imprisoned toolbox for unlocking", /*german*/"eine gefangene Werkzeugkiste zur Entschlüsselung", /*french*/"des anti-portes emprisonnés"),
                                                            // /*spanish*/un conjunto enjaulado de cerrajero
                                                           CustomMessage("an imprisoned dungeon season pass", /*german*/"ein gefangener Dungeon-Season-Pass", /*french*/"les rêves emprisonnés d'un prisonnier"),
                                                            // /*spanish*/un pase vip de una mazmorra enjaulado
                                                           CustomMessage("an imprisoned jingling ring", /*german*/"ein gefangener Multifunktionsschlüssel", /*french*/"des efface-serrures emprisonnés"),
                                                            // /*spanish*/una cadena multiusos enjaulada
                                                           CustomMessage("an imprisoned skeleton key", /*german*/"ein gefangener Skelettschlüssel", /*french*/"des crochets à porte emprisonnés")});
                                                            // /*spanish*/un anillo enjaulado contra cerrojos

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_KEY_RING] = HintText(CustomMessage("a Bottom of the Well Key Ring", /*german*/"ein Schlüsselbund des Grund des Brunnens", /*french*/"un trousseau de clés du fond du Puits"),
                                                              // /*spanish*/un llavero del Fondo del pozo
                                                              {
                                                              CustomMessage("a key ring", /*german*/"ein Schlüsselbund", /*french*/"un trousseau de clés")
                                                               // /*spanish*/un llavero
                                                              }, {
                                                              CustomMessage("a moldy toolbox for unlocking", /*german*/"eine schimmelige Werkzeugkiste zur Entschlüsselung", /*french*/"des anti-portes moisis"),
                                                               // /*spanish*/un conjunto subterráneo de cerrajero
                                                              CustomMessage("a moldy dungeon season pass", /*german*/"ein schimmeliger Dungeon-Season-Pass", /*french*/"les rêves moisis d'un prisonnier"),
                                                               // /*spanish*/un pase vip de una mazmorra subterráneo
                                                              CustomMessage("a moldy jingling ring", /*german*/"ein schimmeliger Multifunktionsschlüssel", /*french*/"des efface-serrures moisis"),
                                                               // /*spanish*/una cadena multiusos subterránea
                                                              CustomMessage("a moldy skeleton key", /*german*/"ein schimmeliger Skelettschlüssel", /*french*/"des crochets à porte moisis")});
                                                               // /*spanish*/un anillo subterráneo contra cerrojos

    hintTextTable[RHT_GANONS_CASTLE_KEY_RING] = HintText(CustomMessage("a Ganon's Castle Key Ring", /*german*/"ein Schlüsselbund von Ganons Schloß", /*french*/"un trousseau de clés du Château de Ganon"),
                                                         // /*spanish*/un llavero del Castillo de Ganon
                                                         {
                                                         CustomMessage("a key ring", /*german*/"ein Schlüsselbund", /*french*/"un trousseau de clés")
                                                          // /*spanish*/un llavero
                                                         }, {
                                                         CustomMessage("a final toolbox for unlocking", /*german*/"eine finale Werkzeugkiste zur Entschlüsselung", /*french*/"des anti-portes finaux"),
                                                          // /*spanish*/un conjunto decisivo de cerrajero
                                                         CustomMessage("a final dungeon season pass", /*german*/"ein finaler Dungeon-Season-Pass", /*french*/"les rêves finaux d'un prisonnier"),
                                                          // /*spanish*/un pase vip de una mazmorra decisivo
                                                         CustomMessage("a final jingling ring", /*german*/"ein finaler Multifunktionsschlüssel", /*french*/"des efface-serrures finaux"),
                                                          // /*spanish*/una cadena multiusos decisiva
                                                         CustomMessage("a final skeleton key", /*german*/"ein finaler Skelettschlüssel", /*french*/"des crochets à porte finaux")});
                                                          // /*spanish*/un anillo decisivo multiusos

    hintTextTable[RHT_TREASURE_GAME_SMALL_KEY] = HintText(CustomMessage("a Treasure Chest Shop Small Key", /*german*/"ein kleiner Schlüssel des Schatztruhenladens", /*french*/"une petite clé de la chasse aux trésors"),
                                                          // /*spanish*/una llave pequeña del Cofre del Tesoro
                                                          {
                                                          CustomMessage("a small key", /*german*/"ein kleiner Schlüssel", /*french*/"une petite clé")
                                                           // /*spanish*/una llave pequeña
                                                          }, {
                                                          CustomMessage("a gambler's tool for unlocking", /*german*/"ein Spieler-Werkzeug zur Entschlüsselung", /*french*/"un anti-porte de parieur"),
                                                           // /*spanish*/una clave de un juego de azar
                                                          CustomMessage("a gambler's dungeon pass", /*german*/"ein Spieler-Dungeon-Pass", /*french*/"le rêve d'un prisonnier parieur"),
                                                           // /*spanish*/un pase de un juego de azar
                                                          CustomMessage("a gambler's lock remover", /*german*/"ein Spieler-Schlossentferner", /*french*/"un efface-serrure de parieur"),
                                                           // /*spanish*/un destructor de cerraduras del juego de azar
                                                          CustomMessage("a gambler's lockpick", /*german*/"ein Spieler-Dietrich", /*french*/"un crochet à serrure de parieur")});
                                                           // /*spanish*/una apertura portentosa del juego de azar

    hintTextTable[RHT_KOKIRI_EMERALD] = HintText(CustomMessage("the Kokiri Emerald", /*german*/"der Kokiri-Smaragd", /*french*/"l'Émeraude Kokiri"),
                                                 // /*spanish*/la Esmeralda de los Kokiri
                                                 {
                                                 CustomMessage("a spiritual stone", /*german*/"ein spiritueller Stein", /*french*/"une Pierre Ancestrale")
                                                  // /*spanish*/una piedra espiritual
                                                 }, {
                                                 CustomMessage("a green stone", /*german*/"ein grüner Stein", /*french*/"une pierre verte"),
                                                  // /*spanish*/una piedra verde
                                                 CustomMessage("a gift before death", /*german*/"ein Geschenk vor dem Tod", /*french*/"le dernier souffle d'un arbre")});
                                                  // /*spanish*/un obsequio testamentario

    hintTextTable[RHT_GORON_RUBY] = HintText(CustomMessage("the Goron Ruby", /*german*/"der Goronen-Rubin", /*french*/"le Rubis Goron"),
                                             // /*spanish*/el Rubí de los Goron
                                             {
                                             CustomMessage("a spiritual stone", /*german*/"ein spiritueller Stein", /*french*/"une Pierre Ancestrale")
                                              // /*spanish*/una piedra espiritual
                                             }, {
                                             CustomMessage("a red stone", /*german*/"ein roter Stein", /*french*/"une pierre rouge"),
                                              // /*spanish*/una piedra carmín
                                             CustomMessage("sworn brotherhood", /*german*/"verschworene Bruderschaft", /*french*/"un serment de fraternité")});
                                              // /*spanish*/el juramento de hermanos de sangre

    hintTextTable[RHT_ZORA_SAPPHIRE] = HintText(CustomMessage("the Zora Sapphire", /*german*/"der Zora-Saphir", /*french*/"le Saphir Zora"),
                                                // /*spanish*/el Zafiro de los Zora
                                                {
                                                CustomMessage("a spiritual stone", /*german*/"ein spiritueller Stein", /*french*/"une Pierre Ancestrale")
                                                 // /*spanish*/una piedra espiritual
                                                }, {
                                                CustomMessage("a blue stone", /*german*/"ein blauer Stein", /*french*/"une pierre bleue"),
                                                 // /*spanish*/una piedra celeste
                                                CustomMessage("an engagement gift", /*german*/"ein Verlobungsgeschenk", /*french*/"un cadeau de mariage")});
                                                 // /*spanish*/un regalo de compromiso

    hintTextTable[RHT_FOREST_MEDALLION] = HintText(CustomMessage("the Forest Medallion", /*german*/"Amulett des Waldes", /*french*/"le Médaillon de la Forêt"),
                                                   // /*spanish*/el Medallón del Bosque
                                                   {
                                                   CustomMessage("a medallion", /*german*/"ein Amulett", /*french*/"un médaillon")
                                                    // /*spanish*/un medallón
                                                   }, {
                                                   CustomMessage("a green coin", /*german*/"eine grüne Münze", /*french*/"une pièce verte"),
                                                    // /*spanish*/una moneda esmeralda
                                                   CustomMessage("Saria's friendship", /*german*/"Salias Freundschaft", /*french*/"l'amitié de Saria")});
                                                    // /*spanish*/la amistad de Saria

    hintTextTable[RHT_FIRE_MEDALLION] = HintText(CustomMessage("the Fire Medallion", /*german*/"Amulett des Feuers", /*french*/"le Médaillon du Feu"),
                                                 // /*spanish*/el Medallón del Fuego
                                                 {
                                                 CustomMessage("a medallion", /*german*/"ein Amulett", /*french*/"un médaillon")
                                                  // /*spanish*/un medallón
                                                 }, {
                                                 CustomMessage("a red coin", /*german*/"eine rote Münze", /*french*/"une pièce rouge"),
                                                  // /*spanish*/una moneda rubí
                                                 CustomMessage("Darunia's power", /*german*/"Darunias Kraft", /*french*/"la fraternité de Darunia")});
                                                  // /*spanish*/la fraternidad de Darunia

    hintTextTable[RHT_WATER_MEDALLION] = HintText(CustomMessage("the Water Medallion", /*german*/"Amulett des Wassers", /*french*/"le Médaillon de l'Eau"),
                                                  // /*spanish*/el Medallón del Agua
                                                  {
                                                  CustomMessage("a medallion", /*german*/"ein Amulett", /*french*/"un médaillon")
                                                   // /*spanish*/un medallón
                                                  }, {
                                                  CustomMessage("a blue coin", /*german*/"eine blaue Münze", /*french*/"une pièce bleue"),
                                                   // /*spanish*/una moneda zafiro
                                                  CustomMessage("Ruto's power", /*german*/"Rutos Macht", /*french*/"l'amour de Ruto")});
                                                   // /*spanish*/el amor de Ruto

    hintTextTable[RHT_SPIRIT_MEDALLION] = HintText(CustomMessage("the Spirit Medallion", /*german*/"Amulett der Geister", /*french*/"le Médaillon de l'Esprit"),
                                                   // /*spanish*/el Medallón del Espíritu
                                                   {
                                                   CustomMessage("a medallion", /*german*/"ein Amulett", /*french*/"un médaillon")
                                                    // /*spanish*/un medallón
                                                   }, {
                                                   CustomMessage("an orange coin", /*german*/"eine orange Münze", /*french*/"une pièce orange"),
                                                    // /*spanish*/una moneda ámbar
                                                   CustomMessage("Nabooru's power", /*german*/"Naborus Macht", /*french*/"le respect de Nabooru")});
                                                    // /*spanish*/el respeto de Nabooru

    hintTextTable[RHT_SHADOW_MEDALLION] = HintText(CustomMessage("the Shadow Medallion", /*german*/"Amulett des Schattens", /*french*/"le Médaillon de l'Ombre"),
                                                   // /*spanish*/el Medallón de la Sombra
                                                   {
                                                   CustomMessage("a medallion", /*german*/"ein Amulett", /*french*/"un médaillon")
                                                    // /*spanish*/un medallón
                                                   }, {
                                                   CustomMessage("a purple coin", /*german*/"eine violette Münze", /*french*/"une pièce pourpre"),
                                                    // /*spanish*/una moneda malva
                                                   CustomMessage("Impa's power", /*german*/"Impas Macht", /*french*/"la confiance d'Impa")});
                                                    // /*spanish*/la confianza de Impa

    hintTextTable[RHT_LIGHT_MEDALLION] = HintText(CustomMessage("the Light Medallion", /*german*/"Amulett des Lichts", /*french*/"le Médaillon de la Lumière"),
                                                  // /*spanish*/el Medallón de la Luz
                                                  {
                                                  CustomMessage("a medallion", /*german*/"ein Amulett", /*french*/"un médaillon")
                                                   // /*spanish*/un medallón
                                                  }, {
                                                  CustomMessage("a yellow coin", /*german*/"eine gelbe Münze", /*french*/"une pièce jaune"),
                                                   // /*spanish*/una moneda resplandeciente
                                                  CustomMessage("Rauru's power", /*german*/"Raurus Macht", /*french*/"la foi de Rauru")});
                                                   // /*spanish*/la fe de Rauru

    hintTextTable[RHT_RECOVERY_HEART] = HintText(CustomMessage("a Recovery Heart", /*german*/"ein Herz", /*french*/"un coeur de vie"),
                                                 // /*spanish*/un corazón
                                                 {
                                                 CustomMessage("something heart-shaped", /*german*/"etwas Herzförmiges", /*french*/"une chose en forme de coeur")
                                                  // /*spanish*/algo con forma de corazón
                                                 }, {
                                                 CustomMessage("a free heal", /*german*/"eine kostenlose Heilung", /*french*/"un bec-au-bobo"),
                                                  // /*spanish*/una cura de regalo
                                                 CustomMessage("a hearty meal", /*german*/"ein herzhaftes Mahl", /*french*/"un petit amour"),
                                                  // /*spanish*/una sanación romántica
                                                 CustomMessage("a Band-Aid", /*german*/"ein Wundpflaster", /*french*/"un diachylon")});
                                                  // /*spanish*/un corazoncito sanador

    hintTextTable[RHT_GREEN_RUPEE] = HintText(CustomMessage("a Green Rupee", /*german*/"ein grüner Rubin", /*french*/"un rubis vert"),
                                              // /*spanish*/una rupia verde
                                              {
                                              CustomMessage("some rupees", /*german*/"einige Rubine", /*french*/"une quantité de rubis")
                                               // /*spanish*/una cantidad de rupias
                                              }, {
                                              CustomMessage("a unique coin", /*german*/"eine einzigartige Münze", /*french*/"un rubis bien mérité"),
                                               // /*spanish*/una singular moneda
                                              CustomMessage("a penny", /*german*/"ein Pfennig", /*french*/"un sou"),
                                               // /*spanish*/un peso hyliano
                                              CustomMessage("a green gem", /*german*/"ein grüner Edelstein", /*french*/"un joyau vert")});
                                               // /*spanish*/una gema verde

    hintTextTable[RHT_GREG_RUPEE] = HintText(CustomMessage("Greg", /*german*/"Greg", /*french*/"Greg"),
                                             // /*spanish*/Greg
                                             {
                                             CustomMessage("a Green Rupee", /*german*/"ein grüner Rubin", /*french*/"un rubis vert")
                                              // /*spanish*/una rupia verde
                                             }, {
                                             CustomMessage("an old friend", /*german*/"ein alter Freund", /*french*/"Greg"),
                                              // /*spanish*/Greg
                                             CustomMessage("a glorious gem", /*german*/"ein glorioser Edelstein", /*french*/"Greg")});
                                              // /*spanish*/Greg

    hintTextTable[RHT_BLUE_RUPEE] = HintText(CustomMessage("a Blue Rupee", /*german*/"ein blauer Rubin", /*french*/"un rubis bleu"),
                                             // /*spanish*/una rupia azul
                                             {
                                             CustomMessage("some rupees", /*german*/"einige Rubine", /*french*/"une quantité de rubis")
                                              // /*spanish*/una cantidad de rupias
                                             }, {
                                             CustomMessage("a common coin", /*german*/"eine gewöhnliche Münze", /*french*/"quelques sous"),
                                              // /*spanish*/una moneda usual
                                             CustomMessage("a blue gem", /*german*/"ein blauer Edelstein", /*french*/"un joyau bleu")});
                                              // /*spanish*/una gema azul

    hintTextTable[RHT_RED_RUPEE] = HintText(CustomMessage("a Red Rupee", /*german*/"ein roter Rubin", /*french*/"un rubis rouge"),
                                            // /*spanish*/una rupia roja
                                            {
                                            CustomMessage("some rupees", /*german*/"einige Rubine", /*french*/"une quantité de rubis")
                                             // /*spanish*/una cantidad de rupias
                                            }, {
                                            CustomMessage("couch cash", /*german*/"ein wenig Zaster", /*french*/"un peu de fric"),
                                             // /*spanish*/una buena moneda
                                            CustomMessage("a red gem", /*german*/"ein roter Edelstein", /*french*/"un joyau rouge")});
                                             // /*spanish*/una gema roja

    hintTextTable[RHT_PURPLE_RUPEE] = HintText(CustomMessage("a Purple Rupee", /*german*/"ein violetter Rubin", /*french*/"un rubis pourpre"),
                                               // /*spanish*/una rupia morada
                                               {
                                               CustomMessage("some rupees", /*german*/"einige Rubine", /*french*/"une quantité de rubis")
                                                // /*spanish*/una cantidad de rupias
                                               }, {
                                               CustomMessage("big bucks", /*german*/"ordentlich Zaster", /*french*/"plein de fric"),
                                                // /*spanish*/plata de calidad
                                               CustomMessage("a purple gem", /*german*/"ein violetter Edelstein", /*french*/"un joyau mauve"),
                                                // /*spanish*/una gema morada
                                               CustomMessage("wealth", /*german*/"Wohlstand", /*french*/"la richesse")});
                                                // /*spanish*/una buena riqueza

    hintTextTable[RHT_HUGE_RUPEE] = HintText(CustomMessage("a Huge Rupee", /*german*/"ein riesiger Rubin", /*french*/"un énorme rubis"),
                                             // /*spanish*/una rupia gigante
                                             {
                                             CustomMessage("some rupees", /*german*/"einige Rubine", /*french*/"une quantité de rubis")
                                              // /*spanish*/una cantidad de rupias
                                             }, {
                                             CustomMessage("a juicy jackpot", /*german*/"ein saftiger Jackpot", /*french*/"le jackpot"),
                                              // /*spanish*/el premio gordo
                                             CustomMessage("a yellow gem", /*german*/"ein gelber Edelstein", /*french*/"un joyau doré"),
                                              // /*spanish*/una gema amarilla
                                             CustomMessage("a giant gem", /*german*/"ein riesiger Edelstein", /*french*/"un gros joyau"),
                                              // /*spanish*/una gema descomunal
                                             CustomMessage("great wealth", /*german*/"großer Wohlstand", /*french*/"l'aisance financière")});
                                              // /*spanish*/dinero a caudales

    hintTextTable[RHT_PIECE_OF_HEART] = HintText(CustomMessage("a Piece of Heart", /*german*/"ein Herzteil", /*french*/"un Quart de Coeur"),
                                                 // /*spanish*/una pieza de corazón
                                                 {
                                                 CustomMessage("something heart-shaped", /*german*/"etwas Herzförmiges", /*french*/"une chose en forme de coeur")
                                                  // /*spanish*/algo con forma de corazón
                                                 }, {
                                                 CustomMessage("a little love", /*german*/"ein wenig Liebe", /*french*/"un peu plus d'amour"),
                                                  // /*spanish*/un cuarto de amor
                                                 CustomMessage("a broken heart", /*german*/"ein gebrochenes Herz", /*french*/"un coeur brisé")});
                                                  // /*spanish*/un corazón roto

    hintTextTable[RHT_HEART_CONTAINER] = HintText(CustomMessage("a Heart Container", /*german*/"ein Herzcontainer", /*french*/"un Réceptacle de Coeur"),
                                                  // /*spanish*/un contenedor de corazón
                                                  {
                                                  CustomMessage("something heart-shaped", /*german*/"etwas Herzförmiges", /*french*/"une chose en forme de coeur")
                                                   // /*spanish*/algo con forma de corazón
                                                  }, {
                                                  CustomMessage("a lot of love", /*german*/"eine Menge Liebe", /*french*/"le grand amour"),
                                                   // /*spanish*/amor por doquier
                                                  CustomMessage("a Valentine's gift", /*german*/"ein Valentinsgeschenk", /*french*/"un cadeau de Saint-Valentin"),
                                                   // /*spanish*/un contenedor de afección
                                                  CustomMessage("a boss's organ", /*german*/"ein monströses Organ", /*french*/"un organe de monstre")});
                                                   // /*spanish*/los órganos de un jefe

    hintTextTable[RHT_ICE_TRAP] = HintText(CustomMessage("an Ice Trap", /*german*/"eine Eisfalle", /*french*/"un Piège de Glace"),
                                           // /*spanish*/una trampa de hielo
                                           {
                                           CustomMessage("a Great Fairy's power", /*german*/"eine Kraft einer großen Fee", /*french*/"le pouvoir d'une grande fée"),
                                            // /*spanish*/el poder de una Gran Hada
                                           CustomMessage("a magic arrow", /*german*/"ein magischer Pfeil", /*french*/"une flèche magique"),
                                            // /*spanish*/una flecha mágica
                                           CustomMessage("a medallion", /*german*/"ein Amulett", /*french*/"un médaillon"),
                                            // /*spanish*/un medallón
                                           CustomMessage("a spiritual stone", /*german*/"ein spiritueller Stein", /*french*/"une Pierre Ancestrale"),
                                            // /*spanish*/una piedra espiritual
                                           CustomMessage("something that can stun", /*german*/"etwas, das paralysieren kann", /*french*/"une chose qui peut paralyser")
                                            // /*spanish*/algo que pueda paralizar
                                           }, {
                                           CustomMessage("a gift from Ganon", /*german*/"ein Geschenk von Ganon", /*french*/"un cadeau de Ganon"),
                                            // /*spanish*/un regalo de Ganon
                                           CustomMessage("a chilling discovery", /*german*/"eine fröstelnde Entdeckung", /*french*/"une frissonante découverte"),
                                            // /*spanish*/un escalofriante hallazgo
                                           CustomMessage("frosty fun", /*german*/"frostiger Spaß", /*french*/"une engelure")});
                                            // /*spanish*/una gélida diversión

    hintTextTable[RHT_BOMBS_5] = HintText(CustomMessage("Bombs (5 pieces)", /*german*/"Bomben (5 Stück)", /*french*/"une demi-dizaine de bombes"),
                                          // /*spanish*/unas (5) bombas
                                          {
                                          CustomMessage("explosives", /*german*/"Explosivpakete", /*french*/"un paquet d'explosifs")
                                           // /*spanish*/un montón de explosivos
                                          }, {
                                          CustomMessage("a few explosives", /*german*/"ein paar Explosivbehälter", /*french*/"une poignée de pétards"),
                                           // /*spanish*/un par de explosivos
                                          CustomMessage("a few blast balls", /*german*/"ein paar Explosionsbälle", /*french*/"une poignée de boules bleues")});
                                           // /*spanish*/un par de estallidos

    hintTextTable[RHT_BOMBS_10] = HintText(CustomMessage("Bombs (10 pieces)", /*german*/"Bomben (10 Stück)", /*french*/"une dizaine de bombes"),
                                           // /*spanish*/unas (10) bombas
                                           {
                                           CustomMessage("explosives", /*german*/"Explosivpakete", /*french*/"un paquet d'explosifs")
                                            // /*spanish*/un montón de explosivos
                                           }, {
                                           CustomMessage("some explosives", /*german*/"einige Explosivbehälter", /*french*/"un paquet de pétards"),
                                            // /*spanish*/unos cuantos explosivos
                                           CustomMessage("some blast balls", /*german*/"einige Explosionsbälle", /*french*/"un paquet de boules bleues")});
                                            // /*spanish*/unos cuantos estallidos

    hintTextTable[RHT_BOMBS_20] = HintText(CustomMessage("Bombs (20 pieces)", /*german*/"Bomben (20 Stück)", /*french*/"une vingtaine de bombes"),
                                           // /*spanish*/unas (20) bombas
                                           {
                                           CustomMessage("explosives", /*german*/"Explosivpakete", /*french*/"un paquet d'explosifs")
                                            // /*spanish*/un montón de explosivos
                                           }, {
                                           CustomMessage("lots-o-explosives", /*german*/"viele Explosivbehälter", /*french*/"une abondance de pétards"),
                                            // /*spanish*/un puñado de explosivos
                                           CustomMessage("plenty of blast balls", /*german*/"viele Explosionsbälle", /*french*/"une abondance de boules bleues")});
                                            // /*spanish*/bastantes estallidos

    hintTextTable[RHT_BOMBCHUS_5] = HintText(CustomMessage("Bombchus (5 pieces)", /*german*/"Krabbelminen (5 Stück)", /*french*/"une demi-dizaine de Missiles"),
                                            // /*spanish*/unos (5) bombchus
                                            {
                                            CustomMessage("a prize of the House of Skulltulas", /*german*/"ein Preis des Skulltula-Hauses", /*french*/"un prix de la maison des Skulltulas"),
                                             // /*spanish*/un obsequio de la Casa Skulltula
                                            CustomMessage("explosives", /*german*/"Explosivpakete", /*french*/"un paquet d'explosifs")
                                             // /*spanish*/un montón de explosivos
                                            }, {
                                            CustomMessage("a few mice bombs", /*german*/"ein paar Mäusebomben", /*french*/"une poignée de mignons explosifs"),
                                             // /*spanish*/un par de bombas roedoras
                                            CustomMessage("a few proximity mice", /*german*/"ein paar Näherungsmäuse", /*french*/"une poignée de jouets à remonter"),
                                             // /*spanish*/un par de explosivos ratoncitos
                                            CustomMessage("a few wall crawlers", /*german*/"ein paar Wandkrabbler", /*french*/"une poignée de rapides grimpeurs"),
                                             // /*spanish*/un par de trepaparedes
                                            CustomMessage("a few trail blazers", /*german*/"ein paar Vorreiter", /*french*/"une poignée de zigzags éclatants")});
                                             // /*spanish*/un par de ratas propulsadas

    hintTextTable[RHT_BOMBCHUS_10] = HintText(CustomMessage("Bombchus (10 pieces)", /*german*/"Krabbelminen (10 Stück)", /*french*/"une dizaine de Missiles"),
                                             // /*spanish*/unos (10) bombchus
                                             {
                                             CustomMessage("a prize of the House of Skulltulas", /*german*/"ein Preis des Skulltula-Hauses", /*french*/"un prix de la maison des Skulltulas"),
                                              // /*spanish*/un obsequio de la Casa Skulltula
                                             CustomMessage("explosives", /*german*/"Explosivpakete", /*french*/"un paquet d'explosifs")
                                              // /*spanish*/un montón de explosivos
                                             }, {
                                             CustomMessage("some mice bombs", /*german*/"einige Mäusebomben", /*french*/"un paquet de mignons explosifs"),
                                              // /*spanish*/unas cuantas bombas roedoras
                                             CustomMessage("some proximity mice", /*german*/"einige Näherungsmäuse", /*french*/"un paquet de jouets à remonter"),
                                              // /*spanish*/unos cuantos explosivos ratoncitos
                                             CustomMessage("some wall crawlers", /*german*/"einige Wandkrabbler", /*french*/"un paquet de rapides grimpeurs"),
                                              // /*spanish*/unos cuantos trepaparedes
                                             CustomMessage("some trail blazers", /*german*/"einige Vorreiter", /*french*/"un paquet de zigzags éclatants")});
                                              // /*spanish*/unas cuantas ratas propulsadas

    hintTextTable[RHT_BOMBCHUS_20] = HintText(CustomMessage("Bombchus (20 pieces)", /*german*/"Krabbelminen (20 Stück)", /*french*/"une vingtaine de Missiles"),
                                             // /*spanish*/unos (20) bombchus
                                             {
                                             CustomMessage("a prize of the House of Skulltulas", /*german*/"ein Preis des Skulltula-Hauses", /*french*/"un prix de la maison des Skulltulas"),
                                              // /*spanish*/un obsequio de la Casa Skulltula
                                             CustomMessage("explosives", /*german*/"Explosivpakete", /*french*/"un paquet d'explosifs")
                                              // /*spanish*/un montón de explosivos
                                             }, {
                                             CustomMessage("plenty of mice bombs", /*german*/"viele Mäusebomben", /*french*/"une abondance de mignons explosifs"),
                                              // /*spanish*/bastantes bombas roedoras
                                             CustomMessage("plenty of proximity mice", /*german*/"viele Näherungsmäuse", /*french*/"une abondance de jouets à remonter"),
                                              // /*spanish*/bastantes explosivos ratoncitos
                                             CustomMessage("plenty of wall crawlers", /*german*/"viele Wandkrabbler", /*french*/"une abondance de rapides grimpeurs"),
                                              // /*spanish*/bastantes trepaparedes
                                             CustomMessage("plenty of trail blazers", /*german*/"viele Vorreiter", /*french*/"une abondance de zigzags éclatants")});
                                              // /*spanish*/bastantes ratas propulsadas

    hintTextTable[RHT_ARROWS_5] = HintText(CustomMessage("Arrows (5 pieces)", /*german*/"Pfeile (5 Stück)", /*french*/"une demi-dizaine de flèches"),
                                           // /*spanish*/unas (5) flechas
                                           {
                                           CustomMessage("a projectile", /*german*/"ein Projektil", /*french*/"un projectile")
                                            // /*spanish*/un proyectil
                                           }, {
                                           CustomMessage("a few danger darts", /*german*/"ein paar gefährliche Spitzen", /*french*/"une poignée d'obus"),
                                            // /*spanish*/un par de peligrosos dardos
                                           CustomMessage("a few sharp shafts", /*german*/"ein paar scharfe Stifte", /*french*/"une poignée de piquets")});
                                            // /*spanish*/un par de puntas afiladas

    hintTextTable[RHT_ARROWS_10] = HintText(CustomMessage("Arrows (10 pieces)", /*german*/"Pfeile (10 Stück)", /*french*/"une dizaine de flèches"),
                                            // /*spanish*/unas (10) flechas
                                            {
                                            CustomMessage("a projectile", /*german*/"ein Projektil", /*french*/"un projectile")
                                             // /*spanish*/un proyectil
                                            }, {
                                            CustomMessage("some danger darts", /*german*/"einige gefährliche Spitzen", /*french*/"un paquet d'obus"),
                                             // /*spanish*/unos cuantos peligrosos dardos
                                            CustomMessage("some sharp shafts", /*german*/"einige scharfe Stifte", /*french*/"un paquet de piquets")});
                                             // /*spanish*/unas cuantas puntas afiladas

    hintTextTable[RHT_ARROWS_30] = HintText(CustomMessage("Arrows (30 pieces)", /*german*/"Pfeile (30 Stück)", /*french*/"une trentaine de flèches"),
                                            // /*spanish*/unas (30) flechas
                                            {
                                            CustomMessage("a projectile", /*german*/"ein Projektil", /*french*/"un projectile")
                                             // /*spanish*/un proyectil
                                            }, {
                                            CustomMessage("plenty of danger darts", /*german*/"viele gefährliche Spitzen", /*french*/"une abondance d'obus"),
                                             // /*spanish*/bastantes peligrosos dardos
                                            CustomMessage("plenty of sharp shafts", /*german*/"viele scharfe Stifte", /*french*/"une abondance de piquets")});
                                             // /*spanish*/bastantes puntas afiladas

    hintTextTable[RHT_DEKU_NUTS_5] = HintText(CustomMessage("Deku Nuts (5 pieces)", /*german*/"Deku-Nüsse (5 Stück)", /*french*/"une demi-dizaine de noix Mojo"),
                                              // /*spanish*/unas (5) nueces deku
                                              {
                                              CustomMessage("some Deku munitions", /*german*/"ein wenig Deku-Munition", /*french*/"un paquet de munitions Mojo"),
                                               // /*spanish*/un montón de municiones Deku
                                              CustomMessage("something that can stun", /*german*/"etwas, das paralysieren kann", /*french*/"une chose qui peut paralyser")
                                               // /*spanish*/algo que pueda paralizar
                                              }, {
                                              CustomMessage("some nuts", /*german*/"ein paar Nüsse", /*french*/"une poignée de noisettes"),
                                               // /*spanish*/un par de nueces
                                              CustomMessage("some flashbangs", /*german*/"ein paar Blendgranaten", /*french*/"une poignée d'éclats"),
                                               // /*spanish*/un par de semillas aturdidoras
                                              CustomMessage("some scrub spit", /*german*/"ein wenig Buschspucke", /*french*/"une poignée de crachats Mojo")});
                                               // /*spanish*/un par de escupitajos deku

    hintTextTable[RHT_DEKU_NUTS_10] = HintText(CustomMessage("Deku Nuts (10 pieces)", /*german*/"Deku-Nüsse (10 Stück)", /*french*/"une dizaine de noix Mojo"),
                                               // /*spanish*/unas (10) nueces deku
                                               {
                                               CustomMessage("some Deku munitions", /*german*/"etwas Deku-Munition", /*french*/"un paquet de munitions Mojo"),
                                                // /*spanish*/un montón de municiones Deku
                                               CustomMessage("something that can stun", /*german*/"etwas, das paralysieren kann", /*french*/"une chose qui peut paralyser")
                                                // /*spanish*/algo que pueda paralizar
                                               }, {
                                               CustomMessage("lots-o-nuts", /*german*/"einige Nüsse", /*french*/"un paquet de noisettes"),
                                                // /*spanish*/un puñado de nueces
                                               CustomMessage("plenty of flashbangs", /*german*/"einige Blendgranaten", /*french*/"un paquet d'éclats"),
                                                // /*spanish*/unas cuantas semillas aturdidoras
                                               CustomMessage("plenty of scrub spit", /*german*/"einige Buschspucke", /*french*/"un paquet de crachats Mojo")});
                                                // /*spanish*/unos cuantos escupitajos deku

    hintTextTable[RHT_DEKU_SEEDS_30] = HintText(CustomMessage("Deku Seeds (30 pieces)", /*german*/"Deku-Samen (30 Stück)", /*french*/"une trentaine de graines Mojo"),
                                                // /*spanish*/unas (30) semillas deku
                                                {
                                                CustomMessage("a projectile", /*german*/"ein Projektil", /*french*/"un projectile"),
                                                 // /*spanish*/un proyectil
                                                CustomMessage("some Deku munitions", /*german*/"viel Deku-Munition", /*french*/"un paquet de munitions Mojo")
                                                 // /*spanish*/un montón de municiones Deku
                                                }, {
                                                CustomMessage("catapult ammo", /*german*/"Katapultmunition", /*french*/"un paquet de délicieuses munitions"),
                                                 // /*spanish*/un par de munición infantil
                                                CustomMessage("lots-o-seeds", /*german*/"viele Samen", /*french*/"un paquet de germes séchés")});
                                                 // /*spanish*/un puñado de semillas

    hintTextTable[RHT_DEKU_STICK_1] = HintText(CustomMessage("a Deku Stick", /*german*/"ein Deku-Stab", /*french*/"un bâton Mojo"),
                                               // /*spanish*/un palo deku
                                               {
                                               CustomMessage("some Deku munitions", /*german*/"ein wenig Deku-Munition", /*french*/"un paquet de munitions Mojo")
                                                // /*spanish*/un montón de municiones Deku
                                               }, {
                                               CustomMessage("a breakable branch", /*german*/"ein brüchiger Zweig", /*french*/"un bout de bois")});
                                                // /*spanish*/un pequeño báculo

    hintTextTable[RHT_TREASURE_GAME_HEART] = HintText(CustomMessage("a Piece of Heart", /*german*/"ein Herzteil", /*french*/"un Quart de Coeur"),
                                                      // /*spanish*/el amor de la victoria
                                                      {
                                                      CustomMessage("something heart-shaped", /*german*/"etwas Herzförmiges", /*french*/"une chose en forme de coeur")
                                                       // /*spanish*/algo con forma de corazón
                                                      }, {
                                                      CustomMessage("a victory valentine", /*german*/"ein siegreicher Valentin", /*french*/"un amour gagnant")});
                                                       // /*spanish*/el amor victorioso

    hintTextTable[RHT_TREASURE_GAME_GREEN_RUPEE] = HintText(CustomMessage("a Green Rupee", /*german*/"ein grüner Rubin", /*french*/"un rubis vert"),
                                                            // /*spanish*/una rupia verde
                                                            {
                                                            CustomMessage("some rupees", /*german*/"einige Rubine", /*french*/"une quantité de rubis")
                                                             // /*spanish*/una cantidad de rupias
                                                            }, {
                                                            CustomMessage("the dollar of defeat", /*german*/"der Rubin der Niederlage", /*french*/"le rubis de la défaite")});
                                                             // /*spanish*/el peso de la derrota

    hintTextTable[RHT_TRIFORCE_PIECE] = HintText(CustomMessage("a Piece of the Triforce", /*german*/"ein Triforce-Fragment", /*french*/"un fragment de la Triforce"),
                                                 // /*spanish*/un fragmento de la Trifuerza
                                                 {}, {
                                                 CustomMessage("a triumph fork", /*german*/"ein Dreieck des Triumphs", /*french*/"la Tribosse"),
                                                  // /*spanish*/un trígono del triunfo
                                                 CustomMessage("cheese", /*german*/"Käse", /*french*/"du fromage"),
                                                  // /*spanish*/un porción de queso
                                                 CustomMessage("a gold fragment", /*german*/"ein goldenes Fragment", /*french*/"un fragment d'or")});
                                                  // /*spanish*/un fragmento dorado

    hintTextTable[RHT_GOHMA_SOUL] = HintText(CustomMessage("the soul of Gohma", /*german*/"die Seele Gohmas", /*french*/""),
                                             {
                                             CustomMessage("something webbed", /*german*/"etwas Verwobenes", /*french*/"")
                                             }, {
                                             CustomMessage("an invasive soul", /*german*/"eine invasive Seele", /*french*/""),
                                             CustomMessage("some spider essence", /*german*/"etwas spinnenartige Essenz", /*french*/"")});

    hintTextTable[RHT_KING_DODONGO_SOUL] = HintText(CustomMessage("the soul of King Dodongo", /*german*/"die Seele König Dodongos", /*french*/""),
                                                    {
                                                    CustomMessage("something explosive", /*german*/"etwas Explosives", /*french*/"")
                                                    }, {
                                                    CustomMessage("a royal soul", /*german*/"eine royale Seele", /*french*/""),
                                                    CustomMessage("some reptile essence", /*german*/"etwas reptilienartige Essenz", /*french*/"")});

    hintTextTable[RHT_BARINADE_SOUL] = HintText(CustomMessage("the soul of Barinade", /*german*/"die Seele Barinades", /*french*/""),
                                                {
                                                CustomMessage("something fishy", /*german*/"etwas Fischiges", /*french*/"")
                                                }, {
                                                CustomMessage("an infectuous soul", /*german*/"eine infektiöse Seele", /*french*/""),
                                                CustomMessage("some parasitic essence", /*german*/"etwas parasitäre Essenz", /*french*/"")});

    hintTextTable[RHT_PHANTOM_GANON_SOUL] = HintText(CustomMessage("the soul of Phantom Ganon", /*german*/"die Seele Phantom-Ganons", /*french*/""),
                                                     {
                                                     CustomMessage("something spectral", /*german*/"etwas Spektrales", /*french*/"")
                                                     }, {
                                                     CustomMessage("a duplicate soul", /*german*/"eine duplizierte Seele", /*french*/""),

                                                     CustomMessage("some illusionary essence", /*german*/"etwas illusionäre Essenz", /*french*/"")});

    hintTextTable[RHT_VOLVAGIA_SOUL] = HintText(CustomMessage("the soul of Volvagia", /*german*/"die Seele Volvagias", /*french*/""),
                                                {
                                                CustomMessage("something hot", /*german*/"etwas Heißes", /*french*/"")
                                                }, {
                                                CustomMessage("a draconic soul", /*german*/"eine drakonische Seele", /*french*/""),
                                                CustomMessage("some magmatic essence", /*german*/"etwas magmatische Essenz", /*french*/"")});

    hintTextTable[RHT_MORPHA_SOUL] = HintText(CustomMessage("the soul of Morpha", /*german*/"die Seele Morphas", /*french*/""),
                                              {
                                              CustomMessage("something wet", /*german*/"etwas Nasses", /*french*/"")
                                              }, {
                                              CustomMessage("an aquatic soul", /*german*/"eine aquatische Seele", /*french*/""),
                                              CustomMessage("some liquid essence", /*german*/"etwas flüssige Essenz", /*french*/"")});

    hintTextTable[RHT_BONGO_BONGO_SOUL] = HintText(CustomMessage("the soul of Bongo Bongo", /*german*/"die Seele Bongo Bongos", /*french*/""),
                                                   {
                                                   CustomMessage("something dark", /*german*/"etwas Dunkles", /*french*/"")
                                                   }, {
                                                   CustomMessage("a shadowy soul", /*german*/"eine schattige Seele", /*french*/""),
                                                   CustomMessage("some handy essence", /*german*/"etwas praktische Essenz", /*french*/"")});

    hintTextTable[RHT_TWINROVA_SOUL] = HintText(CustomMessage("the soul of Twinrova", /*german*/"die Seele Twinrovas", /*french*/""),
                                                {
                                                CustomMessage("something spiritual", /*german*/"etwas Spirituelles", /*french*/"")
                                                }, {
                                                CustomMessage("old souls", /*german*/"alte Seelen", /*french*/""),
                                                CustomMessage("twin essences", /*german*/"Zwillingsessenzen", /*french*/"")});

    hintTextTable[RHT_GANON_SOUL] = HintText(CustomMessage("the soul of Ganon", /*german*/"die Seele Ganons", /*french*/""),
                                             {
                                             CustomMessage("something strong", /*german*/"etwas Starkes", /*french*/"")
                                             }, {
                                             CustomMessage("an evil soul", /*german*/"eine böse Seele", /*french*/""),
                                             CustomMessage("some powerful essence", /*german*/"etwas mächtige Essenz", /*french*/"")});

    hintTextTable[RHT_OCARINA_A_BUTTON] = HintText(CustomMessage("an Ocarina A Button", /*german*/"eine Okarina A Taste", /*french*/"la Touche A de l'Ocarina"),
                                                   // /*spanish*/un botón A de Ocarina
                                                   {
                                                   CustomMessage("something melodic", /*german*/"etwas Melodisches", /*french*/"quelque chose de mélodieux")
                                                    // /*spanish*/algo melódico
                                                   }, {
                                                   CustomMessage("a musical letter", /*german*/"ein musikalischer Brief", /*french*/"une lettre musicale")});
                                                    // /*spanish*/una letra musical

    hintTextTable[RHT_OCARINA_C_UP_BUTTON] = HintText(CustomMessage("an Ocarina C Up Button", /*german*/"eine Okarina C-Oben Taste", /*french*/"la Touche C-Haut de l'Ocarina"),
                                                      // /*spanish*/un botón C superior de Ocarina
                                                      {
                                                      CustomMessage("something melodic", /*german*/"etwas Melodisches", /*french*/"quelque chose de mélodieux")
                                                       // /*spanish*/algo melódico
                                                      }, {
                                                      CustomMessage("a high tone", /*german*/"ein hoher Ton", /*french*/"une tonalité élevée")});
                                                       // /*spanish*/un tono alto

    hintTextTable[RHT_OCARINA_C_DOWN_BUTTON] = HintText(CustomMessage("an Ocarina C Down Button", /*german*/"eine Okarina C-Unten Taste", /*french*/"la Touche C-Bas de l'Ocarina"),
                                                        // /*spanish*/un botón C inferior de Ocarina
                                                        {
                                                        CustomMessage("something melodic", /*german*/"etwas Melodisches", /*french*/"quelque chose de mélodieux")
                                                         // /*spanish*/algo melódico
                                                        }, {
                                                        CustomMessage("a low tone", /*german*/"ein tiefer Ton", /*french*/"une tonalité basse")});
                                                         // /*spanish*/un tono bajo

    hintTextTable[RHT_OCARINA_C_LEFT_BUTTON] = HintText(CustomMessage("an Ocarina C Left Button", /*german*/"eine Okarina C-Links Taste", /*french*/"la Touche C-Gauche de l'Ocarina"),
                                                        // /*spanish*/un botón C izquierdo de Ocarina
                                                        {
                                                        CustomMessage("something melodic", /*german*/"etwas Melodisches", /*french*/"quelque chose de mélodieux")
                                                         // /*spanish*/algo melódico
                                                        }, {
                                                        CustomMessage("a leftward tone", /*german*/"ein linksseitiger Ton", /*french*/"une tonalité vers la gauche")});
                                                         // /*spanish*/un tono hacia la izquierda

    hintTextTable[RHT_OCARINA_C_RIGHT_BUTTON] = HintText(CustomMessage("an Ocarina C Right Button", /*german*/"eine Okarina C-Rechts Taste", /*french*/"la Touche C-Droit de l'Ocarina"),
                                                         // /*spanish*/un botón C derecho de Ocarina
                                                         {
                                                         CustomMessage("something melodic", /*german*/"etwas Melodisches", /*french*/"quelque chose de mélodieux")
                                                          // /*spanish*/algo melódico
                                                         }, {
                                                         CustomMessage("a rightward tone", /*german*/"ein rechtsseitiger Ton", /*french*/"une tonalité vers la droite")});
                                                          // /*spanish*/un tono hacia la derecha

    hintTextTable[RHT_FISHING_POLE] = HintText(CustomMessage("a fishing pole", /*german*/"eine Angelrute", /*french*/"canne à pêche"),
                                               // /*spanish*/caña de pescar
                                               {
                                               CustomMessage("the pond owner's property", /*german*/"der Besitz des Teicheigners", /*french*/"(canne à pêche)")
                                                // /*spanish*/(caña de pescar)
                                               }, {
                                               CustomMessage("a fish-puller", /*german*/"ein Fischzieher", /*french*/"(canne à pêche)")});
                                                // /*spanish*/(caña de pescar)
  
    hintTextTable[RHT_QUIVER_INF] = HintText(CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                             {
                                             CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                             }, {
                                             CustomMessage("", /*german*/"!!!", /*french*/"!!!")});

    hintTextTable[RHT_BOMB_BAG_INF] = HintText(CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                               {
                                               CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                               }, {
                                               CustomMessage("", /*german*/"!!!", /*french*/"!!!")});

    hintTextTable[RHT_BULLET_BAG_INF] = HintText(CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                                 {
                                                 CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                                 }, {
                                                 CustomMessage("", /*german*/"!!!", /*french*/"!!!")});

    hintTextTable[RHT_STICK_UPGRADE_INF] = HintText(CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                                    {
                                                    CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                                    }, {
                                                    CustomMessage("", /*german*/"!!!", /*french*/"!!!")});

    hintTextTable[RHT_NUT_UPGRADE_INF] = HintText(CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                                  {
                                                  CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                                  }, {
                                                  CustomMessage("", /*german*/"!!!", /*french*/"!!!")});

    hintTextTable[RHT_MAGIC_INF] = HintText(CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                            {
                                            CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                            }, {
                                            CustomMessage("", /*german*/"!!!", /*french*/"!!!")});

    hintTextTable[RHT_BOMBCHU_INF] = HintText(CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                              {
                                              CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                              }, {
                                              CustomMessage("", /*german*/"!!!", /*french*/"!!!")});

    hintTextTable[RHT_WALLET_INF] = HintText(CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                             {
                                             CustomMessage("", /*german*/"!!!", /*french*/"!!!"),
                                             }, {
                                             CustomMessage("", /*german*/"!!!", /*french*/"!!!")});

    hintTextTable[RHT_EPONA] = HintText(CustomMessage("Epona", /*german*/"Epona", /*french*/"Epona"),
                                    // /*spanish*/a Epona
                                    {
                                    CustomMessage("something from Malon", /*german*/"ein Geschenk von Malon", /*french*/"un cadeau de Malon"),
                                         // /*spanish*/un obsequio de Malon
                                    CustomMessage("a song sung by frogs", /*german*/"ein von Fröschen gesungenes Lied", /*french*/"une chanson aimée des grenouilles"),
                                         // /*spanish*/una melodía de ranas
                                    CustomMessage("something to cross a broken bridge", /*german*/"etwas, um eine kaputte Brücke zu überqueren", /*french*/"une chose pour traverser un pont brisé")
                                         // /*spanish*/algo para cruzar un puente roto
                                    }, {
                                    CustomMessage("a horse", /*german*/"ein Pferd", /*french*/"un fidèle destrier"),
                                         // /*spanish*/una yegua
                                    CustomMessage("a four legged friend", /*german*/"ein vierbeiniger Freund", /*french*/"un puissant animal")});
                                         // /*spanish*/una amiga cuadrúpeda

    //What is this used for?
    hintTextTable[RHT_HINT_MYSTERIOUS] = HintText(CustomMessage("something mysterious", /*german*/"etwas Mysteriöses", /*french*/"un sacré mystère"));
                                                  // /*spanish*/algo misterioso

    hintTextTable[RHT_MYSTERIOUS_ITEM] = HintText(CustomMessage("mysterious item", /*german*/"mysteriöser Gegenstand", /*french*/"objet mystérieux"));
                                                  // /*spanish*/algo misterioso
               
     hintTextTable[RHT_MYSTERIOUS_ITEM_CAPITAL] = HintText(CustomMessage("Mysterious Item", /*german*/"Mysteriöser Gegenstand", /*french*/"Objet Mystérieux"));
                                                  // /*spanish*/Algo Misterioso

}
}
