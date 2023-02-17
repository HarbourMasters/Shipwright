#pragma once
#define dgMidosHouseBgTLUT "__OTR__textures/vr_K4VR_pal_static/gMidosHouseBgTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gMidosHouseBgTLUT[] = dgMidosHouseBgTLUT;
#else
static const char gMidosHouseBgTLUT[] __attribute__((aligned (2))) = dgMidosHouseBgTLUT;
#endif
            
#define dgMidosHouseBg2TLUT "__OTR__textures/vr_K4VR_pal_static/gMidosHouseBg2TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gMidosHouseBg2TLUT[] = dgMidosHouseBg2TLUT;
#else
static const char gMidosHouseBg2TLUT[] __attribute__((aligned (2))) = dgMidosHouseBg2TLUT;
#endif
            
#define dgMidosHouseBg3TLUT "__OTR__textures/vr_K4VR_pal_static/gMidosHouseBg3TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gMidosHouseBg3TLUT[] = dgMidosHouseBg3TLUT;
#else
static const char gMidosHouseBg3TLUT[] __attribute__((aligned (2))) = dgMidosHouseBg3TLUT;
#endif
            

