#ifndef OTR_GLOBALS_H
#define OTR_GLOBALS_H

#pragma once

#include "GlobalCtx2.h"

#ifdef __cplusplus
#include "Enhancements/savestates.h"
class OTRGlobals
{
public:
    static OTRGlobals* Instance;

    std::shared_ptr<Ship::GlobalCtx2> context;
    std::shared_ptr<SaveStateMgr> gSaveStateMgr;

    OTRGlobals();
    ~OTRGlobals();

private:

};
#endif

#ifndef __cplusplus
void InitOTR();
void Graph_ProcessFrame(void (*run_one_game_iter)(void));
void Graph_StartFrame();
void Graph_ProcessGfxCommands(Gfx* commands);
void OTRLogString(const char* src);
void OTRGfxPrint(const char* str, void* printer, void (*printImpl)(void*, char));
void OTRGetPixelDepthPrepare(float x, float y);
uint16_t OTRGetPixelDepth(float x, float y);
int32_t OTRGetLastScancode();
uint32_t ResourceMgr_GetGameVersion();
void ResourceMgr_CacheDirectory(const char* resName);
void ResourceMgr_LoadFile(const char* resName);
char* ResourceMgr_LoadFileFromDisk(const char* filePath);
char* ResourceMgr_LoadTexByName(const char* texPath);
char* ResourceMgr_LoadTexOrDListByName(const char* filePath);
char* ResourceMgr_LoadPlayerAnimByName(const char* animPath);
char* ResourceMgr_GetNameByCRC(uint64_t crc, char* alloc);
Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
Gfx* ResourceMgr_PatchGfxByName(const char* path, int size);
Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc);
Vtx* ResourceMgr_LoadVtxByName(const char* path);
CollisionHeader* ResourceMgr_LoadColByName(const char* path);
uint64_t GetPerfCounter();
struct SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path);
int ResourceMgr_OTRSigCheck(char* imgData);
uint64_t osGetTime(void);
uint32_t osGetCount(void);
uint32_t OTRGetCurrentWidth(void);
uint32_t OTRGetCurrentHeight(void);
float OTRGetAspectRatio(void);
float OTRGetDimensionFromLeftEdge(float v);
float OTRGetDimensionFromRightEdge(float v);
int16_t OTRGetRectDimensionFromLeftEdge(float v);
int16_t OTRGetRectDimensionFromRightEdge(float v);
void bswapDrum(Drum* swappable);
void bswapInstrument(Instrument* swappable);
bool bswapSoundFontSound(SoundFontSound* swappable);
void bswapSoundFontSample(SoundFontSample* swappable);
void bswapAdpcmLoop(AdpcmLoop* swappable);
void bswapAdpcmBook(AdpcmBook* swappable);
char* ResourceMgr_LoadFileRaw(const char* resName);
bool AudioPlayer_Init(void);
int AudioPlayer_Buffered(void);
int AudioPlayer_GetDesiredBuffered(void);
void AudioPlayer_Play(const uint8_t* buf, uint32_t len);
void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);
int Controller_ShouldRumble(size_t i);
#endif

#endif