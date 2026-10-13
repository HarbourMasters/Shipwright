#include "z64jpeg.h"
#include "libc/stdbool.h"
#include "variables.h"
#include "functions.h"

#define MARKER_ESCAPE 0x00
#define MARKER_SOI 0xD8
#define MARKER_SOF 0xC0
#define MARKER_DHT 0xC4
#define MARKER_DQT 0xDB
#define MARKER_DRI 0xDD
#define MARKER_SOS 0xDA
#define MARKER_APP0 0xE0
#define MARKER_APP1 0xE1
#define MARKER_APP2 0xE2
#define MARKER_COM 0xFE
#define MARKER_EOI 0xD9

extern u64 njpgdspMainTextStart[];
extern u64 njpgdspMainDataStart[];

/**
 * Configures and schedules a JPEG decoder task and waits for it to finish.
 */
void Jpeg_ScheduleDecoderTask(JpegContext* jpegCtx) {
#if 0
    static OSTask_t sJpegTask = {
        M_NJPEGTASK,                         // type
        0,                                   // flags
        NULL,                                // ucode_boot
        0,                                   // ucode_boot_size
        njpgdspMainTextStart,                // ucode
        SP_UCODE_SIZE,                       // ucode_size
        njpgdspMainDataStart,                // ucode_data
        SP_UCODE_DATA_SIZE,                  // ucode_data_size
        NULL,                                // dram_stack
        0,                                   // dram_stack_size
        NULL,                                // output_buff
        NULL,                                // output_buff_size
        NULL,                                // data_ptr
        sizeof(JpegTaskData),                // data_size
        NULL,                                // yield_data_ptr
        sizeof(jpegCtx->workBuf->yieldData), // yield_data_size
    };

    JpegWork* workBuf = jpegCtx->workBuf;
    s32 pad[2];

    workBuf->taskData.address = &workBuf->data;
    workBuf->taskData.mode = jpegCtx->mode;
    workBuf->taskData.mbCount = 4;
    workBuf->taskData.qTableYPtr = &workBuf->qTableY;
    workBuf->taskData.qTableUPtr = &workBuf->qTableU;
    workBuf->taskData.qTableVPtr = &workBuf->qTableV;

    sJpegTask.flags = 0;
    sJpegTask.ucodeBoot = SysUcode_GetUCodeBoot();
    sJpegTask.ucodeBootSize = SysUcode_GetUCodeBootSize();
    sJpegTask.yieldDataPtr = (u64*)&workBuf->yieldData;
    sJpegTask.dataPtr = (u64*)&workBuf->taskData;

    jpegCtx->scTask.next = NULL;
    jpegCtx->scTask.flags = OS_SC_NEEDS_RSP;
    jpegCtx->scTask.msgQ = &jpegCtx->mq;
    jpegCtx->scTask.msg = NULL;
    jpegCtx->scTask.framebuffer = NULL;
    jpegCtx->scTask.list.t = sJpegTask;

    osSendMesg(&gSchedContext.cmdQ, (OSMesg*)&jpegCtx->scTask, OS_MESG_BLOCK);
    Sched_SendEntryMsg(&gSchedContext); // osScKickEntryMsg
    osRecvMesg(&jpegCtx->mq, NULL, OS_MESG_BLOCK);
#endif
}

/**
 * Copies a 16x16 block of decoded image data to the Z-buffer.
 */
void Jpeg_CopyToZbuffer(u16* src, u16* zbuffer, s32 x, s32 y) {
    u16* dst = zbuffer + (((y * SCREEN_WIDTH) + x) * 16);
    s32 i;

    for (i = 0; i < 16; i++) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        dst[8] = src[8];
        dst[9] = src[9];
        dst[10] = src[10];
        dst[11] = src[11];
        dst[12] = src[12];
        dst[13] = src[13];
        dst[14] = src[14];
        dst[15] = src[15];

        src += 16;
        dst += SCREEN_WIDTH;
    }
}

/**
 * Reads an u16 from a possibly unaligned address in memory.
 *
 * Replaces unaligned 16-bit reads with a pair of aligned reads, allowing for reading the possibly
 * unaligned values in JPEG header files.
 */
u16 Jpeg_GetUnalignedU16(u8* ptr) {
    if (((uintptr_t)ptr & 1) == 0) {
        // Read the value normally if it's aligned to a 16-bit address.
        return *(u16*)ptr;
    } else {
        // Read unaligned values using two separate aligned memory accesses when it's not.
        return *(u16*)(ptr - 1) << 8 | (*(u16*)(ptr + 1) >> 8);
    }
}

/**
 * Parses the markers in the JPEG file, storing information such as the pointer to the image data
 * in `jpegCtx` for later processing.
 */
void Jpeg_ParseMarkers(u8* ptr, JpegContext* jpegCtx) {
    u32 exit = false;

    jpegCtx->dqtCount = 0;
    jpegCtx->dhtCount = 0;

    while (true) {
        if (exit) {
            break;
        }

        // 0xFF indicates the start of a JPEG marker, so look for the next.
        if (*ptr++ == 0xFF) {
            switch (*ptr++) {
                case MARKER_ESCAPE: {
                    // Compressed value 0xFF is stored as 0xFF00 to escape it, so ignore it.
                    break;
                }
                case MARKER_SOI: {
                    // Start of Image
                    break;
                }
                case MARKER_APP0: {
                    // Application marker for JFIF
                    ptr += Jpeg_GetUnalignedU16(ptr);
                    break;
                }
                case MARKER_APP1: {
                    // Application marker for EXIF
                    ptr += Jpeg_GetUnalignedU16(ptr);
                    break;
                }
                case MARKER_APP2: {
                    ptr += Jpeg_GetUnalignedU16(ptr);
                    break;
                }
                case MARKER_DQT: {
                    // Define Quantization Table, stored for later processing
                    jpegCtx->dqtPtr[jpegCtx->dqtCount++] = ptr + 2;
                    ptr += Jpeg_GetUnalignedU16(ptr);
                    break;
                }
                case MARKER_DHT: {
                    // Define Huffman Table, stored for later processing
                    jpegCtx->dhtPtr[jpegCtx->dhtCount++] = ptr + 2;
                    ptr += Jpeg_GetUnalignedU16(ptr);
                    break;
                }
                case MARKER_DRI: {
                    // Define Restart Interval
                    ptr += Jpeg_GetUnalignedU16(ptr);
                    break;
                }
                case MARKER_SOF: {
                    // Start of Frame, stores important metadata of the image.
                    // Only used for extracting the sampling factors (jpegCtx->mode).

                    if (ptr[9] == 0x21) {
                        // component Y : V0 == 1
                        jpegCtx->mode = 0;
                    } else if (ptr[9] == 0x22) {
                        // component Y : V0 == 2
                        jpegCtx->mode = 2;
                    }
                    ptr += Jpeg_GetUnalignedU16(ptr);
                    break;
                }
                case MARKER_SOS: {
                    // Start of Scan marker, indicates the start of the image data.
                    ptr += Jpeg_GetUnalignedU16(ptr);
                    jpegCtx->imageData = ptr;
                    break;
                }
                case MARKER_EOI: {
                    // End of Image
                    exit = true;
                    break;
                }
                default: {
                    ptr += Jpeg_GetUnalignedU16(ptr);
                    break;
                }
            }
        }
    }
}

s32 Jpeg_Decode(void* data, void* zbuffer, void* work, u32 workSize) {
    s32 y;
    s32 x;
    s32 j;
    s32 i;
    JpegContext jpegCtx;
    JpegHuffmanTable hTables[4];
    JpegDecoder decoder;
    JpegDecoderState state;
    JpegWork* workBuff = work;

    if (workSize < sizeof(JpegWork)) {
        return -1;
    }

    osCreateMesgQueue(&jpegCtx.mq, &jpegCtx.msg, 1);
    MsgEvent_SendNullTask();

    jpegCtx.workBuf = workBuff;

    Jpeg_ParseMarkers(data, &jpegCtx);

    switch (jpegCtx.dqtCount) {
        case 1:
            JpegUtils_ProcessQuantizationTable(jpegCtx.dqtPtr[0], &workBuff->qTableY, 3);
            break;

        case 2:
            JpegUtils_ProcessQuantizationTable(jpegCtx.dqtPtr[0], &workBuff->qTableY, 1);
            JpegUtils_ProcessQuantizationTable(jpegCtx.dqtPtr[1], &workBuff->qTableU, 1);
            JpegUtils_ProcessQuantizationTable(jpegCtx.dqtPtr[1], &workBuff->qTableV, 1);
            break;

        case 3:
            JpegUtils_ProcessQuantizationTable(jpegCtx.dqtPtr[0], &workBuff->qTableY, 1);
            JpegUtils_ProcessQuantizationTable(jpegCtx.dqtPtr[1], &workBuff->qTableU, 1);
            JpegUtils_ProcessQuantizationTable(jpegCtx.dqtPtr[2], &workBuff->qTableV, 1);
            break;

        default:
            return -1;
    }

    switch (jpegCtx.dhtCount) {
        case 1:
            if (JpegUtils_ProcessHuffmanTable(jpegCtx.dhtPtr[0], &hTables[0], workBuff->codesLengths, workBuff->codes,
                                              4)) {
                return -1;
            }
            break;

        case 4:
            // This chained if-else has printfs inside it on debug
            if (JpegUtils_ProcessHuffmanTable(jpegCtx.dhtPtr[0], &hTables[0], workBuff->codesLengths, workBuff->codes,
                                              1)) {
            } else if (JpegUtils_ProcessHuffmanTable(jpegCtx.dhtPtr[1], &hTables[1], workBuff->codesLengths,
                                                     workBuff->codes, 1)) {
            } else if (JpegUtils_ProcessHuffmanTable(jpegCtx.dhtPtr[2], &hTables[2], workBuff->codesLengths,
                                                     workBuff->codes, 1)) {
            } else if (!JpegUtils_ProcessHuffmanTable(jpegCtx.dhtPtr[3], &hTables[3], workBuff->codesLengths,
                                                      workBuff->codes, 1)) {
                break;
            }
            return -1;

        default:
            return -1;
    }

    decoder.imageData = jpegCtx.imageData;
    decoder.mode = jpegCtx.mode;
    decoder.unk_05 = 2;

    decoder.hTablePtrs[0] = &hTables[0];
    decoder.hTablePtrs[1] = &hTables[1];
    decoder.hTablePtrs[2] = &hTables[2];
    decoder.hTablePtrs[3] = &hTables[3];
    decoder.unk_18 = 0;

    x = y = 0;
    for (i = 0; i < 300; i += 4) {
        if (!JpegDecoder_Decode(&decoder, (u16*)workBuff->data, 4, (i != 0), &state)) {
            Jpeg_ScheduleDecoderTask(&jpegCtx);

            for (j = 0; j < 4; j++) {
                Jpeg_CopyToZbuffer(workBuff->data[j], zbuffer, x, y);
                x++;

                if (x >= 20) {
                    x = 0;
                    y++;
                }
            }
        }
    }

    return 0;
}
