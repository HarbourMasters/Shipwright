#include "global.h"
#include "vt.h"
#include <string.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"

s32 D_8012D280 = 1;

void OTRControllerCallback(uint8_t rumble);

OSMesgQueue* PadMgr_LockSerialMesgQueue(PadMgr* padMgr) {
    OSMesgQueue* ctrlrQ = NULL;

    if (D_8012D280 > 2) {
        // "serialMsgQ Waiting for lock"
        osSyncPrintf("%2d %d serialMsgQロック待ち         %08x %08x          %08x\n", osGetThreadId(NULL),
                     padMgr->serialMsgQ.validCount, padMgr, &padMgr->serialMsgQ, &ctrlrQ);
    }

    osRecvMesg(&padMgr->serialMsgQ, (OSMesg*)&ctrlrQ, OS_MESG_BLOCK);

    if (D_8012D280 > 2) {
        // "serialMsgQ Locked"
        osSyncPrintf("%2d %d serialMsgQをロックしました                     %08x\n", osGetThreadId(NULL),
                     padMgr->serialMsgQ.validCount, ctrlrQ);
    }

    return ctrlrQ;
}

void PadMgr_UnlockSerialMesgQueue(PadMgr* padMgr, OSMesgQueue* ctrlrQ) {
    if (D_8012D280 > 2) {
        // "serialMsgQ Unlock"
        osSyncPrintf("%2d %d serialMsgQロック解除します   %08x %08x %08x\n", osGetThreadId(NULL),
                     padMgr->serialMsgQ.validCount, padMgr, &padMgr->serialMsgQ, ctrlrQ);
    }

    osSendMesgPtr(&padMgr->serialMsgQ, ctrlrQ, OS_MESG_BLOCK);

    if (D_8012D280 > 2) {
        // "serialMsgQ Unlocked"
        osSyncPrintf("%2d %d serialMsgQロック解除しました %08x %08x %08x\n", osGetThreadId(NULL),
                     padMgr->serialMsgQ.validCount, padMgr, &padMgr->serialMsgQ, ctrlrQ);
    }
}

void PadMgr_LockPadData(PadMgr* padMgr) {
    osRecvMesg(&padMgr->lockMsgQ, NULL, OS_MESG_BLOCK);
}

void PadMgr_UnlockPadData(PadMgr* padMgr) {
    osSendMesgPtr(&padMgr->lockMsgQ, NULL, OS_MESG_BLOCK);
}

void PadMgr_RumbleControl(PadMgr* padMgr) {
    static u32 errcnt = 0;
    static u32 frame;
    s32 temp = 1;
    s32 triedRumbleComm;
    OSMesgQueue* ctrlrQ = PadMgr_LockSerialMesgQueue(padMgr);
    s32 var4;
    s32 i;

    triedRumbleComm = 0;

    for (i = 0; i < 4; i++) {
        if (padMgr->ctrlrIsConnected[i]) {
            if (padMgr->padStatus[i].status & 1) {
                if (padMgr->pakType[i] == temp) {
                    if (padMgr->rumbleEnable[i] != 0) {
                        if (padMgr->rumbleCounter[i] < 3) {
                            // clang-format off
                            osSyncPrintf(VT_FGCOL(YELLOW));
                            // clang-format on

                            // "Vibration pack jumble jumble"?
                            osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "振動パック ぶるぶるぶるぶる");
                            osSyncPrintf(VT_RST);

                            if (__osMotorAccess(&padMgr->pfs[i], temp) != 0) {
                                padMgr->pakType[i] = 0;
                                osSyncPrintf(VT_FGCOL(YELLOW));
                                // "A communication error has occurred with the vibration pack"
                                osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "振動パックで通信エラーが発生しました");
                                osSyncPrintf(VT_RST);
                            } else {
                                padMgr->rumbleCounter[i] = 3;
                            }

                            triedRumbleComm = 1;
                        }
                    } else {
                        if (padMgr->rumbleCounter[i] != 0) {
                            // clang-format off
                            osSyncPrintf(VT_FGCOL(YELLOW));
                            // clang-format on

                            // "Stop vibration pack"
                            osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "振動パック 停止");
                            osSyncPrintf(VT_RST);

                            if (osMotorStop(&padMgr->pfs[i]) != 0) {
                                padMgr->pakType[i] = 0;
                                osSyncPrintf(VT_FGCOL(YELLOW));
                                // "A communication error has occurred with the vibration pack"
                                osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "振動パックで通信エラーが発生しました");
                                osSyncPrintf(VT_RST);
                            } else {
                                padMgr->rumbleCounter[i]--;
                            }

                            triedRumbleComm = 1;
                        }
                    }
                }
            } else {
                if (padMgr->pakType[i] != 0) {
                    if (padMgr->pakType[i] == 1) {
                        osSyncPrintf(VT_FGCOL(YELLOW));
                        // "It seems that a vibration pack was pulled out"
                        osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "振動パックが抜かれたようです");
                        osSyncPrintf(VT_RST);
                        padMgr->pakType[i] = 0;
                    } else {
                        osSyncPrintf(VT_FGCOL(YELLOW));
                        // "It seems that a controller pack that is not a vibration pack was pulled out"
                        osSyncPrintf("padmgr: %dコン: %s\n", i + 1,
                                     "振動パックではないコントローラパックが抜かれたようです");
                        osSyncPrintf(VT_RST);
                        padMgr->pakType[i] = 0;
                    }
                }
            }
        }
    }

    if (!triedRumbleComm) {
        i = frame % 4;

        if (padMgr->ctrlrIsConnected[i] && (padMgr->padStatus[i].status & 1) && (padMgr->pakType[i] != 1)) {
            var4 = osMotorInit(ctrlrQ, &padMgr->pfs[i], i);

            if (var4 == 0) {
                padMgr->pakType[i] = 1;
                osMotorStart(&padMgr->pfs[i]);
                osMotorStop(&padMgr->pfs[i]);
                osSyncPrintf(VT_FGCOL(YELLOW));
                // "Recognized vibration pack"
                osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "振動パックを認識しました");
                osSyncPrintf(VT_RST);
            } else if (var4 == 11) {
                padMgr->pakType[i] = 2;
            } else if (var4 == 4) {
                LOG_NUM("++errcnt", ++errcnt);
                osSyncPrintf(VT_FGCOL(YELLOW));
                // "Controller pack communication error"
                osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "コントローラパックの通信エラー");
                osSyncPrintf(VT_RST);
            }
        }
    }

    frame++;
    PadMgr_UnlockSerialMesgQueue(padMgr, ctrlrQ);
}

void PadMgr_RumbleStop(PadMgr* padMgr) {
    s32 i;
    OSMesgQueue* ctrlrQ = PadMgr_LockSerialMesgQueue(padMgr);

    for (i = 0; i < 4; i++) {
        if (osMotorInit(ctrlrQ, &padMgr->pfs[i], i) == 0) {
#if 0
            if ((gFaultStruct.msgId == 0) && (padMgr->rumbleOnFrames != 0))
            {
                osSyncPrintf(VT_FGCOL(YELLOW));
                // "Stop vibration pack"
                osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "振動パック 停止");
                osSyncPrintf(VT_RST);
            }
#endif

            osMotorStop(&padMgr->pfs[i]);
        }
    }

    PadMgr_UnlockSerialMesgQueue(padMgr, ctrlrQ);
}

void PadMgr_RumbleReset(PadMgr* padMgr) {
    padMgr->rumbleOffFrames = 3;
}

void PadMgr_RumbleSetSingle(PadMgr* padMgr, u32 ctrlr, u32 rumble) {
    padMgr->rumbleEnable[ctrlr] = rumble;
    padMgr->rumbleOnFrames = 240;
}

void PadMgr_RumbleSet(PadMgr* padMgr, u8* ctrlrRumbles) {
    s32 i;

    for (i = 0; i < 4; i++) {
        padMgr->rumbleEnable[i] = ctrlrRumbles[i];
    }

    padMgr->rumbleOnFrames = 240;
}

#define PAUSE_BUFFER_INPUT_BLOCK_ID 0
void PadMgr_ProcessInputs(PadMgr* padMgr) {
    s32 i;
    Input* input;
    OSContPad* padnow1; // original name
    s32 buttonDiff;

    PadMgr_LockPadData(padMgr);

    input = &padMgr->inputs[0];
    padnow1 = &padMgr->pads[0];

    for (i = 0; i < padMgr->nControllers; i++, input++, padnow1++) {
        input->prev = input->cur;

        switch (padnow1->err_no) {
            case 0:
                input->cur = *padnow1;

                if (GameInteractor_DisableZTargetingActive()) {
                    input->cur.button &= ~(BTN_Z);
                }

                uint32_t emulatedButtons = GameInteractor_GetEmulatedButtons();
                if (emulatedButtons) {
                    input->cur.button |= emulatedButtons;
                    GameInteractor_SetEmulatedButtons(0);
                }

                if (GameInteractor_ReverseControlsActive()) {
                    if (input->cur.stick_x == -128) {
                        input->cur.stick_x = 127;
                    } else {
                        input->cur.stick_x *= -1;
                    }

                    if (input->cur.stick_y == -128) {
                        input->cur.stick_y = 127;
                    } else {
                        input->cur.stick_y *= -1;
                    }
                }

                if (!padMgr->ctrlrIsConnected[i]) {
                    padMgr->ctrlrIsConnected[i] = true;
                    osSyncPrintf(VT_FGCOL(YELLOW));
                    osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "認識しました"); // "Recognized"
                    osSyncPrintf(VT_RST);
                }
                break;
            case 4:
                input->cur = input->prev;
                LOG_NUM("this->Key_switch[i]", padMgr->ctrlrIsConnected[i]);
                osSyncPrintf(VT_FGCOL(YELLOW));
                // "Overrun error occurred"
                osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "オーバーランエラーが発生");
                osSyncPrintf(VT_RST);
                break;
            case 8:
                input->cur.button = 0;
                input->cur.stick_x = 0;
                input->cur.stick_y = 0;
                input->cur.err_no = padnow1->err_no;
                if (padMgr->ctrlrIsConnected[i]) {
                    padMgr->ctrlrIsConnected[i] = false;
                    padMgr->pakType[i] = 0;
                    padMgr->rumbleCounter[i] = 0xFF;
                    osSyncPrintf(VT_FGCOL(YELLOW));
                    // "Do not respond"?
                    osSyncPrintf("padmgr: %dコン: %s\n", i + 1, "応答しません");
                    osSyncPrintf(VT_RST);
                }
                break;
            default:
                LOG_HEX("padnow1->errno", padnow1->err_no);
                Fault_AddHungupAndCrash(__FILE__, __LINE__);
        }

        // When 3 frames are left on easy pause buffer, re-apply the last held inputs to the prev inputs
        // to compute the pressed difference. This makes it so previously held inputs are continued as "held",
        // but new inputs when unpausing are "pressed" out of the pause menu.
        if (CVarGetInteger("gCheatEasyPauseBufferTimer", 0) == 3) {
            input->prev.button = CVarGetInteger("gCheatEasyPauseBufferLastInputs", 0);
        }

        buttonDiff = input->prev.button ^ input->cur.button;
        input->press.button |= (u16)(buttonDiff & input->cur.button);
        input->rel.button |= (u16)(buttonDiff & input->prev.button);
        PadUtils_UpdateRelXY(input);
        input->press.stick_x += (s8)(input->cur.stick_x - input->prev.stick_x);
        input->press.stick_y += (s8)(input->cur.stick_y - input->prev.stick_y);
    }

    uint8_t rumble = (padMgr->rumbleEnable[0] > 0);
    OTRControllerCallback(rumble);

    PadMgr_UnlockPadData(padMgr);
}

void PadMgr_HandleRetraceMsg(PadMgr* padMgr) {
    s32 i;
    OSMesgQueue* queue = PadMgr_LockSerialMesgQueue(padMgr);
    u32 mask;

    osContStartReadData(queue);
    if (padMgr->retraceCallback) {
        padMgr->retraceCallback(padMgr, padMgr->retraceCallbackValue);
    }
    osRecvMesg(queue, NULL, OS_MESG_BLOCK);
    osContGetReadData(padMgr->pads);

    for (i = 0; i < __osMaxControllers; i++) {
        padMgr->padStatus[i].status = Controller_ShouldRumble(i);
    }

    if (padMgr->preNMIShutdown) {
        memset(padMgr->pads, 0, sizeof(padMgr->pads));
    }
    PadMgr_ProcessInputs(padMgr);
    osContStartQuery(queue);
    osRecvMesg(queue, NULL, OS_MESG_BLOCK);
    osContGetQuery(padMgr->padStatus);
    PadMgr_UnlockSerialMesgQueue(padMgr, queue);

    mask = 0;
    for (i = 0; i < 4; i++) {
        if (padMgr->padStatus[i].err_no == 0) {
            if (padMgr->padStatus[i].type == CONT_TYPE_NORMAL) {
                mask |= 1 << i;
            } else {
                //LOG_HEX("this->pad_status[i].type", padMgr->padStatus[i].type);
                // "An unknown type of controller is connected"
                //osSyncPrintf("知らない種類のコントローラが接続されています\n");
            }
        }
    }
    padMgr->validCtrlrsMask = mask;

    /* if (gFaultStruct.msgId) {
        PadMgr_RumbleStop(padMgr);
    } else */ if (padMgr->rumbleOffFrames > 0) {
        --padMgr->rumbleOffFrames;
        PadMgr_RumbleStop(padMgr);
    } else if (padMgr->rumbleOnFrames == 0) {
        PadMgr_RumbleStop(padMgr);
    } else if (!padMgr->preNMIShutdown) {
        PadMgr_RumbleControl(padMgr);
        --padMgr->rumbleOnFrames;
    }
}

void PadMgr_HandlePreNMI(PadMgr* padMgr) {
    osSyncPrintf("padmgr_HandlePreNMI()\n");
    padMgr->preNMIShutdown = true;
    PadMgr_RumbleReset(padMgr);
}

void PadMgr_RequestPadData(PadMgr* padMgr, Input* inputs, s32 mode) {
    s32 i;
    Input* ogInput;
    Input* newInput;
    s32 buttonDiff;

    PadMgr_LockPadData(padMgr);

    ogInput = &padMgr->inputs[0];
    newInput = &inputs[0];
    for (i = 0; i < 4; i++) {
        if (mode != 0) {
            *newInput = *ogInput;
            ogInput->press.button = 0;
            ogInput->press.stick_x = 0;
            ogInput->press.stick_y = 0;
            ogInput->rel.button = 0;
        } else {
            newInput->prev = newInput->cur;
            newInput->cur = ogInput->cur;
            buttonDiff = newInput->prev.button ^ newInput->cur.button;
            newInput->press.button = newInput->cur.button & buttonDiff;
            newInput->rel.button = newInput->prev.button & buttonDiff;
            PadUtils_UpdateRelXY(newInput);
            newInput->press.stick_x += (s8)(newInput->cur.stick_x - newInput->prev.stick_x);
            newInput->press.stick_y += (s8)(newInput->cur.stick_y - newInput->prev.stick_y);
        }
        ogInput++;
        newInput++;
    }

    PadMgr_UnlockPadData(padMgr);
}

void PadMgr_ThreadEntry(PadMgr* padMgr) {
    s16* mesg = NULL;
    s32 exit;

    //osSyncPrintf("コントローラスレッド実行開始\n"); // "Controller thread execution start"

    exit = false;
    while (!exit) {
        if ((D_8012D280 > 2) && (padMgr->interruptMsgQ.validCount == 0)) {
            // "Waiting for controller thread event"
            osSyncPrintf("コントローラスレッドイベント待ち %lld\n", OS_CYCLES_TO_USEC(osGetTime()));
        }

        osRecvMesg(&padMgr->interruptMsgQ, (OSMesg*)&mesg, OS_MESG_BLOCK);
        //LOG_CHECK_NULL_POINTER("msg", mesg);

        PadMgr_HandleRetraceMsg(padMgr);
        break;

#if 0
        switch (*mesg) {
            case OS_SC_RETRACE_MSG:
                if (D_8012D280 > 2) {
                    osSyncPrintf("padmgr_HandleRetraceMsg START %lld\n", OS_CYCLES_TO_USEC(osGetTime()));
                }

                PadMgr_HandleRetraceMsg(padMgr);

                if (D_8012D280 > 2) {
                    osSyncPrintf("padmgr_HandleRetraceMsg END   %lld\n", OS_CYCLES_TO_USEC(osGetTime()));
                }

                break;
            case OS_SC_PRE_NMI_MSG:
                PadMgr_HandlePreNMI(padMgr);
                break;
            case OS_SC_NMI_MSG:
                exit = true;
                break;
        }
#endif
    }

    // OTRTODO: Removed due to crash
    //IrqMgr_RemoveClient(padMgr->irqMgr, &padMgr->irqClient);

    //osSyncPrintf("コントローラスレッド実行終了\n"); // "Controller thread execution end"
}

void PadMgr_Init(PadMgr* padMgr, OSMesgQueue* siIntMsgQ, IrqMgr* irqMgr, OSId id, OSPri priority, void* stack) {
    osSyncPrintf("パッドマネージャ作成 padmgr_Create()\n"); // "Pad Manager creation"

    memset(padMgr, 0, sizeof(PadMgr));
    padMgr->irqMgr = irqMgr;

    osCreateMesgQueue(&padMgr->interruptMsgQ, padMgr->interruptMsgBuf, 4);
    // OTRTODO: Removed due to crash
    //IrqMgr_AddClient(padMgr->irqMgr, &padMgr->irqClient, &padMgr->interruptMsgQ);
    osCreateMesgQueue(&padMgr->serialMsgQ, padMgr->serialMsgBuf, 1);
    PadMgr_UnlockSerialMesgQueue(padMgr, siIntMsgQ);
    osCreateMesgQueue(&padMgr->lockMsgQ, padMgr->lockMsgBuf, 1);
    PadMgr_UnlockPadData(padMgr);
    PadSetup_Init(siIntMsgQ, (u8*)&padMgr->validCtrlrsMask, padMgr->padStatus);

    padMgr->nControllers = 4;
    osContSetCh(padMgr->nControllers);

    osCreateThread(&padMgr->thread, id, (void (*)(void*))PadMgr_ThreadEntry, padMgr, stack, priority);
    osStartThread(&padMgr->thread);
}
