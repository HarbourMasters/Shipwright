/*
 * File: z_item_inbox.c
 * Overlay: ovl_Item_Inbox
 * Description: Zelda's magic effect when opening gates in castle collapse
 */

#include "z_item_inbox.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void ItemInbox_Init(Actor* thisx, PlayState* play);
void ItemInbox_Destroy(Actor* thisx, PlayState* play);
void ItemInbox_Update(Actor* thisx, PlayState* play);
void ItemInbox_Draw(Actor* thisx, PlayState* play);

void ItemInbox_Wait(ItemInbox* this, PlayState* play);

const ActorInit Item_Inbox_InitVars = {
    ACTOR_ITEM_INBOX,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ItemInbox),
    (ActorFunc)ItemInbox_Init,
    (ActorFunc)ItemInbox_Destroy,
    (ActorFunc)ItemInbox_Update,
    (ActorFunc)ItemInbox_Draw,
    NULL,
};

void ItemInbox_Init(Actor* thisx, PlayState* play) {
    ItemInbox* this = (ItemInbox*)thisx;

    this->actionFunc = ItemInbox_Wait;
    Actor_SetScale(&this->actor, 0.2);
}

void ItemInbox_Destroy(Actor* thisx, PlayState* play) {
}

void ItemInbox_Wait(ItemInbox* this, PlayState* play) {
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

    func_8002EBCC(&this->actor, play, 0);
    func_8002ED80(&this->actor, play, 0);
    GetItem_Draw(play, this->actor.params & 0xFF);
}
