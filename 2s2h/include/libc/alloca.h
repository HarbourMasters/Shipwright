#ifndef LIBC_ALLOCA_H
#define LIBC_ALLOCA_H

#ifndef __linux__
void* alloca(u32);
#endif

#define alloca  __builtin_alloca

#endif
