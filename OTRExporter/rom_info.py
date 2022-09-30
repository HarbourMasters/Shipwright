from enum import Enum
from tabnanny import check
import struct

class Checksums(Enum):
    OOT_NTSC_10 = "EC7011B7"
    OOT_NTSC_11 = "D43DA81F"
    OOT_NTSC_12 = "693BA2AE"
    OOT_PAL_10 = "B044B569"
    OOT_PAL_11 = "B2055FBD"
    OOT_NTSC_JP_GC_CE = "F7F52DB8"
    OOT_NTSC_JP_GC = "F611F4BA"
    OOT_NTSC_US_GC = "F3DD35BA"
    OOT_PAL_GC = "09465AC3"
    OOT_NTSC_JP_MQ = "F43B45BA"
    OOT_NTSC_US_MQ = "F034001A"
    OOT_PAL_MQ = "1D4136F3"
    OOT_PAL_GC_DBG1 = "871E1C92"
    OOT_PAL_GC_DBG2 = "87121EFE"
    OOT_PAL_GC_MQ_DBG = "917D18F6"
    OOT_IQUE_TW = "3D81FB3E"
    OOT_IQUE_CN = "B1E1E07B"
    OOT_UNKNOWN = "FFFFFFFF"

    @classmethod
    def has_value(self, value):
        return value in self._value2member_map_

class RomVersion:
    def __init__(self, file_table_path, file_table_off, xml_ver):
        self.file_table_off = file_table_off
        self.xml_ver = xml_ver
        with open(file_table_path, 'r') as f:
            self.file_table = [line.strip('\n') for line in f]

ROM_INFO_TABLE = dict()
ROM_INFO_TABLE[Checksums.OOT_PAL_GC] = RomVersion("CFG/filelists/gamecube_pal.txt", 0x7170, "GC_NMQ_PAL_F")
ROM_INFO_TABLE[Checksums.OOT_PAL_GC_DBG1] = RomVersion("CFG/filelists/dbg.txt", 0x12F70, "GC_NMQ_D")
ROM_INFO_TABLE[Checksums.OOT_PAL_GC_MQ_DBG] = RomVersion("CFG/filelists/dbg.txt", 0x12F70, "GC_MQ_D")

class RomDmaEntry:
    def __init__(self, rom, i):

        off = rom.version.file_table_off + 16 * i

        (self.virtStart, \
        self.virtEnd, \
        self.physStart, \
        self.physEnd) = struct.unpack('>IIII', rom.rom_data[off:off+4*4])

        self.compressed = self.physEnd != 0
        self.size = self.physEnd - self.physStart \
                if self.compressed \
                else self.virtEnd - self.virtStart
        self.name = rom.version.file_table[i]


class Z64Rom:
    def __init__(self, file_path):
        self.file_path = file_path
        with open(file_path, 'rb') as f:
            self.rom_data = f.read()

        self.is_valid = len(self.rom_data) > 20 * 1024 * 1024

        if not self.is_valid:
            return

        # get checkum
        checksum_str = self.rom_data[16:16+4].hex().upper()
        self.checksum = Checksums(checksum_str) if Checksums.has_value(checksum_str) else Checksums.OOT_UNKNOWN

        if self.checksum == Checksums.OOT_UNKNOWN:
            self.is_valid = False
            return

        # get rom version
        self.version = ROM_INFO_TABLE[self.checksum]

    def getDmaEntryByIndex(self, i):
        return RomDmaEntry(self, i)

    def readDmaEntry(self, entry):
        return self.rom_data[entry.physStart:entry.physStart + entry.size]

    @staticmethod
    def isValidRom(rom_path):
        return Z64Rom(rom_path).is_valid
