#include "ZRom.h"
#include "Utils/BitConverter.h"
#include <Utils/DiskFile.h>
#include "Utils/Directory.h"
#include "yaz0/yaz0.h"

#ifdef __linux__
#include <byteswap.h>
#endif
#include <Globals.h>

namespace fs = std::filesystem;

#define DMA_ENTRY_SIZE 16

#if defined(_MSC_VER)
#define __bswap_32 _byteswap_ulong
#define bswap_32 _byteswap_ulong
#endif
#if defined __APPLE__
#define __bswap32 __builtin_bswap32
#define bswap32 __builtin_bswap32
#endif

// ROM DMA Table Start
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

#define OOT_NTSC_10 0xEC7011B7
#define OOT_NTSC_11 0xD43DA81F
#define OOT_NTSC_12 0x693BA2AE
#define OOT_PAL_10 0xB044B569
#define OOT_PAL_11 0xB2055FBD
#define OOT_NTSC_JP_GC_CE 0xF7F52DB8
#define OOT_NTSC_JP_GC 0xF611F4BA
#define OOT_NTSC_US_GC 0xF3DD35BA
#define OOT_PAL_GC 0x09465AC3
#define OOT_NTSC_JP_MQ 0xF43B45BA
#define OOT_NTSC_US_MQ 0xF034001A
#define OOT_PAL_MQ 0x1D4136F3
#define OOT_PAL_GC_DBG1 0x871E1C92  // 03-21-2002 build
#define OOT_PAL_GC_DBG2 0x87121EFE  // 03-13-2002 build
#define OOT_PAL_GC_MQ_DBG 0x917D18F6
#define OOT_IQUE_TW 0x3D81FB3E
#define OOT_IQUE_CN 0xB1E1E07B
#define UNKNOWN 0xFFFFFFFF

bool ZRom::IsMQ() {
    int crc = BitConverter::ToInt32BE(romData, 0x10);
    switch (crc) {
        case OOT_NTSC_10:
        case OOT_NTSC_11:
        case OOT_NTSC_12:
        case OOT_PAL_10:
        case OOT_PAL_11:
        case OOT_NTSC_JP_GC:
        case OOT_NTSC_JP_GC_CE:
        case OOT_NTSC_US_GC:
        case OOT_PAL_GC:
        case OOT_PAL_GC_DBG1:
        case OOT_PAL_GC_DBG2:
        case OOT_IQUE_CN:
        case OOT_IQUE_TW:
        default:
            return false;
        case OOT_NTSC_JP_MQ:
        case OOT_NTSC_US_MQ:
        case OOT_PAL_MQ:
        case OOT_PAL_GC_MQ_DBG:
            return true;
    }
}

ZRom::ZRom(std::string romPath)
{
	RomVersion version;
	romData = DiskFile::ReadAllBytes(romPath);

	BitConverter::RomToBigEndian(romData.data(), romData.size());

	version.crc = BitConverter::ToInt32BE(romData, 0x10);

	switch (version.crc)
	{
	case OOT_NTSC_10:
		version.version = "N64 NTSC 1.0";
		version.listPath = "ntsc_oot.txt";
		version.offset = OOT_OFF_NTSC_10;
		break;
	case OOT_NTSC_11:
		version.version = "N64 NTSC 1.1";
		version.listPath = "ntsc_oot.txt";
		version.offset = OOT_OFF_NTSC_11;
		break;
	case OOT_NTSC_12:
		version.version = "N64 NTSC 1.2";
		version.listPath = "ntsc_oot.txt";
		version.offset = OOT_OFF_NTSC_12;
		break;
	case OOT_PAL_10:
		version.version = "N64 PAL 1.0";
		version.listPath = "pal_oot.txt";
		version.offset = OOT_OFF_PAL_10;
		break;
	case OOT_PAL_11:
		version.version = "N64 PAL 1.1";
		version.listPath = "pal_oot.txt";
		version.offset = OOT_OFF_PAL_11;
		break;
	case OOT_NTSC_JP_GC:
		version.version = "JP GameCube (MQ Disk)";
		version.listPath = "gamecube.txt";
		version.offset = OOT_OFF_JP_GC;
		break;
	case OOT_NTSC_JP_GC_CE:
		version.version = "GameCube (Collectors Edition Disk)";
		version.listPath = "gamecube.txt";
		version.offset = OOT_OFF_JP_GC_CE;
		break;
	case OOT_NTSC_JP_MQ:
		version.version = "JP Master Quest";
		version.listPath = "gamecube.txt";
		version.offset = OOT_OFF_JP_MQ;
		break;
	case OOT_NTSC_US_MQ:
		version.version = "NTSC Master Quest";
		version.listPath = "gamecube.txt";
		version.offset = OOT_OFF_JP_MQ;
		break;
	case OOT_NTSC_US_GC:
		version.version = "NTSC GameCube";
		version.listPath = "gamecube.txt";
		version.offset = OOT_OFF_US_MQ;
		break;
	case OOT_PAL_GC:
		version.version = "PAL GameCube";
		version.listPath = "gamecube_pal.txt";
		version.offset = OOT_OFF_PAL_GC;
		break;
	case OOT_PAL_MQ:
		version.version = "PAL Master Quest";
		version.listPath = "gamecube_pal.txt";
		version.offset = OOT_OFF_PAL_MQ;
		break;
	case OOT_PAL_GC_DBG1:
		version.version = "GameCube Debug 1.0";
		version.listPath = "dbg.txt";
		version.offset = OOT_OFF_PAL_GC_DBG1;
		break;
	case OOT_PAL_GC_DBG2:
		version.version = "GameCube Debug 2.0";
		version.listPath = "dbg.txt";
		version.offset = OOT_OFF_PAL_GC_DBG2;
		break;
	case OOT_PAL_GC_MQ_DBG:
		version.version = "GameCube MQ-Debug";
		version.listPath = "dbg.txt";
		version.offset = OOT_OFF_PAL_MQ_DBG;
		break;
	case OOT_IQUE_CN:
		version.version = "OoT IQue";
		version.listPath = "ique.txt";
		version.offset = OOT_OFF_CN_IQUE;
		break;
	case OOT_IQUE_TW:
		version.version = "TW IQue";
		version.listPath = "ique.txt";
		version.offset = OOT_OFF_TW_IQUE;
		break;
	}

	auto path = StringHelper::Sprintf("%s/%s", Globals::Instance->fileListPath.string().c_str(), version.listPath.c_str());
	auto txt = DiskFile::ReadAllText(path);
	std::vector<std::string> lines = StringHelper::Split(txt, "\n");

    std::vector<uint8_t> decompressedData(1);

	for (int i = 0; i < lines.size(); i++)
	{
		lines[i] = StringHelper::Strip(lines[i], "\r");
		const int romOffset = version.offset + (DMA_ENTRY_SIZE * i);

		const int virtStart = BitConverter::ToInt32BE(romData, romOffset + 0);
		const int virtEnd = BitConverter::ToInt32BE(romData, romOffset + 4);
		const int physStart = BitConverter::ToInt32BE(romData, romOffset + 8);
		const int physEnd = BitConverter::ToInt32BE(romData, romOffset + 12);

		const bool compressed = physEnd != 0;
		int size = compressed ? physEnd - physStart : virtEnd - virtStart;

		auto outData = std::vector<uint8_t>();
		outData.resize(size);
		memcpy(outData.data(), romData.data() + physStart, size);

		if (compressed)
		{
			int decSize = virtEnd - virtStart;
			decompressedData = std::vector<uint8_t>();
			decompressedData.resize(decSize);
			yaz0_decode(outData.data(), decompressedData.data(), decSize);
			files[lines[i]] = decompressedData;
		}
		else
			files[lines[i]] = outData;

		//DiskFile::WriteAllBytes(StringHelper::Sprintf("baserom/%s", lines[i].c_str()), files[lines[i]]);
	}
}

std::vector<uint8_t> ZRom::GetFile(std::string fileName)
{
	return files[fileName];
}
