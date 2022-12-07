#include "setting_descriptions.hpp"

//Setting descriptions are mostly copied from OoT Randomizer tooltips with minor edits

/*------------------------------
|      RANDOMIZE SETTINGS      |                            *SCREEN WIDTH*
------------------------------*/       /*--------------------------------------------------*/
string_view openRandomize             = "Randomize all Open Settings.";                    //
string_view worldRandomize            = "Randomize all World Settings except for MQ\n"     //
                                        "dungeons.";                                       //
string_view shuffleRandomize          = "Randomize all Shuffle Settings.";                 //
string_view dungeonRandomize          = "Randomize all Dungeon Shuffle Settings.";         //
                                                                                           //
/*------------------------------                                                           //
|            LOGIC             |                                                           //
------------------------------*/                                                           //
string_view logicGlitchless           = "No glitches are required, but may require some\n" //
                                        "minor tricks. Add minor tricks to consider for\n" //
                                        "logic in Logical Tricks.";                        //
string_view logicGlitched             = "The glitches you enable at the set difficulty\n"  //
                                        "or below may be required.\n"                      //
                                        "\n"                                               //
                                        "In development, but still usable.";               //
string_view logicNoLogic              = "Maximize randomization, All locations are\n"      //
                                        "considered available. MAY BE IMPOSSIBLE TO BEAT.";//
string_view logicVanilla              = "For those who want to play the game normally but\n"
                                        "with the improvements of the randomizer. All\n"   //
                                        "locations will contain their vanilla items. This\n"
                                        "supercedes all item shuffle, logic, hint, and\n"  //
                                        "item pool settings. You can still use non-vanilla\n"
                                        "world settings such as adult start or entrance\n" //
                                        "shuffle, but the game may require glitches to\n"  //
                                        "complete if you do.";                             //
/*------------------------------                                                           //
|           FOREST             |                                                           //
------------------------------*/                                                           //
string_view forestOpen                = "Mido no longer blocks the path to the Deku Tree,\n"
                                        "and the Kokiri boy no longer blocks the path out\n"
                                        "of the forest.";                                  //
string_view forestClosed              = "Beating Deku Tree is logically required to leave\n"
                                        "the forest area (Kokiri Forest/Lost Woods/Sacred\n"
                                        "Forest Meadow/Deku Tree), while the Kokiri Sword\n"
                                        "and a Deku Shield are required to access the Deku\n"
                                        "Tree. Items needed for this will be guaranteed\n" //
                                        "inside the forest area. This setting is\n"        //
                                        "incompatible with starting as adult.";            //
string_view forestClosedDeku          = "The Kokiri boy no longer blocks the path out of\n"//
                                        "the forest, but Mido still blocks the path to the\n"
                                        "Deku Tree, requiring Kokiri Sword and Deku Shield\n"
                                        "to access the Deku Tree.";                        //
/*------------------------------                                                           //
|        KAKARIKO GATE         |                                                           //
------------------------------*/                                                           //
string_view kakGateOpen               = "The gate is always open instead of needing\n"     //
                                        "Zelda's Letter. The Happy Mask Shop opens upon\n" //
                                        "obtaining Zelda's Letter without needing to show\n"
                                        "it to the guard.";                                //
string_view kakGateClosed             = "The gate and the Happy Mask Shop both remain\n"   //
                                        "closed until showing Zelda's Letter to the guard\n"
                                        "in Kakariko.";                                    //
/*------------------------------                                                           //
|        DOOR OF TIME          |                                                           //
------------------------------*/                                                           //
string_view doorOfTimeOpen            = "The Door of Time starts opened instead of needing\n"
                                        "to play the Song of Time.";                       //
string_view doorOfTimeSongOnly        = "Only an Ocarina and the Song of Time need to be\n"//
                                        "found to open the Door of Time.";                 //
string_view doorOfTimeClosed          = "The Ocarina of Time, the Song of Time, and\n"     //
                                        "all Spiritual Stones need to be found to\n"       //
                                        "open the Door of Time.";                          //
/*------------------------------                                                           //
|       ZORA'S FOUNTAIN        |                                                           //
------------------------------*/                                                           //
string_view fountainNormal            = "King Zora obstructs the way to Zora's Fountain.\n"//
                                        "Ruto's Letter must be shown as child in order to\n"
                                        "move him for both eras.";                         //
string_view fountainAdult             = "King Zora is always moved in the adult era. This\n"
                                        "means Ruto's Letter is only required to access\n" //
                                        "Zora's Fountain as child.";                       //
string_view fountainOpen              = "King Zora starts as moved in both the child and\n"//
                                        "adult eras. This also removes Ruto's Letter from\n"
                                        "the pool since it can't be used.";                //
/*------------------------------                                                           //
|       GERUDO FORTRESS        |                                                           //
------------------------------*/                                                           //
string_view gerudoNormal              = "All 4 carpenters can be rescued.";                //
string_view gerudoFast                = "Only the bottom left carpenter must be rescued."; //
string_view gerudoOpen                = "The carpenters are rescued from the start of the\n"
                                        "game, and if Shuffle Gerudo Card is disabled,\n"  //
                                        "the player starts with the Gerudo Card in the\n"  //
                                        "inventory allowing access to Gerudo Training\n"   //
                                        "Grounds.";                                        //
/*------------------------------                                                           //
|        RAINBOW BRIDGE        |                                                           //
------------------------------*/                                                           //
string_view bridgeOpen                = "The Rainbow Bridge is always present.";           //
string_view bridgeVanilla             = "The Rainbow Bridge requires Shadow and Spirit\n"  //
                                        "Medallions as well as Light Arrows.";             //
string_view bridgeStones              = "The Rainbow Bridge requires collecting a\n"       //
                                        "configurable number of Spiritual Stones.";        //
string_view bridgeMedallions          = "The Rainbow Bridge requires collecting a\n"       //
                                        "configurable number of Medallions.";              //
string_view bridgeRewards             = "The Rainbow Bridge requires collecting a\n"       //
                                        "configurable number of Dungeon Rewards.";         //
string_view bridgeDungeons            = "The Rainbow Bridge requires completing a\n"       //
                                        "configurable number of Dungeons.\n"               //
                                        "\n"                                               //
                                        "Dungeons are considered complete when Link steps\n"
                                        "into the blue warp at the end of them.";          //
string_view bridgeTokens              = "The Rainbow Bridge requires collecting a\n"       //
                                        "configurable number of Gold Skulltula Tokens.";   //
/*------------------------------                                                           //
|      BRIDGE CONDITIONS       |                                                           //
------------------------------*/                                                           //
string_view bridgeStoneCountDesc      = "Set the number of Spiritual Stones required to\n" //
                                        "spawn the Rainbow Bridge.";                       //
string_view bridgeMedallionCountDesc  = "Set the number of Medallions required to spawn the"
                                        "Rainbow Bridge.";                                 //
string_view bridgeRewardCountDesc     = "Set the number of Dungeon Rewards (Spiritual\n"   //
                                        "Stones and Medallions) required to spawn the\n"   //
                                        "Rainbow Bridge.";                                 //
string_view bridgeDungeonCountDesc    = "Set the number of completed dungeons required to\n"
                                        "spawn the Rainbow Bridge.";                       //
string_view bridgeTokenCountDesc      = "Set the number of Gold Skulltula Tokens required\n"
                                        "to spawn the Rainbow Bridge.";                    //
/*------------------------------                                                           //
|     RANDOM GANONS TRIALS     |                                                           //
------------------------------*/                                                           //
string_view randomGanonsTrialsDesc    = "Sets a random number of required trials to enter\n"
                                        "Ganon's Tower.";                                  //
/*------------------------------                                                           //
|     GANON'S TRIAL COUNT      |                                                           //
------------------------------*/                                                           //
string_view ganonsTrialCountDesc      = "Set the number of trials required to enter\n"     //
                                        "Ganon's Tower. Trials will be randomly selected.";//
/*------------------------------                                                           //
|         STARTING AGE         |                                                           //
------------------------------*/                                                           //
string_view ageDesc                   = "Choose which age Link will start as.\n"           //
                                        "\n"                                               //
                                        "Starting as adult means you start with the Master\n"
                                        "Sword in your inventory.\n"                       //
                                        "\n"                                               //
                                        "Only the child option is compatible with Closed\n"//
                                        "Forest.";                                         //
/*------------------------------                                                           //
|      SHUFFLE ENTRANCES       |                                                           //
------------------------------*/                                                           //
string_view shuffleEntrancesDesc      = "Shuffle where the entrances between areas lead to."
                                        "If turned on, select which kinds of entrances you\n"
                                        "want shuffled in the options below. Note that some"
                                        "types of entrances can have wildly varying\n"     //
                                        "generation times.";                               //
                                                                                           //
/*------------------------------                                                           //
|       DUNGEON ENTRANCES      |                                                           //
------------------------------*/                                                           //
string_view dungeonEntrancesDesc      = "Shuffle the pool of dungeon entrances, including\n"
                                        "Bottom of the Well, Ice Cavern, and Gerudo\n"     //
                                        "Training Grounds. Shuffling Ganon's Castle can\n" //
                                        "be enabled separately.\n"                         //
                                        "\n"                                               //
                                        "Additionally, the entrances of Deku Tree, Fire\n" //
                                        "Temple, Bottom of the Well and Gerudo Training\n" //
                                        "Ground are opened for both adult and child.";     //
/*------------------------------                                                           //
|      OVERWORLD ENTRANCES     |                                                           //
------------------------------*/                                                           //
string_view overworldEntrancesDesc    = "Shuffle the pool of Overworld entrances, which\n" //
                                        "corresponds to almost all loading zones between\n"//
                                        "Overworld areas.\n"                               //
                                        "\n"                                               //
                                        "Some entrances are unshuffled to avoid issues:\n" //
                                        "- Hyrule Castle Courtyard and Garden entrance\n"  //
                                        "- Both Market Back Alley entrances\n"             //
                                        "- Gerudo Valley to Lake Hylia (unless entrances\n"//
                                        "  are decoupled)";                                //
/*------------------------------                                                           //
|      INTERIOR ENTRANCES      |                                                           //
------------------------------*/                                                           //
string_view interiorEntrancesOff      = "Interior entrances will not be shuffled.";        //
string_view interiorEntrancesSimple   = "Shuffle the pool of interior entrances which\n"   //
                                        "contains most Houses and all Great Fairies.";     //
string_view interiorEntrancesAll      = "An extended version of 'Simple' with some extra\n"//
                                        "places:\n"                                        //
                                        "- Windmill\n"                                     //
                                        "- Link's House\n"                                 //
                                        "- Temple of Time\n"                               //
                                        "- Kakariko Potion Shop.";                         //
/*------------------------------                                                           //
|       GROTTO ENTRANCES       |                                                           //
------------------------------*/                                                           //
string_view grottoEntrancesDesc       = "Shuffle the pool of grotto entrances, including\n"//
                                        "all graves, small Fairy Fountains and the Lost\n" //
                                        "Woods Stage.";                                    //
/*------------------------------                                                           //
|          OWL DROPS           |                                                           //
------------------------------*/                                                           //
string_view owlDropsDesc              = "Randomize where Kaepora Gaebora (the Owl) drops\n"//
                                        "you at when you talk to him at Lake Hylia or at\n"//
                                        "the top of Death Mountain Trail.";                //
/*------------------------------                                                           //
|          WARP SONGS          |                                                           //
------------------------------*/                                                           //
string_view warpSongsDesc             = "Randomize where each of the 6 warp songs leads to.";
                                                                                           //
/*------------------------------                                                           //
|       OVERWORLD SPAWNS       |                                                           //
------------------------------*/                                                           //
string_view overworldSpawnsDesc       = "Randomize where you start as Child or Adult when\n"
                                        "loading a save in the Overworld. This means you\n"//
                                        "may not necessarily spawn inside Link's House or\n"
                                        "Temple of Time.\n"                                //
                                        "\n"                                               //
                                        "This stays consistent after saving and loading the"
                                        "game again.";                                     //
/*------------------------------                                                           //
|     MIXED ENTRANCE POOLS     |                                                           //
------------------------------*/                                                           //
string_view mixedPoolsDesc            = "Shuffle entrances into a mixed pool instead of\n" //
                                        "separate ones. For example, enabling the settings\n"
                                        "to shuffle grotto, dungeon, and overworld\n"      //
                                        "entrances and selecting grotto and dungeon\n"     //
                                        "entrances here will allow a dungeon to be inside a"
                                        "grotto or vice versa, while overworld entrances\n"//
                                        "are shuffled in their own separate pool and\n"    //
                                        "indoors stay vanilla.";                           //
string_view mixDungeonsDesc           = "Dungeon entrances will be part of the mixed pool.";
string_view mixOverworldDesc          = "Overworld entrances will be part of the mixed\n"  //
                                        "pool.";                                           //
string_view mixInteriorsDesc          = "Interior entrances will be part of the mixed pool.";
string_view mixGrottosDesc            = "Grotto entrances will be part of the mixed pool.";//
/*------------------------------                                                           //
|      DECOUPLED ENTRANCES     |                                                           //
------------------------------*/                                                           //
string_view decoupledEntrancesDesc    = "Decouple entrances when shuffling them. This means"
                                        "you are no longer guaranteed to end up back where "
                                        "you came from when you go back through an\n"      //
                                        "entrance. This also adds the one-way entrance from"
                                        "Gerudo Valley to Lake Hylia in the pool of\n"     //
                                        "overworld entrances when they are shuffled.";     //
/*------------------------------                                                           //
|      BOMBCHUS IN LOGIC       |                                                           //
------------------------------*/                                                           //
string_view bombchuLogicDesc          = "Bombchus are properly considered in logic.\n"     //
                                        "They can be replenished in shops, or through\n"   //
                                        "bombchu drops, if those are enabled.\n"           //
                                        "\n"                                               //
                                        "Bombchu Bowling is opened by bombchus.";          //
/*------------------------------                                                           //
|          AMMO DROPS          |                                                           //
------------------------------*/                                                           //
string_view defaultAmmoDropsDesc      = "Bombs, arrows, seeds, nuts, sticks and\n"         //
                                        "magic jars appear as normal.\n";                  //
string_view bombchuDropsDesc          = "Bombs, arrows, seeds, nuts, sticks and\n"         //
                                        "magic jars appear as normal.\n"                   //
                                        "Bombchus can sometimes replace bomb drops.";      //
string_view noAmmoDropsDesc           = "All ammo drops will be replaced by blue rupees,\n"//
                                        "except for Deku Sticks.\n"                        //
                                        "Ammo upgrades will only refill ammo by 10 units.";//
/*------------------------------                                                           //
|    HEART DROPS AND REFILLS   |                                                           //
------------------------------*/                                                           //
string_view defaultHeartDropsDesc     = "Heart drops will appear as normal.\n"             //
                                        "Health upgrades fully heal Link when picked up.\n"//
                                        "Fairies heal Link as normal.";                    //
string_view noHeartDropsDesc          = "Heart drops will be replaced by green rupees.\n"  //
                                        "Health upgrades fully heal Link when picked up.\n"//
                                        "Fairies heal Link as normal.";                    //
string_view noHeartRefillDesc         = "Heart drops will appear as normal.\n"             //
                                        "Health upgrades don't heal Link when picked up.\n"//
                                        "Fairies heal Link by only 3 hearts.";             //
string_view scarceHeartsDesc          = "Heart drops will be replaced by green rupees.\n"  //
                                        "Health upgrades don't heal Link when picked up.\n"//
                                        "Fairies heal Link by only 3 hearts.";             //
/*------------------------------                                                           //
|       MQ DUNGEON COUNT       |                                                           //
------------------------------*/                                                           //
string_view mqDungeonCountDesc        = "Specify the number of Master Quest dungeons to\n" //
                                        "appear in the game. Which dungeons become Master\n"
                                        "Quest will be chosen at random.";                 //
/*------------------------------                                                           //
|        SET MQ DUNGEONS       |                                                           //
------------------------------*/                                                           //
string_view setDungeonTypesDesc       = "If set, you can choose specific dungeons to be\n" //
                                        "vanilla, MQ, or random";                          //
/*------------------------------                                                           //
|   SHUFFLE DUNGEON REWARDS    |                                                           //
------------------------------*/                                                           //
string_view shuffleRewardsEndOfDungeon= "Medallions and Spiritual Stones will be given as\n"
                                        "rewards for beating dungeons.\n"                  //
                                        "\n"                                               //
                                        "This setting will force Link's Pocket to be a\n"  //
                                        "Medallion or Spiritual Stone.";                   //
string_view shuffleRewardsAnyDungeon  = "Medallions and Spiritual Stones can only appear\n"//
                                        "inside of dungeons.";                             //
string_view shuffleRewardsOverworld   = "Medallions and Spiritual Stones can only appear\n"//
                                        "outside of dungeons.";                            //
string_view shuffleRewardsAnywhere    = "Medallions and Spiritual Stones can appear\n"     //
                                        "anywhere.";                                       //
/*------------------------------                                                           //
|        LINK'S POCKET         |                                                           //
------------------------------*/                                                           //
string_view linksPocketDungeonReward  = "Link will start with a Dungeon Reward in his\n"   //
                                        "inventory.";                                      //
string_view linksPocketAdvancement    = "Link will receive a random advancement item at the"
                                        "beginning of the playthrough.";                   //
string_view linksPocketAnything       = "Link will receive a random item from the item pool"
                                        "at the beginning of the playthrough.";            //
string_view linksPocketNothing        = "Link will start with a very useful green rupee."; //
/*------------------------------                                                           //
|         SONG SHUFFLE         |                                                           //
------------------------------*/                                                           //
string_view songsSongLocations        = "Songs will only appear at locations that normally\n"
                                        "teach songs.";                                    //
string_view songsDungeonRewards       = "Songs appear at the end of dungeons. For major\n" //
                                        "dungeons, they will be at the boss heart container"
                                        "location. The remaining 4 songs are placed at:\n" //
                                        "- Zelda's Lullaby Location\n"                     //
                                        "- Ice Cavern's Serenade of Water Location\n"      //
                                        "- Bottom of the Well's Lens of Truth Location\n"  //
                                        "- Gerudo Training Ground's Ice Arrow Location";   //
string_view songsAllLocations         = "Songs can appear in any location.";               //
                                                                                           //
/*------------------------------                                                           //
|         SHOPSANITY           |                                                           //
------------------------------*/                                                           //
string_view shopsOff                  = "All shop items will be the same as vanilla.";     //
string_view shopsZero                 = "Vanilla shop items will be shuffled among\n"      //
                                        "different shops.";                                //
string_view shopsOne                  = "Vanilla shop items will be shuffled among\n"      //
                                        "different shops, and each shop will contain\n"    //
                                        "one non-vanilla shop item.";                      //
string_view shopsTwo                  = "Vanilla shop items will be shuffled among\n"      //
                                        "different shops, and each shop will contain\n"    //
                                        "two non-vanilla shop items.";                     //
string_view shopsThree                = "Vanilla shop items will be shuffled among\n"      //
                                        "different shops, and each shop will contain\n"    //
                                        "three non-vanilla shop items.";                   //
string_view shopsFour                 = "Vanilla shop items will be shuffled among\n"      //
                                        "different shops, and each shop will contain\n"    //
                                        "four non-vanilla shop items.";                    //
string_view shopsRandom               = "Vanilla shop items will be shuffled among\n"      //
                                        "different shops, and each shop will contain\n"    //
                                        "1-4 non-vanilla shop items.";                     //
/*------------------------------                                                           //
|         TOKENSANITY          |                                                           //
------------------------------*/                                                           //
string_view tokensOff                 = "GS locations will not be shuffled.";              //
string_view tokensDungeon             = "This only shuffles the GS locations that are\n"   //
                                        "within dungeons, increasing the value of most\n"  //
                                        "dungeons and making internal dungeon exploration\n"
                                        "more diverse.";                                   //
string_view tokensOverworld           = "This only shuffles the GS locations that are\n"   //
                                        "outside of dungeons.";                            //
string_view tokensAllTokens           = "Effectively adds 100 new locations for items to\n"//
                                        "appear.";                                         //
                                                                                           //
/*------------------------------                                                           //
|        SCRUB SHUFFLE         |                                                           //
------------------------------*/                                                           //
string_view scrubsOff                 = "Only the 3 Scrubs that give one-time items in the\n"
                                        "vanilla game (PoH, Deku Nut capacity, and Deku\n" //
                                        "Stick capacity) will have random items.";         //
string_view scrubsAffordable          = "All Scrub prices will be reduced to 10 rupees each";
string_view scrubsExpensive           = "All Scrub prices will be their vanilla prices.\n" //
                                        "This will require spending over 1000 rupees on\n" //
                                        "Scrubs.";                                         //
string_view scrubsRandomPrices        = "All Scrub prices will be between 0-95 rupees. This"
                                        "will on average be very, very expensive overall.";//
/*------------------------------                                                           //
|         SHUFFLE COWS         |                                                           //
------------------------------*/                                                           //
string_view shuffleCowsDesc           = "Enabling this will let cows give you items upon\n"//
                                        "performing Epona's song in front of them. There\n"//
                                        "are 9 cows, and an extra in MQ Jabu.";            //
                                                                                           //
/*------------------------------                                                           //
|     SHUFFLE KOKIRI SWORD     |                                                           //
------------------------------*/                                                           //
string_view kokiriSwordDesc           = "Enabling this shuffles the Kokiri Sword into the\n"
                                        "item pool.\n"                                     //
                                        "\n"                                               //
                                        "This will require extensive use of sticks until\n"//
                                        "the sword is found.";                             //
/*------------------------------                                                           //
|       SHUFFLE OCARINAS       |                                                           //
------------------------------*/                                                           //
string_view ocarinasDesc              = "Enabling this shuffles the Fairy Ocarina and the\n"
                                        "Ocarina of Time into the item pool.\n"            //
                                        "\n"                                               //
                                        "This will require finding an Ocarina before being\n"
                                        "able to play songs.";                             //
/*------------------------------                                                           //
|      SHUFFLE WEIRD EGG       |                                                           //
------------------------------*/                                                           //
string_view weirdEggDesc              = "Enabling this shuffles the Weird Egg from Malon\n"//
                                        "into the item pool.\n"                            //
                                        "This will require finding the Weird Egg to talk to"
                                        "Zelda in Hyrule Castle, which in turn locks\n"    //
                                        "rewards from Impa, Saria, Malon, and Talon, as\n" //
                                        "well as the Happy Mask Sidequest. The Weird Egg is"
                                        "also required for Zelda's Letter to unlock the\n" //
                                        "Kakariko Gate as child which can lock some\n"     //
                                        "progression";                                     //
/*------------------------------                                                           //
|     SHUFFLE Gerudo Membership Card     |                                                           //
------------------------------*/                                                           //
string_view gerudoTokenDesc           = "Enabling this shuffles the Gerudo Membership Card into the\n"
                                        "item pool.\n"                                     //
                                        "\n"                                               //
                                        "The Gerudo Membership Card is required to enter the Gerudo\n"
                                        "Training Ground.";                                //
/*------------------------------                                                           //
|      SHUFFLE MAGIC BEANS     |                                                           //
------------------------------*/                                                           //
string_view magicBeansDesc            = "Enabling this adds a pack of 10 beans to the item\n"
                                        "pool and changes the Magic Bean Salesman to sell a"
                                        "random item at a price of 60 rupees.";            //
/*------------------------------                                                           //
|       SHUFFLE MERCHANTS      |                                                           //
------------------------------*/                                                           //
string_view merchantsDesc             = "Enabling this adds a Giant's Knife and a pack\n"  //
                                        "of Bombchus to the item pool and changes both\n"  //
                                        "Medigoron and the Haunted Wasteland Carpet\n"     //
                                        "Salesman to sell a random item once at the price\n"
                                        "of 200 rupees.";                                  //
string_view merchantsHintsDesc        = "These hints will make Medigoron and the Carpet\n" //
                                        "Salesman tell you which item they're selling.\n"  //
                                        "\n"                                               //
                                        "The Clearer Hints setting will affect how they\n" //
                                        "refer to the item.";                              //
/*------------------------------                                                           //
|   SHUFFLE FROG SONG RUPEES   |                                                           //
------------------------------*/                                                           //
string_view frogSongRupeesDesc        = "Enabling this adds 5 Purple Rupees to the item\n" //
                                        "pool and shuffles the rewards from playing Zelda's\n"
                                        "Lullaby, Epona's Song, Saria's Song, Sun's Song,\n"
                                        "and Song of Time to the frogs in Zora's River.\n";//
/*------------------------------                                                           //
|     SHUFFLE ADULT TRADE      |                                                           //
------------------------------*/                                                           //
string_view adultTradeDesc            = "Enabling this adds all of the adult trade quest\n"//
                                        "items to the pool, each of which can be traded\n" //
                                        "for a unique reward. You will be able to choose\n"//
                                        "which of your owned adult trade items is visible\n"
                                        "in the inventory by selecting the item and using\n"
                                        "the L and R buttons. If disabled, only the Claim\n"
                                        "Check will be found in the pool.";                //
/*------------------------------                                                           //
|    SHUFFLE CHEST MINIGAME    |                                                           //
------------------------------*/                                                           //
string_view chestMinigameDesc         = "The 5 key chests in the Treasure Chest Shop will\n"
                                        "be randomized, and the 6 keys will be added to the"//
                                        "pool. The rupee chests will be replaced by traps.\n"
                                        "Also, the shop owner is on vacation, so he can't\n"
                                        "close any chests or doors once you leave.\n"      //
                                        "\n"                                               //
                                        "If you choose the \"pack\" option, you will get\n"//
                                        "all the keys at once, in a single item.";         //
/*------------------------------                                                           //
|      MAPS AND COMPASSES      |                                                           //
------------------------------*/                                                           //
string_view mapCompassStartWith       = "Maps and Compasses are given to you from the\n"   //
                                        "start. This will add a small amount of money and\n"
                                        "refill items to the pool.";                       //
string_view mapCompassVanilla         = "Maps and Compasses will appear in their vanilla\n"//
                                        "locations.";                                      //
string_view mapCompassOwnDungeon      = "Maps and Compasses can only appear in their\n"    //
                                        "respective dungeon.";                             //
string_view mapCompassAnyDungeon      = "Maps and Compasses can only appear in a dungeon,\n"
                                        "but not necessarily the dungeon they are for.";   //
string_view mapCompassOverworld       = "Maps and Compasses can only appear outside of\n"  //
                                        "dungeons.";                                       //
string_view mapCompassAnywhere        = "Maps and Compasses can appear anywhere in the\n"  //
                                        "world.";                                          //
/*------------------------------                                                           //
|         SMALL KEYS           |                                                           //
------------------------------*/                                                           //
string_view smallKeyStartWith         = "Small Keys are given to you from the start so you\n"
                                        "won't have to worry about locked doors. An easier\n"
                                        "mode.";                                           //
string_view smallKeyVanilla           = "Small Keys will appear in their vanilla locations."
                                        "You start with 3 keys in Spirit Temple MQ because\n"
                                        "the vanilla key layout is not beatable in logic.";//
string_view smallKeyOwnDungeon        = "Small Keys can only appear in their respective\n" //
                                        "dungeon. If Fire Temple is not a Master Quest\n"  //
                                        "dungeon, the door to the Boss Key chest will be\n"//
                                        "unlocked.";                                       //
string_view smallKeyAnyDungeon        = "Small Keys can only appear inside of any dungeon,\n"
                                        "but won't necessarily be in the dungeon that the\n"
                                        "key is for. A difficult mode since it is more\n"  //
                                        "likely to need to enter a dungeon multiple times.";
string_view smallKeyOverworld         = "Small Keys can only appear outside of dungeons.\n"//
                                        "You may need to enter a dungeon multiple times to\n"
                                        "gain items to access the overworld locations with\n"
                                        "the keys required to finish a dungeon.";          //
string_view smallKeyAnywhere          = "Small Keys can appear anywhere in the world. A\n" //
                                        "difficult mode since it is more likely to need to\n"
                                        "enter a dungeon multiple times.";                 //
/*------------------------------                                                           //
|     GERUDO FORTRESS KEYS     |                                                           //
------------------------------*/                                                           //
string_view gerudoKeysVanilla         = "Gerudo Fortress Keys will appear in their vanilla\n"
                                        "location, dropping from fighting Gerudo guards\n" //
                                        "that attack when trying to free the jailed\n"     //
                                        "carpenters.";                                     //
string_view gerudoKeysAnyDungeon      = "Gerudo Fortress Keys can only appear inside of\n" //
                                        "dungeons.";                                       //
string_view gerudoKeysOverworld       = "Gerudo Fortress Keys can only appear outside of\n"//
                                        "dungeons.";                                       //
string_view gerudoKeysAnywhere        = "Gerudo Fortress Keys can appear anywhere in the\n"//
                                        "world.";                                          //
/*------------------------------                                                           //
|           Key Rings          |                                                           //
------------------------------*/                                                           //
string_view keyRingDesc               = "Selected key ring dungeons will have all of their\n"
                                        "keys found at once in a ring rather than\n"       //
                                        "individually.\n\n"                                //
                                        "For example, instead of shuffling 5 Forest Temple\n"
                                        "small keys into the pool, you will find a single\n"
                                        "key ring which will give you all 5 keys at once.\n";
/*------------------------------                                                           //
|          BOSS KEYS           |                                                           //
------------------------------*/                                                           //
string_view bossKeyStartWith          = "Boss Keys are given to you from the start so you\n"
                                        "won't have to worry about boss doors. An easier\n"//
                                        "mode.";                                           //
string_view bossKeyVanilla            = "Boss Keys will appear in their vanilla locations.";
string_view bossKeyOwnDungeon         = "Boss Keys can only appear in their respective\n"  //
                                        "dungeon.";                                        //
string_view bossKeyAnyDungeon         = "Boss Keys can only appear inside of any dungeon,\n"
                                        "but won't necessarily be in the dungeon that the\n"
                                        "key is for. A difficult mode since it is more\n"  //
                                        "likely to need to enter a dungeon multiple times.";
string_view bossKeyOverworld          = "Boss Keys can only appear outside of dungeons.\n" //
                                        "You may need to enter a dungeon without the boss\n"
                                        "key to get items required to find the key in the\n"
                                        "overworld.";                                      //
string_view bossKeyAnywhere           = "Boss Keys can appear anywhere in the world. A\n"  //
                                        "difficult mode since it is more likely to need to\n"
                                        "enter a dungeon multiple times.";                 //
/*------------------------------                                                           //
|   GANON'S CASTLE BOSS KEY    |                                                           //
------------------------------*/                                                           //
string_view ganonKeyStartWith         = "Ganon's Castle Boss Key is given to you from the\n"
                                        "start and you don't have to worry about finding it";
string_view ganonKeyVanilla           = "Ganon's Castle Boss Key will appear in the vanilla"
                                        "location.";                                       //
string_view ganonKeyOwnDungeon        = "Ganon's Castle Boss Key can only appear inside\n" //
                                        "Ganon's Castle.";                                 //
string_view ganonKeyAnyDungeon        = "Ganon's Castle Boss Key can only appear inside of\n"
                                        "a dungeon, but not necessarily Ganon's Castle.";  //
string_view ganonKeyOverworld         = "Ganon's Castle Boss Key can only appear outside of"
                                        "dungeons.";                                       //
string_view ganonKeyAnywhere          = "Ganon's Castle Boss Key can appear anywhere in the"
                                        "world.";                                          //
string_view ganonKeyLACS              = "These settings put the boss key on the Light Arrow"
                                        "Cutscene location, from Zelda in Temple of Time as"
                                        "adult, with differing requirements.";             //
/*------------------------------                                                           //
|        LACS CONDITIONS       |                                                           //
------------------------------*/                                                           //
string_view lacsMedallionCountDesc    = "Set the number of Medallions required to trigger\n"
                                        "the Light Arrow Cutscene.";                       //
string_view lacsStoneCountDesc        = "Set the number of Spiritual Stones required to\n" //
                                        "trigger the Light Arrow Cutscene.";               //
string_view lacsRewardCountDesc       = "Set the number of Dungeon Rewards (Spiritual\n"   //
                                        "Stones and Medallions) required to trigger the\n" //
                                        "Light Arrow Cutscene.";                           //
string_view lacsDungeonCountDesc      = "Set the number of completed dungeons required to\n"
                                        "trigger the Light Arrow Cutscene.\n"              //
                                        "\n"                                               //
                                        "Dungeons are considered complete when Link steps\n"
                                        "into the blue warp at the end of them.";          //
string_view lacsTokenCountDesc        = "Set the number of Gold Skulltula Tokens required\n"
                                        "to trigger the Light Arrow Cutscene.";            //
/*------------------------------                                                           //
|      SKIP CHILD STEALTH      |                                                           //
------------------------------*/                                                           //
string_view childStealthDesc          = "The crawlspace into Hyrule Castle goes straight to"
                                        "Zelda, skipping the guards.";                     //
/*------------------------------                                                           //
|      SKIP TOWER ESCAPE       |                                                           //
------------------------------*/                                                           //
string_view skipTowerEscapeDesc       = "The tower escape sequence between Ganondorf and\n"//
                                        "Ganon will be skipped.";                          //
/*------------------------------                                                           //
|       SKIP EPONA RACE        |                                                           //
------------------------------*/                                                           //
string_view skipEponaRaceDesc         = "Epona can be summoned with Epona's Song without\n"//
                                        "needing to race Ingo.";                           //
/*------------------------------                                                           //
|     SKIP MINIGAME PHASES     |                                                           //
------------------------------*/                                                           //
string_view skipMinigamePhasesDesc    = "Completing the second objective in the Dampe Race\n"
                                        "and Gerudo Archery on the first attempt will give\n"
                                        "both rewards at once for that minigame.";          //
/*------------------------------                                                           //
|        FREE SCARECROW        |                                                           //
------------------------------*/                                                           //
string_view freeScarecrowDesc         = "Pulling out the Ocarina near a spot at which\n"   //
                                        "Pierre can spawn will do so, without needing\n"   //
                                        "the song.";                                       //
/*------------------------------                                                           //
|      FOUR POES CUTSCENE      |                                                           //
------------------------------*/                                                           //
string_view fourPoesDesc              = "The cutscene with the 4 poes in Forest Temple will"
                                        "be skipped. If the cutscene is not skipped, it can"
                                        "be exploited to reach the basement early.";       //
/*------------------------------                                                           //
|        LAKE HYLIA OWL        |                                                           //
------------------------------*/                                                           //
string_view lakeHyliaOwlDesc          = "The owl flight cutscene in Lake Hylia will be\n"  //
                                        "skipped. This cutscene lets you see what item\n"  //
                                        "is on top of the laboratory roof.";               //
/*------------------------------                                                           //
|     BIG POE TARGET COUNT     |                                                           //
------------------------------*/                                                           //
string_view bigPoeTargetCountDesc     = "The Poe buyer will give a reward for turning in\n"//
                                        "the chosen number of Big Poes.";                  //
/*------------------------------                                                           //
|     NUM REQUIRED CUCCOS      |                                                           //
------------------------------*/                                                           //
string_view numRequiredCuccosDesc     = "The cucco lady will give a reward for returning\n"//
                                        "this many of her cuccos to the pen.";             //
/*------------------------------                                                           //
|        KING ZORA SPEED        |                                                          //
-------------------------------*/                                                          //
string_view kingZoraSpeedFast         = "King Zora will move out of the way in 1 shuffle"; //
                                                                                           //
string_view kingZoraSpeedVanilla      = "King Zora will move out of the way in 26 shuffles";
                                                                                           //
string_view kingZoraSpeedRandom       = "King Zora will move out of the way in 1 to 128\n" //
                                        "shuffles, with lower numbers being more common";  //
/*------------------------------                                                           //
|      COMPLETE MASK QUEST     |                                                           //
------------------------------*/                                                           //
string_view completeMaskDesc          = "Once the happy mask shop is opened, all masks\n"  //
                                        "will be available to be borrowed.";               //
/*------------------------------                                                           //
|    ENABLE GLITCH CUTSCENES   |                                                           //
------------------------------*/                                                           //
string_view glitchCutscenesDesc       = "The cutscenes of the Poes in Forest Temple and\n" //
                                        "Darunia in Fire Temple will not be skipped.\n"    //
                                        "These cutscenes are only useful for glitched\n"   //
                                        "gameplay and can be safely skipped otherwise.";   //
/*------------------------------                                                           //
|          QUICK TEXT          |                                                           //
------------------------------*/                                                           //
string_view quickTextDesc0            = "Quick text will be unchanged, requiring\n"        //
                                        "frame-perfect inputs like in the vanilla game.";  //
string_view quickTextDesc1            = "Every text box will be completable by pressing B\n"
                                        "at any point while it's scrolling.";              //
string_view quickTextDesc2            = "Every text box will auto-complete instantly.\n"   //
                                        "No scrolling allowed!";                           //
string_view quickTextDesc3            = "Holding B will advance and close text boxes\n"    //
                                        "automatically, except for choice selections.";    //
/*------------------------------                                                           //
|       SKIP SONG REPLAYS      |                                                           //
------------------------------*/                                                           //
string_view skipSongReplaysDesc       = "The automatic replay after you play a song will\n"//
                                        "be skipped.\n"                                    //
                                        "You can choose to keep the sfx anyway, but you\n" //
                                        "will have control of Link during it.";            //
/*------------------------------                                                           //
|      KEEP FW WARP POINT      |                                                           //
------------------------------*/                                                           //
string_view keepFWWarpPointDesc       = "The Farore's Wind warp point will stay active\n"  //
                                        "after having been warped to. The old point will\n"//
                                        "need to be dispelled before setting a new one.";  //
/*------------------------------                                                           //
|       FAST BUNNY HOOD        |                                                           //
------------------------------*/                                                           //
string_view fastBunnyHoodDesc         = "The Bunny Hood mask behaves like it does in\n"    //
                                        "Majora's Mask and makes you run 50% faster.";     //
/*------------------------------                                                           //
|     GOSSIP STONE HINTS       |                                                           //
------------------------------*/                                                           //
string_view gossipStonesHintsDesc     = "Gossip Stones can be made to give hints about\n"  //
                                        "where items can be found.\n"                      //
                                        "Different settings can be chosen to decide which\n"
                                        "item is needed to speak to Gossip Stones. Choosing"
                                        "to stick with the Mask of Truth will make the\n"  //
                                        "hints very difficult to obtain.\n"                //
                                        "Hints for 'on the way of the hero' are locations\n"
                                        "that contain items that are required to beat the\n"
                                        "game.";                                           //
                                                                                           //
/*------------------------------                                                           //
|        HINT CLARITY          |                                                           //
------------------------------*/                                                           //
string_view obscureHintsDesc          = "Sets the difficulty of hints.\n"                  //
                                        "Obscure: Hints are unique for each thing, but\n"  //
                                        "the writing may be confusing.\n"                  //
                                        "E.g. Kokiri Sword > a butter knife";              //
string_view ambiguousHintsDesc        = "Sets the difficulty of hints.\n"                  //
                                        "Ambiguous: Hints are clearly written, but may\n"  //
                                        "refer to more than one thing.\n"                  //
                                        "E.g. Kokiri Sword > a sword";                     //
string_view clearHintsDesc            = "Sets the difficulty of hints.\n"                  //
                                        "Clear: Hints are clearly written and are unique\n"//
                                        "for each thing.\n"                                //
                                        "E.g. Kokiri Sword > the Kokiri Sword";            //
/*------------------------------                                                           //
|      HINT DISTRIBUTION       |                                                           //
------------------------------*/                                                           //
string_view uselessHintsDesc          = "Only junk hints.";                                //
string_view balancedHintsDesc         = "Recommended hint spread.";                        //
string_view strongHintsDesc           = "More useful hints.";                              //
string_view veryStrongHintsDesc       = "Many powerful hints.";                            //
                                                                                           //
/*------------------------------                                                           //
|  MAP AND COMPASS GIVES INFO  |                                                           //
------------------------------*/                                                           //
string_view compassesShowRewardsDesc  = "If dungeon rewards are set to be shuffled at the\n"
                                        "end of dungeons, the in-game menu will reveal\n"  //
                                        "which reward is in each dungeon, if the compass\n"//
                                        "for that dungeon has been collected.";            //
string_view compassesShowWotHDesc     = "The in-game menu will reveal whether each\n"      //
                                        "dungeon is on the Way of the Hero, a barren\n"    //
                                        "location, or neither, if the compass for that\n"  //
                                        "dungeon has been collected.";                     //
string_view mapsShowDungeonModesDesc  = "If any Master Quest dungeons will be randomly\n"  //
                                        "shuffled, the in-game menu will reveal whether\n" //
                                        "it is in its Vanilla or Master Quest form, if\n"  //
                                        "the map for the dungeon has been collected.\n"    //
                                        "Ganon's Castle and Gerudo Training Grounds are\n" //
                                        "always revealed, as they do not have maps.";      //
/*------------------------------                                                           //
|       DAMAGE MULTIPLIER      |                                                           //
------------------------------*/                                                           //
string_view damageMultiDesc           = "Changes the amount of damage taken.\n"            //
                                        "\n"                                               //
                                        "If set to OHKO, Link will die in one hit.";       //
/*------------------------------                                                           //
|        STARTING TIME         |                                                           //
------------------------------*/                                                           //
string_view startingTimeDesc          = "Change up Link's sleep routine.";                 //
                                                                                           //
/*------------------------------                                                           //
|   ALL LOCATIONS REACHABLE    |                                                           //
------------------------------*/                                                           //
string_view locationsReachableDesc    = "When this options is enabled, the randomizer will\n"
                                        "guarantee that every item is obtainable and every\n"
                                        "location is reachable. When disabled, only\n"     //
                                        "required items and locations to beat the game\n"  //
                                        "will be guaranteed reachable.";                   //
/*------------------------------                                                           //
|     NIGHT GS EXPECT SUNS     |                                                           //
------------------------------*/                                                           //
string_view nightGSDesc               = "GS Tokens that can only be obtained during the\n" //
                                        "night expect you to have Sun's Song to collect\n" //
                                        "them. This prevents needing to wait until night\n"//
                                        "for some locations.";                             //
                                                                                           //
/*------------------------------                                                           //
|       CHEST ANIMATIONS       |                                                           //
------------------------------*/                                                           //
string_view chestAnimDesc             = "Choose if you want the slow animation to play\n"  //
                                        "if a chest contains a major item.\n";             //
                                                                                           //
/*------------------------------                                                           //
|     CHEST SIZE AND COLOR     |                                                           //
------------------------------*/                                                           //
string_view chestSizeDesc             = "This option will change the appearance of all\n"  //
                                        "regular chests depending on their contents:\n"    //
                                        "Major Items    =    Big Wooden Chests\n"          //
                                        "Lesser Items   =    Small Wooden Chests\n"        //
                                        "Boss Keys      =    Big Fancy Chests\n"           //
                                        "Small Keys     =    Small Fancy Chests";          //
                                                                                           //
/*------------------------------                                                           //
|        INGAME SPOILERS       |                                                           //
------------------------------*/                                                           //
string_view ingameSpoilersShowDesc    = "Every spoiler is shown.";                         //
string_view ingameSpoilersHideDesc    = "Hides the spheres page and only shows a\n"        //
                                        "location's item after it has been found.";        //
                                                                                           //
/*------------------------------                                                           //
|      MENU OPENING BUTTON     |                                                           //
------------------------------*/                                                           //
string_view menuButtonDesc            = "Choose which button will bring up the Dungeon\n"  //
                                        "Information Menu. You can also use the menu to\n" //
                                        "buffer frame perfect inputs if you choose D-Pad"; //
                                                                                           //
/*------------------------------                                                           //
|     START WITH CONSUMABLES   |                                                           //
------------------------------*/                                                           //
string_view startWithConsumablesDesc  = "Start the game with maxed out Deku Sticks and Deku"
                                        "Nuts.";                                           //
                                                                                           //
/*------------------------------                                                           //
|     START WITH MAX RUPEES    |                                                           //
------------------------------*/                                                           //
string_view startWithMaxRupeesDesc    = "Start the game with a full wallet.\n"             //
                                        "Wallet upgrades will also fill the wallet.";      //
                                                                                           //
/*------------------------------                                                           //
|          ITEM POOL           |                                                           //
------------------------------*/                                                           //
string_view itemPoolPlentiful         = "Extra major items are added to the pool.";        //
string_view itemPoolBalanced          = "Original item pool.";                             //
string_view itemPoolScarce            = "Some excess items are removed, including health\n"//
                                        "upgrades.";                                       //
string_view itemPoolMinimal           = "Most excess items are removed.";                  //
                                                                                           //
/*------------------------------                                                           //
|          ICE TRAPS           |                                                           //
------------------------------*/                                                           //
string_view iceTrapsOff               = "All Ice Traps are removed.";                      //
string_view iceTrapsNormal            = "Only Ice Traps from the base item pool are placed.";
string_view iceTrapsExtra             = "Chance to add extra Ice Traps when junk items are\n"
                                        "added to the itempool.";                          //
string_view iceTrapsMayhem            = "All added junk items will be Ice Traps.";         //
string_view iceTrapsOnslaught         = "All junk items will be replaced by Ice Traps, even"
                                        "those in the base pool.";                         //
/*------------------------------                                                           //
|    REMOVE DOUBLE DEFENSE     |                                                           //
------------------------------*/                                                           //
string_view removeDDDesc              = "If set the double defense item will be removed\n" //
                                        "from the item pool for balanced and plentiful.";  //
/*------------------------------                                                           //
|   PROGRESSSIVE GORON SOWRD   |                                                           //
------------------------------*/                                                           //
string_view progGoronSword            = "Giant's Knife will always be found\n"             //
                                        "before Biggoron's Sword. Medigoron only starts\n" //
                                        "selling new knives once the Giant's Knife\n"      //
                                        "has been found and broken.";                      //
/*------------------------------                                                           //
|  USE FARORE'S WIND ANYWHERE  |                                                           //
------------------------------*/                                                           //
string_view faroresWindAnywhereDesc   = "Farore's Wind can be used outside of dungeons.";  //
                                                                                           //
/*------------------------------                                                           //
|    LIFT AGE RESTRICTIONS     |                                                           //
------------------------------*/                                                           //
string_view ageRestrictionsDesc       = "Remove age restrictions for inventory items.\n"   //
                                        "Select \"Choose\" to open the list of individual\n"
                                        "options.\n\n"                                     //
                                        "Most of the items won't appear correctly when\n"  //
                                        "used as the wrong version of Link, but they'll be\n"
                                        "fully functional otherwise.";                     //
                                                                                           //
/*------------------------------                                                           //
|    ENABLE ADULT DEKU STICK   |                                                           //
------------------------------*/                                                           //
string_view adultStickDesc            = "Adult Link can wield a deku stick. In game Adult\n"
                                        "Link will look like he's holding a Hylian Shield,\n"
                                        "but rest assured it is a deku stick.";            //
                                                                                           //
/*------------------------------                                                           //
|    ENABLE ADULT BOOMERANG    |                                                           //
------------------------------*/                                                           //
string_view adultBoomerangDesc        = "Adult Link can throw the boomerang.";             //
                                                                                           //
/*------------------------------                                                           //
|     ENABLE CHILD HAMMER      |                                                           //
------------------------------*/                                                           //
string_view childHammerDesc           = "Child Link can swing the Megaton Hammer.";        //
/*------------------------------                                                           //
|    ENABLE ADULT SLINGSHOT    |                                                           //
------------------------------*/                                                           //
string_view adultSlingshotDesc        = "Adult Link can use the Slingshot (but it looks\n" //
                                        "like the Bow).";                                  //
                                                                                           //
/*------------------------------                                                           //
|      ENABLE CHILD BOW        |                                                           //
------------------------------*/                                                           //
string_view childBowDesc              = "Child Link can use the Bow. It will look like the\n"
                                        "Slingshot, but will shoot arrows.";               //
                                                                                           //
/*------------------------------                                                           //
|    ENABLE CHILD HOOKSHOT     |                                                           //
------------------------------*/                                                           //
string_view childHookshotDesc         = "Child Link can use the Hookshot/Longshot.\n"      //
                                        "It will be difficult to aim, the red dot and\n"   //
                                        "laser won't appear and the hook will look like\n" //
                                        "a small bomb.";                                   //
/*------------------------------                                                           //
|   ENABLE CHILD IRON BOOTS    |                                                           //
------------------------------*/                                                           //
string_view childIronBootsDesc        = "Child Link can equip the Iron Boots.";            //
                                                                                           //
/*------------------------------                                                           //
|   ENABLE CHILD HOVER BOOTS   |                                                           //
------------------------------*/                                                           //
string_view childHoverBootsDesc       = "Child Link can equip the Hover Boots. The yellow\n"
                                        "circle beneath Link's feet won't appear.";        //
                                                                                           //
/*------------------------------                                                           //
|     ENABLE ADULT MASKS       |                                                           //
------------------------------*/                                                           //
string_view adultMasksDesc            = "Adult Link can equip masks.\n"                    //
                                        "\n"                                               //
                                        "This setting will not change the logic.";         //
                                                                                           //
/*------------------------------                                                           //
|  ENABLE ADULT KOKIRI SWORD   |                                                           //
------------------------------*/                                                           //
string_view adultKokiriSwordDesc      = "Adult Link can equip the Kokiri Sword.";          //
                                                                                           //
/*------------------------------                                                           //
|  ENABLE CHILD MASTER SWORD   |                                                           //
------------------------------*/                                                           //
string_view childMasterSwordDesc      = "Child Link can equip the Master Sword.";          //
                                                                                           //
/*------------------------------                                                           //
|  ENABLE CHILD BIGGORON SWORD |                                                           //
------------------------------*/                                                           //
string_view childBiggoronSwordDesc    = "Child Link can equip the Biggoron Sword and the\n"//
                                        "Giant's Knife.";                                  //
                                                                                           //
/*------------------------------                                                           //
|   ENABLE ADULT DEKU SHIELD   |                                                           //
------------------------------*/                                                           //
string_view adultDekuShieldDesc       = "Adult Link can equip the Deku Shield.";           //
                                                                                           //
/*------------------------------                                                           //
|  ENABLE CHILD MIRROR SHIELD  |                                                           //
------------------------------*/                                                           //
string_view childMirrorShieldDesc     = "Child Link can equip the Mirror Shield.";         //
                                                                                           //
/*------------------------------                                                           //
|   ENABLE CHILD GORON TUNIC   |                                                           //
------------------------------*/                                                           //
string_view childGoronTunicDesc       = "Child Link can equip the Goron Tunic.";           //
                                                                                           //
/*------------------------------                                                           //
|    ENABLE CHILD ZORA TUNIC   |                                                           //
------------------------------*/                                                           //
string_view childZoraTunicDesc        = "Child Link can equip the Zora Tunic.";            //
                                                                                           //
/*------------------------------                                                           //
|         GK DURABILITY        |                                                           //
------------------------------*/                                                           //
string_view gkDurabilityVanilla       = "The durability will always be set to 8.";         //
string_view gkDurabilityRandomRisk    = "Each Giant's Knife will get a random durability\n"//
                                        "between 1 and 128, with low being more common,\n" //
                                        "and with an average of 15.";                      //
string_view gkDurabilityRandomSafe    = "Each Giant's Knife will get a random durability\n"//
                                        "between 10 and 50, with an average of 30.";       //
                                                                                           //
/*------------------------------                                                           //
|      BLUE FIRE ARROWS        |                                                           //
------------------------------*/                                                           //
string_view blueFireArrowsDesc        = "Ice Arrows gain the ability to melt red ice\n"    //
                                        "and break the mud walls in Dodongo's Cavern.";    //
/*------------------------------                                                           //
|       SUNLIGHT ARROWS        |                                                           //
------------------------------*/                                                           //
string_view sunlightArrowsDesc        = "Light Arrows gain the ability to activate sun\n"  //
                                        "switches.";                                       //
/*------------------------------                                                           //
|         MULTIPLAYER          |                                                           //
------------------------------*/                                                           //
string_view mp_EnabledDesc            = "Enables multiplayer.\n"                           //
                                        "Other players will always be seen and heard\n"    //
                                        "regardless of the other settings.";               //
string_view mp_SharedProgressDesc     = "Progress and certain actors will be synced between"
                                        "everyone in the network that has this option on,\n"
                                        "the same seed hash, and the same sync id.";       //
string_view mp_SyncIdDesc             = "Limits shared progress to only sync with other\n" //
                                        "players that have the same sync ID. This is only\n"
                                        "necessary to set if multiple groups of players\n" //
                                        "play on the same seed hash, but only want to share"
                                        "their progress with certan people.\n"             //
                                        "For example, when doing a 2v2 race.";             //
string_view mp_SharedHealthDesc       = "Syncs health when shared progress is on,\n"       //
                                        "otherwise just shares the damage and recovery.";  //
string_view mp_SharedRupeesDesc       = "Syncs rupees when shared progress is on,\n"       //
                                        "otherwise just shares the gain and loss.";        //
string_view mp_SharedAmmoDesc         = "Syncs ammo when shared progress is on,\n"         //
                                        "otherwise just shares the gain and loss.";        //
                                                                                           //
/*------------------------------                                                           //
|       INGAME DEFAULTS        |                                                           //
------------------------------*/                                                           //
string_view zTargetingDesc            = "Sets L-Targeting to start as switch or hold.";    //
string_view cameraControlDesc         = "Sets the camera controls to start as normal or\n" //
                                        "with the y-axis inverted.";                       //
string_view motionControlDesc         = "Sets the motion controls to start on or off.";    //
string_view togglePlayMusicDesc       = "Starts the game with the music on or off.";       //
string_view togglePlaySFXDesc         = "Starts the game with the sound effects on or off.";
string_view silenceNaviDesc           = "Sets whether Navi should start silenced or not."; //
string_view ignoreMaskReactionDesc    = "Sets whether NPCs ignore the worn mask or not.\n" //
                                        "Does not apply when trading masks.";              //
                                                                                           //
/*------------------------------                                                           //
|      NAVI & TRAIL COLORS     |                                                           //
------------------------------*/                                                           //
string_view naviColorsDesc            = "Inner color is for the main light orb, outer color"
                                        "is for the aura.\n\n"                             //
                                        "The Rainbow option will make the color change\n"  //
                                        "continuously in a 3 seconds loop.";               //
string_view necessarySimpleModeDesc   = "For boomerang and sword trails, OoT3D uses a\n"   //
                                        "special texture that doesn't support every color.\n\n"
                                        "Unsupported colors will always use the plain\n"   //
                                        "texture from OoT instead:\n"                      //
                                        "- Black and Purple sword trails\n"                //
                                        "- White, Black and Purple boomerang trails\n"     //
                                        "- Random or Custom colors without at least one\n" //
                                        "  maxed out RGB component.";                      //
string_view alwaysSimpleModeDesc      = "All boomerang and sword trails will use the plain\n"
                                        "texture from OoT, regardless of what color is\n"  //
                                        "chosen.";                                         //
                                                                                           //
/*------------------------------                                                           //
|         COLORED KEYS         |                                                           //
------------------------------*/                                                           //
string_view coloredKeysDesc           = "If set, small key models will be colored\n"       //
                                        "differently depending on which dungeon they can be"
                                        "used in. Forest Temple keys are green. Fire Temple"
                                        "keys are red. etc.";                              //
string_view coloredBossKeysDesc       = "If set, boss key models will be colored\n"        //
                                        "differently depending on which dungeon they can be"
                                        "used in. The Forest Temple boss key is green. The "
                                        "Fire Temple boss key is red. etc.";               //
/*------------------------------                                                           //
|         MIRROR WORLD         |                                                           //
------------------------------*/                                                           //
string_view mirrorWorldDesc           = "If set, the world will be mirrored.";             //
                                                                                           //
/*------------------------------                                                           //
|        SHUFFLE MUSIC         |                                                           //
------------------------------*/                                                           //
string_view musicRandoDesc            = "Randomize the music in the game.";                //
string_view shuffleBGMDesc            = "Randomize area background music, either\n"        //
                                        "grouped into categories or all mixed together.";  //
string_view fanfaresOffDesc           = "Fanfares are not shuffled.";                      //
string_view onlyFanfaresDesc          = "Fanfares and ocarina songs are shuffled in\n"     //
                                        "separate pools.";                                 //
string_view fanfaresOcarinaDesc       = "Fanfares and ocarina songs are shuffled together\n"
                                        "in the same pool.";                               //
string_view shuffleOcaMusicDesc       = "The music that plays back after you play an"      //
                                        "ocarina song is randomized.";                     //
/*------------------------------                                                           //
|         SHUFFLE SFX          |                                                           //
------------------------------*/                                                           //
string_view shuffleSFXOff             = "Sound effects will stay vanilla.";                //
string_view shuffleSFXAll             = "All sound effects will be shuffled.";             //
string_view shuffleSFXSceneSpecific   = "All sound effects will be shuffled, but will also\n"
                                        "be different in each scene.";                     //
string_view shuffleSFXChaos           = "Each sound effect will become random about\n"     //
                                        "every second.";                                   //
string_view shuffleSFXCategorically   = "Sound effects will be shuffled in categories.\n"  //
                                        "\n"                                               //
                                        "The sound may get annoying fast when disabled.";  //
/*------------------------------                                                           //
|    RANDOM TRAP DAMAGE TYPE   |                                                           //
------------------------------*/                                                           //
string_view randomTrapDmgDesc         = "All traps will be the base game ice trap";        //
                                                                                           //
string_view basicTrapDmgDesc          = "All alternative traps will cause a small damage\n"//
                                        "and no other negative effects";                   //
                                                                                           //
string_view advancedTrapDmgDesc       = "Some chest traps will burn your Deku Shield or\n" //
                                        "cause a lot of damage (with one-hit protection)"; //
                                                                                           //--------------//
/*------------------------------                                                                           //
|  DETAILED LOGIC EXPLANATIONS |                                                                           //
------------------------------*/
string_view ToggleLogicNoneDesc                       = "Disables all the Detailed Logic tricks.";         //
string_view ToggleLogicNoviceDesc                     = "Enables only the easier Detailed Logic tricks";   //
string_view ToggleLogicIntermediateDesc               = "Enables all but the harder Detailed Logic tricks.";
string_view ToggleLogicExpertDesc                     = "Enables all the Detailed Logic tricks.";          //
                                                                                                           //
string_view LogicGrottosWithoutAgonyDesc              = "Difficulty: Novice\n"                             //
                                                        "Grottos can be accessed without Stone of Agony,\n"//
                                                        "simply by knowing where they are located.";       //
string_view LogicVisibleCollisionDesc                 = "Difficulty: Novice\n"                             //
                                                        "The closed Kakariko Village Gate can be crossed\n"//
                                                        "when coming from Death Mountain Trail.\n"         //
                                                        "Useful for Entrance Randomiser.";                 //
string_view LogicFewerTunicRequirementsDesc           = "Difficulty: Novice\n"                             //
                                                        "Allows the following possible without Tunics:\n"  //
                                                        "- Enter Water Temple.\n"                          //
                                                        "- Enter Fire Temple. Only the first floor\n"      //
                                                        "  is accessible, and not Volvagia.\n"             //
                                                        "- Zora's Fountain Bottom Freestanding PoH.\n"     //
                                                        "- Gerudo Training Grounds Underwater Silver Rupee\n"
                                                        "Chest. May need to make multiple trips.";         //
string_view LogicLostWoodsGSBeanDesc                  = "Difficulty: Novice\n"                             //
                                                        "You can collect the token with a precise Hookshot\n"
                                                        "use, as long as you can kill the Skulltula first.\n"
                                                        "It can be killed using Longshot, Bow, Bombchus\n" //
                                                        "or Din's Fire.";                                  //
string_view LogicLabDivingDesc                        = "Difficulty: Novice\n"                             //
                                                        "Remove the Iron Boots in the midst of Hookshotting"
                                                        "the underwater crate";                            //
string_view LogicLabWallGSDesc                        = "Difficulty: Intermediate\n"                       //
                                                        "The jump slash to actually collect the token is\n"//
                                                        "somewhat precise";                                //
string_view LogicGraveyardPoHDesc                     = "Difficulty: Novice\n"                             //
                                                        "Using a precise moving setup you can obtain the\n"//
                                                        "Piece of Heart by having the Boomerang interact\n"//
                                                        "with it along the return path.";                  //
string_view LogicChildDampeRacePoHDesc                = "Difficulty: Intermediate\n"                       //
                                                        "It is possible to complete the second dampe race\n"
                                                        "as child in under a minute, but it is a strict\n" //
                                                        "time limit.";                                     //
string_view LogicGVHammerChestDesc                    = "Difficulty: Novice\n"                             //
                                                        "The chest can be reached by sidehopping between\n"//
                                                        "the wall and the east most hammer rock.";         //
string_view LogicGerudoKitchenDesc                    = "Difficulty: Intermediate\n"                       //
                                                        "The logic normally guarantees one of Bow,\n"      //
                                                        "Hookshot, or Hover Boots.";                       //
string_view LogicLensWastelandDesc                    = "Difficulty: Expert\n"                             //
                                                        "By memorizing the path, you can travel through the"
                                                        "Wasteland without using the Lens of Truth to see\n"
                                                        "the Poe. The equivalent trick for going in reverse"
                                                        "through the Wasteland is \"Reverse Wasteland\"."; //
string_view LogicReverseWastelandDesc                 = "Difficulty: Expert\n"                             //
                                                        "By memorizing the path, you can travel through the"
                                                        "Wasteland in reverse.";                           //
string_view LogicColossusGSDesc                       = "Difficulty: Expert\n"                             //
                                                        "Somewhat precise. If you kill enough Leevers you\n"
                                                        "can get enough of a break to take some time to aim"
                                                        "more carefully.";                                 //
string_view LogicOutsideGanonsGSDesc                  = "Difficulty: Intermediate\n"                       //
                                                        "Can be killed with a precise sidehop jumpslash\n" //
                                                        "from the top of the broken arch.";                //
string_view LogicManOnRoofDesc                        = "Difficulty: Novice\n"                             //
                                                        "Can be reached by side-hopping off the watchtower.";
string_view LogicWindmillPoHHookshotDesc              = "Difficulty: Novice\n"                             //
                                                        "Adult Link can reach the upper area of the windmill"
                                                        "using the hookshot and a midair jump slash.";     //
string_view LogicDMTBombableDesc                      = "Difficulty: Expert\n"                             //
                                                        "Child Link can blow up the wall using a nearby\n" //
                                                        "bomb flower. You must backwalk with the flower and"
                                                        "then quickly throw it toward the wall.";          //
string_view LogicDMTSoilGSDesc                        = "Difficulty: Intermediate\n"                       //
                                                        "Bugs will go into the soft soil even while the\n" //
                                                        "boulder is still blocking the entrance if dropped\n"
                                                        "from above. Then, using a precise moving setup you"
                                                        "can kill the Gold Skulltula and obtain the token by"
                                                        "having the Boomerang collect it while returning.";//
string_view LogicDMTSummitHoverDesc                   = "Difficulty: Intermediate\n"                       //
                                                        "By rolling around the lower boulder with hover\n" //
                                                        "boots and grabbing the ledge with the higher\n"   //
                                                        "boulder near the wall, you'll be able to grab the\n"
                                                        "ledge above it to reach the summit.";             //
string_view LogicLinkGoronDinsDesc                    = "Difficulty: Intermediate\n"                       //
                                                        "The timing is quite awkward.";                    //
string_view LogicGoronCityLeftMostDesc                = "Difficulty: Novice\n"                             //
                                                        "A precise backwalk starting from on top of the\n" //
                                                        "crate and ending with a precisely-timed backflip\n"
                                                        "can reach this chest without needing either the\n"//
                                                        "Hammer or Silver Gauntlets.";                     //
string_view LogicGoronCityPotDesc                     = "Difficulty: Expert\n"                             //
                                                        "A Bombchu can be used to stop the spinning pot,\n"//
                                                        "but it can be quite finicky to get it to work.";  //
string_view LogicGoronCityPotWithStrengthDesc         = "Difficulty: Intermediate\n"                       //
                                                        "Allows for stopping the Goron City Spinning Pot\n"//
                                                        "using a bomb flower alone, requiring strength in\n"
                                                        "lieu of inventory explosives.";                   //
string_view LogicChildRollingWithStrengthDesc         = "Difficulty: Expert\n"                             //
                                                        "Use the bombflower on the stairs or near\n"       //
                                                        "Medigoron. Timing is tight, especially without\n" //
                                                        "backwalking.";                                    //
string_view LogicCraterUpperToLowerDesc               = "Difficulty: Intermediate\n"                       //
                                                        "With the Hammer, you can jump slash the rock twice"
                                                        "in the same jump in order to destroy it before you"
                                                        "fall into the lava.";                             //
string_view LogicCraterBeanPoHWithHoversDesc          = "Difficulty: Expert\n"                             //
                                                        "Hover from the base of the bridge near Goron City\n"
                                                        "and walk up the very steep slope.";               //
string_view LogicBiggoronBoleroDesc                   = "Difficulty: Intermediate\n"                       //
                                                        "Playing a warp song normally causes a trade item\n"
                                                        "to spoil immediately, however, it is possible use\n"
                                                        "Bolero to reach Biggoron and still deliver the Eye"
                                                        "Drops before they spoil. If you do not wear the\n"
                                                        "Goron Tunic, the heat timer inside the crater will"
                                                        "override the trade item's timer. When you exit to\n"
                                                        "Death Mountain Trail you will have one second to\n"
                                                        "show the Eye Drops before they expire.";          //
string_view LogicZoraRiverLowerDesc                   = "Difficulty: Novice\n"                             //
                                                        "Adult can reach this PoH with a precise jump, no\n"
                                                        "Hover Boots required.";                           //
string_view LogicZoraRiverUpperDesc                   = "Difficulty: Novice\n"                             //
                                                        "Adult can reach this PoH with a precise jump, no\n"
                                                        "Hover Boots required.";                           //
string_view LogicZFGreatFairyDesc                     = "Difficulty: Novice\n"                             //
                                                        "Destroying the boulders blocking the hidden area\n"
                                                        "with silver gauntlets and hammer lets you pass\n" //
                                                        "under the wall to the great fairy fountain.";     //
string_view LogicDekuB1WebsWithBowDesc                = "Difficulty: Novice\n"                             //
                                                        "All spider web walls in the Deku Tree basement can"
                                                        "be burnt as adult with just a bow by shooting\n"  //
                                                        "through torches. This trick only applies to the\n"//
                                                        "circular web leading to Gohma; the two vertical\n"//
                                                        "webs are always in logic. Backflip onto the chest\n"
                                                        "near the torch at the bottom of the vine wall.\n" //
                                                        "With precise positioning you can shoot through the"
                                                        "torch to the right edge of the circular web.";    //
string_view LogicDekuB1SkipDesc                       = "Difficulty: Intermediate\n"                       //
                                                        "A precise jump can be used to skip needing to use\n"
                                                        "the Slingshot to go around B1 of the Deku Tree. If"
                                                        "used with the \"Closed Forest\" setting, a\n"     //
                                                        "Slingshot will not be guaranteed to exist\n"      //
                                                        "somewhere inside the Forest. This trick applies to"
                                                        "both Vanilla and Master Quest.";                  //
string_view LogicDekuBasementGSDesc                   = "Difficulty: Intermediate\n"                       //
                                                        "Can be defeated by doing a precise jump slash.";  //
string_view LogicDCStaircaseDesc                      = "Difficulty: Intermediate\n"                       //
                                                        "The Bow can be used to knock down the stairs with\n"
                                                        "two well-timed shots.";                           //
string_view LogicDCJumpDesc                           = "Difficulty: Novice\n"                             //
                                                        "Jump is adult only.";                             //
string_view LogicDCSlingshotSkipDesc                  = "Difficulty: Expert\n"                             //
                                                        "With precise platforming, child can cross the\n"  //
                                                        "platforms while the flame circles are there. When\n"
                                                        "enabling this trick, it's recommended that you\n" //
                                                        "also enable the Adult variant: \"Dodongo's Cavern\n"
                                                        "Spike Trap Room Jump without Hover Boots\".";     //
string_view LogicDCScarecrowGSDesc                    = "Difficulty: Intermediate\n"                       //
                                                        "You can jump off an Armos Statue to reach the\n"  //
                                                        "alcove with the Gold Skulltula. It takes quite a\n"
                                                        "long time to pull the statue the entire way. The\n"
                                                        "jump to the alcove can be a bit picky when done\n"//
                                                        "as child.";                                       //
string_view LogicJabuBossGSAdultDesc                  = "Difficulty: Intermediate\n"                       //
                                                        "You can easily get over to the door to the near\n"//
                                                        "boss area early with Hover Boots. The tricky part\n"
                                                        "is getting through the door without being able to\n"
                                                        "use a box to keep the switch pressed. One way is\n"
                                                        "to quickly roll from the switch and open the door\n"
                                                        "before it closes.";                               //
string_view LogicJabuScrubJumpDiveDesc                = "Difficulty: Novice\n"                             //
                                                        "Standing above the underwater tunnel leading to\n"//
                                                        "the scrub, jump down and swim through the tunnel.\n"
                                                        "This allows adult to access the scrub with no\n"  //
                                                        "Scale or Iron Boots.";                            //
string_view LogicForestOutsideBackdoorDesc            = "Difficulty: Intermediate\n"                       //
                                                        "With a precise jump slash from above, you can\n"  //
                                                        "reach the backdoor to the west courtyard without\n"
                                                        "Hover Boots. Applies to both Vanilla and Master\n"//
                                                        "Quest.";                                          //
string_view LogicForestDoorFrameDesc                  = "Difficulty: Intermediate\n"                       //
                                                        "A precise Hover Boots movement from the upper\n"  //
                                                        "balconies in this courtyard can be used to get on\n"
                                                        "top of the door frame. Applies to both Vanilla and"
                                                        "Master Quest. In Vanilla, from on top the door\n" //
                                                        "frame you can summon Pierre, allowing you to\n"   //
                                                        "access the falling ceiling room early. In Master\n"
                                                        "Quest, this allows you to obtain the GS on the\n" //
                                                        "door frame as adult without Hookshot or Song of\n"//
                                                        "Time.";                                           //
string_view LogicForestOutdoorEastGSDesc              = "Difficulty: Novice\n"                             //
                                                        "Precise Boomerang throws can allow child to kill\n"
                                                        "the Skulltula and collect the token.";            //
string_view LogicFireBossDoorJumpDesc                 = "Difficulty: Intermediate\n"                       //
                                                        "The Fire Temple Boss Door can be reached with a\n"//
                                                        "precise jump. You must be touching the side wall\n"
                                                        "of the room so that Link will grab the ledge from\n"
                                                        "farther away than is normally possible.";         //
string_view LogicFireStrengthDesc                     = "Difficulty: Expert\n"                             //
                                                        "A precise jump can be used to skip pushing the\n" //
                                                        "block.";                                          //
string_view LogicFireScarecrowDesc                    = "Difficulty: Novice\n"                             //
                                                        "Also known as \"Pixelshot\". The Longshot can\n"  //
                                                        "reach the target on the elevator itself, allowing\n"
                                                        "you to skip needing to spawn the scarecrow.";     //
string_view LogicFireFlameMazeDesc                    = "Difficulty: Expert\n"                             //
                                                        "If you move quickly you can sneak past the edge of"
                                                        "a flame wall before it can rise up to block you.\n"
                                                        "To do it without taking damage is more precise.\n"//
                                                        "Allows you to progress without needing either a\n"//
                                                        "Small Key or Hover Boots.";                       //
string_view LogicFireSongOfTimeDesc                   = "Difficulty: Intermediate\n"                       //
                                                        "A precise jump can be used to reach this room.";  //
string_view LogicWaterTempleTorchLongshotDesc         = "Difficulty: Novice\n"                             //
                                                        "Stand on the eastern side of the central pillar\n"//
                                                        "and longshot the torches on the bottom level.\n"  //
                                                        "Swim through the corridor and float up to the top\n"
                                                        "level. This allows access to this area and lower\n"
                                                        "water levels without Iron Boots. The majority of\n"
                                                        "the tricks that allow you to skip Iron Boots in\n"//
                                                        "the Water Temple are not going to be relevant\n"  //
                                                        "unless this trick is first enabled.";             //
string_view LogicWaterTempleUpperBoostDesc            = "Difficulty: Expert\n"                             //
                                                        "Stand on the corner closest to the upper ledge\n" //
                                                        "where you play Zelda's Lullaby to raise the water\n"
                                                        "and put a bomb down behind you. Hold forward when\n"
                                                        "the bomb explodes and Link should jump just far\n"//
                                                        "enough to grab the ledge.";                       //
string_view LogicWaterCentralBowDesc                  = "Difficulty: Intermediate\n"                       //
                                                        "A very precise Bow shot can hit the eye switch\n" //
                                                        "from the floor above. Then, you can jump down into"
                                                        "the hallway and make through it before the gate\n"//
                                                        "closes. It can also be done as child, using the\n"//
                                                        "Slingshot instead of the Bow.";                   //
string_view LogicWaterCentralGSFWDesc                 = "Difficulty: Novice\n"                             //
                                                        "If you set Farore's Wind inside the central pillar"
                                                        "and then return to that warp point after raising\n"
                                                        "the water to the highest level, you can obtain\n" //
                                                        "this Skulltula Token with Hookshot or Boomerang.";//
string_view LogicWaterCrackedWallNothingDesc          = "Difficulty: Expert\n"                             //
                                                        "A precise jump slash (among other methods) will\n"//
                                                        "get you to the cracked wall without needing the\n"//
                                                        "Hover Boots or to raise the water to the middle\n"//
                                                        "level. This trick supersedes \"Water Temple\n"    //
                                                        "Cracked Wall with Hover Boots\".";                //
string_view LogicWaterCrackedWallHoversDesc           = "Difficulty: Expert\n"                             //
                                                        "With a midair side-hop while wearing the Hover\n" //
                                                        "Boots, you can reach the cracked wall without\n"  //
                                                        "needing to raise the water up to the middle level.";
string_view LogicWaterBossKeyRegionDesc               = "Difficulty: Intermediate\n"                       //
                                                        "With precise Hover Boots movement it is possible\n"
                                                        "to reach the boss key chest's region without\n"   //
                                                        "needing the Longshot. It is not necessary to take\n"
                                                        "damage from the spikes. The Gold Skulltula Token\n"
                                                        "in the following room can also be obtained with\n"//
                                                        "just the Hover Boots.";                           //
string_view LogicWaterBKJumpDiveDesc                  = "Difficulty: Intermediate\n"                       //
                                                        "Stand on the very edge of the raised corridor\n"  //
                                                        "leading from the push block room to the rolling\n"//
                                                        "boulder corridor. Face the gold skulltula on the\n"
                                                        "waterfall and jump over the boulder corridor floor"
                                                        "into the pool of water, swimming right once\n"    //
                                                        "underwater. This allows access to the boss key\n" //
                                                        "room without Iron boots.";                        //
string_view LogicWaterNorthBasementLedgeJumpDesc      = "Difficulty: Novice\n"                             //
                                                        "In the northern basement there's a ledge from\n"  //
                                                        "where, in vanilla Water Temple, boulders roll out\n"
                                                        "into the room. Normally to jump directly to this\n"
                                                        "ledge logically requires the Hover Boots, but with"
                                                        "precise jump, it can be done without them. This\n"//
                                                        "trick applies to both Vanilla and Master Quest."; //
string_view LogicWaterDragonAdultDesc                 = "Difficulty: Expert\n"                             //
                                                        "Normally you need both Hookshot and Iron Boots to\n"
                                                        "hit the switch and swim through the tunnel to get\n"
                                                        "to the chest. But by hitting the switch from dry\n"
                                                        "land, using one of Bombchus, Hookshot, or Bow, it\n"
                                                        "is possible to skip one or both of those\n"       //
                                                        "requirements. After the gate has been opened, a\n"//
                                                        "well-timed dive with at least the Silver Scale\n" //
                                                        "could be used to swim through the tunnel.";       //
string_view LogicWaterDragonJumpDiveDesc              = "Difficulty: Expert\n"                             //
                                                        "If you come into the dragon statue room from the\n"
                                                        "serpent river, you can jump down from above and\n"//
                                                        "get into the tunnel without needing either Iron\n"//
                                                        "Boots or a Scale. This trick applies to both\n"   //
                                                        "Vanilla and Master Quest. In Vanilla, you must\n" //
                                                        "shoot the switch from above with the Bow, and then"
                                                        "quickly get through the tunnel before the gate\n" //
                                                        "closes.";                                         //
string_view LogicWaterRiverGSDesc                     = "Difficulty: Novice\n"                             //
                                                        "Standing on the exposed ground toward the end of\n"
                                                        "the river, a precise Longshot use can obtain the\n"
                                                        "token.";                                          //
string_view LogicWaterFallingPlatformGSDesc           = "Difficulty: Intermediate\n"                       //
                                                        "If you stand on the very edge of the platform,\n" //
                                                        "this Gold Skulltula can be obtained with only the\n"
                                                        "Hookshot.";                                       //
string_view LogicSpiritLowerAdultSwitchDesc           = "Difficulty: Novice\n"                             //
                                                        "A bomb can be used to hit the switch on the\n"    //
                                                        "ceiling, but it must be thrown from a particular\n"
                                                        "distance away and with precise timing.";          //
string_view LogicSpiritChildBombchuDesc               = "Difficulty: Intermediate\n"                       //
                                                        "A carefully-timed Bombchu can hit the switch.";   //
string_view LogicSpiritWallDesc                       = "Difficulty: Expert\n"                             //
                                                        "The logic normally guarantees a way of dealing\n" //
                                                        "with both the Beamos and the Walltula before\n"   //
                                                        "climbing the wall.";                              //
string_view LogicSpiritLobbyGSDesc                    = "Difficulty: Intermediate\n"                       //
                                                        "Standing on the highest part of the arm of the\n" //
                                                        "statue, a precise Boomerang throw can kill and\n" //
                                                        "obtain this Gold Skulltula. You must throw the\n" //
                                                        "Boomerang slightly off to the side so that it\n"  //
                                                        "curves into the Skulltula, as aiming directly at\n"
                                                        "it will clank off of the wall in front.";         //
string_view LogicSpiritMapChestDesc                   = "Difficulty: Intermediate\n"                       //
                                                        "To get a line of sight from the upper torch to the"
                                                        "map chest torches, you must pull an Armos statue\n"
                                                        "all the way up the stairs.";                      //
string_view LogicSpiritSunChestDesc                   = "Difficulty: Expert\n"                             //
                                                        "Using the blocks in the room as platforms you can\n"
                                                        "get lines of sight to all three torches. The timer"
                                                        "on the torches is quite short so you must move\n" //
                                                        "quickly in order to light all three.";            //
string_view LogicShadowFireArrowEntryDesc             = "Difficulty: Expert\n"                             //
                                                        "It is possible to light all of the torches to open"
                                                        "the Shadow Temple entrance with just Fire Arrows,\n"
                                                        "but you must be very quick, precise, and strategic"
                                                        "with how you take your shots.";                   //
string_view LogicShadowUmbrellaDesc                   = "Difficulty: Expert\n"                             //
                                                        "A very precise Hover Boots movement from off of\n"//
                                                        "the lower chest can get you on top of the crushing\n"
                                                        "spikes without needing to pull the block. Applies\n"
                                                        "to both Vanilla and Master Quest.";               //
string_view LogicShadowFreestandingKeyDesc            = "Difficulty: Intermediate\n"                       //
                                                        "Release the Bombchu with good timing so that it\n"//
                                                        "explodes near the bottom of the pot.";            //
string_view LogicShadowStatueDesc                     = "Difficulty: Novice\n"                             //
                                                        "By sending a Bombchu around the edge of the gorge,"
                                                        "you can knock down the statue without needing a\n"//
                                                        "Bow. Applies in both vanilla and MQ Shadow.";     //
string_view LogicChildDeadhandDesc                    = "Difficulty: Novice\n"                             //
                                                        "Requires 9 sticks or 5 jump slashes.";            //
string_view LogicGtgWithoutHookshotDesc               = "Difficulty: Expert\n"                             //
                                                        "The final silver rupee on the ceiling can be\n"   //
                                                        "reached by being pulled up into it by the\n"      //
                                                        "Wallmaster.\n"                                    //
                                                        "Then, you must also reach the exit of the room\n" //
                                                        "without the use of the Hookshot. If you move\n"   //
                                                        "quickly, you can sneak past the edge of a\n"      //
                                                        "flame wall before it can rise up to block you.\n" //
                                                        "To do so without taking damage is more precise";  //
string_view LogicGtgFakeWallDesc                      = "Difficulty: Novice\n"                             //
                                                        "A precise Hover Boots use from the top of the\n"  //
                                                        "chest can allow you to grab the ledge without\n"  //
                                                        "needing the usual requirements.";                 //
string_view LogicLensSpiritDesc                       = "Difficulty: Novice\n"                             //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Spirit Temple.";                                  //
string_view LogicLensShadowDesc                       = "Difficulty: Novice\n"                             //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Shadow Temple before the invisible moving platform";
string_view LogicLensShadowBackDesc                   = "Difficulty: Intermediate\n"                       //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Shadow Temple beyond the invisible moving platform";
string_view LogicLensBotwDesc                         = "Difficulty: Intermediate\n"                       //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Bottom of the Well.";                             //
string_view LogicLensGtgDesc                          = "Difficulty: Novice\n"                             //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Gerudo Training Grounds.";                        //
string_view LogicLensCastleDesc                       = "Difficulty: Intermediate\n"                       //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Ganon's Castle.";                                 //
string_view LogicLensJabuMQDesc                       = "Difficulty: Novice\n"                             //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Jabu Jabu's Belly MQ.";                           //
string_view LogicLensSpiritMQDesc                     = "Difficulty: Novice\n"                             //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Spirit Temple MQ.";                               //
string_view LogicLensShadowMQDesc                     = "Difficulty: Novice\n"                             //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Shadow Temple MQ before the invisible moving \n"  //
                                                        "platform";                                        //
string_view LogicLensShadowMQBackDesc                 = "Difficulty: Intermediate\n"                       //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Shadow Temple MQ beyond the invisible moving \n"  //
                                                        "platform";                                        //
string_view LogicLensBotwMQDesc                       = "Difficulty: Novice\n"                             //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Bottom of the Well MQ.";                          //
string_view LogicLensGtgMQDesc                        = "Difficulty: Novice\n"                             //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Gerudo Training Grounds MQ.";                     //
string_view LogicLensCastleMQDesc                     = "Difficulty: Intermediate\n"                       //
                                                        "Removes the requirements for the Lens of Truth in\n"
                                                        "Ganon's Castle MQ.";                              //
string_view LogicSpiritTrialHookshotDesc              = "Difficulty: Intermediate\n"                       //
                                                        "A precise jump off of an Armos can collect the\n" //
                                                        "highest rupee.";                                  //
                                                                                                           //
string_view LogicFlamingChestsDesc                    = "Difficulty: Novice\n"                             //
                                                        "The chests encircled in flames in Gerudo Training\n"
                                                        "Grounds and in Spirit Temple can be opened by\n"  //
                                                        "running into the flames while Link is invincible\n"
                                                        "after taking damage.";                            //
                                                                                                           //
/*------------------------------                                                                           //
|           GLITCHES           |                                                                           //
------------------------------*/                                                                           //
const std::vector<string_view> GlitchDifficulties{"Novice", "Intermediate", "Advanced", "Expert", "Hero"}; //
/* Restricted Items */                                                                                     //
string_view GlitchRestrictedItemsDescDisabled         = "Swapping an item that can normally be used in an\n"
                                                        "area with one that would be dimmed will let you\n"
                                                        "use that item for 1 frame after closing your\n"   //
                                                        "inventory. This can be useful on its own or in\n" //
                                                        "combination with other glitches.";                //
string_view GlitchRestrictedItemsDescNovice           = "You may be required to use restricted items.";    //
/* Super Stab */                                                                                           //
string_view GlitchSuperStabDescDisabled               = "Forcing sticks to unequip during a crouch stab by\n"
                                                        "breaking it and moving them in your inventory has\n"
                                                        "the effect of hitting all spherical collision.";  //
string_view GlitchSuperStabDescNovice                 = "You may be expected to hit switches or kill gold\n"
                                                        "skulltulas with a super stab.";                   //
/* ISG */                                                                                                  //
string_view GlitchISGDescDisabled                     = "Shortened to ISG, allows Link's melee weapon to\n"//
                                                        "be in a constant swinging state. Simply touching\n"
                                                        "objects with this causes them to get hit.\n"      //
                                                        "Putting away the weapon while ISG is on hits\n"   //
                                                        "any object with a spherical hitbox,\n"            //
                                                        "such as small skulltulas. It is initiated by\n"   //
                                                        "interrupting a crouch stab.";                     //
string_view GlitchISGDescNovice                       = "ISG may be required to kill certain enemies,\n"   //
                                                        "or to Bomb Hover when enabled.";                  //
string_view GlitchISGDescIntermediate                 = "You may be required to use a bomb to activate ISG.";
string_view GlitchISGDescAdvanced                     = "You may be required to use a bomb to activate ISG\n"
                                                        "repeatedly or while under attack.";               //
/* Hover */                                                                                                //
string_view GlitchHoverDescDisabled                   = "Hovering allows Link to consecutively backflip\n" //
                                                        "in the air without falling. By shielding\n"       //
                                                        "damage with ISG on, Link will stay in midair.\n"  //
                                                        "While bombs aren't always required, this option\n"//
                                                        "will always expect them to be used.\n"            //
                                                        "\n"                                               //
                                                        "Requires ISG to be enabled.";                     //
string_view GlitchHoverDescNovice                     = "Only bombchus are required for hovering.";        //
string_view GlitchHoverDescIntermediate               = "Some hovers may require that you start from flat\n"
                                                        "terrain, which requires somewhat precise timing.";//
string_view GlitchHoverDescAdvanced                   = "Usage of regular bombs will now also be expected,\n"
                                                        "which may require consecutive precise timings.";  //
/* Bomb OI */                                                                                              //
string_view GlitchBombOIDescDisabled                  = "Allowing a bomb to explode in Link's hands while\n"
                                                        "moving and then attempting to pull out a cutscene\n"
                                                        "item on a specific frame will cause Link to play\n"
                                                        "an invisible ocarina instead.\n"                  //
                                                        "This will only work if bombs are not dimmed on the"
                                                        "frame you use the cutscene item.";                //
string_view GlitchBombOIDescNovice                    = "You may be expected to use ocarina items with a\n"//
                                                        "bomb to play warp songs.";                        //
string_view GlitchBombOIDescIntermediate              = "You may be expected to use ocarina items with a\n"//
                                                        "bomb to play the ocarina where Link's position\n" //
                                                        "matters.";                                        //
string_view GlitchBombOIDescAdvanced                  = "You may be expected to use restricted items to use"
                                                        "the cutscene item or to make the bombs usable on\n"
                                                        "the correct frame.";                              //
string_view GlitchBombOIDescExpert                    = "You may be expected to use restricted items to\n" //
                                                        "perform ocarina items where Link's position isn't\n"
                                                        "particularly lenient";                            //
/* Hover Boost */                                                                                          //
string_view GlitchHoverBoostDescDisabled              = "Equipping hover boots when Link takes damage will\n"
                                                        "cause him to keep the high knockback speed and\n" //
                                                        "lets him traverse large gaps.\n"                  //
                                                        "If performed at the edge of a platform Link will\n"
                                                        "instead perform a mega jump which has less range\n"
                                                        "but more height than a hover boost.";             //
string_view GlitchHoverBoostDescNovice                = "Hover boosts that do not need the maximum speed\n"//
                                                        "may be required.";                                //
string_view GlitchHoverBoostDescIntermediate          = "Hover boosts that do need the maximum speed may be"
                                                        "required.";                                       //
string_view GlitchHoverBoostDescAdvanced              = "Hover boosts that use more complex movement during"
                                                        "the hover may be required.";                      //
/* Super Slide */                                                                                          //
string_view GlitchSuperSlideDescDisabled              = "Holding the circle pad just outside the dead zone\n"
                                                        "will cause Link to turn on the spot which locks\n"//
                                                        "his speed. This can be used to preserve high\n"   //
                                                        "speeds indefinitely.";                            //
string_view GlitchSuperSlideDescNovice                = "Forward extended super slides (FESSes) where Link\n"
                                                        "is damaged by an explosion may be required.";     //
string_view GlitchSuperSlideDescIntermediate          = "Hammer extended super slides where the recoil from"
                                                        "a hammer crouch stab is used may be required.";   //
string_view GlitchSuperSlideDescAdvanced              = "Hyper Extended Super Slides (HESSes) and\n"       //
                                                        "damageless FESSes where Link rolls into a bomb may"
                                                        "be required.";                                    //
string_view GlitchSuperSlideDescExpert                = "HESSes with more precise movement may be required.";
/* Megaflip */                                                                                             //
string_view GlitchMegaflipDescDisabled                = "A backflip or sidehop with high speed from an\n"  //
                                                        "attack hitting your shield during i-frames. This\n"
                                                        "is normally achieved by rolling into an explosion.\n"
                                                        "Equipping hover boots to preserve the high speed\n"
                                                        "when landing is known as a hoverflip.";           //
string_view GlitchMegaflipDescNovice                  = "You may be expected to perform megaflips on flat\n"
                                                        "ground with bombs.";                              //
string_view GlitchMegaflipDescIntermediate            = "You may be expected to perform megaflips in small\n"
                                                        "areas, distance megaflips, or hoverflips with a\n"//
                                                        "bomb.";                                           //
string_view GlitchMegaflipDescAdvanced                = "You may be expected to perform hoverflips with\n" //
                                                        "difficult midair movement, or distance megaflips\n"
                                                        "under time pressure with a bomb.\n\n"             //
                                                        "Additionally, you may be expected to perform\n"   //
                                                        "novice megaflips with a bombchu";                 //
string_view GlitchMegaflipDescExpert                  = "You may be expected to perform intermediate\n"    //
                                                        "megaflips with a bombchu.";                       //
string_view GlitchMegaflipDescHero                    = "You may be expected to perform any megaflip with a"
                                                        "bombchu.";                                        //
/* A-Slide */                                                                                              //
string_view GlitchASlideDescDisabled                  = "An A-slide is performed the same as a megaflip\n" //
                                                        "except without pressing the A button at the end.\n"
                                                        "This causes child Link's collision to glitch below"
                                                        "the ground and lets him bypass certain actors.";  //
string_view GlitchASlideDescNovice                    = "You may be expected to perform A-slides to pass\n"//
                                                        "actors you can't press A to interact with.";      //
string_view GlitchASlideDescIntermediate              = "You may be expected to perform A-slides quickly or"
                                                        "around actors that can be interacted with.";      //
string_view GlitchASlideDescAdvanced                  = "You may be expected to perform novice A-slides\n" //
                                                        "with a bombchu.";                                 //
string_view GlitchASlideDescExpert                    = "You may be expected to perform intermediate\n"    //
                                                        "A-slides with a bombchu.";
/* Hammer Slide */                                                                                         //
string_view GlitchHammerSlideDescDisabled             = "Equipping hover boots after a hammer crouch stab\n"
                                                        "against a wall preserves the recoil speed which\n"//
                                                        "allows Link to cross larger gaps than usual.";    //
string_view GlitchHammerSlideDescNovice               = "Simple hammer slides may be required";            //
string_view GlitchHammerSlideDescIntermediate         = "Hammer slides which require good movement and high"
                                                        "speed may be required.";                          //
/* Ledge Cancel */                                                                                         //
string_view GlitchLedgeCancelDescDisabled             = "Climbing a short ledge and shielding damage will\n"
                                                        "prevent the ledge climbing state from ending.\n"  //
                                                        "This state allows Link to walk through some actors"
                                                        "such as boulders and NPCs.";                      //
string_view GlitchLedgeCancelDescNovice               = "You may be expected to perform ledge cancels using"
                                                        "bombs as a damage source.";                       //
string_view GlitchLedgeCancelDescIntermediate         = "You may be expected to perform ledge cancels in\n"//
                                                        "places with little room.";                        //
string_view GlitchLedgeCancelDescAdvanced             = "You may be expected to perform ledge cancels using"
                                                        "bombchus as a damage source.";                    //
/* Action Swap */                                                                                          //
string_view GlitchActionSwapDescDisabled              = "Action swap allows Link to switch between 2 held\n"
                                                        "items without the put away/equip animations.\n"   //
                                                        "This can be exploited to produce various effects.";
string_view GlitchActionSwapDescNovice                = "You may be expected to use shallow water to set up"
                                                        "action swap.";                                    //
string_view GlitchActionSwapDescAdvanced              = "You may be expected to use bombchus to set up\n"  //
                                                        "action swap.";                                    //
/* QPA */                                                                                                  //
string_view GlitchQPADescDisabled                     = "Certain events can cancel putting away an item\n" //
                                                        "which later lets Link put it away without an\n"   //
                                                        "animation. This can be used with sticks to access\n"
                                                        "a glitched damage value with the properties of\n" //
                                                        "hammer and fire arrows, or with a bottle to store\n"
                                                        "a cutscene for ocarina items.";                   //
string_view GlitchQPADescNovice                       = "You may be expected to get QPA using the boots\n" //
                                                        "animation to delay putting away the item and a\n" //
                                                        "bomb.";                                           //
string_view GlitchQPADescIntermediate                 = "You may be expected to get QPA using only a bomb.";
string_view GlitchQPADescAdvanced                     = "You may be expected to get QPA using a ledge grab\n"
                                                        "to interrupt putting away the item.";             //
string_view GlitchQPADescExpert                       = "You may be expected to get QPA from enemy attacks.";
/* Hookshot Clip */                                                                                        //
string_view GlitchHookshotClipDescDisabled            = "Hookshot Clipping allows Link to hookshot through\n"
                                                        "certain walls, which is useful if a valid\n"      //
                                                        "target is on the other side.";                    //
string_view GlitchHookshotClipDescNovice              = "Basic Hookshot Clipping may be required.";        //
string_view GlitchHookshotClipDescIntermediate        = "Hookshot clips with precise angles and poor\n"    //
                                                        "visibility may be required.";                     //
/* Hookshot Jump: Bonk */                                                                                  //
string_view GlitchHookshotJump_BonkDescDisabled       = "A Hookshot Jump is an umbrella term for techniques"
                                                        "that launch Link into the sky using the Hookshot\n"
                                                        "in various ways, sometimes together with\n"       //
                                                        "other items. The bonk method only requires the\n" //
                                                        "Hookshot itself.";                                //
string_view GlitchHookshotJump_BonkDescNovice         = "Simple hookshot jumps against large flat walls of\n"
                                                        "hookshottable surfaces may be required.";         //
string_view GlitchHookshotJump_BonkDescIntermediate   = "Less lenient hookshot jumps may be required.";    //
string_view GlitchHookshotJump_BonkDescAdvanced       = "Hookshot jumps with precise midair movement may be"
                                                        "required.";                                       //
/* Hookshot Jump: Boots */                                                                                 //
string_view GlitchHookshotJump_BootsDescDisabled      = "This Hookshot Jump technique is one of the easier\n"
                                                        "ones, and require any pair of boots.";            //
string_view GlitchHookshotJump_BootsDescNovice        = "Only relatively short Hookshot Jumps with boots\n"//
                                                        "may be required.";                                //
string_view GlitchHookshotJump_BootsDescIntermediate  = "Higher Hookshot Jumps with boots, where you look\n"
                                                        "further up or downwards may be required.";        //
string_view GlitchHookshotJump_BootsDescAdvanced      = "Hookshot jumps that require a lot of height and\n"//
                                                        "precise midair movement may be required.";        //
/* Cutscene Dives */                                                                                       //
string_view GlitchCutsceneDiveDescDisabled            = "Water physics won't effect Link if he enters the\n"
                                                        "water while a cutscene is playing, allowing him\n"//
                                                        "to sink to the bottom.";                      //
string_view GlitchCutsceneDiveDescNovice              = "Attempting to use Farore's Wind (when it's already"
                                                        "set) with another magic item active prevents the\n"
                                                        "water from clearing the FW cutscene until the\n"  //
                                                        "other effect ends.\n"                             //
                                                        "You may be expected to use Nayru's Love as the\n" //
                                                        "other magic effect.";                             //
string_view GlitchCutsceneDiveDescIntermediate        = "You can catch something in a bottle while standing"
                                                        "over water using the hover boots to fall through\n"
                                                        "the water during the catching cutscene.";         //
string_view GlitchCutsceneDiveDescAdvanced            = "You may be expected to use magic arrows to perform"
                                                        "Farore's Wind cutscene dives.";                   //
/* Navi Dive: Stick */                                                                                     //
string_view GlitchNaviDive_StickDescDisabled          = "A Navi dive is a type of cutscene dive achieved by"
                                                        "falling off a ledge while talking to Navi. While\n"
                                                        "the usual method for achieving this is a TSC, it\n"
                                                        "is also possible to perform using a jump attack\n"//
                                                        "with deku sticks.";                               //
string_view GlitchNaviDive_StickDescNovice            = "You may be expected to enter BotW with a stick\n" //
                                                        "Navi dive.";                                      //
string_view GlitchNaviDive_StickDescIntermediate      = "You may be expected to use the LH to ZD shortcut\n"
                                                        "with a stick Navi dive.";                         //
string_view GlitchNaviDive_StickDescAdvanced          = "You may be expected to use the LW to ZR shortcut\n"
                                                        "with a stick Navi dive.";                         //
/* TSC */                                                                                                  //
string_view GlitchTripleSlashClipDescDisabled         = "When doing a three-slash-combo with either the\n" //
                                                        "Kokiri Sword or the Master Sword and put it away,\n"
                                                        "Link will be placed back a small distance.\n"     //
                                                        "If, while slashing, you use the recoil of hitting\n"
                                                        "a wall and then put away the sword, Link may clip\n"
                                                        "into a wall behind him if angled correctly.";     //
string_view GlitchTripleSlashClipDescNovice           = "Basic Triple Slash Clipping may be required.";    //
string_view GlitchTripleSlashClipDescIntermediate     = "Some more complex OoB movement may be required."; //
string_view GlitchTripleSlashClipDescAdvanced         = "Very precise OoB movement may be required.";      //
string_view GlitchTripleSlashClipDescExpert           = "Very precise TSCs may be required.";              //
/* Ledge Clip */                                                                                           //
string_view GlitchLedgeClipDescDisabled               = "A Ledge Clip allows Link to fall through a floor\n"
                                                        "or pass through an object by facing a wall\n"     //
                                                        "and dropping down to the left in various ways.\n" //
                                                        "These only work as an adult.";                    //
string_view GlitchLedgeClipDescNovice                 = "Basic Ledge Clips may be required.\n"             //
                                                        "Some require that you let go of the ledge with\n" //
                                                        "a specific timing.";                              //
string_view GlitchLedgeClipDescIntermediate           = "Certain harder clips may also be required.";      //
string_view GlitchLedgeClipDescAdvanced               = "Ledge clips with complex OoB movement may be\n"   //
                                                        "required.";                                       //
/* Seam Walk */                                                                                            //
string_view GlitchSeamWalkDescDisabled                = "Where 2 walls come together they form a seam that\n"
                                                        "Link can stand on. It is possible to use these to\n"
                                                        "gain height and reach normally inaccessible areas.\n"
                                                        "Additionally these seams can reach far above the\n"
                                                        "walls that form them, creating invisible seams."; //
string_view GlitchSeamWalkDescNovice                  = "Short seam walks up visible walls with ISG may be\n"
                                                        "required.";                                       //
string_view GlitchSeamWalkDescIntermediate            = "Short seam walks up visible walls without ISG or\n"
                                                        "longer seam walks with ISG may be required.";     //
string_view GlitchSeamWalkDescAdvanced                = "Longer seam walks without ISG may be required.";  //
string_view GlitchSeamWalkDescExpert                  = "Very precise seam walks may be required.";        //
string_view GlitchSeamWalkDescHero                    = "Crossing Gerudo Valley as child by walking up a\n"//
                                                        "wall with the cucco may be required.";            //
/* Misc Glitches */                                                                                        //
string_view GlitchWWTEscapeDesc                       = "Using deku nuts and the crawlspace to let you\n"  //
                                                        "move while reading the nearby sign, you can walk\n"
                                                        "through the Kokiri guarding the exit. This only\n"//
                                                        "affects logic with shuffled entrances.";          //
string_view GlitchGVTentAsChildDesc                   = "The loading zone for the carpenters' tent exists\n"
                                                        "in the child map despite the tent being removed.\n\n"
                                                        "With this setting enabled you may be expected to\n"
                                                        "use that loading zone.";                          //
string_view GlitchGFGuardSneakDesc                    = "By using the bow, hookshot, or boomerang while\n" //
                                                        "looking up and targeting the Fortress wall, you\n"//
                                                        "can walk\n past the guard on the slope without\n" //
                                                        "being caught.";                                   //
string_view GlitchItemlessWastelandDesc               = "By starting a backwalk and then targeting every\n"//
                                                        "other frame Link will maintain backwalk speed\n"  //
                                                        "over the quicksand in the haunted wasteland which\n"
                                                        "lets him cross with no items.";                   //
string_view GlitchOccamsStatueDesc                    = "With some precise movement through loading zones\n"
                                                        "in Darunia's room, it is possible to unload the\n"//
                                                        "collision of the statue blocking the way to DMC.";//
string_view GlitchZDOoBJumpSlashDesc                  = "Jump slashing the wall next to the pillar above\n"//
                                                        "the ZD to LH shortcut as adult will let you clip\n"
                                                        "through. From here you can either jump to the shop"
                                                        "loading zone or fall OoB a little bit and swim\n" //
                                                        "into the shortcut loading zone.";                 //
string_view GlitchJabuStickRecoilDesc                 = "Child can enter Jabu without a fish in a bottle\n"//
                                                        "using the recoil from breaking a deku stick while\n"
                                                        "sidehopping next to the loading zone.";           //
string_view GlitchJabuAdultDesc                       = "Adult can enter Jabu by walking over a specific\n"//
                                                        "section of ice on the side near ice cavern.";     //
string_view GlitchBlueFireWallDesc                    = "Pouring blue fire on certain destructable walls\n"//
                                                        "will cause them to break.";                       //
string_view GlitchClassicHalfieDesc                   = "Dying during a jump attack towards a ledge just\n"//
                                                        "too high to climb will put Link on top of the\n"  //
                                                        "ledge. Combined with a fairy this lets Link climb\n"
                                                        "ledges such as the one to the switch in DC.";     //
string_view GlitchModernHalfieDesc                    = "Using a bomb flower and a twisted backflip, it is\n"
                                                        "possible to access the switch in DC with only a\n"//
                                                        "deku shield and the goron bracelet.";             //
string_view GlitchJabuSwitchDesc                      = "Using a cutscene item the frame you land on the\n"//
                                                        "blue switches in Jabu Jabu's Belly will make them\n"
                                                        "stay held for a while after you step off them.";  //
string_view GlitchForestBKSkipDesc                    = "Using the hookshot or bow to shorten a backflip\n"//
                                                        "it is possible to land on the other side of the\n"//
                                                        "railing in the north of the lobby and get clipped\n"
                                                        "out of bounds. From here it is possible to jump\n"//
                                                        "into the boss loading zone.";                     //
string_view GlitchFireGrunzClipDesc                   = "Grabbing a ledge in a corner partially clips Link\n"
                                                        "through the wall. A bomb can be used to push Link\n"
                                                        "fully through the wall to get out of bounds. This\n"
                                                        "can be used in the fire temple along with hover\n"//
                                                        "boots to skip hammer in the first room.";         //
