#ifndef Z_EN_TEST6_H
#define Z_EN_TEST6_H

#include "global.h"

struct EnTest6;

typedef void (*EnTest6ActionFunc)(struct EnTest6*, PlayState*);

#define SOTCS_GET_OCARINA_MODE(thisx) ((thisx)->params)

// CueIds are a mix of cues from external cutscenes and internal states

typedef enum SoTCsCueId {
    /* 0x00 */ SOTCS_CUEID_NONE,
    /* 0x01 */ SOTCS_CUEID_DOUBLE_WAIT,
    /* 0x02 */ SOTCS_CUEID_DOUBLE_INIT,
    /* 0x03 */ SOTCS_CUEID_DOUBLE_CLOCKS_INWARD,
    /* 0x04 */ SOTCS_CUEID_DOUBLE_CLOCKS_SPIN,
    /* 0x05 */ SOTCS_CUEID_DOUBLE_CLOCKS_OUTWARD,
    /* 0x06 */ SOTCS_CUEID_RESET_INIT,
    /* 0x07 */ SOTCS_CUEID_RESET_CLOCKS_SLOW_DOWN,
    /* 0x08 */ SOTCS_CUEID_RESET_CLOCKS_SPEED_UP,
    /* 0x09 */ SOTCS_CUEID_DOUBLE_END,
    /* 0x5A */ SOTCS_CUEID_INV_INIT = 90,
    /* 0x5B */ SOTCS_CUEID_INV_SETUP_CLOCKS,
    /* 0x5D */ SOTCS_CUEID_INV_UNUSED = 93,
    /* 0x5F */ SOTCS_CUEID_INV_CLOCKS = 95,
    /* 0x63 */ SOTCS_CUEID_INV_END = 99
} SoTCsCueId;

typedef struct {
    /* 0x0 */ LightNode* node;
    /* 0x4 */ LightInfo info;
} SoTCsLight; // size = 0x14

#define SOTCS_INV_NUM_CLOCKS 6
#define SOTCS_DOUBLE_NUM_CLOCKS 51
#define SOTCS_RESET_NUM_CLOCKS 128
#define SOTCS_NUM_PARTICLES 64

typedef struct EnTest6 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnTest6ActionFunc actionFunc;
    /* 0x148 */ Gfx* gfx;
    /* 0x14C */ f32 speed; // Used for clock distance and lights
    /* 0x150 */ f32 clockDist; // Radius from player
    /* 0x154 */ f32 clockSpeed;
    /* 0x158 */ union {
                    f32 clockRadialSpeed; // For double SoT cutscene
                    f32 clockAccel; // For reset SoT cutscene
                };
    /* 0x15C */ f32 invSoTEnvLerp;
    /* 0x160 */ f32 doubleSoTEnvLerp;
    /* 0x164 */ SoTCsLight lights[2];
    /* 0x18C */ CutsceneCamera csCamInfo;
    /* 0x20C */ Vec3f invSoTClockPos[SOTCS_INV_NUM_CLOCKS];
    /* 0x254 */ Vec3f (*invSoTParticles)[SOTCS_NUM_PARTICLES];
    /* 0x258 */ Vec3f subCamAt;
    /* 0x264 */ Vec3f subCamEye;
    /* 0x270 */ f32 subCamFov;
    /* 0x274 */ s16 cueId;
    /* 0x276 */ s16 drawType;
    /* 0x278 */ union {
                    s16 invSoTClockYaw; // For inverted SoT cutscene.
                    s16 counter; // For double/reset SoT cutscenes. Increments every frame, unused.
                };
    /* 0x27A */ s16 timer;
    /* 0x27C */ s16 clockAngle;
    /* 0x27E */ s16 clockRingRotZ; // For double/reset SoT cutscenes
    /* 0x280 */ s16 clockColorGray; // For double SoT cutscene
    /* 0x282 */ s16 alpha;
    /* 0x284 */ s16 subCamId;
    /* 0x286 */ s16 screenFillAlpha; // As a ratio, 20 is an alpha of 255
} EnTest6; // size = 0x288

#endif // Z_EN_TEST6_H
