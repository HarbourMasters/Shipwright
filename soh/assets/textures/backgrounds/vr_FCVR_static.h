#pragma once
#define dgMaskShopBgTex "__OTR__textures/vr_FCVR_static/gMaskShopBgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMaskShopBgTex[] = dgMaskShopBgTex;
#else
static const char gMaskShopBgTex[] __attribute__((aligned (2))) = dgMaskShopBgTex;
#endif
            
#define dgMaskShop2BgTex "__OTR__textures/vr_FCVR_static/gMaskShop2BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMaskShop2BgTex[] = dgMaskShop2BgTex;
#else
static const char gMaskShop2BgTex[] __attribute__((aligned (2))) = dgMaskShop2BgTex;
#endif
            

