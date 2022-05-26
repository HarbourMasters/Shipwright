#include "randomizer.h"

Randomizer::Randomizer() {
    //todo something?
}
Randomizer::~Randomizer() { 
    this->itemLocations.clear();
}

void Randomizer::PopulateItemLocations(std::string spoilerfilename) {
    //todo actually read in locations from a spoilerfile
    this->itemLocations[KF_MIDOS_TOP_LEFT_CHEST] = KOKIRI_SWORD;
    this->itemLocations[KF_MIDOS_TOP_RIGHT_CHEST] = DEKU_SHIELD;
    this->itemLocations[KF_MIDOS_BOTTOM_LEFT_CHEST] = BOMBCHUS_20;
    this->itemLocations[KF_MIDOS_BOTTOM_RIGHT_CHEST] = ICE_TRAP;
    this->itemLocations[UNKNOWN_CHECK] = UNKNOWN_GET;
}

GetItemID Randomizer::GetItemFromSceneAndParams(s16 sceneNum, s16 actorParams) {
    return GetItemFromGet(this->itemLocations[GetCheckFromSceneAndParams(sceneNum, actorParams)]);
}

GetItemID Randomizer::GetItemFromGet(RandomizerGet randoGet) {
    // todo update this to handle progressive upgrades (need to pass in more than just randoGet)
    switch(randoGet) {
        case KOKIRI_SWORD:
            return GI_SWORD_KOKIRI;
        case DEKU_SHIELD:
            return GI_SHIELD_DEKU;
        case BOMBCHUS_20:
            return GI_BOMBCHUS_20;
        case ICE_TRAP:
            return GI_ICE_TRAP;
        case UNKNOWN_GET:
            return GI_NONE;
    }
}

RandomizerCheck Randomizer::GetCheckFromSceneAndParams(s16 sceneNum, s16 actorParams) {
    switch(sceneNum) {
        case 40:
            switch(actorParams) {
                case 22944:
                    return KF_MIDOS_TOP_LEFT_CHEST;
                case 22945:
                    return KF_MIDOS_TOP_RIGHT_CHEST;
                case 22914:
                    return KF_MIDOS_BOTTOM_LEFT_CHEST;
                case 22787:
                    return KF_MIDOS_BOTTOM_RIGHT_CHEST;
            }
        case 85:
            switch(actorParams) {
                case 1248:
                    return KF_KOKIRI_SWORD_CHEST;
            }
        case 62:
            switch(actorParams) {
                case 22988:
                    return KF_STORMS_GROTTO_CHEST;
                case 22964:
                    return LW_NEAR_SHORTCUTS_GROTTO_CHEST;
                case 31409:
                    return SFM_WOLFOS_GROTTO_CHEST;
                case 22944:
                    return HF_NEAR_MARKET_GROTTO_CHEST;
                case 22978:
                    return HF_SOUTHEAST_GROTTO_CHEST;
                case 22947:
                    return HF_OPEN_GROTTO_CHEST;
                case 22984:
                    return KAK_OPEN_GROTTO_CHEST;
                case 31434:
                    return KAK_REDEAD_GROTTO_CHEST;
                case 23255:
                    return DMT_STORMS_GROTTO_CHEST;
                case 23802:
                    return DMC_UPPER_GROTTO_CHEST;
                case 22985:
                    return ZR_OPEN_GROTTO_CHEST;
            }
        case 64:
            switch(actorParams) {
                case 21824:
                    return GRAVEYARD_SHIELD_GRAVE_CHEST;
            }
        case 63:
            switch(actorParams) {
                case -22592:
                    return GRAVEYARD_HEART_PIECE_GRAVE_CHEST;
            }
        case 65:
            switch(actorParams) {
                case -32736:
                    return GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST;
            }
        case 72:
            switch(actorParams) {
                case 4352:
                    return GRAVEYARD_HOOKSHOT_CHEST;
            }
        case 96:
            switch(actorParams) {
                case 23201:
                    return DMT_CHEST;
            }
        case 98:
            switch(actorParams) {
                case 23232:
                    return GC_MAZE_LEFT_CHEST;
                case 23201:
                    return GC_MAZE_RIGHT_CHEST;
                case 23202:
                    return GC_MAZE_CENTER_CHEST;
            }
        case 88:
            switch(actorParams) {
                case -18496:
                    return ZD_CHEST;
            }
        case 90:
            switch(actorParams) {
                case 23200:
                    return GV_CHEST;
            }
        case 93:
            switch(actorParams) {
                case 1984:
                    return GF_CHEST;
            }
        case 94:
            switch(actorParams) {
                case -30048:
                    return WASTELAND_CHEST;
            }
        case 0:
            switch(actorParams) {
                case 2083:
                    return DEKU_TREE_MAP_CHEST;
                case 22789:
                    return DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST;
                case 161:
                    return DEKU_TREE_SLINGSHOT_CHEST;
                case 2050:
                    return DEKU_TREE_COMPASS_CHEST;
                case 22790:
                    return DEKU_TREE_COMPASS_ROOM_SIDE_CHEST;
                case 22788:
                    return DEKU_TREE_BASEMENT_CHEST;
            }
        case 1:
            switch(actorParams) {
                case 2088:
                    return DODONGOS_CAVERN_MAP_CHEST;
                case 2053:
                    return DODONGOS_CAVERN_COMPASS_CHEST;
                case 22982:
                    return DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST;
                case 1604:
                    return DODONGOS_CAVERN_BOMB_BAG_CHEST;
                case 21802:
                    return DODONGOS_CAVERN_END_OF_BRIDGE_CHEST;
                case 20512:
                    return DODONGOS_CAVERN_BOSS_ROOM_CHEST;
            }
        case 2:
            switch(actorParams) {
                case 4289:
                    return JABU_JABUS_BELLY_BOOMERANG_CHEST;
                case 6178:
                    return JABU_JABUS_BELLY_MAP_CHEST;
                case -18428:
                    return JABU_JABUS_BELLY_COMPASS_CHEST;
            }
    }

    return UNKNOWN_CHECK;
}