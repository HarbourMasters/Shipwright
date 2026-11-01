#pragma once

namespace CheckTracker::StateExport {

void Init();
bool IsEnabled();
void RequestWrite();
void WriteIfRequested();
void RemoveFiles();

} // namespace CheckTracker::StateExport
