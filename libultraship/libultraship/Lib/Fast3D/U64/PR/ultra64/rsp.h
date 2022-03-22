#ifndef ULTRA64_RSP_H
#define ULTRA64_RSP_H

/* SP Status Flags */
#define SP_STATUS_HALT          0x001
#define SP_STATUS_BROKE         0x002
#define SP_STATUS_DMA_BUSY      0x004
#define SP_STATUS_DMA_FULL      0x008
#define SP_STATUS_IO_FULL       0x010
#define SP_STATUS_SSTEP         0x020
#define SP_STATUS_INTR_BREAK    0x040
#define SP_STATUS_YIELD         0x080
#define SP_STATUS_YIELDED       0x100
#define SP_STATUS_TASKDONE      0x200
//#define SP_STATUS_SIG0        0x080
//#define SP_STATUS_SIG1        0x100
//#define SP_STATUS_SIG2        0x200
#define SP_STATUS_SIG3          0x400
#define SP_STATUS_SIG4          0x800
#define SP_STATUS_SIG5          0x1000
#define SP_STATUS_SIG6          0x2000
#define SP_STATUS_SIG7          0x4000

#define SP_CLR_HALT         0x00001
#define SP_SET_HALT         0x00002
#define SP_CLR_BROKE        0x00004
#define SP_CLR_INTR         0x00008
#define SP_SET_INTR         0x00010
#define SP_CLR_SSTEP        0x00020
#define SP_SET_SSTEP        0x00040
#define SP_CLR_INTR_BREAK   0x00080
#define SP_SET_INTR_BREAK   0x00100
#define SP_CLR_SIG0         0x00200
#define SP_SET_SIG0         0x00400
#define SP_CLR_SIG1         0x00800
#define SP_SET_SIG1         0x01000
#define SP_CLR_SIG2         0x02000
#define SP_SET_SIG2         0x04000
#define SP_CLR_SIG3         0x08000
#define SP_SET_SIG3         0x10000
#define SP_CLR_SIG4         0x20000
#define SP_SET_SIG4         0x40000
#define SP_CLR_SIG5         0x80000
#define SP_SET_SIG5         0x100000
#define SP_CLR_SIG6         0x200000
#define SP_SET_SIG6         0x400000
#define SP_CLR_SIG7         0x800000
#define SP_SET_SIG7         0x1000000

#endif
