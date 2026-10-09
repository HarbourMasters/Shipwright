#ifndef KANREAD_H
#define KANREAD_H

#include "PR/ultratypes.h"

/**
 * s32 Kanji_OffsetFromShiftJIS(s32 sjis);
 *
 * Returns the offset of the glyph texture data in the file `kanji` corresponding
 * to a given 2-byte Shift-JIS codepoint. No range validity check is carried out.
 *
 * A nice Shift-JIS codepoint table: https://uic.io/en/charset/show/shift_jis/
 * The file `kanji` contains the 'Level 1' kanji (0x889F-0x9872), and a reworked
 * version of the non-kanji section that includes extra English and Hylian glyphs.
 *
 * @note This function assumes that its argument is a valid Shift-JIS codepoint;
 * there is no range protection at all.
 *
 * @param sjis Shift-JIS Codepoint of glyph.
 * @return s32 offset into `kanji` file.
 *
 * @remark Original name: "LeoGetKadr"
 */
s32 Kanji_OffsetFromShiftJIS(s32 sjis);

#endif
