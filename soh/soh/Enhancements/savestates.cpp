#include "savestates.h"

#include <memory>
#include <spdlog/spdlog.h>

#include <ship/Context.h>
#include <ship/window/Window.h>
#include <ship/window/gui/GameOverlay.h>
#include <soh/OTRGlobals.h>
#include <soh/OTRAudio.h>

#include "z64.h"
#include "z64save.h"
#include <variables.h>
#include <functions.h>
#include "savestate_serialize.h"

extern "C" PlayState* gPlayState;

extern "C" void BgDdanKd_SaveState(SaveStateCtx* ctx);
extern "C" void BgDodoago_SaveState(SaveStateCtx* ctx);
extern "C" void BgHakaTrap_SaveState(SaveStateCtx* ctx);
extern "C" void BgHidanRock_SaveState(SaveStateCtx* ctx);
extern "C" void BgMenkuriEye_SaveState(SaveStateCtx* ctx);
extern "C" void BgMoriHineri_SaveState(SaveStateCtx* ctx);
extern "C" void BgPoEvent_SaveState(SaveStateCtx* ctx);
extern "C" void BgRelayObjects_SaveState(SaveStateCtx* ctx);
extern "C" void BgSpot18Basket_SaveState(SaveStateCtx* ctx);
extern "C" void BossGanon_SaveState(SaveStateCtx* ctx);
extern "C" void BossGanon2_SaveState(SaveStateCtx* ctx);
extern "C" void BossMo_SaveState(SaveStateCtx* ctx);
extern "C" void BossSst_SaveState(SaveStateCtx* ctx);
extern "C" void BossTw_SaveState(SaveStateCtx* ctx);
extern "C" void BossVa_SaveState(SaveStateCtx* ctx);
extern "C" void Demo6k_SaveState(SaveStateCtx* ctx);
extern "C" void DemoDu_SaveState(SaveStateCtx* ctx);
extern "C" void DemoKekkai_SaveState(SaveStateCtx* ctx);
extern "C" void DoorWarp1_SaveState(SaveStateCtx* ctx);
extern "C" void EnBw_SaveState(SaveStateCtx* ctx);
extern "C" void EnClearTag_SaveState(SaveStateCtx* ctx);
extern "C" void EnFr_SaveState(SaveStateCtx* ctx);
extern "C" void EnGoma_SaveState(SaveStateCtx* ctx);
extern "C" void EnInsect_SaveState(SaveStateCtx* ctx);
extern "C" void EnIshi_SaveState(SaveStateCtx* ctx);
extern "C" void EnNiw_SaveState(SaveStateCtx* ctx);
extern "C" void EnPoField_SaveState(SaveStateCtx* ctx);
extern "C" void EnTakaraMan_SaveState(SaveStateCtx* ctx);
extern "C" void EnXc_SaveState(SaveStateCtx* ctx);
extern "C" void EnZf_SaveState(SaveStateCtx* ctx);
extern "C" void EnZl3_SaveState(SaveStateCtx* ctx);
extern "C" void ObjectKankyo_SaveState(SaveStateCtx* ctx);
extern "C" void EnHeishi1_SaveState(SaveStateCtx* ctx);

extern "C" void Matrix_SaveState(SaveStateCtx* ctx);
extern "C" void Lights_SaveState(SaveStateCtx* ctx);
extern "C" void MapMark_SaveState(SaveStateCtx* ctx);
extern "C" void Camera_SaveState(SaveStateCtx* ctx);
extern "C" void OnePointCutscene_SaveState(SaveStateCtx* ctx);
extern "C" void Environment_SaveState(SaveStateCtx* ctx);
extern "C" void MapExp_SaveState(SaveStateCtx* ctx);
extern "C" void AudioOca_SaveState(SaveStateCtx* ctx);
extern "C" void MessagePAL_SaveState(SaveStateCtx* ctx);

static void SaveOverlayState(std::unique_ptr<uint8_t[]>& buf, void (*fn)(SaveStateCtx*)) {
    SaveStateCtx ctx = {};
    ctx.mode = SHIP_SAVESTATE_MEASURE;
    fn(&ctx);
    buf = std::make_unique<uint8_t[]>(ctx.offset);
    ctx.mode = SHIP_SAVESTATE_SAVE;
    ctx.buffer = buf.get();
    ctx.offset = 0;
    fn(&ctx);
}

static void LoadOverlayState(std::unique_ptr<uint8_t[]>& buf, void (*fn)(SaveStateCtx*)) {
    SaveStateCtx ctx = {};
    ctx.mode = SHIP_SAVESTATE_LOAD;
    ctx.buffer = buf.get();
    ctx.offset = 0;
    fn(&ctx);
}

typedef struct SaveStateInfo {
    unsigned char sysHeapCopy[SYSTEM_HEAP_SIZE];
    unsigned char audioHeapCopy[AUDIO_HEAP_SIZE];

    SaveContext saveContextCopy;
    GameInfo gameInfoCopy;
    AudioContext audioContextCopy;
    uint32_t rngSeed;

    SeqScriptState seqScriptStateCopy[4]; // Unrelocated
    ActiveSequence gActiveSeqsCopy[4];

    ActiveSound gActiveSoundsCopy[7][MAX_CHANNELS_PER_BANK];
    uint8_t gSoundBankMutedCopy[7];

    u8 D_801333F0_copy;
    u8 gAudioSfxSwapOff_copy;
    uint16_t gAudioSfxSwapSource_copy[10];
    uint16_t gAudioSfxSwapTarget_copy[10];
    uint8_t gAudioSfxSwapMode_copy[10];
    void (*D_801755D0_copy)(void);

    // Static data (per-translation-unit, serialized via SHIP_SAVESTATE_DEFINE)
    std::unique_ptr<uint8_t[]> matrixState;
    std::unique_ptr<uint8_t[]> lightsState;
    std::unique_ptr<uint8_t[]> doorWarp1State;
    std::unique_ptr<uint8_t[]> mapMarkState;
    std::unique_ptr<uint8_t[]> cameraState;
    std::unique_ptr<uint8_t[]> onePointCutsceneState;
    std::unique_ptr<uint8_t[]> environmentState;
    std::unique_ptr<uint8_t[]> mapExpState;
    std::unique_ptr<uint8_t[]> audioOcaState;
    std::unique_ptr<uint8_t[]> messagePalState;

    // Overlay static data
    std::unique_ptr<uint8_t[]> bgDdanKdState;
    std::unique_ptr<uint8_t[]> bgDodoagoState;
    std::unique_ptr<uint8_t[]> bgHakaTrapState;
    std::unique_ptr<uint8_t[]> bgHidanRockState;
    std::unique_ptr<uint8_t[]> bgMenkuriEyeState;
    std::unique_ptr<uint8_t[]> bgMoriHineriState;
    std::unique_ptr<uint8_t[]> bgPoEventState;
    std::unique_ptr<uint8_t[]> bgRelayObjectsState;
    std::unique_ptr<uint8_t[]> bgSpot18BasketState;
    std::unique_ptr<uint8_t[]> bossGanonState;
    std::unique_ptr<uint8_t[]> bossGanon2State;
    std::unique_ptr<uint8_t[]> bossMoState;
    std::unique_ptr<uint8_t[]> bossSstState;
    std::unique_ptr<uint8_t[]> bossTwState;
    std::unique_ptr<uint8_t[]> bossVaState;
    std::unique_ptr<uint8_t[]> demo6kState;
    std::unique_ptr<uint8_t[]> demoDuState;
    std::unique_ptr<uint8_t[]> demoKekkaiState;
    std::unique_ptr<uint8_t[]> enBwState;
    std::unique_ptr<uint8_t[]> enClearTagState;
    std::unique_ptr<uint8_t[]> enFrState;
    std::unique_ptr<uint8_t[]> enGomaState;
    std::unique_ptr<uint8_t[]> enInsectState;
    std::unique_ptr<uint8_t[]> enIshiState;
    std::unique_ptr<uint8_t[]> enNiwState;
    std::unique_ptr<uint8_t[]> enPoFieldState;
    std::unique_ptr<uint8_t[]> enTakaraManState;
    std::unique_ptr<uint8_t[]> enXcState;
    std::unique_ptr<uint8_t[]> enZfState;
    std::unique_ptr<uint8_t[]> enZl3State;
    std::unique_ptr<uint8_t[]> objectKankyoState;
    std::unique_ptr<uint8_t[]> enHeishi1State;

    u8 transitionActorCount_copy;
    s16 transitionActorIds_copy[256];

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
    void SaveOverlayStaticData(void);
    void LoadOverlayStaticData(void);
    void SaveTransitionActors(void);
    void LoadTransitionActors(void);

    SaveStateInfo* GetSaveStateInfo(void);
};

SaveStateMgr::SaveStateMgr() {
    this->SetCurrentSlot(0);
}
SaveStateMgr::~SaveStateMgr() {
    this->states.clear();
}

SaveState::SaveState(std::shared_ptr<SaveStateMgr> mgr, unsigned int slot)
    : slot(slot), saveStateMgr(mgr), info(nullptr) {
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

        info->seqScriptStateCopy[i].pc =
            (u8*)((uintptr_t)gAudioContext.seqPlayers[i].scriptState.pc - (uintptr_t)gAudioHeap);

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

void SaveState::SaveOverlayStaticData(void) {
    SaveOverlayState(info->matrixState, Matrix_SaveState);
    SaveOverlayState(info->lightsState, Lights_SaveState);
    SaveOverlayState(info->doorWarp1State, DoorWarp1_SaveState);
    SaveOverlayState(info->mapMarkState, MapMark_SaveState);
    SaveOverlayState(info->cameraState, Camera_SaveState);
    SaveOverlayState(info->onePointCutsceneState, OnePointCutscene_SaveState);
    SaveOverlayState(info->environmentState, Environment_SaveState);
    SaveOverlayState(info->mapExpState, MapExp_SaveState);
    SaveOverlayState(info->audioOcaState, AudioOca_SaveState);
    SaveOverlayState(info->messagePalState, MessagePAL_SaveState);
    SaveOverlayState(info->bgDdanKdState, BgDdanKd_SaveState);
    SaveOverlayState(info->bgDodoagoState, BgDodoago_SaveState);
    SaveOverlayState(info->bgHakaTrapState, BgHakaTrap_SaveState);
    SaveOverlayState(info->bgHidanRockState, BgHidanRock_SaveState);
    SaveOverlayState(info->bgMenkuriEyeState, BgMenkuriEye_SaveState);
    SaveOverlayState(info->bgMoriHineriState, BgMoriHineri_SaveState);
    SaveOverlayState(info->bgPoEventState, BgPoEvent_SaveState);
    SaveOverlayState(info->bgRelayObjectsState, BgRelayObjects_SaveState);
    SaveOverlayState(info->bgSpot18BasketState, BgSpot18Basket_SaveState);
    SaveOverlayState(info->bossGanonState, BossGanon_SaveState);
    SaveOverlayState(info->bossGanon2State, BossGanon2_SaveState);
    SaveOverlayState(info->bossMoState, BossMo_SaveState);
    SaveOverlayState(info->bossSstState, BossSst_SaveState);
    SaveOverlayState(info->bossTwState, BossTw_SaveState);
    SaveOverlayState(info->bossVaState, BossVa_SaveState);
    SaveOverlayState(info->demo6kState, Demo6k_SaveState);
    SaveOverlayState(info->demoDuState, DemoDu_SaveState);
    SaveOverlayState(info->demoKekkaiState, DemoKekkai_SaveState);
    SaveOverlayState(info->enBwState, EnBw_SaveState);
    SaveOverlayState(info->enClearTagState, EnClearTag_SaveState);
    SaveOverlayState(info->enFrState, EnFr_SaveState);
    SaveOverlayState(info->enGomaState, EnGoma_SaveState);
    SaveOverlayState(info->enInsectState, EnInsect_SaveState);
    SaveOverlayState(info->enIshiState, EnIshi_SaveState);
    SaveOverlayState(info->enNiwState, EnNiw_SaveState);
    SaveOverlayState(info->enPoFieldState, EnPoField_SaveState);
    SaveOverlayState(info->enTakaraManState, EnTakaraMan_SaveState);
    SaveOverlayState(info->enXcState, EnXc_SaveState);
    SaveOverlayState(info->enZfState, EnZf_SaveState);
    SaveOverlayState(info->enZl3State, EnZl3_SaveState);
    SaveOverlayState(info->objectKankyoState, ObjectKankyo_SaveState);
    SaveOverlayState(info->enHeishi1State, EnHeishi1_SaveState);
}

void SaveState::LoadOverlayStaticData(void) {
    LoadOverlayState(info->matrixState, Matrix_SaveState);
    LoadOverlayState(info->lightsState, Lights_SaveState);
    LoadOverlayState(info->doorWarp1State, DoorWarp1_SaveState);
    LoadOverlayState(info->mapMarkState, MapMark_SaveState);
    LoadOverlayState(info->cameraState, Camera_SaveState);
    LoadOverlayState(info->onePointCutsceneState, OnePointCutscene_SaveState);
    LoadOverlayState(info->environmentState, Environment_SaveState);
    LoadOverlayState(info->mapExpState, MapExp_SaveState);
    LoadOverlayState(info->audioOcaState, AudioOca_SaveState);
    LoadOverlayState(info->messagePalState, MessagePAL_SaveState);
    LoadOverlayState(info->bgDdanKdState, BgDdanKd_SaveState);
    LoadOverlayState(info->bgDodoagoState, BgDodoago_SaveState);
    LoadOverlayState(info->bgHakaTrapState, BgHakaTrap_SaveState);
    LoadOverlayState(info->bgHidanRockState, BgHidanRock_SaveState);
    LoadOverlayState(info->bgMenkuriEyeState, BgMenkuriEye_SaveState);
    LoadOverlayState(info->bgMoriHineriState, BgMoriHineri_SaveState);
    LoadOverlayState(info->bgPoEventState, BgPoEvent_SaveState);
    LoadOverlayState(info->bgRelayObjectsState, BgRelayObjects_SaveState);
    LoadOverlayState(info->bgSpot18BasketState, BgSpot18Basket_SaveState);
    LoadOverlayState(info->bossGanonState, BossGanon_SaveState);
    LoadOverlayState(info->bossGanon2State, BossGanon2_SaveState);
    LoadOverlayState(info->bossMoState, BossMo_SaveState);
    LoadOverlayState(info->bossSstState, BossSst_SaveState);
    LoadOverlayState(info->bossTwState, BossTw_SaveState);
    LoadOverlayState(info->bossVaState, BossVa_SaveState);
    LoadOverlayState(info->demo6kState, Demo6k_SaveState);
    LoadOverlayState(info->demoDuState, DemoDu_SaveState);
    LoadOverlayState(info->demoKekkaiState, DemoKekkai_SaveState);
    LoadOverlayState(info->enBwState, EnBw_SaveState);
    LoadOverlayState(info->enClearTagState, EnClearTag_SaveState);
    LoadOverlayState(info->enFrState, EnFr_SaveState);
    LoadOverlayState(info->enGomaState, EnGoma_SaveState);
    LoadOverlayState(info->enInsectState, EnInsect_SaveState);
    LoadOverlayState(info->enIshiState, EnIshi_SaveState);
    LoadOverlayState(info->enNiwState, EnNiw_SaveState);
    LoadOverlayState(info->enPoFieldState, EnPoField_SaveState);
    LoadOverlayState(info->enTakaraManState, EnTakaraMan_SaveState);
    LoadOverlayState(info->enXcState, EnXc_SaveState);
    LoadOverlayState(info->enZfState, EnZf_SaveState);
    LoadOverlayState(info->enZl3State, EnZl3_SaveState);
    LoadOverlayState(info->objectKankyoState, ObjectKankyo_SaveState);
    LoadOverlayState(info->enHeishi1State, EnHeishi1_SaveState);
}

void SaveState::SaveTransitionActors(void) {
    info->transitionActorCount_copy = gPlayState->transiActorCtx.numActors;
    for (u32 i = 0; i < info->transitionActorCount_copy; i++) {
        info->transitionActorIds_copy[i] = gPlayState->transiActorCtx.list[i].id;
    }
}

void SaveState::LoadTransitionActors(void) {
    u32 numActors = MIN(info->transitionActorCount_copy, gPlayState->transiActorCtx.numActors);
    for (u32 i = 0; i < numActors; i++) {
        gPlayState->transiActorCtx.list[i].id = info->transitionActorIds_copy[i];
    }
}

extern "C" void ProcessSaveStateRequests(void) {
    OTRGlobals::Instance->gSaveStateMgr->ProcessSaveStateRequests();
}

void SaveStateMgr::SetCurrentSlot(unsigned int slot) {
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(1.0f, true,
                                                                                                   "slot %u set", slot);
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
                    this->states[request.slot] =
                        std::make_shared<SaveState>(OTRGlobals::Instance->gSaveStateMgr, request.slot);
                }
                this->states[request.slot]->Save();
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(
                    1.0f, true, "saved state %u", request.slot);
                break;
            case RequestType::LOAD:
                if (this->states.contains(request.slot)) {
                    this->states[request.slot]->Load();
                    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(
                        1.0f, true, "loaded state %u", request.slot);
                } else {
                    SPDLOG_ERROR("Invalid SaveState slot: {}", request.slot);
                }
                break;
                [[unlikely]] default
                    : SPDLOG_ERROR("Invalid SaveState request type: Unknown ({})", static_cast<int>(request.type));
                break;
        }
        this->requests.pop();
    }
}

SaveStateReturn SaveStateMgr::AddRequest(const SaveStateRequest request) {
    if (gPlayState == nullptr) {
        SPDLOG_ERROR("[SOH] Can not save or load a state outside of \"GamePlay\"");
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(
            1.0f, true, "states not available here", request.slot);
        return SaveStateReturn::FAIL_WRONG_GAMESTATE;
    }

    switch (request.type) {
        case RequestType::SAVE:
            requests.push(request);
            return SaveStateReturn::SUCCESS;
        case RequestType::LOAD:
            if (states.contains(request.slot)) {
                requests.push(request);
                return SaveStateReturn::SUCCESS;
            } else {
                SPDLOG_ERROR("Invalid SaveState slot: {}", request.slot);
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(
                    1.0f, true, "state slot %u empty", request.slot);
                return SaveStateReturn::FAIL_INVALID_SLOT;
            }
            [[unlikely]] default
                : SPDLOG_ERROR("Invalid SaveState request type: Unknown ({})", static_cast<int>(request.type));
            return SaveStateReturn::FAIL_BAD_REQUEST;
    }
}

void SaveState::Save(void) {
    std::unique_lock<std::mutex> Lock(audio.mutex);
    memcpy(&info->sysHeapCopy, gSystemHeap, SYSTEM_HEAP_SIZE /* sizeof(gSystemHeap) */);
    memcpy(&info->audioHeapCopy, gAudioHeap, AUDIO_HEAP_SIZE /* sizeof(gAudioContext) */);

    memcpy(&info->audioContextCopy, &gAudioContext, sizeof(AudioContext));
    memcpy(&info->gActiveSeqsCopy, gActiveSeqs, sizeof(info->gActiveSeqsCopy));
    BackupSeqScriptState();

    memcpy(info->gActiveSoundsCopy, gActiveSounds, sizeof(gActiveSounds));
    memcpy(&info->gSoundBankMutedCopy, gSoundBankMuted, sizeof(info->gSoundBankMutedCopy));

    info->D_801333F0_copy = D_801333F0;
    info->gAudioSfxSwapOff_copy = gAudioSfxSwapOff;

    memcpy(&info->gAudioSfxSwapSource_copy, gAudioSfxSwapSource, sizeof(info->gAudioSfxSwapSource_copy));
    memcpy(&info->gAudioSfxSwapTarget_copy, gAudioSfxSwapTarget, sizeof(info->gAudioSfxSwapTarget_copy));
    memcpy(&info->gAudioSfxSwapMode_copy, gAudioSfxSwapMode, sizeof(info->gAudioSfxSwapMode_copy));

    info->D_801755D0_copy = D_801755D0;

    memcpy(&info->saveContextCopy, &gSaveContext, sizeof(gSaveContext));
    memcpy(&info->gameInfoCopy, gGameInfo, sizeof(*gGameInfo));

    // Various static data
    SaveOverlayStaticData();
    SaveTransitionActors();
}

void SaveState::Load(void) {
    std::unique_lock<std::mutex> Lock(audio.mutex);
    memcpy(gSystemHeap, &info->sysHeapCopy, SYSTEM_HEAP_SIZE);
    memcpy(gAudioHeap, &info->audioHeapCopy, AUDIO_HEAP_SIZE);

    memcpy(&gAudioContext, &info->audioContextCopy, sizeof(AudioContext));
    memcpy(gActiveSeqs, &info->gActiveSeqsCopy, sizeof(info->gActiveSeqsCopy));
    LoadSeqScriptState();

    memcpy(&gSaveContext, &info->saveContextCopy, sizeof(gSaveContext));
    memcpy(gGameInfo, &info->gameInfoCopy, sizeof(*gGameInfo));

    memcpy(gActiveSounds, info->gActiveSoundsCopy, sizeof(gActiveSounds));
    memcpy(gSoundBankMuted, &info->gSoundBankMutedCopy, sizeof(info->gSoundBankMutedCopy));
    D_801333F0 = info->D_801333F0_copy;
    gAudioSfxSwapOff = info->gAudioSfxSwapOff_copy;

    memcpy(gAudioSfxSwapSource, &info->gAudioSfxSwapSource_copy, sizeof(info->gAudioSfxSwapSource_copy));
    memcpy(gAudioSfxSwapTarget, &info->gAudioSfxSwapTarget_copy, sizeof(info->gAudioSfxSwapTarget_copy));
    memcpy(gAudioSfxSwapMode, &info->gAudioSfxSwapMode_copy, sizeof(info->gAudioSfxSwapMode_copy));

    // Various static data
    D_801755D0 = info->D_801755D0_copy;
    LoadOverlayStaticData();
    LoadTransitionActors();
}