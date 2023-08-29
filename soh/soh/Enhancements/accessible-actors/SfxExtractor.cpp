#include "SfxExtractor.h"
#include "dr_libs/wav.h"
#include "soh/Enhancements/speechsynthesizer/SpeechSynthesizer.h"
#include "soh/Enhancements/tts/tts.h"
#include "miniaudio.h"
#include "z64.h"
#include "functions.h"
#include "soh/OTRGlobals.h"
#include "SfxTable.h"
const char* GetLanguageCode();
extern "C" {
extern Vec3f D_801333D4;
extern f32 D_801333E0;
extern s8 D_801333E8;
extern u8 D_801333F0;
void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);
extern bool freezeGame;
}
enum {
    STEP_SETUP = 0,
    STEP_MAIN,
    STEP_FINISHED,
    STEP_ERROR,
    STEP_ERROR_OTR, // File exists.

} SFX_EXTRACTION_STEPS;
enum {
    CT_WAITING, // for a sound to start ripping.
    CT_PRIMING,
    CT_READY,    // to start ripping a sound.
    CT_FINISHED, // ripping the current sound.
    CT_SHUTDOWN,
} CAPTURE_THREAD_STATES;
#define SFX_EXTRACTION_BUFFER_SIZE 44100 * 15
#define SFX_EXTRACTION_ONE_FRAME 736
bool SfxExtractor::isAllZero(int16_t* buffer, size_t count)

{
    for (auto i = 0; i < count; i++) {
        if (buffer[i] != 0)
            return false;
    }
    return true;
}
void SfxExtractor::setStartOfInput() {
    startOfInput = 0;
    for (startOfInput = 0; startOfInput < SFX_EXTRACTION_BUFFER_SIZE * 2; startOfInput += 2) {
        if (tempBuffer[startOfInput] != 0 || tempBuffer[startOfInput + 1] != 0)
            break;
    }
}
void SfxExtractor::setEndOfInput() {
    while (endOfInput > 0) {
        if (tempBuffer[endOfInput] != 0 || tempBuffer[endOfInput - 1] != 0)
            break;
        endOfInput -= 2;
    }
}
void SfxExtractor::renderOutput() {

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
    std::string fileName = getExternalFileName(currentSfx);
    void* mem = NULL;
    size_t size = 0;

    if (!drwav_init_memory_write(&wav, &mem, &size, &format, NULL))
        throw std::runtime_error("SFX Extractor: Unable to initialize wave writer.");
    int16_t chunk[64];
    int16_t* mark = tempBuffer + startOfInput;
    size_t samplesLeft = endOfInput - startOfInput;
    while (samplesLeft > 0) {
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
void SfxExtractor::setup() {
    try {

        SpeechSynthesizer::Instance->Speak("Sfx extraction speedrun initiated. Please wait. This will take a few minutes.", GetLanguageCode());
        // Kill the audio thread so we can take control.
        captureThreadState = CT_WAITING;
        OTRAudio_InstallSfxCaptureThread();
        // Make sure we're starting from a clean slate.
        std::string sohAccessibilityPath = LUS::Context::GetPathRelativeToAppDirectory("accessibility.otr");
        if (std::filesystem::exists(sohAccessibilityPath)) {
            currentStep = STEP_ERROR_OTR;
            return;
        }
        tempStorage.resize((SFX_EXTRACTION_BUFFER_SIZE + (SFX_EXTRACTION_ONE_FRAME * 3)) * 2,
                           0); // Over-allocated just a tad because otherwise we'll overrun if the last frame is short.
        tempBuffer = tempStorage.data();

        // Build are master sfx extraction todo list.
        for (int i = 0; i < sfxCount; i++)
            sfxToRip.push(sfxTable[i]);

        currentStep = STEP_MAIN;
        for (int i = 1; i < 10; i++)
            progressMilestones[i - 1] = sfxToRip.size() - ((int)ceil(sfxToRip.size() * (i / 10.0f)));
        archive = LUS::Archive::CreateArchive("accessibility.otr", sfxToRip.size());

    } catch (...) { currentStep = STEP_ERROR; }
}
void SfxExtractor::ripNextSfx() {
    {
        auto lock = OTRAudio_Lock();
        if (captureThreadState == CT_READY || captureThreadState == CT_PRIMING)
            return; // Keep going.
    }
    // Was the last sfx a loop? If so then we need to stop it, and then we need to run audio out to nowhere for as long
    // as it takes to get back to a blank slate.
    if (currentSfx != -1) {
        Audio_StopSfxByPos(&D_801333D4);
        captureThreadState = CT_PRIMING;
        currentSfx = -1;

        return;
    }
    if (sfxToRip.empty()) {
        currentStep = STEP_FINISHED; // Caught 'em all!
        return;
    }

    currentSfx = sfxToRip.front();
    sfxToRip.pop();
    startOfInput = 0;
    endOfInput = 0;
    Audio_PlaySoundGeneral(currentSfx, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

    {
        auto lock = OTRAudio_Lock();
        captureThreadState = CT_READY;
    }
    maybeGiveProgressReport();
}
void SfxExtractor::finished() {
    OTRAudio_UninstallSfxCaptureThread(); // Returns to normal audio opperation.
    CVarClear("gExtractSfx");
    CVarSave();
    archive = nullptr;
    freezeGame = false;

    Audio_QueueSeqCmd(NA_BGM_TITLE);

    if (currentStep == STEP_ERROR || currentStep == STEP_ERROR_OTR) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        Audio_PlaySoundGeneral(NA_SE_EN_GANON_LAUGH, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        std::stringstream ss;
        ss << "Sorry, we tried to extract the sound effects, but Ganondorf overruled us with an iron fist."
           << std::endl;
        if (currentStep == STEP_ERROR_OTR)
            ss << "In all seriousness, please delete accessibility.otr and try again.";
        SpeechSynthesizer::Instance->Speak(ss.str().c_str(), GetLanguageCode());
    } else
        Audio_PlayFanfare(NA_BGM_ITEM_GET);

}
void SfxExtractor::maybeGiveProgressReport() {
    size_t ripsRemaining = sfxToRip.size() + 1;
    for (int i = 0; i < 9; i++) {
        if (ripsRemaining == progressMilestones[i]) {
            int percentDone = (i + 1) * 10;
            std::stringstream ss;
            ss << percentDone << " percent complete.";
            SpeechSynthesizer::Instance->Speak(ss.str().c_str(), GetLanguageCode());
        }
    }
}
SfxExtractor::SfxExtractor() {
    currentStep = STEP_SETUP;
}

    void SfxExtractor::frameCallback() {
    switch (currentStep) {
        case STEP_SETUP:
            setup();
            break;
        case STEP_MAIN:
            ripNextSfx();
            break;
        default: // Handles finished as well as a number of error conditions.
            finished();
    }
}
void SfxExtractor::prime() {
    while (true) {
        AudioMgr_CreateNextAudioBuffer(tempBuffer, SFX_EXTRACTION_ONE_FRAME);
        if (isAllZero(tempBuffer, SFX_EXTRACTION_ONE_FRAME * 2))
            break;
    }
    captureThreadState = CT_FINISHED;
}
void SfxExtractor::captureCallback() {
    if (captureThreadState == CT_PRIMING)
        prime();
    if (captureThreadState != CT_READY)
        return; // No work to do at the moment.
    memset(tempBuffer, 0, SFX_EXTRACTION_BUFFER_SIZE * 4);
    int16_t* mark = tempBuffer;
    size_t samplesLeft = SFX_EXTRACTION_BUFFER_SIZE;
    bool outputStarted = false;
    endOfInput = 0;
    int waitTime = 0;
    while (samplesLeft > 0) {
        AudioMgr_CreateNextAudioBuffer(mark, SFX_EXTRACTION_ONE_FRAME);

        if (!outputStarted && isAllZero(mark, SFX_EXTRACTION_ONE_FRAME * 2)) {
waitTime++;
            if (waitTime < 300)
                continue;//Output is silent, allow more time for audio to begin.
            captureThreadState = CT_FINISHED; // Sound is unavailable, so skip over it and move on.
            return;
        }

        else
            outputStarted = true;
        if (isAllZero(mark, SFX_EXTRACTION_ONE_FRAME * 2))
            break; // End of sound.
        mark += (SFX_EXTRACTION_ONE_FRAME * 2);
        endOfInput += (SFX_EXTRACTION_ONE_FRAME * 2);
        samplesLeft -= std::min<size_t>(SFX_EXTRACTION_ONE_FRAME, samplesLeft);
    }
    setStartOfInput();
    setEndOfInput();
    renderOutput();
    captureThreadState = CT_FINISHED; // Go to the next one.
}
std::string SfxExtractor::getExternalFileName(int16_t sfxId) {
    std::stringstream ss;
    ss << "accessibility/audio/";
    ss << std::hex << std::setw(4) << std::setfill('0') << sfxId << ".wav";
    return ss.str();
}
