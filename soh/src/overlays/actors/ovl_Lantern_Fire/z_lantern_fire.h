#ifndef Z_LANTERN_FIRE_H
#define Z_LANTERN_FIRE_H

#include "overlays/actors/ovl_Wind_Zone/z_wind_zone.h"
#include <libultraship/libultra.h>
#include "global.h"

struct LanternFire;

typedef void (*LanternFireActionFunc)(struct LanternFire* this, PlayState* play);

typedef struct LanternFire {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ LanternFireActionFunc actionFunc;
    /* 0x0150 */ ColliderCylinder collider;
    /* 0x019C */ f32 spawnRadius;
    /* 0x01A0 */ f32 scale;
    /* 0x01A4 */ char unk_1A4[0x2];
    /* 0x01A6 */ s16 deathTimer;
    /* 0x01A8 */ s16 tile2Y;
    /* 0x022C */ WeaponInfo weaponInfo;
    /* 0x0248 */ u8 timer; // used for dissapearing when flying or hitting a wall
    /* 0x0249 */ u8 hitFlags;
    /* 0x024A */ u8 touchedPoly;
    /* 0x024C */ Actor* hitActor;
    WindZone* wz;
} LanternFire; // size = 0x01AC

#endif
