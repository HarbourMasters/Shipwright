#pragma once

// Check tracker check visibility categories
typedef enum {
    RCSHOW_UNCHECKED,
    RCSHOW_SKIPPED,
    RCSHOW_SEEN,
    RCSHOW_HINTED,
    RCSHOW_CHECKED,
    RCSHOW_SCUMMED,
    RCSHOW_SAVED,
} RandomizerCheckShow;

void InitCheckTracker();
void DrawCheckTracker(bool& open);
