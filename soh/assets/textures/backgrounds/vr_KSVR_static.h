#pragma once
#define dgKokiriShopBgTex "__OTR__textures/vr_KSVR_static/gKokiriShopBgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gKokiriShopBgTex[] = dgKokiriShopBgTex;
#else
static const char gKokiriShopBgTex[] __attribute__((aligned (2))) = dgKokiriShopBgTex;
#endif
            
#define dgKokiriShop2BgTex "__OTR__textures/vr_KSVR_static/gKokiriShop2BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gKokiriShop2BgTex[] = dgKokiriShop2BgTex;
#else
static const char gKokiriShop2BgTex[] __attribute__((aligned (2))) = dgKokiriShop2BgTex;
#endif
            

