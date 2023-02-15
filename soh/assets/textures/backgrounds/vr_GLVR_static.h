#pragma once
#define dgGoronShopBgTex "__OTR__textures/vr_GLVR_static/gGoronShopBgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gGoronShopBgTex[] = dgGoronShopBgTex;
#else
static const char gGoronShopBgTex[] __attribute__((aligned (2))) = dgGoronShopBgTex;
#endif
            
#define dgGoronShop2BgTex "__OTR__textures/vr_GLVR_static/gGoronShop2BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gGoronShop2BgTex[] = dgGoronShop2BgTex;
#else
static const char gGoronShop2BgTex[] __attribute__((aligned (2))) = dgGoronShop2BgTex;
#endif
            

