#ifndef Z64TRANSITION_H
#define Z64TRANSITION_H

#include "ultra64.h"

#include "libc/stdint.h"
#include "unk.h"

#include "overlays/fbdemos/ovl_fbdemo_triforce/z_fbdemo_triforce.h"
#include "overlays/fbdemos/ovl_fbdemo_wipe1/z_fbdemo_wipe1.h"
#include "overlays/fbdemos/ovl_fbdemo_wipe3/z_fbdemo_wipe3.h"
#include "overlays/fbdemos/ovl_fbdemo_wipe4/z_fbdemo_wipe4.h"
#include "overlays/fbdemos/ovl_fbdemo_wipe5/z_fbdemo_wipe5.h"

typedef enum TransitionOverlayStatus {
    /* -1 */ TRANSITION_OVERLAY_STATUS_FAILED = -1, // failed allocation or null reference
    /*  0 */ TRANSITION_OVERLAY_STATUS_LOAD_FREE, // successfully loaded/freed overlay
    /*  1 */ TRANSITION_OVERLAY_STATUS_ADD_REMOVAL, // successfully added/removed instance
    /*  2 */ TRANSITION_OVERLAY_STATUS_LOADED_NO_INSTANCES, // overlay is loaded but has no instances (?) TODO: Figure out why this exists
    /*  3 */ TRANSITION_OVERLAY_STATUS_INTERNAL // internal overlay, so always loaded
} TransitionOverlayStatus;

typedef enum TransitionTileState {
    /* 0 */ TRANS_TILE_OFF, // Inactive, do nothing
    /* 1 */ TRANS_TILE_SETUP, // Save the necessary buffers
    /* 2 */ TRANS_TILE_PROCESS, // Initialize the transition
    /* 3 */ TRANS_TILE_READY // The transition is ready, so will update and draw each frame
} TransitionTileState;

typedef struct TransitionTileVtxData {
    /* 0x0 */ f32 x;
    /* 0x4 */ f32 y;
} TransitionTileVtxData; // size = 0x8

typedef struct TransitionTile {
    /* 0x00 */ s32 cols;
    /* 0x04 */ s32 rows;
    /* 0x08 */ s32 frame;
    /* 0x0C */ TransitionTileVtxData* vtxData;
    /* 0x10 */ Vtx* vtxFrame1;
    /* 0x14 */ Vtx* vtxFrame2;
    /* 0x18 */ Mtx projection;
    /* 0x58 */ Mtx modelView;
    /* 0x98 */ Mtx unk_98;
    /* 0xD8 */ Gfx* gfx; // "gfxtbl"
    /* 0xDC */ u16* zBuffer;
} TransitionTile; // size = 0xE0


#define TC_SET_PARAMS (1 << 7)

typedef struct TransitionProfile {
    /* 0x00 */ void* (*init)(void* transition);
    /* 0x04 */ void  (*destroy)(void* transition);
    /* 0x08 */ void  (*update)(void* transition, s32 updateRate);
    /* 0x0C */ void  (*draw)(void* transition, Gfx** gfxP);
    /* 0x10 */ void  (*start)(void* transition);
    /* 0x14 */ void  (*setType)(void* transition, s32 type);
    /* 0x18 */ void  (*setColor)(void* transition, u32 color);
    /* 0x1C */ void  (*setEnvColor)(void* transition, u32 color);
    /* 0x20 */ s32   (*isDone)(void* transition);
} TransitionProfile; // size = 0x24

typedef struct TransitionOverlay {
    union {
        struct {
    /* 0x00 */ u32 count : 8;
    /* 0x00 */ uintptr_t addr : 24;
        };
    /* 0x00 */ u32 word;
    } loadInfo;
    /* 0x04 */ void* vramStart;
    /* 0x08 */ void* vramEnd;
    /* 0x0C */ uintptr_t vromStart;
    /* 0x10 */ uintptr_t vromEnd;
    /* 0x14 */ TransitionProfile* profile;
    /* 0x18 */ size_t size;
} TransitionOverlay;

typedef enum {
    /* 1 */ TRANS_INSTANCE_TYPE_FILL_OUT = 1,
    /* 2 */ TRANS_INSTANCE_TYPE_FILL_IN
} TransitionInstanceType;

#define TRANS_INSTANCE_TYPE_FADE_FLASH 3

typedef struct TransitionFade {
    /* 0x0 */ u8 type; // TransitionFadeType enum
    /* 0x1 */ u8 isDone;
    /* 0x2 */ u8 direction;
    /* 0x4 */ Color_RGBA8_u32 color;
    /* 0x8 */ u16 timer;
} TransitionFade; // size = 0xC

#define FBDEMO_CIRCLE_GET_MASK_TYPE(type) ((type) & 1)

typedef struct TransitionCircle {
    /* 0x00 */ Color_RGBA8_u32 color;
    /* 0x04 */ f32 referenceRadius; // Reference for where to transition to
    /* 0x08 */ f32 stepValue; // How fast the Transition is 
    /* 0x0C */ f32 startingRadius; // Radius value where transition will begin
    /* 0x10 */ f32 targetRadius; // Final radius of transition circle
    /* 0x14 */ u8 direction; // Direction the circle is transitioning ( In / Out )
    /* 0x15 */ u8 maskType; // Positive / Negative mask type. Value of 0 will create a black circle
    /* 0x16 */ u8 isDone; // Signals when Transition is done updating
    /* 0x18 */ void const* texture;
    /* 0x1C */ u8 masks;
    /* 0x1D */ u8 maskt;
    /* 0x1E */ s8 unk_1E; // Set to 4 and never used
    /* 0x1F */ s8 unk_1F; // Set to 0 and never used
} TransitionCircle; // size = 0x20

#define TRANS_TRIGGER_OFF 0 // transition is not active
#define TRANS_TRIGGER_START 20 // start transition (exiting an area)
#define TRANS_TRIGGER_END -20 // transition is ending (arriving in a new area)

typedef enum TransitionMode {
    /*  0 */ TRANS_MODE_OFF,
    /*  1 */ TRANS_MODE_SETUP,
    /*  2 */ TRANS_MODE_INSTANCE_INIT,
    /*  3 */ TRANS_MODE_INSTANCE_RUNNING,
    /*  4 */ TRANS_MODE_FILL_WHITE_INIT,
    /*  5 */ TRANS_MODE_FILL_IN,
    /*  6 */ TRANS_MODE_FILL_OUT,
    /*  7 */ TRANS_MODE_FILL_BROWN_INIT,
    /*  8 */ TRANS_MODE_08, // unused
    /*  9 */ TRANS_MODE_09, // unused
    /* 10 */ TRANS_MODE_INSTANT,
    /* 11 */ TRANS_MODE_INSTANCE_WAIT,
    /* 12 */ TRANS_MODE_SANDSTORM_INIT,
    /* 13 */ TRANS_MODE_SANDSTORM,
    /* 14 */ TRANS_MODE_SANDSTORM_END_INIT,
    /* 15 */ TRANS_MODE_SANDSTORM_END,
    /* 16 */ TRANS_MODE_CS_BLACK_FILL_INIT,
    /* 17 */ TRANS_MODE_CS_BLACK_FILL
} TransitionMode;

#define TRANS_TYPE_WIPE4 (1 << 5)
#define TRANS_TYPE_WIPE3 (1 << 6)
#define TRANS_TYPE_SET_PARAMS (1 << 7)

typedef enum TransitionType {
    /*  0 */ TRANS_TYPE_WIPE,
    /*  1 */ TRANS_TYPE_TRIFORCE,
    /*  2 */ TRANS_TYPE_FADE_BLACK,
    /*  3 */ TRANS_TYPE_FADE_WHITE,
    /*  4 */ TRANS_TYPE_FADE_BLACK_FAST,
    /*  5 */ TRANS_TYPE_FADE_WHITE_FAST,
    /*  6 */ TRANS_TYPE_FADE_BLACK_SLOW,
    /*  7 */ TRANS_TYPE_FADE_WHITE_SLOW,
    /*  8 */ TRANS_TYPE_WIPE_FAST,
    /*  9 */ TRANS_TYPE_FILL_WHITE_FAST, // Not actually fast due to bug in Play_UpdateTransition
    /* 10 */ TRANS_TYPE_FILL_WHITE,
    /* 11 */ TRANS_TYPE_INSTANT,
    /* 12 */ TRANS_TYPE_FILL_BROWN,
    /* 13 */ TRANS_TYPE_FADE_WHITE_CS_DELAYED,
    /* 14 */ TRANS_TYPE_SANDSTORM_PERSIST,
    /* 15 */ TRANS_TYPE_SANDSTORM_END,
    /* 16 */ TRANS_TYPE_CS_BLACK_FILL,
    /* 17 */ TRANS_TYPE_FADE_WHITE_INSTANT,
    /* 18 */ TRANS_TYPE_FADE_GREEN,
    /* 19 */ TRANS_TYPE_FADE_BLUE,
    /* 20 */ TRANS_TYPE_FADE_DYNAMIC, // Chooses Black or White based on time of day
    /* 21 */ TRANS_TYPE_CIRCLE,
    /* 22 */ TRANS_TYPE_WIPE5,
    // transition types 23 - 31 are unused
    // transition types 32 - 39 are Wipe4 TODO needs macro
    /* 38 */ TRANS_TYPE_38 = 38,
    // transition types 40 - 63 are unused
    // transition types 64 - 127 are Wipe3 TODO needs macro
    /* 64 */ TRANS_TYPE_64 = 64,
    /* 70 */ TRANS_TYPE_70 = 70,
    /* 72 */ TRANS_TYPE_72 = 72,
    /* 73 */ TRANS_TYPE_73,
    /* 80 */ TRANS_TYPE_80 = 80,
    /* 86 */ TRANS_TYPE_86 = 86
} TransitionType;

#define DEFINE_TRANSITION(enumValue, _structName, _instanceName, _name) enumValue,
#define DEFINE_TRANSITION_INTERNAL(enumValue, _structName, _instanceName) enumValue,

typedef enum FbDemoType {
    #include "tables/transition_table.h"
    /* 7 */ FBDEMO_MAX
} FbDemoType;

#undef DEFINE_TRANSITION
#undef DEFINE_TRANSITION_INTERNAL

#define TRANS_NEXT_TYPE_DEFAULT 0xFF

#define DEFINE_TRANSITION(_enumValue, structName, instanceName, _name) structName instanceName;
#define DEFINE_TRANSITION_INTERNAL(_enumValue, structName, instanceName) structName instanceName;

typedef union TransitionInstance {
    #include "tables/transition_table.h"
} TransitionInstance; // size = 0x218

#undef DEFINE_TRANSITION
#undef DEFINE_TRANSITION_INTERNAL

typedef struct TransitionContext {
    /* 0x000 */ s16 transitionType;
    /* 0x002 */ s8 fbdemoType;
    /* 0x003 */ UNK_TYPE1 unk_003[0x5];
    /* 0x008 */ TransitionInstance instanceData;
    /* 0x220 */ UNK_TYPE1 unk_220[0x10];
    /* 0x230 */ void* (*init)(void* transition);
    /* 0x234 */ void  (*destroy)(void* transition);
    /* 0x238 */ void  (*update)(void* transition, s32 updateRate);
    /* 0x23C */ void  (*draw)(void* transition, Gfx** gfxP);
    /* 0x240 */ void  (*start)(void* transition);
    /* 0x244 */ void  (*setType)(void* transition, s32 type);
    /* 0x248 */ void  (*setColor)(void* transition, u32 color);
    /* 0x24C */ void  (*setEnvColor)(void* transition, u32 color);
    /* 0x250 */ s32   (*isDone)(void* transition);
    /* 0x254 */ UNK_TYPE1 unk_254[0x4];
} TransitionContext; // size = 0x258


// z_dbdemo_dlftbls.c

void Transition_Init(TransitionContext* transitionCtx);
void Transition_Destroy(TransitionContext* transitionCtx);

// z_fbdemo.c

void TransitionTile_Destroy(TransitionTile* thisx);
TransitionTile* TransitionTile_Init(TransitionTile* thisx, s32 cols, s32 rows);
void TransitionTile_Draw(TransitionTile* thisx, Gfx** gfxP);
void TransitionTile_Update(TransitionTile* thisx);

// z_fbdemo_fade.c

void TransitionFade_Start(void* thisx);
void* TransitionFade_Init(void* thisx);
void TransitionFade_Destroy(void* thisx);
void TransitionFade_Update(void* thisx, s32 updateRate);
void TransitionFade_Draw(void* thisx, Gfx** gfxP);
s32 TransitionFade_IsDone(void* thisx);
void TransitionFade_SetColor(void* thisx, u32 color);
void TransitionFade_SetType(void* thisx, s32 type);

// z_fbdemo_circle.c

void TransitionCircle_LoadAndSetTexture(Gfx** gfxP, void const* texture, s32 fmt, s32 arg3, s32 masks, s32 maskt, f32 arg6);

// z_overlay.c

void* TransitionOverlay_VramToRam(TransitionOverlay* overlayEntry, void* vramAddr);
void TransitionOverlay_VramToRamArray(TransitionOverlay *overlayEntry, void** vramAddrs, s32 count);
TransitionOverlayStatus TransitionOverlay_Load(TransitionOverlay *overlayEntry);
TransitionOverlayStatus TransitionOverlay_Free(TransitionOverlay *overlayEntry);
void TransitionOverlay_ClearLoadInfo(TransitionOverlay *overlayEntry);
void TransitionOverlay_SetSegment(TransitionOverlay *overlayEntry, void* vramStart, void* vramEnd, uintptr_t vromStart, uintptr_t vromEnd);

#endif
