#ifndef Z_EN_ANUBICE_H
#define Z_EN_ANUBICE_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_Bg_Hidan_Curtain/z_bg_hidan_curtain.h"

struct EnAnubice;

typedef void (*EnAnubiceActionFunc)(struct EnAnubice*, struct PlayState*);

typedef enum {
    /*  0 */ ANUBICE_LIMB_NONE,
    /*  1 */ ANUBICE_LIMB_ROOT,
    /*  2 */ ANUBICE_LIMB_BODY_ROOT,
    /*  3 */ ANUBICE_LIMB_CHEST,
    /*  4 */ ANUBICE_LIMB_ABDOMEN_ROOT,
    /*  5 */ ANUBICE_LIMB_LOWER_ABDOMEN,
    /*  6 */ ANUBICE_LIMB_UPPER_ABDOMEN,
    /*  7 */ ANUBICE_LIMB_TAIL_ROOT,
    /*  8 */ ANUBICE_LIMB_TAIL_BASE,
    /*  9 */ ANUBICE_LIMB_TAIL_TIP,
    /* 10 */ ANUBICE_LIMB_JEWEL_ROOT,
    /* 11 */ ANUBICE_LIMB_JEWEL,
    /* 12 */ ANUBICE_LIMB_HEAD_ROOT,
    /* 13 */ ANUBICE_LIMB_HEAD,
    /* 14 */ ANUBICE_LIMB_JAW_ROOT,
    /* 15 */ ANUBICE_LIMB_JAW,
    /* 16 */ ANUBICE_LIMB_MAX
} AnubiceLimbs;

typedef struct EnAnubice {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[ANUBICE_LIMB_MAX];
    /* 0x01F0 */ Vec3s morphTable[ANUBICE_LIMB_MAX];
    /* 0x0250 */ EnAnubiceActionFunc actionFunc;
    /* 0x0254 */ s16 timeAlive;
    /* 0x0256 */ s16 isFallingOver;
    /* 0x0258 */ s16 fallTargetPitch;
    /* 0x025A */ s16 deathTimer;
    /* 0x025C */ s16 knockbackTimer;
    /* 0x025E */ s16 isMirroringLink;
    /* 0x0260 */ s16 isLinkOutOfRange;
    /* 0x0262 */ s16 isKnockedback;
    /* 0x0264 */ s16 hasSearchedForFlameCircles;
    /* 0x0268 */ f32 hoverVelocityTimer;
    /* 0x026C */ f32 animLastFrame;
    /* 0x0270 */ f32 targetHeight;
    /* 0x0274 */ f32 playerHeightOffset; // How high above the player to hover at
    /* 0x0278 */ f32 headRot;
    /* 0x027C */ f32 focusHeightOffset;
    /* 0x0280 */ Vec3f fireballPos;
    /* 0x028C */ Vec3f fireballRot;
    /* 0x0298 */ Vec3f home;
    /* 0x02A4 */ Vec3f knockbackRecoveryVelocity;
    /* 0x02B0 */ BgHidanCurtain* flameCircles[5];
    /* 0x02C4 */ char unk_2C4[0x4];
    /* 0x02C8 */ ColliderCylinder collider;
} EnAnubice; // size = 0x0314

#endif
