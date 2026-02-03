#!/usr/bin/env python3
"""
Easy Custom Texture Converter - PNG to IA4 C Array Converter for Item Names

Converts PNG images to C arrays for use in the game.
Using C arrays instead of loading textures at runtime reduces RAM/VRAM usage,
but increases disk space. This tradeoff is intentional for performance.

USAGE:
    python easy_custom_texture_converter.py                    # Convert all PNGs in soh/mods/items/names/
    python easy_custom_texture_converter.py custom_dir/        # Convert all PNGs in specified directory
    python easy_custom_texture_converter.py my_item.png        # Convert single PNG file

REQUIREMENTS:
    - PNG must be 128x16 pixels
    - PIL/Pillow: pip install pillow

OUTPUT FORMAT:
    - IA4 (I3A1): 3-bit intensity + 1-bit alpha
    - 2 pixels per byte, 1024 bytes total
    - Variable: gYourItemNameTex (PascalCase)

INTEGRATION:
    1. Place PNG in soh/mods/items/names/
    2. Run this script
    3. Add #include "names/your_item.c" to custom_names.c
    4. Add extern in z_kaleido_scope_PAL.c
    5. Add to iconNameTextures array
"""

import os
import sys
from PIL import Image
from pathlib import Path

def png_to_ia4_c_array(png_path, output_path, var_name=None):
    """
    Convert a PNG image to an IA4 format C array
    IA4 = Intensity (grayscale) + Alpha, 4 bits each, 2 pixels per byte

    Args:
        png_path: Path to input PNG file
        output_path: Path to output .c file
        var_name: Optional custom variable name (auto-generated if None)
    """
    try:
        # Open the image
        img = Image.open(png_path)

        # Convert to RGBA if not already
        if img.mode != 'RGBA':
            img = img.convert('RGBA')

        # Get dimensions
        width, height = img.size

        # Warning for non-standard dimensions
        if width != 128 or height != 16:
            print(f"  Warning: {png_path.name} has dimensions {width}x{height}")
            print(f"           Expected 128x16 for OOT item names")

        # Generate variable name from filename if not provided
        if var_name is None:
            base_name = png_path.stem  # filename without extension
            # Convert snake_case to PascalCase
            parts = base_name.split('_')
            pascal_name = ''.join(word.capitalize() for word in parts)
            var_name = f"g{pascal_name}Tex"

        # Start building the C file content
        c_content = []
        c_content.append(f"// Generated from {png_path.name}")
        c_content.append(f"// Texture: {width}x{height} IA4 format")
        c_content.append(f"")
        c_content.append(f"const unsigned char {var_name}[] = {{")

        # Convert pixels to IA4 format (3 bits intensity, 1 bit alpha, 2 pixels per byte)
        pixels = img.load()

        for y in range(height):
            row_bytes = []
            for x in range(0, width, 2):  # Process 2 pixels at a time
                # First pixel
                r1, g1, b1, a1 = pixels[x, y]
                intensity1 = int(0.299 * r1 + 0.587 * g1 + 0.114 * b1)
                # Convert to IA4: 3 bits intensity (0-7), 1 bit alpha (0-1)
                i1_3bit = (intensity1 >> 5) & 0x7  # Upper 3 bits (0-7)
                a1_1bit = 1 if a1 >= 128 else 0     # Binary alpha

                # Second pixel
                r2, g2, b2, a2 = pixels[x + 1, y]
                intensity2 = int(0.299 * r2 + 0.587 * g2 + 0.114 * b2)
                # Convert to IA4: 3 bits intensity (0-7), 1 bit alpha (0-1)
                i2_3bit = (intensity2 >> 5) & 0x7  # Upper 3 bits (0-7)
                a2_1bit = 1 if a2 >= 128 else 0     # Binary alpha

                # Pack two IA4 pixels into one byte
                # First pixel (I3A1) in upper nibble, second pixel (I3A1) in lower nibble
                pixel1 = (i1_3bit << 1) | a1_1bit  # 0bIIIA
                pixel2 = (i2_3bit << 1) | a2_1bit  # 0bIIIA
                combined_byte = (pixel1 << 4) | pixel2  # 0bIIIAIIIA

                row_bytes.append(f"0x{combined_byte:02X}")

            # Format the row with proper indentation
            c_content.append("    " + ", ".join(row_bytes) + ",")

        # Remove trailing comma from last line
        if c_content[-1].endswith(","):
            c_content[-1] = c_content[-1][:-1]

        c_content.append(f"}};")
        c_content.append(f"")
        c_content.append(f"// Size: {width * height // 2} bytes ({width}x{height} pixels, 4 bits per pixel)")
        c_content.append(f"")

        # Write to output file
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write('\n'.join(c_content))

        return True

    except Exception as e:
        print(f"  Error processing {png_path.name}: {e}")
        return False

def process_directory(input_dir, output_dir=None):
    """
    Process all PNG files in a directory

    Args:
        input_dir: Directory containing PNG files
        output_dir: Output directory (defaults to input_dir if None)
    """
    input_path = Path(input_dir)
    output_path = Path(output_dir) if output_dir else input_path

    if not input_path.exists():
        print(f"Error: Directory '{input_dir}' does not exist")
        return 1

    if not input_path.is_dir():
        print(f"Error: '{input_dir}' is not a directory")
        return 1

    # Find all PNG files
    png_files = sorted(input_path.glob("*.png"))

    if not png_files:
        print(f"No PNG files found in {input_dir}")
        return 0

    print(f"Found {len(png_files)} PNG files to convert in {input_dir}\n")

    # Process each PNG file
    success_count = 0
    failed_count = 0

    for png_file in png_files:
        output_file = output_path / f"{png_file.stem}.c"
        print(f"Converting: {png_file.name} -> {output_file.name}")

        if png_to_ia4_c_array(png_file, output_file):
            if output_file.exists() and output_file.stat().st_size > 0:
                print(f"  [OK] Success: {output_file.name} ({output_file.stat().st_size} bytes)")
                success_count += 1
            else:
                print(f"  [ERROR] File was not created properly")
                failed_count += 1
        else:
            failed_count += 1

    # Print summary
    print("\n" + "=" * 50)
    print(f"Conversion completed:")
    print(f"  Successes: {success_count}")
    print(f"  Failures: {failed_count}")
    print("=" * 50)

    if success_count > 0:
        print(f"\nGenerated .c files are in: {output_path}")

    return 0 if failed_count == 0 else 1

def process_single_file(input_file, output_file=None, var_name=None):
    """
    Process a single PNG file

    Args:
        input_file: Path to input PNG file
        output_file: Path to output .c file (auto-generated if None)
        var_name: Optional custom variable name
    """
    input_path = Path(input_file)

    if not input_path.exists():
        print(f"Error: File '{input_file}' does not exist")
        return 1

    if not input_path.is_file():
        print(f"Error: '{input_file}' is not a file")
        return 1

    if input_path.suffix.lower() != '.png':
        print(f"Error: '{input_file}' is not a PNG file")
        return 1

    # Generate output filename if not provided
    if output_file is None:
        output_path = input_path.with_suffix('.c')
    else:
        output_path = Path(output_file)

    print(f"Converting: {input_path.name} -> {output_path.name}")

    if png_to_ia4_c_array(input_path, output_path, var_name):
        if output_path.exists() and output_path.stat().st_size > 0:
            print(f"  [OK] Success: {output_path} ({output_path.stat().st_size} bytes)")
            return 0
        else:
            print(f"  [ERROR] File was not created properly")
            return 1
    else:
        return 1

def print_usage():
    """Print usage information"""
    print(__doc__)
    print("\nExamples:")
    print("  python convert_item_names.py")
    print("  python convert_item_names.py custom_items/")
    print("  python convert_item_names.py my_item.png")
    print("  python convert_item_names.py my_item.png output_item.c")

def main():
    """Main entry point"""
    # Default directory
    default_dir = Path(__file__).parent / "soh" / "mods" / "items" / "names"

    if len(sys.argv) == 1:
        # No arguments - use default directory
        if default_dir.exists():
            return process_directory(default_dir)
        else:
            print(f"Default directory not found: {default_dir}")
            print("\nUsage: python convert_item_names.py <input_dir or input_file.png>")
            return 1

    elif len(sys.argv) == 2:
        input_arg = sys.argv[1]

        if input_arg in ['-h', '--help', 'help']:
            print_usage()
            return 0

        input_path = Path(input_arg)

        if input_path.is_dir():
            # Directory - process all PNG files
            return process_directory(input_arg)
        elif input_path.is_file() and input_path.suffix.lower() == '.png':
            # Single PNG file
            return process_single_file(input_arg)
        else:
            print(f"Error: '{input_arg}' is not a directory or PNG file")
            return 1

    elif len(sys.argv) == 3:
        # Single file with custom output path
        return process_single_file(sys.argv[1], sys.argv[2])

    elif len(sys.argv) == 4:
        # Single file with custom output path and variable name
        return process_single_file(sys.argv[1], sys.argv[2], sys.argv[3])

    else:
        print("Error: Too many arguments")
        print_usage()
        return 1

if __name__ == "__main__":
    sys.exit(main())
