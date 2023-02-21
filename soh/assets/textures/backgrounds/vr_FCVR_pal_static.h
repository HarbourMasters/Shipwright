#pragma once
#define dgMaskShopBgTLUT "__OTR__textures/vr_FCVR_pal_static/gMaskShopBgTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gMaskShopBgTLUT[] = dgMaskShopBgTLUT;
#else
static const char gMaskShopBgTLUT[] __attribute__((aligned (2))) = dgMaskShopBgTLUT;
#endif
            
#define dgMaskShopBg2TLUT "__OTR__textures/vr_FCVR_pal_static/gMaskShopBg2TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gMaskShopBg2TLUT[] = dgMaskShopBg2TLUT;
#else
static const char gMaskShopBg2TLUT[] __attribute__((aligned (2))) = dgMaskShopBg2TLUT;
#endif
            

