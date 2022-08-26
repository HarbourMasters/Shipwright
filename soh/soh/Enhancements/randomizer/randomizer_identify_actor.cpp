#include "randomizer_identify_actor.h"
#include "randomizer.h"
#include "randomizer_check_objects.h"
#include <z64.h>

RandomizerCheckObject Randomizer::GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s16 actorParams, s16 otherParams) {
    for (auto const& [checkId, checkObj] : RandomizerCheckObjects::GetAllRCObjects()) {
        if (
            checkObj.actorId == ACTOR_EN_DNS &&
            checkObj.sceneId == sceneNum &&
            (checkObj.identifyParams.size() < 1 || checkObj.identifyParams[0] == actorParams) &&
            (checkObj.identifyParams.size() < 2 || checkObj.identifyParams[1] == otherParams)
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

    RandomizerCheckObject rcObject = Randomizer::GetCheckObjectFromActor(ACTOR_EN_DNS, sceneNum, actorParams == 0x06 ? 0x03 : actorParams, respawnData);

    if (rcObject.rc != RC_UNKNOWN_CHECK) {
        scrubIdentity.randomizerInf = static_cast<RandomizerInf>(rcObject.rcFlag);
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

    RandomizerCheckObject rcObject = Randomizer::GetCheckObjectFromActor(ACTOR_EN_DNS, sceneNum, posX, posZ);

    if (rcObject.rc != RC_UNKNOWN_CHECK) {
        cowIdentity.randomizerInf = static_cast<RandomizerInf>(rcObject.rcFlag);
        cowIdentity.randomizerCheck = rcObject.rc;
    }

    return cowIdentity;
}
