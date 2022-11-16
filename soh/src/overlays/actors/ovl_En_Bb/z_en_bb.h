#ifndef Z_EN_BB_H
#define Z_EN_BB_H

#include "ultra64.h"
#include "global.h"

struct EnBb;

typedef void (*EnBbActionFunc)(struct EnBb*, PlayState*);

typedef struct EnBb {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[16];
    /* 0x01F0 */ Vec3s morphTable[16];
    /* 0x0250 */ s32 action;
    /* 0x0254 */ s32 unk_254; // unused
    /* 0x0258 */ EnBbActionFunc actionFunc;
    /* 0x025C */ s32 moveMode;
    /* 0x0260 */ s32 timer;
    /* 0x0264 */ s32 unk_264; // unused
    /* 0x0268 */ s16 actionState;
    /* 0x026A */ s16 charge;
    /* 0x026C */ s16 actionVar1; // index for flame trail, phase of small hop when blue finds a bomb
    /* 0x026E */ s16 actionVar2; // move yaw for blue, flame timer for green, maxAlpha for flame trail
    /* 0x0270 */ s16 flameScrollMod;
    /* 0x0274 */ f32 bobPhase;
    /* 0x0278 */ f32 bobSize;
    /* 0x027C */ f32 maxSpeed;
    /* 0x0280 */ f32 flyHeightMod;
    /* 0x027C */ f32 bobSpeedMod; // y speed for blue, phase speed for green
    /* 0x0288 */ f32 flameScaleY;
    /* 0x028C */ f32 flameScaleX;
    /* 0x0290 */ Vec3f waypointPos;
    /* 0x029C */ u8 path;
    /* 0x029D */ u8 waypoint;
    /* 0x029E */ u8 flamePrimBlue;
    /* 0x029F */ u8 flamePrimAlpha;
    /* 0x02A0 */ Color_RGB8 flameEnvColor;
    /* 0x02A4 */ s32 blureIdx;
    /* 0x02A8 */ s16 fireIceTimer;
    /* 0x02AA */ u8 dmgEffect;  
    /* 0x02AC */ ColliderJntSph collider;
    /* 0x02CC */ ColliderJntSphElement elements[1];
    /* 0x030C */ BodyBreak bodyBreak;
    /* 0x0324 */ Actor* targetActor;
} EnBb; // size = 0x0328

typedef enum {
    ENBB_GREEN_BIG = -5,
    ENBB_GREEN,
    ENBB_WHITE,
    ENBB_RED,
    ENBB_BLUE,
    ENBB_FLAME_TRAIL,
    ENBB_KILL_TRAIL = 11
} EnBbType;

#endif
