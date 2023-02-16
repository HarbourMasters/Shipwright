#pragma once
#define dsTex "__OTR__overlays/ovl_Oceff_Wipe/sTex"
#ifdef _WIN32
static const __declspec(align(2)) char sTex[] = dsTex;
#else
static const char sTex[] __attribute__((aligned (2))) = dsTex;
#endif
            
#define dsFrustumVtx "__OTR__overlays/ovl_Oceff_Wipe/sFrustumVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sFrustumVtx[] = dsFrustumVtx;
#else
static const char sFrustumVtx[] __attribute__((aligned (2))) = dsFrustumVtx;
#endif
            
#define dsMaterialDL "__OTR__overlays/ovl_Oceff_Wipe/sMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sMaterialDL[] = dsMaterialDL;
#else
static const char sMaterialDL[] __attribute__((aligned (2))) = dsMaterialDL;
#endif
            
#define dsFrustumDL "__OTR__overlays/ovl_Oceff_Wipe/sFrustumDL"
#ifdef _WIN32
static const __declspec(align(2)) char sFrustumDL[] = dsFrustumDL;
#else
static const char sFrustumDL[] __attribute__((aligned (2))) = dsFrustumDL;
#endif
            

