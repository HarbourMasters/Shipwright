#include "randomizer_tricks.h"
#include <map>
#include <string>
#include <libultraship/bridge.h>
#include "z64.h"

// Difficulty base tag vectors:
std::vector<RandomizerTrickTag> noviceBase{ RTTAG_NOVICE };
std::vector<RandomizerTrickTag> intermediateBase{ RTTAG_INTERMEDIATE };
std::vector<RandomizerTrickTag> advancedBase{ RTTAG_ADVANCED };
std::vector<RandomizerTrickTag> expertBase{ RTTAG_EXPERT };
std::vector<RandomizerTrickTag> extremeBase{ RTTAG_EXTREME };

// Trick names and descriptions obtained from https://github.com/OoTRandomizer/OoT-Randomizer/blob/Dev/SettingsList.py
// with additions and edits Difficulties initially decided on personal opinions, will be updated based on community
// feedback. todo: update glitch difficulties Novice - if you were told X is possible, then you would be likely be able
// to do it either first time or it is something easily repeatable without any setup Intermediate - may take a few
// attempts or a small setup to achieve, or is an easily repeatable trick with a small frame window Advanced - may have
// a tight time window to achieve, have a long setup, or small frame window to perform Expert - typically a series of
// small frame windows or longer setups, likely to require buffering Extreme - this may require several frame perfect
// tricks, long setups or heavy reliance on environment to perform Experimental - not implemented; these tricks may let
// you put the game into a softlockable state Please see
// https://www.youtube.com/playlist?list=PLqsXSioZwQyoa23_27l5NZq5IZdKRi5Sm for reference on performing several of these
// tricks
//            RandomizerTrick,                              RTVORMQ,         RTAREA,                         rt_tags
//            rt_glitch (is it a glitch)                 "Short name", "Description"
std::unordered_map<RandomizerTrick, RandomizerTrickObject> rtObjects = {
    RT_OBJECT(RT_ACUTE_ANGLE_CLIP, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Acute angle clip",
              "Enables locations requiring jumpslash clips through walls which meet at an acute angle."),
    RT_OBJECT(RT_ADVANCED_CLIPS, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Advanced clips",
              "Enables locations requiring clips through walls and objects requiring precise jumps or other tricks."),
    RT_OBJECT(RT_BLANK_A, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Blank A",
              "Enables locations requiring blank A button; NOTE: this requires the 'Quick Putaway' restoration."),
    RT_OBJECT(RT_DOOM_JUMP, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Doom Jump",
              "Enables locations requiring doom jumps."),
    RT_OBJECT(RT_EPG, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "EPG",
              "Enables locations requiring use of the Entrance Point Glitch."),
    RT_OBJECT(RT_EQUIP_SWAP, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Equip Swap",
              "Enables locations requiring use of equip swap; NOTE: this may expect the 'Allow cursor to be over any "
              "slot' enhancement to be turned off."),
    RT_OBJECT(RT_EQUIP_SWAP_EXPECTS_DINS, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true,
              "Equip Swap Require's Din's Fire",
              "Enables locations requiring use of equip swap once din's fire is found."),
    RT_OBJECT(RT_FLAME_STORAGE, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Flame Storage",
              "Enables locations requiring flame storage."),
    RT_OBJECT(RT_GROUND_CLIP, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Ground Clip",
              "Enables locations requiring ground clips."),
    RT_OBJECT(RT_GROUND_JUMP, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Ground Jump",
              "Enables locations requiring ground jumps."),
    RT_OBJECT(RT_HESS, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "HESS",
              "Enables locations requiring a Hyper Extended Super Slide."),
    RT_OBJECT(RT_HOOKSHOT_CLIP, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Hookshot Clip",
              "Enables locations requiring hookshot clips."),
    RT_OBJECT(RT_HOOKSHOT_JUMP, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "Hookshot Jump",
              "Enables locations requiring hookshot jumps."),
    RT_OBJECT(RT_ISG, RTVORMQ_BOTH, RTAREA_GENERAL, &advancedBase, true, "ISG",
              "Enables locations requiring use of the infinite sword glitch."),
    RT_OBJECT(RT_VISIBLE_COLLISION, RTVORMQ_BOTH, RTAREA_GENERAL, &noviceBase, false,
              "Pass Through Visible One-Way Collision",
              "Allows climbing through the platform to reach Impa's House Back as adult with no items and going "
              "through the Kakariko Village Gate as child when coming from the Mountain Trail side."),
    RT_OBJECT(RT_GROTTOS_WITHOUT_AGONY, RTVORMQ_BOTH, RTAREA_GENERAL, &noviceBase, false,
              "Hidden Grottos without Stone of Agony", "Allows entering hidden grottos without the Stone of Agony."),
    RT_OBJECT(RT_FEWER_TUNIC_REQUIREMENTS, RTVORMQ_BOTH, RTAREA_GENERAL, &intermediateBase, false,
              "Fewer Tunic Requirements",
              "Allows the following possible without Tunics: - Enter Water Temple. The area below the center pillar "
              "still requires Zora Tunic. Applies to MQ also. - Enter Fire Temple. Only the first floor is accessible, "
              "and not Volvagia. Only Applies to MQ."),
    RT_OBJECT(RT_RUSTED_SWITCHES, RTVORMQ_BOTH, RTAREA_GENERAL, &noviceBase, false,
              "Hammer Rusted Switches Through Walls",
              "Applies to: - Fire Temple Highest Goron Chest. - MQ Fire Temple Lizalfos Maze. - MQ Spirit Trial."),
    RT_OBJECT(RT_FLAMING_CHESTS, RTVORMQ_BOTH, RTAREA_GENERAL, &intermediateBase, false, "Flaming Chests",
              "The chests encircled in flames in Gerudo Training Grounds and in Spirit Temple can be opened by running "
              "into the flames while Link is invincible after taking damage."),
    // RT_OBJECT(RT_BUNNY_HOOD_JUMPS,                        RTVORMQ_BOTH,    RTAREA_GENERAL, &advancedBase, false,
    // "Bunny Hood Jumps", "Allows reaching locations using Bunny Hood's extended jumps."),
    // RT_OBJECT(RT_DAMAGE_BOOST_SIMPLE,                     RTVORMQ_BOTH,    RTAREA_GENERAL, &advancedBase, false,
    // "Simple damage boosts", "Allows damage boosts to reach Hyrule Castle guards, the bomb bag area in DC and the
    // Gerudo Valley crate Piece of Heart. Can be combined with \"Simple hover boosts\" for reaching far distances."),
    // RT_OBJECT(RT_HOVER_BOOST_SIMPLE,                      RTVORMQ_BOTH,    RTAREA_GENERAL, &advancedBase, false,
    // "Simple hover boosts", "Allows equipping of hover boots when link is moving at high speeds to extend distance
    // covered. Can be combined with \"Simple damage boosts\" for greater uses."),
    RT_OBJECT(RT_KF_ADULT_GS, RTVORMQ_BOTH, RTAREA_KOKIRI_FOREST, &noviceBase, false,
              "Adult Kokiri Forest GS with Hover Boots",
              "Can be obtained without Hookshot by using the Hover Boots off of one of the roots."),
    RT_OBJECT(RT_LW_BRIDGE, RTVORMQ_BOTH, RTAREA_LOST_WOODS, &expertBase, false,
              "Jump onto the Lost Woods Bridge as Adult with Nothing",
              "With very precise movement it's possible for adult to jump onto the bridge without needing Longshot, "
              "Hover Boots, or Bean."),
    RT_OBJECT(RT_LW_MIDO_BACKFLIP, RTVORMQ_BOTH, RTAREA_LOST_WOODS, &noviceBase, false, "Backflip over Mido as Adult",
              "With a specific position and angle, you can backflip over Mido."),
    RT_OBJECT(RT_LW_GS_BEAN, RTVORMQ_BOTH, RTAREA_LOST_WOODS, &intermediateBase, false,
              "Lost Woods Adult GS without Bean",
              "You can collect the token with a precise Hookshot use, as long as you can kill the Skulltula somehow "
              "first. It can be killed using Longshot, Bow, Bombchus or Din's Fire."),
    RT_OBJECT(RT_HC_STORMS_GS, RTVORMQ_BOTH, RTAREA_HYRULE_CASTLE, &intermediateBase, false,
              "Hyrule Castle Storms Grotto GS with Just Boomerang",
              "With precise throws, the Boomerang alone can kill the Skulltula and collect the token, without first "
              "needing to blow up the wall."),
    RT_OBJECT(RT_KAK_MAN_ON_ROOF, RTVORMQ_BOTH, RTAREA_KAKARIKO_VILLAGE, &noviceBase, false,
              "Man on Roof without Hookshot",
              "Can be reached by side-hopping off the watchtower as either age, or by jumping onto the potion shop's "
              "roof from the ledge as adult."),
    RT_OBJECT(RT_KAK_TOWER_GS, RTVORMQ_BOTH, RTAREA_KAKARIKO_VILLAGE, &intermediateBase, false,
              "Kakariko Tower GS with Jump Slash",
              "Climb the tower as high as you can without touching the Gold Skulltula, then let go and jump slash "
              "immediately. By jump-slashing from as low on the ladder as possible to still hit the Skulltula, this "
              "trick can be done without taking fall damage."),
    RT_OBJECT(RT_KAK_ADULT_WINDMILL_POH, RTVORMQ_BOTH, RTAREA_KAKARIKO_VILLAGE, &noviceBase, false,
              "Windmill PoH as Adult with Nothing", "Can jump up to the spinning platform from below as adult."),
    // RT_OBJECT(RT_KAK_CHILD_WINDMILL_POH,                  RTVORMQ_BOTH,    RTAREA_KAKARIKO_VILLAGE, &extremeBase,
    // false,                                     "Windmill PoH as Child with Precise Jump Slash", "Can jump up to the
    // spinning platform from below as child with a precise jumpslash timed with the platforms rotation."),
    RT_OBJECT(
        RT_KAK_ROOFTOP_GS, RTVORMQ_BOTH, RTAREA_KAKARIKO_VILLAGE, &advancedBase, false,
        "Kakariko Rooftop GS with Hover Boots",
        "Take the Hover Boots from the entrance to Impa's House over to the rooftop of Skulltula House. From there, a "
        "precise Hover Boots backwalk with backflip can be used to get onto a hill above the side of the village. And "
        "then from there you can Hover onto Impa's rooftop to kill the Skulltula and backflip into the token."),
    RT_OBJECT(RT_GY_POH, RTVORMQ_BOTH, RTAREA_GRAVEYARD, &intermediateBase, false,
              "Graveyard Freestanding PoH with Boomerang",
              "Using a precise moving setup you can obtain the Piece of Heart by having the Boomerang interact with it "
              "along the return path."),
    RT_OBJECT(
        RT_GY_CHILD_DAMPE_RACE_POH, RTVORMQ_BOTH, RTAREA_GRAVEYARD, &noviceBase, false, "Second Dampe Race as Child",
        "It is possible to complete the second dampe race as child in under a minute, but it is a strict time limit."),
    RT_OBJECT(RT_GY_SHADOW_FIRE_ARROWS, RTVORMQ_BOTH, RTAREA_GRAVEYARD, &expertBase, false,
              "Shadow Temple Entry with Fire Arrows",
              "It is possible to light all of the torches to open the Shadow Temple entrance with just Fire Arrows, "
              "but you must be very quick, precise, and strategic with how you take your shots."),
    RT_OBJECT(RT_DMT_SOIL_GS, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_TRAIL, &intermediateBase, false,
              "Death Mountain Trail Soil GS without Destroying Boulder",
              "Bugs will go into the soft soil even while the boulder is still blocking the entrance. Then, using a "
              "precise moving setup you can kill the Gold Skulltula and obtain the token by having the Boomerang "
              "interact with it along the return path."),
    RT_OBJECT(RT_DMT_BOMBABLE, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_TRAIL, &intermediateBase, false,
              "Death Mountain Trail Chest with Strength",
              "Child Link can blow up the wall using a nearby bomb flower. You must backwalk with the flower and then "
              "quickly throw it toward the wall."),
    RT_OBJECT(RT_DMT_HOOKSHOT_LOWER_GS, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_TRAIL, &intermediateBase, false,
              "Death Mountain Trail Lower Red Rock GS with Hookshot",
              "After killing the Skulltula, the token can be fished out of the rock without needing to destroy it, by "
              "using the Hookshot in the correct way."),
    RT_OBJECT(RT_DMT_HOVERS_LOWER_GS, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_TRAIL, &advancedBase, false,
              "Death Mountain Trail Lower Red Rock GS with Hover Boots",
              "After killing the Skulltula, the token can be collected without needing to destroy the rock by "
              "backflipping down onto it with the Hover Boots. First use the Hover Boots to stand on a nearby fence, "
              "and go for the Skulltula Token from there."),
    RT_OBJECT(RT_DMT_BEAN_LOWER_GS, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_TRAIL, &expertBase, false,
              "Death Mountain Trail Lower Red Rock GS with Magic Bean",
              "After killing the Skulltula, the token can be collected without needing to destroy the rock by jumping "
              "down onto it from the bean plant, midflight, with precise timing and positioning."),
    RT_OBJECT(RT_DMT_JS_LOWER_GS, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_TRAIL, &intermediateBase, false,
              "Death Mountain Trail Lower Red Rock GS with Jump Slash",
              "After killing the Skulltula, the token can be collected without needing to destroy the rock by jump "
              "slashing from a precise angle."),
    RT_OBJECT(RT_DMT_CLIMB_HOVERS, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_TRAIL, &advancedBase, false,
              "Death Mountain Trail Climb with Hover Boots",
              "It is possible to use the Hover Boots to bypass needing to destroy the boulders blocking the path to "
              "the top of Death Mountain."),
    RT_OBJECT(
        RT_DMT_UPPER_GS, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_TRAIL, &noviceBase, false,
        "Death Mountain Trail Upper Red Rock GS without Hammer",
        "After killing the Skulltula, the token can be collected by backflipping into the rock at the correct angle."),
    // RT_OBJECT(RT_DMT_BOLERO_BIGGORON,                     RTVORMQ_BOTH,    RTAREA_DEATH_MOUNTAIN_TRAIL,
    // &intermediateBase,           false,                                     "Deliver Eye Drops with Bolero of Fire",
    // "Playing a warp song normally causes a trade item to spoil immediately, however, it is possible use Bolero to
    // reach Biggoron and still deliver the Eye Drops before they spoil. If you do not wear the Goron Tunic, the heat
    // timer inside the crater will override the trade item\'s timer. When you exit to Death Mountain Trail you will
    // have one second to show the Eye Drops before they expire. You can get extra time to show the Eye Drops if you
    // warp immediately upon receiving them. If you don't have many hearts, you may have to reset the heat timer by
    // quickly dipping in and out of Darunia\'s chamber or quickly equipping and unequipping the Goron Tunic. This trick
    // does not apply if \"Randomize Warp Song Destinations\" is enabled, or if the settings are such that trade items
    // do not need to be delivered within a time limit."),
    RT_OBJECT(RT_GC_POT, RTVORMQ_BOTH, RTAREA_GORON_CITY, &advancedBase, false,
              "Goron City Spinning Pot PoH with Bombchu",
              "A Bombchu can be used to stop the spinning pot, but it can be quite finicky to get it to work."),
    RT_OBJECT(RT_GC_POT_STRENGTH, RTVORMQ_BOTH, RTAREA_GORON_CITY, &intermediateBase, false,
              "Goron City Spinning Pot PoH with Strength",
              "Allows for stopping the Goron City Spinning Pot using a bomb flower alone, requiring strength in lieu "
              "of inventory explosives."),
    RT_OBJECT(RT_GC_ROLLING_STRENGTH, RTVORMQ_BOTH, RTAREA_GORON_CITY, &intermediateBase, false,
              "Rolling Goron (Hot Rodder Goron) as Child with Strength",
              "Use the bombflower on the stairs or near Medigoron. Timing is tight, especially without backwalking."),
    RT_OBJECT(RT_GC_LINK_GORON_DINS, RTVORMQ_BOTH, RTAREA_GORON_CITY, &noviceBase, false,
              "Stop Link the Goron with Din\'s Fire", "The timing is quite awkward."),
    RT_OBJECT(RT_GC_LEFTMOST, RTVORMQ_BOTH, RTAREA_GORON_CITY, &advancedBase, false,
              "Goron City Maze Left Chest with Hover Boots",
              "A precise backwalk starting from on top of the crate and ending with a precisely-timed backflip can "
              "reach this chest without needing either the Hammer or Silver Gauntlets."),
    RT_OBJECT(RT_GC_GROTTO, RTVORMQ_BOTH, RTAREA_GORON_CITY, &advancedBase, false,
              "Goron City Grotto with Hookshot While Taking Damage",
              "It is possible to reach the Goron City Grotto by quickly using the Hookshot while in the midst of "
              "taking damage from the lava floor."),
    RT_OBJECT(RT_DMC_HOVER_BEAN_POH, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_CRATER, &noviceBase, false,
              "Crater\'s Bean PoH with Hover Boots",
              "Hover from the base of the bridge near Goron City and walk up the very steep slope."),
    RT_OBJECT(RT_DMC_BOLERO_JUMP, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_CRATER, &extremeBase, false,
              "Death Mountain Crater Jump to Bolero",
              "As Adult , using a shield to drop a pot while you have the perfect speed and position, the pot can push "
              "you that little extra distance you need to jump across the gap in the bridge."),
    RT_OBJECT(RT_DMC_BOULDER_JS, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_CRATER, &noviceBase, false,
              "Death Mountain Crater Upper to Lower with Hammer",
              "With the Hammer, you can jump slash the rock twice in the same jump in order to destroy it before you "
              "fall into the lava."),
    RT_OBJECT(RT_DMC_BOULDER_SKIP, RTVORMQ_BOTH, RTAREA_DEATH_MOUNTAIN_CRATER, &intermediateBase, false,
              "Death Mountain Crater Upper to Lower Boulder Skip",
              "As adult, With careful positioning, you can jump to the ledge where the boulder is, then use repeated "
              "ledge grabs to shimmy to a climbable ledge. This trick supersedes \"Death Mountain Crater Upper to "
              "Lower with Hammer\"."),
    RT_OBJECT(RT_ZR_LOWER, RTVORMQ_BOTH, RTAREA_ZORAS_RIVER, &intermediateBase, false,
              "Zora\'s River Lower Freestanding PoH as Adult with Nothing",
              "Adult can reach this PoH with a precise jump, no Hover Boots required."),
    RT_OBJECT(RT_ZR_UPPER, RTVORMQ_BOTH, RTAREA_ZORAS_RIVER, &intermediateBase, false,
              "Zora\'s River Upper Freestanding PoH as Adult with Nothing",
              "Adult can reach this PoH with a precise jump, no Hover Boots required."),
    RT_OBJECT(RT_ZR_HOVERS, RTVORMQ_BOTH, RTAREA_ZORAS_RIVER, &noviceBase, false, "Zora\'s Domain Entry with Cucco",
              "You can fly behind the waterfall with a Cucco as child."),
    RT_OBJECT(RT_ZR_CUCCO, RTVORMQ_BOTH, RTAREA_ZORAS_RIVER, &noviceBase, false,
              "Zora\'s Domain Entry with Hover Boots", "Can hover behind the waterfall as adult."),
    RT_OBJECT(RT_ZD_KING_ZORA_SKIP, RTVORMQ_BOTH, RTAREA_ZORAS_DOMAIN, &intermediateBase, false,
              "Skip King Zora as Adult with Nothing",
              "With a precise jump as adult, it is possible to get on the fence next to King Zora from the front to "
              "access Zora's Fountain."),
    RT_OBJECT(RT_ZD_GS, RTVORMQ_BOTH, RTAREA_ZORAS_DOMAIN, &intermediateBase, false,
              "Zora\'s Domain GS with No Additional Items",
              "A precise jump slash can kill the Skulltula and recoil back onto the top of the frozen waterfall. To "
              "kill it, the logic normally guarantees one of Hookshot, Bow, or Magic."),
    RT_OBJECT(RT_LH_LAB_WALL_GS, RTVORMQ_BOTH, RTAREA_LAKE_HYLIA, &noviceBase, false,
              "Lake Hylia Lab Wall GS with Jump Slash",
              "The jump slash to actually collect the token is somewhat precise."),
    RT_OBJECT(RT_LH_LAB_DIVING, RTVORMQ_BOTH, RTAREA_LAKE_HYLIA, &noviceBase, false,
              "Lake Hylia Lab Dive without Gold Scale",
              "Remove the Iron Boots in the midst of Hookshotting the underwater crate."),
    RT_OBJECT(RT_LH_WATER_HOOKSHOT, RTVORMQ_BOTH, RTAREA_LAKE_HYLIA, &intermediateBase, false,
              "Water Temple Entry without Iron Boots using Hookshot",
              "When entering Water Temple using Gold Scale instead of Iron Boots, the Longshot is usually used to be "
              "able to hit the switch and open the gate. But, by standing in a particular spot, the switch can be hit "
              "with only the reach of the Hookshot."),
    RT_OBJECT(RT_GV_CRATE_HOVERS, RTVORMQ_BOTH, RTAREA_GERUDO_VALLEY, &intermediateBase, false,
              "Gerudo Valley Crate PoH as Adult with Hover Boots",
              "From the far side of Gerudo Valley, a precise Hover Boots movement and jump-slash recoil can allow "
              "adult to reach the ledge with the crate PoH without needing Longshot. You will take fall damage."),
    RT_OBJECT(RT_GF_KITCHEN, RTVORMQ_BOTH, RTAREA_GERUDO_FORTRESS, &noviceBase, false,
              "Thieves\' Hideout \"Kitchen\" with No Additional Items",
              "Allows passing through the kitchen by avoiding being seen by the guards. The logic normally guarantees "
              "Bow or Hookshot to stun them from a distance, or Hover Boots to cross the room without needing to deal "
              "with the guards."),
    RT_OBJECT(RT_GF_JUMP, RTVORMQ_BOTH, RTAREA_GERUDO_FORTRESS, &noviceBase, false, "Gerudo\'s Fortress Ledge Jumps",
              "Adult can jump onto the top roof of the fortress without going through the interior of the hideout."),
    // RT_OBJECT(RT_HW_BUNNY_CROSSING,                       RTVORMQ_BOTH,    RTAREA_WASTELAND, &noviceBase, false,
    // "Wasteland Crossing with Bunny Hood","You can beat the quicksand by using the increased speed of the Bunny Hood.
    // Note that jumping to the carpet merchant as child typically requires a fairly precise jump slash."),
    RT_OBJECT(RT_HW_CROSSING, RTVORMQ_BOTH, RTAREA_WASTELAND, &intermediateBase, false,
              "Wasteland Crossing without Hover Boots or Longshot",
              "You can beat the quicksand by backwalking across it in a specific way. Note that jumping to the carpet "
              "merchant as child typically requires a fairly precise jump slash."),
    RT_OBJECT(RT_LENS_HW, RTVORMQ_BOTH, RTAREA_WASTELAND, &intermediateBase, false, "Lensless Wasteland",
              "By memorizing the path, you can travel through the Wasteland without using the Lens of Truth to see the "
              "Poe. The equivalent trick for going in reverse through the Wasteland is \"Reverse Wasteland\"."),
    RT_OBJECT(
        RT_HW_REVERSE, RTVORMQ_BOTH, RTAREA_WASTELAND, &intermediateBase, false, "Reverse Wasteland",
        "By memorizing the path, you can travel through the Wasteland in reverse. Note that jumping to the carpet "
        "merchant as child typically requires a fairly precise jump slash. The equivalent trick for going forward "
        "through the Wasteland is \"Lensless Wasteland\". To cross the river of sand with no additional items, be sure "
        "to also enable \"Wasteland Crossing without Hover Boots or Longshot\". Unless all overworld entrances are "
        "randomized, child Link will not be expected to do anything at Gerudo's Fortress."),
    RT_OBJECT(RT_COLOSSUS_GS, RTVORMQ_BOTH, RTAREA_DESERT_COLOSSUS, &noviceBase, false,
              "Colossus Hill GS with Hookshot",
              "Somewhat precise. If you kill enough Leevers you can get enough of a break to take some time to aim "
              "more carefully."),
    RT_OBJECT(RT_DEKU_BASEMENT_GS, RTVORMQ_VANILLA, RTAREA_DEKU_TREE, &noviceBase, false,
              "Deku Tree Basement Vines GS with Jump Slash", "Can be defeated by doing a precise jump slash."),
    RT_OBJECT(RT_DEKU_B1_SKIP, RTVORMQ_BOTH, RTAREA_DEKU_TREE, &intermediateBase, false,
              "Deku Tree Basement without Slingshot",
              "A precise jump can be used to skip needing to use the Slingshot to go around B1 of the Deku Tree. If "
              "used with the \"Closed Forest\" setting, a Slingshot will not be guaranteed to exist somewhere inside "
              "the Forest. This trick applies to both Vanilla and Master Quest."),
    RT_OBJECT(RT_DEKU_B1_BOW_WEBS, RTVORMQ_VANILLA, RTAREA_DEKU_TREE, &noviceBase, false,
              "Deku Tree Basement Web to Gohma with Bow",
              "All spider web walls in the Deku Tree basement can be burnt as adult with just a bow by shooting "
              "through torches. This trick only applies to the circular web leading to Gohma; the two vertical webs "
              "are always in logic. Backflip onto the chest near the torch at the bottom of the vine wall. With "
              "precise positioning you can shoot through the torch to the right edge of the circular web. This allows "
              "completion of adult Deku Tree with no fire source."),
    RT_OBJECT(RT_DEKU_MQ_COMPASS_GS, RTVORMQ_MQ, RTAREA_DEKU_TREE, &noviceBase, false,
              "Deku Tree MQ Compass Room GS Boulders with Just Hammer",
              "Climb to the top of the vines, then let go and jump slash immediately to destroy the boulders using the "
              "Hammer, without needing to spawn a Song of Time block."),
    RT_OBJECT(RT_DEKU_MQ_LOG, RTVORMQ_MQ, RTAREA_DEKU_TREE, &noviceBase, false,
              "Deku Tree MQ Roll Under the Spiked Log",
              "You can get past the spiked log by rolling to briefly shrink your hitbox. As adult, the timing is a bit "
              "more precise."),
    RT_OBJECT(RT_DC_SCARECROW_GS, RTVORMQ_VANILLA, RTAREA_DODONGOS_CAVERN, &noviceBase, false,
              "Dodongo\'s Cavern Scarecrow GS with Armos Statue",
              "You can jump off an Armos Statue to reach the alcove with the Gold Skulltula. It takes quite a long "
              "time to pull the statue the entire way. The jump to the alcove can be a bit picky when done as child."),
    RT_OBJECT(RT_DC_VINES_GS, RTVORMQ_VANILLA, RTAREA_DODONGOS_CAVERN, &noviceBase, false,
              "Dodongo\'s Cavern Vines GS from Below with Longshot",
              "The vines upon which this Skulltula rests are one- sided collision. You can use the Longshot to get it "
              "from below, by shooting it through the vines, bypassing the need to lower the staircase."),
    RT_OBJECT(RT_DC_STAIRCASE, RTVORMQ_VANILLA, RTAREA_DODONGOS_CAVERN, &noviceBase, false,
              "Dodongo\'s Cavern Staircase with Bow",
              "The Bow can be used to knock down the stairs with two well-timed shots."),
    RT_OBJECT(RT_DC_SLINGSHOT_SKIP, RTVORMQ_VANILLA, RTAREA_DODONGOS_CAVERN, &expertBase, false,
              "Dodongo\'s Cavern Child Slingshot Skips",
              "With precise platforming, child can cross the platforms while the flame circles are there. When "
              "enabling this trick, it's recommended that you also enable the Adult variant: \"Dodongo's Cavern Spike "
              "Trap Room Jump without Hover Boots\"."),
    RT_OBJECT(RT_DC_SCRUB_ROOM, RTVORMQ_VANILLA, RTAREA_DODONGOS_CAVERN, &noviceBase, false,
              "Dodongo\'s Cavern Two Scrub Room with Strength",
              "With help from a conveniently-positioned block, Adult can quickly carry a bomb flower over to destroy "
              "the mud wall blocking the room with two Deku Scrubs."),
    RT_OBJECT(RT_DC_JUMP, RTVORMQ_BOTH, RTAREA_DODONGOS_CAVERN, &noviceBase, false,
              "Dodongo\'s Cavern Spike Trap Room Jump without Hover Boots",
              "The jump is adult Link only. Applies to both Vanilla and MQ."),
    RT_OBJECT(RT_DC_HAMMER_FLOOR, RTVORMQ_BOTH, RTAREA_DODONGOS_CAVERN, &noviceBase, false,
              "Dodongo\'s Cavern Smash the Boss Lobby Floor",
              "The bombable floor before King Dodongo can be destroyed with Hammer if hit in the very center. This is "
              "only relevant with Shuffle Boss Entrances or if Dodongo's Cavern is MQ and either variant of "
              "\"Dodongo's Cavern MQ Light the Eyes with Strength\" is on."),
    RT_OBJECT(RT_DC_MQ_CHILD_BOMBS, RTVORMQ_MQ, RTAREA_DODONGOS_CAVERN, &advancedBase, false,
              "Dodongo\'s Cavern MQ Early Bomb Bag Area as Child",
              "With a precise jump slash from above, you can reach the Bomb Bag area as only child without needing a "
              "Slingshot. You will take fall damage."),
    RT_OBJECT(RT_DC_MQ_CHILD_EYES, RTVORMQ_MQ, RTAREA_DODONGOS_CAVERN, &expertBase, false,
              "Dodongo\'s Cavern MQ Light the Eyes with Strength as Child",
              "If you move very quickly, it is possible to use the bomb flower at the top of the room to light the "
              "eyes. To perform this trick as child is significantly more difficult than adult. The player is also "
              "expected to complete the DC back area without explosives, including getting past the Armos wall to the "
              "switch for the boss door."),
    RT_OBJECT(
        RT_DC_MQ_ADULT_EYES, RTVORMQ_MQ, RTAREA_DODONGOS_CAVERN, &advancedBase, false,
        "Dodongo\'s Cavern MQ Light the Eyes with Strength as Adult",
        "If you move very quickly, it is possible to use the bomb flower at the top of the room to light the eyes."),
    RT_OBJECT(RT_JABU_ALCOVE_JUMP_DIVE, RTVORMQ_BOTH, RTAREA_JABU_JABUS_BELLY, &noviceBase, false,
              "Jabu Underwater Alcove as Adult with Jump Dive",
              "Standing above the underwater tunnel leading to the scrub, jump down and swim through the tunnel. This "
              "allows adult to access the alcove with no Scale or Iron Boots. In vanilla Jabu, this alcove has a "
              "business scrub. In MQ Jabu, it has the compass chest and a door switch for the main floor."),
    RT_OBJECT(RT_JABU_BOSS_HOVER, RTVORMQ_VANILLA, RTAREA_JABU_JABUS_BELLY, &intermediateBase, false,
              "Jabu Near Boss Room with Hover Boots",
              "A box for the blue switch can be carried over by backwalking with one while the elevator is at its "
              "peak. Alternatively, you can skip transporting a box by quickly rolling from the switch and opening the "
              "door before it closes. However, the timing for this is very tight."),
    RT_OBJECT(
        RT_JABU_NEAR_BOSS_RANGED, RTVORMQ_BOTH, RTAREA_JABU_JABUS_BELLY, &noviceBase, false,
        "Jabu Near Boss Ceiling Switch/GS without Boomerang or Explosives",
        "Vanilla Jabu: From near the entrance into the room, you can hit the switch that opens the door to the boss "
        "room using a precisely-aimed use of the Slingshot, Bow, or Longshot. As well, if you climb to the top of the "
        "vines you can stand on the right edge of the platform and shoot around the glass. From this distance, even "
        "the Hookshot can reach the switch. This trick is only relevant if \"Shuffle Boss Entrances\" is enabled. MQ "
        "Jabu: A Gold Skulltula Token can be collected with the Hookshot or Longshot using the same methods as hitting "
        "the switch in vanilla. This MQ trick is not currently relevant in logic."),
    RT_OBJECT(RT_JABU_NEAR_BOSS_EXPLOSIVES, RTVORMQ_VANILLA, RTAREA_JABU_JABUS_BELLY, &intermediateBase, false,
              "Jabu Near Boss Ceiling Switch with Explosives",
              "You can hit the switch that opens the door to the boss room using a precisely-aimed Bombchu. Also, "
              "using the Hover Boots, adult can throw a Bomb at the switch. This trick is only relevant if \"Shuffle "
              "Boss Entrances\" is enabled."),
    RT_OBJECT(RT_LENS_JABU_MQ, RTVORMQ_MQ, RTAREA_JABU_JABUS_BELLY, &noviceBase, false, "Jabu MQ without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Jabu MQ."),
    // RT_OBJECT(RT_JABU_MQ_RANG_JUMP,                       RTVORMQ_MQ,      RTAREA_JABU_JABUS_BELLY, &advancedBase,
    // false,                                     "Jabu MQ Compass Chest with Boomerang", "Boomerang can reach the cow
    // switch to spawn the chest by targeting the cow, jumping off of the ledge where the chest spawns, and throwing the
    // Boomerang in midair. This is only relevant with Jabu Jabu's Belly dungeon shortcuts enabled."),
    RT_OBJECT(RT_JABU_MQ_SOT_GS, RTVORMQ_MQ, RTAREA_JABU_JABUS_BELLY, &intermediateBase, false,
              "Jabu MQ Song of Time Block GS with Boomerang",
              "Allow the Boomerang to return to you through the Song of Time block to grab the token."),
    RT_OBJECT(RT_LENS_BOTW, RTVORMQ_VANILLA, RTAREA_BOTTOM_OF_THE_WELL, &noviceBase, false,
              "Bottom of the Well without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Bottom of the Well."),
    RT_OBJECT(RT_BOTW_CHILD_DEADHAND, RTVORMQ_BOTH, RTAREA_BOTTOM_OF_THE_WELL, &noviceBase, false,
              "Child Dead Hand without Kokiri Sword", "Requires 9 sticks or 5 jump slashes."),
    RT_OBJECT(RT_BOTW_BASEMENT, RTVORMQ_VANILLA, RTAREA_BOTTOM_OF_THE_WELL, &noviceBase, false,
              "Bottom of the Well Map Chest with Strength & Sticks",
              "The chest in the basement can be reached with strength by doing a jump slash with a lit stick to access "
              "the bomb flowers."),
    RT_OBJECT(RT_BOTW_MQ_PITS, RTVORMQ_MQ, RTAREA_BOTTOM_OF_THE_WELL, &noviceBase, false,
              "Bottom of the Well MQ Jump Over the Pits",
              "While the pits in Bottom of the Well don't allow you to jump just by running straight at them, you can "
              "still get over them by side-hopping or backflipping across. With explosives, this allows you to access "
              "the central areas without Zelda's Lullaby. With Zelda's Lullaby, it allows you to access the west inner "
              "room without explosives."),
    RT_OBJECT(RT_BOTW_MQ_DEADHAND_KEY, RTVORMQ_MQ, RTAREA_BOTTOM_OF_THE_WELL, &noviceBase, false,
              "Bottom of the Well MQ Dead Hand Freestanding Key with Boomerang",
              "Boomerang can fish the item out of the rubble without needing explosives to blow it up."),
    RT_OBJECT(RT_FOREST_FIRST_GS, RTVORMQ_VANILLA, RTAREA_FOREST_TEMPLE, &noviceBase, false,
              "Forest Temple First Room GS with Difficult-to-Use Weapons",
              "Allows killing this Skulltula with Sword or Sticks by jump slashing it as you let go from the vines. "
              "You can avoid taking fall damage by recoiling onto the tree. Also allows killing it as Child with a "
              "Bomb throw. It's much more difficult to use a Bomb as child due to Child Link's shorter height."),
    RT_OBJECT(RT_FOREST_OUTDOORS_EAST_GS, RTVORMQ_VANILLA, RTAREA_FOREST_TEMPLE, &noviceBase, false,
              "Forest Temple East Courtyard GS with Boomerang",
              "Precise Boomerang throws can allow child to kill the Skulltula and collect the token."),
    RT_OBJECT(RT_FOREST_VINES, RTVORMQ_BOTH, RTAREA_FOREST_TEMPLE, &noviceBase, false,
              "Forest Temple East Courtyard Vines with Hookshot",
              "The vines in Forest Temple leading to where the well drain switch is in the standard form can be barely "
              "reached with just the Hookshot. Applies to MQ also."),
    RT_OBJECT(RT_FOREST_OUTDOORS_LEDGE, RTVORMQ_BOTH, RTAREA_FOREST_TEMPLE, &noviceBase, false,
              "Forest Temple NE Outdoors Ledge with Hover Boots",
              "With precise Hover Boots movement you can fall down to this ledge from upper balconies. If done "
              "precisely enough, it is not necessary to take fall damage. In MQ, this skips a Longshot requirement. In "
              "Vanilla, this can skip a Hookshot requirement in entrance randomizer."),
    RT_OBJECT(RT_FOREST_DOORFRAME, RTVORMQ_BOTH, RTAREA_FOREST_TEMPLE, &advancedBase, false,
              "Forest Temple East Courtyard Door Frame with Hover Boots",
              "A precise Hover Boots movement from the upper balconies in this courtyard can be used to get on top of "
              "the door frame. Applies to both Vanilla and Master Quest. In Vanilla, from on top the door frame you "
              "can summon Pierre, allowing you to access the falling ceiling room early. In Master Quest, this allows "
              "you to obtain the GS on the door frame as adult without Hookshot or Song of Time."),
    RT_OBJECT(RT_FOREST_OUTSIDE_BACKDOOR, RTVORMQ_BOTH, RTAREA_FOREST_TEMPLE, &advancedBase, false,
              "Forest Temple Outside Backdoor with Jump Slash",
              "A jump slash recoil can be used to reach the ledge in the block puzzle room that leads to the west "
              "courtyard. This skips a potential Hover Boots requirement in vanilla, and it can sometimes apply in MQ "
              "as well. This trick can be performed as both ages."),
    RT_OBJECT(RT_FOREST_MQ_WELL_SWIM, RTVORMQ_MQ, RTAREA_FOREST_TEMPLE, &advancedBase, false,
              "Swim Through Forest Temple MQ Well with Hookshot",
              "Shoot the vines in the well as low and as far to the right as possible, and then immediately swim under "
              "the ceiling to the right. This can only be required if Forest Temple is in its Master Quest form."),
    RT_OBJECT(RT_FOREST_MQ_BLOCK_PUZZLE, RTVORMQ_MQ, RTAREA_FOREST_TEMPLE, &noviceBase, false,
              "Skip Forest Temple MQ Block Puzzle with Bombchu",
              "Send the Bombchu straight up the center of the wall directly to the left upon entering the room."),
    RT_OBJECT(RT_FOREST_MQ_JS_HALLWAY_SWITCH, RTVORMQ_MQ, RTAREA_FOREST_TEMPLE, &noviceBase, false,
              "Forest Temple MQ Twisted Hallway Switch with Jump Slash",
              "The switch to twist the hallway can be hit with a jump slash through the glass block. To get in front "
              "of the switch, either use the Hover Boots or hit the shortcut switch at the top of the room and jump "
              "from the glass blocks that spawn. Sticks can be used as child, but the Kokiri Sword is too short to "
              "reach through the glass."),
    // RT_OBJECT(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH,       RTVORMQ_MQ,      RTAREA_FOREST_TEMPLE, &intermediateBase,
    // false,                                     "Forest Temple MQ Twisted Hallway Switch with Hookshot", "There's a
    // very small gap between the glass block and the wall. Through that gap you can hookshot the target on the
    // ceiling."),
    RT_OBJECT(
        RT_FOREST_MQ_RANG_HALLWAY_SWITCH, RTVORMQ_MQ, RTAREA_FOREST_TEMPLE, &intermediateBase, false,
        "Forest Temple MQ Twisted Hallway Switch with Boomerang",
        "The Boomerang can return to Link through walls, allowing child to hit the hallway switch. This can be used to "
        "allow adult to pass through later, or in conjuction with \"Forest Temple Outside Backdoor with Jump Slash\"."),
    RT_OBJECT(RT_FIRE_BOSS_DOOR_JUMP, RTVORMQ_BOTH, RTAREA_FIRE_TEMPLE, &noviceBase, false,
              "Fire Temple Boss Door without Hover Boots or Pillar",
              "The Fire Temple Boss Door can be reached as adult with a precise jump. You must be touching the side "
              "wall of the room so that Link will grab the ledge from farther away than is normally possible."),
    RT_OBJECT(RT_FIRE_SOT, RTVORMQ_VANILLA, RTAREA_FIRE_TEMPLE, &intermediateBase, false,
              "Fire Temple Song of Time Room GS without Song of Time",
              "A precise jump can be used to reach this room."),
    RT_OBJECT(RT_FIRE_STRENGTH, RTVORMQ_VANILLA, RTAREA_FIRE_TEMPLE, &intermediateBase, false,
              "Fire Temple Climb without Strength", "A precise jump can be used to skip pushing the block."),
    RT_OBJECT(RT_FIRE_SCARECROW, RTVORMQ_VANILLA, RTAREA_FIRE_TEMPLE, &expertBase, false,
              "Fire Temple East Tower without Scarecrow\'s Song",
              "Also known as \"Pixelshot\". The Longshot can reach the target on the elevator itself, allowing you to "
              "skip needing to spawn the scarecrow."),
    RT_OBJECT(RT_FIRE_FLAME_MAZE, RTVORMQ_VANILLA, RTAREA_FIRE_TEMPLE, &intermediateBase, false,
              "Fire Temple Flame Wall Maze Skip",
              "If you move quickly you can sneak past the edge of a flame wall before it can rise up to block you. To "
              "do it without taking damage is more precise. Allows you to progress without needing either a Small Key "
              "or Hover Boots."),
    RT_OBJECT(RT_FIRE_MQ_NEAR_BOSS, RTVORMQ_MQ, RTAREA_FIRE_TEMPLE, &noviceBase, false,
              "Fire Temple MQ Chest Near Boss without Breaking Crate",
              "The hitbox for the torch extends a bit outside of the crate. Shoot a flaming arrow at the side of the "
              "crate to light the torch without needing to get over there and break the crate."),
    RT_OBJECT(RT_FIRE_MQ_BLOCKED_CHEST, RTVORMQ_MQ, RTAREA_FIRE_TEMPLE, &intermediateBase, false,
              "Fire Temple MQ Big Lava Room Blocked Door without Hookshot",
              "There is a gap between the hitboxes of the flame wall in the big lava room. If you know where this gap "
              "is located, you can jump through it and skip needing to use the Hookshot. To do this without taking "
              "damage is more precise."),
    RT_OBJECT(RT_FIRE_MQ_BK_CHEST, RTVORMQ_MQ, RTAREA_FIRE_TEMPLE, &intermediateBase, false,
              "Fire Temple MQ Boss Key Chest without Bow",
              "It is possible to light both of the timed torches to unbar the door to the boss key chest's room with "
              "just Din's Fire if you move very quickly between the two torches. It is also possible to unbar the door "
              "with just Din's by abusing an oversight in the way the game counts how many torches have been lit."),
    RT_OBJECT(RT_FIRE_MQ_CLIMB, RTVORMQ_MQ, RTAREA_FIRE_TEMPLE, &noviceBase, false,
              "Fire Temple MQ Climb without Fire Source",
              "You can use the Hover Boots to hover around to the climbable wall, skipping the need to use a fire "
              "source and spawn a Hookshot target."),
    RT_OBJECT(RT_FIRE_MQ_MAZE_SIDE_ROOM, RTVORMQ_MQ, RTAREA_FIRE_TEMPLE, &noviceBase, false,
              "Fire Temple MQ Lizalfos Maze Side Room without Box",
              "You can walk from the blue switch to the door and quickly open the door before the bars reclose. This "
              "skips needing to reach the upper sections of the maze to get a box to place on the switch."),
    RT_OBJECT(RT_FIRE_MQ_MAZE_HOVERS, RTVORMQ_MQ, RTAREA_FIRE_TEMPLE, &noviceBase, false,
              "Fire Temple MQ Lower to Upper Lizalfos Maze with Hover Boots",
              "Use the Hover Boots off of a crate to climb to the upper maze without needing to spawn and use the "
              "Hookshot targets."),
    RT_OBJECT(RT_FIRE_MQ_MAZE_JUMP, RTVORMQ_MQ, RTAREA_FIRE_TEMPLE, &intermediateBase, false,
              "Fire Temple MQ Lower to Upper Lizalfos Maze with Precise Jump",
              "A precise jump off of a crate can be used to climb to the upper maze without needing to spawn and use "
              "the Hookshot targets. This trick supersedes both \"Fire Temple MQ Lower to Upper Lizalfos Maze with "
              "Hover Boots\" and \"Fire Temple MQ Lizalfos Maze Side Room without Box\"."),
    RT_OBJECT(RT_FIRE_MQ_ABOVE_MAZE_GS, RTVORMQ_MQ, RTAREA_FIRE_TEMPLE, &intermediateBase, /*todo*/ false,
              "Fire Temple MQ Above Flame Wall Maze GS from Below with Longshot",
              "The floor of the room that contains this Skulltula is only solid from above. From the maze below, the "
              "Longshot can be shot through the ceiling to obtain the token with two fewer small keys than normal."),
    RT_OBJECT(
        RT_FIRE_MQ_FLAME_MAZE, RTVORMQ_MQ, RTAREA_FIRE_TEMPLE, &intermediateBase, false,
        "Fire Temple MQ Flame Wall Maze Skip",
        "If you move quickly you can sneak past the edge of a flame wall before it can rise up to block you. To do it "
        "without taking damage is more precise. Allows you to reach the side room GS without needing Song of Time or "
        "Hover Boots. If either of \"Fire Temple MQ Lower to Upper Lizalfos Maze with Hover Boots\" or \"with Precise "
        "Jump\" are enabled, this also allows you to progress deeper into the dungeon without Hookshot."),
    RT_OBJECT(RT_WATER_LONGSHOT_TORCH, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &noviceBase, false,
              "Water Temple Torch Longshot",
              "Stand on the eastern side of the central pillar and longshot the torches on the bottom level. Swim "
              "through the corridor and float up to the top level. This allows access to this area and lower water "
              "levels without Iron Boots. The majority of the tricks that allow you to skip Iron Boots in the Water "
              "Temple are not going to be relevant unless this trick is first enabled."),
    RT_OBJECT(RT_WATER_CRACKED_WALL_HOVERS, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &noviceBase, false,
              "Water Temple Cracked Wall with Hover Boots",
              "With a midair side-hop while wearing the Hover Boots, you can reach the cracked wall without needing to "
              "raise the water up to the middle level."),
    RT_OBJECT(RT_WATER_CRACKED_WALL, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &intermediateBase, false,
              "Water Temple Cracked Wall with No Additional Items",
              "A precise jump slash (among other methods) will get you to the cracked wall without needing the Hover "
              "Boots or to raise the water to the middle level. This trick supersedes \"Water Temple Cracked Wall with "
              "Hover Boots\"."),
    RT_OBJECT(RT_WATER_BK_REGION, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &intermediateBase, false,
              "Water Temple Boss Key Region with Hover Boots",
              "With precise Hover Boots movement it is possible to reach the boss key chest's region without needing "
              "the Longshot. It is not necessary to take damage from the spikes. The Gold Skulltula Token in the "
              "following room can also be obtained with just the Hover Boots."),
    RT_OBJECT(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP, RTVORMQ_BOTH, RTAREA_WATER_TEMPLE, &intermediateBase, false,
              "Water Temple North Basement Ledge with Precise Jump",
              "In the northern basement there's a ledge from where, in vanilla Water Temple, boulders roll out into "
              "the room. Normally to jump directly to this ledge logically requires the Hover Boots, but with precise "
              "jump, it can be done without them. This trick applies to both Vanilla and Master Quest."),
    RT_OBJECT(
        RT_WATER_BK_JUMP_DIVE, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &noviceBase, false,
        "Water Temple Boss Key Jump Dive",
        "Stand on the very edge of the raised corridor leading from the push block room to the rolling boulder "
        "corridor. Face the gold skulltula on the waterfall and jump over the boulder corridor floor into the pool of "
        "water, swimming right once underwater. This allows access to the boss key room without Iron boots."),
    RT_OBJECT(RT_WATER_FW_CENTRAL_GS, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &noviceBase, false,
              "Water Temple Central Pillar GS with Farore\'s Wind",
              "If you set Farore's Wind inside the central pillar and then return to that warp point after raising the "
              "water to the highest level, you can obtain this Skulltula Token with Hookshot or Boomerang."),
    RT_OBJECT(
        RT_WATER_IRONS_CENTRAL_GS, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &noviceBase, false,
        "Water Temple Central Pillar GS with Iron Boots",
        "After opening the middle water level door into the central pillar, the door will stay unbarred so long as you "
        "do not leave the room -- even if you were to raise the water up to the highest level. With the Iron Boots to "
        "go through the door after the water has been raised, you can obtain the Skulltula Token with the Hookshot."),
    RT_OBJECT(RT_WATER_CENTRAL_BOW, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &advancedBase, false,
              "Water Temple Central Bow Target without Longshot or Hover Boots",
              "A very precise Bow shot can hit the eye switch from the floor above. Then, you can jump down into the "
              "hallway and make through it before the gate closes. It can also be done as child, using the Slingshot "
              "instead of the Bow."),
    RT_OBJECT(
        RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &noviceBase, false,
        "Water Temple Falling Platform Room GS with Hookshot",
        "If you stand on the very edge of the platform, this Gold Skulltula can be obtained with only the Hookshot."),
    RT_OBJECT(
        RT_WATER_RANG_FALLING_PLATFORM_GS, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &intermediateBase, false,
        "Water Temple Falling Platform Room GS with Boomerang",
        "If you stand on the very edge of the platform, this Gold Skulltula can be obtained with only the Boomerang."),
    RT_OBJECT(RT_WATER_RIVER_GS, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &intermediateBase, false,
              "Water Temple River GS without Iron Boots",
              "Standing on the exposed ground toward the end of the river, a precise Longshot use can obtain the "
              "token. The Longshot cannot normally reach far enough to kill the Skulltula, however. You'll first have "
              "to find some other way of killing it."),
    RT_OBJECT(RT_WATER_DRAGON_JUMP_DIVE, RTVORMQ_BOTH, RTAREA_WATER_TEMPLE, &noviceBase, false,
              "Water Temple Dragon Statue Jump Dive",
              "If you come into the dragon statue room from the serpent river, you can jump down from above and get "
              "into the tunnel without needing either Iron Boots or a Scale. This trick applies to both Vanilla and "
              "Master Quest. In Vanilla, you must shoot the switch from above with the Bow, and then quickly get "
              "through the tunnel before the gate closes."),
    RT_OBJECT(RT_WATER_ADULT_DRAGON, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &noviceBase, false,
              "Water Temple Dragon Statue Switch from Above the Water as Adult",
              "Normally you need both Hookshot and Iron Boots to hit the switch and swim through the tunnel to get to "
              "the chest. But by hitting the switch from dry land, using one of Bombchus, Hookshot, or Bow, it is "
              "possible to skip one or both of those requirements. After the gate has been opened, besides just using "
              "the Iron Boots, a well-timed dive with at least the Silver Scale could be used to swim through the "
              "tunnel. If coming from the serpent river, a jump dive can also be used to get into the tunnel."),
    RT_OBJECT(RT_WATER_CHILD_DRAGON, RTVORMQ_VANILLA, RTAREA_WATER_TEMPLE, &advancedBase, false,
              "Water Temple Dragon Statue Switch from Above the Water as Child",
              "It is possible for child to hit the switch from dry land using one of Bombchus, Slingshot or Boomerang. "
              "Then, to get to the chest, child can dive through the tunnel using at least the Silver Scale. The "
              "timing and positioning of this dive needs to be perfect to actually make it under the gate, and it all "
              "needs to be done very quickly to be able to get through before the gate closes. Be sure to enable "
              "\"Water Temple Dragon Statue Switch from Above the Water as Adult\" for adult's variant of this trick."),
    RT_OBJECT(RT_WATER_MQ_CENTRAL_PILLAR, RTVORMQ_MQ, RTAREA_WATER_TEMPLE, &noviceBase, false,
              "Water Temple MQ Central Pillar with Fire Arrows",
              "Slanted torches have misleading hitboxes. Whenever you see a slanted torch jutting out of the wall, you "
              "can expect most or all of its hitbox is actually on the other side that wall. This can make slanted "
              "torches very finicky to light when using arrows. The torches in the central pillar of MQ Water Temple "
              "are a particularly egregious example. Logic normally expects Din's Fire and Song of Time."),
    RT_OBJECT(
        RT_WATER_MQ_LOCKED_GS, RTVORMQ_MQ, RTAREA_WATER_TEMPLE, &noviceBase, false,
        "Water Temple MQ North Basement GS without Small Key",
        "There is an invisible Hookshot target that can be used to get over the gate that blocks you from going to "
        "this Skulltula early, skipping a small key as well as needing Hovers or Scarecrow to reach the locked door."),
    RT_OBJECT(RT_LENS_SHADOW, RTVORMQ_VANILLA, RTAREA_SHADOW_TEMPLE, &noviceBase, false,
              "Shadow Temple Stationary Objects without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Shadow Temple for most areas in the dungeon except "
              "for crossing the moving platform in the huge pit room and for fighting Bongo Bongo."),
    RT_OBJECT(RT_LENS_SHADOW_PLATFORM, RTVORMQ_VANILLA, RTAREA_SHADOW_TEMPLE, &noviceBase, false,
              "Shadow Temple Invisible Moving Platform without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Shadow Temple to cross the invisible moving platform "
              "in the huge pit room in either direction."),
    RT_OBJECT(RT_LENS_BONGO, RTVORMQ_BOTH, RTAREA_SHADOW_TEMPLE, &noviceBase, false,
              "Shadow Temple Bongo Bongo without Lens of Truth",
              "Bongo Bongo can be defeated without the use of Lens of Truth, as the hands give a pretty good idea of "
              "where the eye is."),
    RT_OBJECT(RT_SHADOW_UMBRELLA, RTVORMQ_BOTH, RTAREA_SHADOW_TEMPLE, &expertBase, false,
              "Shadow Temple Stone Umbrella Skip",
              "A very precise Hover Boots movement from off of the lower chest can get you on top of the crushing "
              "spikes without needing to pull the block. Applies to both Vanilla and Master Quest."),
    RT_OBJECT(RT_SHADOW_UMBRELLA_GS, RTVORMQ_BOTH, RTAREA_SHADOW_TEMPLE, &expertBase, false,
              "Shadow Temple Falling Spikes GS with Hover Boots",
              "After killing the Skulltula, a very precise Hover Boots movement from off of the lower chest can get "
              "you on top of the crushing spikes without needing to pull the block. From there, another very precise "
              "Hover Boots movement can be used to obtain the token without needing the Hookshot. Applies to both "
              "Vanilla and Master Quest. For obtaining the chests in this room with just Hover Boots, be sure to "
              "enable \"Shadow Temple Stone Umbrella Skip\"."),
    RT_OBJECT(RT_SHADOW_FREESTANDING_KEY, RTVORMQ_VANILLA, RTAREA_SHADOW_TEMPLE, &noviceBase, false,
              "Shadow Temple Freestanding Key with Bombchu",
              "Release the Bombchu with good timing so that it explodes near the bottom of the pot."),
    RT_OBJECT(RT_SHADOW_STATUE, RTVORMQ_BOTH, RTAREA_SHADOW_TEMPLE, &intermediateBase, false,
              "Shadow Temple River Statue with Bombchu",
              "By sending a Bombchu around the edge of the gorge, you can knock down the statue without needing a Bow. "
              "Applies in both vanilla and MQ Shadow."),
    RT_OBJECT(RT_SHADOW_BONGO, RTVORMQ_BOTH, RTAREA_SHADOW_TEMPLE, &intermediateBase, false,
              "Shadow Temple Bongo Bongo without projectiles",
              "Using precise sword slashes, Bongo Bongo can be defeated without using projectiles. This is only "
              "relevant in conjunction with Shadow Temple dungeon shortcuts or shuffled boss entrances."),
    RT_OBJECT(RT_LENS_SHADOW_MQ, RTVORMQ_MQ, RTAREA_SHADOW_TEMPLE, &noviceBase, false,
              "Shadow Temple MQ Stationary Objects without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Shadow Temple MQ for most areas in the dungeon. See "
              "\"Shadow Temple MQ Invisible Moving Platform without Lens of Truth\", \"Shadow Temple MQ Invisible "
              "Blades Silver Rupees without Lens of Truth\", \"Shadow Temple MQ 2nd Dead Hand without Lens of Truth\", "
              "and \"Shadow Temple Bongo Bongo without Lens of Truth\" for exceptions."),
    RT_OBJECT(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES, RTVORMQ_MQ, RTAREA_SHADOW_TEMPLE, &noviceBase, false,
              "Shadow Temple MQ Invisible Blades Silver Rupees without Lens of Truth",
              "Removes the requirement for the Lens of Truth or Nayru's Love in Shadow Temple MQ for the Invisible "
              "Blades room silver rupee collection."),
    RT_OBJECT(RT_LENS_SHADOW_MQ_PLATFORM, RTVORMQ_MQ, RTAREA_SHADOW_TEMPLE, &noviceBase, false,
              "Shadow Temple MQ Invisible Moving Platform without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Shadow Temple MQ to cross the invisible moving "
              "platform in the huge pit room in either direction."),
    RT_OBJECT(RT_LENS_SHADOW_MQ_DEADHAND, RTVORMQ_MQ, RTAREA_SHADOW_TEMPLE, &noviceBase, false,
              "Shadow Temple MQ 2nd Dead Hand without Lens of Truth",
              "Dead Hand spawns in a random spot within the room. Having Lens removes the hassle of having to comb the "
              "room looking for his spawn location."),
    RT_OBJECT(RT_SHADOW_MQ_GAP, RTVORMQ_MQ, RTAREA_SHADOW_TEMPLE, &intermediateBase, false,
              "Shadow Temple MQ Truth Spinner Gap with Longshot",
              "You can Longshot a torch and jump-slash recoil onto the tongue. It works best if you Longshot the right "
              "torch from the left side of the room."),
    RT_OBJECT(
        RT_SHADOW_MQ_INVISIBLE_BLADES, RTVORMQ_MQ, RTAREA_SHADOW_TEMPLE, &intermediateBase, false,
        "Shadow Temple MQ Invisible Blades without Song of Time",
        "The Like Like can be used to boost you into the silver rupee or recovery hearts that normally require Song of "
        "Time. This cannot be performed on OHKO since the Like Like does not boost you high enough if you die."),
    RT_OBJECT(RT_SHADOW_MQ_HUGE_PIT, RTVORMQ_MQ, RTAREA_SHADOW_TEMPLE, &intermediateBase, false,
              "Shadow Temple MQ Lower Huge Pit without Fire Source",
              "Normally a frozen eye switch spawns some platforms that you can use to climb down, but there's actually "
              "a small piece of ground that you can stand on that you can just jump down to."),
    // RT_OBJECT(RT_SHADOW_MQ_WINDY_WALKWAY,                 RTVORMQ_MQ,      RTAREA_SHADOW_TEMPLE, &intermediateBase,
    // todo      false,                                     "Shadow Temple MQ Windy Walkway Reverse without Hover
    // Boots", "With shadow dungeon shortcuts enabled, it is possible to jump from the alcove in the windy hallway to
    // the middle platform. There are two methods: wait out the fan opposite the door and hold forward, or jump to the
    // right to be pushed by the fan there towards the platform ledge. Note that jumps of this distance are
    // inconsistent, but still possible."),
    RT_OBJECT(RT_LENS_SPIRIT, RTVORMQ_VANILLA, RTAREA_SPIRIT_TEMPLE, &noviceBase, false,
              "Spirit Temple without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Spirit Temple."),
    RT_OBJECT(RT_SPIRIT_CHILD_CHU, RTVORMQ_VANILLA, RTAREA_SPIRIT_TEMPLE, &noviceBase, false,
              "Spirit Temple Child Side Bridge with Bombchu", "A carefully-timed Bombchu can hit the switch."),
    RT_OBJECT(RT_SPIRIT_LOBBY_GS, RTVORMQ_VANILLA, RTAREA_SPIRIT_TEMPLE, &noviceBase, false,
              "Spirit Temple Main Room GS with Boomerang",
              "Standing on the highest part of the arm of the statue, a precise Boomerang throw can kill and obtain "
              "this Gold Skulltula. You must throw the Boomerang slightly off to the side so that it curves into the "
              "Skulltula, as aiming directly at it will clank off of the wall in front."),
    RT_OBJECT(RT_SPIRIT_LOWER_ADULT_SWITCH, RTVORMQ_VANILLA, RTAREA_SPIRIT_TEMPLE, &advancedBase, false,
              "Spirit Temple Lower Adult Switch with Bombs",
              "A bomb can be used to hit the switch on the ceiling, but it must be thrown from a particular distance "
              "away and with precise timing."),
    RT_OBJECT(
        RT_SPIRIT_LOBBY_JUMP, RTVORMQ_BOTH, RTAREA_SPIRIT_TEMPLE, &intermediateBase, false,
        "Spirit Temple Main Room Jump from Hands to Upper Ledges",
        "A precise jump to obtain the following as adult without needing one of Hover Boots, or Hookshot (in vanilla) "
        "or Song of Time (in MQ): - Spirit Temple Statue Room Northeast Chest - Spirit Temple GS Lobby - Spirit Temple "
        "MQ Central Chamber Top Left Pot (Left) - Spirit Temple MQ Central Chamber Top Left Pot (Right)"),
    // RT_OBJECT(RT_SPIRIT_PLATFORM_HOOKSHOT,                RTVORMQ_VANILLA, RTAREA_SPIRIT_TEMPLE, &intermediateBase,
    // false,                                     "Spirit Temple Main Room Hookshot to Boss Platform", "Precise hookshot
    // aiming at the platform chains can be used to reach the boss platform from the middle landings. Using a jump slash
    // immediately after reaching a chain makes aiming more lenient. Relevant only when Spirit Temple boss shortcuts are
    // on."),
    RT_OBJECT(RT_SPIRIT_MAP_CHEST, RTVORMQ_VANILLA, RTAREA_SPIRIT_TEMPLE, &noviceBase, false,
              "Spirit Temple Map Chest with Bow",
              "To get a line of sight from the upper torch to the map chest torches, you must pull an Armos statue all "
              "the way up the stairs."),
    RT_OBJECT(RT_SPIRIT_SUN_CHEST, RTVORMQ_VANILLA, RTAREA_SPIRIT_TEMPLE, &advancedBase, false,
              "Spirit Temple Sun Block Room Chest with Bow",
              "Using the blocks in the room as platforms you can get lines of sight to all three torches. The timer on "
              "the torches is quite short so you must move quickly in order to light all three."),
    RT_OBJECT(
        RT_SPIRIT_WALL, RTVORMQ_VANILLA, RTAREA_SPIRIT_TEMPLE, &intermediateBase, false,
        "Spirit Temple Shifting Wall with No Additional Items",
        "Logic normally guarantees a way of dealing with both the Beamos and the Walltula before climbing the wall."),
    RT_OBJECT(RT_LENS_SPIRIT_MQ, RTVORMQ_MQ, RTAREA_SPIRIT_TEMPLE, &noviceBase, false,
              "Spirit Temple MQ without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Spirit Temple MQ."),
    RT_OBJECT(RT_SPIRIT_MQ_SUN_BLOCK_SOT, RTVORMQ_MQ, RTAREA_SPIRIT_TEMPLE, &intermediateBase, false,
              "Spirit Temple MQ Sun Block Room as Child without Song of Time",
              "While adult can easily jump directly to the switch that unbars the door to the sun block room, child "
              "Link cannot make the jump without spawning a Song of Time block to jump from. You can skip this by "
              "throwing the crate down onto the switch from above, which does unbar the door, however the crate "
              "immediately breaks, so you must move quickly to get through the door before it closes back up."),
    RT_OBJECT(RT_SPIRIT_MQ_SUN_BLOCK_GS, RTVORMQ_MQ, RTAREA_SPIRIT_TEMPLE, &intermediateBase, false,
              "Spirit Temple MQ Sun Block Room GS with Boomerang",
              "Throw the Boomerang in such a way that it curves through the side of the glass block to hit the Gold "
              "Skulltula."),
    RT_OBJECT(RT_SPIRIT_MQ_LOWER_ADULT, RTVORMQ_MQ, RTAREA_SPIRIT_TEMPLE, &intermediateBase, false,
              "Spirit Temple MQ Lower Adult without Fire Arrows",
              "By standing in a precise position it is possible to light two of the torches with a single use of "
              "Din\'s Fire. This saves enough time to be able to light all three torches with only Din\'s."),
    RT_OBJECT(RT_SPIRIT_MQ_FROZEN_EYE, RTVORMQ_MQ, RTAREA_SPIRIT_TEMPLE, &noviceBase, false,
              "Spirit Temple MQ Frozen Eye Switch without Fire",
              "You can melt the ice by shooting an arrow through a torch. The only way to find a line of sight for "
              "this shot is to first spawn a Song of Time block, and then stand on the very edge of it."),
    RT_OBJECT(RT_ICE_BLOCK_GS, RTVORMQ_VANILLA, RTAREA_ICE_CAVERN, &intermediateBase, false,
              "Ice Cavern Block Room GS with Hover Boots",
              "The Hover Boots can be used to get in front of the Skulltula to kill it with a jump slash. Then, the "
              "Hover Boots can again be used to obtain the Token, all without Hookshot or Boomerang."),
    RT_OBJECT(RT_ICE_MQ_RED_ICE_GS, RTVORMQ_MQ, RTAREA_ICE_CAVERN, &intermediateBase, false,
              "Ice Cavern MQ Red Ice GS without Song of Time",
              "If you side-hop into the perfect position, you can briefly stand on the platform with the red ice just "
              "long enough to dump some blue fire."),
    RT_OBJECT(RT_ICE_MQ_SCARECROW, RTVORMQ_MQ, RTAREA_ICE_CAVERN, &intermediateBase, /*todo*/ false,
              "Ice Cavern MQ Scarecrow GS with No Additional Items",
              "As adult a precise jump can be used to reach this alcove."),
    RT_OBJECT(RT_LENS_GTG, RTVORMQ_VANILLA, RTAREA_GERUDO_TRAINING_GROUND, &noviceBase, false,
              "Gerudo Training Ground without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Gerudo Training Ground."),
    RT_OBJECT(RT_GTG_WITHOUT_HOOKSHOT, RTVORMQ_VANILLA, RTAREA_GERUDO_TRAINING_GROUND, &intermediateBase, false,
              "Gerudo Training Ground Left Side Silver Rupees without Hookshot",
              "After collecting the rest of the silver rupees in the room, you can reach the final silver rupee on the "
              "ceiling by being pulled up into it after getting grabbed by the Wallmaster. Then, you must also reach "
              "the exit of the room without the use of the Hookshot. If you move quickly you can sneak past the edge "
              "of a flame wall before it can rise up to block you. To do so without taking damage is more precise."),
    RT_OBJECT(RT_GTG_FAKE_WALL, RTVORMQ_BOTH, RTAREA_GERUDO_TRAINING_GROUND, &noviceBase, false,
              "Reach Gerudo Training Ground Fake Wall Ledge with Hover Boots",
              "A precise Hover Boots use from the top of the chest can allow you to grab the ledge without needing the "
              "usual requirements. In Master Quest, this always skips a Song of Time requirement. In Vanilla, this "
              "skips a Hookshot requirement, but is only relevant if \"Gerudo Training Ground Left Side Silver Rupees "
              "without Hookshot\" is enabled."),
    RT_OBJECT(RT_LENS_GTG_MQ, RTVORMQ_MQ, RTAREA_GERUDO_TRAINING_GROUND, &noviceBase, false,
              "Gerudo Training Ground MQ without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Gerudo Training Ground MQ."),
    RT_OBJECT(RT_GTG_MQ_WITH_HOOKSHOT, RTVORMQ_MQ, RTAREA_GERUDO_TRAINING_GROUND, &noviceBase, false,
              "Gerudo Training Ground MQ Left Side Silver Rupees with Hookshot",
              "The highest silver rupee can be obtained by hookshotting the target and then immediately jump slashing "
              "toward the rupee."),
    RT_OBJECT(RT_GTG_MQ_WIHTOUT_HOOKSHOT, RTVORMQ_MQ, RTAREA_GERUDO_TRAINING_GROUND, &intermediateBase, false,
              "Gerudo Training Ground MQ Left Side Silver Rupees without Hookshot",
              "After collecting the rest of the silver rupees in the room, you can reach the final silver rupee on the "
              "ceiling by being pulled up into it after getting grabbed by the Wallmaster. The Wallmaster will not "
              "track you to directly underneath the rupee. You should take the last step to be under the rupee after "
              "the Wallmaster has begun its attempt to grab you. Also included with this trick is that fact that the "
              "switch that unbars the door to the final chest of GTG can be hit without a projectile, using a precise "
              "jump slash. This trick supersedes \"Gerudo Training Ground MQ Left Side Silver Rupees with Hookshot\"."),
    RT_OBJECT(RT_LENS_GANON, RTVORMQ_VANILLA, RTAREA_GANONS_CASTLE, &noviceBase, false,
              "Ganon\'s Castle without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Ganon's Castle."),
    RT_OBJECT(RT_GANON_SPIRIT_TRIAL_HOOKSHOT, RTVORMQ_VANILLA, RTAREA_GANONS_CASTLE, &noviceBase, false,
              "Spirit Trial without Hookshot",
              "The highest rupee can be obtained as either age by performing a precise jump and a well-timed jumpslash "
              "off of an Armos."),
    RT_OBJECT(RT_LENS_GANON_MQ, RTVORMQ_MQ, RTAREA_GANONS_CASTLE, &noviceBase, false,
              "Ganon\'s Castle MQ without Lens of Truth",
              "Removes the requirements for the Lens of Truth in Ganon's Castle MQ."),
    RT_OBJECT(RT_GANON_MQ_FIRE_TRIAL, RTVORMQ_MQ, RTAREA_GANONS_CASTLE, &advancedBase, /*todo*/ false,
              "Fire Trial MQ with Hookshot",
              "It's possible to hook the target at the end of fire trial with just Hookshot, but it requires precise "
              "aim and perfect positioning. The main difficulty comes from getting on the very corner of the obelisk "
              "without falling into the lava."),
    RT_OBJECT(RT_GANON_MQ_SHADOW_TRIAL, RTVORMQ_MQ, RTAREA_GANONS_CASTLE, &noviceBase, false,
              "Shadow Trial MQ Torch with Bow",
              "You can light the torch in this room without a fire source by shooting an arrow through the lit torch "
              "at the beginning of the room. Because the room is so dark and the unlit torch is so far away, it can be "
              "difficult to aim the shot correctly."),
    RT_OBJECT(RT_GANON_MQ_LIGHT_TRIAL, RTVORMQ_MQ, RTAREA_GANONS_CASTLE, &intermediateBase, /*todo*/ false,
              "Light Trial MQ without Hookshot",
              "If you move quickly you can sneak past the edge of a flame wall before it can rise up to block you. In "
              "this case to do it without taking damage is especially precise.")
};

std::unordered_map<RandomizerTrickArea, std::string> rtAreaNames = {
    { RTAREA_GENERAL, "General Tricks" },
    //{ RTAREA_BK_SKIPS, "Boss Key Skips"},
    { RTAREA_KOKIRI_FOREST, "Kokiri Forest" },
    { RTAREA_LOST_WOODS, "Lost Woods" },
    { RTAREA_SACRED_FOREST_MEADOW, "Sacred Forest Meadow" },
    { RTAREA_HYRULE_FIELD, "Hyrule Field" },
    { RTAREA_LAKE_HYLIA, "Lake Hylia" },
    { RTAREA_GERUDO_VALLEY, "Gerudo Valley" },
    { RTAREA_GERUDO_FORTRESS, "Gerudo Fortress" },
    { RTAREA_WASTELAND, "Desert Wasteland" },
    { RTAREA_DESERT_COLOSSUS, "Desert Colossus" },
    { RTAREA_MARKET, "Hyrule Market" },
    { RTAREA_HYRULE_CASTLE, "Hyrule Castle" },
    { RTAREA_KAKARIKO_VILLAGE, "Kakariko Village" },
    { RTAREA_GRAVEYARD, "Graveyard" },
    { RTAREA_DEATH_MOUNTAIN_TRAIL, "Death Mountain Trail" },
    { RTAREA_GORON_CITY, "Goron City" },
    { RTAREA_DEATH_MOUNTAIN_CRATER, "Death Mountain Crater" },
    { RTAREA_ZORAS_RIVER, "Zora's River" },
    { RTAREA_ZORAS_DOMAIN, "Zora's Domain" },
    { RTAREA_ZORAS_FOUNTAIN, "Zora's Fountain" },
    { RTAREA_LON_LON_RANCH, "Lon Lon Ranch" },
    { RTAREA_DEKU_TREE, "Deku Tree" },
    { RTAREA_DODONGOS_CAVERN, "Dodongo's Cavern" },
    { RTAREA_JABU_JABUS_BELLY, "Jabu Jabu's Belly" },
    { RTAREA_FOREST_TEMPLE, "Forest Temple" },
    { RTAREA_FIRE_TEMPLE, "Fire Temple" },
    { RTAREA_WATER_TEMPLE, "Water Temple" },
    { RTAREA_SPIRIT_TEMPLE, "Spirit Temple" },
    { RTAREA_SHADOW_TEMPLE, "Shadow Temple" },
    { RTAREA_BOTTOM_OF_THE_WELL, "Bottom of the Well" },
    { RTAREA_ICE_CAVERN, "Ice Cavern" },
    { RTAREA_GERUDO_TRAINING_GROUND, "Gerudo Training Grounds" },
    { RTAREA_GANONS_CASTLE, "Ganon's Castle" },
    { RTAREA_INVALID, "Invalid" },
};

std::unordered_map<RandomizerTrickTag, std::string> rtTagNames = {
    { RTTAG_NOVICE, "Novice" }, { RTTAG_INTERMEDIATE, "Intermediate" }, { RTTAG_ADVANCED, "Advanced" },
    { RTTAG_EXPERT, "Expert" }, { RTTAG_EXTREME, "Extreme" },
    /*{RTTAG_LENS, "Lens"},
     {RTTAG_BKSKIP, "Boss Key Skip"},
     {RTTAG_EXPERIMENTAL, "Experimental"}*/
};

std::unordered_map<RandomizerTrickArea, std::unordered_map<RandomizerTrick, RandomizerTrickObject>>
    rtObjectsByArea = {};

std::unordered_map<RandomizerTrick, RandomizerTrickObject> RandomizerTricks::GetAllRTObjects() {
    return rtObjects;
}

std::unordered_map<RandomizerTrickArea, std::unordered_map<RandomizerTrick, RandomizerTrickObject>>
RandomizerTricks::GetAllRTObjectsByArea() {
    if (rtObjectsByArea.size() == 0) {
        for (auto& [randomizerTrick, rtObject] : rtObjects) {
            rtObjectsByArea[rtObject.rtArea][randomizerTrick] = rtObject;
        }
    }

    return rtObjectsByArea;
}

std::string RandomizerTricks::GetRTAreaName(RandomizerTrickArea area) {
    return rtAreaNames[area];
}

std::string RandomizerTricks::GetRTTagName(RandomizerTrickTag tag) {
    return rtTagNames[tag];
}

std::string RandomizerTricks::GetRTName(RandomizerTrick trick) {
    return rtObjects[trick].rtShortName;
}

ImVec4 RandomizerTricks::GetRTAreaColor(RandomizerTrickArea area) {
    switch (area) {
        case RTAREA_GENERAL:
            return ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        case RTAREA_KOKIRI_FOREST:
            return ImVec4(0.3f, 0.6f, 0.0f, 1.0f);
        case RTAREA_LOST_WOODS:
            return ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
        case RTAREA_SACRED_FOREST_MEADOW:
            return ImVec4(0.0f, 0.6f, 0.2f, 1.0f);
        case RTAREA_HYRULE_FIELD:
            return ImVec4(0.5f, 1.0f, 0.2f, 1.0f);
        case RTAREA_LAKE_HYLIA:
            return ImVec4(0.0f, 0.2f, 1.0f, 1.0f);
        case RTAREA_GERUDO_VALLEY:
            return ImVec4(0.6f, 0.6f, 0.15f, 1.0f);
        case RTAREA_GERUDO_FORTRESS:
            return ImVec4(0.6f, 0.5f, 0.2f, 1.0f);
        case RTAREA_WASTELAND:
            return ImVec4(0.7f, 0.6f, 0.3f, 1.0f);
        case RTAREA_DESERT_COLOSSUS:
            return ImVec4(0.5f, 0.5f, 0.3f, 1.0f);
        case RTAREA_MARKET:
            return ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        case RTAREA_HYRULE_CASTLE:
            return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        case RTAREA_KAKARIKO_VILLAGE:
            return ImVec4(0.8f, 0.4f, 0.25f, 1.0f);
        case RTAREA_GRAVEYARD:
            return ImVec4(0.5f, 0.0f, 1.0f, 1.0f);
        case RTAREA_DEATH_MOUNTAIN_TRAIL:
            return ImVec4(0.8f, 0.3f, 0.1f, 1.0f);
        case RTAREA_GORON_CITY:
            return ImVec4(0.75f, 0.60f, 0.18f, 1.0f);
        case RTAREA_DEATH_MOUNTAIN_CRATER:
            return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        case RTAREA_ZORAS_RIVER:
            return ImVec4(0.0f, 0.5f, 0.7f, 1.0f);
        case RTAREA_ZORAS_DOMAIN:
            return ImVec4(0.0f, 0.5f, 0.5f, 1.0f);
        case RTAREA_ZORAS_FOUNTAIN:
            return ImVec4(0.0f, 0.6f, 0.75f, 1.0f);
        case RTAREA_LON_LON_RANCH:
            return ImVec4(0.4f, 0.25f, 0.1f, 1.0f);
        case RTAREA_DEKU_TREE:
            return ImVec4(0.2f, 0.65f, 0.3f, 1.0f);
        case RTAREA_DODONGOS_CAVERN:
            return ImVec4(0.88f, 0.07f, 0.37f, 1.0f);
        case RTAREA_JABU_JABUS_BELLY:
            return ImVec4(0.06f, 0.32f, 0.73f, 1.0f);
        case RTAREA_FOREST_TEMPLE:
            return ImVec4(0.1f, 0.55f, 0.1f, 1.0f);
        case RTAREA_FIRE_TEMPLE:
            return ImVec4(1.0f, 0.4f, 0.0f, 1.0f);
        case RTAREA_WATER_TEMPLE:
            return ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
        case RTAREA_SPIRIT_TEMPLE:
            return ImVec4(0.7f, 0.35f, 0.0f, 1.0f);
        case RTAREA_SHADOW_TEMPLE:
            return ImVec4(0.45f, 0.0f, 0.45f, 1.0f);
        case RTAREA_BOTTOM_OF_THE_WELL:
            return ImVec4(0.7f, 0.05f, 1.0f, 1.0f);
        case RTAREA_ICE_CAVERN:
            return ImVec4(0.0f, 0.65f, 0.65f, 1.0f);
        case RTAREA_GERUDO_TRAINING_GROUND:
            return ImVec4(0.65f, 0.57f, 0.0f, 1.0f);
        case RTAREA_GANONS_CASTLE:
            return ImVec4(0.2f, 0.0f, 0.4f, 1.0f);
        default:
            return ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    }
}

ImVec4 RandomizerTricks::GetRTTagColor(RandomizerTrickTag tag) {
    switch (tag) {
        case RTTAG_NOVICE:
            return ImVec4(0.09f, 0.55f, 0.37f, 1.0f);
        case RTTAG_INTERMEDIATE:
            return ImVec4(0.95f, 0.52f, 0.0f, 1.0f);
        case RTTAG_ADVANCED:
            return ImVec4(0.0f, 0.29f, 0.71f, 1.0f);
        case RTTAG_EXPERT:
            return ImVec4(0.53f, 0.05f, 0.14f, 1.0f);
        case RTTAG_EXTREME:
            return ImVec4(0.27f, 0.0f, 0.27f, 1.0f);
        default:
            return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
            /*case RTTAG_LENS:
                return ImVec4(.f,.f,.f,1.0f);
            case RTTAG_BKSKIP:
                return ImVec4(.f,.f,.f,1.0f);
            case RTTAG_EXPERIMENTAL:
                return ImVec4(.f,.f,.f,1.0f);*/
    }
}

bool RandomizerTricks::CheckRTTags(std::unordered_map<RandomizerTrickTag, bool>& showTag,
                                   const std::vector<RandomizerTrickTag>&        rtTags) {
    for (auto rtTag : rtTags) {
        if (showTag[rtTag]) {
            return true;
        }
    }
    return false;
}