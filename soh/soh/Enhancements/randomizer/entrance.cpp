#include "entrance.h"

#include "3drando/fill.hpp"
#include "3drando/pool_functions.hpp"
#include "3drando/item_pool.hpp"
#include "rng.h"
#include "../debugger/performanceTimer.h"
#include "soh/Enhancements/gameconsole.h"
#include "soh/util.h"
#include "z64camera.h"
#include "z64scene.h"
#include "dungeon.h"
#include "ShuffleDoors.h"

#include <spdlog/spdlog.h>

extern "C" {
#include "variables.h"
#include "macros.h"
#include "functions.h"
}

const std::vector<Door> DoorsList = {
    // lobby to scrub hall
    { SCENE_DEKU_TREE, 0, 1, 0, -413, 400, 413, 24576 },
    { SCENE_DEKU_TREE, 1, 0, 0, -493, 400, 493, -8192 },
    // scrub hall to falling block
    { SCENE_DEKU_TREE, 1, 2, 5, -896, 400, 896, 24576 },
    { SCENE_DEKU_TREE, 2, 1, 5, -976, 400, 976, -8192 },
    // lobby to raising platforms
    { SCENE_DEKU_TREE, 0, 10, 6, -500, 800, 0, 16384 },
    { SCENE_DEKU_TREE, 10, 0, 6, -620, 800, 0, -16384 },
    // basement to scrub
    { SCENE_DEKU_TREE, 3, 4, 2, -75, -880, 520, -32768 },
    { SCENE_DEKU_TREE, 4, 3, 2, -75, -880, 640, 0 },
    // scrub to spiked log
    { SCENE_DEKU_TREE, 4, 5, 8, -275, -880, 960, 16384 },
    { SCENE_DEKU_TREE, 5, 4, 8, -395, -880, 960, -16384 },
    // spiked log to two torches
    { SCENE_DEKU_TREE, 5, 6, 1, -1475, -760, 1070, 16384 },
    { SCENE_DEKU_TREE, 6, 5, 1, -1595, -760, 1070, -16384 },
    // two torches to back room
    { SCENE_DEKU_TREE, 6, 7, 7, -1855, -760, 860, 0 },
    { SCENE_DEKU_TREE, 7, 6, 7, -1855, -760, 680, -32768 },
    // back room to petrified room
    { SCENE_DEKU_TREE, 7, 8, 3, -2378, -760, -412, 8192 },
    { SCENE_DEKU_TREE, 8, 7, 3, -2458, -760, -492, -24576 },
    // lobby to inside mouth
    { SCENE_DODONGOS_CAVERN, 0, 7, 7, 0, 76, -1910, 0 },
    { SCENE_DODONGOS_CAVERN, 7, 0, 7, 0, 76, -2030, -32768 },
    // loop to back room
    { SCENE_DODONGOS_CAVERN, 8, 14, 13, 1473, 76, -4170, 0 },
    { SCENE_DODONGOS_CAVERN, 14, 8, 13, 1473, 76, -4290, -32768 },
    // lobby to blue cave
    { SCENE_DODONGOS_CAVERN, 0, 4, 22, 1400, 80, -920, -16384 },
    { SCENE_DODONGOS_CAVERN, 4, 0, 22, 1520, 80, -920, 16384 },
    // blue cave to scrub
    { SCENE_DODONGOS_CAVERN, 4, 13, 14, 2433, 100, -2835, 0 },
    { SCENE_DODONGOS_CAVERN, 13, 4, 14, 2433, 100, -2955, -32768 },
    // lower lizalfos to blue cave
    { SCENE_DODONGOS_CAVERN, 3, 4, 8, 2835, 100, -2404, 10923 },
    { SCENE_DODONGOS_CAVERN, 4, 3, 8, 2735, 100, -2462, -21845 },
    // lower lizalfos to keese hall
    { SCENE_DODONGOS_CAVERN, 3, 1, 9, 3560, 100, -1335, -32768 },
    { SCENE_DODONGOS_CAVERN, 1, 3, 9, 3560, 100, -1215, 0 },
    // baby dodongos to skulltula room
    { SCENE_DODONGOS_CAVERN, 1, 11, 12, 2560, 0, 60, -32768 },
    { SCENE_DODONGOS_CAVERN, 11, 1, 12, 2560, 0, 180, 0 },
    // lobby to stairs
    { SCENE_DODONGOS_CAVERN, 0, 2, 21, -780, 0, -1520, 16384 },
    { SCENE_DODONGOS_CAVERN, 2, 0, 21, -900, 0, -1520, -16384 },
    // stairs to compass
    { SCENE_DODONGOS_CAVERN, 2, 15, 15, -1960, 0, -800, -32768 },
    { SCENE_DODONGOS_CAVERN, 15, 2, 15, -1960, 0, -680, 0 },
    // stairs to armos
    { SCENE_DODONGOS_CAVERN, 2, 5, 17, -1960, 531, -790, -32768 },
    { SCENE_DODONGOS_CAVERN, 5, 2, 17, -1960, 531, -670, 0 },
    // armos to bridge
    { SCENE_DODONGOS_CAVERN, 5, 0, 18, -1605, 531, -320, -16384 },
    { SCENE_DODONGOS_CAVERN, 0, 5, 18, -1485, 531, -320, 16384 },
    // spikes to scrubs
    { SCENE_DODONGOS_CAVERN, 9, 6, 16, 1840, 411, -1420, 0 },
    { SCENE_DODONGOS_CAVERN, 6, 9, 16, 1840, 411, -1540, -32768 },
    // spikes to upper lizalfos
    { SCENE_DODONGOS_CAVERN, 3, 10, 10, 4339, 531, -660, -32768 },
    { SCENE_DODONGOS_CAVERN, 10, 3, 10, 4339, 531, -540, 0 },
    // upper lizalfos to two flames
    { SCENE_DODONGOS_CAVERN, 3, 12, 11, 3840, 531, -920, 16384 },
    { SCENE_DODONGOS_CAVERN, 12, 3, 11, 3720, 531, -920, -16384 },
    // entrance to floating platform
    { SCENE_JABU_JABU, 0, 1, 1, 0, -320, -1103, 0 },
    { SCENE_JABU_JABU, 1, 0, 1, 0, -320, -1151, -32768 },
    // floating platform to holes
    { SCENE_JABU_JABU, 1, 2, 2, 0, -340, -2419, 0 },
    { SCENE_JABU_JABU, 2, 1, 2, 0, -340, -2467, -32768 },
    // basement to kiddy pool
    { SCENE_JABU_JABU, 3, 13, 13, -539, -1113, -2779, 23848 },
    { SCENE_JABU_JABU, 13, 3, 13, -624, -1113, -2705, -8920 },
    // basement to water switch
    { SCENE_JABU_JABU, 3, 14, 12, 400, -1113, -3223, -16384 },
    { SCENE_JABU_JABU, 14, 3, 12, 520, -1113, -3223, 16384 },
    // water switch to floating platform
    { SCENE_JABU_JABU, 14, 1, 14, 370, -1233, -1703, 16384 },
    { SCENE_JABU_JABU, 1, 14, 14, 250, -1233, -1703, -16384 },
    // holes to fork
    { SCENE_JABU_JABU, 2, 7, 4, 0, -340, -3930, 0 },
    { SCENE_JABU_JABU, 7, 2, 4, 0, -340, -4050, -32768 },
    // fork to red tentacle
    { SCENE_JABU_JABU, 7, 10, 10, -1000, -340, -4683, 16384 },
    { SCENE_JABU_JABU, 10, 7, 10, -1120, -340, -4683, -16384 },
    // fork to bubbles
    { SCENE_JABU_JABU, 7, 12, 5, -660, -340, -5120, 0 },
    { SCENE_JABU_JABU, 12, 7, 5, -660, -340, -5240, -32768 },
    // fork to stingers
    { SCENE_JABU_JABU, 7, 9, 11, 1000, -340, -4683, -16384 },
    { SCENE_JABU_JABU, 9, 7, 11, 1120, -340, -4683, 16384 },
    // fork to purple tentacle
    { SCENE_JABU_JABU, 7, 11, 7, 660, -340, -5125, 0 },
    { SCENE_JABU_JABU, 11, 7, 7, 660, -340, -5245, -32768 },
    // fork to green tentacle
    { SCENE_JABU_JABU, 7, 8, 6, 0, -340, -5240, 0 },
    { SCENE_JABU_JABU, 8, 7, 6, 0, -340, -5360, -32768 },
    // basement to big octo
    { SCENE_JABU_JABU, 3, 6, 21, -940, -1015, -3343, 16384 },
    { SCENE_JABU_JABU, 6, 3, 21, -1060, -1015, -3343, -16384 },
    // above octo to red tentacles
    { SCENE_JABU_JABU, 6, 4, 3, -1360, 80, -2345, -32768 },
    { SCENE_JABU_JABU, 4, 6, 3, -1360, 80, -2225, 0 },
    // red tentacles to floating platform
    { SCENE_JABU_JABU, 4, 1, 8, -680, 80, -1703, -16384 },
    { SCENE_JABU_JABU, 1, 4, 8, -560, 80, -1703, 16384 },
    // floating platform to boss door
    { SCENE_JABU_JABU, 1, 5, 9, 560, -320, -1703, -16384 },
    { SCENE_JABU_JABU, 5, 1, 9, 680, -320, -1703, 16384 },
    // entrance to entrance hall
    { SCENE_FOREST_TEMPLE, 0, 1, 0, 119, 359, 82, 0 },
    { SCENE_FOREST_TEMPLE, 1, 0, 0, 119, 359, 38, -32768 },
    // entrance hall to lobby
    { SCENE_FOREST_TEMPLE, 1, 2, 23, 119, 383, -558, 0 },
    { SCENE_FOREST_TEMPLE, 2, 1, 23, 119, 383, -602, -32768 },
    // lobby to puzzle hall
    { SCENE_FOREST_TEMPLE, 2, 5, 14, -739, 383, -1440, 16384 },
    { SCENE_FOREST_TEMPLE, 5, 2, 14, -783, 383, -1440, -16384 },
    // puzzle hall to puzzle
    { SCENE_FOREST_TEMPLE, 5, 11, 2, -1345, 383, -1440, 16384 },
    { SCENE_FOREST_TEMPLE, 11, 5, 2, -1389, 383, -1440, -16384 },
    // puzzle to courtyard
    { SCENE_FOREST_TEMPLE, 8, 11, 4, -1502, 523, -1886, -32768 },
    { SCENE_FOREST_TEMPLE, 11, 8, 4, -1502, 523, -1842, 0 },
    // courtyard to floormaster
    { SCENE_FOREST_TEMPLE, 8, 18, 12, -1717, 523, -2046, 16384 },
    { SCENE_FOREST_TEMPLE, 18, 8, 12, -1773, 523, -2046, -16384 },
    // courtyard to below boss key chest
    { SCENE_FOREST_TEMPLE, 8, 21, 11, -1643, 523, -2766, 0 },
    { SCENE_FOREST_TEMPLE, 21, 8, 11, -1643, 523, -2822, -32768 },
    // courtyard to map hall
    { SCENE_FOREST_TEMPLE, 8, 10, 9, -149, 800, -2810, -16384 },
    { SCENE_FOREST_TEMPLE, 10, 8, 9, -93, 800, -2810, 16384 },
    // map hall to island
    { SCENE_FOREST_TEMPLE, 10, 7, 10, 331, 800, -2810, -16384 },
    { SCENE_FOREST_TEMPLE, 7, 10, 10, 387, 800, -2810, 16384 },
    // courtyard to lobby
    { SCENE_FOREST_TEMPLE, 8, 2, 15, -684, 243, -2243, -24576 },
    { SCENE_FOREST_TEMPLE, 2, 8, 15, -653, 243, -2212, 8192 },
    // island to falling floor
    { SCENE_FOREST_TEMPLE, 7, 15, 3, 1663, 523, -2854, 0 },
    { SCENE_FOREST_TEMPLE, 15, 7, 3, 1663, 523, -2898, -32768 },
    // island to lobby
    { SCENE_FOREST_TEMPLE, 7, 2, 16, 926, 243, -2247, 24758 },
    { SCENE_FOREST_TEMPLE, 2, 7, 16, 887, 243, -2208, -8010 },
    // falling floor to green poe
    { SCENE_FOREST_TEMPLE, 15, 16, 24, 1991, 403, -1848, -32768 },
    { SCENE_FOREST_TEMPLE, 16, 15, 24, 1991, 403, -1792, 0 },
    // green poe to green hall
    { SCENE_FOREST_TEMPLE, 16, 3, 13, 1633, 463, -1440, 16384 },
    { SCENE_FOREST_TEMPLE, 3, 16, 13, 1577, 463, -1440, -16384 },
    // green hall to lobby
    { SCENE_FOREST_TEMPLE, 3, 2, 17, 1021, 463, -1440, 16384 },
    { SCENE_FOREST_TEMPLE, 2, 3, 17, 977, 463, -1440, -16384 },
    // lobby to northern hall
    { SCENE_FOREST_TEMPLE, 2, 4, 1, 119, 467, -2494, 0 },
    { SCENE_FOREST_TEMPLE, 4, 2, 1, 119, 467, -2538, -32768 },
    // northern hall to northern room
    { SCENE_FOREST_TEMPLE, 4, 6, 8, 119, 467, -2982, 0 },
    { SCENE_FOREST_TEMPLE, 6, 4, 8, 119, 467, -3038, -32768 },
    // puzzle to twisted hall
    { SCENE_FOREST_TEMPLE, 11, 19, 25, -1761, 1228, -1792, 0 },
    { SCENE_FOREST_TEMPLE, 19, 11, 25, -1761, 1232, -1848, -32768 },
    // twisted hall to red poe
    { SCENE_FOREST_TEMPLE, 19, 12, 26, -1444, 1228, -3320, -16384 },
    { SCENE_FOREST_TEMPLE, 12, 19, 26, -1388, 1228, -3320, 16384 },
    // red poe to bow chest
    { SCENE_FOREST_TEMPLE, 12, 6, 7, -409, 827, -3320, -16384 },
    { SCENE_FOREST_TEMPLE, 6, 12, 7, -353, 827, -3320, 16384 },
    // bow chest to blue poe
    { SCENE_FOREST_TEMPLE, 6, 13, 6, 471, 827, -3320, -16384 },
    { SCENE_FOREST_TEMPLE, 13, 6, 6, 527, 827, -3320, 16384 },
    // blue poe to twisted room
    { SCENE_FOREST_TEMPLE, 13, 20, 5, 1626, 1228, -3320, -16384 },
    { SCENE_FOREST_TEMPLE, 20, 13, 5, 1682, 1228, -3320, 16384 },
    // twisted room to frozen eye
    { SCENE_FOREST_TEMPLE, 20, 14, 27, 1999, 1228, -1848, -32768 },
    { SCENE_FOREST_TEMPLE, 14, 20, 27, 1999, 1228, -1792, 0 },
    // lobby to boss door
    { SCENE_FIRE_TEMPLE, 0, 2, 16, -398, 200, 0, 16384 },
    { SCENE_FIRE_TEMPLE, 2, 0, 16, -442, 200, 0, -16384 },
    // lobby to boss key
    { SCENE_FIRE_TEMPLE, 0, 17, 17, -280, 0, 162, 0 },
    { SCENE_FIRE_TEMPLE, 17, 0, 17, -280, 0, 118, -32768 },
    // hammer locked door
    { SCENE_FIRE_TEMPLE, 0, 15, 34, 280, 0, 222, 0 },
    { SCENE_FIRE_TEMPLE, 15, 0, 34, 280, 0, 58, -32768 },
    // clear enemies to tile room
    { SCENE_FIRE_TEMPLE, 15, 18, 32, 238, 0, -850, 0 },
    { SCENE_FIRE_TEMPLE, 18, 15, 32, 238, 0, -907, -32768 },
    // tile room to flare dancer
    { SCENE_FIRE_TEMPLE, 18, 3, 29, 21, 0, -1240, 16384 },
    { SCENE_FIRE_TEMPLE, 3, 18, 29, -47, 0, -1240, -16384 },
    // flare dancer to boss key
    { SCENE_FIRE_TEMPLE, 3, 17, 25, -280, 0, -926, -32768 },
    { SCENE_FIRE_TEMPLE, 17, 3, 25, -280, 0, -864, 0 },
    // lobby to bridge
    { SCENE_FIRE_TEMPLE, 0, 1, 36, 398, 200, 0, -16384 },
    { SCENE_FIRE_TEMPLE, 1, 0, 36, 442, 200, 0, 16384 },
    // bridge to goron on left
    { SCENE_FIRE_TEMPLE, 1, 20, 13, 1560, 100, -1538, 0 },
    { SCENE_FIRE_TEMPLE, 20, 1, 13, 1560, 100, -1582, -32768 },
    // bridge to tile room
    { SCENE_FIRE_TEMPLE, 1, 19, 12, 1560, 240, -1498, 0 },
    { SCENE_FIRE_TEMPLE, 19, 1, 12, 1560, 240, -1542, -32768 },
    // bridge to goron on right
    { SCENE_FIRE_TEMPLE, 1, 22, 15, 1560, 200, 1558, -32768 },
    { SCENE_FIRE_TEMPLE, 22, 1, 15, 1560, 200, 1602, 0 },
    // bridge to fire geyser
    { SCENE_FIRE_TEMPLE, 1, 21, 14, 2678, 200, 0, -16384 },
    { SCENE_FIRE_TEMPLE, 21, 1, 14, 2722, 200, 0, 16384 },
    // fire geyser to block
    { SCENE_FIRE_TEMPLE, 21, 4, 6, 3092, 2060, 0, 16384 },
    { SCENE_FIRE_TEMPLE, 4, 21, 6, 3048, 2060, 0, -16384 },
    // block to maze
    { SCENE_FIRE_TEMPLE, 4, 5, 27, 2558, 2800, 0, -16384 },
    { SCENE_FIRE_TEMPLE, 5, 4, 27, 2602, 2800, 0, 16384 },
    // maze to passage above maze
    { SCENE_FIRE_TEMPLE, 5, 7, 22, 1580, 4000, -612, -32768 },
    { SCENE_FIRE_TEMPLE, 7, 5, 22, 1580, 4000, -568, 0 },
    // passage above maze to above maze
    { SCENE_FIRE_TEMPLE, 7, 8, 23, 1580, 4400, -612, -32768 },
    { SCENE_FIRE_TEMPLE, 8, 7, 23, 1580, 4400, -568, 0 },
    // maze to bridge above bridge
    { SCENE_FIRE_TEMPLE, 5, 6, 21, 1550, 2800, -352, -32768 },
    { SCENE_FIRE_TEMPLE, 6, 5, 21, 1550, 2800, -308, 0 },
    // maze to goron
    { SCENE_FIRE_TEMPLE, 5, 23, 18, 1780, 2800, -1478, 0 },
    { SCENE_FIRE_TEMPLE, 23, 5, 18, 1780, 2800, -1522, -32768 },
    // maze to fire chase
    { SCENE_FIRE_TEMPLE, 5, 16, 28, 1482, 2940, -630, 16384 },
    { SCENE_FIRE_TEMPLE, 16, 5, 28, 1438, 2940, -630, -16384 },
    // fire chase cage to bridge above bridge
    { SCENE_FIRE_TEMPLE, 16, 6, 20, 1209, 2800, 60, -16384 },
    { SCENE_FIRE_TEMPLE, 6, 16, 20, 1267, 2800, 60, 16384 },
    // fire chase to bridge above bridge
    { SCENE_FIRE_TEMPLE, 16, 6, 19, 1580, 2800, 352, 0 },
    { SCENE_FIRE_TEMPLE, 6, 16, 19, 1580, 2800, 308, -32768 },
    // fire chase to passage
    { SCENE_FIRE_TEMPLE, 16, 9, 5, 362, 2940, 190, 16384 },
    { SCENE_FIRE_TEMPLE, 9, 16, 5, 318, 2940, 190, -16384 },
    // passage to maze
    { SCENE_FIRE_TEMPLE, 9, 10, 7, -538, 2940, 190, 16384 },
    { SCENE_FIRE_TEMPLE, 10, 9, 7, -582, 2940, 190, -16384 },
    // maze to above cage
    { SCENE_FIRE_TEMPLE, 10, 11, 24, -1404, 2980, 0, 16384 },
    { SCENE_FIRE_TEMPLE, 11, 10, 24, -1484, 2980, 0, -16384 },
    // maze to compass
    { SCENE_FIRE_TEMPLE, 10, 25, 33, -1074, 2800, -890, -8192 },
    { SCENE_FIRE_TEMPLE, 25, 10, 33, -1042, 2800, -922, 24576 },
    // maze to cage by dancer
    { SCENE_FIRE_TEMPLE, 10, 11, 8, -1780, 2800, -352, -32768 },
    { SCENE_FIRE_TEMPLE, 11, 10, 8, -1780, 2800, -308, 0 },
    // maze to cage (locked)
    { SCENE_FIRE_TEMPLE, 10, 11, 9, -1780, 2800, 352, 0 },
    { SCENE_FIRE_TEMPLE, 11, 10, 9, -1780, 2800, 308, -32768 },
    // maze to flare dancer
    { SCENE_FIRE_TEMPLE, 10, 24, 26, -2654, 2840, -166, 24576 },
    { SCENE_FIRE_TEMPLE, 24, 10, 26, -2686, 2840, -134, -8192 },
    // flare dancer to passage to hammer
    { SCENE_FIRE_TEMPLE, 24, 12, 11, -2720, 4000, 159, -16384 },
    { SCENE_FIRE_TEMPLE, 12, 24, 11, -2676, 4000, 159, 16384 },
    // passage to hammer to hammer
    { SCENE_FIRE_TEMPLE, 12, 13, 10, -2392, 4400, 20, -16384 },
    { SCENE_FIRE_TEMPLE, 13, 12, 10, -2348, 4400, 20, 16384 },
    // hammer to hammer stairs
    { SCENE_FIRE_TEMPLE, 13, 14, 31, -2378, 4180, -710, -8192 },
    { SCENE_FIRE_TEMPLE, 14, 13, 31, -2266, 4180, -820, 24576 },
    // hammer stairs to maze
    { SCENE_FIRE_TEMPLE, 14, 10, 30, -1202, 3680, -211, -24394 },
    { SCENE_FIRE_TEMPLE, 10, 14, 30, -1168, 3680, -178, 8374 },
    // lobby to boss door
    { SCENE_WATER_TEMPLE, 0, 11, 16, -180, 820, -1218, 0 },
    { SCENE_WATER_TEMPLE, 11, 0, 16, -180, 820, -1262, -32768 },
    // lobby to waterfall
    { SCENE_WATER_TEMPLE, 0, 5, 8, -818, 780, -180, 16384 },
    { SCENE_WATER_TEMPLE, 5, 0, 8, -862, 780, -180, -16384 },
    // waterfall to red crystal
    { SCENE_WATER_TEMPLE, 5, 6, 10, -2778, 760, -180, 16384 },
    { SCENE_WATER_TEMPLE, 6, 5, 10, -2822, 760, -180, -16384 },
    // red crystal to dark link
    { SCENE_WATER_TEMPLE, 6, 13, 18, -3120, 1060, -1632, 0 },
    { SCENE_WATER_TEMPLE, 13, 6, 18, -3120, 1060, -1688, -32768 },
    // dark link to tunnel
    { SCENE_WATER_TEMPLE, 13, 7, 19, -3120, 1060, -3672, 0 },
    { SCENE_WATER_TEMPLE, 7, 13, 19, -3120, 1060, -3728, -32768 },
    // whirlpool to lobby
    { SCENE_WATER_TEMPLE, 8, 9, 20, -1140, 60, -1752, -32768 },
    { SCENE_WATER_TEMPLE, 9, 8, 20, -1140, 60, -1708, 0 },
    // lobby to back rooms
    { SCENE_WATER_TEMPLE, 0, 12, 9, -180, 0, -1768, 0 },
    { SCENE_WATER_TEMPLE, 12, 0, 9, -180, 0, -1812, -32768 },
    // back rooms to boss key
    { SCENE_WATER_TEMPLE, 12, 16, 21, -1280, 108, -2668, 0 },
    { SCENE_WATER_TEMPLE, 16, 12, 21, -1280, 108, -2712, -32768 },
    // back rooms to back block
    { SCENE_WATER_TEMPLE, 12, 14, 11, -180, 0, -3148, 0 },
    { SCENE_WATER_TEMPLE, 14, 12, 11, -180, 0, -3192, -32768 },
    // back block to geyser room
    { SCENE_WATER_TEMPLE, 14, 15, 12, -652, 120, -3670, 16384 },
    { SCENE_WATER_TEMPLE, 15, 14, 12, -708, 120, -3670, -16384 },
    // geyser room to back rooms
    { SCENE_WATER_TEMPLE, 15, 12, 22, -880, 120, -2718, -32768 },
    { SCENE_WATER_TEMPLE, 12, 15, 22, -880, 120, -2662, 0 },
    // top of drain to map
    { SCENE_WATER_TEMPLE, 17, 19, 13, 1142, 780, 180, -16384 },
    { SCENE_WATER_TEMPLE, 19, 17, 13, 1198, 780, 180, 16384 },
    // bottom of drain to bottom room
    { SCENE_WATER_TEMPLE, 17, 18, 23, 1142, 0, 180, -16384 },
    { SCENE_WATER_TEMPLE, 18, 17, 23, 1198, 0, 180, 16384 },
    // lobby to bottom of tower
    { SCENE_WATER_TEMPLE, 0, 1, 14, -180, 80, 42, 0 },
    { SCENE_WATER_TEMPLE, 1, 0, 14, -180, 80, -2, -32768 },
    // lobby to top of tower
    { SCENE_WATER_TEMPLE, 0, 1, 15, -180, 460, -408, -32768 },
    { SCENE_WATER_TEMPLE, 1, 0, 15, -180, 460, -352, 0 },
    // lobby to lift
    { SCENE_WATER_TEMPLE, 0, 10, 17, -818, 460, -180, 16384 },
    { SCENE_WATER_TEMPLE, 10, 0, 17, -862, 460, -180, -16384 },
    // lift to top ledge
    // TODO one way door, need to figure out coordinates for this
    // DOOR 10 0 (-878,860,287)
    // lobby to catacombs
    { SCENE_SHADOW_TEMPLE, 2, 0, 2, -72, -63, -185, 16384 },
    { SCENE_SHADOW_TEMPLE, 0, 2, 2, -116, -63, -185, -16384 },
    // catacombs to map
    { SCENE_SHADOW_TEMPLE, 0, 1, 1, -794, -63, -657, 0 },
    { SCENE_SHADOW_TEMPLE, 1, 0, 1, -794, -63, -713, -32768 },
    // catacombs to dead hand
    { SCENE_SHADOW_TEMPLE, 0, 4, 0, -2026, -63, -405, 16384 },
    { SCENE_SHADOW_TEMPLE, 4, 0, 0, -2082, -63, -405, -16384 },
    // lobby to compass
    { SCENE_SHADOW_TEMPLE, 5, 7, 6, 3358, -543, 508, -32768 },
    { SCENE_SHADOW_TEMPLE, 7, 5, 6, 3358, -543, 564, 0 },
    // lobby to spinning blades
    { SCENE_SHADOW_TEMPLE, 5, 6, 5, 3358, -543, -404, 0 },
    { SCENE_SHADOW_TEMPLE, 6, 5, 5, 3358, -543, -448, -32768 },
    // lobby to depths
    { SCENE_SHADOW_TEMPLE, 5, 8, 4, 3817, -543, 55, -16384 },
    { SCENE_SHADOW_TEMPLE, 8, 5, 4, 3861, -543, 55, 16384 },
    // depths to invisible blades
    { SCENE_SHADOW_TEMPLE, 9, 16, 7, 4678, -1143, 2577, 0 },
    { SCENE_SHADOW_TEMPLE, 16, 9, 7, 4678, -1143, 2533, -32768 },
    // depths to silver room
    { SCENE_SHADOW_TEMPLE, 9, 11, 8, 2478, -1343, 1496, 0 },
    { SCENE_SHADOW_TEMPLE, 11, 9, 8, 2478, -1343, 1452, -32768 },
    // silver room to blue skull
    { SCENE_SHADOW_TEMPLE, 11, 14, 10, 2006, -1343, 1084, 16384 },
    { SCENE_SHADOW_TEMPLE, 14, 11, 10, 1950, -1343, 1084, -16384 },
    // silver room to wind tunnel
    { SCENE_SHADOW_TEMPLE, 11, 18, 9, 2478, -1203, 816, 0 },
    { SCENE_SHADOW_TEMPLE, 18, 11, 9, 2478, -1203, 772, -32768 },
    // wind tunnel to hint
    { SCENE_SHADOW_TEMPLE, 18, 19, 16, 4700, -1363, -126, -16384 },
    { SCENE_SHADOW_TEMPLE, 19, 18, 16, 4756, -1363, -126, 16384 },
    // wind tunnel to double gibdo
    { SCENE_SHADOW_TEMPLE, 18, 20, 15, 4388, -1363, -617, 0 },
    { SCENE_SHADOW_TEMPLE, 20, 18, 15, 4388, -1363, -674, -32768 },
    // double gibdo to boat
    { SCENE_SHADOW_TEMPLE, 20, 21, 17, 4388, -1363, -1384, 0 },
    { SCENE_SHADOW_TEMPLE, 21, 20, 17, 4388, -1363, -1428, -32768 },
    // boat to boss door
    { SCENE_SHADOW_TEMPLE, 21, 3, 3, -2742, -1363, -258, -32768 },
    { SCENE_SHADOW_TEMPLE, 3, 21, 3, -2742, -1363, -214, 0 },
    // boat to maze
    { SCENE_SHADOW_TEMPLE, 21, 15, 14, -3560, -1363, -1586, 16384 },
    { SCENE_SHADOW_TEMPLE, 15, 21, 14, -3604, -1363, -1586, -16384 },
    // maze to x-cross
    { SCENE_SHADOW_TEMPLE, 15, 17, 13, -4306, -1363, -890, -32768 },
    { SCENE_SHADOW_TEMPLE, 17, 15, 13, -4306, -1363, -834, 0 },
    // maze to spinning skulls
    { SCENE_SHADOW_TEMPLE, 15, 12, 11, -5009, -1363, -1586, 16384 },
    { SCENE_SHADOW_TEMPLE, 12, 15, 11, -5053, -1363, -1586, -16384 },
    // maze to wooden spikes
    { SCENE_SHADOW_TEMPLE, 15, 13, 12, -4306, -1363, -2289, 0 },
    { SCENE_SHADOW_TEMPLE, 13, 15, 12, -4306, -1363, -2333, -32768 },
    // lobby to compass
    { SCENE_SPIRIT_TEMPLE, 0, 14, 5, 772, 30, -545, 10923 },
    { SCENE_SPIRIT_TEMPLE, 14, 0, 5, 724, 30, -573, -21845 },
    // lobby to boulders
    { SCENE_SPIRIT_TEMPLE, 0, 13, 4, 1346, 30, -544, -10923 },
    { SCENE_SPIRIT_TEMPLE, 13, 0, 4, 1395, 30, -572, 21845 },
    // boulders to chest
    { SCENE_SPIRIT_TEMPLE, 13, 12, 28, 1569, -130, -1575, 0 },
    { SCENE_SPIRIT_TEMPLE, 12, 13, 28, 1569, -130, -1631, -32768 },
    // lobby to climb
    { SCENE_SPIRIT_TEMPLE, 0, 15, 3, 1060, 0, -558, 0 },
    { SCENE_SPIRIT_TEMPLE, 15, 0, 3, 1060, 0, -614, -32768 },
    // climb to statue
    { SCENE_SPIRIT_TEMPLE, 15, 5, 2, 808, 480, -850, 16384 },
    { SCENE_SPIRIT_TEMPLE, 5, 15, 2, 752, 480, -850, -16384 },
    // statue to beamos passage
    { SCENE_SPIRIT_TEMPLE, 5, 16, 20, 700, 813, -598, -32768 },
    { SCENE_SPIRIT_TEMPLE, 16, 5, 20, 700, 813, -542, 0 },
    // beamos statue to triple anubis
    { SCENE_SPIRIT_TEMPLE, 16, 17, 19, 1032, 843, -210, -16384 },
    { SCENE_SPIRIT_TEMPLE, 17, 16, 19, 1088, 843, -210, 16384 },
    // triple anubis to sliding walls
    { SCENE_SPIRIT_TEMPLE, 17, 23, 14, 1088, 843, 150, 16384 },
    { SCENE_SPIRIT_TEMPLE, 23, 17, 14, 1032, 843, 150, -16384 },
    // triple anubis to armos puzzle
    { SCENE_SPIRIT_TEMPLE, 17, 18, 18, 1672, 843, 150, -16384 },
    { SCENE_SPIRIT_TEMPLE, 18, 17, 18, 1728, 843, 150, 16384 },
    // armos puzzle to mirror chest
    { SCENE_SPIRIT_TEMPLE, 18, 21, 15, 1890, 843, -382, 0 },
    { SCENE_SPIRIT_TEMPLE, 21, 18, 15, 1890, 843, -438, -32768 },
    // armos puzzle to stairs
    { SCENE_SPIRIT_TEMPLE, 18, 19, 17, 1960, 843, 442, -32768 },
    { SCENE_SPIRIT_TEMPLE, 19, 18, 17, 1960, 843, 498, 0 },
    // stairs to iron knuckle
    { SCENE_SPIRIT_TEMPLE, 20, 19, 16, 1960, 1003, 1202, 0 },
    { SCENE_SPIRIT_TEMPLE, 19, 20, 16, 1960, 1003, 1146, -32768 },
    // iron knuckle to mirror shield
    { SCENE_SPIRIT_TEMPLE, 20, 11, 6, 1348, 1003, 1474, 16384 },
    { SCENE_SPIRIT_TEMPLE, 11, 20, 6, 1292, 1003, 1474, -16384 },
    // sliding walls to lullaby door
    { SCENE_SPIRIT_TEMPLE, 23, 24, 13, 680, 1543, -419, 0 },
    { SCENE_SPIRIT_TEMPLE, 24, 23, 13, 680, 1543, -475, -32768 },
    // lullaby door to boss key
    { SCENE_SPIRIT_TEMPLE, 24, 22, 12, 680, 1633, -882, 0 },
    { SCENE_SPIRIT_TEMPLE, 22, 24, 12, 680, 1633, -938, -32768 },
    // lullaby door to light puzzle
    { SCENE_SPIRIT_TEMPLE, 24, 25, 0, 348, 1733, -830, 16384 },
    { SCENE_SPIRIT_TEMPLE, 25, 24, 0, 292, 1733, -830, -16384 },
    // light puzzle to light puzzle
    { SCENE_SPIRIT_TEMPLE, 25, 26, 1, -172, 1733, -830, 16384 },
    { SCENE_SPIRIT_TEMPLE, 26, 25, 1, -228, 1733, -830, -16384 },
    // child to child left
    { SCENE_SPIRIT_TEMPLE, 1, 3, 10, -1344, 30, -543, 10923 },
    { SCENE_SPIRIT_TEMPLE, 3, 1, 10, -1398, 30, -574, -21845 },
    // child to child anubis
    { SCENE_SPIRIT_TEMPLE, 3, 27, 26, -1452, 30, -1310, -8192 },
    { SCENE_SPIRIT_TEMPLE, 27, 3, 26, -1418, 30, -1344, 24576 },
    // child anubis to child right
    { SCENE_SPIRIT_TEMPLE, 27, 2, 27, -835, 30, -1479, -24758 },
    { SCENE_SPIRIT_TEMPLE, 2, 27, 27, -802, 30, -1445, 8010 },
    // child right to child
    { SCENE_SPIRIT_TEMPLE, 2, 1, 11, -722, 30, -574, 21845 },
    { SCENE_SPIRIT_TEMPLE, 1, 2, 11, -776, 30, -543, -10923 },
    // child to child climb
    { SCENE_SPIRIT_TEMPLE, 1, 4, 9, -1060, 40, -1083, 0 },
    { SCENE_SPIRIT_TEMPLE, 4, 1, 9, -1060, 40, -1134, -32768 },
    // child climb to statue
    { SCENE_SPIRIT_TEMPLE, 4, 5, 24, -684, 480, -850, -16384 },
    { SCENE_SPIRIT_TEMPLE, 5, 4, 24, -636, 480, -850, 16384 },
    // statue to child stairs
    { SCENE_SPIRIT_TEMPLE, 5, 7, 25, -580, 813, -598, -32768 },
    { SCENE_SPIRIT_TEMPLE, 7, 5, 25, -580, 813, -542, 0 },
    // child round stairs to child silver
    { SCENE_SPIRIT_TEMPLE, 7, 8, 22, -912, 843, -210, 16384 },
    { SCENE_SPIRIT_TEMPLE, 8, 7, 22, -968, 843, -210, -16384 },
    // child silver to child straight stairs
    { SCENE_SPIRIT_TEMPLE, 8, 9, 8, -1700, 843, 442, -32768 },
    { SCENE_SPIRIT_TEMPLE, 9, 8, 8, -1700, 843, 498, 0 },
    // child straight stairs to iron knuckle
    { SCENE_SPIRIT_TEMPLE, 9, 10, 21, -1700, 1003, 1146, -32768 },
    { SCENE_SPIRIT_TEMPLE, 10, 9, 21, -1700, 1003, 1202, 0 },
    // iron knuckle to silver gauntlets
    { SCENE_SPIRIT_TEMPLE, 10, 11, 7, -1088, 1003, 1474, -16384 },
    { SCENE_SPIRIT_TEMPLE, 11, 10, 7, -1032, 1003, 1474, 16384 },
    // statue to shortcut
    { SCENE_SPIRIT_TEMPLE, 5, 6, 23, 60, 413, -678, -32768 },
    { SCENE_SPIRIT_TEMPLE, 6, 5, 23, 60, 413, -622, 0 },
    // x-cross to eastern room
    { SCENE_BOTTOM_OF_THE_WELL, 0, 5, 3, 450, 0, -878, 0 },
    { SCENE_BOTTOM_OF_THE_WELL, 5, 0, 3, 450, 0, -922, -32768 },
    // x-cross to western room
    { SCENE_BOTTOM_OF_THE_WELL, 0, 6, 4, -410, 0, -878, 0 },
    { SCENE_BOTTOM_OF_THE_WELL, 6, 0, 4, -410, 0, -922, -32768 },
    // perimeter to coffins
    { SCENE_BOTTOM_OF_THE_WELL, 0, 2, 5, -1548, 0, -740, 16384 },
    { SCENE_BOTTOM_OF_THE_WELL, 2, 0, 5, -1592, 0, -740, -16384 },
    // bottom to perimeter TODO one way door
    // DOOR 1 0 (-890,0,-124)
    // lower crawlspace to deadhand
    { SCENE_BOTTOM_OF_THE_WELL, 0, 4, 6, 876, -20, 210, -16384 },
    { SCENE_BOTTOM_OF_THE_WELL, 4, 0, 6, 924, -20, 210, 16384 },
    // crawlspace to four pits
    { SCENE_BOTTOM_OF_THE_WELL, 0, 3, 2, 1140, 0, -1442, -32768 },
    { SCENE_BOTTOM_OF_THE_WELL, 3, 0, 2, 1140, 0, -1398, 0 },
    // four pits to like like cage
    { SCENE_BOTTOM_OF_THE_WELL, 3, 0, 1, 870, 0, -882, -32768 },
    { SCENE_BOTTOM_OF_THE_WELL, 0, 3, 1, 870, 0, -838, 0 },
    { SCENE_ICE_CAVERN, 6, 7, 11, -1360, 280, 709, -24576 },
    { SCENE_ICE_CAVERN, 7, 6, 11, -1320, 279, 749, 8192 },
    { SCENE_ICE_CAVERN, 7, 10, 10, -886, 80, 1196, -24394 },
    { SCENE_ICE_CAVERN, 10, 7, 10, -846, 80, 1235, 8374 },
    // lobby to door maze
    { SCENE_GERUDO_TRAINING_GROUND, 0, 8, 4, -60, -80, -1072, 0 },
    { SCENE_GERUDO_TRAINING_GROUND, 8, 0, 4, -60, -80, -1128, -32768 },
    // lobby to sand
    { SCENE_GERUDO_TRAINING_GROUND, 0, 1, 6, -952, -80, -180, 16384 },
    { SCENE_GERUDO_TRAINING_GROUND, 1, 0, 6, -1008, -80, -180, -16384 },
    // sand to fire maze
    { SCENE_GERUDO_TRAINING_GROUND, 1, 2, 1, -1580, -80, -592, 0 },
    { SCENE_GERUDO_TRAINING_GROUND, 2, 1, 1, -1580, -80, -648, -32768 },
    // fire maze to big block
    { SCENE_GERUDO_TRAINING_GROUND, 2, 3, 2, -1579, 160, -2139, 0 },
    { SCENE_GERUDO_TRAINING_GROUND, 3, 2, 2, -1579, 160, -2195, -32768 },
    // big block to pits
    { SCENE_GERUDO_TRAINING_GROUND, 3, 10, 3, -1577, 159, -3469, 0 },
    { SCENE_GERUDO_TRAINING_GROUND, 10, 3, 3, -1577, 159, -3525, -32768 },
    // big block to eyes
    { SCENE_GERUDO_TRAINING_GROUND, 3, 4, 7, -805, 239, -2747, -16384 },
    { SCENE_GERUDO_TRAINING_GROUND, 4, 3, 7, -749, 239, -2747, 16384 },
    // eyes to above maze
    { SCENE_GERUDO_TRAINING_GROUND, 4, 8, 12, -78, 239, -2088, -32768 },
    { SCENE_GERUDO_TRAINING_GROUND, 8, 4, 12, -78, 239, -2031, 0 },
    // eyes to hammer room
    { SCENE_GERUDO_TRAINING_GROUND, 4, 5, 8, 795, -81, -2748, -16384 },
    { SCENE_GERUDO_TRAINING_GROUND, 5, 4, 8, 851, -81, -2748, 16384 },
    // hammer room to lava
    { SCENE_GERUDO_TRAINING_GROUND, 5, 6, 9, 1443, -81, -2436, -32768 },
    { SCENE_GERUDO_TRAINING_GROUND, 6, 5, 9, 1443, -81, -2380, 0 },
    // lava to water
    { SCENE_GERUDO_TRAINING_GROUND, 6, 9, 10, 1894, -240, -1464, -16384 },
    { SCENE_GERUDO_TRAINING_GROUND, 9, 6, 10, 1950, -240, -1464, 16384 },
    // lava to ring of fire
    { SCENE_GERUDO_TRAINING_GROUND, 6, 7, 11, 1480, -79, -628, -32768 },
    { SCENE_GERUDO_TRAINING_GROUND, 7, 6, 11, 1480, -80, -572, 0 },
    // ring of fire to lobby
    { SCENE_GERUDO_TRAINING_GROUND, 7, 0, 5, 888, -80, -180, 16384 },
    { SCENE_GERUDO_TRAINING_GROUND, 0, 7, 5, 832, -80, -180, -16384 },
    // entrance to lobby
    { SCENE_INSIDE_GANONS_CASTLE, 0, 1, 2, 0, 150, 621, 0 },
    { SCENE_INSIDE_GANONS_CASTLE, 1, 0, 2, 0, 150, 565, -32768 },
    // lobby to spirit
    { SCENE_INSIDE_GANONS_CASTLE, 1, 17, 16, -602, 150, 203, 27307 },
    { SCENE_INSIDE_GANONS_CASTLE, 17, 1, 16, -630, 150, 251, -5461 },
    // spirit to spirit slug
    { SCENE_INSIDE_GANONS_CASTLE, 17, 18, 17, -1220, 150, 710, 10923 },
    { SCENE_INSIDE_GANONS_CASTLE, 18, 17, 17, -1277, 150, 677, -21845 },
    // spirit slug to spirit core
    { SCENE_INSIDE_GANONS_CASTLE, 18, 19, 18, -1281, 150, 1379, 27307 },
    { SCENE_INSIDE_GANONS_CASTLE, 19, 18, 18, -1310, 150, 1429, -5461 },
    // lobby to light
    { SCENE_INSIDE_GANONS_CASTLE, 1, 9, 10, -1291, -240, -840, 16384 },
    { SCENE_INSIDE_GANONS_CASTLE, 9, 1, 10, -1347, -240, -840, -16384 },
    // light to light boulders
    { SCENE_INSIDE_GANONS_CASTLE, 9, 8, 19, -2229, -240, -840, 16384 },
    { SCENE_INSIDE_GANONS_CASTLE, 8, 9, 19, -2285, -240, -840, -16384 },
    // light boulders to light core
    { SCENE_INSIDE_GANONS_CASTLE, 8, 10, 5, -2891, -246, -840, 16384 },
    { SCENE_INSIDE_GANONS_CASTLE, 10, 8, 5, -2947, -240, -840, -16384 },
    // lobby to fire
    { SCENE_INSIDE_GANONS_CASTLE, 1, 14, 11, -602, 150, -1882, 5461 },
    { SCENE_INSIDE_GANONS_CASTLE, 14, 1, 11, -630, 150, -1931, -27307 },
    // fire to fire core
    { SCENE_INSIDE_GANONS_CASTLE, 14, 15, 15, -1938, 153, -3837, 5461 },
    { SCENE_INSIDE_GANONS_CASTLE, 15, 14, 15, -1965, 153, -3884, -27307 },
    // lobby to shadow
    { SCENE_INSIDE_GANONS_CASTLE, 12, 1, 12, 630, 150, -1931, 27307 },
    { SCENE_INSIDE_GANONS_CASTLE, 1, 12, 12, 602, 150, -1883, -5461 },
    // shadow to shadow core
    { SCENE_INSIDE_GANONS_CASTLE, 12, 13, 14, 2102, 210, -4481, -5461 },
    { SCENE_INSIDE_GANONS_CASTLE, 13, 12, 14, 2130, 210, -4529, 27307 },
    // lobby to water
    { SCENE_INSIDE_GANONS_CASTLE, 1, 2, 9, 1204, -240, -840, -16384 },
    { SCENE_INSIDE_GANONS_CASTLE, 2, 1, 9, 1260, -240, -840, 16384 },
    // water to water puzzle
    { SCENE_INSIDE_GANONS_CASTLE, 2, 3, 4, 2324, -248, -840, -16384 },
    { SCENE_INSIDE_GANONS_CASTLE, 3, 2, 4, 2380, -250, -840, 16384 },
    // water puzzle to water core
    { SCENE_INSIDE_GANONS_CASTLE, 3, 4, 13, 3464, -240, -840, -16384 },
    { SCENE_INSIDE_GANONS_CASTLE, 4, 3, 13, 3520, -240, -840, 16384 },
    // lobby to forest
    { SCENE_INSIDE_GANONS_CASTLE, 1, 5, 8, 602, 150, 203, -27307 },
    { SCENE_INSIDE_GANONS_CASTLE, 5, 1, 8, 630, 150, 251, 5461 },
    // forest to forest wind
    { SCENE_INSIDE_GANONS_CASTLE, 5, 6, 6, 1145, 110, 1142, -27307 },
    { SCENE_INSIDE_GANONS_CASTLE, 6, 5, 6, 1172, 110, 1189, 5461 },
    // forest wind to forest core
    { SCENE_INSIDE_GANONS_CASTLE, 6, 7, 7, 1765, 110, 2217, -27307 },
    { SCENE_INSIDE_GANONS_CASTLE, 7, 6, 7, 1793, 110, 2265, 5461 },
    // first floor to second floor
    { SCENE_GANONS_TOWER, 7, 0, 3, -428, 0, 0, -16384 },
    { SCENE_GANONS_TOWER, 0, 7, 3, -372, 0, 0, 16384 },
    // second floor to second floor stairs
    { SCENE_GANONS_TOWER, 0, 1, 2, 477, 40, 0, -16384 },
    { SCENE_GANONS_TOWER, 1, 0, 2, 534, 40, 0, 16384 },
    // second floor stairs to third floor
    { SCENE_GANONS_TOWER, 1, 2, 4, 0, 260, -428, -32768 },
    { SCENE_GANONS_TOWER, 2, 1, 4, 0, 260, -372, 0 },
    // third floor to third floor stairs
    { SCENE_GANONS_TOWER, 2, 3, 0, 0, 300, 478, -32768 },
    { SCENE_GANONS_TOWER, 3, 2, 0, 0, 300, 534, 0 },
    // third floor stairs to fourth floor
    { SCENE_GANONS_TOWER, 3, 4, 5, 428, 520, 0, 16384 },
    { SCENE_GANONS_TOWER, 4, 3, 5, 372, 520, 0, -16384 },
    // fourth floor to fourth floor stairs
    { SCENE_GANONS_TOWER, 4, 5, 1, -478, 560, 0, 16384 },
    { SCENE_GANONS_TOWER, 5, 4, 1, -534, 560, 0, -16384 },
    // fourth floor stairs to pots
    // TODO can't go back in vanilla, get other coords in rando
    // DOOR 5 8 (-1,800,528)
    // pots to fifth floor stairs
    // DOOR 8 6 (472,800,0)
};

namespace Rando {
EntranceLinkInfo NO_RETURN_ENTRANCE = { EntranceType::None, RR_NONE, RR_NONE, -1 };

Entrance::Entrance(RandomizerRegion connectedRegion_, ConditionFn condition_function_, std::string condition_str_,
                   bool spreadsAreasWithPriority_)
    : connectedRegion(connectedRegion_), condition_function(condition_function_), condition_str(condition_str_),
      spreadsAreasWithPriority(spreadsAreasWithPriority_) {
    originalConnectedRegion = connectedRegion_;
}

void Entrance::SetCondition(ConditionFn newCondition) {
    condition_function = newCondition;
}

bool Entrance::GetConditionsMet() const {
    auto ctx = Rando::Context::GetInstance();
    if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS)) {
        return condition_function();
    }
    return true;
}

std::string Entrance::to_string() const {
    return RegionTable(parentRegion)->regionName + " -> " + RegionTable(connectedRegion)->regionName;
}

void Entrance::SetName(std::string name_) {
    if (name_ == "") {
        name = RegionTable(parentRegion)->regionName + " -> " + RegionTable(connectedRegion)->regionName;
    } else {
        name = std::move(name_);
    }
}

std::string Entrance::GetName() const {
    return name;
}

void Entrance::printAgeTimeAccess() {
    auto message = "Child Day:   " + std::to_string(CheckConditionAtAgeTime(logic->IsChild, logic->AtDay)) +
                   "\t"
                   "Child Night: " +
                   std::to_string(CheckConditionAtAgeTime(logic->IsChild, logic->AtNight)) +
                   "\t"
                   "Adult Day:   " +
                   std::to_string(CheckConditionAtAgeTime(logic->IsAdult, logic->AtDay)) +
                   "\t"
                   "Adult Night: " +
                   std::to_string(CheckConditionAtAgeTime(logic->IsAdult, logic->AtNight));
}

bool Entrance::ConditionsMet(bool allAgeTimes) const {
    auto ctx = Rando::Context::GetInstance();
    StartPerformanceTimer(PT_ENTRANCE_LOGIC);
    Region* parent = RegionTable(parentRegion);
    int conditionsMet = 0;

    if (allAgeTimes && !parent->AllAccess()) {
        StopPerformanceTimer(PT_ENTRANCE_LOGIC);
        return false;
    }

    // check all possible day/night condition combinations
    conditionsMet = (parent->childDay && CheckConditionAtAgeTime(logic->IsChild, logic->AtDay, allAgeTimes)) +
                    (parent->childNight && CheckConditionAtAgeTime(logic->IsChild, logic->AtNight, allAgeTimes)) +
                    (parent->adultDay && CheckConditionAtAgeTime(logic->IsAdult, logic->AtDay, allAgeTimes)) +
                    (parent->adultNight && CheckConditionAtAgeTime(logic->IsAdult, logic->AtNight, allAgeTimes));

    StopPerformanceTimer(PT_ENTRANCE_LOGIC);
    return conditionsMet && (!allAgeTimes || conditionsMet == 4);
}

// set the logic to be a specific age and time of day and see if the condition still holds
bool Entrance::CheckConditionAtAgeTime(bool& age, bool& time, bool passAnyway) const {

    logic->IsChild = false;
    logic->IsAdult = false;
    logic->AtDay = false;
    logic->AtNight = false;

    time = true;
    age = true;

    return GetConditionsMet() && (connectedRegion != RR_NONE || passAnyway);
}

RandomizerRegion Entrance::GetConnectedRegionKey() const {
    return connectedRegion;
}

RandomizerRegion Entrance::GetOriginalConnectedRegionKey() const {
    return originalConnectedRegion;
}

Region* Entrance::GetConnectedRegion() const {
    return RegionTable(connectedRegion);
}

void Entrance::SetParentRegion(RandomizerRegion newParent) {
    parentRegion = newParent;
}

RandomizerRegion Entrance::GetParentRegionKey() const {
    return parentRegion;
}

Region* Entrance::GetParentRegion() const {
    return RegionTable(parentRegion);
}

void Entrance::SetNewEntrance(RandomizerRegion newRegion) {
    connectedRegion = newRegion;
}

void Entrance::SetAsShuffled() {
    shuffled = true;
}

bool Entrance::IsShuffled() const {
    return shuffled;
}

bool Entrance::IsAddedToPool() const {
    return addedToPool;
}

void Entrance::AddToPool() {
    addedToPool = true;
}

void Entrance::RemoveFromPool() {
    addedToPool = false;
}

void Entrance::SetAsPrimary() {
    primary = true;
}

bool Entrance::IsPrimary() const {
    return primary;
}

bool Entrance::IsDecoupled() const {
    return decoupled;
}

void Entrance::SetDecoupled() {
    decoupled = true;
}

int16_t Entrance::GetIndex() const {
    return index;
}

void Entrance::SetIndex(int16_t newIndex) {
    index = newIndex;
}

Entrance* Entrance::GetAssumed() const {
    return assumed;
}

void Entrance::SetReplacement(Entrance* newReplacement) {
    replacement = newReplacement;
}

Entrance* Entrance::GetReplacement() const {
    return replacement;
}

EntranceType Entrance::GetType() const {
    return type;
}

void Entrance::SetType(EntranceType newType) {
    type = newType;
}

Entrance* Entrance::GetReverse() const {
    return reverse;
}

void Entrance::Connect(RandomizerRegion newConnectedRegion) {
    connectedRegion = newConnectedRegion;
    RegionTable(newConnectedRegion)->entrances.push_front(this);
}

RandomizerRegion Entrance::Disconnect() {
    RegionTable(connectedRegion)->entrances.remove_if([this](const auto entrance) { return this == entrance; });
    RandomizerRegion previouslyConnected = connectedRegion;
    connectedRegion = RR_NONE;
    return previouslyConnected;
}

void Entrance::BindTwoWay(Entrance* otherEntrance) {
    reverse = otherEntrance;
    otherEntrance->reverse = this;
}

Entrance* Entrance::GetNewTarget() {
    RegionTable(RR_ROOT)->AddExit(
        RR_ROOT, connectedRegion, [] { return true; }, "true");
    Entrance* targetEntrance = RegionTable(RR_ROOT)->GetExit(connectedRegion);
    targetEntrance->SetReplacement(this);
    targetEntrance->SetName(RegionTable(RR_ROOT)->regionName + " -> " + GetConnectedRegion()->regionName);
    return targetEntrance;
}

Entrance* Entrance::AssumeReachable() {
    if (assumed == nullptr) {
        assumed = GetNewTarget();
        Disconnect();
    }
    return assumed;
}

bool Entrance::DoesSpreadAreas() {
    return spreadsAreasWithPriority;
}

const std::string& Entrance::GetConditionStr() const {
    return condition_str;
}

EntranceShuffler::EntranceShuffler() {
    playthroughEntrances = {};
    entranceOverrides = {};
}

bool EntranceShuffler::HasNoRandomEntrances() {
    return mNoRandomEntrances;
}

void EntranceShuffler::SetNoRandomEntrances(bool noRandomEntrances) {
    mNoRandomEntrances = noRandomEntrances;
}

// Construct entrance name from parent and connected region keys
std::string EntranceNameByRegions(RandomizerRegion parentRegion, RandomizerRegion connectedRegion) {
    return RegionTable(parentRegion)->regionName + " -> " + RegionTable(connectedRegion)->regionName;
}

std::unordered_map<int16_t, Entrance*> entranceMap;

void SetAllEntrancesData() {
    std::vector<EntranceInfoPair> entranceShuffleTable = {
        // clang-format off
        // Type                         Parent Region                        Connected Region                      Index
        { { EntranceType::Dungeon,      RR_KF_OUTSIDE_DEKU_TREE,             RR_DEKU_TREE_ENTRYWAY,                ENTR_DEKU_TREE_ENTRANCE },
          { EntranceType::Dungeon,      RR_DEKU_TREE_ENTRYWAY,               RR_KF_OUTSIDE_DEKU_TREE,              ENTR_KOKIRI_FOREST_OUTSIDE_DEKU_TREE } },
        { { EntranceType::Dungeon,      RR_DEATH_MOUNTAIN_TRAIL,             RR_DODONGOS_CAVERN_ENTRYWAY,          ENTR_DODONGOS_CAVERN_ENTRANCE },
          { EntranceType::Dungeon,      RR_DODONGOS_CAVERN_ENTRYWAY,         RR_DEATH_MOUNTAIN_TRAIL,              ENTR_DEATH_MOUNTAIN_TRAIL_OUTSIDE_DODONGOS_CAVERN } },
        { { EntranceType::Dungeon,      RR_ZORAS_FOUNTAIN,                   RR_JABU_JABUS_BELLY_ENTRYWAY,         ENTR_JABU_JABU_ENTRANCE },
          { EntranceType::Dungeon,      RR_JABU_JABUS_BELLY_ENTRYWAY,        RR_ZORAS_FOUNTAIN,                    ENTR_ZORAS_FOUNTAIN_OUTSIDE_JABU_JABU } },
        { { EntranceType::Dungeon,      RR_SACRED_FOREST_MEADOW,             RR_FOREST_TEMPLE_ENTRYWAY,            ENTR_FOREST_TEMPLE_ENTRANCE },
          { EntranceType::Dungeon,      RR_FOREST_TEMPLE_ENTRYWAY,           RR_SACRED_FOREST_MEADOW,              ENTR_SACRED_FOREST_MEADOW_OUTSIDE_TEMPLE } },
        { { EntranceType::Dungeon,      RR_DMC_TEMPLE_EXIT,                  RR_FIRE_TEMPLE_ENTRYWAY,              ENTR_FIRE_TEMPLE_ENTRANCE },
          { EntranceType::Dungeon,      RR_FIRE_TEMPLE_ENTRYWAY,             RR_DMC_TEMPLE_EXIT,                   ENTR_DEATH_MOUNTAIN_CRATER_OUTSIDE_TEMPLE } },
        { { EntranceType::Dungeon,      RR_LH_FROM_WATER_TEMPLE,             RR_WATER_TEMPLE_ENTRYWAY,             ENTR_WATER_TEMPLE_ENTRANCE },
          { EntranceType::Dungeon,      RR_WATER_TEMPLE_ENTRYWAY,            RR_LH_FROM_WATER_TEMPLE,              ENTR_LAKE_HYLIA_OUTSIDE_TEMPLE } },
        { { EntranceType::Dungeon,      RR_DESERT_COLOSSUS,                  RR_SPIRIT_TEMPLE_ENTRYWAY,            ENTR_SPIRIT_TEMPLE_ENTRANCE },
          { EntranceType::Dungeon,      RR_SPIRIT_TEMPLE_ENTRYWAY,           RR_DESERT_COLOSSUS_OUTSIDE_TEMPLE,    ENTR_DESERT_COLOSSUS_OUTSIDE_TEMPLE } },
        { { EntranceType::Dungeon,      RR_GRAVEYARD_WARP_PAD_REGION,        RR_SHADOW_TEMPLE_ENTRYWAY,            ENTR_SHADOW_TEMPLE_ENTRANCE },
          { EntranceType::Dungeon,      RR_SHADOW_TEMPLE_ENTRYWAY,           RR_GRAVEYARD_WARP_PAD_REGION,         ENTR_GRAVEYARD_OUTSIDE_TEMPLE } },
        { { EntranceType::Dungeon,      RR_KAK_WELL,                         RR_BOTW_ENTRYWAY,                     ENTR_BOTTOM_OF_THE_WELL_ENTRANCE },
          { EntranceType::Dungeon,      RR_BOTW_ENTRYWAY,                    RR_KAK_WELL,                          ENTR_KAKARIKO_VILLAGE_OUTSIDE_BOTTOM_OF_THE_WELL } },
        { { EntranceType::Dungeon,      RR_ZF_LEDGE,                         RR_ICE_CAVERN_ENTRYWAY,               ENTR_ICE_CAVERN_ENTRANCE },
          { EntranceType::Dungeon,      RR_ICE_CAVERN_ENTRYWAY,              RR_ZF_LEDGE,                          ENTR_ZORAS_FOUNTAIN_OUTSIDE_ICE_CAVERN } },
        { { EntranceType::Dungeon,      RR_GF_TO_GTG,                        RR_GERUDO_TRAINING_GROUND_ENTRYWAY,   ENTR_GERUDO_TRAINING_GROUND_ENTRANCE },
          { EntranceType::Dungeon,      RR_GERUDO_TRAINING_GROUND_ENTRYWAY,  RR_GF_EXITING_GTG,                    ENTR_GERUDOS_FORTRESS_OUTSIDE_GERUDO_TRAINING_GROUND } },
        { { EntranceType::GanonDungeon, RR_GANONS_CASTLE_LEDGE,              RR_GANONS_CASTLE_ENTRYWAY,            ENTR_INSIDE_GANONS_CASTLE_ENTRANCE },
          { EntranceType::GanonDungeon, RR_GANONS_CASTLE_ENTRYWAY,           RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE, ENTR_CASTLE_GROUNDS_RAINBOW_BRIDGE_EXIT } },
        
        { { EntranceType::Interior, RR_KOKIRI_FOREST,                 RR_KF_MIDOS_HOUSE,                  ENTR_MIDOS_HOUSE_0 },
          { EntranceType::Interior, RR_KF_MIDOS_HOUSE,                RR_KOKIRI_FOREST,                   ENTR_KOKIRI_FOREST_OUTSIDE_MIDOS_HOUSE } },
        { { EntranceType::Interior, RR_KOKIRI_FOREST,                 RR_KF_SARIAS_HOUSE,                 ENTR_SARIAS_HOUSE_0 },
          { EntranceType::Interior, RR_KF_SARIAS_HOUSE,               RR_KOKIRI_FOREST,                   ENTR_KOKIRI_FOREST_OUTSIDE_SARIAS_HOUSE } },
        { { EntranceType::Interior, RR_KOKIRI_FOREST,                 RR_KF_HOUSE_OF_TWINS,               ENTR_TWINS_HOUSE_0 },
          { EntranceType::Interior, RR_KF_HOUSE_OF_TWINS,             RR_KOKIRI_FOREST,                   ENTR_KOKIRI_FOREST_OUTSIDE_TWINS_HOUSE } },
        { { EntranceType::Interior, RR_KOKIRI_FOREST,                 RR_KF_KNOW_IT_ALL_HOUSE,            ENTR_KNOW_IT_ALL_BROS_HOUSE_0 },
          { EntranceType::Interior, RR_KF_KNOW_IT_ALL_HOUSE,          RR_KOKIRI_FOREST,                   ENTR_KOKIRI_FOREST_OUTSIDE_KNOW_IT_ALL_HOUSE } },
        { { EntranceType::Interior, RR_KOKIRI_FOREST,                 RR_KF_KOKIRI_SHOP,                  ENTR_KOKIRI_SHOP_0 },
          { EntranceType::Interior, RR_KF_KOKIRI_SHOP,                RR_KOKIRI_FOREST,                   ENTR_KOKIRI_FOREST_OUTSIDE_SHOP } },
        { { EntranceType::Interior, RR_LAKE_HYLIA,                    RR_LH_LAB,                          ENTR_LAKESIDE_LABORATORY_0 },
          { EntranceType::Interior, RR_LH_LAB,                        RR_LAKE_HYLIA,                      ENTR_LAKE_HYLIA_OUTSIDE_LAB } },
        { { EntranceType::Interior, RR_LH_FISHING_ISLAND,             RR_LH_FISHING_POND,                 ENTR_FISHING_POND_0 },
          { EntranceType::Interior, RR_LH_FISHING_POND,               RR_LH_FISHING_ISLAND,               ENTR_LAKE_HYLIA_OUTSIDE_FISHING_POND } },
        { { EntranceType::Interior, RR_GV_FORTRESS_SIDE,              RR_GV_CARPENTER_TENT,               ENTR_CARPENTERS_TENT_0 },
          { EntranceType::Interior, RR_GV_CARPENTER_TENT,             RR_GV_FORTRESS_SIDE,                ENTR_GERUDO_VALLEY_OUTSIDE_TENT } },
        { { EntranceType::Interior, RR_MARKET_ENTRANCE,               RR_MARKET_GUARD_HOUSE,              ENTR_MARKET_GUARD_HOUSE_0 },
          { EntranceType::Interior, RR_MARKET_GUARD_HOUSE,            RR_MARKET_ENTRANCE,                 ENTR_MARKET_ENTRANCE_OUTSIDE_GUARD_HOUSE } },
        { { EntranceType::Interior, RR_THE_MARKET,                    RR_MARKET_MASK_SHOP,                ENTR_HAPPY_MASK_SHOP_0 },
          { EntranceType::Interior, RR_MARKET_MASK_SHOP,              RR_THE_MARKET,                      ENTR_MARKET_DAY_OUTSIDE_HAPPY_MASK_SHOP } },
        { { EntranceType::Interior, RR_THE_MARKET,                    RR_MARKET_BOMBCHU_BOWLING,          ENTR_BOMBCHU_BOWLING_ALLEY_0 },
          { EntranceType::Interior, RR_MARKET_BOMBCHU_BOWLING,        RR_THE_MARKET,                      ENTR_MARKET_DAY_OUTSIDE_BOMBCHU_BOWLING } },
        { { EntranceType::Interior, RR_THE_MARKET,                    RR_MARKET_POTION_SHOP,              ENTR_POTION_SHOP_MARKET_0 },
          { EntranceType::Interior, RR_MARKET_POTION_SHOP,            RR_THE_MARKET,                      ENTR_MARKET_DAY_OUTSIDE_POTION_SHOP } },
        { { EntranceType::Interior, RR_THE_MARKET,                    RR_MARKET_TREASURE_CHEST_GAME,      ENTR_TREASURE_BOX_SHOP_0 },
          { EntranceType::Interior, RR_MARKET_TREASURE_CHEST_GAME,    RR_THE_MARKET,                      ENTR_MARKET_DAY_OUTSIDE_TREASURE_BOX_SHOP } },
        { { EntranceType::Interior, RR_MARKET_BACK_ALLEY,             RR_MARKET_BOMBCHU_SHOP,             ENTR_BOMBCHU_SHOP_1 },
          { EntranceType::Interior, RR_MARKET_BOMBCHU_SHOP,           RR_MARKET_BACK_ALLEY,               ENTR_BACK_ALLEY_DAY_OUTSIDE_BOMBCHU_SHOP } },
        { { EntranceType::Interior, RR_MARKET_BACK_ALLEY,             RR_MARKET_MAN_IN_GREEN_HOUSE,       ENTR_BACK_ALLEY_MAN_IN_GREEN_HOUSE },
          { EntranceType::Interior, RR_MARKET_MAN_IN_GREEN_HOUSE,     RR_MARKET_BACK_ALLEY,               ENTR_BACK_ALLEY_DAY_OUTSIDE_MAN_IN_GREEN_HOUSE } },
        { { EntranceType::Interior, RR_KAKARIKO_VILLAGE,              RR_KAK_CARPENTER_BOSS_HOUSE,        ENTR_KAKARIKO_CENTER_GUEST_HOUSE_0 },
          { EntranceType::Interior, RR_KAK_CARPENTER_BOSS_HOUSE,      RR_KAKARIKO_VILLAGE,                ENTR_KAKARIKO_VILLAGE_OUTSIDE_CENTER_GUEST_HOUSE } },
        { { EntranceType::Interior, RR_KAKARIKO_VILLAGE,              RR_KAK_HOUSE_OF_SKULLTULA,          ENTR_HOUSE_OF_SKULLTULA_0 },
          { EntranceType::Interior, RR_KAK_HOUSE_OF_SKULLTULA,        RR_KAKARIKO_VILLAGE,                ENTR_KAKARIKO_VILLAGE_OUTSIDE_SKULKLTULA_HOUSE } },
        { { EntranceType::Interior, RR_KAKARIKO_VILLAGE,              RR_KAK_IMPAS_HOUSE,                 ENTR_IMPAS_HOUSE_FRONT },
          { EntranceType::Interior, RR_KAK_IMPAS_HOUSE,               RR_KAKARIKO_VILLAGE,                ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_FRONT } },
        { { EntranceType::Interior, RR_KAK_IMPAS_LEDGE,               RR_KAK_IMPAS_HOUSE_BACK,            ENTR_IMPAS_HOUSE_BACK },
          { EntranceType::Interior, RR_KAK_IMPAS_HOUSE_BACK,          RR_KAK_IMPAS_LEDGE,                 ENTR_KAKARIKO_VILLAGE_OUTSIDE_IMPAS_HOUSE_BACK } },
        { { EntranceType::Interior, RR_KAK_BACKYARD,                  RR_KAK_ODD_POTION_BUILDING,         ENTR_POTION_SHOP_GRANNY_0 },
          { EntranceType::Interior, RR_KAK_ODD_POTION_BUILDING,       RR_KAK_BACKYARD,                    ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOP_GRANNY } },
        { { EntranceType::Interior, RR_THE_GRAVEYARD,                 RR_GRAVEYARD_DAMPES_HOUSE,          ENTR_GRAVEKEEPERS_HUT_0 },
          { EntranceType::Interior, RR_GRAVEYARD_DAMPES_HOUSE,        RR_THE_GRAVEYARD,                   ENTR_GRAVEYARD_OUTSIDE_DAMPES_HUT } },
        { { EntranceType::Interior, RR_GORON_CITY,                    RR_GC_SHOP,                         ENTR_GORON_SHOP_0 },
          { EntranceType::Interior, RR_GC_SHOP,                       RR_GORON_CITY,                      ENTR_GORON_CITY_OUTSIDE_SHOP } },
        { { EntranceType::Interior, RR_ZORAS_DOMAIN,                  RR_ZD_SHOP,                         ENTR_ZORA_SHOP_0 },
          { EntranceType::Interior, RR_ZD_SHOP,                       RR_ZORAS_DOMAIN,                    ENTR_ZORAS_DOMAIN_OUTSIDE_SHOP } },
        { { EntranceType::Interior, RR_LON_LON_RANCH,                 RR_LLR_TALONS_HOUSE,                ENTR_LON_LON_BUILDINGS_TALONS_HOUSE },
          { EntranceType::Interior, RR_LLR_TALONS_HOUSE,              RR_LON_LON_RANCH,                   ENTR_LON_LON_RANCH_OUTSIDE_TALONS_HOUSE } },
        { { EntranceType::Interior, RR_LON_LON_RANCH,                 RR_LLR_STABLES,                     ENTR_STABLE_0 },
          { EntranceType::Interior, RR_LLR_STABLES,                   RR_LON_LON_RANCH,                   ENTR_LON_LON_RANCH_OUTSIDE_STABLES } },
        { { EntranceType::Interior, RR_LON_LON_RANCH,                 RR_LLR_TOWER,                       ENTR_LON_LON_BUILDINGS_TOWER },
          { EntranceType::Interior, RR_LLR_TOWER,                     RR_LON_LON_RANCH,                   ENTR_LON_LON_RANCH_OUTSIDE_TOWER } },
        { { EntranceType::Interior, RR_THE_MARKET,                    RR_MARKET_BAZAAR,                   ENTR_BAZAAR_1 },
          { EntranceType::Interior, RR_MARKET_BAZAAR,                 RR_THE_MARKET,                      ENTR_MARKET_DAY_OUTSIDE_BAZAAR } },
        { { EntranceType::Interior, RR_THE_MARKET,                    RR_MARKET_SHOOTING_GALLERY,         ENTR_SHOOTING_GALLERY_1 },
          { EntranceType::Interior, RR_MARKET_SHOOTING_GALLERY,       RR_THE_MARKET,                      ENTR_MARKET_DAY_OUTSIDE_SHOOTING_GALLERY } },
        { { EntranceType::Interior, RR_KAKARIKO_VILLAGE,              RR_KAK_BAZAAR,                      ENTR_BAZAAR_0 },
          { EntranceType::Interior, RR_KAK_BAZAAR,                    RR_KAKARIKO_VILLAGE,                ENTR_KAKARIKO_VILLAGE_OUTSIDE_BAZAAR } },
        { { EntranceType::Interior, RR_KAKARIKO_VILLAGE,              RR_KAK_SHOOTING_GALLERY,            ENTR_SHOOTING_GALLERY_0 },
          { EntranceType::Interior, RR_KAK_SHOOTING_GALLERY,          RR_KAKARIKO_VILLAGE,                ENTR_KAKARIKO_VILLAGE_OUTSIDE_SHOOTING_GALLERY } },
        { { EntranceType::Interior, RR_DESERT_COLOSSUS,               RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN,   ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_NAYRUS_COLOSSUS },
          { EntranceType::Interior, RR_COLOSSUS_GREAT_FAIRY_FOUNTAIN, RR_DESERT_COLOSSUS,                 ENTR_DESERT_COLOSSUS_GREAT_FAIRY_EXIT } },
        { { EntranceType::Interior, RR_HC_PAST_GATE,                  RR_HC_GREAT_FAIRY_FOUNTAIN,         ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_DINS_HC },
          { EntranceType::Interior, RR_HC_GREAT_FAIRY_FOUNTAIN,       RR_CASTLE_GROUNDS_FROM_GREAT_FAIRY, ENTR_CASTLE_GROUNDS_GREAT_FAIRY_EXIT } },
        { { EntranceType::Interior, RR_GANONS_CASTLE_GROUNDS,         RR_OGC_GREAT_FAIRY_FOUNTAIN,        ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_OGC_DD },
        // ENTR_POTION_SHOP_KAKARIKO_1 is an unused entrance index repurposed to differentiate between HC and OGC fairy fountain exits
        // (normally both use ENTR_CASTLE_GROUNDS_GREAT_FAIRY_EXIT)
          { EntranceType::Interior, RR_OGC_GREAT_FAIRY_FOUNTAIN,      RR_CASTLE_GROUNDS_FROM_GREAT_FAIRY, ENTR_POTION_SHOP_KAKARIKO_1 } },
        { { EntranceType::Interior, RR_DMC_BLOCKED_EXIT,              RR_DMC_GREAT_FAIRY_FOUNTAIN,        ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMC },
          { EntranceType::Interior, RR_DMC_GREAT_FAIRY_FOUNTAIN,      RR_DMC_BLOCKED_ENTRY,               ENTR_DEATH_MOUNTAIN_CRATER_GREAT_FAIRY_EXIT } },
        { { EntranceType::Interior, RR_DEATH_MOUNTAIN_SUMMIT,         RR_DMT_GREAT_FAIRY_FOUNTAIN,        ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMT },
          { EntranceType::Interior, RR_DMT_GREAT_FAIRY_FOUNTAIN,      RR_DEATH_MOUNTAIN_SUMMIT,           ENTR_DEATH_MOUNTAIN_TRAIL_GREAT_FAIRY_EXIT } },
        { { EntranceType::Interior, RR_ZORAS_FOUNTAIN,                RR_ZF_GREAT_FAIRY_FOUNTAIN,         ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_FARORES_ZF },
          { EntranceType::Interior, RR_ZF_GREAT_FAIRY_FOUNTAIN,       RR_ZORAS_FOUNTAIN,                  ENTR_ZORAS_FOUNTAIN_OUTSIDE_GREAT_FAIRY } },
        
        { { EntranceType::SpecialInterior, RR_KF_LINKS_PORCH,         RR_KF_LINKS_HOUSE,         ENTR_LINKS_HOUSE_1 },
          { EntranceType::SpecialInterior, RR_KF_LINKS_HOUSE,         RR_KF_LINKS_PORCH,         ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE } },
        { { EntranceType::SpecialInterior, RR_TOT_ENTRANCE,           RR_TEMPLE_OF_TIME,         ENTR_TEMPLE_OF_TIME_ENTRANCE },
          { EntranceType::SpecialInterior, RR_TEMPLE_OF_TIME,         RR_TOT_ENTRANCE,           ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_OUTSIDE_TEMPLE } },
        { { EntranceType::SpecialInterior, RR_KAKARIKO_VILLAGE,       RR_KAK_WINDMILL_LOWER,     ENTR_WINDMILL_AND_DAMPES_GRAVE_WINDMILL },
          { EntranceType::SpecialInterior, RR_KAK_WINDMILL_LOWER,     RR_KAKARIKO_VILLAGE,       ENTR_KAKARIKO_VILLAGE_OUTSIDE_WINDMILL } },
        { { EntranceType::SpecialInterior, RR_KAKARIKO_VILLAGE,       RR_KAK_POTION_SHOP,        ENTR_POTION_SHOP_KAKARIKO_FRONT },
          { EntranceType::SpecialInterior, RR_KAK_POTION_SHOP,        RR_KAKARIKO_VILLAGE,       ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_FRONT } },
        { { EntranceType::SpecialInterior, RR_KAK_BEHIND_POTION_SHOP, RR_KAK_POTION_SHOP,        ENTR_POTION_SHOP_KAKARIKO_BACK },
          { EntranceType::SpecialInterior, RR_KAK_POTION_SHOP,        RR_KAK_BEHIND_POTION_SHOP, ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_BACK } },
        
        { { EntranceType::ThievesHideout, RR_GF_OUTSKIRTS,                 RR_TH_1_TORCH_CELL,              ENTR_THIEVES_HIDEOUT_0 },
          { EntranceType::ThievesHideout, RR_TH_1_TORCH_CELL,              RR_GF_OUTSKIRTS,                 ENTR_GERUDOS_FORTRESS_1 } },
        { { EntranceType::ThievesHideout, RR_GF_NEAR_GROTTO,               RR_TH_1_TORCH_CELL,              ENTR_THIEVES_HIDEOUT_1 },
          { EntranceType::ThievesHideout, RR_TH_1_TORCH_CELL,              RR_GF_NEAR_GROTTO,               ENTR_GERUDOS_FORTRESS_2 } },
        { { EntranceType::ThievesHideout, RR_GF_NEAR_GROTTO,               RR_TH_KITCHEN_CORRIDOR,          ENTR_THIEVES_HIDEOUT_2 },
          { EntranceType::ThievesHideout, RR_TH_KITCHEN_CORRIDOR,          RR_GF_NEAR_GROTTO,               ENTR_GERUDOS_FORTRESS_3 } },
        { { EntranceType::ThievesHideout, RR_GF_ABOVE_GTG,                 RR_TH_KITCHEN_CORRIDOR,          ENTR_THIEVES_HIDEOUT_3 },
          { EntranceType::ThievesHideout, RR_TH_KITCHEN_CORRIDOR,          RR_GF_ABOVE_GTG,                 ENTR_GERUDOS_FORTRESS_4 } },
        { { EntranceType::ThievesHideout, RR_GF_NEAR_GROTTO,               RR_TH_STEEP_SLOPE_CELL,          ENTR_THIEVES_HIDEOUT_4 },
          { EntranceType::ThievesHideout, RR_TH_STEEP_SLOPE_CELL,          RR_GF_NEAR_GROTTO,               ENTR_GERUDOS_FORTRESS_5 } },
        { { EntranceType::ThievesHideout, RR_GF_BOTTOM_OF_LOWER_VINES,     RR_TH_STEEP_SLOPE_CELL,          ENTR_THIEVES_HIDEOUT_5 },
          { EntranceType::ThievesHideout, RR_TH_STEEP_SLOPE_CELL,          RR_GF_BOTTOM_OF_LOWER_VINES,     ENTR_GERUDOS_FORTRESS_6 } },
        { { EntranceType::ThievesHideout, RR_GF_ABOVE_GTG,                 RR_TH_DOUBLE_CELL,               ENTR_THIEVES_HIDEOUT_6 },
          { EntranceType::ThievesHideout, RR_TH_DOUBLE_CELL,               RR_GF_ABOVE_GTG,                 ENTR_GERUDOS_FORTRESS_7 } },
        { { EntranceType::ThievesHideout, RR_GF_TOP_OF_LOWER_VINES,        RR_TH_DOUBLE_CELL,               ENTR_THIEVES_HIDEOUT_7 },
          { EntranceType::ThievesHideout, RR_TH_DOUBLE_CELL,               RR_GF_TOP_OF_LOWER_VINES,        ENTR_GERUDOS_FORTRESS_8 } },
        { { EntranceType::ThievesHideout, RR_GF_TOP_OF_LOWER_VINES,        RR_TH_KITCHEN_BY_CORRIDOR,       ENTR_THIEVES_HIDEOUT_8 },
          { EntranceType::ThievesHideout, RR_TH_KITCHEN_BY_CORRIDOR,       RR_GF_TOP_OF_LOWER_VINES,        ENTR_GERUDOS_FORTRESS_9 } },
        { { EntranceType::ThievesHideout, RR_GF_NEAR_GS,                   RR_TH_KITCHEN_OPPOSITE_CORRIDOR, ENTR_THIEVES_HIDEOUT_9 },
          { EntranceType::ThievesHideout, RR_TH_KITCHEN_OPPOSITE_CORRIDOR, RR_GF_NEAR_GS,                   ENTR_GERUDOS_FORTRESS_10 } },
        { { EntranceType::ThievesHideout, RR_GF_BELOW_CHEST,               RR_TH_BREAK_ROOM,                ENTR_THIEVES_HIDEOUT_10 },
          { EntranceType::ThievesHideout, RR_TH_BREAK_ROOM,                RR_GF_BELOW_CHEST,               ENTR_GERUDOS_FORTRESS_11 } },
        { { EntranceType::ThievesHideout, RR_GF_ABOVE_JAIL,                RR_TH_BREAK_ROOM_UPPER_CORRIDOR, ENTR_THIEVES_HIDEOUT_11 },
          { EntranceType::ThievesHideout, RR_TH_BREAK_ROOM_UPPER_CORRIDOR, RR_GF_ABOVE_JAIL,                ENTR_GERUDOS_FORTRESS_12 } },
        { { EntranceType::ThievesHideout, RR_GF_BELOW_GS,                  RR_TH_DEAD_END_CELL,             ENTR_THIEVES_HIDEOUT_12 },
          { EntranceType::ThievesHideout, RR_TH_DEAD_END_CELL,             RR_GF_BELOW_GS,                  ENTR_GERUDOS_FORTRESS_13 } },

        // Grotto Loads use an entrance index of 0x0700 + their grotto id. The id is used as index for the
        // grottoLoadTable in soh/soh/Enhancements/randomizer/randomizer_grotto.c
        // Grotto Returns use an entrance index of 0x0800 + their grotto id. The id is used as index for the
        // grottoReturnTable in soh/soh/Enhancements/randomizer/randomizer_grotto.c
        { { EntranceType::GrottoGrave, RR_DESERT_COLOSSUS,          RR_COLOSSUS_GROTTO,            ENTRANCE_GROTTO_LOAD(GROTTO_COLOSSUS_OFFSET) },
          { EntranceType::GrottoGrave, RR_COLOSSUS_GROTTO,          RR_DESERT_COLOSSUS,            ENTRANCE_GROTTO_EXIT(GROTTO_COLOSSUS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_LAKE_HYLIA,               RR_LH_GROTTO,                  ENTRANCE_GROTTO_LOAD(GROTTO_LH_OFFSET) },
          { EntranceType::GrottoGrave, RR_LH_GROTTO,                RR_LAKE_HYLIA,                 ENTRANCE_GROTTO_EXIT(GROTTO_LH_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_ZORAS_RIVER,              RR_ZR_STORMS_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_ZR_STORMS_OFFSET) },
          { EntranceType::GrottoGrave, RR_ZR_STORMS_GROTTO,         RR_ZORAS_RIVER,                ENTRANCE_GROTTO_EXIT(GROTTO_ZR_STORMS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_ZR_ATOP_LADDER,           RR_ZR_FAIRY_GROTTO,            ENTRANCE_GROTTO_LOAD(GROTTO_ZR_FAIRY_OFFSET) },
          { EntranceType::GrottoGrave, RR_ZR_FAIRY_GROTTO,          RR_ZR_ATOP_LADDER,             ENTRANCE_GROTTO_EXIT(GROTTO_ZR_FAIRY_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_ZR_ATOP_LADDER,           RR_ZR_OPEN_GROTTO,             ENTRANCE_GROTTO_LOAD(GROTTO_ZR_OPEN_OFFSET) },
          { EntranceType::GrottoGrave, RR_ZR_OPEN_GROTTO,           RR_ZR_ATOP_LADDER,             ENTRANCE_GROTTO_EXIT(GROTTO_ZR_OPEN_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_DMC_POT_GROTTO_EXIT,      RR_DMC_SCRUB_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_DMC_HAMMER_OFFSET) },
          { EntranceType::GrottoGrave, RR_DMC_SCRUB_GROTTO,         RR_DMC_POT_GROTTO_ENTRY,       ENTRANCE_GROTTO_EXIT(GROTTO_DMC_HAMMER_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_DMC_ROCK_GROTTO,          RR_DMC_UPPER_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_DMC_UPPER_OFFSET) },
          { EntranceType::GrottoGrave, RR_DMC_UPPER_GROTTO,         RR_DMC_ROCKS_GROTTO_ENTRY,     ENTRANCE_GROTTO_EXIT(GROTTO_DMC_UPPER_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_GC_GROTTO_PLATFORM,       RR_GC_GROTTO,                  ENTRANCE_GROTTO_LOAD(GROTTO_GORON_CITY_OFFSET) },
          { EntranceType::GrottoGrave, RR_GC_GROTTO,                RR_GC_GROTTO_PLATFORM,         ENTRANCE_GROTTO_EXIT(GROTTO_GORON_CITY_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_DEATH_MOUNTAIN_TRAIL,     RR_DMT_STORMS_GROTTO,          ENTRANCE_GROTTO_LOAD(GROTTO_DMT_STORMS_OFFSET) },
          { EntranceType::GrottoGrave, RR_DMT_STORMS_GROTTO,        RR_DEATH_MOUNTAIN_TRAIL,       ENTRANCE_GROTTO_EXIT(GROTTO_DMT_STORMS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_DEATH_MOUNTAIN_ROCKFALL,  RR_DMT_COW_GROTTO,             ENTRANCE_GROTTO_LOAD(GROTTO_DMT_COW_OFFSET) },
          { EntranceType::GrottoGrave, RR_DMT_COW_GROTTO,           RR_DEATH_MOUNTAIN_ROCKFALL,    ENTRANCE_GROTTO_EXIT(GROTTO_DMT_COW_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_KAK_BACKYARD,             RR_KAK_OPEN_GROTTO,            ENTRANCE_GROTTO_LOAD(GROTTO_KAK_OPEN_OFFSET) },
          { EntranceType::GrottoGrave, RR_KAK_OPEN_GROTTO,          RR_KAK_BACKYARD,               ENTRANCE_GROTTO_EXIT(GROTTO_KAK_OPEN_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_KAKARIKO_VILLAGE,         RR_KAK_REDEAD_GROTTO,          ENTRANCE_GROTTO_LOAD(GROTTO_KAK_REDEAD_OFFSET) },
          { EntranceType::GrottoGrave, RR_KAK_REDEAD_GROTTO,        RR_KAKARIKO_VILLAGE,           ENTRANCE_GROTTO_EXIT(GROTTO_KAK_REDEAD_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_HC_MOAT,                  RR_HC_STORMS_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_HC_STORMS_OFFSET) },
          { EntranceType::GrottoGrave, RR_HC_STORMS_GROTTO,         RR_CASTLE_GROUNDS_FROM_GROTTO, ENTRANCE_GROTTO_EXIT(GROTTO_HC_STORMS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_HYRULE_FIELD,             RR_HF_TEKTITE_GROTTO,          ENTRANCE_GROTTO_LOAD(GROTTO_HF_TEKTITE_OFFSET) },
          { EntranceType::GrottoGrave, RR_HF_TEKTITE_GROTTO,        RR_HYRULE_FIELD,               ENTRANCE_GROTTO_EXIT(GROTTO_HF_TEKTITE_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_HYRULE_FIELD,             RR_HF_NEAR_KAK_GROTTO,         ENTRANCE_GROTTO_LOAD(GROTTO_HF_NEAR_KAK_OFFSET) },
          { EntranceType::GrottoGrave, RR_HF_NEAR_KAK_GROTTO,       RR_HYRULE_FIELD,               ENTRANCE_GROTTO_EXIT(GROTTO_HF_NEAR_KAK_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_HYRULE_FIELD,             RR_HF_FAIRY_GROTTO,            ENTRANCE_GROTTO_LOAD(GROTTO_HF_FAIRY_OFFSET) },
          { EntranceType::GrottoGrave, RR_HF_FAIRY_GROTTO,          RR_HYRULE_FIELD,               ENTRANCE_GROTTO_EXIT(GROTTO_HF_FAIRY_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_HYRULE_FIELD,             RR_HF_NEAR_MARKET_GROTTO,      ENTRANCE_GROTTO_LOAD(GROTTO_HF_NEAR_MARKET_OFFSET) },
          { EntranceType::GrottoGrave, RR_HF_NEAR_MARKET_GROTTO,    RR_HYRULE_FIELD,               ENTRANCE_GROTTO_EXIT(GROTTO_HF_NEAR_MARKET_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_HYRULE_FIELD,             RR_HF_COW_GROTTO,              ENTRANCE_GROTTO_LOAD(GROTTO_HF_COW_OFFSET) },
          { EntranceType::GrottoGrave, RR_HF_COW_GROTTO,            RR_HYRULE_FIELD,               ENTRANCE_GROTTO_EXIT(GROTTO_HF_COW_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_HYRULE_FIELD,             RR_HF_INSIDE_FENCE_GROTTO,     ENTRANCE_GROTTO_LOAD(GROTTO_HF_INSIDE_FENCE_OFFSET) },
          { EntranceType::GrottoGrave, RR_HF_INSIDE_FENCE_GROTTO,   RR_HYRULE_FIELD,               ENTRANCE_GROTTO_EXIT(GROTTO_HF_INSIDE_FENCE_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_HYRULE_FIELD,             RR_HF_OPEN_GROTTO,             ENTRANCE_GROTTO_LOAD(GROTTO_HF_OPEN_OFFSET) },
          { EntranceType::GrottoGrave, RR_HF_OPEN_GROTTO,           RR_HYRULE_FIELD,               ENTRANCE_GROTTO_EXIT(GROTTO_HF_OPEN_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_HYRULE_FIELD,             RR_HF_SOUTHEAST_GROTTO,        ENTRANCE_GROTTO_LOAD(GROTTO_HF_SOUTHEAST_OFFSET) },
          { EntranceType::GrottoGrave, RR_HF_SOUTHEAST_GROTTO,      RR_HYRULE_FIELD,               ENTRANCE_GROTTO_EXIT(GROTTO_HF_SOUTHEAST_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_LON_LON_RANCH,            RR_LLR_GROTTO,                 ENTRANCE_GROTTO_LOAD(GROTTO_LLR_OFFSET) },
          { EntranceType::GrottoGrave, RR_LLR_GROTTO,               RR_LON_LON_RANCH,              ENTRANCE_GROTTO_EXIT(GROTTO_LLR_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_SFM_ENTRYWAY,             RR_SFM_WOLFOS_GROTTO,          ENTRANCE_GROTTO_LOAD(GROTTO_SFM_WOLFOS_OFFSET) },
          { EntranceType::GrottoGrave, RR_SFM_WOLFOS_GROTTO,        RR_SFM_ENTRYWAY,               ENTRANCE_GROTTO_EXIT(GROTTO_SFM_WOLFOS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_SACRED_FOREST_MEADOW,     RR_SFM_STORMS_GROTTO,          ENTRANCE_GROTTO_LOAD(GROTTO_SFM_STORMS_OFFSET) },
          { EntranceType::GrottoGrave, RR_SFM_STORMS_GROTTO,        RR_SACRED_FOREST_MEADOW,       ENTRANCE_GROTTO_EXIT(GROTTO_SFM_STORMS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_SFM_OUTSIDE_FAIRY_GROTTO, RR_SFM_FAIRY_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_SFM_FAIRY_OFFSET) },
          { EntranceType::GrottoGrave, RR_SFM_FAIRY_GROTTO,         RR_SFM_OUTSIDE_FAIRY_GROTTO,   ENTRANCE_GROTTO_EXIT(GROTTO_SFM_FAIRY_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_LW_BEYOND_MIDO,           RR_LW_SCRUBS_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_LW_SCRUBS_OFFSET) },
          { EntranceType::GrottoGrave, RR_LW_SCRUBS_GROTTO,         RR_LW_BEYOND_MIDO,             ENTRANCE_GROTTO_EXIT(GROTTO_LW_SCRUBS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_THE_LOST_WOODS,           RR_LW_NEAR_SHORTCUTS_GROTTO,   ENTRANCE_GROTTO_LOAD(GROTTO_LW_NEAR_SHORTCUTS_OFFSET) },
          { EntranceType::GrottoGrave, RR_LW_NEAR_SHORTCUTS_GROTTO, RR_THE_LOST_WOODS,             ENTRANCE_GROTTO_EXIT(GROTTO_LW_NEAR_SHORTCUTS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_KF_OUTSIDE_LOST_WOODS,    RR_KF_STORMS_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_KF_STORMS_OFFSET) },
          { EntranceType::GrottoGrave, RR_KF_STORMS_GROTTO,         RR_KF_OUTSIDE_LOST_WOODS,      ENTRANCE_GROTTO_EXIT(GROTTO_KF_STORMS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_ZORAS_DOMAIN_ISLAND,      RR_ZD_STORMS_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_ZD_STORMS_OFFSET) },
          { EntranceType::GrottoGrave, RR_ZD_STORMS_GROTTO,         RR_ZORAS_DOMAIN_ISLAND,        ENTRANCE_GROTTO_EXIT(GROTTO_ZD_STORMS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_GF_NEAR_GROTTO,           RR_GF_STORMS_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_GF_STORMS_OFFSET) },
          { EntranceType::GrottoGrave, RR_GF_STORMS_GROTTO,         RR_GF_NEAR_GROTTO,             ENTRANCE_GROTTO_EXIT(GROTTO_GF_STORMS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_GV_FORTRESS_SIDE,         RR_GV_STORMS_GROTTO,           ENTRANCE_GROTTO_LOAD(GROTTO_GV_STORMS_OFFSET) },
          { EntranceType::GrottoGrave, RR_GV_STORMS_GROTTO,         RR_GV_FORTRESS_SIDE,           ENTRANCE_GROTTO_EXIT(GROTTO_GV_STORMS_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_GV_GROTTO_LEDGE,          RR_GV_OCTOROK_GROTTO,          ENTRANCE_GROTTO_LOAD(GROTTO_GV_OCTOROK_OFFSET) },
          { EntranceType::GrottoGrave, RR_GV_OCTOROK_GROTTO,        RR_GV_GROTTO_LEDGE,            ENTRANCE_GROTTO_EXIT(GROTTO_GV_OCTOROK_OFFSET) } },
        { { EntranceType::GrottoGrave, RR_LW_BEYOND_MIDO,           RR_DEKU_THEATER,               ENTRANCE_GROTTO_LOAD(GROTTO_LW_DEKU_THEATRE_OFFSET) },
          { EntranceType::GrottoGrave, RR_DEKU_THEATER,             RR_LW_BEYOND_MIDO,             ENTRANCE_GROTTO_EXIT(GROTTO_LW_DEKU_THEATRE_OFFSET) } },
        
        // Graves have their own specified entrance indices
        { { EntranceType::GrottoGrave, RR_THE_GRAVEYARD,               RR_GRAVEYARD_SHIELD_GRAVE,      ENTR_GRAVE_WITH_FAIRYS_FOUNTAIN_0 },
          { EntranceType::GrottoGrave, RR_GRAVEYARD_SHIELD_GRAVE,      RR_THE_GRAVEYARD,               ENTR_GRAVEYARD_SHIELD_GRAVE_EXIT } },
        { { EntranceType::GrottoGrave, RR_THE_GRAVEYARD,               RR_GRAVEYARD_HEART_PIECE_GRAVE, ENTR_REDEAD_GRAVE_0 },
          { EntranceType::GrottoGrave, RR_GRAVEYARD_HEART_PIECE_GRAVE, RR_THE_GRAVEYARD,               ENTR_GRAVEYARD_HEART_PIECE_GRAVE_EXIT } },
        { { EntranceType::GrottoGrave, RR_THE_GRAVEYARD,               RR_GRAVEYARD_COMPOSERS_GRAVE,   ENTR_ROYAL_FAMILYS_TOMB_0 },
          { EntranceType::GrottoGrave, RR_GRAVEYARD_COMPOSERS_GRAVE,   RR_THE_GRAVEYARD,               ENTR_GRAVEYARD_ROYAL_TOMB_EXIT } },
        { { EntranceType::GrottoGrave, RR_THE_GRAVEYARD,               RR_GRAVEYARD_DAMPES_GRAVE,      ENTR_WINDMILL_AND_DAMPES_GRAVE_GRAVE },
          { EntranceType::GrottoGrave, RR_GRAVEYARD_DAMPES_GRAVE,      RR_THE_GRAVEYARD,               ENTR_GRAVEYARD_DAMPES_GRAVE_EXIT } },
        
        { { EntranceType::Overworld, RR_KOKIRI_FOREST,           RR_LW_BRIDGE_FROM_FOREST,   ENTR_LOST_WOODS_BRIDGE_EAST_EXIT },
          { EntranceType::Overworld, RR_LW_BRIDGE,               RR_KOKIRI_FOREST,           ENTR_KOKIRI_FOREST_LOWER_EXIT } },
        { { EntranceType::Overworld, RR_KF_OUTSIDE_LOST_WOODS,   RR_THE_LOST_WOODS,          ENTR_LOST_WOODS_SOUTH_EXIT },
          { EntranceType::Overworld, RR_LW_FOREST_EXIT,          RR_KF_OUTSIDE_LOST_WOODS,   ENTR_KOKIRI_FOREST_UPPER_EXIT } },
        { { EntranceType::Overworld, RR_THE_LOST_WOODS,          RR_GC_WOODS_WARP,           ENTR_GORON_CITY_TUNNEL_SHORTCUT },
          { EntranceType::Overworld, RR_GC_WOODS_WARP,           RR_THE_LOST_WOODS,          ENTR_LOST_WOODS_TUNNEL_SHORTCUT } },
        { { EntranceType::Overworld, RR_THE_LOST_WOODS,          RR_ZR_FROM_SHORTCUT,        ENTR_ZORAS_RIVER_UNDERWATER_SHORTCUT },
          { EntranceType::Overworld, RR_ZR_FROM_SHORTCUT,        RR_THE_LOST_WOODS,          ENTR_LOST_WOODS_UNDERWATER_SHORTCUT } },
        { { EntranceType::Overworld, RR_LW_BEYOND_MIDO,          RR_SFM_ENTRYWAY,            ENTR_SACRED_FOREST_MEADOW_SOUTH_EXIT },
          { EntranceType::Overworld, RR_SFM_ENTRYWAY,            RR_LW_BEYOND_MIDO,          ENTR_LOST_WOODS_NORTH_EXIT } },
        { { EntranceType::Overworld, RR_LW_BRIDGE,               RR_HYRULE_FIELD,            ENTR_HYRULE_FIELD_WOODED_EXIT },
          { EntranceType::Overworld, RR_HYRULE_FIELD,            RR_LW_BRIDGE,               ENTR_LOST_WOODS_BRIDGE_WEST_EXIT } },
        { { EntranceType::Overworld, RR_HF_TO_LAKE_HYLIA,        RR_LAKE_HYLIA,              ENTR_LAKE_HYLIA_NORTH_EXIT },
          { EntranceType::Overworld, RR_LAKE_HYLIA,              RR_HF_TO_LAKE_HYLIA,        ENTR_HYRULE_FIELD_FENCE_EXIT } },
        { { EntranceType::Overworld, RR_HYRULE_FIELD,            RR_GERUDO_VALLEY,           ENTR_GERUDO_VALLEY_EAST_EXIT },
          { EntranceType::Overworld, RR_GERUDO_VALLEY,           RR_HYRULE_FIELD,            ENTR_HYRULE_FIELD_ROCKY_PATH } },
        { { EntranceType::Overworld, RR_HYRULE_FIELD,            RR_MARKET_ENTRANCE,         ENTR_MARKET_ENTRANCE_NEAR_GUARD_EXIT },
          { EntranceType::Overworld, RR_MARKET_ENTRANCE,         RR_HYRULE_FIELD,            ENTR_HYRULE_FIELD_ON_BRIDGE_SPAWN } },
        { { EntranceType::Overworld, RR_HYRULE_FIELD,            RR_KAKARIKO_VILLAGE,        ENTR_KAKARIKO_VILLAGE_FRONT_GATE },
          { EntranceType::Overworld, RR_KAKARIKO_VILLAGE,        RR_HYRULE_FIELD,            ENTR_HYRULE_FIELD_STAIRS_EXIT } },
        { { EntranceType::Overworld, RR_HYRULE_FIELD,            RR_ZR_FRONT,                ENTR_ZORAS_RIVER_WEST_EXIT },
          { EntranceType::Overworld, RR_ZR_FRONT,                RR_HYRULE_FIELD,            ENTR_HYRULE_FIELD_RIVER_EXIT } },
        { { EntranceType::Overworld, RR_HYRULE_FIELD,            RR_LON_LON_RANCH,           ENTR_LON_LON_RANCH_ENTRANCE },
          { EntranceType::Overworld, RR_LON_LON_RANCH,           RR_HYRULE_FIELD,            ENTR_HYRULE_FIELD_CENTER_EXIT } },
        { { EntranceType::Overworld, RR_LH_FROM_SHORTCUT,        RR_ZORAS_DOMAIN,            ENTR_ZORAS_DOMAIN_UNDERWATER_SHORTCUT },
          { EntranceType::Overworld, RR_ZORAS_DOMAIN,            RR_LH_FROM_SHORTCUT,        ENTR_LAKE_HYLIA_UNDERWATER_SHORTCUT } },
        { { EntranceType::Overworld, RR_GV_FORTRESS_SIDE,        RR_GF_OUTSKIRTS,            ENTR_GERUDOS_FORTRESS_EAST_EXIT },
          { EntranceType::Overworld, RR_GF_OUTSKIRTS,            RR_GV_FORTRESS_SIDE,        ENTR_GERUDO_VALLEY_WEST_EXIT } },
        { { EntranceType::Overworld, RR_GF_OUTSIDE_GATE,         RR_WASTELAND_NEAR_FORTRESS, ENTR_HAUNTED_WASTELAND_EAST_EXIT },
          { EntranceType::Overworld, RR_WASTELAND_NEAR_FORTRESS, RR_GF_OUTSIDE_GATE,         ENTR_GERUDOS_FORTRESS_GATE_EXIT } },
        { { EntranceType::Overworld, RR_WASTELAND_NEAR_COLOSSUS, RR_DESERT_COLOSSUS,         ENTR_DESERT_COLOSSUS_EAST_EXIT },
          { EntranceType::Overworld, RR_DESERT_COLOSSUS,         RR_WASTELAND_NEAR_COLOSSUS, ENTR_HAUNTED_WASTELAND_WEST_EXIT } },
        { { EntranceType::Overworld, RR_MARKET_ENTRANCE,         RR_THE_MARKET,              ENTR_MARKET_SOUTH_EXIT },
          { EntranceType::Overworld, RR_THE_MARKET,              RR_MARKET_ENTRANCE,         ENTR_MARKET_ENTRANCE_NORTH_EXIT } },
        { { EntranceType::Overworld, RR_THE_MARKET,              RR_CASTLE_GROUNDS,          ENTR_CASTLE_GROUNDS_SOUTH_EXIT },
          { EntranceType::Overworld, RR_CASTLE_GROUNDS,          RR_THE_MARKET,              ENTR_MARKET_DAY_CASTLE_EXIT } },
        { { EntranceType::Overworld, RR_THE_MARKET,              RR_TOT_ENTRANCE,            ENTR_TEMPLE_OF_TIME_EXTERIOR_DAY_GOSSIP_STONE_EXIT },
          { EntranceType::Overworld, RR_TOT_ENTRANCE,            RR_THE_MARKET,              ENTR_MARKET_DAY_TEMPLE_EXIT } },
        { { EntranceType::Overworld, RR_KAKARIKO_VILLAGE,        RR_THE_GRAVEYARD,           ENTR_GRAVEYARD_ENTRANCE },
          { EntranceType::Overworld, RR_THE_GRAVEYARD,           RR_KAKARIKO_VILLAGE,        ENTR_KAKARIKO_VILLAGE_SOUTHEAST_EXIT } },
        { { EntranceType::Overworld, RR_KAK_BEHIND_GATE,         RR_DEATH_MOUNTAIN_TRAIL,    ENTR_DEATH_MOUNTAIN_TRAIL_BOTTOM_EXIT },
          { EntranceType::Overworld, RR_DEATH_MOUNTAIN_TRAIL,    RR_KAK_BEHIND_GATE,         ENTR_KAKARIKO_VILLAGE_GUARD_GATE } },
        { { EntranceType::Overworld, RR_DEATH_MOUNTAIN_TRAIL,    RR_GORON_CITY,              ENTR_GORON_CITY_UPPER_EXIT },
          { EntranceType::Overworld, RR_GORON_CITY,              RR_DEATH_MOUNTAIN_TRAIL,    ENTR_DEATH_MOUNTAIN_TRAIL_GC_EXIT } },
        { { EntranceType::Overworld, RR_GC_DARUNIAS_CHAMBER,     RR_DMC_POTS_ENTRY,          ENTR_DEATH_MOUNTAIN_CRATER_GC_EXIT },
          { EntranceType::Overworld, RR_DMC_POTS,                RR_GC_DARUNIAS_CHAMBER,     ENTR_GORON_CITY_DARUNIA_ROOM_EXIT } },
        { { EntranceType::Overworld, RR_DEATH_MOUNTAIN_SUMMIT,   RR_DMC_UPPER_ENTRY,         ENTR_DEATH_MOUNTAIN_CRATER_UPPER_EXIT },
          { EntranceType::Overworld, RR_DMC_CRATE,               RR_DEATH_MOUNTAIN_SUMMIT,   ENTR_DEATH_MOUNTAIN_TRAIL_SUMMIT_EXIT } },
        { { EntranceType::Overworld, RR_ZR_BEHIND_WATERFALL,     RR_ZORAS_DOMAIN,            ENTR_ZORAS_DOMAIN_ENTRANCE },
          { EntranceType::Overworld, RR_ZORAS_DOMAIN,            RR_ZR_BEHIND_WATERFALL,     ENTR_ZORAS_RIVER_WATERFALL_EXIT } },
        { { EntranceType::Overworld, RR_ZD_BEHIND_KING_ZORA,     RR_ZORAS_FOUNTAIN,          ENTR_ZORAS_FOUNTAIN_TUNNEL_EXIT },
          { EntranceType::Overworld, RR_ZORAS_FOUNTAIN,          RR_ZD_BEHIND_KING_ZORA,     ENTR_ZORAS_DOMAIN_KING_ZORA_EXIT } },
        
        { { EntranceType::Overworld, RR_GV_LOWER_STREAM, RR_LAKE_HYLIA,        ENTR_LAKE_HYLIA_RIVER_EXIT },
          NO_RETURN_ENTRANCE },
        
        { { EntranceType::OwlDrop, RR_LH_OWL_FLIGHT,     RR_HYRULE_FIELD,      ENTR_HYRULE_FIELD_OWL_DROP },
          NO_RETURN_ENTRANCE },
        { { EntranceType::OwlDrop, RR_DMT_OWL_FLIGHT,    RR_KAK_IMPAS_ROOFTOP, ENTR_KAKARIKO_VILLAGE_OWL_DROP },
          NO_RETURN_ENTRANCE },
        
        { { EntranceType::Spawn, RR_CHILD_SPAWN,         RR_KF_LINKS_HOUSE,    ENTR_LINKS_HOUSE_CHILD_SPAWN },
          NO_RETURN_ENTRANCE },
        { { EntranceType::Spawn, RR_ADULT_SPAWN,         RR_TEMPLE_OF_TIME,    ENTR_HYRULE_FIELD_10 },
          NO_RETURN_ENTRANCE }, // 0x282 is an unused entrance index repurposed to differentiate between
                                // Adult Spawn and prelude of light (normally they both use 0x5F4)
        { { EntranceType::WarpSong, RR_MINUET_OF_FOREST_WARP,   RR_SACRED_FOREST_MEADOW,      ENTR_SACRED_FOREST_MEADOW_WARP_PAD },
          NO_RETURN_ENTRANCE },
        { { EntranceType::WarpSong, RR_BOLERO_OF_FIRE_WARP,     RR_DMC_PAD_ENTRY,             ENTR_DEATH_MOUNTAIN_CRATER_WARP_PAD },
          NO_RETURN_ENTRANCE },
        { { EntranceType::WarpSong, RR_SERENADE_OF_WATER_WARP,  RR_LAKE_HYLIA,                ENTR_LAKE_HYLIA_WARP_PAD },
          NO_RETURN_ENTRANCE },
        { { EntranceType::WarpSong, RR_REQUIEM_OF_SPIRIT_WARP,  RR_DESERT_COLOSSUS,           ENTR_DESERT_COLOSSUS_WARP_PAD },
          NO_RETURN_ENTRANCE },
        { { EntranceType::WarpSong, RR_NOCTURNE_OF_SHADOW_WARP, RR_GRAVEYARD_WARP_PAD_REGION, ENTR_GRAVEYARD_WARP_PAD },
          NO_RETURN_ENTRANCE },
        { { EntranceType::WarpSong, RR_PRELUDE_OF_LIGHT_WARP,   RR_TEMPLE_OF_TIME,            ENTR_TEMPLE_OF_TIME_WARP_PAD },
          NO_RETURN_ENTRANCE },
        
        { { EntranceType::ChildBoss, RR_DEKU_TREE_BOSS_ENTRYWAY,        RR_DEKU_TREE_BOSS_ROOM,         ENTR_DEKU_TREE_BOSS_ENTRANCE },
          { EntranceType::ChildBoss, RR_DEKU_TREE_BOSS_ROOM,            RR_DEKU_TREE_BOSS_EXIT,         ENTR_DEKU_TREE_BOSS_DOOR } },
        { { EntranceType::ChildBoss, RR_DODONGOS_CAVERN_BOSS_ENTRYWAY,  RR_DODONGOS_CAVERN_BOSS_ROOM,   ENTR_DODONGOS_CAVERN_BOSS_ENTRANCE },
          { EntranceType::ChildBoss, RR_DODONGOS_CAVERN_BOSS_ROOM,      RR_DODONGOS_CAVERN_BOSS_EXIT,   ENTR_DODONGOS_CAVERN_BOSS_DOOR } },
        { { EntranceType::ChildBoss, RR_JABU_JABUS_BELLY_BOSS_ENTRYWAY, RR_JABU_JABUS_BELLY_BOSS_ROOM,  ENTR_JABU_JABU_BOSS_ENTRANCE },
          { EntranceType::ChildBoss, RR_JABU_JABUS_BELLY_BOSS_ROOM,     RR_JABU_JABUS_BELLY_BOSS_EXIT,  ENTR_JABU_JABU_BOSS_DOOR } },
        { { EntranceType::AdultBoss, RR_FOREST_TEMPLE_BOSS_ENTRYWAY,    RR_FOREST_TEMPLE_BOSS_ROOM,     ENTR_FOREST_TEMPLE_BOSS_ENTRANCE },
          { EntranceType::AdultBoss, RR_FOREST_TEMPLE_BOSS_ROOM,        RR_FOREST_TEMPLE_BOSS_ENTRYWAY, ENTR_FOREST_TEMPLE_BOSS_DOOR } },
        { { EntranceType::AdultBoss, RR_FIRE_TEMPLE_BOSS_ENTRYWAY,      RR_FIRE_TEMPLE_BOSS_ROOM,       ENTR_FIRE_TEMPLE_BOSS_ENTRANCE },
          { EntranceType::AdultBoss, RR_FIRE_TEMPLE_BOSS_ROOM,          RR_FIRE_TEMPLE_BOSS_ENTRYWAY,   ENTR_FIRE_TEMPLE_BOSS_DOOR } },
        { { EntranceType::AdultBoss, RR_WATER_TEMPLE_BOSS_ENTRYWAY,     RR_WATER_TEMPLE_BOSS_ROOM,      ENTR_WATER_TEMPLE_BOSS_ENTRANCE },
          { EntranceType::AdultBoss, RR_WATER_TEMPLE_BOSS_ROOM,         RR_WATER_TEMPLE_BOSS_ENTRYWAY,  ENTR_WATER_TEMPLE_BOSS_DOOR } },
        { { EntranceType::AdultBoss, RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY,    RR_SPIRIT_TEMPLE_BOSS_ROOM,     ENTR_SPIRIT_TEMPLE_BOSS_ENTRANCE },
          { EntranceType::AdultBoss, RR_SPIRIT_TEMPLE_BOSS_ROOM,        RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY, ENTR_SPIRIT_TEMPLE_BOSS_DOOR } },
        { { EntranceType::AdultBoss, RR_SHADOW_TEMPLE_BOSS_ENTRYWAY,    RR_SHADOW_TEMPLE_BOSS_ROOM,     ENTR_SHADOW_TEMPLE_BOSS_ENTRANCE },
          { EntranceType::AdultBoss, RR_SHADOW_TEMPLE_BOSS_ROOM,        RR_SHADOW_TEMPLE_BOSS_ENTRYWAY, ENTR_SHADOW_TEMPLE_BOSS_DOOR } },

        { { EntranceType::GanonTower, RR_GANONS_TOWER_ENTRYWAY,  RR_GANONS_TOWER_STAIRS_1, ENTR_GANONS_TOWER_0 },
          { EntranceType::GanonTower, RR_GANONS_TOWER_STAIRS_1,  RR_GANONS_TOWER_ENTRYWAY, ENTR_INSIDE_GANONS_CASTLE_1 } },

        { { EntranceType::BlueWarp, RR_DEKU_TREE_BOSS_ROOM,        RR_KF_OUTSIDE_DEKU_TREE,      ENTR_KOKIRI_FOREST_DEKU_TREE_BLUE_WARP },
          NO_RETURN_ENTRANCE },
        { { EntranceType::BlueWarp, RR_DODONGOS_CAVERN_BOSS_ROOM,  RR_DEATH_MOUNTAIN_TRAIL,      ENTR_DEATH_MOUNTAIN_TRAIL_DODONGO_BLUE_WARP },
          NO_RETURN_ENTRANCE },
        { { EntranceType::BlueWarp, RR_JABU_JABUS_BELLY_BOSS_ROOM, RR_ZORAS_FOUNTAIN,            ENTR_ZORAS_FOUNTAIN_JABU_JABU_BLUE_WARP },
          NO_RETURN_ENTRANCE },
        { { EntranceType::BlueWarp, RR_FOREST_TEMPLE_BOSS_ROOM,    RR_SACRED_FOREST_MEADOW,      ENTR_SACRED_FOREST_MEADOW_FOREST_TEMPLE_BLUE_WARP },
          NO_RETURN_ENTRANCE },
        { { EntranceType::BlueWarp, RR_FIRE_TEMPLE_BOSS_ROOM,      RR_DMC_PAD_ENTRY,             ENTR_DEATH_MOUNTAIN_CRATER_FIRE_TEMPLE_BLUE_WARP },
          NO_RETURN_ENTRANCE },
        { { EntranceType::BlueWarp, RR_WATER_TEMPLE_BOSS_ROOM,     RR_LAKE_HYLIA,                ENTR_LAKE_HYLIA_WATER_TEMPLE_BLUE_WARP },
          NO_RETURN_ENTRANCE },
        { { EntranceType::BlueWarp, RR_SPIRIT_TEMPLE_BOSS_ROOM,    RR_DESERT_COLOSSUS,           ENTR_DESERT_COLOSSUS_SPIRIT_TEMPLE_BLUE_WARP },
          NO_RETURN_ENTRANCE },
        { { EntranceType::BlueWarp, RR_SHADOW_TEMPLE_BOSS_ROOM,    RR_GRAVEYARD_WARP_PAD_REGION, ENTR_GRAVEYARD_SHADOW_TEMPLE_BLUE_WARP },
          NO_RETURN_ENTRANCE },
        { { EntranceType::BlueWarp, RR_GANONS_TOWER_STAIRS_1,      RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE, ENTR_OUTSIDE_GANONS_CASTLE_1_2 },
          NO_RETURN_ENTRANCE },
        // clang-format on
    };

    auto ctx = Rando::Context::GetInstance();

    if (ctx->GetDungeon(Rando::DEKU_TREE)->IsVanilla()) {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_LOBBY_2F, RR_DEKU_TREE_2F_MIDDLE_ROOM, ENTRANCE_DOOR(0) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_2F_MIDDLE_ROOM, RR_DEKU_TREE_LOBBY_2F,
                    ENTRANCE_DOOR(1) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_2F_MIDDLE_ROOM, RR_DEKU_TREE_SLINGSHOT_ROOM,
                    ENTRANCE_DOOR(2) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_SLINGSHOT_ROOM, RR_DEKU_TREE_2F_MIDDLE_ROOM,
                    ENTRANCE_DOOR(3) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_LOBBY_3F, RR_DEKU_TREE_COMPASS_ROOM, ENTRANCE_DOOR(4) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_COMPASS_ROOM, RR_DEKU_TREE_LOBBY_3F, ENTRANCE_DOOR(5) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_LOWER, RR_DEKU_TREE_BASEMENT_SCRUB_ROOM,
                    ENTRANCE_DOOR(6) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, RR_DEKU_TREE_BASEMENT_LOWER,
                    ENTRANCE_DOOR(7) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_SCRUB_ROOM,
                    RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT, ENTRANCE_DOOR(8) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_WATER_ROOM_FRONT,
                    RR_DEKU_TREE_BASEMENT_SCRUB_ROOM, ENTRANCE_DOOR(9) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK, RR_DEKU_TREE_BASEMENT_TORCH_ROOM,
                    ENTRANCE_DOOR(10) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_TORCH_ROOM, RR_DEKU_TREE_BASEMENT_WATER_ROOM_BACK,
                    ENTRANCE_DOOR(11) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_TORCH_ROOM, RR_DEKU_TREE_BASEMENT_BACK_LOBBY,
                    ENTRANCE_DOOR(12) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_BACK_LOBBY, RR_DEKU_TREE_BASEMENT_TORCH_ROOM,
                    ENTRANCE_DOOR(13) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_BACK_LOBBY, RR_DEKU_TREE_BASEMENT_BACK_ROOM,
                    ENTRANCE_DOOR(14) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_BASEMENT_BACK_ROOM, RR_DEKU_TREE_BASEMENT_BACK_LOBBY,
                    ENTRANCE_DOOR(15) } },
            });
    } else {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_2F, RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, ENTRANCE_DOOR(0) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, RR_DEKU_TREE_MQ_2F,
                    ENTRANCE_DOOR(1) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_EYE_TARGET_ROOM, RR_DEKU_TREE_MQ_COMPASS_ROOM,
                    ENTRANCE_DOOR(2) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_COMPASS_ROOM, RR_DEKU_TREE_MQ_EYE_TARGET_ROOM,
                    ENTRANCE_DOOR(3) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_3F, RR_DEKU_TREE_MQ_SLINGSHOT_ROOM, ENTRANCE_DOOR(4) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_SLINGSHOT_ROOM, RR_DEKU_TREE_MQ_3F,
                    ENTRANCE_DOOR(5) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT, RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM,
                    ENTRANCE_DOOR(6) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM, RR_DEKU_TREE_MQ_BASEMENT,
                    ENTRANCE_DOOR(7) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM,
                    RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT, ENTRANCE_DOOR(8) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_FRONT,
                    RR_DEKU_TREE_MQ_BASEMENT_SOUTHEAST_ROOM, ENTRANCE_DOOR(9) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK,
                    RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM, ENTRANCE_DOOR(10) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM,
                    RR_DEKU_TREE_MQ_BASEMENT_WATER_ROOM_BACK, ENTRANCE_DOOR(11) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM,
                    RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, ENTRANCE_DOOR(12) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM,
                    RR_DEKU_TREE_MQ_BASEMENT_SOUTHWEST_ROOM, ENTRANCE_DOOR(13) } },
                { { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM, RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM,
                    ENTRANCE_DOOR(14) },
                  { EntranceType::DoorDekuTree, RR_DEKU_TREE_MQ_BASEMENT_BACK_ROOM, RR_DEKU_TREE_MQ_BASEMENT_GRAVE_ROOM,
                    ENTRANCE_DOOR(15) } },
            });
    }

    if (ctx->GetDungeon(Rando::DODONGOS_CAVERN)->IsVanilla()) {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_LOBBY,
                                            RR_DODONGOS_CAVERN_BOSS_AREA, ENTRANCE_DOOR(16) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_BOSS_AREA,
                                            RR_DODONGOS_CAVERN_LOBBY, ENTRANCE_DOOR(17) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_BOSS_AREA,
                                            RR_DODONGOS_CAVERN_BACK_ROOM, ENTRANCE_DOOR(18) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_BACK_ROOM,
                                            RR_DODONGOS_CAVERN_BOSS_AREA, ENTRANCE_DOOR(19) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_LOBBY_SWITCH,
                                            RR_DODONGOS_CAVERN_DODONGO_ROOM, ENTRANCE_DOOR(20) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_DODONGO_ROOM,
                                            RR_DODONGOS_CAVERN_LOBBY_SWITCH, ENTRANCE_DOOR(21) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_DODONGO_ROOM,
                                            RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM, ENTRANCE_DOOR(22) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_NEAR_DODONGO_ROOM,
                                            RR_DODONGOS_CAVERN_DODONGO_ROOM, ENTRANCE_DOOR(23) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_DODONGO_ROOM,
                                            RR_DODONGOS_CAVERN_LOWER_LIZALFOS, ENTRANCE_DOOR(24) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_LOWER_LIZALFOS,
                                            RR_DODONGOS_CAVERN_DODONGO_ROOM, ENTRANCE_DOOR(25) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_LOWER_LIZALFOS,
                                            RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS, ENTRANCE_DOOR(26) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_NEAR_LOWER_LIZALFOS,
                                            RR_DODONGOS_CAVERN_LOWER_LIZALFOS, ENTRANCE_DOOR(27) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_SE_CORRIDOR,
                                            RR_DODONGOS_CAVERN_SE_ROOM, ENTRANCE_DOOR(28) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_SE_ROOM,
                                            RR_DODONGOS_CAVERN_SE_CORRIDOR, ENTRANCE_DOOR(29) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_LOBBY,
                                            RR_DODONGOS_CAVERN_STAIRS_LOWER, ENTRANCE_DOOR(30) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_STAIRS_LOWER,
                                            RR_DODONGOS_CAVERN_LOBBY, ENTRANCE_DOOR(31) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_STAIRS_LOWER,
                                            RR_DODONGOS_CAVERN_COMPASS_ROOM, ENTRANCE_DOOR(32) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_COMPASS_ROOM,
                                            RR_DODONGOS_CAVERN_STAIRS_LOWER, ENTRANCE_DOOR(33) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_STAIRS_UPPER,
                                            RR_DODONGOS_CAVERN_ARMOS_ROOM, ENTRANCE_DOOR(34) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_ARMOS_ROOM,
                                            RR_DODONGOS_CAVERN_STAIRS_UPPER, ENTRANCE_DOOR(35) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_ARMOS_ROOM,
                                            RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, ENTRANCE_DOOR(36) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER,
                                            RR_DODONGOS_CAVERN_ARMOS_ROOM, ENTRANCE_DOOR(37) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER,
                                            RR_DODONGOS_CAVERN_2F_SIDE_ROOM, ENTRANCE_DOOR(38) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_2F_SIDE_ROOM,
                                            RR_DODONGOS_CAVERN_BOMB_ROOM_LOWER, ENTRANCE_DOOR(39) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM,
                                            RR_DODONGOS_CAVERN_UPPER_LIZALFOS, ENTRANCE_DOOR(40) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_UPPER_LIZALFOS,
                                            RR_DODONGOS_CAVERN_FIRST_SLINGSHOT_ROOM, ENTRANCE_DOOR(41) } },
                                        { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_UPPER_LIZALFOS,
                                            RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM, ENTRANCE_DOOR(42) },
                                          { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_SECOND_SLINGSHOT_ROOM,
                                            RR_DODONGOS_CAVERN_UPPER_LIZALFOS, ENTRANCE_DOOR(43) } },
                                    });
    } else {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_LOBBY, RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH,
                    ENTRANCE_DOOR(16) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_BEHIND_MOUTH, RR_DODONGOS_CAVERN_MQ_LOBBY,
                    ENTRANCE_DOOR(17) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE,
                    RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE, ENTRANCE_DOOR(18) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_BACK_SWITCH_GRAVE,
                    RR_DODONGOS_CAVERN_MQ_BACK_BEHIND_FIRE, ENTRANCE_DOOR(19) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_LOBBY, RR_DODONGOS_CAVERN_MQ_POES_ROOM,
                    ENTRANCE_DOOR(20) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_POES_ROOM, RR_DODONGOS_CAVERN_MQ_LOBBY,
                    ENTRANCE_DOOR(21) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_POES_ROOM,
                    RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM, ENTRANCE_DOOR(22) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_MAD_SCRUB_ROOM,
                    RR_DODONGOS_CAVERN_MQ_POES_ROOM, ENTRANCE_DOOR(23) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_POES_ROOM,
                    RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS, ENTRANCE_DOOR(24) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS,
                    RR_DODONGOS_CAVERN_MQ_POES_ROOM, ENTRANCE_DOOR(25) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS,
                    RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, ENTRANCE_DOOR(27) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,
                    RR_DODONGOS_CAVERN_MQ_LOWER_LIZALFOS, ENTRANCE_DOOR(26) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE,
                    RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE_SCRUB, ENTRANCE_DOOR(28) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE_SCRUB,
                    RR_DODONGOS_CAVERN_MQ_LOWER_RIGHT_SIDE, ENTRANCE_DOOR(29) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_LOBBY, RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER,
                    ENTRANCE_DOOR(30) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, RR_DODONGOS_CAVERN_MQ_LOBBY,
                    ENTRANCE_DOOR(31) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER,
                    RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_MUD_WALL, ENTRANCE_DOOR(32) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_MUD_WALL,
                    RR_DODONGOS_CAVERN_MQ_STAIRS_LOWER, ENTRANCE_DOOR(33) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS,
                    RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM, ENTRANCE_DOOR(34) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM,
                    RR_DODONGOS_CAVERN_MQ_STAIRS_PAST_BIG_SKULLTULAS, ENTRANCE_DOOR(35) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM,
                    RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, ENTRANCE_DOOR(36) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER,
                    RR_DODONGOS_CAVERN_MQ_DODONGO_ROOM, ENTRANCE_DOOR(37) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER,
                    RR_DODONGOS_CAVERN_MQ_LARVAE_ROOM, ENTRANCE_DOOR(38) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_LARVAE_ROOM,
                    RR_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_LOWER, ENTRANCE_DOOR(39) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM,
                    RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS, ENTRANCE_DOOR(40) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,
                    RR_DODONGOS_CAVERN_MQ_BIG_BLOCK_ROOM, ENTRANCE_DOOR(41) } },
                { { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS,
                    RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM, ENTRANCE_DOOR(42) },
                  { EntranceType::DoorDodongosCavern, RR_DODONGOS_CAVERN_MQ_TWO_FIRES_ROOM,
                    RR_DODONGOS_CAVERN_MQ_UPPER_LIZALFOS, ENTRANCE_DOOR(43) } },
            });
    }

    if (ctx->GetDungeon(Rando::JABU_JABUS_BELLY)->IsVanilla()) {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_BEGINNING,
                                            RR_JABU_JABUS_BELLY_LIFT_ROOM, ENTRANCE_DOOR(44) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_LIFT_ROOM,
                                            RR_JABU_JABUS_BELLY_BEGINNING, ENTRANCE_DOOR(45) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_LIFT_ROOM,
                                            RR_JABU_JABUS_BELLY_HOLES_ROOM, ENTRANCE_DOOR(46) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_HOLES_ROOM,
                                            RR_JABU_JABUS_BELLY_LIFT_ROOM, ENTRANCE_DOOR(47) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_HOLES_BASEMENT,
                                            RR_JABU_JABUS_BELLY_B1_JIGGLY, ENTRANCE_DOOR(48) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_B1_JIGGLY,
                                            RR_JABU_JABUS_BELLY_HOLES_BASEMENT, ENTRANCE_DOOR(49) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_HOLES_LOWER_DOOR_LEDGE,
                                            RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH, ENTRANCE_DOOR(50) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_NORTH,
                                            RR_JABU_JABUS_BELLY_HOLES_LOWER_DOOR_LEDGE, ENTRANCE_DOOR(51) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH,
                                            RR_JABU_JABUS_BELLY_LIFT_ROOM, ENTRANCE_DOOR(52) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_LIFT_ROOM,
                                            RR_JABU_JABUS_BELLY_WATER_SWITCH_ROOM_SOUTH, ENTRANCE_DOOR(53) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_HOLES_ROOM,
                                            RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, ENTRANCE_DOOR(54) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_FORKED_CORRIDOR,
                                            RR_JABU_JABUS_BELLY_HOLES_ROOM, ENTRANCE_DOOR(55) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_FORKED_CORRIDOR,
                                            RR_JABU_JABUS_BELLY_FORK_WEST, ENTRANCE_DOOR(56) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_FORK_WEST,
                                            RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, ENTRANCE_DOOR(57) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_TO_FORK_NORTH_WEST,
                                            RR_JABU_JABUS_BELLY_FORK_NORTH_WEST, ENTRANCE_DOOR(58) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_FORK_NORTH_WEST,
                                            RR_JABU_JABUS_BELLY_TO_FORK_NORTH_WEST, ENTRANCE_DOOR(59) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_FORKED_CORRIDOR,
                                            RR_JABU_JABUS_BELLY_FORK_EAST, ENTRANCE_DOOR(60) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_FORK_EAST,
                                            RR_JABU_JABUS_BELLY_FORKED_CORRIDOR, ENTRANCE_DOOR(61) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_TO_FORK_NORTH_EAST,
                                            RR_JABU_JABUS_BELLY_FORK_NORTH_EAST, ENTRANCE_DOOR(62) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_FORK_NORTH_EAST,
                                            RR_JABU_JABUS_BELLY_TO_FORK_NORTH_EAST, ENTRANCE_DOOR(63) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_TO_FORK_NORTH,
                                            RR_JABU_JABUS_BELLY_FORK_NORTH, ENTRANCE_DOOR(64) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_FORK_NORTH,
                                            RR_JABU_JABUS_BELLY_TO_FORK_NORTH, ENTRANCE_DOOR(65) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_BIGOCTO_LEDGE,
                                            RR_JABU_JABUS_BELLY_BIGOCTO, ENTRANCE_DOOR(66) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_BIGOCTO,
                                            RR_JABU_JABUS_BELLY_BIGOCTO_LEDGE, ENTRANCE_DOOR(67) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO,
                                            RR_JABU_JABUS_BELLY_JIGGLIES_ROOM, ENTRANCE_DOOR(68) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_JIGGLIES_ROOM,
                                            RR_JABU_JABUS_BELLY_ABOVE_BIGOCTO, ENTRANCE_DOOR(69) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_JIGGLIES_ROOM,
                                            RR_JABU_JABUS_BELLY_LIFT_UPPER, ENTRANCE_DOOR(70) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_LIFT_UPPER,
                                            RR_JABU_JABUS_BELLY_JIGGLIES_ROOM, ENTRANCE_DOOR(71) } },
                                        { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_LIFT_ROOM,
                                            RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM, ENTRANCE_DOOR(72) },
                                          { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_NEAR_BOSS_ROOM,
                                            RR_JABU_JABUS_BELLY_LIFT_ROOM, ENTRANCE_DOOR(73) } },
                                    });
    } else {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_BEGINNING, RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,
                    ENTRANCE_DOOR(44) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM, RR_JABU_JABUS_BELLY_MQ_BEGINNING,
                    ENTRANCE_DOOR(45) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM, RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,
                    ENTRANCE_DOOR(46) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,
                    ENTRANCE_DOOR(47) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_INVISIBLE_KEESE_ROOM, ENTRANCE_DOOR(48) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_INVISIBLE_KEESE_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, ENTRANCE_DOOR(49) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM, ENTRANCE_DOOR(50) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, ENTRANCE_DOOR(51) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM_PAST_GEYSER,
                    RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM, ENTRANCE_DOOR(52) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_WATER_SWITCH_ROOM_PAST_GEYSER, ENTRANCE_DOOR(53) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, ENTRANCE_DOOR(54) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR,
                    RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, ENTRANCE_DOOR(55) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR,
                    RR_JABU_JABUS_BELLY_MQ_FORK_WEST, ENTRANCE_DOOR(56) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORK_WEST,
                    RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, ENTRANCE_DOOR(57) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR,
                    RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_WEST, ENTRANCE_DOOR(58) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_WEST,
                    RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, ENTRANCE_DOOR(59) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR,
                    RR_JABU_JABUS_BELLY_MQ_FORK_EAST, ENTRANCE_DOOR(60) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORK_EAST,
                    RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, ENTRANCE_DOOR(61) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR,
                    RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_EAST, ENTRANCE_DOOR(62) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORK_NORTH_EAST,
                    RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, ENTRANCE_DOOR(63) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR,
                    RR_JABU_JABUS_BELLY_MQ_FORK_NORTH, ENTRANCE_DOOR(64) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_FORK_NORTH,
                    RR_JABU_JABUS_BELLY_MQ_FORKED_CORRIDOR, ENTRANCE_DOOR(65) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM, RR_JABU_JABUS_BELLY_MQ_BIGOCTO,
                    ENTRANCE_DOOR(66) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_BIGOCTO, RR_JABU_JABUS_BELLY_MQ_HOLES_ROOM,
                    ENTRANCE_DOOR(67) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_ABOVE_BIGOCTO,
                    RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM, ENTRANCE_DOOR(68) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_ABOVE_BIGOCTO, ENTRANCE_DOOR(69) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_ABOVE_LIFT_ROOM, ENTRANCE_DOOR(70) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_ABOVE_LIFT_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_JIGGLIES_ROOM, ENTRANCE_DOOR(71) } },
                { { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE,
                    RR_JABU_JABUS_BELLY_MQ_NEAR_BOSS_ROOM, ENTRANCE_DOOR(72) },
                  { EntranceType::DoorJabuJabu, RR_JABU_JABUS_BELLY_MQ_NEAR_BOSS_ROOM,
                    RR_JABU_JABUS_BELLY_MQ_LIFT_ROOM_EAST_LEDGE, ENTRANCE_DOOR(73) } },
            });
    }

    if (ctx->GetDungeon(Rando::FOREST_TEMPLE)->IsVanilla()) {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_TREES,
                                            RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_LOWER, ENTRANCE_DOOR(74) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_LOWER,
                                            RR_FOREST_TEMPLE_TREES, ENTRANCE_DOOR(75) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_UPPER,
                                            RR_FOREST_TEMPLE_LOBBY, ENTRANCE_DOOR(76) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_LOBBY,
                                            RR_FOREST_TEMPLE_OVERGROWN_HALLWAY_UPPER, ENTRANCE_DOOR(77) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_LOBBY,
                                            RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY, ENTRANCE_DOOR(78) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY,
                                            RR_FOREST_TEMPLE_LOBBY, ENTRANCE_DOOR(79) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY_DOORMAT,
                                            RR_FOREST_TEMPLE_BLOCK_PUSH_FLOOR, ENTRANCE_DOOR(80) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_BLOCK_PUSH_FLOOR,
                                            RR_FOREST_TEMPLE_RED_DOORMAT_HALLWAY_DOORMAT, ENTRANCE_DOOR(81) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE,
                                            RR_FOREST_TEMPLE_NW_COURTYARD_UPPER, ENTRANCE_DOOR(82) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_NW_COURTYARD_UPPER,
                                            RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM_COURTYARD_ALCOVE, ENTRANCE_DOOR(83) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_NW_COURTYARD_UPPER,
                                            RR_FOREST_TEMPLE_FLOORMASTER_ROOM, ENTRANCE_DOOR(84) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_FLOORMASTER_ROOM,
                                            RR_FOREST_TEMPLE_NW_COURTYARD_UPPER, ENTRANCE_DOOR(85) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_NW_COURTYARD_UPPER,
                                            RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST, ENTRANCE_DOOR(86) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_BELOW_BOSS_KEY_CHEST,
                                            RR_FOREST_TEMPLE_NW_COURTYARD_UPPER, ENTRANCE_DOOR(87) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_NW_COURTYARD_UPPER_ALCOVE,
                                            RR_FOREST_TEMPLE_MAP_ROOM, ENTRANCE_DOOR(88) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MAP_ROOM,
                                            RR_FOREST_TEMPLE_NW_COURTYARD_UPPER_ALCOVE, ENTRANCE_DOOR(89) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MAP_ROOM,
                                            RR_FOREST_TEMPLE_NE_COURTYARD_UPPER, ENTRANCE_DOOR(90) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_NE_COURTYARD_UPPER,
                                            RR_FOREST_TEMPLE_MAP_ROOM, ENTRANCE_DOOR(91) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_NW_COURTYARD_LOWER,
                                            RR_FOREST_TEMPLE_LOBBY, ENTRANCE_DOOR(92) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_LOBBY,
                                            RR_FOREST_TEMPLE_NW_COURTYARD_LOWER, ENTRANCE_DOOR(93) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_NE_COURTYARD_SCARECROW_LEDGE,
                                            RR_FOREST_TEMPLE_FALLING_ROOM, ENTRANCE_DOOR(94) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_FALLING_ROOM,
                                            RR_FOREST_TEMPLE_NE_COURTYARD_SCARECROW_LEDGE, ENTRANCE_DOOR(95) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_NE_COURTYARD_LOWER,
                                            RR_FOREST_TEMPLE_LOBBY, ENTRANCE_DOOR(96) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_LOBBY,
                                            RR_FOREST_TEMPLE_NE_COURTYARD_LOWER, ENTRANCE_DOOR(97) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_FALLING_ROOM,
                                            RR_FOREST_TEMPLE_GREEN_POE_ROOM, ENTRANCE_DOOR(98) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_GREEN_POE_ROOM,
                                            RR_FOREST_TEMPLE_FALLING_ROOM, ENTRANCE_DOOR(99) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_GREEN_POE_ROOM,
                                            RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY_DOORMAT, ENTRANCE_DOOR(100) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY_DOORMAT,
                                            RR_FOREST_TEMPLE_GREEN_POE_ROOM, ENTRANCE_DOOR(101) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY,
                                            RR_FOREST_TEMPLE_LOBBY, ENTRANCE_DOOR(102) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_LOBBY,
                                            RR_FOREST_TEMPLE_BLUE_DOORMAT_HALLWAY, ENTRANCE_DOOR(103) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_LOBBY,
                                            RR_FOREST_TEMPLE_NORTH_HALLWAY, ENTRANCE_DOOR(104) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_NORTH_HALLWAY,
                                            RR_FOREST_TEMPLE_LOBBY, ENTRANCE_DOOR(105) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_NORTH_HALLWAY,
                                            RR_FOREST_TEMPLE_LOWER_STALFOS, ENTRANCE_DOOR(106) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_LOWER_STALFOS,
                                            RR_FOREST_TEMPLE_NORTH_HALLWAY, ENTRANCE_DOOR(107) } },
                                        // TODO change twisted/straight to one RR with logic
                                        //{ { EntranceType::DoorForest, RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,
                                        // RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED, ENTRANCE_DOOR(108) },
                                        //  { EntranceType::DoorForest, RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED,
                                        //  RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, ENTRANCE_DOOR(109) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_NW_HALLWAY_TWISTED,
                                            RR_FOREST_TEMPLE_RED_POE_ROOM, ENTRANCE_DOOR(110) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_RED_POE_ROOM,
                                            RR_FOREST_TEMPLE_NW_HALLWAY_TWISTED, ENTRANCE_DOOR(111) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_RED_POE_ROOM,
                                            RR_FOREST_TEMPLE_UPPER_STALFOS, ENTRANCE_DOOR(112) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_UPPER_STALFOS,
                                            RR_FOREST_TEMPLE_RED_POE_ROOM, ENTRANCE_DOOR(113) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_UPPER_STALFOS,
                                            RR_FOREST_TEMPLE_BLUE_POE_ROOM, ENTRANCE_DOOR(114) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_BLUE_POE_ROOM,
                                            RR_FOREST_TEMPLE_UPPER_STALFOS, ENTRANCE_DOOR(115) } },
                                        // TODO twisted/straight logic
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_BLUE_POE_ROOM,
                                            RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED, ENTRANCE_DOOR(116) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED,
                                            RR_FOREST_TEMPLE_BLUE_POE_ROOM, ENTRANCE_DOOR(117) } },
                                        //{ { EntranceType::DoorForest, RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED,
                                        // RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, ENTRANCE_DOOR(118) },
                                        //  { EntranceType::DoorForest, RR_FOREST_TEMPLE_FROZEN_EYE_ROOM,
                                        //  RR_FOREST_TEMPLE_NE_CORRIDOR_STRAIGHTENED, ENTRANCE_DOOR(119) } },
                                    });
    } else {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_TREES,
                                            RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_LOWER, ENTRANCE_DOOR(74) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_LOWER,
                                            RR_FOREST_TEMPLE_MQ_TREES, ENTRANCE_DOOR(75) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_UPPER,
                                            RR_FOREST_TEMPLE_MQ_LOBBY, ENTRANCE_DOOR(76) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_LOBBY,
                                            RR_FOREST_TEMPLE_MQ_OVERGROWN_HALLWAY_UPPER, ENTRANCE_DOOR(77) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_LOBBY,
                                            RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY, ENTRANCE_DOOR(78) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY,
                                            RR_FOREST_TEMPLE_MQ_LOBBY, ENTRANCE_DOOR(79) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY,
                                            RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE, ENTRANCE_DOOR(80) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_LOWER_BLOCK_PUZZLE,
                                            RR_FOREST_TEMPLE_MQ_RED_DOORMAT_HALLWAY, ENTRANCE_DOOR(81) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE,
                                            RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, ENTRANCE_DOOR(82) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE,
                                            RR_FOREST_TEMPLE_MQ_INDOOR_LEDGE, ENTRANCE_DOOR(83) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE,
                                            RR_FOREST_TEMPLE_MQ_REDEAD_ROOM, ENTRANCE_DOOR(84) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_REDEAD_ROOM,
                                            RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, ENTRANCE_DOOR(85) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE,
                                            RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM, ENTRANCE_DOOR(86) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_FLOORMASTER_ROOM,
                                            RR_FOREST_TEMPLE_MQ_NW_COURTYARD_LEDGE, ENTRANCE_DOOR(87) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NW_COURTYARD,
                                            RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE, ENTRANCE_DOOR(88) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE,
                                            RR_FOREST_TEMPLE_MQ_NW_COURTYARD, ENTRANCE_DOOR(89) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE,
                                            RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES, ENTRANCE_DOOR(90) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES,
                                            RR_FOREST_TEMPLE_MQ_NORTH_PASSAGE, ENTRANCE_DOOR(91) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NE_COURTYARD,
                                            RR_FOREST_TEMPLE_MQ_LOBBY, ENTRANCE_DOOR(92) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_LOBBY,
                                            RR_FOREST_TEMPLE_MQ_NW_COURTYARD, ENTRANCE_DOOR(93) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES,
                                            RR_FOREST_TEMPLE_MQ_FALLING_ROOM, ENTRANCE_DOOR(94) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_FALLING_ROOM,
                                            RR_FOREST_TEMPLE_MQ_COURTYARD_TOP_LEDGES, ENTRANCE_DOOR(95) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NE_COURTYARD,
                                            RR_FOREST_TEMPLE_MQ_LOBBY, ENTRANCE_DOOR(96) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_LOBBY,
                                            RR_FOREST_TEMPLE_MQ_NE_COURTYARD, ENTRANCE_DOOR(97) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_FALLING_ROOM,
                                            RR_FOREST_TEMPLE_MQ_AMY_ROOM, ENTRANCE_DOOR(98) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_AMY_ROOM,
                                            RR_FOREST_TEMPLE_MQ_FALLING_ROOM, ENTRANCE_DOOR(99) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_AMY_ROOM,
                                            RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY, ENTRANCE_DOOR(100) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY,
                                            RR_FOREST_TEMPLE_MQ_AMY_ROOM, ENTRANCE_DOOR(101) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY,
                                            RR_FOREST_TEMPLE_MQ_LOBBY, ENTRANCE_DOOR(102) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_LOBBY,
                                            RR_FOREST_TEMPLE_MQ_BLUE_DOORMAT_HALLWAY, ENTRANCE_DOOR(103) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_LOBBY,
                                            RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY, ENTRANCE_DOOR(104) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY,
                                            RR_FOREST_TEMPLE_MQ_LOBBY, ENTRANCE_DOOR(105) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY,
                                            RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM, ENTRANCE_DOOR(106) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_WOLFOS_ROOM,
                                            RR_FOREST_TEMPLE_MQ_NORTH_HALLWAY, ENTRANCE_DOOR(107) } },
                                        // TODO change twisted/straight to one RR with logic
                                        //{ { EntranceType::DoorForest, RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM,
                                        // RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED, ENTRANCE_DOOR(108) },
                                        //  { EntranceType::DoorForest, RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED,
                                        //  RR_FOREST_TEMPLE_BLOCK_PUSH_ROOM, ENTRANCE_DOOR(109) } },
                                        //{ { EntranceType::DoorForest, RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED,
                                        // RR_FOREST_TEMPLE_RED_POE_ROOM, ENTRANCE_DOOR(110) },
                                        //  { EntranceType::DoorForest, RR_FOREST_TEMPLE_RED_POE_ROOM,
                                        //  RR_FOREST_TEMPLE_NW_CORRIDOR_TWISTED, ENTRANCE_DOOR(111) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_JOELLE_ROOM,
                                            RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM, ENTRANCE_DOOR(112) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM,
                                            RR_FOREST_TEMPLE_MQ_JOELLE_ROOM, ENTRANCE_DOOR(113) } },
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM,
                                            RR_FOREST_TEMPLE_MQ_BETH_ROOM, ENTRANCE_DOOR(114) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_MQ_BETH_ROOM,
                                            RR_FOREST_TEMPLE_MQ_3_STALFOS_ROOM, ENTRANCE_DOOR(115) } },
                                        // TODO twisted/straight logic
                                        { { EntranceType::DoorForest, RR_FOREST_TEMPLE_BLUE_POE_ROOM,
                                            RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED, ENTRANCE_DOOR(116) },
                                          { EntranceType::DoorForest, RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED,
                                            RR_FOREST_TEMPLE_BLUE_POE_ROOM, ENTRANCE_DOOR(117) } },
                                        //{ { EntranceType::DoorForest, RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED,
                                        // RR_FOREST_TEMPLE_FROZEN_EYE_ROOM, ENTRANCE_DOOR(118) },
                                        //  { EntranceType::DoorForest, RR_FOREST_TEMPLE_FROZEN_EYE_ROOM,
                                        //  RR_FOREST_TEMPLE_NE_HALLWAY_STRAIGHTENED, ENTRANCE_DOOR(119) } },
                                    });
    }

    if (ctx->GetDungeon(Rando::FIRE_TEMPLE)->IsVanilla()) {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FOYER, RR_FIRE_TEMPLE_NEAR_BOSS_ROOM, ENTRANCE_DOOR(120) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_NEAR_BOSS_ROOM, RR_FIRE_TEMPLE_FOYER, ENTRANCE_DOOR(121) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FOYER, RR_FIRE_TEMPLE_LOOP_CAGE_FOYER_SIDE,
                    ENTRANCE_DOOR(122) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_LOOP_CAGE_FOYER_SIDE, RR_FIRE_TEMPLE_FOYER,
                    ENTRANCE_DOOR(123) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FOYER, RR_FIRE_TEMPLE_LOOP_HEXAGON_ROOM,
                    ENTRANCE_DOOR(124) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_LOOP_HEXAGON_ROOM, RR_FIRE_TEMPLE_FOYER,
                    ENTRANCE_DOOR(125) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_LOOP_HEXAGON_ROOM, RR_FIRE_TEMPLE_LOOP_5_TILE_ROOM,
                    ENTRANCE_DOOR(126) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_LOOP_5_TILE_ROOM, RR_FIRE_TEMPLE_LOOP_HEXAGON_ROOM,
                    ENTRANCE_DOOR(127) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_LOOP_5_TILE_ROOM, RR_FIRE_TEMPLE_LOOP_FLARE_DANCER,
                    ENTRANCE_DOOR(128) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, RR_FIRE_TEMPLE_LOOP_5_TILE_ROOM,
                    ENTRANCE_DOOR(129) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_LOOP_FLARE_DANCER, RR_FIRE_TEMPLE_LOOP_CAGE_SWITCH,
                    ENTRANCE_DOOR(130) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_LOOP_CAGE_SWITCH, RR_FIRE_TEMPLE_LOOP_FLARE_DANCER,
                    ENTRANCE_DOOR(131) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FOYER, RR_FIRE_TEMPLE_BIG_LAVA_ROOM, ENTRANCE_DOOR(132) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_BIG_LAVA_ROOM, RR_FIRE_TEMPLE_FOYER, ENTRANCE_DOOR(133) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_BIG_LAVA_ROOM, RR_FIRE_TEMPLE_1F_CURVED_CAGE,
                    ENTRANCE_DOOR(134) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_1F_CURVED_CAGE, RR_FIRE_TEMPLE_BIG_LAVA_ROOM,
                    ENTRANCE_DOOR(135) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_BIG_LAVA_ROOM, RR_FIRE_TEMPLE_8_TILE_ROOM,
                    ENTRANCE_DOOR(136) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_8_TILE_ROOM, RR_FIRE_TEMPLE_BIG_LAVA_ROOM,
                    ENTRANCE_DOOR(137) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_BIG_LAVA_ROOM, RR_FIRE_TEMPLE_STRAIGHTFORWARD_CAGE,
                    ENTRANCE_DOOR(138) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_STRAIGHTFORWARD_CAGE, RR_FIRE_TEMPLE_BIG_LAVA_ROOM,
                    ENTRANCE_DOOR(139) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_BIG_LAVA_ROOM, RR_FIRE_TEMPLE_LAVA_GEYSER_1F,
                    ENTRANCE_DOOR(140) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_LAVA_GEYSER_1F, RR_FIRE_TEMPLE_BIG_LAVA_ROOM,
                    ENTRANCE_DOOR(141) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_LAVA_GEYSER_2F, RR_FIRE_TEMPLE_SHORTCUT_ROOM,
                    ENTRANCE_DOOR(142) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_SHORTCUT_ROOM, RR_FIRE_TEMPLE_LAVA_GEYSER_2F,
                    ENTRANCE_DOOR(143) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_SHORTCUT_ROOM, RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER,
                    ENTRANCE_DOOR(144) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, RR_FIRE_TEMPLE_SHORTCUT_ROOM,
                    ENTRANCE_DOOR(145) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_GS_CLIMB_4F, RR_FIRE_TEMPLE_GS_CLIMB_5F,
                    ENTRANCE_DOOR(146) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_GS_CLIMB_5F, RR_FIRE_TEMPLE_GS_CLIMB_4F,
                    ENTRANCE_DOOR(147) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_GS_CLIMB_5F, RR_FIRE_TEMPLE_5F_RUINS, ENTRANCE_DOOR(148) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_5F_RUINS, RR_FIRE_TEMPLE_GS_CLIMB_5F, ENTRANCE_DOOR(149) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, RR_FIRE_TEMPLE_NARROW_PATH_ROOM,
                    ENTRANCE_DOOR(150) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_NARROW_PATH_ROOM, RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER,
                    ENTRANCE_DOOR(151) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER, RR_FIRE_TEMPLE_3F_CURVED_CAGE,
                    ENTRANCE_DOOR(152) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_3F_CURVED_CAGE, RR_FIRE_TEMPLE_BOULDER_MAZE_LOWER,
                    ENTRANCE_DOOR(153) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER, RR_FIRE_TEMPLE_FIRE_WALL_CHASE,
                    ENTRANCE_DOOR(154) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_WALL_CHASE, RR_FIRE_TEMPLE_BOULDER_MAZE_UPPER,
                    ENTRANCE_DOOR(155) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_WALL_CAGE, RR_FIRE_TEMPLE_NARROW_PATH_ROOM,
                    ENTRANCE_DOOR(156) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_NARROW_PATH_ROOM, RR_FIRE_TEMPLE_FIRE_WALL_CAGE,
                    ENTRANCE_DOOR(157) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_WALL_CHASE, RR_FIRE_TEMPLE_NARROW_PATH_ROOM,
                    ENTRANCE_DOOR(158) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_NARROW_PATH_ROOM, RR_FIRE_TEMPLE_FIRE_WALL_CHASE,
                    ENTRANCE_DOOR(159) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_WALL_CHASE, RR_FIRE_TEMPLE_CORRIDOR,
                    ENTRANCE_DOOR(160) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_CORRIDOR, RR_FIRE_TEMPLE_FIRE_WALL_CHASE,
                    ENTRANCE_DOOR(161) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_CORRIDOR, RR_FIRE_TEMPLE_FIRE_MAZE_MAIN,
                    ENTRANCE_DOOR(162) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_MAZE_MAIN, RR_FIRE_TEMPLE_CORRIDOR,
                    ENTRANCE_DOOR(163) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_MAZE_PLATFORMS, RR_FIRE_TEMPLE_SOT_CAGE_UPPER_DOOR,
                    ENTRANCE_DOOR(164) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_SOT_CAGE_UPPER_DOOR, RR_FIRE_TEMPLE_FIRE_MAZE_PLATFORMS,
                    ENTRANCE_DOOR(165) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_MAZE_MAIN, RR_FIRE_TEMPLE_CAGELESS_CHEST_ROOM,
                    ENTRANCE_DOOR(166) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_CAGELESS_CHEST_ROOM, RR_FIRE_TEMPLE_FIRE_MAZE_MAIN,
                    ENTRANCE_DOOR(167) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_MAZE_SWITCH, RR_FIRE_TEMPLE_SOT_CAGE_LOWER,
                    ENTRANCE_DOOR(168) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_SOT_CAGE_LOWER, RR_FIRE_TEMPLE_FIRE_MAZE_SWITCH,
                    ENTRANCE_DOOR(169) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_MAZE_MAIN, RR_FIRE_TEMPLE_SOT_CAGE_LOWER,
                    ENTRANCE_DOOR(170) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_SOT_CAGE_LOWER, RR_FIRE_TEMPLE_FIRE_MAZE_MAIN,
                    ENTRANCE_DOOR(171) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_FIRE_MAZE_PAST_WALL, RR_FIRE_TEMPLE_3F_FLARE_DANCER,
                    ENTRANCE_DOOR(172) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_3F_FLARE_DANCER, RR_FIRE_TEMPLE_FIRE_MAZE_PAST_WALL,
                    ENTRANCE_DOOR(173) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_ABOVE_3F_FLARE_DANCER, RR_FIRE_TEMPLE_SWITCH_CLIMB,
                    ENTRANCE_DOOR(174) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_SWITCH_CLIMB, RR_FIRE_TEMPLE_ABOVE_3F_FLARE_DANCER,
                    ENTRANCE_DOOR(175) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_SWITCH_CLIMB, RR_FIRE_TEMPLE_NARROW_STAIRS,
                    ENTRANCE_DOOR(176) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_NARROW_STAIRS, RR_FIRE_TEMPLE_SWITCH_CLIMB,
                    ENTRANCE_DOOR(177) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_NARROW_STAIRS_4F, RR_FIRE_TEMPLE_TOP_OF_COLLAPSING_STAIRS,
                    ENTRANCE_DOOR(178) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_TOP_OF_COLLAPSING_STAIRS, RR_FIRE_TEMPLE_NARROW_STAIRS_4F,
                    ENTRANCE_DOOR(179) } },
                { { EntranceType::DoorFire, RR_FIRE_TEMPLE_BASE_OF_COLLAPSING_STAIRS, RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE,
                    ENTRANCE_DOOR(180) },
                  { EntranceType::DoorFire, RR_FIRE_TEMPLE_ABOVE_FIRE_MAZE, RR_FIRE_TEMPLE_BASE_OF_COLLAPSING_STAIRS,
                    ENTRANCE_DOOR(181) } },
            });
    } else {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FOYER_UPPER,
                                            RR_FIRE_TEMPLE_MQ_NEAR_BOSS_ROOM, ENTRANCE_DOOR(120) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_NEAR_BOSS_ROOM,
                                            RR_FIRE_TEMPLE_MQ_FOYER_UPPER, ENTRANCE_DOOR(121) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FOYER_LOWER,
                                            RR_FIRE_TEMPLE_MQ_LOOP_CAGE_FOYER_SIDE, ENTRANCE_DOOR(122) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOOP_CAGE_FOYER_SIDE,
                                            RR_FIRE_TEMPLE_MQ_FOYER_LOWER, ENTRANCE_DOOR(123) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FOYER_LOWER,
                                            RR_FIRE_TEMPLE_MQ_LOOP_HEXAGON_ROOM, ENTRANCE_DOOR(124) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOOP_HEXAGON_ROOM,
                                            RR_FIRE_TEMPLE_MQ_FOYER_LOWER, ENTRANCE_DOOR(125) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOOP_HEXAGON_ROOM,
                                            RR_FIRE_TEMPLE_MQ_LOOP_5_TILE_ROOM, ENTRANCE_DOOR(126) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOOP_5_TILE_ROOM,
                                            RR_FIRE_TEMPLE_MQ_LOOP_HEXAGON_ROOM, ENTRANCE_DOOR(127) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOOP_5_TILE_ROOM,
                                            RR_FIRE_TEMPLE_MQ_LOOP_FLARE_DANCER, ENTRANCE_DOOR(128) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOOP_FLARE_DANCER,
                                            RR_FIRE_TEMPLE_MQ_LOOP_5_TILE_ROOM, ENTRANCE_DOOR(129) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOOP_FLARE_DANCER,
                                            RR_FIRE_TEMPLE_MQ_LOOP_CAGE_SWITCH, ENTRANCE_DOOR(130) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOOP_CAGE_SWITCH,
                                            RR_FIRE_TEMPLE_MQ_LOOP_FLARE_DANCER, ENTRANCE_DOOR(131) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FOYER_UPPER,
                                            RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM, ENTRANCE_DOOR(132) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,
                                            RR_FIRE_TEMPLE_MQ_FOYER_UPPER, ENTRANCE_DOOR(133) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,
                                            RR_FIRE_TEMPLE_MQ_GS_GORON_CAGE, ENTRANCE_DOOR(134) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_GS_GORON_CAGE,
                                            RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM, ENTRANCE_DOOR(135) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,
                                            RR_FIRE_TEMPLE_MQ_TORCH_FIREWALL_ROOM, ENTRANCE_DOOR(136) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_TORCH_FIREWALL_ROOM,
                                            RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM, ENTRANCE_DOOR(137) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,
                                            RR_FIRE_TEMPLE_MQ_TORCH_LOCKED_CAGE, ENTRANCE_DOOR(138) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_TORCH_LOCKED_CAGE,
                                            RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM, ENTRANCE_DOOR(139) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM,
                                            RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_1F, ENTRANCE_DOOR(140) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_1F,
                                            RR_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM, ENTRANCE_DOOR(141) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_2F,
                                            RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_LOWER, ENTRANCE_DOOR(142) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_LOWER,
                                            RR_FIRE_TEMPLE_MQ_LAVA_GEYSER_2F, ENTRANCE_DOOR(143) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_3F,
                                            RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE, ENTRANCE_DOOR(144) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE,
                                            RR_FIRE_TEMPLE_MQ_SHORTCUT_ROOM_3F, ENTRANCE_DOOR(145) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_ABOVE_MAZE,
                                            RR_FIRE_TEMPLE_MQ_TORCH_SLUG_CLIMB, ENTRANCE_DOOR(146) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_TORCH_SLUG_CLIMB,
                                            RR_FIRE_TEMPLE_MQ_ABOVE_MAZE, ENTRANCE_DOOR(147) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_TORCH_SLUG_CLIMB,
                                            RR_FIRE_TEMPLE_MQ_BURNING_BLOCK, ENTRANCE_DOOR(148) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_BURNING_BLOCK,
                                            RR_FIRE_TEMPLE_MQ_TORCH_SLUG_CLIMB, ENTRANCE_DOOR(149) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE,
                                            RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM, ENTRANCE_DOOR(150) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM,
                                            RR_FIRE_TEMPLE_MQ_LOWER_LIZALFOS_MAZE, ENTRANCE_DOOR(151) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_MAZE_SWITCH_DOOR,
                                            RR_FIRE_TEMPLE_MQ_3F_CURVED_CAGE, ENTRANCE_DOOR(152) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_3F_CURVED_CAGE,
                                            RR_FIRE_TEMPLE_MQ_MAZE_SWITCH_DOOR, ENTRANCE_DOOR(153) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_UPPER_LIZALFOS_MAZE,
                                            RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM, ENTRANCE_DOOR(154) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM,
                                            RR_FIRE_TEMPLE_MQ_UPPER_LIZALFOS_MAZE, ENTRANCE_DOOR(155) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_CAGE,
                                            RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM, ENTRANCE_DOOR(156) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM,
                                            RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM_CAGE, ENTRANCE_DOOR(157) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM,
                                            RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM, ENTRANCE_DOOR(158) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_NARROW_PATH_ROOM,
                                            RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM, ENTRANCE_DOOR(159) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM,
                                            RR_FIRE_TEMPLE_MQ_CORRIDOR, ENTRANCE_DOOR(160) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_CORRIDOR,
                                            RR_FIRE_TEMPLE_MQ_HIGH_TORCH_ROOM, ENTRANCE_DOOR(161) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_CORRIDOR,
                                            RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN, ENTRANCE_DOOR(162) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN,
                                            RR_FIRE_TEMPLE_MQ_CORRIDOR, ENTRANCE_DOOR(163) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_UPPER_DOOR,
                                            RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PLATFORMS, ENTRANCE_DOOR(164) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PLATFORMS,
                                            RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_UPPER_DOOR, ENTRANCE_DOOR(165) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MIDDLE,
                                            RR_FIRE_TEMPLE_MQ_GS_LIZALFOS_ROOM, ENTRANCE_DOOR(166) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_GS_LIZALFOS_ROOM,
                                            RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MIDDLE, ENTRANCE_DOOR(167) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FIRE_MAZE_SWITCH,
                                            RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER, ENTRANCE_DOOR(168) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER,
                                            RR_FIRE_TEMPLE_MQ_FIRE_MAZE_SWITCH, ENTRANCE_DOOR(169) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN,
                                            RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER, ENTRANCE_DOOR(170) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_2_FIRE_WALLS_LOWER,
                                            RR_FIRE_TEMPLE_MQ_FIRE_MAZE_MAIN, ENTRANCE_DOOR(171) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PAST_WALL,
                                            RR_FIRE_TEMPLE_MQ_3F_FLARE_DANCER, ENTRANCE_DOOR(172) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_3F_FLARE_DANCER,
                                            RR_FIRE_TEMPLE_MQ_FIRE_MAZE_PAST_WALL, ENTRANCE_DOOR(173) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_ABOVE_3F_FLARE_DANCER,
                                            RR_FIRE_TEMPLE_MQ_LOCKED_CLIMB, ENTRANCE_DOOR(174) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOCKED_CLIMB,
                                            RR_FIRE_TEMPLE_MQ_ABOVE_3F_FLARE_DANCER, ENTRANCE_DOOR(175) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_LOCKED_CLIMB,
                                            RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_ROOM, ENTRANCE_DOOR(176) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_ROOM,
                                            RR_FIRE_TEMPLE_MQ_LOCKED_CLIMB, ENTRANCE_DOOR(177) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_4F,
                                            RR_FIRE_TEMPLE_MQ_TOP_OF_COLLAPSING_STAIRS, ENTRANCE_DOOR(178) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_TOP_OF_COLLAPSING_STAIRS,
                                            RR_FIRE_TEMPLE_MQ_NARROW_STAIRS_4F, ENTRANCE_DOOR(179) } },
                                        { { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_BASE_OF_COLLAPSING_STAIRS,
                                            RR_FIRE_TEMPLE_MQ_ABOVE_FIRE_MAZE, ENTRANCE_DOOR(180) },
                                          { EntranceType::DoorFire, RR_FIRE_TEMPLE_MQ_ABOVE_FIRE_MAZE,
                                            RR_FIRE_TEMPLE_MQ_BASE_OF_COLLAPSING_STAIRS, ENTRANCE_DOOR(181) } },
                                    });
    }

    if (ctx->GetDungeon(Rando::WATER_TEMPLE)->IsVanilla()) {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_RISING_TARGET_LEDGE, RR_WATER_TEMPLE_TRAPPED_SLOPE,
                    ENTRANCE_DOOR(182) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_TRAPPED_SLOPE, RR_WATER_TEMPLE_RISING_TARGET_LEDGE,
                    ENTRANCE_DOOR(183) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_OUTSIDE_WATERFALL, RR_WATER_TEMPLE_WATERFALL,
                    ENTRANCE_DOOR(184) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_WATERFALL, RR_WATER_TEMPLE_OUTSIDE_WATERFALL,
                    ENTRANCE_DOOR(185) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_WATERFALL_TOP, RR_WATER_TEMPLE_TOGGLE_SWITCH,
                    ENTRANCE_DOOR(186) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_TOGGLE_SWITCH, RR_WATER_TEMPLE_WATERFALL_TOP,
                    ENTRANCE_DOOR(187) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_LIKE_LIKE_SPIKES, RR_WATER_TEMPLE_DARK_LINK_ROOM,
                    ENTRANCE_DOOR(188) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_DARK_LINK_ROOM, RR_WATER_TEMPLE_LIKE_LIKE_SPIKES,
                    ENTRANCE_DOOR(189) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_DARK_LINK_ROOM, RR_WATER_TEMPLE_SOT_PIT_ROOM,
                    ENTRANCE_DOOR(190) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_SOT_PIT_ROOM, RR_WATER_TEMPLE_DARK_LINK_ROOM,
                    ENTRANCE_DOOR(191) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_DRAGON_ROOM, RR_WATER_TEMPLE_OUTSIDE_DRAGON_ROOM,
                    ENTRANCE_DOOR(192) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_OUTSIDE_DRAGON_ROOM, RR_WATER_TEMPLE_DRAGON_ROOM,
                    ENTRANCE_DOOR(193) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_BEHIND_SPIKE_MOAT, RR_WATER_TEMPLE_BOULDERS_SOUTH,
                    ENTRANCE_DOOR(194) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_BOULDERS_SOUTH, RR_WATER_TEMPLE_BEHIND_SPIKE_MOAT,
                    ENTRANCE_DOOR(195) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_BEHIND_CANAL, RR_WATER_TEMPLE_BOSS_KEY_ROOM,
                    ENTRANCE_DOOR(196) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_BOSS_KEY_ROOM, RR_WATER_TEMPLE_BEHIND_CANAL,
                    ENTRANCE_DOOR(197) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_BOULDERS_NORTH, RR_WATER_TEMPLE_BLOCK_ROOM_TARGET,
                    ENTRANCE_DOOR(198) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_BLOCK_ROOM_TARGET, RR_WATER_TEMPLE_BOULDERS_NORTH,
                    ENTRANCE_DOOR(199) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_BLOCK_ROOM_STAIRS, RR_WATER_TEMPLE_3_JETS_SWITCH,
                    ENTRANCE_DOOR(200) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_3_JETS_SWITCH, RR_WATER_TEMPLE_BLOCK_ROOM_STAIRS,
                    ENTRANCE_DOOR(201) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_3_JETS_NO_SWITCH, RR_WATER_TEMPLE_CANAL_ALCOVE,
                    ENTRANCE_DOOR(202) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_CANAL_ALCOVE, RR_WATER_TEMPLE_3_JETS_NO_SWITCH,
                    ENTRANCE_DOOR(203) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_LOW_EMBLEM, RR_WATER_TEMPLE_4_SPIKES_ROOM,
                    ENTRANCE_DOOR(204) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_4_SPIKES_ROOM, RR_WATER_TEMPLE_LOW_EMBLEM,
                    ENTRANCE_DOOR(205) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_SIDE_TOWER_1F, RR_WATER_TEMPLE_TORCH_ROOM,
                    ENTRANCE_DOOR(206) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_TORCH_ROOM, RR_WATER_TEMPLE_SIDE_TOWER_1F,
                    ENTRANCE_DOOR(207) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_MAIN, RR_WATER_TEMPLE_PILLAR_1F, ENTRANCE_DOOR(208) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_PILLAR_1F, RR_WATER_TEMPLE_MAIN, ENTRANCE_DOOR(209) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_2F_CENTRAL_LM, RR_WATER_TEMPLE_PILLAR_2F,
                    ENTRANCE_DOOR(210) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_PILLAR_2F, RR_WATER_TEMPLE_2F_CENTRAL_LM,
                    ENTRANCE_DOOR(211) } },
                { { EntranceType::DoorWater, RR_WATER_TEMPLE_OUTSIDE_JET_LIFT_2F, RR_WATER_TEMPLE_JET_LIFT_2F,
                    ENTRANCE_DOOR(212) },
                  { EntranceType::DoorWater, RR_WATER_TEMPLE_JET_LIFT_2F, RR_WATER_TEMPLE_OUTSIDE_JET_LIFT_2F,
                    ENTRANCE_DOOR(213) } },
            });
    } else {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_RISING_TARGET_LEDGE,
                                            RR_WATER_TEMPLE_MQ_BOSS_DOOR_RAMP, ENTRANCE_DOOR(182) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_BOSS_DOOR_RAMP,
                                            RR_WATER_TEMPLE_MQ_RISING_TARGET_LEDGE, ENTRANCE_DOOR(183) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_OUTSIDE_WATERFALL,
                                            RR_WATER_TEMPLE_MQ_WATERFALL, ENTRANCE_DOOR(184) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_WATERFALL,
                                            RR_WATER_TEMPLE_MQ_OUTSIDE_WATERFALL, ENTRANCE_DOOR(185) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_WATERFALL_TOP,
                                            RR_WATER_TEMPLE_MQ_STALFOS_PIT, ENTRANCE_DOOR(186) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_STALFOS_PIT,
                                            RR_WATER_TEMPLE_MQ_WATERFALL, ENTRANCE_DOOR(187) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER,
                                            RR_WATER_TEMPLE_MQ_DARK_LINK_ROOM, ENTRANCE_DOOR(188) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_DARK_LINK_ROOM,
                                            RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER, ENTRANCE_DOOR(189) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_DARK_LINK_ROOM,
                                            RR_WATER_TEMPLE_MQ_GATED_PIT, ENTRANCE_DOOR(190) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_GATED_PIT,
                                            RR_WATER_TEMPLE_MQ_DARK_LINK_ROOM, ENTRANCE_DOOR(191) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR,
                                            RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH, ENTRANCE_DOOR(192) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH,
                                            RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR, ENTRANCE_DOOR(193) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_MAIN,
                                            RR_WATER_TEMPLE_MQ_CRATE_VORTEX_ROOM, ENTRANCE_DOOR(194) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_CRATE_VORTEX_ROOM,
                                            RR_WATER_TEMPLE_MQ_MAIN, ENTRANCE_DOOR(195) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_CRATE_VORTEX_ROOM,
                                            RR_WATER_TEMPLE_MQ_BEHIND_SPIKE_MOAT, ENTRANCE_DOOR(196) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_BEHIND_SPIKE_MOAT,
                                            RR_WATER_TEMPLE_MQ_CRATE_VORTEX_ROOM, ENTRANCE_DOOR(197) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_CRATE_VORTEX_CAGE,
                                            RR_WATER_TEMPLE_MQ_DODONGO_ROOM, ENTRANCE_DOOR(198) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_DODONGO_ROOM,
                                            RR_WATER_TEMPLE_MQ_CRATE_VORTEX_CAGE, ENTRANCE_DOOR(199) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_DODONGO_ROOM,
                                            RR_WATER_TEMPLE_MQ_3_JETS_ROOM_NO_SWITCH_SIDE, ENTRANCE_DOOR(200) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_3_JETS_ROOM_NO_SWITCH_SIDE,
                                            RR_WATER_TEMPLE_MQ_DODONGO_ROOM, ENTRANCE_DOOR(201) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_3_JETS_ROOM_SWITCH_SIDE,
                                            RR_WATER_TEMPLE_MQ_CANAL_ALCOVE, ENTRANCE_DOOR(202) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_CANAL_ALCOVE,
                                            RR_WATER_TEMPLE_MQ_3_JETS_ROOM_SWITCH_SIDE, ENTRANCE_DOOR(203) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_LOW_EMBLEM,
                                            RR_WATER_TEMPLE_MQ_3_STALFOS_ROOM, ENTRANCE_DOOR(204) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_3_STALFOS_ROOM,
                                            RR_WATER_TEMPLE_MQ_LOW_EMBLEM, ENTRANCE_DOOR(205) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_SIDE_TOWER_1F,
                                            RR_WATER_TEMPLE_MQ_TOWER_TARGET_ROOM, ENTRANCE_DOOR(206) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_TOWER_TARGET_ROOM,
                                            RR_WATER_TEMPLE_MQ_SIDE_TOWER_1F, ENTRANCE_DOOR(207) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_MAIN,
                                            RR_WATER_TEMPLE_MQ_PILLAR_1F, ENTRANCE_DOOR(208) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_PILLAR_1F,
                                            RR_WATER_TEMPLE_MQ_MAIN, ENTRANCE_DOOR(209) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_2F_CENTRAL_LM,
                                            RR_WATER_TEMPLE_MQ_PILLAR_2F, ENTRANCE_DOOR(210) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_PILLAR_2F,
                                            RR_WATER_TEMPLE_MQ_2F_CENTRAL_LM, ENTRANCE_DOOR(211) } },
                                        { { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_3F_CENTRAL_A,
                                            RR_WATER_TEMPLE_MQ_LIZALFOS_LOOP_A, ENTRANCE_DOOR(212) },
                                          { EntranceType::DoorWater, RR_WATER_TEMPLE_MQ_LIZALFOS_LOOP_A,
                                            RR_WATER_TEMPLE_MQ_3F_CENTRAL_A, ENTRANCE_DOOR(213) } },
                                    });
    }

    if (ctx->GetDungeon(Rando::SHADOW_TEMPLE)->IsVanilla()) {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_BEGINNING, RR_SHADOW_TEMPLE_WHISPERING_WALLS_START,
                    ENTRANCE_DOOR(214) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_WHISPERING_WALLS_START, RR_SHADOW_TEMPLE_BEGINNING,
                    ENTRANCE_DOOR(215) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_WHISPERING_WALLS_SIDE,
                    RR_SHADOW_TEMPLE_WHISPERING_WALLS_SIDE_ROOM, ENTRANCE_DOOR(216) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_WHISPERING_WALLS_SIDE_ROOM,
                    RR_SHADOW_TEMPLE_WHISPERING_WALLS_SIDE, ENTRANCE_DOOR(217) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_WHISPERING_WALLS_END, RR_SHADOW_TEMPLE_DEAD_HAND,
                    ENTRANCE_DOOR(218) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_DEAD_HAND, RR_SHADOW_TEMPLE_WHISPERING_WALLS_END,
                    ENTRANCE_DOOR(219) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_FIRST_BEAMOS, RR_SHADOW_TEMPLE_COMPASS_ROOM,
                    ENTRANCE_DOOR(220) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_COMPASS_ROOM, RR_SHADOW_TEMPLE_FIRST_BEAMOS,
                    ENTRANCE_DOOR(221) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_FIRST_BEAMOS, RR_SHADOW_TEMPLE_SPINNING_BLADES,
                    ENTRANCE_DOOR(222) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_SPINNING_BLADES, RR_SHADOW_TEMPLE_FIRST_BEAMOS,
                    ENTRANCE_DOOR(223) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_FIRST_BEAMOS, RR_SHADOW_TEMPLE_B2_TO_B3_CORRIDOR_B2,
                    ENTRANCE_DOOR(224) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_B2_TO_B3_CORRIDOR_B2, RR_SHADOW_TEMPLE_FIRST_BEAMOS,
                    ENTRANCE_DOOR(225) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_UPPER_HUGE_PIT_DOOR_LEDGE,
                    RR_SHADOW_TEMPLE_INVISIBLE_SPINNING_BLADES, ENTRANCE_DOOR(226) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_INVISIBLE_SPINNING_BLADES,
                    RR_SHADOW_TEMPLE_UPPER_HUGE_PIT_DOOR_LEDGE, ENTRANCE_DOOR(227) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_LOWER_HUGE_PIT_DOOR_LEDGE,
                    RR_SHADOW_TEMPLE_FLOOR_SPIKES_S_DOOR, ENTRANCE_DOOR(228) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_FLOOR_SPIKES_S_DOOR,
                    RR_SHADOW_TEMPLE_LOWER_HUGE_PIT_DOOR_LEDGE, ENTRANCE_DOOR(229) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_FLOOR_SPIKES_W_DOOR, RR_SHADOW_TEMPLE_SKULL_JAR,
                    ENTRANCE_DOOR(230) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_SKULL_JAR, RR_SHADOW_TEMPLE_FLOOR_SPIKES_W_DOOR,
                    ENTRANCE_DOOR(231) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_SPIKES_DOOR_PLATFORM, RR_SHADOW_TEMPLE_UPPER_WIND_TUNNEL,
                    ENTRANCE_DOOR(232) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_UPPER_WIND_TUNNEL, RR_SHADOW_TEMPLE_SPIKES_DOOR_PLATFORM,
                    ENTRANCE_DOOR(233) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_LOWER_WIND_TUNNEL,
                    RR_SHADOW_TEMPLE_WIND_TUNNEL_HINT_ROOM, ENTRANCE_DOOR(234) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_WIND_TUNNEL_HINT_ROOM,
                    RR_SHADOW_TEMPLE_LOWER_WIND_TUNNEL, ENTRANCE_DOOR(235) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_WIND_TUNNEL_ALCOVE, RR_SHADOW_TEMPLE_ROOM_TO_BOAT,
                    ENTRANCE_DOOR(236) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_ROOM_TO_BOAT, RR_SHADOW_TEMPLE_WIND_TUNNEL_ALCOVE,
                    ENTRANCE_DOOR(237) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_ROOM_TO_BOAT, RR_SHADOW_TEMPLE_DOCK,
                    ENTRANCE_DOOR(238) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_DOCK, RR_SHADOW_TEMPLE_ROOM_TO_BOAT,
                    ENTRANCE_DOOR(239) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_ACROSS_CHASM, RR_SHADOW_TEMPLE_PRE_BOSS_ROOM,
                    ENTRANCE_DOOR(240) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_PRE_BOSS_ROOM, RR_SHADOW_TEMPLE_ACROSS_CHASM,
                    ENTRANCE_DOOR(241) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_BEYOND_BOAT, RR_SHADOW_TEMPLE_MAZE, ENTRANCE_DOOR(242) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MAZE, RR_SHADOW_TEMPLE_BEYOND_BOAT,
                    ENTRANCE_DOOR(243) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MAZE, RR_SHADOW_TEMPLE_X_CROSS, ENTRANCE_DOOR(244) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_X_CROSS, RR_SHADOW_TEMPLE_MAZE, ENTRANCE_DOOR(245) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MAZE, RR_SHADOW_TEMPLE_THREE_SKULL_JARS,
                    ENTRANCE_DOOR(246) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_THREE_SKULL_JARS, RR_SHADOW_TEMPLE_MAZE,
                    ENTRANCE_DOOR(247) } },
                { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MAZE, RR_SHADOW_TEMPLE_WOODEN_SPIKES,
                    ENTRANCE_DOOR(248) },
                  { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_WOODEN_SPIKES, RR_SHADOW_TEMPLE_MAZE,
                    ENTRANCE_DOOR(249) } },
            });
    } else {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_START, ENTRANCE_DOOR(214) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_START,
                                            RR_SHADOW_TEMPLE_MQ_SPINNER_ROOM, ENTRANCE_DOOR(215) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_SIDE,
                                            RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_SIDE_ROOM, ENTRANCE_DOOR(216) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_SIDE_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_SIDE, ENTRANCE_DOOR(217) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_END,
                                            RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_DEAD_HAND, ENTRANCE_DOOR(218) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_DEAD_HAND,
                                            RR_SHADOW_TEMPLE_MQ_WHISPERING_WALLS_END, ENTRANCE_DOOR(219) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,
                                            RR_SHADOW_TEMPLE_MQ_B2_GIBDO_ROOM, ENTRANCE_DOOR(220) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_B2_GIBDO_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS, ENTRANCE_DOOR(221) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,
                                            RR_SHADOW_TEMPLE_MQ_B2_SPINNING_BLADE_ROOM, ENTRANCE_DOOR(222) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_B2_SPINNING_BLADE_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS, ENTRANCE_DOOR(223) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS,
                                            RR_SHADOW_TEMPLE_MQ_B2_TO_B3_CORRIDOR_B2, ENTRANCE_DOOR(224) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_B2_TO_B3_CORRIDOR_B2,
                                            RR_SHADOW_TEMPLE_MQ_FIRST_BEAMOS, ENTRANCE_DOOR(225) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT,
                                            RR_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_ROOM, ENTRANCE_DOOR(226) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_UPPER_HUGE_PIT, ENTRANCE_DOOR(227) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT_DOOR_LEDGE,
                                            RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM, ENTRANCE_DOOR(228) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_LOWER_HUGE_PIT_DOOR_LEDGE, ENTRANCE_DOOR(229) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_W_DOOR,
                                            RR_SHADOW_TEMPLE_MQ_STALFOS_ROOM, ENTRANCE_DOOR(230) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_STALFOS_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_FLOOR_SPIKES_W_DOOR, ENTRANCE_DOOR(231) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_SPIKES_DOOR_PLATFORM,
                                            RR_SHADOW_TEMPLE_MQ_UPPER_WIND_TUNNEL, ENTRANCE_DOOR(232) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_UPPER_WIND_TUNNEL,
                                            RR_SHADOW_TEMPLE_MQ_SPIKES_DOOR_PLATFORM, ENTRANCE_DOOR(233) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_LOWER_WIND_TUNNEL,
                                            RR_SHADOW_TEMPLE_MQ_WIND_HINT_ROOM, ENTRANCE_DOOR(234) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_WIND_HINT_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_LOWER_WIND_TUNNEL, ENTRANCE_DOOR(235) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL_ALCOVE,
                                            RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM, ENTRANCE_DOOR(236) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_WIND_TUNNEL_ALCOVE, ENTRANCE_DOOR(237) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_DOCK, ENTRANCE_DOOR(238) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_DOCK,
                                            RR_SHADOW_TEMPLE_MQ_B4_GIBDO_ROOM, ENTRANCE_DOOR(239) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM,
                                            RR_SHADOW_TEMPLE_MQ_BOSS_DOOR, ENTRANCE_DOOR(240) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_BOSS_DOOR,
                                            RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM, ENTRANCE_DOOR(241) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_ACROSS_CHASM,
                                            RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, ENTRANCE_DOOR(242) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE,
                                            RR_SHADOW_TEMPLE_MQ_BEYOND_BOAT, ENTRANCE_DOOR(243) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE,
                                            RR_SHADOW_TEMPLE_MQ_X_CROSS, ENTRANCE_DOOR(244) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_X_CROSS,
                                            RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, ENTRANCE_DOOR(245) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE,
                                            RR_SHADOW_TEMPLE_MQ_THREE_SKULL_JARS, ENTRANCE_DOOR(246) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_THREE_SKULL_JARS,
                                            RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, ENTRANCE_DOOR(247) } },
                                        { { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE,
                                            RR_SHADOW_TEMPLE_MQ_SPIKE_WALLS_ROOM, ENTRANCE_DOOR(248) },
                                          { EntranceType::DoorShadow, RR_SHADOW_TEMPLE_MQ_SPIKE_WALLS_ROOM,
                                            RR_SHADOW_TEMPLE_MQ_INVISIBLE_MAZE, ENTRANCE_DOOR(249) } },
                                    });
    }

    if (ctx->GetDungeon(Rando::SPIRIT_TEMPLE)->IsVanilla()) {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_ADULT_SIDE_HUB,
                                            RR_SPIRIT_TEMPLE_SAND_PIT, ENTRANCE_DOOR(250) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SAND_PIT,
                                            RR_SPIRIT_TEMPLE_ADULT_SIDE_HUB, ENTRANCE_DOOR(251) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_ADULT_SIDE_HUB,
                                            RR_SPIRIT_TEMPLE_ABOVE_BOULDERS, ENTRANCE_DOOR(252) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_ABOVE_BOULDERS,
                                            RR_SPIRIT_TEMPLE_ADULT_SIDE_HUB, ENTRANCE_DOOR(253) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_BOULDERS,
                                            RR_SPIRIT_TEMPLE_PAST_BOULDERS, ENTRANCE_DOOR(254) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_PAST_BOULDERS,
                                            RR_SPIRIT_TEMPLE_BOULDERS, ENTRANCE_DOOR(255) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_ADULT_SIDE_HUB,
                                            RR_SPIRIT_TEMPLE_1F_MIRROR_ROOM, ENTRANCE_DOOR(256) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_1F_MIRROR_ROOM,
                                            RR_SPIRIT_TEMPLE_ADULT_SIDE_HUB, ENTRANCE_DOOR(257) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_2F_MIRROR_ROOM,
                                            RR_SPIRIT_TEMPLE_STATUE_ROOM_ADULT, ENTRANCE_DOOR(258) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_STATUE_ROOM_ADULT,
                                            RR_SPIRIT_TEMPLE_2F_MIRROR_ROOM, ENTRANCE_DOOR(259) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_STATUE_ROOM_ADULT,
                                            RR_SPIRIT_TEMPLE_POT_STAIRS, ENTRANCE_DOOR(260) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_POT_STAIRS,
                                            RR_SPIRIT_TEMPLE_STATUE_ROOM_ADULT, ENTRANCE_DOOR(261) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_POT_STAIRS,
                                            RR_SPIRIT_TEMPLE_BEAMOS_PITS, ENTRANCE_DOOR(262) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_BEAMOS_PITS,
                                            RR_SPIRIT_TEMPLE_POT_STAIRS, ENTRANCE_DOOR(263) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_BEAMOS_PITS,
                                            RR_SPIRIT_TEMPLE_BIG_WALL_BASE, ENTRANCE_DOOR(264) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_BIG_WALL_BASE,
                                            RR_SPIRIT_TEMPLE_BEAMOS_PITS, ENTRANCE_DOOR(265) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_BEAMOS_PITS,
                                            RR_SPIRIT_TEMPLE_4_ARMOS, ENTRANCE_DOOR(266) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_4_ARMOS,
                                            RR_SPIRIT_TEMPLE_BEAMOS_PITS, ENTRANCE_DOOR(267) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_4_ARMOS,
                                            RR_SPIRIT_TEMPLE_4_ARMOS_SIDE_ROOM, ENTRANCE_DOOR(268) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_4_ARMOS_SIDE_ROOM,
                                            RR_SPIRIT_TEMPLE_4_ARMOS, ENTRANCE_DOOR(269) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_4_ARMOS,
                                            RR_SPIRIT_TEMPLE_CHEST_STAIRS, ENTRANCE_DOOR(270) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_CHEST_STAIRS,
                                            RR_SPIRIT_TEMPLE_4_ARMOS, ENTRANCE_DOOR(271) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_CHEST_STAIRS,
                                            RR_SPIRIT_TEMPLE_ADULT_THRONE, ENTRANCE_DOOR(272) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_ADULT_THRONE,
                                            RR_SPIRIT_TEMPLE_CHEST_STAIRS, ENTRANCE_DOOR(273) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_ADULT_THRONE,
                                            RR_SPIRIT_TEMPLE_LEFT_HAND_EXIT, ENTRANCE_DOOR(274) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_LEFT_HAND_EXIT,
                                            RR_SPIRIT_TEMPLE_ADULT_THRONE, ENTRANCE_DOOR(275) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_BIG_WALL_UPPER,
                                            RR_SPIRIT_TEMPLE_4F_CENTRAL, ENTRANCE_DOOR(276) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_4F_CENTRAL,
                                            RR_SPIRIT_TEMPLE_BIG_WALL_UPPER, ENTRANCE_DOOR(277) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_4F_CENTRAL,
                                            RR_SPIRIT_TEMPLE_FAKE_DOORS_ROOM, ENTRANCE_DOOR(278) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_FAKE_DOORS_ROOM,
                                            RR_SPIRIT_TEMPLE_4F_CENTRAL, ENTRANCE_DOOR(279) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_4F_CENTRAL,
                                            RR_SPIRIT_TEMPLE_BIG_MIRROR_ROOM, ENTRANCE_DOOR(280) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_BIG_MIRROR_ROOM,
                                            RR_SPIRIT_TEMPLE_4F_CENTRAL, ENTRANCE_DOOR(281) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_BIG_MIRROR_ROOM,
                                            RR_SPIRIT_TEMPLE_BIG_MIRROR_CAVE, ENTRANCE_DOOR(282) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_BIG_MIRROR_CAVE,
                                            RR_SPIRIT_TEMPLE_BIG_MIRROR_ROOM, ENTRANCE_DOOR(283) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_CHILD_SIDE_HUB,
                                            RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_SOUTH, ENTRANCE_DOOR(284) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_SOUTH,
                                            RR_SPIRIT_TEMPLE_CHILD_SIDE_HUB, ENTRANCE_DOOR(285) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_NORTH,
                                            RR_SPIRIT_TEMPLE_1F_ANUBIS, ENTRANCE_DOOR(286) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_1F_ANUBIS,
                                            RR_SPIRIT_TEMPLE_SWITCH_BRIDGE_NORTH, ENTRANCE_DOOR(287) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_1F_ANUBIS,
                                            RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_NORTH, ENTRANCE_DOOR(288) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_NORTH,
                                            RR_SPIRIT_TEMPLE_1F_ANUBIS, ENTRANCE_DOOR(289) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_SOUTH,
                                            RR_SPIRIT_TEMPLE_CHILD_SIDE_HUB, ENTRANCE_DOOR(290) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_CHILD_SIDE_HUB,
                                            RR_SPIRIT_TEMPLE_RUPEE_BRIDGE_SOUTH, ENTRANCE_DOOR(291) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_CHILD_BOXES,
                                            RR_SPIRIT_TEMPLE_SUN_ON_FLOOR_1F, ENTRANCE_DOOR(292) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SUN_ON_FLOOR_1F,
                                            RR_SPIRIT_TEMPLE_CHILD_BOXES, ENTRANCE_DOOR(293) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SUN_ON_FLOOR_2F,
                                            RR_SPIRIT_TEMPLE_STATUE_ROOM_CHILD, ENTRANCE_DOOR(294) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_STATUE_ROOM_CHILD,
                                            RR_SPIRIT_TEMPLE_SUN_ON_FLOOR_2F, ENTRANCE_DOOR(295) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_STATUE_ROOM_CHILD,
                                            RR_SPIRIT_TEMPLE_EMPTY_STAIRS, ENTRANCE_DOOR(296) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_EMPTY_STAIRS,
                                            RR_SPIRIT_TEMPLE_STATUE_ROOM_CHILD, ENTRANCE_DOOR(297) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_EMPTY_STAIRS,
                                            RR_SPIRIT_TEMPLE_SUN_BLOCK_ROOM, ENTRANCE_DOOR(298) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SUN_BLOCK_ROOM,
                                            RR_SPIRIT_TEMPLE_EMPTY_STAIRS, ENTRANCE_DOOR(299) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SUN_BLOCK_ROOM,
                                            RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS, ENTRANCE_DOOR(300) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS,
                                            RR_SPIRIT_TEMPLE_SUN_BLOCK_ROOM, ENTRANCE_DOOR(301) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS,
                                            RR_SPIRIT_TEMPLE_CHILD_THRONE, ENTRANCE_DOOR(302) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_CHILD_THRONE,
                                            RR_SPIRIT_TEMPLE_SKULLTULA_STAIRS, ENTRANCE_DOOR(303) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_CHILD_THRONE,
                                            RR_SPIRIT_TEMPLE_RIGHT_HAND_EXIT, ENTRANCE_DOOR(304) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_RIGHT_HAND_EXIT,
                                            RR_SPIRIT_TEMPLE_CHILD_THRONE, ENTRANCE_DOOR(305) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_STATUE_ROOM,
                                            RR_SPIRIT_TEMPLE_SHORTCUT, ENTRANCE_DOOR(306) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_SHORTCUT,
                                            RR_SPIRIT_TEMPLE_STATUE_ROOM, ENTRANCE_DOOR(307) } },
                                    });
    } else {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BEHIND_GEYSER,
                                            RR_SPIRIT_TEMPLE_MQ_SAND_PIT, ENTRANCE_DOOR(250) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SAND_PIT,
                                            RR_SPIRIT_TEMPLE_MQ_BEHIND_GEYSER, ENTRANCE_DOOR(251) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BEHIND_GEYSER,
                                            RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_UPPER, ENTRANCE_DOOR(252) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_UPPER,
                                            RR_SPIRIT_TEMPLE_MQ_BEHIND_GEYSER, ENTRANCE_DOOR(253) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_LOWER,
                                            RR_SPIRIT_TEMPLE_MQ_SKULLTULA_ROOM, ENTRANCE_DOOR(254) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SKULLTULA_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_LOWER, ENTRANCE_DOOR(255) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BEHIND_GEYSER,
                                            RR_SPIRIT_TEMPLE_MQ_3_SUNS_ROOM_1F, ENTRANCE_DOOR(256) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_3_SUNS_ROOM_1F,
                                            RR_SPIRIT_TEMPLE_MQ_BEHIND_GEYSER, ENTRANCE_DOOR(257) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_3_SUNS_ROOM_2F,
                                            RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_ADULT, ENTRANCE_DOOR(258) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_ADULT,
                                            RR_SPIRIT_TEMPLE_MQ_3_SUNS_ROOM_2F, ENTRANCE_DOOR(259) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_ADULT,
                                            RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_LOWER, ENTRANCE_DOOR(260) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_LOWER,
                                            RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM_ADULT, ENTRANCE_DOOR(261) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_UPPER,
                                            RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS, ENTRANCE_DOOR(262) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS,
                                            RR_SPIRIT_TEMPLE_MQ_FIRE_WALL_STAIRS_UPPER, ENTRANCE_DOOR(263) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS,
                                            RR_SPIRIT_TEMPLE_MQ_BIG_WALL_BASE, ENTRANCE_DOOR(264) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BIG_WALL_BASE,
                                            RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS, ENTRANCE_DOOR(265) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS,
                                            RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM, ENTRANCE_DOOR(266) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_BEAMOS_PITS, ENTRANCE_DOOR(267) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_3F_GIBDO_ROOM, ENTRANCE_DOOR(268) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_3F_GIBDO_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM, ENTRANCE_DOOR(269) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_FLOORMASTER_STAIRS, ENTRANCE_DOOR(270) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_FLOORMASTER_STAIRS,
                                            RR_SPIRIT_TEMPLE_MQ_SOT_SUN_ROOM, ENTRANCE_DOOR(271) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_FLOORMASTER_STAIRS,
                                            RR_SPIRIT_TEMPLE_MQ_ADULT_THRONE, ENTRANCE_DOOR(272) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_ADULT_THRONE,
                                            RR_SPIRIT_TEMPLE_MQ_FLOORMASTER_STAIRS, ENTRANCE_DOOR(273) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_ADULT_THRONE,
                                            RR_SPIRIT_TEMPLE_MQ_LEFT_HAND_EXIT, ENTRANCE_DOOR(274) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_LEFT_HAND_EXIT,
                                            RR_SPIRIT_TEMPLE_MQ_ADULT_THRONE, ENTRANCE_DOOR(275) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BIG_WALL_UPPER,
                                            RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL, ENTRANCE_DOOR(276) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL,
                                            RR_SPIRIT_TEMPLE_MQ_BIG_WALL_UPPER, ENTRANCE_DOOR(277) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL,
                                            RR_SPIRIT_TEMPLE_MQ_9_THRONES_ROOM, ENTRANCE_DOOR(278) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_9_THRONES_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL, ENTRANCE_DOOR(279) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL,
                                            RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM, ENTRANCE_DOOR(280) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_4F_CENTRAL, ENTRANCE_DOOR(281) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CAVE, ENTRANCE_DOOR(282) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_CAVE,
                                            RR_SPIRIT_TEMPLE_MQ_BIG_MIRROR_ROOM, ENTRANCE_DOOR(283) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_CHILD_SIDE_HUB,
                                            RR_SPIRIT_TEMPLE_MQ_GIBDO_GRAVES, ENTRANCE_DOOR(284) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_GIBDO_GRAVES,
                                            RR_SPIRIT_TEMPLE_MQ_CHILD_SIDE_HUB, ENTRANCE_DOOR(285) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_GIBDO_POTS,
                                            RR_SPIRIT_TEMPLE_MQ_TURNTABLE, ENTRANCE_DOOR(286) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_TURNTABLE,
                                            RR_SPIRIT_TEMPLE_MQ_GIBDO_POTS, ENTRANCE_DOOR(287) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_TURNTABLE_BEHIND_FIRE,
                                            RR_SPIRIT_TEMPLE_MQ_ANUBIS_BRIDGE_GRAVE, ENTRANCE_DOOR(288) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_ANUBIS_BRIDGE_GRAVE,
                                            RR_SPIRIT_TEMPLE_MQ_TURNTABLE_BEHIND_FIRE, ENTRANCE_DOOR(289) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_ANUBIS_BRIDGE_CHEST,
                                            RR_SPIRIT_TEMPLE_MQ_CHILD_SIDE_HUB, ENTRANCE_DOOR(290) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_CHILD_SIDE_HUB,
                                            RR_SPIRIT_TEMPLE_MQ_ANUBIS_BRIDGE_CHEST, ENTRANCE_DOOR(291) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_1F_CHEST_SWITCH,
                                            RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE, ENTRANCE_DOOR(292) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_UNDER_LIKE_LIKE,
                                            RR_SPIRIT_TEMPLE_MQ_1F_CHEST_SWITCH, ENTRANCE_DOOR(293) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SUN_ON_FLOOR,
                                            RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, ENTRANCE_DOOR(294) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_SUN_ON_FLOOR, ENTRANCE_DOOR(295) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_FLAMETHROWER_STAIRS, ENTRANCE_DOOR(296) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_FLAMETHROWER_STAIRS,
                                            RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, ENTRANCE_DOOR(297) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_FLAMETHROWER_STAIRS,
                                            RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM, ENTRANCE_DOOR(298) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_FLAMETHROWER_STAIRS, ENTRANCE_DOOR(299) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_SKULLTULA_STAIRS, ENTRANCE_DOOR(300) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SKULLTULA_STAIRS,
                                            RR_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM, ENTRANCE_DOOR(301) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_SKULLTULA_STAIRS,
                                            RR_SPIRIT_TEMPLE_MQ_CHILD_THRONE, ENTRANCE_DOOR(302) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_CHILD_THRONE,
                                            RR_SPIRIT_TEMPLE_MQ_SKULLTULA_STAIRS, ENTRANCE_DOOR(303) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_CHILD_THRONE,
                                            RR_SPIRIT_TEMPLE_MQ_RIGHT_HAND_EXIT, ENTRANCE_DOOR(304) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_RIGHT_HAND_EXIT,
                                            RR_SPIRIT_TEMPLE_MQ_CHILD_THRONE, ENTRANCE_DOOR(305) } },
                                        { { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM,
                                            RR_SPIRIT_TEMPLE_MQ_BIG_BLOCKS_DOOR, ENTRANCE_DOOR(306) },
                                          { EntranceType::DoorSpirit, RR_SPIRIT_TEMPLE_MQ_BIG_BLOCKS_DOOR,
                                            RR_SPIRIT_TEMPLE_MQ_STATUE_ROOM, ENTRANCE_DOOR(307) } },
                                    });
    }

    if (ctx->GetDungeon(Rando::BOTTOM_OF_THE_WELL)->IsVanilla()) {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_MIDDLE, RR_BOTW_SKULL_WALL_ROOM, ENTRANCE_DOOR(308) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_SKULL_WALL_ROOM, RR_BOTW_MIDDLE, ENTRANCE_DOOR(309) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_MIDDLE, RR_BOTW_INVISIBLE_PATH, ENTRANCE_DOOR(310) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_INVISIBLE_PATH, RR_BOTW_MIDDLE, ENTRANCE_DOOR(311) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_BEHIND_MOAT, RR_BOTW_CRYPT, ENTRANCE_DOOR(312) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_CRYPT, RR_BOTW_BEHIND_MOAT, ENTRANCE_DOOR(313) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_NEAR_BOSS_UPPER, RR_BOTW_DEAD_HAND_ROOM,
                    ENTRANCE_DOOR(314) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_DEAD_HAND_ROOM, RR_BOTW_NEAR_BOSS_UPPER,
                    ENTRANCE_DOOR(315) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_CORNER_CRAWLSPACE, RR_BOTW_HIDDEN_PITS_ROOM,
                    ENTRANCE_DOOR(316) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_HIDDEN_PITS_ROOM, RR_BOTW_CORNER_CRAWLSPACE,
                    ENTRANCE_DOOR(317) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_HIDDEN_PITS_ROOM, RR_BOTW_LOCKED_CAGE,
                    ENTRANCE_DOOR(318) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_LOCKED_CAGE, RR_BOTW_HIDDEN_PITS_ROOM,
                    ENTRANCE_DOOR(319) } },
            });
    } else {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_MIDDLE, RR_BOTW_MQ_GRAVE_ROOM, ENTRANCE_DOOR(308) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_GRAVE_ROOM, RR_BOTW_MQ_MIDDLE, ENTRANCE_DOOR(309) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_MIDDLE, RR_BOTW_MQ_INVISIBLE_PATH,
                    ENTRANCE_DOOR(310) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_INVISIBLE_PATH, RR_BOTW_MQ_MIDDLE,
                    ENTRANCE_DOOR(311) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_BEHIND_MOAT, RR_BOTW_MQ_CRYPT, ENTRANCE_DOOR(312) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_CRYPT, RR_BOTW_MQ_BEHIND_MOAT, ENTRANCE_DOOR(313) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_NEAR_BOSS_UPPER, RR_BOTW_MQ_DEAD_HAND_ROOM,
                    ENTRANCE_DOOR(314) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_DEAD_HAND_ROOM, RR_BOTW_MQ_NEAR_BOSS_UPPER,
                    ENTRANCE_DOOR(315) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_PERIMETER, RR_BOTW_MQ_FLOORMASTER_ROOM,
                    ENTRANCE_DOOR(316) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_FLOORMASTER_ROOM, RR_BOTW_MQ_PERIMETER,
                    ENTRANCE_DOOR(317) } },
                { { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_FLOORMASTER_ROOM, RR_BOTW_MQ_LOCKED_CAGE,
                    ENTRANCE_DOOR(318) },
                  { EntranceType::DoorBottomOfTheWell, RR_BOTW_MQ_LOCKED_CAGE, RR_BOTW_MQ_FLOORMASTER_ROOM,
                    ENTRANCE_DOOR(319) } },
            });
    }

    if (ctx->GetDungeon(Rando::ICE_CAVERN)->IsVanilla()) {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorIceCavern, RR_ICE_CAVERN_BEFORE_FINAL_ROOM,
                                            RR_ICE_CAVERN_FINAL_ROOM, ENTRANCE_DOOR(320) },
                                          { EntranceType::DoorIceCavern, RR_ICE_CAVERN_FINAL_ROOM,
                                            RR_ICE_CAVERN_BEFORE_FINAL_ROOM, ENTRANCE_DOOR(321) } },
                                        { { EntranceType::DoorIceCavern, RR_ICE_CAVERN_FINAL_ROOM_UNDERWATER,
                                            RR_ICE_CAVERN_ABOVE_BEGINNING, ENTRANCE_DOOR(322) },
                                          { EntranceType::DoorIceCavern, RR_ICE_CAVERN_ABOVE_BEGINNING,
                                            RR_ICE_CAVERN_FINAL_ROOM_UNDERWATER, ENTRANCE_DOOR(323) } },
                                    });
    } else {
        entranceShuffleTable.insert(entranceShuffleTable.end(),
                                    {
                                        { { EntranceType::DoorIceCavern, RR_ICE_CAVERN_MQ_WEST_CORRIDOR,
                                            RR_ICE_CAVERN_MQ_STALFOS_ROOM, ENTRANCE_DOOR(320) },
                                          { EntranceType::DoorIceCavern, RR_ICE_CAVERN_MQ_STALFOS_ROOM,
                                            RR_ICE_CAVERN_MQ_WEST_CORRIDOR, ENTRANCE_DOOR(321) } },
                                        { { EntranceType::DoorIceCavern, RR_ICE_CAVERN_MQ_STALFOS_ROOM_UNDERWATER,
                                            RR_ICE_CAVERN_MQ_ABOVE_BEGINNING, ENTRANCE_DOOR(322) },
                                          { EntranceType::DoorIceCavern, RR_ICE_CAVERN_MQ_ABOVE_BEGINNING,
                                            RR_ICE_CAVERN_MQ_STALFOS_ROOM_UNDERWATER, ENTRANCE_DOOR(323) } },
                                    });
    }

    if (ctx->GetDungeon(Rando::GERUDO_TRAINING_GROUND)->IsVanilla()) {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_LOBBY,
                    RR_GERUDO_TRAINING_GROUND_CENTRAL_MAZE, ENTRANCE_DOOR(324) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_CENTRAL_MAZE,
                    RR_GERUDO_TRAINING_GROUND_LOBBY, ENTRANCE_DOOR(325) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_LOBBY,
                    RR_GERUDO_TRAINING_GROUND_SAND_ROOM, ENTRANCE_DOOR(326) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_SAND_ROOM,
                    RR_GERUDO_TRAINING_GROUND_LOBBY, ENTRANCE_DOOR(327) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_SAND_ROOM,
                    RR_GERUDO_TRAINING_GROUND_BOULDER_ROOM, ENTRANCE_DOOR(328) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_BOULDER_ROOM,
                    RR_GERUDO_TRAINING_GROUND_SAND_ROOM, ENTRANCE_DOOR(329) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_BOULDER_ROOM,
                    RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM, ENTRANCE_DOOR(330) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM,
                    RR_GERUDO_TRAINING_GROUND_BOULDER_ROOM, ENTRANCE_DOOR(331) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_BEHIND_HEAVY_BLOCK,
                    RR_GERUDO_TRAINING_GROUND_LIKE_LIKE_ROOM, ENTRANCE_DOOR(332) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_LIKE_LIKE_ROOM,
                    RR_GERUDO_TRAINING_GROUND_BEHIND_HEAVY_BLOCK, ENTRANCE_DOOR(333) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM_UPPER,
                    RR_GERUDO_TRAINING_GROUND_EYE_STATUE_UPPER, ENTRANCE_DOOR(334) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_EYE_STATUE_UPPER,
                    RR_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_ROOM_UPPER, ENTRANCE_DOOR(335) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_EYE_STATUE_UPPER,
                    RR_GERUDO_TRAINING_GROUND_ABOVE_MAZE, ENTRANCE_DOOR(336) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_ABOVE_MAZE,
                    RR_GERUDO_TRAINING_GROUND_EYE_STATUE_UPPER, ENTRANCE_DOOR(337) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_EYE_STATUE_LOWER,
                    RR_GERUDO_TRAINING_GROUND_HAMMER_ROOM, ENTRANCE_DOOR(338) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_HAMMER_ROOM,
                    RR_GERUDO_TRAINING_GROUND_EYE_STATUE_LOWER, ENTRANCE_DOOR(339) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_HAMMER_ROOM,
                    RR_GERUDO_TRAINING_GROUND_LAVA_ROOM_UPPER_LEDGE, ENTRANCE_DOOR(340) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_LAVA_ROOM_UPPER_LEDGE,
                    RR_GERUDO_TRAINING_GROUND_HAMMER_ROOM, ENTRANCE_DOOR(341) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_LAVA_ROOM,
                    RR_GERUDO_TRAINING_GROUND_UNDERWATER, ENTRANCE_DOOR(342) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_UNDERWATER,
                    RR_GERUDO_TRAINING_GROUND_LAVA_ROOM, ENTRANCE_DOOR(343) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_LAVA_ROOM,
                    RR_GERUDO_TRAINING_GROUND_DINALFOS, ENTRANCE_DOOR(344) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_DINALFOS,
                    RR_GERUDO_TRAINING_GROUND_LAVA_ROOM, ENTRANCE_DOOR(345) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_DINALFOS,
                    RR_GERUDO_TRAINING_GROUND_LOBBY, ENTRANCE_DOOR(346) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_LOBBY,
                    RR_GERUDO_TRAINING_GROUND_DINALFOS, ENTRANCE_DOOR(347) } },
            });
    } else {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_LOBBY,
                    RR_GERUDO_TRAINING_GROUND_MQ_MAZE_BY_LOBBY, ENTRANCE_DOOR(324) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_MAZE_BY_LOBBY,
                    RR_GERUDO_TRAINING_GROUND_MQ_LOBBY, ENTRANCE_DOOR(325) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_LOBBY,
                    RR_GERUDO_TRAINING_GROUND_MQ_SAND_ROOM, ENTRANCE_DOOR(326) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_SAND_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_LOBBY, ENTRANCE_DOOR(327) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_SAND_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_BOULDER_ROOM, ENTRANCE_DOOR(328) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_BOULDER_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_SAND_ROOM, ENTRANCE_DOOR(329) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_BOULDER_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM, ENTRANCE_DOOR(330) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_BOULDER_ROOM, ENTRANCE_DOOR(331) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_BEHIND_BLOCK, ENTRANCE_DOOR(332) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_BEHIND_BLOCK,
                    RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM, ENTRANCE_DOOR(333) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM_LEDGE, ENTRANCE_DOOR(334) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM_LEDGE,
                    RR_GERUDO_TRAINING_GROUND_MQ_STALFOS_ROOM, ENTRANCE_DOOR(335) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM_LEDGE,
                    RR_GERUDO_TRAINING_GROUND_MQ_MAGENTA_FIRE_ROOM, ENTRANCE_DOOR(336) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_MAGENTA_FIRE_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM_LEDGE, ENTRANCE_DOOR(337) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SLUG_ROOM, ENTRANCE_DOOR(338) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SLUG_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_STATUE_ROOM, ENTRANCE_DOOR(339) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SLUG_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_SWITCH_LEDGE, ENTRANCE_DOOR(340) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_SWITCH_LEDGE,
                    RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SLUG_ROOM, ENTRANCE_DOOR(341) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_PLATFORMS_UNLIT_TORCH,
                    RR_GERUDO_TRAINING_GROUND_MQ_UNDERWATER, ENTRANCE_DOOR(342) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_UNDERWATER,
                    RR_GERUDO_TRAINING_GROUND_MQ_PLATFORMS_UNLIT_TORCH, ENTRANCE_DOOR(343) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SIDE_PLATFORMS,
                    RR_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_ROOM, ENTRANCE_DOOR(344) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_TORCH_SIDE_PLATFORMS, ENTRANCE_DOOR(345) } },
                { { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_ROOM,
                    RR_GERUDO_TRAINING_GROUND_MQ_LOBBY, ENTRANCE_DOOR(346) },
                  { EntranceType::DoorGerudoTrainingGround, RR_GERUDO_TRAINING_GROUND_MQ_LOBBY,
                    RR_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_ROOM, ENTRANCE_DOOR(347) } },
            });
    }

    if (ctx->GetDungeon(Rando::GANONS_CASTLE)->IsVanilla()) {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_LOBBY, RR_GANONS_CASTLE_MAIN, ENTRANCE_DOOR(348) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MAIN, RR_GANONS_CASTLE_LOBBY,
                    ENTRANCE_DOOR(349) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MAIN, RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM,
                    ENTRANCE_DOOR(350) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM, RR_GANONS_CASTLE_MAIN,
                    ENTRANCE_DOOR(351) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM,
                    RR_GANONS_CASTLE_SPIRIT_TRIAL_BEFORE_SWITCH, ENTRANCE_DOOR(352) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_SPIRIT_TRIAL_BEFORE_SWITCH,
                    RR_GANONS_CASTLE_SPIRIT_TRIAL_BEAMOS_ROOM, ENTRANCE_DOOR(353) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_SPIRIT_TRIAL_AFTER_SWITCH,
                    RR_GANONS_CASTLE_SPIRIT_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(354) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_SPIRIT_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_SPIRIT_TRIAL_AFTER_SWITCH, ENTRANCE_DOOR(355) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MAIN, RR_GANONS_CASTLE_LIGHT_TRIAL_CHESTS_ROOM,
                    ENTRANCE_DOOR(356) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_LIGHT_TRIAL_CHESTS_ROOM, RR_GANONS_CASTLE_MAIN,
                    ENTRANCE_DOOR(357) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_LIGHT_TRIAL_TRIFORCE_ROOM,
                    RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM, ENTRANCE_DOOR(358) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM,
                    RR_GANONS_CASTLE_LIGHT_TRIAL_TRIFORCE_ROOM, ENTRANCE_DOOR(359) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM,
                    RR_GANONS_CASTLE_LIGHT_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(360) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_LIGHT_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_LIGHT_TRIAL_BOULDER_ROOM, ENTRANCE_DOOR(361) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MAIN, RR_GANONS_CASTLE_FIRE_TRIAL_FROM_OPEN,
                    ENTRANCE_DOOR(362) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_FIRE_TRIAL_OPEN_DOOR, RR_GANONS_CASTLE_MAIN,
                    ENTRANCE_DOOR(363) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_FIRE_TRIAL_BARRED_DOOR,
                    RR_GANONS_CASTLE_FIRE_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(364) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_FIRE_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_FIRE_TRIAL_FROM_BARRED, ENTRANCE_DOOR(365) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MAIN, RR_GANONS_CASTLE_SHADOW_TRIAL_START,
                    ENTRANCE_DOOR(366) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_SHADOW_TRIAL_START, RR_GANONS_CASTLE_MAIN,
                    ENTRANCE_DOOR(367) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_SHADOW_TRIAL_END,
                    RR_GANONS_CASTLE_SHADOW_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(368) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_SHADOW_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_SHADOW_TRIAL_END, ENTRANCE_DOOR(369) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MAIN, RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM,
                    ENTRANCE_DOOR(370) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM, RR_GANONS_CASTLE_MAIN,
                    ENTRANCE_DOOR(371) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM,
                    RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM, ENTRANCE_DOOR(372) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM,
                    RR_GANONS_CASTLE_WATER_TRIAL_BLUE_FIRE_ROOM, ENTRANCE_DOOR(373) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM_END,
                    RR_GANONS_CASTLE_WATER_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(374) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_WATER_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_WATER_TRIAL_BLOCK_ROOM_END, ENTRANCE_DOOR(375) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MAIN, RR_GANONS_CASTLE_FOREST_TRIAL_WOLFOS_ROOM,
                    ENTRANCE_DOOR(376) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_FOREST_TRIAL_WOLFOS_ROOM, RR_GANONS_CASTLE_MAIN,
                    ENTRANCE_DOOR(377) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_FOREST_TRIAL_WOLFOS_ROOM,
                    RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM, ENTRANCE_DOOR(378) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM,
                    RR_GANONS_CASTLE_FOREST_TRIAL_WOLFOS_ROOM, ENTRANCE_DOOR(379) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_FINAL_DOOR,
                    RR_GANONS_CASTLE_FOREST_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(380) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_FOREST_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_FOREST_TRIAL_BEAMOS_ROOM_FINAL_DOOR, ENTRANCE_DOOR(381) } },
            });
    } else {
        entranceShuffleTable.insert(
            entranceShuffleTable.end(),
            {
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_LOBBY, RR_GANONS_CASTLE_MQ_MAIN,
                    ENTRANCE_DOOR(348) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_MAIN, RR_GANONS_CASTLE_MQ_LOBBY,
                    ENTRANCE_DOOR(349) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_MAIN,
                    RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM, ENTRANCE_DOOR(350) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM,
                    RR_GANONS_CASTLE_MQ_MAIN, ENTRANCE_DOOR(351) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM,
                    RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH, ENTRANCE_DOOR(352) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_BEFORE_SWITCH,
                    RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_CHAIRS_ROOM, ENTRANCE_DOOR(353) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH,
                    RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(354) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_MQ_SPIRIT_TRIAL_AFTER_SWITCH, ENTRANCE_DOOR(355) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_MAIN,
                    RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM, ENTRANCE_DOOR(356) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_DINOLFOS_ROOM,
                    RR_GANONS_CASTLE_MQ_MAIN, ENTRANCE_DOOR(357) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM,
                    RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT, ENTRANCE_DOOR(358) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_FRONT,
                    RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_TRIFORCE_ROOM, ENTRANCE_DOOR(359) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK,
                    RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(360) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_MQ_LIGHT_TRIAL_BOULDER_ROOM_BACK, ENTRANCE_DOOR(361) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_MAIN, RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FROM_OPEN,
                    ENTRANCE_DOOR(362) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_FIRE_TRIAL_OPEN_DOOR, RR_GANONS_CASTLE_MQ_MAIN,
                    ENTRANCE_DOOR(363) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_FIRE_TRIAL_BARRED_DOOR,
                    RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(364) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_FIRE_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_MQ_FIRE_TRIAL_BARRED_DOOR, ENTRANCE_DOOR(365) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_MAIN,
                    RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE, ENTRANCE_DOOR(366) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_STARTING_LEDGE,
                    RR_GANONS_CASTLE_MQ_MAIN, ENTRANCE_DOOR(367) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BARRED_DOOR,
                    RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(368) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_MQ_SHADOW_TRIAL_BARRED_DOOR, ENTRANCE_DOOR(369) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_MAIN,
                    RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM, ENTRANCE_DOOR(370) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM,
                    RR_GANONS_CASTLE_MQ_MAIN, ENTRANCE_DOOR(371) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM,
                    RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM, ENTRANCE_DOOR(372) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM,
                    RR_GANONS_CASTLE_MQ_WATER_TRIAL_GEYSER_ROOM, ENTRANCE_DOOR(373) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM_END,
                    RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(374) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_WATER_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_MQ_WATER_TRIAL_BLOCK_ROOM_END, ENTRANCE_DOOR(375) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_MAIN,
                    RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM, ENTRANCE_DOOR(376) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM,
                    RR_GANONS_CASTLE_MQ_MAIN, ENTRANCE_DOOR(377) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM,
                    RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM, ENTRANCE_DOOR(378) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM,
                    RR_GANONS_CASTLE_MQ_FOREST_TRIAL_STALFOS_ROOM, ENTRANCE_DOOR(379) } },
                { { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_END,
                    RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM, ENTRANCE_DOOR(380) },
                  { EntranceType::DoorGanonsCastle, RR_GANONS_CASTLE_MQ_FOREST_TRIAL_FINAL_ROOM,
                    RR_GANONS_CASTLE_MQ_FOREST_TRIAL_BEAMOS_ROOM_END, ENTRANCE_DOOR(381) } },
            });
    }

    for (auto& entrancePair : entranceShuffleTable) {
        auto& forwardEntry = entrancePair.first;
        auto& returnEntry = entrancePair.second;

        // set data
        Entrance* forwardEntrance = RegionTable(forwardEntry.parentRegion)->GetExit(forwardEntry.connectedRegion);
        forwardEntrance->SetIndex(forwardEntry.index);
        forwardEntrance->SetType(forwardEntry.type);
        forwardEntrance->SetAsPrimary();
        entranceMap[forwardEntry.index] = forwardEntrance;

        // When decouple entrances is on, mark the forward entrance
        if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
            forwardEntrance->SetDecoupled();
        }

        if (returnEntry.parentRegion != RR_NONE) {
            Entrance* returnEntrance = RegionTable(returnEntry.parentRegion)->GetExit(returnEntry.connectedRegion);
            returnEntrance->SetIndex(returnEntry.index);
            returnEntrance->SetType(returnEntry.type);
            forwardEntrance->BindTwoWay(returnEntrance);
            entranceMap[returnEntry.index] = returnEntrance;

            // Mark reverse entrance as decoupled
            if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                returnEntrance->SetDecoupled();
            }
        }
    }
}

static void SetShuffledEntrances(EntrancePools entrancePools) {
    for (auto& pool : entrancePools) {
        for (Entrance* entrance : pool.second) {
            entrance->SetAsShuffled();
            if (entrance->GetReverse() != nullptr) {
                entrance->GetReverse()->SetAsShuffled();
            }
        }
    }
}

static std::vector<Entrance*>
BuildOneWayTargets(std::vector<EntranceType> typesToInclude,
                   std::vector<std::pair<RandomizerRegion, RandomizerRegion>> exclude = {} /*, target_region_names*/) {
    std::vector<Entrance*> oneWayEntrances = {};
    // Get all entrances of the specified type
    for (EntranceType poolType : typesToInclude) {
        SohUtils::AppendVector(oneWayEntrances, GetShuffleableEntrances(poolType, false));
    }
    // Filter out any that are passed in the exclusion list
    std::erase_if(oneWayEntrances, [&exclude](Entrance* entrance) {
        std::pair<RandomizerRegion, RandomizerRegion> entranceBeingChecked(entrance->GetParentRegionKey(),
                                                                           entrance->GetConnectedRegionKey());
        return SohUtils::Contains(entranceBeingChecked, exclude);
    });

    // The code below is part of the function in ootr, but no use of the function ever provides target_region_names
    // if target_region_names:
    //     return [entrance.get_new_target() for entrance in valid_one_way_entrances
    //             if entrance.connected_region.name in target_region_names]

    std::vector<Entrance*> newTargets = {};
    for (Entrance* entrance : oneWayEntrances) {
        newTargets.push_back(entrance->GetNewTarget());
    }
    return newTargets;
}

std::vector<Entrance*> EntranceShuffler::AssumeEntrancePool(std::vector<Entrance*>& entrancePool) {
    auto ctx = Rando::Context::GetInstance();
    std::vector<Entrance*> assumedPool = {};
    for (Entrance* entrance : entrancePool) {
        mTotalRandomizableEntrances++;
        Entrance* assumedForward = entrance->AssumeReachable();
        if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
            Entrance* assumedReturn = entrance->GetReverse()->AssumeReachable();
            if (!(ctx->GetOption(RSK_MIXED_ENTRANCE_POOLS) &&
                  (ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES) ||
                   ctx->GetOption(RSK_SHUFFLE_INTERIOR_ENTRANCES).Is(RO_INTERIOR_ENTRANCE_SHUFFLE_ALL)))) {
                auto type = entrance->GetType();
                if (((type == EntranceType::Dungeon || type == EntranceType::GrottoGrave) &&
                     entrance->GetReverse()->GetName() !=
                         "Spirit Temple Entryway -> Desert Colossus From Spirit Entryway") ||
                    (type == EntranceType::Interior &&
                     ctx->GetOption(RSK_SHUFFLE_INTERIOR_ENTRANCES).Is(RO_INTERIOR_ENTRANCE_SHUFFLE_ALL))) {
                    // In most cases, Dungeon, Grotto/Grave and Simple Interior exits shouldn't be assumed able to give
                    // access to their parent region
                    assumedReturn->SetCondition([] { return false; });
                }
            }
            assumedForward->BindTwoWay(assumedReturn);
        }
        assumedPool.push_back(assumedForward);
    }
    return assumedPool;
}

static bool AreEntrancesCompatible(Entrance* entrance, Entrance* target, std::vector<EntrancePair>& rollbacks) {
    // Door shuffle only supported within same scene
    if (entrance->GetType() >= EntranceType::DoorDekuTree && entrance->GetType() <= EntranceType::DoorGanonsTower) {
        // avoid room leading to itself, causes duplicate actors
        auto entranceDoor = DoorsList[entrance->GetIndex() - ENTRANCE_DOOR_START];
        auto targetIndex = (target->GetType() == EntranceType::None ? target->GetReplacement() : target)->GetIndex();
        auto targetDoor = DoorsList[targetIndex - ENTRANCE_DOOR_START];
        return entranceDoor.srcRoom != targetDoor.dstRoom;
    }

    // Entrances shouldn't connect to their own scene, fail in this situation
    if (
        // allow "special" areas to connect to eachother
        entrance->GetParentRegion()->scene != SCENE_ID_MAX &&
        // allow grottos and fairy fountains to connect to eachother
        entrance->GetParentRegion()->scene != SCENE_GROTTOS &&
        entrance->GetParentRegion()->scene != SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC &&
        entrance->GetParentRegion()->scene != SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS &&

        (entrance->GetParentRegion()->scene == target->GetConnectedRegion()->scene ||

         // prevent SCENE_HYRULE_CASTLE & SCENE_OUTSIDE_GANONS_CASTLE from connecting to eachother
         (entrance->GetParentRegion()->scene == SCENE_HYRULE_CASTLE &&
          target->GetConnectedRegion()->scene == SCENE_OUTSIDE_GANONS_CASTLE) ||
         (entrance->GetParentRegion()->scene == SCENE_OUTSIDE_GANONS_CASTLE &&
          target->GetConnectedRegion()->scene == SCENE_HYRULE_CASTLE))) {
        SPDLOG_DEBUG("Entrance {} attempted to connect with own scene target {}. Connection failed.",
                     entrance->GetName(), target->to_string());
        return false;
    }

    // One way entrances shouldn't lead to the same scene as other already chosen one way entrances
    auto type = entrance->GetType();
    const std::array<EntranceType, 3> oneWayTypes = { EntranceType::OwlDrop, EntranceType::Spawn,
                                                      EntranceType::WarpSong };
    if (SohUtils::Contains(type, oneWayTypes)) {
        for (auto& rollback : rollbacks) {
            if (rollback.first->GetConnectedRegion()->scene == target->GetConnectedRegion()->scene) {
                SPDLOG_DEBUG("A one way entrance already leads to {}. Connection failed.", target->to_string());
                return false;
            }
        }
    }

    return true;
}

// Change connections between an entrance and a target assumed entrance, in order to test the connections afterwards if
// necessary
static void ChangeConnections(Entrance* entrance, Entrance* targetEntrance) {
    SPDLOG_DEBUG("Attempting to connect {} to {}", entrance->GetName(), targetEntrance->to_string());
    entrance->Connect(targetEntrance->Disconnect());
    entrance->SetReplacement(targetEntrance->GetReplacement());
    if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
        targetEntrance->GetReplacement()->GetReverse()->Connect(entrance->GetReverse()->GetAssumed()->Disconnect());
        targetEntrance->GetReplacement()->GetReverse()->SetReplacement(entrance->GetReverse());
    }
}

static bool EntranceUnreachableAs(Entrance* entrance, uint8_t age, std::vector<Entrance*>& alreadyChecked) {

    if (entrance == nullptr) {
        SPDLOG_DEBUG("Entrance is nullptr in EntranceUnreachableAs()");
        return true;
    }

    alreadyChecked.push_back(entrance);
    auto type = entrance->GetType();

    // The following cases determine when we say an entrance is not safe to affirm unreachable as the given age
    if (type == EntranceType::WarpSong || type == EntranceType::Overworld) {
        // Note that we consider all overworld entrances as potentially accessible as both ages, to be completely safe
        return false;
    } else if (type == EntranceType::OwlDrop) {
        return age == RO_AGE_ADULT;
    } else if (type == EntranceType::Spawn && entrance->GetConnectedRegionKey() == RR_KF_LINKS_HOUSE) {
        return age == RO_AGE_ADULT;
    } else if (type == EntranceType::Spawn && entrance->GetConnectedRegionKey() == RR_TEMPLE_OF_TIME) {
        return age == RO_AGE_CHILD;
    }

    // Other entrances such as Interior, Dungeon or Grotto are fine unless they have a parent which is one of the above
    // cases Recursively check parent entrances to verify that they are also not reachable as the wrong age
    auto& parentEntrances = entrance->GetParentRegion()->entrances;
    for (Entrance* parentEntrance : parentEntrances) {

        // if parentEntrance is in alreadyChecked, then continue
        if (SohUtils::Contains(parentEntrance, alreadyChecked)) {
            continue;
        }

        bool unreachable = EntranceUnreachableAs(parentEntrance, age, alreadyChecked);
        if (!unreachable) {
            return false;
        }
    }

    return true;
}

static bool ValidateWorld(Entrance* entrancePlaced) {
    auto ctx = Rando::Context::GetInstance();
    SPDLOG_DEBUG("Validating world");

    // check certain conditions when certain types of ER are enabled
    EntranceType type = EntranceType::None;
    if (entrancePlaced != nullptr) {
        type = entrancePlaced->GetType();
    }

    bool checkOtherEntranceAccess =
        (ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES) ||
         (ctx->GetOption(RSK_FOREST).Is(RO_CLOSED_FOREST_ON) && ctx->GetOption(RSK_SHUFFLE_GROTTO_ENTRANCES)) ||
         ctx->GetOption(RSK_SHUFFLE_INTERIOR_ENTRANCES).Is(RO_INTERIOR_ENTRANCE_SHUFFLE_ALL) ||
         ctx->GetOption(RSK_SHUFFLE_THIEVES_HIDEOUT_ENTRANCES) || ctx->GetOption(RSK_SHUFFLE_OVERWORLD_SPAWNS) ||
         ctx->GetOption(RSK_SHUFFLE_DUNGEON_DOORS)) &&
        (entrancePlaced == nullptr || ctx->GetOption(RSK_MIXED_ENTRANCE_POOLS) ||
         type == EntranceType::SpecialInterior || type == EntranceType::Overworld ||
         type == EntranceType::ThievesHideout || type == EntranceType::Spawn || type == EntranceType::WarpSong ||
         type == EntranceType::OwlDrop ||
         (type >= EntranceType::DoorDekuTree && type <= EntranceType::DoorGanonsTower));

    // Search the world to verify that all necessary conditions are still being held
    // Conditions will be checked during the search and any that fail will be figured out
    // afterwards
    ctx->GetLogic()->Reset();
    ValidateEntrances(checkOtherEntranceAccess);

    if (!ctx->GetOption(RSK_DECOUPLED_ENTRANCES) &&
        !ctx->GetOption(RSK_SHUFFLE_DUNGEON_DOORS).Is(RO_SHUFFLE_DUNGEON_DOORS_MIX)) {
        // Unless entrances are decoupled, we don't want the player to end up through certain entrances as the wrong age
        // This means we need to hard check that none of the relevant entrances are ever reachable as that age
        // This is mostly relevant when mixing entrance pools or shuffling special interiors (such as windmill or kak
        // potion shop) Warp Songs and Overworld Spawns can also end up inside certain indoors so those need to be
        // handled as well
        std::array<std::string, 1> childForbidden = { "OGC Great Fairy Fountain -> Castle Grounds" };
        std::array<std::string, 2> adultForbidden = { "HC Great Fairy Fountain -> Castle Grounds",
                                                      "HC Storms Grotto -> Castle Grounds" };

        auto allShuffleableEntrances = GetShuffleableEntrances(EntranceType::All, false);
        for (auto& entrance : allShuffleableEntrances) {
            std::vector<Entrance*> alreadyChecked = {};

            if (entrance->IsShuffled()) {
                if (entrance->GetReplacement() != nullptr) {
                    auto replacementName = entrance->GetReplacement()->GetName();
                    alreadyChecked.push_back(entrance->GetReplacement()->GetReverse());

                    if (SohUtils::Contains(replacementName, childForbidden) &&
                        !EntranceUnreachableAs(entrance, RO_AGE_CHILD, alreadyChecked)) {
                        SPDLOG_DEBUG("{} is replaced by an entrance with a potential child access", replacementName);
                        return false;
                    } else if (SohUtils::Contains(replacementName, adultForbidden) &&
                               !EntranceUnreachableAs(entrance, RO_AGE_ADULT, alreadyChecked)) {
                        SPDLOG_DEBUG("{} is replaced by an entrance with a potential adult access", replacementName);
                        return false;
                    }
                }
            } else {
                auto name = entrance->GetName();
                alreadyChecked.push_back(entrance->GetReverse());

                if (SohUtils::Contains(name, childForbidden) &&
                    !EntranceUnreachableAs(entrance, RO_AGE_CHILD, alreadyChecked)) {
                    SPDLOG_DEBUG("{} is potentially accessible as child", name);
                    return false;
                } else if (SohUtils::Contains(name, adultForbidden) &&
                           !EntranceUnreachableAs(entrance, RO_AGE_ADULT, alreadyChecked)) {
                    SPDLOG_DEBUG("{} is potentially accessible as adult");
                    return false;
                }
            }
        }
    }

    // If all locations aren't reachable, that means that one of the conditions failed when searching
    if (!Rando::Context::GetInstance()->allLocationsReachable) {
        if (checkOtherEntranceAccess) {
            // At least one valid starting region with all basic refills should be reachable without using any items at
            // the beginning of the seed
            if (!RegionTable(RR_KOKIRI_FOREST)->HasAccess() && !RegionTable(RR_KAKARIKO_VILLAGE)->HasAccess()) {
                SPDLOG_DEBUG("Invalid starting area");
                return false;
            }

            // Check that a region where time passes is always reachable as both ages without having collected any items
            if (!Regions::HasTimePassAccess(RO_AGE_CHILD) || !Regions::HasTimePassAccess(RO_AGE_ADULT)) {
                SPDLOG_DEBUG("Time passing is not guaranteed as both ages");
                return false;
            }

            // The player should be able to get back to ToT after going through time, without having collected any items
            // This is important to ensure that the player never loses access to the pedestal after going through time
            if (ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_CHILD) &&
                !RegionTable(RR_TEMPLE_OF_TIME)->Adult()) {
                SPDLOG_DEBUG("Path to Temple of Time as adult is not guaranteed");
                return false;
            } else if (ctx->GetOption(RSK_SELECTED_STARTING_AGE).Is(RO_AGE_ADULT) &&
                       !RegionTable(RR_TEMPLE_OF_TIME)->Child()) {
                SPDLOG_DEBUG("Path to Temple of Time as child is not guaranteed");
                return false;
            }
        }

        SPDLOG_DEBUG("All Locations NOT REACHABLE");
        return false;
    }
    return true;
}

// In the event that we need to retry shuffling an entire group we can restore the
// original connections to reset the entrance and target entrance.
static void RestoreConnections(Entrance* entrance, Entrance* targetEntrance) {
    targetEntrance->Connect(entrance->Disconnect());
    entrance->SetReplacement(nullptr);
    if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
        entrance->GetReverse()->GetAssumed()->Connect(targetEntrance->GetReplacement()->GetReverse()->Disconnect());
        targetEntrance->GetReplacement()->GetReverse()->SetReplacement(nullptr);
    }
}

static void DeleteTargetEntrance(Entrance* targetEntrance) {
    if (targetEntrance->GetConnectedRegionKey() != RR_NONE) {
        targetEntrance->Disconnect();
    }
    if (targetEntrance->GetParentRegionKey() != RR_NONE) {
        targetEntrance->GetParentRegion()->RemoveExit(targetEntrance);
        targetEntrance->SetParentRegion(RR_NONE);
    }
}

static void ConfirmReplacement(Entrance* entrance, Entrance* targetEntrance) {
    DeleteTargetEntrance(targetEntrance);
    if (entrance->GetReverse() != nullptr && !entrance->IsDecoupled()) {
        auto replacedReverse = targetEntrance->GetReplacement()->GetReverse();
        DeleteTargetEntrance(replacedReverse->GetReverse()->GetAssumed());
    }
}

bool EntranceShuffler::ReplaceEntrance(Entrance* entrance, Entrance* target, std::vector<EntrancePair>& rollbacks) {
    if (!AreEntrancesCompatible(entrance, target, rollbacks)) {
        return false;
    }
    ChangeConnections(entrance, target);
    if (ValidateWorld(entrance)) {
#ifdef ENABLE_DEBUG
        std::string ticks = std::to_string(svcGetSystemTick());
        // SPDLOG_DEBUG("Dumping World Graph at {}", ticks);
        // Regions::DumpWorldGraph(ticks);
#endif
        rollbacks.push_back(EntrancePair{ entrance, target });
        mCurNumRandomizedEntrances++;
        return true;
    } else {
#ifdef ENABLE_DEBUG
        std::string ticks = std::to_string(svcGetSystemTick());
        // SPDLOG_DEBUG("Dumping World Graph at {}", ticks);
        // Regions::DumpWorldGraph(ticks);
#endif
        if (entrance->GetConnectedRegionKey() != RR_NONE) {
            RestoreConnections(entrance, target);
        }
    }
    return false;
}

bool EntranceShuffler::PlaceOneWayPriorityEntrance(
    std::string priorityName, std::list<RandomizerRegion>& allowedRegions, std::list<EntranceType>& allowedTypes,
    std::vector<EntrancePair>& rollbacks, EntrancePools oneWayEntrancePools, EntrancePools oneWayTargetEntrancePools) {
    auto ctx = Rando::Context::GetInstance();
    // Combine the entrances for allowed types in one list.
    // Shuffle this list.
    // Pick the first one not already set, not adult spawn, that has a valid target entrance.
    // Assemble then clear entrances from the pool and target pools as appropriate.
    std::vector<Entrance*> availPool = {};
    for (auto& pool : oneWayEntrancePools) {
        auto entranceType = pool.first;
        if (SohUtils::Contains(entranceType, allowedTypes)) {
            SohUtils::AppendVector(availPool, pool.second);
        }
    }
    Shuffle(availPool);

    for (Entrance* entrance : availPool) {
        if (entrance->GetReplacement() != nullptr) {
            continue;
        }
        // Only allow Adult Spawn as sole Nocturne access if hints != mask.
        // Otherwise, child access is required here (adult access assumed or guaranteed later).
        if (entrance->GetParentRegionKey() == RR_ADULT_SPAWN) {
            if (priorityName != "Nocturne" || ctx->GetOption(RSK_GOSSIP_STONE_HINTS).Is(RO_GOSSIP_STONES_NEED_TRUTH)) {
                continue;
            }
        }
        // If not shuffling dungeons, Nocturne requires adult access
        if (!ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES) && priorityName == "Nocturne") {
            if (entrance->GetType() != EntranceType::WarpSong && entrance->GetParentRegionKey() != RR_ADULT_SPAWN) {
                continue;
            }
        }
        for (Entrance* target : oneWayTargetEntrancePools[entrance->GetType()]) {
            RandomizerRegion targetRegionKey = target->GetConnectedRegionKey();
            if (targetRegionKey != RR_NONE && SohUtils::Contains(targetRegionKey, allowedRegions)) {
                if (ReplaceEntrance(entrance, target, rollbacks)) {
                    // Return once the entrance has been replaced
                    return true;
                }
            }
        }
    }
    SPDLOG_DEBUG("ERROR: Unable to place priority one-way entrance for {}", priorityName);
    assert(false);
    return false;
}

bool EntranceShuffler::ShuffleOneWayPriorityEntrances(std::map<std::string, PriorityEntrance>& oneWayPriorities,
                                                      EntrancePools oneWayEntrancePools,
                                                      EntrancePools oneWayTargetEntrancePools, int retryCount) {
    while (retryCount > 0) {
        retryCount--;
        std::vector<EntrancePair> rollbacks = {};
        rollbacks.reserve(oneWayPriorities.size());

        bool success = true;
        for (auto& priority : oneWayPriorities) {
            std::string key = priority.first;
            auto& regions = priority.second.targetRegions;
            auto& types = priority.second.allowedTypes;
            success = PlaceOneWayPriorityEntrance(key, regions, types, rollbacks, oneWayEntrancePools,
                                                  oneWayTargetEntrancePools);
            if (!success) {
                for (auto& pair : rollbacks) {
                    RestoreConnections(pair.first, pair.second);
                }
                break;
            }
        }
        if (!success) {
            continue;
        }
        // If there are no issues, log the connections and continue
        for (auto& pair : rollbacks) {
            ConfirmReplacement(pair.first, pair.second);
        }
        break;
    }

    if (retryCount <= 0) {
        SPDLOG_DEBUG(
            "Entrance placement attempt count for one way priorities exceeded. Restarting randomization completely");
        mEntranceShuffleFailure = true;
        return false;
    }
    return true;
}

// returns restrictive entrances and soft entrances in an array of size 2 (restrictive is index 0, soft is index 1)
static std::array<std::vector<Entrance*>, 2> SplitEntrancesByRequirements(std::vector<Entrance*>& entrancesToSplit,
                                                                          std::vector<Entrance*>& assumedEntrances) {
    // First, disconnect all root assumed entrances and save which regions they were originally connected to, so we can
    // reconnect them later
    std::map<Entrance*, RandomizerRegion> originalConnectedRegions = {};
    std::set<Entrance*> entrancesToDisconnect = {};
    for (Entrance* entrance : assumedEntrances) {
        entrancesToDisconnect.insert(entrance);
        if (entrance->GetReverse() != nullptr) {
            entrancesToDisconnect.insert(entrance->GetReverse());
        }
    }

    // disconnect each entrance temporarily to find restrictive vs soft entrances
    // soft entrances are ones that can be accessed by both ages (child/adult) at both times of day (day/night)
    // restrictive entrances are ones that do not meet this criteria
    for (Entrance* entrance : entrancesToDisconnect) {
        if (entrance->GetConnectedRegionKey() != RR_NONE) {
            originalConnectedRegions[entrance] = entrance->Disconnect();
        }
    }

    std::vector<Entrance*> restrictiveEntrances = {};
    std::vector<Entrance*> softEntrances = {};

    logic->Reset();
    // Apply the effects of all advancement items to search for entrance accessibility
    std::vector<RandomizerGet> items = FilterFromPool(
        itemPool, [](const RandomizerGet i) { return Rando::StaticData::RetrieveItem(i).IsAdvancement(); });
    for (RandomizerGet unplacedItem : items) {
        Rando::StaticData::RetrieveItem(unplacedItem).ApplyEffect();
    }
    // run a search to see what's accessible
    ReachabilitySearch({});

    for (Entrance* entrance : entrancesToSplit) {
        logic->CurrentRegionKey = entrance->GetParentRegionKey();
        // if an entrance is accessible at all times of day by both ages, it's a soft entrance with no restrictions
        if (entrance->ConditionsMet(true)) {
            softEntrances.push_back(entrance);
        } else {
            restrictiveEntrances.push_back(entrance);
        }
        logic->CurrentRegionKey = RR_NONE;
    }

    // Reconnect all disconnected entrances
    for (Entrance* entrance : entrancesToDisconnect) {
        entrance->Connect(originalConnectedRegions[entrance]);
    }

    return { restrictiveEntrances, softEntrances };
}

// Shuffle entrances by placing them instead of entrances in the provided target entrances list
bool EntranceShuffler::ShuffleEntrances(std::vector<Entrance*>& entrances, std::vector<Entrance*>& targetEntrances,
                                        std::vector<EntrancePair>& rollbacks) {
    auto ctx = Rando::Context::GetInstance();

    Shuffle(entrances);

    // place all entrances in the pool, validating after every placement
    for (Entrance* entrance : entrances) {
        if (entrance->GetConnectedRegionKey() != RR_NONE) {
            continue;
        }

        Shuffle(targetEntrances);
        for (Entrance* target : targetEntrances) {
            if (target->GetConnectedRegionKey() == RR_NONE) {
                continue;
            }

            if (ReplaceEntrance(entrance, target, rollbacks)) {
                break;
            }
        }

        if (entrance->GetConnectedRegionKey() == RR_NONE) {
            return false;
        }
    }

    // all entrances were validly connected
    return true;
}

void EntranceShuffler::ShuffleEntrancePool(std::vector<Entrance*>& entrancePool,
                                           std::vector<Entrance*>& targetEntrances, int retryCount) {
    mNoRandomEntrances = false;

    auto splitEntrances = SplitEntrancesByRequirements(entrancePool, targetEntrances);

    auto& restrictiveEntrances = splitEntrances[0];
    auto& softEntrances = splitEntrances[1];

    int retries = retryCount;
    while (retries > 0) {
        if (retries != retryCount) {
#ifdef ENABLE_DEBUG
            std::string ticks = std::to_string(svcGetSystemTick());
            SPDLOG_DEBUG("Failed to connect entrances. Retrying {} more times.", retries);
            SPDLOG_DEBUG("Dumping World Graph at {}", ticks);
            // Regions::DumpWorldGraph(ticks);
#endif
        }
        retries--;

        std::vector<EntrancePair> rollbacks = {};
        rollbacks.reserve(entrancePool.size());

        // Shuffle Restrictive Entrances first while more regions are available in
        // order to heavily reduce the chances of the placement failing
        bool success = ShuffleEntrances(restrictiveEntrances, targetEntrances, rollbacks);
        if (success) {
            success = ShuffleEntrances(softEntrances, targetEntrances, rollbacks);
            if (!success) {
                for (auto& pair : rollbacks) {
                    RestoreConnections(pair.first, pair.second);
                    mCurNumRandomizedEntrances--;
                }
                continue;
            }
        } else {
            for (auto& pair : rollbacks) {
                RestoreConnections(pair.first, pair.second);
                mCurNumRandomizedEntrances--;
            }
            continue;
        }

        // If there are no issues, log the connections and continue
        for (auto& pair : rollbacks) {
            ConfirmReplacement(pair.first, pair.second);
        }
        break;
    }

    if (retries <= 0) {
        SPDLOG_DEBUG("Entrance placement attempt count exceeded. Restarting randomization completely");
        mEntranceShuffleFailure = true;
    }
}

int EntranceShuffler::ShuffleAllEntrances() {
    auto ctx = Rando::Context::GetInstance();

    mTotalRandomizableEntrances = 0;
    mCurNumRandomizedEntrances = 0;

    std::map<std::string, PriorityEntrance> priorityEntranceTable = {
        { "Bolero", { { RR_DMC_PAD_ENTRY }, { EntranceType::OwlDrop, EntranceType::WarpSong } } },
        { "Nocturne",
          { { RR_GRAVEYARD_WARP_PAD_REGION },
            { EntranceType::OwlDrop, EntranceType::Spawn, EntranceType::WarpSong } } },
        { "Requiem",
          { { RR_DESERT_COLOSSUS, RR_DESERT_COLOSSUS_OUTSIDE_TEMPLE },
            { EntranceType::OwlDrop, EntranceType::Spawn, EntranceType::WarpSong } } },
    };

    mEntranceShuffleFailure = false;
    SetAllEntrancesData();

    EntrancePools oneWayEntrancePools = {};
    EntrancePools entrancePools = {};
    std::map<std::string, PriorityEntrance> oneWayPriorities = {};

    // Owl Drops
    if (ctx->GetOption(RSK_SHUFFLE_OWL_DROPS)) {
        oneWayEntrancePools[EntranceType::OwlDrop] = GetShuffleableEntrances(EntranceType::OwlDrop);
    }

    // Spawns
    if (ctx->GetOption(RSK_SHUFFLE_OVERWORLD_SPAWNS)) {
        oneWayEntrancePools[EntranceType::Spawn] = GetShuffleableEntrances(EntranceType::Spawn);
    }

    // Warpsongs
    if (ctx->GetOption(RSK_SHUFFLE_WARP_SONGS)) {
        oneWayEntrancePools[EntranceType::WarpSong] = GetShuffleableEntrances(EntranceType::WarpSong);
        // In Glitchless, there aren't any other ways to access these areas
        if (ctx->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS)) {
            oneWayPriorities["Bolero"] = priorityEntranceTable["Bolero"];
            oneWayPriorities["Nocturne"] = priorityEntranceTable["Nocturne"];
            if (!ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES) && !ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES)) {
                oneWayPriorities["Requiem"] = priorityEntranceTable["Requiem"];
            }
        }
    }

    // Shuffle Bosses
    if (ctx->GetOption(RSK_SHUFFLE_BOSS_ENTRANCES).IsNot(RO_BOSS_ROOM_ENTRANCE_SHUFFLE_OFF)) {
        if (ctx->GetOption(RSK_SHUFFLE_BOSS_ENTRANCES).Is(RO_BOSS_ROOM_ENTRANCE_SHUFFLE_FULL)) {
            entrancePools[EntranceType::Boss] = GetShuffleableEntrances(EntranceType::ChildBoss);
            SohUtils::AppendVector(entrancePools[EntranceType::Boss], GetShuffleableEntrances(EntranceType::AdultBoss));
            if (ctx->GetOption(RSK_SHUFFLE_GANONS_TOWER_ENTRANCE)) {
                SohUtils::AppendVector(entrancePools[EntranceType::Boss],
                                       GetShuffleableEntrances(EntranceType::GanonTower));
            }

            if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                for (Entrance* entrance : entrancePools[EntranceType::Boss]) {
                    entrancePools[EntranceType::BossReverse].push_back(entrance->GetReverse());
                }
            }
        } else {
            entrancePools[EntranceType::ChildBoss] = GetShuffleableEntrances(EntranceType::ChildBoss);
            entrancePools[EntranceType::AdultBoss] = GetShuffleableEntrances(EntranceType::AdultBoss);
            if (ctx->GetOption(RSK_SHUFFLE_GANONS_TOWER_ENTRANCE)) {
                SohUtils::AppendVector(entrancePools[EntranceType::AdultBoss],
                                       GetShuffleableEntrances(EntranceType::GanonTower));
            }

            if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                for (Entrance* entrance : entrancePools[EntranceType::ChildBoss]) {
                    entrancePools[EntranceType::ChildBossReverse].push_back(entrance->GetReverse());
                }
                for (Entrance* entrance : entrancePools[EntranceType::AdultBoss]) {
                    entrancePools[EntranceType::AdultBossReverse].push_back(entrance->GetReverse());
                }
            }
        }
    }

    // Shuffle Dungeon Entrances
    if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).IsNot(RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) {
        entrancePools[EntranceType::Dungeon] = GetShuffleableEntrances(EntranceType::Dungeon);
        // Add Ganon's Castle, if set to On + Ganon
        if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES).Is(RO_DUNGEON_ENTRANCE_SHUFFLE_ON_PLUS_GANON)) {
            SohUtils::AppendVector(entrancePools[EntranceType::Dungeon],
                                   GetShuffleableEntrances(EntranceType::GanonDungeon));
        }
        if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
            for (Entrance* entrance : entrancePools[EntranceType::Dungeon]) {
                entrancePools[EntranceType::DungeonReverse].push_back(entrance->GetReverse());
            }
        }
    }

    // Interior entrances
    if (ctx->GetOption(RSK_SHUFFLE_INTERIOR_ENTRANCES).IsNot(RO_INTERIOR_ENTRANCE_SHUFFLE_OFF)) {
        entrancePools[EntranceType::Interior] = GetShuffleableEntrances(EntranceType::Interior);
        // Special interiors
        if (ctx->GetOption(RSK_SHUFFLE_INTERIOR_ENTRANCES).Is(RO_INTERIOR_ENTRANCE_SHUFFLE_ALL)) {
            SohUtils::AppendVector(entrancePools[EntranceType::Interior],
                                   GetShuffleableEntrances(EntranceType::SpecialInterior));
        }
        if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
            for (Entrance* entrance : entrancePools[EntranceType::Interior]) {
                entrancePools[EntranceType::InteriorReverse].push_back(entrance->GetReverse());
            }
        }
    }

    // Thieves' Hideout entrances
    if (ctx->GetOption(RSK_SHUFFLE_THIEVES_HIDEOUT_ENTRANCES)) {
        entrancePools[EntranceType::ThievesHideout] = GetShuffleableEntrances(EntranceType::ThievesHideout);

        if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
            for (Entrance* entrance : entrancePools[EntranceType::ThievesHideout]) {
                entrancePools[EntranceType::ThievesHideoutReverse].push_back(entrance->GetReverse());
            }
        }
    }

    // grotto entrances
    if (ctx->GetOption(RSK_SHUFFLE_GROTTO_ENTRANCES)) {
        entrancePools[EntranceType::GrottoGrave] = GetShuffleableEntrances(EntranceType::GrottoGrave);

        if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
            for (Entrance* entrance : entrancePools[EntranceType::GrottoGrave]) {
                entrancePools[EntranceType::GrottoGraveReverse].push_back(entrance->GetReverse());
            }
        }
    }

    // overworld entrances
    if (ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES)) {
        bool excludeOverworldReverse =
            ctx->GetOption(RSK_MIX_OVERWORLD_ENTRANCES) && !ctx->GetOption(RSK_DECOUPLED_ENTRANCES);
        entrancePools[EntranceType::Overworld] =
            GetShuffleableEntrances(EntranceType::Overworld, excludeOverworldReverse);
        // Only shuffle GV Lower Stream -> Lake Hylia if decoupled entrances are on
        if (!ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
            std::erase_if(entrancePools[EntranceType::Overworld], [](const Entrance* entrance) {
                return entrance->GetParentRegionKey() == RR_GV_LOWER_STREAM &&
                       entrance->GetConnectedRegionKey() == RR_LAKE_HYLIA;
            });
        }
    }

    // door entrances
    if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_DOORS)) {
        entrancePools[EntranceType::DoorDekuTree] = GetShuffleableEntrances(EntranceType::DoorDekuTree);
        entrancePools[EntranceType::DoorDodongosCavern] = GetShuffleableEntrances(EntranceType::DoorDodongosCavern);
        entrancePools[EntranceType::DoorJabuJabu] = GetShuffleableEntrances(EntranceType::DoorJabuJabu);
        entrancePools[EntranceType::DoorForest] = GetShuffleableEntrances(EntranceType::DoorForest);
        entrancePools[EntranceType::DoorFire] = GetShuffleableEntrances(EntranceType::DoorFire);
        entrancePools[EntranceType::DoorWater] = GetShuffleableEntrances(EntranceType::DoorWater);
        entrancePools[EntranceType::DoorShadow] = GetShuffleableEntrances(EntranceType::DoorShadow);
        entrancePools[EntranceType::DoorSpirit] = GetShuffleableEntrances(EntranceType::DoorSpirit);
        entrancePools[EntranceType::DoorBottomOfTheWell] = GetShuffleableEntrances(EntranceType::DoorBottomOfTheWell);
        entrancePools[EntranceType::DoorIceCavern] = GetShuffleableEntrances(EntranceType::DoorIceCavern);
        entrancePools[EntranceType::DoorGerudoTrainingGround] =
            GetShuffleableEntrances(EntranceType::DoorGerudoTrainingGround);
        entrancePools[EntranceType::DoorGanonsCastle] = GetShuffleableEntrances(EntranceType::DoorGanonsCastle);
        entrancePools[EntranceType::DoorGanonsTower] = GetShuffleableEntrances(EntranceType::DoorGanonsTower);

        if (ctx->GetOption(RSK_SHUFFLE_DUNGEON_DOORS).Is(RO_SHUFFLE_DUNGEON_DOORS_MIX)) {
            for (int i = (int)EntranceType::DoorDodongosCavern; i <= (int)EntranceType::DoorGanonsTower; i++) {
                auto type = (EntranceType)i;
                SohUtils::AppendVector(entrancePools[EntranceType::DoorDekuTree], entrancePools[type]);
                entrancePools[type].clear();
            }
        }
    }

    // Set shuffled entrances as such
    SetShuffledEntrances(entrancePools);
    SetShuffledEntrances(oneWayEntrancePools);

    // combine entrance pools if mixing pools. Only continue if more than one pool is selected.
    int totalMixedPools =
        (ctx->GetOption(RSK_MIX_DUNGEON_ENTRANCES) ? 1 : 0) + (ctx->GetOption(RSK_MIX_BOSS_ENTRANCES) ? 1 : 0) +
        (ctx->GetOption(RSK_MIX_OVERWORLD_ENTRANCES) ? 1 : 0) + (ctx->GetOption(RSK_MIX_INTERIOR_ENTRANCES) ? 1 : 0) +
        (ctx->GetOption(RSK_MIX_THIEVES_HIDEOUT_ENTRANCES) ? 1 : 0) +
        (ctx->GetOption(RSK_MIX_GROTTO_ENTRANCES) ? 1 : 0);
    if (totalMixedPools < 2) {
        ctx->GetOption(RSK_MIXED_ENTRANCE_POOLS).Set(RO_GENERIC_OFF);
        ctx->GetOption(RSK_MIX_DUNGEON_ENTRANCES).Set(RO_GENERIC_OFF);
        ctx->GetOption(RSK_MIX_BOSS_ENTRANCES).Set(RO_GENERIC_OFF);
        ctx->GetOption(RSK_MIX_OVERWORLD_ENTRANCES).Set(RO_GENERIC_OFF);
        ctx->GetOption(RSK_MIX_INTERIOR_ENTRANCES).Set(RO_GENERIC_OFF);
        ctx->GetOption(RSK_MIX_THIEVES_HIDEOUT_ENTRANCES).Set(RO_GENERIC_OFF);
        ctx->GetOption(RSK_MIX_GROTTO_ENTRANCES).Set(RO_GENERIC_OFF);
    }
    if (ctx->GetOption(RSK_MIXED_ENTRANCE_POOLS)) {
        std::set<EntranceType> poolsToMix = {};
        if (ctx->GetOption(RSK_MIX_DUNGEON_ENTRANCES)) {
            poolsToMix.insert(EntranceType::Dungeon);
            if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                poolsToMix.insert(EntranceType::DungeonReverse);
            }
        }
        if (ctx->GetOption(RSK_MIX_BOSS_ENTRANCES)) {
            poolsToMix.insert(EntranceType::Boss);
            if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                poolsToMix.insert(EntranceType::BossReverse);
            }
        }
        if (ctx->GetOption(RSK_MIX_OVERWORLD_ENTRANCES)) {
            poolsToMix.insert(EntranceType::Overworld);
        }
        if (ctx->GetOption(RSK_MIX_INTERIOR_ENTRANCES)) {
            poolsToMix.insert(EntranceType::Interior);
            if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                poolsToMix.insert(EntranceType::InteriorReverse);
            }
        }
        if (ctx->GetOption(RSK_MIX_THIEVES_HIDEOUT_ENTRANCES)) {
            poolsToMix.insert(EntranceType::ThievesHideout);
            if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                poolsToMix.insert(EntranceType::ThievesHideoutReverse);
            }
        }
        if (ctx->GetOption(RSK_MIX_GROTTO_ENTRANCES)) {
            poolsToMix.insert(EntranceType::GrottoGrave);
            if (ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                poolsToMix.insert(EntranceType::GrottoGraveReverse);
            }
        }

        for (auto& pool : entrancePools) {

            auto type = pool.first;

            if (poolsToMix.count(type) > 0) {
                SohUtils::AppendVector(entrancePools[EntranceType::Mixed], pool.second);
                entrancePools[type].clear();
            }
        }
    }

    // Build target entrance pools and set the assumption for entrances being reachable
    EntrancePools oneWayTargetEntrancePools = {};
    for (auto& pool : oneWayEntrancePools) {

        std::vector<EntranceType> validTargetTypes = {};
        EntranceType poolType = pool.first;

        if (poolType == EntranceType::OwlDrop) {
            validTargetTypes = { EntranceType::WarpSong, EntranceType::OwlDrop, EntranceType::Overworld,
                                 EntranceType::Extra };
            oneWayTargetEntrancePools[poolType] =
                BuildOneWayTargets(validTargetTypes, { std::make_pair(RR_PRELUDE_OF_LIGHT_WARP, RR_TEMPLE_OF_TIME) });
            // Owl Drops are only accessible as child, so targets should reflect that
            for (Entrance* target : oneWayTargetEntrancePools[poolType]) {
                target->SetCondition([] { return logic->IsChild; });
            }

        } else if (poolType == EntranceType::Spawn) {
            validTargetTypes = { EntranceType::Spawn,       EntranceType::WarpSong, EntranceType::OwlDrop,
                                 EntranceType::Overworld,   EntranceType::Interior, EntranceType::SpecialInterior,
                                 EntranceType::GrottoGrave, EntranceType::Extra };
            oneWayTargetEntrancePools[poolType] = BuildOneWayTargets(validTargetTypes);

        } else if (poolType == EntranceType::WarpSong) {
            validTargetTypes = { EntranceType::Spawn,       EntranceType::WarpSong, EntranceType::OwlDrop,
                                 EntranceType::Overworld,   EntranceType::Interior, EntranceType::SpecialInterior,
                                 EntranceType::GrottoGrave, EntranceType::Extra };
            oneWayTargetEntrancePools[poolType] = BuildOneWayTargets(validTargetTypes);
        }
        // for target in one_way_target_entrance_pools[pool_type]:
        //           target.add_rule((lambda entrances=entrance_pool: (lambda state, **kwargs:
        //           any(entrance.connected_region == None for entrance in entrances)))())
    }

    // Disconnect all one way entrances at this point (they need to be connected during all of the above process)
    for (auto& pool : oneWayEntrancePools) {
        for (Entrance* entrance : pool.second) {
            mTotalRandomizableEntrances++;
            entrance->Disconnect();
        }
    }

    // Assume entrance pools for each type
    EntrancePools targetEntrancePools = {};
    for (auto& pool : entrancePools) {
        targetEntrancePools[pool.first] = AssumeEntrancePool(pool.second);
    }

    // distribution stuff

    // check placed one-way entrances
    // remove replaced entrances so we don't place two in one target
    // remove priority targets if any placed entrances point at their regions

    // Place priority entrances
    ShuffleOneWayPriorityEntrances(oneWayPriorities, oneWayEntrancePools, oneWayTargetEntrancePools);
    if (mEntranceShuffleFailure) {
        return ENTRANCE_SHUFFLE_FAILURE;
    }

    // Delete all targets that we just placed from one way target pools so
    // multiple one way entrances don't use the same target
    std::vector<Entrance*> replacedEntrances = {};
    for (auto& pool : oneWayEntrancePools) {
        for (Entrance* entrance : pool.second) {
            if (entrance->GetReplacement() != nullptr) {
                replacedEntrances.push_back(entrance);
            }
        }
    }
    for (auto& pool : oneWayTargetEntrancePools) {
        for (Entrance* remainingTarget : pool.second) {
            auto replacement = remainingTarget->GetReplacement();
            if (SohUtils::Contains(replacement, replacedEntrances)) {
                DeleteTargetEntrance(remainingTarget);
            }
        }
    }

    // Shuffle all one way entrances among pools to shuffle
    for (auto& pool : oneWayEntrancePools) {
        ShuffleEntrancePool(pool.second, oneWayTargetEntrancePools[pool.first], 5);
        if (mEntranceShuffleFailure) {
            return ENTRANCE_SHUFFLE_FAILURE;
        }
        // Delete all targets that we just placed from other one way target pools so
        // multiple one way entrances don't use the same target
        replacedEntrances =
            FilterFromPool(pool.second, [](Entrance* entrance) { return entrance->GetReplacement() != nullptr; });
        for (auto& targetPool : oneWayTargetEntrancePools) {
            for (Entrance* remainingTarget : targetPool.second) {
                auto replacement = remainingTarget->GetReplacement();
                if (SohUtils::Contains(replacement, replacedEntrances)) {
                    DeleteTargetEntrance(remainingTarget);
                }
            }
        }
        // Delete all unused extra targets after placing a one way pool, since the
        // unused targets won't ever be replaced
        for (Entrance* unusedTarget : oneWayTargetEntrancePools[pool.first]) {
            DeleteTargetEntrance(unusedTarget);
        }
    }

    // shuffle all entrances among pools to shuffle
    for (auto& pool : entrancePools) {
        ShuffleEntrancePool(pool.second, targetEntrancePools[pool.first]);
        if (mEntranceShuffleFailure) {
            return ENTRANCE_SHUFFLE_FAILURE;
        }
    }

    // Determine blue warp targets
    // RANDOTODO: add bluewarp shuffle
    if (true /* ctx->GetOption(RSK_SHUFFLE_BLUEWARP_ENTRANCES).Is(RO_BLUEWARP_ENTRANCE_SHUFFLE_DUNGEON) */) {
        // If a boss room is inside a boss door, make the blue warp go outside the dungeon's entrance
        std::map<std::string, Entrance*> bossExits = {
            { EntranceNameByRegions(RR_DEKU_TREE_BOSS_ROOM, RR_DEKU_TREE_BOSS_EXIT),
              GetEntrance(RR_DEKU_TREE_ENTRYWAY, RR_KF_OUTSIDE_DEKU_TREE) },
            { EntranceNameByRegions(RR_DODONGOS_CAVERN_BOSS_ROOM, RR_DODONGOS_CAVERN_BOSS_EXIT),
              GetEntrance(RR_DODONGOS_CAVERN_ENTRYWAY, RR_DEATH_MOUNTAIN_TRAIL) },
            { EntranceNameByRegions(RR_JABU_JABUS_BELLY_BOSS_ROOM, RR_JABU_JABUS_BELLY_BOSS_EXIT),
              GetEntrance(RR_JABU_JABUS_BELLY_ENTRYWAY, RR_ZORAS_FOUNTAIN) },
            { EntranceNameByRegions(RR_FOREST_TEMPLE_BOSS_ROOM, RR_FOREST_TEMPLE_BOSS_ENTRYWAY),
              GetEntrance(RR_FOREST_TEMPLE_ENTRYWAY, RR_SACRED_FOREST_MEADOW) },
            { EntranceNameByRegions(RR_FIRE_TEMPLE_BOSS_ROOM, RR_FIRE_TEMPLE_BOSS_ENTRYWAY),
              GetEntrance(RR_FIRE_TEMPLE_ENTRYWAY, RR_DMC_TEMPLE_EXIT) },
            { EntranceNameByRegions(RR_WATER_TEMPLE_BOSS_ROOM, RR_WATER_TEMPLE_BOSS_ENTRYWAY),
              GetEntrance(RR_WATER_TEMPLE_ENTRYWAY, RR_LH_FROM_WATER_TEMPLE) },
            { EntranceNameByRegions(RR_SPIRIT_TEMPLE_BOSS_ROOM, RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY),
              GetEntrance(RR_SPIRIT_TEMPLE_ENTRYWAY, RR_DESERT_COLOSSUS_OUTSIDE_TEMPLE) },
            { EntranceNameByRegions(RR_SHADOW_TEMPLE_BOSS_ROOM, RR_SHADOW_TEMPLE_BOSS_ENTRYWAY),
              GetEntrance(RR_SHADOW_TEMPLE_ENTRYWAY, RR_GRAVEYARD_WARP_PAD_REGION) },
            { EntranceNameByRegions(RR_GANONS_TOWER_STAIRS_1, RR_GANONS_TOWER_ENTRYWAY),
              GetEntrance(RR_GANONS_CASTLE_ENTRYWAY, RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE) }
        };

        // If a boss room is inside a dungeon entrance (or inside a dungeon which is inside a dungeon entrance), make
        // the blue warp go to that dungeon's blue warp target
        std::map<std::string, Entrance*> dungeonExits = {
            { EntranceNameByRegions(RR_DEKU_TREE_ENTRYWAY, RR_KF_OUTSIDE_DEKU_TREE),
              GetEntrance(RR_DEKU_TREE_BOSS_ROOM, RR_KF_OUTSIDE_DEKU_TREE) },
            { EntranceNameByRegions(RR_DODONGOS_CAVERN_ENTRYWAY, RR_DEATH_MOUNTAIN_TRAIL),
              GetEntrance(RR_DODONGOS_CAVERN_BOSS_ROOM, RR_DEATH_MOUNTAIN_TRAIL) },
            { EntranceNameByRegions(RR_JABU_JABUS_BELLY_ENTRYWAY, RR_ZORAS_FOUNTAIN),
              GetEntrance(RR_JABU_JABUS_BELLY_BOSS_ROOM, RR_ZORAS_FOUNTAIN) },
            { EntranceNameByRegions(RR_FOREST_TEMPLE_ENTRYWAY, RR_SACRED_FOREST_MEADOW),
              GetEntrance(RR_FOREST_TEMPLE_BOSS_ROOM, RR_SACRED_FOREST_MEADOW) },
            { EntranceNameByRegions(RR_FIRE_TEMPLE_ENTRYWAY, RR_DMC_TEMPLE_EXIT),
              GetEntrance(RR_FIRE_TEMPLE_BOSS_ROOM, RR_DMC_PAD_ENTRY) },
            { EntranceNameByRegions(RR_WATER_TEMPLE_ENTRYWAY, RR_LH_FROM_WATER_TEMPLE),
              GetEntrance(RR_WATER_TEMPLE_BOSS_ROOM, RR_LAKE_HYLIA) },
            { EntranceNameByRegions(RR_SPIRIT_TEMPLE_ENTRYWAY, RR_DESERT_COLOSSUS_OUTSIDE_TEMPLE),
              GetEntrance(RR_SPIRIT_TEMPLE_BOSS_ROOM, RR_DESERT_COLOSSUS) },
            { EntranceNameByRegions(RR_SHADOW_TEMPLE_ENTRYWAY, RR_GRAVEYARD_WARP_PAD_REGION),
              GetEntrance(RR_SHADOW_TEMPLE_BOSS_ROOM, RR_GRAVEYARD_WARP_PAD_REGION) },
            { EntranceNameByRegions(RR_GANONS_CASTLE_ENTRYWAY, RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE),
              GetEntrance(RR_GANONS_TOWER_STAIRS_1, RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE) }
        };

        // Pair <BlueWarp exit, BossRoom reverse exit>
        std::vector<EntrancePair> bossRoomExitPairs = {
            { GetEntrance(RR_DEKU_TREE_BOSS_ROOM, RR_KF_OUTSIDE_DEKU_TREE),
              GetEntrance(RR_DEKU_TREE_BOSS_ROOM, RR_DEKU_TREE_BOSS_EXIT) },
            { GetEntrance(RR_DODONGOS_CAVERN_BOSS_ROOM, RR_DEATH_MOUNTAIN_TRAIL),
              GetEntrance(RR_DODONGOS_CAVERN_BOSS_ROOM, RR_DODONGOS_CAVERN_BOSS_EXIT) },
            { GetEntrance(RR_JABU_JABUS_BELLY_BOSS_ROOM, RR_ZORAS_FOUNTAIN),
              GetEntrance(RR_JABU_JABUS_BELLY_BOSS_ROOM, RR_JABU_JABUS_BELLY_BOSS_EXIT) },
            { GetEntrance(RR_FOREST_TEMPLE_BOSS_ROOM, RR_SACRED_FOREST_MEADOW),
              GetEntrance(RR_FOREST_TEMPLE_BOSS_ROOM, RR_FOREST_TEMPLE_BOSS_ENTRYWAY) },
            { GetEntrance(RR_FIRE_TEMPLE_BOSS_ROOM, RR_DMC_PAD_ENTRY),
              GetEntrance(RR_FIRE_TEMPLE_BOSS_ROOM, RR_FIRE_TEMPLE_BOSS_ENTRYWAY) },
            { GetEntrance(RR_WATER_TEMPLE_BOSS_ROOM, RR_LAKE_HYLIA),
              GetEntrance(RR_WATER_TEMPLE_BOSS_ROOM, RR_WATER_TEMPLE_BOSS_ENTRYWAY) },
            { GetEntrance(RR_SPIRIT_TEMPLE_BOSS_ROOM, RR_DESERT_COLOSSUS),
              GetEntrance(RR_SPIRIT_TEMPLE_BOSS_ROOM, RR_SPIRIT_TEMPLE_BOSS_ENTRYWAY) },
            { GetEntrance(RR_SHADOW_TEMPLE_BOSS_ROOM, RR_GRAVEYARD_WARP_PAD_REGION),
              GetEntrance(RR_SHADOW_TEMPLE_BOSS_ROOM, RR_SHADOW_TEMPLE_BOSS_ENTRYWAY) },
            { GetEntrance(RR_GANONS_TOWER_STAIRS_1, RR_CASTLE_GROUNDS_FROM_GANONS_CASTLE),
              GetEntrance(RR_GANONS_TOWER_STAIRS_1, RR_GANONS_TOWER_ENTRYWAY) }
        };

        for (EntrancePair pair : bossRoomExitPairs) {
            Entrance* target = pair.second->GetReplacement() != nullptr ? pair.second->GetReplacement() : pair.second;

            if (!ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                while (bossExits.find(target->GetName()) != bossExits.end()) {
                    Entrance* next = bossExits.at(target->GetName());
                    target = next->GetReplacement() != nullptr ? next->GetReplacement() : next;
                }

                if (dungeonExits.find(target->GetName()) != dungeonExits.end()) {
                    target = dungeonExits.at(target->GetName());
                }
            }

            pair.first->Connect(target->GetOriginalConnectedRegionKey());
            pair.first->SetReplacement(target);
        }
    }

    erase_if(RegionTable(RR_ROOT)->exits, [](Entrance exit) { return exit.GetReplacement() != NULL; });

    // Validate the world one last time to ensure all special conditions are still valid
    if (!ValidateWorld(nullptr)) {
        return ENTRANCE_SHUFFLE_FAILURE;
    }

    return ENTRANCE_SHUFFLE_SUCCESS;
}

void EntranceShuffler::CreateEntranceOverrides() {
    auto ctx = Rando::Context::GetInstance();
    entranceOverrides.fill({ 0, 0, 0, 0, 0 });
    if (mNoRandomEntrances) {
        return;
    }
    SPDLOG_DEBUG("CREATING ENTRANCE OVERRIDES");
    auto allShuffleableEntrances = GetShuffleableEntrances(EntranceType::All, false);

    int i = 0;
    for (Entrance* entrance : allShuffleableEntrances) {

        // Include blue warps when dungeons or bosses are shuffled
        bool includeBluewarps =
            entrance->GetType() == Rando::EntranceType::BlueWarp &&
            (ctx->GetOption(RSK_SHUFFLE_GANONS_TOWER_ENTRANCE) ||
             entrance->GetParentRegionKey() != RR_GANONS_TOWER_STAIRS_1) &&
            (ctx->GetOption(RSK_SHUFFLE_DUNGEON_ENTRANCES) || ctx->GetOption(RSK_SHUFFLE_BOSS_ENTRANCES));

        // Double-check to make sure the entrance is actually shuffled
        if (!entrance->IsShuffled() && !includeBluewarps) {
            continue;
        }

        SPDLOG_DEBUG("Setting {}", entrance->to_string());

        uint16_t type = (uint16_t)entrance->GetType();
        int16_t originalIndex = entrance->GetIndex();
        int16_t replacementIndex = entrance->GetReplacement()->GetIndex();

        int16_t destinationIndex = -1;
        int16_t replacementDestinationIndex = -1;

        // Track the reverse destination, useful for savewarp handling
        if (entrance->GetReverse() != nullptr) {
            destinationIndex = entrance->GetReverse()->GetIndex();
            if (!ctx->GetOption(RSK_DECOUPLED_ENTRANCES)) {
                replacementDestinationIndex = entrance->GetReplacement()->GetReverse()->GetIndex();
            }
        }

        entranceOverrides[i] = {
            .type = type,
            .index = originalIndex,
            .destination = destinationIndex,
            .override = replacementIndex,
            .overrideDestination = replacementDestinationIndex,
        };

        SPDLOG_DEBUG("\tOriginal {}", originalIndex);
        SPDLOG_DEBUG("\tReplacement {}", replacementIndex);
        i++;
    }
}

// scene & transition actor index identify a door, frontRoom picks which way we're walking through it
const Door* EntranceShuffler::MapDoor(s16 scene, s8 transitionIdx, s8 frontRoom) {
    if (mDoorTable.size() == 0) {
        mDoorTable.reserve(DoorsList.size());
        for (size_t i = 0; i < DoorsList.size(); i++) {
            mDoorTable.push_back((int16_t)i ^ 1);
            // both entries of a door share a key, the even one wins
            mDoorLookup.emplace(DOOR_KEY(DoorsList[i].scene, DoorsList[i].transitionIdx), (int16_t)(i & ~1));
        }
        for (int i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
            if (entranceOverrides[i].type >= (uint16_t)EntranceType::DoorDekuTree &&
                entranceOverrides[i].type <= (uint16_t)EntranceType::DoorGanonsTower) {
                mDoorTable[entranceOverrides[i].index - ENTRANCE_DOOR_START] =
                    (entranceOverrides[i].override ^ 1) - ENTRANCE_DOOR_START;
            }
        }
    }

    auto entry = mDoorLookup.find(DOOR_KEY(scene, transitionIdx));
    if (entry == mDoorLookup.end()) {
        return NULL;
    }

    int16_t doorIndex = entry->second;
    if (DoorsList[doorIndex].dstRoom != frontRoom) {
        doorIndex ^= 1;
        if (DoorsList[doorIndex].dstRoom != frontRoom) {
            return NULL;
        }
    }
    return &DoorsList[mDoorTable[doorIndex]];
}

/// @brief set all the entrances to be 0 to indicate an unshuffled entrance
void EntranceShuffler::UnshuffleAllEntrances() {
    for (auto& entranceOveride : entranceOverrides) {
        entranceOveride.type = 0;
        entranceOveride.index = 0;
        entranceOveride.destination = 0;
        entranceOveride.override = 0;
        entranceOveride.overrideDestination = 0;
    }
}

void EntranceShuffler::ParseJson(const nlohmann::json& spoilerFileJson) {
    UnshuffleAllEntrances();
    try {
        nlohmann::json entrancesJson = spoilerFileJson.value("entrances", nlohmann::json());
        size_t i = 0;
        for (auto it = entrancesJson.begin(); it != entrancesJson.end() && i < entranceOverrides.size(); ++it, i++) {
            nlohmann::json entranceJson = *it;
            for (auto entranceIt = entranceJson.begin(); entranceIt != entranceJson.end(); ++entranceIt) {
                if (entranceIt.key() == "type") {
                    entranceOverrides[i].type = entranceIt.value();
                } else if (entranceIt.key() == "index") {
                    entranceOverrides[i].index = entranceIt.value();
                } else if (entranceIt.key() == "destination") {
                    entranceOverrides[i].destination = entranceIt.value();
                } else if (entranceIt.key() == "override") {
                    entranceOverrides[i].override = entranceIt.value();
                } else if (entranceIt.key() == "overrideDestination") {
                    entranceOverrides[i].overrideDestination = entranceIt.value();
                }
            }
        }
    } catch (const std::exception& e) { throw e; }
    // We may need to reset more things here or elsewhere in spoiler loading
    RegionTable_Init();
    ApplyEntranceOverrides();
    SetAreas();
}

void EntranceShuffler::ApplyEntranceOverrides() {
    SetAllEntrancesData();

    for (size_t i = 0; i < entranceOverrides.size(); i++) {
        EntranceOverride entranceOverride = entranceOverrides[i];

        if (entranceOverride.index == 0 && entranceOverride.destination == 0 && entranceOverride.override == 0 &&
            entranceOverride.overrideDestination == 0) {
            continue;
        }

        Entrance* entrance = entranceMap[entranceOverride.index];
        Entrance* overrideEntrance = entranceMap[entranceOverride.override];

        entrance->Disconnect();
        entrance->Connect(overrideEntrance->GetOriginalConnectedRegionKey());
        entrance->SetAsShuffled();
    }
}

const Entrance* EntranceShuffler::GetEntranceByIndex(int16_t index) {
    auto iter = entranceMap.find(index);
    return iter != entranceMap.end() ? iter->second : nullptr;
}
} // namespace Rando

extern "C" EntranceOverride* Randomizer_GetEntranceOverrides() {
    return Rando::Context::GetInstance()->GetEntranceShuffler()->entranceOverrides.data();
}

static SceneID backedUpScene = (SceneID)0xFF;
static Camera backupCamera;

void RegisterEntranceShuffleHooks() {
    COND_VB_SHOULD(VB_SHOULD_LOAD_BG_IMAGE, IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES), {
        int32_t* camId = va_arg(args, int*);
        Camera* camera = GET_ACTIVE_CAM(gPlayState);
        if (*camId == -1) {
            if (backedUpScene != gPlayState->sceneNum) {
                *should = false;
                return;
            }
            memcpy(camera, &backupCamera, sizeof(Camera));
            Camera_RequestMode(camera, CAM_MODE_TALK);
            *should = false;
        } else if (backedUpScene != gPlayState->sceneNum) {
            memcpy(&backupCamera, camera, sizeof(Camera));
            backedUpScene = (SceneID)gPlayState->sceneNum;
        }
    });

    COND_HOOK(OnLoadGame, IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES),
              [](int32_t) { backedUpScene = (SceneID)0xFF; });
}

static RegisterShipInitFunc initFunc(RegisterEntranceShuffleHooks, { "IS_RANDO" });
