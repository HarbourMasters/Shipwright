#include "ultra64.h"
#include "PR/controller.h"
#include "PR/voiceinternal.h"

s32 __osContRamWrite(OSMesgQueue* mq, s32 channel, u16 address, u8* buffer, s32 force) {
    s32 ret = 0;
    s32 i;
    u8* ptr;
    s32 retry = 2;
    u8 crc;

    if ((force != PFS_FORCE) && (address < PFS_LABEL_AREA) && (address != 0)) {
        return 0;
    }

    __osSiGetAccess();

    do {
        ptr = (u8*)(&__osPfsPifRam);

        if (__osContLastPoll != CONT_CMD_WRITE_MEMPACK || __osPfsLastChannel != channel) {
            __osContLastPoll = CONT_CMD_WRITE_MEMPACK;
            __osPfsLastChannel = channel;

            // clang-format off
            for (i = 0; i < channel; i++) { *ptr++ = 0; }
            // clang-format on

            __osPfsPifRam.status = 1;

            ((__OSContRamReadFormat*)ptr)->dummy = 0xFF;
            ((__OSContRamReadFormat*)ptr)->txsize = 35;
            ((__OSContRamReadFormat*)ptr)->rxsize = 1;
            ((__OSContRamReadFormat*)ptr)->cmd = CONT_CMD_WRITE_MEMPACK;
            ((__OSContRamReadFormat*)ptr)->datacrc = 0xFF;

            ptr[sizeof(__OSContRamReadFormat)] = CONT_CMD_END;
        } else {
            ptr += channel;
        }
        ((__OSContRamReadFormat*)ptr)->hi = address >> 3;
        ((__OSContRamReadFormat*)ptr)->lo = ((address << 5) | __osContAddressCrc(address));

        bcopy(buffer, ((__OSContRamReadFormat*)ptr)->data, BLOCKSIZE);

        ret = __osSiRawStartDma(OS_WRITE, &__osPfsPifRam);
        crc = __osContDataCrc(buffer);
        osRecvMesg(mq, (OSMesg*)NULL, OS_MESG_BLOCK);

        ret = __osSiRawStartDma(OS_READ, &__osPfsPifRam);
        osRecvMesg(mq, (OSMesg*)NULL, OS_MESG_BLOCK);

        ret = ((((__OSContRamReadFormat*)ptr)->rxsize & 0xC0) >> 4);
        if (!ret) {
            if (crc != ((__OSContRamReadFormat*)ptr)->datacrc) {
                if ((ret = __osPfsGetStatus(mq, channel))) {
                    break;
                } else {
                    ret = PFS_ERR_CONTRFAIL;
                }
            }
        } else {
            ret = PFS_ERR_NOPACK;
        }
    } while ((ret == PFS_ERR_CONTRFAIL) && (retry-- >= 0));
    __osSiRelAccess();

    return ret;
}
