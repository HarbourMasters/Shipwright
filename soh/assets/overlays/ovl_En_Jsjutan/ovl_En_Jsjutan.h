#pragma once
#define dsCarpetTex "__OTR__overlays/ovl_En_Jsjutan/sCarpetTex"
#ifdef _WIN32
static const __declspec(align(2)) char sCarpetTex[] = dsCarpetTex;
#else
static const char sCarpetTex[] __attribute__((aligned (2))) = dsCarpetTex;
#endif
            
#define dgShadowOddVtx "__OTR__overlays/ovl_En_Jsjutan/gShadowOddVtx"
#ifdef _WIN32
static const __declspec(align(2)) char gShadowOddVtx[] = dgShadowOddVtx;
#else
static const char gShadowOddVtx[] __attribute__((aligned (2))) = dgShadowOddVtx;
#endif
            
#define dsShadowEvenVtx "__OTR__overlays/ovl_En_Jsjutan/sShadowEvenVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sShadowEvenVtx[] = dsShadowEvenVtx;
#else
static const char sShadowEvenVtx[] __attribute__((aligned (2))) = dsShadowEvenVtx;
#endif
            
#define dsCarpetOddVtx "__OTR__overlays/ovl_En_Jsjutan/sCarpetOddVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sCarpetOddVtx[] = dsCarpetOddVtx;
#else
static const char sCarpetOddVtx[] __attribute__((aligned (2))) = dsCarpetOddVtx;
#endif
            
#define dsCarpetMaterialDL "__OTR__overlays/ovl_En_Jsjutan/sCarpetMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sCarpetMaterialDL[] = dsCarpetMaterialDL;
#else
static const char sCarpetMaterialDL[] __attribute__((aligned (2))) = dsCarpetMaterialDL;
#endif
            
#define dsShadowMaterialDL "__OTR__overlays/ovl_En_Jsjutan/sShadowMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sShadowMaterialDL[] = dsShadowMaterialDL;
#else
static const char sShadowMaterialDL[] __attribute__((aligned (2))) = dsShadowMaterialDL;
#endif
            
#define dsModelDL "__OTR__overlays/ovl_En_Jsjutan/sModelDL"
#ifdef _WIN32
static const __declspec(align(2)) char sModelDL[] = dsModelDL;
#else
static const char sModelDL[] __attribute__((aligned (2))) = dsModelDL;
#endif
            
#define dsCarpetEvenVtx "__OTR__overlays/ovl_En_Jsjutan/sCarpetEvenVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sCarpetEvenVtx[] = dsCarpetEvenVtx;
#else
static const char sCarpetEvenVtx[] __attribute__((aligned (2))) = dsCarpetEvenVtx;
#endif
            
#define dsCol "__OTR__overlays/ovl_En_Jsjutan/sCol"
#ifdef _WIN32
static const __declspec(align(2)) char sCol[] = dsCol;
#else
static const char sCol[] __attribute__((aligned (2))) = dsCol;
#endif
            
extern u8 sShadowTex[2048];

