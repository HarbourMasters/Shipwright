#!/usr/bin/env python3
"""
Generate I4 font glyph data for SoH Chinese support.

Reads charmap_chn.txt to determine which characters need glyphs,
renders them using a specified CJK font, and outputs:
  1. z_kanfont_chinese_data.c  (C array of glyph bytes)
  2. chinese_font_extra.bin    (raw binary for extra glyphs only)

Requirements:
  pip install Pillow

Usage:
  python generate_font_glyphs.py --font /path/to/CJKFont.ttf --charmap charmap_chn.txt --output-dir ../output
"""

import argparse
import struct
import sys
from pathlib import Path

try:
    from PIL import Image, ImageDraw, ImageFont
except ImportError:
    print("Error: Pillow is required. Install with: pip install Pillow")
    sys.exit(1)


# Font glyph format: 16x16 pixels, I4 (4-bit per pixel), 128 bytes per glyph
GLYPH_WIDTH = 16
GLYPH_HEIGHT = 16
FONT_CHAR_TEX_SIZE = 128  # 16*16/2 = 128 bytes (4 bits per pixel)

# Original charmap range
ORIGINAL_CODE_START = 0xA08C
ORIGINAL_CODE_END = 0xA775  # inclusive
ORIGINAL_GLYPH_COUNT = ORIGINAL_CODE_END - ORIGINAL_CODE_START + 1  # 1770

# Extra charmap range
EXTRA_CODE_START = 0xAAAA


def parse_charmap(charmap_path):
    """Parse charmap_chn.txt: each line is 'UNICODE_HEX=INTERNAL_HEX'"""
    entries = []
    with open(charmap_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split('=')
            if len(parts) == 2:
                unicode_cp = int(parts[0], 16)
                internal_code = int(parts[1], 16)
                entries.append((unicode_cp, internal_code))
    return entries


def render_glyph_i4(char, font_obj):
    """Render a single character as 16x16 I4 (4-bit grayscale), return 128 bytes."""
    img = Image.new('L', (GLYPH_WIDTH, GLYPH_HEIGHT), 0)
    draw = ImageDraw.Draw(img)

    # Center the character
    bbox = draw.textbbox((0, 0), char, font=font_obj)
    w = bbox[2] - bbox[0]
    h = bbox[3] - bbox[1]
    x = (GLYPH_WIDTH - w) // 2 - bbox[0]
    y = (GLYPH_HEIGHT - h) // 2 - bbox[1]

    draw.text((x, y), char, fill=255, font=font_obj)

    # Convert to I4: pack two 4-bit pixels per byte
    pixels = list(img.getdata())
    glyph_bytes = bytearray(FONT_CHAR_TEX_SIZE)
    for i in range(0, len(pixels), 2):
        hi = (pixels[i] >> 4) & 0x0F
        lo = (pixels[i + 1] >> 4) & 0x0F if (i + 1) < len(pixels) else 0
        glyph_bytes[i // 2] = (hi << 4) | lo

    return bytes(glyph_bytes)


def main():
    parser = argparse.ArgumentParser(description='Generate I4 font glyphs for SoH Chinese support')
    parser.add_argument('--font', required=True, help='Path to CJK TTF/OTF font file')
    parser.add_argument('--font-size', type=int, default=14, help='Font size in pixels (default: 14)')
    parser.add_argument('--charmap', required=True, help='Path to charmap_chn.txt')
    parser.add_argument('--output-dir', default='.', help='Output directory')
    args = parser.parse_args()

    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    # Load font
    font_obj = ImageFont.truetype(args.font, args.font_size)
    print(f"Loaded font: {args.font} (size {args.font_size})")

    # Parse charmap
    entries = parse_charmap(args.charmap)
    print(f"Charmap entries: {len(entries)}")

    # Separate original vs extra range
    original_entries = [(u, c) for u, c in entries if ORIGINAL_CODE_START <= c <= ORIGINAL_CODE_END]
    extra_entries = [(u, c) for u, c in entries if c >= EXTRA_CODE_START]

    # Sort by internal code
    original_entries.sort(key=lambda x: x[1])
    extra_entries.sort(key=lambda x: x[1])

    total_glyphs = ORIGINAL_GLYPH_COUNT + len(extra_entries)
    print(f"Original glyphs: {ORIGINAL_GLYPH_COUNT}, Extra glyphs: {len(extra_entries)}, Total: {total_glyphs}")

    # Build code-to-unicode lookup for original range
    original_map = {c: u for u, c in original_entries}

    # Generate all glyphs
    all_glyph_data = bytearray()

    # Original range (0xA08C - 0xA775)
    print("Generating original range glyphs...")
    for code in range(ORIGINAL_CODE_START, ORIGINAL_CODE_END + 1):
        if code in original_map:
            char = chr(original_map[code])
            glyph = render_glyph_i4(char, font_obj)
        else:
            glyph = b'\x00' * FONT_CHAR_TEX_SIZE  # blank glyph
        all_glyph_data.extend(glyph)

    # Extra range (0xAAAA+)
    print("Generating extra range glyphs...")
    extra_glyph_data = bytearray()
    for unicode_cp, internal_code in extra_entries:
        char = chr(unicode_cp)
        glyph = render_glyph_i4(char, font_obj)
        all_glyph_data.extend(glyph)
        extra_glyph_data.extend(glyph)

    # Write binary for extra glyphs only
    extra_bin_path = output_dir / 'chinese_font_extra.bin'
    with open(extra_bin_path, 'wb') as f:
        f.write(extra_glyph_data)
    print(f"Wrote {extra_bin_path} ({len(extra_glyph_data)} bytes, {len(extra_entries)} glyphs)")

    # Write C source file
    c_path = output_dir / 'z_kanfont_chinese_data.c'
    extra_code_end = EXTRA_CODE_START + len(extra_entries) - 1 if extra_entries else EXTRA_CODE_START
    with open(c_path, 'w') as f:
        f.write('// Auto-generated Chinese font glyph data for Ship of Harkinian\n')
        f.write(f'// Total glyphs: {total_glyphs}\n')
        f.write(f'// Original range: 0x{ORIGINAL_CODE_START:04X} - 0x{ORIGINAL_CODE_END:04X} ({ORIGINAL_GLYPH_COUNT} glyphs)\n')
        f.write(f'// Extra range: 0x{EXTRA_CODE_START:04X} - 0x{extra_code_end:04X} ({len(extra_entries)} glyphs)\n')
        f.write(f'// Format: 16x16 I4 (4-bit indexed), {FONT_CHAR_TEX_SIZE} bytes per glyph\n')
        f.write(f'// Total size: {len(all_glyph_data)} bytes\n\n')
        f.write('#include "global.h"\n\n')
        f.write(f'const u8 sChineseFontData[{len(all_glyph_data)}] = {{\n')

        # Write bytes, 16 per line
        for i in range(0, len(all_glyph_data), 16):
            chunk = all_glyph_data[i:i + 16]
            hex_str = ', '.join(f'0x{b:02X}' for b in chunk)
            if i + 16 < len(all_glyph_data):
                f.write(f'    {hex_str},\n')
            else:
                f.write(f'    {hex_str}\n')

        f.write('};\n')

    print(f"Wrote {c_path} ({len(all_glyph_data)} bytes)")
    print(f"\nDone! Copy z_kanfont_chinese_data.c to Shipwright/soh/src/code/")


if __name__ == '__main__':
    main()
