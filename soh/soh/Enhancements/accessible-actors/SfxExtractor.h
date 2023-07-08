#pragma once
#include "libultraship/libultraship.h"
class SfxExtractor {
    std::shared_ptr<LUS::Archive> archive;
    int currentStep;
    int captureThreadState;
    std::queue<s16> sfxToRip;
    size_t startOfInput;
    size_t endOfInput;
    s16 currentSfx;
    std::vector<int16_t> tempStorage; // Stores raw audio data for the sfx currently being ripped.
    int16_t* tempBuffer;              // Raw pointer to the above vector.
    int retrievalAttempts; // Counts failures. If the game doesn't render nonzero samples after being asked to play a
                           // sound, we count it as an attempt. If we still don't have output after five attempts, we'll
                           // skip the sound.
    int progressMilestones[9]; // Implements progress reports after every 10 percent.
    // Check if a buffer contains meaningful audio output.
    bool isAllZero(int16_t* buffer, size_t count);
    // Find the beginning of a captured signal.
    void setStartOfInput();
    void setEndOfInput();
    void renderOutput();
    void setup();
    void ripNextSfx();
    void finished();// Also handles failure.
    void maybeGiveProgressReport();
  public:
    SfxExtractor();
    void frameCallback();
    void prime();
    // The below is called by the (hijacked) audio thread.
    void captureCallback();
    static std::string getExternalFileName(int16_t sfxId);
};
