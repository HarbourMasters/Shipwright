#ifndef PR_OS_MOTOR_H
#define PR_OS_MOTOR_H

#include <libultraship/libultra/motor.h>

#if 0

#include "ultratypes.h"
#include "os_pfs.h"
#include "os_message.h"

#define MOTOR_START 1
#define MOTOR_STOP  0

#define osMotorStart(x) __osMotorAccess((x), MOTOR_START)
#define osMotorStop(x)  __osMotorAccess((x), MOTOR_STOP)

s32 __osMotorAccess(OSPfs* pfs, s32 flag);

s32 osMotorInit(OSMesgQueue* mq, OSPfs* pfs, s32 channel);

#endif // 0

#endif
