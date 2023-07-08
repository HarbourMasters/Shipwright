#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_THREADING
#define MA_NO_DEVICE_IO
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define AAE_CHANNELS 2
#define AAE_SAMPLE_RATE 44100
#define AAE_MAX_BUFFER_SIZE AAE_SAMPLE_RATE / 10
#define AAE_PREP_CHUNK_SIZE 64
#define AAE_MIX_CHUNK_SIZE 64
#define AAE_GC_INTERVAL 20 * 60//How often, in frames, do we clean up sound handles that are no longer active.
#define NOMINMAX//because Windows is a joke.
#include "AccessibleAudioEngine.h"

extern "C" {
int AudioPlayer_GetDesiredBuffered();
}
#include <math.h>
#include <algorithm>
#include <stdexcept>
 enum AAE_COMMANDS{
AAE_START = 0,
AAE_STOP,
AAE_PREPARE,
AAE_TERMINATE,
};
void AccessibleAudioEngine::destroy() {
    switch (initialized){
    case 3:
            ma_engine_uninit(&engine);
        case 2:
            ma_pcm_rb_uninit(&preparedOutput);
        case 1:
            ma_resource_manager_uninit(&resourceManager);
     }
 }
void AccessibleAudioEngine::destroyAndThrow(const char* exceptionText) {
    destroy();

throw std::runtime_error(exceptionText);
}
    uint32_t AccessibleAudioEngine::retrieve(float* buffer, uint32_t nFrames) {
    uint32_t framesAvailable = ma_pcm_rb_available_read(&preparedOutput);
    if (nFrames > framesAvailable)
        nFrames = framesAvailable;
    if (nFrames == 0)
        return 0;
    uint32_t ogNFrames = nFrames;
    uint32_t framesObtained = 0;
    while (nFrames > 0) {
        void* readBuffer;
        framesObtained = nFrames;
        ma_pcm_rb_acquire_read(&preparedOutput, &framesObtained, (void**)&readBuffer);
        if (framesObtained > nFrames)
            framesObtained = nFrames;

        memcpy(buffer, readBuffer, sizeof(float) * framesObtained * AAE_CHANNELS);
        buffer += framesObtained * AAE_CHANNELS;
nFrames -= framesObtained;
        ma_pcm_rb_commit_read(&preparedOutput, framesObtained);

    }

    return ogNFrames;

}
void AccessibleAudioEngine::doPrepare(SoundAction& action)
{
    framesUntilGC--;
    int nFrames = ma_pcm_rb_available_write(&preparedOutput);
    if (nFrames <= 0)
        return;

    float* chunk;
    while (nFrames > 0) {
        //This should not loop more than twice.
        uint32_t nextChunk = nFrames;
        ma_pcm_rb_acquire_write(&preparedOutput, &nextChunk, (void**)&chunk);//Might reduce nextChunk if there isn't enough buffer space available to accommodate the request.
        uint64_t framesRead = 0;
        ma_engine_read_pcm_frames(&engine, chunk, nextChunk, &framesRead);
        //Even if we get fewer frames than expected, we should still submit a full buffer of silence.
        if (framesRead < nextChunk)
            ma_silence_pcm_frames(chunk + (framesRead * 2), (nextChunk - framesRead), ma_format_f32, 2);
        ma_pcm_rb_commit_write(&preparedOutput, (uint32_t) nextChunk);
        nFrames -= nextChunk;

    }

}
int AccessibleAudioEngine::getSoundActions(SoundAction* dest, int limit) {
    std::unique_lock<std::mutex> lock(mtx);
    while (soundActions.empty())
        cv.wait(lock);
    int actionsOut = 0;
    while (!soundActions.empty()) {
        dest[actionsOut] = soundActions.front();
        soundActions.pop_front();
        actionsOut++;
    }
    return actionsOut;

}
void AccessibleAudioEngine::postSoundActions(){
    {
        std::scoped_lock<std::mutex> lock(mtx);
        for (int i = 0; i < nextOutgoingSoundAction; i++)
            soundActions.push_back(outgoingSoundActions[i]);
    }
    cv.notify_one();
    nextOutgoingSoundAction = 0;
}
void AccessibleAudioEngine::postHighPrioritySoundAction(SoundAction& action) {
    std::scoped_lock<std::mutex> lock(mtx);
    soundActions.push_front(action);
}
    SoundAction& AccessibleAudioEngine::getNextOutgoingSoundAction() {
    if (nextOutgoingSoundAction >= AAE_SOUND_ACTION_BATCH_SIZE)
        postSoundActions();
    nextOutgoingSoundAction++;
    return outgoingSoundActions[nextOutgoingSoundAction - 1];
    }
    void AccessibleAudioEngine::runThread() {
    bool shouldTerminate = false;
        SoundAction incomingSoundActions[AAE_SOUND_ACTION_BATCH_SIZE];
        while (true) {
        processAudioJobs();
            if (framesUntilGC <= 0)
                garbageCollect();

            int batchSize = getSoundActions(incomingSoundActions, AAE_SOUND_ACTION_BATCH_SIZE);
            for (int i = 0; i < batchSize; i++) {
                SoundAction& action = incomingSoundActions[i];
                switch (action.command) {
                    case AAE_TERMINATE:
                        shouldTerminate = true;
                        break;
                    case AAE_START:
                        doPlaySound(action);
                        break;
                    case AAE_PREPARE:
                        doPrepare(action);
                        break;

                }
                if (shouldTerminate)
                    break;
            }
            if (shouldTerminate)
                break;

    }
}

    void AccessibleAudioEngine::doPlaySound(SoundAction& action) {
    ma_sound* sound;
    if (sounds.contains(action.handle)) {
        sound = &sounds[action.handle];
        ma_sound_uninit(sound);
        }
    else {
        ma_sound temp;
            sounds[action.handle] = temp;
        sound = &sounds[action.handle];
    }
        if (ma_sound_init_from_file(&engine, action.path.c_str(), 0, NULL, NULL, sound) != MA_SUCCESS)
            return;
        ma_sound_set_looping(sound, action.looping);
        ma_sound_start(sound);

    }
    void AccessibleAudioEngine::garbageCollect()
        {
        for (auto i = sounds.begin(); i != sounds.end(); ) {
            if (!ma_sound_is_playing(&i->second)) {
                ma_sound_uninit(&i->second);
                i = sounds.erase(i);
            }

            else
                i++;
        }
        framesUntilGC = AAE_GC_INTERVAL;
    }
    void AccessibleAudioEngine::processAudioJobs()
    {
        ma_job job;
        while (ma_resource_manager_next_job(&resourceManager, &job) == MA_SUCCESS)
            ma_job_process(&job);

    }
    AccessibleAudioEngine::AccessibleAudioEngine() {
        initialized = 0;
        ma_resource_manager_config rmc = ma_resource_manager_config_init();
        rmc.decodedChannels = AAE_CHANNELS;
        rmc.decodedFormat = ma_format_f32;
        rmc.decodedSampleRate = AAE_SAMPLE_RATE;
        rmc.flags = MA_RESOURCE_MANAGER_FLAG_NON_BLOCKING;
        rmc.jobThreadCount = 0;
        if (ma_resource_manager_init(&rmc, &resourceManager) != MA_SUCCESS)
            destroyAndThrow("AccessibleAudioEngine: Unable to initialize the resource manager.");
        initialized = 1;
        if (ma_pcm_rb_init(ma_format_f32, AAE_CHANNELS, AAE_MAX_BUFFER_SIZE, NULL, NULL, &preparedOutput) != MA_SUCCESS)
            destroyAndThrow("AccessibleAudioEngine: Unable to initialize the output buffer.");
        initialized = 2;
        ma_engine_config ec = ma_engine_config_init();
    ec.channels = AAE_CHANNELS;
    ec.noDevice = true;
    ec.sampleRate = AAE_SAMPLE_RATE;
    ec.pResourceManager = &resourceManager;
    if (ma_engine_init(&ec, &engine) != MA_SUCCESS)
        destroyAndThrow("AccessibleAudioEngine: Unable to initialize the audio engine.");
    initialized = 3;
    nextOutgoingSoundAction = 0;
    framesUntilGC = AAE_GC_INTERVAL;
    thread = std::thread(&AccessibleAudioEngine::runThread, this);

    }
    AccessibleAudioEngine::~AccessibleAudioEngine() {
        //Place a terminate command on the top of the pile, then wait for thread to die.
        SoundAction action;
        action.command = AAE_TERMINATE;
        postHighPrioritySoundAction(action);
        thread.join();
        destroy();

    }
        void AccessibleAudioEngine::mix(int16_t * ogBuffer, uint32_t nFrames) {
        uint32_t framesAvailable = ma_pcm_rb_available_read(&preparedOutput);
        float sourceChunk[AAE_MIX_CHUNK_SIZE * AAE_CHANNELS];
        float mixedChunk[AAE_MIX_CHUNK_SIZE * AAE_CHANNELS];
        while (nFrames > 0) {
            uint32_t nextChunk = std::min <uint32_t> (AAE_MIX_CHUNK_SIZE, nFrames);
            ma_silence_pcm_frames(sourceChunk, nextChunk, ma_format_f32, AAE_CHANNELS);//This is so that it doesn't matter if we have less output available than expected.
            ma_silence_pcm_frames(mixedChunk, nextChunk, ma_format_f32, AAE_CHANNELS);
            retrieve(sourceChunk, nextChunk);
            ma_pcm_s16_to_f32(mixedChunk, ogBuffer, nextChunk * AAE_CHANNELS, ma_dither_mode_none);//The game's output is changed to 32-bit floating point samples.
            ma_mix_pcm_frames_f32(mixedChunk, sourceChunk, nextChunk, AAE_CHANNELS, 1.0);
//If we've gone over 1.0, we'll need to scale back before we go back to 16-bit or we'll distort.
            float scalar = 1.0;
            for (int i = 0; i < nextChunk * AAE_CHANNELS; i++)
                scalar = std::max(scalar, mixedChunk[i]);
            if (scalar > 1.0) {
                scalar = 1.0 / scalar;
                for (int i = 0; i < nextChunk * AAE_CHANNELS; i++)
                    mixedChunk[i] *= scalar;
            }
            ma_pcm_f32_to_s16(ogBuffer, mixedChunk, nextChunk * AAE_CHANNELS, ma_dither_mode_triangle);//Chunk is ready to go out via the game's usual channels.
            ogBuffer += nextChunk * AAE_CHANNELS;
            nFrames -= nextChunk;
    }

    }
void AccessibleAudioEngine::playSound(uint64_t handle, const char* path, bool looping) {
        SoundAction& action = getNextOutgoingSoundAction();
    action.handle = handle;
        action.command = AAE_START;
    action.path = path;
        action.looping = looping;
    }
void AccessibleAudioEngine::prepare() {
        SoundAction& action = getNextOutgoingSoundAction();
        action.command = AAE_PREPARE;
//This is called once at the end of every frame, so now is the time to post all of the accumulated commands.
        postSoundActions();
    }
void AccessibleAudioEngine::cacheDecodedSample(std::string& path, void* data, size_t size) {
//The data is stored in wave format, so we register it with MiniAudio as an encoded asset as opposed to a decoded one.
        ma_resource_manager_register_encoded_data(&resourceManager, path.c_str(), data, size);
    }