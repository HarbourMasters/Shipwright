#include "ultra64.h"

s32 osAiSetFrequency(u32 frequency) {
    u8 bitrate;
    f32 dacRateF = ((f32)osViClock / frequency) + 0.5f;
    u32 dacRate = dacRateF;

    if (dacRate < AI_MIN_DAC_RATE) {
        return -1;
    }

    bitrate = (dacRate / 66);
    if (bitrate > AI_MAX_BIT_RATE) {
        bitrate = AI_MAX_BIT_RATE;
    }

    IO_WRITE(AI_DACRATE_REG, dacRate - 1);
    IO_WRITE(AI_BITRATE_REG, bitrate - 1);
    return osViClock / (s32)dacRate;
}
