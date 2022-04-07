#!/usr/bin/env python3

import argparse, json, os, signal, time, sys, shutil
from multiprocessing import Pool, cpu_count, Event, Manager, ProcessError
import shutil

romPath = "..\\soh\\baserom_non_mq.z64"

def BuildOTR(xmlPath):
    shutil.copytree("assets", "Extract/assets")

    execStr = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPD/ZAPD.out"
    execStr += " ed -i %s -b %s -fl CFG\\filelists -o placeholder -osf placeholder -gsf 1 -rconf CFG/Config.xml -se OTR" % (xmlPath, romPath)

    print(execStr)
    exitValue = os.system(execStr)
    if exitValue != 0:
        print("\n")
        print("Error when building the OTR file...", file=os.sys.stderr)
        print("Aborting...", file=os.sys.stderr)
        print("\n")

def main():
    parser = argparse.ArgumentParser(description="baserom asset extractor")
    parser.add_argument("-v", "--version", help="Sets game version.")
    args = parser.parse_args()
    
    # TODO: Read from makerom file to automatically determine game version
    xmlVer = "GC_NMQ_D"

    if (args.version == "gc_pal_nmpq"):
        xmlVer = "GC_NMQ_PAL_F"
    elif (args.version == "dbg_mq"):
        xmlVer = "GC_MQ_D"
        romPath = "..\\soh\\baserom_mq.z64"

    if (os.path.exists("Extract")):
        shutil.rmtree("Extract")
    
    BuildOTR("..\\soh\\assets\\xml\\" + xmlVer + "\\")

if __name__ == "__main__":
    main()
