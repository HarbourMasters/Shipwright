#include <libultraship/libultra.h>
#include "global.h"
#include "soh/mixer.h"

#include "soh/Enhancements/audio/AudioEditor.h"

typedef struct {
    u8 unk_0;
    u8 unk_1; // importance?
} Struct_8016E320;

#define GET_PLAYER_IDX(cmd) (cmd & 0xF000000) >> 24

Struct_8016E320 D_8016E320[4][5];
u8 sNumSeqRequests[4];
u32 sAudioSeqCmds[0x100];
ActiveSequence gActiveSeqs[4];

u8 sSeqCmdWrPos = 0;
u8 sSeqCmdRdPos = 0;
u8 D_80133408 = 0;
u8 D_8013340C = 1;
u8 D_80133410[] = { 0, 1, 2, 3 };
u8 gAudioSpecId = 0;
u8 D_80133418 = 0;

// TODO: clean up these macros. They are similar to ones in code_800EC960.c but without casts.
#define Audio_StartSeq(playerIdx, fadeTimer, seqId) \
    Audio_QueueSeqCmd(0x00000000 | ((playerIdx) << 24) | ((fadeTimer) << 16) | (seqId))
#define Audio_SeqCmdA(playerIdx, a) Audio_QueueSeqCmd(0xA0000000 | ((playerIdx) << 24) | (a))
#define Audio_SeqCmdB30(playerIdx, a, b) Audio_QueueSeqCmd(0xB0003000 | ((playerIdx) << 24) | ((a) << 16) | (b))
#define Audio_SeqCmdB40(playerIdx, a, b) Audio_QueueSeqCmd(0xB0004000 | ((playerIdx) << 24) | ((a) << 16) | (b))
#define Audio_SeqCmd3(playerIdx, a) Audio_QueueSeqCmd(0x30000000 | ((playerIdx) << 24) | (a))
#define Audio_SeqCmd5(playerIdx, a, b) Audio_QueueSeqCmd(0x50000000 | ((playerIdx) << 24) | ((a) << 16) | (b))
#define Audio_SeqCmd4(playerIdx, a, b) Audio_QueueSeqCmd(0x40000000 | ((playerIdx) << 24) | ((a) << 16) | (b))
#define Audio_SetVolScaleNow(playerIdx, volFadeTimer, volScale) \
    Audio_ProcessSeqCmd(0x40000000 | ((u8)playerIdx << 24) | ((u8)volFadeTimer << 16) | ((u8)(volScale * 127.0f)));

void func_800F9280(u8 playerIdx, u8 seqId, u8 arg2, u16 fadeTimer) {
    u8 i;
    u16 dur;
    s32 pad;

    if (D_80133408 == 0 || playerIdx == SEQ_PLAYER_SFX) {
        arg2 &= 0x7F;
        if (arg2 == 0x7F) {
            dur = (fadeTimer >> 3) * 60 * gAudioContext.audioBufferParameters.updatesPerFrame;
            Audio_QueueCmdS32(0x85000000 | _SHIFTL(playerIdx, 16, 8) | _SHIFTL(seqId, 8, 8), dur);
        } else {
            Audio_QueueCmdS32(0x82000000 | _SHIFTL(playerIdx, 16, 8) | _SHIFTL(seqId, 8, 8),
                              (fadeTimer * (u16)gAudioContext.audioBufferParameters.updatesPerFrame) / 4);
        }

        gActiveSeqs[playerIdx].seqId = seqId | (arg2 << 8);
        gActiveSeqs[playerIdx].prevSeqId = seqId | (arg2 << 8);

        if (gActiveSeqs[playerIdx].volCur != 1.0f) {
            Audio_QueueCmdF32(0x41000000 | _SHIFTL(playerIdx, 16, 8), gActiveSeqs[playerIdx].volCur);
        }

        gActiveSeqs[playerIdx].tempoTimer = 0;
        gActiveSeqs[playerIdx].tempoOriginal = 0;
        gActiveSeqs[playerIdx].tempoCmd = 0;

        for (i = 0; i < 0x10; i++) {
            gActiveSeqs[playerIdx].channelData[i].volCur = 1.0f;
            gActiveSeqs[playerIdx].channelData[i].volTimer = 0;
            gActiveSeqs[playerIdx].channelData[i].freqScaleCur = 1.0f;
            gActiveSeqs[playerIdx].channelData[i].freqScaleTimer = 0;
        }

        gActiveSeqs[playerIdx].freqScaleChannelFlags = 0;
        gActiveSeqs[playerIdx].volChannelFlags = 0;
    }
}

void func_800F9474(u8 playerIdx, u16 arg1) {
    Audio_QueueCmdS32(0x83000000 | ((u8)playerIdx << 16),
                      (arg1 * (u16)gAudioContext.audioBufferParameters.updatesPerFrame) / 4);
    gActiveSeqs[playerIdx].seqId = NA_BGM_DISABLED;
}

typedef enum {
    SEQ_START,
    CMD1,
    CMD2,
    CMD3,
    SEQ_VOL_UPD,
    CMD5,
    CMD6,
    CMD7,
    CMD8,
    CMD9,
    CMDA,
    CMDB,
    CMDC,
    CMDD,
    CMDE,
    CMDF
} SeqCmdType;

void Audio_ProcessSeqCmd(u32 cmd) {
    s32 pad[2];
    u16 fadeTimer;
    u16 channelMask;
    u16 val;
    u8 oldSpec;
    u8 spec;
    u8 op;
    u8 subOp;
    u8 playerIdx;
    u16 seqId;
    u8 seqArgs;
    u8 found;
    u8 port;
    u8 duration;
    u8 chanIdx;
    u8 i;
    s32 new_var;
    f32 freqScale;

    if (D_8013340C && (cmd & 0xF0000000) != 0x70000000) {
        AudioDebug_ScrPrt((const s8*)D_80133390, (cmd >> 16) & 0xFFFF); // "SEQ H"
        AudioDebug_ScrPrt((const s8*)D_80133398, cmd & 0xFFFF);         // "    L"
    }

    op = cmd >> 28;
    playerIdx = GET_PLAYER_IDX(cmd);

    switch (op) {
        case 0x0:
            // play sequence immediately
            seqId = cmd & 0xFF;
            seqArgs = (cmd & 0xFF00) >> 8;
            fadeTimer = (cmd & 0xFF0000) >> 13;
            if ((gActiveSeqs[playerIdx].isWaitingForFonts == 0) && (seqArgs < 0x80)) {
                func_800F9280(playerIdx, seqId, seqArgs, fadeTimer);
            }
            break;

        case 0x1:
            // disable seq player
            fadeTimer = (cmd & 0xFF0000) >> 13;
            func_800F9474(playerIdx, fadeTimer);
            break;

        case 0x2:
            // queue sequence
            seqId = cmd & 0xFF;
            seqArgs = (cmd & 0xFF00) >> 8;
            fadeTimer = (cmd & 0xFF0000) >> 13;
            new_var = seqArgs;
            for (i = 0; i < sNumSeqRequests[playerIdx]; i++) {
                if (D_8016E320[playerIdx][i].unk_0 == seqId) {
                    if (i == 0) {
                        func_800F9280(playerIdx, seqId, seqArgs, fadeTimer);
                    }
                    return;
                }
            }

            found = sNumSeqRequests[playerIdx];
            for (i = 0; i < sNumSeqRequests[playerIdx]; i++) {
                if (D_8016E320[playerIdx][i].unk_1 <= new_var) {
                    found = i;
                    i = sNumSeqRequests[playerIdx]; // "break;"
                }
            }

            if (sNumSeqRequests[playerIdx] < 5) {
                sNumSeqRequests[playerIdx]++;
            }
            for (i = sNumSeqRequests[playerIdx] - 1; i != found; i--) {
                D_8016E320[playerIdx][i].unk_1 = D_8016E320[playerIdx][i - 1].unk_1;
                D_8016E320[playerIdx][i].unk_0 = D_8016E320[playerIdx][i - 1].unk_0;
            }
            D_8016E320[playerIdx][found].unk_1 = seqArgs;
            D_8016E320[playerIdx][found].unk_0 = seqId;

            if (found == 0) {
                func_800F9280(playerIdx, seqId, seqArgs, fadeTimer);
            }
            break;

        case 0x3:
            // unqueue/stop sequence
            seqId = cmd & 0xFF;
            fadeTimer = (cmd & 0xFF0000) >> 13;

            found = sNumSeqRequests[playerIdx];
            for (i = 0; i < sNumSeqRequests[playerIdx]; i++) {
                if (D_8016E320[playerIdx][i].unk_0 == seqId) {
                    found = i;
                    i = sNumSeqRequests[playerIdx]; // "break;"
                }
            }

            if (found != sNumSeqRequests[playerIdx]) {
                for (i = found; i < sNumSeqRequests[playerIdx] - 1; i++) {
                    D_8016E320[playerIdx][i].unk_1 = D_8016E320[playerIdx][i + 1].unk_1;
                    D_8016E320[playerIdx][i].unk_0 = D_8016E320[playerIdx][i + 1].unk_0;
                }
                sNumSeqRequests[playerIdx]--;
            }

            if (found == 0) {
                func_800F9474(playerIdx, fadeTimer);
                if (sNumSeqRequests[playerIdx] != 0) {
                    func_800F9280(playerIdx, D_8016E320[playerIdx][0].unk_0, D_8016E320[playerIdx][0].unk_1, fadeTimer);
                }
            }
            break;

        case 0x4:
            // transition seq volume
            duration = (cmd & 0xFF0000) >> 15;
            val = cmd & 0xFF;
            if (duration == 0) {
                duration++;
            }
            gActiveSeqs[playerIdx].volTarget = (f32)val / 127.0f;
            if (gActiveSeqs[playerIdx].volCur != gActiveSeqs[playerIdx].volTarget) {
                gActiveSeqs[playerIdx].volStep =
                    (gActiveSeqs[playerIdx].volCur - gActiveSeqs[playerIdx].volTarget) / (f32)duration;
                gActiveSeqs[playerIdx].volTimer = duration;
            }
            break;

        case 0x5:
            // transition freq scale for all channels
            duration = (cmd & 0xFF0000) >> 15;
            val = cmd & 0xFFFF;
            if (duration == 0) {
                duration++;
            }
            freqScale = (f32)val / 1000.0f;
            for (i = 0; i < 16; i++) {
                gActiveSeqs[playerIdx].channelData[i].freqScaleTarget = freqScale;
                gActiveSeqs[playerIdx].channelData[i].freqScaleTimer = duration;
                gActiveSeqs[playerIdx].channelData[i].freqScaleStep =
                    (gActiveSeqs[playerIdx].channelData[i].freqScaleCur - freqScale) / (f32)duration;
            }
            gActiveSeqs[playerIdx].freqScaleChannelFlags = 0xFFFF;
            break;

        case 0xD:
            // transition freq scale
            duration = (cmd & 0xFF0000) >> 15;
            chanIdx = (cmd & 0xF000) >> 12;
            val = cmd & 0xFFF;
            if (duration == 0) {
                duration++;
            }
            freqScale = (f32)val / 1000.0f;
            gActiveSeqs[playerIdx].channelData[chanIdx].freqScaleTarget = freqScale;
            gActiveSeqs[playerIdx].channelData[chanIdx].freqScaleStep =
                (gActiveSeqs[playerIdx].channelData[chanIdx].freqScaleCur - freqScale) / (f32)duration;
            gActiveSeqs[playerIdx].channelData[chanIdx].freqScaleTimer = duration;
            gActiveSeqs[playerIdx].freqScaleChannelFlags |= 1 << chanIdx;
            break;

        case 0x6:
            // transition vol scale
            duration = (cmd & 0xFF0000) >> 15;
            chanIdx = (cmd & 0xF00) >> 8;
            val = cmd & 0xFF;
            if (duration == 0) {
                duration++;
            }
            gActiveSeqs[playerIdx].channelData[chanIdx].volTarget = (f32)val / 127.0f;
            if (gActiveSeqs[playerIdx].channelData[chanIdx].volCur != gActiveSeqs[playerIdx].channelData[chanIdx].volTarget) {
                gActiveSeqs[playerIdx].channelData[chanIdx].volStep =
                    (gActiveSeqs[playerIdx].channelData[chanIdx].volCur - gActiveSeqs[playerIdx].channelData[chanIdx].volTarget) /
                    (f32)duration;
                gActiveSeqs[playerIdx].channelData[chanIdx].volTimer = duration;
                gActiveSeqs[playerIdx].volChannelFlags |= 1 << chanIdx;
            }
            break;

        case 0x7:
            // set global io port
            port = (cmd & 0xFF0000) >> 16;
            val = cmd & 0xFF;
            Audio_QueueCmdS8(0x46000000 | _SHIFTL(playerIdx, 16, 8) | _SHIFTL(port, 0, 8), val);
            break;

        case 0x8:
            // set io port if channel masked
            chanIdx = (cmd & 0xF00) >> 8;
            port = (cmd & 0xFF0000) >> 16;
            val = cmd & 0xFF;
            if ((gActiveSeqs[playerIdx].channelPortMask & (1 << chanIdx)) == 0) {
                Audio_QueueCmdS8(0x06000000 | _SHIFTL(playerIdx, 16, 8) | _SHIFTL(chanIdx, 8, 8) | _SHIFTL(port, 0, 8),
                                 val);
            }
            break;

        case 0x9:
            // set channel mask for command 0x8
            gActiveSeqs[playerIdx].channelPortMask = cmd & 0xFFFF;
            break;

        case 0xA:
            // set channel stop mask
            channelMask = cmd & 0xFFFF;
            if (channelMask != 0) {
                // with channel mask channelMask...
                Audio_QueueCmdU16(0x90000000 | _SHIFTL(playerIdx, 16, 8), channelMask);
                // stop channels
                Audio_QueueCmdS8(0x08000000 | _SHIFTL(playerIdx, 16, 8) | 0xFF00, 1);
            }
            if ((channelMask ^ 0xFFFF) != 0) {
                // with channel mask ~channelMask...
                Audio_QueueCmdU16(0x90000000 | _SHIFTL(playerIdx, 16, 8), (channelMask ^ 0xFFFF));
                // unstop channels
                Audio_QueueCmdS8(0x08000000 | _SHIFTL(playerIdx, 16, 8) | 0xFF00, 0);
            }
            break;

        case 0xB:
            // update tempo
            gActiveSeqs[playerIdx].tempoCmd = cmd;
            break;

        case 0xC:
            // start sequence with setup commands
            subOp = (cmd & 0xF00000) >> 20;
            if (subOp != 0xF) {
                if (gActiveSeqs[playerIdx].setupCmdNum < 7) {
                    found = gActiveSeqs[playerIdx].setupCmdNum++;
                    if (found < 8) {
                        gActiveSeqs[playerIdx].setupCmd[found] = cmd;
                        gActiveSeqs[playerIdx].setupCmdTimer = 2;
                    }
                }
            } else {
                gActiveSeqs[playerIdx].setupCmdNum = 0;
            }
            break;

        case 0xE:
            subOp = (cmd & 0xF00) >> 8;
            val = cmd & 0xFF;
            switch (subOp) {
                case 0:
                    // set sound mode
                    Audio_QueueCmdS32(0xF0000000, D_80133410[val]);
                    break;
                case 1:
                    // set sequence starting disabled?
                    D_80133408 = val & 1;
                    break;
            }
            break;

        case 0xF:
            // change spec
            spec = cmd & 0xFF;
            gSfxChannelLayout = (cmd & 0xFF00) >> 8;
            oldSpec = gAudioSpecId;
            gAudioSpecId = spec;
            func_800E5F88(spec);
            func_800F71BC(oldSpec);
            Audio_QueueCmdS32(0xF8000000, 0);
            break;
    }
}

extern f32 D_80130F24;
extern f32 D_80130F28;

void Audio_QueueSeqCmd(u32 cmd) 
{
    u8 op = cmd >> 28;
    if (op == 0 || op == 2 || op == 12) {
        u8 seqId = cmd & 0xFF;
        u8 playerIdx = GET_PLAYER_IDX(cmd);
        u16 newSeqId = AudioEditor_GetReplacementSeq(seqId);
        gAudioContext.seqReplaced[playerIdx] = (seqId != newSeqId);
        gAudioContext.seqToPlay[playerIdx] = newSeqId;
        cmd |= (seqId & 0xFF);
    }

    sAudioSeqCmds[sSeqCmdWrPos++] = cmd;
}

void Audio_QueuePreviewSeqCmd(u16 seqId) {
    gAudioContext.seqReplaced[0] = 1;
    gAudioContext.seqToPlay[0] = seqId;
    sAudioSeqCmds[sSeqCmdWrPos++] = 1;
}

void Audio_ProcessSeqCmds(void) {
    while (sSeqCmdWrPos != sSeqCmdRdPos) {
        Audio_ProcessSeqCmd(sAudioSeqCmds[sSeqCmdRdPos++]);
    }
}

u16 func_800FA0B4(u8 playerIdx) {
    if (!gAudioContext.seqPlayers[playerIdx].enabled) {
        return NA_BGM_DISABLED;
    }
    return gActiveSeqs[playerIdx].seqId;
}

s32 func_800FA11C(u32 arg0, u32 arg1) {
    u8 i;

    for (i = sSeqCmdRdPos; i != sSeqCmdWrPos; i++) {
        if (arg0 == (sAudioSeqCmds[i] & arg1)) {
            return false;
        }
    }

    return true;
}

void func_800FA174(u8 playerIdx) {
    sNumSeqRequests[playerIdx] = 0;
}

void func_800FA18C(u8 playerIdx, u8 arg1) {
    u8 i;

    for (i = 0; i < gActiveSeqs[playerIdx].setupCmdNum; i++) {
        u8 unkb = (gActiveSeqs[playerIdx].setupCmd[i] & 0xF00000) >> 20;

        if (unkb == arg1) {
            gActiveSeqs[playerIdx].setupCmd[i] = 0xFF000000;
        }
    }
}

void Audio_SetVolScale(u8 playerIdx, u8 scaleIdx, u8 targetVol, u8 volFadeTimer) {
    f32 volScale;
    u8 i;

    gActiveSeqs[playerIdx].volScales[scaleIdx] = targetVol & 0x7F;

    if (volFadeTimer != 0) {
        gActiveSeqs[playerIdx].fadeVolUpdate = 1;
        gActiveSeqs[playerIdx].volFadeTimer = volFadeTimer;
    } else {
        for (i = 0, volScale = 1.0f; i < 4; i++) {
            volScale *= gActiveSeqs[playerIdx].volScales[i] / 127.0f;
        }

        Audio_SetVolScaleNow(playerIdx, volFadeTimer, volScale);
    }
}

void func_800FA3DC(void) {
    u32 temp_a1;
    u16 temp_lo;
    u16 temp_v1;
    u16 phi_a2;
    u8 temp_v0_4;
    u8 temp_a0;
    u8 temp_s1;
    u8 temp_s0_3;
    u8 temp_a3_3;
    s32 pad[3];
    u32 dummy;
    f32 phi_f0;
    u8 phi_t0;
    u8 playerIdx;
    u8 j;
    u8 k;

    for (playerIdx = 0; playerIdx < 4; playerIdx++) {
        if (gActiveSeqs[playerIdx].isWaitingForFonts != 0) {
            switch (func_800E5E20(&dummy)) {
                case 1:
                case 2:
                case 3:
                case 4:
                    gActiveSeqs[playerIdx].isWaitingForFonts = 0;
                    Audio_ProcessSeqCmd(gActiveSeqs[playerIdx].startSeqCmd);
                    break;
            }
        }

        if (gActiveSeqs[playerIdx].fadeVolUpdate) {
            phi_f0 = 1.0f;
            for (j = 0; j < 4; j++) {
                phi_f0 *= (gActiveSeqs[playerIdx].volScales[j] / 127.0f);
            }
            Audio_SeqCmd4(playerIdx, gActiveSeqs[playerIdx].volFadeTimer, (u8)(phi_f0 * 127.0f));
            gActiveSeqs[playerIdx].fadeVolUpdate = 0;
        }

        if (gActiveSeqs[playerIdx].volTimer != 0) {
            gActiveSeqs[playerIdx].volTimer--;

            if (gActiveSeqs[playerIdx].volTimer != 0) {
                gActiveSeqs[playerIdx].volCur = gActiveSeqs[playerIdx].volCur - gActiveSeqs[playerIdx].volStep;
            } else {
                gActiveSeqs[playerIdx].volCur = gActiveSeqs[playerIdx].volTarget;
            }

            Audio_QueueCmdF32(0x41000000 | _SHIFTL(playerIdx, 16, 8), gActiveSeqs[playerIdx].volCur);
        }

        if (gActiveSeqs[playerIdx].tempoCmd != 0) {
            temp_a1 = gActiveSeqs[playerIdx].tempoCmd;
            phi_t0 = (temp_a1 & 0xFF0000) >> 15;
            phi_a2 = temp_a1 & 0xFFF;
            if (phi_t0 == 0) {
                phi_t0++;
            }

            if (gAudioContext.seqPlayers[playerIdx].enabled) {
                temp_lo = gAudioContext.seqPlayers[playerIdx].tempo / 0x30;
                temp_v0_4 = (temp_a1 & 0xF000) >> 12;
                switch (temp_v0_4) {
                    case 1:
                        phi_a2 += temp_lo;
                        break;
                    case 2:
                        if (phi_a2 < temp_lo) {
                            phi_a2 = temp_lo - phi_a2;
                        }
                        break;
                    case 3:
                        phi_a2 = temp_lo * (phi_a2 / 100.0f);
                        break;
                    case 4:
                        if (gActiveSeqs[playerIdx].tempoOriginal) {
                            phi_a2 = gActiveSeqs[playerIdx].tempoOriginal;
                        } else {
                            phi_a2 = temp_lo;
                        }
                        break;
                }

                if (phi_a2 > 300) {
                    phi_a2 = 300;
                }

                if (gActiveSeqs[playerIdx].tempoOriginal == 0) {
                    gActiveSeqs[playerIdx].tempoOriginal = temp_lo;
                }

                gActiveSeqs[playerIdx].tempoTarget = phi_a2;
                gActiveSeqs[playerIdx].tempoCur = gAudioContext.seqPlayers[playerIdx].tempo / 0x30;
                gActiveSeqs[playerIdx].tempoStep = (gActiveSeqs[playerIdx].tempoCur - gActiveSeqs[playerIdx].tempoTarget) / phi_t0;
                gActiveSeqs[playerIdx].tempoTimer = phi_t0;
                gActiveSeqs[playerIdx].tempoCmd = 0;
            }
        }

        if (gActiveSeqs[playerIdx].tempoTimer != 0) {
            gActiveSeqs[playerIdx].tempoTimer--;
            if (gActiveSeqs[playerIdx].tempoTimer != 0) {
                gActiveSeqs[playerIdx].tempoCur = gActiveSeqs[playerIdx].tempoCur - gActiveSeqs[playerIdx].tempoStep;
            } else {
                gActiveSeqs[playerIdx].tempoCur = gActiveSeqs[playerIdx].tempoTarget;
            }
            // set tempo
            Audio_QueueCmdS32(0x47000000 | _SHIFTL(playerIdx, 16, 8), gActiveSeqs[playerIdx].tempoCur);
        }

        if (gActiveSeqs[playerIdx].volChannelFlags != 0) {
            for (k = 0; k < 0x10; k++) {
                if (gActiveSeqs[playerIdx].channelData[k].volTimer != 0) {
                    gActiveSeqs[playerIdx].channelData[k].volTimer--;
                    if (gActiveSeqs[playerIdx].channelData[k].volTimer != 0) {
                        gActiveSeqs[playerIdx].channelData[k].volCur -= gActiveSeqs[playerIdx].channelData[k].volStep;
                    } else {
                        gActiveSeqs[playerIdx].channelData[k].volCur = gActiveSeqs[playerIdx].channelData[k].volTarget;
                        gActiveSeqs[playerIdx].volChannelFlags ^= (1 << k);
                    }
                    // CHAN_UPD_VOL_SCALE (playerIdx = seq, k = chan)
                    Audio_QueueCmdF32(0x01000000 | _SHIFTL(playerIdx, 16, 8) | _SHIFTL(k, 8, 8),
                                      gActiveSeqs[playerIdx].channelData[k].volCur);
                }
            }
        }

        if (gActiveSeqs[playerIdx].freqScaleChannelFlags != 0) {
            for (k = 0; k < 0x10; k++) {
                if (gActiveSeqs[playerIdx].channelData[k].freqScaleTimer != 0) {
                    gActiveSeqs[playerIdx].channelData[k].freqScaleTimer--;
                    if (gActiveSeqs[playerIdx].channelData[k].freqScaleTimer != 0) {
                        gActiveSeqs[playerIdx].channelData[k].freqScaleCur -= gActiveSeqs[playerIdx].channelData[k].freqScaleStep;
                    } else {
                        gActiveSeqs[playerIdx].channelData[k].freqScaleCur = gActiveSeqs[playerIdx].channelData[k].freqScaleTarget;
                        gActiveSeqs[playerIdx].freqScaleChannelFlags ^= (1 << k);
                    }
                    // CHAN_UPD_FREQ_SCALE
                    Audio_QueueCmdF32(0x04000000 | _SHIFTL(playerIdx, 16, 8) | _SHIFTL(k, 8, 8),
                                      gActiveSeqs[playerIdx].channelData[k].freqScaleCur);
                }
            }
        }

        if (gActiveSeqs[playerIdx].setupCmdNum != 0) {
            if (func_800FA11C(0xF0000000, 0xF0000000) == 0) {
                gActiveSeqs[playerIdx].setupCmdNum = 0;
                return;
            }

            if (gActiveSeqs[playerIdx].setupCmdTimer != 0) {
                gActiveSeqs[playerIdx].setupCmdTimer--;
                continue;
            }

            if (gAudioContext.seqPlayers[playerIdx].enabled) {
                continue;
            }

            for (j = 0; j < gActiveSeqs[playerIdx].setupCmdNum; j++) {
                temp_a0 = (gActiveSeqs[playerIdx].setupCmd[j] & 0x00F00000) >> 20;
                temp_s1 = (gActiveSeqs[playerIdx].setupCmd[j] & 0x000F0000) >> 16;
                temp_s0_3 = (gActiveSeqs[playerIdx].setupCmd[j] & 0xFF00) >> 8;
                temp_a3_3 = gActiveSeqs[playerIdx].setupCmd[j] & 0xFF;

                switch (temp_a0) {
                    case 0:
                        Audio_SetVolScale(temp_s1, 1, 0x7F, temp_a3_3);
                        break;
                    case 7:
                        if (sNumSeqRequests[playerIdx] == temp_a3_3) {
                            Audio_SetVolScale(temp_s1, 1, 0x7F, temp_s0_3);
                        }
                        break;
                    case 1:
                        Audio_SeqCmd3(playerIdx, gActiveSeqs[playerIdx].seqId);
                        break;
                    case 2:
                        Audio_StartSeq(temp_s1, 1, gActiveSeqs[temp_s1].seqId);
                        gActiveSeqs[temp_s1].fadeVolUpdate = 1;
                        gActiveSeqs[temp_s1].volScales[1] = 0x7F;
                        break;
                    case 3:
                        Audio_SeqCmdB30(temp_s1, temp_s0_3, temp_a3_3);
                        break;
                    case 4:
                        Audio_SeqCmdB40(temp_s1, temp_a3_3, 0);
                        break;
                    case 5:
                        temp_v1 = gActiveSeqs[playerIdx].setupCmd[j] & 0xFFFF;
                        Audio_StartSeq(temp_s1, gActiveSeqs[temp_s1].setupFadeTimer, temp_v1);
                        Audio_SetVolScale(temp_s1, 1, 0x7F, 0);
                        gActiveSeqs[temp_s1].setupFadeTimer = 0;
                        break;
                    case 6:
                        gActiveSeqs[playerIdx].setupFadeTimer = temp_s0_3;
                        break;
                    case 8:
                        Audio_SetVolScale(temp_s1, temp_s0_3, 0x7F, temp_a3_3);
                        break;
                    case 14:
                        if (temp_a3_3 & 1) {
                            Audio_QueueCmdS32(0xE3000000, SEQUENCE_TABLE);
                        }
                        if (temp_a3_3 & 2) {
                            Audio_QueueCmdS32(0xE3000000, FONT_TABLE);
                        }
                        if (temp_a3_3 & 4) {
                            Audio_QueueCmdS32(0xE3000000, SAMPLE_TABLE);
                        }
                        break;
                    case 9:
                        temp_v1 = gActiveSeqs[playerIdx].setupCmd[j] & 0xFFFF;
                        Audio_SeqCmdA(temp_s1, temp_v1);
                        break;
                    case 10:
                        Audio_SeqCmd5(temp_s1, temp_s0_3, (temp_a3_3 * 10) & 0xFFFF);
                        break;
                }
            }

            gActiveSeqs[playerIdx].setupCmdNum = 0;
        }
    }
}

u8 func_800FAD34(void) {
    if (D_80133418 != 0) {
        if (D_80133418 == 1) {
            if (func_800E5EDC() == 1) {
                D_80133418 = 0;
                Audio_QueueCmdS8(0x46020000, gSfxChannelLayout);
                func_800F7170();
            }
        } else if (D_80133418 == 2) {
            while (func_800E5EDC() != 1) {}
            D_80133418 = 0;
            Audio_QueueCmdS8(0x46020000, gSfxChannelLayout);
            func_800F7170();
        }
    }

    return D_80133418;
}

void Audio_ResetActiveSequences(void) {
    u8 seqPlayerIndex;
    u8 scaleIndex;

    for (seqPlayerIndex = 0; seqPlayerIndex < 4; seqPlayerIndex++) {
        sNumSeqRequests[seqPlayerIndex] = 0;

        gActiveSeqs[seqPlayerIndex].seqId = NA_BGM_DISABLED;
        gActiveSeqs[seqPlayerIndex].prevSeqId = NA_BGM_DISABLED;
        gActiveSeqs[seqPlayerIndex].tempoTimer = 0;
        gActiveSeqs[seqPlayerIndex].tempoOriginal = 0;
        gActiveSeqs[seqPlayerIndex].tempoCmd = 0;
        gActiveSeqs[seqPlayerIndex].channelPortMask = 0;
        gActiveSeqs[seqPlayerIndex].setupCmdNum = 0;
        gActiveSeqs[seqPlayerIndex].setupFadeTimer = 0;
        gActiveSeqs[seqPlayerIndex].freqScaleChannelFlags = 0;
        gActiveSeqs[seqPlayerIndex].volChannelFlags = 0;
        for (scaleIndex = 0; scaleIndex < 4; scaleIndex++) {
            gActiveSeqs[seqPlayerIndex].volScales[scaleIndex] = 0x7F;
        }

        gActiveSeqs[seqPlayerIndex].volFadeTimer = 1;
        gActiveSeqs[seqPlayerIndex].fadeVolUpdate = true;
    }
}

void func_800FAEB4(void) {
    u8 playerIdx, j;

    for (playerIdx = 0; playerIdx < 4; playerIdx++) {
        gActiveSeqs[playerIdx].volCur = 1.0f;
        gActiveSeqs[playerIdx].volTimer = 0;
        gActiveSeqs[playerIdx].fadeVolUpdate = 0;
        for (j = 0; j < 4; j++) {
            gActiveSeqs[playerIdx].volScales[j] = 0x7F;
        }
    }
    Audio_ResetActiveSequences();
}
