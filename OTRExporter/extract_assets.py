#!/usr/bin/env python3

import os, sys, shutil
import shutil
from rom_info import Z64Rom
import rom_chooser
import struct
import subprocess

def BuildOTR(xmlPath, rom):
    shutil.copytree("assets", "Extract/assets")

    checksum = int(Z64Rom(rom).checksum.value, 16)
    with open("Extract/version", "wb") as f:
        f.write(struct.pack('<L', checksum))

    zapd_exe = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPDTR/ZAPD.out"
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
    rom_path = rom_chooser.chooseROM()
    rom = Z64Rom(rom_path)

    if (os.path.exists("Extract")):
        shutil.rmtree("Extract")

    BuildOTR("../soh/assets/xml/" + rom.version.xml_ver + "/", rom_path)

if __name__ == "__main__":
    main()
