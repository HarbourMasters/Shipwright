#pragma once
#define dgZoraShopBgTLUT "__OTR__textures/vr_ZRVR_pal_static/gZoraShopBgTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gZoraShopBgTLUT[] = dgZoraShopBgTLUT;
#else
static const char gZoraShopBgTLUT[] __attribute__((aligned (2))) = dgZoraShopBgTLUT;
#endif
            
#define dgZoraShopBg2TLUT "__OTR__textures/vr_ZRVR_pal_static/gZoraShopBg2TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gZoraShopBg2TLUT[] = dgZoraShopBg2TLUT;
#else
static const char gZoraShopBg2TLUT[] __attribute__((aligned (2))) = dgZoraShopBg2TLUT;
#endif
            

