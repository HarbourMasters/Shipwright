#!/usr/bin/env python3

import os, sys, shutil
import shutil
from rom_info import Z64Rom
import rom_chooser
import struct
import subprocess
import argparse

def BuildOTR(xmlPath, rom, zapd_exe=None, genHeaders=None):
    shutil.copytree("assets", "Extract/assets")

    if not zapd_exe:
        zapd_exe = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPDTR/ZAPD.out"

    exec_cmd = [zapd_exe, "ed", "-i", xmlPath, "-b", rom, "-fl", "CFG/filelists",
                "-o", "placeholder", "-osf", "placeholder", "-rconf", "CFG/Config.xml"]

    # generate headers, but not otrs by excluding the otr exporter
    if genHeaders:
        exec_cmd.extend(["-gsf", "1"])
    else:
        # generate otrs, but not headers
        exec_cmd.extend(["-gsf", "0", "-se", "OTR", "--otrfile",
                "oot-mq.otr" if Z64Rom.isMqRom(rom) else "oot.otr"])

    print(exec_cmd)
    exitValue = subprocess.call(exec_cmd)
    if exitValue != 0:
        print("\n")
        print("Error when building the OTR file...", file=os.sys.stderr)
        print("Aborting...", file=os.sys.stderr)
        print("\n")

def BuildSohOtr(zapd_exe=None):
    shutil.copytree("assets", "Extract/assets")

    if not zapd_exe:
        zapd_exe = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPDTR/ZAPD.out"

    exec_cmd = [zapd_exe, "botr", "-se", "OTR", "--norom"]

    print(exec_cmd)
    exitValue = subprocess.call(exec_cmd)
    if exitValue != 0:
        print("\n")
        print("Error when building soh.otr file...", file=os.sys.stderr)
        print("Aborting...", file=os.sys.stderr)
        print("\n")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-z", "--zapd", help="Path to ZAPD executable", dest="zapd_exe", type=str)
    parser.add_argument("rom", help="Path to the rom", type=str, nargs="?")
    parser.add_argument("--non-interactive", help="Runs the script non-interactively for use in build scripts.", dest="non_interactive", action="store_true")
    parser.add_argument("-v", "--verbose", help="Display rom's header checksums and their corresponding xml folder", dest="verbose", action="store_true")
    parser.add_argument("--gen-headers", help="Generate source headers to be checked in", dest="gen_headers", action="store_true")
    parser.add_argument("--norom", help="Generate only soh.otr to be bundled to the game", dest="norom", action="store_true")

    args = parser.parse_args()

    if args.norom:
        if (os.path.exists("Extract")):
            shutil.rmtree("Extract")
            
        BuildSohOtr(args.zapd_exe)
        return

    roms = [ Z64Rom(args.rom) ] if args.rom else rom_chooser.chooseROM(args.verbose, args.non_interactive)
    for rom in roms:
        if (os.path.exists("Extract")):
            shutil.rmtree("Extract")

        BuildOTR("../soh/assets/xml/" + rom.version.xml_ver + "/", rom.file_path, zapd_exe=args.zapd_exe, genHeaders=args.gen_headers)

if __name__ == "__main__":
    main()
