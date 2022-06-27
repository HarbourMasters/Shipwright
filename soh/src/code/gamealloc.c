#include "global.h"

void GameAlloc_Log(GameAlloc* this) {
    GameAllocEntry* iter;

    osSyncPrintf("this = %08x\n", this);

    iter = this->base.next;
    while (iter != &this->base) {
        osSyncPrintf("ptr = %08x size = %d\n", iter, iter->size);
        iter = iter->next;
    }
}

void* GameAlloc_MallocDebug(GameAlloc* this, size_t size, const char* file, s32 line) {
    GameAllocEntry* ptr = SystemArena_MallocDebug(size + sizeof(GameAllocEntry), file, line);

    if (ptr != NULL) {
        ptr->size = size;
        ptr->prev = this->head;
        this->head->next = ptr;
        this->head = ptr;
        ptr->next = &this->base;
        this->base.prev = this->head;
        return ptr + 1;
    } else {
        return NULL;
    }
}

void* GameAlloc_Malloc(GameAlloc* this, size_t size) {
    GameAllocEntry* ptr = SystemArena_MallocDebug(size + sizeof(GameAllocEntry), __FILE__, __LINE__);

    if (ptr != NULL) {
        ptr->size = size;
        ptr->prev = this->head;
        this->head->next = ptr;
        this->head = ptr;
        ptr->next = &this->base;
        this->base.prev = this->head;
        return ptr + 1;
    } else {
        return NULL;
    }
}

void GameAlloc_Free(GameAlloc* this, void* data) {
    GameAllocEntry* ptr;

    if (data != NULL) {
        ptr = &((GameAllocEntry*)data)[-1];
        LogUtils_CheckNullPointer("ptr->prev", ptr->prev, __FILE__, __LINE__);
        LogUtils_CheckNullPointer("ptr->next", ptr->next, __FILE__, __LINE__);
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
        this->head = this->base.prev;
        SystemArena_FreeDebug(ptr, __FILE__, __LINE__);
    }
}

void GameAlloc_Cleanup(GameAlloc* this) {
    GameAllocEntry* next = this->base.next;
    GameAllocEntry* cur;

    while (&this->base != next) {
        cur = next;
        next = next->next;
        SystemArena_FreeDebug(cur, __FILE__, __LINE__);
    }

    this->head = &this->base;
    this->base.next = &this->base;
    this->base.prev = &this->base;
}

void GameAlloc_Init(GameAlloc* this) {
    this->head = &this->base;
    this->base.next = &this->base;
    this->base.prev = &this->base;
}
