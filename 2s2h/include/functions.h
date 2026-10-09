#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#define this thisx
#endif

#include "z64.h"

void bootproc(void);
void ViConfig_UpdateVi(u32 black);
void ViConfig_UpdateBlack(void);
s32 DmaMgr_DmaRomToRam(uintptr_t rom, void* ram, size_t size);
s32 DmaMgr_DmaHandler(OSPiHandle* pihandle, OSIoMesg* mb, s32 direction);
DmaEntry* DmaMgr_FindDmaEntry(uintptr_t vrom);
u32 DmaMgr_TranslateVromToRom(uintptr_t vrom);
s32 DmaMgr_FindDmaIndex(uintptr_t vrom);
const char* func_800809F4(uintptr_t param_1);
void DmaMgr_ProcessMsg(DmaRequest* req);
void DmaMgr_ThreadEntry(void* arg);
s32 DmaMgr_SendRequestImpl(DmaRequest* request, void* vramStart, uintptr_t vromStart, size_t size, UNK_TYPE4 unused, OSMesgQueue* queue, OSMesg msg);
s32 DmaMgr_SendRequest0(void* vramStart, uintptr_t vromStart, size_t size);
void DmaMgr_Start(void);
void DmaMgr_Stop(void);
void* Yaz0_FirstDMA(void);
void* Yaz0_NextDMA(void* curSrcPos);
s32 Yaz0_DecompressImpl(u8* src, u8* dst);
void Yaz0_Decompress(uintptr_t romStart, void* dst, size_t size);
void IrqMgr_AddClient(IrqMgr* irqmgr, IrqMgrClient* client, OSMesgQueue* msgQueue);
void IrqMgr_RemoveClient(IrqMgr* irqmgr, IrqMgrClient* remove);
void IrqMgr_SendMesgForClient(IrqMgr* irqmgr, OSMesg msg);
void IrqMgr_JamMesgForClient(IrqMgr* irqmgr, OSMesg msg);
void IrqMgr_HandlePreNMI(IrqMgr* irqmgr);
void IrqMgr_CheckStack(void);
void IrqMgr_HandlePRENMI450(IrqMgr* irqmgr);
void IrqMgr_HandlePRENMI480(IrqMgr* irqmgr);
void IrqMgr_HandlePRENMI500(IrqMgr* irqmgr);
void IrqMgr_HandleRetrace(IrqMgr* irqmgr);
void IrqMgr_ThreadEntry(IrqMgr* irqmgr);
void IrqMgr_Init(IrqMgr* irqmgr, void* stack, OSPri pri, u8 retraceCount);

void RcpUtils_PrintRegisterStatus(void);
void RcpUtils_Reset(void);

void PadUtils_Init(Input* input);
void func_80085150(void);
void PadUtils_ResetPressRel(Input* input);
u32 PadUtils_CheckCurExact(Input* input, u16 value);
u32 PadUtils_CheckCur(Input* input, u16 key);
u32 PadUtils_CheckPressed(Input* input, u16 key);
u32 PadUtils_CheckReleased(Input* input, u16 key);
u16 PadUtils_GetCurButton(Input* input);
u16 PadUtils_GetPressButton(Input* input);
s8 PadUtils_GetCurX(Input* input);
s8 PadUtils_GetCurY(Input* input);
void PadUtils_SetRelXY(Input* input, s32 x, s32 y);
s8 PadUtils_GetRelXImpl(Input* input);
s8 PadUtils_GetRelYImpl(Input* input);
s8 PadUtils_GetRelX(Input* input);
s8 PadUtils_GetRelY(Input* input);
void PadUtils_UpdateRelXY(Input* input);

void MtxConv_F2L(Mtx* mtx, MtxF* mf);
void MtxConv_L2F(MtxF* mtx, Mtx* mf);

s32 func_80086620(OSMesgQueue* param_1, PadMgr* param_2, OSContStatus* param_3);

s32 PrintUtils_VPrintf(PrintCallback* pfn, const char* fmt, va_list args);
s32 PrintUtils_Printf(PrintCallback* pfn, const char* fmt, ...);
void Sleep_Cycles(OSTime time);
void Sleep_Nsec(u32 nsec);
void Sleep_Usec(u32 usec);
void Sleep_Msec(u32 ms);
void Sleep_Sec(u32 sec);

f32 fmodf(f32 dividend, f32 divisor);
void* __osMemset(void* ptr, s32 val, size_t size);
s32 __osStrcmp(const char* str1, const char* str2);
char* __osStrcpy(char* dst, const char* src);
void* __osMemcpy(void* dst, void* src, size_t size);

// void EnItem00_SetObject(EnItem00* thisx, PlayState* play, f32* shadowOffset, f32* shadowScale);
// void EnItem00_Init(Actor* thisx, PlayState* play);
// void EnItem00_Destroy(Actor* thisx, PlayState* play);
// void EnItem00_WaitForHeartObject(EnItem00* thisx, PlayState* play);
// void func_800A640C(EnItem00* thisx, PlayState* play);
// void func_800A6650(EnItem00* thisx, PlayState* play);
// void func_800A6780(EnItem00* thisx, PlayState* play);
// void func_800A6A40(EnItem00* thisx, PlayState* play);
// void EnItem00_Update(Actor* thisx, PlayState* play);
// void EnItem00_Draw(Actor* thisx, PlayState* play);
// void EnItem00_DrawRupee(EnItem00* thisx, PlayState* play);
// void EnItem00_DrawSprite(EnItem00* thisx, PlayState* play);
// void EnItem00_DrawHeartContainer(EnItem00* thisx, PlayState* play);
// void EnItem00_DrawHeartPiece(EnItem00* thisx, PlayState* play);
// s16 func_800A7650(s16 dropId);
Actor* Item_DropCollectible(PlayState* play, Vec3f* spawnPos, u32 params);
Actor* Item_DropCollectible2(PlayState* play, Vec3f* spawnPos, s32 params);
void Item_DropCollectibleRandom(PlayState* play, Actor* fromActor, Vec3f* spawnPos, s16 params);
s32 func_800A8150(s32 index);
s32 func_800A817C(s32 index);
bool Item_CanDropBigFairy(PlayState* play, s32 index, s32 collectibleFlag);
void FlagSet_Update(GameState* gameState);
void FlagSet_Draw(GameState* gameState);
void Overlay_LoadGameState(GameStateOverlay* overlayEntry);
void Overlay_FreeGameState(GameStateOverlay* overlayEntry);

// Ideally these two prototypes would be in z64actor.h, but they use PlayerItemAction which would require including z64player.h there.
s32 Actor_OfferTalkExchange(Actor* actor, struct PlayState* play, f32 xzRange, f32 yRange, PlayerItemAction exchangeItemAction);
s32 Actor_OfferTalkExchangeEquiCylinder(Actor* actor, struct PlayState* play, f32 radius, PlayerItemAction exchangeItemAction);

void GetItem_Draw(PlayState* play, s16 drawId);

u16 QuestHint_GetTatlTextId(PlayState* play);

void Font_LoadChar(PlayState* play, u16 codePointIndex, s32 offset);
void Font_LoadCharNES(PlayState* play, u8 codePointIndex, s32 offset);
void Font_LoadMessageBoxEndIcon(Font* font, u16 icon);
void Font_LoadOrderedFont(Font* font);

void LifeMeter_Init(PlayState* play);
void LifeMeter_UpdateColors(PlayState* play);
s32 LifeMeter_SaveInterfaceHealth(PlayState* play);
s32 LifeMeter_IncreaseInterfaceHealth(PlayState* play);
s32 LifeMeter_DecreaseInterfaceHealth(PlayState* play);
void LifeMeter_Draw(PlayState* play);
void LifeMeter_UpdateSizeAndBeep(PlayState* play);
bool LifeMeter_IsCritical(void);

void Nmi_Init(void);
void Nmi_SetPrenmiStart(void);
// s32 Nmi_GetPrenmiHasStarted(void);
void MsgEvent_SendNullTask(void);
f32 OLib_Vec3fDist(Vec3f* a, Vec3f* b);
f32 OLib_Vec3fDistOutDiff(Vec3f* a, Vec3f* b, Vec3f* dest);
f32 OLib_Vec3fDistXZ(Vec3f* a, Vec3f* b);
f32 OLib_ClampMinDist(f32 val, f32 min);
f32 OLib_ClampMaxDist(f32 val, f32 max);
Vec3f OLib_Vec3fDistNormalize(Vec3f* a, Vec3f* b);
Vec3f OLib_VecSphToVec3f(VecSph* sph);
Vec3f OLib_VecGeoToVec3f(VecGeo* geo);
VecSph OLib_Vec3fToVecSph(Vec3f* vec);
VecGeo OLib_Vec3fToVecGeo(Vec3f* vec);
VecSph OLib_Vec3fDiffToVecSph(Vec3f* a, Vec3f* b);
VecGeo OLib_Vec3fDiffToVecGeo(Vec3f* a, Vec3f* b);
Vec3f OLib_AddVecGeoToVec3f(Vec3f* a, VecGeo* geo);
Vec3f OLib_Vec3fDiffRad(Vec3f* a, Vec3f* b);
Vec3f OLib_Vec3fDiffDegF(Vec3f* a, Vec3f* b);
Vec3s OLib_Vec3fDiffBinAng(Vec3f* a, Vec3f* b);
void OLib_Vec3fDiff(PosRot* a, Vec3f* b, Vec3f* dest, s16 mode);
void OLib_Vec3fAdd(PosRot* a, Vec3f* b, Vec3f* dest, s16 mode);

Path* Path_GetByIndex(PlayState* play, s16 index, s16 indexNone);
f32 Path_OrientAndGetDistSq(Actor* actor, Path* path, s16 waypoint, s16* yaw);
void Path_CopyLastPoint(Path* path, Vec3f* dest);

void Room_Noop(PlayState* play, Room* room, Input* input, s32 arg3);
void Room_Init(PlayState* play, RoomContext* roomCtx);
size_t Room_SetupFirstRoom(PlayState* play, RoomContext* roomCtx);
s32 Room_RequestNewRoom(PlayState* play, RoomContext* roomCtx, s32 index);
s32 Room_ProcessRoomRequest(PlayState* play, RoomContext* roomCtx);
void Room_Draw(PlayState* play, Room* room, u32 flags);

void Room_FinishRoomChange(PlayState* play, RoomContext* roomCtx);
s32 Inventory_GetBtnBItem(PlayState* play);
void Inventory_ChangeEquipment(s16 value);
u8 Inventory_DeleteEquipment(PlayState* play, s16 equipment);
void Inventory_ChangeUpgrade(s16 upgrade, u32 value);
s32 Inventory_IsMapVisible(s16 sceneId);
void Inventory_SetWorldMapCloudVisibility(s16 tingleIndex);
void Inventory_SaveDekuPlaygroundHighScore(s16 timerId);
void Inventory_IncrementSkullTokenCount(s16 sceneIndex);
s16 Inventory_GetSkullTokenCount(s16 sceneIndex);
void Inventory_SaveLotteryCodeGuess(PlayState* play);

s32 Schedule_RunScript(PlayState* play, u8* script, ScheduleOutput* output);

void Graph_FaultClient(void);
void Graph_InitTHGA(TwoHeadGfxArena* arena, Gfx* buffer, s32 size);
void Graph_SetNextGfxPool(GraphicsContext* gfxCtx);
GameStateOverlay* Graph_GetNextGameState(GameState* gameState);
uintptr_t Graph_FaultAddrConv(uintptr_t address, void* param);
void Graph_Init(GraphicsContext* gfxCtx);
void Graph_Destroy(GraphicsContext* gfxCtx);
void Graph_TaskSet00(GraphicsContext* gfxCtx, GameState* gameState);
void Graph_UpdateGame(GameState* gameState);
void Graph_ExecuteAndDraw(GraphicsContext* gfxCtx, GameState* gameState);
void Graph_Update(GraphicsContext* gfxCtx, GameState* gameState);
void Graph_ThreadEntry(void* arg);
Gfx* Graph_GfxPlusOne(Gfx* gfx);
Gfx* Graph_BranchDlist(Gfx* gfx, Gfx* dst);
void* Graph_DlistAlloc(Gfx** gfx, size_t size);

void Sched_SwapFramebuffer(CfbInfo* cfbInfo);
void Sched_RetraceUpdateFramebuffer(SchedContext* sched, CfbInfo* cfbInfo);
void Sched_HandleReset(SchedContext* sched);
void Sched_HandleStop(SchedContext* sched);
void Sched_HandleAudioCancel(SchedContext* sched);
void Sched_HandleGfxCancel(SchedContext* sched);
void Sched_QueueTask(SchedContext* sched, OSScTask* task);
void Sched_Yield(SchedContext* sched);
s32 Sched_Schedule(SchedContext* sched, OSScTask** spTask, OSScTask** dpTask, s32 state);
void Sched_TaskUpdateFramebuffer(SchedContext* sched, OSScTask* task);
void Sched_NotifyDone(SchedContext* sched, OSScTask* task);
void Sched_RunTask(SchedContext* sched, OSScTask* spTask, OSScTask* dpTask);
void Sched_HandleEntry(SchedContext* sched);
void Sched_HandleRetrace(SchedContext* sched);
void Sched_HandleRSPDone(SchedContext* sched);
void Sched_HandleRDPDone(SchedContext* sched);
void Sched_SendEntryMsg(SchedContext* sched);
void Sched_SendAudioCancelMsg(SchedContext* sched);
void Sched_SendGfxCancelMsg(SchedContext* sched);
void Sched_FaultClient(void* param1, void* param2);
void Sched_ThreadEntry(void* arg);
void Sched_Init(SchedContext* sched, void* stack, OSPri pri, u8 viModeType, UNK_TYPE arg4, IrqMgr* irqMgr);

void Mtx_SetTranslateScaleMtx(Mtx* mtx, f32 scaleX, f32 scaleY, f32 scaleZ, f32 translateX, f32 translateY, f32 translateZ);
void Mtx_SetRotationMtx(Mtx* mtx, s32 angle, f32 axisX, f32 axisY, f32 axisZ);
void Mtx_SetTranslationRotationScaleMtx(Mtx* mtx, f32 scaleX, f32 scaleY, f32 scaleZ, s32 angle, f32 axisX, f32 axisY, f32 axisZ,f32 translateX, f32 translateY, f32 translateZ);

void CmpDma_LoadFile(uintptr_t segmentVrom, s32 id, void* dst, size_t size);
void CmpDma_LoadAllFiles(uintptr_t segmentVrom, void* dst, size_t size);
// void Check_WriteRGBA16Pixel(u16* buffer, u32 x, u32 y, u32 value);
// void Check_WriteI4Pixel(u16* buffer, u32 x, u32 y, u32 value);
// void Check_DrawI4Texture(u16* buffer, u32 x, u32 y, u32 width, u32 height, u8* texture);
// void Check_ClearRGBA16(u16* buffer);
// void Check_DrawExpansionPakErrorMessage(void);
// void Check_DrawRegionLockErrorMessage(void);
void Check_ExpansionPak(void);
void Check_RegionIsSupported(void);

u64* SysUcode_GetUCodeBoot(void);
size_t SysUcode_GetUCodeBootSize(void);
// BENTODO move this into its own header so we don't have so much header bloat with lus bridge
uint32_t SysUcode_GetUCode(void);
u64* SysUcode_GetUCodeData(void);

s32 SysFlashrom_InitFlash(void);
s32 SysFlashrom_ReadData(void* addr, u32 pageNum, u32 pageCount);
void SysFlashrom_WriteDataAsync(u8* addr, u32 pageNum, u32 pageCount);
s32 SysFlashrom_IsBusy(void);
s32 SysFlashrom_AwaitResult(void);
void SysFlashrom_WriteDataSync(void* addr, u32 pageNum, u32 pageCount);

Acmd* AudioSynth_Update(Acmd* abiCmdStart, s32* numAbiCmds, s16* aiBufStart, s32 numSamplesPerFrame);

AudioTask* AudioThread_Update(void);
void AudioThread_QueueCmdF32(u32 opArgs, f32 data);
void AudioThread_QueueCmdS32(u32 opArgs, s32 data);
void AudioThread_QueueCmdS8(u32 opArgs, s8 data);
void AudioThread_QueueCmdU16(u32 opArgs, u16 data);
void AudioThread_QueueCmdPtr(u32 opArgs, void* data);
s32 AudioThread_ScheduleProcessCmds(void);
u32 AudioThread_GetExternalLoadQueueMsg(u32* retMsg);
u16* AudioThread_GetFontsForSequence(s32 seqId, u32* outNumFonts, u16* buff);
s32 func_80193C5C(void);
s32 AudioThread_ResetAudioHeap(s32 specId);
void AudioThread_PreNMIInternal(void);
s32 AudioThread_GetEnabledNotesCount(void);
u32 AudioThread_NextRandom(void);
void AudioThread_InitMesgQueues(void);

void Audio_InvalDCache(void* buf, size_t size);
void Audio_WritebackDCache(void* buf, size_t size);

void AudioPlayback_NoteDisable(Note* note);
void AudioPlayback_ProcessNotes(void);
TunedSample* AudioPlayback_GetInstrumentTunedSample(Instrument* instrument, s32 semitone);
Instrument* AudioPlayback_GetInstrumentInner(s32 fontId, s32 instId);
Drum* AudioPlayback_GetDrum(s32 fontId, s32 drumId);
SoundEffect* AudioPlayback_GetSoundEffect(s32 fontId, s32 sfxId);
s32 AudioPlayback_SetFontInstrument(s32 instrumentType, s32 fontId, s32 index, void* value);
void AudioPlayback_SeqLayerNoteDecay(SequenceLayer* layer);
void AudioPlayback_SeqLayerNoteRelease(SequenceLayer* layer);
void AudioPlayback_InitSyntheticWave(Note* note, SequenceLayer* layer);
void AudioPlayback_InitNoteLists(NotePool* pool);
void AudioPlayback_InitNoteFreeList(void);
void AudioPlayback_NotePoolClear(NotePool* pool);
void AudioPlayback_NotePoolFill(NotePool* pool, s32 count);
void AudioPlayback_AudioListRemove(AudioListItem* item);
Note* AudioPlayback_AllocNote(SequenceLayer* layer);
void AudioPlayback_NoteInitAll(void);

void AudioScript_SequenceChannelDisable(SequenceChannel* channel);
void AudioScript_SequencePlayerDisableAsFinished(SequencePlayer* seqPlayer);
void AudioScript_SequencePlayerDisable(SequencePlayer* seqPlayer);
void AudioScript_AudioListPushBack(AudioListItem* list, AudioListItem* item);
void* AudioScript_AudioListPopBack(AudioListItem* list);
void AudioScript_ProcessSequences(s32 arg0);
void AudioScript_SkipForwardSequence(SequencePlayer* seqPlayer);
void AudioScript_ResetSequencePlayer(SequencePlayer* seqPlayer);
void AudioScript_InitSequencePlayerChannels(s32 seqPlayerIndex);
void AudioScript_InitSequencePlayers(void);

void func_8019AE40(s32 param_1, s32 param_2, u32 param_3, s32 param_4);
void func_8019AEC0(UNK_PTR param_1, UNK_PTR param_2);

void Audio_Init(void);
void Audio_InitSound(void);
void Audio_Update(void);
void Audio_ResetForAudioHeapStep3(void);
void Audio_ResetForAudioHeapStep2(void);
void Audio_ResetForAudioHeapStep1(s32 specId);
void Audio_PreNMI(void);

// #region 2S2H [Port] Made Available via C++
s32 osContInit(OSMesgQueue* mq, u8* controllerBits, OSContStatus* status);
s32 osContStartReadData(OSMesgQueue* mesg);
void osContGetReadData(OSContPad* pad);
// #endregion
// #region 2S2H [Port] Previously unavailable functions, made available for porting
void PadMgr_ThreadEntry();
void Heaps_Alloc(void);
void KaleidoScope_UpdateOwlWarpNamePanel(PlayState* play);
void KaleidoScope_UpdateNamePanel(PlayState* play);
void SkinMatrix_Clear(MtxF* mf);
// #endregion
// #region 2S2H [Port] New methods added for porting
void AudioSeq_SetPortVolumeScale(u8 seqPlayerIndex, f32 volume);
float AudioSeq_GetPortVolumeScale(u8 seqPlayerIndex);
void Graph_OpenDisps(Gfx** dispRefs, Gfx* dispVals, GraphicsContext* gfxCtx, const char* file, s32 line);
void Graph_CloseDisps(Gfx** dispRefs, Gfx* dispVals, GraphicsContext* gfxCtx, const char* file, s32 line);
void Lights_GlowCheckPrepare(PlayState* play);
void Flags_SetWeekEventReg(s32 flag);
void Flags_ClearWeekEventReg(s32 flag);
void Flags_SetWeekEventRegHorseRace(u8 state);
void Flags_SetEventInf(s32 flag);
void Flags_ClearEventInf(s32 flag);
s32 Ship_CalcShouldDrawAndUpdate(PlayState* play, Actor* actor, Vec3f* projectedPos, f32 projectedW, bool* shouldDraw,
                                 bool* shouldUpdate);
// #endregion
// #region 2S2H [Rando]
s32 Flags_GetRandoInf(s32 flag);
void Flags_SetRandoInf(s32 flag);
void Flags_ClearRandoInf(s32 flag);
// #endregion
// #region 2S2H [Port] Stubbed methods
void osSetThreadPri(OSThread* thread, OSPri p);
OSPri osGetThreadPri(OSThread* t);
void osSyncPrintfUnused(const char* fmt, ...);
void osSyncPrintf(const char* fmt, ...);
void rmonPrintf(const char* fmt, ...);
void osCreateThread(OSThread* thread, OSId id, void* entry, void* arg, void* sp, OSPri p);
void osStartThread(OSThread* t);
void osViSwapBuffer(void* frameBufPtr);
void osViBlack(u8 active);
void osDestroyThread(OSThread* t);
void osViSetMode(OSViMode* modep);
void osViSetSpecialFeatures(u32 func);
s32 osContStartQuery(OSMesgQueue* mq);
s32 osContSetCh(u8 ch);
void osContGetQuery(OSContStatus* data);
void osSpTaskLoad(OSTask* intp);
void osSpTaskStartGo(OSTask* tp);
void* osViGetCurrentFramebuffer(void);
void* osViGetNextFramebuffer(void);
OSYieldResult osSpTaskYielded(OSTask* task);
void osSpTaskYield(void);
void osViSetXScale(f32 value);
void osViSetYScale(f32 value);
// #endregion

void Regs_InitData(PlayState* play);

#ifdef __cplusplus
}
#undef this
#endif

#endif
