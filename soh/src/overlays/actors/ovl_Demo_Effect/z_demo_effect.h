#ifndef Z_DEMO_EFFECT_H
#define Z_DEMO_EFFECT_H

#include "ultra64.h"
#include "global.h"

struct DemoEffect;

typedef void (*DemoEffectFunc)(struct DemoEffect*, PlayState*);

typedef struct {
    /* 0x00 */ u8 timer;
} DemoEffectFireBall;

typedef struct {
    /* 0x00 */ u8 alpha;
    /* 0x01 */ u8 scale;
    /* 0x02 */ u8 pad;
    /* 0x04 */ s16 rotation;
} DemoEffectBlueOrb;

typedef struct {
    /* 0x00 */ u8 alpha;
    /* 0x01 */ u8 scaleFlag;
    /* 0x02 */ u8 flicker;
    /* 0x04 */ s16 rotation;
} DemoEffectLight;

typedef struct {
    /* 0x00 */ u8 alpha;
} DemoEffectLgtShower;

typedef struct {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 lightRingSpawnDelay;
    /* 0x02 */ u8 rotation;
    /* 0x04 */ s16 lightRingSpawnTimer;
} DemoEffectGodLgt;

typedef struct {
    /* 0x00 */ u8 timerIncrement;
    /* 0x01 */ u8 alpha;
    /* 0x02 */ u8 pad;
    /* 0x04 */ s16 timer;
} DemoEffectLightRing;

typedef struct {
    /* 0x00 */ u8 triforceSpotOpacity;
    /* 0x01 */ u8 lightColumnOpacity;
    /* 0x02 */ u8 crystalLightOpacity;
    /* 0x04 */ s16 rotation;
} DemoEffectTriforceSpot;

typedef struct {
    /* 0x00 */ u8 isPositionInit;
    /* 0x01 */ u8 isLoaded;
    /* 0x02 */ u8 drawId;
    /* 0x04 */ s16 rotation;
} DemoEffectGetItem;

typedef struct {
    /* 0x00 */ u8 pad;
    /* 0x01 */ u8 pad2;
    /* 0x02 */ u8 pad3;
    /* 0x04 */ s16 shrinkTimer;
} DemoEffectTimeWarp;

typedef struct {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 isPositionInit;
    /* 0x02 */ u8 alpha;
    /* 0x04 */ s16 timer;
} DemoEffectJewel;

typedef struct {
    /* 0x00 */ u8 timer;
} DemoEffectDust;

typedef struct DemoEffect {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnimeCurve skelCurve;
    /* 0x016C */ u8 initObjectBankIndex;
    /* 0x0170 */ Gfx* jewelDisplayList;
    /* 0x0174 */ Gfx* jewelHolderDisplayList;
    /* 0x0178 */ u8 primXluColor[3];
    /* 0x017B */ u8 envXluColor[3];
    /* 0x017E */ u8 primOpaColor[3];
    /* 0x0181 */ u8 envOpaColor[3];
    /* 0x0184 */ union {
        DemoEffectFireBall fireBall;
        DemoEffectBlueOrb blueOrb;
        DemoEffectLight light;
        DemoEffectLgtShower lgtShower;
        DemoEffectGodLgt godLgt;
        DemoEffectLightRing lightRing;
        DemoEffectTriforceSpot triforceSpot;
        DemoEffectGetItem getItem;
        DemoEffectTimeWarp timeWarp;
        DemoEffectJewel jewel;
        DemoEffectDust dust;
    };
    /* 0x018A */ s16 effectFlags;
    /* 0x018C */ s16 csActionId;
    /* 0x018E */ Vec3s jewelCsRotation;
    /* 0x0194 */ DemoEffectFunc initUpdateFunc;
    /* 0x0198 */ ActorFunc initDrawFunc;
    /* 0x019C */ DemoEffectFunc updateFunc;
} DemoEffect; // size = 0x01A0

// These names come from the objects that correspond to this actor type.
typedef enum {
    /* 0x00 */ DEMO_EFFECT_CRYSTAL_LIGHT,
    /* 0x01 */ DEMO_EFFECT_FIRE_BALL,
    /* 0x02 */ DEMO_EFFECT_BLUE_ORB, // Object is in GAMEPLAY_KEEP. Not a name from object. It's a blue orb.
    /* 0x03 */ DEMO_EFFECT_LGT_SHOWER,
    /* 0x04 */ DEMO_EFFECT_GOD_LGT_DIN,
    /* 0x05 */ DEMO_EFFECT_GOD_LGT_NAYRU,
    /* 0x06 */ DEMO_EFFECT_GOD_LGT_FARORE,
    /* 0x07 */ DEMO_EFFECT_LIGHTRING_EXPANDING,
    /* 0x08 */ DEMO_EFFECT_TRIFORCE_SPOT,
    /* 0x09 */ DEMO_EFFECT_MEDAL_FIRE,
    /* 0x0A */ DEMO_EFFECT_MEDAL_WATER,
    /* 0x0B */ DEMO_EFFECT_MEDAL_FOREST,
    /* 0x0C */ DEMO_EFFECT_MEDAL_SPIRIT,
    /* 0x0D */ DEMO_EFFECT_MEDAL_SHADOW,
    /* 0x0E */ DEMO_EFFECT_MEDAL_LIGHT,
    /* 0x0F */ DEMO_EFFECT_TIMEWARP_MASTERSWORD,
    /* 0x10 */ DEMO_EFFECT_LIGHTRING_SHRINKING,
    /* 0x11 */ DEMO_EFFECT_LIGHTRING_TRIFORCE,
    /* 0x12 */ DEMO_EFFECT_LIGHT,
    /* 0x13 */ DEMO_EFFECT_JEWEL_KOKIRI,
    /* 0x14 */ DEMO_EFFECT_JEWEL_GORON,
    /* 0x15 */ DEMO_EFFECT_JEWEL_ZORA,
    /* 0x16 */ DEMO_EFFECT_DUST, // Object is jewel, but this is really the dust in the ToT light arrow cutscene.
    /* 0x17 */ DEMO_EFFECT_LIGHTARROW,
    /* 0x18 */ DEMO_EFFECT_TIMEWARP_TIMEBLOCK_LARGE,
    /* 0x19 */ DEMO_EFFECT_TIMEWARP_TIMEBLOCK_SMALL,
    /* 0x1A */ DEMO_EFFECT_MAX_TYPE
} DemoEffectType;

typedef enum {
    /* 0x00 */ DEMO_EFFECT_LIGHT_RED,
    /* 0x01 */ DEMO_EFFECT_LIGHT_BLUE,
    /* 0x02 */ DEMO_EFFECT_LIGHT_GREEN,
    /* 0x03 */ DEMO_EFFECT_LIGHT_ORANGE,
    /* 0x04 */ DEMO_EFFECT_LIGHT_YELLOW,
    /* 0x05 */ DEMO_EFFECT_LIGHT_PURPLE,
    /* 0x06 */ DEMO_EFFECT_LIGHT_GREEN2
} DemoEffectLightColor;

typedef enum {
    /* 0x00 */ GOD_LGT_DIN,
    /* 0x01 */ GOD_LGT_NAYRU,
    /* 0x02 */ GOD_LGT_FARORE
} DemoEffectGodLgtType;

// params info
// type: (params & 0x00FF)
// light size: ((params & 0x0F00) >> 8)
// light color: ((params & 0xF000) >> 12)

#endif
