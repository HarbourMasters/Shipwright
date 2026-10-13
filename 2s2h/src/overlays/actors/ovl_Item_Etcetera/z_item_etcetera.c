/*
 * File: z_item_etcetera.c
 * Overlay: ovl_Item_Etcetera
 * Description: Leftover OoT Collectible Items (bottle, key, Fire Arrow, etc.)
 */

#include "z_item_etcetera.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ItemEtcetera_Init(Actor* thisx, PlayState* play);
void ItemEtcetera_Destroy(Actor* thisx, PlayState* play);
void ItemEtcetera_Update(Actor* thisx, PlayState* play);

void ItemEtcetera_WaitForObject(ItemEtcetera* this, PlayState* play);
void func_8092009C(ItemEtcetera* this, PlayState* play);
void func_809200F8(ItemEtcetera* this, PlayState* play);
void ItemEtcetera_DrawThroughLens(Actor* thisx, PlayState* play);
void ItemEtcetera_Draw(Actor* thisx, PlayState* play);

ActorProfile Item_Etcetera_Profile = {
    /**/ ACTOR_ITEM_ETCETERA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ItemEtcetera),
    /**/ ItemEtcetera_Init,
    /**/ ItemEtcetera_Destroy,
    /**/ ItemEtcetera_Update,
    /**/ NULL,
};

static s16 sObjectIds[] = {
    OBJECT_GI_BOTTLE, OBJECT_GI_BOTTLE, OBJECT_GI_BOTTLE,  OBJECT_GI_BOTTLE, OBJECT_GI_BOTTLE,
    OBJECT_GI_BOTTLE, OBJECT_GI_KEY,    OBJECT_GI_M_ARROW, OBJECT_GI_RUPY,   OBJECT_GI_RUPY,
    OBJECT_GI_RUPY,   OBJECT_GI_RUPY,   OBJECT_GI_HEARTS,  OBJECT_GI_KEY,
};
static s16 sGetItemDrawIds[] = {
    GID_BOTTLE,     GID_BOTTLE,      GID_BOTTLE,     GID_BOTTLE,    GID_BOTTLE,       GID_BOTTLE,      GID_KEY_SMALL,
    GID_ARROW_FIRE, GID_RUPEE_GREEN, GID_RUPEE_BLUE, GID_RUPEE_RED, GID_RUPEE_PURPLE, GID_HEART_PIECE, GID_KEY_SMALL,
};

static s16 sGetItemIds[] = {
    GI_BOTTLE,     GI_BOTTLE, GI_BOTTLE, GI_BOTTLE, GI_BOTTLE, GI_BOTTLE, GI_KEY_SMALL,
    GI_ARROW_FIRE, GI_NONE,   GI_NONE,   GI_NONE,   GI_NONE,   GI_NONE,   GI_NONE,
};

void ItemEtcetera_SetupAction(ItemEtcetera* this, ItemEtceteraActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void ItemEtcetera_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ItemEtcetera* this = (ItemEtcetera*)thisx;
    s32 type = ITEMETCETERA_GET_FF(&this->actor);
    s32 objectSlot = Object_GetSlot(&play->objectCtx, sObjectIds[type]);

    if (objectSlot <= OBJECT_SLOT_NONE) {
        // assert on debug
    } else {
        this->objectSlot = objectSlot;
    }
    this->getItemDrawId = sGetItemDrawIds[type];
    this->getItemId = sGetItemIds[type];
    this->futureActionFunc = func_8092009C;
    this->drawFunc = ItemEtcetera_Draw;
    Actor_SetScale(&this->actor, 0.25f);
    ItemEtcetera_SetupAction(this, ItemEtcetera_WaitForObject);
    switch (type) {
        case ITEM_ETC_ARROW_FIRE:
            Actor_SetScale(&this->actor, 0.5f);
            this->actor.draw = NULL;
            this->actor.shape.yOffset = 50.0f;
            break;
        case ITEM_ETC_RUPEE_GREEN_CHEST_GAME:
        case ITEM_ETC_RUPEE_BLUE_CHEST_GAME:
        case ITEM_ETC_RUPEE_RED_CHEST_GAME:
        case ITEM_ETC_RUPEE_PURPLE_CHEST_GAME:
        case ITEM_ETC_HEART_PIECE_CHEST_GAME:
        case ITEM_ETC_KEY_SMALL_CHEST_GAME:
            Actor_SetScale(&this->actor, 0.5f);
            this->futureActionFunc = func_809200F8;
            this->drawFunc = ItemEtcetera_DrawThroughLens;
            this->actor.world.pos.y += 15.0f;
        default:
            break;
    }
}

void ItemEtcetera_Destroy(Actor* thisx, PlayState* play) {
}

void ItemEtcetera_WaitForObject(ItemEtcetera* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        this->actor.draw = this->drawFunc;
        this->actionFunc = this->futureActionFunc;
    }
}

void func_8092009C(ItemEtcetera* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        Actor_Kill(&this->actor);
        return;
    }

    Actor_OfferGetItem(&this->actor, play, this->getItemId, 30.0f, 50.0f);
}

void func_809200F8(ItemEtcetera* this, PlayState* play) {
    if (Flags_GetTreasure(play, ITEMETCETERA_GET_TREASUREFLAG(&this->actor))) {
        Actor_Kill(&this->actor);
    }
}

void ItemEtcetera_Update(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;

    this->actionFunc(this, play);
}

void ItemEtcetera_DrawThroughLens(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;

    if (play->actorCtx.lensMaskSize == LENS_MASK_ACTIVE_SIZE) {
        func_800B8050(&this->actor, play, 0);
        func_800B8118(&this->actor, play, 0);
        GetItem_Draw(play, this->getItemDrawId);
    }
}

void ItemEtcetera_Draw(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;

    func_800B8050(&this->actor, play, 0);
    func_800B8118(&this->actor, play, 0);
    GetItem_Draw(play, this->getItemDrawId);
}
