#ifndef SAVE_STATES_H
#define SAVE_STATES_H
#include "z64.h"
#include "z64save.h"

#include <cstdint>
#include <array>
#include <queue>

// FROM z_lights.c 
// I didn't feel like moving it into a header file.
#define LIGHTS_BUFFER_SIZE 32
//#define LIGHTS_BUFFER_SIZE 1024 // Kill me

typedef struct {
    /* 0x000 */ s32 numOccupied;
    /* 0x004 */ s32 searchIndex;
    /* 0x008 */ LightNode buf[LIGHTS_BUFFER_SIZE];
} LightsBuffer; // size = 0x188


enum class SaveStateReturn {
    SUCCESS,
    FAIL_INVALID_SLOT,
    FAIL_NO_MEMORY,
    FAIL_STATE_EMPTY,
	FAIL_FILE_NOT_FOUND,
	FAIL_FILE_NOT_OPENED,
	FAIL_INVALID_MAGIC,
	FAIL_INVALID_SIZE,
};

typedef struct SaveStateHeader {
    uint32_t stateMagic;
    uint32_t stateVersion;
    //uint32_t gameVersion;
} SaveStateHeader;

enum class RequestType {
    SAVE,
    LOAD,
};

typedef struct SaveStateInfo {
    SaveStateHeader stateHeader;

    unsigned char sysHeapCopy[SYSTEM_HEAP_SIZE];
    unsigned char audioHeapCopy[AUDIO_HEAP_SIZE];

    SaveContext saveContextCopy;
    GameInfo gameInfoCopy;
    LightsBuffer lightBufferCopy;
    AudioContext audioContextCopy;
    std::array<MtxF, 20> mtxStackCopy; // always 20 matricies
    MtxF currentMtxCopy;
    uint32_t rngSeed;
    int16_t blueWarpTimerCopy; /* From door_warp_1 */

    std::array<SeqScriptState, 4> seqScriptStateCopy; // Unrelocated
    // std::array<u8, 4> seqIdCopy;
    std::array<unk_D_8016E750, 4> unk_D_8016E750Copy;

    ActiveSound gActiveSoundsCopy[7][MAX_CHANNELS_PER_BANK];
    std::array<u8, 7> gSoundBankMutedCopy;
    u8 D_801333F0_copy;
    u8 gAudioSfxSwapOff_copy;
    std::array<u16, 10> gAudioSfxSwapSource_copy;
    std::array<u16, 10> gAudioSfxSwapTarget_copy;
    std::array<u8, 10> gAudioSfxSwapMode_copy;
    void (*D_801755D0_copy)(void);
} SaveStateInfo;

extern unsigned int gCurrentSlot;

typedef struct SaveStateRequest {
    unsigned int slot;
    RequestType type;
} SaveStateRequest;

class SaveState {
  public:
    static constexpr unsigned int SAVE_SLOT_MAX = 2;

	static std::queue <SaveStateRequest> requests;
	
    SaveState();
    ~SaveState();
    
    SaveState& operator=(const SaveState& rhs) = delete;
    SaveState(const SaveState& rhs) = delete;

    void Init(void);

    static SaveStateReturn Export(const unsigned int slot);
    static SaveStateReturn Import(const unsigned int slot);
    static SaveStateReturn Delete(const unsigned int slot);
    SaveStateInfo* GetSaveStateInfo(const unsigned int slot);
    static SaveStateReturn RequestSaveState(const unsigned int slot);
    static SaveStateReturn RequestLoadState(const unsigned int slot);

    static void ProcessSaveStateRequestsImpl(void);
	
  private:
    static void Save(const unsigned int slot);
    static void Load(const unsigned int slot);
    static void SetHeader(SaveStateHeader& header);
    static void GetHeader(SaveStateHeader& header);
    static void BackupSeqScriptState(SaveStateInfo* state);
    static void LoadSeqScriptState(SaveStateInfo* state);
	
    SaveStateInfo saveStateInfo;
	
   
    
};


#endif
