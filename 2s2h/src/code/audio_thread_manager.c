#include "global.h"
#include "audiomgr.h"
#include <string.h>
#include <libultraship/bridge/consolevariablebridge.h>

void AudioMgr_NotifyTaskDone(AudioMgr* audioMgr) {
    AudioTask* task = audioMgr->rspTask;

    if (audioMgr->rspTask->taskQueue != NULL) {
        osSendMesg(task->taskQueue, OS_MESG_PTR(NULL), OS_MESG_BLOCK);
    }
}

void AudioMgr_HandleRetrace(AudioMgr* audioMgr) {
    return;
    static s32 sRetryCount = 10;
    AudioTask* rspTask;
    s32 timerMsgVal = 666;
    OSTimer timer;
    s32 msg;

    if (SREG(20) > 0) {
        audioMgr->rspTask = NULL;
    }

    while (!MQ_IS_EMPTY(&audioMgr->cmdQueue)) {
        osRecvMesg(&audioMgr->cmdQueue, NULL, OS_MESG_NOBLOCK);
    }

    if (audioMgr->rspTask != NULL) {
        audioMgr->audioTask.next = NULL;
        audioMgr->audioTask.flags = OS_SC_NEEDS_RSP;
        audioMgr->audioTask.framebuffer = NULL;

        audioMgr->audioTask.list = audioMgr->rspTask->task;
        audioMgr->audioTask.msgQ = &audioMgr->cmdQueue;

        audioMgr->audioTask.msg.ptr = NULL;
        osSendMesg(&audioMgr->sched->cmdQ, OS_MESG_PTR(&audioMgr->audioTask), OS_MESG_BLOCK);
        Sched_SendEntryMsg(audioMgr->sched);
    }

    if (SREG(20) >= 2) {
        rspTask = NULL;
    } else {
        rspTask = AudioThread_Update();
    }

    AudioMgr_NotifyTaskDone(audioMgr);

    audioMgr->rspTask = rspTask;
}

void AudioMgr_HandlePreNMI(AudioMgr* audioMgr) {
    Audio_PreNMI();
}

void AudioMgr_ThreadEntry(void* arg) {
    AudioMgr* audioMgr = (AudioMgr*)arg;
    IrqMgrClient irqClient;
    s16* msg = NULL;
    s32 exit;

    Audio_Init();
    AudioLoad_SetDmaHandler(DmaMgr_DmaHandler);
    Audio_InitSound();
    osSendMesg(&audioMgr->lockQueue, OS_MESG_PTR(NULL), OS_MESG_BLOCK);
    IrqMgr_AddClient(audioMgr->irqMgr, &irqClient, &audioMgr->interruptQueue);

    exit = false;
    while (!exit) {
        osRecvMesg(&audioMgr->interruptQueue, (OSMesg*)&msg, OS_MESG_BLOCK);
        switch (*msg) {
            case OS_SC_RETRACE_MSG:
                AudioMgr_HandleRetrace(audioMgr);
                while (!MQ_IS_EMPTY(&audioMgr->interruptQueue)) {
                    osRecvMesg(&audioMgr->interruptQueue, (OSMesg*)&msg, OS_MESG_BLOCK);
                    switch (*msg) {
                        case OS_SC_RETRACE_MSG:
                            break;

                        case OS_SC_PRE_NMI_MSG:
                            AudioMgr_HandlePreNMI(audioMgr);
                            break;

                        case OS_SC_NMI_MSG:
                            exit = true;
                            break;
                    }
                }
                break;

            case OS_SC_PRE_NMI_MSG:
                AudioMgr_HandlePreNMI(audioMgr);
                break;

            case OS_SC_NMI_MSG:
                exit = true;
                break;
        }
    }

    IrqMgr_RemoveClient(audioMgr->irqMgr, &irqClient);
}

void AudioMgr_Unlock(AudioMgr* audioMgr) {
    osRecvMesg(&audioMgr->lockQueue, NULL, OS_MESG_BLOCK);
}

void AudioMgr_Init(AudioMgr* audioMgr, void* stack, OSPri pri, OSId id, SchedContext* sched, IrqMgr* irqMgr) {
    memset(audioMgr, 0, sizeof(AudioMgr));

    audioMgr->sched = sched;
    audioMgr->irqMgr = irqMgr;
    audioMgr->rspTask = NULL;

    osCreateMesgQueue(&audioMgr->cmdQueue, audioMgr->cmdMsgBuf, ARRAY_COUNT(audioMgr->cmdMsgBuf));
    osCreateMesgQueue(&audioMgr->interruptQueue, audioMgr->interruptMsgBuf, ARRAY_COUNT(audioMgr->interruptMsgBuf));
    osCreateMesgQueue(&audioMgr->lockQueue, audioMgr->lockMsgBuf, ARRAY_COUNT(audioMgr->lockMsgBuf));

    Audio_Init();
    AudioLoad_SetDmaHandler(DmaMgr_DmaHandler);
    Audio_InitSound();
    osSendMesg(&audioMgr->lockQueue, OS_MESG_PTR(NULL), OS_MESG_BLOCK);

    AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_MAIN, CVarGetFloat("gSettings.Audio.MainMusicVolume", 1.0f));
    AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_SUB, CVarGetFloat("gSettings.Audio.SubMusicVolume", 1.0f));
    AudioSeq_SetPortVolumeScale(SEQ_PLAYER_SFX, CVarGetFloat("gSettings.Audio.SoundEffectsVolume", 1.0f));
    AudioSeq_SetPortVolumeScale(SEQ_PLAYER_FANFARE, CVarGetFloat("gSettings.Audio.FanfareVolume", 1.0f));
    AudioSeq_SetPortVolumeScale(SEQ_PLAYER_AMBIENCE, CVarGetFloat("gSettings.Audio.AmbienceVolume", 1.0f));

    // osCreateThread(&audioMgr->thread, id, AudioMgr_ThreadEntry, audioMgr, stack, pri);
    // osStartThread(&audioMgr->thread);
}
