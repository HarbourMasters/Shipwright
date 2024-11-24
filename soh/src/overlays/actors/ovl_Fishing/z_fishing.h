#ifndef Z_FISHING_H
#define Z_FISHING_H

#include <libultraship/libultra.h>
#include "global.h"
#include "soh/Enhancements/randomizer/fishsanity.h"

struct Fishing;

typedef struct Fishing {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ char unk_14C[0x004];
    /* 0x0150 */ u8 isLoach;
    /* 0x0151 */ u8 lilyTimer; // if near lily and >0, lily moves. Move more if >20
    /* 0x0152 */ u8 unk_152;
    /* 0x0154 */ s16 unk_154;
    /* 0x0156 */ u8 unk_156;
    /* 0x0157 */ u8 unk_157;
    /* 0x0158 */ s16 fishState;  // negative index for loach behavior
    /* 0x015A */ s16 fishStateNext;
    /* 0x015C */ s16 stateAndTimer; // fish use as timer that's AND'd, owner as talking state
    /* 0x015E */ s16 unk_15E;
    /* 0x0160 */ s16 unk_160; // fish use as rotateX, owner as index of eye texture
    /* 0x0162 */ s16 unk_162; // fish use as rotateY, owner as index of eye texture
    /* 0x0164 */ s16 unk_164; // fish use as rotateZ, owner as rotation of head
    /* 0x0166 */ Vec3s rotationTarget;
    /* 0x016C */ s16 fishLimb23RotYDelta;
    /* 0x016E */ s16 unk_16E;
    /* 0x0170 */ s16 fishLimbDRotZDelta;
    /* 0x0172 */ s16 fishLimbEFRotYDelta;
    /* 0x0174 */ s16 fishLimb89RotYDelta;
    /* 0x0176 */ s16 fishLimb4RotYDelta;
    /* 0x0178 */ s16 unk_178;
    /* 0x017A */ s16 timerArray[4];
    /* 0x0184 */ f32 unk_184;
    /* 0x0188 */ f32 speedTarget;
    /* 0x018C */ f32 fishLimbRotPhase;
    /* 0x0190 */ f32 unk_190; // fishLimbRotPhaseStep target
    /* 0x0194 */ f32 unk_194; // fishLimbRotPhaseMag target
    /* 0x0198 */ f32 fishLimbRotPhaseStep;
    /* 0x019C */ f32 fishLimbRotPhaseMag;
    /* 0x01A0 */ s16 bumpTimer; // set when hitting a wall.
    /* 0x01A2 */ s16 unk_1A2; // "scared" timer?
    /* 0x01A4 */ s16 unk_1A4; // "scared" timer? set at same time as above
    /* 0x01A8 */ f32 perception; // how easily they are drawn to the lure.
    /* 0x01AC */ f32 fishLength; // fish are (x^2*.0036+.5) lbs, loach double that.
    /* 0x01B0 */ f32 rotationStep;
    /* 0x01B4 */ Vec3f fishTargetPos;
    /* 0x01C0 */ Vec3f fishMouthPos;
    /* 0x01CC */ s16 loachRotYDelta[3]; // adds rotation to the loach limb 3-5.
    /* 0x01D2 */ u8 bubbleTime; // spawn bubbles while >0
    /* 0x01D3 */ u8 isAquariumMessage;
    /* 0x01D4 */ u8 aquariumWaitTimer;
    /* 0x01D5 */ u8 keepState; // case-switch and keeping or releasing a fish
    /* 0x01D8 */ SkelAnime skelAnime;
    /* 0x021C */ LightNode* lightNode;
    /* 0x0220 */ LightInfo lightInfo;
    /* 0x0230 */ ColliderJntSph collider;
    /* 0x0250 */ ColliderJntSphElement colliderElements[12];
} Fishing; // size = 0x0550

#define EN_FISH_OWNER 1      // param for owner of pond. default if params<100
#define EN_FISH_PARAM 100    // param base for fish in pond.
#define EN_FISH_AQUARIUM 200 // param for record fish in tank.

struct VBFishingData {
    Fishing* actor;
    u8* sFishOnHandIsLoach;
    u8* sSinkingLureLocation;
    f32* sFishOnHandLength;
    f32 fishWeight;
    f32 sFishingRecordLength;
};

#endif
