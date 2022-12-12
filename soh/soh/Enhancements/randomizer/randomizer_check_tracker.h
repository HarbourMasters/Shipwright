#pragma once

namespace CheckTracker {

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

//Converts an index into a Little Endian bitmask, as follows:
//00: 0000000100000000
//01: 0000001000000000
//...
//06: 0100000000000000
//07: 1000000000000000
//08: 0000000000000001
//09: 0000000000000010
//...
//14: 0000000001000000
//15: 0000000010000000
//repeat...
#define INDEX_TO_16BIT_LITTLE_ENDIAN_BITMASK(idx) (0x8000 >> (7 - (idx % 8) + ((idx % 16) / 8) * 8))

void InitCheckTracker();
void DrawCheckTracker(bool& open);

} // namespace CheckTracker