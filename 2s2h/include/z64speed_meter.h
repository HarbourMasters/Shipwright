#ifndef Z64SPEED_METER_H
#define Z64SPEED_METER_H

#include "ultra64.h"

struct GameState;
struct GraphicsContext;

typedef struct SpeedMeter {
    /* 0x00 */ char unk_00[0x18];
    /* 0x18 */ s32 x; // Unused
    /* 0x1C */ s32 y;
} SpeedMeter; // size = 0x20

void SpeedMeter_Init(SpeedMeter* this);
void SpeedMeter_Destroy(SpeedMeter* this);
void SpeedMeter_DrawTimeEntries(SpeedMeter* this, struct GraphicsContext* gfxCtx);
void SpeedMeter_DrawAllocEntries(SpeedMeter* this, struct GraphicsContext* gfxCtx, struct GameState* state);

extern volatile OSTime gRSPAudioTimeTotal;
extern volatile OSTime gRSPGfxTimeTotal;
extern volatile OSTime gRDPTimeTotal;
extern volatile OSTime gGraphUpdatePeriod;
extern volatile OSTime gRSPAudioTimeAcc;
extern volatile OSTime gRSPGfxTimeAcc;
extern volatile OSTime gRSPOtherTimeAcc;
extern volatile OSTime gUnkTimeAcc;
extern volatile OSTime gRDPTimeAcc;

#endif
