#ifndef Z_EN_DRAGON_H
#define Z_EN_DRAGON_H

#include "global.h"
#include "objects/object_utubo/object_utubo.h"

#define EN_DRAGON_GET_PYTHON_INDEX(thisx) (((thisx)->params >> 7) & 0x1F)

typedef enum {
    /* 0 */ DEEP_PYTHON_ACTION_IDLE,
    /* 1 */ DEEP_PYTHON_ACTION_EXTEND,
    /* 2 */ DEEP_PYTHON_ACTION_GRAB,
    /* 3 */ DEEP_PYTHON_ACTION_DAMAGE,
    /* 4 */ DEEP_PYTHON_ACTION_RETREAT,
    /* 5 */ DEEP_PYTHON_ACTION_SETUP_DEAD,
    /* 6 */ DEEP_PYTHON_ACTION_DEAD
} DeepPythonAction;

struct EnDragon;

typedef void (*EnDragonActionFunc)(struct EnDragon*, PlayState*);

typedef struct EnDragon {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[DEEP_PYTHON_LIMB_MAX];
    /* 0x1E8 */ Vec3s morphTable[DEEP_PYTHON_LIMB_MAX];
    /* 0x248 */ EnDragonActionFunc actionFunc;
    /* 0x24C */ s32 animIndex;
    /* 0x250 */ s32 pythonIndex;
    /* 0x254 */ Vec3f jawPos;
    /* 0x260 */ Vec3f burrowEntrancePos; // Vertically and horizontally centered on the burrow opening. Set by Bg_Sinkai_Kabe
    /* 0x26C */ Vec3f playerGrabPosition;
    /* 0x278 */ Vec3f subCamEye;
    /* 0x278 */ Vec3f subCamAt;
    /* 0x290 */ Vec3f playerGrabPositionTemp;
    /* 0x29C */ Vec3f focusPos;
    /* 0x2A8 */ s16 jawZRotation;
    /* 0x2AA */ s16 jawYRotation; // Never initialized by the actor
    /* 0x2AC */ s16 jawXRotation; // Never initialized by the actor
    /* 0x2AE */ s16 retreatTimer; // If the Deep Python is extended and this timer reaches 0, it is forced to retreat back to its burrow
    /* 0x2B0 */ s16 behindJawRetreatTimer; // If the player is behind the jaw, then this will count up. When it reaches 60, the Deep Python will retreat
    /* 0x2B2 */ s16 largeSwayWaitTimer; // If the player is in front of the jaw, it won't play or replay the large sway animation unless this is zero.
    /* 0x2B4 */ s16 timer;
    /* 0x2B6 */ s16 grabWaitTimer; // Cannot grab the player if this is non-zero
    /* 0x2B8 */ s16 hasGrabbedPlayer;
    /* 0x2BA */ s16 action;
    /* 0x2BC */ UNK_TYPE1 unk_2BC[0x2];
    /* 0x2BE */ s16 state;
    /* 0x2C0 */ s16 grabCsId;
    /* 0x2C2 */ s16 deathCsId;
    /* 0x2C4 */ UNK_TYPE1 unk_2C4[0x4];
    /* 0x2C8 */ s16 subCamId;
    /* 0x2CA */ s16 grabTimer; // Counts up from the time a grab starts until the time the actor begins attacking
    /* 0x2CC */ s16 unk_2CC; // Initialized, but never used
    /* 0x2D0 */ f32 animEndFrame;
    /* 0x2D4 */ f32 scale;
    /* 0x2D8 */ UNK_TYPE1 unk_2D8[0x4];
    /* 0x2DC */ ColliderJntSph collider;
    /* 0x2FC */ ColliderJntSphElement colliderElements[8];
} EnDragon; // size = 0x4FC

#endif // Z_EN_DRAGON_H
