#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_In/z_en_in.h"
void func_808F3C40(EnIn* enIn, PlayState* play);
}

void EnIn_OnOpenPurchaseText(u16* textId, bool* loadFromMessageTable) {
    RandoSaveCheck milkPurchaseCheck = RANDO_SAVE_CHECKS[RC_GORMAN_MILK_PURCHASE];
    RandoItemId riMilkPurchase = Rando::ConvertItem(milkPurchaseCheck.randoItemId, RC_GORMAN_MILK_PURCHASE);

    if (milkPurchaseCheck.cycleObtained) {
        return;
    }

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

    entry.msg = "%p{{price}} Rupees%w will do ya for %y{{item}}%w!\x11"
                "\xC2%gYes\x11"
                "No";

    std::string itemName = Rando::StaticData::GetItemName(riMilkPurchase, true, RC_GORMAN_MILK_PURCHASE);
    std::string itemPrice = std::to_string(milkPurchaseCheck.price);

    CustomMessage::ReplaceColorChars(&entry.msg);
    CustomMessage::Replace(&entry.msg, "{{item}}", itemName);
    CustomMessage::Replace(&entry.msg, "{{price}}", itemPrice);
    CustomMessage::EnsureMessageEnd(&entry.msg);
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void Rando::ActorBehavior::InitEnInBehavior() {
    // RC_GORMAN_TRACK_GARO_MASK

    /*
     * This is the same block found for non-scripted actors in OfferGetItem.cpp, with the removal of
     * Player_StartTalking() and addition of the rando check.
     */
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (actor->id == ACTOR_EN_IN) {
            if (*item == GI_MASK_GARO) {
                Player* player = GET_PLAYER(gPlayState);
                *should = false;
                RANDO_SAVE_CHECKS[RC_GORMAN_TRACK_GARO_MASK].eligible = true;
                actor->parent = &player->actor;
                player->talkActor = actor;
                player->talkActorDistance = actor->xzDistToPlayer;
                player->exchangeItemAction = PLAYER_IA_MINUS1;
            }
        }
    });

    COND_VB_SHOULD(VB_HAVE_GARO_MASK, IS_RANDO,
                   { *should = RANDO_SAVE_CHECKS[RC_GORMAN_TRACK_GARO_MASK].cycleObtained; });

    // RC_GORMAN_MILK_PURCHASE

    // 50 Rupees will do ya for one drink!
    COND_ID_HOOK(OnOpenText, 0x3490, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS], EnIn_OnOpenPurchaseText);
    COND_ID_HOOK(OnOpenText, 0x3466, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS], EnIn_OnOpenPurchaseText);

    // Modified section of func_808F4414() for 0x3490 and 0x3466 textIds
    COND_VB_SHOULD(VB_BUY_GORMAN_MILK, IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SHOPS], {
        s32* ret = va_arg(args, s32*);
        EnIn* enIn = va_arg(args, EnIn*);
        if (RANDO_SAVE_CHECKS[RC_GORMAN_MILK_PURCHASE].cycleObtained) {
            return;
        }
        *should = true;
        if (gPlayState->msgCtx.choiceIndex == 0) {
            Audio_PlaySfx_MessageDecide();
            if (gSaveContext.save.saveInfo.playerData.rupees >= RANDO_SAVE_CHECKS[RC_GORMAN_MILK_PURCHASE].price) {
                Player* player = GET_PLAYER(gPlayState);
                RANDO_SAVE_CHECKS[RC_GORMAN_MILK_PURCHASE].eligible = true;
                enIn->actionFunc = func_808F3C40;
                Rupees_ChangeBy(-RANDO_SAVE_CHECKS[RC_GORMAN_MILK_PURCHASE].price);
                *ret = true;

                enIn->actor.parent = &player->actor;
                player->talkActor = &enIn->actor;
                player->talkActorDistance = enIn->actor.xzDistToPlayer;
                player->exchangeItemAction = PLAYER_IA_MINUS1;

            } else {
                Audio_PlaySfx(NA_SE_SY_ERROR);
                Actor_ContinueText(gPlayState, &enIn->actor, 0x3468);
                *ret = false;
            }
        } else {
            Audio_PlaySfx_MessageCancel();
            if (enIn->actor.textId == 0x3490) {
                Actor_ContinueText(gPlayState, &enIn->actor, 0x3491);
            } else {
                Actor_ContinueText(gPlayState, &enIn->actor, 0x3467);
            }
            *ret = false;
        }
    });
}
