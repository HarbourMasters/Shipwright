#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ShipUtils.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"

#include "overlays/actors/ovl_En_Akindonuts/z_en_akindonuts.h"
void Flags_SetRandoInf(s32 flag);
void func_80BEF360(EnAkindonuts* enAkindonuts, PlayState* play);
void func_80BEEFA8(EnAkindonuts* enAkindonuts, PlayState* play);
}

void EnAkindonuts_ReplacePurchaseMessage(RandoCheckId randoCheckId, RandoInf randoInf, s32 cost, u16* textId,
                                         bool* loadFromMessageTable) {
    auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

    // If the check is not shuffled or they have already purchased the item (unless it's goron village scrub, since they
    // don't have anything else to sell) return and let the vanilla message display
    if (!randoSaveCheck.shuffled || (Flags_GetRandoInf(randoInf) && randoCheckId != RC_GORON_VILLAGE_SCRUB_BOMB_BAG)) {
        return;
    }

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.msg = "I'll sell you %g{{item}}%w for %r{{rupees}} Rupees%w!\xE0";

    CustomMessage::Replace(&entry.msg, "{{item}}",
                           Rando::StaticData::GetItemName(randoSaveCheck.randoItemId, true, randoCheckId));
    CustomMessage::Replace(&entry.msg, "{{rupees}}", std::to_string(cost));

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void EnAkindonuts_ReplaceNotEligibleMessage(RandoInf randoInf, u16* textId, bool* loadFromMessageTable) {
    if (!Flags_GetRandoInf(randoInf)) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Oh, it seems I can't sell you that right now. Sorry for the trouble.\xE0";
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    }
}

void EnAkindonuts_IsEligible(RandoCheckId randoCheckId, RandoInf randoInf, bool* should) {
    auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

    if (!randoSaveCheck.shuffled || Flags_GetRandoInf(randoInf)) {
        return;
    }

    *should = true;
}

// This handles the checks for the business scrubs in the Southern Swamp, Goron Village, Zora Hall, and Ikana Canyon.
void Rando::ActorBehavior::InitEnAkindonutsBehavior() {
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_AKINDONUTS, IS_RANDO, [](Actor* actor) {
        EnAkindonuts* enAkindonuts = (EnAkindonuts*)actor;
        if (enAkindonuts->actionFunc == func_80BEF360) {
            if (enAkindonuts->unk_32C & 0x40) {
                RandoInf randoInf = RANDO_INF_MAX;
                switch (ENAKINDONUTS_GET_3(&enAkindonuts->actor)) {
                    case 0:
                        randoInf = RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB;
                        break;
                    case 1:
                        randoInf = RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB;
                        break;
                    case 2:
                        randoInf = RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB;
                        break;
                    case 3:
                        randoInf = RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB;
                        break;
                }
                if (randoInf != RANDO_INF_MAX && !Flags_GetRandoInf(randoInf)) {
                    Flags_SetRandoInf(randoInf);
                    // Happen at the end of the repeatable purchase chain
                    Rupees_ChangeBy(enAkindonuts->unk_364);
                    enAkindonuts->unk_32C &= ~0x40;
                    enAkindonuts->unk_2DC(enAkindonuts, gPlayState);
                    enAkindonuts->actionFunc = func_80BEEFA8;
                }
            } else {
                // Happen at the end of the trade item chain
                enAkindonuts->unk_2DC(enAkindonuts, gPlayState);
                enAkindonuts->actionFunc = func_80BEEFA8;
            }
        }
    });

    COND_VB_SHOULD(VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BEAN_REFILL, IS_RANDO, {
        EnAkindonuts_IsEligible(RC_SOUTHERN_SWAMP_SCRUB_BEANS, RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB,
                                should);
    });

    // Do you know what magic beans are, sir?...
    COND_ID_HOOK(OnOpenText, 0x15E9, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        EnAkindonuts_ReplacePurchaseMessage(RC_SOUTHERN_SWAMP_SCRUB_BEANS,
                                            RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB, 10, textId,
                                            loadFromMessageTable);
    });

    // I sell Magic Beans to Deku Scrubs,...
    COND_ID_HOOK(OnOpenText, 0x15E1, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        if (!RANDO_SAVE_CHECKS[RC_SOUTHERN_SWAMP_SCRUB_BEANS].shuffled) {
            return;
        }

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "I sell %g{{item}} %wand %gMagic Beans %wto Deku Scrubs, but recently I've been thinking about "
                    "relocating to a new area.\xE0";

        CustomMessage::Replace(
            &entry.msg, "{{item}}",
            Rando::StaticData::GetItemName(RANDO_SAVE_CHECKS[RC_SOUTHERN_SWAMP_SCRUB_BEANS].randoItemId, true,
                                           RC_SOUTHERN_SWAMP_SCRUB_BEANS));
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // Oh, you don't know how to use magic beans?...
    COND_ID_HOOK(OnOpenText, 0x15EC, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        EnAkindonuts_ReplaceNotEligibleMessage(RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB, textId,
                                               loadFromMessageTable);
    });

    COND_VB_SHOULD(VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_POTION_REFILL, IS_RANDO, {
        EnAkindonuts* enAkindonuts = va_arg(args, EnAkindonuts*);
        switch (ENAKINDONUTS_GET_3(&enAkindonuts->actor)) {
            case 2:
                EnAkindonuts_IsEligible(RC_ZORA_HALL_SCRUB_POTION_REFILL,
                                        RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB, should);
                break;
            case 3:
                EnAkindonuts_IsEligible(RC_IKANA_CANYON_SCRUB_POTION_REFILL,
                                        RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB, should);
                break;
        }
    });

    // I'll sell you a Green Potion for 40 Rupees!
    COND_ID_HOOK(OnOpenText, 0x1612, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        EnAkindonuts_ReplacePurchaseMessage(RC_ZORA_HALL_SCRUB_POTION_REFILL,
                                            RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB, 40, textId,
                                            loadFromMessageTable);
    });

    // Don't you need any Blue Potion in case you get cursed?...
    COND_ID_HOOK(OnOpenText, 0x1626, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        EnAkindonuts_ReplacePurchaseMessage(RC_IKANA_CANYON_SCRUB_POTION_REFILL,
                                            RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB, 100, textId,
                                            loadFromMessageTable);
    });

    // You get the potion only! if you don't have an empty bottle... (Used for both potion scrubs)
    COND_ID_HOOK(OnOpenText, 0x1613, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        Player* player = GET_PLAYER(gPlayState);
        if (player->talkActor == nullptr || player->talkActor->id != ACTOR_EN_AKINDONUTS) {
            return;
        }
        switch (ENAKINDONUTS_GET_3(player->talkActor)) {
            case 2:
                EnAkindonuts_ReplaceNotEligibleMessage(RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB, textId,
                                                       loadFromMessageTable);
                break;
            case 3:
                EnAkindonuts_ReplaceNotEligibleMessage(RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB, textId,
                                                       loadFromMessageTable);
                break;
        }
    });

    // TODO: Should there be a bomb bag requirement here still?
    COND_VB_SHOULD(VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BOMB_BAG, IS_RANDO, {
        EnAkindonuts_IsEligible(RC_GORON_VILLAGE_SCRUB_BOMB_BAG, RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB,
                                should);
    });

    COND_VB_SHOULD(VB_AKINDONUTS_CONSIDER_BOMB_BAG_PURCHASED, IS_RANDO, {
        if (RANDO_SAVE_CHECKS[RC_GORON_VILLAGE_SCRUB_BOMB_BAG].shuffled) {
            *should = Flags_GetRandoInf(RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB);
        }
    });

    // I'll give you my Biggest Bomb Bag...
    COND_ID_HOOK(OnOpenText, 0x1600, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        EnAkindonuts_ReplacePurchaseMessage(RC_GORON_VILLAGE_SCRUB_BOMB_BAG,
                                            RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB, 200, textId,
                                            loadFromMessageTable);
    });

    // If you don't have a Big Bomb Bag...
    COND_ID_HOOK(OnOpenText, 0x1602, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        EnAkindonuts_ReplaceNotEligibleMessage(RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB, textId,
                                               loadFromMessageTable);
    });

    // What? You already have a Big Bomb Bag?...
    COND_ID_HOOK(OnOpenText, 0x1601, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        if (RANDO_SAVE_CHECKS[RC_GORON_VILLAGE_SCRUB_BOMB_BAG].shuffled) {
            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
            entry.msg = "What? You already bought that from me, Only one purchase per customer allowed!\xE0";
            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        }
    });
}
