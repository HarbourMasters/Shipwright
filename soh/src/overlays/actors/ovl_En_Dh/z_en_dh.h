#ifndef Z_EN_DH_H
#define Z_EN_DH_H

#include "ultra64.h"
#include "global.h"

struct EnDh;

typedef void (*EnDhActionFunc)(struct EnDh*, PlayState*);

typedef struct EnDh {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0170 */ Vec3s jointTable[16];
    /* 0x01F0 */ Vec3s limbRotTable[16];
    /* 0x0250 */ u8 actionState; 
    /* 0x0251 */ u8 retreat; 
    /* 0x0252 */ u8 drawDirtWave;
    /* 0x0254 */ EnDhActionFunc actionFunc;
    /* 0x0258 */ u8 unk_258; // Related to player->unk_845
    /* 0x0259 */ u8 unused_259;
    /* 0x025A */ u8 alpha;
    /* 0x025B */ u8 curAction;
    /* 0x025C */ s16 timer;
    /* 0x025E */ s16 dirtWavePhase;
    /* 0x0260 */ ColliderCylinder collider1;
    /* 0x02AC */ ColliderJntSph collider2;
    /* 0x02CC */ ColliderJntSphElement elements[1];
    /* 0x030C */ Vec3f headPos;
    /* 0x0318 */ f32 dirtWaveSpread;
    /* 0x031C */ f32 dirtWaveHeight;
    /* 0x0320 */ f32 dirtWaveAlpha;
} EnDh; // size = 0x0324

typedef enum {
    ENDH_HANDS_KILLED_4 = -4,
    ENDH_HANDS_KILLED_3,
    ENDH_HANDS_KILLED_2,
    ENDH_HANDS_KILLED_1,
    ENDH_WAIT_UNDERGROUND,
    ENDH_START_ATTACK_GRAB,
    ENDH_START_ATTACK_BOMB = 5,
    ENDH_DEATH = 10
} EnDhParams;

#endif
