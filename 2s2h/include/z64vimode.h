#ifndef Z64_VIMODE_H
#define Z64_VIMODE_H

#include "ultra64.h"

struct Input;

typedef enum {
    /* 0 */ VI_MODE_EDIT_STATE_INACTIVE,
    /* 1 */ VI_MODE_EDIT_STATE_ACTIVE,
    /* 2 */ VI_MODE_EDIT_STATE_2, // active, more adjustments
    /* 3 */ VI_MODE_EDIT_STATE_3  // active, more adjustments, print comparison with NTSC LAN1 mode
} ViModeEditState;

typedef struct ViMode {
    /* 0x00 */ OSViMode customViMode;
    /* 0x50 */ s32 viHeight;
    /* 0x54 */ s32 viWidth;
    /* 0x58 */ s32 rightAdjust;
    /* 0x5C */ s32 leftAdjust;
    /* 0x60 */ s32 lowerAdjust;
    /* 0x64 */ s32 upperAdjust;
    /* 0x68 */ s32 editState;
    /* 0x6C */ s32 tvType;
    /* 0x70 */ u32 loRes;
    /* 0x74 */ u32 antialiasOff;
    /* 0x78 */ u32 modeN;
    /* 0x7C */ u32 fb16Bit;
    /* 0x80 */ u32 viFeatures;
    /* 0x84 */ u32 unk_84;
} ViMode; // size = 0x88

void ViMode_LogPrint(OSViMode* vimode);
void ViMode_Configure(OSViMode* viMode, s32 type, s32 tvType, s32 loRes, s32 antialiasOff, s32 modeN, s32 fb16Bit,
                      s32 width, s32 height, s32 leftAdjust, s32 rightAdjust, s32 upperAdjust, s32 lowerAdjust);
void ViMode_Save(ViMode* viMode);
void ViMode_Load(ViMode* viMode);
void ViMode_Init(ViMode* viMode);
void ViMode_Destroy(ViMode* viMode);
void ViMode_ConfigureFeatures(ViMode* viMode, s32 viFeatures);
void ViMode_Update(ViMode* viMode, struct Input* input);

#endif
