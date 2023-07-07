#include "ActorAccessibility.h"
#include "AccessibleAudioEngine.h"
#include "libultraship/libultraship.h"
#include "soh/Enhancements/audio/AudioDecoder.h"
#include "soh/resource/type/AudioSample.h"
#include "soh/OTRGlobals.h"
#include "dr_libs/wav.h"
#include "miniaudio.h"
#include "soh/Enhancements/speechsynthesizer/SpeechSynthesizer.h"
#include "soh/Enhancements/tts/tts.h"

const char* GetLanguageCode();

#include <map>
#include <random>
#include <vector>
#include <functions.h>
#include <macros.h>
#include "ResourceType.h"
#include <sstream>
#define MAX_DB_REDUCTION 20// This is the amount in DB that a sound will be reduced by when it is at the maximum distance
                        // from the player.
extern "C" {
extern Vec3f D_801333D4;
extern f32 D_801333E0;
extern s8 D_801333E8;
extern u8 D_801333F0;
void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);
}

typedef struct {
    union {
        struct {
            s16 sceneIndex; // Corresponds directly to the game's scene indices.
            s16 roomIndex;  // Corresponds directly to the game's room indices.
        } values;
        s32 raw; // Combination of the two which can be used for dictionary lookups.
    };

    } SceneAndRoom;
    typedef std::map<s16, ActorAccessibilityPolicy> SupportedActors_t;//Maps actors to their accessibility policies, which describe how accessibility should treat them.
typedef std::map<Actor*, uint64_t> TrackedActors_t;//Maps real actors to internal IDs specific to accessibility.
typedef std::map<uint64_t, AccessibleActor> AccessibleActorList_t;//Maps internal IDs to wrapped actor objects. These actors can be real or virtual.
typedef std::vector<AccessibleActor> VAList_t;//Denotes a list of virtual actors specific to a single room.
typedef std::map<s32, VAList_t> VAZones_t;//Maps room/ scene indices to their corresponding virtual actor collections.
enum
{
STEP_SETUP,
STEP_MAIN,
STEP_FINISHING,
STEP_FINISHED,
STEP_ERROR,
}SFX_EXTRACTION_STEPS;
enum
{
CT_WAITING,//for a sound to start ripping.
CT_PRIMING,
CT_READY,//to start ripping a sound.
CT_FINISHED,//ripping the current sound.
CT_SHUTDOWN,
}CAPTURE_THREAD_STATES;
#define SFX_EXTRACTION_BUFFER_SIZE 44100 * 15
#define SFX_EXTRACTION_ONE_FRAME 736
class SoundExtractionState
{
    std::shared_ptr<LUS::Archive> archive;
    int currentStep;
    int captureThreadState;
    std::queue<s16> sfxToRip;
    size_t startOfInput;
    size_t endOfInput;
    s16 currentSfx;
    std::vector < int16_t> tempStorage;//Stores raw audio data for the sfx currently being ripped.
    int16_t* tempBuffer;//Raw pointer to the above vector.
    int retrievalAttempts;//Counts failures. If the game doesn't render nonzero samples after being asked to play a sound, we count it as an attempt. If we still don't have output after five attempts, we'll skip the sound.
    int progressMilestones[9];//Implements progress reports after every 10 percent.
    //Check if a buffer contains meaningful audio output.
    bool isAllZero(int16_t* buffer, size_t count)
    {
        for (auto i = 0; i < count; i++)
        {
            if (buffer[i] != 0)
                return false;

        }
        return true;

    }
        //Find the beginning of a captured signal.
    void setStartOfInput()
    {
        startOfInput = 0;
        for (startOfInput = 0; startOfInput < SFX_EXTRACTION_BUFFER_SIZE * 2; startOfInput += 2)
        {
            if (tempBuffer[startOfInput] != 0 || tempBuffer[startOfInput + 1] != 0)
                break;
        }
    }
    void setEndOfInput()
    {
        while (endOfInput > 0)
        {
            if (tempBuffer[endOfInput] != 0 || tempBuffer[endOfInput - 1] != 0)
                break;
            endOfInput -= 2;

        }
    }
    void renderOutput()
    {

        ma_channel_converter_config config =
            ma_channel_converter_config_init(ma_format_s16, 2, NULL, 1, NULL, ma_channel_mix_mode_default);
        ma_channel_converter converter;
        if (ma_channel_converter_init(&config, NULL, &converter) != MA_SUCCESS)
            throw std::runtime_error("SfxExtractor: Unable to initialize channel converter.");
        drwav_data_format format;
        format.bitsPerSample = 16;
        format.channels = 1;
        format.container = drwav_container_riff;
        format.format = DR_WAVE_FORMAT_PCM;
        format.sampleRate = 44100;
        drwav wav;
        std::string fileName = getSfxFileName();
        void* mem = NULL;
        size_t size = 0;

        if (!drwav_init_memory_write(&wav, &mem, &size, &format, NULL))
            throw std::runtime_error("SFX Extractor: Unable to initialize wave writer.");
        int16_t chunk[64];
        int16_t* mark = tempBuffer + startOfInput;
        size_t samplesLeft = endOfInput - startOfInput;
        while (samplesLeft > 0)
        {
            size_t thisChunk = std::min<size_t>(64, samplesLeft);
            ma_channel_converter_process_pcm_frames(&converter, chunk, mark, thisChunk / 2);
            drwav_write_pcm_frames(&wav, thisChunk / 2, chunk);
            samplesLeft -= thisChunk;
            mark += thisChunk;

        }
        drwav_uninit(&wav);
        archive->AddFile(fileName.c_str(), (uintptr_t)mem, size);
        drwav_free(mem, NULL);
    }
    void setup() {
        try {
            SpeechSynthesizer::Instance->Speak("Welcome to the Ship of Harkinian accessible asset extraction program. "
                                               "Today, we're going to speedrun the "
                                               "process of capturing all of the sounds in the game and preparing them "
                                               "for use as accessibility tools. I "
                                               "expect this process to take about 10 minutes to complete. You'll be "
                                               "treated to progress reports along the "
                                               "way. You can cancel this process by closing the game. Please stand by.",
                                               GetLanguageCode());
            // Kill the audio thread so we can take control.
            captureThreadState = CT_WAITING;
            OTRAudio_InstallSfxCaptureThread();
        tempStorage.resize ((SFX_EXTRACTION_BUFFER_SIZE + (SFX_EXTRACTION_ONE_FRAME * 3)) *
2, 0); // Over-allocated just a tad because otherwise we'll overrun if the last frame is short.
        tempBuffer = tempStorage.data();

        // Build are master sfx extraction todo list.
        for (s16 i = NA_SE_PL_WALK_GROUND; i <= NA_SE_PL_YOBI_DATA14; i++)
                sfxToRip.push(i);
        for (s16 i = NA_SE_IT_SWORD_IMPACT; i <= NA_SE_IT_YOBI18; i++)
                sfxToRip.push(i);
        for (s16 i = NA_SE_EV_DOOR_OPEN; i <= NA_SE_EV_YOBI25; i++)
                sfxToRip.push(i);
        for (s16 i = NA_SE_EN_FLOORMASTER_SLIDING; i <= NA_SE_EN_YOBI30; i++)
                sfxToRip.push(i);
        for (s16 i = NA_SE_SY_WIN_OPEN; i <= NA_SE_SY_YOBI17; i++)
                sfxToRip.push(i);
        for (s16 i = NA_SE_OC_OCARINA; i <= NA_SE_OC_HIBIKI_ISHI; i++)
                sfxToRip.push(i);
        for (s16 i = NA_SE_VO_LI_SWORD_N; i <= NA_SE_VO_DUMMY_0x89_YOBI; i++) {
                if (i == NA_SE_VO_DUMMY_0x7d)
                    continue;//This one will crash the game if you try to play it because it tries to load a sample which has missing data.
                sfxToRip.push(i);
        }

        currentStep = STEP_MAIN;
        for (int i = 1; i < 10; i++)
                progressMilestones[i - 1] = sfxToRip.size() - ((int) ceil(sfxToRip.size() * (i / 10.0f)));
        archive = LUS::Archive::CreateArchive("accessibility.otr", sfxToRip.size());
        }
        catch (...)
        {

}
    }

    void ripNextSfx() {
        {
            auto lock = OTRAudio_Lock();
            if (captureThreadState == CT_READY || captureThreadState == CT_PRIMING)
                return;//Keep going.
        }
//Was the last sfx a loop? If so then we need to stop it, and then we need to run audio out to nowhere for as long as it takes to get back to a blank slate.
        if (Audio_IsSfxPlaying(currentSfx)) {
            Audio_StopSfxById(currentSfx);
            captureThreadState = CT_PRIMING;
            return;
        }
        if (sfxToRip.empty()) {
            currentStep = STEP_FINISHING; // Caught 'em all!
            return;
        }

            currentSfx = sfxToRip.front();
            sfxToRip.pop();
            startOfInput = 0;
            endOfInput = 0;
            retrievalAttempts = 0;
            Audio_PlaySoundGeneral(currentSfx, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            {
                auto lock = OTRAudio_Lock();
            captureThreadState = CT_READY;

            }
            maybeGiveProgressReport();

    }
    void finished()
    {

    }
    std::string getSfxFileName() {
            std::stringstream ss;
            ss << "accessibility/audio/";
            ss << std::hex << std::setw(4) << std::setfill('0') << currentSfx;
            return ss.str();

    }
    void maybeGiveProgressReport()
    {
            size_t ripsRemaining = sfxToRip.size() + 1;
            for (int i = 0; i < 9; i++)
            {
                if (ripsRemaining == progressMilestones [i])
                {
                int percentDone = (i + 1) * 10;
                std::stringstream ss;
                ss << percentDone << " percent complete.";
                SpeechSynthesizer::Instance->Speak(ss.str().c_str(), GetLanguageCode());

                }
            }
    }
  public:
      SoundExtractionState()
      {
        currentStep = STEP_SETUP;
    }
      void run() {
        switch (currentStep) {
                case STEP_SETUP:
                setup();
                break;
                case STEP_MAIN:
                ripNextSfx();
                break;
                case STEP_FINISHED:
                finished();
                break;
        }
      }

          void prime()
          {
        while (true) {
                AudioMgr_CreateNextAudioBuffer(tempBuffer, SFX_EXTRACTION_ONE_FRAME);
                if (isAllZero(tempBuffer, SFX_EXTRACTION_ONE_FRAME * 2))
                break;
        }
        captureThreadState = CT_FINISHED;
    }
//The below is called by the (hijacked) audio thread.
    void runCaptureThread() {
        if (captureThreadState == CT_PRIMING)
                prime();
        if (captureThreadState != CT_READY)
            return; // No work to do at the moment.
        memset(tempBuffer, 0, SFX_EXTRACTION_BUFFER_SIZE * 4);
        int16_t* mark = tempBuffer;
        size_t samplesLeft = SFX_EXTRACTION_BUFFER_SIZE;
        bool outputStarted = false;
        endOfInput = 0;

        while (samplesLeft > 0) {
            AudioMgr_CreateNextAudioBuffer(mark, SFX_EXTRACTION_ONE_FRAME);

            if (!outputStarted && isAllZero(mark, SFX_EXTRACTION_ONE_FRAME * 2)) {
                retrievalAttempts++;
                if (retrievalAttempts < 5)
                    continue;                       // Game is not ready to deliver, try again.
                captureThreadState = CT_FINISHED; // Sound is unavailable, so skip over it and move on.
                return;
            }

            else
                outputStarted = true;
            if (isAllZero(mark, SFX_EXTRACTION_ONE_FRAME * 2))
                break;//End of sound.
            mark += (SFX_EXTRACTION_ONE_FRAME * 2);
            endOfInput += (SFX_EXTRACTION_ONE_FRAME * 2);
            samplesLeft -= std::min<size_t>(SFX_EXTRACTION_ONE_FRAME, samplesLeft);

        }
        setStartOfInput();
        setEndOfInput();
        renderOutput();
        if (!sfxToRip.empty())
        {
            ripNextSfx();

        }
        //captureThreadState = CT_FINISHED; //Go to the next one.
    }

      };
class ActorAccessibility {
  public:
      bool isOn = false;
    uint64_t nextActorID;
    SupportedActors_t supportedActors;
    TrackedActors_t trackedActors;
    AccessibleActorList_t accessibleActorList;
    VAZones_t vaZones;
    AccessibleAudioEngine* audioEngine;
    SoundExtractionState soundExtractionState;

};
static ActorAccessibility* aa;

uint64_t ActorAccessibility_GetNextID() {
    uint64_t result = aa->nextActorID;
    aa->nextActorID++;
    return result;
}
void ActorAccessibility_Init() {
    aa = new ActorAccessibility();
    ActorAccessibility_InitAudio();
    ActorAccessibility_InitActors();

}
void ActorAccessibility_Shutdown() {
    ActorAccessibility_ShutdownAudio();
    delete aa;
}
    void ActorAccessibility_InitPolicy(ActorAccessibilityPolicy * policy, const char* englishName,
                                       ActorAccessibilityCallback callback, s16 sfx) {
    policy->callback = callback;
    policy->distance = 500;
    policy->englishName = englishName;
    policy->n = 20;
    policy->pitch = 1.5;
    policy->runsAlways = false;
    policy->sound = sfx;
    policy->volume = 1.0;
    policy->initUserData = NULL;
    policy->cleanupUserData = NULL;

}

    void ActorAccessibility_AddSupportedActor(s16 type, ActorAccessibilityPolicy policy) {
    aa->supportedActors[type] = policy;

}

ActorAccessibilityPolicy* ActorAccessibility_GetPolicyForActor(s16 type) {
    SupportedActors_t::iterator i = aa->supportedActors.find(type);
     if (i == aa->supportedActors.end())
         return NULL;
     return &(i->second);

 }
int ActorAccessibility_GetRandomStartingFrameCount(int min, int max) {
     static std::mt19937 gen;
    std::uniform_int_distribution<> dist(min, max);
     return dist(gen);

 }

void ActorAccessibility_TrackNewActor(Actor* actor) {
        // Don't track actors for which no accessibility policy has been configured.
        ActorAccessibilityPolicy* policy = ActorAccessibility_GetPolicyForActor(actor->id);
        if (policy == NULL)
            return;
        AccessibleActor accessibleActor;
        accessibleActor.instanceID = ActorAccessibility_GetNextID();
        accessibleActor.actor = actor;
        accessibleActor.id = actor->id;
        //Stagger the start times so that all of the sounds don't play at exactly the same time.
        accessibleActor.frameCount = ActorAccessibility_GetRandomStartingFrameCount(0, policy->n);
        accessibleActor.basePitch = policy->pitch;
        accessibleActor.policy = *policy;
        accessibleActor.currentPitch = accessibleActor.policy.pitch;
        accessibleActor.baseVolume = accessibleActor.policy.volume;
        accessibleActor.currentVolume = accessibleActor.policy.volume;
        accessibleActor.currentReverb = 0;
        aa->trackedActors[actor] = accessibleActor.instanceID;
        aa->accessibleActorList[accessibleActor.instanceID] = accessibleActor;
        AccessibleActor& savedActor = aa->accessibleActorList[accessibleActor.instanceID];
        if (policy->initUserData && !policy->initUserData(&savedActor)) {
            ActorAccessibility_RemoveTrackedActor(actor);

            return; // Probably a malloc error preventing user data initialization.
        }

    }
    void ActorAccessibility_RemoveTrackedActor(Actor* actor) {
        TrackedActors_t::iterator i = aa->trackedActors.find(actor);
        if (i == aa->trackedActors.end())
            return;
        uint64_t id = i->second;
        aa->trackedActors.erase(i);
        AccessibleActorList_t::iterator i2 = aa->accessibleActorList.find(id);
        if (i2 == aa->accessibleActorList.end())
            return;
        if (i2->second.policy.cleanupUserData)
            i2->second.policy.cleanupUserData(&i2->second);
        aa->accessibleActorList.erase(i2);

    }

    f32 ActorAccessibility_DBToLinear(float gain) {
        return (float)pow(10.0, gain / 20.0f);
    }
    f32 ActorAccessibility_ComputeCurrentVolume(f32 maxDistance, f32 xzDistToPlayer) {
        if (maxDistance == 0)
            return 0.0;
        f32 absDistance = fabs(xzDistToPlayer);

        f32 db = LERP(0.0 - MAX_DB_REDUCTION, 0.0, (maxDistance - absDistance) / maxDistance);

        return ActorAccessibility_DBToLinear(db);

    }

    void ActorAccessibility_PlaySpecialSound(AccessibleActor* actor, s16 sfxId) {
        Audio_PlaySoundGeneral(sfxId, &actor->projectedPos, 4, &actor->currentPitch, &actor->currentVolume, &actor->currentReverb);
    }
    bool ActorAccessibility_IsRealActor(AccessibleActor* actor) {
        return actor->actor != NULL;

    }
    void ActorAccessibility_CopyParamsFromRealActor(AccessibleActor* actor) {
        if (actor->actor == NULL)
            return;
        actor->projectedPos = actor->actor->projectedPos;
        actor->xzDistToPlayer = actor->actor->xzDistToPlayer;
        actor->isDrawn = actor->actor->isDrawn;

    }

    void ActorAccessibility_RunAccessibilityForActor(PlayState* play, AccessibleActor* actor) {
        actor->play = play;
        if(ActorAccessibility_IsRealActor(actor))
            ActorAccessibility_CopyParamsFromRealActor(actor);

        else {
//Perform calculations that the game would normally take care of for real actors.
            f32 w = 0.0f;
            //Set actor->projectedPos.
            SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &actor->world.pos, &actor->projectedPos,
                                         &w);
//Set actor->xzDistToPlayer.
            Player* player = GET_PLAYER(play);
            actor->xzDistToPlayer = Math_Vec3f_DistXZ(&actor->world.pos, &player->actor.world.pos);

        }
            actor->currentVolume = ActorAccessibility_ComputeCurrentVolume(actor->policy.distance, actor->xzDistToPlayer);

        actor->frameCount--;
        if (actor->frameCount > 0)
            return;
        actor->frameCount = actor->policy.n;
        if (!actor->policy.runsAlways && actor->xzDistToPlayer > actor->policy.distance) {
            return;
        }
        if (actor->isDrawn == 0)
            return;

        if (actor->policy.callback != NULL)
            actor->policy.callback(actor);
        else
            ActorAccessibility_PlaySpecialSound(actor, actor->policy.sound);

    }
    void ActorAccessibility_RunAccessibilityForAllActors(PlayState* play) {
//Entirely exclude the title screen.
        if (play->sceneNum == 81)
            return;

        //Real actors.
        for (AccessibleActorList_t::iterator i = aa->accessibleActorList.begin(); i != aa->accessibleActorList.end(); i++)
            ActorAccessibility_RunAccessibilityForActor(play, &i->second);
//Virtual actors in the "everywhere" group.
        VAList_t* list = (VAList_t*)ActorAccessibility_GetVirtualActorList(EVERYWHERE, 0);

        for (VAList_t::iterator i = list->begin(); i != list->end(); i++)
            ActorAccessibility_RunAccessibilityForActor(play, &(*i));
//Virtual actors for the current room and scene.
        list = (VAList_t*)ActorAccessibility_GetVirtualActorList(play->sceneNum, play->roomCtx.curRoom.num);
        for (VAList_t::iterator i = list->begin(); i != list->end(); i++)
            ActorAccessibility_RunAccessibilityForActor(play, &(*i));
//Processes external audio engine.
        ActorAccessibility_PrepareNextAudioFrame();
    }
//Virtual actor config.
    VirtualActorList* ActorAccessibility_GetVirtualActorList(s16 sceneNum, s8 roomNum)
    {
        SceneAndRoom sr;
        sr.values.sceneIndex = sceneNum;
        sr.values.roomIndex = roomNum;
        if (sceneNum == EVERYWHERE)
            sr.values.sceneIndex = EVERYWHERE;

        VAList_t* l = &aa->vaZones[sr.raw];
        return (VirtualActorList*)l;

    }
    AccessibleActor* ActorAccessibility_AddVirtualActor(VirtualActorList* list, VIRTUAL_ACTOR_TABLE type, PosRot where)
    {
        ActorAccessibilityPolicy* policy = ActorAccessibility_GetPolicyForActor(type);
        if (policy == NULL)
            return NULL;

        AccessibleActor actor;
        actor.actor = NULL;
        actor.basePitch = 1.0;
        actor.baseVolume = 1.0;
        actor.currentPitch = 1.0;
        actor.currentReverb = 1.0;
        actor.currentVolume = 1.0;
        actor.frameCount = 0;
        actor.id = (s16)type;
        actor.instanceID = ActorAccessibility_GetNextID();
        actor.isDrawn = 1;
        actor.play = NULL;
        actor.world = where;
        actor.policy = *policy;
        VAList_t* l = (VAList_t*)list;
        l->push_back(actor);
        size_t index = l->size() - 1;
        AccessibleActor* savedActor = &(*l)[l->size() - 1];
        if (policy->initUserData && !policy->initUserData(savedActor)) {
            l->pop_back();

            return NULL; // Probably a malloc error preventing user data initialization.
        }
        return savedActor;

    }
//External audio engine stuff.
    bool ActorAccessibility_InitAudio() {
        try {
            aa->audioEngine = new AccessibleAudioEngine();
        } catch (...) {
            aa->audioEngine = NULL;
            return false;
        }
        return true;
    }
    void ActorAccessibility_ShutdownAudio() {
        delete aa->audioEngine;
    }
    void ActorAccessibility_MixAccessibleAudioWithGameAudio(int16_t* ogBuffer, uint32_t nFrames) {
        aa->audioEngine->mix(ogBuffer, nFrames);

    }
    void ActorAccessibility_PlayExternalSound(void* handle, const char* path) {
        aa->audioEngine->playSound((uint64_t)handle, path);

    }
    void ActorAccessibility_PrepareNextAudioFrame() {
        aa->audioEngine->prepare();

    }
    void ActorAccessibility_DecodeAndCacheSamples() {

        auto files = LUS::Context::GetInstance()->GetResourceManager()->GetArchive()->ListFiles("audio/samples/*");
        int samplesFound = 0;
        for (int i = 0; i < files->size(); i++) {
            std::string& path = (*files)[i];
            auto res = LUS::Context::GetInstance()->GetResourceManager()->LoadResource(path);
            if (res == nullptr)
                continue;
            auto type = res->GetInitData()->Type;
            if (type != LUS::ResourceType::SOH_AudioSample)
                continue;
            samplesFound++;

            LUS::AudioSample* sample = reinterpret_cast<LUS::AudioSample*>(res.get());
                //Skip a few samples which appear to be undecodable due to missing information.
            if (sample->book.book == NULL)
                continue;
            if (path == "audio/samples/Bird Chirp 1")
                continue;//Corrupted file.
            AudioDecoder decoder;
            decoder.setSample(sample);
            int16_t* wav = NULL;
            size_t wavSize = decoder.decodeToWav(&wav);
//Who cares about the path: let's keep only the file names for ease of use.
            std::string fileName = path.substr(path.find_last_of('/') + 1);
            aa->audioEngine->cacheDecodedSample(fileName, wav, wavSize);
        }

    }
    void ActorAccessibility_DecodeSampleToFile(SoundFontSample* sample, const char* path)
    {
        AudioDecoder decoder;
        decoder.setSample(sample);
        int16_t* wav = NULL;
        size_t wavSize = decoder.decodeToWav(&wav);
        FILE* f = fopen(path, "wb");
        fwrite(wav, 1, wavSize, f);
        fclose(f);
        free(wav);

    }
    void ActorAccessibility_HandleSoundExtractionMode(PlayState* play)
    {
        aa->soundExtractionState.run();

    }

    void ActorAccessibility_DoSoundExtractionStep()
    {
        aa->soundExtractionState.runCaptureThread();

    }
