#ifndef Z_BG_CRACE_MOVEBG_H
#define Z_BG_CRACE_MOVEBG_H

#include "global.h"
#include "assets/objects/object_crace_object/object_crace_object.h"

struct BgCraceMovebg;

typedef void (*BgCraceMovebgActionFunc)(struct BgCraceMovebg*, PlayState*);

#define BG_CRACE_MOVEBG_GET_TYPE(thisx) (((thisx)->params) & 0xF)
#define BG_CRACE_MOVEBG_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 4) & 0x7F)
#define BG_CRACE_MOVEBG_GET_CLOSE_WAIT_TIMER(thisx) (((thisx)->params >> 0xB) & 0x1F)
#define BG_CRACE_MOVEBG_GET_INDEX(thisx) ((thisx)->world.rot.x)
#define BG_CRACE_MOVEBG_GET_OPEN_SPEED(thisx) ((thisx)->world.rot.z)

#define BG_CRACE_MOVEBG_FLAG_BUTLER_IS_BEYOND_DOOR (1 << 0)
#define BG_CRACE_MOVEBG_FLAG_PLAYER_IS_BEYOND_DOOR (1 << 1)
#define BG_CRACE_MOVEBG_FLAG_ALREADY_LOADED (1 << 3)

typedef enum {
    /* 0 */ BG_CRACE_MOVEBG_TYPE_CLOSING,
    /* 1 */ BG_CRACE_MOVEBG_TYPE_OPENING,
    /* 2 */ BG_CRACE_MOVEBG_TYPE_UNUSED_CLOSING // Not triggered to close by the Deku Butler, and triggers a voidout no matter which side of the door they're on.
} BgCraceMovebgType;

typedef struct BgCraceMovebg {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgCraceMovebgActionFunc actionFunc;
    /* 0x160 */ f32 doorHeight;
    /* 0x164 */ f32 targetDoorHeight;
    /* 0x168 */ UNK_TYPE1 pad168[0x4];
    /* 0x16C */ f32 openSpeed;
    /* 0x170 */ s32 stateFlags;
    /* 0x174 */ s32 closeWaitTimer;
    /* 0x178 */ Vec3f intersectionOffsetFromHome;
    /* 0x184 */ u8 index;
    /* 0x188 */ Vec3f prevPlayerPos;
} BgCraceMovebg; // size = 0x194

#endif // Z_BG_CRACE_MOVEBG_H
