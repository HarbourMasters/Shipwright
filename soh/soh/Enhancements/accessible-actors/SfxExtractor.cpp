#include "SfxExtractor.h"
#include "AccessibleSfxCodec.h"
#include "soh/Enhancements/audio/miniaudio.h"
#include "soh/Enhancements/speechsynthesizer/SpeechSynthesizer.h"
#include "soh/OTRGlobals.h"
#include "SfxTable.h"
#include <sstream>
extern "C" {
#include "functions.h"
#include "variables.h"
void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);
extern bool freezeGame;
}

bool SfxExtractor::isAllSilence(int16_t* buffer, size_t count) {
    for (size_t i = 0; i < count; i++) {
        if (!isSilentSample(buffer[i])) // Tolerance for low-amplitude dither noise.
            return false;
    }
    return true;
}
bool SfxExtractor::isSilentSample(int16_t sample) {
    return abs(sample) <= SFX_EXTRACTION_SILENCE_THRESHOLD;
}

// Find the beginning of a captured signal.
size_t SfxExtractor::adjustedStartOfInput() {
    size_t startOfInput = 0;
    while (startOfInput + 2 < SFX_EXTRACTION_BUFFER_SIZE * 2 && isSilentSample(tempBuffer[startOfInput]) &&
           isSilentSample(tempBuffer[startOfInput + 1])) {
        startOfInput += 2;
    }
    return startOfInput;
}

size_t SfxExtractor::adjustedEndOfInput(size_t endOfInput) {
    while (endOfInput > 0 && (!isSilentSample(tempBuffer[endOfInput]) || isSilentSample(tempBuffer[endOfInput - 1]))) {
        endOfInput -= 2;
    }
    return endOfInput;
}

bool SfxExtractor::renderOutput(size_t endOfInput) {
    size_t startOfInput = adjustedStartOfInput();
    endOfInput = adjustedEndOfInput(endOfInput);
    if (endOfInput <= startOfInput) {
        return true;
    }

    ma_channel_converter_config config =
        ma_channel_converter_config_init(ma_format_s16, 2, NULL, 1, NULL, ma_channel_mix_mode_default);
    ma_channel_converter converter;
    if (ma_channel_converter_init(&config, NULL, &converter) != MA_SUCCESS) {
        return false;
    }
    std::vector<int16_t> mono;
    mono.reserve((endOfInput - startOfInput) / 2);
    std::string fileName = getExternalFileName(currentSfx);
    int16_t chunk[64];
    int16_t* mark = tempBuffer + startOfInput;
    while (mark < tempBuffer + endOfInput) {
        size_t chunkSize = std::min<size_t>(64, ((tempBuffer + endOfInput) - mark) / 2);
        ma_result converter_result = ma_channel_converter_process_pcm_frames(&converter, chunk, mark, chunkSize);
        if (converter_result != MA_SUCCESS) {
            return false;
        }
        mono.insert(mono.end(), chunk, chunk + chunkSize);
        mark += chunkSize * 2;
    }

    std::vector<uint8_t> fileData;
    if (!AccessibleSfx_EncodeVorbis(mono.data(), mono.size(), fileData)) {
        return false;
    }
    return archive->WriteFile(fileName.c_str(), fileData);
}

void SfxExtractor::setup() {
    try {
        SpeechSynthesizer::Instance->Speak(
            "Sfx extraction speedrun initiated. Please wait. This will take a few minutes.", "en-US");
        // Kill the audio thread so we can take control.
        captureThreadState = CT_WAITING;
        OTRAudio_InstallSfxCaptureThread();
        // Make sure we're starting from a clean slate.
        std::string sohAccessibilityPath = Ship::Context::GetPathRelativeToAppBundle("accessibility.o2r");
        if (std::filesystem::exists(sohAccessibilityPath)) {
            currentStep = STEP_ERROR_FILE_EXISTS;
            return;
        }

        sfxToRip = 0;
        currentSfx = -1;
        currentStep = STEP_MAIN;
        archive = std::make_shared<Ship::O2rArchive>(sohAccessibilityPath);
        archive->Open();
    } catch (...) { currentStep = STEP_ERROR; }
}

void SfxExtractor::ripNextSfx() {
    // This entire method is expected to be atomic; Don't try to narrow the scope of this lock please!
    // Todo: remove the thread altogether as we don't actually need or want parallelism here.
    auto lock = OTRAudio_Lock();
    if (captureThreadState == CT_READY || captureThreadState == CT_PRIMING)
        return; // Keep going.
    // Was the last sfx a loop? If so then we need to stop it, and then we need to run audio out to nowhere for as long
    // as it takes to get back to a blank slate.
    if (currentSfx != -1) {
        Audio_StopSfxByPos(&gSfxDefaultPos);
        captureThreadState = CT_PRIMING;
        currentSfx = -1;

        return;
    }
    if (sfxToRip == sfxCount) {
        currentStep = STEP_FINISHED; // Caught 'em all!
        return;
    }

    currentSfx = sfxTable[sfxToRip++];
    Audio_PlaySoundGeneral(currentSfx, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                           &gSfxDefaultReverb);
    captureThreadState = CT_READY;
    maybeGiveProgressReport();
}
void SfxExtractor::finished() {
    OTRAudio_UninstallSfxCaptureThread(); // Returns to normal audio opperation.
    // setup() bails out before opening the archive on some errors, so it may not exist.
    if (archive != nullptr) {
        if (currentStep < STEP_ERROR) {
            // Stamp the format last, so a run that died partway through isn't mistaken for a complete archive.
            std::string version = A11Y_SFX_FORMAT_VERSION;
            archive->WriteFile(A11Y_SFX_FORMAT_PATH, std::vector<uint8_t>(version.begin(), version.end()));
        }
        archive->Close();
        archive = nullptr;
    }
    freezeGame = false;

    Audio_QueueSeqCmd(NA_BGM_TITLE);

    if (currentStep >= STEP_ERROR) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        Audio_PlaySoundGeneral(NA_SE_EN_GANON_LAUGH, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        std::stringstream ss;
        ss << "Sorry, we tried to extract the sound effects, but Ganondorf overruled us with an iron fist."
           << std::endl;
        if (currentStep == STEP_ERROR_FILE_EXISTS)
            ss << "In all seriousness, please delete accessibility.o2r and try again.";
        SpeechSynthesizer::Instance->Speak(ss.str().c_str(), "en-US");
    } else
        Audio_PlayFanfare(NA_BGM_ITEM_GET);
}
void SfxExtractor::maybeGiveProgressReport() {
    for (int i = 0; i < 9; i++) {
        if (sfxToRip == sfxCount * (i + 1) / 10) {
            std::stringstream ss;
            ss << (i + 1) * 10 << " percent complete.";
            SpeechSynthesizer::Instance->Speak(ss.str().c_str(), "en-US");
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
    int frameLimit = 0; // A couple of sounds don't come to a full stop until another sound is loaded, but should be
                        // effectively silent after a couple of seconds.
    do {
        AudioMgr_CreateNextAudioBuffer(tempBuffer + 0, SFX_EXTRACTION_ONE_FRAME);
    } while (frameLimit++ < 200 && !isAllSilence(tempBuffer + 0, SFX_EXTRACTION_ONE_FRAME * 2));
    captureThreadState = CT_FINISHED;
}

void SfxExtractor::captureCallback() {
    if (captureThreadState == CT_PRIMING)
        prime();
    if (captureThreadState != CT_READY)
        return; // No work to do at the moment.
    memset(tempBuffer, 0, sizeof(tempBuffer));
    int16_t* mark = tempBuffer + 0;
    size_t samplesLeft = SFX_EXTRACTION_BUFFER_SIZE;
    bool outputStarted = false;
    int waitTime = 0;
    while (samplesLeft > 0) {
        AudioMgr_CreateNextAudioBuffer(mark, SFX_EXTRACTION_ONE_FRAME);

        if (isAllSilence(mark, SFX_EXTRACTION_ONE_FRAME * 2)) {
            if (outputStarted) {
                break;
            } else if (waitTime++ < 300) {
                continue; // Output is silent, allow more time for audio to begin.
            }
            captureThreadState = CT_FINISHED; // Sound is unavailable, so skip over it and move on.
            return;
        }

        outputStarted = true;
        size_t samples = std::min<size_t>(SFX_EXTRACTION_ONE_FRAME, samplesLeft);
        mark += samples * 2;
        samplesLeft -= samples;
    }
    if (renderOutput(mark - tempBuffer)) {
        captureThreadState = CT_FINISHED;
    } else {
        SPDLOG_ERROR("failed to write file to archive, trying again");
    }
}

std::string SfxExtractor::getExternalFileName(int16_t sfxId) {
    std::stringstream ss;
    ss << "accessibility/audio/" << std::hex << std::setw(4) << std::setfill('0') << sfxId << ".ogg";
    return ss.str();
}
