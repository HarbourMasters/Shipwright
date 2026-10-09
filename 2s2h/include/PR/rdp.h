#ifndef PR_RDP_H
#define PR_RDP_H

#include <libultraship/libultra/rdp.h>
#if 0
/* DP Command Registers */
#define DPC_START_REG    0x04100000
#define DPC_END_REG      0x04100004
#define DPC_CURRENT_REG  0x04100008
#define DPC_STATUS_REG   0x0410000C
#define DPC_CLOCK_REG    0x04100010
#define DPC_BUFBUSY_REG  0x04100014
#define DPC_PIPEBUSY_REG 0x04100018
#define DPC_TMEM_REG     0x0410001C

/* DP Span Registers */
#define DPS_TBIST_REG        0x04200000
#define DPS_TEST_MODE_REG    0x04200004
#define DPS_BUFTEST_ADDR_REG 0x04200008
#define DPS_BUFTEST_DATA_REG 0x0420000C

/* DP Status Read Flags */
#define DPC_STATUS_XBUS_DMEM_DMA    (1 <<  0)
#define DPC_STATUS_FREEZE           (1 <<  1)
#define DPC_STATUS_FLUSH            (1 <<  2)
#define DPC_STATUS_START_GCLK       (1 <<  3)
#define DPC_STATUS_TMEM_BUSY        (1 <<  4)
#define DPC_STATUS_PIPE_BUSY        (1 <<  5)
#define DPC_STATUS_CMD_BUSY         (1 <<  6)
#define DPC_STATUS_CBUF_READY       (1 <<  7)
#define DPC_STATUS_DMA_BUSY         (1 <<  8)
#define DPC_STATUS_END_VALID        (1 <<  9)
#define DPC_STATUS_START_VALID      (1 << 10)

/* DP Status Write Flags */
#define DPC_CLR_XBUS_DMEM_DMA   (1 << 0)
#define DPC_SET_XBUS_DMEM_DMA   (1 << 1)
#define DPC_CLR_FREEZE          (1 << 2)
#define DPC_SET_FREEZE          (1 << 3)
#define DPC_CLR_FLUSH           (1 << 4)
#define DPC_SET_FLUSH           (1 << 5)
#define DPC_CLR_TMEM_CTR        (1 << 6)
#define DPC_CLR_PIPE_CTR        (1 << 7)
#define DPC_CLR_CMD_CTR         (1 << 8)
#define DPC_CLR_CLOCK_CTR       (1 << 9)


u32 osDpGetStatus(void);
void osDpSetStatus(u32 data);
#endif

#endif
