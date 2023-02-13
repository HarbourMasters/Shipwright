#pragma once
#define dgBazaarBgTex "__OTR__textures/vr_SP1a_static/gBazaarBgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gBazaarBgTex[] = dgBazaarBgTex;
#else
static const char gBazaarBgTex[] __attribute__((aligned (2))) = dgBazaarBgTex;
#endif
            
#define dgBazaar2BgTex "__OTR__textures/vr_SP1a_static/gBazaar2BgTex"
#ifdef _WIN32
static const __declspec(align(2)) char gBazaar2BgTex[] = dgBazaar2BgTex;
#else
static const char gBazaar2BgTex[] __attribute__((aligned (2))) = dgBazaar2BgTex;
#endif
            

