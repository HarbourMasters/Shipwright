#pragma once

#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum SaveStateMode {
    SHIP_SAVESTATE_MEASURE,
    SHIP_SAVESTATE_SAVE,
    SHIP_SAVESTATE_LOAD,
} SaveStateMode;

typedef struct SaveStateCtx {
    unsigned char* buffer;
    size_t offset;
    SaveStateMode mode;
} SaveStateCtx;

static inline void SaveState_Blob(SaveStateCtx* ctx, void* data, size_t len) {
    switch (ctx->mode) {
        case SHIP_SAVESTATE_SAVE:
            memcpy(ctx->buffer + ctx->offset, data, len);
            break;
        case SHIP_SAVESTATE_LOAD:
            memcpy(data, ctx->buffer + ctx->offset, len);
            break;
        case SHIP_SAVESTATE_MEASURE:
        default:
            break;
    }
    ctx->offset += len;
}

#define SHIP_SAVESTATE_SERIALIZE_FIELD(field) SaveState_Blob(ctx, &(field), sizeof(field));
#define SHIP_SAVESTATE_DEFINE(Tag, FIELDS)     \
    void Tag##_SaveState(SaveStateCtx* ctx) {  \
        FIELDS(SHIP_SAVESTATE_SERIALIZE_FIELD) \
    }

#ifdef __cplusplus
}
#endif
