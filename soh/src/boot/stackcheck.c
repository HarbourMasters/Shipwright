#include "global.h"
#include "vt.h"

StackEntry* sStackInfoListStart = NULL;
StackEntry* sStackInfoListEnd = NULL;

void StackCheck_Init(StackEntry* entry, void* stackTop, void* stackBottom, u32 initValue, s32 minSpace,
                     const char* name) {
    StackEntry* iter;
    u32* addr;

    if (entry == NULL) {
        sStackInfoListStart = NULL;
    } else {
        entry->head = (uintptr_t)stackTop;
        entry->tail = (uintptr_t)stackBottom;
        entry->initValue = initValue;
        entry->minSpace = minSpace;
        entry->name = name;
        iter = sStackInfoListStart;
        while (iter) {
            if (iter == entry) {
                osSyncPrintf(VT_COL(RED, WHITE) "stackcheck_init: %08x は既にリスト中にある\n" VT_RST, entry);
                return;
            }
            iter = iter->next;
        }

        entry->prev = sStackInfoListEnd;
        entry->next = NULL;

        if (sStackInfoListEnd) {
            sStackInfoListEnd->next = entry;
        }

        sStackInfoListEnd = entry;
        if (!sStackInfoListStart) {
            sStackInfoListStart = entry;
        }

        if (entry->minSpace != -1) {
            addr = (u32*)entry->head;
            while ((uintptr_t)addr < entry->tail) {
                *addr++ = entry->initValue;
            }
        }
    }
}

void StackCheck_Cleanup(StackEntry* entry) {
    u32 inconsistency = false;

    if (!entry->prev) {
        if (entry == sStackInfoListStart) {
            sStackInfoListStart = entry->next;
        } else {
            inconsistency = true;
        }
    } else {
        entry->prev->next = entry->next;
    }

    if (!entry->next) {
        if (entry == sStackInfoListEnd) {
            sStackInfoListEnd = entry->prev;
        } else {
            inconsistency = true;
        }
    }
    if (inconsistency) {
        osSyncPrintf(VT_COL(RED, WHITE) "stackcheck_cleanup: %08x リスト不整合です\n" VT_RST, entry);
    }
}

s32 StackCheck_GetState(StackEntry* entry) {
    u32* last;
    size_t used;
    size_t free;
    s32 ret;

    for (last = (uintptr_t*)entry->head; (uintptr_t)last < entry->tail; last++) {
        if (entry->initValue != *last) {
            break;
        }
    }

    used = entry->tail - (uintptr_t)last;
    free = (uintptr_t)last - entry->head;

    if (free == 0) {
        ret = STACK_STATUS_OVERFLOW;
        osSyncPrintf(VT_FGCOL(RED));
    } else if (free < (u32)entry->minSpace && entry->minSpace != -1) {
        ret = STACK_STATUS_WARNING;
        osSyncPrintf(VT_FGCOL(YELLOW));
    } else {
        osSyncPrintf(VT_FGCOL(GREEN));
        ret = STACK_STATUS_OK;
    }

    osSyncPrintf("head=%08x tail=%08x last=%08x used=%08x free=%08x [%s]\n", entry->head, entry->tail, last, used, free,
                 entry->name != NULL ? entry->name : "(null)");
    osSyncPrintf(VT_RST);

    if (ret != STACK_STATUS_OK) {
        LogUtils_LogHexDump(entry->head, entry->tail - entry->head);
    }

    return ret;
}

u32 StackCheck_CheckAll(void) {
    u32 ret = 0;
    StackEntry* iter = sStackInfoListStart;

    while (iter) {
        u32 state = StackCheck_GetState(iter);

        if (state != STACK_STATUS_OK) {
            ret = 1;
        }
        iter = iter->next;
    }

    return ret;
}

u32 StackCheck_Check(StackEntry* entry) {
    if (entry == NULL) {
        return StackCheck_CheckAll();
    } else {
        return StackCheck_GetState(entry);
    }
}
