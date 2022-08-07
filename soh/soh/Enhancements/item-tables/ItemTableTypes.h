#pragma once
#ifdef __cplusplus
#include <stdint.h>
#endif

typedef struct {
    /* 0x00 */ uint16_t itemId;
    /* 0x01 */ uint16_t field; // various bit-packed data
    /* 0x02 */ int16_t gi;     // defines the draw id and chest opening animation
    /* 0x03 */ uint16_t textId;
    /* 0x04 */ uint16_t objectId;
    /* 0x06 */ uint16_t modIndex; // 0 = Vanilla, 1 = Randomizer, future mods will increment up?
} GetItemEntry;                   // size = 0x08
