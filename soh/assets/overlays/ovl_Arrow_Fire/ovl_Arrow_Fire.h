#pragma once
#define ds1Tex "__OTR__overlays/ovl_Arrow_Fire/s1Tex"
#ifdef _WIN32
static const __declspec(align(2)) char s1Tex[] = ds1Tex;
#else
static const char s1Tex[] __attribute__((aligned (2))) = ds1Tex;
#endif
            
#define ds2Tex "__OTR__overlays/ovl_Arrow_Fire/s2Tex"
#ifdef _WIN32
static const __declspec(align(2)) char s2Tex[] = ds2Tex;
#else
static const char s2Tex[] __attribute__((aligned (2))) = ds2Tex;
#endif
            
#define dsVtx "__OTR__overlays/ovl_Arrow_Fire/sVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sVtx[] = dsVtx;
#else
static const char sVtx[] __attribute__((aligned (2))) = dsVtx;
#endif
            
#define dsMaterialDL "__OTR__overlays/ovl_Arrow_Fire/sMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sMaterialDL[] = dsMaterialDL;
#else
static const char sMaterialDL[] __attribute__((aligned (2))) = dsMaterialDL;
#endif
            
#define dsModelDL "__OTR__overlays/ovl_Arrow_Fire/sModelDL"
#ifdef _WIN32
static const __declspec(align(2)) char sModelDL[] = dsModelDL;
#else
static const char sModelDL[] __attribute__((aligned (2))) = dsModelDL;
#endif
            

