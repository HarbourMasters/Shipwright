#ifndef SAVE_STATES_H
#define SAVE_STATES_H

#include <cstdint>
#include <queue>
#include <unordered_map>
#include <memory>
#include <mutex>

enum class SaveStateReturn {
    SUCCESS,
    FAIL_INVALID_SLOT,
    FAIL_NO_MEMORY,
    FAIL_STATE_EMPTY,
    FAIL_WRONG_GAMESTATE,
    FAIL_BAD_REQUEST,
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

typedef struct SaveStateRequest {
    unsigned int slot;
    RequestType type;
} SaveStateRequest;

class SaveState;

class SaveStateMgr {
    friend class SaveState;
  private:
    unsigned int currentSlot;
    std::unordered_map<unsigned int, std::shared_ptr<SaveState>> states;
    std::queue <SaveStateRequest> requests;
    std::mutex mutex;
    
  public:

    SaveStateReturn AddRequest(const SaveStateRequest request);
    SaveStateMgr();
    ~SaveStateMgr();

    void SetCurrentSlot(unsigned int slot);
    unsigned int GetCurrentSlot(void);
    
    SaveStateMgr& operator=(const SaveStateMgr& rhs) = delete;
    SaveStateMgr(const SaveStateMgr& rhs) = delete;

    void ProcessSaveStateRequests(void);
    
};
extern std::shared_ptr<SaveStateMgr> gSaveStateMgr;

#endif
