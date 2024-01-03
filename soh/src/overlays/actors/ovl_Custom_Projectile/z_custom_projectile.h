#ifndef Z_CUSTOM_PROJECTILE_H
#define Z_CUSTOM_PROJECTILE_H

#include <libultraship/libultra.h>
#include "global.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

struct CustomProjectile;

typedef void (*CustomProjectileActionFunc)(struct CustomProjectile*, PlayState*);

typedef struct CustomProjectile {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ ColliderQuad collider[3];
    /* 0x0190 */ ColliderCylinder colliderCylinder;
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
    /* 0x025C */ CustomProjectileActionFunc actionFunc;
    //float originalScale;
    // Audio
    f32 freqscale;
    f32 volume;
    f32 volumeShoot;
    s8 reverb;
    //ULTRAHAND
    bool madeFirstContact;
    Actor* ultrahandedActor;
    bool ultrahandReleased;
    bool ultrahandedGrounded;
    float ultrahandedOriginalGravity;
    float ultrahandedDistance;
    //ARMCANNON
    int armcannonCharge;
    EnBom* bomb;
    Actor* crown;
    //REFLECTOR
    int relectorFrame;
} CustomProjectile; // size = 0x0260

typedef enum {
    /*  2  */ PROJECTILE_ULTRAHAND,
    /*  3  */ PROJECTILE_ARMCANNON,
    /*  3  */ PROJECTILE_REFLECTOR,
} ProjectileType;

#endif
