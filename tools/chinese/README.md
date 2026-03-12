# Chinese Language Support (iQue Official Translation)

Simplified Chinese support for SoH using the official Nintendo iQue translation from the iQue Player version of Ocarina of Time.

## Overview

The iQue Player (神游机) was Nintendo's official Chinese gaming platform released in 2003. Its version of Ocarina of Time contains a complete Simplified Chinese translation with 2116 messages and 1770 unique Chinese characters.

### Encoding

iQue uses a custom 2-byte encoding for Chinese characters:
- `0x00-0x1F`: Control codes (newline, color, box break, etc.)
- `0x20-0x9E`: ASCII / Latin characters
- `0xA0+`: High byte of a 2-byte Chinese character (e.g., `0xA0F1 0xA0F2` = "德库")
- `0xAA9F-0xAAAB`: Button/controller icon codes

### Font Architecture

All 1770 Chinese character textures are extracted from the iQue ROM's `nes_font_static`:
- **1131 characters** share the same glyphs as existing kanji textures in the base ROM (`gMsgKanji*`)
- **1030 characters** are unique to the iQue ROM (`gMsgCharChn*` in `chinese_font.h`, pointing to `nes_font_static/gMsgChar*` OTR paths)

The mapping is defined in `soh/src/code/z_kanfont_chinese_tbl.inc` (2161 entries).

## Setup

Chinese support requires two ROMs:

1. **Base ROM** (US/JP/PAL) — for standard game assets (scenes, textures, audio, etc.)
2. **iQue ROM** — for Chinese font textures extracted via selective extraction (`IQUE_CN_SEL` config)

### Selective Extraction

Full iQue ROM extraction is not feasible (display list divergence causes 22GB+ output). Instead, selective extraction (`Config_IQUE_CN_SEL.xml`) extracts only:
- `nes_font_static` — Chinese character + icon font textures (1909 entries)
- `message_data_static` — Message offset table
- A few additional overlays and textures

Output: `ique_selective.o2r` (~680KB, 2010 resources), placed in the `mods/` folder.

## Files

| File | Description |
|------|-------------|
| `charmap_chn.txt` | Unicode ↔ iQue 2-byte code mapping (main range 0xA08C-0xA775, 1770 chars) |
| `charmap_chn_extra.txt` | Extended character mappings (0xAAAC+, 391 additional characters) |
| `generate_font_glyphs.py` | Renders CJK characters from TTF font → 16×16 I4 grayscale PNG textures |

## Key Source Files

| File | Description |
|------|-------------|
| `soh/soh/z_message_CHI.cpp` | 2116 iQue Chinese messages (native 2-byte encoding) |
| `soh/src/code/z_message_PAL.c` | `Message_DecodeCHI()` — Chinese message decoder |
| `soh/src/code/z_kanfont.c` | `Font_LoadCharChinese()` — loads Chinese character textures from OTR |
| `soh/src/code/z_kanfont_chinese_tbl.inc` | Character code → OTR texture path mapping table |
| `soh/assets/textures/chinese_font/chinese_font.h` | OTR path macros (pointing to iQue ROM `nes_font_static` textures) |
| `soh/assets/xml/IQUE_CN_SEL/` | Selective extraction XML configs for iQue ROM |
