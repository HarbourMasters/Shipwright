#pragma once

// SoH: Helper for headers which can be included in both C & C++

#if __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif