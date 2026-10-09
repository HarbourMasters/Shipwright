#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "assets/interface/parameter_static/parameter_static.h"
#include "src/overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"

void Player_UseItem(PlayState* play, Player* player, ItemId item);
void func_8082E1F0(Player* player, u16 sfxId);
PlayerItemAction Player_ItemToItemAction(Player* player, ItemId item);
}

#define CVAR_NAME "gEnhancements.Masks.EasyMaskEquip"
#define CVAR CVarGetInteger(CVAR_NAME, 0)
#define CVAR_PERSISTENT_BUNNY_HOOD_NAME "gEnhancements.Masks.PersistentBunnyHood.Enabled"
#define CVAR_PERSISTENT_BUNNY_HOOD CVarGetInteger(CVAR_PERSISTENT_BUNNY_HOOD_NAME, 0)

extern bool ShouldFastTransform();

typedef enum PendingMode {
    PENDING_NONE,
    PENDING_REGULAR_MASK,
    PENDING_PLAYER_ACTION,
    PENDING_RETURN_TO_HUMAN_THEN_REGULAR,
    PENDING_RETURN_TO_HUMAN_THEN_GIANT,
} PendingMode;

struct VisualMaskOverride {
    PlayerMask mask = PLAYER_MASK_NONE;
    bool active = false;
};

struct PendingActionState {
    PendingMode mode = PENDING_NONE;
    PlayerMask selectedMask = PLAYER_MASK_NONE;
    PlayerMask targetMask = PLAYER_MASK_NONE;
    PlayerItemAction itemAction = PLAYER_IA_NONE;
    PlayerItemAction savedItemAction = PLAYER_IA_NONE;
    PlayerUnkAA5 savedUnkAA5 = PLAYER_UNKAA5_0;
    PlayerMask savedEquippedMask = PLAYER_MASK_NONE;
    bool hasSavedEquippedMask = false;
};

struct EasyMaskEquipStateSnapshot {
    PendingActionState pendingAction = {};
    VisualMaskOverride visualMaskOverride = {};
    PlayerMask maskEquippedWithoutButton = PLAYER_MASK_NONE;
    PlayerItemAction playerItemAction = PLAYER_IA_NONE;
    PlayerUnkAA5 playerUnkAA5 = PLAYER_UNKAA5_0;
    PlayerMask equippedMask = PLAYER_MASK_NONE;
    bool hasPendingAction = false;
    bool hasPlayer = false;
};

static bool sShowingAButtonPrompt = false;
static VisualMaskOverride sVisualMaskOverride = {};
static PendingActionState sPendingAction = {};
static PlayerMask sMaskEquippedWithoutButton = PLAYER_MASK_NONE;

static bool IsMaskItem(ItemId item) {
    return (item >= ITEM_MASK_DEKU) && (item <= ITEM_MASK_GIANT);
}

static bool IsRegularMask(PlayerMask mask) {
    return (mask >= PLAYER_MASK_TRUTH) && (mask < PLAYER_MASK_GIANT);
}

static bool ShouldMaintainMaskWithoutButton(PlayerMask mask) {
    return IsRegularMask(mask) || (mask == PLAYER_MASK_GIANT);
}

static ItemId GetMaskItem(PlayerMask mask) {
    if (mask == PLAYER_MASK_NONE) {
        return ITEM_NONE;
    }

    return static_cast<ItemId>(Player_MaskIdToItemId(mask - 1));
}

static void SetVisualMaskOverride(PlayerMask mask) {
    sVisualMaskOverride.mask = mask;
    sVisualMaskOverride.active = true;
}

static void ClearVisualMaskOverride() {
    sVisualMaskOverride.mask = PLAYER_MASK_NONE;
    sVisualMaskOverride.active = false;
}

static bool IsPendingPlayerActionMode() {
    return (sPendingAction.mode == PENDING_PLAYER_ACTION) ||
           (sPendingAction.mode == PENDING_RETURN_TO_HUMAN_THEN_REGULAR) ||
           (sPendingAction.mode == PENDING_RETURN_TO_HUMAN_THEN_GIANT);
}

static bool HasPendingAction() {
    return sPendingAction.mode != PENDING_NONE;
}

static EasyMaskEquipStateSnapshot CaptureEasyMaskEquipState(Player* player) {
    EasyMaskEquipStateSnapshot snapshot = {};

    snapshot.pendingAction = sPendingAction;
    snapshot.visualMaskOverride = sVisualMaskOverride;
    snapshot.maskEquippedWithoutButton = sMaskEquippedWithoutButton;
    snapshot.equippedMask = static_cast<PlayerMask>(gSaveContext.save.equippedMask);
    snapshot.hasPendingAction = HasPendingAction();
    snapshot.hasPlayer = player != nullptr;
    if (snapshot.hasPlayer) {
        snapshot.playerItemAction = static_cast<PlayerItemAction>(player->itemAction);
        snapshot.playerUnkAA5 = static_cast<PlayerUnkAA5>(player->unk_AA5);
    }

    return snapshot;
}

static void RestoreEasyMaskEquipState(Player* player, const EasyMaskEquipStateSnapshot& snapshot) {
    sPendingAction = snapshot.pendingAction;
    sVisualMaskOverride = snapshot.visualMaskOverride;
    sMaskEquippedWithoutButton = snapshot.maskEquippedWithoutButton;
    gSaveContext.save.equippedMask = snapshot.equippedMask;

    if (snapshot.hasPlayer && (player != nullptr)) {
        player->itemAction = snapshot.playerItemAction;
        player->unk_AA5 = snapshot.playerUnkAA5;
    }
}

static void ClearPendingActionOnly() {
    sPendingAction.mode = PENDING_NONE;
    sPendingAction.selectedMask = PLAYER_MASK_NONE;
    sPendingAction.targetMask = PLAYER_MASK_NONE;
    sPendingAction.itemAction = PLAYER_IA_NONE;
    sPendingAction.savedItemAction = PLAYER_IA_NONE;
    sPendingAction.savedUnkAA5 = PLAYER_UNKAA5_0;
    sPendingAction.savedEquippedMask = PLAYER_MASK_NONE;
    sPendingAction.hasSavedEquippedMask = false;
}

static void ResetPendingAction() {
    ClearPendingActionOnly();
    ClearVisualMaskOverride();
}

static void ResetOrRestorePreviousPendingAction(Player* player, const EasyMaskEquipStateSnapshot& previousState) {
    if (previousState.hasPendingAction) {
        RestoreEasyMaskEquipState(player, previousState);
    } else {
        ResetPendingAction();
    }
}

static void SavePendingSnapshot(Player* player) {
    sPendingAction.savedItemAction =
        (player != nullptr) ? static_cast<PlayerItemAction>(player->itemAction) : PLAYER_IA_NONE;
    sPendingAction.savedUnkAA5 = (player != nullptr) ? static_cast<PlayerUnkAA5>(player->unk_AA5) : PLAYER_UNKAA5_0;
    sPendingAction.savedEquippedMask = static_cast<PlayerMask>(gSaveContext.save.equippedMask);
    sPendingAction.hasSavedEquippedMask = true;
}

static void RestorePendingPlayerAction(Player* player) {
    if ((player != nullptr) && IsPendingPlayerActionMode() && (player->itemAction == sPendingAction.itemAction) &&
        (player->unk_AA5 == PLAYER_UNKAA5_5)) {
        player->itemAction = sPendingAction.savedItemAction;
        player->unk_AA5 = sPendingAction.savedUnkAA5;
    }
}

static void CancelPendingAction(Player* player) {
    RestorePendingPlayerAction(player);

    if (sPendingAction.hasSavedEquippedMask) {
        gSaveContext.save.equippedMask = sPendingAction.savedEquippedMask;
    }

    ResetPendingAction();
}

static void PlayAppliedMaskSfx(Player* player, PlayerMask targetMask) {
    if (player == nullptr) {
        return;
    }

    func_8082E1F0(player, (targetMask == PLAYER_MASK_NONE) ? NA_SE_PL_TAKE_OUT_SHIELD : NA_SE_PL_CHANGE_ARMS);
}

static void ApplyRegularMaskTarget(Player* player, PlayerMask targetMask, bool playSfx = true) {
    if (player == nullptr) {
        return;
    }

    player->prevMask = player->currentMask;
    player->currentMask = targetMask;
    gSaveContext.save.equippedMask = targetMask;
    sMaskEquippedWithoutButton = IsRegularMask(targetMask) ? targetMask : PLAYER_MASK_NONE;
    if (playSfx) {
        PlayAppliedMaskSfx(player, targetMask);
    }
}

static bool PlayerActionReachedTarget(Player* player) {
    if (player == nullptr) {
        return false;
    }

    if (sPendingAction.targetMask == PLAYER_MASK_NONE) {
        return (player->currentMask == PLAYER_MASK_NONE) && (player->transformation == PLAYER_FORM_HUMAN);
    }

    if ((sPendingAction.targetMask >= PLAYER_MASK_FIERCE_DEITY) && (sPendingAction.targetMask <= PLAYER_MASK_DEKU)) {
        PlayerTransformation targetForm =
            static_cast<PlayerTransformation>(sPendingAction.targetMask - PLAYER_MASK_FIERCE_DEITY);
        return (player->currentMask == sPendingAction.targetMask) && (player->transformation == targetForm);
    }

    return player->currentMask == sPendingAction.targetMask;
}

static bool PlayerCanProcessFormChangingMaskAction(Player* player) {
    if (player == nullptr) {
        return false;
    }

    // Mirrors Player_ActionHandler_13 so EME does not keep pending state for mask actions vanilla cancels.
    return (player->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) ||
           (player->stateFlags1 & (PLAYER_STATE1_8000000 | PLAYER_STATE1_800000)) ||
           (player->stateFlags3 & PLAYER_STATE3_8) || (player->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT);
}

static bool QueueFormChangingMaskAction(Player* player, ItemId item, PlayerMask selectedMask,
                                        PlayerItemAction itemAction, PlayerMask targetMask, PendingMode pendingMode,
                                        bool playMenuSfx = true) {
    if (!PlayerCanProcessFormChangingMaskAction(player)) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
        return false;
    }

    SavePendingSnapshot(player);

    sPendingAction.mode = pendingMode;
    sPendingAction.selectedMask = selectedMask;
    sPendingAction.targetMask = targetMask;
    sPendingAction.itemAction = itemAction;

    Player_UseItem(gPlayState, player, item);

    if ((player->itemAction != itemAction) || (player->unk_AA5 != PLAYER_UNKAA5_5) ||
        ((sPendingAction.savedItemAction == player->itemAction) && (sPendingAction.savedUnkAA5 == player->unk_AA5))) {
        player->itemAction = sPendingAction.savedItemAction;
        player->unk_AA5 = sPendingAction.savedUnkAA5;
        if (sPendingAction.hasSavedEquippedMask) {
            gSaveContext.save.equippedMask = sPendingAction.savedEquippedMask;
        }
        ClearPendingActionOnly();
        return false;
    }

    SetVisualMaskOverride(targetMask);
    if (playMenuSfx) {
        Audio_PlaySfx(NA_SE_SY_DECIDE);
    }
    return true;
}

static bool PlayerCanStartFollowUpAction(Player* player) {
    return (player != nullptr) && (gPlayState->pauseCtx.state == PAUSE_STATE_OFF) &&
           (player->transformation == PLAYER_FORM_HUMAN) && (player->currentMask == PLAYER_MASK_NONE) &&
           (player->unk_AA5 == PLAYER_UNKAA5_0) &&
           !(player->stateFlags1 & (PLAYER_STATE1_2 | PLAYER_STATE1_100 | PLAYER_STATE1_20000000));
}

static PlayerMask GetMaskFromItem(Player* player, ItemId item) {
    PlayerItemAction itemAction = Player_ItemToItemAction(player, item);

    if ((itemAction < PLAYER_IA_MASK_MIN) || (itemAction > PLAYER_IA_MASK_MAX)) {
        return PLAYER_MASK_NONE;
    }

    return static_cast<PlayerMask>(GET_MASK_FROM_IA(itemAction));
}

static bool ShouldDeferToPersistentBunnyHood(ItemId item) {
    return CVAR_PERSISTENT_BUNNY_HOOD && (item == ITEM_MASK_BUNNY);
}

static bool ShouldAllowFierceDeityMask(PlayerMask targetMask) {
    if (targetMask != PLAYER_MASK_FIERCE_DEITY) {
        return true;
    }

    if (gPlayState == nullptr) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
        return false;
    }

    bool vanillaSceneConditionResult =
        (gPlayState->sceneId != SCENE_MITURIN_BS) && (gPlayState->sceneId != SCENE_HAKUGIN_BS) &&
        (gPlayState->sceneId != SCENE_SEA_BS) && (gPlayState->sceneId != SCENE_INISIE_BS) &&
        (gPlayState->sceneId != SCENE_LAST_BS);
    if (GameInteractor_Should(VB_DISABLE_FD_MASK, vanillaSceneConditionResult)) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
        return false;
    }

    return true;
}

static bool ShouldAllowGiantMask(Player* player, PlayerMask targetMask) {
    if (targetMask != PLAYER_MASK_GIANT) {
        return true;
    }

    ItemId item = ITEM_MASK_GIANT;
    if ((gPlayState == nullptr) ||
        GameInteractor_Should(VB_ITEM_BE_RESTRICTED, gPlayState->sceneId != SCENE_INISIE_BS, &item)) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
        return false;
    }

    if ((player == nullptr) ||
        ((player->currentMask != PLAYER_MASK_GIANT) &&
         ((gSaveContext.magicState != MAGIC_STATE_IDLE) || (gSaveContext.save.saveInfo.playerData.magic == 0)))) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
        return false;
    }

    return true;
}

static bool IsMaskPageReady(PauseContext* pauseCtx) {
    return (gPlayState != nullptr) && (pauseCtx != nullptr) && (pauseCtx->state == PAUSE_STATE_MAIN) &&
           (pauseCtx->pageIndex == PAUSE_MASK) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
           (pauseCtx->debugEditor == DEBUG_EDITOR_NONE) && !pauseCtx->itemDescriptionOn &&
           (gPlayState->msgCtx.msgLength == 0);
}

static bool CursorIsOnMask(PauseContext* pauseCtx) {
    if (!IsMaskPageReady(pauseCtx)) {
        return false;
    }

    u16 cursorItem = pauseCtx->cursorItem[PAUSE_MASK];
    return (cursorItem != PAUSE_ITEM_NONE) && IsMaskItem(static_cast<ItemId>(cursorItem));
}

static void RestoreAButtonPrompt() {
    if (!sShowingAButtonPrompt || (gPlayState == nullptr)) {
        return;
    }

    InterfaceContext* interfaceCtx = &gPlayState->interfaceCtx;
    if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_INFO) {
        Interface_SetAButtonDoAction(gPlayState, DO_ACTION_INFO);
    }

    sShowingAButtonPrompt = false;
}

static void UpdateMaskPageAButtonPrompt(PauseContext* pauseCtx) {
    if (!CursorIsOnMask(pauseCtx)) {
        RestoreAButtonPrompt();
        return;
    }

    InterfaceContext* interfaceCtx = &gPlayState->interfaceCtx;
    if (interfaceCtx->aButtonDoActionDelayed != DO_ACTION_DECIDE) {
        Interface_SetAButtonDoAction(gPlayState, DO_ACTION_DECIDE);
    }

    if (gSaveContext.buttonStatus[EQUIP_SLOT_A] != BTN_ENABLED) {
        gSaveContext.buttonStatus[EQUIP_SLOT_A] = BTN_ENABLED;
        gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
        Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    }

    sShowingAButtonPrompt = true;
}

static s16 GetOwnedMaskSlot(ItemId item) {
    for (s16 slot = 0; slot < MASK_NUM_SLOTS; slot++) {
        if (gSaveContext.save.saveInfo.inventory.items[ITEM_NUM_SLOTS + slot] == item) {
            return slot;
        }
    }

    return -1;
}

static ItemId GetDisplayedMaskItem() {
    if (sVisualMaskOverride.active) {
        return GetMaskItem(sVisualMaskOverride.mask);
    }

    return static_cast<ItemId>(Player_GetCurMaskItemId(gPlayState));
}

static void DrawActiveMaskOutline(PauseContext* pauseCtx, s16 slot) {
    GraphicsContext* gfxCtx = gPlayState->state.gfxCtx;
    Vtx* activeMaskVtx = (Vtx*)GRAPH_ALLOC(gfxCtx, 4 * sizeof(Vtx));
    s16 slotX = slot % MASK_GRID_COLS;
    s16 slotY = slot / MASK_GRID_COLS;
    s16 initialX = 0 - (MASK_GRID_COLS * MASK_GRID_CELL_WIDTH) / 2;
    s16 initialY = (MASK_GRID_ROWS * MASK_GRID_CELL_HEIGHT) / 2 - 6;
    s16 vtxX = (initialX + (slotX * MASK_GRID_CELL_WIDTH)) + MASK_GRID_QUAD_MARGIN;
    s16 vtxY = (initialY - (slotY * MASK_GRID_CELL_HEIGHT)) + pauseCtx->offsetY - MASK_GRID_QUAD_MARGIN;

    activeMaskVtx[0].v.ob[0] = activeMaskVtx[2].v.ob[0] = vtxX + MASK_GRID_SELECTED_QUAD_MARGIN;
    activeMaskVtx[1].v.ob[0] = activeMaskVtx[3].v.ob[0] = activeMaskVtx[0].v.ob[0] + MASK_GRID_SELECTED_QUAD_WIDTH;
    activeMaskVtx[0].v.ob[1] = activeMaskVtx[1].v.ob[1] = vtxY - MASK_GRID_SELECTED_QUAD_MARGIN;
    activeMaskVtx[2].v.ob[1] = activeMaskVtx[3].v.ob[1] = activeMaskVtx[0].v.ob[1] - MASK_GRID_SELECTED_QUAD_HEIGHT;
    activeMaskVtx[0].v.ob[2] = activeMaskVtx[1].v.ob[2] = activeMaskVtx[2].v.ob[2] = activeMaskVtx[3].v.ob[2] = 0;

    activeMaskVtx[0].v.flag = activeMaskVtx[1].v.flag = activeMaskVtx[2].v.flag = activeMaskVtx[3].v.flag = 0;

    activeMaskVtx[0].v.tc[0] = activeMaskVtx[0].v.tc[1] = activeMaskVtx[1].v.tc[1] = activeMaskVtx[2].v.tc[0] = 0;
    activeMaskVtx[1].v.tc[0] = activeMaskVtx[2].v.tc[1] = activeMaskVtx[3].v.tc[0] = activeMaskVtx[3].v.tc[1] =
        MASK_GRID_SELECTED_QUAD_TEX_SIZE * (1 << 5);

    activeMaskVtx[0].v.cn[0] = activeMaskVtx[1].v.cn[0] = activeMaskVtx[2].v.cn[0] = activeMaskVtx[3].v.cn[0] =
        activeMaskVtx[0].v.cn[1] = activeMaskVtx[1].v.cn[1] = activeMaskVtx[2].v.cn[1] = activeMaskVtx[3].v.cn[1] =
            activeMaskVtx[0].v.cn[2] = activeMaskVtx[1].v.cn[2] = activeMaskVtx[2].v.cn[2] = activeMaskVtx[3].v.cn[2] =
                255;
    activeMaskVtx[0].v.cn[3] = activeMaskVtx[1].v.cn[3] = activeMaskVtx[2].v.cn[3] = activeMaskVtx[3].v.cn[3] =
        pauseCtx->alpha;

    OPEN_DISPS(gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 80, 255, 160, pauseCtx->alpha);
    gSPVertex(POLY_OPA_DISP++, (uintptr_t)activeMaskVtx, 4, 0);
    POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, (TexturePtr)gEquippedItemOutlineTex, 32, 32, 0);

    CLOSE_DISPS(gfxCtx);
}

static void DrawActiveMaskSelection(PauseContext* pauseCtx, u16) {
    if ((gPlayState == nullptr) || (pauseCtx == nullptr) || (pauseCtx->state != PAUSE_STATE_MAIN)) {
        return;
    }

    ItemId equippedMaskItem = GetDisplayedMaskItem();
    s16 slot = GetOwnedMaskSlot(equippedMaskItem);
    if ((equippedMaskItem == ITEM_NONE) || (slot < 0)) {
        return;
    }

    DrawActiveMaskOutline(pauseCtx, slot);
}

static void ProcessPendingMaskEquip(Actor* actor) {
    Player* player = GET_PLAYER(gPlayState);

    if (player == nullptr) {
        return;
    }

    if (actor != &player->actor) {
        return;
    }

    switch (sPendingAction.mode) {
        case PENDING_REGULAR_MASK:
            if (gPlayState->pauseCtx.state == PAUSE_STATE_OFF) {
                ApplyRegularMaskTarget(player, sPendingAction.targetMask);
                ResetPendingAction();
            }
            break;
        case PENDING_RETURN_TO_HUMAN_THEN_REGULAR:
            if (player->transformation == PLAYER_FORM_HUMAN) {
                if (player->currentMask != sPendingAction.targetMask) {
                    ApplyRegularMaskTarget(player, sPendingAction.targetMask, ShouldFastTransform());
                } else {
                    sMaskEquippedWithoutButton =
                        IsRegularMask(sPendingAction.targetMask) ? sPendingAction.targetMask : PLAYER_MASK_NONE;
                    if (ShouldFastTransform()) {
                        PlayAppliedMaskSfx(player, sPendingAction.targetMask);
                    }
                }
                ResetPendingAction();
            }
            break;
        case PENDING_RETURN_TO_HUMAN_THEN_GIANT:
            if (PlayerCanStartFollowUpAction(player)) {
                ItemId giantItem = GetMaskItem(PLAYER_MASK_GIANT);
                PlayerItemAction giantAction = Player_ItemToItemAction(player, giantItem);

                if (!ShouldAllowGiantMask(player, PLAYER_MASK_GIANT) ||
                    !QueueFormChangingMaskAction(player, giantItem, PLAYER_MASK_GIANT, giantAction, PLAYER_MASK_GIANT,
                                                 PENDING_PLAYER_ACTION, false)) {
                    ResetPendingAction();
                }
            }
            break;
        case PENDING_PLAYER_ACTION:
            if (PlayerActionReachedTarget(player)) {
                sMaskEquippedWithoutButton = ShouldMaintainMaskWithoutButton(sPendingAction.targetMask)
                                                 ? sPendingAction.targetMask
                                                 : PLAYER_MASK_NONE;
                if (ShouldFastTransform() && (sPendingAction.targetMask != PLAYER_MASK_GIANT)) {
                    PlayAppliedMaskSfx(player, sPendingAction.targetMask);
                }
                ResetPendingAction();
            }
            break;
        case PENDING_NONE:
            if (sVisualMaskOverride.active &&
                (((sVisualMaskOverride.mask == PLAYER_MASK_NONE) && (player->currentMask == PLAYER_MASK_NONE)) ||
                 ((sVisualMaskOverride.mask != PLAYER_MASK_NONE) &&
                  (player->currentMask == sVisualMaskOverride.mask)))) {
                ClearVisualMaskOverride();
            }
            break;
    }

    if ((sMaskEquippedWithoutButton != PLAYER_MASK_NONE) && (player->currentMask != sMaskEquippedWithoutButton)) {
        sMaskEquippedWithoutButton = PLAYER_MASK_NONE;
    }
}

static void PreventButtonlessMaskUnequip(bool* should, va_list args) {
    Player* player = va_arg(args, Player*);
    s32* button = va_arg(args, s32*);

    if (*should && (player != nullptr) && (player->transformation == PLAYER_FORM_HUMAN) &&
        (player->currentMask == sMaskEquippedWithoutButton) &&
        ShouldMaintainMaskWithoutButton(sMaskEquippedWithoutButton)) {
        *should = false;
        *button = EQUIP_SLOT_C_LEFT;
    }
}

static void SuppressDefaultActiveMaskOutline(bool* should, va_list args) {
    if ((gPlayState == nullptr) || !*should) {
        return;
    }

    ItemId* item = va_arg(args, ItemId*);
    va_arg(args, s32); // slot
    va_arg(args, s32); // isDpad
    s32 pageIndex = va_arg(args, s32);
    ItemId equippedMaskItem = GetDisplayedMaskItem();

    if (pageIndex != PAUSE_MASK) {
        return;
    }

    if ((equippedMaskItem != ITEM_NONE) && (*item == equippedMaskItem)) {
        *should = false;
        return;
    }

    if (HasPendingAction() && (sPendingAction.targetMask == PLAYER_MASK_NONE) &&
        (*item == GetMaskItem(sPendingAction.selectedMask))) {
        *should = false;
    }
}

static bool ShouldAllowRegularMask(PlayerMask* mask) {
    if (GameInteractor_Should(VB_USE_ITEM_EQUIP_MASK, true, mask)) {
        return true;
    }

    Audio_PlaySfx(NA_SE_SY_ERROR);
    return false;
}

static bool QueueRegularMaskTarget(Player* player, PlayerMask selectedMask, PlayerMask targetMask,
                                   PlayerMask maskForEquipHook) {
    if (IsRegularMask(maskForEquipHook) && !ShouldAllowRegularMask(&maskForEquipHook)) {
        return false;
    }

    SavePendingSnapshot(player);
    sPendingAction.mode = PENDING_REGULAR_MASK;
    sPendingAction.selectedMask = selectedMask;
    sPendingAction.targetMask = targetMask;
    sPendingAction.itemAction = static_cast<PlayerItemAction>(GET_IA_FROM_MASK(selectedMask));

    SetVisualMaskOverride(sPendingAction.targetMask);
    Audio_PlaySfx(NA_SE_SY_DECIDE);
    return true;
}

static bool QueueNoMask(Player* player, PlayerMask selectedMask) {
    if (player == nullptr) {
        return false;
    }

    PlayerMask currentMask = static_cast<PlayerMask>(player->currentMask);
    if ((player->transformation == PLAYER_FORM_HUMAN) && (currentMask == PLAYER_MASK_NONE)) {
        ResetPendingAction();
        Audio_PlaySfx(NA_SE_SY_MESSAGE_PASS);
        return true;
    }

    if ((player->transformation == PLAYER_FORM_HUMAN) && IsRegularMask(currentMask)) {
        return QueueRegularMaskTarget(player, selectedMask, PLAYER_MASK_NONE, currentMask);
    }

    ItemId item = GetMaskItem(currentMask);
    PlayerItemAction itemAction = Player_ItemToItemAction(player, item);
    if ((item == ITEM_NONE) || (itemAction < PLAYER_IA_MASK_GIANT) || (itemAction > PLAYER_IA_MASK_DEKU)) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
        return false;
    }

    return QueueFormChangingMaskAction(player, item, selectedMask, itemAction, PLAYER_MASK_NONE, PENDING_PLAYER_ACTION);
}

static bool QueueReturnToHumanThenRegular(Player* player, ItemId item, PlayerMask mask, PlayerItemAction itemAction) {
    PlayerMask maskToEquip = mask;

    if (!ShouldAllowRegularMask(&maskToEquip)) {
        return false;
    }

    if (!QueueFormChangingMaskAction(player, item, maskToEquip, itemAction, maskToEquip,
                                     PENDING_RETURN_TO_HUMAN_THEN_REGULAR)) {
        return false;
    }

    gSaveContext.save.equippedMask = maskToEquip;
    return true;
}

static bool QueueReturnToHumanThenGiant(Player* player) {
    if (!ShouldAllowGiantMask(player, PLAYER_MASK_GIANT)) {
        return false;
    }

    PlayerMask currentMask = static_cast<PlayerMask>(player->currentMask);
    ItemId returnItem = GetMaskItem(currentMask);
    PlayerItemAction returnAction = Player_ItemToItemAction(player, returnItem);

    if ((returnItem == ITEM_NONE) || (returnAction < PLAYER_IA_MASK_FIERCE_DEITY) ||
        (returnAction > PLAYER_IA_MASK_DEKU)) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
        return false;
    }

    return QueueFormChangingMaskAction(player, returnItem, PLAYER_MASK_GIANT, returnAction, PLAYER_MASK_GIANT,
                                       PENDING_RETURN_TO_HUMAN_THEN_GIANT);
}

static bool QueueTransformationMask(Player* player, ItemId item, PlayerMask mask, PlayerItemAction itemAction,
                                    PlayerMask targetMask) {
    if (!ShouldAllowFierceDeityMask(targetMask) || !ShouldAllowGiantMask(player, targetMask)) {
        return false;
    }

    return QueueFormChangingMaskAction(player, item, mask, itemAction, targetMask, PENDING_PLAYER_ACTION);
}

static void HandleMaskPageSelection(bool* should, va_list args) {
    if (!*should || (gPlayState == nullptr)) {
        return;
    }

    ItemId item = static_cast<ItemId>(*va_arg(args, u16*));
    if (!IsMaskItem(item) || ShouldDeferToPersistentBunnyHood(item)) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    PlayerMask mask = GetMaskFromItem(player, item);
    PlayerItemAction itemAction = Player_ItemToItemAction(player, item);
    PlayerMask intendedMask =
        HasPendingAction() ? sPendingAction.targetMask : static_cast<PlayerMask>(player->currentMask);
    PlayerMask targetMask = (intendedMask == mask) ? PLAYER_MASK_NONE : mask;

    *should = false;

    EasyMaskEquipStateSnapshot previousState = CaptureEasyMaskEquipState(player);

    if (HasPendingAction()) {
        CancelPendingAction(player);
    }

    if ((player->currentMask == PLAYER_MASK_GIANT) && (mask != PLAYER_MASK_GIANT)) {
        Audio_PlaySfx(NA_SE_SY_ERROR);
        ResetOrRestorePreviousPendingAction(player, previousState);
        return;
    }

    if (targetMask == PLAYER_MASK_NONE) {
        if (!QueueNoMask(player, mask)) {
            ResetOrRestorePreviousPendingAction(player, previousState);
        }
        return;
    }

    if (IsRegularMask(targetMask)) {
        bool queued;

        if (player->transformation == PLAYER_FORM_HUMAN) {
            queued = QueueRegularMaskTarget(player, mask, targetMask, targetMask);
        } else {
            queued = QueueReturnToHumanThenRegular(player, item, targetMask, itemAction);
        }

        if (!queued) {
            ResetOrRestorePreviousPendingAction(player, previousState);
        }
        return;
    }

    if ((targetMask == PLAYER_MASK_GIANT) && (player->transformation != PLAYER_FORM_HUMAN)) {
        if (!QueueReturnToHumanThenGiant(player)) {
            ResetOrRestorePreviousPendingAction(player, previousState);
        }
        return;
    }

    if (!QueueTransformationMask(player, item, mask, itemAction, targetMask)) {
        ResetOrRestorePreviousPendingAction(player, previousState);
    }
}

void RegisterEasyMaskEquip() {
    if (!CVAR) {
        Player* player = (gPlayState != nullptr) ? GET_PLAYER(gPlayState) : nullptr;

        RestoreAButtonPrompt();
        CancelPendingAction(player);
        sMaskEquippedWithoutButton = PLAYER_MASK_NONE;
    }

    COND_HOOK(OnKaleidoUpdate, CVAR, UpdateMaskPageAButtonPrompt);
    COND_ID_HOOK(BeforeKaleidoDrawPage, PAUSE_MASK, CVAR, DrawActiveMaskSelection);
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, CVAR, ProcessPendingMaskEquip);
    COND_VB_SHOULD(VB_DRAW_ITEM_EQUIPPED_OUTLINE, CVAR, SuppressDefaultActiveMaskOutline(should, args));
    COND_VB_SHOULD(VB_UNEQUIP_MASK_NOT_ON_BUTTON, CVAR, PreventButtonlessMaskUnequip(should, args));
    COND_VB_SHOULD(VB_KALEIDO_DISPLAY_ITEM_TEXT, CVAR, HandleMaskPageSelection(should, args));
}

static RegisterShipInitFunc initFunc(RegisterEasyMaskEquip, { CVAR_NAME, CVAR_PERSISTENT_BUNNY_HOOD_NAME });
