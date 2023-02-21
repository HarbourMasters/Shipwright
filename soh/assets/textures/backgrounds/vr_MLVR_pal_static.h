#pragma once
#define dgStableBgTLUT "__OTR__textures/vr_MLVR_pal_static/gStableBgTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gStableBgTLUT[] = dgStableBgTLUT;
#else
static const char gStableBgTLUT[] __attribute__((aligned (2))) = dgStableBgTLUT;
#endif
            
#define dgStableBg2TLUT "__OTR__textures/vr_MLVR_pal_static/gStableBg2TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gStableBg2TLUT[] = dgStableBg2TLUT;
#else
static const char gStableBg2TLUT[] __attribute__((aligned (2))) = dgStableBg2TLUT;
#endif
            
#define dgStableBg3TLUT "__OTR__textures/vr_MLVR_pal_static/gStableBg3TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gStableBg3TLUT[] = dgStableBg3TLUT;
#else
static const char gStableBg3TLUT[] __attribute__((aligned (2))) = dgStableBg3TLUT;
#endif
            
#define dgStableBg4TLUT "__OTR__textures/vr_MLVR_pal_static/gStableBg4TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gStableBg4TLUT[] = dgStableBg4TLUT;
#else
static const char gStableBg4TLUT[] __attribute__((aligned (2))) = dgStableBg4TLUT;
#endif
            

