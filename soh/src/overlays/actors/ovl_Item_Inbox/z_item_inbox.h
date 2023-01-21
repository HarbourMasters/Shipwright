#ifndef Z_ITEM_INBOX_H
#define Z_ITEM_INBOX_H

#include <libultraship/libultra.h>
#include "global.h"

struct ItemInbox;

typedef void (*ItemInboxActionFunc)(struct ItemInbox*, PlayState*);

typedef struct ItemInbox {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ItemInboxActionFunc actionFunc;
} ItemInbox; // size = 0x0150

#endif
