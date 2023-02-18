#pragma once
#define dsWipe1Vtx "__OTR__code/z_fbdemo_wipe1/sWipe1Vtx"
#ifdef _WIN32
static const __declspec(align(2)) char sWipe1Vtx[] = dsWipe1Vtx;
#else
static const char sWipe1Vtx[] __attribute__((aligned (2))) = dsWipe1Vtx;
#endif
            
#define dsWipe1Tex "__OTR__code/z_fbdemo_wipe1/sWipe1Tex"
#ifdef _WIN32
static const __declspec(align(2)) char sWipe1Tex[] = dsWipe1Tex;
#else
static const char sWipe1Tex[] __attribute__((aligned (2))) = dsWipe1Tex;
#endif
            

