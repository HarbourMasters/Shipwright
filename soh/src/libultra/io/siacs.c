#include "global.h"

OSMesg osSiMesgBuff[SIAccessQueueSize];
OSMesgQueue gOSSiMessageQueue;
u32 gOSSiAccessQueueCreated = 0;

void __osSiCreateAccessQueue(void) {
    gOSSiAccessQueueCreated = 1;
    osCreateMesgQueue(&gOSSiMessageQueue, &osSiMesgBuff[0], SIAccessQueueSize - 1);
    osSendMesg(&gOSSiMessageQueue, NULL, OS_MESG_NOBLOCK);
}

void __osSiGetAccess(void) {
    OSMesg mesg;

    if (!gOSSiAccessQueueCreated) {
        __osSiCreateAccessQueue();
    }
    osRecvMesg(&gOSSiMessageQueue, &mesg, OS_MESG_BLOCK);
}

void __osSiRelAccess(void) {
    osSendMesg(&gOSSiMessageQueue, NULL, OS_MESG_NOBLOCK);
}
