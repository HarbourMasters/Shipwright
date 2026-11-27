#pragma once

namespace CheckTracker {

// Runs once per gameplay frame and marks shuffled items the player has clearly seen as RCSHOW_DISCOVERED.
void CheckTrackerDiscoveredFrame(int saveSectionId);
void CheckTrackerDiscoveredTeardown();

} // namespace CheckTracker
