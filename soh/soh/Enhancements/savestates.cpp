#include "savestates.h"

#include "GameVersions.h"

#include <cstdio> // std::sprintf

#include "spdlog/spdlog.h"

#include <soh/OTRGlobals.h>
#include <soh/OTRAudio.h>

#include "z64.h"
#include "z64save.h"
#include <variables.h>
#include <functions.h>
#include "z64map_mark.h"

extern "C" GlobalContext* gGlobalCtx;

// FROM z_lights.c
// I didn't feel like moving it into a header file.
#define LIGHTS_BUFFER_SIZE 32

typedef struct {
    /* 0x000 */ s32 numOccupied;
    /* 0x004 */ s32 searchIndex;
    /* 0x008 */ LightNode buf[LIGHTS_BUFFER_SIZE];
} LightsBuffer; // size = 0x188


extern "C" MtxF* sMatrixStack;
extern "C" MtxF* sCurrentMatrix;
extern "C" LightsBuffer sLightsBuffer;
extern "C" s16 sWarpTimerTarget;
extern "C" MapMarkData** sLoadedMarkDataTable;
extern "C" int32_t sInitRegs; // Camera
extern "C" int32_t gDbgCamEnabled; //Camera
extern "C" int32_t sDbgModeIdx; //Camera
extern "C" int16_t sNextUID;              // Camera
extern "C" int32_t sCameraInterfaceFlags; //Camera
extern "C" int32_t sCameraInterfaceAlpha;
extern "C" int32_t sCameraShrinkWindowVal;
extern "C" int32_t D_8011D3AC;
extern "C" int32_t sDemo5PrevAction12Frame;
extern "C" int32_t sDemo5PrevSfxFrame;
extern "C" int32_t D_8011D3F0;
extern "C" OnePointCsFull D_8011D6AC[];
extern "C" OnePointCsFull D_8011D724[];
extern "C" OnePointCsFull D_8011D79C[];
extern "C" OnePointCsFull D_8011D83C[];
extern "C" OnePointCsFull D_8011D88C[];
extern "C" OnePointCsFull D_8011D8DC[];
extern "C" OnePointCsFull D_8011D954[];
extern "C" OnePointCsFull D_8011D9F4[]; 
extern "C" int16_t D_8011DB08;
extern "C" int16_t D_8011DB0C;
extern "C" int32_t sOOBTimer;
extern "C" f32 D_8015CE50;
extern "C" f32 D_8015CE54;
extern "C" CamColChk D_8015CE58;

typedef struct SaveStateInfo {
    SaveStateHeader stateHeader;

    unsigned char sysHeapCopy[SYSTEM_HEAP_SIZE];
    unsigned char audioHeapCopy[AUDIO_HEAP_SIZE];

    SaveContext saveContextCopy;
    GameInfo gameInfoCopy;
    LightsBuffer lightBufferCopy;
    AudioContext audioContextCopy;
    MtxF mtxStackCopy[20]; // always 20 matricies
    MtxF currentMtxCopy;
    uint32_t rngSeed;
    int16_t blueWarpTimerCopy; /* From door_warp_1 */

    SeqScriptState seqScriptStateCopy[4];// Unrelocated
    unk_D_8016E750 unk_D_8016E750Copy[4];

    ActiveSound gActiveSoundsCopy[7][MAX_CHANNELS_PER_BANK];
    uint8_t gSoundBankMutedCopy[7];

    u8 D_801333F0_copy;
    u8 gAudioSfxSwapOff_copy;
    uint16_t gAudioSfxSwapSource_copy[10];
    uint16_t gAudioSfxSwapTarget_copy[10];
    uint8_t gAudioSfxSwapMode_copy[10];
    void (*D_801755D0_copy)(void);
    MapMarkData** sLoadedMarkDataTableCopy;

    //Static Data

    //Camera data
    int32_t sInitRegs_copy;
    int32_t gDbgCamEnabled_copy;
    int32_t sDbgModeIdx_copy;
    int16_t sNextUID_copy;             
    int32_t sCameraInterfaceFlags_copy;
    int32_t sCameraInterfaceAlpha_copy;
    int32_t sCameraShrinkWindowVal_copy;
    int32_t D_8011D3AC_copy;
    int32_t sDemo5PrevAction12Frame_copy;
    int32_t sDemo5PrevSfxFrame_copy;
    int32_t D_8011D3F0_copy;
    OnePointCsFull D_8011D6AC_copy[3];
    OnePointCsFull D_8011D724_copy[3];
    OnePointCsFull D_8011D79C_copy[3];
    OnePointCsFull D_8011D83C_copy[2];
    OnePointCsFull D_8011D88C_copy[2];
    OnePointCsFull D_8011D8DC_copy[3];
    OnePointCsFull D_8011D954_copy[4];
    OnePointCsFull D_8011D9F4_copy[3];
    int16_t D_8011DB08_copy;
    int16_t D_8011DB0C_copy;
    int32_t sOOBTimer_copy;
    f32 D_8015CE50_copy;
    f32 D_8015CE54_copy;
    CamColChk D_8015CE58_copy;





} SaveStateInfo;

class SaveState {
    friend class SaveStateMgr;

  public:
    SaveState(std::shared_ptr<SaveStateMgr> mgr, unsigned int slot);

  private:
    unsigned int slot;
    std::shared_ptr<SaveStateMgr> saveStateMgr;
    std::shared_ptr<SaveStateInfo> info;

    void Save(void);
    void Load(void);
    void BackupSeqScriptState(void);
    void LoadSeqScriptState(void);
    void BackupCameraData(void);
    void LoadCameraData(void);

    SaveStateInfo* GetSaveStateInfo(void);
};

SaveStateMgr::SaveStateMgr() {
    this->SetCurrentSlot(0);
}
SaveStateMgr::~SaveStateMgr() { 
    this->states.clear();
}

SaveState::SaveState(std::shared_ptr<SaveStateMgr> mgr, unsigned int slot) : saveStateMgr(mgr), slot(slot), info(nullptr) {
    this->info = std::make_shared<SaveStateInfo>();
}

void SaveState::BackupSeqScriptState(void) {
    for (unsigned int i = 0; i < 4; i++) {
        info->seqScriptStateCopy[i].value = gAudioContext.seqPlayers[i].scriptState.value;
        
        info->seqScriptStateCopy[i].remLoopIters[0] = gAudioContext.seqPlayers[i].scriptState.remLoopIters[0];
        info->seqScriptStateCopy[i].remLoopIters[1] = gAudioContext.seqPlayers[i].scriptState.remLoopIters[1];
        info->seqScriptStateCopy[i].remLoopIters[2] = gAudioContext.seqPlayers[i].scriptState.remLoopIters[2];
        info->seqScriptStateCopy[i].remLoopIters[3] = gAudioContext.seqPlayers[i].scriptState.remLoopIters[3];
        
        info->seqScriptStateCopy[i].depth = gAudioContext.seqPlayers[i].scriptState.depth;
        
        info->seqScriptStateCopy[i].pc = (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.pc - (uintptr_t)gAudioHeap);
        
        info->seqScriptStateCopy[i].stack[0] =
            (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.stack[0] - (uintptr_t)gAudioHeap);
        info->seqScriptStateCopy[i].stack[1] =
            (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.stack[1] - (uintptr_t)gAudioHeap);
        info->seqScriptStateCopy[i].stack[2] =
            (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.stack[2] - (uintptr_t)gAudioHeap);
        info->seqScriptStateCopy[i].stack[3] =
            (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.stack[3] - (uintptr_t)gAudioHeap);
    }
}

void SaveState::LoadSeqScriptState(void) {
    for (unsigned int i = 0; i < 4; i++) {
        gAudioContext.seqPlayers[i].scriptState.value = info->seqScriptStateCopy[i].value;

        gAudioContext.seqPlayers[i].scriptState.remLoopIters[0] = info->seqScriptStateCopy[i].remLoopIters[0];
        gAudioContext.seqPlayers[i].scriptState.remLoopIters[1] = info->seqScriptStateCopy[i].remLoopIters[1];
        gAudioContext.seqPlayers[i].scriptState.remLoopIters[2] = info->seqScriptStateCopy[i].remLoopIters[2];
        gAudioContext.seqPlayers[i].scriptState.remLoopIters[3] = info->seqScriptStateCopy[i].remLoopIters[3];

        gAudioContext.seqPlayers[i].scriptState.depth = info->seqScriptStateCopy[i].depth;

        gAudioContext.seqPlayers[i].scriptState.pc =
            (u8*)((uintptr_t)info->seqScriptStateCopy[i].pc + (uintptr_t)gAudioHeap);

        gAudioContext.seqPlayers[i].scriptState.stack[0] =
            (u8*)((uintptr_t)info->seqScriptStateCopy[i].stack[0] + (uintptr_t)gAudioHeap);
        gAudioContext.seqPlayers[i].scriptState.stack[1] =
            (u8*)((uintptr_t)info->seqScriptStateCopy[i].stack[1] + (uintptr_t)gAudioHeap);
        gAudioContext.seqPlayers[i].scriptState.stack[2] =
            (u8*)((uintptr_t)info->seqScriptStateCopy[i].stack[2] + (uintptr_t)gAudioHeap);
        gAudioContext.seqPlayers[i].scriptState.stack[3] =
            (u8*)((uintptr_t)info->seqScriptStateCopy[i].stack[3] + (uintptr_t)gAudioHeap);
    }
}

void SaveState::BackupCameraData(void) {
    info->sInitRegs_copy = sInitRegs;
    info->gDbgCamEnabled_copy = gDbgCamEnabled;
    info->sNextUID_copy = sNextUID;
    info->sCameraInterfaceFlags_copy = sCameraInterfaceFlags;
    info->sCameraInterfaceAlpha_copy = sCameraInterfaceAlpha;
    info->sCameraShrinkWindowVal_copy = sCameraShrinkWindowVal;
    info->D_8011D3AC_copy = D_8011D3AC;
    info->sDemo5PrevAction12Frame_copy = sDemo5PrevAction12Frame;
    info->sDemo5PrevSfxFrame_copy = sDemo5PrevSfxFrame;
    info->D_8011D3F0_copy = D_8011D3F0;
    memcpy(info->D_8011D6AC_copy, D_8011D6AC, sizeof(info->D_8011D6AC_copy));
    memcpy(info->D_8011D724_copy, D_8011D724, sizeof(info->D_8011D724_copy));
    memcpy(info->D_8011D79C_copy, D_8011D79C, sizeof(info->D_8011D79C_copy));
    memcpy(info->D_8011D83C_copy, D_8011D83C, sizeof(info->D_8011D83C_copy));
    memcpy(info->D_8011D88C_copy, D_8011D88C, sizeof(info->D_8011D88C_copy));
    memcpy(info->D_8011D8DC_copy, D_8011D8DC, sizeof(info->D_8011D8DC_copy));
    memcpy(info->D_8011D954_copy, D_8011D954, sizeof(info->D_8011D954_copy));
    memcpy(info->D_8011D9F4_copy, D_8011D9F4, sizeof(info->D_8011D9F4_copy));
    info->D_8011DB08_copy = D_8011DB08;
    info->D_8011DB0C_copy = D_8011DB0C;
    info->sOOBTimer_copy = sOOBTimer;
    info->D_8015CE50_copy = D_8015CE50;
    info->D_8015CE54_copy = D_8015CE54;
    memcpy(&info->D_8015CE58_copy, &D_8015CE58, sizeof(info->D_8015CE58_copy));
}

/*  int32_t sInitRegs_copy;
    int32_t gDbgCamEnabled_copy;
    int32_t sDbgModeIdx_copy;
    int16_t sNextUID_copy;             
    int32_t sCameraInterfaceFlags_copy;
    int32_t sCameraInterfaceAlpha_copy;
    int32_t sCameraShrinkWindowVal_copy;
    int32_t D_8011D3AC_copy;
    int32_t sDemo5PrevAction12Frame_copy;
    int32_t sDemo5PrevSfxFrame_copy;
    int32_t D_8011D3F0_copy;
    OnePointCsFull D_8011D6AC_copy[3];
    OnePointCsFull D_8011D724_copy[3];
    OnePointCsFull D_8011D79C_copy[3];
    OnePointCsFull D_8011D83C_copy[2];
    OnePointCsFull D_8011D88C_copy[2];
    OnePointCsFull D_8011D8DC_copy[3];
    OnePointCsFull D_8011D954_copy[4];
    OnePointCsFull D_8011D9F4_copy[3];
    int16_t D_8011DB08_copy;
    int16_t D_8011DB0C_copy;
    int32_t sOOBTimer_copy;
    f32 D_8015CE50_copy;
    f32 D_8015CE54_copy;
    CamColChk D_8015CE58_copy;
*/


void SaveState::LoadCameraData(void) {
    sInitRegs = info->sInitRegs_copy;
    gDbgCamEnabled = info->gDbgCamEnabled_copy;
    sDbgModeIdx = info->sDbgModeIdx_copy;
    sNextUID = info->sNextUID_copy;
    sCameraInterfaceAlpha = info->sCameraInterfaceAlpha_copy;
    sCameraInterfaceFlags = info->sCameraInterfaceFlags_copy;
    sCameraShrinkWindowVal = info->sCameraShrinkWindowVal_copy;
    D_8011D3AC = info->D_8011D3AC_copy;
    sDemo5PrevAction12Frame = info->sDemo5PrevAction12Frame_copy;
    sDemo5PrevSfxFrame = info->sDemo5PrevSfxFrame_copy;
    D_8011D3F0 = info->D_8011D3F0_copy;
    memcpy(D_8011D6AC, info->D_8011D6AC_copy, sizeof(info->D_8011D6AC_copy));
    memcpy(D_8011D724, info->D_8011D724_copy, sizeof(info->D_8011D724_copy));
    memcpy(D_8011D79C, info->D_8011D79C_copy, sizeof(info->D_8011D79C_copy));
    memcpy(D_8011D83C, info->D_8011D83C_copy, sizeof(info->D_8011D83C_copy));
    memcpy(D_8011D88C, info->D_8011D88C_copy, sizeof(info->D_8011D88C_copy));
    memcpy(D_8011D8DC, info->D_8011D8DC_copy, sizeof(info->D_8011D8DC_copy));
    memcpy(D_8011D954, info->D_8011D954_copy, sizeof(info->D_8011D954_copy));
    memcpy(D_8011D9F4, info->D_8011D9F4_copy, sizeof(info->D_8011D9F4_copy));
    D_8011DB08 = info->D_8011DB08_copy;
    D_8011DB0C = info->D_8011DB0C_copy;
    sOOBTimer = info->sOOBTimer_copy;
    D_8015CE50 = info->D_8015CE50_copy;
    D_8015CE54 = info->D_8015CE54_copy;
    memcpy(&D_8015CE58, &info->D_8015CE58_copy, sizeof(info->D_8015CE58_copy));






}

extern "C" void ProcessSaveStateRequests(void) {
    OTRGlobals::Instance->gSaveStateMgr->ProcessSaveStateRequests();
}

void SaveStateMgr::SetCurrentSlot(unsigned int slot) {
    this->currentSlot = slot;
}

unsigned int SaveStateMgr::GetCurrentSlot(void) {
    return this->currentSlot;
}

void SaveStateMgr::ProcessSaveStateRequests(void) {
    while (!this->requests.empty()) {
        const auto& request = this->requests.front();
        
        switch (request.type) {
            case RequestType::SAVE:
                if (!this->states.contains(request.slot)) {
                    this->states[request.slot] = std::make_shared<SaveState>(OTRGlobals::Instance->gSaveStateMgr, request.slot);
                }
                this->states[request.slot]->Save();
                break;
            case RequestType::LOAD:
                if (this->states.contains(request.slot)) {
                    this->states[request.slot]->Load();
                } else {
                    SPDLOG_ERROR("Invalid SaveState slot: {}", request.type);
                }
                break;
            [[unlikely]] default: 
                SPDLOG_ERROR("Invalid SaveState request type: {}", request.type);
                break;
        }
        this->requests.pop();
    }
}

SaveStateReturn SaveStateMgr::AddRequest(const SaveStateRequest request) {
    if (gGlobalCtx == nullptr) {
        return SaveStateReturn::FAIL_WRONG_GAMESTATE;
    }

    switch (request.type) { 
        case RequestType::SAVE:
            requests.push(request);
            break;
        case RequestType::LOAD:
            if (states.contains(request.slot)) {
                requests.push(request);
            } else {
                SPDLOG_ERROR("Invalid SaveState slot: {}", request.type);
                return SaveStateReturn::FAIL_INVALID_SLOT;
            }
            break;
        [[unlikely]] default: 
            SPDLOG_ERROR("Invalid SaveState request type: {}", request.type);
            break;
        
    }
    requests.push(request);
}

void SaveState::Save(void) {
    std::unique_lock<std::mutex> Lock(audio.mutex);
    memcpy(&info->sysHeapCopy, gSystemHeap, SYSTEM_HEAP_SIZE /* sizeof(gSystemHeap) */);
    memcpy(&info->audioHeapCopy, gAudioHeap, AUDIO_HEAP_SIZE /* sizeof(gAudioContext) */);

    memcpy(&info->audioContextCopy, &gAudioContext, sizeof(AudioContext));
    memcpy(&info->unk_D_8016E750Copy, D_8016E750, sizeof(info->unk_D_8016E750Copy));
    BackupSeqScriptState();

    memcpy(info->gActiveSoundsCopy, gActiveSounds, sizeof(gActiveSounds));
    memcpy(&info->gSoundBankMutedCopy, gSoundBankMuted, sizeof(info->gSoundBankMutedCopy));
    
    info->D_801333F0_copy = D_801333F0;
    info->gAudioSfxSwapOff_copy = gAudioSfxSwapOff;

    memcpy(&info->gAudioSfxSwapSource_copy, gAudioSfxSwapSource,
           sizeof(info->gAudioSfxSwapSource_copy));
    memcpy(&info->gAudioSfxSwapTarget_copy, gAudioSfxSwapTarget,
           sizeof(info->gAudioSfxSwapTarget_copy));
    memcpy(&info->gAudioSfxSwapMode_copy, gAudioSfxSwapMode, 
        sizeof(info->gAudioSfxSwapMode_copy));

    info->D_801755D0_copy = D_801755D0;

    memcpy(&info->saveContextCopy, &gSaveContext, sizeof(gSaveContext));
    memcpy(&info->gameInfoCopy, gGameInfo, sizeof(*gGameInfo));
    memcpy(&info->lightBufferCopy, &sLightsBuffer, sizeof(sLightsBuffer));
    memcpy(&info->mtxStackCopy, &sMatrixStack, sizeof(MtxF) * 20);
    memcpy(&info->currentMtxCopy, &sCurrentMatrix, sizeof(MtxF));

    //Various static data
    info->blueWarpTimerCopy = sWarpTimerTarget;
    info->sLoadedMarkDataTableCopy = sLoadedMarkDataTable;
    BackupCameraData();
    
}

void SaveState::Load(void) {
    std::unique_lock<std::mutex> Lock(audio.mutex);
    memcpy(gSystemHeap, &info->sysHeapCopy, SYSTEM_HEAP_SIZE);
    memcpy(gAudioHeap, &info->audioHeapCopy, AUDIO_HEAP_SIZE);

    memcpy(&gAudioContext, &info->audioContextCopy, sizeof(AudioContext));
    memcpy(D_8016E750, &info->unk_D_8016E750Copy, sizeof(info->unk_D_8016E750Copy));
    LoadSeqScriptState();


    memcpy(&gSaveContext, &info->saveContextCopy, sizeof(gSaveContext));
    memcpy(gGameInfo, &info->gameInfoCopy, sizeof(*gGameInfo));
    memcpy(&sLightsBuffer, &info->lightBufferCopy, sizeof(sLightsBuffer));
    memcpy(&sMatrixStack, &info->mtxStackCopy, sizeof(MtxF) * 20);
    memcpy(&sCurrentMatrix, &info->currentMtxCopy, sizeof(MtxF));
    sWarpTimerTarget = info->blueWarpTimerCopy;

    memcpy(gActiveSounds, info->gActiveSoundsCopy, sizeof(gActiveSounds));
    memcpy(gSoundBankMuted, &info->gSoundBankMutedCopy, sizeof(info->gSoundBankMutedCopy));
    D_801333F0 = info->D_801333F0_copy;
    gAudioSfxSwapOff = info->gAudioSfxSwapOff_copy;

    memcpy(gAudioSfxSwapSource, &info->gAudioSfxSwapSource_copy,
           sizeof(info->gAudioSfxSwapSource_copy));
    memcpy(gAudioSfxSwapTarget, &info->gAudioSfxSwapTarget_copy,
           sizeof(info->gAudioSfxSwapTarget_copy));
    memcpy(gAudioSfxSwapMode, &info->gAudioSfxSwapMode_copy,
           sizeof(info->gAudioSfxSwapMode_copy));
    
    //Various static data
    D_801755D0 = info->D_801755D0_copy;
    sLoadedMarkDataTable = info->sLoadedMarkDataTableCopy;
    LoadCameraData();

}
