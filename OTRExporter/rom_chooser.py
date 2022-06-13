import os, sys, glob

from rom_info import Z64Rom

def chooseROM():
    roms = []

    for file in glob.glob("*.z64"):
        if Z64Rom.isValidRom(file):
            roms.append(file)

    if not (roms):
        print("Error: No roms located, place one in the OTRExporter directory", file=os.sys.stderr)
        sys.exit(1)

    if (len(roms) == 1):
        return roms[0]

    print(str(len(roms))+ " roms found, please select one by pressing 1-"+str(len(roms)))

    for i in range(len(roms)):
        print(str(i+1)+ ". " + roms[i])

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

    return roms[selection - 1]
