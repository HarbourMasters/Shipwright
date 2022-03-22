#include "global.h"

#define BLOCKSIZE 32

s32 __osPfsLastChannel = -1;

s32 __osContRamRead(OSMesgQueue* ctrlrqueue, s32 channel, u16 addr, u8* data) {
    s32 ret;
    s32 i;
    u8* bufptr;
    s32 retryCount = 2;

    __osSiGetAccess();
    do {
        bufptr = (u8*)&gPifMempakBuf;

        if ((__osContLastPoll != 2) || (__osPfsLastChannel != channel)) {
            __osContLastPoll = 2;
            __osPfsLastChannel = channel;
            // clang-format off
            for (i = 0; i < channel; i++) { *bufptr++ = 0; }
            // clang-format on
            gPifMempakBuf.status = 1;
            ((__OSContRamHeader*)bufptr)->unk_00 = 0xFF;
            ((__OSContRamHeader*)bufptr)->txsize = 3;
            ((__OSContRamHeader*)bufptr)->rxsize = 0x21;
            ((__OSContRamHeader*)bufptr)->poll = CONT_CMD_READ_MEMPACK; // read mempak; send byte 0
            ((__OSContRamHeader*)bufptr)->datacrc = 0xFF;               // read mempak; send byte 0
            // Received bytes are 6-26 inclusive
            bufptr[sizeof(__OSContRamHeader)] = CONT_CMD_END; // End of commands
        } else {
            bufptr += channel;
        }

        ((__OSContRamHeader*)bufptr)->hi = addr >> 3;                                    // send byte 1
        ((__OSContRamHeader*)bufptr)->lo = (s8)(__osContAddressCrc(addr) | (addr << 5)); // send byte 2
        __osSiRawStartDma(OS_WRITE, &gPifMempakBuf);
        osRecvMesg(ctrlrqueue, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &gPifMempakBuf);
        osRecvMesg(ctrlrqueue, NULL, OS_MESG_BLOCK);

        ret = (((__OSContRamHeader*)bufptr)->rxsize & 0xC0) >> 4;
        if (!ret) {
            if (((__OSContRamHeader*)bufptr)->datacrc != __osContDataCrc(bufptr + 6)) {
                ret = __osPfsGetStatus(ctrlrqueue, channel);
                if (ret) {
                    break;
                }
                ret = 4; // Retry
            } else {
                bcopy(bufptr + 6, data, BLOCKSIZE);
            }
        } else {
            ret = 1; // Error
        }
        if (ret != 4) {
            break;
        }
    } while (0 <= retryCount--);
    __osSiRelAccess();

    return ret;
}
