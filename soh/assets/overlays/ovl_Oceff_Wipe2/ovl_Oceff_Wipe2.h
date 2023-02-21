#pragma once
#define ds1Tex "__OTR__overlays/ovl_Oceff_Wipe2/s1Tex"
#ifdef _WIN32
static const __declspec(align(2)) char s1Tex[] = ds1Tex;
#else
static const char s1Tex[] __attribute__((aligned (2))) = ds1Tex;
#endif
            
#define ds2Tex "__OTR__overlays/ovl_Oceff_Wipe2/s2Tex"
#ifdef _WIN32
static const __declspec(align(2)) char s2Tex[] = ds2Tex;
#else
static const char s2Tex[] __attribute__((aligned (2))) = ds2Tex;
#endif
            
#define dsFrustumVtx "__OTR__overlays/ovl_Oceff_Wipe2/sFrustumVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sFrustumVtx[] = dsFrustumVtx;
#else
static const char sFrustumVtx[] __attribute__((aligned (2))) = dsFrustumVtx;
#endif
            
#define dsMaterialDL "__OTR__overlays/ovl_Oceff_Wipe2/sMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sMaterialDL[] = dsMaterialDL;
#else
static const char sMaterialDL[] __attribute__((aligned (2))) = dsMaterialDL;
#endif
            
#define dsFrustumDL "__OTR__overlays/ovl_Oceff_Wipe2/sFrustumDL"
#ifdef _WIN32
static const __declspec(align(2)) char sFrustumDL[] = dsFrustumDL;
#else
static const char sFrustumDL[] __attribute__((aligned (2))) = dsFrustumDL;
#endif
            

