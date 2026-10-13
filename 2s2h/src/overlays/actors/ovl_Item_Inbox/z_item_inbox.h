#ifndef Z_ITEM_INBOX_H
#define Z_ITEM_INBOX_H

#include "global.h"

struct ItemInbox;

typedef void (*ItemInboxActionFunc)(struct ItemInbox*, PlayState*);

typedef struct ItemInbox {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ItemInboxActionFunc actionFunc;
} ItemInbox; // size = 0x148

#endif // Z_ITEM_INBOX_H
