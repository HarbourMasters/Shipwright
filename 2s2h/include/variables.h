#ifndef VARIABLES_H
#define VARIABLES_H

#include "z64.h"
#include "segment_symbols.h"
#include "macros.h"

#ifdef __cplusplus
extern "C"
{
#endif

// data
extern size_t gDmaMgrDmaBuffSize;
extern vs32 gIrqMgrResetStatus;
extern volatile OSTime sIrqMgrResetTime;
extern volatile OSTime gIrqMgrRetraceTime;
extern s32 sIrqMgrRetraceCount;

// extern UNK_TYPE1 sGfxPrintFontTLUT;
// extern UNK_TYPE1 sGfxPrintRainbowTLUT;
// extern UNK_TYPE1 sGfxPrintRainbowData;
// extern UNK_TYPE1 sGfxPrintFontData;
// extern UNK_TYPE4 D_80097524;
// extern u32 sRandInt;

extern u8 sYaz0DataBuffer[0x400];
extern u8* sYaz0CurDataEnd;
extern uintptr_t sYaz0CurRomStart;
extern u32 sYaz0CurSize;
extern u8* sYaz0MaxPtr;
extern void* gYaz0DecompressDstEnd;

// extern UNK_TYPE4 D_8009CD10;
// extern UNK_TYPE4 sArenaLockMsg;

extern DmaEntry dmadata[1568];

extern u8 sDropTable[DROP_TABLE_SIZE * DROP_TABLE_NUMBER];
extern u8 sDropTableAmounts[DROP_TABLE_SIZE * DROP_TABLE_NUMBER];
extern s32 D_801AE194[32];
extern u8 D_801AE214[32];

// extern s32 sEntryIndex;
// extern u32 sCurrentBit;
// extern s32 sTimer;

extern ActorOverlay gActorOverlayTable[ACTOR_ID_MAX];
extern ActorId gMaxActorId;
extern BgCheckSceneSubdivisionEntry sSceneSubdivisionList[];
extern BgSpecialSceneMaxObjects sCustomDynapolyMem[];

// extern UNK_TYPE4 D_801BDAC0;
// extern UNK_TYPE4 D_801BDAC4;
// extern UNK_TYPE4 D_801BDAC8;
// extern UNK_TYPE4 D_801BDACC;
// extern UNK_TYPE4 D_801BDAF0;
// extern UNK_TYPE4 D_801BDAF8;

extern u8 kanfontOrdering[92];
// extern actor_init_var_func sInitChainHandlers[11];

extern UNK_PTR D_801BF5C0;
// extern UNK_TYPE4 D_801BEAD4;
// extern UNK_TYPE4 D_801BEAD8;
// extern UNK_TYPE1 D_801BEAE0;
// extern UNK_TYPE4 D_801BEAF4;
// extern UNK_TYPE4 D_801BEAF8;
// extern UNK_TYPE4 D_801BEB04;
// extern UNK_TYPE4 D_801BEB08;
// extern UNK_TYPE4 D_801BEB14;
// extern UNK_TYPE4 D_801BEB18;
// extern UNK_TYPE4 D_801BEB24;
// extern UNK_TYPE4 D_801BEB28;
// extern UNK_TYPE1 D_801BEB38;
// extern UNK_TYPE4 D_801BEBB8;
// extern UNK_TYPE4 D_801BEBD8;
// extern UNK_TYPE2 D_801BEBF8;
// extern UNK_TYPE2 D_801BEBFA;
// extern UNK_TYPE2 D_801BEBFC;
// extern UNK_TYPE2 D_801BEC10;
extern UNK_PTR D_801BEC14;
// extern UNK_TYPE4 D_801BEC1C;
// extern UNK_TYPE4 D_801BEC20;
// extern UNK_TYPE1 D_801BEC24;
// extern UNK_TYPE1 D_801BEC2C;
// extern UNK_TYPE2 D_801BEC5C;
// extern UNK_TYPE2 D_801BEC5E;
// extern UNK_TYPE4 D_801BEC70;
// extern UNK_TYPE1 D_801BEC84;
// extern UNK_TYPE1 D_801BECA4;
// extern UNK_TYPE1 D_801BECC4;
// extern UNK_TYPE1 D_801BECE4;
// extern UNK_TYPE1 D_801BED00;
// extern UNK_TYPE2 D_801BED24;
// extern UNK_TYPE1 D_801BED3C;
// extern UNK_TYPE1 D_801BED40;
// extern UNK_TYPE1 D_801BED4C;
// extern UNK_TYPE1 D_801BED54;
// extern UNK_TYPE1 D_801BED55;
// extern UNK_TYPE1 D_801BED56;
// extern UNK_TYPE1 D_801BED88;
// extern UNK_TYPE1 D_801BEFC8;
// extern UNK_TYPE1 D_801BF15C;
// extern UNK_TYPE1 D_801BF170;
// extern UNK_TYPE1 D_801BF176;
// extern UNK_TYPE1 D_801BF177;
// extern UNK_TYPE1 D_801BF178;
// extern UNK_TYPE1 D_801BF3B4;
// extern UNK_TYPE2 D_801BF550;
// extern UNK_TYPE2 D_801BF554;
// extern UNK_TYPE2 D_801BF558;
// extern UNK_TYPE4 D_801BF55C;
// extern UNK_TYPE4 D_801BF580;
// extern UNK_TYPE4 D_801BF594;
// extern UNK_TYPE4 D_801BF5A4;
// extern UNK_TYPE2 D_801BF5B0;
// extern UNK_TYPE1 D_801BF68C;

extern FlexSkeletonHeader* gPlayerSkeletons[PLAYER_FORM_MAX];
extern PlayerModelIndices gPlayerModelTypes[];
extern struct_80124618 D_801C03A0[];
extern struct_80124618 D_801C0490[];
extern Gfx gCullBackDList[];
extern Gfx gCullFrontDList[];

extern u32 gBitFlags[32];
extern u16 gEquipMasks[];
extern u16 gEquipNegMasks[];
extern u32 gUpgradeMasks[8];
extern u32 gUpgradeNegMasks[];
extern u8 gEquipShifts[];
extern u8 gUpgradeShifts[8];
extern u16 gUpgradeCapacities[][4];
extern u32 gGsFlagsMask[];
extern u32 gGsFlagsShift[];
extern TexturePtr gItemIcons[131];
extern u8 gItemSlots[77];
extern s16 gItemPrices[];
extern u16 gSceneIdsPerRegion[11][27];
extern u8 gPlayerFormItemRestrictions[PLAYER_FORM_MAX][114];

extern s16 gPlayerFormObjectIds[PLAYER_FORM_MAX];
extern ObjectId gObjectTableSize;
extern RomFile gObjectTable[OBJECT_ID_MAX];

extern SceneTableEntry gSceneTable[SCENE_MAX];
extern UNK_PTR D_801C5C50;
// extern UNK_TYPE1 D_801C5C9C;
extern UNK_PTR D_801C5CB0;

// extern UNK_TYPE1 D_801D0D54;
// extern UNK_TYPE2 sQuakeIndex;
// extern UNK_TYPE2 sIsCameraUnderwater;
extern Input* D_801D0D60;
// extern UNK_TYPE2 sPlayerCsIdToCsCamId;
// extern UNK_TYPE1 D_801D0D7A;

extern u32 retryCount;
extern u32 cfbIdx[3];

extern s16 gLowPassFilterData[];
extern s16 gHighPassFilterData[];
extern s16 gBandStopFilterData[];
extern s16 gBandPassFilterData[];
extern s16* gWaveSamples[9];
extern f32 gBendPitchOneOctaveFrequencies[];
extern f32 gBendPitchTwoSemitonesFrequencies[];
extern f32 gPitchFrequencies[];
extern u8 gDefaultShortNoteVelocityTable[];
extern u8 gDefaultShortNoteGateTimeTable[];
extern EnvelopePoint gDefaultEnvelope[];
extern NoteSampleState gZeroedSampleState;
extern NoteSampleState gDefaultSampleState;
extern u16 gHaasEffectDelaySize[];
extern u16 gHaasEffectDelaySize[];
extern s16 gInvalidAdpcmCodeBook[];
extern f32 gHeadsetPanVolume[];
extern f32 gStereoPanVolume[];
extern f32 gDefaultPanVolume[];
extern s32 gAudioCtxInitalized;
extern u8 D_801D6200[0x400];

extern u8 gAudioSpecId;
extern u8 gAudioHeapResetState;
extern AudioSpec gAudioSpecs[21];

// rodata
extern const u16 gAudioEnvironmentalSfx[];
// extern UNK_TYPE1 D_801E0BFC;
extern f32 D_801E0CEC;
extern f32 D_801E0CF0;
extern f32 D_801E0CF4;
extern f32 D_801E0CF8;
extern f32 D_801E0CFC;
extern f32 D_801E0D20;
extern f32 D_801E0D24;
extern f32 D_801E0D28;
extern f32 D_801E0D2C;
extern f32 D_801E0D30;
extern f32 D_801E0D34;
extern f64 D_801E0D58;
extern f32 D_801E0D60;
extern f32 D_801E0D64;
extern f32 D_801E0D68;
extern f32 D_801E0D8C;
extern f32 D_801E0D90;
extern f32 D_801E0D94;
extern f32 D_801E0D98;
extern f32 D_801E0D9C;
extern f32 D_801E0DBC;
extern f32 D_801E0DC0;
extern f32 D_801E0DC4;
extern f32 D_801E0DC8;
extern f32 D_801E0DCC;
extern f32 D_801E0DD0;
extern f32 D_801E0DD4;
extern f64 D_801E0DD8;
extern f64 D_801E0DE0;
extern f32 D_801E0DE8;
extern f32 D_801E0DEC;
extern f32 D_801E0DF0;
extern f32 D_801E0DF4;
extern f32 D_801E0DF8;
extern f32 D_801E0DFC;
extern f32 D_801E0E00;
extern f32 D_801E0E04;
extern f32 D_801E0E08;
extern f32 D_801E0E0C;
extern f32 D_801E0E10;
extern f32 D_801E0E14;
extern f32 D_801E0E18;
extern f32 D_801E0E1C;
extern f32 D_801E0E20;
extern f32 D_801E0E24;
extern f64 D_801E0EB0;
// extern UNK_TYPE4 D_801E1068;
extern UNK_PTR D_801E10B0;
extern const s16 gAudioTatumInit[];
extern const AudioHeapInitSizes gAudioHeapInitSizes;
// extern UNK_TYPE4 D_801E1108;
// extern UNK_TYPE4 D_801E110C;
extern u8 gSoundFontTable[];
extern u8 gSequenceFontTable[];
extern u8 gSequenceTable[];
extern u8 gSampleBankTable[];

// bss
// extern UNK_TYPE1 D_801ED894;

// extern UNK_TYPE1 D_801F4E20;
// extern UNK_TYPE1 sBeatingHeartsDDPrim;
// extern UNK_TYPE1 sBeatingHeartsDDEnv;
// extern UNK_TYPE1 sHeartsDDPrim;
// extern UNK_TYPE1 D_801F4F56;
// extern UNK_TYPE1 D_801F4F58;
// extern UNK_TYPE1 D_801F4F5A;
// extern UNK_TYPE1 D_801F4F60;
// extern UNK_TYPE1 D_801F4F66;
// extern UNK_TYPE1 D_801F4F68;
// extern UNK_TYPE1 D_801F4F6A;

// extern UNK_TYPE1 sSkyboxDrawMatrix;
// extern UNK_TYPE1 D_801F6AF0;
// extern UNK_TYPE1 D_801F6AF2;
// extern UNK_TYPE4 D_801F6B00;
// extern UNK_TYPE4 D_801F6B04;
// extern UNK_TYPE4 D_801F6B08;

extern void (*sKaleidoScopeUpdateFunc)(PlayState* play);
extern void (*sKaleidoScopeDrawFunc)(PlayState* play);

extern GfxMasterList* gGfxMasterDL;

extern u64* gAudioSPDataPtr;
extern u32 gAudioSPDataSize;

extern MtxF* sMatrixStack;
extern MtxF* sCurrentMatrix;

extern Color_RGBA8_u32 gVisMonoColor;

extern s32 D_801FD120;
extern u8 sResetAudioHeapTimer;
extern u16 sResetAudioHeapFadeReverbVolume;
extern u16 sResetAudioHeapFadeReverbVolumeStep;
extern AudioContext gAudioCtx; // at 0x80200C70
extern AudioCustomUpdateFunction gAudioCustomUpdateFunction;
extern AudioCustomSeqFunction gAudioCustomSeqFunction;
extern AudioCustomReverbFunction gAudioCustomReverbFunction;
extern AudioCustomSynthFunction gAudioCustomSynthFunction;

// other segments
extern Mtx D_01000000;
extern u16 D_0F000000[];

// #region 2S2H [General]
extern PlayState* gPlayState;
extern GameState* gGameState;
// #endregion

#ifdef __cplusplus
};
#endif

#endif
