#ifndef Z_OBJ_JG_GAKKI_H
#define Z_OBJ_JG_GAKKI_H

#include "global.h"

struct ObjJgGakki;

typedef struct ObjJgGakki {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ ColliderCylinder collider;
} ObjJgGakki; // size = 0x1D4

#endif // Z_OBJ_JG_GAKKI_H
