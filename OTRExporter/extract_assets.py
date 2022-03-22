#!/usr/bin/env python3

import argparse, json, os, signal, time, sys, shutil
from multiprocessing import Pool, cpu_count, Event, Manager, ProcessError
import shutil

def SignalHandler(sig, frame):
    print(f'Signal {sig} received. Aborting...')
    mainAbort.set()
    # Don't exit immediately to update the extracted assets file.

def BuildOTR():
    shutil.copyfile("baserom/Audiobank", "Extract/Audiobank")
    shutil.copyfile("baserom/Audioseq", "Extract/Audioseq")
    shutil.copyfile("baserom/Audiotable", "Extract/Audiotable")

    shutil.copytree("assets", "Extract/assets")

    execStr = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPD/ZAPD.out"

    execStr += " botr -se OTR"

    print(execStr)
    exitValue = os.system(execStr)
    if exitValue != 0:
        print("\n")
        print("Error when building the OTR file...", file=os.sys.stderr)
        print("Aborting...", file=os.sys.stderr)
        print("\n")

def ExtractFile(xmlPath, outputPath, outputSourcePath):
    execStr = "x64\\Release\\ZAPD.exe" if sys.platform == "win32" else "../ZAPD/ZAPD.out"
    execStr += " e -eh -i %s -b baserom/ -o %s -osf %s -gsf 1 -rconf CFG/Config.xml -se OTR" % (xmlPath, outputPath, outputSourcePath)

    if "overlays" in xmlPath:
        execStr += " --static"

    print(execStr)
    exitValue = os.system(execStr)
    #exitValue = 0
    if exitValue != 0:
        print("\n")
        print("Error when extracting from file " + xmlPath, file=os.sys.stderr)
        print("Aborting...", file=os.sys.stderr)
        print("\n")

def ExtractFunc(fullPath):
    *pathList, xmlName = fullPath.split(os.sep)
    objectName = os.path.splitext(xmlName)[0]

    outPath = os.path.join("..\\soh\\assets\\", *pathList[4:], objectName)
    os.makedirs(outPath, exist_ok=True)
    outSourcePath = outPath

    ExtractFile(fullPath, outPath, outSourcePath)

def initializeWorker(abort, test):
    global globalAbort
    globalAbort = abort


def main():
    parser = argparse.ArgumentParser(description="baserom asset extractor")
    parser.add_argument("-s", "--single", help="asset path relative to assets/, e.g. objects/gameplay_keep")
    parser.add_argument("-f", "--force", help="Force the extraction of every xml instead of checking the touched ones.", action="store_true")
    parser.add_argument("-u", "--unaccounted", help="Enables ZAPD unaccounted detector warning system.", action="store_true")
    args = parser.parse_args()

    global mainAbort
    mainAbort = Event()
    manager = Manager()
    signal.signal(signal.SIGINT, SignalHandler)

    extractedAssetsTracker = manager.dict()

    asset_path = args.single
    if asset_path is not None:
        fullPath = os.path.join("..\\soh\\assets", "xml", asset_path + ".xml")
        if not os.path.exists(fullPath):
            print(f"Error. File {fullPath} doesn't exists.", file=os.sys.stderr)
            exit(1)

        ExtractFunc(fullPath)
    else:
        extract_text_path = "assets/text/message_data.h"
        if os.path.isfile(extract_text_path):
            extract_text_path = None
        extract_staff_text_path = "assets/text/message_data_staff.h"
        if os.path.isfile(extract_staff_text_path):
            extract_staff_text_path = None

        xmlFiles = []
        for currentPath, _, files in os.walk(os.path.join("..\\soh\\assets", "xml")):
            for file in files:
                fullPath = os.path.join(currentPath, file)
                if file.endswith(".xml"):
                    xmlFiles.append(fullPath)

        try:
            numCores = 2
            print("Extracting assets with " + str(numCores) + " CPU cores.")
            with Pool(numCores, initializer=initializeWorker, initargs=(mainAbort, 0)) as p:
                p.map(ExtractFunc, xmlFiles)
        except Exception as e:
            print("Warning: Multiprocessing exception ocurred.", file=os.sys.stderr)
            print("Disabling mutliprocessing.", file=os.sys.stderr)

            initializeWorker(mainAbort, 0)
            for singlePath in xmlFiles:
                ExtractFunc(singlePath)


        BuildOTR()
        shutil.rmtree("Extract")

if __name__ == "__main__":
    main()