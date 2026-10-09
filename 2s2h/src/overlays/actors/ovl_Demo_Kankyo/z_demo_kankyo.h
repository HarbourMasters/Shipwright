#ifndef Z_DEMO_KANKYO_H
#define Z_DEMO_KANKYO_H

#include "global.h"

struct DemoKankyo;

typedef void (*DemoKankyoActionFunc)(struct DemoKankyo*, PlayState*);

typedef struct {
  /* 0x00 */ u8 state;
  /* 0x04 */ Vec3f posOffset;
  /* 0x10 */ Vec3f posOffsetPrev;
  /* 0x1C */ Vec3f posBase;
  /* 0x28 */ Vec3f speedClock; // cycles in radians
  /* 0x34 */ f32 speed;
  /* 0x38 */ f32 speedTarget;
  /* 0x3C */ u16 alphaClock;
  /* 0x3E */ u16 LostWoodsSkyFishSpeedXZClock;
  /* 0x40 */ u8 alpha;
  /* 0x44 */ f32 scale; // size of the effect
  /* 0x48 */ u16 LostWoodsSkyFishSpeedXZ; // the x-z speed (angular velocity) the lost woods skyfish oscillates around player. pos or neg 200-400 
  /* 0x4A */ u16 LostWoodsSkyFishPosOffsetMax; // The x-z range the lost woods skyfish oscillates around player. random value between 15-65
  /* 0x4C */ f32 LostWoodsSkyFishSpeedY; // the y speed (angular velocity) the lost woods skyfish oscillates around player.
  /* 0x50 */ u16 pad50; // unused, always assigned to 0, nothing else in this actor uses it
  // 2S2H [Port]
  /*      */ s16 epoch; // Tracks when an effect is reset for use with interpolation skipping
} DemoKankyoEffect; // size = 0x54

#define DEMOKANKYO_EFFECT_COUNT 64

typedef struct DemoKankyo {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ DemoKankyoEffect effects[DEMOKANKYO_EFFECT_COUNT];
    /* 0x1644 */ DemoKankyoActionFunc actionFunc;
    /* 0x1648 */ s32 objectSlot;
    /* 0x164C */ u8 isSafeToDrawGiants;
} DemoKankyo; // size = 0x1650

typedef enum {
    /* 0 */ DEMO_KANKYO_TYPE_LOSTWOODS,
    /* 1 */ DEMO_KANKYO_TYPE_GIANTS,
    /* 2 */ DEMO_KANKYO_TYPE_MOON
} DemoKankyoType;

typedef enum {
    /* 0 */ DEMO_KANKYO_STATE_INIT,
    /* 1 */ DEMO_KANKYO_STATE_SINGLE,
    /* 2 */ DEMO_KANKYO_STATE_SKYFISH,
    /* 3 */ DEMO_KANKYO_STATE_DISABLED
} DemoKankyoStateType;

#endif // Z_DEMO_KANKYO_H
