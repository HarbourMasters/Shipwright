#pragma once
#define dgGoronShopBgTLUT "__OTR__textures/vr_GLVR_pal_static/gGoronShopBgTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gGoronShopBgTLUT[] = dgGoronShopBgTLUT;
#else
static const char gGoronShopBgTLUT[] __attribute__((aligned (2))) = dgGoronShopBgTLUT;
#endif
            
#define dgGoronShopBg2TLUT "__OTR__textures/vr_GLVR_pal_static/gGoronShopBg2TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gGoronShopBg2TLUT[] = dgGoronShopBg2TLUT;
#else
static const char gGoronShopBg2TLUT[] __attribute__((aligned (2))) = dgGoronShopBg2TLUT;
#endif
            

