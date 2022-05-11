#!/usr/bin/python3

import os
import sys
import struct
from multiprocessing import Pool, cpu_count
from rom_info import Z64Rom
import rom_chooser


rom = None

def initialize_worker(input_rom):
    global rom
    rom = input_rom

def ExtractFunc(i):

    dma_file = rom.getDmaEntryByIndex(i)
    dma_data = rom.readDmaEntry(dma_file)

    filename = '../soh/baserom/' + rom.version.file_table[i]
    print('extracting ' + filename + " (0x%08X, 0x%08X)" % (dma_file.virtStart, dma_file.virtEnd))

    try:
        with open(filename, 'wb') as f:
            f.write(dma_data)
    except IOError:
        print('failed to write file ' + filename)

    # TODO: handle this better
    if dma_file.compressed:
        os.system('tools/yaz0 -d ' + filename + ' ' + filename)

#####################################################################

def main():
    try:
        os.mkdir('../soh/baserom')
    except:
        pass

    rom_path = rom_chooser.chooseROM()
    input_rom = Z64Rom(rom_path)

    # extract files
    num_cores = cpu_count()
    print("Extracting baserom with " + str(num_cores) + " CPU cores.")
    with Pool(num_cores, initialize_worker, (input_rom,)) as p:
        p.map(ExtractFunc, range(len(input_rom.version.file_table)))

if __name__ == "__main__":
    main()
