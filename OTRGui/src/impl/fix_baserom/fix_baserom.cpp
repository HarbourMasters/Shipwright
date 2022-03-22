#include "fix_baserom.h"

#include <cstdlib>
#include <cstdio>

#ifndef _MSC_VER
#include <byteswap.h>
#endif

#include "md5/md5.h"

#define ROM_SIZE 0x3600000

#if defined (_MSC_VER)
#define __bswap_32  _byteswap_ulong
#define bswap_32 _byteswap_ulong
#define __bswap_16 _byteswap_ushort
#endif

int fix_baserom(const char* baserom_path, const char* output_path) {
    FILE* currentFile = fopen(baserom_path, "rb");
    char md5[16];

    if (currentFile != nullptr) md5File(currentFile, md5);

    currentFile = fopen(baserom_path, "rb");
    if (currentFile != NULL) {
        uint8_t firstByte = 0x00;

        if (fread(&firstByte, sizeof(uint8_t), 1, currentFile) != 1) return 1;
        fseek(currentFile, 0, SEEK_SET);
        FILE* outFile = fopen(output_path, "wb");

        // Little Endian N64 Rom

        if (firstByte == 0x40) { 
            uint32_t currentWord;

            for (uint32_t i = 0; i < ROM_SIZE / 4; i++) {
                if (fread(&currentWord, sizeof(uint32_t), 1, currentFile) != 1) {
                    fprintf(stderr, "Failed to read data from rom file\n");
                }
                currentWord = __bswap_32(currentWord);
                if (fwrite(&currentWord, sizeof(uint32_t), 1, outFile) != 1) {
                    fprintf(stderr, "Failed to write to new rom file.\n");
                }
            }
        } else if (firstByte == 0x37) { // Byte Swapped
            uint16_t currentShort;

            for (uint32_t i = 0; i < ROM_SIZE / 2; i++) {
                if (fread(&currentShort, sizeof(uint16_t), 1, currentFile) != 1) {
                    fprintf(stderr, "Failed to read data from rom file\n");
                    return 1;
                }
                currentShort = __bswap_16(currentShort);
                if (fread(&currentShort, sizeof(uint16_t), 1, outFile) != 1) {
                    fprintf(stderr, "Failed to write to new rom file.\n");
                    return 1;
                }
            }
        } else { // Big Endian
            void* data = malloc(ROM_SIZE);
            if (data == NULL) {
                fprintf(stderr, "Failed to allocate memory for new baserom.\n");
                return 1;
            }
            if (fread(data, ROM_SIZE, 1, currentFile) != 1) {
                fprintf(stderr, "Failed to read data from rom file\n");
                return 1;
            }
            if (fread(data, ROM_SIZE, 1, outFile) != 1) {
                fprintf(stderr, "Failed to write to new rom file.\n");
            }
            free(data);
        }
        fseek(outFile, 0x3E, SEEK_SET);
        fputc('P', outFile); // Patch the rom header to be 'PAL'
        fclose(outFile);
        fclose(currentFile);
    }
}