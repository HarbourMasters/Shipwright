#ifndef ULTRA64_RDP_H
#define ULTRA64_RDP_H

/* DP Command Registers */

#define DPC_REG_BASE 0xA4100000

#define DPC_START_REG       (*(vu32*)(DPC_REG_BASE + 0x00))
#define DPC_END_REG         (*(vu32*)(DPC_REG_BASE + 0x04))
#define DPC_CURRENT_REG     (*(vu32*)(DPC_REG_BASE + 0x08))
#define DPC_STATUS_REG      (*(vu32*)(DPC_REG_BASE + 0x0C))
#define DPC_CLOCK_REG       (*(vu32*)(DPC_REG_BASE + 0x10))
#define DPC_BUFBUSY_REG     (*(vu32*)(DPC_REG_BASE + 0x14))
#define DPC_PIPEBUSY_REG    (*(vu32*)(DPC_REG_BASE + 0x18))
#define DPC_TMEM_REG        (*(vu32*)(DPC_REG_BASE + 0x1C))

/* DP Span Registers */

#define DPS_REG_BASE        0xA4200000

#define DPS_TBIST_REG        (*(vu32*)(DPS_REG_BASE + 0x00))
#define DPS_TEST_MODE_REG    (*(vu32*)(DPS_REG_BASE + 0x04))
#define DPS_BUFTEST_ADDR_REG (*(vu32*)(DPS_REG_BASE + 0x08))
#define DPS_BUFTEST_DATA_REG (*(vu32*)(DPS_REG_BASE + 0x0C))

/* DP Status Read Flags */
#define DPC_STATUS_XBUS_DMEM_DMA    0x001
#define DPC_STATUS_FREEZE           0x002
#define DPC_STATUS_FLUSH            0x004
#define DPC_STATUS_START_GCLK       0x008
#define DPC_STATUS_TMEM_BUSY        0x010
#define DPC_STATUS_PIPE_BUSY        0x020
#define DPC_STATUS_CMD_BUSY         0x040
#define DPC_STATUS_CBUF_READY       0x080
#define DPC_STATUS_DMA_BUSY         0x100
#define DPC_STATUS_END_VALID        0x200
#define DPC_STATUS_START_VALID      0x400

/* DP Status Write Flags */
#define DPC_CLR_XBUS_DMEM_DMA   0x0001
#define DPC_SET_XBUS_DMEM_DMA   0x0002
#define DPC_CLR_FREEZE          0x0004
#define DPC_SET_FREEZE          0x0008
#define DPC_CLR_FLUSH           0x0010
#define DPC_SET_FLUSH           0x0020
#define DPC_CLR_TMEM_CTR        0x0040
#define DPC_CLR_PIPE_CTR        0x0080
#define DPC_CLR_CMD_CTR         0x0100
#define DPC_CLR_CLOCK_CTR       0x0200

#endif
