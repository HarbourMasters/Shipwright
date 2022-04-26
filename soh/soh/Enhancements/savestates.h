#ifndef SAVE_STATES_H
#define SAVE_STATES_H
#include "z64.h"
#include "z64save.h"

#include <cstdint>
#include <array>

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

extern unsigned int gCurrentSlot;

class SaveState {
  public:
    static constexpr unsigned int SAVE_SLOT_MAX = 2;
    SaveState();
    ~SaveState();
    
    SaveState& operator=(const SaveState& rhs) = delete;
    SaveState(const SaveState& rhs) = delete;

    void Init(void);
    static SaveStateReturn Save(const unsigned int slot);
    static SaveStateReturn Load(const unsigned int slot);
    static SaveStateReturn Export(const unsigned int slot);
    static SaveStateReturn Import(const unsigned int slot);
    static SaveStateReturn Delete(const unsigned int slot);

  private:
    static void SetHeader(SaveStateHeader& header);
    static void GetHeader(SaveStateHeader& header);
    SaveStateHeader stateHeader;
    unsigned char sysHeapCopy[1024 * 1024 * 4]; //TODO, make a macro for this
    SaveContext saveContextCopy;
    GameInfo gameInfoCopy;
    LightsBuffer lightBufferCopy;
    //AudioContext audioContextCopy;
    std::array<MtxF,20> mtxStackCopy; // always 20 matricies
    MtxF currentMtxCopy;
    uint32_t rngSeed;
    int16_t blueWarpTimerCopy; /* From door_warp_1 */
    
    
};

#endif
