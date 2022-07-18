#!/usr/bin/env python3

import os, sys, shutil
import shutil
from rom_info import Z64Rom
import rom_chooser
import struct

def BuildOTR(xmlPath, rom):
    shutil.copytree("assets", "Extract/assets")

    checksum = int(Z64Rom(rom).checksum.value, 16)
    with open("Extract/version", "wb") as f:
        f.write(struct.pack('<L', checksum))

    execStr = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPDTR/ZAPD.out"
    execStr += " ed -i %s -b %s -fl CFG/filelists -o placeholder -osf placeholder -gsf 1 -rconf CFG/Config.xml -se OTR" % (xmlPath, rom)

    print(execStr)
    exitValue = os.system(execStr)
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
