#include "savestates.h"

#include "GameVersions.h"
#include <variables.h>

#include <cstdio> // std::sprintf


//#include "global.h"
//#include <soh/OTRGlobals.h>

//TODO is there a better way?
unsigned int gCurrentSlot = 0;

static std::array<SaveState*, SaveState::SAVE_SLOT_MAX + 1> gSaveStates;

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
        gSaveStates[slot] = new SaveState;
        if (gSaveStates[slot] == nullptr) {
            fclose(inFile);
            return SaveStateReturn::FAIL_NO_MEMORY;
        }
    }
    
    std::fseek(inFile, 0, SEEK_SET);
    std::fread(gSaveStates[slot], sizeof(SaveState), 1, inFile);
    std::fclose(inFile);
    //TODO version system
    //if (gSaveStates[slot]->stateHeader.stateVersion != 0) {
    //	return SaveStateReturn::FAIL_INVALID_STATE;
    //}
    
    return SaveStateReturn::SUCCESS;
    
}

extern "C" MtxF* sMatrixStack;
extern "C" MtxF* sCurrentMatrix;
extern "C" LightsBuffer sLightsBuffer;
extern "C" s16 sWarpTimerTarget;

extern "C" SaveStateReturn SaveState::Save(const unsigned int slot) {
    if (slot > SaveState::SAVE_SLOT_MAX) {
        return SaveStateReturn::FAIL_INVALID_SLOT;
    }
    if (gSaveStates[slot] == nullptr) {
        gSaveStates[slot] = new SaveState;
        if (gSaveStates[slot] == nullptr) {
            return SaveStateReturn::FAIL_NO_MEMORY;
        }
    }

    SaveState::SetHeader(gSaveStates[slot]->stateHeader);
    
    memcpy(&gSaveStates[slot]->sysHeapCopy, &gSystemHeap, 1024 * 1024 * 4 /* sizeof(gSystemHeap) */);
    memcpy(&gSaveStates[slot]->saveContextCopy, &gSaveContext, sizeof(gSaveContext));
    memcpy(&gSaveStates[slot]->gameInfoCopy, gGameInfo, sizeof(*gGameInfo));
   // memcpy(&gSaveStates[slot]->audioContextCopy, &gAudioContext, sizeof(AudioContext));
    memcpy(&gSaveStates[slot]->lightBufferCopy, &sLightsBuffer, sizeof(sLightsBuffer));
    memcpy(&gSaveStates[slot]->mtxStackCopy, &sMatrixStack, sizeof(MtxF) * 20);
    memcpy(&gSaveStates[slot]->currentMtxCopy, &sCurrentMatrix, sizeof(MtxF));
    gSaveStates[slot]->blueWarpTimerCopy = sWarpTimerTarget;
    //TODO RNG seed

    return SaveStateReturn::SUCCESS;
    
}

SaveStateReturn SaveState::Load(const unsigned int slot) {
    if (slot > SaveState::SAVE_SLOT_MAX) {
        return SaveStateReturn::FAIL_INVALID_SLOT;
    }
    if (gSaveStates[slot] == nullptr) {
        return SaveStateReturn::FAIL_STATE_EMPTY;
    }

    //gSaveStates[slot]->stateHeader.gameVersion = 0;
    //gSaveStates[slot]->stateHeader.stateVersion = 0;

    memcpy(&gSystemHeap, &gSaveStates[slot]->sysHeapCopy, 1024 * 1024 * 4);
    memcpy(&gSaveContext, &gSaveStates[slot]->saveContextCopy, sizeof(gSaveContext));
    memcpy(gGameInfo, &gSaveStates[slot]->gameInfoCopy, sizeof(*gGameInfo));
    //memcpy(&gAudioContext, &gSaveStates[slot]->audioContextCopy, sizeof(AudioContext));
    memcpy(&sLightsBuffer, &gSaveStates[slot]->lightBufferCopy, sizeof(sLightsBuffer));
    memcpy(&sMatrixStack, &gSaveStates[slot]->mtxStackCopy, sizeof(MtxF) * 20);
    memcpy(&sCurrentMatrix, &gSaveStates[slot]->currentMtxCopy, sizeof(MtxF));
    sWarpTimerTarget = gSaveStates[slot]->blueWarpTimerCopy;
    //TODO RNG seed


    return SaveStateReturn::SUCCESS;
}
