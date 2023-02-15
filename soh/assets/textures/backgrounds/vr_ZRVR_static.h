#pragma once
#define dgZoraShopBgTex "__OTR__textures/vr_ZRVR_static/gZoraShopBgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gZoraShopBgTex[] = dgZoraShopBgTex;
#else
static const char gZoraShopBgTex[] __attribute__((aligned (2))) = dgZoraShopBgTex;
#endif
            
#define dgZoraShop2BgTex "__OTR__textures/vr_ZRVR_static/gZoraShop2BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gZoraShop2BgTex[] = dgZoraShop2BgTex;
#else
static const char gZoraShop2BgTex[] __attribute__((aligned (2))) = dgZoraShop2BgTex;
#endif
            

