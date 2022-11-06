#include "ultra64.h"
#include "global.h"

typedef struct {
    s16 val;
    s16 dataType;
} CameraModeValue;

typedef struct {
    s16 funcIdx;
    s16 valueCnt;
    CameraModeValue* values;
} CameraMode;

typedef struct {
    union {
        u32 unk_00;
        struct {
            u32 unk_bit0 : 1;
            u32 unk_bit1 : 1;
            u32 validModes : 30;
        };
    };
    CameraMode* cameraModes;
} CameraSetting;

/*==================================================================*/
// Data
s16 sOREGInit[] = {
    0,   1,   5,  5,  5,   14500, 20,  16, 150, 25,   150, 6,  10, 10, 0,  0,   1,     100,
    250, 120, 80, 30, 120, 4,     1,   50, 20,  1800, 50,  50, 50, 20, 20, -10, -5460, -9100,
    -6,  8,   15, 75, 60,  12,    110, 40, 50,  250,  -10, 30, 30, 70, 20, 20,  20,
};

s16 sOREGInitCnt = 53;

s16 sCamDataRegsInit[CAM_DATA_MAX] = {
    -20, // CAM_DATA_Y_OFFSET
    200, // CAM_DATA_EYE_DIST
    300, // CAM_DATA_EYE_DIST_NEXT
    10,  // CAM_DATA_PITCH_TARGET
    12,  // CAM_DATA_YAW_UPDATE_RATE_TARGET
    10,  // CAM_DATA_XZ_UPDATE_RATE_TARGET
    35,  // CAM_DATA_MAX_YAW_UPDATE
    60,  // CAM_DATA_FOV
    60,  // CAM_DATA_AT_LERP_STEP_SCALE
    3,   // CAM_DATA_FLAGS
    0,   // CAM_DATA_YAW_TARGET
    -40, // CAM_DATA_GROUND_Y_OFFSET
    20,  // CAM_DATA_GROUND_AT_LERP_STEP_SCALE
    25,  // CAM_DATA_SWING_YAW_INIT
    45,  // CAM_DATA_SWING_YAW_FINAL
    -5,  // CAM_DATA_SWING_PITCH_INIT
    15,  // CAM_DATA_SWING_PITCH_FINAL
    15,  // CAM_DATA_SWING_PITCH_ADJ
    20,  // CAM_DATA_MIN_MAX_DIST_FACTOR
    0,   // CAM_DATA_AT_OFFSET_X
    0,   // CAM_DATA_AT_OFFSET_Y
    0,   // CAM_DATA_AT_OFFSET_Z
    6,   // CAM_DATA_UNK_22
    60,  // CAM_DATA_UNK_23
    30,  // CAM_DATA_FOV_SCALE
    0,   // CAM_DATA_YAW_SCALE
    5,   // CAM_DATA_UNK_26
};

s16 sCamDataRegsInitCount = ARRAY_COUNT(sCamDataRegsInit);

char sCameraSettingNames[][12] = {
    "NONE      ",  "NORMAL0    ", "NORMAL1    ", "DUNGEON0   ", "DUNGEON1   ", "NORMAL3    ", "HORSE0     ",
    "BOSS_GOMA  ", "BOSS_DODO  ", "BOSS_BARI  ", "BOSS_FGANON", "BOSS_BAL   ", "BOSS_SHADES", "BOSS_MOFA  ",
    "BOSS_TWIN0 ", "BOSS_TWIN1 ", "BOSS_GANON1", "BOSS_GANON2", "TOWER0     ", "TOWER1     ", "FIXED0     ",
    "FIXED1     ", "CIRCLE0    ", "CIRCLE2    ", "CIRCLE3    ", "PREREND0   ", "PREREND1   ", "PREREND3   ",
    "DOOR0      ", "DOORC      ", "RAIL3      ", "START0     ", "START1     ", "FREE0      ", "FREE2      ",
    "CIRCLE4    ", "CIRCLE5    ", "DEMO0      ", "DEMO1      ", "MORI1      ", "ITEM0      ", "ITEM1      ",
    "DEMO3      ", "DEMO4      ", "UFOBEAN    ", "LIFTBEAN   ", "SCENE0     ", "SCENE1     ", "HIDAN1     ",
    "HIDAN2     ", "MORI2      ", "MORI3      ", "TAKO       ", "SPOT05A    ", "SPOT05B    ", "HIDAN3     ",
    "ITEM2      ", "CIRCLE6    ", "NORMAL2    ", "FISHING    ", "DEMOC      ", "UO_FIBER   ", "DUNGEON2   ",
    "TEPPEN     ", "CIRCLE7    ", "NORMAL4    ",
};

char sCameraModeNames[][12] = {
    "NORMAL     ", "PARALLEL   ", "KEEPON     ", "TALK       ", "BATTLE     ", "CLIMB      ", "SUBJECT    ",
    "BOWARROW   ", "BOWARROWZ  ", "FOOKSHOT   ", "BOOMERANG  ", "PACHINCO   ", "CLIMBZ     ", "JUMP       ",
    "HANG       ", "HANGZ      ", "FREEFALL   ", "CHARGE     ", "STILL      ", "PUSHPULL   ", "BOOKEEPON  ",
};

/**
 *=====================================================================
 *                   General Data: NORMAL0 Setting
 *=====================================================================
 */

CameraModeValue sSetNormal0ModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 200, 300, 10, 12, 10, 35, 60, 60, 0x0003),
};

CameraModeValue sSetNormal0ModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-20, 250, 0, 0, 5, 5, 45, 50, 0x200A, -40, 20),
};

CameraModeValue sSetNormal0ModeFollowTargetData[] = {
    CAM_FUNCDATA_KEEP1(-20, 120, 140, 25, 45, -5, 15, 15, 45, 50, 0x2001, -50, 30),
};

CameraModeValue sSetNormal0ModeTalkData[] = {
    CAM_FUNCDATA_KEEP3(-30, 70, 200, 40, 10, 0, 5, 70, 45, 50, 10, 0x3500),
};

CameraModeValue sSetNormal0ModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 180, 10, 80, 0, 10, 25, 50, 80, 0x2002, -40, 25),
};

CameraModeValue sSetNormal0ModeClimbData[] = {
    CAM_FUNCDATA_JUMP2(-20, 200, 300, 20, 5, 5, 60, 40, 0x0000),
};

CameraModeValue sSetNormal0ModeFirstPersonData[] = {
    CAM_FUNCDATA_SUBJ3(0, 5, 50, 10, 0, 0, 0, 45, 0x0000),
};

CameraModeValue sSetNormal0ModeBowArrowData[] = {
    CAM_FUNCDATA_SUBJ3(-7, 14, 50, 10, 0, -30, -5, 45, 0x2000),
};

CameraModeValue sSetNormal0ModeBowArrowZData[] = {
    CAM_FUNCDATA_SUBJ3(20, 70, 70, 10, -120, 20, 0, 45, 0x2000),
};

CameraModeValue sSetNormal0ModeHookshotData[] = {
    CAM_FUNCDATA_SPEC5_ALT(-20, 80, 250, 45, 60, 40, 6, 0x2000),
};

CameraModeValue sSetNormal0ModeBoomerangData[] = {
    CAM_FUNCDATA_SUBJ3(5, 50, 50, 10, 0, 0, 0, 45, 0x2000),
};

CameraModeValue sSetNormal0ModeSlingshotData[] = {
    CAM_FUNCDATA_SUBJ3(-7, 14, 50, 10, -9, -63, -30, 45, 0x2000),
};

CameraModeValue sSetNormal0ModeClimbZData[] = {
    CAM_FUNCDATA_JUMP2(-20, 200, 300, 20, 999, 5, 60, 40, 0x2006),
};

CameraModeValue sSetNormal0ModeJumpData[] = {
    CAM_FUNCDATA_JUMP1(-20, 200, 300, 12, 35, 60, 40, 0x0000),
};

CameraModeValue sSetNormal0ModeHangData[] = {
    CAM_FUNCDATA_UNIQ1(-80, 200, 300, 40, 60, 10, 0x0000),
};

CameraModeValue sSetNormal0ModeHangZData[] = {
    CAM_FUNCDATA_UNIQ1(-120, 300, 300, 70, 45, 10, 0x2000),
};

CameraModeValue sSetNormal0ModeFreeFallData[] = {
    CAM_FUNCDATA_JUMP1(-20, 200, 300, 15, 80, 60, 20, 0x0000),
};

CameraModeValue sSetNormal0ModeChargeData[] = {
    CAM_FUNCDATA_BATT4(-20, 300, 50, 2, 80, 20, 0xF000),
};

CameraModeValue sSetNormal0ModeStillData[] = {
    CAM_FUNCDATA_NORM1(-20, 200, 300, 10, 100, 10, 100, 60, 5, 0xF003),
};

CameraModeValue sSetNormal0ModePushPullData[] = {
    CAM_FUNCDATA_PARA1(0, 250, 25, 0, 5, 5, 70, 30, 0x206A, -20, 30),
};

CameraModeValue sSetNormal0ModeFollowBoomerangData[] = {
    CAM_FUNCDATA_KEEP1(-5, 120, 140, 5, 85, 10, 5, 25, 45, 50, 0x2001, -15, 30),
};

/**
 *=====================================================================
 *                   Custom Data: NORMAL1 Setting
 *=====================================================================
 */

CameraModeValue sSetNormal1ModeNormalData[] = {
    CAM_FUNCDATA_NORM1(0, 200, 400, 10, 12, 20, 40, 60, 60, 0x0003),
};

CameraModeValue sSetNormal1ModeTargetData[] = {
    CAM_FUNCDATA_PARA1(0, 250, 0, 0, 5, 5, 45, 50, 0x2002, -40, 20),
};

CameraModeValue sSetNormal1ModeFollowTargetData[] = {
    CAM_FUNCDATA_KEEP1(-20, 120, 140, 25, 45, -5, 15, 15, 45, 50, 0x2001, -50, 20),
};

CameraModeValue sSetNormal1ModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 250, 10, 80, 0, 10, 25, 50, 65, 0x2002, -40, 25),
};

CameraModeValue sSetNormal1ModeHookshotData[] = {
    CAM_FUNCDATA_SPEC5(-20, 80, 250, 6, 45, 60, 40, 0x2000),
};

CameraModeValue sSetNormal1ModeJumpData[] = {
    CAM_FUNCDATA_JUMP1(0, 250, 400, 15, 50, 60, 30, 0x0000),
};

CameraModeValue sSetNormal1ModeFreeFallData[] = {
    CAM_FUNCDATA_JUMP1(0, 200, 400, 30, 80, 60, 20, 0x0000),
};

CameraModeValue sSetNormal1ModeClimbData[] = {
    CAM_FUNCDATA_JUMP2(-20, 200, 400, 20, 5, 5, 60, 40, 0x0000),
};

CameraModeValue sSetNormal1ModeClimbZData[] = {
    CAM_FUNCDATA_JUMP2(-20, 250, 400, 20, 999, 5, 60, 40, 0x2006),
};

CameraModeValue sSetNormal1ModeChargeData[] = {
    CAM_FUNCDATA_BATT4(0, 300, 50, 2, 80, 20, 0xF000),
};

CameraModeValue sSetNormal1ModeHangData[] = {
    CAM_FUNCDATA_UNIQ1(-80, 200, 400, 40, 60, 10, 0x0000),
};

CameraModeValue sSetNormal1ModeHangZData[] = {
    CAM_FUNCDATA_UNIQ1(-120, 400, 400, 70, 45, 10, 0x2000),
};

CameraModeValue sSetNormal1ModeStillData[] = {
    CAM_FUNCDATA_NORM1(0, 200, 400, 10, 100, 20, 100, 60, 5, 0xF003),
};

/**
 *=====================================================================
 *                   Custom Data: DUNGEON0 Setting
 *=====================================================================
 */

CameraModeValue sSetDungeon0ModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-10, 150, 250, 5, 10, 5, 30, 60, 60, 0x0003),
};

CameraModeValue sSetDungeon0ModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-20, 150, 0, 0, 5, 5, 45, 50, 0x200A, -40, 20),
};

CameraModeValue sSetDungeon0ModeFollowTargetData[] = {
    CAM_FUNCDATA_KEEP1(-20, 120, 140, 25, 45, -5, 15, 15, 45, 50, 0x2001, -40, 20),
};

CameraModeValue sSetDungeon0ModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 180, 10, 80, 0, 10, 25, 45, 80, 0x2002, -40, 25),
};

CameraModeValue sSetDungeon0ModeJumpData[] = {
    CAM_FUNCDATA_JUMP1(-10, 150, 250, 10, 50, 60, 40, 0x0000),
};

CameraModeValue sSetDungeon0ModeFreeFallData[] = {
    CAM_FUNCDATA_JUMP1(-10, 150, 250, 10, 80, 60, 20, 0x0000),
};

CameraModeValue sSetDungeon0ModeClimbData[] = {
    CAM_FUNCDATA_JUMP2(-40, 150, 250, 20, 5, 5, 60, 40, 0x0000),
};

CameraModeValue sSetDungeon0ModeClimbZData[] = {
    CAM_FUNCDATA_JUMP2(-40, 250, 250, 20, 999, 5, 60, 40, 0x2006),
};

CameraModeValue sSetDungeon0ModeChargeData[] = {
    CAM_FUNCDATA_BATT4(-10, 300, 50, 2, 80, 20, 0xF000),
};

CameraModeValue sSetDungeon0ModeHangData[] = {
    CAM_FUNCDATA_UNIQ1(-80, 150, 250, 40, 60, 10, 0x0000),
};

CameraModeValue sSetDungeon0ModeHangZData[] = {
    CAM_FUNCDATA_UNIQ1(-120, 250, 250, 70, 45, 10, 0x2000),
};

CameraModeValue sSetDungeon0ModeStillData[] = {
    CAM_FUNCDATA_NORM1(-10, 150, 250, 5, 100, 5, 100, 60, 5, 0xF003),
};

/**
 *=====================================================================
 *                   Custom Data: DUNGEON1 Setting
 *=====================================================================
 */

CameraModeValue sSetDungeon1ModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-40, 150, 150, 0, 10, 5, 30, 60, 60, 0x0003),
};

CameraModeValue sSetDungeon1ModeTalkData[] = {
    CAM_FUNCDATA_KEEP3(-20, 70, 200, 40, 10, 0, 5, 70, 45, 50, 10, 0x3500),
};

CameraModeValue sSetDungeon1ModeJumpData[] = {
    CAM_FUNCDATA_JUMP1(-40, 150, 150, 10, 50, 60, 40, 0x0000),
};

CameraModeValue sSetDungeon1ModeFreeFallData[] = {
    CAM_FUNCDATA_JUMP1(-40, 150, 180, 12, 80, 60, 20, 0x0000),
};

CameraModeValue sSetDungeon1ModeClimbData[] = {
    CAM_FUNCDATA_JUMP2(-40, 150, 150, 20, 5, 5, 60, 40, 0x0000),
};

CameraModeValue sSetDungeon1ModeClimbZData[] = {
    CAM_FUNCDATA_JUMP2(-40, 150, 150, 20, 999, 5, 60, 40, 0x2006),
};

CameraModeValue sSetDungeon1ModeChargeData[] = {
    CAM_FUNCDATA_BATT4(-40, 200, 50, 2, 80, 20, 0xF000),
};

CameraModeValue sSetDungeon1ModeHangData[] = {
    CAM_FUNCDATA_UNIQ1(-80, 150, 150, 40, 60, 10, 0x0000),
};

CameraModeValue sSetDungeon1ModeHangZData[] = {
    CAM_FUNCDATA_UNIQ1(-120, 150, 150, 70, 45, 10, 0x2000),
};

CameraModeValue sSetDungeon1ModeStillData[] = {
    CAM_FUNCDATA_NORM1(-40, 150, 150, 0, 100, 5, 100, 60, 5, 0xF003),
};

CameraModeValue sSetDungeon1ModePushPullData[] = {
    CAM_FUNCDATA_PARA1(-40, 180, 25, 0, 5, 5, 60, 50, 0x206A, -20, 30),
};

/**
 *=====================================================================
 *                   Custom Data: NORMAL3 Setting
 *=====================================================================
 */

CameraModeValue sSetNormal3ModeNormalData[] = {
    CAM_FUNCDATA_JUMP3(-20, 280, 300, 20, 15, 5, 40, 60, 100, 0x0004),
};

CameraModeValue sSetNormal3ModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-50, 250, 70, 0, 15, 5, 60, 100, 0x200A, -50, 20),
};

CameraModeValue sSetNormal3ModeTalkData[] = {
    CAM_FUNCDATA_KEEP3(-30, 70, 200, 40, 10, 10, 20, 70, 45, 10, 10, 0x3500),
};

/* These values are for when the eye
 * >= OREG(45) units below the surface of the water.
 */
CameraModeValue sSetNormal3ModeBoomerangData[] = {
    CAM_FUNCDATA_JUMP3(-40, 150, 250, -5, 18, 5, 60, 60, 40, 0x0005),
};

/**
 *=====================================================================
 *                   Custom Data: HORSE Setting
 *=====================================================================
 */

CameraModeValue sSetHorseModeNormalData[] = {
    CAM_FUNCDATA_NORM3(-50, 220, 250, 10, 16, 20, 60, 100, 0x0600),
};

CameraModeValue sSetHorseModeTargetData[] = {
    CAM_FUNCDATA_NORM3(-40, 180, 220, -2, 12, 100, 45, 100, 0x2600),
};

CameraModeValue sSetHorseModeBowArrowData[] = {
    CAM_FUNCDATA_SUBJ3(-7, 14, 100, 10, 0, -30, -5, 40, 0x2600),
};

CameraModeValue sSetHorseModeFollowTargetData[] = {
    CAM_FUNCDATA_KEEP1(-60, 180, 220, 25, 45, -5, 15, 15, 45, 50, 0x2601, -60, 20),
};

CameraModeValue sSetHorseModeTalkData[] = {
    CAM_FUNCDATA_KEEP3(-60, 140, 200, 40, 10, 0, 5, 70, 45, 50, 10, 0x3500),
};

/**
 *=====================================================================
 *                   Custom Data: BOSS_GOHMA Setting
 *=====================================================================
 */

CameraModeValue sSetBossGohmaModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 150, 250, 0, 15, 5, 40, 60, 60, 0x0001),
};

CameraModeValue sSetBossGohmaModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-30, 150, 10, 40, -10, 0, 25, 60, 40, 0x2002, -50, 20),
};

/**
 *=====================================================================
 *                 Custom Data: BOSS_DODONGO Setting
 *=====================================================================
 */

CameraModeValue sSetBossDodongoModeNormalData[] = {
    CAM_FUNCDATA_NORM1(0, 150, 300, 0, 12, 5, 70, 70, 40, 0x0003),
};

CameraModeValue sSetBossDodongoModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 160, 10, 60, -5, 0, 25, 70, 50, 0x2002, -40, 20),
};

/**
 *=====================================================================
 *                Custom Data: BOSS_BARINADE Setting
 *=====================================================================
 */

CameraModeValue sSetBossBarinadeModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 150, 300, -5, 15, 5, 40, 70, 70, 0x0003),
};

CameraModeValue sSetBossBarinadeModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-30, 125, 10, 10, 0, 0, 50, 60, 50, 0x2002, -50, 20),
};

/**
 *=====================================================================
 *              Custom Data: BOSS_PHANTOM_GANON Setting
 *=====================================================================
 */

CameraModeValue sSetBossPhantomGanonModeNormalData[] = {
    CAM_FUNCDATA_NORM1(10, 150, 250, 0, 15, 15, 40, 60, 100, 0x0003),
};

CameraModeValue sSetBossPhantomGanonModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 200, 45, 40, 5, -5, 35, 60, 100, 0x2002, -40, 60),
};

/**
 *=====================================================================
 *                Custom Data: BOSS_VOLVAGIA Setting
 *=====================================================================
 */

CameraModeValue sSetBossVolvagiaModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 10, 16, 10, 40, 60, 80, 0x0003),
};

CameraModeValue sSetBossVolvagiaModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 200, 20, 60, 0, 10, 15, 45, 50, 0x2002, -40, 20),
};

/**
 *=====================================================================
 *                   Custom Data: BOSS_BONGO Setting
 *=====================================================================
 */

CameraModeValue sSetBossBongoModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 10, 20, 10, 40, 60, 80, 0x0083),
};

CameraModeValue sSetBossBongoModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 200, 20, 60, 0, 10, 15, 45, 50, 0x2082, -40, 20),
};

CameraModeValue sSetBossBongoModeJumpData[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 10, 20, 10, 80, 60, 80, 0x0083),
};

/**
 *=====================================================================
 *                 Custom Data: BOSS_MORPHA Setting
 *=====================================================================
 */

CameraModeValue sSetBossMorphaModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 100, 150, -10, 15, 10, 40, 80, 60, 0x0003),
};

CameraModeValue sSetBossMorphaModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 200, 10, 80, -10, 10, 25, 70, 40, 0x2002, -40, 20),
};

/**
 *=====================================================================
 *                  Custom Data: TWINROVA Setting
 *=====================================================================
 */

CameraModeValue sSetBossTwinrovaPlatformModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 150, 300, 0, 20, 10, 40, 60, 80, 0x0003),
};

CameraModeValue sSetBossTwinrovaModeBattleData[] = {
    CAM_FUNCDATA_BATT1(0, 400, 0, 60, -10, 5, 25, 45, 40, 0x2002, -20, 20),
};

CameraModeValue sSetBossTwinrovaFloorModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-10, 150, 200, -10, 12, 10, 40, 60, 50, 0x0003),
};

/**
 *=====================================================================
 *                Custom Data: BOSS_GANONDORF Setting
 *=====================================================================
 */

CameraModeValue sSetBossGanondorfModeNormalData[] = {
    CAM_FUNCDATA_NORM1(40, 330, 330, -5, 15, 15, 40, 60, 100, 0x0000),
};

CameraModeValue sSetBossGanondorfModeChargeData[] = {
    CAM_FUNCDATA_BATT4(-40, 250, 0, 2, 80, 20, 0xF000),
};

/**
 *=====================================================================
 *                 Custom Data: BOSS_GANON Setting
 *=====================================================================
 */

CameraModeValue sSetBossGanonModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 10, 20, 10, 40, 60, 80, 0x0003),
};

CameraModeValue sSetBossGanonModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 180, 20, 60, 0, 10, 25, 45, 50, 0x2002, -40, 20),
};

/**
 *=====================================================================
 *                 Custom Data: TOWER_CLIMB Setting
 *=====================================================================
 */

CameraModeValue sSetTowerClimbModeNormalData[] = {
    CAM_FUNCDATA_NORM2(0, 120, 280, 60, 8, 40, 60, 50, 0x0000),
};

CameraModeValue sSetTowerClimbModeJumpData[] = {
    CAM_FUNCDATA_NORM2(0, 120, 280, 60, 8, 40, 60, 50, 0x0080),
};

/**
 *=====================================================================
 *                Custom Data: TOWER_UNUSED Setting
 *=====================================================================
 */

CameraModeValue sSetTowerUnusedModeNormalData[] = {
    CAM_FUNCDATA_NORM2(0, 270, 300, 120, 8, 60, 60, 100, 0x0000),
};

CameraModeValue sSetTowerUnusedModeJumpData[] = {
    CAM_FUNCDATA_NORM2(0, 270, 300, 120, 6, 60, 60, 100, 0x0000),
};

/**
 *=====================================================================
 *                Custom Data: MARKET_BALCONY Setting
 *=====================================================================
 */

CameraModeValue sSetMarketBalconyModeNormalData[] = {
    CAM_FUNCDATA_FIXD1(-40, 100, 60, 0x0000),
};

CameraModeValue sSetMarketBalconyModeFollowTargetData[] = {
    CAM_FUNCDATA_FIXD1(-40, 100, 60, 0x2000),
};

CameraModeValue sSetMarketBalconyModeTalkData[] = {
    CAM_FUNCDATA_FIXD1(-40, 100, 60, 0x3500),
};

/**
 *=====================================================================
 *                 Custom Data: CHU_BOWLING Setting
 *=====================================================================
 */

CameraModeValue sSetChuBowlingModeNormalData[] = {
    CAM_FUNCDATA_FIXD1(-40, 25, 60, 0x0000),
};

/**
 *=====================================================================
 *               Custom Data: PIVOT_CRAWLSPACE Setting
 *=====================================================================
 */

CameraModeValue sSetPivotCrawlspaceModeNormalData[] = {
    CAM_FUNCDATA_FIXD2(-40, 50, 80, 60, 0x0001),
};

/**
 *=====================================================================
 *              Custom Data: PIVOT_SHOP_BROWSING Setting
 *=====================================================================
 */

CameraModeValue sSetPivotShopBrowsingModeNormalData[] = {
    CAM_FUNCDATA_DATA4(-40, 60, 0x3F00),
};

/**
 *=====================================================================
 *          Custom Data: PIVOT_IN_FRONT and PIVOT_FROM_SIDE
 *=====================================================================
 */

CameraModeValue sSetPivotInFrontAndFromSideModeNormalData[] = {
    CAM_FUNCDATA_FIXD4(-40, 50, 80, 60, 0x0004),
};

/**
 *=====================================================================
 *                 Custom Data: No data, all flags off
 *=====================================================================
 */

CameraModeValue sDataOnlyNullFlags[] = {
    CAM_FUNCDATA_FLAGS(0x0000),
};

/**
 *=====================================================================
 *                 Custom Data: PREREND_FIXED Setting
 *=====================================================================
 */

CameraModeValue sSetPrerendFixedModeFollowTargetData[] = {
    CAM_FUNCDATA_FLAGS(0x2000),
};

/**
 *=====================================================================
 *                Custom Data: PREREND_PIVOT Setting
 *=====================================================================
 */

CameraModeValue sSetPrerendPivotModeNormalData[] = {
    CAM_FUNCDATA_UNIQ7(60, 0x0000),
};

CameraModeValue sSetPrerendPivotModeFollowTargetData[] = {
    CAM_FUNCDATA_UNIQ7(60, 0x2000),
};

CameraModeValue sSetPrerendPivotModeTalkData[] = {
    CAM_FUNCDATA_KEEP0(30, 0, 4, 0x3500),
};

/**
 *=====================================================================
 *                   Custom Data: DOOR0 Setting
 *=====================================================================
 */

CameraModeValue sSetDoor0ModeNormalData[] = {
    CAM_FUNCDATA_UNIQ3(-40, 60, 0x3200),
};

/**
 *=====================================================================
 *                   Custom Data: DOORC Setting
 *=====================================================================
 */

CameraModeValue sSetDoorCModeNormalData[] = {
    CAM_FUNCDATA_SPEC9(-5, 60, 0x3202),
};

CameraModeValue sSetDoorCModeTargetData[] = {
    CAM_FUNCDATA_SPEC9(-5, 60, 0x320A),
};

/**
 *=====================================================================
 *                   Custom Data: CRAWLSPACE Setting
 *=====================================================================
 */

// Camera_Subj4 only reads one setting which is used for flags
CameraModeValue sSetCrawlspaceModeNormalData[] = {
    CAM_FUNCDATA_SUBJ4(0x0000, 2, 30, 10, 45, 0x3200),
};

/**
 *=====================================================================
 *                   Custom Data: START1 Setting
 *=====================================================================
 */

CameraModeValue sSetStart1ModeNormalData[] = {
    CAM_FUNCDATA_FLAGS(0x0001),
};

/**
 *=====================================================================
 *                   Custom Data: FREE0 Setting
 *=====================================================================
 */

CameraModeValue sSetFree0ModeNormalData[] = {
    CAM_FUNCDATA_FLAGS(0xFF00),
};

/**
 *=====================================================================
 *                   Custom Data: FREE1 Setting
 *=====================================================================
 */

CameraModeValue sSetFree1ModeNormalData[] = {
    CAM_FUNCDATA_FLAGS(0xFF01),
};

/**
 *=====================================================================
 *                 Custom Data: PIVOT_CORNER Setting
 *=====================================================================
 */

CameraModeValue sSetPivotCornerModeNormalData[] = {
    CAM_FUNCDATA_FIXD2(-40, 100, 80, 60, 0x0000),
};

/**
 *=====================================================================
 *              Custom Data: PIVOT_WATER_SURFACE Setting
 *=====================================================================
 */

CameraModeValue sSetPivotWaterSurfaceModeNormalData[] = {
    CAM_FUNCDATA_UNIQ2(-40, 60, 60, 0x0002),
};

CameraModeValue sSetPivotWaterSurfaceModeTargetData[] = {
    CAM_FUNCDATA_UNIQ2(-30, 45, 100, 0x2001),
};

/**
 *=====================================================================
 *               Custom Data: Various cutscene settings
 *=====================================================================
 */

CameraModeValue sDataOnlyInterfaceFlags[] = {
    CAM_FUNCDATA_FLAGS(0x3200),
};

/**
 *=====================================================================
 *                Custom Data: FOREST_BIRDS_EYE Setting
 *=====================================================================
 */

CameraModeValue sSetForestBirdsEyeModeNormalData[] = {
    CAM_FUNCDATA_PARA1(-50, 450, 40, 180, 5, 5, 70, 30, 0x000C, -50, 20),
};

CameraModeValue sSetForestBirdsEyeModeTalkData[] = {
    CAM_FUNCDATA_FLAGS(0x3501),
};

/**
 *=====================================================================
 *                 Custom Data: SLOW_CHEST_CS Setting
 *=====================================================================
 */

// Also set to but unused by function Demo4
CameraModeValue sSetSlowChestCsModeNormalData[] = {
    CAM_FUNCDATA_DEMO3(60, 30, 0x3200),
};

/**
 *=====================================================================
 *                     Custom Data: CS_3 Setting
 *=====================================================================
 */

CameraModeValue sSetCs3ModeNormalData[] = {
    CAM_FUNCDATA_FLAGS(0x3212),
};

/**
 *=====================================================================
 *                 Custom Data: BEAN_GENERIC Setting
 *=====================================================================
 */

CameraModeValue sSetBeanGenericModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-50, 300, 300, 50, 20, 10, 50, 70, 40, 0x0002),
};

CameraModeValue sSetBeanGenericModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-50, 300, 10, 0, 5, 5, 45, 50, 0x200A, -40, 20),
};

CameraModeValue sSetBeanGenericModeJumpData[] = {
    CAM_FUNCDATA_JUMP1(-50, 300, 300, 12, 35, 60, 40, 0x0000),
};

CameraModeValue sSetBeanGenericModeHangData[] = {
    CAM_FUNCDATA_UNIQ1(-80, 300, 300, 60, 70, 30, 0x0000),
};

CameraModeValue sSetBeanGenericModeHangZData[] = {
    CAM_FUNCDATA_UNIQ1(-120, 300, 300, 70, 50, 30, 0x2000),
};

CameraModeValue sSetBeanGenericModeStillData[] = {
    CAM_FUNCDATA_NORM1(-20, 300, 350, 50, 100, 10, 100, 70, 30, 0xF002),
};

/**
 *=====================================================================
 *                Custom Data: BEAN_LOST_WOODS Setting
 *=====================================================================
 */

CameraModeValue sSetBeanLostWoodsModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-50, 200, 200, 20, 16, 10, 50, 60, 50, 0x0002),
};

CameraModeValue sSetBeanLostWoodsModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-50, 200, 40, 0, 5, 5, 45, 50, 0x200A, -40, 20),
};

CameraModeValue sSetBeanLostWoodsModeJumpData[] = {
    CAM_FUNCDATA_JUMP1(-50, 150, 250, 12, 35, 60, 40, 0x0000),
};

CameraModeValue sSetBeanLostWoodsModeHangData[] = {
    CAM_FUNCDATA_UNIQ1(-80, 200, 200, 40, 60, 30, 0x0000),
};

CameraModeValue sSetBeanLostWoodsModeHangZData[] = {
    CAM_FUNCDATA_UNIQ1(-120, 200, 200, 60, 50, 30, 0x2000),
};

CameraModeValue sSetBeanLostWoodsModeStillData[] = {
    CAM_FUNCDATA_NORM1(-20, 200, 250, 20, 100, 10, 100, 60, 30, 0xF002),
};

/**
 *=====================================================================
 *                 Custom Data: SCENE_UNUSED Setting
 *=====================================================================
 */

CameraModeValue sSetSceneUnusedModeNormalData[] = {
    CAM_FUNCDATA_SPEC9(-30, 60, 0x010A),
};

/**
 *=====================================================================
 *                Custom Data: SCENE_TRANSITION Setting
 *=====================================================================
 */

CameraModeValue sSetSceneTransitionModeNormalData[] = {
    CAM_FUNCDATA_UNIQ2(-20, 150, 60, 0x0210),
};

/**
 *=====================================================================
 *                   Custom Data: BIG_OCTO Setting
 *=====================================================================
 */

CameraModeValue sSetBigOctoModeNormalData[] = {
    CAM_FUNCDATA_NORM1(0, 400, 500, 35, 14, 5, 20, 60, 40, 0x0012),
};

CameraModeValue sSetBigOctoModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 250, 5, 10, 30, 20, 25, 45, 60, 0x2002, -40, 25),
};

CameraModeValue sSetBigOctoModeStillData[] = {
    CAM_FUNCDATA_NORM1(0, 300, 500, 60, 8, 5, 60, 60, 30, 0x0012),
};

/**
 *=====================================================================
 *               Custom Data: MEADOW_BIRDS_EYE Setting
 *=====================================================================
 */

CameraModeValue sSetMeadowBirdsEyeModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 80, 20, 10, 70, 70, 80, 0x0012),
};

CameraModeValue sSetMeadowBirdsEyeModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-20, 500, 80, 0, 5, 5, 70, 80, 0x201A, -40, 40),
};

CameraModeValue sSetMeadowBirdsEyeModeBattleData[] = {
    CAM_FUNCDATA_PARA1(-20, 500, 80, 0, 5, 5, 60, 80, 0x201A, -40, 40),
};

CameraModeValue sSetMeadowBirdsEyeModeClimbData[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 80, 20, 10, 80, 60, 20, 0x0012),
};

/**
 *=====================================================================
 *                 Custom Data: MEADOW_UNUSED Setting
 *=====================================================================
 */

CameraModeValue sSetMeadowUnusedModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 750, 750, 80, 20, 10, 70, 70, 80, 0x0012),
};

CameraModeValue sSetMeadowUnusedModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-20, 750, 80, 0, 5, 5, 70, 80, 0x201A, -40, 40),
};

CameraModeValue sSetMeadowUnusedModeBattleData[] = {
    CAM_FUNCDATA_PARA1(-20, 750, 80, 0, 5, 5, 70, 80, 0x200A, -40, 40),
};

CameraModeValue sSetMeadowUnusedModeClimbData[] = {
    CAM_FUNCDATA_NORM1(-20, 750, 750, 80, 20, 10, 80, 70, 20, 0x0012),
};

/**
 *=====================================================================
 *                Custom Data: FIRE_BIRDS_EYE Setting
 *=====================================================================
 */

CameraModeValue sSetFireBirdsEyeModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 80, 20, 10, 70, 70, 80, 0x0002),
};

CameraModeValue sSetFireBirdsEyeModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-20, 500, 80, 0, 5, 5, 70, 80, 0x200A, -40, 40),
};

CameraModeValue sSetFireBirdsEyeModeBattleData[] = {
    CAM_FUNCDATA_PARA1(-20, 500, 80, 0, 5, 5, 60, 80, 0x200A, -40, 40),
};

CameraModeValue sSetFireBirdsEyeModeClimbData[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 80, 20, 10, 80, 60, 20, 0x0002),
};

/**
 *=====================================================================
 *                Custom Data: TURN_AROUND Setting
 *=====================================================================
 */

CameraModeValue sSetTurnAroundModeNormalData[] = {
    CAM_FUNCDATA_KEEP4(-30, 120, -10, 170, 0, 60, 0x2502, 25, 6),
};

/**
 *=====================================================================
 *                Custom Data: PIVOT_VERTICAL Setting
 *=====================================================================
 */

CameraModeValue sSetPivotVerticalModeNormalData[] = {
    CAM_FUNCDATA_SPEC0(20, 0x3200),
};

/**
 *=====================================================================
 *                 Custom Data: NORMAL2/4 Setting
 *=====================================================================
 */

CameraModeValue sSetNormal2and4ModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 200, 300, 10, 12, 10, 35, 60, 60, 0x0002),
};

/**
 *=====================================================================
 *                   Custom Data: FISHING Setting
 *=====================================================================
 */

CameraModeValue sSetFishingModeNormalData[] = {
    CAM_FUNCDATA_NORM1(0, 200, 300, 20, 12, 10, 35, 55, 60, 0x0F02),
};

CameraModeValue sSetFishingModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-20, 250, 0, 0, 5, 5, 45, 50, 0x2F0A, -40, 20),
};

CameraModeValue sSetFishingModeFollowTargetData[] = {
    CAM_FUNCDATA_BATT1(-20, 250, 0, 80, 0, 0, 25, 55, 80, 0x2F02, -40, 25),
};

CameraModeValue sSetFishingModeTalkData[] = {
    CAM_FUNCDATA_KEEP3(-30, 70, 200, 40, 10, 0, 5, 70, 45, 50, 10, 0x3F20),
};

CameraModeValue sSetFishingModeFirstPersonData[] = {
    CAM_FUNCDATA_SUBJ3(0, 5, 50, 10, 0, 0, 0, 45, 0x0F00),
};

CameraModeValue sSetFishingModeJumpData[] = {
    CAM_FUNCDATA_JUMP1(-20, 200, 300, 12, 35, 60, 40, 0x0F00),
};

CameraModeValue sSetFishingModeFreeFallData[] = {
    CAM_FUNCDATA_JUMP1(-20, 200, 300, 15, 80, 60, 20, 0x0F00),
};

CameraModeValue sSetFishingModeHangData[] = {
    CAM_FUNCDATA_UNIQ1(-80, 200, 300, 40, 60, 10, 0x0F00),
};

CameraModeValue sSetFishingModeHangZData[] = {
    CAM_FUNCDATA_UNIQ1(-120, 300, 300, 70, 45, 10, 0x2F00),
};

/**
 *=====================================================================
 *                   Custom Data: CS_C Setting
 *=====================================================================
 */

CameraModeValue sSetCsCModeNormalData[] = {
    CAM_FUNCDATA_FLAGS(0x3F00),
};

/**
 *=====================================================================
 *                   Custom Data: JABU_TENTACLE Setting
 *=====================================================================
 */

CameraModeValue sSetJabuTentacleModeNormalData[] = {
    CAM_FUNCDATA_NORM1_ALT(30, 200, 300, -20, 15, 5, 50, 70, 70, 0x0003),
};

CameraModeValue sSetJabuTentacleModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-30, 160, 10, 10, 0, 0, 70, 60, 40, 0x2002, -50, 20),
};

/**
 *=====================================================================
 *                   Custom Data: DUNGEON2 Setting
 *=====================================================================
 */

CameraModeValue sSetDungeon2ModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-20, 350, 350, 20, 15, 5, 30, 60, 60, 0x0003),
};

CameraModeValue sSetDungeon2ModeTargetData[] = {
    CAM_FUNCDATA_PARA1(-20, 200, 0, 0, 5, 5, 45, 50, 0x200A, -40, 20),
};

CameraModeValue sSetDungeon2ModeBattleData[] = {
    CAM_FUNCDATA_BATT1(-20, 180, 10, 80, -10, 10, 25, 45, 80, 0x2002, -40, 25),
};

CameraModeValue sSetDungeon2ModeJumpData[] = {
    CAM_FUNCDATA_JUMP1(-20, 350, 350, 10, 50, 60, 40, 0x0000),
};

CameraModeValue sSetDungeon2ModeFreeFallData[] = {
    CAM_FUNCDATA_JUMP1(-20, 350, 350, 15, 80, 60, 20, 0x0000),
};

CameraModeValue sSetDungeon2ModeClimbData[] = {
    CAM_FUNCDATA_JUMP2(-40, 350, 350, 20, 5, 5, 60, 40, 0x0000),
};

CameraModeValue sSetDungeon2ModeClimbZData[] = {
    CAM_FUNCDATA_JUMP2(-40, 350, 350, 20, 999, 5, 60, 40, 0x2006),
};

CameraModeValue sSetDungeon2ModeHangData[] = {
    CAM_FUNCDATA_UNIQ1(-50, 350, 350, 40, 60, 10, 0x0000),
};

CameraModeValue sSetDungeon2ModeHangZData[] = {
    CAM_FUNCDATA_UNIQ1(-100, 350, 350, 70, 45, 10, 0x2000),
};

CameraModeValue sSetDungeon2ModeStillData[] = {
    CAM_FUNCDATA_NORM1(-10, 350, 350, 20, 100, 5, 100, 60, 5, 0xF003),
};

CameraModeValue sSetDungeon2ModePushPullData[] = {
    CAM_FUNCDATA_PARA1(0, 280, 25, 0, 5, 5, 70, 30, 0x206A, -20, 30),
};

/**
 *=====================================================================
 *                 Custom Data: DIRECTED_YAW Setting
 *=====================================================================
 */

CameraModeValue sSetDirectedYawModeNormalData[] = {
    CAM_FUNCDATA_NORM1(-10, 280, 320, -8, 20, 10, 80, 60, 80, 0x0002),
};

CameraModeValue sSetDirectedYawModeFollowTargetData[] = {
    CAM_FUNCDATA_KEEP1(-20, 180, 200, 35, 45, 0, -5, 20, 50, 50, 0x2001, -50, 30),
};

CameraModeValue sSetDirectedYawModeTalkData[] = {
    CAM_FUNCDATA_KEEP3(-80, 200, 250, 30, 10, -8, -8, 30, 50, 50, 10, 0x3520),
};

/**
 *=====================================================================
 *                   Custom Data: NORMAL4 Setting
 *=====================================================================
 */

CameraModeValue sNormal4ModeTalkData[] = {
    CAM_FUNCDATA_KEEP3(-30, 70, 200, 40, 10, 0, 5, 70, 45, 50, 10, 0x35A0),
};

/**
 * =====================================================================
 *    CAMERA SETTINGS: USAGE OF FUNCTIONS AND DATA FOR SPECIFIC MODES
 * =====================================================================
 */

#define CAM_SETTING_MODE_ENTRY(func, data) \
    { func, ARRAY_COUNT(data), data }

CameraMode sCamSetNormal0Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal0ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeFreeFallData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetNormal1Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal1ModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal1ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal1ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal1ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal1ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal1ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal1ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal1ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal1ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal1ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal1ModeFreeFallData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal1ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal1ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetDungeon0Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetDungeon0ModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetDungeon0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetDungeon0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetDungeon0ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetDungeon0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal1ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetDungeon0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetDungeon0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetDungeon0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetDungeon0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetDungeon0ModeFreeFallData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetDungeon0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetDungeon0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetDungeon1Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetDungeon1ModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetDungeon0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal1ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetDungeon1ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetDungeon0ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetDungeon1ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal1ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetDungeon1ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetDungeon1ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetDungeon1ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetDungeon1ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetDungeon1ModeFreeFallData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetDungeon1ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetDungeon1ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetDungeon1ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetNormal3Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP3, sSetNormal3ModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal3ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal3ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal0ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP3, sSetNormal3ModeBoomerangData),
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
};

CameraMode sCamSetHorseModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM3, sSetHorseModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM3, sSetHorseModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetHorseModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetHorseModeTalkData),
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetHorseModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
};

CameraMode sCamSetBossGohmaModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossGohmaModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossGohmaModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetDungeon0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossDodongoModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossDodongoModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossDodongoModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossBarinadeModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossBarinadeModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossBarinadeModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossPhantomGanonModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossPhantomGanonModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossPhantomGanonModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossVolvagiaModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossVolvagiaModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossVolvagiaModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossBongoModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossBongoModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossBongoModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossBongoModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossBongoModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossMorphaModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossMorphaModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossMorphaModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossTwinrovaPlatformModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossTwinrovaPlatformModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossTwinrovaModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossTwinrovaFloorModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossTwinrovaFloorModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossTwinrovaModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossGanondorfModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossGanondorfModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossPhantomGanonModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetBossGanondorfModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBossGanonModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBossGanonModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBossGanonModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetTowerClimbModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM2, sSetTowerClimbModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal0ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM2, sSetTowerClimbModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetTowerUnusedModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM2, sSetTowerUnusedModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal0ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM2, sSetTowerUnusedModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetMarketBalconyModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD1, sSetMarketBalconyModeNormalData),
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD1, sSetMarketBalconyModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD1, sSetMarketBalconyModeTalkData),
};

CameraMode sCamSetChuBowlingModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD1, sSetChuBowlingModeNormalData),
};

CameraMode sCamSetPivotCrawlspaceModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD2, sSetPivotCrawlspaceModeNormalData),
};

CameraMode sCamSetPivotShopBrowsingModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DATA4, sSetPivotShopBrowsingModeNormalData),
};

CameraMode sCamSetPivotInFrontModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD4, sSetPivotInFrontAndFromSideModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal1ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetPreRendFixedModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD3, sDataOnlyNullFlags),
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD3, sSetPrerendFixedModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD3, sSetPrerendFixedModeFollowTargetData),
};

CameraMode sCamSetPreRendPivotModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ7, sSetPrerendPivotModeNormalData),
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ7, sSetPrerendPivotModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP0, sSetPrerendPivotModeTalkData),
};

CameraMode sCamSetPreRendSideScrollModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC6, sDataOnlyNullFlags),
};

CameraMode sCamSetDoor0Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ3, sSetDoor0ModeNormalData),
};

CameraMode sCamSetDoorCModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC9, sSetDoorCModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC9, sSetDoorCModeTargetData),
};

CameraMode sCamSetCrawlspaceModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ4, sSetCrawlspaceModeNormalData),
};

CameraMode sCamSetStart0Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ0, sDataOnlyNullFlags),
};

CameraMode sCamSetStart1Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ0, sSetStart1ModeNormalData),
};

CameraMode sCamSetFree0Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ6, sSetFree0ModeNormalData),
};

CameraMode sCamSetFree1Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ6, sSetFree1ModeNormalData),
};

CameraMode sCamSetPivotCornerModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD2, sSetPivotCornerModeNormalData),
};

CameraMode sCamSetPivotDivingModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ2, sSetPivotWaterSurfaceModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ2, sSetPivotWaterSurfaceModeTargetData),
};

CameraMode sCamSetCs0Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO1, sDataOnlyInterfaceFlags),
};

CameraMode sCamSetCsTwistedHallwayModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO2, sDataOnlyInterfaceFlags),
};

CameraMode sCamSetForestBirdsEyeModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetForestBirdsEyeModeNormalData),
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA3, sSetForestBirdsEyeModeTalkData),
};

CameraMode sCamSetSlowChestCsModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO3, sSetSlowChestCsModeNormalData),
};

CameraMode sCamSetItemUnusedModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO4, sSetSlowChestCsModeNormalData),
};

CameraMode sCamSetCs3Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO9, sSetCs3ModeNormalData),
};

CameraMode sCamSetCsAttentionModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO5, sDataOnlyInterfaceFlags),
};

CameraMode sCamSetBeanGenericModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBeanGenericModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetBeanGenericModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal1ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetBeanGenericModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetBeanGenericModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetBeanGenericModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBeanGenericModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetBeanLostWoodsModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBeanLostWoodsModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetBeanLostWoodsModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal1ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetBeanLostWoodsModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetBeanLostWoodsModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetBeanLostWoodsModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBeanLostWoodsModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetSceneUnusedModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC9, sSetSceneUnusedModeNormalData),
};

CameraMode sCamSetSceneTransitionModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ2, sSetSceneTransitionModeNormalData),
};

CameraMode sCamSetFirePlatformModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC7, sDataOnlyNullFlags),
};

CameraMode sCamSetFireStaircaseModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC4, sDataOnlyInterfaceFlags),
};

CameraMode sCamSetForestUnusedModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ5, sDataOnlyInterfaceFlags),
};

CameraMode sCamSetForestDefeatPoeModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO6, sDataOnlyInterfaceFlags),
};

CameraMode sCamSetBigOctoModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBigOctoModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetDungeon0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetDungeon0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetBigOctoModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetDungeon0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal1ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetDungeon0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetDungeon0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetDungeon0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetDungeon0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetDungeon0ModeFreeFallData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetDungeon0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetBigOctoModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetMeadowBirdsEyeModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetMeadowBirdsEyeModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetMeadowBirdsEyeModeTargetData),
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetMeadowBirdsEyeModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetMeadowBirdsEyeModeClimbData),
};

CameraMode sCamSetMeadowUnusedModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetMeadowUnusedModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetMeadowUnusedModeTargetData),
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetMeadowUnusedModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetMeadowUnusedModeClimbData),
};

CameraMode sCamSetFireBirdsEyeModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetFireBirdsEyeModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetFireBirdsEyeModeTargetData),
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetFireBirdsEyeModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetFireBirdsEyeModeClimbData),
};

CameraMode sCamSetTurnAroundModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP4, sSetTurnAroundModeNormalData),
};

CameraMode sCamSetPivotVerticalModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC0, sSetPivotVerticalModeNormalData),
};

CameraMode sCamSetNorm2Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal2and4ModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal1ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetFishingModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetFishingModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetFishingModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetFishingModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetFishingModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetFishingModeFollowTargetData),
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetFishingModeFirstPersonData),
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetFishingModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetFishingModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetFishingModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetFishingModeFreeFallData),
};

CameraMode sCamSetCsCModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ9, sSetCsCModeNormalData),
};

CameraMode sCamSetJabuTentacleModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetJabuTentacleModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetJabuTentacleModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetDungeon2Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetDungeon2ModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetDungeon2ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetDungeon0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetDungeon2ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetDungeon2ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal1ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetDungeon2ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetDungeon2ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetDungeon2ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetDungeon2ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetDungeon2ModeFreeFallData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetDungeon2ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetDungeon2ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetDirectedYawModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetDirectedYawModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetDirectedYawModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetDirectedYawModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal0ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraMode sCamSetPivotFromSideModes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD4, sSetPivotInFrontAndFromSideModeNormalData),
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD4, sSetPivotInFrontAndFromSideModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sSetNormal0ModeTalkData),
    { CAM_FUNC_NONE, 0, NULL },
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    { CAM_FUNC_NONE, 0, NULL },
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
};

CameraMode sCamSetNormal4Modes[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal2and4ModeNormalData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModeTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowTargetData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, sNormal4ModeTalkData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, sSetNormal1ModeBattleData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeFirstPersonData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBowArrowZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, sSetNormal0ModeHookshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeBoomerangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, sSetNormal0ModeSlingshotData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, sSetNormal0ModeClimbZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, sSetNormal0ModeHangZData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, sSetNormal0ModeJumpData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, sSetNormal0ModeChargeData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, sSetNormal0ModeStillData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, sSetNormal0ModePushPullData),
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, sSetNormal0ModeFollowBoomerangData),
};

CameraSetting sCameraSettings[] = {
    { { 0x00000000 }, NULL },
    { { 0x051FFFFF }, sCamSetNormal0Modes },
    { { 0x051FFFFF }, sCamSetNormal1Modes },
    { { 0x051FFFFF }, sCamSetDungeon0Modes },
    { { 0x051FFFFF }, sCamSetDungeon1Modes },
    { { 0x050FF7FF }, sCamSetNormal3Modes },
    { { 0x8500018F }, sCamSetHorseModes },
    { { 0x051FFFFF }, sCamSetBossGohmaModes },
    { { 0x051FFFFF }, sCamSetBossDodongoModes },
    { { 0x051FFFFF }, sCamSetBossBarinadeModes },
    { { 0x051FFFFF }, sCamSetBossPhantomGanonModes },
    { { 0x051FFFFF }, sCamSetBossVolvagiaModes },
    { { 0x051FFFFF }, sCamSetBossBongoModes },
    { { 0x051FFFFF }, sCamSetBossMorphaModes },
    { { 0x051FFFFF }, sCamSetBossTwinrovaPlatformModes },
    { { 0x051FFFFF }, sCamSetBossTwinrovaFloorModes },
    { { 0x051FFFFF }, sCamSetBossGanondorfModes },
    { { 0x051FFFFF }, sCamSetBossGanonModes },
    { { 0x851FFFFF }, sCamSetTowerClimbModes },
    { { 0x851FFFFF }, sCamSetTowerUnusedModes },
    { { 0x8500000D }, sCamSetMarketBalconyModes },
    { { 0x85000001 }, sCamSetChuBowlingModes },
    { { 0x85000001 }, sCamSetPivotCrawlspaceModes },
    { { 0x85000001 }, sCamSetPivotShopBrowsingModes },
    { { 0x851E1FFF }, sCamSetPivotInFrontModes },
    { { 0x8C00000D }, sCamSetPreRendFixedModes },
    { { 0x8C00000D }, sCamSetPreRendPivotModes },
    { { 0x8C000001 }, sCamSetPreRendSideScrollModes },
    { { 0xC5000001 }, sCamSetDoor0Modes },
    { { 0xC5000003 }, sCamSetDoorCModes },
    { { 0xC5000001 }, sCamSetCrawlspaceModes },
    { { 0xC5000001 }, sCamSetStart0Modes },
    { { 0xC5000001 }, sCamSetStart1Modes },
    { { 0x05000001 }, sCamSetFree0Modes },
    { { 0x05000001 }, sCamSetFree1Modes },
    { { 0x85000001 }, sCamSetPivotCornerModes },
    { { 0x05000003 }, sCamSetPivotDivingModes },
    { { 0xCE000001 }, sCamSetCs0Modes },
    { { 0x4E000001 }, sCamSetCsTwistedHallwayModes },
    { { 0x05000009 }, sCamSetForestBirdsEyeModes },
    { { 0x45000001 }, sCamSetSlowChestCsModes },
    { { 0x45000001 }, sCamSetItemUnusedModes },
    { { 0x45000001 }, sCamSetCs3Modes },
    { { 0x45000001 }, sCamSetCsAttentionModes },
    { { 0x451FFFFF }, sCamSetBeanGenericModes },
    { { 0x451FFFFF }, sCamSetBeanLostWoodsModes },
    { { 0xC5000001 }, sCamSetSceneUnusedModes },
    { { 0x45000001 }, sCamSetSceneTransitionModes },
    { { 0x05000001 }, sCamSetFirePlatformModes },
    { { 0x45000001 }, sCamSetFireStaircaseModes },
    { { 0x45000001 }, sCamSetForestUnusedModes },
    { { 0x45000001 }, sCamSetForestDefeatPoeModes },
    { { 0x451FFFFF }, sCamSetBigOctoModes },
    { { 0x05000033 }, sCamSetMeadowBirdsEyeModes },
    { { 0x05000033 }, sCamSetMeadowUnusedModes },
    { { 0x05000033 }, sCamSetFireBirdsEyeModes },
    { { 0x4A000001 }, sCamSetTurnAroundModes },
    { { 0x05000001 }, sCamSetPivotVerticalModes },
    { { 0x051FFFFF }, sCamSetNorm2Modes },
    { { 0x0501E05F }, sCamSetFishingModes },
    { { 0x45000001 }, sCamSetCsCModes },
    { { 0x051FFFFF }, sCamSetJabuTentacleModes },
    { { 0x051FFFFF }, sCamSetDungeon2Modes },
    { { 0x051FFFFF }, sCamSetDirectedYawModes },
    { { 0xC5000ECD }, sCamSetPivotFromSideModes },
    { { 0x051FFFFF }, sCamSetNormal4Modes },
};

s32 Camera_Normal0(Camera* camera);
s32 Camera_Normal1(Camera* camera);
s32 Camera_Normal2(Camera* camera);
s32 Camera_Normal3(Camera* camera);
s32 Camera_Normal4(Camera* camera);
s32 Camera_Parallel0(Camera* camera);
s32 Camera_Parallel1(Camera* camera);
s32 Camera_Parallel2(Camera* camera);
s32 Camera_Parallel3(Camera* camera);
s32 Camera_Parallel4(Camera* camera);
s32 Camera_KeepOn0(Camera* camera);
s32 Camera_KeepOn1(Camera* camera);
s32 Camera_KeepOn2(Camera* camera);
s32 Camera_KeepOn3(Camera* camera);
s32 Camera_KeepOn4(Camera* camera);
s32 Camera_Subj0(Camera* camera);
s32 Camera_Subj1(Camera* camera);
s32 Camera_Subj2(Camera* camera);
s32 Camera_Subj3(Camera* camera);
s32 Camera_Subj4(Camera* camera);
s32 Camera_Jump0(Camera* camera);
s32 Camera_Jump1(Camera* camera);
s32 Camera_Jump2(Camera* camera);
s32 Camera_Jump3(Camera* camera);
s32 Camera_Jump4(Camera* camera);
s32 Camera_Battle0(Camera* camera);
s32 Camera_Battle1(Camera* camera);
s32 Camera_Battle2(Camera* camera);
s32 Camera_Battle3(Camera* camera);
s32 Camera_Battle4(Camera* camera);
s32 Camera_Fixed0(Camera* camera);
s32 Camera_Fixed1(Camera* camera);
s32 Camera_Fixed2(Camera* camera);
s32 Camera_Fixed3(Camera* camera);
s32 Camera_Fixed4(Camera* camera);
s32 Camera_Data0(Camera* camera);
s32 Camera_Data1(Camera* camera);
s32 Camera_Data2(Camera* camera);
s32 Camera_Data3(Camera* camera);
s32 Camera_Data4(Camera* camera);
s32 Camera_Unique0(Camera* camera);
s32 Camera_Unique1(Camera* camera);
s32 Camera_Unique2(Camera* camera);
s32 Camera_Unique3(Camera* camera);
s32 Camera_Unique4(Camera* camera);
s32 Camera_Unique5(Camera* camera);
s32 Camera_Unique6(Camera* camera);
s32 Camera_Unique7(Camera* camera);
s32 Camera_Unique8(Camera* camera);
s32 Camera_Unique9(Camera* camera);
s32 Camera_Demo0(Camera* camera);
s32 Camera_Demo1(Camera* camera);
s32 Camera_Demo2(Camera* camera);
s32 Camera_Demo3(Camera* camera);
s32 Camera_Demo4(Camera* camera);
s32 Camera_Demo5(Camera* camera);
s32 Camera_Demo6(Camera* camera);
s32 Camera_Demo7(Camera* camera);
s32 Camera_Demo8(Camera* camera);
s32 Camera_Demo9(Camera* camera);
s32 Camera_Special0(Camera* camera);
s32 Camera_Special1(Camera* camera);
s32 Camera_Special2(Camera* camera);
s32 Camera_Special3(Camera* camera);
s32 Camera_Special4(Camera* camera);
s32 Camera_Special5(Camera* camera);
s32 Camera_Special6(Camera* camera);
s32 Camera_Special7(Camera* camera);
s32 Camera_Special8(Camera* camera);
s32 Camera_Special9(Camera* camera);

s32 (*sCameraFunctions[])(Camera*) = {
    NULL,
    Camera_Normal0,
    Camera_Normal1,
    Camera_Normal2,
    Camera_Normal3,
    Camera_Normal4,
    Camera_Parallel0,
    Camera_Parallel1,
    Camera_Parallel2,
    Camera_Parallel3,
    Camera_Parallel4,
    Camera_KeepOn0,
    Camera_KeepOn1,
    Camera_KeepOn2,
    Camera_KeepOn3,
    Camera_KeepOn4,
    Camera_Subj0,
    Camera_Subj1,
    Camera_Subj2,
    Camera_Subj3,
    Camera_Subj4,
    Camera_Jump0,
    Camera_Jump1,
    Camera_Jump2,
    Camera_Jump3,
    Camera_Jump4,
    Camera_Battle0,
    Camera_Battle1,
    Camera_Battle2,
    Camera_Battle3,
    Camera_Battle4,
    Camera_Fixed0,
    Camera_Fixed1,
    Camera_Fixed2,
    Camera_Fixed3,
    Camera_Fixed4,
    Camera_Data0,
    Camera_Data1,
    Camera_Data2,
    Camera_Data3,
    Camera_Data4,
    Camera_Unique0,
    Camera_Unique1,
    Camera_Unique2,
    Camera_Unique3,
    Camera_Unique4,
    Camera_Unique5,
    Camera_Unique6,
    Camera_Unique7,
    Camera_Unique8,
    Camera_Unique9,
    Camera_Demo0,
    Camera_Demo1,
    Camera_Demo2,
    Camera_Demo3,
    Camera_Demo4,
    Camera_Demo5,
    Camera_Demo6,
    Camera_Demo7,
    Camera_Demo8,
    Camera_Demo9,
    Camera_Special0,
    Camera_Special1,
    Camera_Special2,
    Camera_Special3,
    Camera_Special4,
    Camera_Special5,
    Camera_Special6,
    Camera_Special7,
    Camera_Special8,
    Camera_Special9,
};

s32 sInitRegs = 1;

s32 gDbgCamEnabled = 0;
s32 sDbgModeIdx = -1;
s16 sNextUID = 0;

s32 sCameraInterfaceFlags = 1;

s32 sCameraInterfaceAlpha = 0x02;
s32 sCameraShrinkWindowVal = 0x20;
s32 D_8011D3AC = -1;

s16 D_8011D3B0[] = {
    0x0AAA, 0xF556, 0x1555, 0xEAAB, 0x2AAA, 0xD556, 0x3FFF, 0xC001, 0x5555, 0xAAAB, 0x6AAA, 0x9556, 0x7FFF, 0x0000,
};

s16 D_8011D3CC[] = {
    0x0000, 0x02C6, 0x058C, 0x0000, 0x0000, 0xFD3A, 0x0000, 0x0852, 0x0000, 0x0000, 0x0B18, 0x02C6, 0xFA74, 0x0000,
};

s32 sUpdateCameraDirection = 0;
s32 D_8011D3EC = 0;
s32 D_8011D3F0 = 0;

s32 sDemo5PrevAction12Frame = -16;

char sCameraFunctionNames[][8] = {
    "NONE   ", "NORM0()", "NORM1()", "NORM2()", "NORM3()", "NORM4()", "PARA0()", "PARA1()", "PARA2()", "PARA3()",
    "PARA4()", "KEEP0()", "KEEP1()", "KEEP2()", "KEEP3()", "KEEP4()", "SUBJ0()", "SUBJ1()", "SUBJ2()", "SUBJ3()",
    "SUBJ4()", "JUMP0()", "JUMP1()", "JUMP2()", "JUMP3()", "JUMP4()", "BATT0()", "BATT1()", "BATT2()", "BATT3()",
    "BATT4()", "FIXD0()", "FIXD1()", "FIXD2()", "FIXD3()", "FIXD4()", "DATA0()", "DATA1()", "DATA2()", "DATA3()",
    "DATA4()", "UNIQ0()", "UNIQ1()", "UNIQ2()", "UNIQ3()", "UNIQ4()", "UNIQ5()", "UNIQ6()", "UNIQ7()", "UNIQ8()",
    "UNIQ9()", "DEMO0()", "DEMO1()", "DEMO2()", "DEMO3()", "DEMO4()", "DEMO5()", "DEMO6()", "DEMO7()", "DEMO8()",
    "DEMO9()", "SPEC0()", "SPEC1()", "SPEC2()", "SPEC3()", "SPEC4()", "SPEC5()", "SPEC6()", "SPEC7()", "SPEC8()",
    "SPEC9()", "",        "",        "",        "",        "",
};

VecSph D_8011D658[] = {
    { 50.0f, 0xEE3A, 0xD558 },
    { 75.0f, 0x0000, 0x8008 },
    { 80.0f, 0xEE3A, 0x8008 },
    { 15.0f, 0xEE3A, 0x8008 },
};

Vec3f D_8011D678[] = {
    { 0.0f, 40.0f, 20.0f },
    { 0.0f, 40.0f, 0.0f },
    { 0.0f, 3.0f, -3.0f },
    { 0.0f, 3.0f, -3.0 },
};

/*******************************************************
 * OnePoint initalization values for Demo5
 ********************************************************/
s32 sDemo5PrevSfxFrame = -200;

// target is player, far from eye
OnePointCsFull D_8011D6AC[] = {
    { // initflags & 0x00FF (at): 2, atTarget is view lookAt + atInit
      // initFlags & 0xFF00 (eye): none
      // action: 15, copy at, eye, roll, fov to camera
      // result: eye remains in the same locaiton, at is View's lookAt
      0x8F,
      0xFF,
      0x0002,
      0x0001,
      0x0000,
      60.0f,
      1.0f,
      { 0.0f, 0.0f, 0.0f },
      { 0.0f, 0.0f, 0.0f } },
    { // initFlags & 0x00FF (at): 3, atTarget is camera's current at + atInit
      // initFlags & 0xFF00 (eye): 3, eyeTarget is the camera's current eye + eyeInit
      // action: interplate eye and at.
      // result: eye and at's y interpolate to become +20 from their current location.
      0x81,
      0xFF,
      0x0303,
      0x0013,
      0x0000,
      45.0f,
      1.0f,
      { 0.0f, 20.0f, 0.0f },
      { 0.0f, 20.0f, 0.0f } },
    { // initFlags & 0x00FF (at): 0 none
      // initFlags & 0xFF00 (eye): 0 none
      // action: 18, copy this camera to default camera.
      0x12,
      0xFF,
      0x0000,
      0x0001,
      0x0000,
      60.0f,
      1.0f,
      { -1.0f, -1.0f, -1.0f },
      { -1.0f, -1.0f, -1.0f } },
};

// target is player close to current eye
OnePointCsFull D_8011D724[] = {
    { 0x8F, 0xFF, 0x2424, 0x0001, 0x0000, 60.0f, 1.0f, { 0.0f, 0.0f, 0.0f }, { 0.0f, 10.0f, -20.0f } },
    { 0x81, 0xFF, 0x2121, 0x0013, 0x0000, 50.0f, 1.0f, { 0.0f, -10.0f, 0.0f }, { 0.0f, 0.0f, 60.0f } },
    { 0x12, 0xFF, 0x0000, 0x0001, 0x0000, 60.0f, 1.0f, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },
};

// target is close to player
OnePointCsFull D_8011D79C[] = {
    { 0xCF, 0xFF, 0x0002, 0x0001, 0x0000, 60.0f, 1.0f, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0xC1, 0xFF, 0x0303, 0x0013, 0x0000, 45.0f, 1.0f, { 0.0f, -20.0f, 0.0f }, { 0.0f, -10.0f, 5.0f } },
    {
        0xC1,
        0xFF,
        0x0303,
        0x0009,
        0x0000,
        60.0f,
        1.0f,
        { 0.0f, 10.0f, 0.0f },
        { 0.0f, 10.0f, 0.0f },
    },
    { 0x12, 0xFF, 0x0000, 0x0001, 0x0000, 60.0f, 1.0f, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },
};

// target is within 300 units of eye, and player is within 30 units of eye
OnePointCsFull D_8011D83C[] = {
    { 0x83, 0xFF, 0x2141, 0x0014, 0x0000, 45.0f, 0.2f, { 0.0f, 0.0f, 10.0f }, { 0.0f, 0.0f, 10.0f } },
    { 0x12, 0xFF, 0x0000, 0x0001, 0x0000, 60.0f, 1.0f, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },
};

// target is within 700 units of eye, angle between player/eye and target/eye is less than
// 76.9 degrees.  The x/y coordinates of the target on screen is between (21, 41) and (300, 200),
// and the player is farther than 30 units of the eye
OnePointCsFull D_8011D88C[] = {
    { 0x81, 0xFF, 0x0303, 0x0014, 0x0000, 45.0f, 1.0f, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    { 0x12, 0xFF, 0x0000, 0x0001, 0x0000, 60.0f, 1.0f, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },
};

// same as above, but the target is NOT within the screen area.
OnePointCsFull D_8011D8DC[] = {
    { 0x8F, 0xFF, 0x0404, 0x0014, 0x0001, 50.0f, 1.0f, { 0.0f, 5.0f, 10.0f }, { 0.0f, 10.0f, -80.0f } },
    { 0x82, 0xFF, 0x2121, 0x0005, 0x0000, 60.0f, 1.0f, { 0.0f, 5.0f, 0.0f }, { 5.0f, 5.0f, -200.0f } },
    { 0x12, 0xFF, 0x0000, 0x0001, 0x0000, 60.0f, 1.0f, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },
};

// target is a door.
OnePointCsFull D_8011D954[] = {
    { 0x0F, 0xFF, 0xC1C1, 0x0014, 0x0000, 60.0f, 1.0f, { 0.0f, 0.0f, 50.0f }, { 0.0f, 0.0f, 250.0f } },
    { 0x83, 0xFF, 0x05B1, 0x0005, 0x0000, 60.0f, 0.1f, { 0.0f, 10.0f, 50.0f }, { 0.0f, 10.0f, 100.0f } },
    { 0x82, 0xFF, 0x2121, 0x0005, 0x0002, 60.0f, 1.0f, { 0.0f, 10.0f, 0.0f }, { 0.0f, 20.0f, -150.0f } },
    { 0x12, 0xFF, 0x0000, 0x0001, 0x0000, 60.0f, 1.0f, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },
};

// otherwise
OnePointCsFull D_8011D9F4[] = {
    { 0x8F, 0xFF, 0x0504, 0x0014, 0x0002, 60.0f, 1.0f, { 0.0f, 5.0f, 50.0f }, { 0.0f, 20.0f, 300.0f } },
    { 0x82, 0xFF, 0x2121, 0x0005, 0x0002, 60.0f, 1.0f, { 0.0f, 10.0f, 0.0f }, { 0.0f, 20.0f, -150.0f } },
    { 0x12, 0xFF, 0x0000, 0x0001, 0x0000, 60.0f, 1.0f, { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f } },
};

Vec3f D_8011DA6C[] = {
    { 3050.0f, 700.0f, 0.0f }, { 1755.0f, 3415.0f, -380.0f }, { -3120.0f, 3160.0f, 245.0f }, { 0.0f, -10.0f, 240.0f }
};

Vec3f D_8011DA9C[] = {
    { 3160.0f, 2150.0f, 0.0f },
    { 1515.0f, 4130.0f, -835.0f },
    { -3040.0f, 4135.0f, 230.0f },
    { -50.0f, 600.0f, -75.0f },
};

f32 D_8011DACC[] = { 1570.0f, 3680.0f, 3700.0f, 395.0f };

f32 D_8011DADC[] = { 320.0f, 320.0f, 320.0f, 0.0f };

s16 D_8011DAEC[] = { -2000, -1000, 0, 0, 0, 0, 0, 0 };

s16 D_8011DAFC[] = {
    CAM_SET_NORMAL0, CAM_SET_NORMAL1, CAM_SET_NORMAL2, CAM_SET_DUNGEON0, CAM_SET_DUNGEON1, CAM_SET_DUNGEON2,
};

PlayState* D_8015BD7C;
DbCamera D_8015BD80;
CollisionPoly* playerFloorPoly;
