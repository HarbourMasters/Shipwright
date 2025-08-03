/**
 * @file padmgr.c
 *
 * This file implements communicating with joybus devices at a high level and serving the results to other threads.
 *
 * Any device that can be plugged into one of the four controller ports such as a standard N64 controller is a joybus
 * device. Some joybus devices are also located inside the cartridge such as EEPROM for save data or the Real-Time
 * Clock, however neither of these are used in Zelda64 and so this type of communication is unimplemented. Of the
 * possible devices that can be plugged into the controller ports, the only device that padmgr will recognize and
 * attempt to communicate with is the standard N64 controller.
 *
 * Communicating with these devices is broken down into various layers:
 *
 * Other threads                    : The rest of the program that will use the polled data
 *  |
 * PadMgr                           : Manages devices, submits polling commands at vertical retrace
 *  |
 * Libultra osCont* routines        : Interface for building commands and safely using the Serial Interface
 *  |
 * Serial Interface                 : Hardware unit for sending joybus commands and receiving data via DMA
 *  |
 * PIF                              : Forwards joybus commands and receives response data from the devices
 *  |---¬---¬---¬-------¬
 *  1   2   3   4       5           : The joybus devices plugged into the four controller ports or on the cartridge
 *
 * Joybus communication is handled on another thread as polling and receiving controller data is a slow process; the
 * N64 programming manual section 26.2.4.1 quotes 2 milliseconds as the expected delay from calling
 * `osContStartReadData` to receiving the data. By running this on a separate thread to the game state, work can be
 * done while waiting for this operation to complete.
 */
#include "libu64/debug.h"
#include "libu64/padsetup.h"
#include "array_count.h"
#include "padmgr.h"
#include "printf.h"
#include "fault.h"
#include "terminal.h"
#include "translation.h"
#include "line_numbers.h"

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/controls/Mouse.h"
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"

#define PADMGR_LOG(controllerNum, msg) (void)0


#define LOG_SEVERITY_NOLOG 0
#define LOG_SEVERITY_CRITICAL 1
#define LOG_SEVERITY_ERROR 2
#define LOG_SEVERITY_VERBOSE 3

s32 gPadMgrLogSeverity = LOG_SEVERITY_CRITICAL;
void OTRControllerCallback(uint8_t rumble);

/**
 * Acquires exclusive access to the serial event queue.
 *
 * When a DMA to/from PIF RAM completes, an SI interrupt is generated to notify the process that the DMA has completed
 * and a message is posted to the serial event queue. If multiple processes are trying to use the SI at the same time
 * it becomes ambiguous as to which DMA has completed, so a locking system is required to arbitrate access to the SI.
 *
 * Once the task requiring the serial event queue is complete, it should be released with a call to
 * `PadMgr_ReleaseSerialEventQueue()`.
 *
 * If another process tries to acquire the event queue, the current thread will be blocked until the event queue is
 * released. Note the possibility for a deadlock, if the thread that already holds the serial event queue attempts to
 * acquire it again it will block forever.
 *
 * @return The message queue to which SI interrupt events are posted.
 *
 * @see PadMgr_ReleaseSerialEventQueue
 */
OSMesgQueue* PadMgr_AcquireSerialEventQueue(PadMgr* padMgr) {
    OSMesgQueue* serialEventQueue = NULL;

    if (gPadMgrLogSeverity >= LOG_SEVERITY_VERBOSE) {
        // "serialMsgQ Waiting for lock"
        osSyncPrintf("%2d %d serialMsgQロック待ち         %08x %08x          %08x\n", osGetThreadId(NULL),
                     padMgr->serialMsgQ.validCount, padMgr, &padMgr->serialMsgQ, &ctrlrQ);
    }

    osRecvMesg(&padMgr->serialLockQueue, (OSMesg*)&serialEventQueue, OS_MESG_BLOCK);

    if (gPadMgrLogSeverity >= LOG_SEVERITY_VERBOSE) {
        // "serialMsgQ Locked"
        osSyncPrintf("%2d %d serialMsgQをロックしました                     %08x\n", osGetThreadId(NULL),
                     padMgr->serialMsgQ.validCount, ctrlrQ);
    }

    return serialEventQueue;
}

/**
 * Relinquishes access to the serial message queue, allowing another process to acquire and use it.
 *
 * @param serialEventQueue The serial message queue acquired by `PadMgr_AcquireSerialEventQueue`
 *
 * @see PadMgr_AcquireSerialEventQueue
 */
void PadMgr_ReleaseSerialEventQueue(PadMgr* padMgr, OSMesgQueue* serialEventQueue) {
    if (gPadMgrLogSeverity >= LOG_SEVERITY_VERBOSE) {
        // "serialMsgQ Unlock"
        osSyncPrintf("%2d %d serialMsgQロック解除します   %08x %08x %08x\n", osGetThreadId(NULL),
                     padMgr->serialMsgQ.validCount, padMgr, &padMgr->serialMsgQ, ctrlrQ);
    }

    osSendMesgPtr(&padMgr->serialMsgQ, ctrlrQ, OS_MESG_BLOCK);

    if (gPadMgrLogSeverity >= LOG_SEVERITY_VERBOSE) {
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

        buttonDiff = input->prev.button ^ input->cur.button;
        input->press.button |= (u16)(buttonDiff & input->cur.button);
        input->rel.button |= (u16)(buttonDiff & input->prev.button);
        PadUtils_UpdateRelXY(input);
        input->press.stick_x += (s8)(input->cur.stick_x - input->prev.stick_x);
        input->press.stick_y += (s8)(input->cur.stick_y - input->prev.stick_y);
        // #region SOH [Enhancement]
        PadUtils_UpdateRelRXY(input);
        input->press.right_stick_x += (s8)(input->cur.right_stick_x - input->prev.right_stick_x);
        input->press.right_stick_y += (s8)(input->cur.right_stick_y - input->prev.right_stick_y);
        // #endregion
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

    Mouse_UpdateAll();

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
                // LOG_HEX("this->pad_status[i].type", padMgr->padStatus[i].type);
                //  "An unknown type of controller is connected"
                // osSyncPrintf("知らない種類のコントローラが接続されています\n");
            }
        }
    }
    padMgr->validCtrlrsMask = mask;

    /* if (gFaultStruct.msgId) {
        PadMgr_RumbleStop(padMgr);
    } else */
    if (padMgr->rumbleOffFrames > 0) {
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
            // #region SOH [Enhancement]
            PadUtils_UpdateRelRXY(newInput);
            newInput->press.right_stick_x += (s8)(newInput->cur.right_stick_x - newInput->prev.right_stick_x);
            newInput->press.right_stick_y += (s8)(newInput->cur.right_stick_y - newInput->prev.right_stick_y);
            // #endregion
        }
        ogInput++;
        newInput++;
    }

    PadMgr_UnlockPadData(padMgr);
}

void PadMgr_ThreadEntry(PadMgr* padMgr) {
    s16* mesg = NULL;
    s32 exit;

    // osSyncPrintf("コントローラスレッド実行開始\n"); // "Controller thread execution start"

    exit = false;
    while (!exit) {
        if ((D_8012D280 > 2) && (padMgr->interruptMsgQ.validCount == 0)) {
            // "Waiting for controller thread event"
            osSyncPrintf("コントローラスレッドイベント待ち %lld\n", OS_CYCLES_TO_USEC(osGetTime()));
        }

        osRecvMesg(&padMgr->interruptMsgQ, (OSMesg*)&mesg, OS_MESG_BLOCK);
        // LOG_CHECK_NULL_POINTER("msg", mesg);

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
    // IrqMgr_RemoveClient(padMgr->irqMgr, &padMgr->irqClient);

    // osSyncPrintf("コントローラスレッド実行終了\n"); // "Controller thread execution end"
}

void PadMgr_Init(PadMgr* padMgr, OSMesgQueue* siIntMsgQ, IrqMgr* irqMgr, OSId id, OSPri priority, void* stack) {
    osSyncPrintf("パッドマネージャ作成 padmgr_Create()\n"); // "Pad Manager creation"

    memset(padMgr, 0, sizeof(PadMgr));
    padMgr->irqMgr = irqMgr;

    osCreateMesgQueue(&padMgr->interruptMsgQ, padMgr->interruptMsgBuf, 4);
    // OTRTODO: Removed due to crash
    // IrqMgr_AddClient(padMgr->irqMgr, &padMgr->irqClient, &padMgr->interruptMsgQ);
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
