#ifndef Z_EN_DINOFOS_H
#define Z_EN_DINOFOS_H

#include "global.h"
#include "objects/object_dinofos/object_dinofos.h"

struct EnDinofos;

typedef void (*EnDinofosActionFunc)(struct EnDinofos*, PlayState*);

typedef enum EnDinofosBodyPart {
    /*  0 */ DINOFOS_BODYPART_LEFT_UPPER_LEG,
    /*  1 */ DINOFOS_BODYPART_LEFT_CLAWS,
    /*  2 */ DINOFOS_BODYPART_RIGHT_UPPER_LEG,
    /*  3 */ DINOFOS_BODYPART_RIGHT_CLAWS,
    /*  4 */ DINOFOS_BODYPART_UPPER_BODY,
    /*  5 */ DINOFOS_BODYPART_LEFT_FOREARM,
    /*  6 */ DINOFOS_BODYPART_LEFT_HAND,
    /*  7 */ DINOFOS_BODYPART_RIGHT_FOREARM,
    /*  8 */ DINOFOS_BODYPART_RIGHT_HAND,
    /*  9 */ DINOFOS_BODYPART_HEAD,
    /* 10 */ DINOFOS_BODYPART_JAW,
    /* 11 */ DINOFOS_BODYPART_LOWER_TAIL,
    /* 12 */ DINOFOS_BODYPART_MAX
} EnDinofosBodyPart;

typedef enum EnDinofosJumpType {
    /* 0 */ DINOFOS_JUMP_TYPE_FORWARD,
    /* 1 */ DINOFOS_JUMP_TYPE_IN_PLACE,
    /* 2 */ DINOFOS_JUMP_TYPE_BACKWARD,
    /* 3 */ DINOFOS_JUMP_TYPE_SLASH
} EnDinofosJumpType;

/**
 * The elements before DINOFOS_COLLIDER_FIRE_START_INDEX are for a Dinofos' body in the actor's collider.
 * The last three elements are for the flame.
 */
#define DINOFOS_COLLIDER_FIRE_START_INDEX 6

/**
 * unk_294 explanation :
 *
 * The best way to compare is to look from timer1 to circlingRate in this current actor struct and from
 * 0x03F0 to 0x0408 in OoT's struct for Lizalfos/Dinolfos.
 *
 * In OoT, an additionnal ice timer, booleans related to the platforms fight and platforms indices are stored
 * after the two timers EnDinofos still has (all integers). Moreover, the alpha value (envColorAlpha) has been moved up
 * in MM.
 *
 * In MM, subCamId and subCamRot were added to handle the new cutscenes. Also, the effectIndex for blur is new.
 *
 * All in all, unk_294 is probably remnants of the now unnecessary variables mentioned above.
 */

typedef struct EnDinofos {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[DINOLFOS_LIMB_MAX];
    /* 0x206 */ Vec3s morphTable[DINOLFOS_LIMB_MAX];
    /* 0x284 */ EnDinofosActionFunc actionFunc;
    /* 0x288 */ u8 envColorAlpha;
    /* 0x289 */ u8 eyeTexIndex;
    /* 0x28A */ u8 drawDmgEffType;
    /* 0x28B */ u8 isDodgingGoronPound;
    /* 0x28C */ s16 targetRotY;
    /* 0x28E */ s16 headRotY;
    /* 0x290 */ union { // multi-use timer
        s16 timer1;
        s16 cutsceneTimer;
        s16 attackTimer;
        s16 actionTimer;
        s16 headTimer;
        s16 stunTimer;
        s16 jumpType;
        s16 isJumpingBackward;
    };
    /* 0x292 */ union { // second multi-use timer
        s16 timer2;
        s16 sidestepTimer;
        s16 walkTimer;
        s16 idleTimer;
    };
    // unk_294 is all zeroes in-game. Leftovers from OoT.
    /* 0x294 */ UNK_TYPE1 unk_294[4];
    /* 0x298 */ s16 subCamId;
    /* 0x29A */ Vec3s subCamRot;
    /* 0x2A0 */ s32 effectIndex;
    /* 0x2A4 */ f32 circlingRate;
    /* 0x2A8 */ f32 subCamAtStep;
    /* 0x2AC */ f32 subCamEyeStep;
    /* 0x2B0 */ f32 drawDmgEffAlpha;
    /* 0x2B4 */ f32 drawDmgEffScale;
    /* 0x2B8 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x2BC */ Vec3f subCamEye;
    /* 0x2C8 */ Vec3f subCamAt;
    /* 0x2D4 */ Vec3f bodyPartsPos[DINOFOS_BODYPART_MAX];
    /* 0x364 */ ColliderJntSph bodyAndFireCollider;
    /* 0x384 */ ColliderJntSphElement bodyAndFireColliderElements[9];
    /* 0x5C4 */ ColliderQuad knifeCollider;
} EnDinofos; // size = 0x644

#endif // Z_EN_DINOFOS_H
