#include "settings.h"

namespace Rando {
void Settings::CreateOptionDescriptions() {
    mOptionDescriptions[RSK_FOREST] = "Closed - Kokiri sword & shield are required to access "
                                      "the Deku Tree, and completing the Deku Tree is required to "
                                      "access the Hyrule Field exit.\n"
                                      "\n"
                                      "Closed Deku - Kokiri boy no longer blocks the path to Hyrule "
                                      "Field but Mido still requires the Kokiri sword and Deku shield "
                                      "to access the tree.\n"
                                      "\n"
                                      "Open - Mido no longer blocks the path to the Deku Tree. Kokiri "
                                      "boy no longer blocks the path out of the forest.";
    mOptionDescriptions[RSK_KAK_GATE] = "Closed - The gate will remain closed until Zelda's letter "
                                        "is shown to the guard.\n"
                                        "\n"
                                        "Open - The gate is always open. The happy mask shop "
                                        "will open immediately after obtaining Zelda's letter.";
    mOptionDescriptions[RSK_DOOR_OF_TIME] = "Closed - The Ocarina of Time, the Song of Time and all "
                                            "three spiritual stones are required to open the Door of Time.\n"
                                            "\n"
                                            "Song only - Play the Song of Time in front of the Door of "
                                            "Time to open it.\n"
                                            "\n"
                                            "Open - The Door of Time is permanently open with no requirements.";
    mOptionDescriptions[RSK_ZORAS_FOUNTAIN] = "Closed - King Zora obstructs the way to Zora's Fountain. "
                                              "Ruto's letter must be shown as child Link in order to move "
                                              "him in both time periods.\n"
                                              "\n"
                                              "Closed as child - Ruto's Letter is only required to move King Zora "
                                              "as child Link. Zora's Fountain starts open as adult.\n"
                                              "\n"
                                              "Open - King Zora has already mweeped out of the way in both "
                                              "time periods. Ruto's Letter is removed from the item pool.";
    mOptionDescriptions[RSK_STARTING_AGE] =
        "Choose which age Link will start as.\n\n"
        "Starting as adult means you start with the Master Sword in your inventory.\n"
        "The child option is forcefully set if it would conflict with other options.";
    mOptionDescriptions[RSK_GERUDO_FORTRESS] = "Sets the amount of carpenters required to repair the bridge "
                                               "in Gerudo Valley.\n"
                                               "\n"
                                               "Normal - All 4 carpenters are required to be saved.\n"
                                               "\n"
                                               "Fast - Only the bottom left carpenter requires rescuing.\n"
                                               "\n"
                                               "Open - The bridge is repaired from the start.\n"
                                               "\n"
                                               "Only \"Normal\" is compatible with Gerudo Fortress Key Rings.";
    mOptionDescriptions[RSK_RAINBOW_BRIDGE] =
        "Alters the requirements to open the bridge to Ganon's Castle.\n"
        "\n"
        "Vanilla - Obtain the Shadow Medallion, Spirit Medallion and Light Arrows.\n"
        "\n"
        "Always open - No requirements.\n"
        "\n"
        "Stones - Obtain the specified amount of spiritual stones.\n"
        "\n"
        "Medallions - Obtain the specified amount of medallions.\n"
        "\n"
        "Dungeon rewards - Obtain the specified total sum of spiritual "
        "stones or medallions.\n"
        "\n"
        "Dungeons - Complete the specified amount of dungeons. Dungeons "
        "are considered complete after stepping in to the blue warp after "
        "the boss.\n"
        "\n"
        "Tokens - Obtain the specified amount of Skulltula tokens.\n"
        "\n"
        "Greg - Find Greg the Green Rupee.";
    mOptionDescriptions[RSK_BRIDGE_OPTIONS] =
        "Standard Rewards - Greg does not change logic, Greg does not help open the bridge, max "
        "number of rewards on slider does not change.\n"
        "\n"
        "Greg as Reward - Greg does change logic (can be part of expected path for opening "
        "bridge), Greg helps open bridge, max number of rewards on slider increases by 1 to "
        "account for Greg. \n"
        "\n"
        "Greg as Wildcard - Greg does not change logic, Greg helps open the bridge, max number of "
        "rewards on slider does not change.";
    mOptionDescriptions[RSK_GANONS_TRIALS] =
        "Sets the number of Ganon's Trials required to dispel the barrier.\n"
        "\n"
        "Skip - No Trials are required and the barrier is already dispelled.\n"
        "\n"
        "Set Number - Select a number of trials that will be required from the"
        "slider below. Which specific trials you need to complete will be random.\n"
        "\n"
        "Random Number - A Random number and set of trials will be required.";
    mOptionDescriptions[RSK_TRIAL_COUNT] = "Set the number of trials required to enter Ganon's Tower.";
    mOptionDescriptions[RSK_MQ_DUNGEON_RANDOM] =
        "Sets the number of Master Quest Dungeons that are shuffled into the pool.\n"
        "\n"
        "None - All Dungeons will be their Vanilla versions.\n"
        "\n"
        "Set Number - Select a number of dungeons that will be their Master Quest versions "
        "using the slider below. Which dungeons are set to be the Master Quest variety will be random.\n"
        "\n"
        "Random Number - A Random number and set of dungeons will be their Master Quest varieties.\n"
        "\n"
        "Selection Only - Specify which dungeons are Vanilla, Master Quest or a 50/50 between the two.\n"
        "Differs from Random Number in that they are rolled individually, making the exact total a bell curve.";
    mOptionDescriptions[RSK_MQ_DUNGEON_SET] =
                "Choose specific Dungeons to be Master Quest or Vanilla.\n"
                "\n"
                "If Master Quest Dungeons is set to Set Number or Random, the dungeons chosen "
                "to be Master Quest here will count towards that total. Any Dungeons set to Vanilla "
                "here will be guaranteed to be Vanilla. If Set Number is higher than the amount of dungeons "
                "set to either MQ or Random here, you will have fewer MQ Dungeons than the number you "
                "set.";
    mOptionDescriptions[RSK_TRIFORCE_HUNT] =
        "Pieces of the Triforce of Courage have been scattered across the world. Find them all to finish the game!\n\n"
        "When the required amount of pieces have been found, the game is saved and Ganon's Boss key is given "
        "to you when you load back into the game if you desire to beat Ganon afterwards.\n\n"
        "Keep in mind Ganon might not be logically beatable when \"All Locations Reachable\" is turned off.";
    mOptionDescriptions[RSK_TRIFORCE_HUNT_PIECES_TOTAL] =
        "The amount of Triforce pieces that will be placed in the world. "
        "Keep in mind seed generation can fail if more pieces are placed than there are junk items in the item pool.";
    mOptionDescriptions[RSK_TRIFORCE_HUNT_PIECES_REQUIRED] = "The amount of Triforce pieces required to win the game.";
    mOptionDescriptions[RSK_SHUFFLE_DUNGEON_ENTRANCES] =
        "Shuffle the pool of dungeon entrances, including Bottom of the Well, Ice Cavern and Gerudo Training Grounds.\n"
        "\n"
        "Shuffling Ganon's Castle can be enabled separately.\n"
        "\n"
        "Additionally, the entrances of Deku Tree, Fire Temple, Bottom of the Well and Gerudo Training Ground are "
        "opened for both child and adult.\n"
        "\n"
        "- Deku Tree will be open for adult after Mido has seen child Link with a sword and shield.\n"
        "- Bottom of the Well will be open for adult after playing Song of Storms to the Windmill guy as child.\n"
        "- Gerudo Training Ground will be open for child after adult has paid to open the gate once.";
    mOptionDescriptions[RSK_SHUFFLE_BOSS_ENTRANCES] =
        "Shuffle the pool of dungeon boss entrances. This affects the boss rooms of all stone and medallion dungeons.\n"
        "\n"
        "Age Restricted - Shuffle the entrances of child and adult boss rooms separately.\n"
        "\n"
        "Full - Shuffle the entrances of all boss rooms together. Child may be expected to defeat Phantom Ganon and/or "
        "Bongo Bongo.";
    mOptionDescriptions[RSK_SHUFFLE_OVERWORLD_ENTRANCES] =
        "Shuffle the pool of Overworld entrances, which corresponds to almost all loading zones between overworld "
        "areas.\n"
        "\n"
        "Some entrances are unshuffled to avoid issues:\n"
        "- Hyrule Castle Courtyard and Garden entrance\n"
        "- Both Market Back Alley entrances\n"
        "- Gerudo Valley to Lake Hylia (unless entrances are decoupled)";
    mOptionDescriptions[RSK_SHUFFLE_INTERIOR_ENTRANCES] =
        "Shuffle the pool of interior entrances which contains most Houses and all Great Fairies.\n"
        "\n"
        "All - An extended version of 'Simple' with some extra places:\n"
        "- Windmill\n"
        "- Link's House\n"
        "- Temple of Time\n"
        "- Kakariko Potion Shop";
    mOptionDescriptions[RSK_SHUFFLE_GROTTO_ENTRANCES] =
        "Shuffle the pool of grotto entrances, including all graves, small Fairy fountains and the Deku Theatre.";
    mOptionDescriptions[RSK_SHUFFLE_OWL_DROPS] = "Randomize where Kaepora Gaebora (the Owl) drops you at when you talk "
                                                 "to him at Lake Hylia or at the top of Death Mountain Trail.";
    mOptionDescriptions[RSK_SHUFFLE_WARP_SONGS] = "Randomize where each of the 6 warp songs leads to.";
    mOptionDescriptions[RSK_SHUFFLE_OVERWORLD_SPAWNS] =
        "Randomize where you start as Child or Adult when loading a save in the Overworld. This "
        "means you may not necessarily spawn inside Link's House or Temple of Time.\n"
        "\n"
        "This stays consistent after saving and loading the game again.\n"
        "\n"
        "Keep in mind you may need to temporarily disable the \"Remember Save Location\" time saver to "
        "be able use the spawn positions, especially if they are the only logical way to get to certain areas.";
    mOptionDescriptions[RSK_DECOUPLED_ENTRANCES] =
        "Decouple entrances when shuffling them. This means you are no longer guaranteed "
        "to end up back where you came from when you go back through an entrance.\n"
        "\n"
        "This also adds the one-way entrance from Gerudo Valley to Lake Hylia in the pool of "
        "overworld entrances when they are shuffled.";
    mOptionDescriptions[RSK_MIXED_ENTRANCE_POOLS] =
        "Shuffle entrances into a mixed pool instead of separate ones. Has no affect on pools whose "
        "entrances aren't shuffled, and \"Shuffle Boss Entrances\" must be set to \"Full\" to include them.\n"
        "\n"
        "For example, enabling the settings to shuffle grotto, dungeon, and overworld entrances and "
        "selecting grotto and dungeon entrances here will allow a dungeon to be inside a grotto or "
        "vice versa, while overworld entrances are shuffled in their own separate pool and indoors stay vanilla.";
    mOptionDescriptions[RSK_MIX_DUNGEON_ENTRANCES] = "Dungeon entrances will be part of the mixed pool";
    mOptionDescriptions[RSK_MIX_BOSS_ENTRANCES] = "Boss entrances will be part of the mixed pool";
    mOptionDescriptions[RSK_MIX_OVERWORLD_ENTRANCES] = "Overworld entrances will be part of the mixed pool";
    mOptionDescriptions[RSK_MIX_INTERIOR_ENTRANCES] = "Interior entrances will be part of the mixed pool";
    mOptionDescriptions[RSK_MIX_GROTTO_ENTRANCES] = "Grotto entrances will be part of the mixed pool";
    mOptionDescriptions[RSK_SHUFFLE_SONGS] =
        "Song locations - Songs will only appear at locations that normally teach songs.\n"
        "\n"
        "Dungeon rewards - Songs appear after beating a major dungeon boss.\n"
        "The 4 remaining songs are located at:\n"
        "  - Zelda's lullaby location\n"
        "  - Ice Cavern's Serenade of Water location\n"
        "  - Bottom of the Well Lens of Truth location\n"
        "  - Gerudo Training Ground's Ice Arrows location\n"
        "\n"
        "Anywhere - Songs can appear at any location.";
    mOptionDescriptions[RSK_SHUFFLE_TOKENS] = "Shuffles Golden Skulltula Tokens into the item pool. This means "
                                              "Golden Skulltulas can contain other items as well.\n"
                                              "\n"
                                              "Off - GS tokens will not be shuffled.\n"
                                              "\n"
                                              "Dungeons - Only shuffle GS tokens that are within dungeons.\n"
                                              "\n"
                                              "Overworld - Only shuffle GS tokens that are outside of dungeons.\n"
                                              "\n"
                                              "All Tokens - Shuffle all 100 GS tokens.";
    mOptionDescriptions[RSK_SKULLS_SUNS_SONG] = "All Golden Skulltulas that require nighttime to appear will only be "
                                                "expected to be collected after getting Sun's Song.";
    mOptionDescriptions[RSK_SHUFFLE_KOKIRI_SWORD] =
        "Shuffles the Kokiri Sword into the item pool.\n"
        "\n"
        "This will require the use of sticks until the Kokiri Sword is found.";
    mOptionDescriptions[RSK_SHUFFLE_MASTER_SWORD] =
        "Shuffles the Master Sword into the item pool.\n"
        "\n"
        "Adult Link will start with a second free item instead of the Master Sword.\n"
        "If you haven't found the Master Sword before facing Ganon, you won't receive it during the fight.";
    mOptionDescriptions[RSK_SHUFFLE_CHILD_WALLET] =
        "Enabling this shuffles the Child's Wallet into the item pool.\n"
        "\n"
        "You will not be able to carry any rupees until you find a wallet.";
    mOptionDescriptions[RSK_INCLUDE_TYCOON_WALLET] =
        "Enabling this adds an extra Progressive Wallet to the pool and adds a new 999 capacity tier after Giant's Wallet.\n";
    mOptionDescriptions[RSK_SHUFFLE_OCARINA] =
        "Enabling this shuffles the Fairy Ocarina and the Ocarina of Time into the item pool.\n"
        "\n"
        "This will require finding an Ocarina before being able to play songs.";
    mOptionDescriptions[RSK_SHUFFLE_OCARINA_BUTTONS] =
        "Enabling this shuffles the Ocarina buttons into the item pool.\n"
        "\n"
        "This will require finding the buttons before being able to use them in songs.";

    mOptionDescriptions[RSK_SHUFFLE_SWIM] =
        "Shuffles the ability to Swim into the item pool.\n"
        "The ability to swim has to be found as an item (you can still be underwater if you use iron boots).\n"
        "\n"
        "If you enter a water entrance without swim you will be respawned on land to prevent infinite death loops.\n"
        "If you void out in Water Temple you will immediately be kicked out to prevent a softlock.";
    mOptionDescriptions[RSK_SHUFFLE_WEIRD_EGG] = "Shuffles the Weird Egg from Malon in to the item pool. Enabling "
                                                 "\"Skip Child Zelda\" disables this feature.\n"
                                                 "\n"
                                                 "The Weird Egg is required to unlock several events:\n"
                                                 "  - Zelda's Lullaby from Impa\n"
                                                 "  - Saria's song in Sacred Forest Meadow\n"
                                                 "  - Epona's song and chicken minigame at Lon Lon Ranch\n"
                                                 "  - Zelda's letter for Kakariko gate (if set to closed)\n"
                                                 "  - Happy Mask Shop sidequest\n";
    mOptionDescriptions[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD] =
        "Shuffles the Gerudo Membership Card into the item pool.\n"
        "\n"
        "The Gerudo Card is required to enter the Gerudo Training Grounds, opening "
        "the gate to Haunted Wasteland and the Horseback Archery minigame.";
    mOptionDescriptions[RSK_SHUFFLE_FISHING_POLE] = "Shuffles the fishing pole into the item pool.\n"
        "\n"
        "The fishing pole is required to play the fishing pond minigame.";
    mOptionDescriptions[RSK_INFINITE_UPGRADES] = "Adds upgrades that hold infinite quanities of items (bombs, arrows, etc.)\n"
        "\n"
        "Progressive - The infinite upgrades are obtained after getting the last normal capacity upgrade\n"
        "\n"
        "Condensed Progressive - The infinite upgrades are obtained as the first capacity upgrade (doesn't apply to the infinite wallet or to infinite magic)";
    mOptionDescriptions[RSK_SHUFFLE_DEKU_STICK_BAG] = "Shuffles the deku stick bag into the item pool.\n"
        "\n"
        "The deku stick bag is required to hold deku sticks.";
    mOptionDescriptions[RSK_SHUFFLE_DEKU_NUT_BAG] = "Shuffles the deku nut bag into the item pool.\n"
        "\n"
        "The deku nut bag is required to hold deku nuts.";
    mOptionDescriptions[RSK_SHOPSANITY] = "Off - All shop items will be the same as vanilla.\n"
                                          "\n"
                                          "Specifc Count - Vanilla shop items will be shuffled among different shops, and "
                                          "each shop will contain a specifc number (0-7) of non-vanilla shop items.\n"
                                          "\n"
                                          "Random - Vanilla shop items will be shuffled among different shops, and "
                                          "each shop will contain a random number (1-7) of non-vanilla shop items.";
    mOptionDescriptions[RSK_SHOPSANITY_COUNT] = "0 Items - Vanilla shop items will be shuffled among different shops.\n"
                                                "\n"
                                                "1-7 Items - Vanilla shop items will be shuffled among different shops, and "
                                                "each shop will contain 1-7 non-vanilla shop items.\n"
                                                /*
                                                "\n"
                                                "8 Items - All shops will contain 8 non-vanilla shop items.\n"
                                                */;
    mOptionDescriptions[RSK_SHOPSANITY_PRICES] =
        "Vanilla - The same price as the item it replaced\n"
        "Cheap Balanced - Prices will range between 0 to 95 rupees, favoring lower numbers\n"
        "Balanced - Prices will range between 0 to 300 rupees, favoring lower numbers\n"
        "Fixed - A fixed number\n"
        "Range - A random point between specific ranges\n"
        "Set By Wallet - Set weights that decide the choice of each wallet, and get a random price in that range if that wallet is chosen";
    mOptionDescriptions[RSK_SHOPSANITY_PRICES_FIXED_PRICE] =
        "The price for Shopsanity checks.";
    mOptionDescriptions[RSK_SHOPSANITY_PRICES_RANGE_1] =
        "The first part of the inclusive range of prices to allow for Shopsanity checks.";
    mOptionDescriptions[RSK_SHOPSANITY_PRICES_RANGE_2] =
        "The second part of the inclusive range of prices to allow for Shopsanity checks.";
    mOptionDescriptions[RSK_SHOPSANITY_PRICES_NO_WALLET_WEIGHT] =
        "The chance for Shopsanity checks to be free.";
    mOptionDescriptions[RSK_SHOPSANITY_PRICES_CHILD_WALLET_WEIGHT] =
        "The chance for Shopsanity checks to be purchasable with Child's Wallet (1-99).";
    mOptionDescriptions[RSK_SHOPSANITY_PRICES_ADULT_WALLET_WEIGHT] =
        "The chance for Shopsanity checks to be purchasable with Adult's Wallet (100-200).";
    mOptionDescriptions[RSK_SHOPSANITY_PRICES_GIANT_WALLET_WEIGHT] =
        "The chance for Shopsanity checks to be purchasable with Giant's Wallet (201-500).";
    mOptionDescriptions[RSK_SHOPSANITY_PRICES_TYCOON_WALLET_WEIGHT] =
        "The chance for Shopsanity checks to be purchasable with Tycoon Wallet. (500+)";
    mOptionDescriptions[RSK_SHOPSANITY_PRICES_AFFORDABLE] =
        "After choosing a price, set it to the affordable amount based on the wallet required.\n\n"
        "Affordable prices per tier: starter = 1, adult = 100, giant = 201, tycoon = 501\n\n"
        "Use this to enable wallet tier locking, but make shop items not as expensive as they could be.";
    mOptionDescriptions[RSK_FISHSANITY] = "Off - Fish will not be shuffled. No changes will be made to fishing behavior.\n\n"
        "Shuffle only Hyrule Loach - Allows you to earn an item by catching the hyrule loach at the fishing pond and giving it to the owner.\n\n"
        "Shuffle Fishing Pond - The fishing pond's fish will be shuffled. Catching a fish in the fishing pond will grant a reward.\n\n"
        "Shuffle Overworld Fish - Fish in generic grottos and Zora's Domain will be shuffled. Catching a fish in a bottle will give a reward.\n\n"
        "Shuffle Both - Both overworld fish and fish in the fishing pond will be shuffled.";
    mOptionDescriptions[RSK_FISHSANITY_POND_COUNT] = "The number of fish to randomize in the fishing pool.\n\n"
        "If set to maximum, each fish will have a unique check, including a Hyrule Loach which appears only as child, and "
        "uncaught fish will be given a visual indicator to distinguish from already-caught fish.\n\n"
        "Otherwise, any fish caught in the pond will give a reward, until all rewards have been given.";
    mOptionDescriptions[RSK_FISHSANITY_AGE_SPLIT] = "Enabling this will split the fishing pond fish by age, making fishing pond fish grant different rewards as child and adult.\n\n"
        "If disabled, then the child pond will be shuffled and shared between both ages.\n\n"
        "Note that, as child, there is a second loach available in the pond!";
    mOptionDescriptions[RSK_SHUFFLE_SCRUBS] =
        "Off - Scrubs will not be shuffled. The 3 Scrubs that give one-time items in the "
        "vanilla game (PoH, Deku Nut capacity, and Deku Stick capacity) will not spawn."
        "\n"
        "One-Time Only - Only the 3 Scrubs that give one-time items in the "
        "vanilla game are shuffled.\n"
        "\n"
        "All - All Scrubs are shuffled.";
    mOptionDescriptions[RSK_SCRUBS_PRICES] =
        "Vanilla - The same price as the item it replaced\n"
        "Cheap Balanced - Prices will range between 0 to 95 rupees, favoring lower numbers\n"
        "Balanced - Prices will range between 0 to 300 rupees, favoring lower numbers\n"
        "Fixed - A fixed number\n"
        "Range - A random point between specific ranges\n"
        "Set By Wallet - Set weights that decide the choice of each wallet, and get a random price in that range if that wallet is chosen";
    mOptionDescriptions[RSK_SCRUBS_PRICES_FIXED_PRICE] =
        "The price for Scrub checks.";
    mOptionDescriptions[RSK_SCRUBS_PRICES_RANGE_1] =
        "The first part of the inclusive range of prices to allow for Scrub checks.";
    mOptionDescriptions[RSK_SCRUBS_PRICES_RANGE_2] =
        "The second part of the inclusive range of prices to allow for Scrub checks.";
    mOptionDescriptions[RSK_SCRUBS_PRICES_NO_WALLET_WEIGHT] =
        "The chance for Scrub checks to be free.";
    mOptionDescriptions[RSK_SCRUBS_PRICES_CHILD_WALLET_WEIGHT] =
        "The chance for Scrub checks to be purchasable with Child's Wallet (1-99).";
    mOptionDescriptions[RSK_SCRUBS_PRICES_ADULT_WALLET_WEIGHT] =
        "The chance for Scrub checks to be purchasable with Adult's Wallet (100-200).";
    mOptionDescriptions[RSK_SCRUBS_PRICES_GIANT_WALLET_WEIGHT] =
        "The chance for Scrub checks to be purchasable with Giant's Wallet (201-500).";
    mOptionDescriptions[RSK_SCRUBS_PRICES_TYCOON_WALLET_WEIGHT] =
        "The chance for Scrub checks to be purchasable with Tycoon Wallet. (500+)";
    mOptionDescriptions[RSK_SCRUBS_PRICES_AFFORDABLE] =
        "After choosing a price, set it to the affordable amount based on the wallet required.\n\n"
        "Affordable prices per tier: starter = 1, adult = 100, giant = 201, tycoon = 501\n\n"
        "Use this to enable wallet tier locking, but make scrub items not as expensive as they could be.";
    mOptionDescriptions[RSK_SHUFFLE_BEEHIVES] =
        "Beehives give a randomized item from the pool when broken.";
    mOptionDescriptions[RSK_SHUFFLE_COWS] =
        "Cows give a randomized item from the pool upon performing Epona's Song in front of them.";
    mOptionDescriptions[RSK_SHUFFLE_MERCHANTS] =
        "This setting governs if the Bean Salesman, Medigoron, Granny and the Carpet Salesman "
        "sell a random item.\n"
        "Beans Only - Only the Bean Salesman will have a check, and a pack of Magic Beans will be added "
        " to the item pool."
        "All But Beans - Medigoron, Granny and the Carpet Salesman will have checks, "
        "A Giant's Knife and a pack of Bombchus will be added to the item pool, and "
        "one of the bottles will contain a Blue Potion.\n\n"
        "All - Apply both effects.\n"
        "\n"
        "Granny's item will only be offered after you have traded in the Odd Mushroom when Shuffle Adult Trade is on. "
        "Otherwise when off, you will need to have found the Claim Check to buy her item (simulating the trade quest "
        "is complete).";
    mOptionDescriptions[RSK_MERCHANT_PRICES] =
        "Vanilla - The same price as the Check in vanilla, 60 for the Bean Salesman\n"
        "Cheap Balanced - Prices will range between 0 to 95 rupees, favoring lower numbers\n"
        "Balanced - Prices will range between 0 to 300 rupees, favoring lower numbers\n"
        "Fixed - A fixed number\n"
        "Range - A random point between specific ranges\n"
        "Set By Wallet - Set weights that decide the choice of each wallet, and get a random price in that range if that wallet is chosen";
    mOptionDescriptions[RSK_MERCHANT_PRICES_FIXED_PRICE] =
        "The price for Merchant checks.";
    mOptionDescriptions[RSK_MERCHANT_PRICES_RANGE_1] =
        "The first part of the inclusive range of prices to allow for Merchant checks.";
    mOptionDescriptions[RSK_MERCHANT_PRICES_RANGE_2] =
        "The second part of the inclusive range of prices to allow for Merchant checks.";
    mOptionDescriptions[RSK_MERCHANT_PRICES_NO_WALLET_WEIGHT] =
        "The chance for Merchant checks to be free.";
    mOptionDescriptions[RSK_MERCHANT_PRICES_CHILD_WALLET_WEIGHT] =
        "The chance for Merchant checks to be purchasable with Child's Wallet (1-99).";
    mOptionDescriptions[RSK_MERCHANT_PRICES_ADULT_WALLET_WEIGHT] =
        "The chance for Merchant checks to be purchasable with Adult's Wallet (100-200).";
    mOptionDescriptions[RSK_MERCHANT_PRICES_GIANT_WALLET_WEIGHT] =
        "The chance for Merchant checks to be purchasable with Giant's Wallet (201-500).";
    mOptionDescriptions[RSK_MERCHANT_PRICES_TYCOON_WALLET_WEIGHT] =
        "The chance for Merchant checks to be purchasable with Tycoon Wallet. (500+)";
    mOptionDescriptions[RSK_MERCHANT_PRICES_AFFORDABLE] =
        "After choosing a price, set it to the affordable amount based on the wallet required.\n\n"
        "Affordable prices per tier: starter = 1, adult = 100, giant = 201, tycoon = 501\n\n"
        "Use this to enable wallet tier locking, but make merchant items not as expensive as they could be.";
    mOptionDescriptions[RSK_SHUFFLE_FROG_SONG_RUPEES] = "Shuffles 5 Purple Rupees into to the item pool, and allows\n"
                                                        "you to earn items by playing songs at the Frog Choir.\n"
                                                        "\n"
                                                        "This setting does not effect the item earned from playing\n"
                                                        "the Song of Storms and the frog song minigame.";
    mOptionDescriptions[RSK_SHUFFLE_ADULT_TRADE] =
        "Adds all of the adult trade quest items into the pool, each of which "
        "can be traded for a unique reward.\n"
        "\n"
        "You will be able to choose which of your owned adult trade items is visible "
        "in the inventory by selecting the item with A and using the control stick or "
        "D-pad.\n"
        "\n"
        "If disabled, only the Claim Check will be found in the pool.";
    mOptionDescriptions[RSK_SHUFFLE_100_GS_REWARD] =
        "Shuffle the item the cursed rich man in the House of Skulltula gives when you "
        "have collected all 100 Gold Skulltula Tokens.\n"
        "\n"
        "You can still talk to him multiple times to get Huge Rupees.";
    mOptionDescriptions[RSK_SHUFFLE_DUNGEON_REWARDS] =
        "Shuffles the location of spiritual stones and medallions.\n"
        "\n"
        "End of dungeons - Spiritual stones and medallions will be given as rewards "
        "for beating major dungeons. Link will always start with one stone or medallion.\n"
        "\n"
        "Any dungeon - Spiritual stones and medallions can be found inside any dungeon.\n"
        "\n"
        "Overworld - Spiritual stones and medallions can only be found outside of dungeons.\n"
        "\n"
        "Anywhere - Spiritual stones and medallions can appear anywhere.";
    mOptionDescriptions[RSK_SHUFFLE_MAPANDCOMPASS] =
        "Start with - You will start with Maps & Compasses from all dungeons.\n"
        "\n"
        "Vanilla - Maps & Compasses will appear in their vanilla locations.\n"
        "\n"
        "Own dungeon - Maps & Compasses can only appear in their respective dungeon.\n"
        "\n"
        "Any dungeon - Maps & Compasses can only appear inside of any dungon.\n"
        "\n"
        "Overworld - Maps & Compasses can only appear outside of dungeons.\n"
        "\n"
        "Anywhere - Maps & Compasses can appear anywhere in the world.";
    mOptionDescriptions[RSK_KEYSANITY] =
        "Start with - You will start with all Small Keys from all dungeons.\n"
        "\n"
        "Vanilla - Small Keys will appear in their vanilla locations. "
        "You start with 3 keys in Spirit Temple MQ because the vanilla key layout is not beatable in logic.\n"
        "\n"
        "Own dungeon - Small Keys can only appear in their respective dungeon. "
        "If Fire Temple is not a Master Quest dungeon, the door to the Boss Key chest will be unlocked.\n"
        "\n"
        "Any dungeon - Small Keys can only appear inside of any dungon.\n"
        "\n"
        "Overworld - Small Keys can only appear outside of dungeons.\n"
        "\n"
        "Anywhere - Small Keys can appear anywhere in the world.";
    mOptionDescriptions[RSK_KEYRINGS] =
        "Keyrings will replace all small keys from a particular dungeon with a single keyring that awards all keys for "
        "it's associated dungeon\n"
        "\n"
        "Off - No dungeons will have their keys replaced with keyrings.\n"
        "\n"
        "Random - A random amount of dungeons(0-8 or 9) will have their keys replaced with keyrings.\n"
        "\n"
        "Count - A specified amount of randomly selected dungeons will have their keys replaced with keyrings.\n"
        "\n"
        "Selection - Hand select which dungeons will have their keys replaced with keyrings\n"
        "(can also be left as random, in which case each one will have a 50% chance of being a keyring).\n"
        "\n"
        "Selecting key ring for dungeons will have no effect if Small Keys are set to Start With or Vanilla.\n"
        "\n"
        "If Gerudo Fortress Carpenters is set to Normal, and Gerudo Fortress Keys is set to anything "
        "other than Vanilla, then the maximum amount of Key Rings that can be selected by Random or "
        "Count will be 9. Otherwise, the maximum amount of Key Rings will be 8.";
    mOptionDescriptions[RSK_GERUDO_KEYS] = "Vanilla - Thieve's Hideout Keys will appear in their vanilla locations.\n"
                                           "\n"
                                           "Any dungeon - Thieve's Hideout Keys can only appear inside of any dungon.\n"
                                           "\n"
                                           "Overworld - Thieve's Hideout Keys can only appear outside of dungeons.\n"
                                           "\n"
                                           "Anywhere - Thieve's Hideout Keys can appear anywhere in the world.";
    mOptionDescriptions[RSK_BOSS_KEYSANITY] = "Start with - You will start with Boss keys from all dungeons.\n"
                                              "\n"
                                              "Vanilla - Boss Keys will appear in their vanilla locations.\n"
                                              "\n"
                                              "Own dungeon - Boss Keys can only appear in their respective dungeon.\n"
                                              "\n"
                                              "Any dungeon - Boss Keys can only appear inside of any dungon.\n"
                                              "\n"
                                              "Overworld - Boss Keys can only appear outside of dungeons.\n"
                                              "\n"
                                              "Anywhere - Boss Keys can appear anywhere in the world.";
    mOptionDescriptions[RSK_GANONS_BOSS_KEY] =
        "Vanilla - Ganon's Boss Key will appear in the vanilla location.\n"
        "\n"
        "Own dungeon - Ganon's Boss Key can appear anywhere inside Ganon's Castle.\n"
        "\n"
        "Start with - Places Ganon's Boss Key in your starting inventory."
        "\n"
        "Any dungeon - Ganon's Boss Key Key can only appear inside of any dungon.\n"
        "\n"
        "Overworld - Ganon's Boss Key Key can only appear outside of dungeons.\n"
        "\n"
        "Anywhere - Ganon's Boss Key Key can appear anywhere in the world.\n"
        "\n"
        "LACS - These settings put the boss key on the Light Arrow Cutscene location, from Zelda in Temple of Time as "
        "adult, with differing requirements:\n"
        "- Vanilla: Obtain the Shadow Medallion and Spirit Medallion\n"
        "- Stones: Obtain the specified amount of spiritual stones.\n"
        "- Medallions: Obtain the specified amount of medallions.\n"
        "- Dungeon rewards: Obtain the specified total sum of spiritual stones or medallions.\n"
        "- Dungeons: Complete the specified amount of dungeons. Dungeons are considered complete after stepping in to "
        "the blue warp after the boss.\n"
        "- Tokens: Obtain the specified amount of Skulltula tokens.\n"
        "\n"
        "100 GS Reward - Ganon's Boss Key will be awarded by the cursed rich man after you collect 100 Gold Skulltula "
        "Tokens.";
    mOptionDescriptions[RSK_LACS_OPTIONS] =
        "Standard Rewards - Greg does not change logic, Greg does not help obtain GBK, max "
        "number of rewards on slider does not change.\n"
        "\n"
        "Greg as Reward - Greg does change logic (can be part of expected path for obtaining "
        "GBK), Greg helps obtain GBK, max number of rewards on slider increases by 1 to "
        "account for Greg. \n"
        "\n"
        "Greg as Wildcard - Greg does not change logic, Greg helps obtain GBK, max number of "
        "rewards on slider does not change.";
    mOptionDescriptions[RSK_CUCCO_COUNT] = "The amount of cuccos needed to claim the reward from Anju the cucco lady";
    mOptionDescriptions[RSK_BIG_POE_COUNT] = "The Poe collector will give a reward for turning in this many Big Poes.";
    mOptionDescriptions[RSK_SKIP_CHILD_STEALTH] =
        "The crawlspace into Hyrule Castle goes straight to Zelda, skipping the guards.";
    mOptionDescriptions[RSK_SKIP_CHILD_ZELDA] =
        "Start with Zelda's Letter and the item Impa would normally give you and skip the sequence up "
        "until after meeting Zelda. Disables the ability to shuffle Weird Egg.";
    mOptionDescriptions[RSK_SKIP_EPONA_RACE] = "Epona can be summoned with Epona's Song without needing to race Ingo.";
    mOptionDescriptions[RSK_COMPLETE_MASK_QUEST] =
        "Once the happy mask shop is opened, all masks will be available to be borrowed.";
    mOptionDescriptions[RSK_SKIP_SCARECROWS_SONG] =
        "Start with the ability to summon Pierre the scarecrow. Pulling out an ocarina in the usual locations will "
        "automatically summon him.\n"
        "With \"Shuffle Ocarina Buttons\" enabled, you'll need at least two ocarina buttons to summon him.";
    mOptionDescriptions[RSK_ITEM_POOL] = "Sets how many major items appear in the item pool.\n"
                                         "\n"
                                         "Plentiful - Extra major items are added to the pool.\n"
                                         "\n"
                                         "Balanced - Original item pool.\n"
                                         "\n"
                                         "Scarce - Some excess items are removed, including health upgrades.\n"
                                         "\n"
                                         "Minimal - Most excess items are removed.";
    mOptionDescriptions[RSK_ICE_TRAPS] = "Sets how many items are replaced by ice traps.\n"
                                         "\n"
                                         "Off - No ice traps.\n"
                                         "\n"
                                         "Normal - Only Ice Traps from the base item pool are shuffled in.\n"
                                         "\n"
                                         "Extra - Chance to replace added junk items with additional ice traps.\n"
                                         "\n"
                                         "Mayhem - All added junk items will be Ice Traps.\n"
                                         "\n"
                                         "Onslaught - All junk items will be replaced by Ice Traps, even those "
                                         "in the base pool.";
    mOptionDescriptions[RSK_GOSSIP_STONE_HINTS] =
        "Allows Gossip Stones to provide hints on item locations. Hints mentioning "
        "\"Way of the Hero\" indicate a location that holds an item required to beat "
        "the seed.\n"
        "\n"
        "No hints - No hints will be given at all.\n"
        "\n"
        "Need Nothing - Hints are always available from Gossip Stones.\n"
        "\n"
        "Need Stone of Agony - Hints are only available after obtaining the Stone of Agony.\n"
        "\n"
        "Need Mask of Truth - Hints are only available whilst wearing the Mask of Truth.\n";
    mOptionDescriptions[RSK_HINT_CLARITY] =
        "Sets the difficulty of hints.\n"
        "\n"
        "Obscure - Hints are unique for each item, but the writing may be cryptic.\n"
        "Ex: Kokiri Sword > a butter knife\n"
        "\n"
        "Ambiguous - Hints are clearly written, but may refer to more than one item.\n"
        "Ex: Kokiri Sword > a sword\n"
        "\n"
        "Clear - Hints are clearly written and are unique for each item.\n"
        "Ex: Kokiri Sword > the Kokiri Sword";
    mOptionDescriptions[RSK_HINT_DISTRIBUTION] = "Sets how many hints will be useful.\n"
                                                 "\n"
                                                 "Useless - Only junk hints.\n"
                                                 "\n"
                                                 "Balanced - Recommended hint spread.\n"
                                                 "\n"
                                                 "Strong - More useful hints.\n"
                                                 "\n"
                                                 "Very Strong - Many powerful hints.";
    mOptionDescriptions[RSK_TOT_ALTAR_HINT] =
        "Reading the Temple of Time altar as child will tell you the locations of the Spiritual Stones.\n"
        "Reading the Temple of Time altar as adult will tell you the locations of the Medallions, as well as the "
        "conditions for building the Rainbow Bridge and getting the Boss Key for Ganon's Castle.";
    mOptionDescriptions[RSK_GANONDORF_HINT] =
        "Talking to Ganondorf in his boss room will tell you the location of the Light Arrows and Master Sword."
        "If this option is enabled and Ganondorf is reachable without these items, Gossip Stones will never hint the "
        "appropriote items.";//RANDOTODO make this hint text about no dupe hints a global hint for static hints. Add to navi?
    mOptionDescriptions[RSK_SHEIK_LA_HINT] =
        "Talking to Sheik inside Ganon's Castle will tell you the location of the Light Arrows."
        "If this option is enabled and Sheik is reachable without Light Arrows, Gossip Stones will never hint the "
        "Light Arrows.";
    mOptionDescriptions[RSK_DAMPES_DIARY_HINT] =
        "Reading the diary of Dampé the gravekeeper as adult will tell you the location of one of the Hookshots.";
    mOptionDescriptions[RSK_GREG_HINT] =
        "Talking to the chest game owner after buying a key will tell you the location of Greg the Green Rupee.";
    mOptionDescriptions[RSK_LOACH_HINT] =
        "Talking to the fishing pond owner and asking to talk about something will tell you what's the reward for the Hyrule Loach.";
    mOptionDescriptions[RSK_SARIA_HINT] = "Talking to Saria either in person or through Saria's Song will tell you the "
                                          "location of a progressive magic meter.";
    mOptionDescriptions[RSK_FISHING_POLE_HINT] = "Talking to the fishing pond owner without the fishing pole will tell you its location.";
    mOptionDescriptions[RSK_OOT_HINT] = "Sheik in the Temple of Time will tell you the item and song on the Ocarina of Time.";
    mOptionDescriptions[RSK_FROGS_HINT] = "Standing near the pedestal for the frogs in Zora's River will tell you the "
                                          "reward for the frogs' ocarina game.";
    mOptionDescriptions[RSK_BIGGORON_HINT] = "Talking to Biggoron will tell you the item he will give you in exchange for the Claim Check.";
    mOptionDescriptions[RSK_BIG_POES_HINT] = "Talking to the Poe Collector in the Market Guardhouse while adult will tell you what you receive for handing in Big Poes.";
    mOptionDescriptions[RSK_CHICKENS_HINT] = "Talking to Anju as a child will tell you the item she will give you for delivering her Cuccos to the pen";
    mOptionDescriptions[RSK_MALON_HINT] = "Talking to Malon as adult will tell you the item on \"Link's cow\", the cow you win from beating her time on the Lon Lon Obsticle Course.";
    mOptionDescriptions[RSK_HBA_HINT] = "Talking to the Horseback Archery gerudo in Gerudo Fortress, or the nearby sign, will tell you what you win for scoring 1000 and 1500 points on Horseback Archery.";
    mOptionDescriptions[RSK_WARP_SONG_HINTS] = "Playing a warp song will tell you where it leads. (If warp song destinations are vanilla, this is always enabled.)";
    mOptionDescriptions[RSK_SCRUB_TEXT_HINT] = "Business scrubs will reveal the identity of what they're selling.";
    mOptionDescriptions[RSK_MERCHANT_TEXT_HINT] = "Merchants will reveal the identity of what they're selling (Shops are not affected by this setting).";
    mOptionDescriptions[RSK_KAK_10_SKULLS_HINT] = "Talking to the Cursed Resident in the Skultulla House who is saved after 10 tokens will tell you the reward";
    mOptionDescriptions[RSK_KAK_20_SKULLS_HINT] = "Talking to the Cursed Resident in the Skultulla House who is saved after 20 tokens will tell you the reward";
    mOptionDescriptions[RSK_KAK_30_SKULLS_HINT] = "Talking to the Cursed Resident in the Skultulla House who is saved after 30 tokens will tell you the reward";
    mOptionDescriptions[RSK_KAK_40_SKULLS_HINT] = "Talking to the Cursed Resident in the Skultulla House who is saved after 40 tokens will tell you the reward";
    mOptionDescriptions[RSK_KAK_50_SKULLS_HINT] = "Talking to the Cursed Resident in the Skultulla House who is saved after 50 tokens will tell you the reward";
    mOptionDescriptions[RSK_KAK_100_SKULLS_HINT] = "Talking to the Cursed Resident in the Skultulla House who is saved after 100 tokens will tell you the reward";
    mOptionDescriptions[RSK_MASK_SHOP_HINT] = "Reading the mask shop sign will tell you rewards from showing masks at the Deku Theatre.";
    mOptionDescriptions[RSK_FULL_WALLETS] = "Start with a full wallet. All wallet upgrades come filled with rupees.";
    mOptionDescriptions[RSK_BOMBCHUS_IN_LOGIC] =
        "Bombchus are properly considered in logic. Without this setting, any Bombchu requirement"
        " is filled by Bomb Bag + a renewable source of Bombchus\n"
        "\n"
        "The first Bombchu pack will always be 20, and subsequent packs will be "
        "5 or 10 based on how many you have.\n"
        "Once found, they can be replenished at the Bombchu shop.\n"
        "\n"
        "Bombchu Bowling is opened by obtaining Bombchus.";
    mOptionDescriptions[RSK_ENABLE_BOMBCHU_DROPS] = "Once you obtain bombchus for the first time, refills can be found "
                                                    "in bushes and other places where bomb drops can normally spawn."
                                                    "\n"
                                                    "If you have Bombchus in Logic disabled, you will also need a"
                                                    "Bomb bag for bombchus to drop";
    mOptionDescriptions[RSK_BLUE_FIRE_ARROWS] =
        "Ice Arrows act like Blue Fire, making them able to melt red ice. "
        "Item placement logic will respect this option, so it might be required to use this to progress.";
    mOptionDescriptions[RSK_SKELETON_KEY] = "Adds a new item called the \"Skeleton Key\", it unlocks every dungeon door locked by a small key.";
    mOptionDescriptions[RSK_SUNLIGHT_ARROWS] =
        "Light Arrows can be used to light up the sun switches instead of using the Mirror Shield. "
        "Item placement logic will respect this option, so it might be required to use this to progress.";
    mOptionDescriptions[RSK_LOGIC_RULES] =
        "Glitchless - No glitches are required, but may require some minor tricks. Additional tricks may be enabled "
        "and disabled below.\n"
        "\n"
        //"Glitched - Glitches may be required to beat the game. You can disable and enable glitches below.\n"
        //"\n"
        "No logic - Item placement is completely random. MAY BE IMPOSSIBLE TO BEAT.\n"
        "\n"
        "Vanilla - Places all items and dungeon rewards in their vanilla locations.";
    mOptionDescriptions[RSK_ALL_LOCATIONS_REACHABLE] = "When this options is enabled, the randomizer will "
                                                       "guarantee that every item is obtainable and every "
                                                       "location is reachable. When disabled, only "
                                                       "required items and locations to beat the game "
                                                       "will be guaranteed reachable.";
    mOptionDescriptions[RSK_ENABLE_GLITCH_CUTSCENES] =
        "The cutscenes of the Poes in Forest Temple and Darunia in Fire Temple will not be skipped. "
        "These cutscenes are only useful for glitched gameplay and can be safely skipped otherwise.";
    mOptionDescriptions[RSK_SHUFFLE_BOSS_SOULS] = "Shuffles 8 boss souls (one for each blue warp dungeon). A boss will not appear until you collect its respective soul."
                "\n\"On + Ganon\" will also hide Ganon and Ganondorf behind a boss soul.";
}
} // namespace Rando