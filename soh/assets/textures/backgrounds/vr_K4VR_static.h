#pragma once
#define dgMidosHouseBgTex "__OTR__textures/vr_K4VR_static/gMidosHouseBgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMidosHouseBgTex[] = dgMidosHouseBgTex;
#else
static const char gMidosHouseBgTex[] __attribute__((aligned (2))) = dgMidosHouseBgTex;
#endif
            
#define dgMidosHouse2BgTex "__OTR__textures/vr_K4VR_static/gMidosHouse2BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMidosHouse2BgTex[] = dgMidosHouse2BgTex;
#else
static const char gMidosHouse2BgTex[] __attribute__((aligned (2))) = dgMidosHouse2BgTex;
#endif
            
#define dgMidosHouse3BgTex "__OTR__textures/vr_K4VR_static/gMidosHouse3BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMidosHouse3BgTex[] = dgMidosHouse3BgTex;
#else
static const char gMidosHouse3BgTex[] __attribute__((aligned (2))) = dgMidosHouse3BgTex;
#endif
            

