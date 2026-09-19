#define AAE_MAX_BUFFER_SIZE AAE_SAMPLE_RATE / 10
#define AAE_PREP_CHUNK_SIZE 64
#define AAE_MIX_CHUNK_SIZE 64
#define AAE_GC_INTERVAL 20 * 60 // How often, in frames, do we clean up sound handles that are no longer active.
#define AAE_MAX_DB_REDUCTION -20
#define AAE_LPF_ORDER 4

#define NOMINMAX // because Windows is a joke.
#define MINIAUDIO_IMPLEMENTATION
#include "AccessibleAudioEngine.h"

extern "C" {
int AudioPlayer_GetDesiredBuffered();
}
#include <math.h>
#include <algorithm>
#include <stdexcept>

enum AAE_COMMANDS {
    AAE_START = 0,
    AAE_STOP,
    AAE_STOP_ALL,
    AAE_PITCH,
    AAE_PITCH_BEHIND, // Specify how much to change the pitch when the sound is behind the listener.
    AAE_VOLUME,
    AAE_PAN,
    AAE_FILTER,
    AAE_SEEK,
    AAE_POS,
    AAE_PREPARE,
    AAE_TERMINATE,
};
typedef int8_t s8;
typedef uint8_t u8;
// Processing for our custom audio positioning.
static float lerp_aae(float x, float y, float z) {
    return (1.0 - z) * x + z * y;
}

static float computeGain(SoundExtras* extras) {
    if (extras->maxDistance == 0)
        return 0;
    float leftover = ma_volume_db_to_linear(AAE_MAX_DB_REDUCTION);
    float normDist = fabs(extras->distToPlayer) / extras->maxDistance;
    float db = lerp_aae(0, AAE_MAX_DB_REDUCTION, normDist);
    float gain = ma_volume_db_to_linear(db);
    gain -= lerp_aae(0, leftover, normDist);
    return gain;
}
// Borrow pan calculation from game itself. Todo: this is technical debt, so copy/revise or something
extern "C" int8_t Audio_ComputeSoundPanSigned(float x, float z, uint8_t token);
static void positioner_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn,
                                          float** ppFramesOut, ma_uint32* pFrameCountOut) {

    const float* framesIn = ppFramesIn[0];
    float* framesOut = ppFramesOut[0];
    ma_copy_pcm_frames(framesOut, framesIn, *pFrameCountIn, ma_format_f32, 2);
    *pFrameCountOut = *pFrameCountIn;
    SoundExtras* extras = (SoundExtras*)pNode;
    // Pan the sound based on its projected position.
    float pan;
    // Use the game's panning mechanism, which returns a signed 8-bit integer between 0 (far-left) and 127 (far-right).
    // It would appear that the correct thing to do is interpret this value as a gain factor in decibels. In practice,
    // values below 38 or above 90 are never seen, so a sound that's panned far to one side or the other amounts to
    // about -25DB worth of attenuation. Also: lie about the value of Z and give it a constant value to prevent weird
    // behaviour when Z is far away.
    s8 panSigned = Audio_ComputeSoundPanSigned(extras->x, extras->z, 4);
    int db;
    if (panSigned < 64)
        db = 64 - panSigned;
    else
        db = panSigned - 64;
    pan = 1.0 - fabs(ma_volume_db_to_linear(-db / 2));
    if (panSigned < 64)
        pan = -pan;

    ma_panner_set_pan(&extras->panner, pan);
    ma_panner_process_pcm_frames(&extras->panner, framesOut, framesOut, *pFrameCountIn);
    // Next we'll apply the gain based on the object's distance relationship to the player. The strategy here is to use
    // a combination of decibel-based and linear attenuation, so that the gain reaches 0 at the exact point when the
    // object is at exactly the maximum distance from the player.

    float gain = computeGain(extras);
    ma_gainer_set_gain(&extras->gainer, gain);
    ma_gainer_process_pcm_frames(&extras->gainer, framesOut, framesOut, *pFrameCountIn);
    // Run LPF only when necessary because we can't afford to run a 4th-order lowpass on every single sound. This
    // probably causes minor glitches when the filter switches on and off. Todo: cross that bridge.
    if (extras->cutoff != 1.0)
        ma_lpf_process_pcm_frames(&extras->filter, framesOut, framesOut, *pFrameCountIn);
}

static ma_node_vtable positioner_vtable = { positioner_process_pcm_frames, NULL, 1, 1, 0 };
static ma_uint32 positioner_channels[1] = { 2 };

void AccessibleAudioEngine::destroy() {
    switch (initialized) {
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
    while (nFrames > 0) {
        void* readBuffer;
        uint32_t framesObtained = nFrames;
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

void AccessibleAudioEngine::doPrepare(SoundAction& action) {
    framesUntilGC--;
    int nFrames = ma_pcm_rb_available_write(&preparedOutput);
    if (nFrames <= 0)
        return;

    float* chunk;
    while (nFrames > 0) {
        // This should not loop more than twice.
        uint32_t nextChunk = nFrames;
        ma_pcm_rb_acquire_write(&preparedOutput, &nextChunk,
                                (void**)&chunk); // Might reduce nextChunk if there isn't enough buffer space available
                                                 // to accommodate the request.
        ma_uint64 framesRead = 0;
        ma_engine_read_pcm_frames(&engine, chunk, nextChunk, &framesRead);
        // Even if we get fewer frames than expected, we should still submit a full buffer of silence.
        if (framesRead < nextChunk)
            ma_silence_pcm_frames(chunk + (framesRead * 2), (nextChunk - framesRead), ma_format_f32, 2);
        ma_pcm_rb_commit_write(&preparedOutput, nextChunk);
        nFrames -= nextChunk;
    }
}
int AccessibleAudioEngine::getSoundActions(SoundAction* dest, int limit) {
    std::unique_lock<std::mutex> lock(mtx);
    while (soundActions.empty())
        cv.wait(lock);
    int actionsOut = 0;
    while (!soundActions.empty() && limit > 0) {
        dest[actionsOut] = soundActions.front();
        soundActions.pop_front();
        actionsOut++;
        limit--;
    }
    return actionsOut;
}
void AccessibleAudioEngine::postSoundActions() {
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
    cv.notify_one();
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
                    return;
                case AAE_START:
                    doPlaySound(action);
                    break;
                case AAE_STOP:
                    doStopSound(action);
                    break;
                case AAE_STOP_ALL:
                    doStopAllSounds(action);
                    break;
                case AAE_PITCH:
                    doSetPitch(action);
                    break;
                case AAE_PITCH_BEHIND:
                    doSetPitchBehindModifier(action);
                    break;
                case AAE_VOLUME:
                    doSetVolume(action);
                    break;
                case AAE_PAN:
                    doSetPan(action);
                    break;
                case AAE_FILTER:
                    doSetFilter(action);
                    break;
                case AAE_SEEK:
                    doSeekSound(action);
                    break;
                case AAE_POS:
                    doSetSoundPos(action);
                    break;
                case AAE_PREPARE:
                    doPrepare(action);
                    break;
            }
        }
    }
}

SoundSlot* AccessibleAudioEngine::findSound(SoundAction& action) {
    if (action.slot >= AAE_SLOTS_PER_HANDLE)
        return NULL;
    auto i = sounds.find(action.handle);
    if (i == sounds.end())
        return NULL;
    SoundSlot& target = i->second[action.slot];
    if (!target.active)
        return NULL;
    return &target;
}

void AccessibleAudioEngine::doPlaySound(SoundAction& action) {
    SoundSlot* sound;
    if (sounds.contains(action.handle)) {
        sound = &sounds[action.handle][action.slot];
        if (sound->active) {
            ma_sound_stop(&sound->sound);
            destroySound(sound);
        }
    } else {
        SoundSlots temp;
        for (int i = 0; i < AAE_SLOTS_PER_HANDLE; i++)
            temp[i].active = false;

        sounds[action.handle] = temp;
        sound = &sounds[action.handle][action.slot];
    }

    ma_result result = ma_sound_init_from_file(&engine, action.path.c_str(),
                                               MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT,
                                               NULL, NULL, &sound->sound);
    if (result != MA_SUCCESS) {
        return;
    }

    if (action.handle != 0) {
        initSoundExtras(sound);
        // We actually attach the extras to the engine, not the sound itself.
        ma_node_attach_output_bus(&sound->extras, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0);
    } else {
        sound->extras.base.pNodeGraph = NULL;
        ma_node_attach_output_bus(&sound->sound, 0, ma_node_graph_get_endpoint(&engine.nodeGraph), 0);
    }

    ma_sound_start(&sound->sound);

    sound->active = true;
}

void AccessibleAudioEngine::doStopSound(SoundAction& action) {
    SoundSlot* slot = findSound(action);
    if (slot == NULL)
        return;
    destroySound(slot);
}

void AccessibleAudioEngine::doStopAllSounds(SoundAction& action) {
    auto it = sounds.find(action.handle);
    if (it == sounds.end())
        return;
    SoundSlots& slots = it->second;
    for (int i = 0; i < AAE_SLOTS_PER_HANDLE; i++) {
        if (slots[i].active)
            destroySound(&slots[i]);
    }
    sounds.erase(it);
}

void AccessibleAudioEngine::doSetPitch(SoundAction& action) {
    SoundSlot* slot = findSound(action);
    if (slot == NULL)
        return;
    slot->extras.pitch = action.pitch;
    float pitch = action.pitch;
    if (slot->extras.z < 0)
        pitch *= (1.0 - slot->extras.pitchBehindModifier);
    ma_sound_set_pitch(&slot->sound, pitch);
}

void AccessibleAudioEngine::doSetPitchBehindModifier(SoundAction& action) {
    SoundSlot* slot = findSound(action);
    if (slot == NULL)
        return;
    slot->extras.pitchBehindModifier = action.pitch;
}

void AccessibleAudioEngine::doSetVolume(SoundAction& action) {
    SoundSlot* slot = findSound(action);
    if (slot == NULL)
        return;
    ma_sound_set_volume(&slot->sound, action.pitch);
}

void AccessibleAudioEngine::doSetPan(SoundAction& action) {
    SoundSlot* slot = findSound(action);
    if (slot == NULL)
        return;
    ma_sound_set_pan(&slot->sound, action.pan);
}

void AccessibleAudioEngine::doSetFilter(SoundAction& action) {
    SoundSlot* slot = findSound(action);
    if (slot == NULL)
        return;
    slot->extras.cutoff = action.cutoff;
    ma_lpf_config config = ma_lpf_config_init(ma_format_f32, AAE_CHANNELS, AAE_SAMPLE_RATE,
                                              lerp_aae(0.0, AAE_SAMPLE_RATE / 2, action.cutoff), AAE_LPF_ORDER);
    ma_lpf_reinit(&config, &slot->extras.filter);
}

void AccessibleAudioEngine::doSeekSound(SoundAction& action) {
    SoundSlot* slot = findSound(action);
    if (slot == NULL)
        return;
    ma_sound_seek_to_pcm_frame(&slot->sound, action.offset);
}

void AccessibleAudioEngine::doSetSoundPos(SoundAction& action) {
    SoundSlot* slot = findSound(action);
    if (slot == NULL)
        return;
    slot->extras.x = action.posX;
    slot->extras.y = action.posY;
    slot->extras.z = action.posZ;
    slot->extras.distToPlayer = action.distToPlayer;
    slot->extras.maxDistance = action.maxDistance;
    float pitch = slot->extras.pitch;
    if (action.posZ < 0)
        pitch *= (1.0 - slot->extras.pitchBehindModifier);
    ma_sound_set_pitch(&slot->sound, pitch);
}
void AccessibleAudioEngine::garbageCollect() {
    for (auto i = sounds.begin(); i != sounds.end();) {
        bool deadSlots = true;
        for (int x = 0; x < AAE_SLOTS_PER_HANDLE; x++) {
            if (i->second[x].active) {
                if (!ma_sound_is_playing(&i->second[x].sound)) {
                    destroySound(&i->second[x]);
                } else {
                    deadSlots = false;
                }
            }
        }
        if (deadSlots) {
            i = sounds.erase(i);
        } else {
            i++;
        }
    }
    framesUntilGC = AAE_GC_INTERVAL;
}

void AccessibleAudioEngine::processAudioJobs() {
    ma_job job;
    while (ma_resource_manager_next_job(&resourceManager, &job) == MA_SUCCESS)
        ma_job_process(&job);
}

bool AccessibleAudioEngine::initSoundExtras(SoundSlot* slot) {
    ma_node_config config = ma_node_config_init();
    config.inputBusCount = 1;
    config.outputBusCount = 1;
    config.pInputChannels = positioner_channels;
    config.pOutputChannels = positioner_channels;
    config.vtable = &positioner_vtable;
    memset(&slot->extras, 0, sizeof(SoundExtras));
    if (ma_node_init(&engine.nodeGraph, &config, NULL, &slot->extras) != MA_SUCCESS)
        return false;
    ma_panner_config pc = ma_panner_config_init(ma_format_f32, AAE_CHANNELS);
    pc.mode = ma_pan_mode_balance;
    ma_panner_init(&pc, &slot->extras.panner);
    ma_gainer_config gc = ma_gainer_config_init(
        AAE_CHANNELS,
        AAE_SAMPLE_RATE / 20); // Allow one in-game frame for the gain to work its way towards the target value.
    if (ma_gainer_init(&gc, NULL, &slot->extras.gainer) != MA_SUCCESS)
        return false;
    ma_lpf_config fc =
        ma_lpf_config_init(ma_format_f32, AAE_CHANNELS, AAE_SAMPLE_RATE, AAE_SAMPLE_RATE / 2, AAE_LPF_ORDER);
    ma_lpf_init(&fc, NULL, &slot->extras.filter);
    slot->extras.cutoff = 1.0f;
    slot->extras.pitch = 1.0f;
    slot->extras.pitchBehindModifier = 0.0f;
    ma_node_attach_output_bus(&slot->sound, 0, &slot->extras, 0);
    return true;
}

void AccessibleAudioEngine::destroySound(SoundSlot* slot) {
    if (slot->extras.base.pNodeGraph != NULL) {
        ma_node_detach_all_output_buses(&slot->extras);
        ma_sound_uninit(&slot->sound);
        ma_gainer_uninit(&slot->extras.gainer, NULL);
    } else {
        ma_sound_uninit(&slot->sound);
    }

    slot->active = false;
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
    ec.listenerCount = 1;

    if (ma_engine_init(&ec, &engine) != MA_SUCCESS)
        destroyAndThrow("AccessibleAudioEngine: Unable to initialize the audio engine.");
    initialized = 3;
    nextOutgoingSoundAction = 0;
    framesUntilGC = AAE_GC_INTERVAL;
    thread = std::thread(&AccessibleAudioEngine::runThread, this);
}

AccessibleAudioEngine::~AccessibleAudioEngine() {
    // Place a terminate command on the top of the pile, then wait for thread to die.
    SoundAction action;
    action.command = AAE_TERMINATE;
    postHighPrioritySoundAction(action);
    thread.join();
    destroy();
}

void AccessibleAudioEngine::mix(int16_t* ogBuffer, uint32_t nFrames) {
    float sourceChunk[AAE_MIX_CHUNK_SIZE * AAE_CHANNELS];
    float mixedChunk[AAE_MIX_CHUNK_SIZE * AAE_CHANNELS];
    while (nFrames > 0) {
        uint32_t nextChunk = std::min<uint32_t>(AAE_MIX_CHUNK_SIZE, nFrames);
        // This is so that it doesn't matter if we have less output available than expected.
        ma_silence_pcm_frames(sourceChunk, nextChunk, ma_format_f32, AAE_CHANNELS);
        ma_silence_pcm_frames(mixedChunk, nextChunk, ma_format_f32, AAE_CHANNELS);
        retrieve(sourceChunk, nextChunk);
        // The game's output is changed to 32-bit floating point samples.
        ma_pcm_s16_to_f32(mixedChunk, ogBuffer, nextChunk * AAE_CHANNELS, ma_dither_mode_none);
        ma_mix_pcm_frames_f32(mixedChunk, sourceChunk, nextChunk, AAE_CHANNELS, 1.0);
        // If we've gone over 1.0, we'll need to scale back before we go back to 16-bit or we'll distort.
        float scalar = 1.0;
        for (uint32_t i = 0; i < nextChunk * AAE_CHANNELS; i++)
            scalar = std::max(scalar, mixedChunk[i]);
        if (scalar > 1.0) {
            scalar = 1.0 / scalar;
            for (uint32_t i = 0; i < nextChunk * AAE_CHANNELS; i++)
                mixedChunk[i] *= scalar;
        }
        // Chunk is ready to go out via the game's usual channels
        ma_pcm_f32_to_s16(ogBuffer, mixedChunk, nextChunk * AAE_CHANNELS, ma_dither_mode_triangle);
        ogBuffer += nextChunk * AAE_CHANNELS;
        nFrames -= nextChunk;
    }
}

void AccessibleAudioEngine::playSound(uintptr_t handle, uint8_t slot, const char* path) {
    if (slot >= AAE_SLOTS_PER_HANDLE)
        return;
    SoundAction& action = getNextOutgoingSoundAction();
    action.handle = handle;
    action.slot = slot;
    action.command = AAE_START;
    action.path = path;
}

void AccessibleAudioEngine::stopSound(uintptr_t handle, uint8_t slot) {
    if (slot >= AAE_SLOTS_PER_HANDLE)
        return;
    SoundAction& action = getNextOutgoingSoundAction();
    action.command = AAE_STOP;
    action.handle = (uintptr_t)handle;
    action.slot = slot;
}

void AccessibleAudioEngine::stopAllSounds(uintptr_t handle) {
    SoundAction& action = getNextOutgoingSoundAction();
    action.command = AAE_STOP_ALL;
    action.handle = handle;
}

void AccessibleAudioEngine::setPitch(uintptr_t handle, uint8_t slot, float pitch) {
    if (slot >= AAE_SLOTS_PER_HANDLE)
        return;
    SoundAction& action = getNextOutgoingSoundAction();
    action.command = AAE_PITCH;
    action.handle = handle;
    action.slot = slot;
    action.pitch = pitch;
}

void AccessibleAudioEngine::setPitchBehindModifier(uintptr_t handle, uint8_t slot, float mod) {
    if (slot >= AAE_SLOTS_PER_HANDLE)
        return;
    SoundAction& action = getNextOutgoingSoundAction();
    action.command = AAE_PITCH_BEHIND;
    action.handle = handle;
    action.slot = slot;
    action.pitch = mod;
}

void AccessibleAudioEngine::setVolume(uintptr_t handle, uint8_t slot, float volume) {
    if (slot >= AAE_SLOTS_PER_HANDLE)
        return;
    SoundAction& action = getNextOutgoingSoundAction();
    action.command = AAE_VOLUME;
    action.handle = handle;
    action.slot = slot;
    action.volume = volume;
}

void AccessibleAudioEngine::setPan(uintptr_t handle, uint8_t slot, float pan) {
    if (slot >= AAE_SLOTS_PER_HANDLE)
        return;
    SoundAction& action = getNextOutgoingSoundAction();
    action.command = AAE_PAN;
    action.handle = handle;
    action.slot = slot;
    action.pan = pan;
}
void AccessibleAudioEngine::setFilter(uintptr_t handle, uint8_t slot, float cutoff) {
    if (slot >= AAE_SLOTS_PER_HANDLE)
        return;
    if (cutoff < 0.0 || cutoff > 1.0)
        return;
    SoundAction& action = getNextOutgoingSoundAction();
    action.handle = handle;
    action.slot = slot;
    action.command = AAE_FILTER;
    action.cutoff = cutoff;
}

void AccessibleAudioEngine::seekSound(uintptr_t handle, uint8_t slot, size_t offset) {
    if (slot >= AAE_SLOTS_PER_HANDLE)
        return;
    SoundAction& action = getNextOutgoingSoundAction();
    action.handle = handle;
    action.slot = slot;
    action.command = AAE_SEEK;
    action.offset = offset;
}

void AccessibleAudioEngine::setSoundPosition(uintptr_t handle, uint8_t slot, float posX, float posY, float posZ,
                                             float distToPlayer, float maxDistance) {
    if (slot >= AAE_SLOTS_PER_HANDLE)
        return;
    SoundAction& action = getNextOutgoingSoundAction();
    action.command = AAE_POS;
    action.handle = handle;
    action.slot = slot;
    action.posX = posX;
    action.posY = posY;
    action.posZ = posZ;
    action.distToPlayer = distToPlayer;
    action.maxDistance = maxDistance;
}

void AccessibleAudioEngine::prepare() {
    SoundAction& action = getNextOutgoingSoundAction();
    action.command = AAE_PREPARE;
    // This is called once at the end of every frame, so now is the time to post all of the accumulated commands.
    postSoundActions();
}