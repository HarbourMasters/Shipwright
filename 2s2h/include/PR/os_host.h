#ifndef PR_OS_HOST_H
#define PR_OS_HOST_H

void __osInitialize_common(void);
void __osInitialize_autodetect(void);

#define osInitialize() __osInitialize_common()

#endif
