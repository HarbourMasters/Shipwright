#ifndef GFX_ENHANCEMENT_H
#define GFX_EHHANCEMENT_H

#include "z64.h"

typedef struct {
    void *tex;
    uint16_t width;
    uint16_t height;
    uint8_t im_fmt;
    uint8_t im_siz;
} sprite_t;

void sprite_load(sprite_t *sprite);
void sprite_draw(sprite_t *sprite, int left, int top, int width, int height);

#endif
