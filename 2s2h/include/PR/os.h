#ifndef PR_OS_H
#define PR_OS_H

#include "os_ai.h"
#include "os_cache.h"
#include "os_cont.h"
#include "os_convert.h"
#include "os_exception.h"
#include "os_flash.h"
#include "os_host.h"
#include "os_internal_error.h"
#include "os_internal_reg.h"
#include "os_internal_si.h"
#include "os_internal.h"
#include "os_libc.h"
#include "os_message.h"
#include "os_pfs.h"
#include "os_pi.h"
#include "os_reg.h"
#include "os_system.h"
#include "os_thread.h"
#include "os_time.h"
#include "os_tlb.h"
#include "os_vi.h"
#include "os_voice.h"

/*
 * Stack size for I/O device managers: PIM (PI Manager), VIM (VI Manager),
 *	SIM (SI Manager)
 *
 */
#define OS_PIM_STACKSIZE	4096
#define OS_VIM_STACKSIZE	4096
#define OS_SIM_STACKSIZE	4096

#define	OS_MIN_STACKSIZE	72

#endif
