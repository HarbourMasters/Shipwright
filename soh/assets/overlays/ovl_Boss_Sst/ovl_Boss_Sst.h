#pragma once
#define dsBodyStaticDList "__OTR__overlays/ovl_Boss_Sst/sBodyStaticDList"
#ifdef _WIN32
static const __declspec(align(2)) char sBodyStaticDList[] = dsBodyStaticDList;
#else
static const char sBodyStaticDList[] __attribute__((aligned (2))) = dsBodyStaticDList;
#endif
            
#define dsHandTrailDList "__OTR__overlays/ovl_Boss_Sst/sHandTrailDList"
#ifdef _WIN32
static const __declspec(align(2)) char sHandTrailDList[] = dsHandTrailDList;
#else
static const char sHandTrailDList[] __attribute__((aligned (2))) = dsHandTrailDList;
#endif
            
#define dsIntroVanishDList "__OTR__overlays/ovl_Boss_Sst/sIntroVanishDList"
#ifdef _WIN32
static const __declspec(align(2)) char sIntroVanishDList[] = dsIntroVanishDList;
#else
static const char sIntroVanishDList[] __attribute__((aligned (2))) = dsIntroVanishDList;
#endif
            
#define dsShadowDList "__OTR__overlays/ovl_Boss_Sst/sShadowDList"
#ifdef _WIN32
static const __declspec(align(2)) char sShadowDList[] = dsShadowDList;
#else
static const char sShadowDList[] __attribute__((aligned (2))) = dsShadowDList;
#endif
            
#define dovl_Boss_SstTex_00A438 "__OTR__overlays/ovl_Boss_Sst/ovl_Boss_SstTex_00A438"
#ifdef _WIN32
static const __declspec(align(2)) char ovl_Boss_SstTex_00A438[] = dovl_Boss_SstTex_00A438;
#else
static const char ovl_Boss_SstTex_00A438[] __attribute__((aligned (2))) = dovl_Boss_SstTex_00A438;
#endif
            
#define dovl_Boss_SstTex_00A8F0 "__OTR__overlays/ovl_Boss_Sst/ovl_Boss_SstTex_00A8F0"
#ifdef _WIN32
static const __declspec(align(2)) char ovl_Boss_SstTex_00A8F0[] = dovl_Boss_SstTex_00A8F0;
#else
static const char ovl_Boss_SstTex_00A8F0[] __attribute__((aligned (2))) = dovl_Boss_SstTex_00A8F0;
#endif
            

