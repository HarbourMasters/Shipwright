#pragma once
#define dgKokiriShopBgTLUT "__OTR__textures/vr_KSVR_pal_static/gKokiriShopBgTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gKokiriShopBgTLUT[] = dgKokiriShopBgTLUT;
#else
static const char gKokiriShopBgTLUT[] __attribute__((aligned (2))) = dgKokiriShopBgTLUT;
#endif
            
#define dgKokiriShopBg2TLUT "__OTR__textures/vr_KSVR_pal_static/gKokiriShopBg2TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gKokiriShopBg2TLUT[] = dgKokiriShopBg2TLUT;
#else
static const char gKokiriShopBg2TLUT[] __attribute__((aligned (2))) = dgKokiriShopBg2TLUT;
#endif
            

