#ifndef Z_EN_FR_H
#define Z_EN_FR_H

#include "ultra64.h"
#include "global.h"

struct EnFr;

typedef void (*EnFrActionFunc)(struct EnFr*, PlayState*);
typedef void (*EnFrBlinkFunc)(struct EnFr*);

typedef enum {
    /* 00 */ FROG_YELLOW,   // Middle
    /* 01 */ FROG_BLUE,     // Front Left
    /* 02 */ FROG_RED,      // Front Right
    /* 03 */ FROG_PURPLE,   // Back Left
    /* 04 */ FROG_WHITE     // Back Right
} FrogType;

typedef enum {
    /* 00 */ FROG_ZL, 
    /* 01 */ FROG_EPONA,
    /* 02 */ FROG_SARIA,
    /* 03 */ FROG_SUNS,
    /* 04 */ FROG_SOT,
    /* 05 */ FROG_STORMS,
    /* 06 */ FROG_CHOIR_SONG,
    /* 07 */ FROG_NO_SONG
} FrogSongType;

typedef struct {
    f32 xzDist;
    f32 yaw;
    f32 yDist;
} LogSpotToFromWater;

typedef struct EnFr {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;   // Frog Skeleton
    /* 0x0190 */ Vec3s jointTable[24];                                   
    /* 0x0220 */ Vec3s morphTable[24];
    /* 0x02B0 */ SkelAnime skelAnimeButterfly;  // Butterfly Skeleton above Frog during Frog Song
    /* 0x02F4 */ Vec3s jointTableButterfly[8]; 
    /* 0x0324 */ Vec3s morphTableButterfly[8];
    /* 0x0354 */ EnFrActionFunc actionFunc;
    /* 0x0358 */ LightNode* lightNode; // Frogs light up during the Frog Song, specifically the frog with the next note. 
    /* 0x035C */ LightInfo lightInfo;
    /* 0x036A */ s8 objBankIndex; 
    /* 0x036C */ s32 jumpCounter; // cyclic clock used to time celebratory frog jumps every 4 frames when child song is played.
    /* 0x0370 */ s32 blinkTimer; // blinking timer
    /* 0x0374 */ EnFrBlinkFunc blinkFunc; // unique case of using a function pointer for blinking
    /* 0x0378 */ u8 eyeTexIndex; // index for blinking graphics 
    /* 0x0379 */ u8 isJumpingUp; // Conditional for jumping up from the log back onto the log
    /* 0x037A */ u8 isBelowWaterSurfaceCurrent;   // Used for diving effects
    /* 0x037B */ u8 isBelowWaterSurfacePrevious;
    /* 0x037C */ u8 isDeactivating; // Related to debugging (osSyncPrintf)
    /* 0x037D */ u8 isActive; // Each Frog grows when their specific song is played.
    /* 0x037E */ u8 growingScaleIndex; // Target Scale Level 0-3 while growing (3 highest smooth)
    /* 0x037F */ u8 isGrowing; // While growing, the frog will switch between its larger size and its original size every frame
    /* 0x0380 */ u8 ocarinaNoteIndex; // Related to unk_381
    /* 0x0381 */ u8 ocarinaNote; // Ocarina Note to Play for Frogs 2
    /* 0x0382 */ u8 songIndex; // Song index 1-7 (6 songs + custom HP Song): 5 = sos, 6 = all songs
    /* 0x0383 */ u8 isJumpingToFrogSong; // Conditional: 
    /* 0x0384 */ char unk_384[0x02];
    /* 0x0386 */ s16 frogSongTimer; // timer #2 for frogs 2 HP
    /* 0x0388 */ s32 reward; // Prize for completing a song
    /* 0x038C */ Vec3f posLogSpot; // Where the frog goes when activated
    /* 0x0398 */ f32 xzDistToLogSpot; // z position or axis
    /* 0x039C */ f32 unusedFloat; // Unused float, only set to zero.
    /* 0x03A0 */ f32 scale; //scale
    /* 0x03A4 */ Actor* unusedButterflyActor; // unused pointer to Actor. Likely intended for butterfly
    /* 0x03A8 */ u8 isButterflyDrawn; // isButterflyDrawn
    /* 0x03AA */ s16 xyAngleButterfly; // Butterfly Travels along random angles in the x-y plane
    /* 0x03AC */ Vec3f posButterfly; // Position/Coordinates of the Butterfly
    /* 0x03B8 */ Vec3f posButterflyLight; // Used in Lights_PointNoGlowSetInfo()
    /*        */ GetItemEntry getItemEntry;
} EnFr; // size = 0x03C4

typedef struct {
    u8 flags;
    EnFr* frogs[5];
} EnFrPointers;

#endif
