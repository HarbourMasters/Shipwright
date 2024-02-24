#include <unordered_map>
#include "silver_rupee.h"
#include "static_data.h"
#include "randomizer.h"

namespace Rando {

    std::unordered_map<Identifier, RandomizerGet> StaticData::silverTrackerMap = {
        { { SCENE_ICE_CAVERN, RCQUEST_VANILLA, 328 }, RG_ICE_CAVERN_SPINNING_BLADES_SILVER_RUPEE },
        { { SCENE_ICE_CAVERN, RCQUEST_VANILLA, 329 }, RG_ICE_CAVERN_SLIDING_SILVER_RUPEE }, 
        { { SCENE_BOTTOM_OF_THE_WELL, RCQUEST_VANILLA, 351 }, RG_BOTTOM_OF_THE_WELL_SILVER_RUPEE },
        { { SCENE_GERUDO_TRAINING_GROUND, RCQUEST_VANILLA, 348 }, RG_GERUDO_TRAINING_GROUNDS_BOULDER_SILVER_RUPEE },
        { { SCENE_GERUDO_TRAINING_GROUND, RCQUEST_VANILLA, 332 }, RG_GERUDO_TRAINING_GROUNDS_LAVA_SILVER_RUPEE },
        { { SCENE_GERUDO_TRAINING_GROUND, RCQUEST_VANILLA, 347 }, RG_GERUDO_TRAINING_GROUNDS_TOILET_SILVER_RUPEE },
        { { SCENE_SPIRIT_TEMPLE, RCQUEST_VANILLA, 325 }, RG_SPIRIT_CHILD_GATE_SILVER_RUPEE },
        { { SCENE_SPIRIT_TEMPLE, RCQUEST_VANILLA, 330 }, RG_SPIRIT_CHILD_BLOCK_PUSH_SILVER_RUPEE },
        { { SCENE_SPIRIT_TEMPLE, RCQUEST_VANILLA, 322 }, RG_SPIRIT_BOULDER_SILVER_RUPEE },
        { { SCENE_SHADOW_TEMPLE, RCQUEST_VANILLA, 321 }, RG_SHADOW_SCYTHE_SILVER_RUPEE },
        { { SCENE_SHADOW_TEMPLE, RCQUEST_VANILLA, 329 }, RG_SHADOW_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE },
        { { SCENE_SHADOW_TEMPLE, RCQUEST_VANILLA, 328 }, RG_SHADOW_INVISIBLE_SPIKES_SILVER_RUPEE },
        { { SCENE_INSIDE_GANONS_CASTLE, RCQUEST_VANILLA, 334 }, RG_FOREST_TRIAL_SILVER_RUPEE },
        { { SCENE_INSIDE_GANONS_CASTLE, RCQUEST_VANILLA, 329 }, RG_FIRE_TRIAL_SILVER_RUPEE },
        { { SCENE_INSIDE_GANONS_CASTLE, RCQUEST_VANILLA, 331 }, RG_SPIRIT_TRIAL_SILVER_RUPEE },
        { { SCENE_INSIDE_GANONS_CASTLE, RCQUEST_VANILLA, 338 }, RG_LIGHT_TRIAL_SILVER_RUPEE },
    };

    std::unordered_map<Position, RandomizerCheck> StaticData::silverRupeeMap = {
            { { SCENE_ICE_CAVERN, {414.0f, 178.0f, -579.0f} }, RC_ICE_CAVERN_SPINNING_BLADES_SILVER_RUPEE_1 },
            { { SCENE_ICE_CAVERN, {389.0f, 0.0f, -382.0f } }, RC_ICE_CAVERN_SPINNING_BLADES_SILVER_RUPEE_2 },
            { { SCENE_ICE_CAVERN, {278.0f, 0.0f, -637.0f } }, RC_ICE_CAVERN_SPINNING_BLADES_SILVER_RUPEE_3 },
            { { SCENE_ICE_CAVERN, { 198.0f, 0.0f, -388.0f } }, RC_ICE_CAVERN_SPINNING_BLADES_SILVER_RUPEE_4 },
            { { SCENE_ICE_CAVERN, { 1.0f, 0.0f, -143.0f } }, RC_ICE_CAVERN_SPINNING_BLADES_SILVER_RUPEE_5 },
            { { SCENE_ICE_CAVERN, { -1040.0f, 112.0f, -485.0f } }, RC_ICE_CAVERN_SLIDING_SILVER_RUPEE_1 },
            { { SCENE_ICE_CAVERN, { -1120.0f, 119.0f, -1577.0f } }, RC_ICE_CAVERN_SLIDING_SILVER_RUPEE_2 },
            { { SCENE_ICE_CAVERN, { -1294.0f, 113.0f, -899.0f } }, RC_ICE_CAVERN_SLIDING_SILVER_RUPEE_3 },
            { { SCENE_ICE_CAVERN, { -1558.0f, 41.0f, -951.0f } }, RC_ICE_CAVERN_SLIDING_SILVER_RUPEE_4 },
            { { SCENE_ICE_CAVERN, { -1676.0f, 112.0f, -552.0f } }, RC_ICE_CAVERN_SLIDING_SILVER_RUPEE_5 },
            { { SCENE_BOTTOM_OF_THE_WELL, { -259.0f, -708.0f, -234.0f } }, RC_BOTTOM_OF_THE_WELL_SILVER_RUPEE_1 },
            { { SCENE_BOTTOM_OF_THE_WELL, { -402.0f, -701.0f, -401.0f } }, RC_BOTTOM_OF_THE_WELL_SILVER_RUPEE_2 },
            { { SCENE_BOTTOM_OF_THE_WELL, { -560.0f, -720.0f, -291.0f } }, RC_BOTTOM_OF_THE_WELL_SILVER_RUPEE_3 },
            { { SCENE_BOTTOM_OF_THE_WELL, { -614.0f, -465.0f, -297.0f } }, RC_BOTTOM_OF_THE_WELL_SILVER_RUPEE_4 },
            { { SCENE_BOTTOM_OF_THE_WELL, { -796.0f, 0.0f, -150.0f } }, RC_BOTTOM_OF_THE_WELL_SILVER_RUPEE_5 },
            { { SCENE_GERUDO_TRAINING_GROUND, { -1887.0f, 160.0f, -2134.0f } }, RC_GERUDO_TRAINING_GROUNDS_BOULDER_SILVER_RUPEE_1 },
            { { SCENE_GERUDO_TRAINING_GROUND, { -1627.0f, 182.0f, -1462.0f } }, RC_GERUDO_TRAINING_GROUNDS_BOULDER_SILVER_RUPEE_2 },
            { { SCENE_GERUDO_TRAINING_GROUND, { -1579.0f, 236.0f, -999.0f } }, RC_GERUDO_TRAINING_GROUNDS_BOULDER_SILVER_RUPEE_3 },
            { { SCENE_GERUDO_TRAINING_GROUND, { -1886.0f, -80.0f, -956.0f } }, RC_GERUDO_TRAINING_GROUNDS_BOULDER_SILVER_RUPEE_4 },
            { { SCENE_GERUDO_TRAINING_GROUND, { -1332.0f, -80.0f, -992.0f } }, RC_GERUDO_TRAINING_GROUNDS_BOULDER_SILVER_RUPEE_5 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 1560.0f, -239.0f, -1861.0f } }, RC_GERUDO_TRAINING_GROUNDS_LAVA_SILVER_RUPEE_1 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 1437.0f, 30.0f, -2193.0f } }, RC_GERUDO_TRAINING_GROUNDS_LAVA_SILVER_RUPEE_2 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 1134.0f, -239.0f, -1841.0f } }, RC_GERUDO_TRAINING_GROUNDS_LAVA_SILVER_RUPEE_3 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 1558.0f, -239.0f, -1370.0f } }, RC_GERUDO_TRAINING_GROUNDS_LAVA_SILVER_RUPEE_4 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 1320.0f, -239.0f, -1248.0f } }, RC_GERUDO_TRAINING_GROUNDS_LAVA_SILVER_RUPEE_5 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 2160.0f, -900.0f, -1315.0f } }, RC_GERUDO_TRAINING_GROUNDS_TOILET_SILVER_RUPEE_1 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 2078.0f, -806.0f, -1458.0f } }, RC_GERUDO_TRAINING_GROUNDS_TOILET_SILVER_RUPEE_2 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 2453.0f, -890.0f, -1612.0f } }, RC_GERUDO_TRAINING_GROUNDS_TOILET_SILVER_RUPEE_3 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 2497.0f, -605.0f, -1465.0f } }, RC_GERUDO_TRAINING_GROUNDS_TOILET_SILVER_RUPEE_4 },
            { { SCENE_GERUDO_TRAINING_GROUND, { 2308.0f, -961.0f, -1464.0f } }, RC_GERUDO_TRAINING_GROUNDS_TOILET_SILVER_RUPEE_5 },
            { { SCENE_SPIRIT_TEMPLE, { -335.0f, 30.0f, -1409.0f } }, RC_SPIRIT_CHILD_GATE_SILVER_RUPEE_1 },
            { { SCENE_SPIRIT_TEMPLE, { -347.0f, 229.0f, -1075.0f } }, RC_SPIRIT_CHILD_GATE_SILVER_RUPEE_2 },
            { { SCENE_SPIRIT_TEMPLE, { -511.0f, 50.0f, -1075.0f } }, RC_SPIRIT_CHILD_GATE_SILVER_RUPEE_3 },
            { { SCENE_SPIRIT_TEMPLE, { -672.0f, 50.0f, -1075.0f } }, RC_SPIRIT_CHILD_GATE_SILVER_RUPEE_4 },
            { { SCENE_SPIRIT_TEMPLE, { -766.0f, 229.0f, -1075.0f } }, RC_SPIRIT_CHILD_GATE_SILVER_RUPEE_5 },
            { { SCENE_SPIRIT_TEMPLE, { -984.0f, 923.0f, -450.0f } }, RC_SPIRIT_CHILD_BLOCK_PUSH_SILVER_RUPEE_1 },
            { { SCENE_SPIRIT_TEMPLE, { -1123.0f, 923.0f, 428.0f } }, RC_SPIRIT_CHILD_BLOCK_PUSH_SILVER_RUPEE_2 },
            { { SCENE_SPIRIT_TEMPLE, { -1275.0f, 922.0f, -247.0f } }, RC_SPIRIT_CHILD_BLOCK_PUSH_SILVER_RUPEE_3 },
            { { SCENE_SPIRIT_TEMPLE, { -1433.0f, 913.0f, -283.0f } }, RC_SPIRIT_CHILD_BLOCK_PUSH_SILVER_RUPEE_4 },
            { { SCENE_SPIRIT_TEMPLE, { -1836.0f, 923.0f, -446.0f } }, RC_SPIRIT_CHILD_BLOCK_PUSH_SILVER_RUPEE_5 },
            { { SCENE_SPIRIT_TEMPLE, { 1573.0f, -33.0f, -920.0f } }, RC_SPIRIT_BOULDER_SILVER_RUPEE_1 },
            { { SCENE_SPIRIT_TEMPLE, { 1856.0f, -33.0f, -1219.0f } }, RC_SPIRIT_BOULDER_SILVER_RUPEE_2 },
            { { SCENE_SPIRIT_TEMPLE, { 1856.0f, -33.0f, -944.0f } }, RC_SPIRIT_BOULDER_SILVER_RUPEE_3 },
            { { SCENE_SPIRIT_TEMPLE, { 1284.0f, -33.0f, -813.0f } }, RC_SPIRIT_BOULDER_SILVER_RUPEE_4 },
            { { SCENE_SPIRIT_TEMPLE, { 1284.0f, -33.0f, -1355.0f } }, RC_SPIRIT_BOULDER_SILVER_RUPEE_5 },
            { { SCENE_SHADOW_TEMPLE, { 3399.0f, -543.0f, -838.0f } }, RC_SHADOW_SCYTHE_SILVER_RUPEE_1 },
            { { SCENE_SHADOW_TEMPLE, { 2869.0f, -543.0f, -948.0f } }, RC_SHADOW_SCYTHE_SILVER_RUPEE_2 },
            { { SCENE_SHADOW_TEMPLE, { 3554.0f, -543.0f, -1432.0f } }, RC_SHADOW_SCYTHE_SILVER_RUPEE_3 },
            { { SCENE_SHADOW_TEMPLE, { 3007.0f, -423.0f, -1222.0f } }, RC_SHADOW_SCYTHE_SILVER_RUPEE_4 },
            { { SCENE_SHADOW_TEMPLE, { 3243.0f, -543.0f, -1061.0f } }, RC_SHADOW_SCYTHE_SILVER_RUPEE_5 },
            { { SCENE_SHADOW_TEMPLE, { 1998.0f, -1343.0f, 3358.0f } }, RC_SHADOW_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE_1 },
            { { SCENE_SHADOW_TEMPLE, { 2243.0f, -1343.0f, 3361.0f } }, RC_SHADOW_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE_2 },
            { { SCENE_SHADOW_TEMPLE, { 2110.0f, -1343.0f, 3368.0f } }, RC_SHADOW_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE_3 },
            { { SCENE_SHADOW_TEMPLE, { 2115.0f, -1343.0f, 3738.0f } }, RC_SHADOW_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE_4 },
            { { SCENE_SHADOW_TEMPLE, { 2131.0f, -1343.0f, 3030.0f } }, RC_SHADOW_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE_5 },
            { { SCENE_SHADOW_TEMPLE, { 2254.0f, -1197.0f, 988.0f } }, RC_SHADOW_INVISIBLE_SPIKES_SILVER_RUPEE_1 },
            { { SCENE_SHADOW_TEMPLE, { 2135.0f, -1225.0f, 1297.0f } }, RC_SHADOW_INVISIBLE_SPIKES_SILVER_RUPEE_2 },
            { { SCENE_SHADOW_TEMPLE, { 2478.0f, -1343.0f, 1208.0f } }, RC_SHADOW_INVISIBLE_SPIKES_SILVER_RUPEE_3 },
            { { SCENE_SHADOW_TEMPLE, { 2940.0f, -1268.0f, 1069.0f } }, RC_SHADOW_INVISIBLE_SPIKES_SILVER_RUPEE_4 },
            { { SCENE_SHADOW_TEMPLE, { 2042.0f, -1203.0f, 849.0f } }, RC_SHADOW_INVISIBLE_SPIKES_SILVER_RUPEE_5 },
            { { SCENE_INSIDE_GANONS_CASTLE, { 1634.0f, 164.0f, 1550.0f } }, RC_FOREST_TRIAL_SILVER_RUPEE_1 },
            { { SCENE_INSIDE_GANONS_CASTLE, { 1651.0f, 30.0f, 2021.0f } }, RC_FOREST_TRIAL_SILVER_RUPEE_2 },
            { { SCENE_INSIDE_GANONS_CASTLE, { 1538.0f, 118.0f, 2225.0f } }, RC_FOREST_TRIAL_SILVER_RUPEE_3 },
            { { SCENE_INSIDE_GANONS_CASTLE, { 1361.0f, 115.0f, 1222.0f } }, RC_FOREST_TRIAL_SILVER_RUPEE_4 },
            { { SCENE_INSIDE_GANONS_CASTLE, { 1247.0f, 59.0f, 1787.0f } }, RC_FOREST_TRIAL_SILVER_RUPEE_5 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -1867.0f, 227.0f, -2754.0f } }, RC_FIRE_TRIAL_SILVER_RUPEE_1 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -604.0f, 133.0f, -2592.0f } }, RC_FIRE_TRIAL_SILVER_RUPEE_2 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -515.0f, 133.0f, -3253.0f } }, RC_FIRE_TRIAL_SILVER_RUPEE_3 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -1555.0f, 151.0f, -2317.0f } }, RC_FIRE_TRIAL_SILVER_RUPEE_4 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -854.0f, 152.0f, -3679.0f } }, RC_FIRE_TRIAL_SILVER_RUPEE_5 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -1142.0f, 165.0f, 639.0f } }, RC_SPIRIT_TRIAL_SILVER_RUPEE_1 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -725.0f, 165.0f, 942.0f } }, RC_SPIRIT_TRIAL_SILVER_RUPEE_2 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -865.0f, 165.0f, 656.0f } }, RC_SPIRIT_TRIAL_SILVER_RUPEE_3 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -940.0f, 165.0f, 270.0f } }, RC_SPIRIT_TRIAL_SILVER_RUPEE_4 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -829.0f, 274.0f, 591.0f } }, RC_SPIRIT_TRIAL_SILVER_RUPEE_5 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -2674.0f, -240.0f, -777.0f } }, RC_LIGHT_TRIAL_SILVER_RUPEE_1 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -2680.0f, -240.0f, -893.0f } }, RC_LIGHT_TRIAL_SILVER_RUPEE_2 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -2646.0f, -120.0f, -839.0f } }, RC_LIGHT_TRIAL_SILVER_RUPEE_3 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -2649.0f, -240.0f, -545.0f } }, RC_LIGHT_TRIAL_SILVER_RUPEE_4 },
            { { SCENE_INSIDE_GANONS_CASTLE, { -2509.0f, -240.0f, -1091.0f } }, RC_LIGHT_TRIAL_SILVER_RUPEE_5 },
    };
    std::unordered_map<RandomizerGet, Text> SilverRupees::mSilverRupeeRoomNames = {
            // TODO: Translate
            //                                              "English"                            "German"                             "French"
            { RG_ICE_CAVERN_SPINNING_BLADES_SILVER_RUPEE, { "Ice Cavern's Spinning Blades Room", "Ice Cavern's Spinning Blades Room", "Ice Cavern's Spinning Blades Room" } },
            { RG_ICE_CAVERN_SLIDING_SILVER_RUPEE, { "Ice Cavern's Block Sliding Puzzle Room", "Ice Cavern's Block Sliding Puzzle Room", "Ice Cavern's Block Sliding Puzzle Room" } },
            { RG_BOTTOM_OF_THE_WELL_SILVER_RUPEE, { "Bottom of the Well", "Bottom of the Well", "Bottom of the Well" } },
            { RG_GERUDO_TRAINING_GROUNDS_BOULDER_SILVER_RUPEE, { "Boulder Maze in the Gerudo Training Grounds", "Boulder Maze in the Gerudo Training Grounds", "Boulder Maze in the Gerudo Training Grounds" } },
            { RG_GERUDO_TRAINING_GROUNDS_LAVA_SILVER_RUPEE, { "Lava room in the Gerudo Training Grounds", "Lava room in the Gerudo Training Grounds", "Lava room in the Gerudo Training Grounds" } },
            { RG_GERUDO_TRAINING_GROUNDS_TOILET_SILVER_RUPEE, { "Whirlpool room in the Gerudo Training Grounds", "Whirlpool room in the Gerudo Training Grounds", "Whirlpool room in the Gerudo Training Grounds" } },
            { RG_SPIRIT_CHILD_GATE_SILVER_RUPEE, { "Child-side Gate room in the Spirit Temple", "Child-side Gate room in the Spirit Temple", "Child-side Gate room in the Spirit Temple" } },
            { RG_SPIRIT_CHILD_BLOCK_PUSH_SILVER_RUPEE, { "Child-side Block Pushing room in the Spirit Temple", "Child-side Block Pushing room in the Spirit Temple", "Child-side Block Pushing room in the Spirit Temple" } },
            { RG_SPIRIT_BOULDER_SILVER_RUPEE, { "Boulder room in the Spirit Temple", "Boulder room in the Spirit Temple", "Boulder room in the Spirit Temple" } },
            { RG_SHADOW_SCYTHE_SILVER_RUPEE, { "Spinning Scythes room in the Shadow Temple", "Spinning Scythes room in the Shadow Temple", "Spinning Scythes room in the Shadow Temple" } },
            { RG_SHADOW_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE, { "door to the Falling Spikes room in the Shadow Temple", "door to the Falling Spikes room in the Shadow Temple", "door to the Falling Spikes room in the Shadow Temple" } },
            { RG_SHADOW_INVISIBLE_SPIKES_SILVER_RUPEE, { "room with Invisible Spikes in the Shadow Temple", "room with Invisible Spikes in the Shadow Temple", "room with Invisible Spikes in the Shadow Temple" } },
            { RG_FOREST_TRIAL_SILVER_RUPEE, { "Forest Trial in Ganon's Castle", "Forest Trial in Ganon's Castle", "Forest Trial in Ganon's Castle" } },
            { RG_FIRE_TRIAL_SILVER_RUPEE, { "Fire Trial in Ganon's Castle", "Fire Trial in Ganon's Castle", "Fire Trial in Ganon's Castle" } },
            { RG_SPIRIT_TRIAL_SILVER_RUPEE, { "Spirit Trial in Ganon's Castle", "Spirit Trial in Ganon's Castle", "Spirit Trial in Ganon's Castle" } },
            { RG_LIGHT_TRIAL_SILVER_RUPEE, { "Light Trial in Ganon's Castle", "Light Trial in Ganon's Castle", "Light Trial in Ganon's Castle" } },
            { RG_DODONGOS_CAVERN_MQ_SILVER_RUPEE, { "Dodongo's Cavern Staircase Room", "Dodongo's Cavern Staircase Room", "Dodongo's Cavern Staircase Room" } },
            { RG_SPIRIT_MQ_LOBBY_SILVER_RUPEE, { "Spirit Temple Lobby", "Spirit Temple Lobby", "Spirit Temple Lobby" } },
            { RG_SHADOW_MQ_INVISIBLE_SCYTHE_SILVER_RUPEE, { "Invisible Scythes room in the Shadow Temple", "Invisible Scythes room in the Shadow Temple", "Invisible Scythes room in the Shadow Temple" } },
            { RG_WATER_TRIAL_MQ_SILVER_RUPEE, { "Water Trial in Ganon's Castle", "Water Trial in Ganon's Castle", "Water Trial in Ganon's Castle" } },
            { RG_SHADOW_TRIAL_MQ_SILVER_RUPEE, { "Shadow Trial in Ganon's Castle", "Shadow Trial in Ganon's Castle", "Shadow Trial in Ganon's Castle" } }
    };

    SilverRupees::SilverRupees() {
        mSilverRupees = {
                // Vanilla
                SilverRupeesInfo(5, RG_ICE_CAVERN_SPINNING_BLADES_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_ICE_CAVERN_SLIDING_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_BOTTOM_OF_THE_WELL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_GERUDO_TRAINING_GROUNDS_BOULDER_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_GERUDO_TRAINING_GROUNDS_LAVA_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_GERUDO_TRAINING_GROUNDS_TOILET_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SPIRIT_CHILD_BLOCK_PUSH_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SPIRIT_BOULDER_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_SCYTHE_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_INVISIBLE_SPIKES_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_FOREST_TRIAL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_FIRE_TRIAL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SPIRIT_TRIAL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_LIGHT_TRIAL_SILVER_RUPEE),
                // MQ
                SilverRupeesInfo(5, RG_GERUDO_TRAINING_GROUNDS_MQ_BOULDER_SILVER_RUPEE),
                SilverRupeesInfo(6, RG_GERUDO_TRAINING_GROUNDS_MQ_LAVA_SILVER_RUPEE),
                SilverRupeesInfo(3, RG_GERUDO_TRAINING_GROUNDS_MQ_TOILET_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_DODONGOS_CAVERN_MQ_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SPIRIT_MQ_LOBBY_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_MQ_SCYTHE_SILVER_RUPEE),
                SilverRupeesInfo(10, RG_SHADOW_MQ_INVISIBLE_SCYTHE_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_MQ_OUTSIDE_SPIKE_RAIN_SILVER_RUPEE),
                SilverRupeesInfo(10, RG_SHADOW_MQ_INVISIBLE_SPIKES_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_FIRE_TRIAL_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_WATER_TRIAL_MQ_SILVER_RUPEE),
                SilverRupeesInfo(5, RG_SHADOW_TRIAL_MQ_SILVER_RUPEE),
        };
    }

    CustomMessage SilverRupees::GetSilverRupeeMessage(u16 rgid) {
        CustomMessage messageEntry;
        if (rgid >= RG_SILVER_RUPEE_FIRST && rgid <= RG_SILVER_RUPEE_LAST) {
            messageEntry =
                    CustomMessageManager::Instance->RetrieveMessage(Randomizer::getItemMessageTableID, RG_SILVER_RUPEE_FIRST);
        }/* else if (rgid >= RG_SILVER_RUPEE_POUCH_FIRST && rgid <= RG_SILVER_RUPEE_POUCH_LAST) {
            messageEntry =
                    CustomMessageManager::Instance->RetrieveMessage(Randomizer::getItemMessageTableID, RG_SILVER_RUPEE_POUCH_FIRST);
        }*/
        u16 roomNameKey =
                /*(rgid >= RG_SILVER_RUPEE_POUCH_FIRST) ? (rgid - RG_SILVER_RUPEE_POUCH_FIRST) + RG_SILVER_RUPEE_FIRST :*/ rgid;
        Text locationNames = mSilverRupeeRoomNames[(RandomizerGet)roomNameKey];
        messageEntry.Replace("{{location}}", locationNames.english.c_str(), locationNames.german.c_str(), locationNames.french.c_str());
        return messageEntry;
    }

    size_t SilverRupees::GetInfoListSize() {
        return mSilverRupees.size();
    }

    void SilverRupees::ResetAll() {
        for (auto& silverRupee : mSilverRupees) {
            silverRupee.Reset();
        }
    }

    SilverRupeesInfo &SilverRupees::GetInfo(RandomizerGet rgid) {
        return mSilverRupees[rgid - RG_SILVER_RUPEE_FIRST];
    }

    SilverRupeesInfo::SilverRupeesInfo(uint8_t total, RandomizerGet randoGet) : mCollected(0), mTotal(total), mRandoGet(randoGet) {

    }

    uint8_t SilverRupeesInfo::GetCollected() const {
        return mCollected;
    }

    uint8_t SilverRupeesInfo::GetTotal() const {
        return mTotal;
    }

    SilverRupeesInfo::SilverRupeesInfo() : mCollected(0), mTotal(0), mRandoGet(RG_NONE) {

    }

    void SilverRupeesInfo::IncrementCollected(uint8_t amount) {
        mCollected += amount;
    }
    void SilverRupeesInfo::Reset() {
        mCollected = 0;
    }
    } // namespace Rando