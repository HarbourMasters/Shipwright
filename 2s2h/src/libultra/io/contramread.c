#include "ultra64.h"
#include "PR/controller.h"
#include "PR/voiceinternal.h"

#define BLOCKSIZE 32

s32 __osPfsLastChannel = -1;

s32 __osContRamRead(OSMesgQueue* ctrlrqueue, s32 channel, u16 addr, u8* buffer) {
    s32 ret;
    s32 i;
    u8* bufptr;
    s32 retryCount = 2;

    __osSiGetAccess();
    do {
        bufptr = (u8*)&__osPfsPifRam;

        if ((__osContLastPoll != 2) || (__osPfsLastChannel != channel)) {
            __osContLastPoll = 2;
            __osPfsLastChannel = channel;
            // clang-format off
            for (i = 0; i < channel; i++) { *bufptr++ = 0; }
            // clang-format on
            __osPfsPifRam.status = 1;
            ((__OSContRamReadFormat*)bufptr)->dummy = 0xFF;
            ((__OSContRamReadFormat*)bufptr)->txsize = 3;
            ((__OSContRamReadFormat*)bufptr)->rxsize = 0x21;
            ((__OSContRamReadFormat*)bufptr)->cmd = CONT_CMD_READ_MEMPACK; // read mempak; send byte 0
            ((__OSContRamReadFormat*)bufptr)->datacrc = 0xFF;              // read mempak; send byte 0
            // Received bytes are 6-26 inclusive
            bufptr[sizeof(__OSContRamReadFormat)] = CONT_CMD_END; // End of commands
        } else {
            bufptr += channel;
        }

        ((__OSContRamReadFormat*)bufptr)->hi = addr >> 3;                                    // send byte 1
        ((__OSContRamReadFormat*)bufptr)->lo = (s8)(__osContAddressCrc(addr) | (addr << 5)); // send byte 2
        __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        osRecvMesg(ctrlrqueue, NULL, OS_MESG_BLOCK);
        __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(ctrlrqueue, NULL, OS_MESG_BLOCK);

        ret = (((__OSContRamReadFormat*)bufptr)->rxsize & 0xC0) >> 4;
        if (!ret) {
            if (((__OSContRamReadFormat*)bufptr)->datacrc != __osContDataCrc(bufptr + 6)) {
                ret = __osPfsGetStatus(ctrlrqueue, channel);
                if (ret) {
                    break;
                }
                ret = 4; // Retry
            } else {
                bcopy(bufptr + 6, buffer, BLOCKSIZE);
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
