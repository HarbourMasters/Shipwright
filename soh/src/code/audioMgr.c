#include "global.h"
#include "SohHooks.h"
#include <string.h>

void func_800C3C80(AudioMgr* audioMgr) {
    AudioTask* task;

    task = audioMgr->rspTask;
    if (audioMgr->rspTask->taskQueue != NULL) {
        osSendMesg(task->taskQueue, NULL, OS_MESG_BLOCK);
    }
}

void AudioMgr_HandleRetrace(AudioMgr* audioMgr) {
    AudioTask* rspTask;

    if (SREG(20) > 0) {
        audioMgr->rspTask = NULL;
    }
    if (audioMgr->rspTask != NULL) {
        audioMgr->audioTask.next = NULL;
        audioMgr->audioTask.flags = 2;
        audioMgr->audioTask.framebuffer = NULL;

        audioMgr->audioTask.list = audioMgr->rspTask->task;
        audioMgr->audioTask.msgQ = &audioMgr->unk_AC;

        audioMgr->audioTask.msg = NULL;
        osSendMesg(&audioMgr->sched->cmdQ, &audioMgr->audioTask, OS_MESG_BLOCK);
        Sched_SendEntryMsg(audioMgr->sched);
    }

    D_8016A550 = osGetTime();
    if (SREG(20) >= 2) {
        rspTask = NULL;
    } else {
        rspTask = func_800E4FE0();
    }
    D_8016A558 += osGetTime() - D_8016A550;
    D_8016A550 = 0;
    if (audioMgr->rspTask != NULL) {
        osRecvMesg(&audioMgr->unk_AC, NULL, OS_MESG_BLOCK);
        func_800C3C80(audioMgr);
    }
    audioMgr->rspTask = rspTask;
}

void AudioMgr_HandlePRENMI(AudioMgr* audioMgr) {
    // "Audio manager received OS_SC_PRE_NMI_MSG"
    osSyncPrintf("オーディオマネージャが OS_SC_PRE_NMI_MSG を受け取りました\n");
    Audio_PreNMI();
}

void AudioMgr_ThreadEntry(void* arg0) {
    AudioMgr* audioMgr = (AudioMgr*)arg0;
    s16* msg = NULL;

    //while (true) {
        osRecvMesg(&audioMgr->unk_74, (OSMesg*)&msg, OS_MESG_BLOCK);
        AudioMgr_HandleRetrace(audioMgr);
        /*switch (*msg) {
            case OS_SC_RETRACE_MSG:
                AudioMgr_HandleRetrace(audioMgr);
                while (audioMgr->unk_74.validCount != 0) {
                    osRecvMesg(&audioMgr->unk_74, (OSMesg*)&msg, OS_MESG_BLOCK);
                    switch (*msg) {
                        case OS_SC_RETRACE_MSG:
                            break;
                        case OS_SC_PRE_NMI_MSG:
                            AudioMgr_HandlePRENMI(audioMgr);
                            break;
                    }
                }
                break;
            case OS_SC_PRE_NMI_MSG:
                AudioMgr_HandlePRENMI(audioMgr);
                break;
        }*/
    //}
}

void AudioMgr_Unlock(AudioMgr* audioMgr) {
    osRecvMesg(&audioMgr->unk_C8, NULL, OS_MESG_BLOCK);
}

void AudioMgr_Init(AudioMgr* audioMgr, void* stack, OSPri pri, OSId id, SchedContext* sched, IrqMgr* irqMgr) {
    AudioPlayer_Init();

    memset(audioMgr,0, sizeof(AudioMgr));

    audioMgr->sched = sched;
    audioMgr->irqMgr = irqMgr;
    audioMgr->rspTask = NULL;

    osCreateMesgQueue(&audioMgr->unk_AC, &audioMgr->unk_C4, 1);
    osCreateMesgQueue(&audioMgr->unk_74, &audioMgr->unk_8C, 8);
    osCreateMesgQueue(&audioMgr->unk_C8, &audioMgr->unk_E0, 1);

    osSendMesg(&audioMgr->unk_AC, NULL, OS_MESG_BLOCK);

        static bool hasInitialized = false;

    if (!hasInitialized) {
        IrqMgrClient irqClient;

        osSyncPrintf("オーディオマネージャスレッド実行開始\n"); // "Start running audio manager thread"
        Audio_Init();
        AudioLoad_SetDmaHandler(DmaMgr_DmaHandler);
        Audio_InitSound();
        osSendMesg(&audioMgr->unk_C8, NULL, OS_MESG_BLOCK);
        ModInternal_ExecuteAudioInitHooks();
        // Removed due to crash
        //IrqMgr_AddClient(audioMgr->irqMgr, &irqClient, &audioMgr->unk_74);
        hasInitialized = true;
    }

    //osCreateThread(&audioMgr->unk_E8, id, AudioMgr_ThreadEntry, audioMgr, stack, pri);
    //osStartThread(&audioMgr->unk_E8);
}
