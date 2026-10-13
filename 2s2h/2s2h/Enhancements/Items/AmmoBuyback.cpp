#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "message_data_fmt_nes.h"

extern "C" {
#include "overlays/actors/ovl_En_Fsn/z_en_fsn.h"

void EnFsn_MakeOffer(EnFsn* thisx, PlayState* play);
void EnFsn_GiveItem(EnFsn* thisx, PlayState* play);
void EnFsn_StartBuying(EnFsn* thisx, PlayState* play);
void EnFsn_ResumeInteraction(EnFsn* enFsn, PlayState* play);
void Player_StartTalking(PlayState* play, Actor* actor);
void Player_StopCutscene(Player* player);
}

#define CVAR_NAME "gEnhancements.Items.AmmoBuyback"
#define CVAR CVarGetInteger(CVAR_NAME, AMMO_BUYBACK_VANILLA)

#define TEXT_ID_FSN_REJECTION 0x29CF // "Sorry, but I can't sell that here."
#define TEXT_ID_FSN_OFFER 0x29EF     // Price offer dialogue
#define MAX_AMMO_SELL_QUANTITY 99
#define STICK_THRESHOLD 30
#define REPEAT_DELAY_INITIAL 10
#define REPEAT_DELAY_CONTINUOUS 2

static struct {
    EnFsn* actor = nullptr;
    ItemId itemId = ITEM_NONE;
    s16 quantity = 0;
    s16 price = 0;
    s16 lastRupeesSelected = 0;
    bool isInputActive = false;
} sAmmoSale;

static void Reset() {
    sAmmoSale = {};
}

static s16 GetPricePerUnit(ItemId itemId) {
    switch (itemId) {
        case ITEM_BOW:
            return 1;
        case ITEM_BOMB:
            return 3;
        case ITEM_BOMBCHU:
            return 4;
        case ITEM_DEKU_STICK:
            return 10;
        case ITEM_DEKU_NUT:
            return 3;
        case ITEM_MAGIC_BEANS:
            return 10;
        case ITEM_POWDER_KEG:
            return 50;
        case ITEM_SEAHORSE:
            return 50;
        default:
            return 0;
    }
}

static const char* GetNameForDialogue(ItemId itemId) {
    switch (itemId) {
        case ITEM_BOW:
            return "%rArrows%w";
        case ITEM_BOMB:
            return "%rBombs%w";
        case ITEM_BOMBCHU:
            return "%rBombchus%w";
        case ITEM_DEKU_STICK:
            return "%rDeku Sticks%w";
        case ITEM_DEKU_NUT:
            return "%rDeku Nuts%w";
        case ITEM_MAGIC_BEANS:
            return "%rMagic Beans%w";
        case ITEM_POWDER_KEG:
            return "%rPowder Kegs%w";
        case ITEM_SEAHORSE:
            return "%rSeahorse%w";
        default:
            return "%ritems%w";
    }
}

static s16 GetSalePrice(ItemId itemId, s16 count) {
    s16 totalPrice = count * GetPricePerUnit(itemId);
    if (CVAR == AMMO_BUYBACK_HALF_PRICE && itemId != ITEM_BOW) {
        totalPrice = (totalPrice + 1) / 2;
    }
    return totalPrice;
}

static bool IsItemAvailable(ItemId itemId) {
    if (itemId == ITEM_POWDER_KEG) {
        return INV_CONTENT(ITEM_POWDER_KEG) == ITEM_POWDER_KEG && AMMO(ITEM_POWDER_KEG) > 0;
    }
    if (itemId == ITEM_MAGIC_BEANS) {
        return INV_CONTENT(ITEM_MAGIC_BEANS) == ITEM_MAGIC_BEANS && AMMO(ITEM_MAGIC_BEANS) > 0;
    }
    if (itemId == ITEM_SEAHORSE) {
        return true;
    }
    return Item_CheckObtainability(itemId) != ITEM_NONE;
}

static void UpdateDigits(PlayState* play, s16 value) {
    if (play->msgCtx.unk120C0 == 0)
        return;

    char digits[3];
    // Layout: "XX0" where 3rd digit is hidden.
    // Forces left alignment and matches internal *10 calc.
    digits[0] = ((value / 10) % 10) + '0';
    digits[1] = (value % 10) + '0';
    digits[2] = '0';

    // Writes digits and extra space to decodedBuffer
    play->msgCtx.decodedBuffer.schar[play->msgCtx.unk120C0] = digits[0];
    play->msgCtx.decodedBuffer.schar[play->msgCtx.unk120C0 + 1] = digits[1];
    play->msgCtx.decodedBuffer.schar[play->msgCtx.unk120C0 + 2] = digits[2];
    play->msgCtx.decodedBuffer.schar[play->msgCtx.unk120C0 + 3] = ' ';

    // Render: [Tens] [Units] [Space]
    Font_LoadCharNES(play, digits[0], play->msgCtx.unk120C4);
    Font_LoadCharNES(play, digits[1], play->msgCtx.unk120C4 + (1 << 7));
    Font_LoadCharNES(play, ' ', play->msgCtx.unk120C4 + (2 << 7));
}

static bool UpdateInputState(bool isActive, bool* isHeld, s16* timer) {
    if (!isActive) {
        *isHeld = false;
        *timer = 0;
        return false;
    }
    if (!*isHeld) {
        *isHeld = true;
        *timer = REPEAT_DELAY_INITIAL;
        return true;
    }
    if (*timer > 0) {
        (*timer)--;
        return false;
    }
    *timer = REPEAT_DELAY_CONTINUOUS;
    return true;
}

static void HandleStart(EnFsn* enFsn, PlayState* play) {
    if (enFsn->price != 0 || enFsn->actor.textId != TEXT_ID_FSN_REJECTION)
        return;

    Player* player = GET_PLAYER(play);
    ItemId buttonItem = (ItemId)(IS_HELD_DPAD(player->heldItemButton)
                                     ? DPAD_GET_CUR_FORM_BTN_ITEM(HELD_ITEM_TO_DPAD(player->heldItemButton))
                                     : GET_CUR_FORM_BTN_ITEM(player->heldItemButton));

    if (GetPricePerUnit(buttonItem) > 0 && IsItemAvailable(buttonItem) &&
        (buttonItem == ITEM_SEAHORSE || AMMO(buttonItem) > 0)) {
        sAmmoSale.actor = enFsn;
        sAmmoSale.itemId = buttonItem;
        sAmmoSale.lastRupeesSelected = 10; // Default for input mode

        // If player only has 1, skip inputs
        s16 maxAllowed = (buttonItem == ITEM_SEAHORSE) ? 1 : MIN(AMMO(buttonItem), MAX_AMMO_SELL_QUANTITY);

        if (maxAllowed == 1) {
            sAmmoSale.quantity = 1;
            sAmmoSale.price = GetSalePrice(buttonItem, 1);
            sAmmoSale.isInputActive = false;
            enFsn->price = sAmmoSale.price;
        } else {
            sAmmoSale.quantity = 1;
            sAmmoSale.price = 0;
            sAmmoSale.isInputActive = true;
            play->msgCtx.rupeesSelected = 10;
        }

        enFsn->actionFunc = EnFsn_MakeOffer;
        player->actor.textId = TEXT_ID_FSN_OFFER;
    }
}

static void HandleInput(EnFsn* enFsn, PlayState* play) {
    static bool sStickHeldX = false, sStickHeldY = false;
    static s16 sRepeatTimerX = 0, sRepeatTimerY = 0;

    MessageContext* msgCtx = &play->msgCtx;
    Input* input = &play->state.input[0];

    // Prevent input processing if the message system hasn't loaded our text yet
    if (msgCtx->currentTextId != TEXT_ID_FSN_OFFER || msgCtx->unk120C0 == 0 || msgCtx->rupeesSelected < 10 ||
        msgCtx->msgMode == MSGMODE_NONE || msgCtx->msgMode == MSGMODE_TEXT_START ||
        msgCtx->msgMode == MSGMODE_TEXT_BOX_GROWING || msgCtx->msgMode == MSGMODE_TEXT_STARTING) {
        return;
    }

    s16 currentQuantity = (s16)(msgCtx->rupeesSelected / 10);
    s16 newQuantity = currentQuantity;
    s16 maxAllowed = (sAmmoSale.itemId == ITEM_SEAHORSE) ? 1 : MIN(AMMO(sAmmoSale.itemId), MAX_AMMO_SELL_QUANTITY);

    // Enforce cursor on visible digits (0=Tens, 1=Units)
    if (msgCtx->unk120C2 > 1)
        msgCtx->unk120C2 = 1;

    // Vertical Input (Quantity)
    bool inputUp = CHECK_BTN_ALL(input->cur.button, BTN_DUP) || input->rel.stick_y >= STICK_THRESHOLD;
    bool inputDown = CHECK_BTN_ALL(input->cur.button, BTN_DDOWN) || input->rel.stick_y <= -STICK_THRESHOLD;
    s16 delta = (msgCtx->unk120C2 == 0) ? 10 : 1;

    if (inputUp) {
        if (UpdateInputState(true, &sStickHeldY, &sRepeatTimerY))
            newQuantity += delta;
    } else if (inputDown) {
        if (UpdateInputState(true, &sStickHeldY, &sRepeatTimerY))
            newQuantity -= delta;
    } else {
        UpdateInputState(false, &sStickHeldY, &sRepeatTimerY);
    }

    // Horizontal Input (Cursor)
    bool inputRight = CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT) || input->rel.stick_x >= STICK_THRESHOLD;
    bool inputLeft = CHECK_BTN_ALL(input->cur.button, BTN_DLEFT) || input->rel.stick_x <= -STICK_THRESHOLD;

    if (inputRight) {
        if (UpdateInputState(true, &sStickHeldX, &sRepeatTimerX)) {
            msgCtx->unk120C2 = (msgCtx->unk120C2 >= 1) ? 1 : msgCtx->unk120C2 + 1;
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else if (inputLeft) {
        if (UpdateInputState(true, &sStickHeldX, &sRepeatTimerX)) {
            msgCtx->unk120C2 = (msgCtx->unk120C2 <= 0) ? 0 : msgCtx->unk120C2 - 1;
            Audio_PlaySfx(NA_SE_SY_CURSOR);
        }
    } else {
        UpdateInputState(false, &sStickHeldX, &sRepeatTimerX);
    }

    // Shortcuts
    if (CHECK_BTN_ALL(input->press.button, BTN_Z))
        newQuantity = maxAllowed;
    if (CHECK_BTN_ALL(input->press.button, BTN_R))
        newQuantity = 1;

    // Cleanup & Update
    input->press.button &= ~(BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT);
    input->cur.button &= ~(BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT);
    input->rel.stick_x = 0;
    input->rel.stick_y = 0;

    newQuantity = CLAMP(newQuantity, 1, maxAllowed);

    if (newQuantity != currentQuantity) {
        msgCtx->rupeesSelected = newQuantity * 10;
        UpdateDigits(play, newQuantity);
    }

    if (newQuantity != (sAmmoSale.lastRupeesSelected / 10)) {
        Audio_PlaySfx(NA_SE_SY_FSEL_CURSOR);
        sAmmoSale.lastRupeesSelected = newQuantity * 10;
    }

    if (Message_ShouldAdvance(play)) {
        sAmmoSale.quantity = newQuantity;
        sAmmoSale.price = GetSalePrice(sAmmoSale.itemId, newQuantity);

        enFsn->price = sAmmoSale.price;
        sAmmoSale.isInputActive = false;
        Message_StartTextbox(play, TEXT_ID_FSN_OFFER, &enFsn->actor);
    }
}

static void Resolve(EnFsn* enFsn, bool* shouldGiveItem) {
    if (enFsn->actionFunc == EnFsn_GiveItem) {
        *shouldGiveItem = false;

        if (sAmmoSale.itemId == ITEM_SEAHORSE) {
            Inventory_ReplaceItem(gPlayState, ITEM_SEAHORSE, ITEM_BOTTLE);
        } else {
            Inventory_ChangeAmmo(sAmmoSale.itemId, -sAmmoSale.quantity);
        }
        Rupees_ChangeBy(sAmmoSale.price);

        enFsn->actor.parent = nullptr;
        enFsn->actor.flags |= ACTOR_FLAG_TALK;
        enFsn->actor.textId = 0;
        enFsn->actionFunc = EnFsn_ResumeInteraction;

        Player* player = GET_PLAYER(gPlayState);
        player->talkActor = &enFsn->actor;
        player->talkActorDistance = enFsn->actor.xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;
        player->getItemDrawIdPlusOne = 0;
        player->getItemId = GI_NONE;
        player->interactRangeActor = nullptr;

        Player_StopCutscene(player);
        Player_StartTalking(gPlayState, &enFsn->actor);

        Reset();
    }
}

static void HandleOfferResponse(EnFsn* enFsn, PlayState* play) {
    if (sAmmoSale.isInputActive)
        return;

    // Guard against these text states to prevent premature Reset
    if (play->msgCtx.msgMode == MSGMODE_NONE || play->msgCtx.msgMode == MSGMODE_TEXT_START ||
        play->msgCtx.msgMode == MSGMODE_TEXT_BOX_GROWING || play->msgCtx.msgMode == MSGMODE_TEXT_STARTING) {
        return;
    }

    u8 talkState = Message_GetState(&play->msgCtx);

    // If choice was made (not 0/Yes), or text ended, reset
    if ((talkState == TEXT_STATE_CHOICE && Message_ShouldAdvance(play) && play->msgCtx.choiceIndex != 0) ||
        talkState == TEXT_STATE_NONE) {
        Reset();
    }
}

static void DrawAmmoSelectionDigits() {
    if (!sAmmoSale.isInputActive)
        return;

    PlayState* play = gPlayState;
    if (play->msgCtx.rupeesSelected < 10) {
        play->msgCtx.rupeesSelected = 10;
        sAmmoSale.lastRupeesSelected = 10;
    }
    UpdateDigits(play, play->msgCtx.rupeesSelected / 10);
}

static void GenerateBuybackDialogue(u16* textId, bool* loadFromMessageTable) {
    if (!sAmmoSale.actor)
        return;

    if (sAmmoSale.isInputActive) {
        // Custom Input Textbox
        CustomMessage::Entry entry;
        entry.textboxType = 6;
        entry.textboxYPos = 1;
        entry.msg = (char)MESSAGE_QUICKTEXT_ENABLE;
        entry.msg += std::string("How many ") + GetNameForDialogue(sAmmoSale.itemId) + "? " + (char)MESSAGE_NEWLINE +
                     (char)MESSAGE_INPUT_BANK + (char)MESSAGE_NEWLINE + "Set the amount with " +
                     (char)MESSAGE_CONTROL_PAD + " and" + (char)MESSAGE_NEWLINE + "press " + (char)MESSAGE_BTN_A +
                     " to decide." + (char)MESSAGE_END;

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    } else {
        // Offer Textbox with Price Injection
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        CustomMessage::Replace(&entry.msg, std::string(1, (char)MESSAGE_HELD_ITEM_PRICE),
                               std::to_string(sAmmoSale.price) + " Rupees");
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    }
}

static void UpdateBuybackInteraction(Actor* actor) {
    EnFsn* enFsn = (EnFsn*)actor;
    PlayState* play = gPlayState;

    if (enFsn->actionFunc == EnFsn_StartBuying) {
        HandleStart(enFsn, play);
        return;
    }
    if (sAmmoSale.actor != enFsn)
        return;
    if (sAmmoSale.isInputActive)
        HandleInput(enFsn, play);
    if (enFsn->actionFunc == EnFsn_MakeOffer)
        HandleOfferResponse(enFsn, play);
}

static void OnFsnDestroy(Actor* actor) {
    if (sAmmoSale.actor == (EnFsn*)actor)
        Reset();
}

static void BlockAmmoBuybackInput() {
    if (!gPlayState || (GameState*)gPlayState != gGameState)
        return;

    if (sAmmoSale.isInputActive) {
        Input* input = &gPlayState->state.input[0];
        input->press.button &= ~(BTN_B | BTN_CUP);
        input->cur.button &= ~(BTN_B | BTN_CUP);
    }
}

static void RegisterAmmoBuyback() {
    COND_HOOK(OnInterfaceDrawStart, CVAR != AMMO_BUYBACK_VANILLA, DrawAmmoSelectionDigits);
    COND_ID_HOOK(OnOpenText, TEXT_ID_FSN_OFFER, CVAR != AMMO_BUYBACK_VANILLA, GenerateBuybackDialogue);
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_FSN, CVAR != AMMO_BUYBACK_VANILLA, UpdateBuybackInteraction);
    COND_ID_HOOK(OnActorDestroy, ACTOR_EN_FSN, CVAR != AMMO_BUYBACK_VANILLA, OnFsnDestroy);
    COND_HOOK(OnGameStateMainStart, CVAR != AMMO_BUYBACK_VANILLA, BlockAmmoBuybackInput);

    COND_VB_SHOULD(VB_MSG_LOAD_RUPEES_TEXT, CVAR != AMMO_BUYBACK_VANILLA, {
        if (sAmmoSale.isInputActive)
            *should = false;
    });

    COND_VB_SHOULD(VB_MSG_PLAY_INPUT_COUNT_SOUND, sAmmoSale.isInputActive, { *should = false; });

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, CVAR != AMMO_BUYBACK_VANILLA, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* actor = va_arg(args, Actor*);
        if (sAmmoSale.actor != nullptr && sAmmoSale.actor == (EnFsn*)actor) {
            Resolve((EnFsn*)actor, should);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterAmmoBuyback, { CVAR_NAME });
