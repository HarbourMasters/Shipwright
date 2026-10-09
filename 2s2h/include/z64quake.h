#ifndef Z64QUAKE_H
#define Z64QUAKE_H

#include "z64camera.h"
#include "z64math.h"

struct PlayState;

typedef struct {
    /* 0x00 */ Vec3f atOffset;
    /* 0x0C */ Vec3f eyeOffset;
    /* 0x18 */ s16 upRollOffset;
    /* 0x1A */ s16 fovOffset;
    /* 0x1C */ f32 maxOffset; // Set to scaled max data of struct (mag for Vec3f), never used
} ShakeInfo; // size = 0x20

typedef enum {
    /* 0 */ QUAKE_TYPE_NONE,
    /* 1 */ QUAKE_TYPE_1, // Periodic, sustaining, random X perturbations
    /* 2 */ QUAKE_TYPE_2, // Aperiodic, sustaining, random X perturbations
    /* 3 */ QUAKE_TYPE_3, // Periodic, decaying
    /* 4 */ QUAKE_TYPE_4, // Aperiodic, decaying, random X perturbations
    /* 5 */ QUAKE_TYPE_5, // Periodic, sustaining
    /* 6 */ QUAKE_TYPE_6 // See below
} QuakeType;

s16 Quake_Request(Camera* camera, u32 type);

u32 Quake_SetSpeed(s16 index, s16 speed);
u32 Quake_SetPerturbations(s16 index, s16 y, s16 x, s16 fov, s16 roll);
u32 Quake_SetDuration(s16 index, s16 duration);
u32 Quake_SetOrientation(s16 index, s16 isRelativeToScreen, Vec3s orientation);

s16 Quake_GetTimeLeft(s16 index);
s32 Quake_GetNumActiveQuakes(void);
u32 Quake_RemoveRequest(s16 index);

void Quake_Init(void);
s16 Quake_Update(Camera* camera, ShakeInfo* camShake);

#define DISTORTION_TYPE_HOT_ROOM (1 << 0) // Hot Room
#define DISTORTION_TYPE_NON_ZORA_SWIMMING (1 << 2) // Non-Zora swimming
#define DISTORTION_TYPE_ZORA_SWIMMING (1 << 3) // Zora swimming, also used for boss warp pad part 1

#define DISTORTION_TYPE_UNDERWATER_ENTRY (1 << 4) // Entering water, also used for boss warp pad part 2
#define DISTORTION_TYPE_SONG_OF_TIME (1 << 5) // Song of Time effects
#define DISTORTION_TYPE_ZORA_KICK (1 << 6) // PLAYER_MWA_ZORA_PUNCH_KICK
#define DISTORTION_TYPE_UNK_ATTACK (1 << 7) // Impossible to achieve, inferred to be another `PLAYER_MWA_`, 

#define DISTORTION_TYPE_GORON_BUTT (1 << 8) // PLAYER_MWA_GORON_PUNCH_BUTT
#define DISTORTION_TYPE_MASK_TRANSFORM_1 (1 << 9) // Mask transformation part 1
#define DISTORTION_TYPE_BOSS_WARP (1 << 10) // Boss warp pad part 3
#define DISTORTION_TYPE_MASK_TRANSFORM_2 (1 << 11) // Mask transformation part 2

void Distortion_Request(s32 type);
void Distortion_SetDuration(s16 duration);

s16 Distortion_GetTimeLeft(void);
s16 Distortion_GetType(void);
void Distortion_RemoveRequest(s32 type);

void Distortion_Init(struct PlayState* play);
void Distortion_Update(void);

#endif
