#include "ultra64.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#if defined(WIN32)
#define DIR_SEPARATOR '\\'
#else
#define DIR_SEPARATOR '/'
#endif

#if 0
typedef struct {
    /* 0x00 */ OSPiHandle piHandle;
    /* 0x74 */ OSIoMesg ioMesg;
    /* 0x8C */ OSMesgQueue mesgQ;
} SsSramContext; // size = 0xA4

SsSramContext sSsSramContext = { 0 };


    
void SsSram_Init(uintptr_t addr, u8 handleType, u8 handleDomain, u8 handleLatency, u8 handlePageSize, u8 handleRelDuration,
                 u8 handlePulse, u32 handleSpeed) {
    u32 prevInt;
    OSPiHandle* handle = &sSsSramContext.piHandle;

    if ((uintptr_t)OS_PHYSICAL_TO_K1(addr) != (*handle).baseAddress) {
        sSsSramContext.piHandle.type = handleType;
        (*handle).baseAddress = OS_PHYSICAL_TO_K1(addr);
        sSsSramContext.piHandle.latency = handleLatency;
        sSsSramContext.piHandle.pulse = handlePulse;
        sSsSramContext.piHandle.pageSize = handlePageSize;
        sSsSramContext.piHandle.relDuration = handleRelDuration;
        sSsSramContext.piHandle.domain = handleDomain;
        sSsSramContext.piHandle.speed = handleSpeed;

        bzero(&sSsSramContext.piHandle.transferInfo, sizeof(__OSTranxInfo));

        prevInt = __osDisableInt();
        //sSsSramContext.piHandle.next = __osPiTable;
        //__osPiTable = &sSsSramContext.piHandle;
        __osRestoreInt(prevInt);

        sSsSramContext.ioMesg.hdr.pri = OS_MESG_PRI_NORMAL;
        sSsSramContext.ioMesg.hdr.retQueue = &sSsSramContext.mesgQ;
        sSsSramContext.ioMesg.devAddr = addr;
    }
}

void SsSram_Dma(void* dramAddr, size_t size, s32 direction) {
    OSMesg mesg;

    osCreateMesgQueue(&sSsSramContext.mesgQ, &mesg, 1);
    sSsSramContext.ioMesg.dramAddr = dramAddr;
    sSsSramContext.ioMesg.size = size;
    osWritebackDCache(dramAddr, size);
    osEPiStartDma(&sSsSramContext.piHandle, &sSsSramContext.ioMesg, direction);
    osRecvMesg(&sSsSramContext.mesgQ, &mesg, OS_MESG_BLOCK);
    osInvalDCache(dramAddr, size);
}
#endif

//https://stackoverflow.com/a/15019112
void combineDirectoryAndFileName(char* destination, char* directory, char* fileName)
{
    if(directory && *directory) {
        int len = strlen(directory);
        strcpy(destination, directory);

        if (destination[len - 1] == DIR_SEPARATOR)
        {
            if (fileName && *fileName)
            {
                strcpy(destination + len, (*fileName == DIR_SEPARATOR) ? (fileName + 1) : fileName);
            }
        }
        else
        {
            if(fileName && *fileName)
            {
                if (*fileName == DIR_SEPARATOR)
                {
                    strcpy(destination + len, fileName);
                }
                else
                {
                    destination[len] = DIR_SEPARATOR;
                    strcpy(destination + len + 1, fileName);
                }
            }
        }
    }
    else if(fileName && *fileName)
    {
        strcpy(destination, fileName);
    }
    else
    {
        destination[0] = '\0';
    }
}

void SsSram_ReadWrite(uintptr_t addr, void* dramAddr, size_t size, s32 direction) {
    osSyncPrintf("ssSRAMReadWrite:%08x %08x %08x %d\n", addr, (uintptr_t)dramAddr, size, direction);
    
    char* directory = Config_getValue("SAVE", "Save File Directory");
    char* fileName = Config_getValue("SAVE", "Save Filename");
     
    // This only happens if a user deletes the default filename and doesn't replace it
    if(fileName[0] == '\0')
    {
        Config_setValue("SAVE", "Save Filename", "oot_save.sav");
        free(fileName);
        fileName = malloc(sizeof("oot_save.sav")+1);
        strcpy(fileName, "oot_save.sav");
    }

    char* file = malloc(strlen(directory) + strlen(fileName) + 2);
    combineDirectoryAndFileName(file, directory, fileName);

    //Check to see if the file exists
    FILE* saveFile;
	saveFile = fopen(file, "rb");

    if (saveFile == NULL) {

        saveFile = fopen(file, "wb");
        fseek(saveFile, 0, SEEK_SET);
        assert(saveFile != NULL); // OTRTODO LOG
        uint8_t zero = 0;

        for (uint32_t i = 0; i < SRAM_SIZE; i++) {
            fwrite(&zero, 1, 1, saveFile);
        }
        fclose(saveFile);
    } else {
        fclose(saveFile);
    }
    switch (direction) { 
        case OS_WRITE: {
            saveFile = fopen(file, "r+b");
            rewind(saveFile);
            fseek(saveFile, addr, SEEK_SET);
            fwrite(dramAddr, size, 1, saveFile);
            fclose(saveFile);
        } break;
        case OS_READ: {
            saveFile = fopen(file, "rb+");
            rewind(saveFile);
            fseek(saveFile, addr, SEEK_SET);
            fread(dramAddr, size, 1, saveFile);
            fclose(saveFile);
        } break;
    }
    //SsSram_Init(addr, DEVICE_TYPE_SRAM, PI_DOMAIN2, 5, 0xD, 2, 0xC, 0);
    //SsSram_Dma(dramAddr, size, direction);

    free(file);
    free(directory);
    free(fileName);
}
