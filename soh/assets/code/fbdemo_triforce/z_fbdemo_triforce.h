#pragma once
#define dsTriforceWipeDL "__OTR__code/z_fbdemo_triforce/sTriforceWipeDL"
#ifdef _WIN32
static const __declspec(align(2)) char sTriforceWipeDL[] = dsTriforceWipeDL;
#else
static const char sTriforceWipeDL[] __attribute__((aligned (2))) = dsTriforceWipeDL;
#endif
            
#define dsTriforceWipeVtx "__OTR__code/z_fbdemo_triforce/sTriforceWipeVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sTriforceWipeVtx[] = dsTriforceWipeVtx;
#else
static const char sTriforceWipeVtx[] __attribute__((aligned (2))) = dsTriforceWipeVtx;
#endif
            

