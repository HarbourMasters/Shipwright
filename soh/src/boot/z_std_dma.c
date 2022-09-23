#include "global.h"
#include "vt.h"

StackEntry sDmaMgrStackInfo;
OSMesgQueue sDmaMgrMsgQueue;
OSMesg sDmaMgrMsgs[0x20];
OSThread sDmaMgrThread;
u8 sDmaMgrStack[0x500];
const char* sDmaMgrCurFileName;
s32 sDmaMgrCurFileLine;

u32 D_80009460 = 0;
u32 gDmaMgrDmaBuffSize = 0x2000;
u32 sDmaMgrIsRomCompressed = false;

// dmadata filenames
#define DEFINE_DMA_ENTRY(name) #name,

const char* sDmaMgrFileNames[] = {
#include "tables/dmadata_table.h"
};

#undef DEFINE_DMA_ENTRY

s32 DmaMgr_CompareName(const char* name1, const char* name2) {
    while (*name1 != 0u) {
        if (*name1 > *name2) {
            return 1;
        }

        if (*name1 < *name2) {
            return -1;
        }

        name1++;
        name2++;
    }

    if (*name2 > 0) {
        return -1;
    }

    return 0;
}

s32 DmaMgr_DmaRomToRam(uintptr_t rom, uintptr_t ram, size_t size) {
    OSIoMesg ioMsg;
    OSMesgQueue queue;
    OSMesg msg;
    s32 ret;
    u32 buffSize = gDmaMgrDmaBuffSize;
    s32 pad[2];

    if (buffSize == 0) {
        buffSize = 0x2000;
    }

    osInvalICache((void*)ram, size);
    osInvalDCache((void*)ram, size);
    osCreateMesgQueue(&queue, &msg, 1);

    while (size > buffSize) {

        ioMsg.hdr.pri = OS_MESG_PRI_NORMAL;
        ioMsg.hdr.retQueue = &queue;
        ioMsg.devAddr = rom;
        ioMsg.dramAddr = (void*)ram;
        ioMsg.size = buffSize;

        if (D_80009460 == 10) {
            osSyncPrintf("%10lld ノーマルＤＭＡ %08x %08x %08x (%d)\n", OS_CYCLES_TO_USEC(osGetTime()), ioMsg.dramAddr,
                         ioMsg.devAddr, ioMsg.size, gPiMgrCmdQ.validCount);
        }

        ret = osEPiStartDma(gCartHandle, &ioMsg, OS_READ);
        if (ret) {
            goto end;
        }

        if (D_80009460 == 10) {
            osSyncPrintf("%10lld ノーマルＤＭＡ START (%d)\n", OS_CYCLES_TO_USEC(osGetTime()), gPiMgrCmdQ.validCount);
        }

        osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
        if (D_80009460 == 10) {
            osSyncPrintf("%10lld ノーマルＤＭＡ END (%d)\n", OS_CYCLES_TO_USEC(osGetTime()), gPiMgrCmdQ.validCount);
        }

        size -= buffSize;
        rom += buffSize;
        ram += buffSize;
    }


    ioMsg.hdr.pri = OS_MESG_PRI_NORMAL;
    ioMsg.hdr.retQueue = &queue;
    ioMsg.devAddr = rom;
    ioMsg.dramAddr = (void*)ram;
    ioMsg.size = size;

    if (D_80009460 == 10) {
        osSyncPrintf("%10lld ノーマルＤＭＡ %08x %08x %08x (%d)\n", OS_CYCLES_TO_USEC(osGetTime()), ioMsg.dramAddr,
                     ioMsg.devAddr, ioMsg.size, gPiMgrCmdQ.validCount);
    }

    ret = osEPiStartDma(gCartHandle, &ioMsg, OS_READ);
    if (ret) {
        goto end;
    }

    osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
    if (D_80009460 == 10) {
        osSyncPrintf("%10lld ノーマルＤＭＡ END (%d)\n", OS_CYCLES_TO_USEC(osGetTime()), gPiMgrCmdQ.validCount);
    }

end:
    osInvalICache((void*)ram, size);
    osInvalDCache((void*)ram, size);

    return ret;
}

s32 DmaMgr_DmaHandler(OSPiHandle* pihandle, OSIoMesg* mb, s32 direction) {
    s32 ret;

    ASSERT(pihandle == gCartHandle);
    ASSERT(direction == OS_READ);
    ASSERT(mb != NULL);

    if (D_80009460 == 10) {
        osSyncPrintf("%10lld サウンドＤＭＡ %08x %08x %08x (%d)\n", OS_CYCLES_TO_USEC(osGetTime()), mb->dramAddr,
                     mb->devAddr, mb->size, gPiMgrCmdQ.validCount);
    }

    ret = osEPiStartDma(pihandle, mb, direction);
    if (ret) {
        osSyncPrintf("OOPS!!\n");
    }
    return ret;
}

void DmaMgr_DmaFromDriveRom(uintptr_t ram, uintptr_t rom, size_t size) {
    OSPiHandle* handle = osDriveRomInit();
    OSMesgQueue queue;
    OSMesg msg;
    OSIoMesg ioMsg;

    osInvalICache((void*)ram, size);
    osInvalDCache((void*)ram, size);
    osCreateMesgQueue(&queue, &msg, 1);

    ioMsg.hdr.retQueue = &queue;
    ioMsg.hdr.pri = OS_MESG_PRI_NORMAL;
    ioMsg.devAddr = rom;
    ioMsg.dramAddr = (void*)ram;
    ioMsg.size = size;
    handle->transferInfo.cmdType = 2;

    osEPiStartDma(handle, &ioMsg, OS_READ);
    osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
    return;
}

void DmaMgr_Error(DmaRequest* req, const char* file, const char* errorName, const char* errorDesc) {
    uintptr_t vrom = req->vromAddr;
    uintptr_t ram = (uintptr_t)req->dramAddr;
    size_t size = req->size;
    char buff1[80];
    char buff2[80];

    osSyncPrintf("%c", 7);
    osSyncPrintf(VT_FGCOL(RED));
    osSyncPrintf("DMA致命的エラー(%s)\nROM:%X RAM:%X SIZE:%X %s\n",
                 errorDesc != NULL ? errorDesc : (errorName != NULL ? errorName : "???"), vrom, ram, size,
                 file != NULL ? file : "???");

    if (req->filename) {
        osSyncPrintf("DMA ERROR: %s %d", req->filename, req->line);
    } else if (sDmaMgrCurFileName) {
        osSyncPrintf("DMA ERROR: %s %d", sDmaMgrCurFileName, sDmaMgrCurFileLine);
    }

    osSyncPrintf(VT_RST);

    if (req->filename) {
        sprintf(buff1, "DMA ERROR: %s %d", req->filename, req->line);
    } else if (sDmaMgrCurFileName) {
        sprintf(buff1, "DMA ERROR: %s %d", sDmaMgrCurFileName, sDmaMgrCurFileLine);
    } else {
        sprintf(buff1, "DMA ERROR: %s", errorName != NULL ? errorName : "???");
    }

    sprintf(buff2, "%07X %08X %X %s", vrom, ram, size, file != NULL ? file : "???");
    Fault_AddHungupAndCrashImpl(buff1, buff2);
}

const char* DmaMgr_GetFileNameImpl(uintptr_t vrom) {
    DmaEntry* iter = gDmaDataTable;
    const char** name = sDmaMgrFileNames;

    while (iter->vromEnd) {
        if (vrom >= iter->vromStart && vrom < iter->vromEnd) {
            return *name;
        }

        iter++;
        name++;
    }
    //! @bug Since there is no return, in case the file isn't found, the return value will be a pointer to the end
    // of gDmaDataTable
}

const char* DmaMgr_GetFileName(uintptr_t vrom) {
    const char* ret = DmaMgr_GetFileNameImpl(vrom);

    if (ret == NULL) {
        return "(unknown)";
    }

    if (DmaMgr_CompareName(ret, "kanji") == 0 || DmaMgr_CompareName(ret, "link_animetion") == 0) {
        return NULL;
    }

    return ret;
}

void DmaMgr_ProcessMsg(DmaRequest* req) {
    uintptr_t vrom = req->vromAddr;
    void* ram = req->dramAddr;
    size_t size = req->size;
    uintptr_t romStart;
    uintptr_t romSize;
    u8 found = false;
    DmaEntry* iter;
    const char* filename;

    if (0) {
        // the string is defined in .rodata but not used, suggesting
        // a debug print is here but optimized out in some way
        osSyncPrintf("DMA ROM:%08X RAM:%08X SIZE:%08X %s\n");
        // the last arg of this print looks like it may be filename, but
        // filename above this block does not match
    }

    filename = DmaMgr_GetFileName(vrom);
    iter = gDmaDataTable;

    while (iter->vromEnd) {
        if (vrom >= iter->vromStart && vrom < iter->vromEnd) {

            if (iter->romEnd == 0) {
                if (iter->vromEnd < vrom + size) {
                    DmaMgr_Error(req, filename, "Segment Alignment Error",
                                 "セグメント境界をまたがってＤＭＡ転送することはできません");
                }

                DmaMgr_DmaRomToRam(iter->romStart + (vrom - iter->vromStart), ram, size);
                found = true;

                if (0) {
                    osSyncPrintf("No Press ROM:%08X RAM:%08X SIZE:%08X\n", vrom, ram, size);
                }
            } else {
                romStart = iter->romStart;
                romSize = iter->romEnd - iter->romStart;

                if (vrom != iter->vromStart) {
                    DmaMgr_Error(req, filename, "Can't Transfer Segment",
                                 "圧縮されたセグメントの途中からはＤＭＡ転送することはできません");
                }

                if (size != iter->vromEnd - iter->vromStart) {
                    DmaMgr_Error(req, filename, "Can't Transfer Segment",
                                 "圧縮されたセグメントの一部だけをＤＭＡ転送することはできません");
                }

                osSetThreadPri(NULL, Z_PRIORITY_MAIN);
                Yaz0_Decompress(romStart, ram, romSize);
                osSetThreadPri(NULL, Z_PRIORITY_DMAMGR);
                found = true;

                if (0) {
                    osSyncPrintf("   Press ROM:%X RAM:%X SIZE:%X\n", vrom, ram, size);
                }
            }
            break;
        }
        iter++;
    }

    if (!found) {
        if (sDmaMgrIsRomCompressed) {
            DmaMgr_Error(req, NULL, "DATA DON'T EXIST", "該当するデータが存在しません");
            return;
        }

        DmaMgr_DmaRomToRam(vrom, (uintptr_t)ram, size);

        if (0) {
            osSyncPrintf("No Press ROM:%08X RAM:%08X SIZE:%08X (非公式)\n", vrom, ram, size);
        }
    }
}

void DmaMgr_ThreadEntry(void* arg0) {
    OSMesg msg;
    DmaRequest* req;

    osSyncPrintf("ＤＭＡマネージャスレッド実行開始\n");
    while (true) {
        osRecvMesg(&sDmaMgrMsgQueue, &msg, OS_MESG_BLOCK);
        req = (DmaRequest*)msg.ptr;
        if (req == NULL) {
            break;
        }

        if (0) {
            osSyncPrintf("ＤＭＡ登録受付 dmap=%08x\n", req);
        }

        DmaMgr_ProcessMsg(req);
        if (req->notifyQueue) {
            osSendMesg(req->notifyQueue, req->notifyMsg, OS_MESG_NOBLOCK);
            if (0) {
                osSyncPrintf("osSendMesg: dmap=%08x, mq=%08x, m=%08x \n", req, req->notifyQueue, req->notifyMsg);
            }
        }
    }
    osSyncPrintf("ＤＭＡマネージャスレッド実行終了\n");
}

s32 DmaMgr_SendRequestImpl(DmaRequest* req, uintptr_t ram, uintptr_t vrom, size_t size, u32 unk, OSMesgQueue* queue, OSMesg msg) {
    static s32 sDmaMgrQueueFullLogged = 0;

    if ((1 && (ram == 0)) || (osMemSize < ram + size + 0x80000000) || (vrom & 1) || (vrom > 0x4000000) || (size == 0) ||
        (size & 1)) {
        //! @bug `req` is passed to `DmaMgr_Error` without rom, ram and size being set
        DmaMgr_Error(req, NULL, "ILLIGAL DMA-FUNCTION CALL", "パラメータ異常です");
    }

    req->vromAddr = vrom;
    req->dramAddr = (void*)ram;
    req->size = size;
    req->unk_14 = 0;
    req->notifyQueue = queue;
    req->notifyMsg = msg;

    if (1) {
        if ((sDmaMgrQueueFullLogged == 0) && (sDmaMgrMsgQueue.validCount >= sDmaMgrMsgQueue.msgCount)) {
            sDmaMgrQueueFullLogged++;
            osSyncPrintf("%c", 7);
            osSyncPrintf(VT_FGCOL(RED));
            osSyncPrintf("dmaEntryMsgQが一杯です。キューサイズの再検討をおすすめします。");
            LOG_NUM("(sizeof(dmaEntryMsgBufs) / sizeof(dmaEntryMsgBufs[0]))", ARRAY_COUNT(sDmaMgrMsgs));
            osSyncPrintf(VT_RST);
        }
    }

    osSendMesgPtr(&sDmaMgrMsgQueue, req, OS_MESG_BLOCK);
    return 0;
}

s32 DmaMgr_SendRequest0(uintptr_t ram, uintptr_t vrom, size_t size) {
    DmaRequest req;
    OSMesgQueue queue;
    OSMesg msg;
    s32 ret;

    osCreateMesgQueue(&queue, &msg, 1);
    ret = DmaMgr_SendRequestImpl(&req, ram, vrom, size, 0, &queue, OS_MESG_PTR(NULL));
    if (ret == -1) {
        return ret;
    }

    osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
    return 0;
}

void DmaMgr_Init(void) {
    const char** name;
    s32 idx;
    DmaEntry* iter;

    //DmaMgr_DmaRomToRam((uintptr_t)_dmadataSegmentRomStart, (uintptr_t)_dmadataSegmentStart,
                       //(uintptr_t)(_dmadataSegmentRomEnd - _dmadataSegmentRomStart));
    osSyncPrintf("dma_rom_ad[]\n");

    sDmaMgrIsRomCompressed = false;
    name = sDmaMgrFileNames;
    iter = gDmaDataTable;
    idx = 0;

    while (iter->vromEnd != 0) {
        if (iter->romEnd != 0) {
            sDmaMgrIsRomCompressed = true;
        }

        osSyncPrintf(
            "%3d %08x %08x %08x %08x %08x %c %s\n", idx, iter->vromStart, iter->vromEnd, iter->romStart, iter->romEnd,
            (iter->romEnd != 0) ? iter->romEnd - iter->romStart : iter->vromEnd - iter->vromStart,
            (((iter->romEnd != 0) ? iter->romEnd - iter->romStart : 0) > 0x10000) ? '*' : ' ', name ? *name : "");

        idx++;
        iter++;

        if (name) {
            name++;
        }
    }

#if 0
    if ((uintptr_t)_bootSegmentRomStart != gDmaDataTable[0].vromEnd)
    {
        osSyncPrintf("_bootSegmentRomStart(%08x) != dma_rom_ad[0].rom_b(%08x)\n", _bootSegmentRomStart,
                     gDmaDataTable[0].vromEnd);
        Fault_AddHungupAndCrash(__FILE__, __LINE__);
    }
#endif

    osCreateMesgQueue(&sDmaMgrMsgQueue, sDmaMgrMsgs, ARRAY_COUNT(sDmaMgrMsgs));
    StackCheck_Init(&sDmaMgrStackInfo, sDmaMgrStack, sDmaMgrStack + sizeof(sDmaMgrStack), 0, 0x100, "dmamgr");
    osCreateThread(&sDmaMgrThread, 0x12, DmaMgr_ThreadEntry, 0, sDmaMgrStack + sizeof(sDmaMgrStack), Z_PRIORITY_DMAMGR);
    osStartThread(&sDmaMgrThread);
}

s32 DmaMgr_SendRequest2(DmaRequest* req, uintptr_t ram, uintptr_t vrom, size_t size, u32 unk5, OSMesgQueue* queue, OSMesg msg,
                        const char* file, s32 line) {
#if 0
    req->filename = file;
    req->line = line;
    DmaMgr_SendRequestImpl(req, ram, vrom, size, unk5, queue, msg);
#endif
}

s32 DmaMgr_SendRequest1(void* ram0, uintptr_t vrom, size_t size, const char* file, s32 line) {
    //printf("DmaMgr_SendRequest1 called...\n");
    return 0;

#if 0
    DmaRequest req;
    s32 ret;
    OSMesgQueue queue;
    OSMesg msg;
    uintptr_t ram = (uintptr_t)ram0;

    req.filename = file;
    req.line = line;
    osCreateMesgQueue(&queue, &msg, 1);
    ret = DmaMgr_SendRequestImpl(&req, ram, vrom, size, 0, &queue, 0);
    if (ret == -1) {
        return ret;
    }

    osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
    return 0;
#endif
}
