#pragma once

#include "GlobalCtx2.h"

#ifdef __cplusplus
class OTRGlobals
{
public:
	static OTRGlobals* Instance;

	std::shared_ptr<Ship::GlobalCtx2> context;

	OTRGlobals();
	~OTRGlobals();

private:

};
#endif

#ifndef __cplusplus
void InitOTR();
void Graph_ProcessFrame(void (*run_one_game_iter)(void));
void Graph_ProcessGfxCommands(Gfx* commands);
void OTRLogString(const char* src);
void OTRGfxPrint(const char* str, void* printer, void (*printImpl)(void*, char));
void OTRSetFrameDivisor(int divisor);
uint16_t OTRGetPixelDepth(float x, float y);
int32_t OTRGetLastScancode();
uint32_t ResourceMgr_GetGameVersion();
void ResourceMgr_CacheDirectory(const char* resName);
void ResourceMgr_LoadFile(const char* resName);
char* ResourceMgr_LoadFileFromDisk(const char* filePath);
char* ResourceMgr_LoadTexByName(char* texPath);
char* ResourceMgr_LoadTexOrDListByName(char* filePath);
char* ResourceMgr_LoadPlayerAnimByName(char* animPath);
char* ResourceMgr_GetNameByCRC(uint64_t crc, char* alloc);
Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc);
Gfx* ResourceMgr_LoadGfxByName(char* path);
Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc);
Vtx* ResourceMgr_LoadVtxByName(char* path);
CollisionHeader* ResourceMgr_LoadColByName(char* path);
void Ctx_ReadSaveFile(uintptr_t addr, void* dramAddr, size_t size);
void Ctx_WriteSaveFile(uintptr_t addr, void* dramAddr, size_t size);
char* Config_getValue(char* category, char* key);
bool Config_setValue(char* category, char* key, char* value);
uint64_t GetPerfCounter();
struct SkeletonHeader* ResourceMgr_LoadSkeletonByName(char* path);
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
#endif
