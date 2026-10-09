#include "global.h"

// Blocks the current thread until all currently queued scheduler tasks have been completed
void MsgEvent_SendNullTask(void) {
    OSScTask task;
    OSMesgQueue queue;
    OSMesg msg;

    task.next = NULL;
    task.flags = OS_SC_RCP_MASK;
    task.msgQ = &queue;
    task.msg.ptr = NULL;
    task.framebuffer = NULL;
    task.list.t.type = M_NULTASK;

    osCreateMesgQueue(task.msgQ, &msg, 1);
    osSendMesg(&gSchedContext.cmdQ, OS_MESG_PTR(&task), OS_MESG_BLOCK);
    Sched_SendEntryMsg(&gSchedContext);
    osRecvMesg(&queue, NULL, OS_MESG_BLOCK);
}
