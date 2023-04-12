import os, sys, glob

from rom_info import Z64Rom

def chooseROM(verbose=False, non_interactive=False):
    roms = []

    for file in glob.glob("*.z64"):
        rom = Z64Rom(file)
        if rom.is_valid:
            roms.append(rom)

    if not (roms):
        print("Error: No roms located, place one in the OTRExporter directory", file=os.sys.stderr)
        sys.exit(1)

    if (len(roms) == 1):
        return roms

    if non_interactive:
        mq_rom = None
        non_mq_rom = None
        for rom in roms:
            if rom.isMq and mq_rom is None:
                mq_rom = rom
            elif not rom.isMq and non_mq_rom is None:
                non_mq_rom = rom
        return [rom for rom in [non_mq_rom, mq_rom] if rom is not None]

    print(f"{len(roms)} roms found, please select one by pressing 1-{len(roms)}")
    print()

    for i in range(len(roms)):
        print(f"[{i+1:>2d}] {roms[i].file_path}")
        if verbose:
            print(f"     Checksum: {roms[i].checksum.value}, Version XML: {roms[i].version.xml_ver}")
            print()

    while(1):
        try:
            selection = int(input())
        except KeyboardInterrupt:
            sys.exit(1)
        except:
            print("Bad input. Try again with the number keys.")
            continue

        if (selection < 1 or selection > len(roms)):
            print("Bad input. Try again.")
            continue

        else: break

    return [ roms[selection - 1] ]
