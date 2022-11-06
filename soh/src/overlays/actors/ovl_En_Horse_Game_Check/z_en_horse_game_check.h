#ifndef Z_EN_HORSE_GAME_CHECK_H
#define Z_EN_HORSE_GAME_CHECK_H

#include "ultra64.h"
#include "global.h"

struct EnHorseGameCheckBase;

typedef s32 (*EnHorseGameCheckFunc)(struct EnHorseGameCheckBase* this, PlayState* play);

typedef struct EnHorseGameCheckBase {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ s32 type;
} EnHorseGameCheckBase; // size = 0x0150

typedef struct EnHorseGameCheckIngoRace {
    /* 0x0000 */ EnHorseGameCheckBase base;
    /* 0x0150 */ s32 startFlags;
    /* 0x0154 */ s32 playerCheck[3];
    /* 0x0160 */ s32 ingoCheck[3];
    /* 0x016C */ Actor* ingoHorse;
    /* 0x0170 */ s32 startTimer;
    /* 0x0174 */ s32 result;
    /* 0x0178 */ s32 finishTimer;
    /* 0x017C */ s32 playerFinish;
    /* 0x0180 */ s32 ingoFinish;
} EnHorseGameCheckIngoRace; // size = 0x0184

typedef struct EnHorseGameCheckGerudoArchery {
    /* 0x0000 */ EnHorseGameCheckBase base;
    /* 0x0150 */ s32 unk_150;
    /* 0x0154 */ u32 startTimer;
} EnHorseGameCheckGerudoArchery; // size = 0x0158

typedef struct EnHorseGameCheck3 {
    /* 0x0000 */ EnHorseGameCheckBase base;
    /* 0x0150 */ s32 unk_150;
} EnHorseGameCheck3; // size = 0x0154

typedef struct EnHorseGameCheckMalonRace {
    /* 0x0000 */ EnHorseGameCheckBase base;
    /* 0x0150 */ s32 startTimer;
    /* 0x0154 */ s32 raceFlags;
    /* 0x0158 */ s32 fenceCheck[16];
    /* 0x0198 */ s32 finishTimer;
    /* 0x019C */ s32 result;
    /* 0x01A0 */ s32 lapCount;
} EnHorseGameCheckMalonRace; // size = 0x01A4

typedef union EnHorseGameCheck {
    EnHorseGameCheckIngoRace ingo;
    EnHorseGameCheckGerudoArchery gerudo;
    EnHorseGameCheck3 type3;
    EnHorseGameCheckMalonRace malon;
} EnHorseGameCheck; // size = 0x01A4

typedef enum {
    /* 0 */ HORSEGAME_NONE,
    /* 1 */ HORSEGAME_INGO_RACE,
    /* 2 */ HORSEGAME_GERUDO_ARCHERY,
    /* 3 */ HORSEGAME_TYPE3,
    /* 4 */ HORSEGAME_MALON_RACE
} EnHorseGameCheckType;

#endif
