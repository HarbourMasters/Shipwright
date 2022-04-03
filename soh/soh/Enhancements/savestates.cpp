#include "savestates.h"

#include "GameVersions.h"
#include <variables.h>
//#include "global.h"
//#include <soh/OTRGlobals.h>

std::array<SaveState*, 3> gSaveStates;


SaveState::SaveState() {
    
}
#if 0

SaveState::~SaveState() {
}
#endif

void SaveState::Init(void) {
    gSaveStates[0] = nullptr;
    gSaveStates[1] = nullptr;
    gSaveStates[2] = nullptr;
}

void SaveState::WriteHeader(SaveStateHeader& header) {
    //OTRGlobals::Instance->context->GetResourceManager()->GetGameVersion();
    //    header.gameVersion = ResourceMgr_GetGameVersion();
}

extern "C" MtxF* sMatrixStack; // "Matrix_stack"
extern "C" MtxF* sCurrentMatrix;      // "Matrix_now"
extern "C" LightsBuffer sLightsBuffer;

extern "C" SaveStateReturn SaveState::Save(unsigned int slot) {
    if (slot > 2) {
        return SaveStateReturn::FAIL_INVALID_SLOT;
    }
    if (gSaveStates[slot] == nullptr) {
        gSaveStates[slot] = new SaveState;
        if (gSaveStates[slot] == nullptr) {
            return SaveStateReturn::FAIL_NO_MEMORY;
        }
    }

    gSaveStates[slot]->stateHeader.gameVersion = 0;
    gSaveStates[slot]->stateHeader.stateVersion = 0;

    memcpy(&gSaveStates[slot]->sysHeapCopy, &gSystemHeap, 1024 * 1024 * 4 /* sizeof(gSystemHeap) */);
    memcpy(&gSaveStates[slot]->saveContextCopy, &gSaveContext, sizeof(gSaveContext));
    memcpy(&gSaveStates[slot]->gameInfoCopy, gGameInfo, sizeof(*gGameInfo));
   // memcpy(&gSaveStates[slot]->audioContextCopy, &gAudioContext, sizeof(AudioContext));
    memcpy(&gSaveStates[slot]->lightBufferCopy, &sLightsBuffer, sizeof(sLightsBuffer));
    memcpy(&gSaveStates[slot]->mtxStackCopy, &sMatrixStack, sizeof(MtxF) * 20);
    memcpy(&gSaveStates[slot]->currentMtxCopy, &sCurrentMatrix, sizeof(MtxF));
    //TODO RNG seed

    return SaveStateReturn::SUCCESS;
    
}

SaveStateReturn SaveState::Load(unsigned int slot) {
    if (slot > 2) {
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
    //TODO RNG seed


    return SaveStateReturn::SUCCESS;
}
