#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Masks.3DSMaskEquip"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static PlayerMask sPendingMask = PLAYER_MASK_NONE;
static HOOK_ID sPlayerUpdateHookId = 0;

static bool IsTransformationMask(PlayerMask mask) {
    return mask <= PLAYER_MASK_DEKU && mask >= PLAYER_MASK_FIERCE_DEITY;
}

static bool IsMask(ItemId itemId) {
    // Non-transformation masks
    if ((itemId >= ITEM_MASK_TRUTH) && (itemId <= ITEM_MASK_SCENTS)) {
        return true;
    }

    // Transformation masks
    Player* player = GET_PLAYER(gPlayState);
    if ((player != NULL) && (player->transformation == PLAYER_FORM_FIERCE_DEITY)) {
        return itemId >= ITEM_MASK_DEKU && itemId <= ITEM_MASK_ZORA;
    }
    return false;
}

static bool IsMaskAction(PlayerItemAction itemAction) {
    return (itemAction >= PLAYER_IA_MASK_TRUTH) && (itemAction <= PLAYER_IA_MASK_SCENTS);
}

static void UnregisterMaskSwap() {
    if (sPlayerUpdateHookId != 0) {
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(sPlayerUpdateHookId);
        sPlayerUpdateHookId = 0;
    }
}

static void OnTransform(Actor* actor) {
    Player* player = (Player*)actor;

    if (player->transformation == PLAYER_FORM_HUMAN) {
        if (sPendingMask != PLAYER_MASK_NONE && !IsTransformationMask(sPendingMask) &&
            player->currentMask != sPendingMask) {
            player->currentMask = sPendingMask;
            gSaveContext.save.equippedMask = sPendingMask;
        }
        sPendingMask = PLAYER_MASK_NONE;
        UnregisterMaskSwap();
    }
}

static void RegisterMaskSwap() {
    if (sPlayerUpdateHookId == 0) {
        sPlayerUpdateHookId =
            GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_PLAYER, OnTransform);
    }
}

static void AllowMask(ItemId* itemId, bool* should) {
    if (IsMask(*itemId)) {
        *should = false;
    }
}

void RegisterMaskSwapHooks() {
    COND_VB_SHOULD(VB_ITEM_BE_RESTRICTED, CVAR, {
        ItemId* itemId = va_arg(args, ItemId*);
        AllowMask(itemId, should);
    });

    COND_VB_SHOULD(VB_USE_ITEM_CONSIDER_LINK_HUMAN, CVAR, {
        PlayerItemAction* itemAction = va_arg(args, PlayerItemAction*);
        Player* player = GET_PLAYER(gPlayState);
        if (player != NULL && player->transformation != PLAYER_FORM_HUMAN && IsMaskAction(*itemAction)) {
            PlayerMask mask = static_cast<PlayerMask>(GET_MASK_FROM_IA(*itemAction));
            if (!IsTransformationMask(mask)) { // don't queue transformation masks
                sPendingMask = mask;
                gSaveContext.save.equippedMask = sPendingMask;
                RegisterMaskSwap();
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterMaskSwapHooks, { CVAR_NAME });
