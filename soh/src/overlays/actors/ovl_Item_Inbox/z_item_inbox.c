/*
 * File: z_item_inbox.c
 * Overlay: ovl_Item_Inbox
 * Description: Zelda's magic effect when opening gates in castle collapse
 */

#include "z_item_inbox.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void ItemInbox_Init(Actor* thisx, GlobalContext* globalCtx);
void ItemInbox_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ItemInbox_Update(Actor* thisx, GlobalContext* globalCtx);
void ItemInbox_Draw(Actor* thisx, GlobalContext* globalCtx);

void ItemInbox_Wait(ItemInbox* this, GlobalContext* globalCtx);

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

void ItemInbox_Init(Actor* thisx, GlobalContext* globalCtx) {
    ItemInbox* this = (ItemInbox*)thisx;

    this->actionFunc = ItemInbox_Wait;
    Actor_SetScale(&this->actor, 0.2);
}

void ItemInbox_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void ItemInbox_Wait(ItemInbox* this, GlobalContext* globalCtx) {
    if (Flags_GetTreasure(globalCtx, (this->actor.params >> 8) & 0x1F)) {
        Actor_Kill(&this->actor);
    }
}

void ItemInbox_Update(Actor* thisx, GlobalContext* globalCtx) {
    ItemInbox* this = (ItemInbox*)thisx;

    this->actionFunc(this, globalCtx);
}

void ItemInbox_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ItemInbox* this = (ItemInbox*)thisx;

    func_8002EBCC(&this->actor, globalCtx, 0);
    func_8002ED80(&this->actor, globalCtx, 0);
    GetItem_Draw(globalCtx, this->actor.params & 0xFF);
}
