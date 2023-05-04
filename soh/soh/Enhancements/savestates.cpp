#include "savestates.h"

#include <GameVersions.h>

#include <cstdio> // std::sprintf

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

#include <soh/OTRGlobals.h>
#include <soh/OTRAudio.h>

#include <ImGuiImpl.h>

#include "z64.h"
#include "z64save.h"
#include <variables.h>
#include <functions.h>
#include "z64map_mark.h"
#include "../../src/overlays/actors/ovl_Boss_Ganon/z_boss_ganon.h"
#include "../../src/overlays/actors/ovl_Boss_Ganon2/z_boss_ganon2.h"
#include "../../src/overlays/actors/ovl_Boss_Tw/z_boss_tw.h"
#include "../../src/overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "../../src/overlays/actors/ovl_En_Fr/z_en_fr.h"

extern "C" PlayState* gPlayState;

template <> struct fmt::formatter<RequestType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const RequestType& type, FormatContext& ctx) {
        switch (type) {
            case RequestType::SAVE: return fmt::format_to(ctx.out(), "Save");
            case RequestType::LOAD: return fmt::format_to(ctx.out(), "Load");
            default: return fmt::format_to(ctx.out(), "Unknown");
        }
    }
};

// FROM z_lights.c
// I didn't feel like moving it into a header file.
#define LIGHTS_BUFFER_SIZE 32

typedef struct {
    /* 0x000 */ s32 numOccupied;
    /* 0x004 */ s32 searchIndex;
    /* 0x008 */ LightNode buf[LIGHTS_BUFFER_SIZE];
} LightsBuffer; // size = 0x188

#include "savestates_extern.inc"

typedef struct SaveStateInfo {
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
    int16_t depthPhase_copy;
    int16_t screenPlanePhase_copy;
    int32_t sOOBTimer_copy;
    f32 D_8015CE50_copy;
    f32 D_8015CE54_copy;
    CamColChk D_8015CE58_copy;

    //Gameover
    uint16_t gGameOverTimer_copy;

    //One point demo
    uint32_t sPrevFrameCs1100_copy;
    CutsceneCameraPoint D_8012013C_copy[14];
    CutsceneCameraPoint D_8012021C_copy[14];
    CutsceneCameraPoint D_801204D4_copy[14];
    CutsceneCameraPoint D_801205B4_copy[14];
    OnePointCsFull D_801208EC_copy[3];
    OnePointCsFull D_80120964_copy[2];
    OnePointCsFull D_801209B4_copy[4];
    OnePointCsFull D_80120ACC_copy[5];
    OnePointCsFull D_80120B94_copy[11];
    OnePointCsFull D_80120D4C_copy[7];
    OnePointCsFull D_80120FA4_copy[6];
    OnePointCsFull D_80121184_copy[2];
    OnePointCsFull D_801211D4_copy[2];
    OnePointCsFull D_8012133C_copy[3];
    OnePointCsFull D_801213B4_copy[5];
    OnePointCsFull D_8012151C_copy[2];
    OnePointCsFull D_8012156C_copy[2];
    OnePointCsFull D_801215BC_copy[1];
    OnePointCsFull D_80121C24_copy[7];
    OnePointCsFull D_80121D3C_copy[3];
    OnePointCsFull D_80121F1C_copy[4];
    OnePointCsFull D_80121FBC_copy[4];
    OnePointCsFull D_801220D4_copy[5];
    OnePointCsFull D_80122714_copy[4];
    OnePointCsFull D_80122CB4_copy[2];
    OnePointCsFull D_80122D04_copy[2];
    OnePointCsFull D_80122E44_copy[2][7];
    OnePointCsFull D_8012313C_copy[3];
    OnePointCsFull D_801231B4_copy[4];
    OnePointCsFull D_80123254_copy[2];
    OnePointCsFull D_801232A4_copy[1];
    OnePointCsFull D_80123894_copy[3];
    OnePointCsFull D_8012390C_copy[2];
    OnePointCsFull D_8012395C_copy[3];
    OnePointCsFull D_801239D4_copy[3];

    uint16_t gTimeIncrement_copy;

    //Overlay static data
    //  z_bg_ddan_kd
    Vec3f sBgDdanKdVelocity_copy;
    Vec3f sBgDdanKdAccel_copy;

    // z_bg_dodoago
    s16 sBgDodoagoFirstExplosiveFlag_copy;
    u8 sBgDodoagoDisableBombCatcher_copy;
    s32 sBgDodoagoTimer_copy;

    // z_bg_haka_trap
    uint32_t D_80880F30_copy;
    uint32_t D_80881014_copy;

    // z_bg_hidan_rock
    float D_8088BFC0_copy;

    // z_bg_menkuri_eye
    int32_t D_8089C1A0_copy;

    // z_bg_mori_hineri
    int16_t sBgMoriHineriNextCamIdx_copy;

    // z_bg_po_event
    uint8_t sBgPoEventBlocksAtRest_copy;
    uint8_t sBgPoEventPuzzleState_copy;
    float sBgPoEventblockPushDist_copy;

    // z_bg_relay_objects
    uint32_t D_808A9508_copy;

    // z_bg_spot18_basket
    int16_t D_808B85D0_copy;

    // z_boss_ganon
    uint32_t sBossGanonSeed1_copy;
    uint32_t sBossGanonSeed2_copy;
    uint32_t sBossGanonSeed3_copy;
    void* sBossGanonGanondorf_copy;
    void* sBossGanonZelda_copy;
    void* sBossGanonCape_copy;
    GanondorfEffect sBossGanonEffectBuf_copy[200];

    // z_boss_ganon
    uint32_t sBossGanonSeed1;
    uint32_t sBossGanonSeed2;
    uint32_t sBossGanonSeed3;
    void* sBossGanonGanondorf;
    void* sBossGanonZelda;
    void* sBossGanonCape;
    GanondorfEffect sBossGanonEffectBuf[200];

    // z_boss_ganon2
    Vec3f D_8090EB20_copy;
    int8_t D_80910638_copy;
    void* sBossGanon2Zelda_copy;
    void* D_8090EB30_copy;
    int32_t sBossGanon2Seed1_copy;
    int32_t sBossGanon2Seed2_copy;
    int32_t sBossGanon2Seed3_copy;
    Vec3f D_809105D8_copy[4];
    Vec3f D_80910608_copy[4];
    BossGanon2Effect sBossGanon2Particles_copy[100];

    // z_boss_tw
    uint8_t sTwInitalized_copy;
    BossTwEffect sTwEffects_copy[150];

    // z_demo_6k
    Vec3f sDemo6kVelocity_copy;

    // z_demo_du
    int32_t D_8096CE94_copy;

    // z_demo_kekkai
    Vec3f demoKekkaiVel_copy;

    // z_en_bw
    int32_t sSlugGroup_copy;

    // z_en_clear_tag
    uint8_t sClearTagIsEffectInitialized_copy;
    EnClearTagEffect sClearTagEffects_copy[CLEAR_TAG_EFFECT_MAX_COUNT];

    // z_en_fr
    EnFrPointers sEnFrPointers_copy;

    // z_en_goma
    uint8_t sSpawnNum_copy;

    // z_en_insect
    float D_80A7DEB0_copy;
    int16_t D_80A7DEB4_copy;
    int16_t D_80A7DEB8_copy;

    // z_en_ishi
    int16_t sRockRotSpeedX_copy;
    int16_t sRockRotSpeedY_copy;

    // z_en_niw
    int16_t D_80AB85E0_copy;
    uint8_t sLowerRiverSpawned_copy;
    uint8_t sUpperRiverSpawned_copy;

    // z_en_po_field
    int32_t sEnPoFieldNumSpawned_copy;
    Vec3s sEnPoFieldSpawnPositions_copy[10];
    u8 sEnPoFieldSpawnSwitchFlags_copy[10];

    // z_en_takara_man
    uint8_t sTakaraIsInitialized_copy;

    // z_en_xc
    int32_t D_80B41D90_copy;
    int32_t sEnXcFlameSpawned_copy;
    int32_t D_80B41DA8_copy;
    int32_t D_80B41DAC_copy;

    // z_en_zf
    int16_t D_80B4A1B0_copy;
    int16_t D_80B4A1B4_copy;

    int32_t D_80B5A468_copy;
    int32_t D_80B5A494_copy;
    int32_t D_80B5A4BC_copy;

    uint8_t sKankyoIsSpawned_copy;
    int16_t sTrailingFairies_copy;

    // z_en_heishi1
    uint32_t sHeishi1PlayerIsCaughtCopy;


    //Misc static data
    // z_map_exp

    s16 sPlayerInitialPosX_copy;
    s16 sPlayerInitialPosZ_copy;
    s16 sPlayerInitialDirection_copy;

    // code_800E(something. fill me in later)
    u8 sOcarinaInpEnabled_copy;
    s8 D_80130F10_copy;
    u8 sCurOcarinaBtnVal_copy;
    u8 sPrevOcarinaNoteVal_copy;
    u8 sCurOcarinaBtnIdx_copy;
    u8 sLearnSongLastBtn_copy;
    f32 D_80130F24_copy;
    f32 D_80130F28_copy;
    s8 D_80130F2C_copy;
    s8 D_80130F30_copy;
    s8 D_80130F34_copy;
    u8 sDisplayedNoteValue_copy;
    u8 sPlaybackState_copy;
    u32 D_80130F3C_copy;
    u32 sNotePlaybackTimer_copy;
    u16 sPlaybackNotePos_copy;
    u16 sStaffPlaybackPos_copy;

    u32 sCurOcarinaBtnPress_copy;
    u32 D_8016BA10_copy;
    u32 sPrevOcarinaBtnPress_copy;
    s32 D_8016BA18_copy;
    s32 D_8016BA1C_copy;
    u8 sCurOcarinaSong_copy[8];
    u8 sOcarinaSongAppendPos_copy;
    u8 sOcarinaHasStartedSong_copy;
    u8 sOcarinaSongNoteStartIdx_copy;
    u8 sOcarinaSongCnt_copy;
    u16 sOcarinaAvailSongs_copy;
    u8 sStaffPlayingPos_copy;
    u16 sLearnSongPos_copy[0x10];
    u16 D_8016BA50_copy[0x10];
    u16 D_8016BA70_copy[0x10];
    u8 sLearnSongExpectedNote_copy[0x10];
    OcarinaNote D_8016BAA0_copy;
    u8 sAudioHasMalonBgm_copy;
    f32 sAudioMalonBgmDist_copy;

    // Message_PAL
    s16 sOcarinaNoteBufPos_copy;
    s16 sOcarinaNoteBufLen_copy;
    u8 sOcarinaNoteBuf_copy[12];

    u8 D_8014B2F4_copy;
    u8 sTextboxSkipped_copy;
    u16 sNextTextId_copy;
    s16 sLastPlayedSong_copy;
    s16 sHasSunsSong_copy;
    s16 sMessageHasSetSfx_copy;
    u16 sOcarinaSongBitFlags_copy;


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
    void SaveOnePointDemoData(void);
    void LoadOnePointDemoData(void);
    void SaveOverlayStaticData(void);
    void LoadOverlayStaticData(void);

    void SaveMiscCodeData(void);
    void LoadMiscCodeData(void);

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
    info->depthPhase_copy = depthPhase;
    info->screenPlanePhase_copy = screenPlanePhase;
    info->sOOBTimer_copy = sOOBTimer;
    info->D_8015CE50_copy = D_8015CE50;
    info->D_8015CE54_copy = D_8015CE54;
    memcpy(&info->D_8015CE58_copy, &D_8015CE58, sizeof(info->D_8015CE58_copy));
}

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
    depthPhase = info->depthPhase_copy;
    screenPlanePhase = info->screenPlanePhase_copy;
    sOOBTimer = info->sOOBTimer_copy;
    D_8015CE50 = info->D_8015CE50_copy;
    D_8015CE54 = info->D_8015CE54_copy;
    memcpy(&D_8015CE58, &info->D_8015CE58_copy, sizeof(info->D_8015CE58_copy));
}

void SaveState::SaveOnePointDemoData(void) {
    info->sPrevFrameCs1100_copy = sPrevFrameCs1100;
    memcpy(info->D_8012013C_copy, D_8012013C, sizeof(info->D_8012013C_copy));
    memcpy(info->D_8012021C_copy, D_8012021C, sizeof(info->D_8012021C_copy));
    memcpy(info->D_801204D4_copy, D_801204D4, sizeof(info->D_801204D4_copy));
    memcpy(info->D_801205B4_copy, D_801205B4, sizeof(info->D_801205B4_copy));
    memcpy(info->D_801208EC_copy, D_801208EC, sizeof(info->D_801208EC_copy));
    memcpy(info->D_80120964_copy, D_80120964, sizeof(info->D_80120964_copy));
    memcpy(info->D_801209B4_copy, D_801209B4, sizeof(info->D_801209B4_copy));
    memcpy(info->D_80120ACC_copy, D_80120ACC, sizeof(info->D_80120ACC_copy));
    memcpy(info->D_80120B94_copy, D_80120B94, sizeof(info->D_80120B94_copy));
    memcpy(info->D_80120D4C_copy, D_80120D4C, sizeof(info->D_80120D4C_copy));
    memcpy(info->D_80120FA4_copy, D_80120FA4, sizeof(info->D_80120FA4_copy));
    memcpy(info->D_80121184_copy, D_80121184, sizeof(info->D_80121184_copy));
    memcpy(info->D_801211D4_copy, D_801211D4, sizeof(info->D_801211D4_copy));
    memcpy(info->D_8012133C_copy, D_8012133C, sizeof(info->D_8012133C_copy));
    memcpy(info->D_801213B4_copy, D_801213B4, sizeof(info->D_801213B4_copy));
    memcpy(info->D_8012151C_copy, D_8012151C, sizeof(info->D_8012151C_copy));
    memcpy(info->D_8012156C_copy, D_8012156C, sizeof(info->D_8012156C_copy));
    memcpy(info->D_801215BC_copy, D_801215BC, sizeof(info->D_801215BC_copy));
    memcpy(info->D_80121C24_copy, D_80121C24, sizeof(info->D_80121C24_copy));
    memcpy(info->D_80121D3C_copy, D_80121D3C, sizeof(info->D_80121D3C_copy));
    memcpy(info->D_80121F1C_copy, D_80121F1C, sizeof(info->D_80121F1C_copy));
    memcpy(info->D_80121FBC_copy, D_80121FBC, sizeof(info->D_80121FBC_copy));
    memcpy(info->D_801220D4_copy, D_801220D4, sizeof(info->D_801220D4_copy));
    memcpy(info->D_80122714_copy, D_80122714, sizeof(info->D_80122714_copy));
    memcpy(info->D_80122CB4_copy, D_80122CB4, sizeof(info->D_80122CB4_copy));
    memcpy(info->D_80122D04_copy, D_80122D04, sizeof(info->D_80122D04_copy));
    memcpy(info->D_80122E44_copy, D_80122E44, sizeof(info->D_80122E44_copy));
    memcpy(info->D_8012313C_copy, D_8012313C, sizeof(info->D_8012313C_copy));
    memcpy(info->D_801231B4_copy, D_801231B4, sizeof(info->D_801231B4_copy));
    memcpy(info->D_80123254_copy, D_80123254, sizeof(info->D_80123254_copy));
    memcpy(info->D_801232A4_copy, D_801232A4, sizeof(info->D_801232A4_copy));
    memcpy(info->D_80123894_copy, D_80123894, sizeof(info->D_80123894_copy));
    memcpy(info->D_8012390C_copy, D_8012390C, sizeof(info->D_8012390C_copy));
    memcpy(info->D_8012395C_copy, D_8012395C, sizeof(info->D_8012395C_copy));
    memcpy(info->D_801239D4_copy, D_801239D4, sizeof(info->D_801239D4_copy));
}

void SaveState::LoadOnePointDemoData(void) {
    sPrevFrameCs1100 = info->sPrevFrameCs1100_copy;
    memcpy(D_8012013C, info->D_8012013C_copy, sizeof(info->D_8012013C_copy));
    memcpy(D_8012021C, info->D_8012021C_copy, sizeof(info->D_8012021C_copy));
    memcpy(D_801204D4, info->D_801204D4_copy, sizeof(info->D_801204D4_copy));
    memcpy(D_801205B4, info->D_801205B4_copy, sizeof(info->D_801205B4_copy));
    memcpy(D_801208EC, info->D_801208EC_copy, sizeof(info->D_801208EC_copy));
    memcpy(D_80120964, info->D_80120964_copy, sizeof(info->D_80120964_copy));
    memcpy(D_801209B4, info->D_801209B4_copy, sizeof(info->D_801209B4_copy));
    memcpy(D_80120ACC, info->D_80120ACC_copy, sizeof(info->D_80120ACC_copy));
    memcpy(D_80120B94, info->D_80120B94_copy, sizeof(info->D_80120B94_copy));
    memcpy(D_80120D4C, info->D_80120D4C_copy, sizeof(info->D_80120D4C_copy));
    memcpy(D_80120FA4, info->D_80120FA4_copy, sizeof(info->D_80120FA4_copy));
    memcpy(D_80121184, info->D_80121184_copy, sizeof(info->D_80121184_copy));
    memcpy(D_801211D4, info->D_801211D4_copy, sizeof(info->D_801211D4_copy));
    memcpy(D_8012133C, info->D_8012133C_copy, sizeof(info->D_8012133C_copy));
    memcpy(D_801213B4, info->D_801213B4_copy, sizeof(info->D_801213B4_copy));
    memcpy(D_8012151C, info->D_8012151C_copy, sizeof(info->D_8012151C_copy));
    memcpy(D_8012156C, info->D_8012156C_copy, sizeof(info->D_8012156C_copy));
    memcpy(D_801215BC, info->D_801215BC_copy, sizeof(info->D_801215BC_copy));
    memcpy(D_80121C24, info->D_80121C24_copy, sizeof(info->D_80121C24_copy));
    memcpy(D_80121D3C, info->D_80121D3C_copy, sizeof(info->D_80121D3C_copy));
    memcpy(D_80121F1C, info->D_80121F1C_copy, sizeof(info->D_80121F1C_copy));
    memcpy(D_80121FBC, info->D_80121FBC_copy, sizeof(info->D_80121FBC_copy));
    memcpy(D_801220D4, info->D_801220D4_copy, sizeof(info->D_801220D4_copy));
    memcpy(D_80122714, info->D_80122714_copy, sizeof(info->D_80122714_copy));
    memcpy(D_80122CB4, info->D_80122CB4_copy, sizeof(info->D_80122CB4_copy));
    memcpy(D_80122D04, info->D_80122D04_copy, sizeof(info->D_80122D04_copy));
    memcpy(D_80122E44, info->D_80122E44_copy, sizeof(info->D_80122E44_copy));
    memcpy(D_8012313C, info->D_8012313C_copy, sizeof(info->D_8012313C_copy));
    memcpy(D_801231B4, info->D_801231B4_copy, sizeof(info->D_801231B4_copy));
    memcpy(D_80123254, info->D_80123254_copy, sizeof(info->D_80123254_copy));
    memcpy(D_801232A4, info->D_801232A4_copy, sizeof(info->D_801232A4_copy));
    memcpy(D_80123894, info->D_80123894_copy, sizeof(info->D_80123894_copy));
    memcpy(D_8012390C, info->D_8012390C_copy, sizeof(info->D_8012390C_copy));
    memcpy(D_8012395C, info->D_8012395C_copy, sizeof(info->D_8012395C_copy));
    memcpy(D_801239D4, info->D_801239D4_copy, sizeof(info->D_801239D4_copy));
}

void SaveState::SaveOverlayStaticData(void) {
    info->sBgDdanKdVelocity_copy = sBgDdanKdVelocity;
    info->sBgDdanKdAccel_copy = sBgDdanKdAccel;
    info->sBgDodoagoFirstExplosiveFlag_copy = sBgDodoagoFirstExplosiveFlag;
    info->sBgDodoagoDisableBombCatcher_copy = sBgDodoagoDisableBombCatcher;
    info->sBgDodoagoTimer_copy = sBgDodoagoTimer;
    info->D_80880F30_copy = D_80880F30;
    info->D_80881014_copy = D_80881014;
    info->D_8088BFC0_copy = D_8088BFC0;
    info->sBgMoriHineriNextCamIdx_copy = sBgMoriHineriNextCamIdx;
    info->sBgPoEventBlocksAtRest_copy = sBgPoEventBlocksAtRest;
    info->sBgPoEventPuzzleState_copy = sBgPoEventPuzzleState;
    info->sBgPoEventblockPushDist_copy = sBgPoEventblockPushDist;
    info->D_808A9508_copy = D_808A9508;
    info->D_808B85D0_copy = D_808B85D0;
    info->sBossGanonSeed1_copy = sBossGanonSeed1;
    info->sBossGanonSeed2_copy = sBossGanonSeed2;
    info->sBossGanonSeed3_copy = sBossGanonSeed3;
    info->sBossGanonGanondorf_copy = sBossGanonGanondorf;
    info->sBossGanonZelda_copy = sBossGanonZelda;
    info->sBossGanonCape_copy = sBossGanonCape;
    memcpy(info->sBossGanonEffectBuf_copy, sBossGanonEffectBuf, sizeof(info->sBossGanonEffectBuf_copy));
    info->D_8090EB20_copy = D_8090EB20;
    info->D_80910638_copy = D_80910638;
    info->sBossGanon2Zelda_copy = sBossGanon2Zelda;
    info->D_8090EB30_copy = D_8090EB30;
    info->sBossGanon2Seed1_copy = sBossGanon2Seed1;
    info->sBossGanon2Seed2_copy = sBossGanon2Seed2;
    info->sBossGanon2Seed3_copy = sBossGanon2Seed3;
    memcpy(info->D_809105D8_copy, D_809105D8, sizeof(D_809105D8));
    memcpy(info->D_80910608_copy, D_80910608, sizeof(D_80910608));
    memcpy(info->sBossGanon2Particles_copy, sBossGanon2Particles, sizeof(sBossGanon2Particles));
    info->sTwInitalized_copy = sTwInitalized;
    memcpy(info->sTwEffects_copy, sTwEffects, sizeof(sTwEffects));
    info->sDemo6kVelocity_copy = sDemo6kVelocity;
    info->D_8096CE94_copy = D_8096CE94;
    info->demoKekkaiVel_copy = demoKekkaiVel;
    info->sSlugGroup_copy = sSlugGroup;
    info->sClearTagIsEffectInitialized_copy = sClearTagIsEffectsInitialized;
    memcpy(info->sClearTagEffects_copy, sClearTagEffects, sizeof(sClearTagEffects));

    memcpy(&info->sEnFrPointers_copy, &sEnFrPointers, sizeof(info->sEnFrPointers_copy));
    info->sSpawnNum_copy = sSpawnNum;

    info->D_80A7DEB0_copy = D_80A7DEB0;
    info->D_80A7DEB4_copy = D_80A7DEB4;
    info->D_80A7DEB8_copy = D_80A7DEB8; 
    info->sRockRotSpeedX_copy = sRockRotSpeedX;
    info->sRockRotSpeedY_copy = sRockRotSpeedY;
    info->D_80AB85E0_copy = D_80AB85E0;
    info->sLowerRiverSpawned_copy = sLowerRiverSpawned;
    info->sUpperRiverSpawned_copy = sUpperRiverSpawned;
    info->sEnPoFieldNumSpawned_copy = sEnPoFieldNumSpawned;
    memcpy(info->sEnPoFieldSpawnPositions_copy, sEnPoFieldSpawnPositions, sizeof(info->sEnPoFieldSpawnPositions_copy));
    memcpy(info->sEnPoFieldSpawnSwitchFlags_copy, sEnPoFieldSpawnSwitchFlags, sizeof(info->sEnPoFieldSpawnSwitchFlags_copy));

    info->sTakaraIsInitialized_copy = sTakaraIsInitialized;
    info->D_80B41D90_copy = D_80B41D90;
    info->sEnXcFlameSpawned_copy = sEnXcFlameSpawned;
    info->D_80B41DA8_copy = D_80B41DA8;
    info->D_80B41DAC_copy = D_80B41DAC;
    info->D_80B4A1B0_copy = D_80B4A1B0;
    info->D_80B4A1B4_copy = D_80B4A1B4;
    info->D_80B5A468_copy = D_80B5A468;
    info->D_80B5A494_copy = D_80B5A494;
    info->D_80B5A4BC_copy = D_80B5A4BC;
    info->sKankyoIsSpawned_copy = sKankyoIsSpawned;
    info->sTrailingFairies_copy = sTrailingFairies;
    
    info->sHeishi1PlayerIsCaughtCopy = sHeishi1PlayerIsCaught;

}

void SaveState::LoadOverlayStaticData(void) {
    sBgDdanKdVelocity = info->sBgDdanKdVelocity_copy;
    sBgDdanKdAccel = info->sBgDdanKdAccel_copy;
    sBgDodoagoFirstExplosiveFlag = info->sBgDodoagoFirstExplosiveFlag_copy;
    sBgDodoagoDisableBombCatcher = info->sBgDodoagoDisableBombCatcher_copy;
    sBgDodoagoTimer = info->sBgDodoagoTimer_copy;
    D_80880F30 = info->D_80880F30_copy;
    D_80881014 = info->D_80881014_copy;
    D_8088BFC0 = info->D_8088BFC0_copy;
    sBgMoriHineriNextCamIdx = info->sBgMoriHineriNextCamIdx_copy;
    sBgPoEventBlocksAtRest = info->sBgPoEventBlocksAtRest_copy;
    sBgPoEventPuzzleState = info->sBgPoEventPuzzleState_copy;
    sBgPoEventblockPushDist = info->sBgPoEventblockPushDist_copy;
    D_808A9508 = info->D_808A9508_copy;
    D_808B85D0 = info->D_808B85D0_copy;
    sBossGanonSeed1 = info->sBossGanonSeed1_copy;
    sBossGanonSeed2 = info->sBossGanonSeed2_copy;
    sBossGanonSeed3 = info->sBossGanonSeed3_copy;
    sBossGanonGanondorf = info->sBossGanonGanondorf_copy;
    sBossGanonZelda = info->sBossGanonZelda_copy;
    sBossGanonCape = info->sBossGanonCape_copy;
    memcpy(sBossGanonEffectBuf, info->sBossGanonEffectBuf_copy, sizeof(info->sBossGanonEffectBuf_copy));

    D_8090EB20 = info->D_8090EB20_copy;
    D_80910638 = info->D_80910638_copy;
    sBossGanon2Zelda = info->sBossGanon2Zelda_copy;
    D_8090EB30 = info->D_8090EB30_copy;
    sBossGanon2Seed1 = info->sBossGanon2Seed1_copy;
    sBossGanon2Seed2 = info->sBossGanon2Seed2_copy;
    sBossGanon2Seed3 = info->sBossGanon2Seed3_copy;
    memcpy(D_809105D8, info->D_809105D8_copy, sizeof(D_809105D8));
    memcpy(D_80910608, info->D_80910608_copy, sizeof(D_80910608));
    memcpy(sBossGanon2Particles, info->sBossGanon2Particles_copy, sizeof(sBossGanon2Particles));
    sTwInitalized = info->sTwInitalized_copy;
    memcpy(sTwEffects, info->sTwEffects_copy, sizeof(sTwEffects));
    sDemo6kVelocity = info->sDemo6kVelocity_copy;

    D_8096CE94 = info->D_8096CE94_copy;
    demoKekkaiVel = info->demoKekkaiVel_copy;
    sSlugGroup = info->sSlugGroup_copy;
    sClearTagIsEffectsInitialized = info->sClearTagIsEffectInitialized_copy;
    memcpy(sClearTagEffects, info->sClearTagEffects_copy, sizeof(sClearTagEffects));

    D_80A7DEB0 = info->D_80A7DEB0_copy;
    D_80A7DEB4 = info->D_80A7DEB4_copy;
    D_80A7DEB8 = info->D_80A7DEB8_copy;
    sRockRotSpeedX = info->sRockRotSpeedX_copy;
    sRockRotSpeedY = info->sRockRotSpeedY_copy;
    D_80AB85E0 = info->D_80AB85E0_copy;
    sLowerRiverSpawned = info->sLowerRiverSpawned_copy;
    sUpperRiverSpawned = info->sUpperRiverSpawned_copy;
    sEnPoFieldNumSpawned = info->sEnPoFieldNumSpawned_copy;
    memcpy(sEnPoFieldSpawnPositions, info->sEnPoFieldSpawnPositions_copy, sizeof(info->sEnPoFieldSpawnPositions_copy));
    memcpy(sEnPoFieldSpawnSwitchFlags, info->sEnPoFieldSpawnSwitchFlags_copy, sizeof(info->sEnPoFieldSpawnSwitchFlags_copy));

    sTakaraIsInitialized = info->sTakaraIsInitialized_copy;
    D_80B41D90 = info->D_80B41D90_copy;
    sEnXcFlameSpawned = info->sEnXcFlameSpawned_copy;
    D_80B41DA8 = info->D_80B41DA8_copy;
    D_80B41DAC = info->D_80B41DAC_copy;
    D_80B4A1B0 = info->D_80B4A1B0_copy;
    D_80B4A1B4 = info->D_80B4A1B4_copy;
    D_80B5A468 = info->D_80B5A468_copy;
    D_80B5A494 = info->D_80B5A494_copy;
    D_80B5A4BC = info->D_80B5A4BC_copy;
    sKankyoIsSpawned = info->sKankyoIsSpawned_copy;
    sTrailingFairies = info->sTrailingFairies_copy;

    sHeishi1PlayerIsCaught = info->sHeishi1PlayerIsCaughtCopy;
}

void SaveState::SaveMiscCodeData(void) {
    info->gGameOverTimer_copy = gGameOverTimer;
    info->gTimeIncrement_copy = gTimeIncrement;
    info->sLoadedMarkDataTableCopy = sLoadedMarkDataTable;

    info->sPlayerInitialPosX_copy = sPlayerInitialPosX;
    info->sPlayerInitialPosZ_copy = sPlayerInitialPosZ;
    info->sPlayerInitialDirection_copy = sPlayerInitialDirection;

    info->sOcarinaInpEnabled_copy = sOcarinaInpEnabled;
    info->D_80130F10_copy = D_80130F10;
    info->sCurOcarinaBtnVal_copy = sCurOcarinaBtnVal;
    info->sPrevOcarinaNoteVal_copy = sPrevOcarinaNoteVal;
    info->sCurOcarinaBtnIdx_copy = sCurOcarinaBtnIdx;
    info->sLearnSongLastBtn_copy = sLearnSongLastBtn;
    info->D_80130F24_copy = D_80130F24;
    info->D_80130F28_copy = D_80130F28;
    info->D_80130F2C_copy = D_80130F2C;
    info->D_80130F30_copy = D_80130F30;
    info->D_80130F34_copy = D_80130F34;
    info->sPlaybackState_copy = sPlaybackState;
    info->D_80130F3C_copy = D_80130F3C;
    info->sNotePlaybackTimer_copy = sNotePlaybackTimer;
    info->sPlaybackNotePos_copy = sPlaybackNotePos;
    info->sStaffPlaybackPos_copy = sStaffPlaybackPos;


    info->sCurOcarinaBtnPress_copy = sCurOcarinaBtnPress;
    info->D_8016BA10_copy = D_8016BA10;
    info->sPrevOcarinaBtnPress_copy = sPrevOcarinaBtnPress;
    info->D_8016BA18_copy = D_8016BA18;
    info->D_8016BA1C_copy = D_8016BA1C;
    memcpy(info->sCurOcarinaSong_copy, sCurOcarinaSong, sizeof(sCurOcarinaSong));
    info->sOcarinaSongAppendPos_copy = sOcarinaSongAppendPos;
    info->sOcarinaHasStartedSong_copy = sOcarinaHasStartedSong;
    info->sOcarinaSongNoteStartIdx_copy = sOcarinaSongNoteStartIdx;
    info->sOcarinaSongCnt_copy = sOcarinaSongCnt;
    info->sOcarinaAvailSongs_copy = sOcarinaAvailSongs;
    info->sStaffPlayingPos_copy = sStaffPlayingPos;
    memcpy(info->sLearnSongPos_copy, sLearnSongPos, sizeof(sLearnSongPos));
    memcpy(info->D_8016BA50_copy, D_8016BA50, sizeof(D_8016BA50));
    memcpy(info->D_8016BA70_copy, D_8016BA70, sizeof(D_8016BA70));
    memcpy(info->sLearnSongExpectedNote_copy, sLearnSongExpectedNote, sizeof(sLearnSongExpectedNote));
    memcpy(&info->D_8016BAA0_copy, &D_8016BAA0, sizeof(D_8016BAA0));
    info->sAudioHasMalonBgm_copy = sAudioHasMalonBgm;
    info->sAudioMalonBgmDist_copy = sAudioMalonBgmDist;
    info->sDisplayedNoteValue_copy = sDisplayedNoteValue;


    info->sOcarinaNoteBufPos_copy = sOcarinaNoteBufPos;
    info->sOcarinaNoteBufLen_copy = sOcarinaNoteBufLen;
    memcpy(info->sOcarinaNoteBuf_copy, sOcarinaNoteBuf, sizeof(sOcarinaNoteBuf));
    info->D_8014B2F4_copy = D_8014B2F4;
    info->sTextboxSkipped_copy = sTextboxSkipped;
    info->sNextTextId_copy = sNextTextId;
    info->sLastPlayedSong_copy = sLastPlayedSong;
    info->sHasSunsSong_copy = sHasSunsSong;
    info->sMessageHasSetSfx_copy = sMessageHasSetSfx;
    info->sOcarinaSongBitFlags_copy = sOcarinaSongBitFlags;
}

void SaveState::LoadMiscCodeData(void) {
    gGameOverTimer = info->gGameOverTimer_copy;
    gTimeIncrement = info->gTimeIncrement_copy;
    sLoadedMarkDataTable = info->sLoadedMarkDataTableCopy;

    sPlayerInitialPosX = info->sPlayerInitialPosX_copy;
    sPlayerInitialPosZ = info->sPlayerInitialPosZ_copy;
    sPlayerInitialDirection = info->sPlayerInitialDirection_copy;

    sOcarinaInpEnabled = info->sOcarinaInpEnabled_copy;
    D_80130F10 = info->D_80130F10_copy;
    sCurOcarinaBtnVal = info->sCurOcarinaBtnVal_copy;
    sPrevOcarinaNoteVal = info->sPrevOcarinaNoteVal_copy;
    sCurOcarinaBtnIdx = info->sCurOcarinaBtnIdx_copy;
    sLearnSongLastBtn = info->sLearnSongLastBtn_copy;
    D_80130F24 = info->D_80130F24_copy;
    D_80130F28 = info->D_80130F28_copy;
    D_80130F2C = info->D_80130F2C_copy;
    D_80130F30 = info->D_80130F30_copy;
    D_80130F34 = info->D_80130F34_copy;
    sPlaybackState = info->sPlaybackState_copy;
    D_80130F3C = info->D_80130F3C_copy;
    sNotePlaybackTimer = info->sNotePlaybackTimer_copy;
    sPlaybackNotePos = info->sPlaybackNotePos_copy;
    sStaffPlaybackPos = info->sStaffPlaybackPos_copy;

    sCurOcarinaBtnPress = info->sCurOcarinaBtnPress_copy;
    D_8016BA10 = info->D_8016BA10_copy;
    sPrevOcarinaBtnPress = info->sPrevOcarinaBtnPress_copy;
    D_8016BA18 = info->D_8016BA18_copy;
    D_8016BA1C = info->D_8016BA1C_copy;
    memcpy(sCurOcarinaSong, info->sCurOcarinaSong_copy, sizeof(sCurOcarinaSong));
    sOcarinaSongAppendPos = info->sOcarinaSongAppendPos_copy;
    sOcarinaHasStartedSong = info->sOcarinaHasStartedSong_copy;
    sOcarinaSongNoteStartIdx = info->sOcarinaSongNoteStartIdx_copy;
    sOcarinaSongCnt = info->sOcarinaSongCnt_copy;
    sOcarinaAvailSongs = info->sOcarinaAvailSongs_copy;
    sStaffPlayingPos = info->sStaffPlayingPos_copy;
    memcpy(info->sLearnSongPos_copy, info->sLearnSongPos_copy, sizeof(sLearnSongPos));
    memcpy(info->D_8016BA50_copy, info->D_8016BA50_copy, sizeof(D_8016BA50));
    memcpy(info->D_8016BA70_copy, info->D_8016BA70_copy, sizeof(D_8016BA70));
    memcpy(info->sLearnSongExpectedNote_copy, info->sLearnSongExpectedNote_copy, sizeof(sLearnSongExpectedNote));
    memcpy(&D_8016BAA0, &info->D_8016BAA0_copy, sizeof(D_8016BAA0));
    sAudioHasMalonBgm = info->sAudioHasMalonBgm_copy;
    sAudioMalonBgmDist = info->sAudioMalonBgmDist_copy;
    sDisplayedNoteValue = info->sDisplayedNoteValue_copy;
    
    sOcarinaNoteBufPos = info->sOcarinaNoteBufPos_copy;
    sOcarinaNoteBufLen = info->sOcarinaNoteBufLen_copy;
    memcpy(sOcarinaNoteBuf, info->sOcarinaNoteBuf_copy, sizeof(sOcarinaNoteBuf));

    D_8014B2F4 = info->D_8014B2F4_copy;
    sTextboxSkipped = info->sTextboxSkipped_copy;
    sNextTextId = info->sNextTextId_copy;
    sLastPlayedSong = info->sLastPlayedSong_copy;
    sHasSunsSong = info->sHasSunsSong_copy;
    sMessageHasSetSfx = info->sMessageHasSetSfx_copy;
    sOcarinaSongBitFlags = info->sOcarinaSongBitFlags_copy;


}

extern "C" void ProcessSaveStateRequests(void) {
    OTRGlobals::Instance->gSaveStateMgr->ProcessSaveStateRequests();
}

void SaveStateMgr::SetCurrentSlot(unsigned int slot) {
    Ship::GetGameOverlay()->TextDrawNotification(1.0f, true, "slot %u set", slot);
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
                Ship::GetGameOverlay()->TextDrawNotification(1.0f, true, "saved state %u", request.slot);
                break;
            case RequestType::LOAD:
                if (this->states.contains(request.slot)) {
                    this->states[request.slot]->Load();
                    Ship::GetGameOverlay()->TextDrawNotification(1.0f, true, "loaded state %u", request.slot);
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
    if (gPlayState == nullptr) {
        SPDLOG_ERROR("[SOH] Can not save or load a state outside of \"GamePlay\"");
        Ship::GetGameOverlay()->TextDrawNotification(1.0f, true, "states not available here", request.slot);
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
                SPDLOG_ERROR("Invalid SaveState slot: {}", request.type);
                Ship::GetGameOverlay()->TextDrawNotification(1.0f, true, "state slot %u empty", request.slot);
                return SaveStateReturn::FAIL_INVALID_SLOT;
            }
        [[unlikely]] default: 
            SPDLOG_ERROR("Invalid SaveState request type: {}", request.type);
            return SaveStateReturn::FAIL_BAD_REQUEST;
    }
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
    memcpy(&info->mtxStackCopy, sMatrixStack, sizeof(MtxF) * 20);
    memcpy(&info->currentMtxCopy, sCurrentMatrix, sizeof(MtxF));

    //Various static data
    info->blueWarpTimerCopy = sWarpTimerTarget;
    BackupCameraData();
    SaveOnePointDemoData();
    SaveOverlayStaticData();
    SaveMiscCodeData();

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
    memcpy(sMatrixStack, &info->mtxStackCopy, sizeof(MtxF) * 20);
    memcpy(sCurrentMatrix, &info->currentMtxCopy, sizeof(MtxF));
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
    LoadCameraData();
    LoadOnePointDemoData();
    LoadOverlayStaticData();
    LoadMiscCodeData();

}
