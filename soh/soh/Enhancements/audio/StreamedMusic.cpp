#include "StreamedMusic.h"

#include <array>
#include <unordered_set>

#include <libultraship/libultra.h>
#include "variables.h"

namespace {

std::unordered_set<uint16_t> sStreamedSequences;
std::array<bool, 4> sPlayerIsStreamed{};

} // namespace

extern "C" void SOH_StreamedMusic_SetStreamed(uint16_t seqId, uint8_t isStreamed) {
    if (isStreamed != 0) {
        sStreamedSequences.insert(seqId);
    } else {
        sStreamedSequences.erase(seqId);
    }
}

extern "C" void SOH_StreamedMusic_SequenceStarted(uint8_t playerIdx, uint16_t seqId) {
    sPlayerIsStreamed.at(playerIdx) = sStreamedSequences.contains(seqId);
}

extern "C" void SOH_StreamedMusic_SequenceStopped(uint8_t playerIdx) {
    sPlayerIsStreamed.at(playerIdx) = false;
}

extern "C" uint8_t SOH_StreamedMusic_ShouldSplitChannels(uint8_t playerIdx) {
    // An empty player is about to receive the enemy sequence, queued a step earlier.
    const bool shouldSplit = gAudioContext.seqPlayers[playerIdx].enabled && !sPlayerIsStreamed.at(playerIdx);
    return static_cast<uint8_t>(shouldSplit);
}
