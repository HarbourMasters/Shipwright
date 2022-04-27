#include "savestates.h"

#include "GameVersions.h"
#include <variables.h>

#include <cstdio> // std::sprintf
#include <mutex>

#include "soh/OTRAudio.h"
//#include "spdlog/spdlog.h"

//#include "global.h"
//#include <soh/OTRGlobals.h>


//TODO is there a better way?
unsigned int gCurrentSlot = 0;

static std::array<SaveStateInfo*, SaveState::SAVE_SLOT_MAX + 1> gSaveStates;



SaveState::SaveState(){}
SaveState::~SaveState() {}

void SaveState::Init(void) {
    gSaveStates[0] = nullptr;
    gSaveStates[1] = nullptr;
    gSaveStates[2] = nullptr;
}

void SaveState::SetHeader(SaveStateHeader& header) {
    header.stateMagic = 0x07151129;
    //TODO state version ENUM;
    header.stateVersion = 0;
}

SaveStateReturn SaveState::Delete(const unsigned int slot) {
    if (slot > SAVE_SLOT_MAX) {
        return SaveStateReturn::FAIL_INVALID_SLOT;
    }
    if (gSaveStates[slot] != nullptr) {
        delete gSaveStates[slot];
        gSaveStates[slot] = nullptr;
    }
    return SaveStateReturn::SUCCESS;
}

SaveStateReturn SaveState::Export(const unsigned int slot) {
    if (slot > SAVE_SLOT_MAX) {
        return SaveStateReturn::FAIL_INVALID_SLOT;
    }
    if (gSaveStates[slot] == nullptr) {
        return SaveStateReturn::FAIL_STATE_EMPTY;
    }

    char outFileName[20];
    std::sprintf(outFileName, "SOH_STATE_%u.state", gCurrentSlot);
    
    FILE* outFile = fopen(outFileName, "wb+");
    
    if (outFile == nullptr) {
        return SaveStateReturn::FAIL_FILE_NOT_OPENED;
    }

    SaveState::SetHeader(gSaveStates[slot]->stateHeader);
    std::fwrite(gSaveStates[slot], sizeof(SaveState), 1, outFile);
    std::fclose(outFile);
    
    return SaveStateReturn::SUCCESS;
}

SaveStateReturn SaveState::Import(const unsigned int slot) {
    if (slot > SAVE_SLOT_MAX) {
        return SaveStateReturn::FAIL_INVALID_SLOT;
    }
    
    char inFileName[20];
    std::sprintf(inFileName, "SOH_STATE_%u.state", gCurrentSlot);
    
    FILE* inFile = std::fopen(inFileName, "rb");
    
    if (inFile == nullptr) {
        return SaveStateReturn::FAIL_FILE_NOT_OPENED;
    }
    
    std::fseek(inFile, 0, SEEK_END);
    const size_t inFileSize = std::ftell(inFile);
    
    if (inFileSize != sizeof(SaveState)) {
        std::fclose(inFile);
        return SaveStateReturn::FAIL_INVALID_SIZE;
    }
    std::fseek(inFile, 0, SEEK_SET);
    
    SaveStateHeader tempHeader;
    std::fread(&tempHeader, sizeof(SaveStateHeader), 1, inFile);
    
    if (tempHeader.stateMagic != 0x07151129) {
        std::fclose(inFile);
        return SaveStateReturn::FAIL_INVALID_MAGIC;
    }

    if (gSaveStates[slot] == nullptr) {
        gSaveStates[slot] = new SaveStateInfo;
        if (gSaveStates[slot] == nullptr) {
            fclose(inFile);
            return SaveStateReturn::FAIL_NO_MEMORY;
        }
    }
    
    std::fseek(inFile, 0, SEEK_SET);
    std::fread(gSaveStates[slot], sizeof(SaveStateInfo), 1, inFile);
    std::fclose(inFile);
    //TODO version system
    //if (gSaveStates[slot]->stateHeader.stateVersion != 0) {
    //	return SaveStateReturn::FAIL_INVALID_STATE;
    //}
    
    return SaveStateReturn::SUCCESS;
    
}

void SaveState::BackupSeqScriptState(SaveStateInfo* state) {
    for (unsigned int i = 0; i < 4; i++) {
        state->seqScriptStateCopy[i].value = gAudioContext.seqPlayers[i].scriptState.value;
        
        state->seqScriptStateCopy[i].remLoopIters[0] = gAudioContext.seqPlayers[i].scriptState.remLoopIters[0];
        state->seqScriptStateCopy[i].remLoopIters[1] = gAudioContext.seqPlayers[i].scriptState.remLoopIters[1];
        state->seqScriptStateCopy[i].remLoopIters[2] = gAudioContext.seqPlayers[i].scriptState.remLoopIters[2];
        state->seqScriptStateCopy[i].remLoopIters[3] = gAudioContext.seqPlayers[i].scriptState.remLoopIters[3];
        
        state->seqScriptStateCopy[i].depth = gAudioContext.seqPlayers[i].scriptState.depth;
        
        state->seqScriptStateCopy[i].pc = (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.pc - (uintptr_t)gAudioHeap);
        
        state->seqScriptStateCopy[i].stack[0] =
            (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.stack[0] - (uintptr_t)gAudioHeap);
        state->seqScriptStateCopy[i].stack[1] =
            (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.stack[1] - (uintptr_t)gAudioHeap);
        state->seqScriptStateCopy[i].stack[2] =
            (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.stack[2] - (uintptr_t)gAudioHeap);
        state->seqScriptStateCopy[i].stack[3] =
            (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.stack[3] - (uintptr_t)gAudioHeap);
    }
}

void SaveState::LoadSeqScriptState(SaveStateInfo* state) {
    for (unsigned int i = 0; i < 4; i++) {
        gAudioContext.seqPlayers[i].scriptState.value = state->seqScriptStateCopy[i].value;

        gAudioContext.seqPlayers[i].scriptState.remLoopIters[0] = state->seqScriptStateCopy[i].remLoopIters[0];
        gAudioContext.seqPlayers[i].scriptState.remLoopIters[1] = state->seqScriptStateCopy[i].remLoopIters[1];
        gAudioContext.seqPlayers[i].scriptState.remLoopIters[2] = state->seqScriptStateCopy[i].remLoopIters[2];
        gAudioContext.seqPlayers[i].scriptState.remLoopIters[3] = state->seqScriptStateCopy[i].remLoopIters[3];

        gAudioContext.seqPlayers[i].scriptState.depth = state->seqScriptStateCopy[i].depth;

        gAudioContext.seqPlayers[i].scriptState.pc =
            (u8*)((uintptr_t)state->seqScriptStateCopy[i].pc + (uintptr_t)gAudioHeap);

        gAudioContext.seqPlayers[i].scriptState.stack[0] =
            (u8*)((uintptr_t)state->seqScriptStateCopy[i].stack[0] + (uintptr_t)gAudioHeap);
        gAudioContext.seqPlayers[i].scriptState.stack[1] =
            (u8*)((uintptr_t)state->seqScriptStateCopy[i].stack[1] + (uintptr_t)gAudioHeap);
        gAudioContext.seqPlayers[i].scriptState.stack[2] =
            (u8*)((uintptr_t)state->seqScriptStateCopy[i].stack[2] + (uintptr_t)gAudioHeap);
        gAudioContext.seqPlayers[i].scriptState.stack[3] =
            (u8*)((uintptr_t)state->seqScriptStateCopy[i].stack[3] + (uintptr_t)gAudioHeap);

    }
}

extern "C" MtxF* sMatrixStack;
extern "C" MtxF* sCurrentMatrix;
extern "C" LightsBuffer sLightsBuffer;
extern "C" s16 sWarpTimerTarget;

std::queue<SaveStateRequest> SaveState::requests;

extern "C" void ProcessSaveStateRequests(void) {
    SaveState::ProcessSaveStateRequestsImpl();
}

void SaveState::ProcessSaveStateRequestsImpl(void) {
    while (!requests.empty()) {
        if (requests.front().type == RequestType::SAVE) {
            SaveState::Save(requests.front().slot);
        } else if (requests.front().type == RequestType::LOAD) {
            SaveState::Load(requests.front().slot);
        } else {
            // SPDLOG_ERROR("Invalid Request type for SaveState: {}", requests.front().type);
        }
        requests.pop();
    }
}

SaveStateReturn SaveState::RequestSaveState(const unsigned int slot) {
    if (slot > SaveState::SAVE_SLOT_MAX) {
        return SaveStateReturn::FAIL_INVALID_SLOT;
    }
    if (gSaveStates[slot] == nullptr) {
        gSaveStates[slot] = new SaveStateInfo;
        if (gSaveStates[slot] == nullptr) {
            return SaveStateReturn::FAIL_NO_MEMORY;
        }
    }

    SaveState::requests.push({ slot, RequestType::SAVE });
    return SaveStateReturn::SUCCESS;
}

extern "C" void SaveState::Save(const unsigned int slot) {
    std::unique_lock<std::mutex> Lock(audio.mutex);


    SaveState::SetHeader(gSaveStates[slot]->stateHeader);
    
    memcpy(&gSaveStates[slot]->sysHeapCopy, gSystemHeap, SYSTEM_HEAP_SIZE /* sizeof(gSystemHeap) */);
    memcpy(&gSaveStates[slot]->audioHeapCopy, gAudioHeap, AUDIO_HEAP_SIZE /* sizeof(gAudioContext) */);

    memcpy(&gSaveStates[slot]->audioContextCopy, &gAudioContext, sizeof(AudioContext));
    memcpy(&gSaveStates[slot]->unk_D_8016E750Copy, D_8016E750, sizeof(gSaveStates[slot]->unk_D_8016E750Copy));
    BackupSeqScriptState(gSaveStates[slot]);

    memcpy(gSaveStates[slot]->gActiveSoundsCopy, gActiveSounds, sizeof(gActiveSounds));
    memcpy(&gSaveStates[slot]->gSoundBankMutedCopy, gSoundBankMuted, sizeof(gSaveStates[0]->gSoundBankMutedCopy));
    gSaveStates[slot]->D_801333F0_copy = D_801333F0;
    gSaveStates[slot]->gAudioSfxSwapOff_copy = gAudioSfxSwapOff;

    memcpy(&gSaveStates[slot]->gAudioSfxSwapSource_copy, gAudioSfxSwapSource,
           sizeof(gSaveStates[slot]->gAudioSfxSwapSource_copy));
    memcpy(&gSaveStates[slot]->gAudioSfxSwapTarget_copy, gAudioSfxSwapTarget,
           sizeof(gSaveStates[slot]->gAudioSfxSwapTarget_copy));
    memcpy(&gSaveStates[slot]->gAudioSfxSwapMode_copy, gAudioSfxSwapMode, 
        sizeof(&gSaveStates[slot]->gAudioSfxSwapMode_copy));

    gSaveStates[slot]->D_801755D0_copy = D_801755D0;


   // gSaveStates[slot]->seqIdCopy[0] = gAudioContext.seqPlayers[0].seqId;
   // gSaveStates[slot]->seqIdCopy[1] = gAudioContext.seqPlayers[1].seqId;
   // gSaveStates[slot]->seqIdCopy[2] = gAudioContext.seqPlayers[2].seqId;
   // gSaveStates[slot]->seqIdCopy[3] = gAudioContext.seqPlayers[3].seqId;


    memcpy(&gSaveStates[slot]->saveContextCopy, &gSaveContext, sizeof(gSaveContext));
    memcpy(&gSaveStates[slot]->gameInfoCopy, gGameInfo, sizeof(*gGameInfo));
    memcpy(&gSaveStates[slot]->lightBufferCopy, &sLightsBuffer, sizeof(sLightsBuffer));
    memcpy(&gSaveStates[slot]->mtxStackCopy, &sMatrixStack, sizeof(MtxF) * 20);
    memcpy(&gSaveStates[slot]->currentMtxCopy, &sCurrentMatrix, sizeof(MtxF));
    gSaveStates[slot]->blueWarpTimerCopy = sWarpTimerTarget;
    
}

SaveStateReturn SaveState::RequestLoadState(const unsigned int slot) {
    if (slot > SaveState::SAVE_SLOT_MAX) {
        return SaveStateReturn::FAIL_INVALID_SLOT;
    }
    if (gSaveStates[slot] == nullptr) {
        return SaveStateReturn::FAIL_STATE_EMPTY;
    }

    SaveState::requests.push({ slot, RequestType::LOAD });
    return SaveStateReturn::SUCCESS;
}

void SaveState::Load(const unsigned int slot) {
    
    std::unique_lock<std::mutex> Lock(audio.mutex);



    //gSaveStates[slot]->stateHeader.gameVersion = 0;
    //gSaveStates[slot]->stateHeader.stateVersion = 0;

    memcpy(gSystemHeap, &gSaveStates[slot]->sysHeapCopy, SYSTEM_HEAP_SIZE);
    memcpy(gAudioHeap, &gSaveStates[slot]->audioHeapCopy, AUDIO_HEAP_SIZE);

    memcpy(&gAudioContext, &gSaveStates[slot]->audioContextCopy, sizeof(AudioContext));
    memcpy(D_8016E750, &gSaveStates[slot]->unk_D_8016E750Copy, sizeof(gSaveStates[slot]->unk_D_8016E750Copy));
    LoadSeqScriptState(gSaveStates[slot]);
    //gAudioContext.seqPlayers[0].seqId = gSaveStates[slot]->seqIdCopy[0];
    //gAudioContext.seqPlayers[1].seqId = gSaveStates[slot]->seqIdCopy[1];
    //gAudioContext.seqPlayers[2].seqId = gSaveStates[slot]->seqIdCopy[2];
    //gAudioContext.seqPlayers[3].seqId = gSaveStates[slot]->seqIdCopy[3];

    memcpy(&gSaveContext, &gSaveStates[slot]->saveContextCopy, sizeof(gSaveContext));
    memcpy(gGameInfo, &gSaveStates[slot]->gameInfoCopy, sizeof(*gGameInfo));
    memcpy(&sLightsBuffer, &gSaveStates[slot]->lightBufferCopy, sizeof(sLightsBuffer));
    memcpy(&sMatrixStack, &gSaveStates[slot]->mtxStackCopy, sizeof(MtxF) * 20);
    memcpy(&sCurrentMatrix, &gSaveStates[slot]->currentMtxCopy, sizeof(MtxF));
    sWarpTimerTarget = gSaveStates[slot]->blueWarpTimerCopy;
    //TODO RNG seed

    memcpy(gActiveSounds, gSaveStates[slot]->gActiveSoundsCopy, sizeof(gActiveSounds));
    memcpy(gSoundBankMuted, &gSaveStates[slot]->gSoundBankMutedCopy, sizeof(gSaveStates[0]->gSoundBankMutedCopy));
    D_801333F0 = gSaveStates[slot]->D_801333F0_copy;
    gAudioSfxSwapOff = gSaveStates[slot]->gAudioSfxSwapOff_copy;

    memcpy(gAudioSfxSwapSource, &gSaveStates[slot]->gAudioSfxSwapSource_copy,
           sizeof(&gSaveStates[slot]->gAudioSfxSwapSource_copy));
    memcpy(gAudioSfxSwapTarget, &gSaveStates[slot]->gAudioSfxSwapTarget_copy,
           sizeof(&gSaveStates[slot]->gAudioSfxSwapTarget_copy));
    memcpy(gAudioSfxSwapMode, &gSaveStates[slot]->gAudioSfxSwapMode_copy,
           sizeof(&gSaveStates[slot]->gAudioSfxSwapMode_copy));

    D_801755D0 = gSaveStates[slot]->D_801755D0_copy;

}
