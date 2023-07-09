#ifndef ACCESSIBLE_AUDIO_ENGINE_H
#define ACCESSIBLE_AUDIO_ENGINE_H
#include "miniaudio.h"
#include <stdint.h>
#include <thread>
#include <mutex>
#include <deque>
#include <string>
#include <unordered_map>
#include <array>
#define AAE_SOUND_ACTION_BATCH_SIZE 64
#define AAE_SLOTS_PER_HANDLE 10
class IResource;
struct DecodedSample {
    void* data;//A wav file.
    size_t dataSize;
};
struct SoundAction {
    uintptr_t handle; // This handle is user-defined and uniquely identifies a sound source. It can be anything, but the
                     // address of an object with which the sound is associated is recommended.
    int slot;//Allows multiple sounds per handle. The exact number is controlled by AAE_SOUNDS_PER_HANDLE.
    int command; // One of the items belonging to AAE_COMMANDS.
        std::string path; // If command is AAE_START, this is the path to the desired resource.
    bool looping;//If command is AAE_START, specifies whether or not the sound should loop.
    //A single float argument with several aliases depending on the command.
        union {
            float pitch;
            float volume;
            float pan;

            float distance;
        };

        //Position and rotation vectors for AAE_LISTENER and AAE_POS.
        float posX;
        float posY;
        float posZ;
        float rotX;
        float rotY;
        float rotZ;
        float velX;
        float velY;
        float velZ;
        uint32_t frames; // If command is AAE_PREPARE, this tells the engine how many PCM frames to get ready.
};
typedef struct
{
        ma_sound sound;
        bool active;
}SoundSlot;
typedef std::array<SoundSlot, AAE_SLOTS_PER_HANDLE> SoundSlots;

class AccessibleAudioEngine {
    int initialized;
    ma_resource_manager resourceManager;
    ma_engine engine;
    ma_pcm_rb preparedOutput;//Lock-free single producer single consumer.
    std::deque <SoundAction> soundActions;//A command cue.
    std::thread thread;
    std::condition_variable cv;
    std::mutex mtx;
    std::unordered_map<uintptr_t, SoundSlots> sounds;
    SoundAction outgoingSoundActions[AAE_SOUND_ACTION_BATCH_SIZE];//Allows batch delivery of SoundActions to the FIFO to minimize the amount of time spent locking and unlocking.
    int nextOutgoingSoundAction;
    int framesUntilGC;
    void destroy();//Called by the destructor, or if a throw occurs during construction.
    //Dismantal a partial initialization and throw an exception.
     void destroyAndThrow(const char* exceptionText);

    //Retrieve some audio from the output buffer. This is to be performed by the audio thread. May return less data than requested.
    uint32_t retrieve(float* buffer, uint32_t nFrames);
    //Functions dealing with the command queue.
    int getSoundActions(SoundAction* dest, int limit);
    void postSoundActions();
    void postHighPrioritySoundAction(SoundAction& action);//For now this is used only for termination events.

    SoundAction& getNextOutgoingSoundAction();
    void runThread();
//Find a sound by handle and slot, if it exists.
SoundSlot* findSound(SoundAction& action);

//Functions which correspond to SoundAction commands.
    //Ready a sound for playback.
    void doPlaySound(SoundAction& action);
    void doStopSound(SoundAction& action);
    void doStopAllSounds(SoundAction& action);

    void doSetPitch(SoundAction& action);
    void doSetVolume(SoundAction& action);
    void doSetPan(SoundAction& action);

    void doSetListenerPos(SoundAction& action);
    void doSetSoundPos(SoundAction& action);
    void doSetMaxDistance(SoundAction& actoun);




    // Generate some output, and store it in the output buffer for later retrieval. May generate less output than
    // requested if buffer space is insufficient.
    void doPrepare(SoundAction& action);
//Run every so often to clean up expired sound handles.
    void garbageCollect();
    //Run MiniAudio's jobs.
    void processAudioJobs();

  public:
    AccessibleAudioEngine();
    ~AccessibleAudioEngine();
        //Mix the game's audio with this engine's audio to produce the final mix. To be performed exclusively in the audio thread. Mixing is done in-place (meaning the buffer containing the game's audio is overwritten with the mixed content).
    void mix(int16_t* ogBuffer, uint32_t nFrames);
//Start playing a sound.
    void playSound(uintptr_t handle, int slot, const char* path, bool looping);
    void stopSound(uintptr_t handle, int slot);
    //Stop all sounds belonging to a handle.
    void stopAllSounds(uintptr_t handle);

    void setPitch(uintptr_t handle, int slot, float pitch);
    void setVolume(uintptr_t handle, int slot, float volume);
    void setPan(uintptr_t handle, int slot, float pan);

    void setListenerPosition(float posX, float posY, float posZ);
    void setSoundPosition(uintptr_t handle, int slot, float posX, float posY, float posZ);
    void setMaxDistance(uintptr_t handle, int slot, float distance);



    //Schedule the preparation of output for delivery.
    void prepare();
    void cacheDecodedSample(std::string& path, void* data, size_t size);

};
#endif
