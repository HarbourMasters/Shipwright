#include "baserom_extractor.h"
#include "utils/mutils.h"
#include "yaz0/yaz0.h"
#include <filesystem>
#include <fstream>
#include <cstring>

#ifndef _MSC_VER
#include <byteswap.h>
#endif

namespace fs = std::filesystem;

#define DMA_ENTRY_SIZE 16

#if defined (_MSC_VER)
#define __bswap_32  _byteswap_ulong
#define bswap_32 _byteswap_ulong
#endif

//Rom DMA table start

#define OOT_OFF_NTSC_10_RC 0x7430
#define OOT_OFF_NTSC_10 0x7430
#define OOT_OFF_NTSC_11 0x7430
#define OOT_OFF_PAL_10 0x7950
#define OOT_OFF_NTSC_12 0x7960
#define OOT_OFF_PAL_11 0x7950
#define OOT_OFF_JP_GC 0x7170
#define OOT_OFF_JP_MQ 0x7170
#define OOT_OFF_US_GC 0x7170
#define OOT_OFF_US_MQ 0x7170
#define OOT_OFF_PAL_GC_DBG1 0x12F70
#define OOT_OFF_PAL_MQ_DBG 0x12F70
#define OOT_OFF_PAL_GC_DBG2 0x12F70
#define OOT_OFF_PAL_GC 0x7170
#define OOT_OFF_PAL_MQ 0x7170
#define OOT_OFF_JP_GC_CE 007170
#define OOT_OFF_CN_IQUE 0xB7A0
#define OOT_OFF_TW_IQUE 0xB240

#define MM_OFF_US_10 0x1A500
#define MM_OFF_JP_10 0x1C110
#define MM_OFF_JP_11 0x1C050
#define MM_OFF_DBG 0x24F60

RomVersion GetVersion(FILE* rom) {
    RomVersion version;

    fseek(rom, 0x10, SEEK_SET);

    if (fread(&version.crc, sizeof(uint32_t), 1, rom) != 1) {
        version.error = "Could not read rom CRC";
        return version;
    }

    version.crc = __bswap_32(version.crc);
    switch (version.crc) {
    case OOT_NTSC_10:
        version.version = "N64 NTSC 1.0";
        version.listPath = "ntsc_oot.txt";
        version.offset = OOT_OFF_NTSC_10;
        version.isMQ = false;
        break;
    case OOT_NTSC_11:
        version.version = "N64 NTSC 1.1";
        version.listPath = "ntsc_oot.txt";
        version.offset = OOT_OFF_NTSC_11;
        version.isMQ = false;
        break;
    case OOT_NTSC_12:
        version.version = "N64 NTSC 1.2";
        version.listPath = "ntsc_oot.txt";
        version.offset = OOT_OFF_NTSC_12;
        version.isMQ = false;
        break;
    case OOT_PAL_10:
        version.version = "N64 PAL 1.0";
        version.listPath = "pal_oot.txt";
        version.offset = OOT_OFF_PAL_10;
        version.isMQ = false;
        break;
    case OOT_PAL_11:
        version.version = "N64 PAL 1.1";
        version.listPath = "pal_oot.txt";
        version.offset = OOT_OFF_PAL_11;
        version.isMQ = false;
        break;
    case OOT_NTSC_JP_GC:
        version.version = "JP GameCube (MQ Disk)";
        version.listPath = "gamecube.txt";
        version.offset = OOT_OFF_JP_GC;
        version.isMQ = true;
        break;
    case OOT_NTSC_JP_GC_CE:
        version.version = "GameCube (Collectors Edition Disk)";
        version.listPath = "gamecube.txt";
        version.offset = OOT_OFF_JP_GC_CE;
        version.isMQ = false;
        break;
    case OOT_NTSC_JP_MQ:
        version.version = "JP Master Quest";
        version.listPath = "gamecube.txt";
        version.offset = OOT_OFF_JP_MQ;
        version.isMQ = true;
        break;
    case OOT_NTSC_US_MQ:
        version.version = "NTSC Master Quest";
        version.listPath = "gamecube.txt";
        version.offset = OOT_OFF_JP_MQ;
        version.isMQ = true;
        break;
    case OOT_NTSC_US_GC:
        version.version = "NTSC GameCube";
        version.listPath = "gamecube.txt";
        version.offset = OOT_OFF_US_MQ;
        version.isMQ = true;
        break;
    case OOT_PAL_GC:
        version.version = "PAL GameCube";
        version.listPath = "gamecube_pal.txt";
        version.offset = OOT_OFF_PAL_GC;
        version.isMQ = false;
        break;
    case OOT_PAL_MQ:
        version.version = "PAL Master Quest";
        version.listPath = "gamecube_pal.txt";
        version.offset = OOT_OFF_PAL_MQ;
        version.isMQ = true;
        break;
    case OOT_PAL_GC_DBG1:
        version.version = "GameCube Debug 1.0";
        version.listPath = "dbg.txt";
        version.offset = OOT_OFF_PAL_GC_DBG1;
        version.isMQ = false;
        break;
    case OOT_PAL_GC_DBG2:
        version.version = "GameCube Debug 2.0";
        version.listPath = "dbg.txt";
        version.offset = OOT_OFF_PAL_GC_DBG2;
        version.isMQ = false;
        break;
    case OOT_PAL_GC_MQ_DBG:
        version.version = "GameCube MQ-Debug";
        version.listPath = "dbg.txt";
        version.offset = OOT_OFF_PAL_MQ_DBG;
        version.isMQ = true;
        break;
    case OOT_IQUE_CN:
        version.version = "OoT IQue";
        version.listPath = "ique.txt";
        version.offset = OOT_OFF_CN_IQUE;
        version.isMQ = false;
        break;
    case OOT_IQUE_TW:
        version.version = "TW IQue";
        version.listPath = "ique.txt";
        version.offset = OOT_OFF_TW_IQUE;
        version.isMQ = false;
        break;
    default:
        version.error = MoonUtils::format("Unknown CRC %x given: ", version.crc);
    }
    return version;
}

std::string read(const std::string& file) {
	if (std::ifstream in(file, std::ios::in | std::ios::binary); in)
        return { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
    return "None";
}

int to_int(char* buffer) {
    int a;
    memcpy(&a, buffer, sizeof(int));
    return a;
}

WriteResult ExtractBaserom(const char* romPath) {
    FILE* rom = fopen(romPath, "rb");
    RomVersion version = GetVersion(rom);
    WriteResult result;

    if (rom == nullptr) {
        result.error = "Could not open baserom.z64";
        return result;
    }

    fseek(rom, 0, SEEK_END);
    const long romSize = ftell(rom);
    char* romData = new char[romSize];
    rewind(rom);
    fread(romData, sizeof(char), romSize, rom);

    MoonUtils::mkdir("tmp/baserom/");

    const std::vector<std::string> lines = MoonUtils::split(read(MoonUtils::join("assets/extractor/filelists", version.listPath)), '\n');

    std::vector<uint8_t> decompressedData(1);

    for (int i = 0; i < lines.size(); i++) {
        FILE* outFile = fopen(MoonUtils::join("tmp/baserom", lines[i]).c_str(), "wb");
        const int romOffset = version.offset + (DMA_ENTRY_SIZE * i);

        const int virtStart = bswap_32(to_int(romData + romOffset));
        const int virtEnd   = bswap_32(to_int(romData + romOffset + 4));
        const int physStart = bswap_32(to_int(romData + romOffset + 8));
        const int physEnd   = bswap_32(to_int(romData + romOffset + 12));

    	printf("File: %s vStart: 0x%08X vEnd: 0x%08X pStart: 0x%08X pEnd: 0x%08X\n", lines[i].c_str(), virtStart, virtEnd, physStart, physEnd);

        const bool compressed = physEnd != 0;
    	int size = compressed ? physEnd - physStart : virtEnd - virtStart;

        auto outData = new uint8_t[size];
        memcpy(outData, romData + physStart, size);


        if (compressed) {
            int decSize = virtEnd - virtStart;
            decompressedData = std::vector<uint8_t>(decSize);
            yaz0_decode(outData, decompressedData.data(), decSize);
            outData = decompressedData.data();
            size = decSize;
        }

        fwrite(outData, sizeof(char), size, outFile);
        fflush(outFile);
        fclose(outFile);
    }
    fclose(rom);

    return result;
}