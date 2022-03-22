#ifndef ULTRA64_MOTOR_H
#define ULTRA64_MOTOR_H

#include "pfs.h"

#define MOTOR_START 1
#define MOTOR_STOP  0

#define osMotorStart(x) __osMotorAccess((x), MOTOR_START)
#define osMotorStop(x)  __osMotorAccess((x), MOTOR_STOP)

s32 __osMotorAccess(OSPfs* pfs, u32 vibrate);
s32 osMotorInit(OSMesgQueue* ctrlrqueue, OSPfs* pfs, s32 channel);

#endif
