#ifndef Z_EN_PST_H
#define Z_EN_PST_H

#include "global.h"
#include "objects/object_pst/object_pst.h"

typedef enum {
    /* 0 */ POSTBOX_SOUTH_UPPER_CLOCKTOWN,
    /* 1 */ POSTBOX_NORTH_CLOCKTOWN,
    /* 2 */ POSTBOX_EAST_UPPER_CLOCKTOWN,
    /* 3 */ POSTBOX_EAST_LOWER_CLOCKTOWN,
    /* 4 */ POSTBOX_SOUTH_LOWER_CLOCKTOWN
} PostboxType;

struct EnPst;

typedef void (*EnPstActionFunc)(struct EnPst*, PlayState*);

typedef struct EnPst {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnPstActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ u8 scheduleResult;
    /* 0x1DC */ MsgScript* msgScript;
    /* 0x1E0 */ s32 msgScriptPos;
    /* 0x1E4 */ Vec3s jointTable[POSTBOX_LIMB_MAX];
    /* 0x1F6 */ Vec3s morphTable[POSTBOX_LIMB_MAX];
    /* 0x208 */ u16 stateFlags;
    /* 0x20A */ UNK_TYPE1 pad20A[4];
    /* 0x20E */ s16 behaviour;
    /* 0x210 */ MsgScriptCallback msgScriptCallback;
    /* 0x214 */ s32 unk214; // Set and not used
    /* 0x218 */ s32 exchangeItemAction;
    /* 0x21C */ s32 isLetterToKafeiDeposited;
} EnPst; // size = 0x220

#endif // Z_EN_PST_H
