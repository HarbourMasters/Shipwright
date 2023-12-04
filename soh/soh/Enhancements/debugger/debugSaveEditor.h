#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <soh/Enhancements/randomizer/randomizer_inf.h>
#include <libultraship/libultraship.h>

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
// The source was last referenced on 2022-09-03 and had a last updated value of 2020-05-02
const std::vector<FlagTable> flagTables = {
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
        { 0x0F, "Spoke to Mido about Saria's whereabouts" },
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
        { 0x1B, "Obtained Treasure Chest Game Reward" },
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
        { 0x41, "Spoke to Kokiri Boy on Bed in Mido's House" },
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
        { 0xBC, "Fado requested Odd Potion" },
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
   { "Randomizer Inf Flags", RANDOMIZER_INF, 0x09, {
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

        { RAND_INF_COWS_MILKED_KF_LINKS_HOUSE_COW, "COWS_MILKED_KF_LINKS_HOUSE_COW" },
        { RAND_INF_COWS_MILKED_HF_COW_GROTTO_COW, "COWS_MILKED_HF_COW_GROTTO_COW" },
        { RAND_INF_COWS_MILKED_LLR_STABLES_LEFT_COW, "COWS_MILKED_LLR_STABLES_LEFT_COW" },
        { RAND_INF_COWS_MILKED_LLR_STABLES_RIGHT_COW, "COWS_MILKED_LLR_STABLES_RIGHT_COW" },
        { RAND_INF_COWS_MILKED_LLR_TOWER_LEFT_COW, "COWS_MILKED_LLR_TOWER_LEFT_COW" },
        { RAND_INF_COWS_MILKED_LLR_TOWER_RIGHT_COW, "COWS_MILKED_LLR_TOWER_RIGHT_COW" },
        { RAND_INF_COWS_MILKED_KAK_IMPAS_HOUSE_COW, "COWS_MILKED_KAK_IMPAS_HOUSE_COW" },
        { RAND_INF_COWS_MILKED_DMT_COW_GROTTO_COW, "COWS_MILKED_DMT_COW_GROTTO_COW" },
        { RAND_INF_COWS_MILKED_GV_COW, "COWS_MILKED_GV_COW" },
        { RAND_INF_COWS_MILKED_JABU_JABUS_BELLY_MQ_COW, "COWS_MILKED_JABU_JABUS_BELLY_MQ_COW" },

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
        { RAND_INF_SCRUBS_PURCHASED_DEKU_TREE_MQ_DEKU_SCRUB, "SCRUBS_PURCHASED_DEKU_TREE_MQ_DEKU_SCRUB" },
        { RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR, "SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR" },
        { RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT, "SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT" },
        { RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE, "SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE" },
        { RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, "SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS" },
        { RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT, "SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT, "SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT" },
        { RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER, "SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER" },
        { RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, "SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT" },
        { RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT, "SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT" },

        { RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1, "SHOP_ITEMS_KF_SHOP_ITEM_1" },
        { RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_2, "SHOP_ITEMS_KF_SHOP_ITEM_2" },
        { RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_3, "SHOP_ITEMS_KF_SHOP_ITEM_3" },
        { RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_4, "SHOP_ITEMS_KF_SHOP_ITEM_4" },
        { RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_5, "SHOP_ITEMS_KF_SHOP_ITEM_5" },
        { RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_6, "SHOP_ITEMS_KF_SHOP_ITEM_6" },
        { RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_7, "SHOP_ITEMS_KF_SHOP_ITEM_7" },
        { RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_8, "SHOP_ITEMS_KF_SHOP_ITEM_8" },
        { RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_1, "SHOP_ITEMS_GC_SHOP_ITEM_1" },
        { RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_2, "SHOP_ITEMS_GC_SHOP_ITEM_2" },
        { RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_3, "SHOP_ITEMS_GC_SHOP_ITEM_3" },
        { RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_4, "SHOP_ITEMS_GC_SHOP_ITEM_4" },
        { RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_5, "SHOP_ITEMS_GC_SHOP_ITEM_5" },
        { RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_6, "SHOP_ITEMS_GC_SHOP_ITEM_6" },
        { RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_7, "SHOP_ITEMS_GC_SHOP_ITEM_7" },
        { RAND_INF_SHOP_ITEMS_GC_SHOP_ITEM_8, "SHOP_ITEMS_GC_SHOP_ITEM_8" },
        { RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_1, "SHOP_ITEMS_ZD_SHOP_ITEM_1" },
        { RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_2, "SHOP_ITEMS_ZD_SHOP_ITEM_2" },
        { RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_3, "SHOP_ITEMS_ZD_SHOP_ITEM_3" },
        { RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_4, "SHOP_ITEMS_ZD_SHOP_ITEM_4" },
        { RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_5, "SHOP_ITEMS_ZD_SHOP_ITEM_5" },
        { RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_6, "SHOP_ITEMS_ZD_SHOP_ITEM_6" },
        { RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_7, "SHOP_ITEMS_ZD_SHOP_ITEM_7" },
        { RAND_INF_SHOP_ITEMS_ZD_SHOP_ITEM_8, "SHOP_ITEMS_ZD_SHOP_ITEM_8" },
        { RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_1, "SHOP_ITEMS_KAK_BAZAAR_ITEM_1" },
        { RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_2, "SHOP_ITEMS_KAK_BAZAAR_ITEM_2" },
        { RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_3, "SHOP_ITEMS_KAK_BAZAAR_ITEM_3" },
        { RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_4, "SHOP_ITEMS_KAK_BAZAAR_ITEM_4" },
        { RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_5, "SHOP_ITEMS_KAK_BAZAAR_ITEM_5" },
        { RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_6, "SHOP_ITEMS_KAK_BAZAAR_ITEM_6" },
        { RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_7, "SHOP_ITEMS_KAK_BAZAAR_ITEM_7" },
        { RAND_INF_SHOP_ITEMS_KAK_BAZAAR_ITEM_8, "SHOP_ITEMS_KAK_BAZAAR_ITEM_8" },
        { RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_1, "SHOP_ITEMS_KAK_POTION_SHOP_ITEM_1" },
        { RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_2, "SHOP_ITEMS_KAK_POTION_SHOP_ITEM_2" },
        { RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_3, "SHOP_ITEMS_KAK_POTION_SHOP_ITEM_3" },
        { RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_4, "SHOP_ITEMS_KAK_POTION_SHOP_ITEM_4" },
        { RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_5, "SHOP_ITEMS_KAK_POTION_SHOP_ITEM_5" },
        { RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_6, "SHOP_ITEMS_KAK_POTION_SHOP_ITEM_6" },
        { RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_7, "SHOP_ITEMS_KAK_POTION_SHOP_ITEM_7" },
        { RAND_INF_SHOP_ITEMS_KAK_POTION_SHOP_ITEM_8, "SHOP_ITEMS_KAK_POTION_SHOP_ITEM_8" },
        { RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_1, "SHOP_ITEMS_MARKET_BAZAAR_ITEM_1" },
        { RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_2, "SHOP_ITEMS_MARKET_BAZAAR_ITEM_2" },
        { RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_3, "SHOP_ITEMS_MARKET_BAZAAR_ITEM_3" },
        { RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_4, "SHOP_ITEMS_MARKET_BAZAAR_ITEM_4" },
        { RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_5, "SHOP_ITEMS_MARKET_BAZAAR_ITEM_5" },
        { RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_6, "SHOP_ITEMS_MARKET_BAZAAR_ITEM_6" },
        { RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_7, "SHOP_ITEMS_MARKET_BAZAAR_ITEM_7" },
        { RAND_INF_SHOP_ITEMS_MARKET_BAZAAR_ITEM_8, "SHOP_ITEMS_MARKET_BAZAAR_ITEM_8" },
        { RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_1, "SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_1" },
        { RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_2, "SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_2" },
        { RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_3, "SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_3" },
        { RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_4, "SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_4" },
        { RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_5, "SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_5" },
        { RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_6, "SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_6" },
        { RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_7, "SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_7" },
        { RAND_INF_SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_8, "SHOP_ITEMS_MARKET_POTION_SHOP_ITEM_8" },
        { RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_1, "SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_1" },
        { RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_2, "SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_2" },
        { RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_3, "SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_3" },
        { RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_4, "SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_4" },
        { RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_5, "SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_5" },
        { RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_6, "SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_6" },
        { RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_7, "SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_7" },
        { RAND_INF_SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_8, "SHOP_ITEMS_MARKET_BOMBCHU_SHOP_ITEM_8" },
        
        { RAND_INF_MERCHANTS_CARPET_SALESMAN, "RAND_INF_MERCHANTS_CARPET_SALESMAN" },
        { RAND_INF_MERCHANTS_MEDIGORON, "RAND_INF_MERCHANTS_MEDIGORON" },
        { RAND_INF_MERCHANTS_GRANNYS_SHOP, "RAND_INF_MERCHANTS_GRANNY_SHOP"},

        { RAND_INF_ADULT_TRADES_LW_TRADE_COJIRO, "ADULT_TRADES_LW_TRADE_COJIRO" },
        { RAND_INF_ADULT_TRADES_GV_TRADE_SAW, "ADULT_TRADES_GV_TRADE_SAW" },
        { RAND_INF_ADULT_TRADES_DMT_TRADE_BROKEN_SWORD, "ADULT_TRADES_DMT_TRADE_BROKEN_SWORD" },
        { RAND_INF_ADULT_TRADES_LH_TRADE_FROG, "ADULT_TRADES_LH_TRADE_FROG" },
        { RAND_INF_ADULT_TRADES_DMT_TRADE_EYEDROPS, "ADULT_TRADES_DMT_TRADE_EYEDROPS" },

        { RAND_INF_KAK_100_GOLD_SKULLTULA_REWARD, "KAK_100_GOLD_SKULLTULA_REWARD" },
        { RAND_INF_GREG_FOUND, "RAND_INF_GREG_FOUND" },

        { RAND_INF_HAS_WALLET, "RAND_INF_HAS_WALLET" },
        
        { RAND_INF_TOT_MASTER_SWORD, "RAND_INF_TOT_MASTER_SWORD"},
        { RAND_INF_CHILD_FISHING, "RAND_INF_CHILD_FISHING" },
        { RAND_INF_ADULT_FISHING, "RAND_INF_ADULT_FISHING" },
        { RAND_INF_10_BIG_POES, "RAND_INF_10_BIG_POES" },
        { RAND_INF_GRANT_GANONS_BOSSKEY, "RAND_INF_GRANT_GANONS_BOSSKEY" },
    } },
};

const std::vector<std::string> state1 = {
    "Loading",
    "Swinging Bottle",
    "Falling from Hookshot",
    "Hookshot/Bow in Hand",
    "Targeting Enemy",
    "Input Disabled",
    "Text on Screen",
    "Death",
    "Starting to put away",
    "Ready to Fire",
    "Get Item",
    "Item over Head",
    "Charging Spin Attack",
    "Hanging off Ledge",
    "Climbing Ledge",
    "Targeting",
    "Target Locked",
    "Targeting Nothing",
    "Jumping Forward",
    "In Freefall",
    "In First-Person View",
    "Climbing Ladder",
    "Shielding",
    "On Horse",
    "Boomerang in Hand",
    "Boomerang Thrown",
    "Damaged",
    "In Water",
    "In Item Cutscene",
    "In Cutscene",
    "30", //Unknown
    "Floor collision disabled"
};

const std::vector<std::string> state2 = {
    "Grab",
    "Speak/Check",
    "Climb",
    "Footstep",
    "Moving Dynapoly",
    "Disabled Rotation on Z target",
    "Disabled Rotation",
    "Grabbed by Enemy",
    "Grabbing Dynapoly",
    "Spawning Dust",
    "Underwater",
    "Diving",
    "Stationary on Ladder",
    "Switch Targeting",
    "Frozen",
    "Pause Most Updating",
    "Enter",
    "Spin Attack w/o Magic",
    "Crawling",
    "Hopping",
    "Navi Out",
    "Navi Alert",
    "Down",
    "Near Ocarina Actor",
    "Attempt playing for Oca. Actor",
    "Playing for Oca. Actor",
    "Reflection",
    "Ocarina Playing",
    "Idling",
    "Disabled draw func",
    "Sword Lunge",
    "Void out"
};

const std::vector<std::string> state3 = {
    "Ignore ceiling for Floor and Water",
    "Midair",
    "Pause Action Func",
    "Finished Attacking",
    "Check Floor Water Collision",
    "Force Pull Ocarina",
    "Restore Nayru's Love",
    "Travelling to Hook Target"
};

class SaveEditorWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};
