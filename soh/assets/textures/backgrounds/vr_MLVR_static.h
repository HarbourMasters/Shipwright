#pragma once
#define dgStableBgTex "__OTR__textures/vr_MLVR_static/gStableBgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gStableBgTex[] = dgStableBgTex;
#else
static const char gStableBgTex[] __attribute__((aligned (2))) = dgStableBgTex;
#endif
            
#define dgStable2BgTex "__OTR__textures/vr_MLVR_static/gStable2BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gStable2BgTex[] = dgStable2BgTex;
#else
static const char gStable2BgTex[] __attribute__((aligned (2))) = dgStable2BgTex;
#endif
            
#define dgStable3BgTex "__OTR__textures/vr_MLVR_static/gStable3BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gStable3BgTex[] = dgStable3BgTex;
#else
static const char gStable3BgTex[] __attribute__((aligned (2))) = dgStable3BgTex;
#endif
            
#define dgStable4BgTex "__OTR__textures/vr_MLVR_static/gStable4BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gStable4BgTex[] = dgStable4BgTex;
#else
static const char gStable4BgTex[] __attribute__((aligned (2))) = dgStable4BgTex;
#endif
            

