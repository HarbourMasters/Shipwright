#!/usr/bin/env python3

# How to use:
#   Place a rom in this directory then run the script.
#   If you are using multiple roms, the script will let you choose one.
#   To choose with a commandline argument:
#      Python3 extract_assets.py <rom_name>
#      Invalid input results in the first rom being selected

import json, os, signal, time, sys, shutil, glob
from multiprocessing import Pool, cpu_count, Event, Manager, ProcessError
from enum import Enum
from rom_info import Z64Rom
import rom_chooser

def BuildOTR(xmlPath, rom):
    shutil.copytree("assets", "Extract/assets")

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
    
    romToUse = "";
    
    for file in glob.glob("*.z64"):
        roms.append(file)
        
    if not (roms):
        print("Error: No roms located, place one in the OTRExporter directory", file=os.sys.stderr)
        sys.exit(1)
        
    if (len(roms) > 1):
    
        # If commandline args exist
        if (len(sys.argv) > 1):
            try:
                romToUse = sys.argv[1]
            except:
                print("Error with rom inputted via argument")
            
        # No commandline args, select rom using user input
        else:
        
            print(str(len(roms))+" roms found, please select one by pressing 1-"+str(len(roms)))
        
            count = 1
            for list in range(len(roms)):
                print(str(count)+". "+roms[list])
                count += 1
            
            while(1):
                try:
                    selection = int(input())
                except:
                    print("Bad input. Try again with the number keys.")
                    continue
                
                if (selection < 1 or selection > len(roms)):
                    print("Bad input. Try again.")
                    continue
            
                else: break
            
            romToUse = roms[selection - 1]
        
    else:
        romToUse = roms[0]
    
    if checkChecksum(romToUse) == Checksums.OOT_PAL_GC:
        xmlVer = "GC_NMQ_PAL_F"
    elif checkChecksum(romToUse) == Checksums.OOT_PAL_GC_DBG1:
        xmlVer = "GC_NMQ_D"
    else: # default selection
        xmlVer = "GC_MQ_D"

    if (os.path.exists("Extract")):
        shutil.rmtree("Extract")

    BuildOTR("../soh/assets/xml/" + rom.version.xml_ver + "/", rom_path)

if __name__ == "__main__":
    main()
