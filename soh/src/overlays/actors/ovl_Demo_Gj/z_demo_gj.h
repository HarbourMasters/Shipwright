#ifndef Z_DEMO_GJ_H
#define Z_DEMO_GJ_H

#include "ultra64.h"
#include "global.h"

#include "overlays/actors/ovl_Boss_Ganon2/z_boss_ganon2.h"

struct DemoGj;

typedef struct DemoGj {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ s32 updateMode;
    /* 0x0168 */ s32 drawConfig;
    /* 0x016C */ Vec3s rotationVec;
    /* 0x0172 */ Vec3s unk_172; // It is assigned only. @see DemoGj_SetupTranslation and @see DemoGj_SetupMovement
    /* 0x0178 */ BossGanon2* ganon;
    /* 0x017C */ s32 isTransformedIntoGanon; // flag
    /* 0x0180 */ s32 isRotated; // flag
    /* 0x0184 */ ColliderCylinder cylinders[3];
    /* 0x0268 */ s32 killFlag; // This actor never sets this flag, but it reads it. If set to `true` and the actor type is DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1, DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_2 or DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_TALL, then the actor will be killed and will drop the specified amount of collectibles.
    /* 0x026C */ Vec3f unk_26C; // This actor never sets this. Specifies which direction will this actor explode when killed using `killFlag`.
} DemoGj; // size = 0x0278

/**
 * The format of this actor's params is the following:
 * bits 11-15: The collectible that will be dropped when killed.
 * bits  8-10: The amount of collectible that will be dropped when killed.
 * bits  0- 7: A value of the enum DemoGjType.
 */
typedef enum {
    /* 04 */ DEMOGJ_TYPE_AROUNDARENA = 4, // This is the indestructible rubble around the arena. This actor keeps alive the whole fight.
    /* 08 */ DEMOGJ_TYPE_RUBBLE_PILE_1 = 8, // DEMOGJ_TYPE_RUBBLE_PILE_X are the rubbles from where Ganondorf rises. When he transforms into Ganon these are removed from the scene (Actor_Kill).
    /* 09 */ DEMOGJ_TYPE_RUBBLE_PILE_2,
    /* 10 */ DEMOGJ_TYPE_RUBBLE_PILE_3,
    /* 11 */ DEMOGJ_TYPE_RUBBLE_PILE_4,
    /* 12 */ DEMOGJ_TYPE_RUBBLE_PILE_5,
    /* 13 */ DEMOGJ_TYPE_RUBBLE_PILE_6,
    /* 14 */ DEMOGJ_TYPE_RUBBLE_PILE_7,
    /* 16 */ DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1 = 16,   // This three rubbles are the ones that Ganon can destroy and drop collectables.
    /* 17 */ DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_2,        // They are spawned as soon as the others, but they have collision and are rendered only when Ganondorf has transformed into Ganon.
    /* 22 */ DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_TALL = 22 //
} DemoGjType;

#endif
