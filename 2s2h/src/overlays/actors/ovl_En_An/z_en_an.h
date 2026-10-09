#ifndef Z_EN_AN_H
#define Z_EN_AN_H

#include "global.h"

#include "z64msgevent.h"

#include "assets/objects/object_an1/object_an1.h"

struct EnAn;

typedef void (*EnAnActionFunc)(struct EnAn*, PlayState*);
typedef void (*EnAnDialogueFunc)(struct EnAn*, PlayState*);

#define ENAN_8000 0x8000

#define ENAN_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)
#define ENAN_GET_8000(thisx) ((thisx)->params & ENAN_8000)

#define ENAN_STATE_8                (1 << 3) // path related
#define ENAN_STATE_REACHED_PATH_END (1 << 4)
#define ENAN_STATE_ENGAGED          (1 << 5)
#define ENAN_STATE_IGNORE_GRAVITY   (1 << 6)
#define ENAN_STATE_FACE_TARGET      (1 << 7)
#define ENAN_STATE_UPDATE_EYES      (1 << 8)
#define ENAN_STATE_LOST_ATTENTION   (1 << 9)
#define ENAN_STATE_TALKING          (1 << 10)
#define ENAN_STATE_DRAW_TRAY        (1 << 11)
#define ENAN_STATE_DRAW_UMBRELLA    (1 << 12)
#define ENAN_STATE_DRAW_BROOM       (1 << 13)
#define ENAN_STATE_DRAW_KAFEIS_MASK (1 << 14)
#define ENAN_STATE_DRAW_CHOPSTICKS  (1 << 15)

typedef struct EnAn {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnAnActionFunc actionFunc;
    /* 0x18C */ EnAnDialogueFunc dialogueFunc;
    /* 0x190 */ ColliderCylinder collider;
    /* 0x1DC */ Path* timePath;
    /* 0x1E0 */ Vec3f timePathTargetPos;
    /* 0x1EC */ f32 timePathProgress;
    /* 0x1F0 */ s32 timePathTotalTime;
    /* 0x1F4 */ s32 timePathWaypointTime;
    /* 0x1F8 */ s32 timePathWaypoint;
    /* 0x1FC */ s32 timePathElapsedTime;
    /* 0x200 */ u8 scheduleResult;
    /* 0x204 */ MsgScript* msgScript;
    /* 0x208 */ s8 an2ObjectSlot;
    /* 0x209 */ s8 an3ObjectSlot;
    /* 0x20A */ s8 maskKerfayObjectSlot;
    /* 0x20B */ s8 an4ObjectSlot;
    /* 0x20C */ s8 msmoObjectSlot;
    /* 0x210 */ s32 msgScriptPos;
    /* 0x214 */ s8 roomNum;
    /* 0x215 */ s8 doorOpenTimer;
    /* 0x218 */ Actor* lookAtActor; // interactActor
    /* 0x21C */ UNK_TYPE1 unk_21C[0xC];
    /* 0x228 */ Vec3f doorEntrancePos; // Position of the point just before entering the door
    /* 0x234 */ Vec3f doorExitPos; // Position of the point just after passing through the door
    /* 0x240 */ Vec3f headComputedPos;
    /* 0x24C */ UNK_TYPE1 unk_24C[0xC];
    /* 0x258 */ Vec3s headComputedRot;
    /* 0x25E */ UNK_TYPE1 unk_25E[0x6];
    /* 0x264 */ Vec3s jointTable[ANJU1_LIMB_MAX];
    /* 0x2E2 */ Vec3s morphTable[ANJU1_LIMB_MAX];
    /* 0x360 */ u16 stateFlags;
    /* 0x362 */ u16 prevTextId;
    /* 0x364 */ u8 cueId;
    /* 0x368 */ f32 animPlaySpeed;
    /* 0x36C */ UNK_TYPE1 unk_36C[0x8];
    /* 0x374 */ f32 offerRange;
    /* 0x378 */ s16 doorTimeTotalDiff; // Time difference between the point before going through a door and after going through it
    /* 0x37A */ union {
        s16 doorTimeProgress; // schedule time diff
        s16 laundryPoolState;
        s16 followScheduleState; // unused
    } schState;
    /* 0x37C */ s16 headRotZ;
    /* 0x37E */ s16 headRotY;
    /* 0x380 */ UNK_TYPE1 unk_380[0x4];
    /* 0x384 */ s16 timePathTimeSpeed;
    /* 0x386 */ s16 unk_386; // timer
    /* 0x388 */ s16 loseAttentionTimer;
    /* 0x38A */ s16 savedFaceIndex; // EnAnFace enum
    /* 0x38C */ s16 faceIndex; // EnAnFace enum
    /* 0x38E */ s16 eyeTimer;
    /* 0x390 */ s16 eyeTexIndex;
    /* 0x392 */ s16 mouthTexIndex;
    /* 0x394 */ s16 msgEventState;
    /* 0x396 */ s16 dialogueFuncState;
    /* 0x398 */ MsgScriptCallback msgScriptCallback;
    /* 0x39C */ s32 animIndex; // EnAnAnimation enum
    /* 0x3A0 */ UNK_TYPE1 unk_3A0[0x8];
    /* 0x3A8 */ u32 trayTexScrollTimer1; // TwoTexScroll
    /* 0x3AC */ u32 trayTexScrollTimer2; // TwoTexScroll
    /* 0x3B0 */ s32 forceDraw;
    /* 0x3B4 */ s32 drawMoonMask;
    /* 0x3B8 */ s32 slotsInitialized;
    /* 0x3BC */ s32 unk_3BC;
    /* 0x3C0 */ s32 unk_3C0; // true if ENAN_8000
    /* 0x3C4 */ s32 unk_3C4; // set but not read
} EnAn; // size = 0x3C8

#endif // Z_EN_AN_H
