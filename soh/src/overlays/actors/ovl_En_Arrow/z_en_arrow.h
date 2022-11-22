#ifndef Z_EN_ARROW_H
#define Z_EN_ARROW_H

#include "ultra64.h"
#include "global.h"

struct EnArrow;

typedef void (*EnArrowActionFunc)(struct EnArrow*, PlayState*);

typedef struct EnArrow {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ ColliderQuad collider;
    /* 0x0210 */ Vec3f unk_210;
    /* 0x021C */ Vec3f unk_21C;
    /* 0x0228 */ s32 effectIndex;
    /* 0x022C */ WeaponInfo weaponInfo;
    /* 0x0248 */ u8 timer; // used for dissapearing when flying or hitting a wall
    /* 0x0249 */ u8 hitFlags;
    /* 0x024A */ u8 touchedPoly;
    /* 0x024B */ u8 isCsNut;
    /* 0x024C */ Actor* hitActor;
    /* 0x0250 */ Vec3f unk_250;
    /* 0x025C */ EnArrowActionFunc actionFunc;
} EnArrow; // size = 0x0260

typedef enum {
    /* -10 */ ARROW_CS_NUT = -10, // cutscene deku nuts are allowed to update in blocking mode
    /* -1  */ ARROW_NORMAL_SILENT = -1, // normal arrow that does not make a sound when being shot
    /*  0  */ ARROW_NORMAL_LIT, // normal arrow lit on fire
    /*  1  */ ARROW_NORMAL_HORSE, // normal arrow shot while riding a horse
    /*  2  */ ARROW_NORMAL,
    /*  3  */ ARROW_FIRE,
    /*  4  */ ARROW_ICE,
    /*  5  */ ARROW_LIGHT,
    /*  6  */ ARROW_0C,
    /*  7  */ ARROW_0D,
    /*  8  */ ARROW_0E,
    /*  9  */ ARROW_SEED,
    /*  10 */ ARROW_NUT
} ArrowType;

#endif
