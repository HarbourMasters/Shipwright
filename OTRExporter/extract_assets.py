#!/usr/bin/env python3

import os, sys, shutil
import shutil
from rom_info import Z64Rom
import rom_chooser
import struct
import subprocess
import argparse

def BuildOTR(xmlPath, rom, zapd_exe=None, custom_music_path=None):
    shutil.copytree("assets", "Extract/assets")

    if not zapd_exe:
        zapd_exe = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPDTR/ZAPD.out"

    exec_cmd = [zapd_exe, "ed", "-i", xmlPath, "-b", rom, "-fl", "CFG/filelists",
            "-o", "placeholder", "-osf", "placeholder", "-gsf", "1",
            "-rconf", "CFG/Config.xml", "-se", "OTR", "--otrfile", 
            "oot-mq.otr" if Z64Rom.isMqRom(rom) else "oot.otr"]
    if custom_music_path != None:
        exec_cmd.append("--custom-music")
        exec_cmd.append(custom_music_path)

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
    parser.add_argument("--non-interactive", help="Runs the script non-interactively for use in build scripts.", dest="non_interactive", action="store_true")

    args = parser.parse_args()

    rom_paths = [ args.rom ] if args.rom else rom_chooser.chooseROM(args.non_interactive)
    create_music_patch = os.path.exists("Custom/Music");
    for rom_path in rom_paths:
        rom = Z64Rom(rom_path)

        if (os.path.exists("Extract")):
            shutil.rmtree("Extract")

        if create_music_patch:
            BuildOTR("../soh/assets/xml/" + rom.version.xml_ver + "/", rom_path, zapd_exe=args.zapd_exe, custom_music_path="Custom/Music")
            create_music_patch = False
        else:
            BuildOTR("../soh/assets/xml/" + rom.version.xml_ver + "/", rom_path, zapd_exe=args.zapd_exe)

if __name__ == "__main__":
    main()
