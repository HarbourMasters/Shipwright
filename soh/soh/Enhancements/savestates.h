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
};

typedef struct SaveStateHeader {
    uint32_t stateVersion;
    uint32_t gameVersion;
} SaveStateHeader;

class SaveState {
  public:
    SaveState();
    ~SaveState() = delete;
    SaveState& operator=(const SaveState& rhs) = delete;
    SaveState(const SaveState& rhs) = delete;

    void Init(void);
    static SaveStateReturn Save(unsigned int slot);
    static SaveStateReturn Load(unsigned int slot);

  private:
    static void WriteHeader(SaveStateHeader& header);
    static void ReadHeader(SaveStateHeader& header);
    SaveStateHeader stateHeader;
    unsigned char sysHeapCopy[1024 * 1024 * 4]; //TODO, make a macro for this
    SaveContext saveContextCopy;
    GameInfo gameInfoCopy;
    LightsBuffer lightBufferCopy;
    //AudioContext audioContextCopy;
    std::array<MtxF,20> mtxStackCopy; // always 20 matricies
    MtxF currentMtxCopy;
    uint32_t rngSeed;
    
    
};

#endif
