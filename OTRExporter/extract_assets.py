#!/usr/bin/env python3

import os, sys, shutil
import shutil
from rom_info import Z64Rom
import rom_chooser
import struct
import subprocess
import argparse
import glob

def ConvertPngTextures(zapd_exe):
    textures = []
    for type in ("rgba32", "rgb5a1", "i4", "i8", "ia4", "ia8", "ia16", "ci4", "ci8"):
        textures.extend(glob.glob("assets/**/*." + type + ".png", recursive=True))

    for file in textures:
        splitFile = file.split('.')
        fileExt = splitFile.pop()
        fileFormat = splitFile.pop()
        fileName = ".".join(splitFile)
        exec_cmd = [zapd_exe, "btex", "-brt",
            "-i", file,
            "-o", os.path.join("Extract", fileName),
            "-tt", fileFormat]

        print(exec_cmd)
        buildTextureResult = subprocess.call(exec_cmd)
        if buildTextureResult != 0:
            print("\n")
            print("Error converting PNG texture", file=os.sys.stderr)
            print("Aborting...", file=os.sys.stderr)
            print("\n")
        os.remove(os.path.join("Extract", file))

def BuildOTR(xmlPath, rom, zapd_exe=None):
    shutil.copytree("assets", "Extract/assets")

    if not zapd_exe:
        zapd_exe = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPDTR/ZAPD.out"

    ConvertPngTextures(zapd_exe)

    exec_cmd = [zapd_exe, "ed", "-i", xmlPath, "-b", rom, "-fl", "CFG/filelists",
            "-o", "placeholder", "-osf", "placeholder", "-gsf", "1",
            "-rconf", "CFG/Config.xml", "-se", "OTR"]

    print(exec_cmd)
    exitValue = subprocess.call(exec_cmd)
    if exitValue != 0:
        print("\n")
        print("Error when building the OTR file...", file=os.sys.stderr)
        print("Aborting...", file=os.sys.stderr)
        print("\n")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-z", "--zapd", help="Path to ZAPD executable", dest="zapd_exe", type=str)
    parser.add_argument("rom", help="Path to the rom", type=str, nargs="?")

    args = parser.parse_args()

    rom_path = args.rom if args.rom else rom_chooser.chooseROM()
    rom = Z64Rom(rom_path)

    if (os.path.exists("Extract")):
        shutil.rmtree("Extract")

    BuildOTR("../soh/assets/xml/" + rom.version.xml_ver + "/", rom_path, zapd_exe=args.zapd_exe)

if __name__ == "__main__":
    main()
