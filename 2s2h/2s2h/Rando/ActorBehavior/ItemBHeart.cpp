#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include "functions.h"
#include "variables.h"

#include "overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
void ItemBHeart_UpdateModel(ItemBHeart* itemBHeart, PlayState* play);
}

void ItemBHeart_DrawCustom(Actor* thisx, PlayState* play) {
    auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE, 0x1F, gPlayState->sceneId);
    if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
        return;
    }

    auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

    Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, randoStaticCheck.randoCheckId),
                    randoStaticCheck.randoCheckId, thisx);
}

void ItemBHeart_UpdateCustom(Actor* thisx, PlayState* play) {
    ItemBHeart* itemBHeart = (ItemBHeart*)thisx;

    ItemBHeart_UpdateModel(itemBHeart, play);

    if (!(itemBHeart->baseScale < BHEART_SCALE_MIN_COLLECTIBLE)) {
        if ((thisx->xzDistToPlayer <= 30.0f) && (fabsf(thisx->playerHeightRel) <= fabsf(80.0f))) {
            Flags_SetCollectible(play, 0x1F);
            Actor_Kill(&itemBHeart->actor);
            return;
        }
    }
}

void Rando::ActorBehavior::InitItemBHeartBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_ITEM_B_HEART, IS_RANDO, [](Actor* actor) {
        ItemBHeart* itemBHeart = (ItemBHeart*)actor;

        auto randoStaticCheck =
            Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE, 0x1F, gPlayState->sceneId);
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

        actor->draw = ItemBHeart_DrawCustom;
        actor->update = ItemBHeart_UpdateCustom;
    });
}
