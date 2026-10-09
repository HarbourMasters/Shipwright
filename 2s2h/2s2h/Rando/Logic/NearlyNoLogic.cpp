#include "Logic.h"

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

namespace Rando {

namespace Logic {

void ApplyNearlyNoLogicToSaveContext(std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool) {
    PreplaceConfinedItems(checkPool, itemPool);

    for (size_t i = 0; i < itemPool.size(); i++) {
        std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size())]);
    }

    std::map<RandoItemId, RandoCheckId> importantItems;
    std::vector<RandoCheckId> safeChecks;

    std::map<RandoItemId, std::vector<SceneId>> itemToSceneBlacklist = {
        { RI_MASK_DEKU,
          { SCENE_MITURIN, SCENE_MITURIN_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK,
            SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_SONG_SONATA,
          { SCENE_MITURIN, SCENE_MITURIN_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK,
            SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_MASK_ZORA,
          { SCENE_SEA, SCENE_SEA_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN,
            SCENE_LAST_BS } },
        { RI_SONG_NOVA,
          { SCENE_SEA, SCENE_SEA_BS, SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN,
            SCENE_LAST_BS } },
        { RI_REMAINS_ODOLWA,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_REMAINS_GOHT,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_REMAINS_GYORG,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_REMAINS_TWINMOLD,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
        { RI_SONG_OATH,
          { SCENE_LAST_DEKU, SCENE_LAST_GORON, SCENE_LAST_ZORA, SCENE_LAST_LINK, SCENE_SOUGEN, SCENE_LAST_BS } },
    };

    std::map<RandoItemId, RandoCheckType> itemToCheckTypeBlacklist = { { RI_SONG_EPONA, RCTYPE_COW } };

    auto IsSafeScene = [&](SceneId sceneId) {
        return sceneId != SCENE_MITURIN &&    // Woodfall Temple
               sceneId != SCENE_MITURIN_BS && // Woodfall Temple Boss
               sceneId != SCENE_SEA &&        // Great Bay Temple
               sceneId != SCENE_SEA_BS &&     // Great Bay Temple Boss
               sceneId != SCENE_LAST_DEKU &&  // Moon Deku
               sceneId != SCENE_LAST_GORON && // Moon Goron
               sceneId != SCENE_LAST_ZORA &&  // Moon Goron
               sceneId != SCENE_LAST_LINK &&  // Moon Human
               sceneId != SCENE_SOUGEN &&     // Moon
               sceneId != SCENE_LAST_BS;      // Moon Boss
    };

    auto IsSafeCheckType = [&](RandoCheckType randoCheckType) { return randoCheckType != RCTYPE_COW; };

    for (auto& randoCheckId : checkPool) {
        if (randoCheckId == RC_UNKNOWN) {
            continue;
        }

        RandoItemId randoItemId = itemPool.back();
        itemPool.pop_back();

        RANDO_SAVE_CHECKS[randoCheckId].shuffled = true;
        RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoItemId;

        auto randoStaticCheck = Rando::StaticData::Checks[randoCheckId];

        if (itemToSceneBlacklist.find(randoItemId) != itemToSceneBlacklist.end() ||
            itemToCheckTypeBlacklist.find(randoItemId) != itemToCheckTypeBlacklist.end()) {
            importantItems[randoItemId] = randoCheckId;
        } else if (IsSafeScene(randoStaticCheck.sceneId) && IsSafeCheckType(randoStaticCheck.randoCheckType)) {
            safeChecks.push_back(randoCheckId);
        }
    }

    auto PerformBlacklistReplacement = [&](RandoItemId randoItemId) {
        auto otherCheckIndex = Ship_Random(0, safeChecks.size());
        RANDO_SAVE_CHECKS[importantItems[randoItemId]].randoItemId =
            RANDO_SAVE_CHECKS[safeChecks[otherCheckIndex]].randoItemId;
        RANDO_SAVE_CHECKS[safeChecks[otherCheckIndex]].randoItemId = randoItemId;
        safeChecks.erase(safeChecks.begin() + otherCheckIndex);
    };

    for (auto& [randoItemId, blacklistedScenes] : itemToSceneBlacklist) {
        if (importantItems.find(randoItemId) != importantItems.end()) {
            auto randoStaticCheck = Rando::StaticData::Checks[importantItems.at(randoItemId)];
            const auto& blacklist = itemToSceneBlacklist.at(randoItemId);
            // This item is blacklisted from this scene, so replace it
            if (std::find(blacklist.begin(), blacklist.end(), randoStaticCheck.sceneId) != blacklist.end()) {
                PerformBlacklistReplacement(randoItemId);
            }
        }
    }

    for (auto& [randoItemId, blackListedChecks] : itemToCheckTypeBlacklist) {
        if (importantItems.find(randoItemId) != importantItems.end()) {
            auto randoStaticCheck = Rando::StaticData::Checks[importantItems.at(randoItemId)];
            const auto& blacklistedRandoCheckType = itemToCheckTypeBlacklist.at(randoItemId);
            // This item is blacklisted from this check type, so replace it
            if (randoStaticCheck.randoCheckType == blacklistedRandoCheckType) {
                PerformBlacklistReplacement(randoItemId);
            }
        }
    }
}

} // namespace Logic

} // namespace Rando
