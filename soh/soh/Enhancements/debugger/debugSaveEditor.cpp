#include "debugSaveEditor.h"
#include "../../util.h"
#include "../../OTRGlobals.h"
#include "../libultraship/ImGuiImpl.h"
#include "ImGuiHelpers.h"

#include <spdlog/fmt/fmt.h>
#include <array>
#include <bit>
#include <map>
#include <string>
#include <Cvar.h>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"
extern GlobalContext* gGlobalCtx;

#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
}

typedef enum {
    EVENT_CHECK_INF,
    ITEM_GET_INF,
    INF_TABLE,
    EVENT_INF,
    RANDOMIZER_INF,
} FlagTableType;

typedef struct {
    const char* name;
    FlagTableType flagTableType;
    uint32_t size;
    std::map<uint16_t, const char*> flagDescriptions;
} FlagTable;

// Reference https://tcrf.net/Proto:The_Legend_of_Zelda:_Ocarina_of_Time_Master_Quest/Event_Editor
std::vector<FlagTable> flagTables = {
    { "Event Check Inf Flags", EVENT_CHECK_INF, 0x0D, {
        { 0x02, "First Spoke to Mido" },
        { 0x03, "Complained About Mido to Saria" },
        { 0x04, "Showed Mido Sword & Shield" },
        { 0x05, "Deku Tree Opened Mouth" },
        { 0x06, "Spoke to Saria After Deku Tree's Death" },
        { 0x07, "Obtained Kokiri Emerald & Deku Tree Dead" },
        { 0x09, "Used Deku Tree Blue Warp" },
        { 0x0A, "Played Saria's Song for Mido as Adult" },
        { 0x0C, "Met Deku Tree" },
        { 0x10, "Spoke to Child Malon at Castle or Market" },
        { 0x11, "Spoke to Ingo at Ranch before Talon returns" },
        { 0x12, "Obtained Pocket Egg" },
        { 0x13, "Woke Talon" },
        { 0x14, "Talon Fled Hyrule Castle" },
        { 0x15, "Spoke to Child Malon at Ranch" },
        { 0x16, "Invited to Sing With Child Malon" },
        { 0x17, "Great Deku Tree is Dead" },
        { 0x18, "Obtained Epona" },
        { 0x19, "Obtained Kokiri's Emerald" },
        { 0x1B, "Rented Horse From Ingo" },
        { 0x1C, "Spoke to Mido After Deku Tree's Death" },
        { 0x1D, "Destroyed the Royal Family's Tomb" },
        { 0x1E, "Won the Cow in Malon's Race" },
        { 0x23, "Bombed Dodongo's Cavern Entrance" },
        { 0x25, "Used Dodongo's Cavern Blue Warp" },
        { 0x2F, "Death Mountain Erupted" },
        { 0x30, "Spoke to a Zora" },
        { 0x31, "Obtained Ruto's Letter" },
        { 0x33, "King Zora Moved Aside" },
        { 0x37, "Used Jabu-Jabu Blue Warp" },
        { 0x38, "Obtained Silver Scale" },
        { 0x39, "Opened Entrance to Zora's Domain" },
        { 0x3A, "Offered Fish to Jabu-Jabu" },
        { 0x3B, "Began Nabooru Battle" },
        { 0x3C, "Finished Nabooru Battle" },
        { 0x40, "Obtained Zelda's Letter" },
        { 0x43, "Obtained Ocarina of Time" },
        { 0x45, "Pulled Master Sword from Pedestal" },
        { 0x48, "Used Forest Temple Blue Warp" },
        { 0x49, "Used Fire Temple Blue Warp" },
        { 0x4A, "Used Water Temple Blue Warp" },
        { 0x4B, "Opened the Door of Time" },
        { 0x4D, "Rainbow Bridge Built by Sages" },
        { 0x4E, "Caught by Hyrule Castle Guards" },
        { 0x4F, "Entered the Master Sword Chamber" },
        { 0x50, "Learned Minuet of Forest" },
        { 0x51, "Learned Bolero of Fire" },
        { 0x52, "Learned Serenade of Water" },
        { 0x54, "Learned Nocturne of Shadow" },
        { 0x55, "Sheik Moved From Sword Pedestal" },
        { 0x57, "Learned Saria's Song" },
        { 0x59, "Learned Zelda's Lullaby" },
        { 0x5A, "Learned Sun's Song" },
        { 0x5B, "Learned Song of Storms" },
        { 0x65, "Played Song of Storms in Windmill" },
        { 0x67, "Drained Well in Kakariko Village" },
        { 0x68, "Played Gerudo Archery Minigame" },
        { 0x69, "Restored Lake Hylia's Water" },
        { 0x6A, "Woke Talon in Kakariko" },
        { 0x6B, "Spoke to Talon After Saving Ranch" },
        { 0x6F, "Spoke to Kaepora Gaebora by Lost Woods" },
        { 0x70, "Began Gohma Battle" },
        { 0x71, "Began King Dodongo Battle" },
        { 0x72, "Began Phantom Ganon Battle" },
        { 0x73, "Began Volvagia Battle" },
        { 0x74, "Began Morpha Battle" },
        { 0x75, "Began Twinrova Battle" },
        { 0x76, "Began Barinade Battle" },
        { 0x77, "Began Bongo Bongo Battle" },
        { 0x78, "Began Ganondorf Battle" },
        { 0x80, "Zelda Fled Hyrule Castle" },
        { 0x82, "Bridge Unlocked (After Zelda Escape Cutscene)" },
        { 0x8C, "Paid Back Keaton Mask Fee" },
        { 0x8D, "Paid Back Skull Mask Fee" },
        { 0x8E, "Paid Back Spooky Mask Fee" },
        { 0x8F, "Paid Back Bunny Hood Fee" },
        { 0x90, "Rescued Red Carpenter" },
        { 0x91, "Rescued Yellow Carpenter" },
        { 0x92, "Rescued Blue Carpenter" },
        { 0x93, "Rescued Green Carpenter" },
        { 0x94, "Spoke to Nabooru in Spirit Temple" },
        { 0x95, "Nabooru Captured by Twinrova" },
        { 0x96, "Spoke to Cursed Man in Skulltula House" },
        { 0x9C, "Played Song for Scarecrow as Adult" },
        { 0xA0, "Entered Hyrule Field" },
        { 0xA1, "Entered Death Mountain Trail" },
        { 0xA3, "Entered Kakariko Village" },
        { 0xA4, "Entered Zora's Domain" },
        { 0xA5, "Entered Hyrule Castle" },
        { 0xA6, "Entered Goron City" },
        { 0xA7, "Entered Temple of Time" },
        { 0xA8, "Entered Deku Tree" },
        { 0xA9, "Learned Song of Time" },
        { 0xAA, "Bongo Bongo Escaped Well" },
        { 0xAC, "Learned Requiem of Spirit" },
        { 0xAD, "Completed Spirit Trial" },
        { 0xB0, "Entered Dodongo's Cavern" },
        { 0xB1, "Entered Lake Hylia" },
        { 0xB2, "Entered Gerudo Valley" },
        { 0xB3, "Entered Gerudo's Fortress" },
        { 0xB4, "Entered Lon Lon Ranch" },
        { 0xB5, "Entered Jabu-Jabu's Belly" },
        { 0xB6, "Entered Graveyard" },
        { 0xB7, "Entered Zora's Fountain" },
        { 0xB8, "Entered Desert Colossus" },
        { 0xB9, "Entered Death Mountain Crater" },
        { 0xBA, "Entered Ganon's Castle (Exterior)" },
        { 0xBB, "Completed Forest Trial" },
        { 0xBC, "Completed Water Trial" },
        { 0xBD, "Completed Shadow Trial" },
        { 0xBE, "Completed Fire Trial" },
        { 0xBF, "Completed Light Trial" },
        { 0xC0, "Nabooru Ordered to Fight by Twinrova" },
        { 0xC1, "Spoke to Saria on Lost Woods Bridge" },
        { 0xC3, "Dispelled Ganon's Tower Barrier" },
        { 0xC4, "Returned to Temple of Time With All Medallions" },
        { 0xC5, "Sheik, Spawned at Master Sword Pedestal as Adult" },
        { 0xC6, "Spoke to Deku Tree Sprout" },
        { 0xC7, "Watched Ganon's Tower Collapse / Caught by Gerudo" },
        { 0xC8, "Obtained Spirit Medallion" },
        { 0xC9, "Demo_Effect, Temple of Time Warp in blue aura + sfx the first time you spawn as Adult" },
        { 0xD0, "Obtained Frogs' Piece of Heart" },
        { 0xD1, "Played Zelda's Lullaby for Frogs" },
        { 0xD2, "Played Epona's Song for Frogs" },
        { 0xD3, "Played Sun's Song for Frogs" },
        { 0xD4, "Played Saria's Song for Frogs" },
        { 0xD5, "Played Song of Time for Frogs" },
        { 0xD6, "Played Song of Storms for Frogs" },
        { 0xDA, "Obtained Adult's Wallet" },
        { 0xDB, "Obtained Stone of Agony" },
        { 0xDC, "Obtained Giant's Wallet" },
        { 0xDD, "Obtained Skulltula House's Bombchu" },
        { 0xDE, "Obtained Skulltula House's Piece of Heart" },
    } },
    { "Item Get Inf Flags", ITEM_GET_INF, 0x03, {
        { 0x02, "Obtained Super Cucco Bottle" },
        { 0x03, "Bought Bombchu (Shelf 2: Top Right)" },
        { 0x04, "Bought Bombchu (Shelf 1: Bot. Right)" },
        { 0x05, "Bought Bombchu (Shelf 1: Top Left)" },
        { 0x06, "Bought Bombchu (Shelf 2: Bot. Left)" },
        { 0x07, "Bought Bombchu (Shelf 2: Bot. Right)" },
        { 0x08, "Bought Bombchu (Shelf 1: Top Right)" },
        { 0x09, "Bought Bombchu (Shelf 1: Bot. Left)" },
        { 0x0A, "Bought Bombchu (Shelf 2: Top Left)" },
        { 0x0B, "Obtained Scrub's Heart Piece" },
        { 0x0C, "Obtained Cucco Lady's Bottle" },
        { 0x0D, "Obtained Bullet Bag Upgrade (Market)" },
        { 0x0E, "Obtained Big Quiver" },
        { 0x0F, "Obtained Biggest Quiver" },
        { 0x10, "Obtained Scientist's Heart Piece" },
        { 0x11, "Obtained Bowling Bomb Bag Upgrade" },
        { 0x12, "Obtained Bowling Heart Piece" },
        { 0x13, "Obtained Deku Seeds" },
        { 0x15, "Obtained Roof Guy's Heart Piece" },
        { 0x16, "Obtained Skullkid's Heart Piece" },
        { 0x17, "Obtained Skullkids' Heart Piece" },
        { 0x18, "Obtained Farore's Wind" },
        { 0x19, "Obtained Din's Fire" },
        { 0x1A, "Obtained Nayru's Love" },
        { 0x1C, "Obtained Grave-Dig Heart Piece" },
        { 0x1D, "Obtained Bullet Bag Upgrade (Woods)" },
        { 0x1E, "Obtained Deku Stick Upgrade (Stage)" },
        { 0x1F, "Obtained Deku Nut Upgrade (Stage)" },
        { 0x23, "Obtained Keaton Mask" },
        { 0x24, "Obtained Skull Mask" },
        { 0x25, "Obtained Spooky Mask" },
        { 0x26, "Obtained Bunny Hood" },
        { 0x2A, "Obtained Mask of Truth" },
        { 0x2C, "Obtained Pocket Egg" },
        { 0x2E, "Obtained Cojiro" },
        { 0x30, "Obtained Odd Potion" },
        { 0x31, "Obtained Poacher's Saw" },
        { 0x38, "Sold Keaton Mask" },
        { 0x39, "Sold Skull Mask" },
        { 0x3A, "Sold Spooky Mask" },
        { 0x3B, "Sold Bunny Hood" },
        { 0x3F, "Obtained Mask of Truth" },
    } },
    { "Inf Flags", INF_TABLE, 0x1D, {
        { 0x00, "Greeted by Saria" },
        { 0x01, "Spoke to Saria About Obtaining Fairy" },
        { 0x03, "Complained About Mido to Saria" },
        { 0x05, "Spoke to Saria in Saria's House" },
        { 0x0C, "Mido Asked to See Sword & Shield" },
        { 0x15, "Spoke to Mido in Lost Woods as Adult" },
        { 0x19, "Told Mido Saria Won't Return" },
        { 0x1E, "Spoke to Kokiri Girl by Jumping Stones" },
        { 0x22, "Spoke to Kokiri Boy Guarding Forest Exit" },
        { 0x24, "Spoke to Kokiri Boy Cutting Grass" },
        { 0x26, "Spoke to Kokiri Girl on Shop Awning" },
        { 0x28, "Spoke to Kokiri Girl About Training Center" },
        { 0x31, "Spoke to Kokiri Boy on Bed in Mido's House" },
        { 0x51, "Spoke to Kokiri Girl in Saria's House" },
        { 0x59, "Spoke to Know-It-All Bro. About Temple" },
        { 0x61, "Spoke to Know-It-All Bro. About Saria" },
        { 0x66, "Spoke to Deku Tree Sprout After Cutscene" },
        { 0x6C, "Spoke to Dying Knight" },
        { 0x76, "Showed Zelda's Letter to Gate Guard" },
        { 0x77, "Gate Guard Put On Keaton Mask" },
        { 0x7E, "Spoke to Talon in Lon Lon Ranch House" },
        { 0x84, "Spoke to Child Malon at Castle or Market" },
        { 0x85, "Child Malon Said Epona Was Afraid of You" },
        { 0x8B, "Entered Hyrule Castle" },
        { 0x94, "Spoke to Ingo at Ranch Before Talon's Return" },
        { 0x97, "Spoke to Ingo at Ranch" },
        { 0x9A, "Spoke to Ingo as Adult" },
        { 0xA2, "Refused Ingo's 50 Rupee Rental Request" },
        { 0xAB, "Rode a Horse at Ingo's Ranch" },
        { 0xB0, "Spoke to Medigoron as Child" },
        { 0xB1, "Spoke to Medigoron as Adult" },
        { 0xB6, "Spoke to Poe Collector in Ruined Market" },
        { 0xB7, "Spoke to Fado in Kokiri Forest as Child" },
        { 0xB8, "Spoke to Malon After Saving Ranch" },
        { 0xB9, "Spoke to Malon on Horseback" },
        { 0xBC, "Spoke to Carpenter Boss by Tent" },
        { 0xC0, "Spoke to Fat Woman by Market Potion Shop" },
        { 0xC1, "Spoke to Fat Woman After Zelda's Escape" },
        { 0xC2, "Spoke to Burly Man About Talon Search" },
        { 0xC3, "Spoke to Burly Man After Zelda's Escape" },
        { 0xC4, "Spoke to Thief After Zelda's Escape" },
        { 0xC5, "Spoke to Thin Man by Market Target Shop" },
        { 0xC6, "Spoke to Old Woman by Market Fountain" },
        { 0xC7, "Spoke to Old Man by Bombchu Bowling" },
        { 0xC8, "Spoke to Thin Lady by Bombchu Bowling" },
        { 0xC9, "Spoke to Thin Lady After Zelda's Escape (1.0)" },
        { 0xCA, "Spoke to Red Joker in Market" },
        { 0xCB, "Spoke to Blue Joker in Market (1.0) / Dog Lady as Adult (DBG)" },
        { 0xCC, "Spoke to Itchy Lady After Malon Left Town" },
        { 0xCD, "Spoke to Blue Jokester in Market (DBG)" },
        { 0xCE, "Spoke to Thin Lady After Zelda's Escape (DBG)" },
        { 0xD9, "Spoke to Dampé as Child" },
        { 0xE0, "Spoke to Goron by Cavern" },
        { 0xE3, "Spoke to Goron Hiding Stick" },
        { 0xE6, "Spoke to Goron by Woods Exit" },
        { 0xEB, "Spoke to Goron by Bomb Flowers" },
        { 0xF0, "Spoke to Goron at City? Entrance" },
        { 0xF4, "Spoke to Ruby-Crazed Goron" },
        { 0xFC, "Spoke to Goron Shop Owner" },
        { 0x109, "Goron City Doors Unlocked" },
        { 0x10B, "Spoke to Goron Link About Volvagia" },
        { 0x10C, "Stopped Goron Link's Rolling" },
        { 0x10E, "Spoke to Goron Link" },
        { 0x113, "Spoke to Darunia in Goron City" },
        { 0x11A, "Spoke to Darunia in Fire Temple" },
        { 0x11E, "Obtained Bomb Bag Upgrade (Goron City)" },
        { 0x124, "Spoke to Zora Near Zora Shop?" },
        { 0x128, "Spoke to Zora Beside Zora Shop?" },
        { 0x129, "Spoke to Zora Swimming Behind Zora Shop" },
        { 0x138, "Thawed King Zora" },
        { 0x139, "Obtained Zora Tunic" },
        { 0x140, "Ruto in JJ (M7) on Blue Switch" },
        { 0x141, "Ruto in JJ (M2) Meet Ruto" },
        { 0x142, "Ruto in JJ (M3) Talk First Time" },
        { 0x143, "Ruto in JJ (M10) Can Be Escorted" },
        { 0x144, "Ruto in JJ (?) Wants to be Tossed to Sapphire" },
        { 0x145, "Ruto in JJ (M6) on Sapphire platform" },
        { 0x146, "Ruto in JJ (M6) Kidnapped" },
        { 0x147, "Ruto in JJ, Spawns on F1 Instead of B1" },
        { 0x160, "Spoke to Man in Impa's House During Day" },
        { 0x161, "Spoke to Man in Impa's House at Night" },
        { 0x162, "Spoke to Man in Impa's House as Adult" },
        { 0x163, "Spoke to Carpenter Boss's Wife as Child" },
        { 0x164, "Spoke to Carpenter Boss's Wife as Adult" },
        { 0x16C, "Refused Nabooru's Request" },
        { 0x170, "Spoke to Carpenter Boss in Valley" },
        { 0x172, "Spoke to Carpenter Boss in Kakariko" },
        { 0x176, "Spoke to Blue Carpenter in Tent" },
        { 0x178, "Spoke to Green Carpenter in Tent" },
        { 0x17F, "Running Man Suggested a Race" },
        { 0x190, "Obtained Gerudo Archery Piece of  Heart" },
        { 0x191, "Obtained Dog Lady's Piece of Heart" },
        { 0x192, "Obtained Deku Stick Upgrade (Lost Woods)" },
        { 0x193, "Obtained Deku Nut Upgrade (Grotto)" },
        { 0x195, "Spoke to Kaepora in Lake Hylia" },
        { 0x196, "Spoke to Cursed Man With <10 Tokens" },
        { 0x197, "Spoke to Cursed Man With 10 Tokens" },
        { 0x198, "Obtained Small Magic Jar" },
        { 0x199, "Caught Cucco by Field Entrance" },
        { 0x19A, "Caught Cucco by Bazaar" },
        { 0x19B, "Caught Cucco by Cucco Pen" },
        { 0x19C, "Caught Cucco Behind Windmill" },
        { 0x19D, "Caught Cucco in Crate" },
        { 0x19E, "Caught Cucco by Skulltula House" },
        { 0x19F, "Caught Cucco Behind Potion Shop" },
        { 0x1A0, "Entered Deku Tree" },
        { 0x1A1, "Entered Dodongo's Cavern" },
        { 0x1A2, "Entered Jabu-Jabu's Belly" },
        { 0x1A3, "Entered Forest Temple" },
        { 0x1A4, "Entered Fire Temple" },
        { 0x1A5, "Entered Water Temple" },
        { 0x1A6, "Entered Spirit Temple" },
        { 0x1A7, "Entered Shadow Temple" },
        { 0x1A8, "Entered Bottom of the Well" },
        { 0x1A9, "Entered Ice Cavern" },
        { 0x1AA, "Entered Ganon's Tower" },
        { 0x1AB, "Entered Gerudo Training Ground" },
        { 0x1AC, "Entered Thieves' Hideout" },
        { 0x1AD, "Entered Ganon's Castle" },
        { 0x1AE, "Entered Ganon's Tower (Collapsing)" },
        { 0x1AF, "Entered Ganon's Castle (Collapsing)" },
        { 0x1D0, "No Sword on B" },
    } },
    { "Event Inf Flags", EVENT_INF, 0x03, {
        { 0x00, "Rented Horse From Ingo" },
        { 0x01, "Racing Ingo" },
        { 0x02, "Won First Race With Ingo?" },
        { 0x05, "Lost Race With Ingo?" },
        { 0x06, "Racing Ingo For the Second Time" },
        { 0x08, "Finished Special Cucco Minigame / Began Gerudo Archery" },
        { 0x0A, "Won Special Cucco Minigame?" },
        { 0x0F, "Rented Horse From Ingo" },
        { 0x10, "Racing Running Man" },
        { 0x20, "Market Crowd Text Randomizer" },
        { 0x21, "Market Crowd Text Randomizer" },
        { 0x22, "Market Crowd Text Randomizer" },
        { 0x23, "Market Crowd Text Randomizer" },
        { 0x24, "Market Crowd Text Randomizer" },
        { 0x30, "Entered the Market" },
    } },
   { "Randomizer Inf Flags", RANDOMIZER_INF, 0x03, {
        { RAND_INF_DUNGEONS_DONE_DEKU_TREE, "DUNGEONS_DONE_DEKU_TREE" },
        { RAND_INF_DUNGEONS_DONE_DODONGOS_CAVERN, "DUNGEONS_DONE_DODONGOS_CAVERN" },
        { RAND_INF_DUNGEONS_DONE_JABU_JABUS_BELLY, "DUNGEONS_DONE_JABU_JABUS_BELLY" },
        { RAND_INF_DUNGEONS_DONE_FOREST_TEMPLE, "DUNGEONS_DONE_FOREST_TEMPLE" },
        { RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE, "DUNGEONS_DONE_FIRE_TEMPLE" },
        { RAND_INF_DUNGEONS_DONE_WATER_TEMPLE, "DUNGEONS_DONE_WATER_TEMPLE" },
        { RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE, "DUNGEONS_DONE_SPIRIT_TEMPLE" },
        { RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE, "DUNGEONS_DONE_SHADOW_TEMPLE" },

        { RAND_INF_TRIALS_DONE_LIGHT_TRIAL, "TRIALS_DONE_LIGHT_TRIAL" },
        { RAND_INF_TRIALS_DONE_FOREST_TRIAL, "TRIALS_DONE_FOREST_TRIAL" },
        { RAND_INF_TRIALS_DONE_FIRE_TRIAL, "TRIALS_DONE_FIRE_TRIAL" },
        { RAND_INF_TRIALS_DONE_WATER_TRIAL, "TRIALS_DONE_WATER_TRIAL" },
        { RAND_INF_TRIALS_DONE_SPIRIT_TRIAL, "TRIALS_DONE_SPIRIT_TRIAL" },
        { RAND_INF_TRIALS_DONE_SHADOW_TRIAL, "TRIALS_DONE_SHADOW_TRIAL" },

        { RAND_INF_COWS_MILKED_LINKS_HOUSE_COW, "COWS_MILKED_LINKS_HOUSE_COW" },
        { RAND_INF_COWS_MILKED_HF_COW_GROTTO_COW, "COWS_MILKED_HF_COW_GROTTO_COW" },
        { RAND_INF_COWS_MILKED_LLR_STABLES_LEFT_COW, "COWS_MILKED_LLR_STABLES_LEFT_COW" },
        { RAND_INF_COWS_MILKED_LLR_STABLES_RIGHT_COW, "COWS_MILKED_LLR_STABLES_RIGHT_COW" },
        { RAND_INF_COWS_MILKED_LLR_TOWER_LEFT_COW, "COWS_MILKED_LLR_TOWER_LEFT_COW" },
        { RAND_INF_COWS_MILKED_LLR_TOWER_RIGHT_COW, "COWS_MILKED_LLR_TOWER_RIGHT_COW" },
        { RAND_INF_COWS_MILKED_KAK_IMPAS_HOUSE_COW, "COWS_MILKED_KAK_IMPAS_HOUSE_COW" },
        { RAND_INF_COWS_MILKED_DMT_COW_GROTTO_COW, "COWS_MILKED_DMT_COW_GROTTO_COW" },
        { RAND_INF_COWS_MILKED_GV_COW, "COWS_MILKED_GV_COW" },
        { RAND_INF_COWS_MILKED_JABU_JABUS_BELLY_MQ_COW, "COWS_MILKED_JABU_JABUS_BELLY_MQ_COW" },
        { RAND_INF_COWS_MILKED_HF_COW_GROTTO_GOSSIP_STONE, "COWS_MILKED_HF_COW_GROTTO_GOSSIP_STONE" },

        { RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT, "SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT" },
        { RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, "SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS" },
        { RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, "SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY, "SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY" },
        { RAND_INF_SCRUBS_PURCHASED_JABU_JABUS_BELLY_DEKU_SCRUB, "SCRUBS_PURCHASED_JABU_JABUS_BELLY_DEKU_SCRUB" },
        { RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT, "SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT" },
        { RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, "SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_RIGHT, "SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_LEFT, "SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_LEFT" },
        { RAND_INF_SCRUBS_PURCHASED_HF_DEKU_SCRUB_GROTTO, "SCRUBS_PURCHASED_HF_DEKU_SCRUB_GROTTO" },
        { RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_REAR, "SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_REAR" },
        { RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_FRONT, "SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_FRONT" },
        { RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_REAR, "SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_REAR" },
        { RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_FRONT, "SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_FRONT" },
        { RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_LEFT, "SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_LEFT" },
        { RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_RIGHT, "SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_CENTER, "SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_CENTER" },
        { RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_REAR, "SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_REAR" },
        { RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_FRONT, "SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_FRONT" },
        { RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_REAR, "SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_REAR" },
        { RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_FRONT, "SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_FRONT" },
        { RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_LEFT, "SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_LEFT" },
        { RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_RIGHT, "SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_CENTER, "SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_CENTER" },
        { RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_LEFT, "SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_LEFT" },
        { RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_RIGHT, "SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_CENTER, "SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_CENTER" },
        { RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_LEFT, "SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_LEFT" },
        { RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_RIGHT, "SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_CENTER, "SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_CENTER" },
        { RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_REAR, "SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_REAR" },
        { RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, "SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT" },
        { RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, "SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT, "SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT" },
        { RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_BRIDGE, "SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_BRIDGE" },
        { RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB, "SCRUBS_PURCHASED_DMC_DEKU_SCRUB" },
    } },
};

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    std::string texturePath;
} ItemMapEntry;

#define ITEM_MAP_ENTRY(id)                              \
    {                                                   \
        id, {                                           \
            id, #id, #id "_Faded", static_cast<char*>(gItemIcons[id]) \
        }                                               \
    }

// Maps items ids to info for use in ImGui
std::map<uint32_t, ItemMapEntry> itemMapping = {
    ITEM_MAP_ENTRY(ITEM_STICK),
    ITEM_MAP_ENTRY(ITEM_NUT),
    ITEM_MAP_ENTRY(ITEM_BOMB),
    ITEM_MAP_ENTRY(ITEM_BOW),
    ITEM_MAP_ENTRY(ITEM_ARROW_FIRE),
    ITEM_MAP_ENTRY(ITEM_DINS_FIRE),
    ITEM_MAP_ENTRY(ITEM_SLINGSHOT),
    ITEM_MAP_ENTRY(ITEM_OCARINA_FAIRY),
    ITEM_MAP_ENTRY(ITEM_OCARINA_TIME),
    ITEM_MAP_ENTRY(ITEM_BOMBCHU),
    ITEM_MAP_ENTRY(ITEM_HOOKSHOT),
    ITEM_MAP_ENTRY(ITEM_LONGSHOT),
    ITEM_MAP_ENTRY(ITEM_ARROW_ICE),
    ITEM_MAP_ENTRY(ITEM_FARORES_WIND),
    ITEM_MAP_ENTRY(ITEM_BOOMERANG),
    ITEM_MAP_ENTRY(ITEM_LENS),
    ITEM_MAP_ENTRY(ITEM_BEAN),
    ITEM_MAP_ENTRY(ITEM_HAMMER),
    ITEM_MAP_ENTRY(ITEM_ARROW_LIGHT),
    ITEM_MAP_ENTRY(ITEM_NAYRUS_LOVE),
    ITEM_MAP_ENTRY(ITEM_BOTTLE),
    ITEM_MAP_ENTRY(ITEM_POTION_RED),
    ITEM_MAP_ENTRY(ITEM_POTION_GREEN),
    ITEM_MAP_ENTRY(ITEM_POTION_BLUE),
    ITEM_MAP_ENTRY(ITEM_FAIRY),
    ITEM_MAP_ENTRY(ITEM_FISH),
    ITEM_MAP_ENTRY(ITEM_MILK_BOTTLE),
    ITEM_MAP_ENTRY(ITEM_LETTER_RUTO),
    ITEM_MAP_ENTRY(ITEM_BLUE_FIRE),
    ITEM_MAP_ENTRY(ITEM_BUG),
    ITEM_MAP_ENTRY(ITEM_BIG_POE),
    ITEM_MAP_ENTRY(ITEM_MILK_HALF),
    ITEM_MAP_ENTRY(ITEM_POE),
    ITEM_MAP_ENTRY(ITEM_WEIRD_EGG),
    ITEM_MAP_ENTRY(ITEM_CHICKEN),
    ITEM_MAP_ENTRY(ITEM_LETTER_ZELDA),
    ITEM_MAP_ENTRY(ITEM_MASK_KEATON),
    ITEM_MAP_ENTRY(ITEM_MASK_SKULL),
    ITEM_MAP_ENTRY(ITEM_MASK_SPOOKY),
    ITEM_MAP_ENTRY(ITEM_MASK_BUNNY),
    ITEM_MAP_ENTRY(ITEM_MASK_GORON),
    ITEM_MAP_ENTRY(ITEM_MASK_ZORA),
    ITEM_MAP_ENTRY(ITEM_MASK_GERUDO),
    ITEM_MAP_ENTRY(ITEM_MASK_TRUTH),
    ITEM_MAP_ENTRY(ITEM_SOLD_OUT),
    ITEM_MAP_ENTRY(ITEM_POCKET_EGG),
    ITEM_MAP_ENTRY(ITEM_POCKET_CUCCO),
    ITEM_MAP_ENTRY(ITEM_COJIRO),
    ITEM_MAP_ENTRY(ITEM_ODD_MUSHROOM),
    ITEM_MAP_ENTRY(ITEM_ODD_POTION),
    ITEM_MAP_ENTRY(ITEM_SAW),
    ITEM_MAP_ENTRY(ITEM_SWORD_BROKEN),
    ITEM_MAP_ENTRY(ITEM_PRESCRIPTION),
    ITEM_MAP_ENTRY(ITEM_FROG),
    ITEM_MAP_ENTRY(ITEM_EYEDROPS),
    ITEM_MAP_ENTRY(ITEM_CLAIM_CHECK),
    ITEM_MAP_ENTRY(ITEM_BOW_ARROW_FIRE),
    ITEM_MAP_ENTRY(ITEM_BOW_ARROW_ICE),
    ITEM_MAP_ENTRY(ITEM_BOW_ARROW_LIGHT),
    ITEM_MAP_ENTRY(ITEM_SWORD_KOKIRI),
    ITEM_MAP_ENTRY(ITEM_SWORD_MASTER),
    ITEM_MAP_ENTRY(ITEM_SWORD_BGS),
    ITEM_MAP_ENTRY(ITEM_SHIELD_DEKU),
    ITEM_MAP_ENTRY(ITEM_SHIELD_HYLIAN),
    ITEM_MAP_ENTRY(ITEM_SHIELD_MIRROR),
    ITEM_MAP_ENTRY(ITEM_TUNIC_KOKIRI),
    ITEM_MAP_ENTRY(ITEM_TUNIC_GORON),
    ITEM_MAP_ENTRY(ITEM_TUNIC_ZORA),
    ITEM_MAP_ENTRY(ITEM_BOOTS_KOKIRI),
    ITEM_MAP_ENTRY(ITEM_BOOTS_IRON),
    ITEM_MAP_ENTRY(ITEM_BOOTS_HOVER),
    ITEM_MAP_ENTRY(ITEM_BULLET_BAG_30),
    ITEM_MAP_ENTRY(ITEM_BULLET_BAG_40),
    ITEM_MAP_ENTRY(ITEM_BULLET_BAG_50),
    ITEM_MAP_ENTRY(ITEM_QUIVER_30),
    ITEM_MAP_ENTRY(ITEM_QUIVER_40),
    ITEM_MAP_ENTRY(ITEM_QUIVER_50),
    ITEM_MAP_ENTRY(ITEM_BOMB_BAG_20),
    ITEM_MAP_ENTRY(ITEM_BOMB_BAG_30),
    ITEM_MAP_ENTRY(ITEM_BOMB_BAG_40),
    ITEM_MAP_ENTRY(ITEM_BRACELET),
    ITEM_MAP_ENTRY(ITEM_GAUNTLETS_SILVER),
    ITEM_MAP_ENTRY(ITEM_GAUNTLETS_GOLD),
    ITEM_MAP_ENTRY(ITEM_SCALE_SILVER),
    ITEM_MAP_ENTRY(ITEM_SCALE_GOLDEN),
    ITEM_MAP_ENTRY(ITEM_SWORD_KNIFE),
    ITEM_MAP_ENTRY(ITEM_WALLET_ADULT),
    ITEM_MAP_ENTRY(ITEM_WALLET_GIANT),
    ITEM_MAP_ENTRY(ITEM_SEEDS),
    ITEM_MAP_ENTRY(ITEM_FISHING_POLE),
    ITEM_MAP_ENTRY(ITEM_KEY_BOSS),
    ITEM_MAP_ENTRY(ITEM_COMPASS),
    ITEM_MAP_ENTRY(ITEM_DUNGEON_MAP),
    ITEM_MAP_ENTRY(ITEM_KEY_SMALL),
    ITEM_MAP_ENTRY(ITEM_HEART_CONTAINER),
    ITEM_MAP_ENTRY(ITEM_MAGIC_SMALL),
    ITEM_MAP_ENTRY(ITEM_MAGIC_LARGE)
};

// Maps entries in the GS flag array to the area name it represents
std::vector<std::string> gsMapping = {
    "Deku Tree",
    "Dodongo's Cavern",
    "Inside Jabu-Jabu's Belly",
    "Forest Temple",
    "Fire Temple",
    "Water Temple",
    "Spirit Temple",
    "Shadow Temple",
    "Bottom of the Well",
    "Ice Cavern",
    "Hyrule Field",
    "Lon Lon Ranch",
    "Kokiri Forest",
    "Lost Woods, Sacred Forest Meadow",
    "Castle Town and Ganon's Castle",
    "Death Mountain Trail, Goron City",
    "Kakariko Village",
    "Zora Fountain, River",
    "Lake Hylia",
    "Gerudo Valley",
    "Gerudo Fortress",
    "Desert Colossus, Haunted Wasteland",
};

extern "C" u8 gAreaGsFlags[];

extern "C" u8 gAmmoItems[];

// Modification of gAmmoItems that replaces ITEM_NONE with the item in inventory slot it represents
u8 gAllAmmoItems[] = {
    ITEM_STICK,     ITEM_NUT,          ITEM_BOMB,    ITEM_BOW,      ITEM_ARROW_FIRE, ITEM_DINS_FIRE,
    ITEM_SLINGSHOT, ITEM_OCARINA_TIME, ITEM_BOMBCHU, ITEM_LONGSHOT, ITEM_ARROW_ICE,  ITEM_FARORES_WIND,
    ITEM_BOOMERANG, ITEM_LENS,         ITEM_BEAN,    ITEM_HAMMER,
};

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    std::string texturePath;
} QuestMapEntry;

#define QUEST_MAP_ENTRY(id, tex)                              \
    {                                                   \
        id, {                                           \
            id, #id, #id "_Faded", tex \
        }                                               \
    }

// Maps quest items ids to info for use in ImGui
std::map<uint32_t, QuestMapEntry> questMapping = {
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FOREST, dgForestMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FIRE, dgFireMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_WATER, dgWaterMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SPIRIT, dgSpiritMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SHADOW, dgShadowMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_LIGHT, dgLightMedallionIconTex),
    QUEST_MAP_ENTRY(QUEST_KOKIRI_EMERALD, dgKokiriEmeraldIconTex),
    QUEST_MAP_ENTRY(QUEST_GORON_RUBY, dgGoronRubyIconTex),
    QUEST_MAP_ENTRY(QUEST_ZORA_SAPPHIRE, dgZoraSapphireIconTex),
    QUEST_MAP_ENTRY(QUEST_STONE_OF_AGONY, dgStoneOfAgonyIconTex),
    QUEST_MAP_ENTRY(QUEST_GERUDO_CARD, dgGerudosCardIconTex),
    QUEST_MAP_ENTRY(QUEST_SKULL_TOKEN, dgGoldSkulltulaIconTex),
};

typedef struct {
    uint32_t id;
    std::string name;
    std::string nameFaded;
    ImVec4 color;
} SongMapEntry;

#define SONG_MAP_ENTRY(id, r, g, b)       \
    {                                  \
            id, #id, #id "_Faded", ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f) \
    }

// Maps song ids to info for use in ImGui
std::array<SongMapEntry, 12> songMapping = { {
    SONG_MAP_ENTRY(QUEST_SONG_LULLABY,  224, 107, 255),
    SONG_MAP_ENTRY(QUEST_SONG_EPONA,    255, 195, 60),
    SONG_MAP_ENTRY(QUEST_SONG_SARIA,    127, 255, 137),
    SONG_MAP_ENTRY(QUEST_SONG_SUN,      255, 255, 60),
    SONG_MAP_ENTRY(QUEST_SONG_TIME,     119, 236, 255),
    SONG_MAP_ENTRY(QUEST_SONG_STORMS,   165, 165, 165),
    SONG_MAP_ENTRY(QUEST_SONG_MINUET,   150, 255, 100),
    SONG_MAP_ENTRY(QUEST_SONG_BOLERO,   255, 80,  40),
    SONG_MAP_ENTRY(QUEST_SONG_SERENADE, 100, 150, 255),
    SONG_MAP_ENTRY(QUEST_SONG_REQUIEM,  255, 160, 0),
    SONG_MAP_ENTRY(QUEST_SONG_NOCTURNE, 255, 100, 255),
    SONG_MAP_ENTRY(QUEST_SONG_PRELUDE,  255, 240, 100),
} };

#define VANILLA_SONG_MAP_ENTRY(id, r, g, b)       \
    {                                  \
            id, #id "_Vanilla", #id "_Vanilla_Faded", ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f) \
    }

// Maps song ids to info for use in ImGui
std::array<SongMapEntry, 12> vanillaSongMapping = { {
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_LULLABY,  255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_EPONA,    255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_SARIA,    255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_SUN,      255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_TIME,     255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_STORMS,   255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_MINUET,   150, 255, 100),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_BOLERO,   255, 80,  40),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_SERENADE, 100, 150, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_REQUIEM,  255, 160, 0),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_NOCTURNE, 255, 100, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_PRELUDE,  255, 240, 100),
} };

// Encapsulates what is drawn by the passed-in function within a border
template<typename T>
void DrawGroupWithBorder(T&& drawFunc) {
    // First group encapsulates the inner portion and border
    ImGui::BeginGroup();

    ImVec2 padding = ImGui::GetStyle().FramePadding;
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(ImVec2(p0.x + padding.x, p0.y + padding.y));

    // Second group encapsulates just the inner portion
    ImGui::BeginGroup();

    drawFunc();

    ImGui::Dummy(padding);
    ImGui::EndGroup();

    ImVec2 p1 = ImGui::GetItemRectMax();
    p1.x += padding.x;
    ImVec4 borderCol = ImGui::GetStyle().Colors[ImGuiCol_Border];
    ImGui::GetWindowDrawList()->AddRect(p0, p1, IM_COL32(borderCol.x * 255, borderCol.y * 255, borderCol.z * 255, borderCol.w * 255));

    ImGui::EndGroup();
}

char z2ASCII(int code) {
    int ret;
    if (code < 10) { //Digits
        ret = code + 0x30;
    } else if (code >= 10 && code < 36) { //Uppercase letters
        ret = code + 0x37;
    } else if (code >= 36 && code < 62) { //Lowercase letters
        ret = code + 0x3D;
    } else if (code == 62) { //Space
        ret = code - 0x1E;
    } else if (code == 63 || code == 64) { // _ and .
        ret = code - 0x12;
    } else {
        ret = code;
    }
    return char(ret);

}

void DrawInfoTab() {
    // TODO Needs a better method for name changing but for now this will work.
    std::string name;
    ImU16 one = 1;
    for (int i = 0; i < 8; i++) {
        char letter = z2ASCII(gSaveContext.playerName[i]);
        name += letter;
    }
    name += '\0';

    ImGui::PushItemWidth(ImGui::GetFontSize() * 6);

    ImGui::Text("Name: %s", name.c_str());
    InsertHelpHoverText("Player Name");
    std::string nameID;
    for (int i = 0; i < 8; i++) {
        nameID = z2ASCII(i);
        if (i % 4 != 0) {
            ImGui::SameLine();
        }
        ImGui::InputScalar(nameID.c_str(), ImGuiDataType_U8, &gSaveContext.playerName[i], &one, NULL);
    }

    // Use an intermediary to keep the health from updating (and potentially killing the player)
    // until it is done being edited
    int16_t healthIntermediary = gSaveContext.healthCapacity;
    ImGui::InputScalar("Max Health", ImGuiDataType_S16, &healthIntermediary);
    if (ImGui::IsItemDeactivated()) {
        gSaveContext.healthCapacity = healthIntermediary;
    }
    InsertHelpHoverText("Maximum health. 16 units per full heart");
    if (gSaveContext.health > gSaveContext.healthCapacity) {
        gSaveContext.health = gSaveContext.healthCapacity; // Clamp health to new max
    }

    const uint16_t healthMin = 0;
    const uint16_t healthMax = gSaveContext.healthCapacity;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    ImGui::SliderScalar("Health", ImGuiDataType_S16, &gSaveContext.health, &healthMin, &healthMax);
    InsertHelpHoverText("Current health. 16 units per full heart");

    bool doubleDefense = gSaveContext.doubleDefense != 0;
    if (ImGui::Checkbox("Double Defense", &doubleDefense)) {
        gSaveContext.doubleDefense = doubleDefense;
        gSaveContext.inventory.defenseHearts =
            gSaveContext.doubleDefense ? 20 : 0; // Set to get the border drawn in the UI
    }
    InsertHelpHoverText("Is double defense unlocked?");

    std::string magicName;
    if (gSaveContext.magicLevel == 2) {
        magicName = "Double";
    } else if (gSaveContext.magicLevel == 1) {
        magicName = "Single";
    } else {
        magicName = "None";
    }
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
    if (ImGui::BeginCombo("Magic Level", magicName.c_str())) {
        if (ImGui::Selectable("Double")) {
            gSaveContext.magicLevel = 2;
            gSaveContext.magicAcquired = true;
            gSaveContext.doubleMagic = true;
        }
        if (ImGui::Selectable("Single")) {
            gSaveContext.magicLevel = 1;
            gSaveContext.magicAcquired = true;
            gSaveContext.doubleMagic = false;
        }
        if (ImGui::Selectable("None")) {
            gSaveContext.magicLevel = 0;
            gSaveContext.magicAcquired = false;
            gSaveContext.doubleMagic = false;
        }

        ImGui::EndCombo();
    }
    InsertHelpHoverText("Current magic level");
    gSaveContext.unk_13F4 = gSaveContext.magicLevel * 0x30; // Set to get the bar drawn in the UI
    if (gSaveContext.magic > gSaveContext.unk_13F4) {
        gSaveContext.magic = gSaveContext.unk_13F4; // Clamp magic to new max
    }

    const uint8_t magicMin = 0;
    const uint8_t magicMax = gSaveContext.unk_13F4;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    ImGui::SliderScalar("Magic", ImGuiDataType_S8, &gSaveContext.magic, &magicMin, &magicMax);
    InsertHelpHoverText("Current magic. 48 units per magic level");

    ImGui::InputScalar("Rupees", ImGuiDataType_S16, &gSaveContext.rupees);
    InsertHelpHoverText("Current rupees");

    const uint16_t dayTimeMin = 0;
    const uint16_t dayTimeMax = 0xFFFF;
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 15);
    ImGui::SliderScalar("Time", ImGuiDataType_U16, &gSaveContext.dayTime, &dayTimeMin, &dayTimeMax);
    InsertHelpHoverText("Time of day");
    if (ImGui::Button("Dawn")) {
        gSaveContext.dayTime = 0x4000;
    }
    ImGui::SameLine();
    if (ImGui::Button("Noon")) {
        gSaveContext.dayTime = 0x8000;
    }
    ImGui::SameLine();
    if (ImGui::Button("Sunset")) {
        gSaveContext.dayTime = 0xC001;
    }
    ImGui::SameLine();
    if (ImGui::Button("Midnight")) {
        gSaveContext.dayTime = 0;
    }

    ImGui::InputScalar("Total Days", ImGuiDataType_S32, &gSaveContext.totalDays);
    InsertHelpHoverText("Total number of days elapsed since the start of the game");

    ImGui::InputScalar("Deaths", ImGuiDataType_U16, &gSaveContext.deaths);
    InsertHelpHoverText("Total number of deaths");

    bool bgsFlag = gSaveContext.bgsFlag != 0;
    if (ImGui::Checkbox("Has BGS", &bgsFlag)) {
        gSaveContext.bgsFlag = bgsFlag;
    }
    InsertHelpHoverText("Is Biggoron sword unlocked? Replaces Giant's knife");

    ImGui::InputScalar("Sword Health", ImGuiDataType_U16, &gSaveContext.swordHealth);
    InsertHelpHoverText("Giant's knife health. Default is 8. Must be >0 for Biggoron sword to work");

    ImGui::InputScalar("Bgs Day Count", ImGuiDataType_S32, &gSaveContext.bgsDayCount);
    InsertHelpHoverText("Total number of days elapsed since giving Biggoron the claim check");

    ImGui::InputScalar("Entrance Index", ImGuiDataType_S32, &gSaveContext.entranceIndex);
    InsertHelpHoverText("From which entrance did Link arrive?");

    ImGui::InputScalar("Cutscene Index", ImGuiDataType_S32, &gSaveContext.cutsceneIndex);
    InsertHelpHoverText("Which cutscene is this?");

    ImGui::InputScalar("Navi Timer", ImGuiDataType_U16, &gSaveContext.naviTimer);
    InsertHelpHoverText("Navi wants to talk at 600 units, decides not to at 3000.");

    ImGui::InputScalar("Timer 1 State", ImGuiDataType_S16, &gSaveContext.timer1State);
    InsertHelpHoverText("Heat timer, race timer, etc. Has white font");

    ImGui::InputScalar("Timer 1 Value", ImGuiDataType_S16, &gSaveContext.timer1Value, &one, NULL);
    InsertHelpHoverText("Time, in seconds");

    ImGui::InputScalar("Timer 2 State", ImGuiDataType_S16, &gSaveContext.timer2State);
    InsertHelpHoverText("Trade timer, Ganon collapse timer, etc. Has yellow font");

    ImGui::InputScalar("Timer 2 Value", ImGuiDataType_S16, &gSaveContext.timer2Value, &one, NULL);
    InsertHelpHoverText("Time, in seconds");
     
    const char* audioName;
    switch (gSaveContext.audioSetting) { 
        case 0:
            audioName = "Stereo";
            break;
        case 1:
            audioName = "Mono";
            break;
        case 2:
            audioName = "Headset";
            break;
        case 3:
            audioName = "Surround";
            break;
        default:
            audioName = "?";
    }
    if (ImGui::BeginCombo("Audio", audioName)) {
        if (ImGui::Selectable("Stereo")) {
            gSaveContext.audioSetting = 0;
        }
        if (ImGui::Selectable("Mono")) {
            gSaveContext.audioSetting = 1;
        }
        if (ImGui::Selectable("Headset")) {
            gSaveContext.audioSetting = 2;
        }
        if (ImGui::Selectable("Surround")) {
            gSaveContext.audioSetting = 3;
        }

        ImGui::EndCombo();
    }
    InsertHelpHoverText("Sound setting");
    
    bool n64DDFlag = gSaveContext.n64ddFlag != 0;
    if (ImGui::Checkbox("64 DD file?", &n64DDFlag)) {
        gSaveContext.n64ddFlag = n64DDFlag;
    }
    InsertHelpHoverText("WARNING! If you save, your file may be locked! Use caution!");
    
    if (ImGui::BeginCombo("Z Target Mode", gSaveContext.zTargetSetting ? "Hold" : "Switch")) {
        if (ImGui::Selectable("Switch")) {
            gSaveContext.zTargetSetting = 0;
        }
        if (ImGui::Selectable("Hold")) {
            gSaveContext.zTargetSetting = 1;
        }
        ImGui::EndCombo();
    }
    InsertHelpHoverText("Z-Targeting behavior");


    ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
    static std::array<const char*, 7> minigameHS = { "Horseback Archery", 
        "Big Poe Points",                                            
        "Fishing",
        "Malon's Obstacle Course",                                    
        "Running Man Race",
        "?",
        "Dampe's Race" };
    
    if (ImGui::TreeNode("Minigames")) {
        for (int i = 0; i < 7; i++) {
            if (i == 5) { //HS_UNK_05 is unused
                continue;
            }
            std::string minigameLbl = minigameHS[i];
            ImGui::InputScalar(minigameLbl.c_str(), ImGuiDataType_S32, &gSaveContext.highScores[i], &one, NULL);
        }
        
        ImGui::TreePop();
    }
    
    ImGui::PopItemWidth();
}

void DrawBGSItemFlag(uint8_t itemID) {
    const ItemMapEntry& slotEntry = itemMapping[itemID];
    ImGui::Image(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::SameLine();
    int tradeIndex = itemID - ITEM_POCKET_EGG;
    bool hasItem = (gSaveContext.adultTradeItems & (1 << tradeIndex)) != 0;
    bool shouldHaveItem = hasItem;
    ImGui::Checkbox(("##adultTradeFlag" + std::to_string(itemID)).c_str(), &shouldHaveItem);
    if (hasItem != shouldHaveItem) {
        if (shouldHaveItem) {
            gSaveContext.adultTradeItems |= (1 << tradeIndex);
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_NONE) {
                INV_CONTENT(ITEM_TRADE_ADULT) = ITEM_POCKET_EGG + tradeIndex;
            }
        } else {
            gSaveContext.adultTradeItems &= ~(1 << tradeIndex);
            Inventory_ReplaceItem(gGlobalCtx, itemID, Randomizer_GetNextAdultTradeItem());
        }
    }
}

void DrawInventoryTab() {
    static bool restrictToValid = true;

    ImGui::Checkbox("Restrict to valid items", &restrictToValid);
    InsertHelpHoverText("Restricts items and ammo to only what is possible to legally acquire in-game");

    for (int32_t y = 0; y < 4; y++) {
        for (int32_t x = 0; x < 6; x++) {
            int32_t index = x + y * 6;
            static int32_t selectedIndex = -1;
            static const char* itemPopupPicker = "itemPopupPicker";

            ImGui::PushID(index);

            if (x != 0) {
                ImGui::SameLine();
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            uint8_t item = gSaveContext.inventory.items[index];
            if (item != ITEM_NONE) {
                const ItemMapEntry& slotEntry = itemMapping.find(item)->second;
                if (ImGui::ImageButton(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0),
                                       ImVec2(1, 1), 0)) {
                    selectedIndex = index;
                    ImGui::OpenPopup(itemPopupPicker);
                }
            } else {
                if (ImGui::Button("##itemNone", ImVec2(32.0f, 32.0f))) {
                    selectedIndex = index;
                    ImGui::OpenPopup(itemPopupPicker);
                }
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            if (ImGui::BeginPopup(itemPopupPicker)) {
                if (ImGui::Button("##itemNonePicker", ImVec2(32.0f, 32.0f))) {
                    gSaveContext.inventory.items[selectedIndex] = ITEM_NONE;
                    if (selectedIndex == SLOT_TRADE_ADULT) {
                        gSaveContext.adultTradeItems = 0;
                    }
                    ImGui::CloseCurrentPopup();
                }
                SetLastItemHoverText("None");

                std::vector<ItemMapEntry> possibleItems;
                if (restrictToValid) {
                    // Scan gItemSlots to find legal items for this slot. Bottles are a special case
                    for (int slotIndex = 0; slotIndex < 56; slotIndex++) {
                        int testIndex = (selectedIndex == SLOT_BOTTLE_1 || selectedIndex == SLOT_BOTTLE_2 ||
                                         selectedIndex == SLOT_BOTTLE_3 || selectedIndex == SLOT_BOTTLE_4)
                                            ? SLOT_BOTTLE_1
                                            : selectedIndex;
                        if (gItemSlots[slotIndex] == testIndex) {
                            possibleItems.push_back(itemMapping[slotIndex]);
                        }
                    }
                } else {
                    for (const auto& entry : itemMapping) {
                        possibleItems.push_back(entry.second);
                    }
                }

                for (int32_t pickerIndex = 0; pickerIndex < possibleItems.size(); pickerIndex++) {
                    if (((pickerIndex + 1) % 8) != 0) {
                        ImGui::SameLine();
                    }
                    const ItemMapEntry& slotEntry = possibleItems[pickerIndex];
                    if (ImGui::ImageButton(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f),
                                           ImVec2(0, 0), ImVec2(1, 1), 0)) {
                        gSaveContext.inventory.items[selectedIndex] = slotEntry.id;
                        // Set adult trade item flag if you're playing adult trade shuffle in rando  
                        if (gSaveContext.n64ddFlag &&
                            OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ADULT_TRADE);
                            selectedIndex == SLOT_TRADE_ADULT &&
                            slotEntry.id >= ITEM_POCKET_EGG && slotEntry.id <= ITEM_CLAIM_CHECK) {
                            gSaveContext.adultTradeItems |= ADULT_TRADE_FLAG(slotEntry.id);
                        }
                        ImGui::CloseCurrentPopup();
                    }
                    SetLastItemHoverText(SohUtils::GetItemName(slotEntry.id));
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleVar();

            ImGui::PopID();
        }
    }

    ImGui::Text("Ammo");
    for (uint32_t ammoIndex = 0, drawnAmmoItems = 0; ammoIndex < 16; ammoIndex++) {
        uint8_t item = (restrictToValid) ? gAmmoItems[ammoIndex] : gAllAmmoItems[ammoIndex];
        if (item != ITEM_NONE) {
            // For legal items, display as 1 row of 7. For unrestricted items, display rows of 6 to match
            // inventory
            if ((restrictToValid && (drawnAmmoItems != 0)) || ((drawnAmmoItems % 6) != 0)) {
                ImGui::SameLine();
            }
            drawnAmmoItems++;

            ImGui::PushID(ammoIndex);
            ImGui::PushItemWidth(32.0f);
            ImGui::BeginGroup();

            ImGui::Image(SohImGui::GetTextureByName(itemMapping[item].name), ImVec2(32.0f, 32.0f));
            ImGui::InputScalar("##ammoInput", ImGuiDataType_S8, &AMMO(item));

            ImGui::EndGroup();
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
    }
    
    // Trade quest flags are only used when shuffling the trade sequence, so
    // don't show this if it isn't needed.
    if (gSaveContext.n64ddFlag && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ADULT_TRADE)
        && ImGui::TreeNode("Adult trade quest items")) {
        for (int i = ITEM_POCKET_EGG; i <= ITEM_CLAIM_CHECK; i++) {
            DrawBGSItemFlag(i);
        }
        ImGui::TreePop();
    }
}

// Draw a flag bitfield as an grid of checkboxes
void DrawFlagArray32(const std::string& name, uint32_t& flags) {
    ImGui::PushID(name.c_str());
    for (int32_t flagIndex = 0; flagIndex < 32; flagIndex++) {
        if ((flagIndex % 8) != 0) {
            ImGui::SameLine();
        }
        ImGui::PushID(flagIndex);
        uint32_t bitMask = 1 << flagIndex;
        bool flag = (flags & bitMask) != 0;
        if (ImGui::Checkbox("##check", &flag)) {
            if (flag) {
                flags |= bitMask;
            } else {
                flags &= ~bitMask;
            }
        }
        ImGui::PopID();
    }
    ImGui::PopID();
}

void DrawFlagArray16(FlagTable& flagTable, uint16_t row, uint16_t& flags) {
    ImGui::PushID((std::to_string(row) + flagTable.name).c_str());
    for (int32_t flagIndex = 15; flagIndex >= 0; flagIndex--) {
        ImGui::SameLine();
        ImGui::PushID(flagIndex);
        bool hasDescription = !!flagTable.flagDescriptions[row * 16 + flagIndex];
        uint32_t bitMask = 1 << flagIndex;
        ImGui::PushStyleColor(ImGuiCol_FrameBg, hasDescription ? ImVec4(0.16f, 0.29f, 0.48f, 0.54f) : ImVec4(0.16f, 0.29f, 0.48f, 0.24f));
        bool flag = (flags & bitMask) != 0;
        if (ImGui::Checkbox("##check", &flag)) {
            if (flag) {
                flags |= bitMask;
            } else {
                flags &= ~bitMask;
            }
        }
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered() && hasDescription) {
            ImGui::BeginTooltip();
            ImGui::Text(SohImGui::BreakTooltip(flagTable.flagDescriptions[row * 16 + flagIndex], 60).c_str());
            ImGui::EndTooltip();
        }
        ImGui::PopID();
    }
    ImGui::PopID();
}

void DrawFlagsTab() {
    if (ImGui::TreeNode("Current Scene")) {
        if (gGlobalCtx != nullptr) {
            ActorContext* act = &gGlobalCtx->actorCtx;

            DrawGroupWithBorder([&]() {
                ImGui::Text("Switch");
                InsertHelpHoverText("Permanently-saved switch flags");
                DrawFlagArray32("Switch", act->flags.swch);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Switch");
                InsertHelpHoverText("Temporary switch flags. Unset on scene transitions");
                DrawFlagArray32("Temp Switch", act->flags.tempSwch);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Clear");
                InsertHelpHoverText("Permanently-saved room-clear flags");
                DrawFlagArray32("Clear", act->flags.clear);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Clear");
                InsertHelpHoverText("Temporary room-clear flags. Unset on scene transitions");
                DrawFlagArray32("Temp Clear", act->flags.tempClear);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Collect");
                InsertHelpHoverText("Permanently-saved collect flags");
                DrawFlagArray32("Collect", act->flags.collect);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("Temp Collect");
                InsertHelpHoverText("Temporary collect flags. Unset on scene transitions");
                DrawFlagArray32("Temp Collect", act->flags.tempCollect);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Chest");
                InsertHelpHoverText("Permanently-saved chest flags");
                DrawFlagArray32("Chest", act->flags.chest);
            });

            ImGui::SameLine();

            ImGui::BeginGroup();

            if (ImGui::Button("Reload Flags")) {
                act->flags.swch = gSaveContext.sceneFlags[gGlobalCtx->sceneNum].swch;
                act->flags.clear = gSaveContext.sceneFlags[gGlobalCtx->sceneNum].clear;
                act->flags.collect = gSaveContext.sceneFlags[gGlobalCtx->sceneNum].collect;
                act->flags.chest = gSaveContext.sceneFlags[gGlobalCtx->sceneNum].chest;
            }
            SetLastItemHoverText("Load flags from saved scene flags. Normally happens on scene load");

            if (ImGui::Button("Save Flags")) {
                gSaveContext.sceneFlags[gGlobalCtx->sceneNum].swch = act->flags.swch;
                gSaveContext.sceneFlags[gGlobalCtx->sceneNum].clear = act->flags.clear;
                gSaveContext.sceneFlags[gGlobalCtx->sceneNum].collect = act->flags.collect;
                gSaveContext.sceneFlags[gGlobalCtx->sceneNum].chest = act->flags.chest;
            }
            SetLastItemHoverText("Save current scene flags. Normally happens on scene exit");

            ImGui::EndGroup();
        } else {
            ImGui::Text("Current game state does not have an active scene");
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Saved Scene Flags")) {
        static uint32_t selectedSceneFlagMap = 0;
        ImGui::Text("Map");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Map", SohUtils::GetSceneName(selectedSceneFlagMap).c_str())) {
            for (int32_t sceneIndex = 0; sceneIndex < SCENE_ID_MAX; sceneIndex++) {
                if (ImGui::Selectable(SohUtils::GetSceneName(sceneIndex).c_str())) {
                    selectedSceneFlagMap = sceneIndex;
                }
            }

            ImGui::EndCombo();
        }

        // Don't show current scene button if there is no current scene
        if (gGlobalCtx != nullptr) {
            ImGui::SameLine();
            if (ImGui::Button("Current")) {
                selectedSceneFlagMap = gGlobalCtx->sceneNum;
            }
            SetLastItemHoverText("Open flags for current scene");
        }

        DrawGroupWithBorder([&]() {
            ImGui::Text("Switch");
            InsertHelpHoverText("Switch flags");
            DrawFlagArray32("Switch", gSaveContext.sceneFlags[selectedSceneFlagMap].swch);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Clear");
            InsertHelpHoverText("Room-clear flags");
            DrawFlagArray32("Clear", gSaveContext.sceneFlags[selectedSceneFlagMap].clear);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Collect");
            InsertHelpHoverText("Collect flags");
            DrawFlagArray32("Collect", gSaveContext.sceneFlags[selectedSceneFlagMap].collect);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Chest");
            InsertHelpHoverText("Chest flags");
            DrawFlagArray32("Chest", gSaveContext.sceneFlags[selectedSceneFlagMap].chest);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Rooms");
            InsertHelpHoverText("Flags for visted rooms");
            DrawFlagArray32("Rooms", gSaveContext.sceneFlags[selectedSceneFlagMap].rooms);
        });

        ImGui::SameLine();

        DrawGroupWithBorder([&]() {
            ImGui::Text("Floors");
            InsertHelpHoverText("Flags for visted floors");
            DrawFlagArray32("Floors", gSaveContext.sceneFlags[selectedSceneFlagMap].floors);
        });

        ImGui::TreePop();
    }

    DrawGroupWithBorder([&]() {
        static uint32_t selectedGsMap = 0;
        ImGui::Text("Gold Skulltulas");
        ImGui::Text("Map");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Gold Skulltula Map", gsMapping[selectedGsMap].c_str())) {
            for (int32_t gsIndex = 0; gsIndex < gsMapping.size(); gsIndex++) {
                if (ImGui::Selectable(gsMapping[gsIndex].c_str())) {
                    selectedGsMap = gsIndex;
                }
            }

            ImGui::EndCombo();
        }

        // TODO We should write out descriptions for each one... ugh
        ImGui::Text("Flags");
        uint32_t currentFlags = GET_GS_FLAGS(selectedGsMap);
        uint32_t allFlags = gAreaGsFlags[selectedGsMap];
        uint32_t setMask = 1;
        // Iterate over bitfield and create a checkbox for each skulltula
        while (allFlags != 0) {
            bool isThisSet = (currentFlags & 0x1) == 0x1;

            ImGui::SameLine();
            ImGui::PushID(allFlags);
            if (ImGui::Checkbox("##gs", &isThisSet)) {
                if (isThisSet) {
                    SET_GS_FLAGS(selectedGsMap, setMask);
                } else {
                    // Have to do this roundabout method as the macro does not support clearing flags
                    uint32_t currentFlagsBase = GET_GS_FLAGS(selectedGsMap);
                    gSaveContext.gsFlags[selectedGsMap >> 2] &= ~gGsFlagsMasks[selectedGsMap & 3];
                    SET_GS_FLAGS(selectedGsMap, currentFlagsBase & ~setMask);
                }
            }

            ImGui::PopID();

            allFlags >>= 1;
            currentFlags >>= 1;
            setMask <<= 1;
        }

        // If playing a Randomizer Save with Shuffle Skull Tokens on anything other than "Off" we don't want to keep
        // GS Token Count updated, since Gold Skulltulas killed will not correlate to GS Tokens Collected.
        if (!(gSaveContext.n64ddFlag && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_TOKENS))) {
            static bool keepGsCountUpdated = true;
            ImGui::Checkbox("Keep GS Count Updated", &keepGsCountUpdated);
            InsertHelpHoverText("Automatically adjust the number of gold skulltula tokens acquired based on set flags.");
            int32_t gsCount = 0;
            if (keepGsCountUpdated) {
                for (int32_t gsFlagIndex = 0; gsFlagIndex < 6; gsFlagIndex++) {
                    gsCount += std::popcount(static_cast<uint32_t>(gSaveContext.gsFlags[gsFlagIndex]));
                }
                gSaveContext.inventory.gsTokens = gsCount;
            }
        }
    });

    for (int i = 0; i < flagTables.size(); i++) {
        FlagTable& flagTable = flagTables[i];
        if (ImGui::TreeNode(flagTable.name)) {
            for (int j = 0; j < flagTable.size + 1; j++) {
                DrawGroupWithBorder([&]() {
                    ImGui::Text(fmt::format("{:<2x}", j).c_str());
                    switch (flagTable.flagTableType) {
                        case EVENT_CHECK_INF:
                            DrawFlagArray16(flagTable, j, gSaveContext.eventChkInf[j]);
                            break;
                        case ITEM_GET_INF:
                            DrawFlagArray16(flagTable, j, gSaveContext.itemGetInf[j]);
                            break;
                        case INF_TABLE:
                            DrawFlagArray16(flagTable, j, gSaveContext.infTable[j]);
                            break;
                        case EVENT_INF:
                            DrawFlagArray16(flagTable, j, gSaveContext.eventInf[j]);
                            break;
                        case RANDOMIZER_INF:
                            DrawFlagArray16(flagTable, j, gSaveContext.randomizerInf[j]);
                            break;
                    }
                });
            }
            ImGui::TreePop();
        }
    }
}

// Draws a combo that lets you choose and upgrade value from a drop-down of text values
void DrawUpgrade(const std::string& categoryName, int32_t categoryId, const std::vector<std::string>& names) {
    ImGui::Text("%s", categoryName.c_str());
    ImGui::SameLine();
    ImGui::PushID(categoryName.c_str());
    if (ImGui::BeginCombo("##upgrade", names[CUR_UPG_VALUE(categoryId)].c_str())) {
        for (int32_t i = 0; i < names.size(); i++) {
            if (ImGui::Selectable(names[i].c_str())) {
                Inventory_ChangeUpgrade(categoryId, i);
            }
        }

        ImGui::EndCombo();
    }
    ImGui::PopID();
    SetLastItemHoverText(categoryName.c_str());
}

// Draws a combo that lets you choose and upgrade value from a popup grid of icons
void DrawUpgradeIcon(const std::string& categoryName, int32_t categoryId, const std::vector<uint8_t>& items) {
    static const char* upgradePopupPicker = "upgradePopupPicker";

    ImGui::PushID(categoryName.c_str());

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    uint8_t item = items[CUR_UPG_VALUE(categoryId)];
    if (item != ITEM_NONE) {
        const ItemMapEntry& slotEntry = itemMapping[item];
        if (ImGui::ImageButton(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0),
                               ImVec2(1, 1), 0)) {
            ImGui::OpenPopup(upgradePopupPicker);
        }
    } else {
        if (ImGui::Button("##itemNone", ImVec2(32.0f, 32.0f))) {
            ImGui::OpenPopup(upgradePopupPicker);
        }
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    SetLastItemHoverText(categoryName.c_str());

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    if (ImGui::BeginPopup(upgradePopupPicker)) {
        for (int32_t pickerIndex = 0; pickerIndex < items.size(); pickerIndex++) {
            if ((pickerIndex % 8) != 0) {
                ImGui::SameLine();
            }

            if (items[pickerIndex] == ITEM_NONE) {
                if (ImGui::Button("##upgradePopupPicker", ImVec2(32.0f, 32.0f))) {
                    Inventory_ChangeUpgrade(categoryId, pickerIndex);
                    ImGui::CloseCurrentPopup();
                }
                SetLastItemHoverText("None");
            } else {
                const ItemMapEntry& slotEntry = itemMapping[items[pickerIndex]];
                if (ImGui::ImageButton(SohImGui::GetTextureByName(slotEntry.name), ImVec2(32.0f, 32.0f), ImVec2(0, 0),
                                       ImVec2(1, 1), 0)) {
                    Inventory_ChangeUpgrade(categoryId, pickerIndex);
                    ImGui::CloseCurrentPopup();
                }
                SetLastItemHoverText(SohUtils::GetItemName(slotEntry.id));
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ImGui::PopID();
}

void DrawEquipmentTab() {
    const std::vector<uint8_t> equipmentValues = {
        ITEM_SWORD_KOKIRI, ITEM_SWORD_MASTER,  ITEM_SWORD_BGS,     ITEM_SWORD_BROKEN,
        ITEM_SHIELD_DEKU,  ITEM_SHIELD_HYLIAN, ITEM_SHIELD_MIRROR, ITEM_NONE,
        ITEM_TUNIC_KOKIRI, ITEM_TUNIC_GORON,   ITEM_TUNIC_ZORA,    ITEM_NONE,
        ITEM_BOOTS_KOKIRI, ITEM_BOOTS_IRON,    ITEM_BOOTS_HOVER,   ITEM_NONE,
    };
    for (int32_t i = 0; i < equipmentValues.size(); i++) {
        // Skip over unused 4th slots for shields, boots, and tunics
        if (equipmentValues[i] == ITEM_NONE) {
            continue;
        }
        if ((i % 4) != 0) {
            ImGui::SameLine();
        }

        ImGui::PushID(i);
        uint32_t bitMask = 1 << i;
        bool hasEquip = (bitMask & gSaveContext.inventory.equipment) != 0;
        const ItemMapEntry& entry = itemMapping[equipmentValues[i]];
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        if (ImGui::ImageButton(SohImGui::GetTextureByName(hasEquip ? entry.name : entry.nameFaded),
                               ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
            if (hasEquip) {
                gSaveContext.inventory.equipment &= ~bitMask;
            } else {
                gSaveContext.inventory.equipment |= bitMask;
            }
        }
        ImGui::PopStyleColor();
        ImGui::PopID();
        SetLastItemHoverText(SohUtils::GetItemName(entry.id));
    }

    const std::vector<uint8_t> bulletBagValues = {
        ITEM_NONE,
        ITEM_BULLET_BAG_30,
        ITEM_BULLET_BAG_40,
        ITEM_BULLET_BAG_50,
    };
    DrawUpgradeIcon("Bullet Bag", UPG_BULLET_BAG, bulletBagValues);

    ImGui::SameLine();

    const std::vector<uint8_t> quiverValues = {
        ITEM_NONE,
        ITEM_QUIVER_30,
        ITEM_QUIVER_40,
        ITEM_QUIVER_50,
    };
    DrawUpgradeIcon("Quiver", UPG_QUIVER, quiverValues);

    ImGui::SameLine();

    const std::vector<uint8_t> bombBagValues = {
        ITEM_NONE,
        ITEM_BOMB_BAG_20,
        ITEM_BOMB_BAG_30,
        ITEM_BOMB_BAG_40,
    };
    DrawUpgradeIcon("Bomb Bag", UPG_BOMB_BAG, bombBagValues);

    ImGui::SameLine();

    const std::vector<uint8_t> scaleValues = {
        ITEM_NONE,
        ITEM_SCALE_SILVER,
        ITEM_SCALE_GOLDEN,
    };
    DrawUpgradeIcon("Scale", UPG_SCALE, scaleValues);

    ImGui::SameLine();

    const std::vector<uint8_t> strengthValues = {
        ITEM_NONE,
        ITEM_BRACELET,
        ITEM_GAUNTLETS_SILVER,
        ITEM_GAUNTLETS_GOLD,
    };
    DrawUpgradeIcon("Strength", UPG_STRENGTH, strengthValues);

    // There is no icon for child wallet, so default to a text list
    const std::vector<std::string> walletNames = {
        "Child (99)",
        "Adult (200)",
        "Giant (500)",
    };
    DrawUpgrade("Wallet", UPG_WALLET, walletNames);

    const std::vector<std::string> stickNames = {
        "None",
        "10",
        "20",
        "30",
    };
    DrawUpgrade("Sticks", UPG_STICKS, stickNames);

    const std::vector<std::string> nutNames = {
        "None",
        "20",
        "30",
        "40",
    };
    DrawUpgrade("Deku Nuts", UPG_NUTS, nutNames);
}

// Draws a toggleable icon for a quest item that is faded when disabled
void DrawQuestItemButton(uint32_t item) {
    const QuestMapEntry& entry = questMapping[item];
    uint32_t bitMask = 1 << entry.id;
    bool hasQuestItem = (bitMask & gSaveContext.inventory.questItems) != 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    if (ImGui::ImageButton(SohImGui::GetTextureByName(hasQuestItem ? entry.name : entry.nameFaded),
                           ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
        if (hasQuestItem) {
            gSaveContext.inventory.questItems &= ~bitMask;
        } else {
            gSaveContext.inventory.questItems |= bitMask;
        }
    }
    ImGui::PopStyleColor();
    SetLastItemHoverText(SohUtils::GetQuestItemName(entry.id));
}

// Draws a toggleable icon for a dungeon item that is faded when disabled
void DrawDungeonItemButton(uint32_t item, uint32_t scene) {
    const ItemMapEntry& entry = itemMapping[item];
    uint32_t bitMask = 1 << (entry.id - ITEM_KEY_BOSS); // Bitset starts at ITEM_KEY_BOSS == 0. the rest are sequential
    bool hasItem = (bitMask & gSaveContext.inventory.dungeonItems[scene]) != 0;
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    if (ImGui::ImageButton(SohImGui::GetTextureByName(hasItem ? entry.name : entry.nameFaded),
                           ImVec2(32.0f, 32.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
        if (hasItem) {
            gSaveContext.inventory.dungeonItems[scene] &= ~bitMask;
        } else {
            gSaveContext.inventory.dungeonItems[scene] |= bitMask;
        }
    }
    ImGui::PopStyleColor();
    SetLastItemHoverText(SohUtils::GetItemName(entry.id));
}

void DrawQuestStatusTab() {
    ImGui::PushItemWidth(ImGui::GetFontSize() * 6);

    for (int32_t i = QUEST_MEDALLION_FOREST; i < QUEST_MEDALLION_LIGHT + 1; i++) {
        if (i != QUEST_MEDALLION_FOREST) {
            ImGui::SameLine();
        }
        DrawQuestItemButton(i);
    }

    for (int32_t i = QUEST_KOKIRI_EMERALD; i < QUEST_ZORA_SAPPHIRE + 1; i++) {
        if (i != QUEST_KOKIRI_EMERALD) {
            ImGui::SameLine();
        }
        DrawQuestItemButton(i);
    }

    // Put Stone of Agony and Gerudo Card on the same line with a little space between them
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20, 0));

    ImGui::SameLine();
    DrawQuestItemButton(QUEST_STONE_OF_AGONY);

    ImGui::SameLine();
    DrawQuestItemButton(QUEST_GERUDO_CARD);

    for (const SongMapEntry& entry : songMapping) {
        if ((entry.id != QUEST_SONG_MINUET) && (entry.id != QUEST_SONG_LULLABY)) {
            ImGui::SameLine();
        }

        uint32_t bitMask = 1 << entry.id;
        bool hasQuestItem = (bitMask & gSaveContext.inventory.questItems) != 0;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        if (ImGui::ImageButton(SohImGui::GetTextureByName(hasQuestItem ? entry.name : entry.nameFaded),
                               ImVec2(16.0f, 24.0f), ImVec2(0, 0), ImVec2(1, 1), 0)) {
            if (hasQuestItem) {
                gSaveContext.inventory.questItems &= ~bitMask;
            } else {
                gSaveContext.inventory.questItems |= bitMask;
            }
        }
        ImGui::PopStyleColor();
        SetLastItemHoverText(SohUtils::GetQuestItemName(entry.id));
    }

    ImGui::InputScalar("GS Count", ImGuiDataType_S16, &gSaveContext.inventory.gsTokens);
    InsertHelpHoverText("Number of gold skulltula tokens aquired");

    uint32_t bitMask = 1 << QUEST_SKULL_TOKEN;
    bool gsUnlocked = (bitMask & gSaveContext.inventory.questItems) != 0;
    if (ImGui::Checkbox("GS unlocked", &gsUnlocked)) {
        if (gsUnlocked) {
            gSaveContext.inventory.questItems |= bitMask;
        } else {
            gSaveContext.inventory.questItems &= ~bitMask;
        }
    }
    InsertHelpHoverText("If unlocked, enables showing the gold skulltula count in the quest status menu");

    int32_t pohCount = (gSaveContext.inventory.questItems & 0xF0000000) >> 28;
    if (ImGui::BeginCombo("PoH count", std::to_string(pohCount).c_str())) {
        for (int32_t i = 0; i < 4; i++) {
            if (ImGui::Selectable(std::to_string(i).c_str(), pohCount == i)) {
                gSaveContext.inventory.questItems &= ~0xF0000000;
                gSaveContext.inventory.questItems |= (i << 28);
            }
        }
        ImGui::EndCombo();
    }
    InsertHelpHoverText("The number of pieces of heart acquired towards the next heart container");

    DrawGroupWithBorder([&]() {
        ImGui::Text("Dungeon Items");

        static int32_t dungeonItemsScene = SCENE_YDAN;
        ImGui::PushItemWidth(-ImGui::GetWindowWidth() * 0.35f);
        if (ImGui::BeginCombo("##DungeonSelect", SohUtils::GetSceneName(dungeonItemsScene).c_str())) {
            for (int32_t dungeonIndex = SCENE_YDAN; dungeonIndex < SCENE_BDAN_BOSS + 1; dungeonIndex++) {
                if (ImGui::Selectable(SohUtils::GetSceneName(dungeonIndex).c_str(),
                                      dungeonIndex == dungeonItemsScene)) {
                    dungeonItemsScene = dungeonIndex;
                }
            }

            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();

        DrawDungeonItemButton(ITEM_KEY_BOSS, dungeonItemsScene);
        ImGui::SameLine();
        DrawDungeonItemButton(ITEM_COMPASS, dungeonItemsScene);
        ImGui::SameLine();
        DrawDungeonItemButton(ITEM_DUNGEON_MAP, dungeonItemsScene);

        if (dungeonItemsScene != SCENE_BDAN_BOSS) {
            float lineHeight = ImGui::GetTextLineHeightWithSpacing();
            ImGui::Image(SohImGui::GetTextureByName(itemMapping[ITEM_KEY_SMALL].name), ImVec2(lineHeight, lineHeight));
            ImGui::SameLine();
            ImGui::InputScalar("##Keys", ImGuiDataType_S8, gSaveContext.inventory.dungeonKeys + dungeonItemsScene);
        } else {
            // dungeonItems is size 20 but dungeonKeys is size 19, so there are no keys for the last scene (Barinade's Lair)
            ImGui::Text("Barinade's Lair does not have small keys");
        }
    });

    ImGui::PopItemWidth();
}

void DrawPlayerTab() {
    if (gGlobalCtx != nullptr) {
        Player* player = GET_PLAYER(gGlobalCtx);
        const char* curSword;
        const char* curShield;
        const char* curTunic;
        const char* curBoots;

        switch (player->currentSwordItem) {
            case ITEM_SWORD_KOKIRI:
                curSword = "Kokiri Sword"; 
                break;
            case ITEM_SWORD_MASTER:
                curSword = "Master Sword";
                break;
            case ITEM_SWORD_BGS:
                curSword = "Biggoron's Sword";
                break;
            case ITEM_FISHING_POLE:
                curSword = "Fishing Pole";
                break;
            case ITEM_NONE:
                curSword = "None";
                break;
            default:
                curSword = "None";
                break;
        }

        switch (player->currentShield) {
            case PLAYER_SHIELD_NONE:
                curShield = "None";
                break;
            case PLAYER_SHIELD_DEKU:
                curShield = "Deku Shield";
                break;
            case PLAYER_SHIELD_HYLIAN:
                curShield = "Hylian Shield";
                break;
            case PLAYER_SHIELD_MIRROR:
                curShield = "Mirror Shield";
                break;
            default:
                break;
        }

        switch (player->currentTunic) {
            case PLAYER_TUNIC_KOKIRI:
                curTunic = "Kokiri Tunic";
                break;
            case PLAYER_TUNIC_GORON:
                curTunic = "Goron Tunic";
                break;
            case PLAYER_TUNIC_ZORA:
                curTunic = "Zora Tunic";
                break;
            default:
                break;
        }

        switch (player->currentBoots) {
            case PLAYER_BOOTS_KOKIRI:
                curBoots = "Kokiri Boots";
                break;
            case PLAYER_BOOTS_IRON:
                curBoots = "Iron Boots";
                break;
            case PLAYER_BOOTS_HOVER:
                curBoots = "Hover Boots";
                break;
            default:
                break;
        }

        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        DrawGroupWithBorder([&]() {
            ImGui::Text("Link's Position");
            ImGui::InputScalar("X Pos", ImGuiDataType_Float, &player->actor.world.pos.x);
            ImGui::SameLine();
            ImGui::InputScalar("Y Pos", ImGuiDataType_Float, &player->actor.world.pos.y);
            ImGui::SameLine();
            ImGui::InputScalar("Z Pos", ImGuiDataType_Float, &player->actor.world.pos.z);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Link's Rotation");
            InsertHelpHoverText("For Link's rotation in relation to the world");
            ImGui::InputScalar("X Rot", ImGuiDataType_S16, &player->actor.world.rot.x);
            ImGui::SameLine();
            ImGui::InputScalar("Y Rot", ImGuiDataType_S16, &player->actor.world.rot.y);
            ImGui::SameLine();
            ImGui::InputScalar("Z Rot", ImGuiDataType_S16, &player->actor.world.rot.z);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("Link's Model Rotation");
            InsertHelpHoverText("For Link's actual model");
            ImGui::InputScalar("X ModRot", ImGuiDataType_S16, &player->actor.shape.rot.x);
            ImGui::SameLine();
            ImGui::InputScalar("Y ModRot", ImGuiDataType_S16, &player->actor.shape.rot.y);
            ImGui::SameLine();
            ImGui::InputScalar("Z ModRot", ImGuiDataType_S16, &player->actor.shape.rot.z);
        });

        ImGui::InputScalar("Linear Velocity", ImGuiDataType_Float, &player->linearVelocity);
        InsertHelpHoverText("Link's speed along the XZ plane");

        ImGui::InputScalar("Y Velocity", ImGuiDataType_Float, &player->actor.velocity.y);
        InsertHelpHoverText("Link's speed along the Y plane. Caps at -20");

        ImGui::InputScalar("Wall Height", ImGuiDataType_Float, &player->wallHeight);
        InsertHelpHoverText("Height used to determine whether Link can climb or grab a ledge at the top");

        ImGui::InputScalar("Invincibility Timer", ImGuiDataType_S8, &player->invincibilityTimer);
        InsertHelpHoverText("Can't take damage while this is nonzero");

        ImGui::InputScalar("Gravity", ImGuiDataType_Float, &player->actor.gravity);
        InsertHelpHoverText("Rate at which Link falls. Default -4.0f");

        if (ImGui::BeginCombo("Link Age on Load", gGlobalCtx->linkAgeOnLoad == 0 ? "Adult" : "Child")) {
            if (ImGui::Selectable("Adult")) {
                gGlobalCtx->linkAgeOnLoad = 0;
            }
            if (ImGui::Selectable("Child")) {
                gGlobalCtx->linkAgeOnLoad = 1;
            }
            ImGui::EndCombo();
        }

        InsertHelpHoverText("This will change Link's age when you load a map");

        ImGui::Separator();
        
        ImGui::Text("Link's Current Equipment");
        ImGui::PushItemWidth(ImGui::GetFontSize() * 15);
        if (ImGui::BeginCombo("Sword", curSword)) {
            if (ImGui::Selectable("None")) {
                player->currentSwordItem = ITEM_NONE;
                gSaveContext.equips.buttonItems[0] = ITEM_NONE;
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_NONE);
            }
            if (ImGui::Selectable("Kokiri Sword")) {
                player->currentSwordItem = ITEM_SWORD_KOKIRI;
                gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KOKIRI;
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_KOKIRI);
            }
            if (ImGui::Selectable("Master Sword")) {
                player->currentSwordItem = ITEM_SWORD_MASTER;
                gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_MASTER);
            }
            if (ImGui::Selectable("Biggoron's Sword")) {
                if (gSaveContext.bgsFlag) {
                    if (gSaveContext.swordHealth < 8) {
                        gSaveContext.swordHealth = 8;
                    }
                    player->currentSwordItem = ITEM_SWORD_BGS;
                    gSaveContext.equips.buttonItems[0] = ITEM_SWORD_BGS;
                } else {
                    if (gSaveContext.swordHealth < 8) {
                        gSaveContext.swordHealth = 8;
                    }
                    player->currentSwordItem = ITEM_SWORD_BGS;
                    gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KNIFE;
                }
                
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_BGS);
            }
            if (ImGui::Selectable("Fishing Pole")) {
                player->currentSwordItem = ITEM_FISHING_POLE;
                gSaveContext.equips.buttonItems[0] = ITEM_FISHING_POLE;
                Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_MASTER);
            }
            ImGui::EndCombo();

        }
        if (ImGui::BeginCombo("Shield", curShield)) {
            if (ImGui::Selectable("None")) {
                player->currentShield = PLAYER_SHIELD_NONE;
                Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_NONE);
            }
            if (ImGui::Selectable("Deku Shield")) {
                player->currentShield = PLAYER_SHIELD_DEKU;
                Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_DEKU);
            }
            if (ImGui::Selectable("Hylian Shield")) {
                player->currentShield = PLAYER_SHIELD_HYLIAN;
                Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_HYLIAN);
            }
            if (ImGui::Selectable("Mirror Shield")) {
                player->currentShield = PLAYER_SHIELD_MIRROR;
                Inventory_ChangeEquipment(EQUIP_SHIELD, PLAYER_SHIELD_MIRROR);
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Tunic", curTunic)) {
            if (ImGui::Selectable("Kokiri Tunic")) {
                player->currentTunic = PLAYER_TUNIC_KOKIRI;
                Inventory_ChangeEquipment(EQUIP_TUNIC, PLAYER_TUNIC_KOKIRI + 1);
            }
            if (ImGui::Selectable("Goron Tunic")) {
                player->currentTunic = PLAYER_TUNIC_GORON;
                Inventory_ChangeEquipment(EQUIP_TUNIC, PLAYER_TUNIC_GORON + 1);
            }
            if (ImGui::Selectable("Zora Tunic")) {
                player->currentTunic = PLAYER_TUNIC_ZORA;
                Inventory_ChangeEquipment(EQUIP_TUNIC, PLAYER_TUNIC_ZORA + 1);
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Boots", curBoots)) {
            if (ImGui::Selectable("Kokiri Boots")) {
                player->currentBoots = PLAYER_BOOTS_KOKIRI;
                Inventory_ChangeEquipment(EQUIP_BOOTS, PLAYER_BOOTS_KOKIRI + 1);
            }
            if (ImGui::Selectable("Iron Boots")) {
                player->currentBoots = PLAYER_BOOTS_IRON;
                Inventory_ChangeEquipment(EQUIP_BOOTS, PLAYER_BOOTS_IRON + 1);
            }
            if (ImGui::Selectable("Hover Boots")) {
                player->currentBoots = PLAYER_BOOTS_HOVER;
                Inventory_ChangeEquipment(EQUIP_BOOTS, PLAYER_BOOTS_HOVER + 1);
            }
            ImGui::EndCombo();
        }

        ImU16 one = 1;
        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        DrawGroupWithBorder([&]() {
            ImGui::Text("Current C Equips");
            ImGui::InputScalar("C Left", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[1], &one, NULL);
            ImGui::SameLine();
            ImGui::InputScalar("C Down", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[2], &one, NULL);
            ImGui::SameLine();
            ImGui::InputScalar("C Right", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[3], &one, NULL);

            if (CVar_GetS32("gDpadEquips", 0)) {
                ImGui::NewLine();
                ImGui::Text("Current D-pad Equips");
                ImGui::InputScalar("D-pad Up  ", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[4], &one, NULL); // Two spaces at the end for aligning, not elegant but it's working
                ImGui::SameLine();
                ImGui::InputScalar("D-pad Down", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[5], &one, NULL);
                // Intentionnal to not put everything on the same line, else it's taking too much for lower resolution.
                ImGui::InputScalar("D-pad Left", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[6], &one, NULL);
                ImGui::SameLine();
                ImGui::InputScalar("D-pad Right", ImGuiDataType_U8, &gSaveContext.equips.buttonItems[7], &one, NULL);
            }
        });

    } else {
        ImGui::Text("Global Context needed for player info!");
    }
}

void DrawSaveEditor(bool& open) {
    if (!open) {
        CVar_SetS32("gSaveEditorEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Save Editor", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("SaveContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Info")) {
            DrawInfoTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Inventory")) {
            DrawInventoryTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Flags")) {
            DrawFlagsTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Equipment")) {
            DrawEquipmentTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Quest Status")) {
            DrawQuestStatusTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Player")) {
            DrawPlayerTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void InitSaveEditor() {
    SohImGui::AddWindow("Developer Tools", "Save Editor", DrawSaveEditor);

    // Load item icons into ImGui
    for (const auto& entry : itemMapping) {
        SohImGui::LoadResource(entry.second.name, entry.second.texturePath);
        SohImGui::LoadResource(entry.second.nameFaded, entry.second.texturePath, ImVec4(1, 1, 1, 0.3f));
    }
    for (const auto& entry : questMapping) {
        SohImGui::LoadResource(entry.second.name, entry.second.texturePath);
        SohImGui::LoadResource(entry.second.nameFaded, entry.second.texturePath, ImVec4(1, 1, 1, 0.3f));
    }
    for (const auto& entry : songMapping) {
        SohImGui::LoadResource(entry.name, gSongNoteTex, entry.color);
        ImVec4 fadedCol = entry.color;
        fadedCol.w = 0.3f;
        SohImGui::LoadResource(entry.nameFaded, gSongNoteTex, fadedCol);
    }
    for (const auto& entry : vanillaSongMapping) {
        SohImGui::LoadResource(entry.name, gSongNoteTex, entry.color);
        ImVec4 fadedCol = entry.color;
        fadedCol.w = 0.3f;
        SohImGui::LoadResource(entry.nameFaded, gSongNoteTex, fadedCol);
    }
}
