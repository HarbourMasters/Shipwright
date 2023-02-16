#pragma once
#define dsTex "__OTR__overlays/ovl_Oceff_Wipe4/sTex"
#ifdef _WIN32
static const __declspec(align(2)) char sTex[] = dsTex;
#else
static const char sTex[] __attribute__((aligned (2))) = dsTex;
#endif
            
#define dsFrustumVtx "__OTR__overlays/ovl_Oceff_Wipe4/sFrustumVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sFrustumVtx[] = dsFrustumVtx;
#else
static const char sFrustumVtx[] __attribute__((aligned (2))) = dsFrustumVtx;
#endif
            
#define dsMaterialDL "__OTR__overlays/ovl_Oceff_Wipe4/sMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sMaterialDL[] = dsMaterialDL;
#else
static const char sMaterialDL[] __attribute__((aligned (2))) = dsMaterialDL;
#endif
            
#define dsUnusedMaterialDL "__OTR__overlays/ovl_Oceff_Wipe4/sUnusedMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sUnusedMaterialDL[] = dsUnusedMaterialDL;
#else
static const char sUnusedMaterialDL[] __attribute__((aligned (2))) = dsUnusedMaterialDL;
#endif
            
#define dsMaterial2DL "__OTR__overlays/ovl_Oceff_Wipe4/sMaterial2DL"
#ifdef _WIN32
static const __declspec(align(2)) char sMaterial2DL[] = dsMaterial2DL;
#else
static const char sMaterial2DL[] __attribute__((aligned (2))) = dsMaterial2DL;
#endif
            

