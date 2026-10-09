/*
 * File: z_item_inbox.c
 * Overlay: ovl_Item_Inbox
 * Description: Unused, can draw GetItem models. Perhaps intended to draw items inside chests.
 */

#include "z_item_inbox.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void ItemInbox_Init(Actor* thisx, PlayState* play);
void ItemInbox_Destroy(Actor* thisx, PlayState* play);
void ItemInbox_Update(Actor* thisx, PlayState* play);
void ItemInbox_Draw(Actor* thisx, PlayState* play);

void ItemInbox_Idle(ItemInbox* this, PlayState* play);

ActorProfile Item_Inbox_Profile = {
    /**/ ACTOR_ITEM_INBOX,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ItemInbox),
    /**/ ItemInbox_Init,
    /**/ ItemInbox_Destroy,
    /**/ ItemInbox_Update,
    /**/ ItemInbox_Draw,
};

void ItemInbox_Init(Actor* thisx, PlayState* play) {
    ItemInbox* this = (ItemInbox*)thisx;

    this->actionFunc = ItemInbox_Idle;
    Actor_SetScale(&this->actor, 0.2f);
}

void ItemInbox_Destroy(Actor* thisx, PlayState* play) {
}

void ItemInbox_Idle(ItemInbox* this, PlayState* play) {
    if (Flags_GetTreasure(play, (this->actor.params >> 8) & 0x1F)) {
        Actor_Kill(&this->actor);
    }
}

void ItemInbox_Update(Actor* thisx, PlayState* play) {
    ItemInbox* this = (ItemInbox*)thisx;

    this->actionFunc(this, play);
}

void ItemInbox_Draw(Actor* thisx, PlayState* play) {
    ItemInbox* this = (ItemInbox*)thisx;

    func_800B8050(&this->actor, play, 0);
    func_800B8118(&this->actor, play, 0);
    GetItem_Draw(play, this->actor.params & 0xFF);
}
