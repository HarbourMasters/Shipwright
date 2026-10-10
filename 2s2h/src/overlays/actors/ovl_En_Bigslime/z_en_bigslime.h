#ifndef Z_EN_BIGSLIME_H
#define Z_EN_BIGSLIME_H

#include "global.h"
#include "overlays/actors/ovl_En_Minislime/z_en_minislime.h"
#include "overlays/actors/ovl_En_Pametfrog/z_en_pametfrog.h"

struct EnBigslime;

typedef void (*EnBigslimeActionFunc)(struct EnBigslime*, PlayState*);

#define BIGSLIME_NUM_VTX 162 // Number of vertices
#define BIGSLIME_NUM_FACES 320 // Number of triangles
#define BIGSLIME_NUM_RING_FACES 12  // Number of rings composed of faces
#define BIGSLIME_NUM_RING_VTX (1 + BIGSLIME_NUM_RING_FACES)  // Number of rings composed of vertices
#define BIGSLIME_RADIUS_S 1000      // Radius of reference sphere int
#define BIGSLIME_RADIUS_F 1000.0f   // Radius of reference sphere float
#define BIGSLIME_NUM_ICE_SHARD (BIGSLIME_NUM_VTX + 10*MINISLIME_NUM_SPAWN)

#define GBT_ROOM_5_CENTER_X 1338.0f
#define GBT_ROOM_5_CENTER_Y -390.0f
#define GBT_ROOM_5_CENTER_Z -2492.0f
#define GBT_ROOM_5_MIN_X (GBT_ROOM_5_CENTER_X - 480.0f)
#define GBT_ROOM_5_MAX_X (GBT_ROOM_5_CENTER_X + 480.0f)
#define GBT_ROOM_5_MIN_Y (GBT_ROOM_5_CENTER_Y - 300.0f)
#define GBT_ROOM_5_MAX_Y (GBT_ROOM_5_CENTER_Y + 300.0f)
#define GBT_ROOM_5_MIN_Z (GBT_ROOM_5_CENTER_Z - 480.0f)
#define GBT_ROOM_5_MAX_Z (GBT_ROOM_5_CENTER_Z + 480.0f)

/*
 * Triangle face
 * Based on "Tri" struct from gbi.h, but without the flag
 */
typedef struct {
    /* 0x0 */ u8 v[3]; // vertex index
} EnBigslimeTri; // size = 0x3

typedef struct {
    /* 0x0 */ s32 v; // vertex index
    /* 0x4 */ f32 scaleVtx; // fraction of distance from center to vtx (range 0-1)
} EnBigslimeBubbles; // size = 0x8

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ s16 isEnabled;
    /* 0x1A */ Vec3s rot;
    /* 0x20 */ f32 scale;
} EnBigslimeIceShardEffect; // size = 0x24

typedef struct EnBigslime {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ SkelAnime skelAnime;
    /* 0x0188 */ EnBigslimeActionFunc actionFunc;
    /* 0x018C */ EnBigslimeActionFunc actionFuncStored;
    /* 0x0190 */ Vec3s jointTable[GEKKO_LIMB_MAX];
    /* 0x0220 */ Vec3s morphTable[GEKKO_LIMB_MAX];
    /* 0x02B0 */ u8 minislimeState;
    /* 0x02B1 */ u8 dynamicVtxState; // Toggles between two states of dynamic Vtx
    /* 0x02B2 */ u8 isAnimFinished;
    /* 0x02B3 */ union {
                    u8 formBigslimeTimer; // Bigslime will start forming when timer reaches 0
                    u8 minislimeCounter;
                    u8 numGekkoMeleeAttacks; // The Gekko will melee-attack link at 1-3 times at each position while engulfed in bigslime
                };
    /* 0x02B4 */ u8 shockwaveAlpha;
    /* 0x02B5 */ u8 gekkoDrawDmgEffType;
    /* 0x02B6 */ s16 gekkoYaw;
    /* 0x02B8 */ s16 csId;
    /* 0x02BA */ union { // multi-use timer
                    s16 idleTimer;
                    s16 noticeTimer;
                    s16 callTimer;
                    s16 jumpTimer;
                    s16 damageSpinTimer;
                    s16 scaleFactor; // Used between individual gekko melee-attacks while engulfed in bigslime
                    s16 windupPunchTimer;
                    s16 throwPlayerTimer;
                    s16 ceilingMoveTimer;
                    s16 squishFlatTimer;
                    s16 riseCounter;
                    s16 grabPlayerTimer;
                    s16 formBigslimeCutsceneTimer;
                    s16 defeatTimer;
                    s16 despawnTimer;
                    s16 spawnFrogTimer;
                    s16 isDespawned;
                    s16 isInitJump;
                };
    /* 0x02BC */ s16 wavySurfaceTimer; // decrements from 24, used in Math_SinF for currData2
    /* 0x02BE */ s16 stunTimer;
    /* 0x02C0 */ union {
                    s16 freezeTimer;
                    s16 meltCounter;
                };
    /* 0x02C2 */ s16 ceilingDropTimer; // Bigslime is still during this timer and shakes before dropping
    /* 0x02C4 */ s16 numGekkoPosGrabPlayer; // The Gekko will melee-attack link at 6 positions while engulfed in bigslime
    /* 0x02C6 */ s16 subCamId;
    /* 0x02C8 */ s16 subCamYawGrabPlayer;
    /* 0x02CA */ s16 rotation; // is always 0, used in Matrix_RotateYS
    /* 0x02CC */ s16 itemDropTimer; // items only drop when zero, Set to 40 then decrements, prevents itemDrop spam
    /* 0x02CE */ Vec3s gekkoRot;
    /* 0x02D4 */ Vec3f gekkoPosOffset; // Used when Bigslime grabs link
    /* 0x02E0 */ Vec3f gekkoProjectedPos;
    /* 0x02EC */ union {
                    Vec3f frozenPos;
                    Vec3f subCamDistToFrog; // Used to zoom into frogs as Gekko despawns/Frog spawns
                };
    /* 0x02F8 */ Vec3f gekkoBodyPartsPos[GEKKO_BODYPART_MAX];
    /* 0x0388 */ f32 gekkoDrawDmgEffAlpha;
    /* 0x038C */ f32 gekkoDrawDmgEffScale;
    /* 0x0390 */ f32 gekkoDrawDmgEffFrozenSteamScale;
    /* 0x0394 */ f32 gekkoScale;
    /* 0x0398 */ f32 vtxSurfacePerturbation[BIGSLIME_NUM_VTX];
    /* 0x0620 */ f32 vtxScaleX;
    /* 0x0624 */ f32 vtxScaleZ;
    /* 0x0628 */ f32 shockwaveScale;
    /* 0x062C */ ColliderCylinder gekkoCollider;
    /* 0x0678 */ ColliderCylinder bigslimeCollider[BIGSLIME_NUM_RING_FACES];
    /* 0x0A14 */ EnMinislime* minislime[MINISLIME_NUM_SPAWN];
    /* 0x0A44 */ EnMinislime* minislimeToThrow;
    /* 0x0A48 */ AnimatedMaterial* bigslimeFrozenTexAnim;
    /* 0x0A4C */ AnimatedMaterial* minislimeFrozenTexAnim;
    /* 0x0A50 */ AnimatedMaterial* iceShardTexAnim;
    /* 0x0A54 */ EnBigslimeIceShardEffect iceShardEffect[BIGSLIME_NUM_ICE_SHARD]; // 312 = 162 (bigslime) + 10 * 15 (minislime)
} EnBigslime; // size = 0x3634

#endif // Z_EN_BIGSLIME_H
