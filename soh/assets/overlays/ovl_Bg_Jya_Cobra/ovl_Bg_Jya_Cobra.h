#pragma once
#define dsShadowVtx "__OTR__overlays/ovl_Bg_Jya_Cobra/sShadowVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sShadowVtx[] = dsShadowVtx;
#else
static const char sShadowVtx[] __attribute__((aligned (2))) = dsShadowVtx;
#endif
            
#define dsShadowDL "__OTR__overlays/ovl_Bg_Jya_Cobra/sShadowDL"
#ifdef _WIN32
static const __declspec(align(2)) char sShadowDL[] = dsShadowDL;
#else
static const char sShadowDL[] __attribute__((aligned (2))) = dsShadowDL;
#endif
            

