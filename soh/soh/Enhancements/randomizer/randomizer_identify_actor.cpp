#include "randomizer_identify_actor.h"
#include "randomizer.h"
#include "randomizer_check_objects.h"
#include "randomizer_inf.h"
#include "randomizerTypes.h"
#include <macros.h>
#include <map>

// There has been some talk about potentially just using the RC identifier to store flags rather than randomizer inf, so
// for now we're not going to store randomzierInf in the randomizer check objects, we're just going to map them 1:1 here
std::map<RandomizerCheck, RandomizerInf> rcToRandomizerInf = {
    { RC_KF_LINKS_HOUSE_COW,                                          RAND_INF_COWS_MILKED_KF_LINKS_HOUSE_COW },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT,                       RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,                        RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT },
    { RC_LW_DEKU_SCRUB_NEAR_BRIDGE,                                   RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_BRIDGE },
    { RC_LW_DEKU_SCRUB_GROTTO_REAR,                                   RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_REAR },
    { RC_LW_DEKU_SCRUB_GROTTO_FRONT,                                  RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_FRONT },
    { RC_SFM_DEKU_SCRUB_GROTTO_REAR,                                  RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_REAR },
    { RC_SFM_DEKU_SCRUB_GROTTO_FRONT,                                 RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_FRONT },
    { RC_HF_DEKU_SCRUB_GROTTO,                                        RAND_INF_SCRUBS_PURCHASED_HF_DEKU_SCRUB_GROTTO },
    { RC_HF_COW_GROTTO_COW,                                           RAND_INF_COWS_MILKED_HF_COW_GROTTO_COW },
    { RC_LH_DEKU_SCRUB_GROTTO_LEFT,                                   RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_LEFT },
    { RC_LH_DEKU_SCRUB_GROTTO_RIGHT,                                  RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_LH_DEKU_SCRUB_GROTTO_CENTER,                                 RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_CENTER },
    { RC_GV_DEKU_SCRUB_GROTTO_REAR,                                   RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_REAR },
    { RC_GV_DEKU_SCRUB_GROTTO_FRONT,                                  RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_FRONT },
    { RC_GV_COW,                                                      RAND_INF_COWS_MILKED_GV_COW },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,                             RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_REAR },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT,                            RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT },
    { RC_KAK_IMPAS_HOUSE_COW,                                         RAND_INF_COWS_MILKED_KAK_IMPAS_HOUSE_COW },
    { RC_DMT_COW_GROTTO_COW,                                          RAND_INF_COWS_MILKED_DMT_COW_GROTTO_COW },
    { RC_GC_DEKU_SCRUB_GROTTO_LEFT,                                   RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_LEFT },
    { RC_GC_DEKU_SCRUB_GROTTO_RIGHT,                                  RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_GC_DEKU_SCRUB_GROTTO_CENTER,                                 RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_CENTER },
    { RC_DMC_DEKU_SCRUB,                                              RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB },
    { RC_DMC_DEKU_SCRUB_GROTTO_LEFT,                                  RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_LEFT },
    { RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,                                 RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_DMC_DEKU_SCRUB_GROTTO_CENTER,                                RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_CENTER },
    { RC_ZR_DEKU_SCRUB_GROTTO_REAR,                                   RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_REAR },
    { RC_ZR_DEKU_SCRUB_GROTTO_FRONT,                                  RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_FRONT },
    { RC_LLR_DEKU_SCRUB_GROTTO_LEFT,                                  RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_LEFT },
    { RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,                                 RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_LLR_DEKU_SCRUB_GROTTO_CENTER,                                RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_CENTER },
    { RC_LLR_STABLES_LEFT_COW,                                        RAND_INF_COWS_MILKED_LLR_STABLES_LEFT_COW },
    { RC_LLR_STABLES_RIGHT_COW,                                       RAND_INF_COWS_MILKED_LLR_STABLES_RIGHT_COW },
    { RC_LLR_TOWER_LEFT_COW,                                          RAND_INF_COWS_MILKED_LLR_TOWER_LEFT_COW },
    { RC_LLR_TOWER_RIGHT_COW,                                         RAND_INF_COWS_MILKED_LLR_TOWER_RIGHT_COW },
    { RC_DEKU_TREE_MQ_DEKU_SCRUB,                                     RAND_INF_SCRUBS_PURCHASED_DEKU_TREE_MQ_DEKU_SCRUB },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,               RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS,          RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,              RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,                            RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,                    RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,                   RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,                     RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS },
    { RC_JABU_JABUS_BELLY_DEKU_SCRUB,                                 RAND_INF_SCRUBS_PURCHASED_JABU_JABUS_BELLY_DEKU_SCRUB },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,                        RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT,                       RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT },
    { RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,                              RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_RIGHT },
    { RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,                               RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_LEFT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,                           RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,                     RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,                          RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT,                    RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,                            RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT },
};

RandomizerCheckObject Randomizer::GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams = 0x00) {
    for (auto const& [checkId, checkObj] : RandomizerCheckObjects::GetAllRCObjects()) {
        if (
            checkObj.actorId == actorId &&
            checkObj.sceneId == sceneNum &&
            checkObj.actorParams == actorParams
        ) {
            return checkObj;
        }
    }

    return RandomizerCheckObjects::GetAllRCObjects()[RC_UNKNOWN_CHECK];
}

ScrubIdentity Randomizer::IdentifyScrub(s16 sceneNum, s16 actorParams, s16 respawnData) {
    struct ScrubIdentity scrubIdentity;

    scrubIdentity.randomizerInf = RAND_INF_MAX;
    scrubIdentity.randomizerCheck = RC_UNKNOWN_CHECK;
    scrubIdentity.getItemId = GI_NONE;
    scrubIdentity.itemPrice = -1;
    scrubIdentity.isShuffled = GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) > 0;

    RandomizerCheckObject rcObject = Randomizer::GetCheckObjectFromActor(ACTOR_EN_DNS, sceneNum, TWO_ACTOR_PARAMS(actorParams == 0x06 ? 0x03 : actorParams, respawnData));

    if (rcObject.rc != RC_UNKNOWN_CHECK) {
        scrubIdentity.randomizerInf = rcToRandomizerInf[rcObject.rc];
        scrubIdentity.randomizerCheck = rcObject.rc;
        scrubIdentity.getItemId = rcObject.ogItemId;
        scrubIdentity.isShuffled = GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) > 0;

        if (rcObject.rc == RC_HF_DEKU_SCRUB_GROTTO || rcObject.rc == RC_LW_DEKU_SCRUB_GROTTO_FRONT || rcObject.rc == RC_LW_DEKU_SCRUB_NEAR_BRIDGE) {
            scrubIdentity.isShuffled = true;
        }

        if (randomizerMerchantPrices.find(scrubIdentity.randomizerCheck) != randomizerMerchantPrices.end()) {
            scrubIdentity.itemPrice = randomizerMerchantPrices[scrubIdentity.randomizerCheck];
        }
    }

    return scrubIdentity;
}

CowIdentity Randomizer::IdentifyCow(s16 sceneNum, s16 posX, s16 posZ) {
    struct CowIdentity cowIdentity;

    cowIdentity.randomizerInf = RAND_INF_MAX;
    cowIdentity.randomizerCheck = RC_UNKNOWN_CHECK;

    RandomizerCheckObject rcObject = Randomizer::GetCheckObjectFromActor(ACTOR_EN_COW, sceneNum, TWO_ACTOR_PARAMS(posX, posZ));

    if (rcObject.rc != RC_UNKNOWN_CHECK) {
        cowIdentity.randomizerInf = rcToRandomizerInf[rcObject.rc];
        cowIdentity.randomizerCheck = rcObject.rc;
    }

    return cowIdentity;
}

RandomizerCheck Randomizer::GetCheckFromActor(s16 actorId, s16 sceneNum, s32 actorParams) {

    // TODO: Migrate these special cases into table, or at least document why they are special
    switch(sceneNum) {
        case SCENE_TAKARAYA:
            if(actorParams == 20170) return RC_MARKET_TREASURE_CHEST_GAME_REWARD;

            // RANDOTODO update logic to match 3ds rando when we implement keysanity
            // keep keys og
            if ((actorParams & 0x60) == 0x20) break;

            if (GetRandoSettingValue(RSK_SHUFFLE_CHEST_MINIGAME)) {
                if((actorParams & 0xF) < 2) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_1;
                if((actorParams & 0xF) < 4) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_2;
                if((actorParams & 0xF) < 6) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_3;
                if((actorParams & 0xF) < 8) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_4;
                if((actorParams & 0xF) < 10) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_5;
            }
            break;
        case SCENE_SHRINE:
        case SCENE_SHRINE_N:
        case SCENE_SHRINE_R:
            switch (actorParams) {
                case 14342:
                    return RC_TOT_LEFT_GOSSIP_STONE;
                case 14599:
                    return RC_TOT_LEFT_CENTER_GOSSIP_STONE;
                case 14862:
                    return RC_TOT_RIGHT_CENTER_GOSSIP_STONE;
                case 15120:
                    return RC_TOT_RIGHT_GOSSIP_STONE;
            }
            break;
        case SCENE_SPOT01:
            switch (actorId) {
                case ACTOR_EN_NIW_LADY:
                    if (LINK_IS_ADULT) {
                        return RC_KAK_ANJU_AS_ADULT;
                    } else {
                        return RC_KAK_ANJU_AS_CHILD;
                    }
            }
            break;
        case SCENE_SPOT06:
            switch (actorId) {
                case ACTOR_ITEM_ETCETERA:
                    if (LINK_IS_ADULT) {
                        return RC_LH_SUN;
                    } else {
                        return RC_LH_UNDERWATER_ITEM;
                    }
            }
            break;
        case SCENE_SPOT08:
            switch (actorParams) {
                case 15362:
                case 14594:
                    return RC_JABU_GOSSIP_STONE;
                case 14849:
                case 14337:
                    return RC_FAIRY_GOSSIP_STONE;
            }
            break;
        case SCENE_SPOT12:
            switch (actorParams) {
                case 262:
                case 1984:
                    return RC_GF_CHEST;
            }
            break;
    }

    return Randomizer::GetCheckObjectFromActor(actorId, sceneNum, actorParams).rc;
}