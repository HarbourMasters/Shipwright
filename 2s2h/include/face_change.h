#ifndef FACE_CHANGE_H
#define FACE_CHANGE_H

typedef struct FaceChange {
    /* 0x0 */ s16 face;
    /* 0x2 */ s16 timer;
} FaceChange; // size = 0x4

s16 FaceChange_UpdateBlinking(FaceChange* faceChange, s16 blinkIntervalBase, s16 blinkIntervalRandRange, s16 blinkDuration);
s16 FaceChange_UpdateBlinkingNonHuman(FaceChange* faceChange, s16 blinkIntervalBase, s16 blinkIntervalRandRange, s16 blinkDuration);
s16 FaceChange_UpdateRandomSet(FaceChange* faceChange, s16 changeTimerBase, s16 changeTimerRandRange, s16 faceSetRange);

#endif
