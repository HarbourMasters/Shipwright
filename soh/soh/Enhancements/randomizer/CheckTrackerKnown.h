#pragma once

namespace CheckTracker {

// Runs once per gameplay frame and marks shuffled items the player has clearly seen as RCSHOW_KNOWN.
void CheckTrackerKnownFrame(int saveSectionId);
void CheckTrackerKnownTeardown();

} // namespace CheckTracker
