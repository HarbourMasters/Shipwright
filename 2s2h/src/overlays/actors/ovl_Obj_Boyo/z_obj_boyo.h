#ifndef Z_OBJ_BOYO_H
#define Z_OBJ_BOYO_H

#include "global.h"

typedef struct ObjBoyo {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ AnimatedMaterial* animatedMaterial;
    /* 0x194 */ s16 unk194;
    /* 0x196 */ s16 unk196;
    /* 0x198 */ f32 unk198;
    /* 0x19C */ f32 unk19C;
    /* 0x1A0 */ f32 unk1A0;
    /* 0x1A4 */ s16 unk1A4;
    /* 0x1A6 */ s16 unk1A6;
    /* 0x1A8 */ s16 unk1A8;
    /* 0x1AA */ s16 unk1AA;
    /* 0x1AC */ s16 unk1AC;
    /* 0x1AE */ s16 unk1AE;
} ObjBoyo; // size = 0x1B0

#endif // Z_OBJ_BOYO_H
