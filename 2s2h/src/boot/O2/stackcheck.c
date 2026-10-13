#include "stackcheck.h"
#include "libc/stdbool.h"
#include "libc/stdint.h"

StackEntry* sStackInfoListStart = NULL;
StackEntry* sStackInfoListEnd = NULL;

void StackCheck_Init(StackEntry* entry, void* stackBottom, void* stackTop, u32 initValue, s32 minSpace,
                     const char* name) {
    if (entry == NULL) {
        sStackInfoListStart = NULL;
    } else {
        StackEntry* iter;

        entry->head = stackBottom;
        entry->tail = stackTop;
        entry->initValue = initValue;
        entry->minSpace = minSpace;
        entry->name = name;
        iter = sStackInfoListStart;
        while (iter) {
            if (iter == entry) {
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
        if (sStackInfoListStart == NULL) {
            sStackInfoListStart = entry;
        }

        if (entry->minSpace != -1) {
            u32* addr = entry->head;

            while (addr < (u32*)entry->tail) {
                *addr++ = entry->initValue;
            }
        }
    }
}

void StackCheck_Cleanup(StackEntry* entry) {
    u32 inconsistency = false;

    if (entry->prev == NULL) {
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

    if (inconsistency) {}
}

StackStatus StackCheck_GetState(StackEntry* entry) {
    u32* last;
    size_t used;
    size_t free;
    StackStatus status;

    for (last = entry->head; last < (u32*)entry->tail; last++) {
        if (entry->initValue != *last) {
            break;
        }
    }

    used = (uintptr_t)entry->tail - (uintptr_t)last;
    free = (uintptr_t)last - (uintptr_t)entry->head;

    if (free == 0) {
        status = STACK_STATUS_OVERFLOW;
    } else if ((free < (size_t)entry->minSpace) && (entry->minSpace != -1)) {
        status = STACK_STATUS_WARNING;
    } else {
        status = STACK_STATUS_OK;
    }

    return status;
}

u32 StackCheck_CheckAll(void) {
    u32 ret = 0;
    StackEntry* iter = sStackInfoListStart;

    while (iter != NULL) {
        StackStatus state = StackCheck_GetState(iter);

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
