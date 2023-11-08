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
        "Selection Only - Specify which dungeons are Vanilla or Master Quest.";
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
        "Shuffle entrances into a mixed pool instead of separate ones.\n"
        "\n"
        "For example, enabling the settings to shuffle grotto, dungeon, and overworld entrances and "
        "selecting grotto and dungeon entrances here will allow a dungeon to be inside a grotto or "
        "vice versa, while overworld entrances are shuffled in their own separate pool and indoors stay vanilla.";
    mOptionDescriptions[RSK_MIX_DUNGEON_ENTRANCES] = "Dungeon entrances will be part of the mixed pool";
    mOptionDescriptions[RSK_MIX_OVERWORLD_ENTRANCES] = "Overworld entrances will be part of the mixed pool";
    mOptionDescriptions[RSK_MIX_INTERIOR_ENTRANCES] = "Interior entrances will be part of the mixed pool";
    mOptionDescriptions[RSK_MIX_GROTTO_ENTRANCES] = "Grotto entrances will be part of the mixed pool";
}
} // namespace Rando