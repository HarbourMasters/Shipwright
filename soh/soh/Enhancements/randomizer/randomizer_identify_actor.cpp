#include "randomizer_identify_actor.h"
#include "randomizer.h"
#include <z64.h>

ScrubIdentity Randomizer::IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData) {
    struct ScrubIdentity scrubIdentity;

    scrubIdentity.randomizerInf = RAND_INF_MAX;
    scrubIdentity.randomizerCheck = RC_UNKNOWN_CHECK;
    scrubIdentity.getItemId = GI_NONE;
    scrubIdentity.itemPrice = -1;
    scrubIdentity.isShuffled = GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) > 0;

    // Based on z_en_dns.c 93-113
    switch (actorParams) {
        case 0x00:
            scrubIdentity.getItemId = GI_NUTS_5_2;
            break;
        case 0x01:
            scrubIdentity.getItemId = GI_STICKS_1;
            break;
        case 0x02:
            scrubIdentity.getItemId = GI_HEART_PIECE;
            break;
        case 0x03:
            scrubIdentity.getItemId = GI_SEEDS_30;
            break;
        case 0x04:
            scrubIdentity.getItemId = GI_SHIELD_DEKU;
            break;
        case 0x05:
            scrubIdentity.getItemId = GI_BOMBS_5;
            break;
        case 0x06:
            scrubIdentity.getItemId = GI_ARROWS_LARGE;
            break;
        case 0x07:
            scrubIdentity.getItemId = GI_POTION_RED;
            break;
        case 0x08:
            scrubIdentity.getItemId = GI_POTION_GREEN;
            break;
        case 0x09:
            scrubIdentity.getItemId = GI_STICK_UPGRADE_20;
            break;
        case 0x0A:
            scrubIdentity.getItemId = GI_NUT_UPGRADE_30;
            break;
    }

    // TODO: Handle MQ scrubs
    switch (sceneNum) {
        case SCENE_DDAN: // Dodongo's Cavern
            switch (actorParams) {
                case 0x00:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT;
                    scrubIdentity.randomizerCheck = RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT;
                    break;
                case 0x01:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS;
                    scrubIdentity.randomizerCheck = RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS;
                    break;
                case 0x03:
                case 0x06:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT;
                    scrubIdentity.randomizerCheck = RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT;
                    break;
                case 0x04:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY;
                    scrubIdentity.randomizerCheck = RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY;
                    break;
            }
            break;
        case SCENE_BDAN: // Jabu Jabu's Belly
            switch (actorParams) {
                case 0x00:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_JABU_JABUS_BELLY_DEKU_SCRUB;
                    scrubIdentity.randomizerCheck = RC_JABU_JABUS_BELLY_DEKU_SCRUB;
                    break;
            }
            break;
        case SCENE_GANONTIKA: // Ganon's Castle
            switch (actorParams) {
                case 0x05:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT;
                    scrubIdentity.randomizerCheck = RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT;
                    break;
                case 0x03:
                case 0x06:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT;
                    scrubIdentity.randomizerCheck = RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT;
                    break;
                case 0x07:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_RIGHT;
                    scrubIdentity.randomizerCheck = RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT;
                    break;
                case 0x08:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_LEFT;
                    scrubIdentity.randomizerCheck = RC_GANONS_CASTLE_DEKU_SCRUB_LEFT;
                    break;
            }
            break;
        case SCENE_KAKUSIANA: // Grotto
            switch (respawnData) { 
                case 0xE6: // Hyrule Field Scrub Grotto
                    switch (actorParams) {
                        case 0x02:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_HF_DEKU_SCRUB_GROTTO;
                            scrubIdentity.randomizerCheck = RC_HF_DEKU_SCRUB_GROTTO;
                            scrubIdentity.isShuffled = true;
                            break;
                    }
                    break;
                case 0xEB: // ZR Scrub Grotto
                    switch (actorParams) {
                        case 0x07:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_ZR_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x08:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_ZR_DEKU_SCRUB_GROTTO_FRONT;
                            break;
                    }
                    break;
                case 0xEE: // Sacred Forest Meadow Scrub Grotto
                    switch (actorParams) {
                        case 0x07:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_SFM_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x08:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_SFM_DEKU_SCRUB_GROTTO_FRONT;
                            break;
                    }
                    break;
                case 0xEF: // Lake Hylia Scrub Grotto
                    switch (actorParams) {
                        case 0x00:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_LEFT;
                            scrubIdentity.randomizerCheck = RC_LH_DEKU_SCRUB_GROTTO_LEFT;
                            break;
                        case 0x05:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_RIGHT;
                            scrubIdentity.randomizerCheck = RC_LH_DEKU_SCRUB_GROTTO_RIGHT;
                            break;
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_CENTER;
                            scrubIdentity.randomizerCheck = RC_LH_DEKU_SCRUB_GROTTO_CENTER;
                            break;
                    }
                    break;
                case 0xF0: // Gerudo Valley Scrub Grotto
                    switch (actorParams) {
                        case 0x07:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_GV_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x08:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_GV_DEKU_SCRUB_GROTTO_FRONT;
                            break;
                    }
                    break;
                case 0xF5: // Lost Woods Scrub Grotto
                    switch (actorParams) {
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x0A:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.isShuffled = true;
                            break;
                    }
                    break;
                case 0xF9: // Death Mountain Crater Scrub Grotto
                    switch (actorParams) {
                        case 0x00:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_LEFT;
                            scrubIdentity.randomizerCheck = RC_DMC_DEKU_SCRUB_GROTTO_LEFT;
                            break;
                        case 0x05:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_RIGHT;
                            scrubIdentity.randomizerCheck = RC_DMC_DEKU_SCRUB_GROTTO_RIGHT;
                            break;
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_CENTER;
                            scrubIdentity.randomizerCheck = RC_DMC_DEKU_SCRUB_GROTTO_CENTER;
                            break;
                    }
                    break;
                case 0xFB: // Gerudo City Scrub Grotto
                    switch (actorParams) {
                        case 0x00:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_LEFT;
                            scrubIdentity.randomizerCheck = RC_GC_DEKU_SCRUB_GROTTO_LEFT;
                            break;
                        case 0x05:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_RIGHT;
                            scrubIdentity.randomizerCheck = RC_GC_DEKU_SCRUB_GROTTO_RIGHT;
                            break;
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_CENTER;
                            scrubIdentity.randomizerCheck = RC_GC_DEKU_SCRUB_GROTTO_CENTER;
                            break;
                    }
                    break;
                case 0xFC: // Lon Lon Ranch Scrub Grotto
                    switch (actorParams) {
                        case 0x00:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_LEFT;
                            scrubIdentity.randomizerCheck = RC_LLR_DEKU_SCRUB_GROTTO_LEFT;
                            break;
                        case 0x05:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_RIGHT;
                            scrubIdentity.randomizerCheck = RC_LLR_DEKU_SCRUB_GROTTO_RIGHT;
                            break;
                        case 0x03:
                        case 0x06:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_CENTER;
                            scrubIdentity.randomizerCheck = RC_LLR_DEKU_SCRUB_GROTTO_CENTER;
                            break;
                    }
                    break;
                case 0xFD: // Desert Colossus Scrub Grotto
                    switch (actorParams) {
                        case 0x07:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_REAR;
                            scrubIdentity.randomizerCheck = RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR;
                            break;
                        case 0x08:
                            scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT;
                            scrubIdentity.randomizerCheck = RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT;
                            break;
                    }
                    break;
            }
            break;
        case SCENE_SPOT10: // Lost woods
            switch (actorParams) {
                case 0x00:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT;
                    scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT;
                    break;
                case 0x01:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT;
                    scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT;
                    break;
                case 0x09:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_BRIDGE;
                    scrubIdentity.randomizerCheck = RC_LW_DEKU_SCRUB_NEAR_BRIDGE;
                    scrubIdentity.isShuffled = true;
                    break;
            }
            break;
        case SCENE_SPOT17: // Death Mountain Crater
            switch (actorParams) {
                case 0x05:
                    scrubIdentity.randomizerInf = RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB;
                    scrubIdentity.randomizerCheck = RC_DMC_DEKU_SCRUB;
                    break;
            }
            break;
    }
    
    if (randomizerMerchantPrices.find(scrubIdentity.randomizerCheck) != randomizerMerchantPrices.end()) {
        scrubIdentity.itemPrice = randomizerMerchantPrices[scrubIdentity.randomizerCheck];
    }

    return scrubIdentity;
}

CowIdentity Randomizer::IdentifyCow(s32 sceneNum, s32 posX, s32 posZ) {
    struct CowIdentity cowIdentity;

    cowIdentity.randomizerInf = RAND_INF_MAX;
    cowIdentity.randomizerCheck = RC_UNKNOWN_CHECK;

    switch (sceneNum) {
        case SCENE_SOUKO: // Lon Lon Tower
            if (posX == -229 && posZ == 157) {
                cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_LLR_TOWER_LEFT_COW;
                cowIdentity.randomizerCheck = RC_LLR_TOWER_LEFT_COW;
            } else if (posX == -142 && posZ == -140) {
                cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_LLR_TOWER_RIGHT_COW;
                cowIdentity.randomizerCheck = RC_LLR_TOWER_RIGHT_COW;
            }
            break;
        case SCENE_MALON_STABLE:
            if (posX == 116 && posZ == -254) {
                cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_LLR_STABLES_RIGHT_COW;
                cowIdentity.randomizerCheck = RC_LLR_STABLES_RIGHT_COW;
            } else if (posX == -122 && posZ == -254) {
                cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_LLR_STABLES_LEFT_COW;
                cowIdentity.randomizerCheck = RC_LLR_STABLES_LEFT_COW;
            }
            break;
        case SCENE_KAKUSIANA: // Grotto
            if (posX == 2444 && posZ == -471) {
                cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_DMT_COW_GROTTO_COW;
                cowIdentity.randomizerCheck = RC_DMT_COW_GROTTO_COW;
            } else if (posX == 3485 && posZ == -291) {
                cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_HF_COW_GROTTO_COW;
                cowIdentity.randomizerCheck = RC_HF_COW_GROTTO_COW;
            }
            break;
        case SCENE_LINK_HOME:
            cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_KF_LINKS_HOUSE_COW;
            cowIdentity.randomizerCheck = RC_KF_LINKS_HOUSE_COW;
            break;
        case SCENE_LABO: // Impa's house
            cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_KAK_IMPAS_HOUSE_COW;
            cowIdentity.randomizerCheck = RC_KAK_IMPAS_HOUSE_COW;
            break;
        case SCENE_SPOT09: // Gerudo Valley
            cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_GV_COW;
            cowIdentity.randomizerCheck = RC_GV_COW;
            break;
        case SCENE_BDAN: // Jabu's Belly
            cowIdentity.randomizerInf = RAND_INF_COWS_MILKED_JABU_JABUS_BELLY_MQ_COW;
            cowIdentity.randomizerCheck = RC_JABU_JABUS_BELLY_MQ_COW;
            break;
    }

    return cowIdentity;
}
