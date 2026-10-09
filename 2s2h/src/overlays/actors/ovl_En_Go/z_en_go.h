#ifndef Z_EN_GO_H
#define Z_EN_GO_H

#include "global.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"

struct EnGo;

typedef void (*EnGoActionFunc)(struct EnGo*, PlayState*);

#define ENGO_GET_TYPE(thisx) (((thisx)->params & 0xF) & 0xFF)
#define ENGO_GET_SUBTYPE(thisx) ((((thisx)->params & 0x70) >> 4) & 0xFF)
#define ENGO_GET_PATH_INDEX(thisx) ((((thisx)->params & 0x7F80) >> 7) & 0xFF)

#define ENGO_PATH_INDEX_NONE 0xFF

#define ENGO_FIDGET_TABLE_LEN 3

#define ENGO_SNOW_EFFECT_COUNT 16
#define ENGO_OTHER_EFFECT_COUNT 16
#define ENGO_EFFECT_COUNT (ENGO_SNOW_EFFECT_COUNT + ENGO_OTHER_EFFECT_COUNT)

/**
 * Struct to track the state of various visual effects.
 *
 * A Goron's 'effectTable' is comprised of a number of these effects, which depending on the context in which they are
 * used, take on the form of snow, dust, or steam. The functions using these objects have an expectation that they are
 * iterating through the full effectTable, or half of the effectTable. See individual functions containing 'EnGoEffect*'
 * parameters for more details.
 */
typedef struct EnGoEffect {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 alphaDenom;
    /* 0x02 */ u8 alphaNumer;
    /* 0x04 */ Vec3s angularVelocity;
    /* 0x0A */ Vec3s rot;
    /* 0x10 */ Vec3f pos;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Vec3f velocity;
    /* 0x34 */ f32 scaleXY;
    /* 0x38 */ f32 scaleXYDelta;
} EnGoEffect; // size = 0x3C;

/**
 * Goron Type, identified by bits [3..0] of the actor's params
 *
 * - The ENGO_ATHLETIC and ENGO_SPECTATOR types share the same subtype domain (EnGoRacetrackSubtype: [0,7])
 * - ENGO_GRAVEYARD types have their own subtype domain (EnGoGraveyardSubtype: [0,1])
 * - Others have no subtypes.
 */
typedef enum EnGoType {
    /* 0 */ ENGO_F_0,
    /* 1 */ ENGO_ATHLETIC,         // Racers stretching before the race
    /* 2 */ ENGO_SPECTATOR,        // Spectators to the race
    /* 3 */ ENGO_GATEKEEPER,       // Gatekeeper to the Goron Shrine
    /* 4 */ ENGO_GRAVEYARD,        // Gravemaker and other
    /* 5 */ ENGO_ASIDE_STORE,      // Shrine Goron outside store
    /* 6 */ ENGO_ASIDE_ELDERSROOM, // Shrine Goron outside Elder's room
    /* 7 */ ENGO_ASIDE_ELDERSSON,  // Shrine Goron aside the Elder's Son
    /* 8 */ ENGO_MEDIGORON         // Medigoron selling Powder Kegs
} EnGoType;

/**
 * Subtype for Gorons at the racetrack (ENGO_ATHLETIC and ENGO_SPECTATOR)
 */
typedef enum EnGoRacetrackSubtype {
    /* 0 */ ENGO_ATHLETIC_A,
    /* 1 */ ENGO_ATHLETIC_B,
    /* 2 */ ENGO_ATHLETIC_C,
    /* 3 */ ENGO_ATHLETIC_D,
    /* 4 */ ENGO_ATHLETIC_HAMSTRINGSIT,
    /* 5 */ ENGO_ATHLETIC_HAMSTRINGSTAND,
    /* 6 */ ENGO_SPECTATOR_CHEER,
    /* 7 */ ENGO_SPECTATOR_SHOUTING
} EnGoRacetrackSubtype;

/**
 * Subtype for Gorons outside the Goron Graveyard (ENGO_GRAVEYARD)
 */
typedef enum EnGoGraveyardSubtype {
    /* 0 */ ENGO_GRAVEYARD_GRAVEMAKER,
    /* 1 */ ENGO_GRAVEYARD_FROZEN
} EnGoGraveyardSubtype;

typedef struct EnGo {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnGoActionFunc actionFunc;
    /* 0x18C */ EnGoActionFunc interruptedActionFunc;
    /* 0x190 */ EnGoActionFunc graveyardDialogActionFunc;
    /* 0x194 */ ColliderCylinder colliderCylinder;
    /* 0x1E0 */ ColliderCylinder unusedCylinder;
    /* 0x22C */ ColliderSphere colliderSphere;
    /* 0x284 */ Path* gatekeeperPath;
    /* 0x288 */ s8 taisouObjectSlot;
    /* 0x289 */ s8 hakuginDemoObjectSlot;
    /* 0x28C */ s32 msgScriptPos;
    /* 0x290 */ Vec3f headPos;
    /* 0x29C */ Vec3f bodyPos;
    /* 0x2A8 */ Vec3s headRot;
    /* 0x2AE */ Vec3s bodyRot;
    /* 0x2B4 */ Vec3s jointTable[GORON_LIMB_MAX];
    /* 0x320 */ Vec3s morphTable[GORON_LIMB_MAX];
    /* 0x38C */ Actor* attentionTarget;
    /* 0x390 */ u16 actionFlags;
    /* 0x392 */ u16 lastTextId;
    /* 0x394 */ u8 springArrivalCueId;
    /* 0x398 */ f32 animPlaySpeed;
    /* 0x39C */ f32 iceBlockScale;
    /* 0x3A0 */ f32 iceBlockAlpha;
    /* 0x3A4 */ f32 scaleFactor;
    /* 0x3A8 */ f32 scaleFactorPoundDistortion;
    /* 0x3AC */ UNK_TYPE1 unk3AC[0x2];
    /* 0x3AE */ union {
        s16 snorePhase;      // Used whilst a Goron is sleeping, +0x400 at a time
        s16 elapsedHangtime; // Hangtime at the top of a pound.
    };
    /* 0x3B0 */ s16 headRotZ;
    /* 0x3B2 */ s16 headRotY;
    /* 0x3B4 */ s16 bodyRotZ;
    /* 0x3B6 */ s16 bodyRotY;
    /* 0x3B8 */ s16 csId;
    /* 0x3BA */ s16 loseAttentionTimer;
    /* 0x3BC */ s16 blinkTimer;
    /* 0x3BE */ s16 eyeTexIndex;
    /* 0x3C0 */ s16 cutsceneState;
    /* 0x3C2 */ s16 cutsceneDelayTimer;
    /* 0x3C4 */ s16 gatekeeperAnimState;
    /* 0x3C6 */ s16 harmlessTimer;
    /* 0x3C8 */ s16 fidgetTableZ[ENGO_FIDGET_TABLE_LEN];
    /* 0x3CE */ s16 fidgetTableY[ENGO_FIDGET_TABLE_LEN];
    /* 0x3D4 */ s16 surprisePhase;
    /* 0x3D8 */ MsgScriptCallback msgScriptCallback;
    /* 0x3DC */ s32 animIndex;
    /* 0x3E0 */ UNK_TYPE1 unk3E0[0x4];
    /* 0x3E4 */ s32 indexPathPoint;
    /* 0x3E8 */ s32 indexEffect;
    /* 0x3EC */ s32 sleepState;
    /* 0x3F0 */ s32 springArrivalCutsceneActive;
    /* 0x3F4 */ s32 changedText;
    /* 0x3F8 */ EnGoEffect effectTable[ENGO_EFFECT_COUNT];
} EnGo; // size = 0xB78

#endif // Z_EN_GO_H
