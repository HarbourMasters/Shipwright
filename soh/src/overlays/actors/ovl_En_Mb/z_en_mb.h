#ifndef Z_EN_MB_H
#define Z_EN_MB_H

#include "ultra64.h"
#include "global.h"

struct EnMb;

typedef void (*EnMbActionFunc)(struct EnMb*, PlayState*);

typedef enum {
    /*  0 */ ENMB_STATE_SPEAR_SPEARPATH_DAMAGED,
    /*  1 */ ENMB_STATE_CLUB_DEAD,
    /*  2 */ ENMB_STATE_CLUB_KNEELING_DAMAGED,
    /*  3 */ ENMB_STATE_CLUB_KNEELING,
    /*  5 */ ENMB_STATE_STUNNED = 5,
    /*  6 */ ENMB_STATE_IDLE,
    /*  9 */ ENMB_STATE_WALK = 9,
    /* 10 */ ENMB_STATE_ATTACK,
    /* 11 */ ENMB_STATE_ATTACK_END
} EnMbState;

typedef struct EnMb {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ Vec3s bodyPartsPos[10];
    /* 0x0188 */ u8 damageEffect;
    /* 0x018C */ SkelAnime skelAnime;
    /* 0x01D0 */ Vec3s jointTable[28];
    /* 0x0278 */ Vec3s morphTable[28];
    /* 0x0320 */ EnMbState state;
    /* 0x0324 */ EnMbActionFunc actionFunc;
    /* 0x0328 */ s16 iceEffectTimer;
    /* 0x032A */ s16 timer1;
    /* 0x032C */ s16 timer2;
    /* 0x032E */ s16 timer3;
    /* 0x0330 */ s16 yawToWaypoint;
    /* 0x0332 */ s16 unk_332;
    /* 0x0334 */ s16 attack;
    /* 0x0338 */ Vec3f effSpawnPos;
    /* 0x0344 */ Vec3f waypointPos;
    /* 0x0350 */ char unk_34A[0xC];
    /* 0x035C */ s8 waypoint;
    /* 0x035D */ s8 path;
    /* 0x035E */ s8 direction;
    /* 0x0360 */ f32 maxHomeDist;
    /* 0x0364 */ f32 playerDetectionRange;
    /* 0x0368 */ ColliderCylinder hitbox;
    /* 0x03B4 */ ColliderQuad attackCollider; // for attacking the player
    /* 0x0434 */ ColliderTris frontShielding; // Moblins don't have shields, but this acts as one
    /* 0x0454 */ ColliderTrisElement frontShieldingTris[2];
} EnMb; // size = 0x050C

#endif
